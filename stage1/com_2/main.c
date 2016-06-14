//#include <common.h>
#include <stdarg.h>
#include <malloc.h>

#include "standard.h"

#include "s3c2440.h"
#include "global_data.h"

#include "timer.h"
#include "led.h"
#include "vsprintf.h"

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
/*
	LED1_ON();
	delay(80000);
	LED1_OFF();
*/
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

void init_baudrate()
{
	gd->baudrate = CONFIG_BAUDRATE;
}

void DisableWatchdog()
{
	WATCHDOG = 0;
}

/** U-Boot INITIAL CONSOLE-NOT COMPATIBLE FUNCTIONS *************************/
void serial_printf (const char *fmt, ...)
{   
	va_list args;
	unsigned int i;
	char printbuffer[CFG_PBSIZE];

	va_start (args, fmt);        

	/* For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	//i = vsprintf (printbuffer, fmt, args);
	va_end (args);               

	serial_puts (printbuffer);   
}   

int main()
{
	LED1_ENABLE();
	LED1_ON();

/*
	LED1_ON();
	delay(800000);
	LED1_OFF();
*/

	unsigned char ch = '\0';
	init_baudrate();
	clock_init();

	initUART();
	
	serial_putc('c'); 
	serial_putc('o'); 
	serial_putc('d'); 
	serial_putc('e'); 
	serial_putc('\n'); 
	serial_putc('\n'); 

	ch = '\n';
	serial_putc(ch/10+'0'); 
	serial_putc(ch%10+'0'); 
	serial_putc(' '); 

	//printf1("pclk scale is %d\n", get_PCLK());
	
	while(1)
	{
		ch = serial_getc();

		//if (ch <= '9' && ch >= '0')
		if (ch == '\r')
			ch = '\n';
		serial_putc(ch);
	};

	return 0;
};

int raise (int num)
{
	return 0;
}

