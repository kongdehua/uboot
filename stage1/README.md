# 顺序
## 第一个例子 `ex_1`
此工程仅包含最简单的程序逻辑，代码中仅包含程序段，仅仅关闭看门狗
注：使用ubuntu的vim+arm系列工具，完成一个可执行程序，完全用汇编编写。

所需资料：《嵌入式Linux应用开发完全手册》95页

实现方式：需要在开发平台上实现两个文件

1. head.S

![head.S文件截图](https://github.com/kongdehua/uboot/raw/master/stage1/image/firstExample_head.png "head.S文件截图")

2. Makefile

![Makefile文件截图](https://github.com/kongdehua/uboot/raw/master/stage1/image/firstExample_Makefile.png "Makefile文件截图")

## 第二个例子 `ex_2`
实现点灯程序：程序中需要控制ARM芯片的多个引脚，每个引脚可以控制一个LED灯。

LED原理图
![LED原理图](https://github.com/kongdehua/uboot/raw/master/stage1/ex_2/image/led.png "LED原理图")

控制管脚原理图
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
![GPF控制寄存器](https://github.com/kongdehua/uboot/raw/master/stage1/image/ex_2/image/gpio_register.png "GPF控制寄存器")

### 分析
从上图中可以看出
