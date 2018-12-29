#ifndef LISTBASEDPAGE_H
#define LISTBASEDPAGE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyMoney.h"

class CListBasedPage : public CPropertyPage
	{
	public:
	CListBasedPage(UINT aResourceId, CListCtrl & aList) : CPropertyPage(aResourceId), myList(aList) 
		{ 
		}

	private:
	CListCtrl & myList;

	test_protected:
	// Generated message map functions
	//{{AFX_MSG(CListBasedPage)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
	};

#endif // LISTBASEDPAGE_H
	
