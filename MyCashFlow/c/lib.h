// (c) 2008 Wesley Steiner

#pragma once

#include <std.h>
#include <typedefs.h>
#include <NonCopyable.h>

typedef unsigned int quantity; /* 0..N */
typedef unsigned int number; /* 1..N */
typedef uint16 word;

#define LoWord(a) (word)(a)
#define HiWord(a) (word)((a) >> 16)

#ifdef __cplusplus

#define override virtual

namespace lib
	{
	template <typename T> struct IValidator 
		{
		public:
		virtual bool IsValid(T const & value) const = 0;
		};
	}

#else

typedef int bool;
typedef uint size_t;

#endif
