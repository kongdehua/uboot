#ifndef _VSPRINTF_H_
#define _VSPRINTF_H_

#include <stdarg.h>

void puts (const char *s);
void printf (const char *fmt, ...);
int vsprintf(char *buf, const char *fmt, va_list args);

#endif
