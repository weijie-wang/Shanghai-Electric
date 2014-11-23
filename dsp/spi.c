#include "include/spi.h"

Hwi_Handle myHwi;
size_t tx_len = 0;
size_t rx_len = 0;
unsigned char* p_rx;
unsigned char* p_tx;

void ReCATSPIClear()
{

}

void ReCATSPISend(void *data, size_t len)
{

	if(!tx_len)
	{
		p_tx = data;
		tx_len = len;
	
		HWREG(SPIINT0) |= 0x00000200;
		HWREG(SPILVL)  |= 0x00000200;
	}
	while(tx_len)
	{
		Task_sleep(1);
	};

  
}
size_t ReCATSPIRecv(unsigned char *data, size_t len)
{
	if(!rx_len)
	{
		p_rx = data;
		rx_len = len;
		HWREG(SPIINT0) |= 0x00000100;
		HWREG(SPILVL)  |= 0x00000100;
		HWREG(SPIDAT0) = 0xAA;
	}
	
	while(rx_len)
	{
		Task_sleep(1);
	};	
    size_t ret = len;
    return ret;
}

void myIsr()
{
	unsigned int intCode = 0;
	Hwi_clearInterrupt(4);
	intCode = HWREG(INTVEC1)>>1;
	while(intCode)
	{
		if(intCode == 0x14)
		{
			tx_len--;
			HWREG(SPIDAT0) = *p_tx; // write spi
			p_tx++;
			if(!tx_len)
			{
				HWREG(SPIINT0) &= ~(0x00000200);
				HWREG(SPILVL)  &= ~(0x00000200);
			}
		}
		if(intCode == 0x12)
		{
			
			*p_rx = (char)(HWREG(SPIBUF)&0x0000FFFFu);
			rx_len--;
			p_rx++;
			HWREG(SPIDAT0) = 0xAA;
			if(!rx_len)
			{
				HWREG(SPIINT0) &= ~(0x00000100);
				HWREG(SPILVL)  &= ~(0x00000100);
			}
		}
		intCode = HWREG(INTVEC1)>>1;
		
	}
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
    unsigned int prescale = (150000000/1000000) - 1;  //SPI clock frequency = SPI module clock/(PRESCALE + 1)
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
    HWREG(SPIINT0) = 0x00000000;
    HWREG(SPILVL)  = 0x00000000;
	Hwi_Params hwiParams;
	Error_Block eb;
	Hwi_Params_init(&hwiParams);
	Error_init(&eb);
	hwiParams.eventId = 43;
	hwiParams.maskSetting = Hwi_MaskingOption_SELF;
	myHwi = Hwi_create(4, myIsr, &hwiParams, &eb);
	Hwi_enable();
	Hwi_enableInterrupt(4);


    /** enable spi */
    HWREG(SPIGCR1) |= 0x01000000;


}

