#include "s3c2440.h"
#include "serial.h"
#include "sdio.h"
#include "vsprintf.h"
#include "timer.h"

SD_CardInfo SDCardInfo;
//static u32 CSD_Tab[4];

SD_API_STATUS GetCommandResponse(unsigned char* pRequest, int Response);
SD_API_STATUS SendCommand(short Cmd, int Arg, 
		short respType, BOOL bDataTransfer);

SD_CARD_TYPE g_enType = SD_CARD_UNKNOWN;

SD_Error sdio_init_power_on()
{
	S3C2440_SDI* sdi = S3C2440_GetBase_SDI();

	// 初始化管脚
	SET_TWO_BIT(GPECON,  5*2, 0x2);
	SET_TWO_BIT(GPECON,  6*2, 0x2);
	SET_TWO_BIT(GPECON,  7*2, 0x2);
	SET_TWO_BIT(GPECON,  8*2, 0x2);
	SET_TWO_BIT(GPECON,  9*2, 0x2);
	SET_TWO_BIT(GPECON, 10*2, 0x2);

	sdi->SDIPRE    = 0x7C; // 50M/0x7D=400k
	sdi->SDICON    = (1<<4) | 1;
	sdi->SDIBSize  = 0x200;
	sdi->SDIDTimer = 0x7FFFFF;
  sdi->SDIFSTA   = sdi->SDIFSTA|(1<<16);  //YH 040223 FIFO reset  

	//*******************************************************
	// 初始化SD卡，74个时钟间隔的CMD0
	for ( int i = 0; i < 74; i++ )
	{
		if ( SD_API_STATUS_SUCCESS != SendCommand(0x0, 0x0, NoResponse, FALSE))
		{
			return SD_COMMAND_ERROR; //printf("CMD0 error!\n");
		}
	}
	return SD_OK;
}

SD_Error sdio_CMD0()
{
	if ( SD_API_STATUS_SUCCESS != SendCommand(0x0, 0x0, NoResponse, FALSE))
	{
		return SD_COMMAND_ERROR; //printf("CMD0 error!\n");
	}
	return SD_OK;
}

SD_Error sdio_CMD8()
{
	unsigned char result[6] = {0};
	//*******************************************************
	//CMD8
	if ( SD_API_STATUS_SUCCESS != SendCommand(0x8, 0x1AA, ResponseR7, FALSE))
	{
			printf("CMD8 send error!\n");
			return SD_COMMAND_ERROR;
	}

	if ( SD_API_STATUS_SUCCESS != GetCommandResponse(&result[0], ResponseR7))
	{
			printf("CMD8 recv error!\n");
			return SD_COMMAND8_NO_RESPONSE_ERROR;
	}

	if ( (0xFF & result[1]) == 0xAA)
		return SD_OK;

	return SD_UNKNOWN_ERROR;
	/*
	printf("cmd8 response is:\n");
	for (int i = 0; i < 5; i++)
	{
		printf("%x ", result[i]);
	}
	printf("\n");
	*/
}

SD_Error sdio_ACMD41_HCS0()
{
	S3C2440_SDI* sdi = S3C2440_GetBase_SDI();

	int valid = 0;
	unsigned char result[6] = {0};
	while (!valid)
	{
	//CMD55
	if ( SD_API_STATUS_SUCCESS != SendCommand(55, 0x0, ResponseR1, FALSE))
		printf("CMD55 send error!\n");
	if ( SD_API_STATUS_SUCCESS != GetCommandResponse(&result[0], ResponseR1))
		printf("CMD55 recv error!\n");

	/*
	printf("cmd55 response is:\n");
	for (int i = 0; i < 5; i++)
	{
		printf("%x ", result[i]);
	}
	printf("\n");
	*/

	//ACMD41
	if ( SD_API_STATUS_SUCCESS != SendCommand(41, 0xC0ff8000, ResponseR3, FALSE))
		printf("ACMD41 send error!\n");
	if ( SD_API_STATUS_SUCCESS != GetCommandResponse(&result[0], ResponseR3))
		printf("ACMD41 recv error!\n");

	printf("cmd ACMD41 response is:\n");
	printf("%x\n", sdi->SDIRSP0);
	for (int i = 0; i < 6; i++)
	{
		printf("%x ", result[i]);
	}
	printf("\n");

	valid = (sdi->SDIRSP0 == 0xC0ff8000) ? 1 : 0;

	if (sdi->SDIRSP0 & 0xC0ff8000)
	{
		printf("this is %s SDHC card", valid ? "valid" : "invalid");
	}
	else
	{
		printf("this is %s SD card", valid ? "valid" : "invalid");
	}
	}
}

SD_Error sdio_ACMD41_HCS1()
{
	unsigned char result[6] = {0};
	S3C2440_SDI* sdi = S3C2440_GetBase_SDI();

	int valid = 0;
	int nCount = 0;
	while ((!valid) && nCount++ < 50 )
	{
		//CMD55
		if ( SD_API_STATUS_SUCCESS != SendCommand(55, 0x0, ResponseR1, FALSE))
		{
			return SD_COMMAND_ERROR; //printf("CMD55 send error!\n");
		}
		if ( SD_API_STATUS_SUCCESS != GetCommandResponse(&result[0], ResponseR1))
		{
			return SD_RESPONSE_ERROR; //printf("CMD55 recv error!\n");
		}

		//ACMD41
		if ( SD_API_STATUS_SUCCESS != SendCommand(41, 0xC0ff8000, ResponseR3, FALSE))
		{
			return SD_COMMAND_ERROR; //printf("ACMD41 send error!\n");
		}
		if ( SD_API_STATUS_SUCCESS != GetCommandResponse(&result[0], ResponseR3))
		{
			return SD_RESPONSE_ERROR; //printf("ACMD41 recv error!\n");
		}

		printf("cmd ACMD41 response is:\n");
		printf("%x\n", sdi->SDIRSP0);
		for (int i = 0; i < 6; i++)
		{
			printf("%x ", result[i]);
		}
		printf("\n");

		valid = (sdi->SDIRSP0 == 0xC0ff8000);
	}

	if (!valid) return SD_UNUSABLE_ERROR;

	return SD_OK;
		/*
		if (sdi->SDIRSP0 & 0xC0ff8000)
		{
			printf("this is %s SDHC card", valid ? "valid" : "invalid");
		}
		else
		{
			printf("this is %s SD card", valid ? "valid" : "invalid");
		}
		*/
}

