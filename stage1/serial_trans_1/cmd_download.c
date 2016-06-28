#include "standard.h"
#include "vsprintf.h"
#include "string.h"
#include "serial.h"

#include "command.h"
#include "xyzModem.h"

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

    return offset;
}

U_BOOT_CMD(
	download,	1,		1,	do_download,
 	"download - download a file from pc\n",
	NULL
);
