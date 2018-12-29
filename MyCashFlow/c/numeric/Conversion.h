// Numeric Conversion Functions
// Copyright (C) 2004 Wesley Steiner
// Platforms: {C++}

#ifndef _MODULE_NUMERIC_CONVERSION_
#define _MODULE_NUMERIC_CONVERSION_

#include <stdlib.h>
#include <string>

#include <numeric\Converter.h>

namespace numeric 
	{
	template<typename T> 
	std::string ToString(T a_val) { return Converter<T>::ToString(a_val); }

	inline int StringToInt(const std::string& a_str) { return FromString::ToInt(a_str.c_str()); }
	inline unsigned int StringToUInt(const std::string& a_str) { return FromString::ToUInt(a_str.c_str()); }
	}

#endif
