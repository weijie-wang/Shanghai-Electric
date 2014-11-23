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

    while(1)
    {
        unsigned char data[] = {0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0x01,0x02,0x01,0x01,0x32,0x32};
        ReCATSyslinkSend(data, 12);
        printf("[send] %s\n", data);

        size_t len;
        char* buffer = ReCATSyslinkRecv(&len);
        printf("[recv] %s\n", buffer);
        free(buffer);

    }


    return 0;
}


