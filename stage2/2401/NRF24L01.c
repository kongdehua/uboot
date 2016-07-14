/*
	 NRF24L01是NORDIC公司最近生产的一款无线通信芯片，采用FSK调制，内部集成NORDIC自己的Enhanced Short Burst协议。可以实现点对点或是1对6的无线通信。无线通信速度可以达到2M(bps)。NORDIC公司提供通信模块的GERBER文件，可以直接加工生产。嵌入式工程师或是单片机爱好者只需要为单片机系统预留5个GPIO，1个中断输入引脚，就可以很容易实现无线通信的功能，非常适合用来为MCU系统构建无线通信功能。
	 从单片机控制的角度来看，我们只需要关注六个控制和数据信号，分别为CSN、SCK、MISO、MOSI、IRQ、CE。

CSN:芯片的片选线，CSN为低电平芯片工作。
SCK：芯片控制的时钟线（SPI时钟）
MISO：芯片控制数据线（Master input slave output）
MOSI：芯片控制数据线（Master output slave input）
IRQ：中断信号。无线通信过程中MCU主要是通过IRQ与NRF24L01进行通信
CE：芯片的模式控制线。在CSN为低的情况下，CE协同NRF24L01的CONFIG寄存器共同决定NRF24L01的状态。（参照NRF24L01的状态机）

  NRF24L01的状态机。对于NRF24L01的固件编程工作主要是参照NRF24L01的状态机。状态列表：
	1. Power Down Mode：掉电模式
	2. Tx Mode： 发射模式
	3. Rx Mode： 接收模式
	4. Standby-1 Mode：待机1模式
	5. Standby-2 Mode：待机2模式

上面五种模式之间的相互切换方法以及切换所需要的时间参照
1)置CSN为低，使能芯片，配置芯片各个参数。（过程见Tx和Rx的配置过程）配置参数在Power Down状态中完成。
2）如果是Tx模式，填充Tx FIFO
3）配置完成以后，通过CE和CONFIG中的PWR_UP和PRIM_RX参数确定24L01要切换的状态。
Tx Mode：PWR_UP=1; PRIM_RX=0; CE=1 (more than 10us)
Rx Mode：PWR_UP=1; PRIM_RX=1; CE=1
4）IRQ引脚会在以下三种情况变低
Tx FIFO发完并且收到ACK（使能ACK情况下）
Rx FIFO收到数据
达到最大重发次数
将IRQ接到外部中断输入引脚，通过中断程序进行处理。

3. Tx和Rx的配置过程
本节只是叙述了采用ENHANCED SHORT BURST通信方式的Tx和Rx的配置及通信过程，熟悉了24L01以后可以采用别的通信方式。

Tx模式初始化过程
初始化步骤
1. 写Tx节点的地址              TX_ADDR
2. 写Rx节点的地址（主要为了使能Auto Ack） RX_ADDR_P0
3. 使能Auto ACK                           EN_AA
4. 使能PIPE 0                             EN_RXADDR
5. 配置自动重发次数                       SETUP_RETR
6. 选择通信频率                           RF_CH
7. 配置发射参数（缔造放大器增益，发射功率、无线速率）
                                          RF_SETUP
8. 选择通道0有效数据宽度                  Rx_Pw_P0
9. 配置24L01的基本参数以及切换工作模式    CONFIG

Rx模式初始化过程
1. 写Rx节点的地址                         RX_ADDR_P0
2. 使能AUTO ACK                           EN_AA
3. 使能PIPE 0                             EN_RXADDR
4. 选择通信频率                           RF_CH
5. 选择通道0有效数据宽度                  Rx_Pw_P0
6. 配置发射参数
7. 配置24L01的基本参数以及切换工作模式    CONFIG

uchar nrf2401_RW(uchar byte) {
	uchar bit_ctr;
	for(bit_ctr = 0; bit_ctr<0; bit_ctr++)
	{
		MOSI = byte&0x80;
		byte = byte << 1;
		SCK = 1;
		byte |= MISO;
		SCK = 0;
	}
	return byte;
}

GPIO完成SPI的功能模拟。将输出字节MOSI从MSB循环输出，同时将输入字节MISO从LSB循环一如。上升沿读入，下降沿输出。（好像写反了）

uchar nrf2401_RW_Reg(uchar reg, uchar value) {
	uchar status;
	CSN = 0;
	status = nrf2401_RW(reg);
	nrf2401_RW(value);
	CSN = 1;
	return status;
}
寄存器访问函数：用来设置24L01的寄存器的值。基本思路就是通过WRITE_REG命令（也就是0x20+寄存器地址）把要设定的值写到相应的寄存器地址里面去，并读取返回值。对于函数来说也就是把value值reg寄存器中。
需要注意的是，访问NRF24L01之前首先要enable芯片（CSN=0；），访问完了再disable（CSN=1；）。
*/
#include "NRF24L01.h"

