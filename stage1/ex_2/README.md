# 第二个例子 `ex_2`
实现点灯程序：程序中需要控制ARM芯片的多个引脚，每个引脚可以控制一个LED灯。

1. LED原理图

![LED原理图](https://github.com/kongdehua/uboot/raw/master/stage1/ex_2/image/led.png "LED原理图")

2. 控制管脚原理图

![控制管脚原理图](https://github.com/kongdehua/uboot/raw/master/stage1/ex_2/image/gpio.png "控制管脚LED原理图")

从上图中可以看出，LED D10的亮灭受nLED1引脚控制，
从下图中看到，nLED1引脚实际就是EINT4/GPF4引脚；

从原理图上可以看出，只需将EINT4/GPF4引脚设置为输出，就可以控制LED D10的亮灭；
同样，EINT5/GPF5引脚控制LED D11；
同样，EINT6/GPF6引脚控制LED D12；

## I/O PORTS

### OVERVIEW
S3C2440A has 130 multi-functional input/output port pins and there are eight ports as shown below:
	* Port A(GPA): 25-output port
	* Port B(GPB): 11-input/output port
	* Port C(GPC): 16-input/output port
	* Port D(GPD): 16-input/output port
	* Port E(GPE): 16-input/output port
	* Port F(GPF):  8-input/output port
	* Port G(GPG): 16-input/output port
	* Port H(GPH):  9-input/output port
	* Port J(GPJ): 13-input/output port
Each port can be easily configured by software to meet various system configurations and design requirements. You have to define which function of each pin is used before starting the main program. If a pin is not used for multiplexed functions, the pin can be configured as I/O ports.

### PORT CONTROL DESCRIPTIONS
#### PORT CONFIGURATION REGISTER (GPACON-GPJCON)
	In S3C2440A, most of the pins are multiplexed pins. So, it is determined which function is selected for each pins. The PnCON(port control register) determines which function is used for each pin.

	If PE0 - PE7 is used for the wakeup signal in power down mode, these ports must be configured in interrupt mode.

#### PORT DATA REGISTER (GPADAT-GPJDAT)
  If Ports are configured as output ports, data can be written to the corresponding bit of PnDAT. If ports are configured as input ports, the data can be read from the corresponding bit of PnDAT.

#### PORT PULL-UP REGISTER (GPBUP-GPJUP)
  The port pull-up register controls the pull-up resister enable/disable of each port group. When the corresponding bit is 0, the pull-up resister of the pin is enabled. When 1, the pull-up resister is disabled.

  If the port pull-up register is enabled then the pull-up resisters work without pin's functional setting(input, output, DATAn, EINTn and etc)

#### MISCELLANEOUS CONTROL REGISTER
  This register controls DATA port pull-up resister in Sleep mode, USB pad, and CLKOUT selection.

#### EXTERNAL INTERRUPT CONTROL REGISTER
  The 24 external interrupts are requested by various signaling methods. The EXTINT register configures the signaling methods among the low level trigger, high level trigger, falling edge trigger, rising edge trigger, and both edge trigger for the external interrupt request.

  Because each external interrupt pin has a digital filter, the interrupt controller can recognize the request signal that is longer than 3 clocks.

### PORT F CONTROL REGISTER (GPFCON, GPFDAT)
![GPF控制寄存器](https://github.com/kongdehua/uboot/raw/master/stage1/ex_2/image/gpio_register.png "GPF控制寄存器")
![GPF控制寄存器续](https://github.com/kongdehua/uboot/raw/master/stage1/ex_2/image/gpio_register_2.png "GPF控制寄存器续")

### 分析
从上图中可以看出，配置GPF4，GPF5，GPF6为输出端口，
例如：
将GPFCON的[9:8]设置为01，就表示GPF4端口的功能为输出；
将GPFDAT中[4]设置为1，LED0就熄灭，设置为0，LED0就点亮。

### 代码实现
代码中如何设置某两位为00，01，10，11， 

```c 
#define GPFCON (*(volatile unsigned long*)0x56000050)
#define GPFDAT (*(volatile unsigned long*)0x56000054)

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
		l |=   (VAL << StartBit) ;\
		ADDR = l; \
	}\
	while(0)

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
```
