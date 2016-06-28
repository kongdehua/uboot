#ifndef _SDIO_H_
#define _SDIO_H_

typedef int BOOL;
typedef unsigned char BYTE;

#define TRUE 1
#define FALSE 0

typedef enum
{
	SD_CARD_MMC,
	SD_CARD_1_0,
	SD_CARD_2_0,
	SD_CARD_HD,
	SD_CARD_UNKNOWN,
}SD_CARD_TYPE;

typedef struct
{
	u8  CSDStruct;            /*!< CSD structure */
	u8  SysSpecVersion;       /*!< System specification version */
	u8  Reserved1;            /*!< Reserved */        
	u8  TAAC;                 /*!< Data read access-time 1 */
	u8  NSAC;                 /*!< Data read access-time 2 in CLK cycles */
	u8  MaxBusClkFrec;        /*!< Max. bus clock frequency */
	u16 CardComdClasses;      /*!< Card command classes */
	u8  RdBlockLen;           /*!< Max. read data block length */
	u8  PartBlockRead;        /*!< Partial blocks for read allowed */
	u8  WrBlockMisalign;      /*!< Write block misalignment */
	u8  RdBlockMisalign;      /*!< Read block misalignment */
	u8  DSRImpl;              /*!< DSR implemented */ 
	u8  Reserved2;            /*!< Reserved */
	u32 DeviceSize;           /*!< Device Size */     
	u8  MaxRdCurrentVDDMin;   /*!< Max. read current @ VDD min */
	u8  MaxRdCurrentVDDMax;   /*!< Max. read current @ VDD max */
	u8  MaxWrCurrentVDDMin;   /*!< Max. write current @ VDD min */
	u8  MaxWrCurrentVDDMax;   /*!< Max. write current @ VDD max */
	u8  DeviceSizeMul;        /*!< Device size multiplier */
	u8  EraseGrSize;          /*!< Erase group size */
	u8  EraseGrMul;           /*!< Erase group size multiplier */
	u8  WrProtectGrSize;      /*!< Write protect group size */
	u8  WrProtectGrEnable;    /*!< Write protect group enable */
	u8  ManDeflECC;           /*!< Manufacturer default ECC */
	u8  WrSpeedFact;          /*!< Write speed factor */
	u8  MaxWrBlockLen;        /*!< Max. write data block length */
	u8  WriteBlockPaPartial;  /*!< Partial blocks for write allowed */
	u8  Reserved3;            /*!< Reserded */
	u8  ContentProtectAppli;  /*!< Content protection application */
	u8  FileFormatGrouop;     /*!< File format group */
	u8  CopyFlag;             /*!< Copy flag (OTP) */
	u8  PermWrProtect;        /*!< Permanent write protection */
	u8  TempWrProtect;        /*!< Temporary write protection */
	u8  FileFormat;           /*!< File Format */
	u8  ECC;                  /*!< ECC code */
	u8  CSD_CRC;              /*!< CSD CRC */
	u8  Reserved4;            /*!< always 1*/
} SD_CSD;

//SD
typedef struct
{
	u8  ManufacturerID;       /*!< ManufacturerID */
	u16 OEM_AppliID;          /*!< OEM/Application ID */
	u32 ProdName1;            /*!< Product Name part1 */
	u8  ProdName2;            /*!< Product Name part2*/
	u8  ProdRev;              /*!< Product Revision */
	u32 ProdSN;               /*!< Product Serial Number */
	u8  Reserved1;            /*!< Reserved1 */
	u16 ManufactDate;         /*!< Manufacturing Date */
	u8  CID_CRC;              /*!< CID CRC */
	u8  Reserved2;            /*!< always 1 */
} SD_CID;

//
typedef struct                 
{ 
	SD_CSD SD_csd;               
	SD_CID SD_cid;               
	long long CardCapacity;   //
	u32 CardBlockSize;    //
	u16 RCA;          //
	SD_CARD_TYPE CardType;        //
} SD_CardInfo;                 
extern SD_CardInfo SDCardInfo;//SD¿¨ÐÅÏ¢       

#define SDCLK 24000000  //get_PCLK()=50MHz

#define NoResponse  0
#define ResponseR1  1               // Short response required
#define ResponseR1b 11
#define ResponseR2  2
#define ResponseR3  3
#define ResponseR4  4
#define ResponseR5  5
#define ResponseR6  6
#define ResponseR7  7

#define SDIO_COMMAND_WITH_DATA  0x00000800
#define LONG_RESPONSE           0x00000400
#define WAIT_FOR_RESPONSE       0x00000200
#define START_COMMAND           0x00000100

#define COMMAND_START_BIT     0x00000040        // Commands are 6 bits, but the 7th bit must be set!
#define MAX_CMD_VALUE         0x0000003F        // In SD/MMC clock cycles

//*************************************************************************
#define WAIT_TIME                       0x20000000        // while loop delay
#define START_BIT                           0x00
#define TRANSMISSION_BIT                    0x00
#define START_RESERVED                      0x3F
#define END_RESERVED                        0xFE
#define END_BIT                             0x01


#define CRC_CHECK_FAILED        0x00001000        
#define COMMAND_SENT          0x00000800
#define COMMAND_TIMED_OUT       0x00000400        
#define RESPONSE_RECEIVED       0x00000200        
#define COMMAND_IN_PROGRESS       0x00000100        

#define SHORT_RESPONSE_LENGTH     0x00000006        // In bytes
#define LONG_RESPONSE_LENGTH      0x00000010        // In bytes

