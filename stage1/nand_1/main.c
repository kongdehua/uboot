#include "string.h"
#include "command.h"

#include "standard.h"
#include "s3c2440.h"
#include "global_data.h"

#include "timer.h"
#include "serial.h"
#include "led.h"
#include "vsprintf.h"

DECLARE_GLOBAL_DATA_PTR;

void init_baudrate()
{
	gd->baudrate = CONFIG_BAUDRATE;
}

int had_ctrlc (void);
/* test if ctrl-c was pressed */
static int ctrlc_disabled = 0;  /* see disable_ctrl() */
static int ctrlc_was_pressed = 0;

int parse_line (char *line, char *argv[])
{
  int nargs = 0;

#ifdef DEBUG_PARSER
  printf ("parse_line: \"%s\"\n", line);
#endif
  while (nargs < CFG_MAXARGS) {
      
    /* skip any white space */
    while ((*line == ' ') || (*line == '\t')) {
      ++line;
    }

    if (*line == '\0') {  /* end of line, no more args  */
      argv[nargs] = NULL;
#ifdef DEBUG_PARSER
    printf ("parse_line: nargs=%d\n", nargs);
#endif
      return (nargs);
    }

    argv[nargs++] = line; /* begin of argument string */

    /* find end of string */
    while (*line && (*line != ' ') && (*line != '\t')) {
      ++line;
    }

    if (*line == '\0') {  /* end of line, no more args  */
      argv[nargs] = NULL;
#ifdef DEBUG_PARSER
    printf ("parse_line: nargs=%d\n", nargs);
#endif
      return (nargs);
    }

    *line++ = '\0';   /* terminate current arg   */
  }

  printf ("** Too many args (max. %d) **\n", CFG_MAXARGS);

#ifdef DEBUG_PARSER
  printf ("parse_line: nargs=%d\n", nargs);
#endif
  return (nargs);
}

/***************************************************************************
 * find command table entry for a command
 */
cmd_tbl_t *find_cmd (const char *cmd)
{     
  cmd_tbl_t *cmdtp;
  cmd_tbl_t *cmdtp_temp = &__u_boot_cmd_start;  /*Init value */
  const char *p;
  int len;
  int n_found = 0;
      
  /*  
   * Some commands allow length modifiers (like "cp.b");
   * compare command name only until first dot.
   */
  len = ((p = strchr(cmd, '.')) == NULL) ? strlen (cmd) : (p - cmd);
    
  for (cmdtp = &__u_boot_cmd_start;
       cmdtp != &__u_boot_cmd_end;
       cmdtp++) {
    if (strncmp (cmd, cmdtp->name, len) == 0) {
      if (len == strlen (cmdtp->name))
        return cmdtp; /* full match */

      cmdtp_temp = cmdtp; /* abbreviated command ? */
      n_found++;
    }
  }
  if (n_found == 1) {     /* exactly one match */
    return cmdtp_temp;
  }

  return NULL;  /* not found or ambiguous command */
}


/****************************************************************************                                                          
 * returns:
 *  1  - command executed, repeatable
 *  0  - command executed but not repeatable, interrupted commands are                                                                 
 *       always considered not repeatable
 *  -1 - not executed (unrecognized, bootd recursion or too many args)
 *           (If cmd is NULL or "" or longer than CFG_CBSIZE-1 it is
 *           considered unrecognized)
 *  
 * WARNING:
 *  
 * We must create a temporary copy of the command since the command we get                                                             
 * may be the result from getenv(), which returns a pointer directly to                                                                
 * the environment data, which may change magicly when the command we run
 * creates or modifies environment variables (like "bootp" does).
 */ 
    
