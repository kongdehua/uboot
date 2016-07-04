#include "standard.h"
#include "vsprintf.h"
#include "string.h"
#include "serial.h"

#include "command.h"
#include "fatfs/ff.h"

FATFS file;
DIR dir;				//目录
FILINFO fileinfo;	//文件信息

int do_filesystem_init(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	int res = f_mount(&file,"0:",1);

	if(res==0X0D)//FLASH磁盘,FAT文件系统错误,重新格式化FLASH
	{
		printf("file system of Disk invalid\n");	//格式化完成
		res=f_mkfs("0:",1,4096);//格式化FLASH,1,盘符;1,不需要引导区,8个扇区为1个簇
		if(res==0)
		{
			f_setlabel((const TCHAR *)"0:ALIENTEK");	//设置Flash磁盘的名字为：ALIENTEK
			printf("Flash Disk Format Finish\n");	//格式化完成
		}else printf("Flash Disk Format Error \n");	//格式化失败
	}													    

	printf("file system of Disk init successed!\n");	//格式化完成
	return 0;
}

U_BOOT_CMD(
	fsinit,	CFG_MAXARGS,	1,	do_filesystem_init,
 	"fsinit  - init file system\n",
 	"[command ...]\n"
 	"    - show help information (for 'command')\n"
 	"'help' prints online help for the monitor commands.\n\n"
 	"Without arguments, it prints a short usage message for all commands.\n\n"
 	"To get detailed help information for specific commands you can type\n"
  "'help' with one or more command names as arguments.\n"
);

int do_filesystem_dir(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	int res;
	if (argc != 2)
	{
		printf("argc must = 2\n");
	}
	u8* path = argv[1];
	char *fn;   /* This function is assuming non-Unicode cfg. */

	res = f_opendir(&dir,(const TCHAR*)path); //打开一个目录
	if (res == FR_OK) 
	{	
		printf("\r\n"); 
		while(1)
		{
			res = f_readdir(&dir, &fileinfo);                   //读取目录下的一个文件
			if (res != FR_OK || fileinfo.fname[0] == 0) break;  //错误了/到末尾了,退出
			//if (fileinfo.fname[0] == '.') continue;             //忽略上级目录
#if _USE_LFN
			fn = *fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
#else							   
			fn = fileinfo.fname;
#endif	                                              /* It is a file. */
			printf("%s/", path);//打印路径	
			printf("%s\r\n",  fn);//打印文件名	  
		} 
	}	  
	printf("file system of Disk init successed!\n");	//格式化完成
	return 0;
}

U_BOOT_CMD(
	fsdir,	CFG_MAXARGS,	1,	do_filesystem_dir,
 	"fsdir  - list direcotry \n",
 	"[command ...]\n"
 	"    - show help information (for 'command')\n"
 	"'help' prints online help for the monitor commands.\n\n"
 	"Without arguments, it prints a short usage message for all commands.\n\n"
 	"To get detailed help information for specific commands you can type\n"
  "'help' with one or more command names as arguments.\n"
);

u8 mf_read(u16 len)
{
}

int do_filesystem_read(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("args number error!\n");
		return 0;
	}

	FIL file;
	//u16 len = simple_strtoul(argv[2], NULL, 10);
	const TCHAR* path = argv[1];

	if (FR_OK != f_open(&file,(const TCHAR*)path, FA_READ))//打开文件
	{
		printf("file open failed!\n");
		return 0;
	}

	u16 i,t;
	u8 bFlag = 1;
	u8 res=0;
	u16 tlen=0;
	u8 fatbuf[512] = {0};
	u32 br = 0;

	printf("\r\nRead file data is:\r\n");
	while (!f_eof(&file))
	{
		if (FR_OK != f_read(&file,fatbuf,512,&br))
		{
			printf("Read encounter unknown error!\n");
			bFlag = 0;
			break;
		}

		tlen+=br;
		for(t=0;t<br;t++)printf("%c",fatbuf[t]); 
	}

	f_close(&file);

	if (!bFlag) return 0;

	if(tlen)printf("\r\nReaded data len:%d\r\n",tlen);//读到的数据长度
	return res;
}

U_BOOT_CMD(
	fsread,	CFG_MAXARGS,	1,	do_filesystem_read,
 	"fsread  - read file from disk \n",
 	"[command ...]\n"
 	"    - show help information (for 'command')\n"
 	"'help' prints online help for the monitor commands.\n\n"
 	"Without arguments, it prints a short usage message for all commands.\n\n"
 	"To get detailed help information for specific commands you can type\n"
  "'help' with one or more command names as arguments.\n"
);


int do_filesystem_append(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("args number error!\n");
		return 0;
	}

	FIL file;
	//u16 len = simple_strtoul(argv[2], NULL, 10);
	const TCHAR* path = argv[1];
	const TCHAR* content = argv[2];

	if (FR_OK != f_open(&file,(const TCHAR*)path, FA_WRITE | FA_OPEN_ALWAYS))//打开文件
	{
		printf("file write open failed!\n");
		return 0;
	}
	if (FR_OK != f_lseek(&file, f_size(&file)))
	{
		printf("file seek failed!\n");
		return 0;
	}

	u16 i,t;
	u8 bFlag = 1;
	u8 res=0;
	u16 tlen=0;
	u8 fatbuf[512] = {0};
	u32 br = 0;

	if (EOF != f_puts(content, &file))
		printf("write successful\n");
	else
		printf("write error\n");

	f_close(&file);
	if (!bFlag) return 0;

	return res;
}

U_BOOT_CMD(
	fsappend,	CFG_MAXARGS,	1,	do_filesystem_append,
 	"fsappedn  - append content to file on disk \n",
 	"[command ...]\n"
 	"    - show help information (for 'command')\n"
 	"'help' prints online help for the monitor commands.\n\n"
 	"Without arguments, it prints a short usage message for all commands.\n\n"
 	"To get detailed help information for specific commands you can type\n"
  "'help' with one or more command names as arguments.\n"
);


