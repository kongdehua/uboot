#ifndef _NAND_H_
#define _NAND_H_

void nand_init();
void nand_read(unsigned char *buf, unsigned long start_addr, int size);

#endif