int run_command (const char *cmd, int flag)
{
  cmd_tbl_t *cmdtp;
  char cmdbuf[CFG_CBSIZE];  /* working copy of cmd    */

  char *token;      /* start of token in cmdbuf */
  char *sep;      /* end of token (separator) in cmdbuf */  
  char finaltoken[CFG_CBSIZE]; 
  char *str = cmdbuf;          
  char *argv[CFG_MAXARGS + 1];  /* NULL terminated  */
  int argc, inquotes;          
  int repeatable = 1;
  int rc = 0;

  //clear_ctrlc();    /* forget any previous Control C */

  if (!cmd || !*cmd) {
    return -1;  /* empty command */
  }

  if (strlen(cmd) >= CFG_CBSIZE) {
    puts ("## Command too long!\n");
    return -1;
  }

  strcpy (cmdbuf, cmd);

  /* Process separators and check for invalid
   * repeatable commands
   */

#ifdef DEBUG_PARSER
  printf ("[PROCESS_SEPARATORS] %s\n", cmd);
#endif
  while (*str) {

    /*
     * Find separator, or string end
     * Allow simple escape of ';' by writing "\;"
     */
    for (inquotes = 0, sep = str; *sep; sep++) {
      if ((*sep=='\'') &&
          (*(sep-1) != '\\'))
        inquotes=!inquotes;

      if (!inquotes &&
          (*sep == ';') &&  /* separator    */
          ( sep != str) &&  /* past string start  */
          (*(sep-1) != '\\')) /* and NOT escaped  */
        break;
    }

    /*
     * Limit the token to data between separators
     */
    token = str;
    if (*sep) {
      str = sep + 1;  /* start of command for next pass */
      *sep = '\0';
    }
    else
      str = sep;  /* no more commands for next pass */
#ifdef DEBUG_PARSER
    printf ("token: \"%s\"\n", token);
#endif

    /* find macros in this token and replace them */
    //process_macros (token, finaltoken);

    /* Extract arguments */
    if ((argc = parse_line (finaltoken, argv)) == 0) {
      rc = -1;  /* no command at all */
      continue;
    }

    /* Look up command in command table */
    if ((cmdtp = find_cmd(argv[0])) == NULL) {
      printf ("Unknown command '%s' - try 'help'\n", argv[0]);
      rc = -1;  /* give up after bad command */
      continue;
    }

    /* found - check max args */
    if (argc > cmdtp->maxargs) {
      printf ("Usage:\n%s\n", cmdtp->usage);
      rc = -1;
      continue;
    }

#if (CONFIG_COMMANDS & CFG_CMD_BOOTD)
    /* avoid "bootd" recursion */
    if (cmdtp->cmd == do_bootd) {
#ifdef DEBUG_PARSER
      printf ("[%s]\n", finaltoken);
#endif
      if (flag & CMD_FLAG_BOOTD) {
        puts ("'bootd' recursion detected\n");
        rc = -1;
        continue;
      } else {
        flag |= CMD_FLAG_BOOTD;
      }
    }
#endif  /* CFG_CMD_BOOTD */

    /* OK - call function to do the command */
    if ((cmdtp->cmd) (cmdtp, flag, argc, argv) != 0) {
      rc = -1;
    }

    repeatable &= cmdtp->repeatable;

    /* Did the user stop this? */
    if (had_ctrlc ())
      return 0; /* if stopped then not repeatable */
  }

  return rc ? rc : repeatable;
}

/* pass 1 to disable ctrlc() checking, 0 to enable.
 * returns previous state
 */
int disable_ctrlc (int disable)
{
  int prev = ctrlc_disabled;  /* save previous state */

  ctrlc_disabled = disable;
  return prev;
} 
  
int had_ctrlc (void)
{ 
  return ctrlc_was_pressed;
} 
  
void clear_ctrlc (void)
{ 
  ctrlc_was_pressed = 0;
} 

char        console_buffer[CFG_CBSIZE];   /* console I/O buffer */

static char * delete_char (char *buffer, char *p, int *colp, int *np, int plen);
static char erase_seq[] = "\b \b";    /* erase sequence */
static char   tab_seq[] = "        ";   /* used to expand TABs  */


/** U-Boot INITIAL CONSOLE-COMPATIBLE FUNCTION *****************************/

int getc (void)                
{
  /* Send directly to the handler */
  return serial_getc ();       
}

void putc (const char c)
{
	/* Send directly to the handler */
	serial_putc (c);
}

static char * delete_char (char *buffer, char *p, int *colp, int *np, int plen)
{
  char *s;

  if (*np == 0) {
    return (p);
  }

  if (*(--p) == '\t') {     /* will retype the whole line */
    while (*colp > plen) {
      puts (erase_seq);
      (*colp)--;
    }
    for (s=buffer; s<p; ++s) {
      if (*s == '\t') {
        puts (tab_seq+((*colp) & 07));
        *colp += 8 - ((*colp) & 07);
      } else {
        ++(*colp);
        putc (*s);
      }
    }
  } else {
    puts (erase_seq);
    (*colp)--;
  }
  (*np)--;
  return (p);
}


/*
 * Prompt for input and read a line.
 * If  CONFIG_BOOT_RETRY_TIME is defined and retry_time >= 0,
 * time out when time goes past endtime (timebase time in ticks).
 * Return:  number of read characters
 *    -1 if break
 *    -2 if timed out
 */
