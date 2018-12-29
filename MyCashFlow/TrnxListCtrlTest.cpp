// (C) 2006 Wesley Steiner

#include "stdafx.h"

#ifdef TEST

#include "TrnxListCtrl.h"

#include <cppunit\extensions\HelperMacros.h>
#include "mfc.h"
#include "mfc\ConfigDataManager.h"
#include "CashFlowCtrl.h"

using namespace win::mfc;

class TransListCtrlTester : public CppUnit::TestFixture 
	{
	void TestConfigurationStrings()
		{
		fake::ConfigDataManager rConfig;
		fake::CashflowListCtrl list;
		CTransListCtrl tester(rConfig, &list);
		CPPUNIT_ASSERT_EQUAL(string("TransactionsList"), CStringToString(tester.MySettingsCategory()));
		}

	CPPUNIT_TEST_SUITE(TransListCtrlTester );
		CPPUNIT_TEST(TestConfigurationStrings); 
	CPPUNIT_TEST_SUITE_END();
	};

CPPUNIT_TEST_SUITE_REGISTRATION(TransListCtrlTester);

#endif

