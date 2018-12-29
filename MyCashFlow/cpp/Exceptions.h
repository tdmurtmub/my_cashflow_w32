// Cross Platform Exception Handling {C++}
// Copyright (C) 2003-2006 by Wesley Steiner. All rights reserved.

#ifndef _MODULE_XP_EXCEPTIONS
#define _MODULE_XP_EXCEPTIONS

#include "bldenv.h"

// Cross-platform exception specification for a function declaration that 
// guarantees not to throw any exceptions.
#if defined(WINCE) || defined(BCPP16)
	#define _WSM_NOTHROW
#else
	#define _WSM_NOTHROW throw()
#endif

namespace XP { }

#endif
