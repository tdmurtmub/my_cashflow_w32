// Copyright (C) 2006 by Wesley Steiner. All rights reserved.

#include "stdafx.h"

#ifdef TEST

#include "mfc\ConfigDataManager.h"

#include <cppunit\extensions\HelperMacros.h>

using namespace std;
using namespace win::mfc;
using namespace app;

class ConfigDataManagerTester : public CppUnit::TestFixture 
	{
	ConfigDataManager & m_rMgr;

	public:
	ConfigDataManagerTester() : m_rMgr(ConfigDataManager::Instance())
		{
		}

/*	void TestDefaults()
		{
		CPPUNIT_ASSERT_EQUAL(string("default"), CStringToString(m_rMgr.GetKeyValue("non existent category", "non existent key", "default")));
		CPPUNIT_ASSERT_EQUAL(-1, m_rMgr.GetKeyValue("non existent category", "non existent key", -1));
		}
*/
/*	void TestStringValues()
		{
		m_rMgr.SetKeyValue("CAT1", "VAL1", "value");
		CPPUNIT_ASSERT_EQUAL(string("value"), CStringToString(m_rMgr.GetKeyValue("CAT1", "VAL1", "default")));
		}

	void TestIntValues()
		{
		m_rMgr.SetKeyValue("CAT2", "VAL2", -999);
		CPPUNIT_ASSERT_EQUAL(-999, m_rMgr.GetKeyValue("CAT2", "VAL2", 0));
		}
*/
/*	void TestCommit()
		{
		fake::ConfigDataStorage aFakeStorage;
		ConfigDataManager aMgr(aFakeStorage);
		aMgr.SetKeyValue("CAT1", "VAL1", 1);
		aMgr.SetKeyValue("CAT2", "VAL1", 2);
		aMgr.SetKeyValue("CAT1", "VAL2", 3);
		aMgr.Commit();
		CPPUNIT_ASSERT_EQUAL(3, aFakeStorage.WriteDataInfo.Called());
		CPPUNIT_ASSERT_EQUAL(string("CAT2"), CStringToString(aFakeStorage.m_aLastCategory));
		CPPUNIT_ASSERT_EQUAL(string("VAL1"), CStringToString(aFakeStorage.m_aLastKey));
		CPPUNIT_ASSERT_EQUAL(string("2"), CStringToString(aFakeStorage.m_aLastValue));
		}
*/
	CPPUNIT_TEST_SUITE(ConfigDataManagerTester);
//		CPPUNIT_TEST(TestDefaults); 
//		CPPUNIT_TEST(TestStringValues); 
//		CPPUNIT_TEST(TestIntValues); 
//		CPPUNIT_TEST(TestCommit); 
	CPPUNIT_TEST_SUITE_END();
	};

CPPUNIT_TEST_SUITE_REGISTRATION(ConfigDataManagerTester);

#endif

