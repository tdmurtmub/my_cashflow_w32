#include "stdafx.h"

#ifdef TEST

#include <iostream>

#include <cppunit\extensions\HelperMacros.h>
#include <cppunit\ui\text\TestRunner.h>

#include <FixedArray.h>
#include "mfc\Markup.h"
#include "mfc\ConfigDataManager.h"

#include "AccountData.h"
#include "AccountEditDlg.h"
#include "AccountsCtrl.h"
#include "ICashflowListCtrl.h"
#include "ITransListCtrl.h"
#include "MyMoney.h"
#include "MyMoneyDlg.h"
#include "TransactionData.h"
#include "TransactionsPgDlg.h"
#include "TrnxListCtrl.h"
#include "XEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMyCashflowApp theApp;

using namespace std;
using namespace Utility;
using namespace app;

inline string CStringToString2(const CString & rCString) { return string((LPCTSTR) rCString); }

namespace {

class CMyCashflowAppHookStub : public CMyCashflowAppHooks 
	{
	public: 
	string lpszOldName_arg, lpszNewName_arg;
	CFileException *CFileRename_should_throw;
	
	CMyCashflowAppHookStub() : CMyCashflowAppHooks()
		{
		CFileRename_should_throw = NULL;
		}
		
	virtual CString GetPersistenceFolderPath()
		{
		return ".\\";
		}
		
	virtual void PASCAL CFileRename(LPCTSTR lpszOldName, LPCTSTR lpszNewName) 
		{ 
		if (CFileRename_should_throw) throw CFileRename_should_throw;
		lpszOldName_arg = lpszOldName;
		lpszNewName_arg = lpszNewName;
		}
	};

void CleanStartup()
	{
	TRY
		{
		CMyCashflowApp::hooks = new CMyCashflowAppHookStub();
		CFile::Remove(CMyCashflowApp::GetDefaultFileName());
		}
	CATCH_ALL(pException)
		{
		}
	END_CATCH_ALL
	}

struct FakeSettings : public ISettings 
	{ 
	void SetKeyValue(const CString &category, const CString &key, int value) { }
	void SetKeyValue(const CString &category, const CString &key, const CString &value) { }
	void SetKeyValueInt(const CString &category, const CString &key, int value) { }
	int GetKeyValueInt(const CString &category, const CString &key) const { return 0; }
	void SetKeyValueStr(const CString &category, const CString &key, const CString &value) { }
	CString GetKeyValueStr(const CString &category, const CString &key) const { return ""; }
	};

struct CMyCashFlowDlgWrap : public CMyCashflowDlg 
	{ 
	fake::ConfigDataManager aFakeConfigDataManager;
	CMyCashFlowDlgWrap(ISettings * settings) : CMyCashflowDlg(aFakeConfigDataManager, 13) { }
	bool m_bUpdateMenuItem;
	void SetupFirstRun() { }
	override int AskToSave() const { return IDYES; }
	afx_msg void OnEditDelSelEntry() { CMyCashflowDlg::OnEditDelSelEntry(); }
	afx_msg void OnFileSave() { CMyCashflowDlg::OnFileSave(); }
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) { CMyCashflowDlg::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu); }
	};

class CFakeCashFlowListCtrl : public fake::CashflowListCtrl
	{
	public:
	IAccountsListCtrl* myAccountListCtrl;
	ITransListCtrl* myTransactionListCtrl;
	CFakeCashFlowListCtrl(IAccountsListCtrl* pAccntsListCtrl, ITransListCtrl* pTranxListCtrl) :
		myAccountListCtrl(pAccntsListCtrl), 
		myTransactionListCtrl(pTranxListCtrl) 
		{ }
	bool m_bRefreshBalance_WasCalled;
	virtual void RefreshBalance(UINT nStart = 0) { m_bRefreshBalance_WasCalled = true; }
	TTransactionCookie GetItemCookie(int aItemIndex) const { return 0; }
	CDate GetEndDate() const { return CDate::Today() + 31 * 12; }
	void RemoveItem(int aItemIndex) { }

	std::vector< std::pair< CDate, appl::Dollar::Value::type> > m_aList;
	virtual quantity GetItemCount() const { return m_aList.size(); }
	virtual appl::Dollar::Value::type GetItemBalance(int iIndex) const { return m_aList[iIndex].second; }
	virtual CDate GetItemDate(UINT iIndex) const { return m_aList[iIndex].first; };
	virtual int AddEntry(const CDate& oDate, LPCTSTR pszDesc, appl::Dollar::Value::type vAmt, 
		TTransactionCookie aTransactionCookie = NULL_TRANSACTION_COOKIE, 
		BOOL bUpdateBal = TRUE) 
		{ 
		if (m_aList.size() > 0)
			m_aList.push_back(make_pair(oDate, m_aList.back().second + vAmt));
		else
			m_aList.push_back(make_pair(oDate, vAmt));
		return m_aList.size() - 1; 
		}
	};

}

class CMyCashflowAppTestFixture : public CppUnit::TestFixture 
	{
	public:

	void setUp() 
		{ 
		CleanStartup(); 
		}

	void tearDown() 
		{ 		
		CMyCashflowApp::hooks = new CMyCashflowAppHookStub();
		CMyCashflowApp::the_storage_dir = CMyCashflowApp::DiscoverPersistenceFolderPath();
		}

	void testTrEdDlgRepBttnDflts()
	// Verify that controls are enabled correctly based on radio button selections. 
		{
		CTransEditDlg aDlg;
		aDlg.Create(IDD_ADD_TRANSACTION);

		aDlg.OnEndsNever();
		BOOL bOccur = aDlg.m_aOccur.IsWindowEnabled();
		BOOL bOccurSpin = aDlg.m_aOccurSpin.IsWindowEnabled();

		// Select Repeat Once ...
		aDlg.OnRepeatsOnce();
		CPPUNIT_ASSERT(aDlg.m_aRep1Freq.IsWindowEnabled() == FALSE);
		CPPUNIT_ASSERT(aDlg.m_aRep1Interval.IsWindowEnabled() == FALSE);
		CPPUNIT_ASSERT(aDlg.m_aEndGrpNever.IsWindowEnabled() == FALSE);
		CPPUNIT_ASSERT(aDlg.m_aEndGrpAfter.IsWindowEnabled() == FALSE);
		CPPUNIT_ASSERT(aDlg.m_aOccur.IsWindowEnabled() == FALSE);
		CPPUNIT_ASSERT(aDlg.m_aOccurSpin.IsWindowEnabled() == FALSE);

		// Select Repeat Every ...
		aDlg.OnRepeatsEvery();
		CPPUNIT_ASSERT(aDlg.m_aRep1Freq.IsWindowEnabled() == TRUE);
		CPPUNIT_ASSERT(aDlg.m_aRep1Interval.IsWindowEnabled() == TRUE);
		CPPUNIT_ASSERT(aDlg.m_aEndGrpNever.IsWindowEnabled() == TRUE);
		CPPUNIT_ASSERT(aDlg.m_aEndGrpAfter.IsWindowEnabled() == TRUE);
		CPPUNIT_ASSERT(aDlg.m_aOccur.IsWindowEnabled() == bOccur);
		CPPUNIT_ASSERT(aDlg.m_aOccurSpin.IsWindowEnabled() == bOccurSpin);

		// Select Repeat Once again...
		aDlg.OnRepeatsOnce();
		CPPUNIT_ASSERT(aDlg.m_aRep1Freq.IsWindowEnabled() == FALSE);
		CPPUNIT_ASSERT(aDlg.m_aRep1Interval.IsWindowEnabled() == FALSE);
		CPPUNIT_ASSERT(aDlg.m_aEndGrpNever.IsWindowEnabled() == FALSE);
		CPPUNIT_ASSERT(aDlg.m_aEndGrpAfter.IsWindowEnabled() == FALSE);
		CPPUNIT_ASSERT(aDlg.m_aOccur.IsWindowEnabled() == FALSE);
		CPPUNIT_ASSERT(aDlg.m_aOccurSpin.IsWindowEnabled() == FALSE);
		}

	void testTrEdDlgRepBttnSelection()
	// Correct behavior requires that the group of "Ends" controls be disabled when 
	// the Repeat/Once radio button is selected and re-enabled when not.
		{
		CTransEditDlg aDlg;
		aDlg.Create(IDD_ADD_TRANSACTION);

		// Simulate selecting Repeats/Every radio button.
		aDlg.OnRepeatsEvery();
		aDlg.m_aRepGrpOnce.SetCheck(0);
		aDlg.m_aRepGrpEvery.SetCheck(1);
		CPPUNIT_ASSERT(aDlg.m_aRep1Freq.IsWindowEnabled() == TRUE);
		CPPUNIT_ASSERT(aDlg.m_aRep1Interval.IsWindowEnabled() == TRUE);
		CPPUNIT_ASSERT(aDlg.m_aEndGrpNever.IsWindowEnabled() == TRUE);
		CPPUNIT_ASSERT(aDlg.m_aEndGrpAfter.IsWindowEnabled() == TRUE);

		// Simulate selecting Ends/After radio button.
		aDlg.OnEndsAfter();
		aDlg.m_aEndGrpNever.SetCheck(0);
		aDlg.m_aEndGrpAfter.SetCheck(1);
		aDlg.m_iRepEndGrp = CTransEditDlg::Repeat::Ends::Index::AFTER;
		CPPUNIT_ASSERT(aDlg.m_aOccur.IsWindowEnabled() == TRUE);
		CPPUNIT_ASSERT(aDlg.m_aOccurSpin.IsWindowEnabled() == TRUE);

		// Record state of the occurrence controls.
		BOOL bOccur = aDlg.m_aOccur.IsWindowEnabled();
		BOOL bOccurSpin = aDlg.m_aOccurSpin.IsWindowEnabled();

		// Select Repeat/Once ...
		aDlg.OnRepeatsOnce();
		aDlg.m_aRepGrpOnce.SetCheck(1);
		aDlg.m_aRepGrpEvery.SetCheck(0);

		// Select Repeat/Every ...
		aDlg.OnRepeatsEvery();
		aDlg.m_aRepGrpOnce.SetCheck(0);
		aDlg.m_aRepGrpEvery.SetCheck(1);

		// State of occurrence controls should be restored.
		CPPUNIT_ASSERT(aDlg.m_aOccur.IsWindowEnabled() == bOccur);
		CPPUNIT_ASSERT(aDlg.m_aOccurSpin.IsWindowEnabled() == bOccurSpin);
		}

	void testTrAddEntry()
		{
		CTransListCtrl& oList = theApp.main_dialog->transactions_dialog.GetListCtrl();
		CCashFlowListCtrl& oCash = theApp.main_dialog->cashflow_dialog.GetListCtrl();

		// Verify that adding items to the transaction list correctly updates 
		// the cashflow list ...

		// setup
		CPPUNIT_ASSERT(oList.DeleteAllItems());
		CPPUNIT_ASSERT(oCash.DeleteAllItems());
		CPPUNIT_ASSERT_EQUAL(0, oList.GetItemCount());
		CPPUNIT_ASSERT_EQUAL((size_t) 0, oCash.GetItemCount());
		theApp.main_dialog->accounts_dialog.GetListCtrl().DeleteAllItems();
		oCash.AddCurrBalEntry();

		// add a non-repeating item
		CDate oDate = CDate::Today();
		oCash.SetStartDate(oDate);
		oList.AddEntry(oDate, "Item 1", 100);
		CPPUNIT_ASSERT_EQUAL(1, oList.GetItemCount());
		CPPUNIT_ASSERT_EQUAL((size_t) 2, oCash.GetItemCount());

		// add a repeating item
		oList.AddEntry(oDate, "Item 2", 200, new CDateRepeaterDaily(1, 9));
		CPPUNIT_ASSERT_EQUAL(2, oList.GetItemCount());
		CPPUNIT_ASSERT_EQUAL((size_t) 11, oCash.GetItemCount());
		CCashFlowListCtrl::ItemData * itemData = oCash.GetItemDataPtr(10);
		CPPUNIT_ASSERT_EQUAL(CStringToString((oDate + 8).ToString()), CStringToString(itemData->date.ToString()));
		}

	void testTrAddEntryDailyPerf()
	// Ensure that adding a Daily repeater transaction performs well.
		{
		CTransListCtrl& oList = theApp.main_dialog->transactions_dialog.GetListCtrl();
		CPPUNIT_ASSERT(oList.DeleteAllItems());
		oList.AddEntry(CDate::Today(), "Daily", -10, new CDateRepeaterDaily());
		}

	void testFileExit()
		{	
		FakeSettings settings;
		CMyCashFlowDlgWrap dialog(&settings);
		dialog.Create(); 
		dialog.ShowWindow(SW_NORMAL);
		dialog.SendMessage(WM_COMMAND, ID_FILE_EXIT, 0);
		CPPUNIT_ASSERT(NULL == dialog.m_hWnd);
		// we won't get here if it doesn't shut down!
		}

	void testApp()
		{
		// test that it has version ...
			{
			CString aFileName(theApp.m_pszExeName);
			aFileName += ".exe";
			FixedArray<TCHAR> aExeName(aFileName.GetLength() + 1);
			aExeName.Copy(aFileName);
			DWORD dwHandle;
			DWORD dwSize = GetFileVersionInfoSize(&aExeName[0], &dwHandle);
			CPPUNIT_ASSERT(dwSize > 0);
			}

		// menu ...
			{
			CMenu aMenu;
			aMenu.LoadMenu(IDR_MAINFRAME);
			CMenu * aSubmenu = aMenu.GetSubMenu(0); // File

			// has an ID_FILE_SAVE item ...
			CPPUNIT_ASSERT(aSubmenu->ModifyMenu(ID_FILE_SAVE, MF_BYCOMMAND, 0, (LPCSTR) NULL) == TRUE);
			}
		}

	void TestDefaults()
		{
		CPPUNIT_ASSERT_EQUAL(string("MyCashflow"), CStringToString(CMyCashflowApp::PERSISTENCE_EXTENSION));
		CPPUNIT_ASSERT_EQUAL(string("default.MyCashflow"), CStringToString(CMyCashflowApp::GetDefaultFileName()));
		}

	void TestGetDefaultFilePath()
		{
		CMyCashflowApp::the_storage_dir = "X:\\MyDocs\\DirWithOutBackSlash";
		CPPUNIT_ASSERT_EQUAL(string("X:\\MyDocs\\DirWithOutBackSlash\\default.MyCashflow"), CStringToString(CMyCashflowApp::GetDefaultFilePath()));
		CMyCashflowApp::the_storage_dir = "X:\\DirWithBackSlash\\";
		CPPUNIT_ASSERT_EQUAL(string("X:\\DirWithBackSlash\\default.MyCashflow"), CStringToString(CMyCashflowApp::GetDefaultFilePath()));
		}

	void Test_RenameLegacyFile()
		{
		CMyCashflowAppHookStub * hooks = (CMyCashflowAppHookStub*) (CMyCashflowApp::hooks);
		CMyCashflowApp::the_storage_dir = "X:\\MyDocs\\SomeDir";
		theApp.RenameLegacyFile();
		CPPUNIT_ASSERT_EQUAL(string("X:\\MyDocs\\SomeDir\\default.MyCa$hflow"), hooks->lpszOldName_arg);
		CPPUNIT_ASSERT_EQUAL(string("X:\\MyDocs\\SomeDir\\default.MyCashflow"), hooks->lpszNewName_arg);
		}
	
	void Test_RenameLegacyFile_catches_all_exceptions()
		{
		CMyCashflowAppHookStub * hooks = (CMyCashflowAppHookStub*) (CMyCashflowApp::hooks);
		hooks->lpszNewName_arg = "unchanged";
		hooks->CFileRename_should_throw = new CFileException();
		theApp.RenameLegacyFile();
		CPPUNIT_ASSERT_EQUAL(string("unchanged"), hooks->lpszNewName_arg);
		}
		
	CPPUNIT_TEST_SUITE(CMyCashflowAppTestFixture);
		CPPUNIT_TEST(testApp);
		CPPUNIT_TEST(testFileExit);
		CPPUNIT_TEST(TestDefaults);
		CPPUNIT_TEST(testTrAddEntry);
		CPPUNIT_TEST(testTrAddEntryDailyPerf);
		CPPUNIT_TEST(testTrEdDlgRepBttnDflts);
		CPPUNIT_TEST(testTrEdDlgRepBttnSelection);
		CPPUNIT_TEST(TestGetDefaultFilePath);
		CPPUNIT_TEST(Test_RenameLegacyFile);
		CPPUNIT_TEST(Test_RenameLegacyFile_catches_all_exceptions);
	CPPUNIT_TEST_SUITE_END();
	};

