#include "standard.h"
#include "vsprintf.h"
#include "string.h"
#include "serial.h"

#include "led.h"

void EINT_Handle()
{
	led1_off();
	led2_off();
	led4_off();
	//SET_TWO_BIT(GPFCON, 4*2, 1);
	//SET_ONE_BIT(GPFDAT, 4, 0);

	unsigned long oft = INTOFFSET;
	unsigned long val;

	switch( oft )
	{
		// S2被按下
		case 0: 
			{   
				led1_on();
				break;
			}

			// S3被按下
		case 2:
			{   
				led2_on();
				break;
			}

			// K4被按下
		case 5:
			{   
				led4_on();
				break;
			}

		default:
			break;
	}

	//清中断
	if( oft == 5 ) 
		EINTPEND = (1<<11);   // EINT8_23合用IRQ5
	SRCPND = 1<<oft;
	INTPND = 1<<oft;
}

