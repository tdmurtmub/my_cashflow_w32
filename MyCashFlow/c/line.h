// Copyright (C) 2006 Wesley Steiner. All rights reserved.

#pragma once

#ifdef __cplusplus

#include <numex.h>

namespace line
	{
	inline int CenterPoint(int low, int high) { return CENTEROF(low, high); }
	}

#endif

#ifdef TEST

#include <cppunit\extensions\HelperMacros.h>

class lineTester : public CppUnit::TestFixture 
	{
	public:
	void TestCenterPoint()
		{
		CPPUNIT_ASSERT_EQUAL((int) 45, line::CenterPoint((int) 45, (int) 45));
		CPPUNIT_ASSERT_EQUAL((int) -6, line::CenterPoint((int) -8, (int) -4));
		CPPUNIT_ASSERT_EQUAL((int) -3, line::CenterPoint((int) -6, (int) 0));
		CPPUNIT_ASSERT_EQUAL((int)  0, line::CenterPoint((int) -5, (int) 5));
		CPPUNIT_ASSERT_EQUAL((int)  3, line::CenterPoint((int)  1, (int) 5));
		}

	CPPUNIT_TEST_SUITE(lineTester);
		CPPUNIT_TEST(TestCenterPoint); 
	CPPUNIT_TEST_SUITE_END();
	};

#endif
