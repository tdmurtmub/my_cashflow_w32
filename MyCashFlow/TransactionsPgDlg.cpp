#include "stdafx.h"

#include "TransactionsPgDlg.h"

#include "MyMoney.h"

#include <win\mfc.h>
#include <win\mfc\Markup.h>

#include "CashFlowPgDlg.h"
#include "TransactionData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace app;
using namespace win;
using namespace win::mfc;

CTransactionsPageDialog::CTransactionsPageDialog(IConfigDataManager & rConfigDataManager, ICashflowListCtrl * pCashFlowListCtrl, CWnd* pParent /*=NULL*/ )
	: CListBasedPage(CTransactionsPageDialog::IDD, m_aList),
	m_aList(rConfigDataManager, pCashFlowListCtrl)
{
	//{{AFX_DATA_INIT(CTransactionsPageDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CTransactionsPageDialog::DoDataExchange(CDataExchange* pDX)
{
	CListBasedPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransactionsPageDialog)
	DDX_Control(pDX, IDC_TLIST, m_aList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTransactionsPageDialog, CListBasedPage)
	//{{AFX_MSG_MAP(CTransactionsPageDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CTransactionsPageDialog::OnInitDialog() 
	{
	CListBasedPage::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void CTransactionsPageDialog::PutMarkupData(CMarkup & rMarkup) const
{
	if (m_aList.GetItemCount() > 0)
	{
		VERIFY(rMarkup.AddChildElem("TRANSACTIONS"));
		VERIFY(rMarkup.IntoElem());
		for (int iItem = 0; iItem < m_aList.GetItemCount(); ++iItem)
		{
			TransactionData* data = r_cast<TransactionData*> (m_aList.GetItemData(iItem)); ASSERT(data);
			VERIFY(rMarkup.AddChildElem("TRANSACTION"));
			VERIFY(rMarkup.AddChildAttrib("Type", m_aList.GetItemText(iItem, CTransListCtrl::Column::Index::REPEAT)));
			VERIFY(rMarkup.IntoElem());
			VERIFY(rMarkup.AddChildElem("DESCRIPTION", m_aList.GetItemText(iItem, CTransListCtrl::Column::Index::DESC)));
			VERIFY(rMarkup.AddChildElem("DATE", convert::DateToText(data->myStartDate)));
			VERIFY(rMarkup.AddChildElem("AMOUNT", m_aList.GetItemText(iItem, CTransListCtrl::Column::Index::AMOUNT)));
			CDateRepeater* pRepeater = data->m_pRepeater; ASSERT(pRepeater);
			VERIFY(rMarkup.AddChildElem("REPEAT"));
			VERIFY(rMarkup.AddChildAttrib("Type", m_aList.GetItemText(iItem, CTransListCtrl::Column::Index::REPEAT)));
			VERIFY(rMarkup.AddChildAttrib("Delta", (LPCTSTR) ToCString(pRepeater->IntervalDelta())));
			VERIFY(rMarkup.AddChildAttrib("Count", (LPCTSTR) ToCString(pRepeater->Count())));
			if (pRepeater->IntervalType() == CDateRepeater::Interval::MONTH)
			{
				rMarkup.AddChildAttrib("Day", (LPCTSTR) ToCString(s_cast<CDateRepeaterMonthly*> (pRepeater)->DayOfMonth()));
			}
			VERIFY(rMarkup.OutOfElem());
		}
		VERIFY(rMarkup.OutOfElem());
	}
}

void CTransactionsPageDialog::OnEditDelSelEntry()
	{
	int iSelected = m_aList.GetSelectedItemIndex(); ASSERT(iSelected != -1);
	m_aList.TryDeleteEntry(iSelected);
	}
