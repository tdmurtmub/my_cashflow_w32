// (C) 2006 Wesley Steiner

#include "stdafx.h"

#ifdef TEST

#include "CashflowCtrl.h"

#include <cppunit\extensions\HelperMacros.h>

#include "mfc.h"
#include "mfc\cdate.h"
#include "mfc\ConfigDataManager.h"

#include "AccountsCtrl.h"
#include "TrnxListCtrl.h"

using namespace win::mfc;

class CCashFlowListCtrlFixture : public CppUnit::TestFixture 
	{
	public:

	fake::AccountsListCtrl m_aList1;
	fake::TransListCtrl m_aList2;

	void setUp() { }
	void tearDown() { }

	void TestConfigurationStrings()
		{
		fake::ConfigDataManager aConfig;
		CCashFlowListCtrl aTester(aConfig, &m_aList1, &m_aList2);
		CPPUNIT_ASSERT_EQUAL(string("CashflowList"), CStringToString(aTester.MySettingsCategory()));
		}

	void testConstructor()
		{
		fake::AccountsListCtrl fakeAccntsListCtrl;
		fake::TransListCtrl fakeTranxListCtrl;
		fake::ConfigDataManager aFakeConfigDataManager;
		CCashFlowListCtrl aCashFlowListCtrl(aFakeConfigDataManager, &fakeAccntsListCtrl, &fakeTranxListCtrl);

		// default start date is today
		CPPUNIT_ASSERT(aCashFlowListCtrl.GetStartDate() == CDate::Today());

		// ending date should be ...
		CPPUNIT_ASSERT(aCashFlowListCtrl.GetEndDate() == CDate::Today() + (31 * 13));
		}

	void testChangeDuration()
	{
		struct CCashFlowListCtrlTester : public CCashFlowListCtrl 
		{
			CCashFlowListCtrlTester(IAccountsListCtrl* pAccntsListCtrl, ITransListCtrl* pTranxListCtrl) :
			CCashFlowListCtrl(fake::ConfigDataManager(), pAccntsListCtrl, pTranxListCtrl) { }
			bool Rebuild_wasCalled;
			virtual void Rebuild() { Rebuild_wasCalled = true; }
		};
		
		fake::AccountsListCtrl fakeAccntsListCtrl;
		fake::TransListCtrl fakeTranxListCtrl;
		CCashFlowListCtrlTester aList(&fakeAccntsListCtrl, &fakeTranxListCtrl);
		
		aList.Rebuild_wasCalled = false;
		
		// min ...
		aList.ChangeDuration(CCashFlowListCtrl::Limits::Duration::MINIMUM);
		CPPUNIT_ASSERT_EQUAL((aList.GetStartDate() + (CCashFlowListCtrl::Limits::Duration::MINIMUM * 31)).GetDays(), aList.GetEndDate().GetDays());

		// max ...
		aList.ChangeDuration(CCashFlowListCtrl::Limits::Duration::MAXIMUM);
		CPPUNIT_ASSERT_EQUAL((aList.GetStartDate() + (CCashFlowListCtrl::Limits::Duration::MAXIMUM * 31)).GetDays(), aList.GetEndDate().GetDays());
		
		// make sure Rebuild was called
		CPPUNIT_ASSERT(aList.Rebuild_wasCalled);
	}

	void testDoDblClickOnItem()
		{
		fake::AccountsListCtrl fakeAccntsListCtrl;
		fake::TransListCtrl fakeTranxListCtrl;

		struct CCashFlowListCtrlTester : public CCashFlowListCtrl
			{
			ItemData myItemData;
			CCashFlowListCtrlTester(IAccountsListCtrl * accounts, ITransListCtrl * transactions) : 
				CCashFlowListCtrl(fake::ConfigDataManager(), accounts, transactions), 
				myItemData(CDate::Today(), (TTransactionCookie) 123) { }
			virtual DWORD GetItemDataV(int nItem) const { return (DWORD) &myItemData; }
			bool IsTransaction_result;
			virtual bool IsTransaction(UINT iIndex) const { return IsTransaction_result; }
			bool EditStartDate_was_called;
			virtual void EditStartDate() { EditStartDate_was_called = true; }
			};

		CCashFlowListCtrlTester aList(&fakeAccntsListCtrl, &fakeTranxListCtrl);

		aList.EditStartDate_was_called = false;
		aList.EditEntry(0);
		CPPUNIT_ASSERT(aList.EditStartDate_was_called);

		aList.IsTransaction_result = false;
		aList.EditStartDate_was_called = false;
		aList.EditEntry(1);
		CPPUNIT_ASSERT(!aList.EditStartDate_was_called);

		aList.IsTransaction_result = true;
		fakeTranxListCtrl.EditEntryWasCalled = false;
		aList.EditEntry(1);
		CPPUNIT_ASSERT(fakeTranxListCtrl.EditEntryWasCalled);

		aList.IsTransaction_result = false;
		fakeTranxListCtrl.EditEntryWasCalled = false;
		aList.EditEntry(1);
		CPPUNIT_ASSERT(!fakeTranxListCtrl.EditEntryWasCalled);
		}
	 
	void testEditStartDate()
		{

		struct CCashFlowListCtrlTester : public CCashFlowListCtrl
			{
			CCashFlowListCtrlTester(IAccountsListCtrl* pAccntsListCtrl, ITransListCtrl* pTranxListCtrl) : 
				CCashFlowListCtrl(fake::ConfigDataManager(), pAccntsListCtrl, pTranxListCtrl) { }

			bool SetStartDateWasCalled;
			CDate SetStartDateArg;
			virtual void SetStartDate(const CDate& aNewStartDate)
				{
				SetStartDateWasCalled = true;
				SetStartDateArg = aNewStartDate;
				}

			CDate GetStartDateFromUserResult;
			virtual CDate GetStartDateFromUser() 
				{ 
				return GetStartDateFromUserResult; 
				}
			};

		fake::AccountsListCtrl fakeAccntsListCtrl;
		fake::TransListCtrl fakeTranxListCtrl;
		CCashFlowListCtrlTester aList(&fakeAccntsListCtrl, &fakeTranxListCtrl);

		aList.theStartDate = CDate(2005, 10, 31);

		aList.GetStartDateFromUserResult = CDate(2005, 11, 1);
		aList.SetStartDateWasCalled = false;
		aList.EditStartDate();
		CPPUNIT_ASSERT(aList.SetStartDateWasCalled);
		CPPUNIT_ASSERT(aList.GetStartDateFromUserResult == aList.SetStartDateArg);

		aList.GetStartDateFromUserResult = aList.theStartDate;
		aList.SetStartDateWasCalled = false;
		aList.EditStartDate();
		CPPUNIT_ASSERT(!aList.SetStartDateWasCalled);
		}

	CPPUNIT_TEST_SUITE(CCashFlowListCtrlFixture);
		CPPUNIT_TEST(TestConfigurationStrings); 
		CPPUNIT_TEST(testConstructor);
		CPPUNIT_TEST(testDoDblClickOnItem);
		CPPUNIT_TEST(testChangeDuration);
		CPPUNIT_TEST(testEditStartDate);
	CPPUNIT_TEST_SUITE_END();
	};

CPPUNIT_TEST_SUITE_REGISTRATION(CCashFlowListCtrlFixture);

#endif

