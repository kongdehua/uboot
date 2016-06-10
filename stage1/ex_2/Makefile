main : head.S
	arm-linux-gcc -g -c -o head.o head.S
	arm-linux-gcc -g -c -o main.o main.c
	arm-linux-ld -Ttext=0x00000000 -o main_elf head.o main.o
	arm-linux-objcopy -O binary -S main_elf main.bin

clean :
	rm head.o main.o main_elf main.bin
