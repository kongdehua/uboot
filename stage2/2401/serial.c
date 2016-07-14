#include "standard.h"
#include "s3c2440.h"
#include "global_data.h"

#include "serial.h"
#include "string.h"
#include "timer.h"

#define ENABLE_UART0_RX() \
	SET_TWO_BIT(GPHCON, 3 * 2, 0x2); \
	SET_ONE_BIT(GPHUP , 3, 0x1); \

#define ENABLE_UART0_TX()\
	SET_TWO_BIT(GPHCON, 2 * 2, 0x2); \
	SET_ONE_BIT(GPHUP , 2, 0x1); \

#define ENABLE_UART0() \
	ENABLE_UART0_RX(); \
	ENABLE_UART0_TX(); 

#define ENABLE_UART1_RX() \
	SET_TWO_BIT(GPHCON, 5 * 2, 0x2); \
	SET_ONE_BIT(GPHUP , 5, 0x1); \

#define ENABLE_UART1_TX()\
	SET_TWO_BIT(GPHCON, 4 * 2, 0x2); \
	SET_ONE_BIT(GPHUP , 4, 0x1); \

#define ENABLE_UART1() \
	ENABLE_UART1_RX(); \
	ENABLE_UART1_TX(); 

DECLARE_GLOBAL_DATA_PTR;
	
static S3C2440_UARTS_NR g_curUart = 0;
static S3C2440_UARTS_NR g_default = 0;
static S3C2440_UARTS_NR g_tmpUart = 0;

void initUART()
{
// 初始化UART0
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

// 初始化UART1
	ENABLE_UART1();

// 设置UART参数
	S3C2440_UART * const uart1 = S3C2440_GetBase_UART(S3C2440_UART1);

	// FIFO enable, Tx/Rx FIFO clear
	uart1->UFCON = 0x0;
	uart1->UMCON = 0x0;
	// Normal, No parity, 1 stop, 8 bit
	uart1->ULCON = 0x3;
	// tx=level, rx=edge, disable timeout int., enable rx error int.
	uart1->UCON  = 0x5;
	//uart->UBRDIV = get_PCLK() / (16 * 115200) - 1;
	uart1->UBRDIV = 50000000 / (16 * 115200) - 1;

	int i = 0;
	for ( i = 0; i < 100; i++ )
		;

	g_curUart = S3C2440_UART1;
	g_default = S3C2440_UART1;
	g_tmpUart = S3C2440_UART0;
}

void setCurrentUart(S3C2440_UARTS_NR nr) {g_curUart = nr;} 
S3C2440_UARTS_NR getCurrentUart() { return g_curUart;} 

void restoreUart() {setCurrentUart( g_default );}
void useTmpUart()  {setCurrentUart( g_tmpUart );}
/*    
 * Test whether a character is in the RX buffer
 */   
int serial_tstc (void)         
{     
	S3C2440_UART * const uart = S3C2440_GetBase_UART(g_curUart); 
	return uart->UTRSTAT & 0x1;  
} 

int serial_getc(void)
{
	S3C2440_UART * const uart = S3C2440_GetBase_UART(g_curUart);
	while (!(uart->UTRSTAT & 0x1));
	
	return uart->URXH & 0xFF;
}

void serial_putc(const char c, int bConvert)
{
	S3C2440_UART * const uart = S3C2440_GetBase_UART(g_curUart);

	while (!(uart->UTRSTAT & 0x2));

	uart->UTXH = c;
	if (bConvert && c == '\n')
	{
		serial_putc('\r', 0);
	}
}

void serial_puts (const char *s)
{
	while (*s)
	{
		serial_putc(*s++, 1);
	}
}


void puts (const char *s)
{   
	/* Send directly to the handler */
	serial_puts (s);
}

int tstc (void)
{   
	/* Send directly to the handler */
	return serial_tstc ();
}


