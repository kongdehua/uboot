#ifndef _ASM_GLB_DATA_H_
#define _ASM_GLB_DATA_H_

typedef struct global_data {
	unsigned long baudrate;
} gd_t;

#define DECLARE_GLOBAL_DATA_PTR register volatile gd_t *gd asm("r8")

#endif