class CTranxListCtrlTest : public CppUnit::TestFixture 
	{
	public:
	CTranxListCtrlTest() : CppUnit::TestFixture (), dialog(new FakeSettings()) { }
	CMyCashFlowDlgWrap dialog;

	void setUp() 
		{ 
		CleanStartup();
		dialog.Create(); 
		dialog.ShowWindow(SW_HIDE);
		}

	void tearDown() 
		{ 
		dialog.EndDialog(IDOK); 
		}

	void testInitDialog()
		{
		// do not allow multi item selection ...
		dialog.transactions_dialog.GetListCtrl().AddEntry(CDate::Today(), "Transaction1", 10);
		dialog.transactions_dialog.GetListCtrl().AddEntry(CDate::Today(), "Transaction2", 20);
		dialog.transactions_dialog.GetListCtrl().AddEntry(CDate::Today(), "Transaction3", 30);
		dialog.transactions_dialog.GetListCtrl().SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
		dialog.transactions_dialog.GetListCtrl().SetItemState(1, LVIS_SELECTED, LVIS_SELECTED);
		CPPUNIT_ASSERT_EQUAL(1, (int) dialog.transactions_dialog.GetListCtrl().GetSelectedCount());
		}

	void testDeleteEntry()
		{
		dialog.transactions_dialog.GetListCtrl().AddEntry(CDate::Today(), "Transaction1", 10);
		dialog.transactions_dialog.GetListCtrl().AddEntry(CDate::Today(), "Transaction2", 20);
		dialog.transactions_dialog.GetListCtrl().AddEntry(CDate::Today(), "Transaction3", 30);

		dialog.transactions_dialog.GetListCtrl().DeleteEntry(1);

		// verify entry was deleted ...

		CPPUNIT_ASSERT_EQUAL(2, dialog.transactions_dialog.GetListCtrl().GetItemCount());

		// verify cashflow was updated ...

		appl::Dollar::Value::type vBalance = appl::Dollar::Value::FromStr(dialog.cashflow_dialog.GetListCtrl().GetItemText(2, CCashFlowListCtrl::Column::index::BALANCE));
		CPPUNIT_ASSERT_EQUAL(40, vBalance);

		// add a 4 times repeating entry ...

		int iAt = dialog.transactions_dialog.GetListCtrl().AddEntry(CDate::Today(), "Repeater", 10, new CDateRepeaterWeekly(1, 4));
		vBalance = appl::Dollar::Value::FromStr(dialog.cashflow_dialog.GetListCtrl().GetItemText(6, CCashFlowListCtrl::Column::index::BALANCE));
		CPPUNIT_ASSERT_EQUAL(80, vBalance);

		dialog.transactions_dialog.GetListCtrl().DeleteEntry(iAt);

		// verify transaction was deleted ...

		CPPUNIT_ASSERT_EQUAL(2, dialog.transactions_dialog.GetListCtrl().GetItemCount());

		// verify cashflow was updated ...

		CPPUNIT_ASSERT_EQUAL((size_t) 3, dialog.cashflow_dialog.GetListCtrl().GetItemCount());
		vBalance = appl::Dollar::Value::FromStr(dialog.cashflow_dialog.GetListCtrl().GetItemText(2, CCashFlowListCtrl::Column::index::BALANCE));
		CPPUNIT_ASSERT_EQUAL(40, vBalance);
		}

	void testUpdateEntry()
		{
		dialog.accounts_dialog.GetListCtrl().DeleteAllItems();
		dialog.cashflow_dialog.GetListCtrl().DeleteAllItems();
		dialog.cashflow_dialog.GetListCtrl().AddCurrBalEntry();

		// start with an empty list ...

		CTransListCtrl& oList = dialog.transactions_dialog.GetListCtrl();
		CPPUNIT_ASSERT(oList.DeleteAllItems());

		// add 3 non-repeating items ...

		oList.AddEntry(CDate::Today(), "Item 1", 10);
		oList.AddEntry(CDate::Today() + 1, "Item 2", 1000);
		oList.AddEntry(CDate::Today() + 2, "Item 3", 30);

		// verify cash flow was updated ...

		CPPUNIT_ASSERT_EQUAL((size_t) 4, dialog.cashflow_dialog.GetListCtrl().GetItemCount());
		appl::Dollar::Value::type vBalance = appl::Dollar::Value::FromStr(dialog.cashflow_dialog.GetListCtrl().GetItemText(3, CCashFlowListCtrl::Column::index::BALANCE));
		CPPUNIT_ASSERT_EQUAL(1040, vBalance);

		// Update the second transaction and make it a repeater ...

		oList.UpdateEntry(1, CDate::Today() + 3, "NewText", 2000, new CDateRepeaterWeekly(1, 2));

		// Verify that all the fields were updated correctly ...
		TransactionData* pTData = (TransactionData*) oList.GetItemData(1);
		CPPUNIT_ASSERT(CDate::Today() + 3 == pTData->myStartDate);
		CPPUNIT_ASSERT_EQUAL(CString("NewText"), oList.GetItemText(1, CTransListCtrl::Column::Index::DESC));
		CPPUNIT_ASSERT_EQUAL(CString("2000"), CString(oList.GetItemText(1, CTransListCtrl::Column::Index::AMOUNT)));
		const CDateRepeater* pDateRepeater = pTData->m_pRepeater;
		CPPUNIT_ASSERT(CDateRepeater::Interval::WEEK == pDateRepeater->IntervalType());
		CPPUNIT_ASSERT_EQUAL(CString("Weekly"), CString(oList.GetItemText(1, CTransListCtrl::Column::Index::REPEAT)));

		// verify cash flow was updated ...

		CPPUNIT_ASSERT_EQUAL((size_t) 5, dialog.cashflow_dialog.GetListCtrl().GetItemCount());
		}

	CPPUNIT_TEST_SUITE(CTranxListCtrlTest);
		CPPUNIT_TEST(testDeleteEntry);
		CPPUNIT_TEST(testInitDialog);
		CPPUNIT_TEST(testUpdateEntry);
	CPPUNIT_TEST_SUITE_END();
	};

