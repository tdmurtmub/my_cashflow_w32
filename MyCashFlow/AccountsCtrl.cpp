// (C) 2006 Wesley Steiner

#include "stdafx.h"

#include "AccountsCtrl.h"

#include <numeric>

#include "mfc\ConfigDataManager.h"
#include "AccountCookie.h"
#include "AccountEditDlg.h"
#include "CashFlowCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

bool CAccountsListCtrl::Column::sort::Ascending[CAccountsListCtrl::Column::Index::COUNT] = {false};
UINT CAccountsListCtrl::column::WIDTHS[CAccountsListCtrl::Column::Index::COUNT] = {200, 60};

CAccountsListCtrl::CAccountsListCtrl(IConfigDataManager & rConfigDataManager, ICashflowListCtrl* pCashFlowListCtrl) : 
	CBaseListCtrl(rConfigDataManager),
	m_pCashFlowListCtrl(pCashFlowListCtrl),
	m_rConfigDataManager(rConfigDataManager)
	{
	}

CAccountsListCtrl::~CAccountsListCtrl()
	{
	}

BEGIN_MESSAGE_MAP(CAccountsListCtrl, CBaseListCtrl)
	//{{AFX_MSG_MAP(CAccountsListCtrl)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CAccountsListCtrl::AddEntry(LPCTSTR pDescription, appl::Dollar::Value::type aDollarAmount /* = 0 */,
	bool make_current /* = false */)
	// Adds a new entry to the list.
	// Returns the zero-based index of the entry in the list or -1 if error.
	{
	int index = InsertItem(0, pDescription);

	if (index != -1) 
		{
		VERIFY(SetItemText(index, Column::Index::AMOUNT, appl::Dollar::Value::ToStr(aDollarAmount)));

		// update the data field ...
		AccountData * pData = new AccountData(pDescription, aDollarAmount);
		WAXONZ(SetItemData(index, (DWORD) pData));

		// Make the new entry the current selection ...
		if (make_current) WAXONZ(SetItemState(index, LVIS_SELECTED, LVIS_SELECTED));

		m_pCashFlowListCtrl->RefreshBalance();
		}

	return index;
	}

void CAccountsListCtrl::UpdateEntry(UINT nItem, LPCTSTR pDescription, appl::Dollar::Value::type aDollarAmount /* = 0 */ )
{
	ASSERT(nItem < (UINT) GetItemCount());
	ASSERT(pDescription);

	VERIFY(SetItemText(nItem, Column::Index::DESC, pDescription));
	VERIFY(SetItemText(nItem, Column::Index::AMOUNT, appl::Dollar::Value::ToStr(aDollarAmount)));
	AccountData* pData = (AccountData*) GetItemData(nItem);
	pData->myDescription = pDescription;
	pData->myAmount = aDollarAmount;

	m_pCashFlowListCtrl->RefreshBalance();
}

appl::Dollar::Value::type CAccountsListCtrl::GetBalance() const
{
	appl::Dollar::Value::type vBal = 0;
	if (IsWindow(m_hWnd)) 
	{
		for (int iItem = 0; iItem < GetItemCount(); ++iItem)
		{
			vBal += appl::Dollar::Value::FromStr(GetItemText(iItem, Column::Index::AMOUNT));
		}
	}
	return vBal;
}

void CAccountsListCtrl::DeleteEntry(UINT nItem)
	{
	ASSERT(nItem < (UINT) GetItemCount());
	VERIFY(DeleteItem(nItem));
	m_pCashFlowListCtrl->RefreshBalance();
	}

void CAccountsListCtrl::EditEntry(UINT nIndex)
	{
	ASSERT(nIndex < (UINT) GetItemCount());
	CAccountEditDlg dlg(CAccountEditDlg::Mode::UPDATE);
	dlg.myDescription = GetItemText(nIndex, Column::Index::DESC);
	dlg.m_nBalance = appl::Dollar::Value::FromStr(GetItemText(nIndex, Column::Index::AMOUNT));
	if (dlg.DoModal() == IDOK) 
		{
		UpdateEntry(nIndex, dlg.myDescription, dlg.m_nBalance);
		}
	}

void CAccountsListCtrl::PreSubclassWindow() 
{
	// Set up the column headers.
	CString str;
	VERIFY(str.LoadString(IDS_TRANXCOLHDRDESC));
	InsertColumn(0, str, LVCFMT_LEFT, ColumnWidth(0));
	VERIFY(str.LoadString(IDS_ACCNTSCOLHDRBAL));
	InsertColumn(1, str, LVCFMT_RIGHT, ColumnWidth(1));

	CBaseListCtrl::PreSubclassWindow();
}

static int CALLBACK 
ColumnCompareCallback(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	// lParamSort is the column # to sort on
	ASSERT(lParamSort < CAccountsListCtrl::Column::Index::COUNT);

	// lParam1, lParam2 are the data fields of each item
	ASSERT(lParam1);
	ASSERT(lParam2);
	AccountData* pData1 = (AccountData*) lParam1;
	AccountData* pData2 = (AccountData*) lParam2;

	int result = 0;
	switch (lParamSort)
	{
	case CAccountsListCtrl::Column::Index::DESC:
		result = stricmp(pData1->myDescription, pData2->myDescription);
		break;
	case CAccountsListCtrl::Column::Index::AMOUNT:
		result = pData1->myAmount - pData2->myAmount;
		break;
	default:
		ASSERT(FALSE); // should not happen
	}

	return (CAccountsListCtrl::Column::sort::Ascending[lParamSort]) ? result : -result;
}

void CAccountsListCtrl::ProcessColumnClick(size_t nColIndex) 
{
	ASSERT(nColIndex < CAccountsListCtrl::Column::Index::COUNT);

	SortItems(ColumnCompareCallback, nColIndex);

	// toggle the sort direction ...
	CAccountsListCtrl::Column::sort::Ascending[nColIndex] = !CAccountsListCtrl::Column::sort::Ascending[nColIndex];
}

void CAccountsListCtrl::OnDestroy() 
{
	// deallocate list item data objects ...
	for (int nItem = 0; nItem < GetItemCount(); ++nItem)
	{
		delete r_cast<TAccountCookie> (GetItemData(nItem));
	}

	CBaseListCtrl::OnDestroy();
}

appl::Dollar::Value::type CAccountsListCtrl::GetItemAmount(int iItem) const
{
	ASSERT(iItem >= 0);
	ASSERT(iItem < GetItemCount());
	return appl::Dollar::Value::FromStr(GetItemText(iItem, Column::Index::AMOUNT));
}

BOOL CAccountsListCtrl::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
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

UINT CAccountsListCtrl::DefaultColumnWidth(UINT aColumnIndex) const
	{
	ASSERT(aColumnIndex < Column::Index::COUNT);
	return column::WIDTHS[aColumnIndex];
	}
