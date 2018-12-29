/* (C) 1994 Wesley Steiner */

#ifndef _MODULE_ANSI
#define _MODULE_ANSI

#define ARRAYSIZE(a) (sizeof(a) / sizeof(a[0]))

#ifdef __cplusplus  
extern "C" 
{
#endif

char* StrLowerCase(char* sz);
char* StrUpperCase(char* sz);

/* legacy */

#define t_word unsigned int
#define t_byte unsigned char

#define UpperCase(a) StrUpperCase(s)

#ifdef __cplusplus  
}
#endif

#endif
