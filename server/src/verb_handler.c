#include "verb_handler.h"
#include "common.h"
#include <string.h>

int commandParser(char* commandBuffer, VERB* verb, char* parameterBuffer)
{
    /* store the command */
	*verb = keyFromString(commandBuffer);
    /* find the parameter */
	commandBuffer += 4;
	if (*commandBuffer == 0) *commandBuffer = ' ';
	int errono = 0;

	if (*verb == USER || *verb == CWD || *verb == RMD || *verb == MKD || *verb == RETR || *verb == STOR || *verb == RNFR || *verb == RNTO || *verb == REST || *verb == ABOR)
	{
        /* copy start from the non-space char */
		while (*commandBuffer == ' ') commandBuffer++;
		strcpy(parameterBuffer, commandBuffer);
        //while (*commandBuffer != 0) *(parameterBuffer++) = *(commandBuffer++);
	}
	else if (*verb == PASS)
	{
        /* space can be a part of password */
		strcpy(parameterBuffer, commandBuffer);
        //while (*commandBuffer != 0) *(parameterBuffer++) = *(commandBuffer++);
	}
	else if (*verb == TYPE)
	{
        /* only TYPE I */
	}
	else if (*verb == PASV || *verb == QUIT || *verb == PWD || *verb == LIST || *verb == SYST)
	{
        /* take no parameter */
		*parameterBuffer = 0;
	}
	else if ( *verb == PORT)
	{
        /* find the ip address */
		char* pre = parameterBuffer;
		while (*commandBuffer > '9' || *commandBuffer < '0') commandBuffer++;
		int comma_counter = 0;
		int number = 0;
		while (comma_counter < 5 && number < 18)
		{
			if (*commandBuffer == ',')
				comma_counter++;
			*(parameterBuffer++) = *(commandBuffer++);
			number++;
		}
		if (comma_counter != 5)
		{
			strcpy(pre, "501 Parameter analyse failure.\n");
			errono = 501;
		}
		else
		{
			while (*commandBuffer <= '9' && *commandBuffer >= '0') *(parameterBuffer++) = *(commandBuffer++);
			*parameterBuffer = 0;
		}
	}
	else if ( *verb == UNKOWN)
	{
        /* undefined verb */
		errono = 500;
		strcpy(parameterBuffer, "500 Undefined verb.\n");
	}
	else
	{
        /* command verb is not implemented(required by the homework) */
		errono = 202;
		strcpy(parameterBuffer, "202 Verb not implemented.\n");
	}
	return errono;
}

struct sendinfo
{
	int connfd;
	int dataconnfd;
	FILE* file;
	STATUS* state;
};

void sendinfofunction(struct sendinfo* info)
{
	char buff[8192];
	int success = 1;
	int t = 0;
	while (1)
	{
		t = fread(buff, 1, 8192, info->file);
		if (t <= 0) break;
		if (send(info->dataconnfd, buff, t, 0) == -1)
		{
			success = 0;
			break;
		}
	}
	fclose(info->file);
	close(info->dataconnfd);
	if (success)
	{
		writeToSocket(info->connfd, "226 Success.\n", 14);
	}
	else
	{
		writeToSocket(info->connfd, "426 Connection failure.\n", 0);
	}
	*(info->state) = NOT_DATA_CONNECT;
}


char move_temp[150];
char user[100];

