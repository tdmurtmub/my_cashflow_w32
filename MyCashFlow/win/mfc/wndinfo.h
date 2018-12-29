// (C) 2006 Wesley Steiner

#pragma once

namespace win { namespace mfc { 

class CWndInfo // OBSOLETE: use CWndWrapper
	{
	public:
	CWndInfo(const CWnd & rWnd) : myCWnd(rWnd) { }
	CWndInfo(const CWnd * pWnd) : myCWnd(*pWnd) { }

	CRect ClientRect() const
		{
		CRect aRect;
		myCWnd.GetClientRect(aRect);
		return aRect;
		}
			
	CRect WindowRect() const
		{
		CRect aRect;
		myCWnd.GetWindowRect(aRect);
		return aRect;
		}

	int Left() const { return WindowRect().left; }
	int Top() const { return WindowRect().top; }
	int Width() const { return WindowRect().Width(); }
	int Height() const { return WindowRect().Height(); }
	int LeftBorderSize() const { return GetSystemMetrics(SM_CYEDGE); }
	int RightBorderSize() const { return GetSystemMetrics(SM_CYEDGE); }
	int BottomBorderSize() const { return GetSystemMetrics(SM_CYEDGE); }

	private:
	const CWnd & myCWnd;
	};

}}
