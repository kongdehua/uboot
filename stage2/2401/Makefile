objs= head.o \
			init.o \
			nand.o \
			timer.o \
			main.o \
			serial.o \
			string.o \
			led.o \
			command.o \
			spi.o \
			interrupt.o \
			xyzModem.o \
			crc16.o \
			sdio.o \
			NRF24L01.o \
			fatfs/diskio.o \
			fatfs/ff.o \
			fatfs/option/syscall.o \
			fatfs/option/unicode.o \
			cmd_download.o \
			cmd_sdio.o \
			cmd_file.o \
			cmd_nRF24L01.o \
			vsprintf.o

gccincdir := $(shell $(CC) -print-file-name=include)

main.bin : $(objs)
	arm-linux-ld  -Tmain.lds -Ttext=0x30000000 -o main_elf $(objs) -lgcc \
			-L $(shell dirname `arm-linux-gcc -print-libgcc-file-name`) 
	arm-linux-objcopy -O binary -S main_elf main.bin
	arm-linux-objdump -D -m arm main_elf > main.dis

%.o:%.c
	arm-linux-gcc -g -c -o $@ $< -I. -fno-builtin -ffreestanding -nostdinc -isystem $(gccincdir) 

%.o:%.S
	arm-linux-gcc -g -c -o $@ $< -I.

#-fno-builtin -ffreestanding -nostdinc -isystem $(gccincdir) 

.PHONY: clean
clean:
	-rm $(objs) main_elf main.bin main.dis

distclean: clean
	rm cscope* tags

