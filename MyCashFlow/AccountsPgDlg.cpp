// Copyright (C) 2004-2006 by Wesley Steiner. All rights reserved.

#include "stdafx.h"

#include "AccountsPgDlg.h"

#include <Win\MFC\Markup.h>

#include "AccountsCtrl.h"
#include "CashFlowPgDlg.h"
#include "MyMoney.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

CAccountsPageDialog::CAccountsPageDialog(IConfigDataManager & rConfigDataManager, ICashflowListCtrl* pCashFlowListCtrl, CWnd* pParent /*=NULL*/ ) : 
	CListBasedPage(CAccountsPageDialog::IDD, m_aList),
	m_aList(rConfigDataManager, pCashFlowListCtrl)
{
	//{{AFX_DATA_INIT(CAccountsPageDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAccountsPageDialog::~CAccountsPageDialog()
{
}

void CAccountsPageDialog::DoDataExchange(CDataExchange* pDX)
{
	inherited::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAccountsPageDialog)
	DDX_Control(pDX, IDC_ALIST, m_aList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAccountsPageDialog, CListBasedPage)
	//{{AFX_MSG_MAP(CAccountsPageDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAccountsPageDialog::OnInitDialog() 
{
	inherited::OnInitDialog();
	
	// Current Accounts must be loaded before CashFlow InitDialog is called
	// since it needs the balance for its first entry.

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CAccountsPageDialog::PutMarkupData(CMarkup& rMarkup) const 
	{ 
	if (m_aList.GetItemCount() > 0)
		{
		VERIFY(rMarkup.AddChildElem("ACCOUNTS"));
		VERIFY(rMarkup.IntoElem());
		for (int iItem = 0; iItem < m_aList.GetItemCount(); ++iItem)
			{
			VERIFY(rMarkup.AddChildElem("ACCOUNT"));
			VERIFY(rMarkup.IntoElem());
			VERIFY(rMarkup.AddChildElem("DESCRIPTION", 
				m_aList.GetItemText(iItem, CAccountsListCtrl::Column::Index::DESC)));
			VERIFY(rMarkup.AddChildElem("AMOUNT", 
				m_aList.GetItemText(iItem, CAccountsListCtrl::Column::Index::AMOUNT)));
			VERIFY(rMarkup.OutOfElem());
			}
		VERIFY(rMarkup.OutOfElem());
		}
	}

void CAccountsPageDialog::OnEditDelSelEntry()
	{
	int iSelected = m_aList.GetSelectedItemIndex(); ASSERT(iSelected != -1);
	m_aList.TryDeleteEntry(iSelected);
	}