int readline (const char *const prompt)
{
  char   *p = console_buffer;

  int n = 0;        /* buffer index   */
  int plen = 0;     /* prompt length  */
  int col;        /* output column cnt  */
  char  c;

  /* print prompt */
  if (prompt) {
    plen = strlen (prompt);
    puts (prompt);
  }
  col = plen;

  for (;;) {
#ifdef CONFIG_BOOT_RETRY_TIME
    while (!tstc()) { /* while no incoming data */
      if (retry_time >= 0 && get_ticks() > endtime)
        return (-2);  /* timed out */
    }
#endif

#ifdef CONFIG_SHOW_ACTIVITY
    while (!tstc()) {
      extern void show_activity(int arg);
      show_activity(0);
    }
#endif
    c = getc();
    /*
     * Special character handling
     */
    switch (c) {
    case '\r':        /* Enter    */
    case '\n':
      *p = '\0';
      puts ("\r\n");
      return (p - console_buffer);

    case '\0':        /* nul      */
      continue;

    case 0x03:        /* ^C - break   */
      console_buffer[0] = '\0'; /* discard input */
      return (-1);

    case 0x15:        /* ^U - erase line  */
      while (col > plen) {
        puts (erase_seq);
        --col;
      }
      p = console_buffer;
      n = 0;
      continue;

    case 0x17:        /* ^W - erase word  */
      p=delete_char(console_buffer, p, &col, &n, plen);
      while ((n > 0) && (*p != ' ')) {
        p=delete_char(console_buffer, p, &col, &n, plen);
      }
      continue;

    case 0x08:        /* ^H  - backspace  */
    case 0x7F:        /* DEL - backspace  */
      p=delete_char(console_buffer, p, &col, &n, plen);
      continue;

    default:
      /*
       * Must be a normal character then
       */
      if (n < CFG_CBSIZE-2) {
        if (c == '\t') {  /* expand TABs    */
#ifdef CONFIG_AUTO_COMPLETE
          /* if auto completion triggered just continue */
          *p = '\0';
          if (cmd_auto_complete(prompt, console_buffer, &n, &col)) {
            p = console_buffer + n; /* reset */
            continue;
          }
#endif
          puts (tab_seq+(col&07));
          col += 8 - (col&07);
        } else {
          ++col;    /* echo input   */
          putc (c);
        }
        *p++ = c;
        ++n;
      } else {      /* Buffer full    */
        putc ('\a');
      }
    }
  }
}

int main()
{
	unsigned char ch = '\0';
  static char lastcommand[CFG_CBSIZE] = { 0, };
	int flag = 0;

	int i = 0;

	LED1_ENABLE();
	LED1_ON();

	init_baudrate();
	//clock_init();

	initUART();

	int len = 0;
  for (;;) {
    len = readline (CFG_PROMPT);
    if (len == -1)
      puts ("<INTERRUPT>\n");
    else
      run_command (lastcommand, flag);
}

	//printf1("code\n");
	serial_putc('c'); 
	serial_putc('d'); 
	serial_putc('o'); 
	printf("11 is %d\n", 11);
	//printf1("11.1 is %s\n", "kdh");

/*
	ch = '\n';
	serial_putc(ch/10+'0'); 
	serial_putc(ch%10+'0'); 
	serial_putc(' '); 
*/

	//nand_init();

/*
	unsigned char data[2049] = {0};

	nand_read( &data[0], 0, 2048);

	for ( i = 0; i < 20; i++ )
	{
		ch = data[i];
		serial_putc(ch/16 >= 10 ? ch/16 - 10 +'A' : ch/16+'0'); 
		serial_putc(ch%16 >= 10 ? ch%16 - 10 +'A' : ch%16+'0'); 
		serial_putc(' '); 
	}
*/

/*
	LED1_ON();
	init_baudrate();
	clock_init();

	initUART();
	
	serial_putc('c'); 
	serial_putc('o'); 
	serial_putc('d'); 
	serial_putc('e'); 
	serial_putc('\n'); 
	serial_putc('\n'); 

	ch = '\n';
	serial_putc(ch/10+'0'); 
	serial_putc(ch%10+'0'); 
	serial_putc(' '); 

	delay(800000);
	LED1_OFF();
*/

	//printf1("pclk scale is %d\n", get_PCLK());
	
	while(1)
	{
		ch = serial_getc();

		//if (ch <= '9' && ch >= '0')
		if (ch == '\r')
			ch = '\n';
		serial_putc(ch);
	};

	return 0;
};

int raise (int num)
{
	return 0;
}

