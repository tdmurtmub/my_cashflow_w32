/*****************************************************************************
Automatic (stack-based) GDI Classes {C++|SDK}
Copyright (C) 2000-2003 Wesley Steiner. All rights reserved.
OBSOLETE! Use Win::GDI::Lib.h instead.
******************************************************************************/

#ifndef _INC_AUTOGDI
#define _INC_AUTOGDI

#ifdef SIFT
#pragma message("  [" __FILE__ "]")
#endif

#include <windows.h>

#include "bldenv.h"

#ifdef __cplusplus

#if defined(MSVC16) || defined(BCPP16) || defined(WINCE)
	#define EXCEPTION_SPEC(a) // ignore it
	#define throw // ignore it
	#define STD_EXCEPTION return
#else
	#include <exception>
	#define EXCEPTION_SPEC(a) throw(a)
	#define STD_EXCEPTION std::exception()
#endif

// Hierarchy

class	AutoHDC;
class		AutoBeginPaint;					// BeginPaint...EndPaint
class		AutoCreateCompatibleDC;
class		AutoGetDC;						// GetDC...ReleaseDC
class	AutoHBITMAP;
class		AutoCreateBitmap;
class		AutoCreateCompatibleBitmap;

// a common class for all Auto HDC classes 
class AutoHDC
	{
	public:
	HDC mHDC;

	AutoHDC(HDC aHDC = NULL) : mHDC(aHDC), 
			mPrevBitmap(NULL), mPrevPen(NULL), mPrevBrush(NULL),
			mBitmapSelected(false), mPenSelected(false), mBrushSelected(false) 
		{ }

	// implicit conversion operator
	operator HDC() { return mHDC; }

	protected:
	// previously selected objects via SelectObject
	HBITMAP mPrevBitmap;
	HPEN mPrevPen;
	HBRUSH mPrevBrush;

	// flags to indicate objects have been selected into this DC
	// these are needed in the derived class destructor
	bool mBitmapSelected;
	bool mPenSelected;
	bool mBrushSelected;

	// Clean up the DC before deleting it.
	// Can't be done in the destructor because of the destructor calling order 
	// in derived classes.
	void Cleanup()
		{
		if (mBrushSelected) ::SelectObject(mHDC, mPrevBrush);
		if (mPenSelected) ::SelectObject(mHDC, mPrevPen);
		if (mBitmapSelected) ::SelectObject(mHDC, mPrevBitmap);
		}

#ifndef WIN16

	public:
	HBITMAP SelectObject(HBITMAP handle)
		{
		mPrevBitmap = static_cast<HBITMAP> (::SelectObject(mHDC, handle));
		mBitmapSelected = true; // flag it as so
		return mPrevBitmap;
		}

	HPEN SelectObject(HPEN handle)
		{
		mPrevPen = static_cast<HPEN> (::SelectObject(mHDC, handle));
		mPenSelected = true; // flag it as so
		return mPrevPen;
		}

	HBRUSH SelectObject(HBRUSH handle)
		{
		mPrevBrush = static_cast<HBRUSH> (::SelectObject(mHDC, handle));
		mBrushSelected = true; // flag it as so
		return mPrevBrush;
		}

#endif // WIN16
	// 07-06-01: Added conditional compile wrapper since the method arguments
	// are not distinct under 16-bit Windows.
	};
	// 10-20-01: Made member and constructor public for direct use by outside modules.

class AutoCreateCompatibleDC : public AutoHDC
// Creates a stack based exception-safe Device Context (HDC) similar to
// the Windows CreateCompatibleDC API.
	{
	typedef AutoHDC inherited;

	public:
	AutoCreateCompatibleDC(HDC aHDC = NULL) EXCEPTION_SPEC(std::exception) 
		: AutoHDC(CreateCompatibleDC(aHDC))
		{ 
		if (!mHDC) throw STD_EXCEPTION;
		}

	virtual ~AutoCreateCompatibleDC() 
		{ 
		inherited::Cleanup();
		DeleteDC(mHDC); 
		}
	};

class AutoGetDC : public AutoHDC
// Gets a stack based exception-safe Device Context (HDC) from a Window
// or from the desktop window and releases it on destruction.
	{
	typedef AutoHDC inherited;

	public: // constructors

	AutoGetDC(HWND aHWnd = NULL) EXCEPTION_SPEC(std::exception)	
		: AutoHDC(GetDC(aHWnd)), m_hWnd(aHWnd)
		{
		if (!mHDC) throw STD_EXCEPTION;
		}

	virtual ~AutoGetDC()
		{
		inherited::Cleanup();
		ReleaseDC(m_hWnd, mHDC);
		}

	private:
	HWND m_hWnd;
	};

// a base class for all Auto HBITMAP classes
class AutoHBITMAP
	{
	public:
	HBITMAP m_hBitmap;

	AutoHBITMAP(HBITMAP a_hBitmap = NULL) : m_hBitmap(a_hBitmap) { }
	virtual ~AutoHBITMAP() { DeleteObject(m_hBitmap); }

	// implicit conversion operator
	operator HBITMAP() { return m_hBitmap; }

	// Detaches the current bitmap from the instance so that the destructor
	// will not dispose of it. Returns the detached bitmap handle.
	HBITMAP DetachBitmap() 
		{ 
		HBITMAP hb = m_hBitmap;
		m_hBitmap = NULL;
		return hb;
		}
	};
	// 10-20-01: Made member and constructor public for direct use by outside modules.

class AutoCreateBitmap : public AutoHBITMAP
// Creates a stack based exception-safe HBITMAP of the specified dimensions
// and attributes as per the SDK CreateBitmap API. By default the bitmap is 
// compatible with the screen device. On exit deletes the bitmap object. Call 
// DetachBitmap to avoid automatic destruction of the associated HBITMAP.
	{
	public:
	AutoCreateBitmap(int aWd, int aHt, UINT cPlanes = 1, UINT cBitsPerPel = 1)
			EXCEPTION_SPEC(std::exception)
		: AutoHBITMAP(CreateBitmap(aWd, aHt, cPlanes, cBitsPerPel, NULL))
		{
		if (!m_hBitmap) throw STD_EXCEPTION;
		}
	};

class AutoCreateCompatibleBitmap : public AutoHBITMAP
// Creates a stack based exception-safe HBITMAP of the specified dimensions
// and attributes as per the SDK CreateCompatibleBitmap API. By default the 
// bitmap is compatible with the screen device. On exit deletes the bitmap 
// object. Call DetachBitmap to avoid automatic destruction of the associated 
// HBITMAP.
	{
	public:
	AutoCreateCompatibleBitmap(size_t aWd, size_t aHt) EXCEPTION_SPEC(std::exception)
		: AutoHBITMAP()
		{
		AutoGetDC dc;
		m_hBitmap = ::CreateCompatibleBitmap(dc, static_cast<int> (aWd), static_cast<int> (aHt));
		if (!m_hBitmap) throw STD_EXCEPTION;
		}
	};
	// 12-31-03: Converted arguements to size_t from int to quiet compilers.

class AutoBeginPaint : public AutoHDC
	{
	HWND m_hWnd;
	PAINTSTRUCT m_paintStruct;

	public:
	AutoBeginPaint(HWND aHWnd) : AutoHDC(BeginPaint(aHWnd, &m_paintStruct)),
			m_hWnd(aHWnd) { }
	virtual ~AutoBeginPaint() { EndPaint(m_hWnd, &m_paintStruct); }

	RECT GetRect() const { return m_paintStruct.rcPaint; }
	};

#endif /* __cplusplus */

#endif // _INC_AUTOGDI

