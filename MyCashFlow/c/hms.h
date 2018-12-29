/***********************************************************************
Hours, Minutes, Seconds Information {C++|C}
Copyright (C) 2000 Wesley Steiner
***********************************************************************/

#ifndef _MODULE_HMS
#define _MODULE_HMS

#include <typedefs.h>

#define HMS_SECONDS_PER_DAY (60L * 60L * 24L)

#ifdef __cplusplus  
extern "C" 
{
#endif

#ifdef __cplusplus  
}
#endif

#ifdef __cplusplus  
#define hms HMS
namespace HMS {
	const ulong SECONDS_PER_DAY = HMS_SECONDS_PER_DAY; /* seconds in a day */
}
#endif // __cplusplus  

#endif
