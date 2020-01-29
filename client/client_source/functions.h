#ifndef FUNCTIONS
#define FUNCTIONS

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
#include <pthread.h>
#include "common.h"

extern int skip;

int login(int connfd, char* user, char* passwd);

int download(char* path, int connfd, int positive, char* savepath, bool& downloading, char* way = "wb");

void getIpAddress(char* addr);

/* return -1 when fail, 0 otherwise */
int positive(int connfd);
int passive(int connfd);

/* send local to remote with control connection in positive mode */
int _send(int connfd, char* local, char* remote, int positiv);

#endif
