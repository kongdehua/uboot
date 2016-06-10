.text
.global _start
_start:
	ldr     sp, =1024*4
	bl main    @ 跳转到main

main_loop:
	bl main_loop
