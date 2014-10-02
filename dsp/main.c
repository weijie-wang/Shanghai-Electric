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
void* ReCATSSPIRecv(size_t* len);
int ReCATSPIInit();

Void ReCATSPITask(UArg arg0, UArg arg1)
{
    ReCATSPIInit();
//    while(1)
//    {
//        Task_sleep (10);
//        HWREG(GPIO8OUT) = 0x1000u;
//        Task_sleep (10);
//        HWREG(GPIO8OUT) = 0x0000u;
//        HWREG(0x01C4103C) = (HWREG(0x01C4103C) & ~ 0x0000FFFFu) | 0x80; // write spi
//        //strcpy(msgbuffer,"Perfect is shit!");
//        //Message_write();
//    }
    /* Issue a Reset to the Fingerprint Sensor */
    //ResetCommand();

    /* Read the Status of the Fingerprint Sensor */
    // GetStatusCommand();
}

Void ReCATMainTask(UArg arg0, UArg arg1)
{
    /** start SPI **/
//    Task_Params taskParams;
//    Task_Params_init(&taskParams);
//    taskParams.priority = 4;
//    Task_Handle tsk_spi = Task_create (ReCATSPITask, &taskParams, NULL);

    int status = ReCATSyslinkInit(arg0);

    ReCATPrintBuffer("syslink recving!");
    while(1)
    {
        size_t len;

        char *buffer = ReCATSyslinkRecv(&len);
        ReCATPrintBuffer("syslink recv ok!");

        int i;
        for(i = 0; i < len - 1; i++)
            buffer[i] += 1;
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
