#ifndef COMMON
#define COMMON

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

typedef enum VERB 
{
    USER, PASS, ACCT,           //0
    TYPE, STRU, MODE,           //3
    CWD,  PWD,  CDUP,           //6
    PASV, RETR, REST, PORT,     //9
    LIST, NLST, QUIT,           //13
    SYST, STAT, HELP, NOOP,     //16
    STOR, APPE, STOU, ALLO,     //20
    MKD,  RMD,  DELE,           //24
    RNFR, RNTO, UNKOWN,         //27
    ABOR
} VERB;

typedef enum STATUS
{
    NOT_LOGIN,
    POSITIVE,
    PASSIVE,
    NOT_DATA_CONNECT,
    DATA_CONNECTED,
    WAIT_PASSWD,
    QUT,
    MOVE
} STATUS;

struct info
{
    int fd;
    char* path;
};

/* create a socket and listen to a specific port, return listenfd */
int CreateAndListen(int* port);

/* following functions deal with different commands, and give adequate reply, return true when quit successfully, false otherwise */
int ListenAndReply(struct info* _info);

int readFromSocket(int connfd, char* buffer, int length);
void writeToSocket(int connfd, char* buffer, int length);

/* extract key info from string */
VERB keyFromString(char* commandBuffer);

void getIpAddress(char* addr);

/* parse the command verb and parameter, return 0 if successful, else, return error No. and put reply into parameterBuffer */
int commandParser(char* commandBuffer, VERB* verb, char* parameterBuffer);

/* Execute the command, return error No., 0 if no error */
int Execute(STATUS* state, int connfd, char* parameter, VERB verb, int* dataconnfd, char* currentDir, int* currentPos);
#endif
