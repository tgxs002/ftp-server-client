#include "functions.h"

/* login process */
int login(int connfd, char* user, char* passwd)
{
	char command[100] = {0};
	strcpy(command, "USER ");
	strcat(command, user);
	if (user[strlen(user) - 1] != '\n')
	{
		strcat(command, "\n");
	}
	writeToSocket(connfd, command, 0);
	char resp[100];
	readFromSocket(connfd, resp, 100);
	if (resp[0] == '2')
		readFromSocket(connfd, resp, 100);
	if (resp[0] == '3')
	{
		strcpy(command, "PASS ");
		strcat(command, passwd);
		if (user[strlen(passwd) - 1] != '\n')
		{
			strcat(command, "\n");
		}
		writeToSocket(connfd, command, 0);
	}
	readFromSocket(connfd, resp, 100);
	if (resp[0] != '2')
	{
        return 0;
	}
    return 1;
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

struct data
{
    FILE* file;
    int   dataconnfd;
    bool *flag;
};

void getinfo(struct data *data)
{
    FILE* file = data->file;
    int dataconnfd = data->dataconnfd;
    printf("dataconnfd: %d\n", dataconnfd);
    char buffer[8192];
    int size = 0;
    *(data->flag) = true;
    while (1)
    {
        size = read(dataconnfd, buffer, 8192);
        if (!size) break;
        if (fwrite(buffer, 1, size, file) != size)
        {
            printf("error writing...\n");
            fclose(file);
            close(dataconnfd);
            *(data->flag) = false;
        }
    }
    fclose(file);
    close(dataconnfd);
    *(data->flag) = false;
}

int download(char* path, int connfd, int positiv, char* savepath, bool& downloading, char* way)
{
    int dataconnfd;
    char cmd[200];
    strcpy(cmd, "RETR ");
    strcat(cmd, path);
    if (path[strlen(path) - 1] != '\n')
    {
        strcat(cmd, "\r\n");
    }
    if (positiv)
    {
        int listenfd = positive(connfd);
        writeToSocket(connfd, cmd, 0);
        if ((dataconnfd = accept(listenfd, NULL, NULL)) == -1) {
            printf("Error accept(): %s(%d)\n", strerror(errno), errno);
            return -1;
        }
    }
    else
    {
        dataconnfd = passive(connfd);
        if (dataconnfd == -1)
            return 0;
        writeToSocket(connfd, cmd, 0);
    }

    strcat(savepath, "/");
    strcat(savepath, path);

    FILE* file = fopen(savepath, way);
    if (!file)
    {
        printf("error open/creating file.\n");
        close(dataconnfd);
        return 0;
    }
    pthread_t th;
    struct data* _info = (struct data*)malloc(sizeof(struct data));

    _info->file = file;
    _info->dataconnfd = dataconnfd;
    _info->flag = &downloading;
    int ret = pthread_create(&th, NULL, (void*(*)(void*))&getinfo, _info);
    if (ret != 0)
    {
        printf("thread create fail.\n");
    }
    return 1;
}

int positive(int connfd)
{
	int listenfd;
	if ((listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		printf("Error socket(): %s(%d)\n", strerror(errno), errno);
		return -1;
	}
	
    /* listen */    
	int port = 10253;
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	while (bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) addr.sin_port = htons(++port);

    /* send instruction */    
	char ip[50];
	getIpAddress(ip);
	int t = inet_addr(ip);
	char h1 = (t >> 24) & 0x000000ff;
	char h2 = (t >> 16) & 0x000000ff;
	char h3 = (t >> 8) & 0x000000ff;
	char h4 = t & 0x000000ff;
	char p1 = (port >> 8) & 0xff;
	char p2 = port & 0xff;

	char command[100];
    sprintf(command, "PORT %d,%d,%d,%d,%d,%d \r\n", h4, h3, h2, h1, p1, p2);
	if (listen(listenfd, 10) == -1) {
		printf("Error listen(): %s(%d)\n", strerror(errno), errno);
		return -1;
	}
    writeToSocket(connfd, command, 0);
	return listenfd;
}

int passive(int connfd)
{
	char buffer[100];
	int dataconnfd;
	writeToSocket(connfd, "PASV \n", 0);
	while (!startWith(buffer, "227"))
	{
		readFromSocket(connfd, buffer, 100);
	}
	int nop;
	int a0, a1, a2, a3, p1, p2;
	char* p = buffer;
	p += 3;
	while (*p >= '9' || *p <= '0') p++;
	if (sscanf(p, "%d,%d,%d,%d,%d,%d", &a0, &a1, &a2, &a3, &p1, &p2) != 6)
	{
		printf("Error: parse ip fail.\n");

		printf("%s\n", p);
		printf("%s\n", buffer);
		printf("%d,%d,%d,%d,%d,%d", a0, a1, a2, a3, p1,p2);
		return -1;
	}
	int a = 0;
	a = (a3 << 24) | (a2 << 16) | (a1 << 8) | a0;
	unsigned short _port = (p2 << 8) | p1;
	struct sockaddr_in addr;
                //create a socket
	if ((dataconnfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		printf("Error socket(): %s(%d)\n", strerror(errno), errno);
		return -1;
	}
	            //the socket will listen to port 21(by default) for connect request from any address.
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = _port;
			/* "0.0.0.0" */
	addr.sin_addr.s_addr = a;
	if (connect(dataconnfd, (struct sockaddr*)&addr, sizeof(addr)))
	{
		printf("Error connect(): %s(%d)\n", strerror(errno), errno);
		return -1;
	}
	printf("data connected\n");
	return dataconnfd;
}

int list(int connfd, int positiv)
{
	int dataconnfd;
	if (positiv)
	{
		int listenfd = positive(connfd);
		if ((dataconnfd = accept(listenfd, NULL, NULL)) == -1) {
			return -1;
        }
        sleep(200);
        writeToSocket(connfd, "LIST \n", 0);
	}
	else
	{
		dataconnfd = passive(connfd);
		if (dataconnfd == -1)
			return -1;
		writeToSocket(connfd, "LIST \n", 0);
	}
	char resp[8192];
	readFromSocket(dataconnfd, resp, 8192);
	printf("%s\n", resp);
	close(dataconnfd);
	return 0;
}

int _send(int connfd, char* local, char* remote, int positiv)
{
	int dataconnfd;
	char cmd[200];
    struct stat s_buf;
    stat(local, &s_buf);
    if (S_ISDIR(s_buf.st_mode))
    {
        return 4;
    }
    FILE* file = fopen(local, "rb");
    if (!file)
    {
        printf("Error reading file.\n");
        printf("%s\n", local);
        return 1;
    }
	strcpy(cmd, "STOR ");
	strcat(cmd, remote);
	if (remote[strlen(remote) - 1] != '\n')
	{
		strcat(cmd, "\n");
	}
	if (positiv)
	{
		int listenfd = positive(connfd);
		if ((dataconnfd = accept(listenfd, NULL, NULL)) == -1) {
			printf("Error accept(): %s(%d)\n", strerror(errno), errno);
            return 2;
		}
	}
	else
	{
		dataconnfd = passive(connfd);
		if (dataconnfd == -1)
		{
            return 2;
		}
	}
    writeToSocket(connfd, cmd, 0);
    readFromSocket(connfd, cmd, 200);
    if (startWith(cmd, "451"))
    {
        fclose(file);
        close(dataconnfd);
        return 3;
    }
	char buff[8192];
	int t = 0;
	while (1)
	{
		t = fread(buff, 1, 8192, file);
		if (t <= 0) break;
		writeToSocket(dataconnfd, buff, t);
	}
    readFromSocket(connfd, cmd, 200);
	fclose(file);
	close(dataconnfd);
	return 0;
}
