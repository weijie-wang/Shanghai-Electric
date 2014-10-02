#include <stdio.h>
#include <stdlib.h>
#include "syslink.h"

int main (int argc, char ** argv)
{
    int i = 0;
    if( ReCATSyslinkInit() != 0)
    {
        fprintf(stderr, "ReCATSyslinkInit error\n");
        ReCATSyslinkClear();
        return -1;
    };

    for(i = 0; i < 100; i++)
    {
        unsigned char data[] = "12345678";
        ReCATSyslinkSend(data, 9);
        printf("[send] %s\n", data);

        size_t len;
        char* buffer = ReCATSyslinkRecv(&len);
        printf("[send] %s\n", buffer);
        free(buffer);

    }


    return 0;
}


