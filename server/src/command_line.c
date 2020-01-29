#include "command_line.h"
#include <string.h>
#include <stdlib.h>

/* return false when no efficient argument found, return true otherwise */
int parseCommandLine(int argc, char** argv, int* port, char** root)
{

	*root = "/tmp";
	*port = 21;

    int result = 0;
    int index = 1;

    while (index < argc)
    {
        /* get port number */
        if (strncmp(argv[index], "-port", 5) == 0)
        {
            *port = atoi(argv[index + 1]);
            result = 1;
        }

        /* get root path */
        else if (strncmp(argv[index], "-root", 5) == 0)
        {
            *root = argv[index + 1];
            result = 1;
        }

        index++;
    }

    return result;
}
