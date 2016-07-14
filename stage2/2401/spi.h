#ifndef _SPI_H_
#define _SPI_H_

#include "standard.h"

void initSPI();
char spi_readByte(void);
uchar spi_writeByte(uchar c);

#endif