char        console_buffer[CFG_CBSIZE];   /* console I/O buffer */
static char * delete_char (char *buffer, char *p, int *colp, int *np, int plen);
static char erase_seq[] = "\b \b";    /* erase sequence */
static char   tab_seq[] = "        ";   /* used to expand TABs  */

/*
 * Prompt for input and read a line.
 * If  CONFIG_BOOT_RETRY_TIME is defined and retry_time >= 0,
 * time out when time goes past endtime (timebase time in ticks).
 * Return:  number of read characters
 *    -1 if break
 *    -2 if timed out
 */
int readline (const char *const prompt)
{
  char   *p = console_buffer;

  int n = 0;        /* buffer index   */
  int plen = 0;     /* prompt length  */
  int col;        /* output column cnt  */
  char  c;

  /* print prompt */
  if (prompt) {
    plen = strlen (prompt);
    puts (prompt);
  }
  col = plen;

  for (;;) {
#ifdef CONFIG_BOOT_RETRY_TIME
    while (!tstc()) { /* while no incoming data */
      if (retry_time >= 0 && get_ticks() > endtime)
        return (-2);  /* timed out */
    }
#endif

#ifdef CONFIG_SHOW_ACTIVITY
    while (!tstc()) {
      extern void show_activity(int arg);
      show_activity(0);
    }
#endif
    c = getc();
    /*
     * Special character handling
     */
    switch (c) {
    case '\r':        /* Enter    */
    case '\n':
      *p = '\0';
      puts ("\r\n");
      return (p - console_buffer);

    case '\0':        /* nul      */
      continue;

    case 0x03:        /* ^C - break   */
      console_buffer[0] = '\0'; /* discard input */
      return (-1);

    case 0x04:        /* ^D - break   */
      console_buffer[0] = '\0'; /* discard input */
      return (-2);

    case 0x15:        /* ^U - erase line  */
      while (col > plen) {
        puts (erase_seq);
        --col;
      }
      p = console_buffer;
      n = 0;
      continue;

    case 0x17:        /* ^W - erase word  */
      p=delete_char(console_buffer, p, &col, &n, plen);
      while ((n > 0) && (*p != ' ')) {
        p=delete_char(console_buffer, p, &col, &n, plen);
      }
      continue;

    case 0x08:        /* ^H  - backspace  */
    case 0x7F:        /* DEL - backspace  */
      p=delete_char(console_buffer, p, &col, &n, plen);
      continue;

    default:
      /*
       * Must be a normal character then
       */
      if (n < CFG_CBSIZE-2) {
        if (c == '\t') {  /* expand TABs    */
#ifdef CONFIG_AUTO_COMPLETE
          /* if auto completion triggered just continue */
          *p = '\0';
          if (cmd_auto_complete(prompt, console_buffer, &n, &col)) {
            p = console_buffer + n; /* reset */
            continue;
          }
#endif
          puts (tab_seq+(col&07));
          col += 8 - (col&07);
        } else {
          ++col;    /* echo input   */
          putc (c);
        }
        *p++ = c;
        ++n;
      } else {      /* Buffer full    */
        putc ('\a');
      }
    }
  }
}

/** U-Boot INITIAL CONSOLE-COMPATIBLE FUNCTION *****************************/

int getc (void)                
{
  /* Send directly to the handler */
  return serial_getc ();       
}

void putc (const char c)
{
	/* Send directly to the handler */
	serial_putc (c, 0);
}

static char * delete_char (char *buffer, char *p, int *colp, int *np, int plen)
{
  char *s;

  if (*np == 0) {
    return (p);
  }

  if (*(--p) == '\t') {     /* will retype the whole line */
    while (*colp > plen) {
      puts (erase_seq);
      (*colp)--;
    }
    for (s=buffer; s<p; ++s) {
      if (*s == '\t') {
        puts (tab_seq+((*colp) & 07));
        *colp += 8 - ((*colp) & 07);
      } else {
        ++(*colp);
        putc (*s);
      }
    }
  } else {
    puts (erase_seq);
    (*colp)--;
  }
  (*np)--;
  return (p);
}

