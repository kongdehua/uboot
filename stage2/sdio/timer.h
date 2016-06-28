#ifndef _TIMER_H_
#define _TIMER_H_

#include "standard.h"

void delay (unsigned long loops);
void clock_init(void);

ulong get_MPLLCLK();
ulong get_FCLK();
ulong get_HCLK();
ulong get_PCLK();

#endif
