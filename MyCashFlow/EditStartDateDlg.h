#if !defined(AFX_EDITSTARTDATEDLG_H__DEF4B534_BCB0_4B6C_874E_A6831382EEEC__INCLUDED_)
#define AFX_EDITSTARTDATEDLG_H__DEF4B534_BCB0_4B6C_874E_A6831382EEEC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include <win/mfc/CDate.h>
#include "resource.h"

class CEditStartDateDlg : public CDialog
	{
	public:
	CEditStartDateDlg(const CDate & aStartDate, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CEditStartDateDlg)
	enum { IDD = IDD_EDIT_STARTDATE };
	CDate m_aStartDate;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditStartDateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	protected:

	// Generated message map functions
	//{{AFX_MSG(CEditStartDateDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
