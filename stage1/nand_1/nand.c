#include "s3c2440.h"
#include "timer.h"

#define GSTATUS1    (*(volatile unsigned int *)0x560000B0)
#define BUSY        1

#define NAND_SECTOR_SIZE_LP 2048
#define NAND_BLOCK_MASK_LP  (NAND_SECTOR_SIZE_LP - 1)

void nand_wait_idle()
{
	S3C2440_NAND * const nand = S3C2440_GetBase_NAND();
	
	volatile unsigned char *p 
		= (volatile unsigned char *)&nand->NFSTAT;

	while (!(*p & BUSY)) ; delay(100);
}

void nand_select_chip(void)
{
	S3C2440_NAND * const nand = S3C2440_GetBase_NAND();
	SET_ONE_BIT(&nand->NFCONT, 1, 0x0);  delay(100);
}

void nand_deselect_chip(void)
{
	S3C2440_NAND * const nand = S3C2440_GetBase_NAND();
	SET_ONE_BIT(&nand->NFCONT, 1, 0x1);  delay(100);
}

void nand_write_cmd(int cmd)
{
	S3C2440_NAND * const nand = S3C2440_GetBase_NAND();
	
	volatile unsigned char *p 
		= (volatile unsigned char *)&nand->NFCMMD;

	*p = cmd;
}

void nand_write_addr(unsigned int addr)
{
	S3C2440_NAND * const nand = S3C2440_GetBase_NAND();
	
	volatile unsigned char *p 
		= (volatile unsigned char *)&nand->NFADDR;
	
	*p = addr & 0xFF;           delay(100);
	*p = ( addr >>  9 ) & 0xFF; delay(100);
	*p = ( addr >> 17 ) & 0xFF; delay(100);
	*p = ( addr >> 25 ) & 0xFF; delay(100);
}

void nand_write_addr_lp(unsigned int addr)
{
	S3C2440_NAND * const nand = S3C2440_GetBase_NAND();
	
	volatile unsigned char *p 
		= (volatile unsigned char *)&nand->NFADDR;

	int col  = addr & NAND_BLOCK_MASK_LP;
	int page = addr / NAND_SECTOR_SIZE_LP;

	*p = (col  >> 0) & 0xFF; delay(100);  // Col Add A0~A7
	*p = (col  >> 8) & 0x0F; delay(100);  // Col Add A8~A11
	*p = (page >> 0) & 0xFF; delay(100);  // Row Add A12~A19
	*p = (page >> 8) & 0xFF; delay(100);  // Row Add A20~A27
	*p = (page >>16) & 0x03; delay(100);  // Row Add A28~A29
}

unsigned char nand_read_data(void)
{
	S3C2440_NAND * const nand = S3C2440_GetBase_NAND();
	
	volatile unsigned char *p 
		= (volatile unsigned char *)&nand->NFDATA;

	return *p;
}

void nand_reset()
{
	nand_select_chip();
	nand_write_cmd(0xFF);
	nand_wait_idle();
	nand_deselect_chip();
}


void nand_init()
{
#define TACLS   0
#define TWRPH0  3
#define TWRPH1  0

	S3C2440_NAND * const nand = S3C2440_GetBase_NAND();
	
	/* 设置时序 */
	nand->NFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);
	/* 使能NAND Flash控制器, 初始化ECC, 禁止片选 */
	nand->NFCONT = (1<<4)|(1<<1)|(1<<0);

	/* 复位NAND Flash */
	nand_reset();
}

/* 读函数 */
void nand_read(unsigned char *buf, unsigned long start_addr, int size)
{
	int i, j;

	if ((start_addr & NAND_BLOCK_MASK_LP) || (size & NAND_BLOCK_MASK_LP)) {
		return ;    /* 地址或长度不对齐 */
	}

	/* 选中芯片 */
	nand_select_chip();

	for(i=start_addr; i < (start_addr + size);) {
		/* 发出READ0命令 */
		nand_write_cmd(0);

		/* Write Address */
		nand_write_addr_lp(i);
		nand_write_cmd(0x30);		
		nand_wait_idle();

		for(j=0; j < NAND_SECTOR_SIZE_LP; j++, i++) {
				*buf = nand_read_data();
				buf++;
			}
		}

		/* 取消片选信号 */
		nand_deselect_chip();

		return ;
	}

