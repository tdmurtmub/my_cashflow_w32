// (C) 2004 Wesley Steiner

#include "stdafx.h"

#include "CashFlowCtrl.h"

#include "mfc\ConfigDataManager.h"
#include "AccountsCtrl.h"
#include "EditStartDateDlg.h"
#include "IAccountsListCtrl.h"
#include "TrnxListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;
using namespace app;

CDate CCashFlowListCtrl::theStartDate = CDate::Today();
UINT CCashFlowListCtrl::column::WIDTHS[COUNT] = {Appl::column::Width::DATE, 130, Appl::column::Width::AMOUNT, Appl::column::Width::BALANCE};

CCashFlowListCtrl::CCashFlowListCtrl(IConfigDataManager &configuration, IAccountsListCtrl *account_list, ITransListCtrl *transaction_list,
	quantity duration /* = Limits::Duration::DEFAULT */ ) :
	CBaseListCtrl(configuration),
	myAccountListCtrl(account_list), 
	myTransactionListCtrl(transaction_list),
	duration(duration)
	{
	ASSERT(duration >= Limits::Duration::MINIMUM && duration <= Limits::Duration::MAXIMUM);
	}

CCashFlowListCtrl::~CCashFlowListCtrl()
	{
	}

BEGIN_MESSAGE_MAP(CCashFlowListCtrl, CBaseListCtrl)
	//{{AFX_MSG_MAP(CCashFlowListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CCashFlowListCtrl::BSearch(int left, int right, const CDate& date) const
	{
	if ((right - left) == 0) // 1 item
		return (date >= GetItemDate(right)) ? right + 1 : left;

	int iSplit = left + (right - left) / 2;
	if (date >= GetItemDate(iSplit))
		return BSearch(iSplit + 1, right, date);
	else
		return BSearch(left, iSplit, date);
	}

int CCashFlowListCtrl::GetInsertPos(const CDate& date) const
// Returns the index in the list where date should be inserted.
	{ 
	if (GetItemCount() == 0) return 0;
	return BSearch(0, GetItemCount() - 1, date);
	}

int CCashFlowListCtrl::AddEntry(const CDate &date, LPCTSTR description, appl::Dollar::Value::type amount, 
	TTransactionCookie cookie /* = NULL_TRANSACTION_COOKIE */,
	BOOL update /* = TRUE */)
	// Adds a single entry to the list.
	// Returns the zero-based index of the entry in the list or -1 if error.
	{
	int index = -1;

	// Ignore all entries outside the current window of valid dates.
	if ((date >= theStartDate) && (date < GetEndDate()))
		{
		// Insert the new entry.
		index = InsertItem(GetInsertPos(date), date.ToString()); ASSERT(index >= 0);
		SetItemText(index, Column::index::AMOUNT, appl::Dollar::Value::ToStr(amount));
		SetItemText(index, Column::index::DESC, description);
		VERIFY(SetItemData(index, r_cast<DWORD> (new ItemData(date, cookie))) != LB_ERR);
		if (update) RefreshBalance(index);
		}
	return index;
	}

CDate CCashFlowListCtrl::GetItemDate(UINT aIndex) const
	{
	ASSERT(aIndex < GetItemCount());
	return GetItemDataPtr(aIndex)->date;
	}

void CCashFlowListCtrl::UpdateEntryBalance(UINT index)
// Updates the balance column for a single entry.
	{
	ASSERT(index < GetItemCount());
	if (index == 0) SetItemText(index, Column::index::AMOUNT, appl::Dollar::Value::ToStr(myAccountListCtrl->GetBalance()));
	appl::Dollar::Value::type vBal = 0;
	if (index > 0) 
	{
		vBal = appl::Dollar::Value::FromStr(GetItemText(index - 1, Column::index::BALANCE));
	}
	vBal += appl::Dollar::Value::FromStr(GetItemText(index, Column::index::AMOUNT));
	SetItemText(index, Column::index::BALANCE, appl::Dollar::Value::ToStr(vBal));
	}

