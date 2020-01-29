#include "common.h"
#include <QDebug>
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

	return length;
}

void writeToSocket(int connfd, char* buffer, int length)
{
	int nop = write(connfd, buffer, strlen(buffer));
	connfd = nop;
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
	else
	{
		return UNKOWN;
	}
}

