// Numeric Conversion / Manipulation Functions {C++}
// Copyright (c) 2000 by Wesley Steiner. All rights reserved.

#ifndef _MODULE_NUMEXT
#define _MODULE_NUMEXT

#include "bldenv.h"
#include "numex.h"

#ifdef __cplusplus

template <typename T> inline int CenterInRange(T size, T lo, T hi)
// Return the lower position of size centered in the range lo to hi.
// Eg: CenterInRange(1, 4, 2) = 2
// Eg: CenterInRange(1, 5, 2) = 2
// Eg: CenterInRange(1, 6, 2) = 3
	{ return CenterIn(lo, hi, size); }
	 
template <typename T> inline T ForceInRange(T value, T lo, T hi)
// Returns a value forced into the range lo to hi. A value 
// outside the range will be assigned the nearest limit.
// Eg: ForceInRange(800, -100, 900) = 800 {no change}
// Eg: ForceInRange(-900, 100, 900) = 100 {lo limit}
// Eg: ForceInRange(900, -100, 800) = 800 {hi limit}
	{
	value = MIN(value, hi);
	value = MAX(value, lo);
	return value;
	}

template <typename T> inline T PercentAdjust(T value, int aPercentage)
// Returns value adjusted by an integer percentage amount.
	{
#ifdef _WINDOWS
	return MulDiv(value, aPercentage, 100);
#else
	return (long int) (value * aPercentage) / 100;
#endif
	} 

template <typename T> class Circular
// A circular indexing class that automatically increments and decrements a
// value within predefined circular limits.
	{
	T m_val, m_min, m_max;
	int m_delta;	

	public:
	Circular(T iniVal, T minVal = 0, T maxVal = 0, int delta = 1) 
		: m_val(iniVal), m_min(minVal), m_max(maxVal), m_delta(delta) { }

	void SetValue(T value) { m_val = value; } // hack because operator=(T value) won't work?

	// derived classes can override these for dynamic ranging
	virtual T MinVal() { return m_min; }
	virtual T MaxVal() { return m_max; }
	virtual T Delta() { return m_delta; }

	// implicit casts
	operator T() const { return m_val; }
	operator T&() { return m_val; }

	Circular& operator=(const Circular& op) { m_val = op.m_val; return *this; }
	Circular& operator=(T value) { m_val = value; return *this; } // can not invoke this with "object = 123;"?
	T operator++() { m_val = CIRCULARINC(m_val, MinVal(), MaxVal(), Delta()); return m_val; }
	T operator--() { m_val = CIRCULARDEC(m_val, MinVal(), MaxVal(), Delta()); return m_val; }
	};

template <typename T> inline T CircularInc(T val, T minVal, T maxVal, int delta = 1)
// Increments a value by delta in a circular sequence.
	{ return CIRCULARINC(val, minVal, maxVal, delta); }

template <typename T> inline T CircularDec(T val, T minVal, T maxVal, int delta = 1)
// Decrements a value by delta in a circular sequence.
	{ return CIRCULARDEC(val, minVal, maxVal, delta); }

template <typename T> inline T CircularDelta(T val, T minVal, T maxVal, int delta = 1)
// Adjusts a value by delta in a circular sequence.
	{ return CircularInc(val, minVal, maxVal, delta); }

// Legacy

template <typename T> inline int CenterIn(T lo, T hi, T size) 
	// Return the lower position of size centered in the range lo to hi.
	// Eg: CenterIn(1, 4, 2) = 2
	// Eg: CenterIn(1, 5, 2) = 2
	// Eg: CenterIn(1, 6, 2) = 3
	{ 
	return lo + ((hi - lo + 1 - size) / 2); 
	}

#else

__inline int CenterInRange(int size, int lo, int hi)
	{
	return CENTERIN(size, lo, hi);
	}

#endif /* __cplusplus */

#endif
