main : head.S
	arm-linux-gcc -c -o head.o head.S
	arm-linux-ld -Ttext=0x00000000 -o main_elf head.o
	arm-linux-objcopy -O binary -S  main_elf main.bin

clean :
	-rm main.bin main_elf head.o