namespace fake
	{
	class CCashFlowListCtrl : public CFakeCashFlowListCtrl
		{
		public:
		CCashFlowListCtrl(IAccountsListCtrl* pAccntsListCtrl, ITransListCtrl* pTranxListCtrl) 
			: CFakeCashFlowListCtrl(pAccntsListCtrl, pTranxListCtrl) 
			{ 
			}
		};

	class CAccountsPageDialog : public ::CAccountsPageDialog
		{
		public:
		CAccountsPageDialog(ICashflowListCtrl* pCashFlowListCtrl) : ::CAccountsPageDialog(fake::ConfigDataManager(), pCashFlowListCtrl) 
			{
			}

		bool OnEditDelSelEntryWasCalled;
		virtual void OnEditDelSelEntry()
			{
			OnEditDelSelEntryWasCalled = true;
			}
		};

	class CTransPageDlg : public ::CTransactionsPageDialog
		{
		public:
		CTransPageDlg(ICashflowListCtrl* pCashFlowListCtrl) : ::CTransactionsPageDialog(fake::ConfigDataManager(), pCashFlowListCtrl) 
			{
			}

		bool OnEditDelSelEntryWasCalled;
		virtual void OnEditDelSelEntry()
			{
			OnEditDelSelEntryWasCalled = true;
			}
		};
	}