SD_Error sdio_CMD2()
{
	S3C2440_SDI* sdi = S3C2440_GetBase_SDI();
	unsigned short RCA = 0;
	unsigned char response[20] = {0};

	if ( SD_API_STATUS_SUCCESS != SendCommand(2, 0, ResponseR2, FALSE) )
	{
		return SD_COMMAND_ERROR; //printf("cmd2 send error");
	}

	if ( SD_API_STATUS_SUCCESS != GetCommandResponse(response, ResponseR2) )
	{
		return SD_RESPONSE_ERROR; //printf("cmd2 recv error");
	}

	printf("cmd 2 response is:\n");
	for (int i = 0; i < 20; i++)
	{
		printf("%x ", response[i]);
	}
	printf("\n");
	return SD_OK;
}

SD_Error sdio_CMD3(unsigned short *pRCA)
{
	//unsigned short RCA = 0;
	unsigned char response[20] = {0};
	S3C2440_SDI* sdi = S3C2440_GetBase_SDI();
	//CMD3
	if ( SD_API_STATUS_SUCCESS != SendCommand(3, 0x0, ResponseR6, FALSE))
	{
		return SD_COMMAND_ERROR; //printf("CMD3 send error");
	}
	if ( SD_API_STATUS_SUCCESS != GetCommandResponse(&response[0], ResponseR6))
	{
		return SD_RESPONSE_ERROR; //printf("CMD3 recv error!\n");
	}

	printf("cmd3 response is:\n");
	for (int i = 0; i < 6; i++)
	{
		printf("%x ", response[i]);
	}
	printf("\n");
	*pRCA = (unsigned short)((response[4] << 8) | response[3]);
	printf("RCA is %x\n", pRCA);
	return SD_OK;
}

SD_Error sdio_CMD9(unsigned short RCA, u32 CSD_Tab[4])
{
	unsigned char response[20] = {0};
	S3C2440_SDI* sdi = S3C2440_GetBase_SDI();
	//CMD9
	if ( SD_API_STATUS_SUCCESS != SendCommand(9, RCA<<16, ResponseR2, FALSE))
	{
		return SD_COMMAND_ERROR; //printf("CMD9 send error!\n");
	}
	if ( SD_API_STATUS_SUCCESS != GetCommandResponse(&response[0], ResponseR2))
	{
		return SD_RESPONSE_ERROR; //printf("CMD9 ror!\n");
	}
	printf("cmd9 response is:\n");
	for (int i = 20-1; i >= 0; i--)
	{
		printf("%x ", response[i]);
	}

	CSD_Tab[0]=sdi->SDIRSP0; 
	CSD_Tab[1]=sdi->SDIRSP1; 
	CSD_Tab[2]=sdi->SDIRSP2;           
	CSD_Tab[3]=sdi->SDIRSP3; 
	return SD_OK;
}

// 初始化SD卡
int sdio_poweron(void)
{
	if (SD_OK != sdio_init_power_on()) { printf("power_on error\n"); return SD_COMMAND_ERROR;}
	if (SD_OK != sdio_CMD0())          { printf("CMD0     error\n"); return SD_COMMAND_ERROR;}

	switch (sdio_CMD8())
	{
	case SD_COMMAND8_NO_RESPONSE_ERROR:
		printf("possible SD1.0 or MMC\n");
		return SD_UNSUPPORT_ERROR; break;
	case SD_OK:
		break;
	case SD_UNKNOWN_ERROR:
		printf("Unusable card!\n");
		return SD_UNUSABLE_ERROR; break;
	default:
		printf("CMD8     error\n");
		return SD_COMMAND_ERROR;   break;
	}

	switch (sdio_ACMD41_HCS1())
	{
	case SD_OK:
		g_enType = SD_CARD_HD;
		break;
	default:
		printf("CMD41    error\n"); 
		return SD_COMMAND_ERROR;     break;
	}

	return SD_OK;
}

