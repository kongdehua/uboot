/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "standard.h"
#include "sdio.h"
#include "diskio.h"		/* FatFs lower layer API */
//#include "usbdisk.h"	/* Example: Header file of existing USB MSD control module */
//#include "atadrive.h"	/* Example: Header file of existing ATA harddisk control module */
//#include "sdcard.h"		/* Example: Header file of existing MMC/SDC contorl module */

/* Definitions of physical drive number for each drive */
#define ATA		0	/* Example: Map ATA harddisk to physical drive 0 */
#define MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define USB		2	/* Example: Map USB MSD to physical drive 2 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	/*
	int result;

	switch (pdrv) {
	case ATA :
		result = ATA_disk_status();

		// translate the reslut code here

		return stat;

	case MMC :
		result = MMC_disk_status();

		// translate the reslut code here

		return stat;

	case USB :
		result = USB_disk_status();

		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
	*/
	return 0;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;

	unsigned short RCA;
	u8 res=sdio_Initialize();//SD卡初始化 
	sdio_ScanCard(&RCA);
	sdio_SelectCard(RCA);

	if(res)return  STA_NOINIT;
	else return 0; //初始化成功
	/*
	switch (pdrv) {
	case ATA :
		result = ATA_disk_initialize();

		// translate the reslut code here

		return stat;

	case MMC :
		result = MMC_disk_initialize();

		// translate the reslut code here

		return stat;

	case USB :
		result = USB_disk_initialize();

		// translate the reslut code here

		return stat;
	}
	*/
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	int result;

	BYTE res=0; 
	if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误			 

	res=sdio_ReadData(buff,sector,count);	 
	while(res)//读出错
	{
		unsigned short RCA;
		u8 res=sdio_Initialize();//SD卡初始化 
		sdio_ScanCard(&RCA);
		sdio_SelectCard(RCA);

		res=sdio_ReadData(buff,sector,count);	
		//printf("sd rd error:%d\r\n",res);
	}
	//处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
	if(res==0x00)return RES_OK;	 
	else return RES_ERROR;	 

	/*
		 switch (pdrv) {
		 case ATA :
	// translate the arguments here

		result = ATA_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;

	case MMC :
		// translate the arguments here

		result = MMC_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;

	case USB :
		// translate the arguments here

		result = USB_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;
	}

	*/
	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	int result;

	u8 res=0;  
	if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误			 

	res=sdio_WriteData((u8*)buff,sector,count);
	while(res)//写出错
	{
		unsigned short RCA;
		u8 res=sdio_Initialize();//SD卡初始化 
		sdio_ScanCard(&RCA);
		sdio_SelectCard(RCA);

		res=sdio_WriteData((u8*)buff,sector,count);	
		//printf("sd wr error:%d\r\n",res);
	}

	//处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
	if(res == 0x00)return RES_OK;	 
	else return RES_ERROR;	
	/*
	switch (pdrv) {
	case ATA :
		// translate the arguments here

		result = ATA_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case MMC :
		// translate the arguments here

		result = MMC_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case USB :
		// translate the arguments here

		result = USB_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;
	}

	*/
	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	int result;

	switch(cmd)
	{
		case CTRL_SYNC:
			res = RES_OK; 
			break;	 
		case GET_SECTOR_SIZE:
			*(DWORD*)buff = 512; 
			res = RES_OK;
			break;	 
		case GET_BLOCK_SIZE:
			//*(WORD*)buff = SDCardInfo.CardBlockSize;
			*(WORD*)buff = 512; //SDCardInfo.CardBlockSize;
			res = RES_OK;
			break;	 
		case GET_SECTOR_COUNT:
			//*(DWORD*)buff = SDCardInfo.CardCapacity/512;
			*(DWORD*)buff = 1<<(33-9);
			res = RES_OK;
			break;
		default:
			res = RES_PARERR;
			break;
	}
	return res;
	/*
	switch (pdrv) {
	case ATA :

		// Process of the command for the ATA drive

		return res;

	case MMC :

		// Process of the command for the MMC/SD card

		return res;

	case USB :

		// Process of the command the USB drive

		return res;
	}

	return RES_PARERR;
	*/
}

DWORD get_fattime (void)
{
	return 0;
}
