#ifndef _S3C2440_H_
#define _S3C2440_H_

#include "standard.h"

typedef volatile u8  S3C2440_REG8;
typedef volatile u32 S3C2440_REG32;

#define CONFIG_BAUDRATE 115200
#define CFG_MAXARGS   16    /* max number of command args */

#define CLK_INPUT_FREQ 12000000

#define S3C2440_CLOCK_POWER_BASE 0x4c000000

#define WATCHDOG ((volatile unsigned long*)0x53000000)

#define GPBCON ((volatile unsigned long*)0x56000010)
#define GPBDAT ((volatile unsigned long*)0x56000014)
#define GPBUP  ((volatile unsigned long*)0x56000018)

#define GPECON ((volatile unsigned long*)0x56000040)
#define GPEDAT ((volatile unsigned long*)0x56000044)
#define GPEUP  ((volatile unsigned long*)0x56000048)

#define GPFCON ((volatile unsigned long*)0x56000050)
#define GPFDAT ((volatile unsigned long*)0x56000054)
#define GPFUP  ((volatile unsigned long*)0x56000058)

#define GPGCON ((volatile unsigned long*)0x56000060)
#define GPGDAT ((volatile unsigned long*)0x56000064)
#define GPGUP  ((volatile unsigned long*)0x56000068)

#define GPHCON ((volatile unsigned long*)0x56000070)
#define GPHDAT ((volatile unsigned long*)0x56000074)
#define GPHUP  ((volatile unsigned long*)0x56000078)

#define S3C2440_MEMCTL_BASE 0x48000000
#define S3C2440_NAND_BASE   0x4E000000
#define S3C2440_UART_BASE   0x50000000
#define S3C2440_SDI_BASE    0x5A000000
#define S3C2440_SPI_BASE    0x59000020 


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

// NAND Flash
typedef struct {
	S3C2440_REG32 NFCONF;
	S3C2440_REG32 NFCONT;
	S3C2440_REG32 NFCMMD;
	S3C2440_REG32 NFADDR;
	S3C2440_REG32 NFDATA;
	S3C2440_REG32 NFMECCD0;
	S3C2440_REG32 NFMECCD1;
	S3C2440_REG32 NFSECCD;
	S3C2440_REG32 NFSTAT;
	S3C2440_REG32 NFESTAT0;
	S3C2440_REG32 NFESTAT1;
	S3C2440_REG32 NFMECC0;
	S3C2440_REG32 NFMECC1;
	S3C2440_REG32 NFSECC;
	S3C2440_REG32 NFSBLK;
	S3C2440_REG32 NFEBLK;
} S3C2440_NAND;

static inline S3C2440_NAND * S3C2440_GetBase_NAND()
{
  return (S3C2440_NAND * const)(S3C2440_NAND_BASE);
}  

typedef struct _S3C2440_SDI
{
	S3C2440_REG32 SDICON;
	S3C2440_REG32 SDIPRE;
	S3C2440_REG32 SDICmdArg;
	S3C2440_REG32 SDICmdCon;
	S3C2440_REG32 SDICmdSta;
	S3C2440_REG32 SDIRSP0;
	S3C2440_REG32 SDIRSP1;
	S3C2440_REG32 SDIRSP2;
	S3C2440_REG32 SDIRSP3;
	S3C2440_REG32 SDIDTimer;
	S3C2440_REG32 SDIBSize;
	S3C2440_REG32 SDIDatCon;
	S3C2440_REG32 SDIDatCnt;
	S3C2440_REG32 SDIDatSta;
	S3C2440_REG32 SDIFSTA;
	S3C2440_REG32 SDIIntMsk;
	S3C2440_REG32 SDIDAT;
} S3C2440_SDI;

static inline S3C2440_SDI * S3C2440_GetBase_SDI()
{
  return (S3C2440_SDI * const)(S3C2440_SDI_BASE);
}  

/* SPI (see manual chapter 22) */
#define S3C2440_SPI_CHANNELS  1

typedef struct {
	S3C2440_REG32   SPCON;       
	S3C2440_REG32   SPSTA;     
	S3C2440_REG32   SPPIN;       
	S3C2440_REG32   SPPRE;     
	S3C2440_REG32   SPTDAT;    
	S3C2440_REG32   SPRDAT;    
	S3C2440_REG32   res[2];      
} S3C2440_SPI_CHANNEL;

typedef struct {               
	S3C2440_SPI_CHANNEL ch[S3C2440_SPI_CHANNELS];
} /*__attribute__((__packed__))*/ S3C2440_SPI;

static inline S3C2440_SPI_CHANNEL * const S3C2440_GetBase_SPI(void)
{    
	return (S3C2440_SPI_CHANNEL * const)S3C2440_SPI_BASE;
}   

/*interrupt registes*/
#define SRCPND              (*(volatile unsigned long *)0x4A000000)
#define INTMOD              (*(volatile unsigned long *)0x4A000004)
#define INTMSK              (*(volatile unsigned long *)0x4A000008)
#define PRIORITY            (*(volatile unsigned long *)0x4A00000c)
#define INTPND              (*(volatile unsigned long *)0x4A000010)
#define INTOFFSET           (*(volatile unsigned long *)0x4A000014)
#define SUBSRCPND           (*(volatile unsigned long *)0x4A000018)
#define INTSUBMSK           (*(volatile unsigned long *)0x4A00001c)

/*external interrupt registers*/
#define EINTMASK            (*(volatile unsigned long *)0x560000a4)
#define EINTPEND            (*(volatile unsigned long *)0x560000a8)

#endif
