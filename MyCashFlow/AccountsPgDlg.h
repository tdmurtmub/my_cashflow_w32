#if !defined(AFX_ACCOUNTSPGDLG_H__4BDD7F14_C2B8_48E2_9E13_0360CC9AB1F2__INCLUDED_)
#define AFX_ACCOUNTSPGDLG_H__4BDD7F14_C2B8_48E2_9E13_0360CC9AB1F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AccountsCtrl.h"
#include "ListBasedPage.h"
#include "MyMoney.h"

class CAccountsPageDialog : public CListBasedPage, public app::IMarkupPersistence
	{
	typedef CListBasedPage inherited;

	public:
	CAccountsPageDialog(IConfigDataManager & rConfigDataManager, ICashflowListCtrl* pCashFlowListCtrl, CWnd* pParent = NULL);
	~CAccountsPageDialog();

	#ifdef TEST
	CAccountsListCtrl & GetListCtrl() { return m_aList; }
	#endif

	//{{AFX_DATA(CAccountsPageDialog)
	enum { IDD = IDD_PAGE_ACCOUNTS };
	//}}AFX_DATA

	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAccountsPageDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	protected:
	// Generated message map functions
	//{{AFX_MSG(CAccountsPageDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	private: 
	CAccountsListCtrl m_aList;

	// IMarkupPersistence ...
	void PutMarkupData(CMarkup& rMarkup) const;
	void GetMarkupData(CMarkup& rMarkup) { }

	test_private: 
	test_virtual void OnEditDelSelEntry();

	friend class CMyCashflowDlg;
	};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACCOUNTSPGDLG_H__4BDD7F14_C2B8_48E2_9E13_0360CC9AB1F2__INCLUDED_)
