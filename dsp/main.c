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


#define flag_len                6
#define data_len              247
#define buffer_len            256

extern ReCATSyslink syslink;
size_t len = 0;
char *buffer;
unsigned char SPI_data[buffer_len];


Void ReCATDEFTask(UArg arg0, UArg arg1)
{
	while(1)
	{
	
	
	
	
	
	
	
		Task_sleep (10);
	}
}


Void ReCATSPITask(UArg arg0, UArg arg1)
{   
    unsigned char SPI_rx_data[flag_len + 3 + data_len];
	memset(&SPI_data[0], 0x00, buffer_len);
    ReCATSPIInit();
    while(1)
    { 
	  Semaphore_pend(syslink.SpiLock, BIOS_WAIT_FOREVER) ;
      int i = 0;
      ReCATSPISend(buffer, len);
      while( i < flag_len )
      {         
        ReCATSPIRecv(&SPI_rx_data[i], 1);
        i = SPI_rx_data[i] == 0xAA ? i+1 : 0 ;
      }
      ReCATSPIRecv(&SPI_rx_data[flag_len], len - flag_len);
      buffer = &SPI_rx_data[0];
      Semaphore_post(syslink.SysLock);
	  Task_sleep (10);
      
    }
}

Void ReCATMainTask(UArg arg0, UArg arg1)
{
    /** set the lock for SPI*/
    Semaphore_Params        semParams_spi;
    Error_Block             eb_spi;
    Error_init (&eb_spi) ;
    Semaphore_Params_init (&semParams_spi) ;
    semParams_spi.mode = Semaphore_Mode_BINARY;
    syslink.SpiLock = Semaphore_create (0, &semParams_spi, &eb_spi);

    /** set the lock for SYSLINK*/
    Semaphore_Params        semParams_sys;
    Error_Block             eb_sys;
    Error_init (&eb_sys) ;
    Semaphore_Params_init (&semParams_sys) ;
    semParams_sys.mode = Semaphore_Mode_BINARY;
    syslink.SysLock = Semaphore_create (0, &semParams_sys, &eb_sys);
    
    /** start SPI **/
    Task_Params taskParams;
    Task_Params_init(&taskParams);
    taskParams.priority = 4;
    Task_Handle tsk_spi = Task_create (ReCATSPITask, &taskParams, NULL);
	
	/** start DEFENDER **/
    Task_Params taskParams_def;
    Task_Params_init(&taskParams_def);
    taskParams_def.priority = 5;
    Task_Handle tsk_def = Task_create (ReCATDEFTask, &taskParams_def, NULL);

    int status = ReCATSyslinkInit(arg0);

    ReCATPrintBuffer("syslink recving!");
    while(1)
    {
        buffer = ReCATSyslinkRecv(&len);
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
    
                      Semaphore_post(syslink.SpiLock);
                      Semaphore_pend(syslink.SysLock, BIOS_WAIT_FOREVER) ;
                      
                      /***** GPIO Clear for CS0 ******/
                      HWREG(PINMUX5) &= ~(0xf);
                      HWREG(PINMUX5) |= (0x8);
                      HWREG(GPIO2DIR) = 0;//0 for output
                      HWREG(GPIO2OUT) |= 0x00008000u;
                      break;
                      
            case   2 :
                      /***** GPIO Setting for CS1 ******/
                      HWREG(PINMUX4) &= ~(0xf<<12);
                      HWREG(PINMUX4) |= (0x8<<12);
                      HWREG(GPIO1DIR) = 0;//0 for output
                      HWREG(GPIO1OUT) &= ~(0x00100000u);
                      
                      Semaphore_post(syslink.SpiLock);
                      Semaphore_pend(syslink.SysLock, BIOS_WAIT_FOREVER) ;
                      
                      /***** GPIO Clear for CS1 ******/
                      HWREG(PINMUX4) &= ~(0xf<<12);
                      HWREG(PINMUX4) |= (0x8<<12);
                      HWREG(GPIO1DIR) = 0;//0 for output
                      HWREG(GPIO1OUT) |= 0x00100000u;
                      break;
            
            default  : 
                      /***** GPIO Setting for CS2 ******/
                      HWREG(PINMUX4) &= ~(0xf<<8);
                      HWREG(PINMUX4) |= (0x8<<8);
                      HWREG(GPIO1DIR) = 0;//0 for output
                      HWREG(GPIO1OUT) &= ~(0x00200000u);
                      
                      Semaphore_post(syslink.SpiLock);
                      Semaphore_pend(syslink.SysLock, BIOS_WAIT_FOREVER) ;
                      
                      /***** GPIO Clear for CS2 ******/
                      HWREG(PINMUX4) &= ~(0xf<<8);
                      HWREG(PINMUX4) |= (0x8<<8);
                      HWREG(GPIO1DIR) = 0;//0 for output
                      HWREG(GPIO1OUT) |= 0x00200000u;
                      break;
        }
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
