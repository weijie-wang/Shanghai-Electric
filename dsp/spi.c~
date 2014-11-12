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
#define PINMUX5 0x01C14134
#define PINMUX18 0x01C14168
#define GPIO_BASE 0x01E26000
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



void ReCATSPIClear()
{

}

void ReCATSPISend(void *data, size_t len)
{
    unsigned char *p_tx = data;
    
    while(len > 0)
    {
        len--;
        while (HWREG( SPIFLG) & 0x00200 == 0);
            //Task_sleep(1);
        HWREG(SPIDAT0) = *p_tx; // write spi
        p_tx++;
    }
}
size_t ReCATSPIRecv(unsigned char *data, size_t len)
{
    size_t ret = len;
    while(len > 0)
    {
        while (HWREG( SPIFLG) & 0x00200 == 0);
            //Task_sleep(1);
        HWREG(SPIDAT0) = 0x88; // write spi
        
        while (HWREG( SPIFLG) & 0x00100 == 0);
        *data = (char)(HWREG(SPIBUF)&0x0000FFFFu);
        len--;
        data++;
    }
    return ret;
}

void ReCATSPIInit()
{
    /***** GPIO Setting for CS ******/
    HWREG(PINMUX18) &= ~(0xf<<20);
    HWREG(PINMUX18) |= (0x8<<20);
    HWREG(GPIO8DIR) = 0;//0 for output
    HWREG(GPIO8OUT) = 0x0000u;

    /***** SPI INITIALIZATION ******/
    /* Waking up the SPI1 instance. */
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_SPI1, PSC_POWERDOMAIN_ALWAYS_ON,
                     PSC_MDCTL_NEXT_ENABLE);


    /** setup pinmux for spi1 */
    HWREG(PINMUX5) &= ~0x00FFFF00u;
    HWREG(PINMUX5) |=  0x00110100u;

    /** spi reset */
    HWREG(SPIGCR0) = ~(0x00000001u); HWREG(SPIGCR0) =   0x00000001u;

    /** set spi as master */
    HWREG(SPIGCR1) |=  0x00000003u;

    /** set spi clk */
    unsigned int prescale = (150000000/2000) - 1;  //SPI clock frequency = SPI module clock/(PRESCALE + 1)
    HWREG(SPIFMT0) =  (0x0000FF00u & (prescale << 8));

    /** set CS and simo, somi */
    HWREG(SPIPC0) = 0x00000E00;   //SPI Pin function no CS is selected
    HWREG(SPIDEF) = 0x00;         //SPIDefaultCSSet

    /** set up format */
    HWREG(SPIFMT0) |= 0x00020000u;   // SPI clock signal is delayed by a half SPI clock cycle
    HWREG(SPIFMT0) &= ~0x00100000u;   /* Configures SPI to transmit MSB bit First during data transfer */
    HWREG(SPIFMT0) &= ~0x0000001Fu;
    HWREG(SPIFMT0) |= 8; /* Sets the Charcter length */
    HWREG(SPIDAT1) |= 0x00000000; /* make it sense */

    /** setup interrupts. */
    /* No Interrupt */
    HWREG(SPIINT0) =0x00000000;
    HWREG(SPILVL) =0x00000000;


    /** enable spi */
    HWREG(SPIGCR1) |= 0x01000000;


}
