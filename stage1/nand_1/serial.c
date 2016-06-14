#include "standard.h"
#include "s3c2440.h"
#include "global_data.h"

#define ENABLE_UART0_RX() \
	SET_TWO_BIT(GPHCON, 2 * 2, 0x2); \
	SET_ONE_BIT(GPHUP , 2, 0x1); \

#define ENABLE_UART0_TX()\
	SET_TWO_BIT(GPHCON, 3 * 2, 0x2); \
	SET_ONE_BIT(GPHUP , 3, 0x1); \

#define ENABLE_UART0() \
	ENABLE_UART0_RX(); \
	ENABLE_UART0_TX(); 

DECLARE_GLOBAL_DATA_PTR;
	
void initUART()
{
// 初始化UART
	ENABLE_UART0();

// 设置UART参数
	S3C2440_UART * const uart = S3C2440_GetBase_UART(S3C2440_UART0);

	// FIFO enable, Tx/Rx FIFO clear
	uart->UFCON = 0x0;
	uart->UMCON = 0x0;
	// Normal, No parity, 1 stop, 8 bit
	uart->ULCON = 0x3;
	// tx=level, rx=edge, disable timeout int., enable rx error int.
	uart->UCON  = 0x5;
	//uart->UBRDIV = get_PCLK() / (16 * 115200) - 1;
	uart->UBRDIV = 50000000 / (16 * 115200) - 1;

	int i = 0;
	for ( i = 0; i < 100; i++ )
		;
}

int serial_getc(void)
{
	S3C2440_UART * const uart = S3C2440_GetBase_UART(S3C2440_UART0);
	while (!(uart->UTRSTAT & 0x1));
	
	return uart->URXH & 0xFF;
}

void serial_putc(const char c)
{
	S3C2440_UART * const uart = S3C2440_GetBase_UART(S3C2440_UART0);

	while (!(uart->UTRSTAT & 0x2));

	uart->UTXH = c;
	if (c == '\n')
	{
		serial_putc('\r');
	}
}

void serial_puts (const char *s)
{
	while (*s)
	{
		serial_putc(*s++);
	}
}
