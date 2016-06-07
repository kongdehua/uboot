# 第一个例子 `ex_1`
此工程仅包含最简单的程序逻辑，代码中仅包含程序段，仅仅关闭看门狗
注：使用ubuntu的vim+arm系列工具，完成一个可执行程序，完全用汇编编写。

所需资料：《嵌入式Linux应用开发完全手册》95页

实现方式：需要在开发平台上实现两个文件

1. head.S
![head.S文件截图](https://github.com/kongdehua/uboot/raw/master/stage1/image/firstExample_head.png "head.S文件截图")

2. Makefile
![Makefile文件截图](https://github.com/kongdehua/uboot/raw/master/stage1/image/firstExample_Makefile.png "Makefile文件截图")

## 第二个例子
实现点灯程序：程序中需要控制ARM芯片的多个引脚，每个引脚可以控制一个LED灯。
![LED原理图](https://github.com/kongdehua/uboot/raw/master/stage1/ex_2/image/led.png "LED原理图")

