## 串口第一个例子 `com_1`
识别时钟，获取相应的时钟频率

## 系统时钟和定时器
### 1 时钟体系和各类时钟不见
#### 1.1 S3C2440时钟体系
芯片的时钟控制逻辑既可以外接晶振，然后通过内部电路产生时钟源；也可以直接使用外部提供的时钟源，他们通过引脚的设置来选择。时钟控制逻辑给整个芯片提供3种时钟：FCLK用于CPU核；HCLK用于AHB总线上设备，比如CPU核/存储器控制器、中断控制器、LCD控制器、DMA和USB主机模块等；PCLK用于APB总线上的设备，比如WATCHDOG、IIS、I2C、PWM定时器、MMC接口、ADC、UART、GPIO、RTC和SPI。

AHB总线（Advanced High perfermance Bus）总线主要用于高性能模块（如CPU、DMA、DSP等）之间的连接；APB（Advanced Peripheral Bus）总线主要用于低带宽的周边外设之间的连接，例如UART、I2C等。

S3C2410 CPU核的工作电压为1.8V时，主频可以达到200MHz；工作电压为2.0V时，主频可以达到266MHz。S3C2440 CPU核的工作电压为1.2V时，主频可以达到300MHz；工作电压为1.3V时，主频可达400MHz/为了降低电磁干扰/降低板间布线的要求，S3C2440外接的晶振频率通常很低，本开发板为12MHz，需要通过时钟控制逻辑的PLL提高系统时钟。

S3C2440有两个PLL：MPLL和UPLL。UPLL专用于USB设备，MPLL用于设置FCLK、HCLK、PLCK。它们的设置方法类似，本书以MPLL为例。

上电时，PLL没有启动，FCLK等于外部输入的时钟，成为Fin。若要提高系统时钟，需要软件来启用PLL，下面介绍PLL的设置过程。请跟随FCLK的图像了解启动过程。
1. 上电几毫秒后，晶振输出稳定，FCLK=Fin（晶振频率），nRESET信号恢复高电平后，CPU开始执行指令。
S3C2440 CPU核的工作电压为1.2V时，主频可以达到300MHz；工作电压为1.3V时，主频可达400MHz/为了降低电磁干扰/降低板间布线的要求，S3C2440外接的晶振频率通常很低，本开发板为12MHz，需要通过时钟控制逻辑的PLL提高系统时钟。

S3C2440有两个PLL：MPLL和UPLL。UPLL专用于USB设备，MPLL用于设置FCLK、HCLK、PLCK。它们的设置方法类似，本书以MPLL为例。

上电时，PLL没有启动，FCLK等于外部输入的时钟，成为Fin。若要提高系统时钟，需要软件来启用PLL，下面介绍PLL的设置过程。请跟随FCLK的图像了解启动过程。
1. 上电几毫秒后，晶振输出稳定，FCLK=Fin（晶振频率），nRESET信号恢复高电平后，CPU开始执行指令。
S3C2440 CPU核的工作电压为1.2V时，主频可以达到300MHz；工作电压为1.3V时，主频可达400MHz/为了降低电磁干扰/降低板间布线的要求，S3C2440外接的晶振频率通常很低，本开发板为12MHz，需要通过时钟控制逻辑的PLL提高系统时钟。

S3C2440有两个PLL：MPLL和UPLL。UPLL专用于USB设备，MPLL用于设置FCLK、HCLK、PLCK。它们的设置方法类似，本书以MPLL为例。

上电时，PLL没有启动，FCLK等于外部输入的时钟，成为Fin。若要提高系统时钟，需要软件来启用PLL，下面介绍PLL的设置过程。请跟随FCLK的图像了解启动过程。
1. 上电几毫秒后，晶振输出稳定，FCLK=Fin（晶振频率），nRESET信号恢复高电平后，CPU开始执行指令。
2. 可以在程序开头启动MPLL，设置MPLL的几个寄存器后，需要等待一段时间（LOCK Time），MPLL的输出才稳定，在这段时间（成为Lock Time）内，FCLK停振，CPU停止工作。Lock Time的长短由寄存器LOCKTIME设定。
3. Lock Time之后，MPLL输出正常，CPU工作在新的FCLK下。

FCLK、HCLK、PCLK的比例是可以改变的，设置它们三者的比例，启动MPLL只需要设置3个寄存器（对于S3C2440的一些时钟比例，还需要额外设置一个寄存器）。
1. LOCKTIME寄存器（LOCK TIME COUNT）：用于设置”LockTIME“的长度。

前面说过，MPLL启动后需要等待一段时间（Lock Time），使得其输出稳定。S3C2440中，位[31:16]用于UPLL，位[15:0]用于MPLL。一般而言，使用它的默认值即可，S3C2440中的默认值为0xFFFFFFFF。

2. MPLLCON寄存器（Main PLL Control）：用于设置FCLK与Fin的倍数。
位[19:12]的值成为MDIV，位[9:4]的值成为PDIV，位[1:0]的值成为SDIV。FCLK与Fin的关系计算公式如下：
MPLL(FCLK)=(2*m*Fin)/(p*2^s)
其中：m = MDIV+8, p = PDIV+2, s = SDIV。

当设置MPLLCON之后，相当于：首先使用软件设置PLL，Lock Time就被自动插入。Lock Time之后，MPLL输出纹等，CPU工作在新的FCLK下。

3. CLKDIVN寄存器（CLOCK DIVIDER CONTROL）：用于设置FCLK、HCLK、PCLK三者之间的比例。

| HDIVN | PDIVN | HCLF3_HALF/ | FCLK | HCLK  | PCLK   | RESULT |
|       |       | HCLF4_HALF  |      |       |        |        |
|-------|-------|-------------|------|-------|--------|--------|
| 0     | 0     | -           | FCLK | HCLK  | PCLK   | 1:1:1  |
| 0     | 1     | -           | FCLK | HCLK  | PCLK/2 | 1:1:2  |
|       |       |             |      |       |        |        |

对于S3C2440
