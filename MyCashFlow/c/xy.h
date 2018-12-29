/* (C) 2003 Wesley Steiner */

#ifndef _MODULE_XY_
#define _MODULE_XY_

#include <sgeom.h>

#ifdef __cplusplus

namespace xy
	{
	typedef ::SSize Extent;
	typedef ::SPoint Point;
	
	class Rectangle : public ::SRect 
		{
		public:
		Rectangle() : SRect() { }
		Rectangle(SRect const & aRect) : SRect(aRect) { }
		Rectangle(int left, int top, int right, int bottom) : SRect(left, top, right, bottom) { }
		int Bottom() const { return mBottom; }
		int Left() const { return mLeft; }
		int Right() const { return mRight; }
		int Top() const { return mTop; }
		};
	}

#else /*__cplusplus*/

#include <ooc.h>

typedef SSize Extent;
typedef SPoint Point;

SUB_CLASS(xy_Rectangle, SRect)
END_CLASS(xy_Rectangle)

#endif /*__cplusplus*/

#endif
