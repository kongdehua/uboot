#include "spi.h"

#include "s3c2440.h"

#define ENABLE_SPI1_CLK() \
	SET_TWO_BIT(GPGCON, 7 * 2, 0x3); \
	SET_ONE_BIT(GPGUP , 7, 0x1); \

#define ENABLE_SPI1_MOSI()\
	SET_TWO_BIT(GPGCON, 6 * 2, 0x3); \
	SET_ONE_BIT(GPGUP , 6, 0x1); \

#define ENABLE_SPI1_MISO()\
	SET_TWO_BIT(GPGCON, 5 * 2, 0x3); \
	SET_ONE_BIT(GPGUP , 5, 0x1); \

#define ENABLE_SPI1() \
	ENABLE_SPI1_CLK(); \
	ENABLE_SPI1_MOSI();\
	ENABLE_SPI1_MISO();

#define SPI_TXRX_READY      (((spi->SPSTA)&0x1) == 0x1)

int loopChar=0xFF;

uchar spi_writeByte(uchar c)
{
	int j = 0;
	S3C2440_SPI_CHANNEL * spi =  S3C2440_GetBase_SPI();
	spi->SPTDAT = c;

	for(j=0;j<0xFF;j++);
	while(!SPI_TXRX_READY)
		for(j=0;j<0xFF;j++);

	return spi->SPRDAT;
}

char spi_readByte(void)
{
	int j = 0;
	char ch = 0;

	S3C2440_SPI_CHANNEL * spi =  S3C2440_GetBase_SPI();
	spi->SPTDAT = (char)loopChar;
	for(j=0;j<0xFF;j++);
	while(!SPI_TXRX_READY)
		for(j=0;j<0xFF;j++);
	ch=spi->SPRDAT;

	return ch;
}

void initSPI()
{
	ENABLE_SPI1();

	S3C2440_SPI_CHANNEL * spi =  S3C2440_GetBase_SPI(); 
	//SPI Baud Rate Prescaler Register,Baud Rate=PCLK/2/(Prescaler value+1)
	spi->SPPRE = 0x18;           
	//polling,en-sck,master,low,format A,nomal = 0 | TAGD = 1
	spi->SPCON = (0<<6)|(0<<5)|(1<<4)|(1<<3)|(0<<2)|(0<<1)|(0<<0);
	//Multi Master error detect disable,reserved,rech=*spi_sprdat0;lease
	spi->SPPIN=(0<<2)|(0<<0);    

	spi->SPSTA = 0;              
	//for (uint8_t i = 0; i < 10; i++) spi_writeByte(0XFF); 

	//spi_chip_select();
}

