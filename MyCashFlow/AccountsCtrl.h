// (C) 2006 Wesley Steiner

#if !defined(AFX_ACCOUNTCTRL_H__729531FF_4651_4C0A_8AEA_0CEBFA504C96__INCLUDED_)
#define AFX_ACCOUNTCTRL_H__729531FF_4651_4C0A_8AEA_0CEBFA504C96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AccountData.h"
#include "BaseListCtrl.h"
#include "IAccountsListCtrl.h"

interface ICashflowListCtrl;

class CAccountsListCtrl : public CBaseListCtrl, public IAccountsListCtrl
	{
	public:
	struct Column 
		{ 
		struct Index 
			{
			enum {DESC = 0, AMOUNT, COUNT };
			};
		struct sort
			{
			static bool Ascending[Index::COUNT]; // records next sort direction for each column
			};
	};

	struct column { static UINT WIDTHS[]; };
	
	CAccountsListCtrl(IConfigDataManager & rConfigDataManager, ICashflowListCtrl* pCashFlowListCtrl);
	virtual ~CAccountsListCtrl();

	int AddEntry(LPCTSTR pszDesc, appl::Dollar::Value::type vAmnt = 0, bool make_current = false);
	virtual void EditEntry(UINT nIndex);
	void UpdateEntry(UINT nIndx, LPCTSTR pszDesc, appl::Dollar::Value::type vAmnt = 0);
	appl::Dollar::Value::type GetItemAmount(int iItem) const;

	// IAccountsListCtrl ...
	appl::Dollar::Value::type GetBalance() const;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAccountsListCtrl)
	public:
	virtual void PreSubclassWindow();
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	//}}AFX_VIRTUAL

	protected:
	virtual void DeleteEntry(UINT nIndx);

	//{{AFX_MSG(CAccountsListCtrl)
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	private:
	virtual UINT DefaultColumnWidth(UINT aColumnIndex) const;
	void ProcessColumnClick(size_t nColIndex);
	ICashflowListCtrl* m_pCashFlowListCtrl;
	IConfigDataManager & m_rConfigDataManager;

	test_protected: 
	virtual CString MySettingsCategory() const { return "AccountsList"; }

	private:
	friend class CAccntsPgDlgTest;
	friend class AccountsListCtrlTester;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACCOUNTCTRL_H__729531FF_4651_4C0A_8AEA_0CEBFA504C96__INCLUDED_)
