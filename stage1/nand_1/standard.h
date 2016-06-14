#ifndef _STANDARD_H_
#define _STANDARD_H_


typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long ulong;

#define GET_ONE_BIT(ADDR, StartBit)\
	(ADDR & 1 << StartBit) >> StartBit

#define GET_TWO_BIT(ADDR, StartBit)\
	(ADDR & 3 << StartBit) >> StartBit

#define SET_TWO_BIT(ADDR, StartBit, VAL) \
	do\
	{\
		unsigned long l = *(ADDR);\
    l &= (~(3 << StartBit));\
		l |= (VAL << StartBit);\
		*(ADDR) = l;\
	}\
	while(0)

#define SET_ONE_BIT(ADDR, StartBit, VAL) \
	do\
	{\
		unsigned long l = *(ADDR); \
		l &= (~(1 << StartBit));\
		l |= (VAL << StartBit) ;\
		*(ADDR) = l; \
	}\
	while(0)

#define CFG_PROMPT    "OpenJTAG> "  /* Monitor Command Prompt */
#define CFG_CBSIZE    256   /* Console I/O Buffer Size  */
#define CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16) /* Print Buffer Size */

#endif