//----- Register definitions for SDIDATSTA control register (transfer status register) -----                               
#define NO_BUSY             0x00000800                                                                                     
#define READ_WAIT_REQUEST_OCCURED   0x00000400        
#define SDIO_INTERRUPT_DETECTED     0x00000200                                                                             
//#define FIFO_FAIL_ERROR         0x00000100
#define DATA_TRANSMIT_CRC_ERROR     0x00000080
#define DATA_RECEIVE_CRC_ERROR      0x00000040
#define DATA_TIME_OUT         0x00000020
#define DATA_TRANSMIT_FINISHED      0x00000010
#define BUSY_CHECKS_FINISH        0x00000008
#define DATA_TRANSMIT_IN_PROGRESS   0x00000002
#define DATA_RECIEVE_IN_PROGRESS    0x00000001


typedef enum
{  
	SD_CMD_CRC_FAIL                    = (1), /*!< Command response received (but CRC check failed) */
	SD_DATA_CRC_FAIL                   = (2), /*!< Data bock sent/received (CRC check Failed) */
	SD_CMD_RSP_TIMEOUT                 = (3), /*!< Command response timeout */
	SD_DATA_TIMEOUT                    = (4), /*!< Data time out */
	SD_TX_UNDERRUN                     = (5), /*!< Transmit FIFO under-run */
	SD_RX_OVERRUN                      = (6), /*!< Receive FIFO over-run */
	SD_START_BIT_ERR                   = (7), /*!< Start bit not detected on all data signals in widE bus mode */
	SD_CMD_OUT_OF_RANGE                = (8), /*!< CMD's argument was out of range.*/
	SD_ADDR_MISALIGNED                 = (9), /*!< Misaligned address */
	SD_BLOCK_LEN_ERR                   = (10), /*!< Transferred block length is not allowed for the card or the number of transferred bytes does not match the block length */
	SD_ERASE_SEQ_ERR                   = (11), /*!< An error in the sequence of erase command occurs.*/
	SD_BAD_ERASE_PARAM                 = (12), /*!< An Invalid selection for erase groups */
	SD_WRITE_PROT_VIOLATION            = (13), /*!< Attempt to program a write protect block */
	SD_LOCK_UNLOCK_FAILED              = (14), /*!< Sequence or password error has been detected in unlock command or if there was an attempt to access a locked card */
	SD_COM_CRC_FAILED                  = (15), /*!< CRC check of the previous command failed */
	SD_ILLEGAL_CMD                     = (16), /*!< Command is not legal for the card state */
	SD_CARD_ECC_FAILED                 = (17), /*!< Card internal ECC was applied but failed to correct the data */
	SD_CC_ERROR                        = (18), /*!< Internal card controller error */
	SD_GENERAL_UNKNOWN_ERROR           = (19), /*!< General or Unknown error */
	SD_STREAM_READ_UNDERRUN            = (20), /*!< The card could not sustain data transfer in stream read operation. */
  SD_STREAM_WRITE_OVERRUN            = (21), /*!< The card could not sustain data programming in stream mode */
	SD_CID_CSD_OVERWRITE               = (22), /*!< CID/CSD overwrite error */
	SD_WP_ERASE_SKIP                   = (23), /*!< only partial address space was erased */
	SD_CARD_ECC_DISABLED               = (24), /*!< Command has been executed without using internal ECC */
	SD_ERASE_RESET                     = (25), /*!< Erase sequence was cleared before executing because an out of erase sequence command was received */
	SD_AKE_SEQ_ERROR                   = (26), /*!< Error in sequence of authentication. */
	SD_INVALID_VOLTRANGE               = (27),
	SD_ADDR_OUT_OF_RANGE               = (28),
	SD_SWITCH_ERROR                    = (29),
	SD_SDIO_DISABLED                   = (30),
	SD_SDIO_FUNCTION_BUSY              = (31),
	SD_SDIO_FUNCTION_FAILED            = (32),
	SD_SDIO_UNKNOWN_FUNCTION           = (33),
	//
	SD_INTERNAL_ERROR,
	SD_NOT_CONFIGURED,
	SD_REQUEST_PENDING,
	SD_REQUEST_NOT_APPLICABLE,
	SD_INVALID_PARAMETER,
	SD_UNSUPPORTED_FEATURE,
	SD_UNSUPPORTED_HW,
	SD_ERROR,
	SD_COMMAND_ERROR,
	SD_RESPONSE_ERROR,
	SD_COMMAND8_NO_RESPONSE_ERROR,

	SD_UNKNOWN_ERROR,
	SD_UNSUPPORT_ERROR,
	SD_UNUSABLE_ERROR,
	SD_OK = 0
} SD_Error;


typedef enum
{
	SD_API_STATUS_SUCCESS,
	SD_API_STATUS_RESPONSE_TIMEOUT,
	SD_API_STATUS_DEVICE_REMOVED,
	SD_API_STATUS_CRC_ERROR,
	SD_API_STATUS_NOT_IMPLEMENTED,
	SD_API_STATUS_DATA_TIMEOUT
} SD_API_STATUS;

//int sdio_init(void);
int sdio_poweron(void);
SD_Error sdio_Initialize(void);

int sdio_cmdSendSuccess();
int sdio_cmdResponseSuccess();

SD_Error sdio_init_power_on();
SD_Error sdio_CMD0();
SD_Error sdio_CMD8();
SD_Error sdio_ACMD41_HCS0();
SD_Error sdio_ACMD41_HCS1();

SD_Error sdio_CMD2();
SD_Error sdio_CMD3();
#endif
