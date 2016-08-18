# Linux内核Makefile分析
Makefile维护这程序编译链接的相关规则，分析Makefile可以从总体上把握程序的结构。在Linux内核学习过程中，我们将看到熟悉Linux内核Makefile将为源码分析带来极大的方便。在分析过程中涉及许多Makefile的知识。
## 3.1 Linux内核编译概述
Linux2.6内核引入kbuild机制，通过make menuconfig配置把配置信息保存在.config文件中，当.config文件被改变之后，在重新编译内核，Kbuild能够保证只进行最小化的编译。Linux内核的编译系统主要包括以下文件：
* 交互配置工具，这些文件是编译生成的可执行文件，它负责提供内核编译过程中的交互，并把用户配置交互的结果保存在.config文件。比如用户输入make menuconfig命令时，主Makefile会编译出mconf的文件，mconf会根据内核根目录下的.config初始化文件配置界面，在配置结束的时候，将配置结果保存到.config文件中。
* Kconfig文件，位于各个子目录下。这个文件必须符合kconfig language规范，它定义了交互配置时的菜单等信息。例如当输入make menuconfig命令的时候，会运行mconf arch/x86/Kconfig，这个Kconfig定义了出现在menuconfig中的所有菜单项。
* .config文件，这是内核配置文件，由配置工具生成.config文件，该文件由CONFIG_XXX=Z组成，其中Z可以是y,m,空，数字或者字符串。其中Z为y表示把对应的模块编译进内核；m表示把对应的模块编译成可加载模块；空代表不编译进内核；其他的可以传递到内核代码做参数。
* `scripts/Makefile.*`,这些Makefile定义了各种编译选项，例如gcc、ld的参数等。具体的各个Makefile的主要作用见下面的分析；
* 顶层Makefile，这个文件接受make xxx命令，然后根据xxx做相应的操作，最主要的操作是编译出内核文件vmlinux和相关模块文件。
* kbuild Makefiles，分布在各个子目录下的Makefile，它并不符合GNU Makefile的语法，为了区别于通常意义下的Makefile，所以被称为kbuild Makefile。它的内容形如obj-$(CONFIG_XXX) += Z/或者obj-$(CONFIG_XXX) += Z.o。这里obj-$(CONFIG_XXX)被替换为obj-y，obj-m，obj-。Kbuild会根据obj-y+=z.o把对应目录下z.c文件编译成z.o，对于obj-y+=Z/，Kbuild会读取Z/目录下的Makefile，这依然是一个kbuild Makefile，通过这个Kbuild Makefile，Kbuild就可以对子目录Z进行递归处理了。对于obj-m的情况是类似的。
* `*.cmd`文件，把一个.c文件编译成.o文件后，如果再次编译的时候，Make会根据.o和.c文件的修改时间来判断是否需要再次编译该.c文件。当修改了.config文件或者编译的命令行参数或者依赖关系后，再次编译内核，make根据修改时间判断是否需要重新编译是不够的，因为编译参数可能改变了，为了解决这个问题，当编译链接一个文件的时候，kbuild把编译时的依赖关系和编译链接的命令保存在对应的.cmd文件中。下次编译的时候，除了比对文件的修改时间外，还要根据.cmd文件保存的历史编译参数和依赖关系，来判断是否需要进行重新编译。

## 3.2 内核编译过程分析
通常编译x86的平台命令时make bzImage，打开主Makefile却找不到bzImage
