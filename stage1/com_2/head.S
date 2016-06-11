.text
.global _start

_start:
	ldr sp, =1024*4
	bl  main

main_loop:
	bl main_loop
