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

