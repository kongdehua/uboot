main.bin : head.S
	arm-linux-gcc -c -o head.o head.S
	arm-linux-gcc -c -o main.o main.c
	arm-linux-ld  -Ttext=0x00000000 -o main_elf head.o main.o -lgcc -L /home/kdh/x-tools/lib/gcc/arm-kdh-linux-gnueabi/5.2.0
	arm-linux-objcopy -O binary -S main_elf main.bin

clean:
	rm head.o main.o main_elf main.bin
