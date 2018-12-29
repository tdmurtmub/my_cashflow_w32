#if !defined(AFX_TRANSACTIONSPGDLG_H__A24CC03F_A8C0_49AB_BE14_EC2BC6BD2322__INCLUDED_)
#define AFX_TRANSACTIONSPGDLG_H__A24CC03F_A8C0_49AB_BE14_EC2BC6BD2322__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ListBasedPage.h"
#include "TrnxListCtrl.h"

class CTransactionsPageDialog : public CListBasedPage, public app::IMarkupPersistence
	{
	public:
	CTransactionsPageDialog(IConfigDataManager & rConfigDataManager, ICashflowListCtrl * pCashFlowListCtrl, CWnd* pParent = NULL);

	#ifdef TEST
	CTransListCtrl & GetListCtrl() { return m_aList; }
	#endif

	//{{AFX_DATA(CTransactionsPageDialog)
	enum { IDD = IDD_PAGE_TRANSACTIONS };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransactionsPageDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	protected:
	// Generated message map functions
	//{{AFX_MSG(CTransactionsPageDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	private:
	// IMarkupPersistence ...
	void PutMarkupData(CMarkup& rMarkup) const;
	void GetMarkupData(CMarkup& rMarkup) { }
	CTransListCtrl m_aList;

	test_private: 
	test_virtual void OnEditDelSelEntry();

	friend class CMyCashflowDlg;
	};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSACTIONSPGDLG_H__A24CC03F_A8C0_49AB_BE14_EC2BC6BD2322__INCLUDED_)
