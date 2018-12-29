// Copyright (C) 2001-2006 by Wesley Steiner. All rights reserved.

#ifndef _MODULE_NUMEX
#define _MODULE_NUMEX

#define CENTERIN(size, low, high) (low + (high - low - size) / 2)
#define CENTEROF(low, high) (low + (high - low) / 2)

// Returns B percent of a value A.
// B must be a positive integer value
#ifdef _WINDOWS
#define PERCENT(A, B) MulDiv((A), (B), 100)
#else
#define PERCENT(A, B) (((A) * (B)) / 100)
#endif
	
// Macros that increment and decrement a value by delta in a circular sequence.
// Returns the new value.
// All values must be integral types or define the +, - and % operators.
// In both cases the delta argument must be a positive number.
#define CIRCULARINC(v, vMin, vMax, delta) (vMin + ((v - vMin + delta) % (vMax - vMin + 1)))
#define CIRCULARDEC(v, vMin, vMax, delta) (vMax - ((vMax - v + delta) % (vMax - vMin + 1)))

#endif
