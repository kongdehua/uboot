#ifndef _NRF24L01_H_
#define _NRF24L01_H_

#include "standard.h"
#define TX_ADR_WIDTH 5
#define TX_PLOAD_WIDTH 5

#define READ_REG  0x00 //Define read cmd
#define WRITE_REG 0x20 //Define write cmd
#define RD_RX_PLOAD 0x61 // Define RX payload register
#define WR_TX_PLOAD 0xA0 // Define TX payload register
#define FLUSH_TX    0xE1 // Define flush TX register
#define FLUSH_RX    0xE2 // Define flush RX register
#define REUSE_TX_PL 0xE3 // Define reuse TX payload
#define NOP         0xFF // nop operation

// register list
#define CONFIG      0x00 // Config 
    //reserved: bit7, only '0' allowed
    #define MASK_RX_DR   6 //RW, Mask interrupt caused by RX_DR
    #define MASK_TX_DS   5 //RW, Mask interrupt caused by TX_DS
    #define MASK_MAX_RT  4 //RW, Mask interrupt caused by MAX_RT
    #define EN_CRC       3 //RW, Enable CRC.
    #define CRCO         2 //RW, CRC encoding scheme, 0: 1 byte, 1: 2 bytes
    #define PWR_UP       1 //RW, Power up, 1: power up, 0: power down
    #define PRIM_RX      0 //RW, RX/TX control, 1: PRX, 0: PTX
#define EN_AA       0x01 // Enable Auto Acknowledgment
#define EN_RXADDR   0x02 // Enabled RX addresses
#define SETUP_AW    0x03 // Setup address width
    // reserved: bit7-bit2, only '000000' allowed
    #define AW_RERSERVED 0x0 
    #define AW_3BYTES    0x1
    #define AW_4BYTES    0x2
    #define AW_5BYTES    0x3
#define SETUP_RETR  0x04 // Setup Auto Retrans
    //for bit7-bit4, Auto transmission delay
    #define ARD_250US    (0x00<<4)
    #define ARD_500US    (0x01<<4)
    #define ARD_750US    (0x02<<4)
    #define ARD_1000US   (0x03<<4)
    #define ARD_2000US   (0x07<<4)
    //......
    #define ARD_4000US   (0x0F<<4)
    //for bit3-bit0, Auto retransmission Count
    #define ARC_DISABLE   0x00
    //......
    #define ARC_15        0x0F
#define RF_CH       0x05 // RF channel
#define RF_SETUP    0x06 // RF setup
#define STATUS      0x07 // Status
    //reserved, bit7, only '0' allowed
    #define RX_DR         6 //RW, Data ready RX FIFO interrupt
    #define TX_DS         5 //RW, Data sent TX FIFO interrupt
    #define MAX_RT        4 //RW, Maximum number of TX retransmits interrupt
    //for bit3-bit1, R, Data pipe number for the payload available for reading from
    //RX_FIFO, 000-101, Data pipe number, 110, not used, 111, RX FIFO empty
    #define TX_FULL_0     0 //R, TX FIFO full flag
#define OBSERVE_TX  0x08 // Observe TX
#define CD          0x09 // Carrier Detect
#define RX_ADDR_P0  0x0A // RX address pipe0
#define RX_ADDR_P1  0x0B // RX address pipe1
#define RX_ADDR_P2  0x0C // RX address pipe2
#define RX_ADDR_P3  0x0D // RX address pipe3
#define RX_ADDR_P4  0x0E // RX address pipe4
#define RX_ADDR_P5  0x0F // RX address pipe5

#define TX_ADDR     0x10 // TX address

#define RX_PW_P0    0x11 // RX payload width, pipe0
#define RX_PW_P1    0x12 // RX payload width, pipe1
#define RX_PW_P2    0x13 // RX payload width, pipe2
#define RX_PW_P3    0x14 // RX payload width, pipe3
#define RX_PW_P4    0x15 // RX payload width, pipe4
#define RX_PW_P5    0x16 // RX payload width, pipe5

#define FIFO_STATUS 0x17 // FIFO Status Register
#define DYNPD       0x1C
#define FEATURE     0x1D

#define REPEAT_CNT          15//0-15, repeat transmit count
void nrf2401_select();
void nrf2401_deselect();
void nrf2401_CE_Enable();
void nrf2401_CE_Disable();

void nrf2401_init();
void nrf2401_init_tx_mode();
void nrf2401_init_rx_mode();
void nrf2401_power_off();

uchar nrf2401_RW_Reg(BYTE reg, BYTE value);
BYTE nrf2401_Read(BYTE reg);

uchar nrf2401_Write_Buffer(uchar reg, const uchar *pBuf, uchar bytes);
uchar nrf2401_Read_Buffer(uchar reg, uchar *pBuf, uchar bytes);


/*L01 clear IRQ*/
#define IRQ_ALL  ( (1<<RX_DR) | (1<<TX_DS) | (1<<MAX_RT) )
void nrf2401_ClearIRQ( BYTE IRQ_Source );
#endif
