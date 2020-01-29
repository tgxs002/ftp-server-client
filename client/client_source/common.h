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

#ifndef COMMON
#define COMMON

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
    RNFR, RNTO, UNKOWN          //27
} VERB;


int readFromSocket(int connfd, char* buffer, int length);
void writeToSocket(int connfd, char* buffer, int length);
VERB keyFromString(char* commandBuffer);
int startWith(char* source, char* pattern);

#endif