SD_Error sdio_Initialize(void)
{
	unsigned short RCA = 0;
	u32 CSD_Tab[4] = {0};
	S3C2440_SDI* sdi = S3C2440_GetBase_SDI();

	if (SD_OK != sdio_CMD2())                { printf("cmd2 error\n"); return SD_COMMAND_ERROR;}
	if (SD_OK != sdio_CMD3(&RCA))            { printf("cmd3 error\n"); return SD_COMMAND_ERROR;}

	printf("PCLK is %d\n", get_PCLK());
	sdi->SDIPRE = get_PCLK() / SDCLK - 1;

	printf("SD Frequency is %dHz\n",(get_PCLK()/(sdi->SDIPRE+1)));
	printf("SDIPRE is %d\n",(sdi->SDIPRE));

	if (SD_OK != sdio_CMD9(RCA, CSD_Tab))  { printf("cmd9 error\n"); return SD_COMMAND_ERROR;}

	unsigned char response[20] = {0};
	// CMD7
	if ( SD_API_STATUS_SUCCESS != SendCommand(7, RCA<<16, ResponseR1b, FALSE))
	{
		return SD_COMMAND_ERROR; //printf("CMD9 send error!\n");
	}
	if ( SD_API_STATUS_SUCCESS != GetCommandResponse(&response[0], ResponseR1b))
	{
		return SD_RESPONSE_ERROR; //printf("CMD9 ror!\n");
	}
	printf("cmd7 response is:\n");
	for (int i = 20-1; i >= 0; i--)
	{
		printf("%x ", response[i]);
	}
	if( sdi->SDIRSP0 & 0x1e00!=0x800 ) printf("cmd7 error\n");

	unsigned char result[6] = {0};
	////////////////////////////////////////////////////////////////////////////////
	// ACMD6
	if ( SD_API_STATUS_SUCCESS != SendCommand(55, RCA << 16, ResponseR1, FALSE))
	{
		return SD_COMMAND_ERROR; //printf("CMD55 send error!\n");
	}
	if ( SD_API_STATUS_SUCCESS != GetCommandResponse(&result[0], ResponseR1))
	{
		return SD_RESPONSE_ERROR; //printf("CMD55 recv error!\n");
	}

	if ( SD_API_STATUS_SUCCESS != SendCommand(6, 0x1<<1, ResponseR1, FALSE))
	{
		return SD_COMMAND_ERROR; //printf("ACMD6 send error!\n");
	}
	if ( SD_API_STATUS_SUCCESS != GetCommandResponse(&result[0], ResponseR1))
	{
		return SD_RESPONSE_ERROR; //printf("ACMD6 recv error!\n");
	}

	printf("cmd ACMD6 response is:\n");
	printf("%x\n", sdi->SDIRSP0);
	for (int i = 0; i < 6; i++)
	{
		printf("%x ", result[i]);
	}
	printf("\n");

	unsigned char buf[512] = {0x12, 0x34, 0x56, 0x78, 0x90};
	unsigned char *Tx_buf = buf;
	long long addr = 0;
	u16 sizeBlk = 512;
	addr >>= 9;

	for (int nIndex = 0; nIndex < 512; nIndex++)
	{
	buf[nIndex] = 0x66;
	}
	int block = 1;
	int wt_cnt = 0;
	int status = 0;

	/*
	u32 cardstatus = 0;
	u32 timeout = 200;
	do
	{
		timeout--;
		if ( SD_API_STATUS_SUCCESS != SendCommand(13, RCA<<16, ResponseR1, FALSE))
		{
			printf("cmd13 error");
		}
		if ( SD_API_STATUS_SUCCESS != GetCommandResponse(response, ResponseR1))
		{
			printf("cmd13 response error");
		}
		cardstatus=sdi->SDIRSP0;                           
	} while (((cardstatus&0x00000100)==0)&&(timeout>0));
	//if(timeout==0)return SD_ERROR;
	printf("\n55555555555555555555\n");
	*/

	sdi->SDIDTimer=0x7fffff;		// Set timeout count
	sdi->SDIBSize=0x200;			// 512byte(128word)
	sdi->SDIFSTA = sdi->SDIFSTA|(1<<16); // FIFO reset

	//************************************************************************
	sdi->SDIFSTA = sdi->SDIFSTA | (1<<16);
	sdi->SDIDatCon = (2<<22)|(1<<20)|(1<<17)|(1<<16)|(1<<14)|(3<<12)|(block<<0);
	if ( SD_API_STATUS_SUCCESS != SendCommand(24, 0x0, ResponseR1, FALSE))
	{
		printf("cmd24 error");
	}
	if ( SD_API_STATUS_SUCCESS != GetCommandResponse(response, ResponseR1))
	{
		printf("cmd24 response error");
	}
	sdi->SDICmdSta=0xa00; // Clear cmd_end(with rsp)       

	while(wt_cnt<sizeBlk*block/4)  
	{
		status=sdi->SDIFSTA;
		if((status&0x2000)==0x2000) 
		{
			sdi->SDIDAT=(*Tx_buf) | *(Tx_buf+1)<<8 | *(Tx_buf+1)<<16 | *(Tx_buf+1)<<24 ;    
			Tx_buf+=4;
			wt_cnt++;
			//Uart_Printf("Block No.=%d, wt_cnt=%d\n",block,wt_cnt);
		}
	}

	printf("\n111111111111111111\n");

	int finish = sdi->SDIDatSta;
	// Chek timeout or data end
	while( !( ((finish&0x10)==0x10) | ((finish&0x20)==0x20) ))  
		finish=sdi->SDIDatSta;

	if( (finish&0xfc) != 0x10 )
	{
		printf("DATA:finish=0x%x\n", finish);
		sdi->SDIDatSta=0xec;  // Clear error state
		return 0;
	}
	//return 1;

	sdi->SDIDatCon = sdi->SDIDatCon &~(7<<12);   //YH 040220, Clear Data Transfer mode => no operation, Cleata Data Transfer start
	sdi->SDIDatSta=0x10;  // Clear data Tx/Rx end


	for (int nIndex = 0; nIndex < 512; nIndex++)
	{
		buf[nIndex] = 0;
	}

	//***********************************************************************
	u32 mode = 0 ;
	int rd_cnt = 0;
	unsigned char *Rx_buffer = buf;
	sdi->SDIFSTA = sdi->SDIFSTA|(1<<16);  // FIFO reset
	sdi->SDIDatCon =(2<<22)|(1<<19)|(1<<17)|(1<<16)|(1<<14)|(2<<12)|(block<<0); //

	if ( SD_API_STATUS_SUCCESS != SendCommand(17, 0x0, ResponseR1, FALSE))
	{
		printf("cmd17 error");
	}
	if ( SD_API_STATUS_SUCCESS != GetCommandResponse(response, ResponseR1))
	{
		printf("cmd17 response error");
	}
	sdi->SDICmdSta=0xa00; // Clear cmd_end(with rsp)       

	printf("1111\n");
	while(rd_cnt<128*block) // 512*block bytes 
	{ 
		if((sdi->SDIDatSta&0x20)==0x20) // Check timeout  
		{ 
			sdi->SDIDatSta=(0x1<<0x5);  // Clear timeout flag 
			break; 
		} 
		status=sdi->SDIFSTA; 
		if((status&0x1000)==0x1000) // Is Rx data? 
		{ 
			u32 dat = sdi->SDIDAT;
			*Rx_buffer++= dat& 0xFF; 
			*Rx_buffer++= (dat& 0xFF00) >> 8; 
			*Rx_buffer++= (dat& 0xFF0000) >> 16;  
			*Rx_buffer++= (dat& 0xFF000000) >> 24;  
			rd_cnt++; 
		} 
	}
	printf("222\n");

	 finish = sdi->SDIDatSta;
	// Chek timeout or data end
	while( !( ((finish&0x10)==0x10) | ((finish&0x20)==0x20) ))  
		finish=sdi->SDIDatSta;

	printf("333\n");

	if( (finish&0xfc) != 0x10 )
	{
		printf("DATA:finish=0x%x\n", finish);
		sdi->SDIDatSta=0xec;  // Clear error state
		return 0;
	}
	//return 1;

	sdi->SDIDatCon = sdi->SDIDatCon &~(7<<12);   //YH 040220, Clear Data Transfer mode => no operation, Cleata Data Transfer start
	sdi->SDIDatSta=sdi->SDIDatSta&0x200;  //Clear Rx FIFO Last data Ready, YH 040221
	sdi->SDIDatSta=0x10;  // Clear data Tx/Rx end

	for (int nIndex = 0; nIndex < 512; nIndex++)
	{
		printf("%x ", buf[nIndex]);
	}

	// CMD7
	if ( SD_API_STATUS_SUCCESS != SendCommand(7, 0<<16, ResponseR1b, FALSE))
	{
		return SD_COMMAND_ERROR; //printf("CMD9 send error!\n");
	}
	if ( SD_API_STATUS_SUCCESS != GetCommandResponse(&response[0], ResponseR1b))
	{
		return SD_RESPONSE_ERROR; //printf("CMD9 ror!\n");
	}
	printf("cmd7 response is:\n");
	for (int i = 20-1; i >= 0; i--)
	{
		printf("%x ", response[i]);
	}
	sdi->SDICmdSta=0x800; // Clear cmd_end(no rsp)

	while(1)
	{
	}
}

