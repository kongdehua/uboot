# 第二个例子 `com_2`
实现驱动串口

## UART原理及UART部件的使用方法

通用异步收发器简称UART，即“Universal Asynchronous Receiver Transmitter”，它用来传输串行数据：发送数据时，CPU将并行数据写入UART，UART按照一定的格式在一根电线上穿行发出；接收数据时，UART检测另一根电线上的信号，将数据串行收集放在缓冲区中，CPU即可读取UART获得这些数据。UART之间以全双工方式传输数据，最精简的连线方法只有三根电线：TxD用于发送数据，RxD用于接收数据，Gnd用于给双方提供参考点评，连线如图。

UART使用标准的TTL/CMOS逻辑电平（0～5V、0～3.3V、0～2.5V或0～1.8V）来表示数据，高电平表示1，低电平表示0。为了增强数据的抗干扰能力、提高传输长度，通常将TTL/CMOS逻辑电平转换为RS-232逻辑电平，3～12V表示0，-3～-12V表示1。

## S3C2440 UART的使用

在使用UART之前需要设置波特率、传输格式（有多少个数据位、是否使用校验位、是奇校验还是偶校验、有多少个停止位、是否使用流量控制）；对于S3C2440，还要选择所涉及管脚为UART功能、选择UART通道的工作模式为中断模式或DMA模式。设置好之后，往某个寄存器写入数据即可发送，读取某个寄存器即可得到接收到的数据。可以通过查询状态寄存器或设置中断来获知数据是否已经发送完毕、是否已经接收到数据。

### 1. 将涉及的UART通道管脚设为UART功能
比如，在UART通道0中，GPH2、GPH3分别用做TXD0、RXD0，要使用UART通道0时，先设置GPHCON寄存器将GPH2、GPH3引脚的功能设为TXD0、RXD0。

### 2. UBRDIVn寄存器（UART BAUD RATE DIVISOR）：设置波特率
S3C2440 UART的时钟源有三种选择：PCLK、UEXTCLK、FCLK/n，其中n值通过UCON0～UCON2联合设置。

根据给定的波特率，所选择的时钟源的频率，可以通过以下公式计算UBRDIVn寄存器（n为0～2，对应3个UART通道）的值，如下所示：

UBRDIVn = (int)(UART clock / ( buad rate * 16 ) ) - 1

上式计算出来的UBRDIVn寄存器值不一定是整数，只要其误差在1.87%之内即可。误差计算公式如下：

tUPCLK = (UBRDIVn + 1) * 16 * lFrame / (UART clock) // 实际的UART时钟

tUEXACT = lFrame / baud-rate  // 理论的uART时钟

UART error = (tUPCLK - tUEXACT) / tUEXACT * 100%    // 误差

### 3. ULCONn寄存器（UART LINE CONTROL）：设置传输格式
ULCONn寄存器（n为0～2）格式如下表所示。

| 功能      |   位   | 说明 |

### 4. UCONn寄存器（UART CONTROL）
UCONn寄存器用于选择UART时钟源、设置UART中断方式等。S3C2440有三个时钟源PCLK、UEXTCLK、FCLK/n。

### 5. UFCONn寄存器（UART FIFO CONTROL）、UFSTATn寄存器（UART FIFO STATUS）
UFCONn寄存器用于设置是否使用FIFO，设置各FIFO的处罚阈值，即发送FIFO中有多少个数据时产生中断、接收FIFO中有多少个数据时产生中断。并可以通过设置UFCONn寄存器来复位各个FIFO。

实例中并不使用FIFO。

### 6. UMCONn寄存器（UART MODEM CONTROL）、UMSTATn寄存器（UART MODEM STATUS）
这两类寄存器用于流量控制。

### 7. UTRSTATn寄存器（UART TX/RX STATUS）
UTRSTATn寄存器用来表明数据是否已经发送完毕、是否已经接收到数据。缓冲区其实就是图中的FIFO，只不过不使用FIFO功能时，可以认为其深度为1.

|     功     能      | 位  | 说明                                |
|--------------------|-----|-------------------------------------|
| 接收缓冲区数据就绪 | [0] | 当接收到数据时，此位被自动设为1     |
| 发送缓冲区空       | [1] | 发送缓冲区没有数据时，此位自动设为1 |
| 发送器空           | [2] | 发送缓冲区没有数据时，并且最后一个数据也已经发送出去，此位自动设为1 |

### 8. UERSTATn寄存器（UART ERROR STATUS）
用来表示各种错误是否发生，位[0~3]为1时，分别表示溢出错误、校验错误、帧错误、检测到“break”信号。读取这个寄存器后，它会自动清零。

### 9. UTXHn寄存器（UART TRANSMIT BUFFER REGISTER）
CPU将数据写入这个寄存器，UART即会将它保存到缓冲区中，并自动发送出去。

### 10. URXHn寄存器（UART RECEIVE BUFFER REGISTER）
当UART接收到数据时，CPU读取这个寄存器，即可获得数据。

## UART 操作实例
串口上输入一个字符
串口上输出一个字符
