// Simple Geometric Classes {C++|VC6|VC7}
// Copyright (C) 2001-2004 by Wesley Steiner. All rights reserved.
// No templates.
// OBSOLETE if your C++ environment supports namespaces. Use XY.h instead.

#ifndef _MODULE_SGEOM_
#define _MODULE_SGEOM_

#include <bldenv.h>
#include <geometry.h>
#include <typedefs.h>

class SSize;
class SPoint;
class SRect;

class SSize : public tSize
	{
	public:

	SSize() { }
	
	SSize(int a_width, int a_height) 
		{
		mWidth = a_width;
		mHeight = a_height;
		}
	
	// create from another size
	SSize(tSize initSize);
	
	// create from a point
	SSize(tPoint initPt);

	// create from a uint32: cx = LOWORD(dw) cy = HIWORD(dw)
	SSize(uint32 dwSize);

	int Width() const { return mWidth; }
	int Height() const { return mHeight; }

//	bool operator==(tSize size) const;
//	bool operator!=(tSize size) const;
//	void operator+=(tSize size);
//	void operator-=(tSize size);

	// Operators returning SSize values
//	SSize operator+(tSize size) const;
//	SSize operator-(tSize size) const;
//	SSize operator-() const;

	// Operators returning SPoint values
//	SPoint operator+(tPoint point) const;
//	SPoint operator-(tPoint point) const;

	// Operators returning SRect values
//	SRect operator+(const tRect* lpRect) const;
//	SRect operator-(const tRect* lpRect) const;
	};

// A 2-D point, similar to Windows POINT structure.
class SPoint : public tPoint
{
	public:

	SPoint() { }
		// create an uninitialized point
	SPoint(int initX, int initY) { mX = initX; mY = initY; }
		// create from two integers
//	SPoint(tPoint initPt);
		// create from another point
	SPoint(tSize size) { mX = size.mWidth; mY = size.mHeight; }
		// create from a size
		// 11-10-01: Added this constructor.

//	SPoint(uint32 dwPoint);
		// create from a dword: x = LOWORD(dw) y = HIWORD(dw)

	// Operations

	// translate the point
	void Offset(int xOffset, int yOffset) { mX += xOffset; mY += yOffset; }
//	void Offset(tPoint point);
//	void Offset(tSize size);

//	bool operator==(tPoint point) const;
//	bool operator!=(tPoint point) const;
	void operator+=(tSize size) { Offset(size.mWidth, size.mHeight); };
//	void operator-=(tSize size);
//	void operator+=(tPoint point);
//	void operator-=(tPoint point);
	void operator+=(int delta) { Offset(delta, delta); }

	// Operators returning SPoint values
	SPoint operator+(int delta) const 
		{ return SPoint(mX + delta, mY + delta); }
	SPoint operator+(tSize size) const 
		{ return SPoint(mX + size.mWidth, mY + size.mHeight); }
//	SPoint operator-(tSize size) const;
//	SPoint operator-() const;
//	SPoint operator+(tPoint point) const;

	// Operators returning SSize values
	SSize operator-(tPoint point) const
		{ return SSize(mX - point.mX, mY - point.mY); }
		// 11-10-01: Added this operator.

	// Operators returning SRect values
//	SRect operator+(const tRect* lpRect) const;
//	SRect operator-(const tRect* lpRect) const;
	};

