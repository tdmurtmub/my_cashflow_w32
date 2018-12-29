// (C) 2004 Wesley Steiner

#if !defined(AFX_PAGECASHFLOW_H__6AD438B6_3E3A_42FE_BB90_5EA5B5BA54D3__INCLUDED_)
#define AFX_PAGECASHFLOW_H__6AD438B6_3E3A_42FE_BB90_5EA5B5BA54D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CashFlowCtrl.h"
#include "ListBasedPage.h"

interface ITransListCtrl;

class CAccountsListCtrl;

class CCashflowPageDialog : public CListBasedPage, public app::IMarkupPersistence
	{
	typedef CListBasedPage inherited;

	public:
	CCashflowPageDialog(IConfigDataManager & rConfigDataManager,
		IAccountsListCtrl* pAccntsListCtrl, ITransListCtrl* pTranxListCtrl, 
		size_t nDurationInMonths = CCashFlowListCtrl::Limits::Duration::DEFAULT,
		CWnd* pParent = NULL);

	#ifdef TEST
	const CCashFlowListCtrl & ListCtrl() const { return myList; }
	CCashFlowListCtrl & GetListCtrl() { return myList; }
	#endif

	//{{AFX_DATA(CCashflowPageDialog)
	enum { IDD = IDD_PAGE_CASHFLOW };
	CSpinButtonCtrl	myDurationSpinCtrl;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCashflowPageDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	private:
	CCashFlowListCtrl myList;

	// IMarkupPersistence ...
	void PutMarkupData(CMarkup& rMarkup) const;
	void GetMarkupData(CMarkup& rMarkup);

	test_protected:
	// Generated message map functions
	//{{AFX_MSG(CCashflowPageDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonDecDate();
	afx_msg void OnButtonIncDate();
	afx_msg void OnStartingDate();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()

	test_private:
	friend class CMyCashflowDlg;

	// unit testing ...
	friend class CCashflowPageDialogTest;
	friend class CCashflowPageDialogFixture;
	};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGECASHFLOW_H__6AD438B6_3E3A_42FE_BB90_5EA5B5BA54D3__INCLUDED_)
