#ifndef _VSPRINTF_H_
#define _VSPRINTF_H_

#include <stdarg.h>

void printf (const char *fmt, ...);
int sprintf(char * buf, const char *fmt, ...);
int vsprintf(char *buf, const char *fmt, va_list args);

unsigned long simple_strtoul(const char *cp,char **endp,unsigned int base);

#endif
