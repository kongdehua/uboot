#include "string.h"

#ifndef __HAVE_ARCH_STRLEN                                                                              
/**                                                                                                     
 * strlen - Find the length of a string                                                                 
 * @s: The string to be sized                                                                           
 */                                                                                                     
size_t strlen(const char * s)                                                                           
{                                                                                                       
  const char *sc;                                                                                       
                                                                                                        
  for (sc = s; *sc != '\0'; ++sc)                                                                       
    /* nothing */;                                                                                      
  return sc - s;                                                                                        
}                                                                                                       
#endif

#ifndef __HAVE_ARCH_STRCPY
/**
 * strcpy - Copy a %NUL terminated string
 * @dest: Where to copy the string to
 * @src: Where to copy the string from
 */
char * strcpy(char * dest,const char *src)
{
  char *tmp = dest;

  while ((*dest++ = *src++) != '\0')
    /* nothing */;
  return tmp;
}
#endif 

#ifndef __HAVE_ARCH_MEMCPY
/**
 * memcpy - Copy one area of memory to another
 * @dest: Where to copy to
 * @src: Where to copy from
 * @count: The size of the area.
 *
 * You should not use this function to access IO space, use memcpy_toio()
 * or memcpy_fromio() instead.
 */
void * memcpy(void * dest,const void *src,size_t count)
{
  char *tmp = (char *) dest, *s = (char *) src;

  while (count--)
    *tmp++ = *s++;

  return dest;
}
#endif


#ifndef __HAVE_ARCH_MEMSET
void * memset(void * s,int c,unsigned int count)
{
  char *xs = (char *) s;

  while (count--)
    *xs++ = c;

  return s;
}
#endif                                                                                                  

#ifndef __HAVE_ARCH_STRCHR
/**
 * strchr - Find the first occurrence of a character in a string
 * @s: The string to be searched
 * @c: The character to search for
 */
char * strchr(const char * s, int c)
{
  for(; *s != (char) c; ++s)
    if (*s == '\0')
      return NULL;
  return (char *) s;
}   
#endif

#ifndef __HAVE_ARCH_STRCMP
/**
	 * strcmp - Compare two strings
	  * @cs: One string
		 * @ct: Another string
		  */
int strcmp(const char * cs,const char * ct)
{
	  register signed char __res; 

		  while (1) {
				    if ((__res = *cs - *ct++) != 0 || !*cs++)
							      break;
						  }

			  return __res;
}
#endif 

#ifndef __HAVE_ARCH_STRNCMP
/**
 * strncmp - Compare two length-limited strings
 * @cs: One string
 * @ct: Another string
 * @count: The maximum number of bytes to compare
 */
int strncmp(const char * cs,const char * ct,size_t count)
{
  register signed char __res = 0;

  while (count) {
    if ((__res = *cs - *ct++) != 0 || !*cs++)
      break;
    count--;
  }

  return __res;
}
#endif