class CMyCashFlowDlgTest : public CppUnit::TestFixture 
	{
	public:
	bool m_bDelAutoSaveFile;

	CMyCashFlowDlgTest() : CppUnit::TestFixture (), dialog(new FakeSettings()) { }
	CMyCashFlowDlgWrap dialog;

	void setUp() 
		{ 
		dialog.Create(); 
		dialog.ShowWindow(SW_HIDE);
		m_bDelAutoSaveFile = true; 
		}

	void tearDown() 
		{ 
		dialog.Save();
		dialog.EndDialog(IDOK); 
		if (m_bDelAutoSaveFile) 
			{
			TRY
				{
				CFile::Remove(CMyCashflowApp::GetDefaultFileName());
				}
			CATCH_ALL(pException)
				{
				}
			END_CATCH_ALL
			}
		}

	class CMyCashFlowDlgTester : public CMyCashflowDlg
		{
		public: 
		fake::ConfigDataManager aFakeConfigDataManager;
		CMyCashFlowDlgTester () : CMyCashflowDlg(aFakeConfigDataManager, 13) { }
		override bool ConditionalSave() { return true; }
		override bool IsModified() const { return true; }
		void SetupFirstRun() { }
		override int AskToSave() const { return IDYES; }

		bool PostMessageWasCalled;
		UINT PostMessageMessage;
		WPARAM PostMessageWParam;
		BOOL PostMessageResult;
		
		BOOL CallPostMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0)
			{
			PostMessageWasCalled = true;
			PostMessageMessage = message;
			PostMessageWParam = wParam;
			return PostMessageResult;
			}

		int OverridableGetActiveIndexResult;
		int OverridableGetActiveIndex() const { return OverridableGetActiveIndexResult; }

		afx_msg void OnEditDelSelEntry() 
			{ 
			CMyCashflowDlg::OnEditDelSelEntry(); 
			}

		afx_msg void OnFileSave() 
			{ 
			CMyCashflowDlg::OnFileSave(); 
			}

		afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
			{ 
			CMyCashflowDlg::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu); 
			}
		};

	void testUpdateMenuItem()
		{
		CMenu oMenu;
		oMenu.LoadMenu(IDR_MAINFRAME);
		CPPUNIT_ASSERT(oMenu.GetMenuItemCount() > 0);
		CMenu* pPopupMenu = oMenu.GetSubMenu(1);
		CPPUNIT_ASSERT(pPopupMenu->GetMenuItemCount() > 0);

		// ID_EDIT_SELECTEDITEM ...

		{ // ... for the Cash Flow page

			// nothing in the list so Edit should not be enabled
			CPPUNIT_ASSERT(dialog.SetActivePage(CMyCashflowDlg::tab::index::CASHFLOW));
			dialog.cashflow_dialog.GetListCtrl().DeleteAllItems();
			dialog.OnInitMenuPopup(pPopupMenu, 0, false);
			CPPUNIT_ASSERT((pPopupMenu->GetMenuState(ID_EDIT_SELECTEDITEM, MF_BYCOMMAND) & MF_GRAYED) != 0);

			// something in the list but nothing selected so Edit should not be enabled ...
			dialog.cashflow_dialog.GetListCtrl().Rebuild();
			dialog.cashflow_dialog.GetListCtrl().AddEntry(CDate::Today(), "entry 1", 100, NULL);
			TransactionData * pTData = new TransactionData(CDate::Today(), "entry 2", 200, new CDateRepeaterOnce());
			dialog.cashflow_dialog.GetListCtrl().AddEntry(CDate::Today(), "entry 2", 200, pTData);
			dialog.OnInitMenuPopup(pPopupMenu, 0, false);
			CPPUNIT_ASSERT((pPopupMenu->GetMenuState(ID_EDIT_SELECTEDITEM, MF_BYCOMMAND) & MF_GRAYED) != 0);

			// Edit should be enabled for selected transactions only ...
			WAXONZ(dialog.cashflow_dialog.GetListCtrl().SetItemState(2, LVIS_SELECTED, LVIS_SELECTED));
			dialog.OnInitMenuPopup(pPopupMenu, 0, false);
			UINT state = pPopupMenu->GetMenuState(ID_EDIT_SELECTEDITEM, MF_BYCOMMAND);
			ASSERT(state != 0xFFFFFFFF);
			CPPUNIT_ASSERT(state == MF_ENABLED);
		}

		{ // ... for the Settings page

			// Edit should not be enabled
//			CPPUNIT_ASSERT(dialog.SetActivePage(CMyCashflowDlg::tab::index::PREFERENCES));
//			dialog.OnInitMenuPopup(pPopupMenu, 0, false);
//			CPPUNIT_ASSERT((pPopupMenu->GetMenuState(ID_EDIT_SELECTEDITEM, MF_BYCOMMAND) & MF_GRAYED) != 0);
		}

			{ // ... for the Account List

			// nothing in the list so Edit should not be enabled
			CPPUNIT_ASSERT(dialog.SetActivePage(CMyCashflowDlg::tab::index::ACCOUNTS));
			dialog.accounts_dialog.GetListCtrl().DeleteAllItems();
			dialog.OnInitMenuPopup(pPopupMenu, 0, false);
			CPPUNIT_ASSERT((pPopupMenu->GetMenuState(ID_EDIT_SELECTEDITEM, MF_BYCOMMAND) & MF_GRAYED) != 0);

			// something in the list but nothing selected so Edit should not be enabled
			dialog.accounts_dialog.GetListCtrl().AddEntry("Account1", 100, false);
			dialog.OnInitMenuPopup(pPopupMenu, 0, false);
			CPPUNIT_ASSERT((pPopupMenu->GetMenuState(ID_EDIT_SELECTEDITEM, MF_BYCOMMAND) & MF_GRAYED) != 0);

			// something selected so Edit should be enabled
			dialog.accounts_dialog.GetListCtrl().AddEntry("Account2", 200, true);
			dialog.OnInitMenuPopup(pPopupMenu, 0, false);
			UINT state = pPopupMenu->GetMenuState(ID_EDIT_SELECTEDITEM, MF_BYCOMMAND);
			ASSERT(state != 0xFFFFFFFF);
			CPPUNIT_ASSERT(state == MF_ENABLED);
			}

		// ID_EDIT_DELSELENTRY ...

			{ // ... for the Cash Flow page

			// Edit should not be enabled
			CPPUNIT_ASSERT(dialog.SetActivePage(CMyCashflowDlg::tab::index::CASHFLOW));
			dialog.OnInitMenuPopup(pPopupMenu, 0, false);
			CPPUNIT_ASSERT((pPopupMenu->GetMenuState(ID_EDIT_DELSELENTRY, MF_BYCOMMAND) & MF_GRAYED) != 0);
			}

			{ // ... for the Account List

			// nothing in the list so Delete should not be enabled
			CPPUNIT_ASSERT(dialog.SetActivePage(CMyCashflowDlg::tab::index::ACCOUNTS));
			dialog.accounts_dialog.GetListCtrl().DeleteAllItems();
			dialog.OnInitMenuPopup(pPopupMenu, 1, false);
			CPPUNIT_ASSERT((pPopupMenu->GetMenuState(ID_EDIT_DELSELENTRY, MF_BYCOMMAND) & MF_GRAYED) != 0);

			// something in the list but nothing selected so Delete should not be enabled
			dialog.accounts_dialog.GetListCtrl().AddEntry("Account1", 100, false);
			dialog.OnInitMenuPopup(pPopupMenu, 1, false);
			UINT state = pPopupMenu->GetMenuState(ID_EDIT_DELSELENTRY, MF_BYCOMMAND);
			CPPUNIT_ASSERT((state & MF_GRAYED) != 0);

			// something selected so Delete should be enabled
			dialog.accounts_dialog.GetListCtrl().AddEntry("Account2", 200, true);
			dialog.OnInitMenuPopup(pPopupMenu, 1, false);
			state = pPopupMenu->GetMenuState(ID_EDIT_DELSELENTRY, MF_BYCOMMAND);
			ASSERT(state != 0xFFFFFFFF);
			CPPUNIT_ASSERT(state == MF_ENABLED);
			}

			{ // ... and the Transaction List

			// nothing in the list so Delete should not be enabled
			CPPUNIT_ASSERT(dialog.SetActivePage(CMyCashflowDlg::tab::index::TRANSACTIONS));
			dialog.transactions_dialog.GetListCtrl().DeleteAllItems();
			dialog.OnInitMenuPopup(pPopupMenu, 1, false);
			CPPUNIT_ASSERT((pPopupMenu->GetMenuState(ID_EDIT_DELSELENTRY, MF_BYCOMMAND) & MF_GRAYED) != 0);

			// something in the list but nothing selected so Delete should not be enabled
			dialog.transactions_dialog.GetListCtrl().AddEntry(CDate::Today(), "Transaction1", 100, new CDateRepeaterOnce(), false);
			dialog.OnInitMenuPopup(pPopupMenu, 1, false);
			UINT state = pPopupMenu->GetMenuState(ID_EDIT_DELSELENTRY, MF_BYCOMMAND);
			CPPUNIT_ASSERT((state & MF_GRAYED) != 0);

			// something selected so Delete should be enabled
			int iAt = dialog.transactions_dialog.GetListCtrl().AddEntry(CDate::Today(), "Transaction2", 200);

			// select the entry ...
			VERIFY(dialog.transactions_dialog.GetListCtrl().SetItemState(iAt, LVIS_SELECTED, LVIS_SELECTED));
			// this test fails because I cannot get the item selected with the above code

			dialog.OnInitMenuPopup(pPopupMenu, 1, false);
			state = pPopupMenu->GetMenuState(ID_EDIT_DELSELENTRY, MF_BYCOMMAND);
			ASSERT(state != 0xFFFFFFFF);
			CPPUNIT_ASSERT(state == MF_ENABLED);
			}
		}

	void testOnInitMenuPopup()
		{
		CMenu oMenu;
		oMenu.LoadMenu(IDR_MAINFRAME);
		CPPUNIT_ASSERT(oMenu.GetMenuItemCount() > 0);
		CMenu* pPopupMenu = oMenu.GetSubMenu(0);

		struct CTest : public CMyCashFlowDlgWrap 
			{ 
			CTest() : CMyCashFlowDlgWrap(new FakeSettings()) { }
			bool m_bUpdateMenuItem;
			virtual void UpdateMenuItem(CMenu* pPopupMenu, UINT nCmd) { m_bUpdateMenuItem = true; }
			} oMyMoneyDlg;

		// verify that no updating takes place for SysMenu messages
		oMyMoneyDlg.m_bUpdateMenuItem = false;
		oMyMoneyDlg.OnInitMenuPopup(pPopupMenu, 0, true);
		CPPUNIT_ASSERT(!oMyMoneyDlg.m_bUpdateMenuItem);

		// verify that each menu item gets an opportunity to update itself
		oMyMoneyDlg.OnInitMenuPopup(pPopupMenu, 0, false);
		CPPUNIT_ASSERT(oMyMoneyDlg.m_bUpdateMenuItem);
		}

	void testOnEditDelSelEntry()
		{
		class CTester : public CMyCashFlowDlgTester
			{
			public:
			CTester() : CMyCashFlowDlgTester(), 
				AccountsPageDlgResult(new fake::CCashFlowListCtrl(new fake::AccountsListCtrl(), new fake::TransListCtrl())),
				TransactionsPageDlgResult(new fake::CCashFlowListCtrl(new fake::AccountsListCtrl(), new fake::TransListCtrl()))
				{ 
				} 

			fake::CAccountsPageDialog AccountsPageDlgResult;
			virtual CAccountsPageDialog & AccountsPageDlg() { return AccountsPageDlgResult; }

			fake::CTransPageDlg TransactionsPageDlgResult;
			virtual CTransactionsPageDialog & TransactionsPageDlg() { return TransactionsPageDlgResult; }
			};

		CTester aTester;

		aTester.OverridableGetActiveIndexResult = CMyCashflowDlg::tab::index::ACCOUNTS;
		aTester.AccountsPageDlgResult.OnEditDelSelEntryWasCalled = false;
		aTester.OnEditDelSelEntry();
		CPPUNIT_ASSERT(aTester.AccountsPageDlgResult.OnEditDelSelEntryWasCalled);

		aTester.OverridableGetActiveIndexResult = CMyCashflowDlg::tab::index::TRANSACTIONS;
		aTester.TransactionsPageDlgResult.OnEditDelSelEntryWasCalled = false;
		aTester.OnEditDelSelEntry();
		CPPUNIT_ASSERT(aTester.TransactionsPageDlgResult.OnEditDelSelEntryWasCalled);
		}

	void testMenuSave()
		{
		// add something ...
		dialog.accounts_dialog.GetListCtrl().AddEntry("Account1", 100, false);
		dialog.OnFileSave();

		// start up another one ...
		fake::ConfigDataManager aFakeConfigDataManager;
		CMyCashflowDlg aMyMoneyDlg(aFakeConfigDataManager, 13);
		aMyMoneyDlg.Create(); 
		aMyMoneyDlg.ShowWindow(SW_HIDE);

		// verify data was restored ...
		CPPUNIT_ASSERT_EQUAL(1, aMyMoneyDlg.accounts_dialog.GetListCtrl().GetItemCount());
	}

	void testSaveLoad()
		{
		// test startup with no autosave file ...
			{
			tearDown();

			// make sure the auto save file does not exist
//			CFile::Remove(CMyCashflowApp::GetDefaultFileName());
			
			// test that startup ignores the missing file ...
			setUp();
			}

		// test autosave load ...
			{
			CDate aStartDate = CDate::Today();
			dialog.cashflow_dialog.GetListCtrl().SetStartDate(aStartDate);

			// add some account entries ...
			dialog.accounts_dialog.GetListCtrl().AddEntry("Account1", 100, false);
			dialog.accounts_dialog.GetListCtrl().AddEntry("Account2", -100, false);
			
			// add some transaction entries ...
			dialog.transactions_dialog.GetListCtrl().AddEntry(aStartDate + 1, "aaa", 10);
			dialog.transactions_dialog.GetListCtrl().AddEntry(aStartDate + 2, "bbb", -10, 
				new CDateRepeaterDaily(2, 10));

			m_bDelAutoSaveFile = false;
			tearDown();

			// verify saved file is versioned ...
			{
				CStdioFile aFile(CMyCashflowApp::GetDefaultFileName(), CFile::modeRead);
				CString aDoc;
				CString aToken;
				while (aFile.ReadString(aToken)) aDoc += aToken;
				CMarkup aMarkup;
				if (aMarkup.SetDoc(aDoc))
					{
					if (aMarkup.FindElem("MYCA$HFLOW"))
						{
						CString csFormat = aMarkup.GetAttrib("Format");
						CPPUNIT_ASSERT(csFormat.GetLength() > 0);
						}
					}
			}

			setUp();

			// verify account entries were restored automatically on startup ...
			CPPUNIT_ASSERT_EQUAL(2, dialog.accounts_dialog.GetListCtrl().GetItemCount());
			CPPUNIT_ASSERT(CString("Account1") == dialog.accounts_dialog.GetListCtrl().GetItemText(0, CTransListCtrl::Column::Index::DESC));
			appl::Dollar::Value::type vAmnt= appl::Dollar::Value::FromStr(dialog.accounts_dialog.GetListCtrl().GetItemText(0, CAccountsListCtrl::Column::Index::AMOUNT));
			CPPUNIT_ASSERT_EQUAL(100, vAmnt);
			CPPUNIT_ASSERT(CString("Account2") == dialog.accounts_dialog.GetListCtrl().GetItemText(1, CTransListCtrl::Column::Index::DESC));
			vAmnt= appl::Dollar::Value::FromStr(dialog.accounts_dialog.GetListCtrl().GetItemText(1, CAccountsListCtrl::Column::Index::AMOUNT));
			CPPUNIT_ASSERT_EQUAL(-100, vAmnt);

			// verify transaction entries were restored automatically on startup ...
			CPPUNIT_ASSERT_EQUAL(2, dialog.transactions_dialog.GetListCtrl().GetItemCount());

			TransactionData* pTData = (TransactionData*) dialog.transactions_dialog.GetListCtrl().GetItemData(0);
			CDate aDate = pTData->myStartDate;
			CPPUNIT_ASSERT((aStartDate + 1) == aDate);
			CPPUNIT_ASSERT(CString("aaa") == dialog.transactions_dialog.GetListCtrl().GetItemText(0, CTransListCtrl::Column::Index::DESC));
			vAmnt= appl::Dollar::Value::FromStr(dialog.transactions_dialog.GetListCtrl().GetItemText(0, CTransListCtrl::Column::Index::AMOUNT));
			CPPUNIT_ASSERT_EQUAL(10, vAmnt);
			CDateRepeater* pRepeater = pTData->m_pRepeater;
			CPPUNIT_ASSERT_EQUAL((UINT) 1, pRepeater->Count());

			CPPUNIT_ASSERT_EQUAL((size_t) 12, dialog.cashflow_dialog.GetListCtrl().GetItemCount());
			pTData = (TransactionData*) dialog.transactions_dialog.GetListCtrl().GetItemData(1);
			pRepeater = pTData->m_pRepeater;
			aDate = pTData->myStartDate;
			CPPUNIT_ASSERT((aDate + 2) == pRepeater->Next(aDate, 1));

			// weekly repeater ...

			tearDown();
			setUp();
			dialog.transactions_dialog.GetListCtrl().AddEntry(aStartDate, "Weekly", 10, 
				new CDateRepeaterWeekly(2, 3));

			m_bDelAutoSaveFile = false;
			tearDown();
			setUp();
			CPPUNIT_ASSERT_EQUAL((size_t) 4, dialog.cashflow_dialog.GetListCtrl().GetItemCount());
			CPPUNIT_ASSERT(CString("Weekly") == dialog.transactions_dialog.GetListCtrl().GetItemText(0, CTransListCtrl::Column::Index::DESC));

			// monthly repeater ...

			tearDown();
			setUp();
			dialog.transactions_dialog.GetListCtrl().AddEntry(aStartDate, "Monthly", -10, 
				new CDateRepeaterMonthly(aStartDate.GetDay(), 2, 3));

			m_bDelAutoSaveFile = false;
			tearDown();
			setUp();
			CPPUNIT_ASSERT_EQUAL((size_t) 4, dialog.cashflow_dialog.GetListCtrl().GetItemCount());
			CPPUNIT_ASSERT(CString("Monthly") == dialog.transactions_dialog.GetListCtrl().GetItemText(0, CTransListCtrl::Column::Index::DESC));

			// yearly repeater ...

			tearDown();
			setUp();
			dialog.transactions_dialog.GetListCtrl().AddEntry(aStartDate, "Yearly", 10, 
				new CDateRepeaterYearly(1));

			m_bDelAutoSaveFile = false;
			tearDown();
			setUp();
			CPPUNIT_ASSERT_EQUAL((size_t) 3, dialog.cashflow_dialog.GetListCtrl().GetItemCount());
			CPPUNIT_ASSERT(CString("Yearly") == dialog.transactions_dialog.GetListCtrl().GetItemText(0, CTransListCtrl::Column::Index::DESC));

			dialog.cashflow_dialog.GetListCtrl().SetStartDate(CDate::Today());

			m_bDelAutoSaveFile = true;
			}
		}

	CPPUNIT_TEST_SUITE(CMyCashFlowDlgTest);
		CPPUNIT_TEST(testMenuSave);
		CPPUNIT_TEST(testOnEditDelSelEntry);
		CPPUNIT_TEST(testOnInitMenuPopup);
		CPPUNIT_TEST(testSaveLoad);
		CPPUNIT_TEST(testUpdateMenuItem);
	CPPUNIT_TEST_SUITE_END();
	};

