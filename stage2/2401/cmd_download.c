#include "standard.h"
#include "vsprintf.h"
#include "string.h"
#include "serial.h"

#include "command.h"
#include "xyzModem.h"
#include "fatfs/ff.h"

static int getcxmodem(void) {
    if (tstc())
        return (getc());
    return -1;
}   

int
do_download (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	//printf ("\ndownload by kdh\n");

    int size;
		int offset = 0x32000000;
    char buf[32];
    int err;
    int res;
    connection_info_t info;
    char ymodemBuf[1024];
    ulong store_addr = ~0;
    ulong addr = 0;

    size = 0;
    info.mode = xyzModem_ymodem;
    res = xyzModem_stream_open (&info, &err);
    if (!res) {
        while ((res =
            xyzModem_stream_read (ymodemBuf, 1024, &err)) > 0) {
            store_addr = addr + offset;
            size += res;
            addr += res;
						memcpy ((char *) (store_addr), ymodemBuf, res);
        }
    } else {
        printf ("%s\n", xyzModem_error (err));
    }

    xyzModem_stream_close (&err);
    xyzModem_stream_terminate (false, &getcxmodem);


    //flush_cache (offset, size);

    printf ("## Total Size      = 0x%08x = %d Bytes\n", size, size);
    sprintf (buf, "%X", size);

		const unsigned char *p = (unsigned char *)offset;
		for ( int nIndex = 0; nIndex < size; nIndex++)
		{
			printf("%x ", p[nIndex]);
		}
    //setenv ("filesize", buf);

	printf ("\nend of download\n");
  return offset;
}

U_BOOT_CMD(
	download,	1,		1,	do_download,
 	"download - download a file from pc\n",
	NULL
);

int
do_upload (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	//printf ("\ndownload by kdh\n");
	//setenv ("filesize", buf);
	if (argc != 2)
	{
		printf("args number error!\n");
		return 0;
	}

	const TCHAR* path = argv[1];
	//path[strlen(path) - 1] = '\0';
	//ss = strrchr(path, '/');

	//u16 len = simple_strtoul(argv[2], NULL, 10);

	xyzModem_stream_write(path, 0, NULL);

	printf ("\nend of upload\n");
	return 0;
}

U_BOOT_CMD(
	upload,	4,		1,	do_upload,
 	"upload - upload a file to pc\n",
	NULL
);
