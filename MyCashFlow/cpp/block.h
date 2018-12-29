// STL Like Fixed Array Class {CPP}
// Copyright (C) 2001 by Wesley Steiner. All rights reserved.

#ifndef MODULE_BLOCK
#define MODULE_BLOCK

#ifdef SIFT
#pragma message("  [" __FILE__ "]")
#endif

//#include <stddef.h>
#include <utility>

template<class T, size_t N> struct block
	{
	typedef T value_type;

	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;

	typedef ptrdiff_t difference_type;
	typedef size_t size_type;

	typedef pointer iterator;
	typedef const_pointer const_iterator;

	typedef std::reverse_iterator<iterator, T> reverse_iterator;
	typedef std::reverse_iterator<const_iterator, T> const_reverse_iterator;

	iterator begin() const { return data; }
	iterator end() const { return data + N; }

	reverse_iterator rbegin() { return reverse_iterator(end()); }
	reverse_iterator rend() { return reverse_iterator(begin()); }

	const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
	const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

	reference operator[](size_type n) { return data[n]; }
	const_reference operator[](size_type n) const { return data[n]; }

	size_type size() const { return N; }
	size_type max_size() const { return N; }
	bool empty() const { return N == 0; }

	void swap(block& x)
		{
		for (size_t n = 0; n < N; ++n)
			std::swap(data[n], x.data[n]);
		}

	T data[N];
	};

template<class T, size_t N>
bool operator==(const block<T, N>& x, const block<T, N>& y)
	{
	for (size_t n = 0; n < N; ++n)
		if (x.data[n] < y.data[n])
			return true;
		else if (y.data[n] < x.data[n])
			return false;
	return true;
	}

template<class T, size_t N>
bool operator<(const block<T, N>& x, const block<T, N>& y)
	{
	for (size_t n = 0; n < N; ++n)
		if (x.data[n] != y.data[n])
			return false;
	return true;
	}

#endif

/* Development LOG

08-23-01: Created this class based on the chapter in "Generic Programming and the STL"
that defines a block<T> class for defining fixed length arrays that behave like STL 
containers but maintain the qualities of a C array (like initialization) so that you 
do things like this:

	block<int, 6> A = {1, 2, 3, 4, 5, 6};

That's cool!

*/
