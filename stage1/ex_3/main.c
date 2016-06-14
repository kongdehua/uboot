#define GPBCON (*(volatile unsigned long*)0x56000010)
#define GPBDAT (*(volatile unsigned long*)0x56000014)

#define GPFCON (*(volatile unsigned long*)0x56000050)
#define GPFDAT (*(volatile unsigned long*)0x56000054)

#define GPGCON (*(volatile unsigned long*)0x56000060)
#define GPGDAT (*(volatile unsigned long*)0x56000064)

#define SET_TWO_BIT(ADDR, StartBit, VAL) \
	do\
	{\
		unsigned long l = ADDR;\
    l &= (~(3 << StartBit));\
		l |= (VAL << StartBit);\
		ADDR = l;\
	}\
	while(0)

#define SET_ONE_BIT(ADDR, StartBit, VAL) \
	do\
	{\
		unsigned long l = ADDR; \
		l &= (~(1 << StartBit));\
		l |= (VAL << StartBit) ;\
		ADDR = l; \
	}\
	while(0)

#define GET_ONE_BIT(ADDR, StartBit) \
	((ADDR & (1 << StartBit)) >> StartBit)

#define LED1_ENABLE() SET_TWO_BIT(GPFCON, 4*2, 1)
#define LED2_ENABLE() SET_TWO_BIT(GPFCON, 5*2, 1)
#define LED4_ENABLE() SET_TWO_BIT(GPFCON, 6*2, 1)

#define LED1_ON()     SET_ONE_BIT(GPFDAT, 4, 0)  
#define LED2_ON()     SET_ONE_BIT(GPFDAT, 5, 0)  
#define LED4_ON()     SET_ONE_BIT(GPFDAT, 6, 0)  

#define LED1_OFF()    SET_ONE_BIT(GPFDAT, 4, 1)  
#define LED2_OFF()    SET_ONE_BIT(GPFDAT, 5, 1)  
#define LED4_OFF()    SET_ONE_BIT(GPFDAT, 6, 1)  

#define INT0_ENABLE()  SET_TWO_BIT(GPFCON, 0*2, 0)
#define INT2_ENABLE()  SET_TWO_BIT(GPFCON, 2*2, 0)
#define INT11_ENABLE() SET_TWO_BIT(GPGCON, 3*2, 0)

#define GET_INT0()    !GET_ONE_BIT(GPFDAT, 0)
#define GET_INT2()    !GET_ONE_BIT(GPFDAT, 2)
#define GET_INT11()   !GET_ONE_BIT(GPGDAT, 3)

int main()
{
	LED1_ENABLE(); INT0_ENABLE();
	LED2_ENABLE(); INT2_ENABLE();
	LED4_ENABLE(); INT11_ENABLE();

	while(1)
	{
		if (GET_INT0()) LED1_ON();
		else            LED1_OFF();
		
		if (GET_INT2()) LED2_ON();
		else            LED2_OFF();

		
		if (GET_INT11()) LED4_ON();
		else            LED4_OFF();
	}

	return 0;
};

/*
		if (GET_ONE_BIT(ADDR, StartBit) != VAL) \
		{ \
		if (VAL == 0) ADDR &= (~(1 << StartBit));\
		else          ADDR |=   (1 << StartBit) ;\
		} \
*/
