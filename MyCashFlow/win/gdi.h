// (C) 2003 Wesley Steiner

#ifndef MODULE_WIN_GDI_LIB
#define MODULE_WIN_GDI_LIB

#include <windows.h>
#include "utility.h"
#include "autogdi.h"

#ifdef __cplusplus

#define GDI gdi // legacy
#define RGBColor rgb // legacy

namespace win { namespace gdi 
	{
	namespace rgb
		{
		typedef COLORREF type;
		const type BLUE = RGB(0, 0, 255);
		const type BLACK = RGB(0, 0, 0);
		const type GRAY = RGB(128, 128, 128);
		const type GREEN = RGB(0, 255, 0);
		const type LIGHT_BLUE = RGB(128, 128, 255);
		const type LIGHT_GRAY = RGB(196, 196, 196);
		const type RED = RGB(255, 0, 0);
		const type WHITE = RGB(255, 255, 255);
		}

	const COLORREF WHITE = rgb::WHITE;
	const COLORREF BLACK = rgb::BLACK;

	namespace Auto
	// Implements auto-cleanup versions of common GDI functions and objects.
		{
		typedef AutoBeginPaint BeginPaint;

		class Brush : _WSM_DECLARE_NONCOPYABLE
			{
			private:
			HBRUSH m_hbrush;
			public:
			Brush(rgb::type a_rgb)
				{
				if ((m_hbrush = CreateSolidBrush(a_rgb)) == NULL) 
					throw STD_EXCEPTION;
				}
			~Brush() { DeleteObject(m_hbrush); }
			operator HBRUSH() { return m_hbrush; }
			};

		typedef AutoCreateCompatibleBitmap CreateCompatibleBitmap;
		typedef AutoCreateCompatibleDC CreateCompatibleDC;
		typedef AutoGetDC GetDC;
		}

	inline BOOL 
	BitBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc,
			int nXSrc = 0, int nYSrc = 0, DWORD dwRop = SRCCOPY)
		{ 
		return ::BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
		}

	inline int 
	FillRect(HDC hDC, const RECT* lprc, HBRUSH hbr)
		{ 
		return ::FillRect(hDC, lprc, hbr); 
		}

	inline int 
	FillRect(HDC hDC, const RECT& rc, HBRUSH hbr)
		{
		return gdi::FillRect(hDC, &rc, hbr);
		}

	inline RECT
	GetClientRect(HWND hWnd)
		{
		RECT r;
		::GetClientRect(hWnd, &r);
		return r;
		}
	}}

#endif /* __cplusplus */

#endif // MODULE
