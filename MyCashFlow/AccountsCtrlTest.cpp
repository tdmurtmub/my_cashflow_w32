// Copyright (C) 2006 by Wesley Steiner. All rights reserved.

#include "stdafx.h"

#ifdef TEST

#include "AccountsCtrl.h"

#include <cppunit\extensions\HelperMacros.h>
#include "mfc.h"
#include "mfc\ConfigDataManager.h"
#include "CashFlowCtrl.h"

using namespace win::mfc;

class AccountsListCtrlTester : public CppUnit::TestFixture 
	{
	public: 
	void setUp() 
		{ 
		}

	void tearDown() 
		{ 
		}

	void test_configuration_strings()
		{
		fake::ConfigDataManager rConfig;
		fake::CashflowListCtrl list;
		CAccountsListCtrl aTester(rConfig, &list);
		CPPUNIT_ASSERT_EQUAL(string("AccountsList"), CStringToString(aTester.MySettingsCategory()));
		}

	CPPUNIT_TEST_SUITE(AccountsListCtrlTester);
		CPPUNIT_TEST(test_configuration_strings); 
	CPPUNIT_TEST_SUITE_END();
	};

CPPUNIT_TEST_SUITE_REGISTRATION(AccountsListCtrlTester);

#endif

