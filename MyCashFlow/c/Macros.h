// Common Utility Macros {C++}
// Copyright (C) 2003-2006 by Wesley Steiner. All rights reserved.

#ifndef MODULE_UTILITY_MACROS
#define MODULE_UTILITY_MACROS

// OBSOLETE! Do not use.
#define DECLARE_SBCLASS(A, B) private: \
	typedef A ctype; \
	typedef B inherited; 
// Include this macro inside the class declaration of single base classes to define the 
// "ctype" and "inherited" typedefs to be used within the class declaration. Especially
// useful for very long templated class definitions.
//
// Example:
//
//	class MyClass<origin, flavor, allocator> : public BaseClass<origin, flavor, allocator, mem> 
//		{
//		DECLARE_SBCLASS(MyClass<origin, flavor, allocator>, BaseClass<origin, flavor, allocator, mem>)
//		...
//		MyClass() : inherited() { }
//		ctype Myself() { return *this; }
//		}

#endif // MODULE
