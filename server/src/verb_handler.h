#ifndef VERB_HANDLER
#define VERB_HANDLER
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
#include <limits.h>
#include <pthread.h>
#include "common.h"

extern int level;

/* parse the command verb and parameter, return 0 if successful, else, return error No. and put reply into parameterBuffer */
int commandParser(char* commandBuffer, VERB* verb, char* parameterBuffer);

/* Execute the command, return error No., 0 if no error */
int Execute(STATUS* state, int connfd, char* parameter, VERB verb, int* dataconnfd, char* currentDir, int* currentPos);

#endif
