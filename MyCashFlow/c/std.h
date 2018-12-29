/* (c) 1989 Wesley Steiner */

// OBSOLETE: use lib.h

#ifndef MODULE_STD
#define MODULE_STD

#define INT				int
#define LONG			long
#define SHORT			short
#define CHAR			char
typedef unsigned int 	UINT;
typedef unsigned long	ULONG;
typedef	LONG			INT_32;
typedef	ULONG			UINT_32;

#ifndef BOOL
#define BOOL int
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif

#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef WORD
#define WORD unsigned int
#endif

#ifndef DWORD
#define DWORD unsigned long
#endif

/* ASCII codes */
#define BS		8
#define TAB		9
#define CR		13
#define ESC		27
#define SPACE 	' '
#define BLANK	32

/* Extented Key Codes (0 + x) */
#define F1	59
#define F2	60
#define F3	61
#define F4	62
#define F5	63
#define F6	64
#define F7	65
#define F8	66
#define F9	67
#define F10	68

#define LARROW 75
#define RARROW 77
#define UARROW 72
#define DARROW 80

#define DOS_ERROR			61			/* function rc when a dos error occurs */
#define EOF_ERROR			62			/* because DOS cannot detect this */

#define LINEFEED	LF
#define FORMFEED	FF

#define sl_INTEGER	6
#define sl_WORD 		sl_INTEGER

#define sl_Path		80
#define sl_Name		9
#define sl_Ext			5

#define sl_DRIVE		2
#define sl_DIR			79
#define sl_PATH		sl_Path
#define sl_NAME		sl_Name
#define sl_EXT			sl_Ext

#define SL_NAME		8
#define SL_EXT			3

#define SL_FILE		(SL_NAME + 1 + SL_EXT)

#define keypressed kbhit()
#define KeyPressed keypressed
#define pause getch()
#define Pause pause
#define strend(s) strchr(s, 0)
#define StrEnd(s) strend(s)
#define sqr(n) ((n) * (n))
#define Sqr(n) sqr(n)

#define strmt(szString) (szString[0] == '\0') // empty string

#define Center(size, start, end) (start + (end - start - size) / 2)

#ifdef _MSC_VER
#define MSC
#include "stdms.h"
#endif

#ifdef __TURBOC__
#define TC
#include "stdtc.h"
#endif

#define Round(x) round(x);
int round(double x);

#ifdef __BORLANDC__
#define BC
#endif

#ifdef BC
long int GetFileSize(const char *path); /* -1 if file does not exist */
#endif

#endif // MODULE_STD
