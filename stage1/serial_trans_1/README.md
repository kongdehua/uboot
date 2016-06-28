# 第一个例子 `nand_1`
实现nand flash驱动

## NAND Flash介绍和NAND Flash控制器使用
常用的Flash类型有NOR Flash和Nand Flash两种。

Nor Flash支持XIP，即代码可以直接在NOR Flash上执行，无需复制到内存中。这是由于NOR Flash的接口与RAM完全相同，可以随机访问任意地址的数据。在NOR Flash上进行读操作的效率非常高，但是擦除和写操作的效率很低；另外，NOR Flash的容量一般比较小。

Flash存储器件由擦出单元组成，当要写某个块时，需要确保这个块已经被擦除。NOR Flash的块大小范围为64kB～128kB；Nand Flash的块大小范围为8kB～64kB，擦写一个NOR Flash块需要4s，而擦鞋一个Nand Flash块仅需2ms。NOR Flash的块太大，不仅增加了擦写时间，对于给定的写操作，NOR Flash也需要更多的擦出操作————特别是小文件。

NOR Flash的接口与RAM完全相同，可以随意访问任意地址的数据。而NAND Flash的接口仅仅包含几个I/O引脚，需要穿行地访问。Nand Flash一般以512字节为单位进行读写。这是的NOR Flash适合于运行程序，而NAND Flash更适合于存储数据。

嵌入式linux对NOR、NAND Flash的软件支持都很成熟。在NOR Flash上常用jffs2文件系统，而在NAND Flash上常用yaffs文件系统。在更底层，有MTD驱动程序实现对它们的读写、擦除操作，它也实现了EDC/ECC校验。

命令寄存器 NFCMMD
地址寄存器 NFAddr
数据寄存器 NFDATA
状态寄存器 NFSTAT
