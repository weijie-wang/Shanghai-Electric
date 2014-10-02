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



volatile unsigned int flag = 1;
unsigned int tx_len;
unsigned int rx_len;
unsigned char tx_data[260];
volatile unsigned char rx_data[260];
unsigned char *p_tx;
volatile unsigned char *p_rx;
volatile unsigned char DataReceived[16];
Hwi_Handle myHwi;



//static void SetUpInt(void)
//{
//    Hwi_Params hwiParams;
//    Error_Block eb;

//    Hwi_Params_init(&hwiParams);
//    Error_init(&eb);



//    // set the event id 37 of the peripheral assigned to this interrupt
//    hwiParams.eventId = SYS_INT_SPI0_INT;

//    // don't allow any interrupt to nest
//    hwiParams.maskSetting = Hwi_MaskingOption_ALL;

//    //
//    // Configure interrupt 4 to invoke "SPIIsr".
//    // Automatically enables interrupt 4 by default
//    // set params.enableInt = FALSE if you want to control
//    // when the interrupt is enabled using Hwi_enableInterrupt()
//    //

//    myHwi = Hwi_create(4, SPIIsr, &hwiParams, &eb);

//    if (Error_check(&eb)) {
//        // handle the error
//        System_abort  ("SPI Interrupt initialization is failed\n");

//    }

//    Hwi_enableInterrupt(4);
//    Hwi_enable();


//}
//void SPIIsr(void)
//{
//    unsigned int intCode = 0;
//    Hwi_clearInterrupt(SYS_INT_SPI0_INT);
//    /*#ifdef _TMS320C6X
//    IntEventClear(SYS_INT_SPI0_INT);
//#else
//    IntSystemStatusClear(56);
//#endif*/

//    intCode = SPIInterruptVectorGet(SOC_SPI_0_REGS);

//    while (intCode)
//    {
//        if(intCode == SPI_TX_BUF_EMPTY)
//        {
//            tx_len--;
//            SPITransmitData1(SOC_SPI_0_REGS, *p_tx);
//            p_tx++;
//            if (!tx_len)
//            {
//                SPIIntDisable(SOC_SPI_0_REGS, SPI_TRANSMIT_INT);
//            }
//        }

//        if(intCode == SPI_RECV_FULL)
//        {
//            rx_len--;
//            *p_rx = (char)SPIDataReceive(SOC_SPI_0_REGS);
//            p_rx++;
//            if (!rx_len)
//            {
//                flag = 0;
//                SPIIntDisable(SOC_SPI_0_REGS, SPI_RECV_INT);
//            }
//        }

//        intCode = SPIInterruptVectorGet(SOC_SPI_0_REGS);
//    }
//}

void ReCATSPIClear();
void ReCATSPISend(void *data, size_t len);
void* ReCATSSPIRecv(size_t* len);

void ReCATSPIInit()
{
    /***** GPIO Setting for CS ******/
    HWREG(PINMUX18) &= ~(0xf<<20);
    HWREG(PINMUX18) |= (0x8<<20);
    HWREG(GPIO8DIR) = 0;//0 for output
    HWREG(GPIO8OUT) = 0x0000u;

    /***** SPI INITIALIZATION ******/
    /* Waking up the SPI1 instance. */
    PSCModuleControl(SOC_PSC_0_REGS, HW_PSC_SPI1, PSC_POWERDOMAIN_ALWAYS_ON,
                     PSC_MDCTL_NEXT_ENABLE);


    /** setup pinmux for spi1 */
    HWREG(PINMUX5) &= ~0x00FFFF00u;
    HWREG(PINMUX5) |=  0x00110100u;

    /** spi reset */
    HWREG(SPIGCR0) = ~(0x00000001u); HWREG(SPIGCR0) =   0x00000001u;

    /** set spi as master */
    HWREG(SPIGCR1) |=  0x00000003u;

    /** set spi clk */
    unsigned int prescale = (150000000/20000000) - 1;  //SPI clock frequency = SPI module clock/(PRESCALE + 1)
    HWREG(SPIFMT0) =  (0x0000FF00u & (prescale << 8));

    /** set CS and simo, somi */
    HWREG(SPIPC0) = 0x00000E00;   //SPI Pin function no CS is selected
    HWREG(SPIDEF) = 0x00;         //SPIDefaultCSSet

    /** set up format */
    HWREG(SPIFMT0) |= 0x00020000u;   // SPI clock signal is delayed by a half SPI clock cycle
    HWREG(SPIFMT0) &= ~0x00100000u;   /* Configures SPI to transmit MSB bit First during data transfer */
    HWREG(SPIFMT0) &= ~0x0000001Fu;
    HWREG(SPIFMT0) |= 8; /* Sets the Charcter length */
    HWREG(SPIDAT1) |= 0x02000000; /* make it sense */

    /** setup interrupts. */
    /* map interrupts to interrupt line INT1 */
    // SPIIntLevelSet(SOC_SPI_0_REGS, SPI_RECV_INTLVL | SPI_TRANSMIT_INTLVL);
    HWREG(SPIINT0) =0x00000000;
    HWREG(SPILVL) =0x00000000;
    //SetUpInt();


    /** enable spi */
    HWREG(SPIGCR1) |= 0x01000000;


    //    HWREG(SPIDELAY) =0x00000000;


    p_tx = &tx_data[0];
    p_rx = &rx_data[0];


    while(1)
    {
        HWREG(SPIDAT0) = SPIDAT0; // write spi
        while (HWREG( SPIFLG) & 0x00200 == 0);
    }


}
