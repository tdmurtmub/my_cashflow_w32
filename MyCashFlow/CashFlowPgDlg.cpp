// (C) 2004 Wesley Steiner

#include "stdafx.h"

#include "CashFlowPgDlg.h"

#include <limits>

#include "mfc\markup.h"
#include "mfc\wndinfo.h"

#include "IAccountsListCtrl.h"

#ifdef TEST
#include "mfc\ConfigDataManager.h"
#include <cppunit\extensions\HelperMacros.h>
#include "IAccountsListCtrl.h"
#include "ITransListCtrl.h"
#include "TransactionData.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace app;
using namespace std;
using namespace win::mfc;

CCashflowPageDialog::CCashflowPageDialog(IConfigDataManager & rConfigDataManager, 
	IAccountsListCtrl* pAccntsListCtrl, ITransListCtrl* pTranxListCtrl, 
	size_t nDurationInMonths /* = CCashFlowListCtrl::Limits::Duration::DEFAULT */,
	CWnd* pParent /*=NULL*/) : 
	CListBasedPage(CCashflowPageDialog::IDD, myList), 
	myList(rConfigDataManager, pAccntsListCtrl, pTranxListCtrl, nDurationInMonths)
{
	//{{AFX_DATA_INIT(CCashflowPageDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CCashflowPageDialog::DoDataExchange(CDataExchange* pDX)
{
	inherited::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCashflowPageDialog)
	DDX_Control(pDX, IDC_SPIN_DURATION, myDurationSpinCtrl);
	DDX_Control(pDX, IDC_XLIST, myList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCashflowPageDialog, CListBasedPage)
	//{{AFX_MSG_MAP(CCashflowPageDialog)
	ON_BN_CLICKED(IDC_BUTTONDECDATE, OnButtonDecDate)
	ON_BN_CLICKED(IDC_BUTTONINCDATE, OnButtonIncDate)
	ON_BN_CLICKED(IDC_STARTINGDATE, OnStartingDate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CCashflowPageDialog::OnInitDialog() 
{
	inherited::OnInitDialog();
	myDurationSpinCtrl.SetRange(
		CCashFlowListCtrl::Limits::Duration::MINIMUM, 
		CCashFlowListCtrl::Limits::Duration::MAXIMUM);
	myDurationSpinCtrl.SetBuddy(GetDlgItem(IDC_STATIC_MONTHS));
	myDurationSpinCtrl.SetPos(myList.GetDuration());
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CCashflowPageDialog::OnButtonDecDate() 
{
	myList.SetStartDate(myList.GetStartDate() - 1);
}

void CCashflowPageDialog::OnButtonIncDate() 
{
	myList.SetStartDate(myList.GetStartDate() + 1);
}

BOOL CCashflowPageDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
	{
//	TRACE("CCashflowPageDialog::OnNotify(%d)\n", wParam);
	if (wm::Notify(wParam, lParam, pResult).Code() == UDN_DELTAPOS)
	{
		NM_UPDOWN * pNMUpDown = (NM_UPDOWN*) lParam;
		int iNewPos = pNMUpDown->iPos + pNMUpDown->iDelta;
		if ((iNewPos < CCashFlowListCtrl::Limits::Duration::MINIMUM) || (iNewPos > CCashFlowListCtrl::Limits::Duration::MAXIMUM))
		{
			*pResult = 1; // do not update
		}
		else
		{
			myList.ChangeDuration(pNMUpDown->iPos + pNMUpDown->iDelta);
			*pResult = 0;
		}
	}
	return inherited::OnNotify(wParam, lParam, pResult);
	}

void CCashflowPageDialog::PutMarkupData(CMarkup& rMarkup) const 
{ 
	VERIFY(rMarkup.AddChildElem("CASHFLOW"));
	VERIFY(rMarkup.AddChildAttrib("StartDate", convert::DateToText(myList.GetStartDate())));
	CString aDuration;
	aDuration.Format("%u", myList.GetDuration());
	VERIFY(rMarkup.AddChildAttrib(Settings::category::cashflow::Key::DURATION, aDuration));
}

void CCashflowPageDialog::GetMarkupData(CMarkup& rMarkup) 
{ 
	if (rMarkup.FindChildElem("CASHFLOW"))
	{
		CString csString = rMarkup.GetChildAttrib("StartDate");
		if (!csString.IsEmpty()) myList.SetStartDate(convert::TextToDate(csString));

/* !!!	csString = rMarkup.GetChildAttrib("Duration");
		if (!csString.IsEmpty()) myList.SetDuration(
			// ensure the persisted value is within range ...
			min<size_t>
				(
				CCashFlowListCtrl::Limits::Duration::MAXIMUM,
				max<size_t>
					(
					CCashFlowListCtrl::Limits::Duration::MINIMUM, 
					s_cast<size_t> (atoi(csString)))
					)
				);
*/	}
}

void CCashflowPageDialog::OnStartingDate() 
{
	myList.EditStartDate();
}

#ifdef TEST

class CCashflowPageDialogFixture : public CppUnit::TestFixture 
	{
	void testPutMarkupData()
		{
		fake::AccountsListCtrl aFakeAccntsListCtrl;
		fake::TransListCtrl aFakeTranxListCtrl;
		CCashflowPageDialog aCashFlowPgDlg(fake::ConfigDataManager(), &aFakeAccntsListCtrl, &aFakeTranxListCtrl);
		aCashFlowPgDlg.Create(IDD_PAGE_CASHFLOW);
		aCashFlowPgDlg.ShowWindow(SW_HIDE);
		aCashFlowPgDlg.GetListCtrl().SetStartDate(CDate::Today() + 10);
		aCashFlowPgDlg.GetListCtrl().SetDuration(10);

		// need a markup xml root to play with ...
		CMarkup aMarkup;
		VERIFY(aMarkup.AddElem("ROOT"));

		aCashFlowPgDlg.PutMarkupData(aMarkup);
		CString s = aMarkup.GetDoc();
		aMarkup.SetDoc(s);
		CPPUNIT_ASSERT(aMarkup.FindElem("ROOT"));
		CPPUNIT_ASSERT(aMarkup.FindChildElem("CASHFLOW"));
		CPPUNIT_ASSERT(aMarkup.GetChildAttrib("StartDate") != "");
		CPPUNIT_ASSERT(aMarkup.GetChildAttrib("StartDate") == convert::DateToText(aCashFlowPgDlg.GetListCtrl().GetStartDate()));
		CPPUNIT_ASSERT(aMarkup.GetChildAttrib("Duration") != "");
		CPPUNIT_ASSERT(aMarkup.GetChildAttrib("Duration") == "10");
		}

	void testGetMarkupData()
		{
		fake::AccountsListCtrl aFakeAccntsListCtrl;
		fake::TransListCtrl aFakeTranxListCtrl;
		CCashflowPageDialog aCashFlowPgDlg(fake::ConfigDataManager(), &aFakeAccntsListCtrl, &aFakeTranxListCtrl);
		aCashFlowPgDlg.Create(IDD_PAGE_CASHFLOW);
		aCashFlowPgDlg.ShowWindow(SW_HIDE);

		// persist the object's data ...
		aCashFlowPgDlg.GetListCtrl().SetStartDate(CDate::Today() + 10);
		aCashFlowPgDlg.GetListCtrl().SetDuration(10);
		CMarkup aMarkup;
		VERIFY(aMarkup.AddElem("ROOT"));
		aCashFlowPgDlg.PutMarkupData(aMarkup);
		CString s = aMarkup.GetDoc();
		aMarkup.SetDoc(s);
		CPPUNIT_ASSERT(aMarkup.FindElem("ROOT"));

		// reset the values to something different ...
		aCashFlowPgDlg.GetListCtrl().SetStartDate(CDate::Today());
		aCashFlowPgDlg.GetListCtrl().SetDuration(9);
		}

	void testListAddEntry()
	{
		fake::AccountsListCtrl aFakeAccntsListCtrl;
		fake::TransListCtrl aFakeTranxListCtrl;
		CCashflowPageDialog aCashFlowPgDlg(fake::ConfigDataManager(), &aFakeAccntsListCtrl, &aFakeTranxListCtrl);
		aCashFlowPgDlg.Create(IDD_PAGE_CASHFLOW);
		aCashFlowPgDlg.ShowWindow(SW_HIDE);
		aCashFlowPgDlg.GetListCtrl().SetStartDate(CDate::Today());

		// adding one entry increases the entry count by 1
		int itemCount = aCashFlowPgDlg.GetListCtrl().GetItemCount();
		aCashFlowPgDlg.GetListCtrl().AddEntry(CDate::Today(), "ID:0 Item 1", 100);
		CPPUNIT_ASSERT_EQUAL((size_t) (itemCount + 1), aCashFlowPgDlg.GetListCtrl().GetItemCount());

		// Verify new entries get added in correct order.
		int i3 = aCashFlowPgDlg.GetListCtrl().AddEntry(CDate::Today() + 2, "ID:0 Item 3", 200);
		CPPUNIT_ASSERT_EQUAL(itemCount + 1, i3);

		int i2 = aCashFlowPgDlg.GetListCtrl().AddEntry(CDate::Today() + 1, "ID:0 Item 2", 200);
		CPPUNIT_ASSERT_EQUAL(itemCount + 1, i2);

		int i0 = aCashFlowPgDlg.GetListCtrl().AddEntry(CDate::Today() - 1, "ID:0 Item 0", 200);
		CPPUNIT_ASSERT_EQUAL(-1, i0);

		int i4 = aCashFlowPgDlg.GetListCtrl().AddEntry(CDate::Today() + 20, "ID:0 Item 20", 200);
		CPPUNIT_ASSERT_EQUAL(itemCount + 3, i4);

		// equal date entries get added at the end of the sequence
		{
		aCashFlowPgDlg.GetListCtrl().DeleteAllItems();
		aCashFlowPgDlg.GetListCtrl().AddEntry(CDate::Today(), "First", 100);
		aCashFlowPgDlg.GetListCtrl().AddEntry(CDate::Today(), "Second", 200);
		CPPUNIT_ASSERT("Second" == aCashFlowPgDlg.GetListCtrl().GetItemText(1, CCashFlowListCtrl::Column::index::DESC));
		}

		// sort correctly across years
		{
		aCashFlowPgDlg.GetListCtrl().DeleteAllItems();
		CDate oDate1(CDate::Today() + 1);
		CDate oDate2(oDate1.GetYear() + 1, oDate1.GetMonth(), oDate1.GetDay());
		--oDate2; // the day before
		int i1 = aCashFlowPgDlg.GetListCtrl().AddEntry(oDate2, "Second", 100);
		int i2 = aCashFlowPgDlg.GetListCtrl().AddEntry(oDate1, "First", 100);
		CPPUNIT_ASSERT_EQUAL(0, i1);
		CPPUNIT_ASSERT_EQUAL(0, i2);
		}

		// item data (SetItemData) field should get assigned
		TransactionData * pTData = new TransactionData(CDate::Today(), "entry", 100, new CDateRepeaterOnce());
		CCashFlowListCtrl::ItemData * data = new CCashFlowListCtrl::ItemData(CDate::Today(), pTData);
		int iAt = aCashFlowPgDlg.GetListCtrl().AddEntry(CDate::Today(), "entry", 100, pTData, false);
		CCashFlowListCtrl::ItemData * p = aCashFlowPgDlg.GetListCtrl().GetItemDataPtr(iAt);
		CPPUNIT_ASSERT(p->date == CDate::Today());
		CPPUNIT_ASSERT((DWORD) p->transaction_cookie == (DWORD) pTData);
		delete pTData;
		delete p;
	}

	void testListRebuild()
		{
		fake::AccountsListCtrl aFakeAccntsListCtrl;
		fake::TransListCtrl aFakeTranxListCtrl;
		CCashflowPageDialog aCashFlowPgDlg(fake::ConfigDataManager(), &aFakeAccntsListCtrl, &aFakeTranxListCtrl);
		aCashFlowPgDlg.Create(IDD_PAGE_CASHFLOW);

		aFakeTranxListCtrl.InsertAllCashFlowEntries_wasCalled = false;
		aCashFlowPgDlg.GetListCtrl().Rebuild();

		CPPUNIT_ASSERT(aFakeTranxListCtrl.InsertAllCashFlowEntries_wasCalled);
		}

	void testListGetSelectedItemIndex()
		{
		fake::AccountsListCtrl aFakeAccntsListCtrl;
		fake::TransListCtrl aFakeTranxListCtrl;
		CCashflowPageDialog aCashFlowPgDlg(fake::ConfigDataManager(), &aFakeAccntsListCtrl, &aFakeTranxListCtrl);
		aCashFlowPgDlg.Create(IDD_PAGE_CASHFLOW);
		aCashFlowPgDlg.ShowWindow(SW_SHOW);
		aCashFlowPgDlg.GetListCtrl().SetStartDate(CDate::Today());

		aCashFlowPgDlg.GetListCtrl().Rebuild();
		aCashFlowPgDlg.GetListCtrl().AddEntry(CDate::Today(), "entry 1", 100);
		aCashFlowPgDlg.GetListCtrl().AddEntry(CDate::Today(), "entry 2", 200);

		// nothing selected ...
		CPPUNIT_ASSERT(aCashFlowPgDlg.GetListCtrl().GetSelectedItemIndex() == -1);

		// select item index 1 ...
		aCashFlowPgDlg.GetListCtrl().SetItemState(1, LVIS_SELECTED, LVIS_SELECTED);
		CPPUNIT_ASSERT_EQUAL(1, aCashFlowPgDlg.GetListCtrl().GetSelectedItemIndex());
		}

	void testListIsTransaction()
	{
		fake::AccountsListCtrl aFakeAccntsListCtrl;
		fake::TransListCtrl aFakeTranxListCtrl;
		CCashflowPageDialog aCashFlowPgDlg(fake::ConfigDataManager(), &aFakeAccntsListCtrl, &aFakeTranxListCtrl);
		aCashFlowPgDlg.Create(IDD_PAGE_CASHFLOW);
		aCashFlowPgDlg.ShowWindow(SW_SHOW);
		aCashFlowPgDlg.GetListCtrl().SetStartDate(CDate::Today());

		aCashFlowPgDlg.GetListCtrl().Rebuild();
		aCashFlowPgDlg.GetListCtrl().AddEntry(CDate::Today(), "entry 1", 100, NULL);
		TransactionData * pTData = new TransactionData(CDate::Today(), "entry 2", 200, new CDateRepeaterOnce());
		aCashFlowPgDlg.GetListCtrl().AddEntry(CDate::Today(), "entry 2", 200, pTData);

		// current accounts entry is not a transaction
		CPPUNIT_ASSERT(!aCashFlowPgDlg.GetListCtrl().IsTransaction(0));

		// entry 1 is not a transaction
		CPPUNIT_ASSERT(!aCashFlowPgDlg.GetListCtrl().IsTransaction(1));

		// entry 2 is a transaction
		CPPUNIT_ASSERT(aCashFlowPgDlg.GetListCtrl().IsTransaction(2));
	}

	void testOnInitDialog()
	{
		// Case 1: default duration ...
		{
		fake::AccountsListCtrl aFakeAccntsListCtrl;
		fake::TransListCtrl aFakeTranxListCtrl;
		CCashflowPageDialog aDlg(fake::ConfigDataManager(), &aFakeAccntsListCtrl, &aFakeTranxListCtrl);
		aDlg.Create(IDD_PAGE_CASHFLOW);

		// test duration spin control range ...
		int lo, hi;
		aDlg.myDurationSpinCtrl.GetRange(lo, hi);
		CPPUNIT_ASSERT_EQUAL((int) CCashFlowListCtrl::Limits::Duration::MINIMUM, lo);
		CPPUNIT_ASSERT_EQUAL((int) CCashFlowListCtrl::Limits::Duration::MAXIMUM, hi);

		// test duration spin control setting ...
		CPPUNIT_ASSERT_EQUAL((int) CCashFlowListCtrl::Limits::Duration::DEFAULT, aDlg.myDurationSpinCtrl.GetPos());

		// test that the spin button buddy window is set ...
		CPPUNIT_ASSERT(aDlg.myDurationSpinCtrl.GetBuddy() == aDlg.GetDlgItem(IDC_STATIC_MONTHS));
		}

		// Case 2: non-default duration ...
		{
		fake::AccountsListCtrl aFakeAccntsListCtrl;
		fake::TransListCtrl aFakeTranxListCtrl;
		CCashflowPageDialog aDlg(fake::ConfigDataManager(), &aFakeAccntsListCtrl, &aFakeTranxListCtrl, 3 /* months */ );
		aDlg.Create(IDD_PAGE_CASHFLOW);

		// test duration spin control setting ...
		CPPUNIT_ASSERT_EQUAL(3, aDlg.myDurationSpinCtrl.GetPos());
		}
	}

	CPPUNIT_TEST_SUITE(CCashflowPageDialogFixture);
		CPPUNIT_TEST(testPutMarkupData);
		CPPUNIT_TEST(testGetMarkupData); 
		CPPUNIT_TEST(testListAddEntry);
		CPPUNIT_TEST(testListRebuild);
		CPPUNIT_TEST(testListGetSelectedItemIndex);
		CPPUNIT_TEST(testListIsTransaction);
		CPPUNIT_TEST(testOnInitDialog);
	CPPUNIT_TEST_SUITE_END();
	};

CPPUNIT_TEST_SUITE_REGISTRATION(CCashflowPageDialogFixture);

#endif
