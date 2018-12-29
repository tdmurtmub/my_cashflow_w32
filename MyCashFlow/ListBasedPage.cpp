#include "stdafx.h"

#include "ListBasedPage.h"

#include <win\mfc\wndinfo.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;
using namespace win::mfc;

BEGIN_MESSAGE_MAP(CListBasedPage, CPropertyPage)
	//{{AFX_MSG_MAP(CListBasedPage)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CListBasedPage::OnSize(UINT nType, int cx, int cy) 
	{
//	TRACE("CListBasedPage::OnSize(nType, %d, %d)\n", cx, cy);
	CPropertyPage::OnSize(nType, cx, cy);
	if (myList.m_hWnd != 0) 
		{
		CRect aScreenRect = CWndInfo(myList).WindowRect();
		CRect aParentScreenRect = CWndInfo(this).WindowRect();
		int aTopOffset = aScreenRect.top - aParentScreenRect.top;
		int aLeftOffset = aScreenRect.left - aParentScreenRect.left;
		myList.SetWindowPos(NULL, 0, 0, 
			cx - aLeftOffset - aLeftOffset - CWndInfo(this).RightBorderSize(), 
			cy - aTopOffset - aLeftOffset - CWndInfo(this).BottomBorderSize(), 
			SWP_NOZORDER | SWP_NOMOVE);
		}
	}