class CAccntEditDlgTest : public CppUnit::TestFixture 
	{
	void testResource()
		{
		struct CAccntEditDlgWrap : public CAccountEditDlg { } oAccntEditDlg;
		CPPUNIT_ASSERT(oAccntEditDlg.Create(IDD_ADD_ACCOUNT));
		oAccntEditDlg.ShowWindow(SW_SHOW);

		// Verify that the description field has the focus by default.
		CWnd* pCtrl = oAccntEditDlg.GetDlgItem(IDC_DESC);
		CWnd* pFocus = oAccntEditDlg.GetFocus();
		CPPUNIT_ASSERT(pCtrl == pFocus);
		}

	void testInitDialog()
		{
			// UPDATE mode
			{
			CAccountEditDlg aDlg(CAccountEditDlg::Mode::UPDATE);
			aDlg.Create(IDD_ADD_ACCOUNT);
			CWnd* pFocus = aDlg.GetFocus();
			CPPUNIT_ASSERT_EQUAL(IDC_BALANCE, pFocus->GetDlgCtrlID());
			}	
		}

	void testDoDataExchange()
		{
		CAccountEditDlg aDlg;
		aDlg.Create(IDD_ADD_ACCOUNT);
		aDlg.ShowWindow(SW_HIDE);
		CDataExchange aDx(&aDlg, FALSE);
		aDlg.myDescription = " \t\t\t t r i m m e d\t  \t";
		aDlg.DoDataExchange(&aDx);
		aDx.m_bSaveAndValidate = TRUE;
		aDlg.DoDataExchange(&aDx);
		CPPUNIT_ASSERT_EQUAL(string("t r i m m e d"), string(aDlg.myDescription));		
		}

	CPPUNIT_TEST_SUITE(CAccntEditDlgTest);
		CPPUNIT_TEST(testDoDataExchange);
		CPPUNIT_TEST(testInitDialog);
//		CPPUNIT_TEST(testResource); // can't make this one work!
	CPPUNIT_TEST_SUITE_END();
	};

class CTranxEditDlgTest : public CppUnit::TestFixture 
	{
	void testConstructor()
	// test UPDATE mode ...
		{
		CTransEditDlg aDlg(CTransEditDlg::Mode::UPDATE);
		CPPUNIT_ASSERT(aDlg.Create(IDD_ADD_TRANSACTION));
		aDlg.ShowWindow(SW_SHOW);
		CString oText;
		aDlg.GetDlgItem(IDOK)->GetWindowText(oText);
		CString oShouldBe;		
		oShouldBe.LoadString(IDS_TRANXDLGMODEUPDATE);
		CPPUNIT_ASSERT(oShouldBe == oText);
		}

	void testOnInitDialog()
		{
		CTransEditDlg aDlg(CTransEditDlg::Mode::UPDATE);
		aDlg.Create(IDD_ADD_TRANSACTION);
		aDlg.InitializeControls(CDate::Today(), "Description", 100, new CDateRepeaterOnce());
		CPPUNIT_ASSERT_EQUAL((int) CTransEditDlg::Repeat::Ends::Index::NEVER, aDlg.m_iRepEndGrp);

		// UPDATE mode, amount field should get the focus
			{
			CTransEditDlg aDlg(CTransEditDlg::Mode::UPDATE);
			aDlg.Create(IDD_ADD_TRANSACTION);
			CWnd* pFocus = aDlg.GetFocus();
			CPPUNIT_ASSERT_EQUAL(IDC_AMT, pFocus->GetDlgCtrlID());
			}	
		}

	void testDoDataExchange()
		{
		CTransEditDlg aDlg;
		aDlg.Create(IDD_ADD_TRANSACTION);
		aDlg.ShowWindow(SW_HIDE);
		CDataExchange aDx(&aDlg, FALSE);
		aDlg.my_description = " \t\t\t t r i m m e d\t  \t";
		aDlg.DoDataExchange(&aDx);
		aDx.m_bSaveAndValidate = TRUE;
		aDlg.DoDataExchange(&aDx);
		CPPUNIT_ASSERT_EQUAL(string("t r i m m e d"), string(aDlg.my_description));		
		}

	CPPUNIT_TEST_SUITE(CTranxEditDlgTest);
		CPPUNIT_TEST(testConstructor);
		CPPUNIT_TEST(testDoDataExchange);
		CPPUNIT_TEST(testOnInitDialog);
	CPPUNIT_TEST_SUITE_END();
	};

class CCashflowPageDialogTest : public CppUnit::TestFixture 
	{
	public: 

	void setUp() 
		{ 
		}

	void tearDown() 
		{ 
		}

	void testIncDecStartDate()
		{
		fake::AccountsListCtrl aFakeAccntsListCtrl;
		fake::TransListCtrl aFakeTranxListCtrl;
		CCashflowPageDialog aCashFlowPgDlg(fake::ConfigDataManager(), &aFakeAccntsListCtrl, &aFakeTranxListCtrl);
		aCashFlowPgDlg.Create(IDD_PAGE_CASHFLOW);
		aCashFlowPgDlg.GetListCtrl().SetStartDate(CDate::Today());

		// prev day ...
		aCashFlowPgDlg.OnButtonDecDate();

		// starting date has been updated
		CPPUNIT_ASSERT((CDate::Today() - 1) == aCashFlowPgDlg.GetListCtrl().GetStartDate());

		// list has been rebuilt
		CPPUNIT_ASSERT((CDate::Today() - 1) == aCashFlowPgDlg.GetListCtrl().GetItemDate(0));

		// next day ...
		aCashFlowPgDlg.OnButtonIncDate();
		aCashFlowPgDlg.OnButtonIncDate();

		// starting date has been updated
		CPPUNIT_ASSERT((CDate::Today() + 1) == aCashFlowPgDlg.GetListCtrl().GetStartDate());
		}

	void testSetStartDate()
	{
		fake::AccountsListCtrl aFakeAccntsListCtrl;
		fake::TransListCtrl aFakeTranxListCtrl;
		CCashflowPageDialog aCashFlowPgDlg(fake::ConfigDataManager(), &aFakeAccntsListCtrl, &aFakeTranxListCtrl);
		aCashFlowPgDlg.Create(IDD_PAGE_CASHFLOW);

		aCashFlowPgDlg.GetListCtrl().SetStartDate(CDate::Today() + 10);

		// make sure the starting date was updated ...
		CPPUNIT_ASSERT(CDate::Today() + 10 == aCashFlowPgDlg.GetListCtrl().GetStartDate());

		// make sure the list was updated ...
		CPPUNIT_ASSERT(CDate::Today() + 10 == aCashFlowPgDlg.GetListCtrl().GetItemDate(0));
	}

	// todo: these tests play a role in the fitst-time StartDate failure
	// if I move them to CCashflowPageDialogFixture then it always fails
	CPPUNIT_TEST_SUITE(CCashflowPageDialogTest);
		CPPUNIT_TEST(testIncDecStartDate);
		CPPUNIT_TEST(testSetStartDate);
	CPPUNIT_TEST_SUITE_END();
	};