// A Simple 2-D rectangle class.
class SRect : public tRect
	{
	public:

	SRect()
		{ 
		mLeft = 0;
		mTop = 0;
		mRight = 0;
		mBottom = 0;
		}

	SRect(int l, int t, int r, int b)
		// from left, top, right, and bottom
		{
		mLeft = l;
		mTop = t;
		mRight = r;
		mBottom = b;
		}

	SRect(const tPoint& pos, const tSize& sz)
		// from a point and a size
		{
		mLeft = pos.mX;	
		mTop = pos.mY;	
		mRight = mLeft + sz.mWidth;	
		mBottom = mTop + sz.mHeight;	
		}
		// 06-10-01: Added this constructor for initializing a rectangle from a top-left
		// point and a known size.

	SRect(const tRect& srcRect)
		// from another rect
		{
		mLeft = srcRect.mLeft;	
		mTop = srcRect.mTop;	
		mRight = srcRect.mRight;	
		mBottom = srcRect.mBottom;	
		}

//	SRect(SRect* lpSrcRect);
	// from a point and size
//	SRect(tPoint point, tSize size);
	// from two points
//	SRect(tPoint topLeft, tPoint bottomRight);

	// Attributes (in addition to tRect members)

	int Width() const { return mRight - mLeft; }
		// retrieves the width
	int Height() const { return mBottom - mTop; }
		// returns the height

	// returns the size
//	SSize Size() const;
	// reference to the top-left point
//	SPoint& TopLeft();
	// reference to the bottom-right point
//	SPoint& BottomRight();
	// const reference to the top-left point
//	const SPoint& TopLeft() const;
	// const reference to the bottom-right point
//	const SPoint& BottomRight() const;
	// the geometric center point of the rectangle
//	SPoint CenterPoint() const;
	// swap the left and right
//	void SwapLeftRight();
//	static void SwapLeftRight(tRect* lpRect);

	// convert between SRect and tRect*/SRect* (no need for &)
//	operator tRect*();
	operator SRect*() { return this; }

	bool IsEmpty() const { return (Width() == 0) && (Height() == 0); }
		// returns TRUE if rectangle has no area

	// returns TRUE if rectangle is at (0,0) and has no area
//	bool IsRectNull() const;

	// Returns true if point is within rectangle.
	bool PtWithin(int x, int y) const
		{ return ((x >= mLeft) && (x < mRight)) && ((y >= mTop) && (y < mBottom)); }
		// 11-11-01: Added this member function.

	// Operations

	// set rectangle from left, top, right, and bottom
//	void SetRect(int x1, int y1, int x2, int y2);
//	void SetRect(tPoint topLeft, tPoint bottomRight);
	// empty the rectangle
//	void SetRectEmpty();
	// copy from another rectangle
//	void CopyRect(SRect* lpSrcRect);
	// TRUE if exactly the same as another rectangle
//	bool EqualRect(SRect* lpRect) const;
	void SetPos(int x, int y) { mLeft = x; mTop = y; }
		// set the position
	void SetPos(SPoint& pt) { mLeft = pt.mX; mTop = pt.mY; }
		// set the position
	void SetSize(int dx, int dy) { mRight = mLeft + dx; mBottom = mTop + dy; }
		// set the size keeping the position constant

	// inflate rectangle's width and height without
	// moving its top or left
//	void InflateRect(int x, int y);
//	void InflateRect(tSize size);
//	void InflateRect(SRect* lpRect);
//	void InflateRect(int l, int t, int r, int b);
	// deflate the rectangle's width and height without
	// moving its top or left
//	void DeflateRect(int x, int y);
//	void DeflateRect(tSize size);
//	void DeflateRect(SRect* lpRect);
//	void DeflateRect(int l, int t, int r, int b);

	// translate the rectangle by moving its top and left
	void Offset(const tSize size) { Offset(size.mWidth, size.mHeight); }
	void Offset(const SSize& size) { Offset(size.Width(), size.Height()); }
	void Offset(const tPoint point) { Offset(point.mX, point.mY); }
	void Offset(const SPoint& point) { Offset(point.mX, point.mY); }
	void Offset(int x, int y)
		{
		mLeft += x;
		mTop += y;
		mRight += x;
		mBottom += y;
		} 

//	void NormalizeRect();

	bool IntersectRect(const SRect* lpRect1, const SRect* lpRect2)
		// set this rectangle to the intersection of two others
		// returns true if the intersection is not empty; false if the intersection is empty.
		// Makes an SRect equal to the intersection of two existing rectangles. 
		// The intersection is the largest rectangle contained in both existing 
		// rectangles. Both of the rectangles must be normalized or this function 
		// may fail.
		{
		mLeft = MAX(lpRect1->mLeft, lpRect2->mLeft);
		mRight = MIN(lpRect1->mRight, lpRect2->mRight);
		mTop = MAX(lpRect1->mTop, lpRect2->mTop);
		mBottom = MIN(lpRect1->mBottom, lpRect2->mBottom);
			// 10-04-02: Switch to explicit MIN / MAX macros from troublesome min / max.

		if ((Width() < 0) || (Height() < 0))
			{
			SetPos(0, 0);
			SetSize(0, 0);
			}
			
		return (!IsEmpty());
		}

	// set this rectangle to the bounding union of two others
	// returns true if the union is not empty; false if the union is empty
	// Makes the dimensions of CRect equal to the union of the two source 
	// rectangles. The union is the smallest rectangle that contains both 
	// source rectangles.  Both of the rectangles must be normalized or this function 
	// may fail.
	bool UnionRect(const SRect* lpRect1, const SRect* lpRect2)
		{
		mLeft = MIN(lpRect1->mLeft, lpRect2->mLeft);
		mTop = MIN(lpRect1->mTop, lpRect2->mTop);
		mRight = MAX(lpRect1->mRight, lpRect2->mRight);
		mBottom = MAX(lpRect1->mBottom, lpRect2->mBottom);
			// 10-04-02: Switch to explicit MIN / MAX macros from troublesome min / max.

		return !IsEmpty();
		}

	// set this rectangle to minimum of two others
//	bool SubtractRect(SRect* lpRectSrc1, SRect* lpRectSrc2);

	bool Intersects(const SRect* aRect) const
		// returns true if this rectangle intersects another rectangle
		{
		SRect intersect;
		intersect.IntersectRect(this, aRect);
		return !intersect.IsEmpty();
		}

	// Additional Operations
//	void operator=(const tRect& srcRect);
	bool operator==(const tRect& rect) const
		{
		return (mLeft == rect.mLeft) && (mTop == rect.mTop) && (mRight == rect.mRight)
				&& (mBottom == rect.mBottom);
		}
//	bool operator!=(const tRect& rect) const;
//	void operator+=(tPoint point);
//	void operator+=(tSize size);

//	void operator-=(tPoint point);
//	void operator-=(tSize size);
//	void operator-=(SRect* lpRect);
//	void operator&=(const tRect& rect);
//	void operator|=(const tRect& rect);

	SRect operator+(tPoint point) const
		{ 
		SRect sr(*this);
		sr.Offset(point);
		return sr;
		}

	// Shrink the rect.
	SRect operator<<(int delta) const
		{ 
		SRect sr(*this);
		sr.mLeft += delta;
		sr.mTop += delta;
		sr.mRight -= delta;
		sr.mBottom -= delta;
		return sr;
		}

	// Expand the rect.
	SRect operator>>(int delta) const
		{ 
		SRect sr(*this);
		sr.mLeft -= delta;
		sr.mTop -= delta;
		sr.mRight += delta;
		sr.mBottom += delta;
		return sr;
		}

//	SRect operator-(tPoint point) const;

	// Union operator
	SRect operator|(SRect* lpRect) const
		{
		SRect result;
		result.UnionRect(this, lpRect);
		return result;
		} 
		// 05-06-01: Added this operator to implement the union of two rectangles.
		// 05-07-01: Changeed union operator from '+' to '|'.

//	SRect operator+(SRect* lpRect) const
//	SRect operator+(tSize size) const;
//	SRect operator-(tSize size) const;
//	SRect operator-(SRect* lpRect) const;
//	SRect operator&(const tRect& rect2) const;
	SRect MulDiv(int nMultiplier, int nDivisor) const;

	// Union
	void operator|=(SRect* lpRect) { *this = *this | lpRect; }
		// 05-06-01: Added this operator to implement the union of two rectangles.
		// 05-07-01: Changed union operator from '+=' to '|='.

	};

