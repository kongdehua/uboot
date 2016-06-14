objs= head.o \
			main.o \
			timer.o \
			serial.o \
#			vsprintf.o

main.bin : $(objs)
	arm-linux-ld  -Ttext=0x30000000 -o main_elf $(objs) -lgcc -L /home/kdh/x-tools/lib/gcc/arm-kdh-linux-gnueabi/5.2.0
	arm-linux-objcopy -O binary -S main_elf main.bin
	arm-linux-objdump -D -m arm main_elf > main.dis

%.o:%.c
	arm-linux-gcc -c -o $@ $<

%.o:%.S
	arm-linux-gcc -c -o $@ $<

.PHONY: clean
clean:
	-rm $(objs) main_elf main.bin main.dis

distclean: clean
	rm cscope* tags