class CTransPgDlgTest : public CppUnit::TestFixture 
	{
	public: 

	void setUp() 
		{ 
		CleanStartup();
		}

	void tearDown() 
		{ 
		}

	class CFakeCashFlowPgDlg : public CCashflowPageDialog
		{
		};

	void testListInsertAllCashFlowEntries()
		{
		// setup ...
		fake::AccountsListCtrl aFakeAccntsListCtrl;
		fake::TransListCtrl aFakeTranxListCtrl;
		CCashflowPageDialog aCashFlowPgDlg(fake::ConfigDataManager(), &aFakeAccntsListCtrl, &aFakeTranxListCtrl);
		aCashFlowPgDlg.Create(IDD_PAGE_CASHFLOW);
		CTransactionsPageDialog aTranxPgDlg(fake::ConfigDataManager(), &aCashFlowPgDlg.GetListCtrl());
		aTranxPgDlg.Create(IDD_PAGE_TRANSACTIONS);
		aTranxPgDlg.ShowWindow(SW_HIDE);

		// add some transactions ...
		aTranxPgDlg.GetListCtrl().AddEntry(CDate::Today(), "PayDay", 500, new CDateRepeaterWeekly(2, 10));

		// empty the cashflow for this test ...
		aCashFlowPgDlg.GetListCtrl().DeleteAllItems();

		aTranxPgDlg.GetListCtrl().InsertAllCashFlowEntries();

		CPPUNIT_ASSERT_EQUAL((size_t) 10, aCashFlowPgDlg.GetListCtrl().GetItemCount());
		}

	void testListAddEntry()
		{
			// transactions with value=0 should not be added to the cashflow ...

			{
			fake::AccountsListCtrl aFakeAccntsListCtrl;
			fake::TransListCtrl aFakeTranxListCtrl;
			CCashflowPageDialog aCashFlowPgDlg(fake::ConfigDataManager(), &aFakeAccntsListCtrl, &aFakeTranxListCtrl);
			aCashFlowPgDlg.Create(IDD_PAGE_CASHFLOW);
			CTransactionsPageDialog aTranxPgDlg(fake::ConfigDataManager(), &aCashFlowPgDlg.GetListCtrl());
			aTranxPgDlg.Create(IDD_PAGE_TRANSACTIONS);
			aTranxPgDlg.ShowWindow(SW_HIDE);

			aTranxPgDlg.GetListCtrl().AddEntry(CDate::Today(), "Zero", 0, new CDateRepeaterWeekly(2));
			CPPUNIT_ASSERT_EQUAL((size_t) 1, aCashFlowPgDlg.GetListCtrl().GetItemCount()); // current balance only
			}

			// item data must be initialized ...

			{
			fake::AccountsListCtrl aFakeAccntsListCtrl;
			fake::TransListCtrl aFakeTranxListCtrl;
			CCashflowPageDialog aCashFlowPgDlg(fake::ConfigDataManager(), &aFakeAccntsListCtrl, &aFakeTranxListCtrl);
			aCashFlowPgDlg.Create(IDD_PAGE_CASHFLOW);
			CTransactionsPageDialog aTranxPgDlg(fake::ConfigDataManager(), &aCashFlowPgDlg.GetListCtrl());
			aTranxPgDlg.Create(IDD_PAGE_TRANSACTIONS);
			aTranxPgDlg.ShowWindow(SW_HIDE);

			int iAt = aTranxPgDlg.GetListCtrl().AddEntry(CDate::Today() + 10, "Description", 199, new CDateRepeaterOnce);
			TransactionData* pTData = (TransactionData*) aTranxPgDlg.GetListCtrl().GetItemData(iAt);
			CPPUNIT_ASSERT(pTData != NULL);
			CPPUNIT_ASSERT(CDate::Today() + 10 == pTData->myStartDate);
			CPPUNIT_ASSERT("Description" == pTData->myDescription);
			CPPUNIT_ASSERT_EQUAL(199, pTData->m_aAmount);
			}
		}

	void testListUpdateEntry()
		{ 
		// setup ...
		fake::AccountsListCtrl aFakeAccntsListCtrl;
		fake::TransListCtrl aFakeTranxListCtrl;
		CCashflowPageDialog aCashFlowPgDlg(fake::ConfigDataManager(), &aFakeAccntsListCtrl, &aFakeTranxListCtrl);
		aCashFlowPgDlg.Create(IDD_PAGE_CASHFLOW);
		CTransactionsPageDialog aTranxPgDlg(fake::ConfigDataManager(), &aCashFlowPgDlg.GetListCtrl());
		aTranxPgDlg.Create(IDD_PAGE_TRANSACTIONS);
		aTranxPgDlg.ShowWindow(SW_HIDE);

		aTranxPgDlg.GetListCtrl().AddEntry(CDate::Today(), "Zero", 100, new CDateRepeaterWeekly(2));

		// transactions that are updated to value=0 should be removed from the cashflow ...
		aTranxPgDlg.GetListCtrl().UpdateEntry(0, CDate::Today(), " Zero ", 0, new CDateRepeaterWeekly(2));
		CPPUNIT_ASSERT_EQUAL((size_t) 1, aCashFlowPgDlg.GetListCtrl().GetItemCount()); // current balance only
		}

	void testListAddToCashFlow()
		{
		class CCashFlowListCtrlTester : public CCashFlowListCtrl
			{
			public:
			CCashFlowListCtrlTester(IAccountsListCtrl* pAccntsListCtrl, ITransListCtrl* pTranxListCtrl) :
				CCashFlowListCtrl(fake::ConfigDataManager(), pAccntsListCtrl, pTranxListCtrl) { }
			bool m_bRefreshBalance_WasCalled;
			virtual void RefreshBalance(UINT nStart = 0) { m_bRefreshBalance_WasCalled = true; }
			};

		// setup ...
		fake::AccountsListCtrl aFakeAccntsListCtrl;
		fake::TransListCtrl aFakeTranxListCtrl;
		CCashFlowListCtrlTester aTestCashFlowListCtrl(&aFakeAccntsListCtrl, &aFakeTranxListCtrl);
		aTestCashFlowListCtrl.m_bRefreshBalance_WasCalled = false;
		CTransactionsPageDialog aTranxPgDlg(fake::ConfigDataManager(), &aTestCashFlowListCtrl);
		aTranxPgDlg.Create(IDD_PAGE_TRANSACTIONS);
		aTranxPgDlg.ShowWindow(SW_HIDE);

		aTranxPgDlg.GetListCtrl().AddEntry(CDate::Today(), "Zero", 0, new CDateRepeaterWeekly(2));

		// cashflow balance must be called when transaction amount = 0
		CPPUNIT_ASSERT(aTestCashFlowListCtrl.m_bRefreshBalance_WasCalled);
		}

	void testListFindItemByCookie()
	{
		class CCashFlowListCtrlTester : public CCashFlowListCtrl
		{
			public:
			CCashFlowListCtrlTester(IAccountsListCtrl* pAccntsListCtrl, ITransListCtrl* pTranxListCtrl) :
				CCashFlowListCtrl(fake::ConfigDataManager(), pAccntsListCtrl, pTranxListCtrl) { }
			bool m_bRefreshBalance_WasCalled;
			virtual void RefreshBalance(UINT nStart = 0) { m_bRefreshBalance_WasCalled = true; }
		};
		fake::AccountsListCtrl aFakeAccntsListCtrl;
		fake::TransListCtrl aFakeTranxListCtrl;
		CCashFlowListCtrlTester aTestCashFlowListCtrl(&aFakeAccntsListCtrl, &aFakeTranxListCtrl);
		CTransactionsPageDialog aTranxPgDlg(fake::ConfigDataManager(), &aTestCashFlowListCtrl);
		aTranxPgDlg.Create(IDD_PAGE_TRANSACTIONS);
		aTranxPgDlg.ShowWindow(SW_HIDE);
	
		CTransListCtrl& aList = aTranxPgDlg.GetListCtrl();

		aList.AddEntry(CDate::Today(), "Zero", 0, new CDateRepeaterWeekly(2));
		int iAt = aList.AddEntry(CDate::Today(), "One", 0, new CDateRepeaterWeekly(3));
		TTransactionCookie aCookie = (TTransactionCookie) aList.GetItemData(iAt);
		aList.AddEntry(CDate::Today(), "Two", 0, new CDateRepeaterWeekly(4));

		// not found ...
		CPPUNIT_ASSERT_EQUAL(-1, aList.FindItemByCookie(NULL_TRANSACTION_COOKIE));

		// found ...
		CPPUNIT_ASSERT(aList.FindItemByCookie(aCookie) >= 0);
	}

	void testListProcessColumnClick()
	{
		fake::AccountsListCtrl aFakeAccntsListCtrl;
		fake::TransListCtrl aFakeTranxListCtrl;
		CFakeCashFlowListCtrl aFakeCashFlowListCtrl(&aFakeAccntsListCtrl, &aFakeTranxListCtrl);
		CTransactionsPageDialog aTranxPgDlg(fake::ConfigDataManager(), &aFakeCashFlowListCtrl);
		aTranxPgDlg.Create(IDD_PAGE_TRANSACTIONS);
		aTranxPgDlg.ShowWindow(SW_HIDE);
		CTransListCtrl& aList = aTranxPgDlg.GetListCtrl();

		// description column (case-insensitive) ...

		CTransListCtrl::Column::sort::Ascending[CTransListCtrl::Column::Index::DESC] = true;

		aList.DeleteAllItems();
		aList.AddEntry(CDate::Today(), "zZZ", 100);
		aList.AddEntry(CDate::Today(), "DDd", 100);
		aList.AddEntry(CDate::Today(), "aAa", 100);
		aList.AddEntry(CDate::Today(), "mMM", 100);

		aList.ProcessColumnClick(CTransListCtrl::Column::Index::DESC);

		CPPUNIT_ASSERT(CTransListCtrl::Column::sort::Ascending[CTransListCtrl::Column::Index::DESC] == false);

		CPPUNIT_ASSERT_EQUAL(string("aAa"), string(aList.GetItemText(0, CTransListCtrl::Column::Index::DESC)));
		CPPUNIT_ASSERT_EQUAL(string("zZZ"), string(aList.GetItemText(3, CTransListCtrl::Column::Index::DESC)));

		aList.ProcessColumnClick(CTransListCtrl::Column::Index::DESC);
		CPPUNIT_ASSERT_EQUAL(string("zZZ"), string(aList.GetItemText(0, CTransListCtrl::Column::Index::DESC)));
		CPPUNIT_ASSERT_EQUAL(string("aAa"), string(aList.GetItemText(3, CTransListCtrl::Column::Index::DESC)));

		// date column ...

		CTransListCtrl::Column::sort::Ascending[CTransListCtrl::Column::Index::DATE] = true;

		aList.DeleteAllItems();
		aList.AddEntry(CDate::Today() + 4, "ZZZ", 300);
		aList.AddEntry(CDate::Today() + 2, "DDD", 150);
		aList.AddEntry(CDate::Today() + 3, "AAA", 200);
		aList.AddEntry(CDate::Today() + 1, "MMM", 100);

		aList.ProcessColumnClick(CTransListCtrl::Column::Index::DATE);

		CPPUNIT_ASSERT(CTransListCtrl::Column::sort::Ascending[CTransListCtrl::Column::Index::DATE] == false);

		CPPUNIT_ASSERT(CDate::Today() + 1 == aList.GetItemDate(0));
		CPPUNIT_ASSERT(CDate::Today() + 4 == aList.GetItemDate(3));

		aList.ProcessColumnClick(CTransListCtrl::Column::Index::DATE);
		CPPUNIT_ASSERT(CDate::Today() + 4 == aList.GetItemDate(0));
		CPPUNIT_ASSERT(CDate::Today() + 1 == aList.GetItemDate(3));

		// amount column ...

		CTransListCtrl::Column::sort::Ascending[CTransListCtrl::Column::Index::AMOUNT] = true;

		aList.DeleteAllItems();
		aList.AddEntry(CDate::Today(), "ZZZ", 300);
		aList.AddEntry(CDate::Today(), "DDD", 0);
		aList.AddEntry(CDate::Today(), "AAA", 200);
		aList.AddEntry(CDate::Today(), "MMM", -100);

		aList.ProcessColumnClick(CTransListCtrl::Column::Index::AMOUNT);

		CPPUNIT_ASSERT(CTransListCtrl::Column::sort::Ascending[CTransListCtrl::Column::Index::AMOUNT] == false);

		CPPUNIT_ASSERT_EQUAL((int) -100, aList.GetItemAmount(0));
		CPPUNIT_ASSERT_EQUAL((int) 0, aList.GetItemAmount(1));
		CPPUNIT_ASSERT_EQUAL((int) 200, aList.GetItemAmount(2));
		CPPUNIT_ASSERT_EQUAL((int) 300, aList.GetItemAmount(3));

		aList.ProcessColumnClick(CTransListCtrl::Column::Index::AMOUNT);

		CPPUNIT_ASSERT_EQUAL((int) -100, aList.GetItemAmount(3));
		CPPUNIT_ASSERT_EQUAL((int) 0, aList.GetItemAmount(2));
		CPPUNIT_ASSERT_EQUAL((int) 200, aList.GetItemAmount(1));
		CPPUNIT_ASSERT_EQUAL((int) 300, aList.GetItemAmount(0));

		// repeat type column ...

		CTransListCtrl::Column::sort::Ascending[CTransListCtrl::Column::Index::REPEAT] = true;

		aList.DeleteAllItems();
		aList.AddEntry(CDate::Today(), "AAA", 100, new CDateRepeaterYearly());
		aList.AddEntry(CDate::Today(), "AAA", 100, new CDateRepeaterOnce());
		aList.AddEntry(CDate::Today(), "AAA", 100, new CDateRepeaterDaily());
		aList.AddEntry(CDate::Today(), "AAA", 100, new CDateRepeaterMonthly());
		aList.AddEntry(CDate::Today(), "AAA", 100, new CDateRepeaterWeekly());

		aList.ProcessColumnClick(CTransListCtrl::Column::Index::REPEAT);

		CPPUNIT_ASSERT(CTransListCtrl::Column::sort::Ascending[CTransListCtrl::Column::Index::REPEAT] == false);

		CPPUNIT_ASSERT_EQUAL(string(CTransListCtrl::GetDisplayStr(CDateRepeaterOnce())), string(aList.GetItemText(0, CTransListCtrl::Column::Index::REPEAT)));
		CPPUNIT_ASSERT_EQUAL(string(CTransListCtrl::GetDisplayStr(CDateRepeaterDaily())), string(aList.GetItemText(1, CTransListCtrl::Column::Index::REPEAT)));
		CPPUNIT_ASSERT_EQUAL(string(CTransListCtrl::GetDisplayStr(CDateRepeaterWeekly())), string(aList.GetItemText(2, CTransListCtrl::Column::Index::REPEAT)));
		CPPUNIT_ASSERT_EQUAL(string(CTransListCtrl::GetDisplayStr(CDateRepeaterMonthly())), string(aList.GetItemText(3, CTransListCtrl::Column::Index::REPEAT)));
 		CPPUNIT_ASSERT_EQUAL(string(CTransListCtrl::GetDisplayStr(CDateRepeaterYearly())), string(aList.GetItemText(4, CTransListCtrl::Column::Index::REPEAT)));

		aList.ProcessColumnClick(CTransListCtrl::Column::Index::REPEAT);
 		CPPUNIT_ASSERT_EQUAL(string(CTransListCtrl::GetDisplayStr(CDateRepeaterYearly())), string(aList.GetItemText(0, CTransListCtrl::Column::Index::REPEAT)));
		CPPUNIT_ASSERT_EQUAL(string(CTransListCtrl::GetDisplayStr(CDateRepeaterMonthly())), string(aList.GetItemText(1, CTransListCtrl::Column::Index::REPEAT)));
		CPPUNIT_ASSERT_EQUAL(string(CTransListCtrl::GetDisplayStr(CDateRepeaterWeekly())), string(aList.GetItemText(2, CTransListCtrl::Column::Index::REPEAT)));
		CPPUNIT_ASSERT_EQUAL(string(CTransListCtrl::GetDisplayStr(CDateRepeaterDaily())), string(aList.GetItemText(3, CTransListCtrl::Column::Index::REPEAT)));
		CPPUNIT_ASSERT_EQUAL(string(CTransListCtrl::GetDisplayStr(CDateRepeaterOnce())), string(aList.GetItemText(4, CTransListCtrl::Column::Index::REPEAT)));
	}

	void testListGetItemDate()
	{
		fake::AccountsListCtrl aFakeAccntsListCtrl;
		fake::TransListCtrl aFakeTranxListCtrl;
		CFakeCashFlowListCtrl aFakeCashFlowListCtrl(&aFakeAccntsListCtrl, &aFakeTranxListCtrl);
		CTransactionsPageDialog aTranxPgDlg(fake::ConfigDataManager(), &aFakeCashFlowListCtrl);
		aTranxPgDlg.Create(IDD_PAGE_TRANSACTIONS);
		aTranxPgDlg.ShowWindow(SW_HIDE);
		CTransListCtrl& aList = aTranxPgDlg.GetListCtrl();
		aList.AddEntry(CDate::Today() + 10, "ABC", 100);
		CPPUNIT_ASSERT(CDate::Today() + 10 == aList.GetItemDate(0));
	}
			
	void testListGetItemAmount()
	{
		fake::AccountsListCtrl aFakeAccntsListCtrl;
		fake::TransListCtrl aFakeTranxListCtrl;
		CFakeCashFlowListCtrl aFakeCashFlowListCtrl(&aFakeAccntsListCtrl, &aFakeTranxListCtrl);
		CTransactionsPageDialog aTranxPgDlg(fake::ConfigDataManager(), &aFakeCashFlowListCtrl);
		aTranxPgDlg.Create(IDD_PAGE_TRANSACTIONS);
		aTranxPgDlg.ShowWindow(SW_HIDE);
		CTransListCtrl& aList = aTranxPgDlg.GetListCtrl();
		aList.AddEntry(CDate::Today(), "ABC", -100);
		CPPUNIT_ASSERT_EQUAL(-100, aList.GetItemAmount(0));
	}
			
	CPPUNIT_TEST_SUITE(CTransPgDlgTest);
		CPPUNIT_TEST(testListGetItemAmount);
		CPPUNIT_TEST(testListGetItemDate);
		CPPUNIT_TEST(testListAddEntry);
		CPPUNIT_TEST(testListAddToCashFlow);
		CPPUNIT_TEST(testListFindItemByCookie);
		CPPUNIT_TEST(testListInsertAllCashFlowEntries);
		CPPUNIT_TEST(testListProcessColumnClick);
		CPPUNIT_TEST(testListUpdateEntry);
	CPPUNIT_TEST_SUITE_END();
	};