#include "standard.h"
#include "s3c2440.h"
#include "global_data.h"

#include "timer.h"
#include "spi.h"

#define ENABLE_2401_CSN() \
	  SET_TWO_BIT(GPECON, 15 * 2, 0x1); \

#define ENABLE_2401_CE() \
	  SET_TWO_BIT(GPECON, 14 * 2, 0x1); \


uchar const TX_ADDRESS[TX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01}; // Define a static TX address
uchar rx_buf[TX_PLOAD_WIDTH];
uchar tx_buf[] = {"ashining\r\n"};
//uchar tx_buf[TX_PLOAD_WIDTH]={0X42,0X42,0X43,0X45,0X49};

void nrf2401_init()
{
	ENABLE_2401_CSN();
	ENABLE_2401_CE();

	nrf2401_deselect();
	nrf2401_CE_Disable();

	initSPI();
}

void nrf2401_select()
{
	  SET_ONE_BIT(GPEDAT, 15, 0x0);
} 
  
void nrf2401_deselect()
{ 
	  SET_ONE_BIT(GPEDAT, 15, 0x1); 
} 

void nrf2401_CE_Enable()
{
	  SET_ONE_BIT(GPEDAT, 14, 0x0);
} 
  
void nrf2401_CE_Disable()
{ 
	  SET_ONE_BIT(GPEDAT, 14, 0x1); 
} 

uchar nrf2401_RW_Reg(BYTE reg, BYTE value)
{
	uchar status = 0x0;

	nrf2401_select();

	status = spi_writeByte(reg); // select register
	//status = spi_readByte();
	spi_writeByte(value); // ..and write value to it..

	nrf2401_deselect();

	return(status); // return nRF24L01 status byte
}

BYTE nrf2401_Read(BYTE reg)
{
	BYTE reg_val;
	nrf2401_select();

	spi_writeByte(reg); // Select register to read from..
	reg_val = spi_readByte(); // ..then read registervalue

	nrf2401_deselect();
	return(reg_val); // return register value
}


uchar nrf2401_Write_Buffer(uchar reg, const uchar *pBuf, uchar bytes)
{
	uchar status, byte_ctr;
	nrf2401_select();

	status = spi_writeByte(reg);

	for (byte_ctr = 0; byte_ctr < bytes; byte_ctr++)
	{
		spi_writeByte(pBuf[byte_ctr]);
	}

	nrf2401_deselect();
	return status;
}

uchar nrf2401_Read_Buffer(uchar reg, uchar *pBuf, uchar bytes)
{
	uchar status, byte_ctr;
	nrf2401_select();
	status = spi_writeByte(reg);

	for (byte_ctr = 0; byte_ctr < bytes; byte_ctr++)
	{
		pBuf[byte_ctr] = spi_readByte();
	}

	nrf2401_deselect();
	return status;
}

void nrf2401_power_off()
{
	nrf2401_CE_Enable();

	nrf2401_RW_Reg(WRITE_REG + CONFIG, 0x0D);

	nrf2401_CE_Disable();
	delay(20);
}