int CCashFlowListCtrl::AddCurrBalEntry()
{
	CString str;
	str.LoadString(IDS_CURRENTACCOUNTS);
	return AddEntry(theStartDate, str, myAccountListCtrl->GetBalance());
}

void CCashFlowListCtrl::RefreshBalance(UINT nStart /* = 0 */ )
	// Updates the balance of all entries starting from nStart.
	{
	ASSERT(nStart >= 0);

	// Add the Current Balance entry if its not already there.
	if (GetItemCount() == 0) AddCurrBalEntry();

	ASSERT(nStart < GetItemCount());
	for (; nStart < (int) GetItemCount(); ++nStart) UpdateEntryBalance(nStart);
	Appl::Chart::Refresh();
	}

void CCashFlowListCtrl::Clear()
	{
	for (int i = 0; i < (int) GetItemCount(); ++i) delete GetItemDataPtr(i);
	DeleteAllItems();
	}

void CCashFlowListCtrl::Rebuild()
	{
	Clear();
	AddCurrBalEntry();
	ASSERT(myTransactionListCtrl);
	myTransactionListCtrl->InsertAllCashFlowEntries();
	}

void CCashFlowListCtrl::PreSubclassWindow() 
{
	CString str;
	WAXONZ(str.LoadString(IDS_CASHFLOWCOLHDRDATE));
	InsertColumn(Column::index::DATE, str, LVCFMT_LEFT, ColumnWidth(Column::index::DATE));
	WAXONZ(str.LoadString(IDS_TRANXCOLHDRDESC));
	InsertColumn(Column::index::DESC, str, LVCFMT_LEFT, ColumnWidth(Column::index::DESC));
	WAXONZ(str.LoadString(IDS_TRANXCOLHDRAMNT));
	InsertColumn(Column::index::AMOUNT, str, LVCFMT_RIGHT, ColumnWidth(Column::index::AMOUNT));
	WAXONZ(str.LoadString(IDS_CASHFLOWCOLHDRBAL));
	InsertColumn(Column::index::BALANCE, str, LVCFMT_RIGHT, ColumnWidth(Column::index::BALANCE));
	AddCurrBalEntry();
	CBaseListCtrl::PreSubclassWindow();
}

void CCashFlowListCtrl::SetStartDate(const CDate& date) 
	{ 
	theStartDate = date; 
	Rebuild();
	}

appl::Dollar::Value::type CCashFlowListCtrl::GetItemBalance(int index) const
	{
	return appl::Dollar::Value::FromStr(GetItemText(index, Column::index::BALANCE));
	}

void CCashFlowListCtrl::ChangeDuration(size_t months)
	{
	SetDuration(months);
	Rebuild();
	}

bool CCashFlowListCtrl::IsTransaction(UINT index) const 
// Returns true if the list item at index is a transaction based entry.
	{ 
	ASSERT(index >= 0);
	ASSERT(index < GetItemCount());
	return (GetItemDataPtr(index)->transaction_cookie) != NULL_TRANSACTION_COOKIE; 
	}

void CCashFlowListCtrl::EditEntry(UINT index)
	{
	ASSERT(index >= 0);
	if (index == 0) EditStartDate();
	else if (IsTransaction(index)) myTransactionListCtrl->EditEntry(myTransactionListCtrl->FindItemByCookie(GetItemDataPtr(index)->transaction_cookie));
	}

CDate CCashFlowListCtrl::GetStartDateFromUser() 
	{
	CEditStartDateDlg aDialog(theStartDate);
	aDialog.DoModal(); 
	return aDialog.m_aStartDate;
	}

void CCashFlowListCtrl::EditStartDate()
	{	
	CDate date = GetStartDateFromUser();
	if (date != theStartDate) SetStartDate(date);
	}

UINT CCashFlowListCtrl::DefaultColumnWidth(UINT aColumnIndex) const
	{
	ASSERT(aColumnIndex < column::COUNT);
	return column::WIDTHS[aColumnIndex];
	}

void CCashFlowListCtrl::RemoveItem(int index) 
	{
	delete GetItemDataPtr(index);
	DeleteItem(index); 
	}
