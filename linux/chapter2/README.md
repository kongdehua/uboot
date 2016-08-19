## 2. gcc内嵌汇编
linux内核源码中，许多c代码嵌入了汇编语句，这是通过关键字asm实现的。

```
  static inline unsigned long native_read_cr2(void)
  {
    unsigned long val;
    asm volatile("movl %%cr2, %0\n\t" : "=r" (val));
    return val;
  }
```

其中asm表示汇编指令的开始，由于gcc在编译优化阶段，可能会调整指令的顺序，关键字volatile阻止gcc对这里的内嵌汇编指令进行优化。另外在内核代码中常常还看到`__asm__`,`__volatile__`,他们的作用和asm，volatile是一样的，为什么要两个作用相同的关键字呢？这是由于在C标准制定之初，并没有asm和volatile这两个关键字，后来由于实际需要。考虑到已有C代码中可能存在变量名位asm或volatile，加入了另外两个关键字。

内嵌代码的通用形式：

```
  asm volatile (assemble template : output : input : clobber);
```

其中assembler template为汇编指令部分，例如“movl %%cr2， %0”. output是输出部分，input表示输入部分, clobber表示被修改的部分。汇编指令中的数组和前缀表示样板操作数，例如%0，%1等，用来依次指代后面的输出部分，输入部分等样板操作数。由于这些样板操作数使用了%，因此寄存器前面要加两个%，例如用来依次指代后面的输出部分，输入部分等样本操作数。寄存器前面需要加两个%
