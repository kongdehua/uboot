objs= head.o \
			nand.o \
			main.o \
			timer.o \
			serial.o \
			string.o \
			led.o \
			vsprintf.o

gccincdir := $(shell $(CC) -print-file-name=include)

main.bin : $(objs)
	arm-linux-ld  -Tmain.lds -Ttext=0x30000000 -o main_elf $(objs) -lgcc \
			-L $(shell dirname `arm-linux-gcc -print-libgcc-file-name`) 
	arm-linux-objcopy -O binary -S main_elf main.bin
	arm-linux-objdump -D -m arm main_elf > main.dis

%.o:%.c
	arm-linux-gcc -c -o $@ $< -fno-builtin -ffreestanding -nostdinc -isystem $(gccincdir) 

%.o:%.S
	arm-linux-gcc -c -o $@ $<

#-fno-builtin -ffreestanding -nostdinc -isystem $(gccincdir) 

.PHONY: clean
clean:
	-rm $(objs) main_elf main.bin main.dis

distclean: clean
	rm cscope* tags

