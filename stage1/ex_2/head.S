.text
.global _start
_start:

	bl main    @ 跳转到main

main_loop:
	bl main_loop