SD_Error SD_GetCardInfo(SD_CardInfo *cardinfo)
{ 
	SD_Error errorstatus=SD_OK;
	/*
	u8 tmp=0;    
	cardinfo->CardType=(u8)CardType;        //<81>0<86>7¡§<81>0<86>8¨¤<81>0<88>4<81>0<88>1
	cardinfo->RCA=(u16)RCA;             //<81>0<86>7¡§RCA<81>0<89>0<81>0<85>8
	tmp=(u8)((CSD_Tab[0]&0xFF000000)>>24);
	cardinfo->SD_csd.CSDStruct=(tmp&0xC0)>>6;   //CSD<81>0<86>5¨¢<81>0<86>1<81>0<86>1
	cardinfo->SD_csd.SysSpecVersion=(tmp&0x3C)>>2;  //2.0<81>0<88>4<81>0<85>2<81>0<88>6¨¦<81>0<86>3<81>0<86>1<81>0<87>1<81>0<
	cardinfo->SD_csd.Reserved1=tmp&0x03;      //2<81>0<86>0<81>0<8b>2¡À<81>0<84>5<81>0<86>9<81>0<8b>0<81>0<88>2<81>0<86>3  
	tmp=(u8)((CSD_Tab[0]&0x00FF0000)>>16);      //<81>0<85>8<81>0<89>31<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3
	cardinfo->SD_csd.TAAC=tmp;              //<81>0<87>8<81>0<8b>5<81>0<86>6<81>0<89>6<81>0<85>9<81>0<86>9<81>0<87>8¡À<81>0<8
	tmp=(u8)((CSD_Tab[0]&0x0000FF00)>>8);       //<81>0<85>8<81>0<89>32<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3
	cardinfo->SD_csd.NSAC=tmp;              //<81>0<87>8<81>0<8b>5<81>0<86>6<81>0<89>6<81>0<85>9<81>0<86>9<81>0<87>8¡À<81>0<8
	tmp=(u8)(CSD_Tab[0]&0x000000FF);        //<81>0<85>8<81>0<89>33<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3
	cardinfo->SD_csd.MaxBusClkFrec=tmp;         //<81>0<85>7<81>0<85>0<81>0<87>8<81>0<8a>1<81>0<87>9<81>0<89>2<81>0<85>9<81>0
	tmp=(u8)((CSD_Tab[1]&0xFF000000)>>24);      //<81>0<85>8<81>0<89>34<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3
	cardinfo->SD_csd.CardComdClasses=tmp<<4;      //<81>0<86>7¡§<81>0<89>0<81>0<86>0<81>0<86>9<81>0<8a>6<81>0<86>8¨¤<81>0<86>
	tmp=(u8)((CSD_Tab[1]&0x00FF0000)>>16);      //<81>0<85>8<81>0<89>35<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3
	cardinfo->SD_csd.CardComdClasses|=(tmp&0xF0)>>4;//<81>0<86>7¡§<81>0<89>0<81>0<86>0<81>0<86>9<81>0<8a>6<81>0<86>8¨¤<81>0<8
	cardinfo->SD_csd.RdBlockLen=tmp&0x0F;       //¡Á<81>0<8a>6<81>0<85>7¨®<81>0<85>9<81>0<86>9<81>0<87>6<81>0<84>3<81>0<87>8<
	tmp=(u8)((CSD_Tab[1]&0x0000FF00)>>8);     //<81>0<85>8<81>0<89>36<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3
	cardinfo->SD_csd.PartBlockRead=(tmp&0x80)>>7; //<81>0<88>8<81>0<87>8<81>0<88>4¨ª¡¤<81>0<89>0<81>0<86>7¨¦<81>0<85>9<81>0<8
	cardinfo->SD_csd.WrBlockMisalign=(tmp&0x40)>>6; //<81>0<88>4<81>0<85>7<81>0<86>7¨¦<81>0<85>7¨ª<81>0<88>2<81>0<86>3
	cardinfo->SD_csd.RdBlockMisalign=(tmp&0x20)>>5; //<81>0<85>9<81>0<86>9<81>0<86>7¨¦<81>0<85>7¨ª<81>0<88>2<81>0<86>3
	cardinfo->SD_csd.DSRImpl=(tmp&0x10)>>4;
	cardinfo->SD_csd.Reserved2=0;           //¡À<81>0<84>5<81>0<86>9<81>0<8b>0
	if((CardType==SDIO_STD_CAPACITY_SD_CARD_V1_1)||(CardType==SDIO_STD_CAPACITY_SD_CARD_V2_0)||(SDIO_MULTIMEDIA_CARD==CardTyp
				{
				cardinfo->SD_csd.DeviceSize=(tmp&0x03)<<10; //C_SIZE(12<81>0<88>2<81>0<86>3)
				tmp=(u8)(CSD_Tab[1]&0x000000FF);      //<81>0<85>8<81>0<89>37<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3 
				cardinfo->SD_csd.DeviceSize|=(tmp)<<2;
				tmp=(u8)((CSD_Tab[2]&0xFF000000)>>24);    //<81>0<85>8<81>0<89>38<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3 
				cardinfo->SD_csd.DeviceSize|=(tmp&0xC0)>>6;
				cardinfo->SD_csd.MaxRdCurrentVDDMin=(tmp&0x38)>>3;
				cardinfo->SD_csd.MaxRdCurrentVDDMax=(tmp&0x07);
				tmp=(u8)((CSD_Tab[2]&0x00FF0000)>>16);    //<81>0<85>8<81>0<89>39<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3 
				cardinfo->SD_csd.MaxWrCurrentVDDMin=(tmp&0xE0)>>5;
				cardinfo->SD_csd.MaxWrCurrentVDDMax=(tmp&0x1C)>>2;
				cardinfo->SD_csd.DeviceSizeMul=(tmp&0x03)<<1;//C_SIZE_MULT
				tmp=(u8)((CSD_Tab[2]&0x0000FF00)>>8);     //<81>0<85>8<81>0<89>310<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3 
				cardinfo->SD_csd.DeviceSizeMul|=(tmp&0x80)>>7;
				cardinfo->CardCapacity=(cardinfo->SD_csd.DeviceSize+1);//<81>0<86>4<81>0<87>4<81>0<87>9<81>0<8a>0<81>0<86>7¡§<81>0<87>6
				cardinfo->CardCapacity*=(1<<(cardinfo->SD_csd.DeviceSizeMul+2));
				cardinfo->CardBlockSize=1<<(cardinfo->SD_csd.RdBlockLen);//<81>0<86>7¨¦<81>0<85>7¨®<81>0<88>4<81>0<84>3
				cardinfo->CardCapacity*=cardinfo->CardBlockSize;
				}else if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)  //<81>0<86>0<81>0<89>8<81>0<87>6<81>0<89>6<81>0<86>9<81>0<86>7<81>0<86>7¡
				{
					tmp=(u8)(CSD_Tab[1]&0x000000FF);    //<81>0<85>8<81>0<89>37<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3 
					cardinfo->SD_csd.DeviceSize=(tmp&0x3F)<<16;//C_SIZE
					tmp=(u8)((CSD_Tab[2]&0xFF000000)>>24);  //<81>0<85>8<81>0<89>38<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3 
					cardinfo->SD_csd.DeviceSize|=(tmp<<8);
					tmp=(u8)((CSD_Tab[2]&0x00FF0000)>>16);  //<81>0<85>8<81>0<89>39<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3 
					cardinfo->SD_csd.DeviceSize|=(tmp);
					tmp=(u8)((CSD_Tab[2]&0x0000FF00)>>8);   //<81>0<85>8<81>0<89>310<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3  
					cardinfo->CardCapacity=(long long)(cardinfo->SD_csd.DeviceSize+1)*512*1024;//<81>0<86>4<81>0<87>4<81>0<87>9<81>0<8a>0<8
					cardinfo->CardBlockSize=512;      //<81>0<86>7¨¦<81>0<85>7¨®<81>0<88>4<81>0<84>3<81>0<86>1<81>0<88>0<81>0<85>9¡§<81>0<8
				}
	cardinfo->SD_csd.EraseGrSize=(tmp&0x40)>>6;
	cardinfo->SD_csd.EraseGrMul=(tmp&0x3F)<<1;
	tmp=(u8)(CSD_Tab[2]&0x000000FF);      //<81>0<85>8<81>0<89>311<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3  
	cardinfo->SD_csd.EraseGrMul|=(tmp&0x80)>>7;
	cardinfo->SD_csd.WrProtectGrSize=(tmp&0x7F);
	tmp=(u8)((CSD_Tab[3]&0xFF000000)>>24);    //<81>0<85>8<81>0<89>312<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3  
	cardinfo->SD_csd.WrProtectGrEnable=(tmp&0x80)>>7;
	cardinfo->SD_csd.ManDeflECC=(tmp&0x60)>>5;
	cardinfo->SD_csd.WrSpeedFact=(tmp&0x1C)>>2;
	cardinfo->SD_csd.MaxWrBlockLen=(tmp&0x03)<<2;
	tmp=(u8)((CSD_Tab[3]&0x00FF0000)>>16);    //<81>0<85>8<81>0<89>313<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3
	cardinfo->SD_csd.MaxWrBlockLen|=(tmp&0xC0)>>6;
	cardinfo->SD_csd.WriteBlockPaPartial=(tmp&0x20)>>5;
	cardinfo->SD_csd.Reserved3=0;
	cardinfo->SD_csd.ContentProtectAppli=(tmp&0x01);
	tmp=(u8)((CSD_Tab[3]&0x0000FF00)>>8);   //<81>0<85>8<81>0<89>314<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3
	cardinfo->SD_csd.FileFormatGrouop=(tmp&0x80)>>7;
	cardinfo->SD_csd.CopyFlag=(tmp&0x40)>>6;
	cardinfo->SD_csd.PermWrProtect=(tmp&0x20)>>5;
	cardinfo->SD_csd.TempWrProtect=(tmp&0x10)>>4;
	cardinfo->SD_csd.FileFormat=(tmp&0x0C)>>2;
	cardinfo->SD_csd.ECC=(tmp&0x03);
	tmp=(u8)(CSD_Tab[3]&0x000000FF);      //<81>0<85>8<81>0<89>315<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3
	cardinfo->SD_csd.CSD_CRC=(tmp&0xFE)>>1;
	cardinfo->SD_csd.Reserved4=1;
	tmp=(u8)((CID_Tab[0]&0xFF000000)>>24);    //<81>0<85>8<81>0<89>30<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3
	cardinfo->SD_cid.ManufacturerID=tmp;
	tmp=(u8)((CID_Tab[0]&0x00FF0000)>>16);    //<81>0<85>8<81>0<89>31<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3
	cardinfo->SD_cid.OEM_AppliID=tmp<<8;
	tmp=(u8)((CID_Tab[0]&0x000000FF00)>>8);   //<81>0<85>8<81>0<89>32<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3
	cardinfo->SD_cid.OEM_AppliID|=tmp;
	tmp=(u8)(CID_Tab[0]&0x000000FF);      //<81>0<85>8<81>0<89>33<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3 
	cardinfo->SD_cid.ProdName1=tmp<<24;
	tmp=(u8)((CID_Tab[1]&0xFF000000)>>24);    //<81>0<85>8<81>0<89>34<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3
	cardinfo->SD_cid.ProdName1|=tmp<<16;
	tmp=(u8)((CID_Tab[1]&0x00FF0000)>>16);      //<81>0<85>8<81>0<89>35<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3
	cardinfo->SD_cid.ProdName1|=tmp<<8;
	tmp=(u8)((CID_Tab[1]&0x0000FF00)>>8);   //<81>0<85>8<81>0<89>36<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3
	cardinfo->SD_cid.ProdName1|=tmp;
	tmp=(u8)(CID_Tab[1]&0x000000FF);        //<81>0<85>8<81>0<89>37<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3
	cardinfo->SD_cid.ProdName2=tmp;
	tmp=(u8)((CID_Tab[2]&0xFF000000)>>24);    //<81>0<85>8<81>0<89>38<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3
	cardinfo->SD_cid.ProdRev=tmp;
	tmp=(u8)((CID_Tab[2]&0x00FF0000)>>16);    //<81>0<85>8<81>0<89>39<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3
	cardinfo->SD_cid.ProdSN=tmp<<24;
	tmp=(u8)((CID_Tab[2]&0x0000FF00)>>8);     //<81>0<85>8<81>0<89>310<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3
	cardinfo->SD_cid.ProdSN|=tmp<<16;
	tmp=(u8)(CID_Tab[2]&0x000000FF);        //<81>0<85>8<81>0<89>311<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3
	cardinfo->SD_cid.ProdSN|=tmp<<8;
	tmp=(u8)((CID_Tab[3]&0xFF000000)>>24);    //<81>0<85>8<81>0<89>312<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3
	cardinfo->SD_cid.ProdSN|=tmp;
	tmp=(u8)((CID_Tab[3]&0x00FF0000)>>16);    //<81>0<85>8<81>0<89>313<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3
	cardinfo->SD_cid.Reserved1|=(tmp&0xF0)>>4;
	cardinfo->SD_cid.ManufactDate=(tmp&0x0F)<<8;
	tmp=(u8)((CID_Tab[3]&0x0000FF00)>>8);   //<81>0<85>8<81>0<89>314<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3
	cardinfo->SD_cid.ManufactDate|=tmp;
	tmp=(u8)(CID_Tab[3]&0x000000FF);      //<81>0<85>8<81>0<89>315<81>0<86>0<81>0<8b>2¡Á<81>0<89>0<81>0<86>5<81>0<89>3
	cardinfo->SD_cid.CID_CRC=(tmp&0xFE)>>1;
	cardinfo->SD_cid.Reserved2=1;
	*/
	return errorstatus;

}

