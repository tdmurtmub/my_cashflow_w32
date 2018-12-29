#include "stdafx.h"
#include <cppunit\extensions\HelperMacros.h>
#include <lib.h>

class LibTester : public CppUnit::TestFixture 
	{
	public: 
	void setUp() 
		{ 
		}

	void tearDown() 
		{ 
		}

	void test_LoWord()
		{
		CPPUNIT_ASSERT_EQUAL((word) 65535, LoWord(0x0000FFFF));
		CPPUNIT_ASSERT_EQUAL((word) 0, LoWord(0x00000000));
		CPPUNIT_ASSERT_EQUAL((word) 1, LoWord(0x12340001));
		}

	void test_HiWord()
		{
		CPPUNIT_ASSERT_EQUAL((word) 65535, HiWord(0xFFFF0000));
		CPPUNIT_ASSERT_EQUAL((word) 0, HiWord(0x00000000));
		CPPUNIT_ASSERT_EQUAL((word) 1, HiWord(0x00019876));
		}

	CPPUNIT_TEST_SUITE(LibTester);
		CPPUNIT_TEST(test_LoWord); 
		CPPUNIT_TEST(test_HiWord); 
	CPPUNIT_TEST_SUITE_END();
	};

CPPUNIT_TEST_SUITE_REGISTRATION(LibTester);
