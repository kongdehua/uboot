#ifndef _LED_H_
#define _LED_H_

#include "s3c2440.h"

#define LED1_ENABLE() SET_TWO_BIT(GPFCON, 4*2, 1)
#define LED2_ENABLE() SET_TWO_BIT(GPFCON, 5*2, 1)
#define LED4_ENABLE() SET_TWO_BIT(GPFCON, 6*2, 1)

#define LED1_ON()     SET_ONE_BIT(GPFDAT, 4, 0)  
#define LED2_ON()     SET_ONE_BIT(GPFDAT, 5, 0)  
#define LED4_ON()     SET_ONE_BIT(GPFDAT, 6, 0)  

#define LED1_OFF()    SET_ONE_BIT(GPFDAT, 4, 1)  
#define LED2_OFF()    SET_ONE_BIT(GPFDAT, 5, 1)  
#define LED4_OFF()    SET_ONE_BIT(GPFDAT, 6, 1)  

void led1_on();

void led1_off();

void led2_on();

void led2_off();

void led4_on();

void led4_off();

#endif
