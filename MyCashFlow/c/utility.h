// Copyright (C) 2003-2006 by Wesley Steiner. All rights reserved.

#ifndef _MODULE_UTILITY_
#define _MODULE_UTILITY_

#include <Macros.h>

#ifdef __cplusplus

#include <CppMacros.h>
#include <NonCopyable.h>
#include <Exceptions.h>

namespace utility 
	{
	inline void Unimplemented() { throw; }
	}

#endif // __cplusplus

#endif