#endif // MODULE

/************************************* LOG *************************************

01/21/01 -----------------------------------------------------------------------

Implement methods as needed.

SRect* operator is exploding the stack (calling itself). Needed to remove the
const'ness and change the return value from *this to this.

01/30/01 -----------------------------------------------------------------------

Added SetPos(SPoint& pt) method.

02/02/01 -----------------------------------------------------------------------

Added SRect::Offset() methods.

02/03/01 -----------------------------------------------------------------------
	
Added a SRect::Intersects method to do a qualitative test for intersection with
another rectangle.

02/18/01 -----------------------------------------------------------------------
	
Fixed and simplified a bug in IntersectRect.	

04/01/01 -----------------------------------------------------------------------

Included the minmax.h module.

04/15/01 -----------------------------------------------------------------------

Added << and >> operators to shrink and expand a normalized SRect.

04/19/01 -----------------------------------------------------------------------

Added operators to extend the SPoint class.

04/21/01 -----------------------------------------------------------------------

Added SPoint::operator+ operator.

04/22/01 -----------------------------------------------------------------------

Expanded SPoint::operator+ and opertor+= operators.

12/03/03 -----------------------------------------------------------------------

Changed all occurences of DWORD to uint32 since DWORD is specific to Windows.

************************************** LOG ************************************/
