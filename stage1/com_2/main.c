typedef unsigned int u32;
typedef unsigned long ulong;

typedef volatile u32 S3C2440_REG32;

#define GET_ONE_BIT(ADDR, StartBit)\
	(ADDR & 1 << StartBit) >> StartBit

#define GET_TWO_BIT(ADDR, StartBit)\
	(ADDR & 3 << StartBit) >> StartBit

#define CLK_INPUT_FREQ 12000000

#define S3C2440_CLOCK_POWER_BASE 0x4c000000

// CLOCK POWER MANAGEMENT
typedef struct {
	S3C2440_REG32 LOCKTIME;
	S3C2440_REG32 MPLLCON;
	S3C2440_REG32 UPLLCON;
	S3C2440_REG32 CLKCON;
	S3C2440_REG32 CLKSLOW;
	S3C2440_REG32 CLKDIVN;
	S3C2440_REG32 CAMDIVN;
} CLOCK_POWER;

static CLOCK_POWER* GetBase_CLOCK_POWER()
{
	return (CLOCK_POWER*)S3C2440_CLOCK_POWER_BASE;
}

/*
MPLLCON寄存器（Main PLL Control）：用于设置FCLK与Fin的倍数。
位[19:12]的值成为MDIV，位[9:4]的值成为PDIV，位[1:0]的值成为SDIV。FCLK与Fin的关系计算公式如下：
MPLL(FCLK)=(2*m*Fin)/(p*2^s)
其中：m = MDIV+8, p = PDIV+2, s = SDIV。
*/
static ulong get_MPLLCLK()
{
	CLOCK_POWER * const clk_power = GetBase_CLOCK_POWER();
	ulong mpll, m, p, s;
	
	mpll = clk_power->MPLLCON;
	m = (( (0xFF << 12) & mpll) >> 12) + 8;
	p = (( (0x3F <<  4) & mpll) >>  4) + 2;
	s = (0x03 & mpll);
	
	//return ((2 * m * CLK_INPUT_FREQ) / (p * (1 << s)));
	return ((2 * m ) / p);
	//return 0;
}

static ulong get_FCLK()
{
	return get_MPLLCLK();
}

