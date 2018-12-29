// (C) 2004 Wesley Steiner

#include "stdafx.h"

#include "TrnxListCtrl.h"

#include <commctrl.h>

#include "CashFlowPgDlg.h"
#include "mfc\ConfigDataManager.h"
#include "MyMoney.h"
#include "TransactionData.h"
#include "XEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;
using namespace app;

UINT CTransListCtrl::column::WIDTHS[] = {160, Appl::column::Width::DATE, Appl::column::Width::AMOUNT, 60};
bool CTransListCtrl::Column::sort::Ascending[CTransListCtrl::Column::Index::COUNT] = {false};

CTransListCtrl::CTransListCtrl(IConfigDataManager & rConfigDataManager, ICashflowListCtrl * pCashFlowListCtrl) :
	CBaseListCtrl(rConfigDataManager),
	m_pCashFlowListCtrl(pCashFlowListCtrl),
	m_rConfigDataManager(rConfigDataManager)
{
}

CTransListCtrl::~CTransListCtrl()
{
}

void CTransListCtrl::UpdateEntry(int nItem, const CDate& rStartDate, LPCTSTR description, 
	appl::Dollar::Value::type vAmt, 
	CDateRepeater* repeat /* = new CDateRepeaterOnce() */ )
// Updates an existing entry in the list.
// NOTE: Gains ownership of *repeat.
	{
	ASSERT(nItem < GetItemCount());

	VERIFY(SetItemText(nItem, Column::Index::DESC, description));
	VERIFY(SetItemText(nItem, Column::Index::DATE, rStartDate.ToString()));
	VERIFY(SetItemText(nItem, Column::Index::AMOUNT, appl::Dollar::Value::ToStr(vAmt)));

	TTransactionCookie cookie = r_cast<TTransactionCookie> (GetItemData(nItem));

	// assign the new data item ...

	VERIFY(SetItemData(nItem, r_cast<LPARAM> (new TransactionData(rStartDate, description, vAmt, repeat))));
	VERIFY(SetItemText(nItem, Column::Index::REPEAT, GetDisplayStr(*repeat)));

	// update/refresh the cash flow ...

	DeleteFromCashFlow(cookie, FALSE);
	AddToCashFlow(nItem);

	cookie->m_pRepeater = NULL; // prevent double delete
	delete cookie;
	}

void CTransListCtrl::AddToCashFlow(int nItem) const
// Updates the cashflow by inserting new entries for a given transaction.
// Assumes the transaction has not been added yet. 
	{
	ASSERT((nItem >= 0) && (nItem < GetItemCount()));

	// Cache the column data for this transaction ...
	CString oDesc = GetItemText(nItem, Column::Index::DESC);
	appl::Dollar::Value::type amount = GetItemAmount(nItem);
	TTransactionCookie aCookie = r_cast<TTransactionCookie> (GetItemData(nItem));
	TransactionData* pTransactionData = aCookie;
	CDateRepeater* repeat = pTransactionData->m_pRepeater;
	CDate aStartDate = pTransactionData->myStartDate;

	// do not process zero value transactions ...
	if (amount != appl::Dollar::Value::ZERO)
	{
		// Iterate over the date repeater inserting entries into the cashflow list ...

		CDate iDate(repeat->Next(aStartDate, 0));
		size_t iRepCnt = 0;
		while ((repeat->Count() == 0) || (iRepCnt < repeat->Count()) )
		{
			int iAt = m_pCashFlowListCtrl->AddEntry(iDate, oDesc, amount, aCookie, FALSE /* no update */ );
			iDate = repeat->Next(aStartDate, ++iRepCnt);

			// Break if we have hit the cash-flow ending date.
			if (iDate >= m_pCashFlowListCtrl->GetEndDate()) break;
		}
	}

	// Update the balance once after we have completed all inserts.
	m_pCashFlowListCtrl->RefreshBalance();
	}

int CTransListCtrl::AddEntry(const CDate &rStartDate, LPCTSTR description, 
	appl::Dollar::Value::type amount,
	CDateRepeater* repeat /* = new CDateRepeaterOnce() */,
	bool make_current /* = true */)