class CChartCtrlTest : public CppUnit::TestFixture 
	{
	public: 

	void setUp() 
		{ 
		}

	void tearDown() 
		{ 
		}

	void testPlotDataCache()
	{
		fake::AccountsListCtrl aFakeAccntsListCtrl;
		fake::TransListCtrl aFakeTranxListCtrl;

		{	// min/max must be end-of-day balance only ...

			CFakeCashFlowListCtrl aFakeCashFlowListCtrl(&aFakeAccntsListCtrl, &aFakeTranxListCtrl);
			aFakeCashFlowListCtrl.AddEntry(CDate::Today(), "internal min", -500);
			aFakeCashFlowListCtrl.AddEntry(CDate::Today(), "actual min", +500);
			aFakeCashFlowListCtrl.AddEntry(CDate::Today(), "internal max", +500);
			aFakeCashFlowListCtrl.AddEntry(CDate::Today(), "actual max", -500);
			CChartCtrl::PlotDataCache aPlotDataCache(&aFakeCashFlowListCtrl);
			CPPUNIT_ASSERT_EQUAL(0, aPlotDataCache.myMinDayBalance);
			CPPUNIT_ASSERT_EQUAL(0, aPlotDataCache.myMaxDayBalance);
		}

		{	// max should be 0 when max $ balance is < 0 ...

			CFakeCashFlowListCtrl aFakeCashFlowListCtrl(&aFakeAccntsListCtrl, &aFakeTranxListCtrl);
			aFakeCashFlowListCtrl.AddEntry(CDate::Today(), "current accounts", 500);
			CChartCtrl::PlotDataCache aPlotDataCache(&aFakeCashFlowListCtrl);
			CPPUNIT_ASSERT_EQUAL(0, aPlotDataCache.myMinDayBalance);
		}

		{	//	max should be 0 when max $ balance is < 0 ...

			CFakeCashFlowListCtrl aFakeCashFlowListCtrl(&aFakeAccntsListCtrl, &aFakeTranxListCtrl);
			aFakeCashFlowListCtrl.AddEntry(CDate::Today(), "current accounts", -500);
			CChartCtrl::PlotDataCache aPlotDataCache(&aFakeCashFlowListCtrl);
			CPPUNIT_ASSERT_EQUAL(0, aPlotDataCache.myMaxDayBalance);
		}
	}

	CPPUNIT_TEST_SUITE(CChartCtrlTest);
		CPPUNIT_TEST(testPlotDataCache);
	CPPUNIT_TEST_SUITE_END();
	};

namespace {

class FakeStorage : public Settings::IStorage
	{
	public:
	typedef map<string, string> TKeyValuePair;
	map<string, TKeyValuePair> m_aCategories;
	bool m_bLoadCalled;
	virtual void LoadConfig(ISettings * settings) { m_bLoadCalled = true; }
	};

}

class StorageTest : public CppUnit::TestFixture 
	{
	public: 

	void setUp() 
		{ 
		}

	void tearDown() 
		{ 
		}

	void testInstance()
		{
		FakeStorage storage;
		storage.m_bLoadCalled = false;
		Settings::storage = &storage;
		Settings aCongiguration;
		CPPUNIT_ASSERT(storage.m_bLoadCalled == true);
		}

	void testStorageLoadConfig()
	{
		struct TestStorage : public Settings::Storage 
			{
			CString m_csGetXmlString;
			virtual CString GetXmlString() const { return m_csGetXmlString; }
			};

		// no xml data ...
		{
			TestStorage aStorage;
			Settings::storage = &aStorage;
			Settings settings;

			aStorage.m_csGetXmlString = "";
			try
			{
				aStorage.LoadConfig(&settings);
			} 
			catch (Settings::Storage::Exception::BadlyFormedXml *)
			{
				CPPUNIT_ASSERT(0); // *** should not get here!
			}
		}

		// badly formatted xml should throw ...
		{
			TestStorage aStorage;
			Settings::storage = &aStorage;
			Settings settings;

			aStorage.m_csGetXmlString = "BADLY FORMED XML";
			try
			{
				aStorage.LoadConfig(&settings);
				CPPUNIT_ASSERT(0); // *** should not get here!
			} 
			catch (Settings::Storage::Exception::BadlyFormedXml *)
			{
				// we should get here ...
			}
		}

		{
			TestStorage aStorage;
			Settings::storage = &aStorage;
			Settings settings;

			// test that data was loaded into settings from storage ...
			aStorage.m_csGetXmlString = "<MYCA$HFLOW><CASHFLOW Duration=\"39\"/></MYCA$HFLOW>";
			aStorage.LoadConfig(&settings);
			CPPUNIT_ASSERT_EQUAL(string("39"), CStringToString(settings.GetKeyValueStr(cfg::cat::CASHFLOW, Settings::category::cashflow::Key::DURATION)));

			// test minimum value limit ...
			CString csMin;
			csMin.Format("<MYCA$HFLOW><CASHFLOW Duration=\"%d\"/></MYCA$HFLOW>", CCashFlowListCtrl::Limits::Duration::MINIMUM - 1);
			aStorage.m_csGetXmlString = csMin;
			aStorage.LoadConfig(&settings);
			CPPUNIT_ASSERT_EQUAL((int) CCashFlowListCtrl::Limits::Duration::DEFAULT, settings.GetKeyValueInt(cfg::cat::CASHFLOW, Settings::category::cashflow::Key::DURATION));

			// test maximum value limit ...
			CString csMax;
			csMax.Format("<MYCA$HFLOW><CASHFLOW Duration=\"%d\"/></MYCA$HFLOW>", CCashFlowListCtrl::Limits::Duration::MAXIMUM + 1);
			aStorage.m_csGetXmlString = csMax;
			aStorage.LoadConfig(&settings);
			CPPUNIT_ASSERT_EQUAL((int) CCashFlowListCtrl::Limits::Duration::DEFAULT, settings.GetKeyValueInt(cfg::cat::CASHFLOW, Settings::category::cashflow::Key::DURATION));
		}
	}

	CPPUNIT_TEST_SUITE(StorageTest);
		CPPUNIT_TEST(testInstance); 
		CPPUNIT_TEST(testStorageLoadConfig); 
	CPPUNIT_TEST_SUITE_END();
	};

