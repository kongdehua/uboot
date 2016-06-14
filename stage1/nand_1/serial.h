#ifndef _SERIAL_H_
#define _SERIAL_H_

void initUART();
int serial_getc(void);
void serial_putc(const char c);
void serial_puts (const char *s);

#endif