SD_API_STATUS SendCommand(short Cmd, int Arg, short respType, BOOL bDataTransfer)
{
	unsigned int uiNewCmdRegVal = 0;
	int dwWaitCount = 0;
	S3C2440_SDI* sdi = S3C2440_GetBase_SDI();
	sdi->SDICmdArg = Arg;

	printf("SendCommand (0x%04x, 0x%08x, 0x%04x, 0x%x) starts\r\n",                       
				Cmd, Arg, respType, bDataTransfer);
	//----- 1. Reset any pending status flags -----
	sdi->SDICmdSta = (CRC_CHECK_FAILED | COMMAND_SENT | COMMAND_TIMED_OUT | RESPONSE_RECEIVED);                       

	//----- 2. Specify the command's argument -----                                                                        
	sdi->SDICmdArg = Arg;

	//----- 3. Specify the command's data transfer requirements -----                                                      
	if(bDataTransfer == TRUE)
	{
		sdi->SDICmdCon  |= SDIO_COMMAND_WITH_DATA;                                                                    
	}else
	{
		sdi->SDICmdCon  &= ~SDIO_COMMAND_WITH_DATA;                                                                   
	}

	//----- 4. Send the command to the MMC controller -----                                                                
	switch(respType)  
	{
		case NoResponse:                // Response is not required, but make sure the command was sent
			printf("SendCommand no response required\r\n");
			sdi->SDICmdCon = START_COMMAND | COMMAND_START_BIT | (Cmd & MAX_CMD_VALUE);

			while(!(sdi->SDICmdSta & COMMAND_SENT))
			{
				dwWaitCount++;
				if( dwWaitCount > WAIT_TIME )
				{
					printf("SDHCD:SendCommand() - timeout waiting for command completion!\r\n");
					return SD_API_STATUS_RESPONSE_TIMEOUT;
				}

				/*
				if( !IsCardPresent() )
				{
					printf("SDHCD:SendCommand() - Card ejected!\r\n");
					return SD_API_STATUS_DEVICE_REMOVED;
				}
				*/
				if(sdi->SDICmdSta & COMMAND_TIMED_OUT)
				{
					printf("SDHCD:SendCommand() - Command 0x%04x timed out!\r\n", Cmd);
					sdi->SDICmdSta = COMMAND_TIMED_OUT;                // Clear the error
					return SD_API_STATUS_RESPONSE_TIMEOUT;
				}
			}
			sdi->SDICmdSta = COMMAND_SENT;                         // Clear the status 
			break;

		case ResponseR1:                // Short response required
		case ResponseR1b:
		case ResponseR3:
		case ResponseR4:
		case ResponseR5:
		case ResponseR6:
		case ResponseR7:
			printf("sendSDICommand short response required\r\n");
			//      sdi->SDICmdCon = uiNewCmdRegVal | WAIT_FOR_RESPONSE | START_COMMAND | COMMAND_START_BIT | (Cmd & MAX_CMD_VALUE);
			sdi->SDICmdCon = WAIT_FOR_RESPONSE | START_COMMAND | COMMAND_START_BIT | (Cmd & MAX_CMD_VALUE);
			break;

		case ResponseR2:                // Long response required       
			printf("sendSDICommand long response required\r\n");
			//      sdi->SDICmdCon = uiNewCmdRegVal | LONG_RESPONSE | WAIT_FOR_RESPONSE | START_COMMAND | COMMAND_START_BIT | (Cmd & MAX_CMD_VALUE);
			sdi->SDICmdCon =  LONG_RESPONSE | WAIT_FOR_RESPONSE | START_COMMAND | COMMAND_START_BIT | (Cmd & MAX_CMD_VALUE);
			break;

		default:
			printf("SDHCD:sendSDICommand() - Invalid response type.  Command not sent!\r\n");
			return SD_API_STATUS_NOT_IMPLEMENTED;
			break;
	}

	return SD_API_STATUS_SUCCESS;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Function:       CSDIOControllerBase::GetCommandResponse()

Description:    Retrieves the response info for the last SDI command
issues.

Notes:          This routine assumes that the caller has already locked
the current request and checked for errors.

Returns:        SD_API_STATUS status code.
-------------------------------------------------------------------*/
//SD_API_STATUS GetCommandResponse(PSD_BUS_REQUEST pRequest)
SD_API_STATUS GetCommandResponse(unsigned char* pRequest, int Response)
{
	printf("GetCommandResponse started\r\n");
	unsigned char *              respBuff = pRequest;       // response buffer
	int dwWaitCount = 0;
	S3C2440_SDI* sdi = S3C2440_GetBase_SDI();

	//----- 1. Wait for the response information to get arrive at the controller -----
	while(!(sdi->SDICmdSta & RESPONSE_RECEIVED))
	{
		dwWaitCount++;
		if( dwWaitCount > WAIT_TIME )
		{
			printf("SDHCD:GetCommandResponse() - timeout waiting for command response!\r\n");
			return SD_API_STATUS_RESPONSE_TIMEOUT;
		}

		/*
		if( !IsCardPresent() )
		{
			printf("SDHCD:GetCommandResponse() - Card ejected!\r\n");
			return SD_API_STATUS_DEVICE_REMOVED;
		}
		*/
		if(sdi->SDICmdSta & COMMAND_TIMED_OUT)
		{
			sdi->SDICmdSta = COMMAND_TIMED_OUT;                // Clear the error
			printf("GetCommandResponse returned SD_API_STATUS_RESPONSE_TIMEOUT (COMMAND_TIMED_OUT)\r\n");
			return SD_API_STATUS_RESPONSE_TIMEOUT;
		}

		if(sdi->SDIDatSta & CRC_CHECK_FAILED)
		{
			sdi->SDIDatSta = CRC_CHECK_FAILED;             // Clear the error
			printf("GetCommandResponse returned SD_API_STATUS_CRC_ERROR (CRC_CHECK_FAILED)\r\n");
			return SD_API_STATUS_CRC_ERROR;
		}

		if(sdi->SDIDatSta & DATA_TRANSMIT_CRC_ERROR)
		{
			sdi->SDIDatSta = DATA_TRANSMIT_CRC_ERROR;      // Clear the error
			printf("getSDICommandResponse returned SD_API_STATUS_CRC_ERROR (DATA_TRANSMIT_CRC_ERROR)\r\n");
			return SD_API_STATUS_CRC_ERROR;
		}

		if(sdi->SDIDatSta & DATA_RECEIVE_CRC_ERROR)
		{
			sdi->SDIDatSta = DATA_RECEIVE_CRC_ERROR;           // Clear the error
			printf("GetCommandResponse returned SD_API_STATUS_CRC_ERROR (DATA_RECEIVE_CRC_ERROR)\r\n");
			return SD_API_STATUS_CRC_ERROR;
		}

		if(sdi->SDIDatSta & DATA_TIME_OUT)
		{
			sdi->SDIDatSta = DATA_TIME_OUT;                    // Clear the error
			printf("GetCommandResponse returned SD_API_STATUS_DATA_TIMEOUT (DATA_TIME_OUT)\r\n");
			return SD_API_STATUS_DATA_TIMEOUT;
		}
	}
	sdi->SDICmdSta = RESPONSE_RECEIVED;                        // Clear the status 


	//----- 2. Copy the response information to our "response buffer" -----
	//         NOTE: All START_BIT and TRANSMISSION_BIT bits ='0'. All END_BIT bits ='0'. All RESERVED bits ='1'
	//respBuff = pRequest->CommandResponse.ResponseBuffer;
	//switch(pRequest->CommandResponse.ResponseType)
	switch(Response)
	{
		case NoResponse:
			break;

		case ResponseR1:                
		case ResponseR1b:
		case ResponseR7:
			//--- SHORT RESPONSE (48 bits total)--- 
			// Format: { START_BIT(1) | TRANSMISSION_BIT(1) | COMMAND_INDEX(6) | CARD_STATUS(32) | CRC7(7) | END_BIT(1) }
			// NOTE: START_BIT and TRANSMISSION_BIT = 0, END_BIT = 1
			//
			*(respBuff    ) = 11;//(BYTE)(START_BIT | TRANSMISSION_BIT | pRequest->CommandCode);
			*(respBuff + 1) = (BYTE)(sdi->SDIRSP0      );
			*(respBuff + 2) = (BYTE)(sdi->SDIRSP0 >> 8 );
			*(respBuff + 3) = (BYTE)(sdi->SDIRSP0 >> 16);
			*(respBuff + 4) = (BYTE)(sdi->SDIRSP0 >> 24);
			*(respBuff + 5) = (BYTE)(END_RESERVED | END_BIT);
			break;

		case ResponseR3:
		case ResponseR4:
			//--- SHORT RESPONSE (48 bits total)--- 
			// Format: { START_BIT(1) | TRANSMISSION_BIT(1) | RESERVED(6) | CARD_STATUS(32) | RESERVED(7) | END_BIT(1) }
			//
			*(respBuff    ) = (BYTE)(START_BIT | TRANSMISSION_BIT | START_RESERVED);
			*(respBuff + 1) = (BYTE)(sdi->SDIRSP0      );
			*(respBuff + 2) = (BYTE)(sdi->SDIRSP0 >> 8 );
			*(respBuff + 3) = (BYTE)(sdi->SDIRSP0 >> 16);
			*(respBuff + 4) = (BYTE)(sdi->SDIRSP0 >> 24);
			*(respBuff + 5) = (BYTE)(END_RESERVED | END_BIT);
			break;

		case ResponseR5:
		case ResponseR6:
			//--- SHORT RESPONSE (48 bits total)--- 
			// Format: { START_BIT(1) | TRANSMISSION_BIT(1) | COMMAND_INDEX(6) | RCA(16) | CARD_STATUS(16) | CRC7(7) | END_BIT(1) }
			//
			*(respBuff    ) = 11;//(BYTE)(START_BIT | TRANSMISSION_BIT | pRequest->CommandCode);
			*(respBuff + 1) = (BYTE)(sdi->SDIRSP0      );
			*(respBuff + 2) = (BYTE)(sdi->SDIRSP0 >> 8 );
			*(respBuff + 3) = (BYTE)(sdi->SDIRSP0 >> 16);
			*(respBuff + 4) = (BYTE)(sdi->SDIRSP0 >> 24);
			*(respBuff + 5) = (BYTE)(sdi->SDIRSP1 >> 24);
			break;

		case ResponseR2:
			//--- LONG RESPONSE (136 bits total)--- 
			// Format: { START_BIT(1) | TRANSMISSION_BIT(1) | RESERVED(6) | CARD_STATUS(127) | END_BIT(1) }
			//
			// NOTE: In this implementation, the caller doesn't require the upper 8 bits of reserved data.
			//       Consequently, these bits aren't included and the response info is copied directly into
			//       the beginning of the supplied buffer.      
			//
			//*(respBuff    )   = (BYTE)(START_BIT | TRANSMISSION_BIT | START_RESERVED);
			//
			*(respBuff + 0) = (BYTE)(sdi->SDIRSP3      );
			*(respBuff + 1) = (BYTE)(sdi->SDIRSP3 >> 8 );
			*(respBuff + 2) = (BYTE)(sdi->SDIRSP3 >> 16);
			*(respBuff + 3) = (BYTE)(sdi->SDIRSP3 >> 24);

			*(respBuff + 4) = (BYTE)(sdi->SDIRSP2      );
			*(respBuff + 5) = (BYTE)(sdi->SDIRSP2 >> 8 );
			*(respBuff + 6) = (BYTE)(sdi->SDIRSP2 >> 16);
			*(respBuff + 7) = (BYTE)(sdi->SDIRSP2 >> 24);

			*(respBuff + 8) = (BYTE)(sdi->SDIRSP1      );
			*(respBuff + 9) = (BYTE)(sdi->SDIRSP1 >> 8 );
			*(respBuff + 10)= (BYTE)(sdi->SDIRSP1 >> 16);
			*(respBuff + 11)= (BYTE)(sdi->SDIRSP1 >> 24);

			*(respBuff + 12)= (BYTE)(sdi->SDIRSP0      );
			*(respBuff + 13)= (BYTE)(sdi->SDIRSP0 >> 8 );
			*(respBuff + 14)= (BYTE)(sdi->SDIRSP0 >> 16);
			*(respBuff + 15)= (BYTE)(sdi->SDIRSP0 >> 24);
			break;

		default:
			printf("SDHCD:GetCmdResponse(): Unrecognized response type!\r\n");
			break;
	}

	printf("GetCommandResponse returned SD_API_STATUS_SUCCESS\r\n");
	return SD_API_STATUS_SUCCESS;
	//-path "$ROOT/*/*"                                                 -o 
}

int Chk_CMDend(int cmd, int be_resp)
	//0: Timeout
{
	int finish0;
	S3C2440_SDI* sdi = S3C2440_GetBase_SDI();

	if(!be_resp)    // No response
	{
		finish0 = sdi->SDICmdSta;
		while((finish0&0x800)!=0x800) // Check cmd end
			finish0 = sdi->SDICmdSta;
		sdi->SDICmdSta = finish0;// Clear cmd end state

		return 1;
	}
	else  // With response
	{
		finish0=sdi->SDICmdSta;
		while( !( ((finish0&0x200)==0x200) | ((finish0&0x400)==0x400) ))    // Check cmd/rsp end
			finish0=sdi->SDICmdSta;

		if(cmd==1 | cmd==41)  // CRC no check, CMD9 is a long Resp. command.
		{
			if( (finish0&0xf00) != 0xa00 )  // Check error
			{
				sdi->SDICmdSta=finish0;   // Clear error state

				if(((finish0&0x400)==0x400))
					return 0; // Timeout error
			}
			sdi->SDICmdSta=finish0; // Clear cmd & rsp end state
		}
		else  // CRC check
		{
			if( (finish0&0x1f00) != 0xa00 ) // Check error
			{
				printf("CMD%d:sdi->SDICmdSta=0x%x, rSDIRSP0=0x%x\n",cmd, sdi->SDICmdSta, sdi->SDIRSP0);
				sdi->SDICmdSta=finish0;   // Clear error state

				if(((finish0&0x400)==0x400))
					return 0; // Timeout error
			}
			sdi->SDICmdSta=finish0;
		}
		return 1;
	}
}


int sdio_cmdResponseSuccess()
{
	int nCount = 0;
	int bFlag = 1;

	S3C2440_SDI* sdi = S3C2440_GetBase_SDI();

	int temp = sdi->SDICmdSta;
	while( !(((temp & 0x200) == 0x200) || ((temp & 0x400) == 0x400)) )
		temp = sdi->SDICmdSta;
	/*
	{
		temp = sdi->SDICmdSta;
		if (nCount++ > 200)
		{
			bFlag = 0;
			printf("bFlag\n");
			break;
		}
	};
	*/
	printf("temp is %x\n", temp);
	sdi->SDICmdSta = 0xA00;
	return bFlag;
}

int sdio_cmdSendSuccess()
{
	int nCount = 0;
	int bFlag = 1;

	S3C2440_SDI* sdi = S3C2440_GetBase_SDI();
	int temp = sdi->SDICmdSta;
	while( (temp & 0x800) != 0x800)
	{
		temp = sdi->SDICmdSta;
		if (nCount++ > WAIT_TIME)
		{
			bFlag = 0;
			break;
		}
	};
	sdi->SDICmdSta = temp;
	sdi->SDICmdSta = 0x800;
	return bFlag;
}

void SDIO_Send_Cmd(u8 cmdindex,u8 waitrsp,u32 arg)
{     
	u32 tmpreg;
	S3C2440_SDI* sdi = S3C2440_GetBase_SDI();
	sdi->SDICmdArg = arg;
	tmpreg|=1<<10;         //
	sdi->SDICmdCon=(1<<8) | 0x40 | cmdindex;

	while(sdi->SDICmdSta & 0x800);
}

SD_Error CmdError(void)
{
	SD_Error errorstatus = SD_OK;
	/*
	u32 timeout= 100; //SDIO_CMD0TIMEOUT;

	S3C2440_SDI* sdi = S3C2440_GetBase_SDI();
	sdi->SDICmdArg = arg;
	while(timeout--)
	{
		if(sdi->SDIDatSta & (1<<7))break;  //ÃüÁîÒÑ·¢ËÍ(ÎÞÐèÏìÓ¦)   
	}
	if(timeout==0)return SD_CMD_RSP_TIMEOUT;
	SDIO->SDIDatSta
		ICR=0X5FF;        //Çå³ý±ê¼Ç
		*/
	return errorstatus;
}

