.equ        MEM_CTL_BASE,       0x48000000
.equ        SDRAM_BASE,         0x30000000

.text
.global _start
_start:
@******************************************************************************       
@ 中断向量，本程序中，除Reset和HandleIRQ外，其它异常都没有使用
@******************************************************************************       
b   Reset

@ 0x04: 未定义指令中止模式的向量地址
HandleUndef:
	b   HandleUndef 

@ 0x08: 管理模式的向量地址，通过SWI指令进入此模式
HandleSWI:
	b   HandleSWI

@ 0x0c: 指令预取终止导致的异常的向量地址
HandlePrefetchAbort:
	b   HandlePrefetchAbort

@ 0x10: 数据访问终止导致的异常的向量地址
HandleDataAbort:
	b   HandleDataAbort

@ 0x14: 保留
HandleNotUsed:
	b   HandleNotUsed

@ 0x18: 中断模式的向量地址
b   HandleIRQ

@ 0x1c: 快中断模式的向量地址
HandleFIQ:
	b   HandleFIQ

Reset:           
	mrs r0,cpsr
	bic r0,r0,#0x1f              
	orr r0,r0,#0xd3
	msr cpsr,r0

	ldr sp, =#4096
	bl  disable_watch_dog               @ 关闭WATCHDOG，否则CPU会不断重启

#define INTMOD     0X4A000004                                                                         
#define INTMSK   0x4A000008  /* Interupt-Controller base addresses */                                 
#define INTSUBMSK  0x4A00001C

	mov r1, #0xffffffff
	ldr r0, =INTMSK
	str r1, [r0]
	ldr r1, =0x3ff
	ldr r0, =INTSUBMSK
	str r1, [r0]

	bl  memsetup        @ 设置存储控制器
	bl  clock_init      @ 初始化时钟

	bl nand_init
	ldr     r0,     =0x30000000     @1. 目标地址=0x30000000，这是SDRAM的起始地址
	mov     r1,     #0              @2.  源地址   = 4096，连接的时候，main.c中的代码都存在NAND Flash地址4096开始处
	mov     r2,     #0x18000          @3.  复制长度= 2048(bytes)，对于本实验的main.c，这是足够了
	bl      nand_read               @调用C函数nand_read

	msr cpsr_c, #0xd2       @ 进入中断模式
	ldr sp, =0x34000000     @ 设置中断模式栈指针

	msr cpsr_c, #0xd3       @ 进入管理模式
	ldr sp, =0x32000000     @ 设置堆栈


	ldr pc, =on_sdram                   @ 跳到SDRAM中继续执行
on_sdram:
	bl  main

halt_loop:
	b   halt_loop

disable_watch_dog:
	@ 往WATCHDOG寄存器写0即可
	mov r1,     #0x53000000
	mov r2,     #0x0
	str r2,     [r1]
	mov pc,     lr      @ 返回

copy_steppingstone_to_sdram:
	@ 将Steppingstone的4K数据全部复制到SDRAM中去
	@ Steppingstone起始地址为0x00000000，SDRAM中起始地址为0x30000000

	mov r1, #0
	ldr r2, =SDRAM_BASE
	mov r3, #4*1024
1:  
	ldr r4, [r1],#4     @ 从Steppingstone读取4字节的数据，并让源地址加4
	str r4, [r2],#4     @ 将此4字节的数据复制到SDRAM中，并让目地地址加4
	cmp r1, r3          @ 判断是否完成：源地址等于Steppingstone的未地址？
	bne 1b              @ 若没有复制完，继续
	mov pc,     lr      @ 返回

memsetup:
	@ 设置存储控制器以便使用SDRAM等外设
	mov r1,     #MEM_CTL_BASE       @ 存储控制器的13个寄存器的开始地址
	adrl    r2, mem_cfg_val         @ 这13个值的起始存储地址
	add r3,     r1, #52             @ 13*4 = 54
1:  
	ldr r4,     [r2], #4            @ 读取设置值，并让r2加4
	str r4,     [r1], #4            @ 将此值写入寄存器，并让r1加4
	cmp r1,     r3                  @ 判断是否设置完所有13个寄存器
	bne 1b                          @ 若没有写成，继续
	mov pc,     lr                  @ 返回

HandleIRQ:
	sub lr, lr, #4                  @ 计算返回地址
	stmdb   sp!,    { r0-r12,lr }   @ 保存使用到的寄存器
	@ 注意，此时的sp是中断模式的sp
	@ 初始值是上面设置的3072

	ldr lr, =int_return             @ 设置调用ISR即EINT_Handle函数后的返回地址  
	ldr pc, =EINT_Handle            @ 调用中断服务函数，在interrupt.c中
int_return:
	ldmia   sp!,    { r0-r12,pc }^  @ 中断返回, ^表示将spsr的值复制到cpsr

.align 4
mem_cfg_val:
@ 存储控制器13个寄存器的设置值
.long   0x22011110      @ BWSCON
.long   0x00000700      @ BANKCON0
.long   0x00000700      @ BANKCON1
.long   0x00000700      @ BANKCON2
.long   0x00000700      @ BANKCON3  
.long   0x00000700      @ BANKCON4
.long   0x00000700      @ BANKCON5
.long   0x00018005      @ BANKCON6
.long   0x00018005      @ BANKCON7
.long   0x008C07A3      @ REFRESH
.long   0x000000B1      @ BANKSIZE
.long   0x00000030      @ MRSRB6
.long   0x00000030      @ MRSRB7

