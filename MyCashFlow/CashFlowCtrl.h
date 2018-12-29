// (C) 2004 Wesley Steiner

#if !defined(AFX_CASHFLOWCTRL_H__61821FEB_EE56_465A_B37E_AE9AD074055B__INCLUDED_)
#define AFX_CASHFLOWCTRL_H__61821FEB_EE56_465A_B37E_AE9AD074055B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseListCtrl.h"
#include "ICashflowListCtrl.h"

class CDate; 

interface IAccountsListCtrl;
interface ITransListCtrl;

class CCashFlowListCtrl : public CBaseListCtrl, public ICashflowListCtrl
	{
	private:
	IAccountsListCtrl * myAccountListCtrl;
	ITransListCtrl * myTransactionListCtrl;

	void UpdateEntryBalance(UINT iIndex);
	int BSearch(int left, int right, const CDate& date) const;
	int GetInsertPos(const CDate& date) const;

	public:
	struct column 
		{ 
		enum { COUNT = 4 }; 
		static UINT WIDTHS[];
		};

	struct Column { struct index 
		{
		SCM(int, DATE, 0);
		SCM(int, DESC, 1);
		SCM(int, AMOUNT, 2);
		SCM(int, BALANCE, 3);
		};
	};

	struct Limits 
		{ 
		struct Duration 
			{
			SCM(size_t, MINIMUM, 1);
			SCM(size_t, MAXIMUM, 99);
			SCM(size_t, DEFAULT, 13);
			};
		};

	struct ItemData
		{ 
		ItemData(CDate const & date, TTransactionCookie cookie) : date(date), transaction_cookie(cookie) { }
		CDate date;
		TTransactionCookie transaction_cookie;
		};

	CCashFlowListCtrl(IConfigDataManager & configuration, IAccountsListCtrl* account_list, ITransListCtrl* transaction_list,
		quantity duration = Limits::Duration::DEFAULT);
	virtual ~CCashFlowListCtrl();

	virtual void Rebuild(); // Rebuilds the entire list from scratch.

	CDate GetStartDate() const { return theStartDate; }
	size_t GetDuration() const { return this->duration; }
	
	void SetDuration(size_t months) 
		{ 
		ASSERT(months >= Limits::Duration::MINIMUM && months <= Limits::Duration::MAXIMUM);
		this->duration = months; 
		}

	void ChangeDuration(size_t months);
	int AddCurrBalEntry(); // Adds the default Current Balance entry to the list.
	virtual bool IsTransaction(UINT iIndex) const;

	test_virtual void EditStartDate();
	test_virtual void SetStartDate(const CDate &date);

	// ICashflowListCtrl interface ...
	quantity GetItemCount() const { return CBaseListCtrl::GetItemCount(); }
	appl::Dollar::Value::type GetItemBalance(int index) const;
	CDate GetItemDate(UINT index) const;
	void RefreshBalance(UINT nStart = 0);
	int AddEntry(const CDate& date, LPCTSTR description, appl::Dollar::Value::type amount, TTransactionCookie cookie = NULL_TRANSACTION_COOKIE, BOOL update = TRUE);
	CDate GetEndDate() const { return theStartDate + (31 * this->duration); }
	TTransactionCookie GetItemCookie(int index) const { return GetItemDataPtr(index)->transaction_cookie; }
	void RemoveItem(int index);
	virtual void EditEntry(UINT nIndex);
	
	protected: virtual void DeleteEntry(UINT nIndex) { }
	protected: virtual UINT DefaultColumnWidth(UINT aColumnIndex) const;
	test_protected: virtual CString MySettingsCategory() const { return "CashflowList"; }

	protected:
	//{{AFX_MSG(CCashFlowListCtrl)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	private:
	virtual DWORD GetItemDataV(int nItem) const { return GetItemData(nItem); }
	ItemData * GetItemDataPtr(UINT nItem) const { return (r_cast<ItemData*> (GetItemDataV(nItem))); }
	void Clear();
	quantity duration;

	test_private: 
	test_virtual CDate CCashFlowListCtrl::GetStartDateFromUser();
	static CDate theStartDate;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCashFlowListCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	friend class CCashflowPageDialogFixture;
	friend class CCashFlowListCtrlFixture;
	friend class CashflowListCtrlTester;
	friend class CCashflowPageDialogTest;
	friend class CMyCashflowAppTestFixture;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CASHFLOWCTRL_H__61821FEB_EE56_465A_B37E_AE9AD074055B__INCLUDED_)
