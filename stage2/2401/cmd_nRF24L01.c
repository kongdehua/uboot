#include "standard.h"
#include "vsprintf.h"
#include "string.h"
#include "serial.h"

#include "command.h"
#include "NRF24L01.h"

//***********************************************************************
int do_2410_init(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	if (argc !=  2)
	{
		printf("args number error!\n");
		return 0;
	}

	u8 len = simple_strtoul(argv[1], NULL, 10);
	nrf2401_init();

	nrf2401_CE_Enable();
	printf( "write reg is : 0x%x\n", nrf2401_RW_Reg(WRITE_REG + EN_AA, len));
	nrf2401_CE_Disable();

	u8 tmplen =  nrf2401_Read(EN_AA);
	printf( "read  reg is : 0x%x\n", tmplen);
	if (len != tmplen)
	{
		printf("2401 something wrong!\n");
		return 0;
	}
	printf("2401 run right!\n");


	nrf2401_power_off();



	nrf2401_init_tx_mode();
	/*
	nrf2401_CE_Enable();
	printf( "write reg is : 0x%x\n", nrf2401_RW_Reg(WRITE_REG + EN_AA, len));
	nrf2401_CE_Disable();

	printf( "read  reg is : 0x%x\n", nrf2401_Read(EN_AA));
	*/
	printf("init tx mode success!\n");
	return 0;
}

U_BOOT_CMD(
	nrfinit,	CFG_MAXARGS,	1,	do_2410_init,
 	"nrfinit  - init nrf2401\n",
 	"[command ...]\n"
 	"    - show help information (for 'command')\n"
 	"'help' prints online help for the monitor commands.\n\n"
 	"Without arguments, it prints a short usage message for all commands.\n\n"
 	"To get detailed help information for specific commands you can type\n"
  "'help' with one or more command names as arguments.\n"
);