int Execute(STATUS* state, int connfd, char* parameter, VERB verb, int* dataconnfd, char* currentDir, int* currentPos)
{
	int errono = 0;

	if ((*state == NOT_LOGIN || *state == WAIT_PASSWD) && (verb != USER && verb != PASS))
	{
		writeToSocket(connfd, "220 Please login first.\r\n", 24);
		return 0;
	}

	switch (verb)
	{
		case USER:
		{
			if (*state == NOT_LOGIN || *state == WAIT_PASSWD)
			{
                strcpy(user, parameter);
				writeToSocket(connfd, "331 Please specify the password.\n", 33);
				*state = WAIT_PASSWD;
			}
			else
			{
				writeToSocket(connfd, "230 Already logged in.\n", 24);
			}
			break;
		}
		case PASS:
		{
			if (*state == WAIT_PASSWD)
			{
                int found = 0;
                while (user[strlen(user) - 1] == '\n' || user[strlen(user) - 1] == '\r' || user[strlen(user) - 1] == ' ')
                {
                    user[strlen(user) - 1] = '\0';
                }
                if (strstr(user, "anonymous")) found = 1;
                strcat(user, " ");
                while (*parameter == ' ') parameter++;
                strcat(user, parameter);
                char file[100];
                while (user[strlen(user) - 1] == '\n' || user[strlen(user) - 1] == '\r')
                {
                    user[strlen(user) - 1] = '\0';
                }
                FILE* fp;
                fp = fopen("userdata", "rt+");
                
                if (fp != NULL && !found)
                {
                    while (fgets(file, sizeof(file), fp))
                    {
                        if (strstr(file, user))
                        {
                            found = 1;
                            break;
                        }
                    }
                }
                else if (fp == NULL)
                    found = 1;
                if (!found)
                {
                    sleep(5);
                    writeToSocket(connfd, "530 Login incorrect.\r\n", 22);
                    *state = NOT_LOGIN;
                }
				else
                {
                    writeToSocket(connfd, "230 Login successful.\n", 22);
				    *state = POSITIVE;
                }
			}
			else if (*state == NOT_LOGIN)
			{
				writeToSocket(connfd, "220 Please input user name first.\n", 35);
			}
			else
			{
				writeToSocket(connfd, "230 Already logged in.\n", 24);
			}
			break;
		}
		case SYST:
		{
			writeToSocket(connfd, "215 UNIX Type: L8\n", 19);
			break;
		}
		case TYPE:
		{
			writeToSocket(connfd, "200 Type set to I.\n", 19);
			break;
		}
		case QUIT:
		{
			writeToSocket(connfd, "221 Bye.\n", 10);
			*state = QUT;
			break;
		}
		case PASV:
		{
            /* if there is already a data connection, disconnect it */
			if (*state == DATA_CONNECTED)
			{
				close(*dataconnfd);
			}
			int port = -1;
			int listen_temp;
            /* create a listening socket */
			listen_temp = CreateAndListen(&port);
			char ip[100];
			getIpAddress(ip);
			int t = inet_addr(ip);
			char h1 = (t >> 24) & 0x000000ff;
			char h2 = (t >> 16) & 0x000000ff;
			char h3 = (t >> 8) & 0x000000ff;
			char h4 = t & 0x000000ff;
			char p1 = (port >> 8) & 0x000000ff;
			char p2 = port & 0x000000ff;
			int l = sprintf(ip, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)\r\n", h4, h3, h2, h1, p1, p2);
			writeToSocket(connfd, ip, l);
			if ((*dataconnfd = accept(listen_temp, NULL, NULL)) == -1) 
			{
				printf("Error accept(): %s(%d)\n", strerror(errno), errno);
				return 0;
			}
			close(listen_temp);
			*state = PASSIVE;
			break;
		}
		case PORT:
		{
			if (*state == DATA_CONNECTED)
			{
				close(*dataconnfd);
			}
			int a0, a1, a2, a3, p1, p2;
			if (sscanf(parameter, "%d,%d,%d,%d,%d,%d", &a0, &a1, &a2, &a3, &p1, &p2) != 6)
			{
				printf("Error: parse ip fail.\n");
				return 1;
			}
			int a = 0;
			a = (a3 << 24) | (a2 << 16) | (a1 << 8) | a0;
			unsigned short _port = (p2 << 8) | p1;
			struct sockaddr_in addr;

                //create a socket
			if ((*dataconnfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
				printf("Error socket(): %s(%d)\n", strerror(errno), errno);
				return -1;
			}
	            //the socket will listen to port 21(by default) for connect request from any address.
			memset(&addr, 0, sizeof(addr));
			addr.sin_family = AF_INET;
			addr.sin_port = _port;
			/* "0.0.0.0" */
			addr.sin_addr.s_addr = a;
			if (connect(*dataconnfd, (struct sockaddr*)&addr, sizeof(addr)))
			{
				printf("Error connect(): %s(%d)\n", strerror(errno), errno);
				return 0;
			}
			writeToSocket(connfd, "200 Connected.\n", 15);
			*state = POSITIVE;
			break;
		}
		case MKD:
		{
			char temp[150];
			strcpy(temp, "");
			strcat(temp, currentDir);
            if (parameter[0] != '/')
            {
                strcat(temp, "/");
            }
			strcat(temp, parameter);
			int t = strlen(temp);
            temp[t - 1] = '/';
			if (!mkdir(temp, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
			{
				writeToSocket(connfd, "257 Directory successfully made.\n", 34);
			}
			else
			{
				writeToSocket(connfd, "550 Creation Fail.\n", 31);
			}
			break;
		}
		case CWD:
		{
            if (parameter[0] == '.' && parameter[1] == '.')
            {
                int count = 0;
                char* start = currentDir;
                while (*start != '\0')
                {
                    if (*start == '/') count++;
                    start++;
                }
                if (count == level)
                {
                    writeToSocket(connfd, "550 Root directory, permission denied.\n", 31);
                }
                else
                {
                    while (*start != '/') start--;
                    *start = '\0';
                    writeToSocket(connfd, "250 OK.\n", 8);
                }
                printf("%s\n", currentDir);
                break;
            }
            char temp[200];
            strcpy(temp, currentDir);
            if (parameter[0] != '/')
            {
                strcat(temp, "/");
            }
            strcat(temp, parameter);
			int l = strlen(temp);
			temp[l - 1] = '\0';
            if (temp[l - 2] == '/')
            {
                temp[l - 2] = '\0';
            }
			if (!access(temp, F_OK))
			{
				strcpy(currentDir, temp);
				writeToSocket(connfd, "250 OK.\n", 8);
			}
			else
			{
				writeToSocket(connfd, "550 No such file or directory.\n", 31);
			}
			printf("%s\n", currentDir);
			break;
		}
		case PWD:
		{
			sprintf(parameter, "257 %s\n", currentDir);
			writeToSocket(connfd, parameter, strlen(parameter));
			break;
		}
		case RMD:
		{
			char temp[150];
			strcpy(temp, currentDir);
            if (parameter[0] != '/')
            {
                strcat(temp, "/");
            }
			strcat(temp, parameter);
			temp[strlen(temp) - 1] = '/';
			if (rmdir(temp) == 0)
			{
				writeToSocket(connfd, "250 Folder deleted.\n", 18);
			}
			else
			{
				writeToSocket(connfd, "550 No such file or directory.\n", 31);
			}
			break;
		}
		case RNFR:
		{
            strcpy(move_temp, currentDir);
            if (parameter[0] != '/')
            {
                strcat(move_temp, "/");
            }
			strcat(move_temp, parameter);
            move_temp[strlen(move_temp) - 1] = '\0';

			if (access(move_temp, R_OK) == 0)
			{
				writeToSocket(connfd, "350 File exist.\n", 16);
				*state = MOVE;
			}
			else
			{
				writeToSocket(connfd, "550 No such file or directory.\n", 31);
			}
			break;
		}
		case RNTO:
		{
			char temp[150];
			if (*state == MOVE)
			{
				strcpy(temp, currentDir);
                if (parameter[0] != '/')
                {
                    strcat(temp, "/");
                }
			    strcat(temp, parameter);
                temp[strlen(temp) - 1] = '\0';
				if (rename(move_temp, temp) == 0)
				{
					writeToSocket(connfd, "250 Move success.\n", 18);
				}
				else
				{
					writeToSocket(connfd, "550 Move fail.\n", 31);
				}
			}
			else
			{
				writeToSocket(connfd, "503 Wrong sequence.\n", 20);
			}
			break;
		}
		case LIST:
		{
			if (*state != PASSIVE && *state != POSITIVE)
			{
                    /* rejects the LIST or NLST request with code 425 if no TCP connection was established; */
				writeToSocket(connfd, "425 Not connected.\n", 19);
				break;
			}
			char path[100];
			strcpy(path, currentDir);
			strcat(path, parameter);
			DIR* dir = opendir(path);
			struct dirent *ptr;
			char dirInfo[500];
			char result[8000] = {0};

			if (!dir)
			{
                    /* reject the LIST or NLST request 550 without first responding with a mark. */
				writeToSocket(connfd, "550 No such file or directory.\n", 31);
				close(*dataconnfd);
				*state = NOT_DATA_CONNECT;
				break;
			}

			writeToSocket(connfd, "150 Start transfer...\r\n", 22);
			strcpy(result, path);
			strcat(result, "\r\n");

			while ((ptr = readdir(dir)) != NULL)
			{
				sprintf(dirInfo, "name: %s , type: %s\r\n", ptr->d_name, (ptr->d_type == DT_DIR ? "directory" : "file"));
				strcat(result, dirInfo);
			}
			strcat(result, "this is the end of line.\r\n");
			closedir(dir);

			writeToSocket(*dataconnfd, result, strlen(result) - 1);

			writeToSocket(connfd, "226 Success.\r\n", 14);
			close(*dataconnfd);
			*state = NOT_DATA_CONNECT;
			break;
		}
		case STOR:
		{
			if (*state != PASSIVE && *state != POSITIVE)
			{
				writeToSocket(connfd, "425 Not connected.\n", 19);
				break;
			}
			char path[200];
			strcpy(path, currentDir);
			if (parameter[0] != '/')
			{
				strcat(path, "/");
			}
			strcat(path, parameter);
			if (path[strlen(path) - 1] == '\r')
    			path[strlen(path) - 1] = 0;

			FILE* file = fopen(path, "wb");
            if (!file)
            {
                writeToSocket(connfd, "451 Error writing...\r\n", 26);
                break;
            }
			char buffer[8192];
			int size = 0;
			writeToSocket(connfd, "150 Ready for recieve...\r\n", 26);
			while (1)
			{
				size = read(*dataconnfd, buffer, 8192);
				if (!size) break;
				if (fwrite(buffer, 1, size, file) != size)
				{
					writeToSocket(connfd, "451 Error writing...\r\n", 26);
					break;
				}
			}
			fclose(file);
			writeToSocket(connfd, "226 Success.\r\n", 14);
			close(*dataconnfd);
			*state = NOT_DATA_CONNECT;
			break;
		}
		case RETR:
		{
			if (*state != PASSIVE && *state != POSITIVE)
			{
				writeToSocket(connfd, "425 Not connected.\n", 19);
				break;
			}

                /* open the file */
			char path[200];
			strcpy(path, currentDir);
			if (parameter[0] != '/')
				strcat(path, "/");
			strcat(path, parameter);
			path[strlen(path) - 1] = 0;
			FILE* file = fopen(path, "rb");

			if (!file)
			{
				writeToSocket(connfd, "550 File not found.\n", 20);
				break;
			}
			fseek(file, *currentPos, SEEK_SET);
			printf("%d\n", *currentPos);
			
			char sen_[200];
			sprintf(sen_, "150 %s\n", path);
			writeToSocket(connfd, sen_, 23);
			
			struct sendinfo* info = (struct sendinfo*)malloc(sizeof(struct sendinfo));
			info->connfd = connfd;
			info->dataconnfd = *dataconnfd;
			info->state = state;
			info->file = file;

			pthread_t th;
			int ret = pthread_create(&th, NULL, (void*)&sendinfofunction, info);
			if (ret != 0)
			{
			    printf("thread create fail.\n");
			}
			break;
		}
		case REST:
		{
			int temp;
			if (sscanf(parameter, "%d", &temp) == 1)
			{
				*currentPos = temp;
				writeToSocket(connfd, "350 Pos set OK.\n", 14);
			}
			else
			{
				writeToSocket(connfd, "501 Syntax error in parameters or arguments.", 14);
			}
		}
		case ABOR:
		{
            close(*dataconnfd);
            writeToSocket(connfd, "226 Abort transforming.\r\n", 0);
            break;
        }
		default:
		break;
	}
	if (verb != REST && verb != PASV && verb != PORT && verb != SYST)
	{
		*currentPos = 0;
	}
	return errono;
}


