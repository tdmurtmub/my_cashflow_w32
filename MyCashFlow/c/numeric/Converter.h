// Numeric Conversion Class
// Copyright (C) 2004 Wesley Steiner
// Platforms: {C++}

#ifndef _MODULE_NUMERIC_CONVERTER_
#define _MODULE_NUMERIC_CONVERTER_

#include <string>
#include <sstream>

namespace numeric 
	{
	template <typename T>
	struct Converter
		{
		static std::string ToString(T a_val)
			{ 
			std::ostringstream aStream;
			aStream << a_val ;
			return aStream.str(); 
			}
		};

	struct FromString
		{
		static int 
		ToInt(const std::string& a_str) 
			{ 
			return atoi(a_str.c_str()); 
			}

		static unsigned int 
		ToUInt(const std::string& a_val) 
			{ 
			return static_cast<unsigned int> (atol(a_val.c_str())); 
			}
		};

	} // namespace Numeric

#endif // _MODULE_NUMERIC_CONVERTER_
