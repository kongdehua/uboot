#ifndef CONFIG_IDENT_STRING
#define CONFIG_IDENT_STRING ""
#endif

#define U_BOOT_VERSION "U-Boot 1.1.6"
char version_string[] =
  U_BOOT_VERSION" (" __DATE__ " - " __TIME__ ")"CONFIG_IDENT_STRING;

#include "s3c2440.h"


/*
 * 初始化GPIO引脚为外部中断
 * GPIO引脚用作外部中断时，默认为低电平触发、IRQ方式(不用设置INTMOD)
 */ 
void init_irq( )
{
/*
 * S2,S3,S4对应GPF0、GPF2、GPG3
 */

	// S2,S3对应的2根引脚设为中断引脚 EINT0,ENT2
	SET_TWO_BIT(GPFCON, 0 * 2, 0x2);
	//SET_TWO_BIT(GPFCON, 2 * 2, 0x2);

	// S4对应的引脚设为中断引脚EINT11
	SET_TWO_BIT(GPGCON, 3 * 2, 0x2);

	// 对于EINT11，需要在EINTMASK寄存器中使能它
	EINTMASK &= ~(1<<11);

	/*
	 * 设定优先级：
	 * ARB_SEL0 = 00b, ARB_MODE0 = 0: REQ1 > REQ3，即EINT0 > EINT2
	 * 仲裁器1、6无需设置
	 * 最终：
	 * EINT0 > EINT2 > EINT11即K2 > K3 > K4
	 */
	PRIORITY = (PRIORITY & ((~0x01) | (0x3<<7))) | (0x0 << 7) ;

	// EINT0、EINT2、EINT8_23使能
	INTMSK   &= (~(1<<0)) & (~(1<<2)) & (~(1<<5));
}