// Adds a new transaction to the list.
// Returns the zero-based index of the entry in the list or -1 if error.
// NOTE: Gains ownership of *repeat.
{
	ASSERT(IsWindow(m_hWnd));
	ASSERT(Column::Index::DESC == 0);
	ASSERT(description);
	ASSERT(repeat != NULL);

	int iInsertedAt = InsertItem(LVIF_TEXT | LVIF_PARAM, 0, description,
		0, // state
		0, // state mask
		0, // image
		r_cast<LPARAM> (new TransactionData(rStartDate, description, amount, repeat)));
	ASSERT(iInsertedAt != -1);
	VERIFY(SetItemText(iInsertedAt, Column::Index::DATE, rStartDate.ToString()));
	VERIFY(SetItemText(iInsertedAt, Column::Index::AMOUNT, appl::Dollar::Value::ToStr(amount)));
	VERIFY(SetItemText(iInsertedAt, Column::Index::REPEAT, GetDisplayStr(*repeat)));

	// Make the new entry the current selection.
	if (make_current) WAXONZ(SetItemState(iInsertedAt, LVIS_SELECTED, LVIS_SELECTED));

	AddToCashFlow(iInsertedAt);
	return iInsertedAt;
}

BEGIN_MESSAGE_MAP(CTransListCtrl, CBaseListCtrl)
	//{{AFX_MSG_MAP(CTransListCtrl)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CTransListCtrl::DeleteFromCashFlow(TTransactionCookie aTransactionCookie, BOOL bUpdateBal /* = TRUE */) const
// Delete all entries from the cash flow that match lData.
{
	ASSERT(aTransactionCookie != NULL_TRANSACTION_COOKIE);
	UINT nRefreshFrom = m_pCashFlowListCtrl->GetItemCount();
	UINT nItem = 0;
	while (nItem < m_pCashFlowListCtrl->GetItemCount())
	{
		if (m_pCashFlowListCtrl->GetItemCookie(nItem) == aTransactionCookie) 
		{
			nRefreshFrom = min(nRefreshFrom, nItem);
			m_pCashFlowListCtrl->RemoveItem(nItem);
		}
		else
		{
			++nItem;
		}
	}
	if ((bUpdateBal) && (nRefreshFrom < m_pCashFlowListCtrl->GetItemCount())) 
		m_pCashFlowListCtrl->RefreshBalance(nRefreshFrom);
}

void CTransListCtrl::DeleteEntry(uint nIndx)
{
	ASSERT(nIndx < (UINT) GetItemCount());
	DeleteFromCashFlow(r_cast<TTransactionCookie> (GetItemData(nIndx)));
	VERIFY(DeleteItem(nIndx));
}

TransactionData * CTransListCtrl::GetItemDataPtr(UINT nIndex) const
	{
	return r_cast<TransactionData *> (GetItemData(nIndex));
	}

void CTransListCtrl::EditEntry(UINT nIndex)
// Edit a single item from the list.
{
	CTransEditDlg aDialog(CTransEditDlg::Mode::UPDATE);

	aDialog.InitializeControls(
		GetItemDataPtr(nIndex)->myStartDate,
		GetItemText(nIndex, Column::Index::DESC),
		GetItemAmount(nIndex),
		GetItemDataPtr(nIndex)->m_pRepeater);

	if (aDialog.DoModal() == IDOK) 
	{
		UpdateEntry(nIndex, aDialog.m_aStartDate, aDialog.my_description, 
			(aDialog.m_iDebitGrp == 0) ? -(int) aDialog.m_nAmt : aDialog.m_nAmt, 
			aDialog.CreateDateRepeater());
	}
}

void CTransListCtrl::InsertAllCashFlowEntries() const
{
	for (int nItem = 0; nItem < GetItemCount(); ++nItem)
	{	
		AddToCashFlow(nItem);
	}
}

void CTransListCtrl::OnDestroy() 
{
	// deallocate list item data objects ...

	for (int nItem = 0; nItem < GetItemCount(); ++nItem)
	{
		delete r_cast<TTransactionCookie> (GetItemData(nItem));
	}

	CBaseListCtrl::OnDestroy();
}

