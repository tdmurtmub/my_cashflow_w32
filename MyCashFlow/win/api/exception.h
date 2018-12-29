// Windows API Exception Handling Interface
// Copyright (C) 2003-2005 by Wesley Steiner. All rights reserved.
// Target Platforms: {W32|C++}

#ifndef _MODULE_WIN_API_EXCEPTION
#define _MODULE_WIN_API_EXCEPTION

#include <exception>

namespace win { namespace api 
	{ 
	class Exception : public std::exception 
		{ 
		private:
		DWORD m_last_error;
		public:
		Exception(DWORD a_last_error) : m_last_error(a_last_error) { }
		}; 
	}}

// Convenience macros to inline Win API return code checking to generate
// exceptions. Note these are not debugging aids. These macros should be
// used if you need to know when an SDK call has failed unexpectedly.
// Also, by throwing an exception you can then incorporate the ScopeGaurd
// mechanism to handle errors.
//
//		WAXONZ		- throws if return code is Zero
//		WAXONZORN	- throws if return code is Zero or Negative
//		WAXONVAL	- throws if return code is a specific value

#define WAXONZ(stm) if (stm == 0) throw win::api::Exception(GetLastError())
#define WAXONZORN(stm) if (stm <= 0) throw win::api::Exception(GetLastError())
#define WAXONVAL(stm, val) if (stm == val) throw win::api::Exception(GetLastError())

#endif
