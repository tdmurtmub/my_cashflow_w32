/***********************************************************************
Discovers the Current Build Environment {C}
Copyright (C) 2001-2003 by Wesley Steiner. All rights reserved.
***********************************************************************/

/* This file MUST be stand-alone. It MUST not include any other headers. */

#ifndef _MODULE_BLDENV
#define _MODULE_BLDENV

/* Generated Definitions:

	ATL_MAJOR	n	Microsoft ATL Library Major Version #
	BCPP			Borland C++
    BCPP16			Borland C++ for 16-bit Windows
    BCPP_V31		Borland C++ v3.1
	MSC_MAJOR	n	Microsoft C Compiler Major Version #
	MSVC			Microsoft Visual C++
	MSVC16			Microsoft Visual C++ for 16-bit Windows
	MSVC_V15		Microsoft Visual C++ v1.5
	MSVC_V60		Microsoft Visual C++ v6.0
	MSVC_V70		Microsoft Visual C++ v7.0
	WIN16			Windows 16-bit Compilation
	WINCE			Windows CE
*/

#ifdef _MSC_VER
	#define MSVC
	#if (_MSC_VER <= 800)
		#define WIN16
		#define NOCPPBOOL
		#define MSVC16
	#endif
	#if (_MSC_VER == 800)
		#define MSVC_V15
	#endif
	#if (_MSC_VER == 1200)
		#define MSC_MAJOR 6
		#define MSVC_V60
	#endif
	#if (_MSC_VER == 1300)
		#define MSC_MAJOR 7
		#define MSVC_V70
	#endif
	#ifdef _WIN32_WCE
		#define WINCE
	#endif
#endif

#ifdef _ATL_VER
	#define ATL_MAJOR (_ATL_VER >> 8)
#endif

#ifdef __BORLANDC__
	#define BCPP
	#if (__BORLANDC__ <= 0x0410)
		#define WIN16
		#define NOCPPBOOL
		#define BCPP16
	#endif
	#if (__BORLANDC__ == 0x0410)
    	#define BCPP_V31
	#endif
#endif

#ifdef __cplusplus
#ifdef NOCPPBOOL
	#define bool int
	#define false 0
	#define true 1
#endif
#endif

#ifdef MSVC16
	#pragma warning(disable:4097) // deriving a class from a struct
#endif

#ifndef MAX
	#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
	#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif
	// 10-04-02: Switch to explicit MIN / MAX macros from troublesome min / max.
	
#if defined(MSVC_V60) || defined(BCPP16)
	#define SCM(ty, name, val) enum {name = val}
#else
	#define SCM(ty, name, val) static const ty name = val;                 
#endif
	// 09-15-01: Added this macro to support definition of static const members
	// inside a class declaration using Microsoft C++ compilers.
	// 01-31-02: Added SCM macro support for Borland C++ 16-bit.
	// 02-15-04: Fixed for MFC v7.

#ifdef BCPP16
	#define typename class
#endif
	// 01-13-02: Added this macro to support the typename attribute of a template
	// declaration for the 16-bit Borland C++ compilers.

#ifdef MSVC
	// MS Visual C++ does not and is not expected to support exception specifications.
	#pragma warning(disable:4290)
		// 10-04-02: Added this line to quiet the compiler.

	// Identifier exceeds 255 chars.
	#pragma warning(disable:4786)
		// 11-10-02: Added this line to quiet the compiler. To be effective this file
		// should be included before any STL headers.
#endif

#endif // MODULE

/* JOURNAL

07-02-01: 

Created.

07-07-01: 

Added support for Borland C 3.1 compiler and introduced the
NOCPPBOOL symbol for multiple platforms. Also added WIN16 symbol to
define a Windows 16-bit compilation.

01-06-03: 

Removed SIFT support code.

09-11-03: 

Added symbol for MSVC 7 and ATL versioning.

11-23-03: 

In general I am adopting a naming convention that uses a "_WSM_" prefix for 
macros to further avoid conflicts with third party libraries.

12-02-03:

Moved definition of cross-platform throw() macro out to XP/Exception.h module
and renamed it to avoid conflict with the throw statement.

*/
