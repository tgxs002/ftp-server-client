
#include <ctype.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "command_line.h"
#include "verb_handler.h"

int level = 0;

int main(int argc, char **argv) {

    /* to hold the command line arguments */
    int * port = (int*)malloc(sizeof(int));
    char* path = NULL;	

	parseCommandLine(argc, argv, port, &path);
    
    /* listener, will stay alive through out the process */
	int listenfd;

    /* create a socket and listen to the specific port */
    listenfd = CreateAndListen(port);

    /* get path level */
    char* p = path;
    while (*p != '\0')
    {
        if (*p == '/') level++;
        p++;
    }

    while (1)
    {
        pthread_t th;
        int* connfd = (int*)malloc(sizeof(int));
        //when request arrives, accept it, and create a socket for the connector
        if ((*connfd = accept(listenfd, NULL, NULL)) == -1) {
            printf("Error accept(): %s(%d)\n", strerror(errno), errno);
            return 0;
        }
        struct info* _info = (struct info*)malloc(sizeof(struct info));

        _info->path = NULL;
        _info->fd = *connfd;
        _info->path = path;
        int ret = pthread_create(&th, NULL, (void*)&ListenAndReply, _info);
        if (ret != 0)
        {
            printf("thread create fail.\n");
        }
    }
    return 0;
}