/*
HDIVN是CLKDIVN寄存器中的[2:1], PDIVN是位[0]；
HCLK4_HALF、HCLK3_HALF分别为CAMDIVN寄存器中的位[9],[8]。

| HDIVN | PDIVN | HCLF3/4_HALF | FCLK | HCLK   | PCLK   | RESULT |
|-------|-------|--------------|------|--------|--------|--------|
| 0     | 0     | -            | FCLK | HCLK   | PCLK   | 1:1:1  |
| 0     | 1     | -            | FCLK | HCLK   | PCLK/2 | 1:1:2  |
| 1     | 0     | -            | FCLK | HCLK/2 | PCLK/2 | 1:2:2  |
| 1     | 1     | -            | FCLK | HCLK/2 | PCLK/4 | 1:2:4  |
| 3     | 0     | 0/0          | FCLK | HCLK/3 | PCLK/3 | 1:3:3  |
| 3     | 1     | 0/0          | FCLK | HCLK/3 | PCLK/6 | 1:3:6  |
| 3     | 0     | 1/0          | FCLK | HCLK/6 | PCLK/6 | 1:6:6  |
| 3     | 1     | 1/0          | FCLK | HCLK/6 | PCLK/12| 1:6:12 |
| 2     | 0     | 0/0          | FCLK | HCLK/4 | PCLK/4 | 1:4:4  |
| 2     | 1     | 0/0          | FCLK | HCLK/4 | PCLK/8 | 1:4:8  |
| 2     | 0     | 0/1          | FCLK | HCLK/8 | PCLK/8 | 1:8:8  |
| 2     | 1     | 0/1          | FCLK | HCLK/8 | PCLK/16| 1:8:16 |
*/
static ulong get_HCLK()
{
	CLOCK_POWER * const clk_power = GetBase_CLOCK_POWER();
	ulong hdivn = GET_TWO_BIT(clk_power->CLKDIVN, 1);
	ulong pdivn = GET_ONE_BIT(clk_power->CLKDIVN, 0);
	ulong hclf3 = GET_ONE_BIT(clk_power->CAMDIVN, 8);
	ulong hclf4 = GET_ONE_BIT(clk_power->CAMDIVN, 9);

	ulong scale = 0;
	if ( hdivn < 2 )
	{
		switch (hdivn << 3 | pdivn << 2)
		{
			case 0 << 3 | 0 << 2: scale = 1; break;
			case 0 << 3 | 1 << 2: scale = 1; break;
			case 1 << 3 | 0 << 2: scale = 2; break;
			case 1 << 3 | 1 << 2: scale = 2; break;
		}
	}
	else
	{
		switch (hdivn << 3 | pdivn << 2 | hclf3 << 1 | hclf4 << 0)
		{
			case 3 << 3 | 0 << 2 | 0 << 1 | 0 << 0: scale = 3; break;
			case 3 << 3 | 1 << 2 | 0 << 1 | 0 << 0: scale = 3; break;
			case 3 << 3 | 0 << 2 | 1 << 1 | 0 << 0: scale = 6; break;
			case 3 << 3 | 1 << 2 | 1 << 1 | 0 << 0: scale = 6; break;

			case 2 << 3 | 0 << 2 | 0 << 1 | 0 << 0: scale = 4; break;
			case 2 << 3 | 1 << 2 | 0 << 1 | 0 << 0: scale = 4; break;
			case 2 << 3 | 0 << 2 | 0 << 1 | 1 << 0: scale = 8; break;
			case 2 << 3 | 1 << 2 | 0 << 1 | 1 << 0: scale = 8; break;
		}
	}

	return get_FCLK() / scale;
}

static ulong get_PCLK()
{
	CLOCK_POWER * const clk_power = GetBase_CLOCK_POWER();
	ulong hdivn = GET_TWO_BIT(clk_power->CLKDIVN, 1);
	ulong pdivn = GET_ONE_BIT(clk_power->CLKDIVN, 0);
	ulong hclf3 = GET_ONE_BIT(clk_power->CAMDIVN, 8);
	ulong hclf4 = GET_ONE_BIT(clk_power->CAMDIVN, 9);

	ulong scale = 0;
	if ( hdivn < 2 )
	{
		switch (hdivn << 3 | pdivn << 2)
		{
			case 0 << 3 | 0 << 2: scale = 1; break;
			case 0 << 3 | 1 << 2: scale = 2; break;
			case 1 << 3 | 0 << 2: scale = 2; break;
			case 1 << 3 | 1 << 2: scale = 4; break;
		}
	}
	else
	{
		switch (hdivn << 3 | pdivn << 2 | hclf3 << 1 | hclf4 << 0)
		{
			case 3 << 3 | 0 << 2 | 0 << 1 | 0 << 0: scale = 3; break;
			case 3 << 3 | 1 << 2 | 0 << 1 | 0 << 0: scale = 6; break;
			case 3 << 3 | 0 << 2 | 1 << 1 | 0 << 0: scale = 6; break;
			case 3 << 3 | 1 << 2 | 1 << 1 | 0 << 0: scale =12; break;

			case 2 << 3 | 0 << 2 | 0 << 1 | 0 << 0: scale = 4; break;
			case 2 << 3 | 1 << 2 | 0 << 1 | 0 << 0: scale = 8; break;
			case 2 << 3 | 0 << 2 | 0 << 1 | 1 << 0: scale = 8; break;
			case 2 << 3 | 1 << 2 | 0 << 1 | 1 << 0: scale =16; break;
		}
	}

	return get_FCLK() / scale;
}

int main()
{
	return 0;
};

int raise (int num)
{
	return 0;
}

