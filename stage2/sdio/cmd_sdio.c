#include "standard.h"
#include "vsprintf.h"
#include "string.h"
#include "serial.h"

#include "command.h"
#include "sdio.h"

unsigned short g_sRCA = 0;

//***********************************************************************
int do_sdio_init(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	if (SD_OK == sdio_Initialize())
	{
		printf("init success\n");
	}
	else
	{
		printf("init error\n");
	}
	return 0;
}

U_BOOT_CMD(
	sdinit,	CFG_MAXARGS,	1,	do_sdio_init,
 	"sdinit  - init sdio\n",
 	"[command ...]\n"
 	"    - show help information (for 'command')\n"
 	"'help' prints online help for the monitor commands.\n\n"
 	"Without arguments, it prints a short usage message for all commands.\n\n"
 	"To get detailed help information for specific commands you can type\n"
  "'help' with one or more command names as arguments.\n"
);

//***********************************************************************
int do_sdio_scan(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	if (SD_OK == sdio_ScanCard(&g_sRCA))
	{
		printf("init success\n");
	}
	else
	{
		printf("init error\n");
	}
	return 0;
}

U_BOOT_CMD(
	sdscan,	CFG_MAXARGS,	1,	do_sdio_scan,
 	"sdscan  - scan cards\n",
 	"[command ...]\n"
 	"    - show help information (for 'command')\n"
 	"'help' prints online help for the monitor commands.\n\n"
 	"Without arguments, it prints a short usage message for all commands.\n\n"
 	"To get detailed help information for specific commands you can type\n"
  "'help' with one or more command names as arguments.\n"
);

//***********************************************************************
int do_sdio_select_card(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	if (SD_OK == sdio_SelectCard(g_sRCA))
	{
		printf("select card:%x success\n", g_sRCA);
	}
	else
	{
		printf("select card:%x fail\n", g_sRCA);
	}
	return 0;
}

U_BOOT_CMD(
	sdselect,	CFG_MAXARGS,	1,	do_sdio_select_card,
 	"sdselect  - select cards\n",
 	"[command ...]\n"
 	"    - show help information (for 'command')\n"
 	"'help' prints online help for the monitor commands.\n\n"
 	"Without arguments, it prints a short usage message for all commands.\n\n"
 	"To get detailed help information for specific commands you can type\n"
  "'help' with one or more command names as arguments.\n"
);

//***********************************************************************
int do_sdio_read_data(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	if (argc > 1 && argc != 3) 
		printf("args number error\n");;

	int sector = 0;
	int count = 0;

	unsigned char buf[512*20] = {0};

	if (argc>1)
	{
		sector = simple_strtoul(argv[1], NULL, 16); 
		count  = simple_strtoul(argv[2], NULL, 16); 
	}
	if (SD_OK == sdio_ReadData(buf, sector, count))
	{
		printf("read data success\n");
	}
	else
	{
		printf("read data error\n");
	}
	return 0;
}

U_BOOT_CMD(
	sdread,	CFG_MAXARGS,	1,	do_sdio_read_data,
 	"sdread  - read cards\n",
 	"[command ...]\n"
 	"    - show help information (for 'command')\n"
 	"'help' prints online help for the monitor commands.\n\n"
 	"Without arguments, it prints a short usage message for all commands.\n\n"
 	"To get detailed help information for specific commands you can type\n"
  "'help' with one or more command names as arguments.\n"
);

//***********************************************************************
int do_sdio_write_data(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	if (argc > 1 && argc != 4) 
		printf("args number error\n");;

	int value = 0;
	int sector = 0;
	int count = 0;

	if (argc>1)
	{
		value  = simple_strtoul(argv[1], NULL, 16); 
		sector = simple_strtoul(argv[2], NULL, 16); 
		count  = simple_strtoul(argv[3], NULL, 16); 
	}

	unsigned char buf[512*4] = {0};
	for(int i = 0; i < 512*count/4; i++)
	{
		buf[0 + i*4] = value&0xff;
		buf[1 + i*4] = (value&0xff00)>>8;
		buf[2 + i*4] = (value&0xff0000)>>16;
		buf[3 + i*4] = (value&0xff000000)>>24;
	}


	if (SD_OK == sdio_WriteData(buf, sector, count))
	{
		printf("write data success\n");
	}
	else
	{
		printf("write data fail\n");
	}
	return 0;
}

U_BOOT_CMD(
	sdwrite,	CFG_MAXARGS,	1,	do_sdio_write_data,
 	"sdwrite  - write cards\n",
 	"[command ...]\n"
 	"    - show help information (for 'command')\n"
 	"'help' prints online help for the monitor commands.\n\n"
 	"Without arguments, it prints a short usage message for all commands.\n\n"
 	"To get detailed help information for specific commands you can type\n"
  "'help' with one or more command names as arguments.\n"
);
