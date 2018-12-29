#if !defined(AFX_TRNXLISTCTRL_H__2E36766F_00AB_4A62_ACC3_ABD5FB50747A__INCLUDED_)
#define AFX_TRNXLISTCTRL_H__2E36766F_00AB_4A62_ACC3_ABD5FB50747A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <win\mfc\cdate.h>
#include "BaseListCtrl.h"
#include "ITransListCtrl.h"
#include "MyMoney.h"

struct ICashflowListCtrl;
class IConfigDataManager;

class CTransListCtrl : public CBaseListCtrl, public ITransListCtrl
	{
	public:

	struct Column 
		{ 
		struct Index { enum 
			{
			DESC,
			DATE,
			AMOUNT,
			REPEAT,
			COUNT
			};};
		struct sort
			{
			static bool Ascending[Index::COUNT]; // records next sort direction for each column
			};
		};

	struct column 
		{ 
		static UINT WIDTHS[Column::Index::COUNT];
		};

	CTransListCtrl(IConfigDataManager & rConfigDataManager, ICashflowListCtrl * pCashFlowListCtrl);
	virtual ~CTransListCtrl();

	CDate GetItemDate(UINT iItem) const;
	appl::Dollar::Value::type GetItemAmount(int iItem) const;

	static CString GetDisplayStr(const CDateRepeater& ptDateRepeater)
		// Returns the string to be displayed in the Date column.
		{
		switch (ptDateRepeater.IntervalType())
			{
			case CDateRepeater::Interval::NONE: return "Once";
			case CDateRepeater::Interval::DAY: return "Daily";
			case CDateRepeater::Interval::WEEK: return "Weekly";
			case CDateRepeater::Interval::MONTH: return "Monthly";
			case CDateRepeater::Interval::YEAR: return "Yearly";
			default: ASSERT(FALSE);
			}
		return "<none>";
		}

	int AddEntry(const CDate& oStartDate, LPCTSTR description, 
		appl::Dollar::Value::type amount, 
		CDateRepeater* repeat = new CDateRepeaterOnce(), bool make_current = true);

	virtual void EditEntry(UINT nIndex);

	void UpdateEntry(int nItem, const CDate& oStartDate, LPCTSTR description, 
		appl::Dollar::Value::type amount, 
		CDateRepeater* repeat = new CDateRepeaterOnce());

	virtual void InsertAllCashFlowEntries() const;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransListCtrl)
	public:
	virtual void PreSubclassWindow();
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	//}}AFX_VIRTUAL

	protected:
	virtual void DeleteEntry(UINT nIndx);
	virtual UINT DefaultColumnWidth(UINT aColumnIndex) const;

	test_protected:
	virtual CString MySettingsCategory() const { return "TransactionsList"; }

	// Generated message map functions
	protected:
	//{{AFX_MSG(CTransListCtrl)
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	private:
	void ProcessColumnClick(size_t nColIndex);
	void AddToCashFlow(int nItem) const;
	void DeleteFromCashFlow(TTransactionCookie aTransactionCookie, BOOL bUpdateBal = TRUE) const;
	int FindItemByCookie(TTransactionCookie aTransactionCookie) const; 
	TransactionData * CTransListCtrl::GetItemDataPtr(UINT nIndex) const;

	private:
	ICashflowListCtrl * m_pCashFlowListCtrl;
	IConfigDataManager & m_rConfigDataManager;

	// unit testing ...
	private:
	friend class CTransPgDlgTest;
	friend class CTranxListCtrlTest;
	friend class TransListCtrlTester;
	};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRNXLISTCTRL_H__2E36766F_00AB_4A62_ACC3_ABD5FB50747A__INCLUDED_)
