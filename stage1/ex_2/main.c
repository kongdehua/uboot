#define GPBCON (*(volatile unsigned long*)0x56000010)
#define GPBDAT (*(volatile unsigned long*)0x56000014)

#define GPFCON (*(volatile unsigned long*)0x56000050)
#define GPFDAT (*(volatile unsigned long*)0x56000054)

#define SET_TWO_BIT(ADDR, StartBit, VAL) \
	ADDR &= ~(3 << StartBit);\
  ADDR |= VAL << StartBit;

#define SET_ONE_BIT(ADDR, StartBit, VAL) \
	ADDR &= ~(1 << StartBit);\
  ADDR |= VAL << StartBit;

#define LED1_ENABLE() SET_TWO_BIT(GPFCON, 4*2, 1)
#define LED2_ENABLE() SET_TWO_BIT(GPFCON, 5*2, 1)
#define LED4_ENABLE() SET_TWO_BIT(GPFCON, 6*2, 1)

#define LED1_ON()     SET_ONE_BIT(GPFDAT, 4, 0)  
#define LED2_ON()     SET_ONE_BIT(GPFDAT, 5, 0)  
#define LED4_ON()     SET_ONE_BIT(GPFDAT, 6, 0)  

#define LED1_OFF()    SET_ONE_BIT(GPFDAT, 4, 1)  
#define LED2_OFF()    SET_ONE_BIT(GPFDAT, 5, 1)  
#define LED4_OFF()    SET_ONE_BIT(GPFDAT, 6, 1)  

int main()
{
	LED1_ENABLE();
	LED2_ENABLE();
	LED4_ENABLE();

	LED1_ON();
	LED2_OFF();
	LED4_ON();
	return 0;
};

