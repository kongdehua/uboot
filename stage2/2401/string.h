#ifndef _STRING_H_
#define _STRING_H_

#ifndef NULL
#define NULL 0
#endif

typedef unsigned int    __kernel_size_t;

#ifndef _SIZE_T
#define _SIZE_T
typedef __kernel_size_t   size_t;
#endif 

#ifndef __HAVE_ARCH_STRLEN     
extern __kernel_size_t strlen(const char *);
#endif

#ifndef __HAVE_ARCH_STRCPY
extern char * strcpy(char *,const char *);
#endif

#ifndef __HAVE_ARCH_STRCAT     
extern char * strcat(char *, const char *); 
#endif

#ifndef __HAVE_ARCH_MEMCPY     
extern void * memcpy(void *,const void *,__kernel_size_t);
#endif

#ifndef __HAVE_ARCH_STRRCHR
extern char * strrchr(const char *,int);
#endif

#ifndef __HAVE_ARCH_STRCHR
extern char * strchr(const char *,int);
#endif

#ifndef __HAVE_ARCH_STRCMP     
extern int strcmp(const char *,const char *);
#endif

#ifndef __HAVE_ARCH_STRNCMP
extern int strncmp(const char *,const char *,__kernel_size_t);
#endif

#ifndef __HAVE_ARCH_MEMSET
extern void * memset(void *, int, unsigned int);
#endif

#endif
