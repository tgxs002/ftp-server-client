#include "common.h"
int readFromSocket(int connfd, char* buffer, int length)
{
	int p = 0;
	while (1) 
	{
		int n = read(connfd, buffer + p, length - p);
		if (n < 0) 
		{
			printf("Error read(): %s(%d)\n", strerror(errno), errno);
			close(connfd);
			return 0;
		}
		else if (n == 0) 
		{
			break;
		} 
		else 
		{
			p += n;
			if (buffer[p - 1] == '\n') 
			{
				break;
			}
		}

	}

	buffer[p - 1] = '\0';
	length = p - 1;
    printf("read: %s\n", buffer);
	return length;
}

void writeToSocket(int connfd, char* buffer, int length)
{
	int nop = write(connfd, buffer, strlen(buffer));
	connfd = nop;
	printf("write: %s\n", buffer);
	return;
}

/* translate the revieved information into instruction keys */
VERB keyFromString(char* commandBuffer)
{
	char* p = commandBuffer;
	while (*p <= 'Z' && *p >= 'A') p++;
	*p = 0;
	if (strcmp(commandBuffer, "USER") == 0)
	{
		return USER;
	}
	else if (strcmp(commandBuffer, "PASS") == 0)
	{
		return PASS;
	}
	else if (strcmp(commandBuffer, "ACCT") == 0)
	{
		return ACCT;
	}
	else if (strcmp(commandBuffer, "TYPE") == 0)
	{
		return TYPE;
	}
	else if (strcmp(commandBuffer, "STRU") == 0)
	{
		return STRU;
	}
	else if (strcmp(commandBuffer, "MODE") == 0)
	{
		return MODE;
	}
	else if (strcmp(commandBuffer, "CWD") == 0)
	{
		return CWD;
	}
	else if (strcmp(commandBuffer, "PWD") == 0)
	{
		return PWD;
	}
	else if (strcmp(commandBuffer, "CDUP") == 0)
	{
		return CDUP;
	}
	else if (strcmp(commandBuffer, "PASV") == 0)
	{
		return PASV;
	}
	else if (strcmp(commandBuffer, "RETR") == 0)
	{
		return RETR;
	}
	else if (strcmp(commandBuffer, "REST") == 0)
	{
		return REST;
	}
	else if (strcmp(commandBuffer, "PORT") == 0)
	{
		return PORT;
	}
	else if (strcmp(commandBuffer, "LIST") == 0)
	{
		return LIST;
	}
	else if (strcmp(commandBuffer, "NLST") == 0)
	{
		return NLST;
	}
	else if (strcmp(commandBuffer, "QUIT") == 0)
	{
		return QUIT;
	}
	else if (strcmp(commandBuffer, "SYST") == 0)
	{
		return SYST;
	}
	else if (strcmp(commandBuffer, "STAT") == 0)
	{
		return STAT;
	}
	else if (strcmp(commandBuffer, "HELP") == 0)
	{
		return HELP;
	}    
	else if (strcmp(commandBuffer, "NOOP") == 0)
	{
		return NOOP;
	}
	else if (strcmp(commandBuffer, "STOR") == 0)
	{
		return STOR;
	}
	else if (strcmp(commandBuffer, "APPE") == 0)
	{
		return APPE;
	}
	else if (strcmp(commandBuffer, "STOU") == 0)
	{
		return STOU;
	}
	else if (strcmp(commandBuffer, "ALLO") == 0)
	{
		return ALLO;
	}
	else if (strcmp(commandBuffer, "MKD") == 0)
	{
		return MKD;
	}
	else if (strcmp(commandBuffer, "RMD") == 0)
	{
		return RMD;
	}
	else if (strcmp(commandBuffer, "DELE") == 0)
	{
		return DELE;
	}
	else if (strcmp(commandBuffer, "RNFR") == 0)
	{
		return RNFR;
	}
	else if (strcmp(commandBuffer, "RNTO") == 0)
	{
		return RNTO;
	}
	else if (strcmp(commandBuffer, "ABOR") == 0)
	{
	    return ABOR;
	}
	else
	{
		return UNKOWN;
	}
}

void getIpAddress(char* addr)
{
	struct ifaddrs * ifAddrStruct=NULL;
	void * tmpAddrPtr=NULL;

	getifaddrs(&ifAddrStruct);

	while (ifAddrStruct!=NULL) 
	{
		if (ifAddrStruct->ifa_addr->sa_family==AF_INET)
		{
			tmpAddrPtr = &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
			inet_ntop(AF_INET, tmpAddrPtr, addr, INET_ADDRSTRLEN);
			return;
		}
		else
		{
			ifAddrStruct = ifAddrStruct->ifa_next;
		}
	}
}

int ListenAndReply(struct info* _info)
{
	int connfd = _info->fd;
	char sentence[200];
	char parameter[200];
	char currentDir[200] = {'\0'};
	int currentPos = 0;

	if (_info->path == NULL)
	{
		strcpy(currentDir, "/tmp");
	}
	else
	{
		if (_info->path[0] != '/')
		{
			strcpy(currentDir, "./");
		}
		strcat(currentDir, _info->path);
	}

	VERB verb;
	int  errono = 0;
	STATUS state = NOT_LOGIN;
	int dataconnfd = -1;
    /* to tell the client the connection is successfully made */
	int nop = write(connfd, "220 Service ready for new user.\n", 32);
	sentence[199] = nop;
	while (readFromSocket(connfd, sentence, 200))
	{
		errono = commandParser(sentence, &verb, parameter);

		if (errono != 0)
		{
			writeToSocket(connfd, parameter, strlen(parameter) + 1);
		}
		else
		{
            /* execute */
			errono = Execute(&state, connfd, parameter, verb, &dataconnfd, currentDir, &currentPos);
		}
		if (state == QUT)
		{
            /* quit when recieve QUIT */
			break;
		}
	}

	close(connfd);
	return 1;
}

int CreateAndListen(int* port)
{
	struct sockaddr_in addr;
	int errno;
	int listenfd;

    //create a socket
	if ((listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		printf("Error socket(): %s(%d)\n", strerror(errno), errno);
		return -1;
	}

	//the socket will listen to port 21(by default) for connect request from any address.
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(*port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);	//"0.0.0.0"

	//bind the socket to addr
	if (*port != -1)
	{
		if (bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
			printf("Error bind(): %s(%d)\n", strerror(errno), errno);
			return -1;
		}
	}
	else
	{
		int port_temp = 1025;
		addr.sin_port = htons(port_temp);
		while (bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) addr.sin_port = htons(++port_temp);
		*port = port_temp;
	}

	//socket listen to port 21 for connect request
	if (listen(listenfd, 10) == -1) {
		printf("Error listen(): %s(%d)\n", strerror(errno), errno);
		return -1;
	}
	return listenfd;
}