int CTransListCtrl::FindItemByCookie(TTransactionCookie cookie) const 
{ 
	for (int nItem = 0; nItem < GetItemCount(); ++nItem)
	{	
		if (r_cast<TTransactionCookie> (GetItemData(nItem)) == cookie) return nItem;
	}
	return -1;
}

void CTransListCtrl::PreSubclassWindow() 
	{
	CString str;
	VERIFY(str.LoadString(IDS_TRANXCOLHDRDATE));
	InsertColumn(Column::Index::DATE, str, LVCFMT_LEFT, ColumnWidth(Column::Index::DATE));
	VERIFY(str.LoadString(IDS_TRANXCOLHDRDESC));
	InsertColumn(Column::Index::DESC, str, LVCFMT_LEFT, ColumnWidth(Column::Index::DESC));
	VERIFY(str.LoadString(IDS_TRANXCOLHDRAMNT));
	InsertColumn(Column::Index::AMOUNT, str, LVCFMT_RIGHT, ColumnWidth(Column::Index::AMOUNT));
	VERIFY(str.LoadString(IDS_TRANXCOLHDRREPEAT));
	InsertColumn(Column::Index::REPEAT, str, LVCFMT_RIGHT, ColumnWidth(Column::Index::REPEAT));
	CBaseListCtrl::PreSubclassWindow();
	}

BOOL CTransListCtrl::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
	LPNMLISTVIEW pNmListView = (LPNMLISTVIEW) lParam;
	switch (pNmListView->hdr.code)
	{
		case LVN_COLUMNCLICK:
			ProcessColumnClick(pNmListView->iSubItem);
			break;
	}
	return CBaseListCtrl::OnChildNotify(message, wParam, lParam, pLResult);
}

static int CALLBACK 
ColumnCompareCallback(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	// lParamSort is the column # to sort on
	ASSERT(lParamSort < CTransListCtrl::Column::Index::COUNT);

	// lParam1, lParam2 are the data fields of each item
	ASSERT(lParam1);
	ASSERT(lParam2);
	TransactionData* pData1 = (TransactionData*) lParam1;
	TransactionData* pData2 = (TransactionData*) lParam2;

	int result = 0;
	switch (lParamSort)
	{
	case CTransListCtrl::Column::Index::DATE:
		result = pData1->myStartDate < pData2->myStartDate ? -1 : pData1->myStartDate > pData2->myStartDate ? +1 : 0;
		break;
	case CTransListCtrl::Column::Index::DESC:
		result = stricmp(pData1->myDescription, pData2->myDescription);
		break;
	case CTransListCtrl::Column::Index::AMOUNT:
		result = pData1->m_aAmount - pData2->m_aAmount;
		break;
	case CTransListCtrl::Column::Index::REPEAT:
		result = pData1->m_pRepeater->IntervalType() - pData2->m_pRepeater->IntervalType();
		break;
	default: ASSERT(FALSE); // should not happen
	}

	return (CTransListCtrl::Column::sort::Ascending[lParamSort]) ? result : -result;
}

void CTransListCtrl::ProcessColumnClick(size_t nColIndex) 
	{
	ASSERT(nColIndex < CTransListCtrl::Column::Index::COUNT);
	SortItems(ColumnCompareCallback, nColIndex);
	// toggle the sort direction ...
	CTransListCtrl::Column::sort::Ascending[nColIndex] = !CTransListCtrl::Column::sort::Ascending[nColIndex];
	}

CDate CTransListCtrl::GetItemDate(UINT iItem) const
	{
	ASSERT(iItem < (UINT) GetItemCount());
	return GetItemDataPtr(iItem)->myStartDate;
	}

appl::Dollar::Value::type CTransListCtrl::GetItemAmount(int iItem) const
	{
	ASSERT(iItem >= 0);
	ASSERT(iItem < GetItemCount());
	return appl::Dollar::Value::FromStr(GetItemText(iItem, Column::Index::AMOUNT));
	}

UINT CTransListCtrl::DefaultColumnWidth(UINT aColumnIndex) const
	{
	ASSERT(aColumnIndex < Column::Index::COUNT);
	return column::WIDTHS[aColumnIndex];
	}
