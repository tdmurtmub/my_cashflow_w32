/***********************************************************************

	Copyright (c) 1994 by Wesley Steiner
	All rights reserved.

***********************************************************************/

/**********************************************************************

Purpose:

	Extra standard declarations for Microsoft C.

Comments:

$Header:   K:/lib/stdms.h_v   1.0   28 Jan 1995 10:31:46   WES  $

$Log:   K:/lib/stdms.h_v  $
   
      Rev 1.0   28 Jan 1995 10:31:46   WES
   Initial revision.

***********************************************************************/

#ifndef __STDMS_H

#include <io.h>

/* compatible with Borland's MATH.H */

/* Constants rounded for 21 decimals. */
#define M_E         2.71828182845904523536
#define M_LOG2E     1.44269504088896340736
#define M_LOG10E    0.434294481903251827651
#define M_LN2       0.693147180559945309417
#define M_LN10      2.30258509299404568402
#define M_PI        3.14159265358979323846
#define M_PI_2      1.57079632679489661923
#define M_PI_4      0.785398163397448309616
#define M_1_PI      0.318309886183790671538
#define M_2_PI      0.636619772367581343076
#define M_1_SQRTPI  0.564189583547756286948
#define M_2_SQRTPI  1.12837916709551257390
#define M_SQRT2     1.41421356237309504880
#define M_SQRT_2    0.707106781186547524401

#define FileExists(fs) (_access(fs, 0) == 0)

#define __STDMS_H
#endif /* __STDMS_H */
