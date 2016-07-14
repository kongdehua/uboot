#ifndef _SERIAL_H_
#define _SERIAL_H_

#include "s3c2440.h"
void restoreUart();
void useTmpUart() ;

void initUART();
int serial_getc(void);
void serial_putc(const char c, int bConvert);
void serial_puts (const char *s);

void setCurrentUart(S3C2440_UARTS_NR nr);
S3C2440_UARTS_NR getCurrentUart();

void puts (const char *s);
int tstc (void);
int getc (void);
void putc (const char c);
int readline (const char *const prompt);

#define Tmp_Serial_puts(s) \
	useTmpUart();\
	serial_puts(s);\
	restoreUart();

#endif
