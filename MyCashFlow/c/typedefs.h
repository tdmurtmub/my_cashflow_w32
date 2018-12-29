/* (C) 2001 Wesley Steiner */

#ifndef _MODULE_TYPEDEFS
#define _MODULE_TYPEDEFS

typedef signed char schar;
typedef signed short sshort;
typedef signed long slong;
typedef signed int sint;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned int uint;

typedef schar int8;
typedef sshort int16;
typedef slong int32;

typedef int8 sint8;
typedef int16 sint16;
typedef int32 sint32;

typedef uchar uint8;
typedef ushort uint16;
typedef ulong uint32;

#ifdef _WINDOWS_
typedef UINT64 uint64;
#endif

typedef uchar byte;

typedef uchar bit8;
typedef ushort bit16;
typedef ulong bit32;

#endif
