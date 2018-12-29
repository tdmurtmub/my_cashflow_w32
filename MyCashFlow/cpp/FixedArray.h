// Defines a Fixed Size Array Class {C++}
// Copyright (C) 2003-2004 by Wesley Steiner. All rights reserved.

#ifndef MODULE_C_UTILITY_FIXEDARRAY
#define MODULE_C_UTILITY_FIXEDARRAY

#include <assert.h>

#include "NonCopyable.h"

namespace Utility 
	{

	template<typename T> class FixedArray : _WSM_DECLARE_NONCOPYABLE
	// Implements a fixed length array class for simple types.
	// Size = 0 is allowed!
	// Use this class when you need a fixed length array of simple types
	// that will clean up when they leave scope.
		{
		private:
		size_t m_size;
		T* m_buffer_p;

		public:
		FixedArray(size_t a_size) : m_size(a_size) { m_buffer_p = new T[a_size]; }
		~FixedArray() throw() { delete [] m_buffer_p; }
		size_t Size() const { return m_size; }
		void Copy(const T* a_source_p) const 
			{
			assert(a_source_p != m_buffer_p);
			memcpy(m_buffer_p, a_source_p, m_size);
			}
		operator const T*() { return m_buffer_p; }
		operator void*() { return m_buffer_p; }
		T& operator[](size_t a_index) 
			{ 
			assert(a_index < m_size);
			return m_buffer_p[a_index]; 
			}
		T& operator[](int a_index) 
			{ 
			assert(a_index >= 0);
			return operator[](static_cast<size_t>(a_index)); 
			}
		};

	} // namespace Utility

#endif
