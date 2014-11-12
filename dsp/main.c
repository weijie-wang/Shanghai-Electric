#include <xdc/std.h>
#include <string.h>

#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/IHeap.h>

#include <ti/ipc/GateMP.h>
#include <ti/ipc/Ipc.h>
#include <ti/sdo/ipc/Ipc.h>
#include <ti/ipc/MessageQ.h>
#include <ti/ipc/HeapMemMP.h>
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/Notify.h>
#include <ti/sysbios/hal/Hwi.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/hal/Cache.h>
#include <xdc/cfg/global.h>

/***********SYSLINK*********/
void ReCATSyslinkClear();
void ReCATSyslinkSend(void *data, size_t len);
void* ReCATSyslinkRecv(size_t* len);
int ReCATSyslinkInit(UInt16 armID);
int ReCATPrintBuffer(char *fmt, ...);

/***********SPI*************/
void ReCATSPIClear();
void ReCATSPISend(void *data, size_t len);
size_t ReCATSPIRecv(unsigned char *data, size_t len);
int ReCATSPIInit();
unsigned char SPI_rx_data[256];

Void ReCATSPITask(UArg arg0, UArg arg1)
{   
    unsigned char SPI_tx_data[1]=0x30;
    ReCATSPIInit();
    while(1)
    { 
      //ReCATSPISend(&SPI_tx_data[0], 1);
      ReCATSPIRecv(SPI_rx_data, 256);
      Task_sleep (10);
    }
}
void ReCATDEFTask(UArg arg0, UArg arg1)
{


	while(1)
	{
	
	
	
	
	  Task_sleep (10);
	}
}
Void ReCATMainTask(UArg arg0, UArg arg1)
{
	/** Start defender **/
	Task_Params taskParams_DEF;
    Task_Params_init(&taskParams_DEF);
    taskParams_DEF.priority = 5;
    Task_Handle tsk_def = Task_create (ReCATDEFTask, &taskParams_DEF, NULL);
	
	
    /** start SPI **/
    Task_Params taskParams;
    Task_Params_init(&taskParams);
    taskParams.priority = 4;
    Task_Handle tsk_spi = Task_create (ReCATSPITask, &taskParams, NULL);

    int status = ReCATSyslinkInit(arg0);

    ReCATPrintBuffer("syslink recving!");
    while(1)
    {
        size_t len;

        char *buffer = ReCATSyslinkRecv(&len);
        ReCATPrintBuffer("syslink recv ok!");

        int i;
        //for(i = 0; i < len - 1; i++)
            buffer[1] = (SPI_rx_data[0] & 0x0f) + '0';
            buffer[0] = (SPI_rx_data[0] >> 4) + '0';
        ReCATSyslinkSend(buffer, len);
        free(buffer);
    }
}

Int main(Int argc, Char* argv[])
{
    Int status = Ipc_S_SUCCESS;

    do {
        /* Call Ipc_start() */
        status = Ipc_start();
    } while (status != Ipc_S_SUCCESS);

    BIOS_start();
    return (0);
}
