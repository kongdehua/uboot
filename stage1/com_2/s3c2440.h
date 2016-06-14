#ifndef _S3C2440_H_
#define _S3C2440_H_

#include "standard.h"

typedef volatile u8  S3C2440_REG8;
typedef volatile u32 S3C2440_REG32;

#define CLK_INPUT_FREQ 12000000

#define S3C2440_CLOCK_POWER_BASE 0x4c000000

#define CONFIG_BAUDRATE 115200

#define WATCHDOG (*(volatile unsigned long*)0x53000000)

#define GPBCON (*(volatile unsigned long*)0x56000010)
#define GPBDAT (*(volatile unsigned long*)0x56000014)
#define GPBUP  (*(volatile unsigned long*)0x56000018)

#define GPFCON (*(volatile unsigned long*)0x56000050)
#define GPFDAT (*(volatile unsigned long*)0x56000054)
#define GPFUP  (*(volatile unsigned long*)0x56000058)

#define GPGCON (*(volatile unsigned long*)0x56000060)
#define GPGDAT (*(volatile unsigned long*)0x56000064)
#define GPGUP  (*(volatile unsigned long*)0x56000068)

#define GPHCON (*(volatile unsigned long*)0x56000070)
#define GPHDAT (*(volatile unsigned long*)0x56000074)
#define GPHUP  (*(volatile unsigned long*)0x56000078)

#define S3C2440_MEMCTL_BASE 0x48000000
#define S3C2440_UART_BASE   0x50000000

// CLOCK POWER MANAGEMENT
typedef struct {
	S3C2440_REG32 LOCKTIME;
	S3C2440_REG32 MPLLCON;
	S3C2440_REG32 UPLLCON;
	S3C2440_REG32 CLKCON;
	S3C2440_REG32 CLKSLOW;
	S3C2440_REG32 CLKDIVN;
	S3C2440_REG32 CAMDIVN;
} S3C2440_CLOCK_POWER;

static S3C2440_CLOCK_POWER* GetBase_CLOCK_POWER()
{
	return (S3C2440_CLOCK_POWER*)S3C2440_CLOCK_POWER_BASE;
}

// UART
typedef enum {
  S3C2440_UART0,
  S3C2440_UART1,
} S3C2440_UARTS_NR;

typedef struct {
	S3C2440_REG32 ULCON;
	S3C2440_REG32 UCON;
	S3C2440_REG32 UFCON;
	S3C2440_REG32 UMCON;
	S3C2440_REG32 UTRSTAT;
	S3C2440_REG32 UERSTAT;
	S3C2440_REG32 UFSTAT;
	S3C2440_REG32 UMSTAT;

	S3C2440_REG8    UTXH;      
	S3C2440_REG8    res1[3];   
	S3C2440_REG8    URXH;
	S3C2440_REG8    res2[3];

	S3C2440_REG32   UBRDIV;    
} S3C2440_UART;

static inline S3C2440_UART * S3C2440_GetBase_UART(S3C2440_UARTS_NR nr)
{
  return (S3C2440_UART * const)(S3C2440_UART_BASE + (nr * 0x4000));
}  

#endif
