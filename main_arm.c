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
        unsigned char data[256];
        for(i = 0; i < 6; i++)
            data[i] = '1';
        
        for(i = 6; i < 256; i++)
            data[i] = 0xAA;
		data[7] = 2;
            
        ReCATSyslinkSend(data, 256);
        printf("[send] %s\n", data);

        size_t len;
        char* buffer = ReCATSyslinkRecv(&len);
        printf("[recv] %s\n", buffer);
        free(buffer);

    }


    return 0;
}


