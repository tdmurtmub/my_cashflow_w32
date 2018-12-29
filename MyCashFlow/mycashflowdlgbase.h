// Copyright (C) 2004 by Wesley Steiner. All rights reserved.

#pragma once

class CMyCashflowDlgBase : public CPropertySheet
	{
	protected: typedef CPropertySheet inherited;

	DECLARE_DYNAMIC(CMyCashflowDlgBase)

	public:
	CMyCashflowDlgBase(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyCashflowDlgBase)
	public:
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//}}AFX_VIRTUAL

	public:
	virtual ~CMyCashflowDlgBase();

	protected: test_virtual int OverridableGetActiveIndex() const { return GetActiveIndex(); }

	protected:
	HACCEL myAcceleratorTable;

	//{{AFX_MSG(CMyCashflowDlgBase)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
