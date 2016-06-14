main.bin : head.S main.c timer.c vsprintf.c
	arm-linux-gcc -c -o head.o head.S
	arm-linux-gcc -c -o vsprintf.o vsprintf.c
	arm-linux-gcc -c -o timer.o timer.c
	arm-linux-gcc -c -o main.o main.c
	arm-linux-ld  -Ttext=0x30000000 -o main_elf head.o main.o timer.o -lgcc -L /home/kdh/x-tools/lib/gcc/arm-kdh-linux-gnueabi/5.2.0
	arm-linux-objcopy -O binary -S main_elf main.bin
	arm-linux-objdump -D -m arm main_elf > main.dis

.PHONY: clean
clean:
	-rm head.o main.o timer.o main_elf main.bin vsprintf.o main.dis

distclean: clean
	rm cscope* tags

