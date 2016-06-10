# 第三个例子 `ex_3`
实现按键点灯程序：程序中需要获取ARM芯片的多个引脚，每个引脚可以控制一个按键。

1. 按键原理图

![按键原理图](https://github.com/kongdehua/uboot/raw/master/stage1/ex_3/image/switch.png "按键原理图")

![按键原理图](https://github.com/kongdehua/uboot/raw/master/stage1/ex_3/image/switch2.png "按键原理图")

2. 控制管脚原理图

![控制管脚原理图](https://github.com/kongdehua/uboot/raw/master/stage1/ex_3/image/gpio_INT_0_2.png "控制管脚原理图")

![控制管脚原理图](https://github.com/kongdehua/uboot/raw/master/stage1/ex_3/image/gpio_INT_11.png "控制管脚原理图")

从上面两幅图中可以看出，S2开关受nINT0引脚控制，

从下图两幅图中可以看到，nINT0引脚实际就是EINT0/GPF0引脚；

从原理图上可以看出，只需将EINT0/GPF0引脚设置为输入，就可以获取S2开关是否按下；

同样，EINT2/GPF2引脚控制S3；

同样，EINT11/GPG3引脚控制S4；

### 分析
从上图中可以看出，配置GPF0，GPF2，GPG3为输出端口，

例如：将GPFCON的[1:0]设置为00，就表示GPF0端口的功能为输入；

如果S2按下，则GPFDAT中[0]为0，如果没有按下，则GPFDAT中[0]为1；

### 代码实现
此代码在上一课的基础上实现

代码中展示如何获取寄存器中的某一位

```c 
#define GPFCON (*(volatile unsigned long*)0x56000050)
#define GPFDAT (*(volatile unsigned long*)0x56000054)

#define GPGCON (*(volatile unsigned long*)0x56000060)
#define GPGDAT (*(volatile unsigned long*)0x56000064)

#define GET_ONE_BIT(ADDR, StartBit) \
	((ADDR & (1 << StartBit)) >> StartBit)

#define INT0_ENABLE()  SET_TWO_BIT(GPFCON, 0*2, 0)
#define INT2_ENABLE()  SET_TWO_BIT(GPFCON, 2*2, 0)
#define INT11_ENABLE() SET_TWO_BIT(GPGCON, 3*2, 0)

#define GET_INT0()    !GET_ONE_BIT(GPFDAT, 0)
#define GET_INT2()    !GET_ONE_BIT(GPFDAT, 2)
#define GET_INT11()   !GET_ONE_BIT(GPGDAT, 3)
	
int main()
{
	LED1_ENABLE(); INT0_ENABLE();
	LED4_ENABLE(); INT11_ENABLE();
	LED2_ENABLE(); INT2_ENABLE();

	while(1)
	{
		if (GET_INT0()) LED1_ON();
		else            LED1_OFF();
		
		if (GET_INT2()) LED2_ON();
		else            LED2_OFF();
	}
	return 0;
};
```