class SettingsTest : public CppUnit::TestFixture 
	{
	public: 

	void setUp() 
		{ 
		}

	void tearDown() 
		{ 
		}

	void test_backwards_compatibility()
	{
		CPPUNIT_ASSERT_EQUAL(string("MYCA$HFLOW"), string(xml::HEAD));
		CPPUNIT_ASSERT_EQUAL(string(""), string(cfg::cat::ROOT));
		CPPUNIT_ASSERT_EQUAL(string("StorageDirectory"), string(cfg::cat::root::STORAGEDIR));
		CPPUNIT_ASSERT_EQUAL(string("Frame"), string(cfg::cat::FRAME));
		CPPUNIT_ASSERT_EQUAL(string("Left"), string(Settings::category::frame::key::LEFT));
		CPPUNIT_ASSERT_EQUAL(string("Top"), string(Settings::category::frame::key::TOP));
		CPPUNIT_ASSERT_EQUAL(string("Width"), string(Settings::category::frame::key::WIDTH));
		CPPUNIT_ASSERT_EQUAL(string("Height"), string(Settings::category::frame::key::HEIGHT));
		CPPUNIT_ASSERT_EQUAL(string("CashFlow"), string(cfg::cat::CASHFLOW));
		CPPUNIT_ASSERT_EQUAL(string("Duration"), string(Settings::category::cashflow::Key::DURATION));
		CPPUNIT_ASSERT_EQUAL(string("Autosave"), string(Settings::Root::Key::AUTOSAVE));
	}

	void test_default_configuration_values()
	{
		FakeStorage storage;
		Settings::storage = &storage;
		Settings * settings = Settings::Instance();
		CPPUNIT_ASSERT_EQUAL(
			(int) CCashFlowListCtrl::Limits::Duration::DEFAULT, 
			settings->GetKeyValueInt(cfg::cat::CASHFLOW, Settings::category::cashflow::Key::DURATION));
	}

	void test_GetKeyValueInt()
	{
		FakeStorage storage;
		Settings::storage = &storage;
		Settings * settings = Settings::Instance();
		CPPUNIT_ASSERT_EQUAL(0, settings->GetKeyValueInt("non exisitent category", "non exisitent key"));
		CPPUNIT_ASSERT_EQUAL(0, settings->GetKeyValueInt(cfg::cat::CASHFLOW, "non exisitent key"));
	}

	void test_SetKeyValue_Int()
	{
		Settings * settings = Settings::Instance();
		settings->SetKeyValue("Einstein", "Special Theory", 1905);
		CPPUNIT_ASSERT_EQUAL(1905, settings->GetKeyValueInt("Einstein", "Special Theory"));
	}

	CPPUNIT_TEST_SUITE(SettingsTest);
		CPPUNIT_TEST(test_backwards_compatibility); 
		CPPUNIT_TEST(test_default_configuration_values); 
		CPPUNIT_TEST(test_GetKeyValueInt); 
		CPPUNIT_TEST(test_SetKeyValue_Int); 
	CPPUNIT_TEST_SUITE_END();
	};

class CAccntsPgDlgTest : public CppUnit::TestFixture 
	{
	public: 

	void setUp() 
		{ 
		}

	void tearDown() 
		{ 
		}

	void AddEntryToList()
		{
		fake::AccountsListCtrl aFakeAccntsListCtrl;
		fake::TransListCtrl aFakeTranxListCtrl;
		CFakeCashFlowListCtrl aFakeCashFlowListCtrl(&aFakeAccntsListCtrl, &aFakeTranxListCtrl);
		CAccountsPageDialog aAccntsPgDlg(fake::ConfigDataManager(), &aFakeCashFlowListCtrl);
		aAccntsPgDlg.Create(IDD_PAGE_ACCOUNTS);
		aAccntsPgDlg.ShowWindow(SW_HIDE);
		CAccountsListCtrl& aList = aAccntsPgDlg.GetListCtrl();

		// item data (SetItemData) field should get assigned ...
		int iAt = aList.AddEntry("entry", 100);
		AccountData* pData = (AccountData*) aList.GetItemData(iAt);
		CPPUNIT_ASSERT(pData != NULL);
		CPPUNIT_ASSERT_EQUAL(string("entry"), CStringToString(pData->myDescription));
		CPPUNIT_ASSERT_EQUAL(100, pData->myAmount);
		}

	void testListUpdateEntry()
		{
		fake::AccountsListCtrl aFakeAccntsListCtrl;
		fake::TransListCtrl aFakeTranxListCtrl;
		CFakeCashFlowListCtrl aFakeCashFlowListCtrl(&aFakeAccntsListCtrl, &aFakeTranxListCtrl);
		CAccountsPageDialog aAccntsPgDlg(fake::ConfigDataManager(), &aFakeCashFlowListCtrl);
		aAccntsPgDlg.Create(IDD_PAGE_ACCOUNTS);
		aAccntsPgDlg.ShowWindow(SW_HIDE);
		CAccountsListCtrl& aList = aAccntsPgDlg.GetListCtrl();

		int iAt = aList.AddEntry("entry", 100);
		aList.UpdateEntry(iAt, "new desc", 200);

		// should update data item ...
		AccountData* pData = (AccountData*) aList.GetItemData(iAt);
		CPPUNIT_ASSERT_EQUAL(string("new desc"), CStringToString(pData->myDescription));
		CPPUNIT_ASSERT_EQUAL(200, pData->myAmount);
		}

	void testListGetItemAmount()
		{
		fake::AccountsListCtrl aFakeAccntsListCtrl;
		fake::TransListCtrl aFakeTranxListCtrl;
		CFakeCashFlowListCtrl aFakeCashFlowListCtrl(&aFakeAccntsListCtrl, &aFakeTranxListCtrl);
		CAccountsPageDialog aAccntsPgDlg(fake::ConfigDataManager(), &aFakeCashFlowListCtrl);
		aAccntsPgDlg.Create(IDD_PAGE_ACCOUNTS);
		aAccntsPgDlg.ShowWindow(SW_HIDE);
		CAccountsListCtrl& aList = aAccntsPgDlg.GetListCtrl();
		aList.AddEntry("ABC", -100);
		CPPUNIT_ASSERT_EQUAL(-100, aList.GetItemAmount(0));
		}
			
	void testListProcessColumnClick()
		{
		fake::AccountsListCtrl aFakeAccntsListCtrl;
		fake::TransListCtrl aFakeTranxListCtrl;
		CFakeCashFlowListCtrl aFakeCashFlowListCtrl(&aFakeAccntsListCtrl, &aFakeTranxListCtrl);
		CAccountsPageDialog aAccntsPgDlg(fake::ConfigDataManager(), &aFakeCashFlowListCtrl);
		aAccntsPgDlg.Create(IDD_PAGE_ACCOUNTS);
		aAccntsPgDlg.ShowWindow(SW_HIDE);
		CAccountsListCtrl& aList = aAccntsPgDlg.GetListCtrl();

		// description column (case-insensitive) ...
		CAccountsListCtrl::Column::sort::Ascending[CAccountsListCtrl::Column::Index::DESC] = true;

		aList.DeleteAllItems();
		aList.AddEntry("zZZ", 100);
		aList.AddEntry("DDd", 100);
		aList.AddEntry("aAa", 100);
		aList.AddEntry("mMM", 100);

		aList.ProcessColumnClick(CAccountsListCtrl::Column::Index::DESC);

		CPPUNIT_ASSERT(CAccountsListCtrl::Column::sort::Ascending[CAccountsListCtrl::Column::Index::DESC] == false);

		CPPUNIT_ASSERT_EQUAL(string("aAa"), string(aList.GetItemText(0, CAccountsListCtrl::Column::Index::DESC)));
		CPPUNIT_ASSERT_EQUAL(string("zZZ"), string(aList.GetItemText(3, CAccountsListCtrl::Column::Index::DESC)));

		// amount column ...
		CAccountsListCtrl::Column::sort::Ascending[CAccountsListCtrl::Column::Index::AMOUNT] = true;

		aList.DeleteAllItems();
		aList.AddEntry("ZZZ", 300);
		aList.AddEntry("DDD", 0);
		aList.AddEntry("AAA", 200);
		aList.AddEntry("MMM", -100);

		aList.ProcessColumnClick(CAccountsListCtrl::Column::Index::AMOUNT);

		CPPUNIT_ASSERT(CAccountsListCtrl::Column::sort::Ascending[CAccountsListCtrl::Column::Index::AMOUNT] == false);

		CPPUNIT_ASSERT_EQUAL((int) -100, aList.GetItemAmount(0));
		CPPUNIT_ASSERT_EQUAL((int) 0, aList.GetItemAmount(1));
		CPPUNIT_ASSERT_EQUAL((int) 200, aList.GetItemAmount(2));
		CPPUNIT_ASSERT_EQUAL((int) 300, aList.GetItemAmount(3));
		}

	CPPUNIT_TEST_SUITE(CAccntsPgDlgTest);
		CPPUNIT_TEST(AddEntryToList);
		CPPUNIT_TEST(testListUpdateEntry);
		CPPUNIT_TEST(testListGetItemAmount);
		CPPUNIT_TEST(testListProcessColumnClick);
	CPPUNIT_TEST_SUITE_END();
};

class MyCashflowFixture : public CppUnit::TestFixture 
	{
	public: 

	void setUp() 
		{ 
		}

	void tearDown() 
		{ 
		}

	void TestConstants()
		{
		CPPUNIT_ASSERT_EQUAL(string("http://www.wesleysteiner.com/mycashflow/main.html"), CStringToString(app::data::HOMEPAGE_URL));
		CPPUNIT_ASSERT_EQUAL(string("Wesley Steiner"), CStringToString(app::data::REGISTRY_KEY));
		}

	CPPUNIT_TEST_SUITE(MyCashflowFixture);
		CPPUNIT_TEST(TestConstants);
	CPPUNIT_TEST_SUITE_END();
	};

class CAboutDlgTester : public CppUnit::TestFixture 
	{
	public: 

	void setUp() 
		{ 
		}

	void tearDown() 
		{ 
		}

	void TestGetBrowserNotFoundTitle()
		{
		CPPUNIT_ASSERT_EQUAL(string("Application Error"), CStringToString(GetBrowserNotFoundTitle()));
		CPPUNIT_ASSERT_EQUAL(string("Unable to launch your Internet browser."), CStringToString(GetBrowserNotFoundMessage().Left(39)));
		CPPUNIT_ASSERT_EQUAL(string("http://www.wesleysteiner.com/mycashflow/main.html"), CStringToString(GetBrowserNotFoundMessage().Right(49)));
		}

	CPPUNIT_TEST_SUITE(CAboutDlgTester);
		CPPUNIT_TEST(TestGetBrowserNotFoundTitle);
	CPPUNIT_TEST_SUITE_END();
	};

using namespace std;

#include <line.h>
CPPUNIT_TEST_SUITE_REGISTRATION(lineTester);

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		fake::ConfigDataManager aFakeConfigDataManager;
		theApp.main_dialog = new CMyCashflowDlg(aFakeConfigDataManager, 13) ;
		theApp.m_pMainWnd = theApp.main_dialog;
		theApp.main_dialog->Create();
		theApp.main_dialog->ShowWindow(SW_HIDE);

		CppUnit::TextUi::TestRunner runner;

		CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
		runner.addTest(registry.makeTest());

 		runner.addTest(CAboutDlgTester::suite());
		runner.addTest(CAccntsPgDlgTest::suite());
		runner.addTest(CAccntEditDlgTest::suite());
		runner.addTest(CCashflowPageDialogTest::suite());
		runner.addTest(CChartCtrlTest::suite());
		runner.addTest(CMyCashflowAppTestFixture::suite());
		runner.addTest(CMyCashFlowDlgTest::suite());
		runner.addTest(SettingsTest::suite());
		runner.addTest(CTranxEditDlgTest::suite());
		runner.addTest(CTranxListCtrlTest::suite());
		runner.addTest(CTransPgDlgTest::suite());
		runner.addTest(StorageTest::suite());
		runner.addTest(MyCashflowFixture::suite());

		if (!runner.run()) nRetCode = 2;
	}
	return nRetCode;
}

#endif TEST