void nrf2401_init_rx_mode()
{
	nrf2401_CE_Enable();

	nrf2401_Write_Buffer(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);
	nrf2401_RW_Reg(WRITE_REG + EN_AA, 0x01); // Enable Auto.Ack:Pipe0
	nrf2401_RW_Reg(WRITE_REG + EN_RXADDR, 0x01); // Enable Pipe0
	nrf2401_RW_Reg(WRITE_REG + RF_CH, 40); // Select RF channel 40
	nrf2401_RW_Reg(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);
	nrf2401_RW_Reg(WRITE_REG + RF_SETUP, 0x07);
	nrf2401_RW_Reg(WRITE_REG + CONFIG, 0x0f);

	nrf2401_CE_Disable();
}

/*
 *Read statu register of nRF24L01
 *@param Statu register of nRF24L01
 */
BYTE nrf2401_ReadStatusReg( void )
{
	BYTE Status;
	nrf2401_select( );
	Status = nrf2401_Read( STATUS );
	nrf2401_deselect( );
	return Status;
}

/*
 *Clear IRQ cuased by nRF24L01
 */
void nrf2401_ClearIRQ( BYTE IRQ_Source )
{
	BYTE btmp = 0;

	IRQ_Source &= ( 1<<RX_DR ) | ( 1<<TX_DS ) | ( 1<<MAX_RT );
	btmp = nrf2401_ReadStatusReg( );

	nrf2401_select();
	nrf2401_RW_Reg(WRITE_REG + STATUS, IRQ_Source | btmp);
	nrf2401_deselect();

	nrf2401_ReadStatusReg( );
}

/*
 *Flush the TX buffer
 */
void L01_FlushTX( void )
{
	nrf2401_select( );
	spi_writeByte(FLUSH_TX); // select register
	nrf2401_deselect( );
}
/*
 *Flush the RX buffer
 */
void L01_FlushRX( void )
{
	nrf2401_select( );
	spi_writeByte(FLUSH_RX); // select register
	nrf2401_deselect( );
}

void nrf2401_init_tx_mode()
{
	nrf2401_CE_Disable();


	nrf2401_ClearIRQ(IRQ_ALL);
	nrf2401_RW_Reg( WRITE_REG + DYNPD , ( 1<<0 ) );//Enable pipe 0 dynamic payload length
	nrf2401_RW_Reg( WRITE_REG + FEATURE,( 0x07 ) );
	nrf2401_Read( WRITE_REG + DYNPD );
	nrf2401_Read( WRITE_REG + FEATURE );

	nrf2401_RW_Reg(WRITE_REG + CONFIG , 1 << EN_CRC | 1 << PWR_UP); 

	nrf2401_RW_Reg(WRITE_REG + EN_AA  , 0x01); // Enable Auto.Ack:Pipe0
	nrf2401_RW_Reg(WRITE_REG + EN_RXADDR, 0x01); // Enable Pipe0
	nrf2401_RW_Reg(WRITE_REG + SETUP_AW  , AW_5BYTES); // Enable Pipe0
	nrf2401_RW_Reg(WRITE_REG + SETUP_RETR, ARD_4000US | 0xa); // 500us + 86us, 10 retrans...
	nrf2401_RW_Reg(WRITE_REG + RF_CH, 60); // Select RF channel 40
	nrf2401_RW_Reg(WRITE_REG + RF_SETUP, 0x26); 
	//nrf2401_RW_Reg(WRITE_REG + CONFIG, 0x0e); // Set PWR_UP bit

	nrf2401_Write_Buffer(WRITE_REG + TX_ADDR   , TX_ADDRESS, TX_ADR_WIDTH);
	nrf2401_Write_Buffer(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);


	BYTE controlreg = nrf2401_Read( CONFIG );
	controlreg &= ~( 1<<PRIM_RX ); 
	nrf2401_RW_Reg( WRITE_REG + CONFIG, controlreg );
	nrf2401_RW_Reg(WRITE_REG + RF_SETUP, 0x6 | 1<<3 ); 

	L01_FlushTX( );
	L01_FlushRX( );
	nrf2401_Write_Buffer(WR_TX_PLOAD, tx_buf, 16); // Writes data to TX payload

	delay(10000);

	//nrf2401_CE_Enable();
}
