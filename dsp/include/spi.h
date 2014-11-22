#ifndef H_SPI
#define H_SPI

#include <xdc/std.h>
#include <string.h>

/*  ----------------------------------- XDC.RUNTIME module Headers    */
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/IHeap.h>

/*  ----------------------------------- IPC module Headers           */
#include <ti/ipc/GateMP.h>
#include <ti/ipc/Ipc.h>
#include <ti/sdo/ipc/Ipc.h>
#include <ti/ipc/MessageQ.h>
#include <ti/ipc/HeapMemMP.h>
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/Notify.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Task.h>

#include "include/hw_types.h"
#include "include/soc_C6748.h"
#include "include/hw_psc_C6748.h"

#define PINMUX0 0x01C14120
#define PINMUX4 0x01C14130
#define PINMUX5 0x01C14134
#define PINMUX18 0x01C14168
#define GPIO_BASE 0x01E26000
#define GPIO1DIR (0x10 + GPIO_BASE)
#define GPIO1OUT (0x14 + GPIO_BASE)
#define GPIO2DIR (0x38 + GPIO_BASE)
#define GPIO2OUT (0x3C + GPIO_BASE)
#define GPIO8DIR (0xB0 + GPIO_BASE)
#define GPIO8OUT (0xB4 + GPIO_BASE)


/******* SPI Register ********/
#define SPI_BASE 0x01F0E000
#define SPIGCR0 SPI_BASE + 0x00
#define SPIGCR1 SPI_BASE + 0x04
#define SPIINT0 SPI_BASE + 0x08
#define SPILVL  SPI_BASE + 0x0C
#define SPIFLG  SPI_BASE + 0x10
#define SPIPC0  SPI_BASE + 0x14
#define SPIPC1  SPI_BASE + 0x18
#define SPIPC2  SPI_BASE + 0x1c
#define SPIPC3  SPI_BASE + 0x20
#define SPIPC4  SPI_BASE + 0x24
#define SPIPC5  SPI_BASE + 0x28
#define SPIDAT0 SPI_BASE + 0x38
#define SPIDAT1 SPI_BASE + 0x3C

#define SPIBUF  SPI_BASE + 0x40
#define SPIEMU  SPI_BASE + 0x44
#define SPIDELAY SPI_BASE+ 0x48
#define SPIDEF  SPI_BASE + 0x4C
#define SPIFMT0 SPI_BASE + 0x50
#define SPIFMT1 SPI_BASE + 0x54
#define SPIFMT2 SPI_BASE + 0x58
#define SPIFMT3 SPI_BASE + 0x5C
#define INTVEC1 SPI_BASE + 0x64

#define SIMO_SOMI_CLK_CS        0x00000E00
#define CHAR_LENGTH             0x8


#define SYS_INT_SPI0_INT 			37
#define SYS_INT_SPI1_INT 			43


#define SPI_TX_BUF_EMPTY            0x14
#define SPI_RECV_FULL               0x12


/***********SPI*************/
extern void ReCATSPIClear();
extern void ReCATSPISend(void *data, size_t len);
extern size_t ReCATSPIRecv(unsigned char *data, size_t len);
extern void ReCATSPIInit();


#endif
