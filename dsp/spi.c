#include "include/spi.h"



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
