#ifndef CONFIG_IDENT_STRING
#define CONFIG_IDENT_STRING ""
#endif

#define U_BOOT_VERSION "U-Boot 1.1.6"
char version_string[] =
  U_BOOT_VERSION" (" __DATE__ " - " __TIME__ ")"CONFIG_IDENT_STRING;

