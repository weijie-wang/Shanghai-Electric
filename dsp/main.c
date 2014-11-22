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
#include "include/spi.h"
#include "include/syslink.h"

#define 	flag_len 			6

unsigned char SPI_rx_data[256];
extern ReCATSyslink syslink;

Void ReCATSPITask(UArg arg0, UArg arg1)
{   
    unsigned char SPI_tx_data[100];
	memset(SPI_tx_data, 0x01, 100);
    ReCATSPIInit();
    while(1)
    { 
      //ReCATSPISend(&SPI_tx_data[0], 100);
	  ReCATSPISend_test(&SPI_tx_data[0]);
      //ReCATSPIRecv(SPI_rx_data, 256);
      Task_sleep (10);
    }
}

Void ReCATMainTask(UArg arg0, UArg arg1)
{
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
		
		UInt32 dev_num;
        dev_num = *( buffer + flag_len + 1) & 0x0F;
        switch ( dev_num )
        {
            case   1 :
                      /***** GPIO Setting for CS0 ******/
                      HWREG(PINMUX5) &= ~(0xf);
                      HWREG(PINMUX5) |= (0x8);
                      HWREG(GPIO2DIR) = 0;//0 for output
                      HWREG(GPIO2OUT) &= ~(0x00008000u);
    
                      // Semaphore_post(syslink.SpiLock);
                      // Semaphore_pend(syslink.SysLock, BIOS_WAIT_FOREVER) ;
                      
                      // /***** GPIO Clear for CS0 ******/
                      // HWREG(PINMUX5) &= ~(0xf);
                      // HWREG(PINMUX5) |= (0x8);
                      // HWREG(GPIO2DIR) = 0;//0 for output
                      // HWREG(GPIO2OUT) |= 0x00008000u;
                      break;
                      
            case   2 :
                      /***** GPIO Setting for CS1 ******/
                      HWREG(PINMUX4) &= ~(0xf<<12);
                      HWREG(PINMUX4) |= (0x8<<12);
                      HWREG(GPIO1DIR) = 0;//0 for output
                      HWREG(GPIO1OUT) &= ~(0x00100000u);
                      
                      // Semaphore_post(syslink.SpiLock);
                      // Semaphore_pend(syslink.SysLock, BIOS_WAIT_FOREVER) ;
                      
                      // /***** GPIO Clear for CS1 ******/
                      // HWREG(PINMUX4) &= ~(0xf<<12);
                      // HWREG(PINMUX4) |= (0x8<<12);
                      // HWREG(GPIO1DIR) = 0;//0 for output
                      // HWREG(GPIO1OUT) |= 0x00100000u;
                      break;
            
            default  : 
                      /***** GPIO Setting for CS2 ******/
                      HWREG(PINMUX4) &= ~(0xf<<8);
                      HWREG(PINMUX4) |= (0x8<<8);
                      HWREG(GPIO1DIR) = 0;//0 for output
                      HWREG(GPIO1OUT) &= ~(0x00200000u);
                      
                      // Semaphore_post(syslink.SpiLock);
                      // Semaphore_pend(syslink.SysLock, BIOS_WAIT_FOREVER) ;
                      
                      // /***** GPIO Clear for CS2 ******/
                      // HWREG(PINMUX4) &= ~(0xf<<8);
                      // HWREG(PINMUX4) |= (0x8<<8);
                      // HWREG(GPIO1DIR) = 0;//0 for output
                      // HWREG(GPIO1OUT) |= 0x00200000u;
                      break;
        }

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
