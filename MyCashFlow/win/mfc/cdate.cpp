// (C) 2006-2008 Wesley Steiner

#include "stdafx.h"

#include "cdate.h"

#include <win/mfc.h>

using namespace std;

CString MONTH_NAMES[12];

bool CMonth::Initialized = false;

CString GetLocaleMonthName(CMonth::tag tag)  
	{
	return CTime(2000, tag, 1, 0, 0, 0).Format(_T("%B"));
	}

const TCHAR * CMonth::Name(tag month) 
	{
	if (!Initialized)
		{
		for (int t = JAN; t <= DEC; ++t) MONTH_NAMES[t - 1] = GetLocaleMonthName((tag) t);
		Initialized = true;
		}
	return MONTH_NAMES[month-1].GetBuffer(); 
	}

void CDate::SetFirstDayOfMonth()
	{
	*this = CDate(GetYear(), GetMonth(), 1);
	}

#ifdef TEST

#include <cppunit\exception.h>
#include <cppunit\extensions\HelperMacros.h>

using namespace std;
using namespace win::mfc;

class CDateTest : public CppUnit::TestFixture
	{
	public:
	void setUp() { }
	void tearDown() { }

	void testCDate()
		{
		CPPUNIT_ASSERT(CDate() == CDate());
		CPPUNIT_ASSERT(CDate() == CDate::Today());

		CPPUNIT_ASSERT(CDate(2000, 1, 1) != CDate(2000, 1, 2));
		CPPUNIT_ASSERT(CDate(2000, 1, 1) != CDate(2001, 1, 1));

		CPPUNIT_ASSERT(CDate(2000, 1, 1) <= CDate(2000, 1, 1));
		CPPUNIT_ASSERT(CDate(1999, 12, 31) <= CDate(2000, 1, 1));

		CPPUNIT_ASSERT(CDate(2000, 1, 1) < CDate(2000, 1, 2));
		CPPUNIT_ASSERT(CDate(2000, 1, 1) < CDate(2000, 12, 1));
		CPPUNIT_ASSERT(CDate(2000, 1, 1) < CDate(2001, 1, 1));
		CPPUNIT_ASSERT(CDate(2000, 12, 31) < CDate(2001, 1, 1));
		CPPUNIT_ASSERT(CDate(2004, 11, 14) < CDate(2005, 11, 14));
		CPPUNIT_ASSERT(CDate(2004, 10, 31) < CDate(2004, 11, 1));

		CPPUNIT_ASSERT(!(CDate(2000, 1, 1) < CDate(2000, 1, 1)));
		CPPUNIT_ASSERT(!(CDate(2000, 1, 1) < CDate(1999, 12, 31)));

		CPPUNIT_ASSERT(CDate(2000, 1, 2) > CDate(2000, 1, 1));
		CPPUNIT_ASSERT(CDate(2000, 1, 1) > CDate(1999, 12, 31));

		CPPUNIT_ASSERT(CDate(2000, 1, 1) >= CDate(2000, 1, 1));
		CPPUNIT_ASSERT(CDate(2000, 1, 1) >= CDate(1999, 12, 31));
		CPPUNIT_ASSERT(CDate(2000, 1, 1) >= CDate(1999, 1, 1));
		CPPUNIT_ASSERT(CDate(2000, 1, 1) >= CDate(1999, 7, 1));

		// ++
			{
			CPPUNIT_ASSERT(CDate(2000, 1, 1) == ++CDate(1999, 12, 31));
			CPPUNIT_ASSERT(CDate(2000, 11, 1) == ++CDate(2000, 10, 31));
			}

		// repeated ++
			{
			CDate aStartDate(2004, 8, 1);
			CDate aDate(aStartDate);
			for (size_t i = 0; i < 750; ++i)
				{
				CPPUNIT_ASSERT((aStartDate + i) == aDate);
				++aDate;
				}
			}

		// operator--
			{
			CPPUNIT_ASSERT(CDate(1999, 12, 31) == --CDate(2000, 1, 1));
			}

		// repeated operator--
			{
			CDate aStartDate(2000, 8, 1);
			CDate aDate(aStartDate);
			for (size_t i = 0; i < 750; ++i)
				{
				CPPUNIT_ASSERT((aStartDate - i) == aDate);
				--aDate;
				}
			}
		}
	
	void testOperatorPlus()
		{
		CPPUNIT_ASSERT((CDate(1999, 12, 31) + 1) == CDate(2000, 1, 1));

		// make sure original is not modified ...
			{
			CDate aDate;
			CDate aNewDate = aDate + 1;
			CPPUNIT_ASSERT(aDate == CDate::Today());
			}
		}

	void testOperatorMinus()
		{
		CPPUNIT_ASSERT((CDate(2000, 1, 1) - 1) == CDate(1999, 12, 31));

		// make sure original is not modified ...
			{
			CDate aDate;
			CDate aNewDate = aDate - 1;
			CPPUNIT_ASSERT(aDate == CDate::Today());
			}
		}

	void testOperatorMinusEqual()
		{
		CDate aDate(2000, 1, 1);
		aDate -= 1;
		CPPUNIT_ASSERT(CDate(1999, 12, 31) == aDate);

		// repeated -=
			{
			CDate aStartDate(2000, 8, 1);
			CDate aDate(aStartDate);
			for (size_t i = 0; i < 750; ++i)
				{
				CPPUNIT_ASSERT((aStartDate - i) == aDate);
				aDate -= 1;
				}
			}
		}

	void testOperatorPlusEqual()
		{
		CDate aDate(1999, 12, 31);
		aDate += 1;
		CPPUNIT_ASSERT(CDate(2000, 1, 1) == aDate);
			
		// repeated +=
			{
			CDate aStartDate(2004, 8, 1);
			CDate aDate(aStartDate);
			for (size_t i = 0; i < 750; ++i)
				{
				CPPUNIT_ASSERT((aStartDate + i) == aDate);
				aDate += 1;
				}
			}
		}

	void testSetFirstDayOfMonth()
		{
		CDate aDate(2000, 12, 13);
		aDate.SetFirstDayOfMonth();
		CPPUNIT_ASSERT_EQUAL((int) 1, (int) aDate.GetDay());
		CPPUNIT_ASSERT_EQUAL((int) 12, (int) aDate.GetMonth());
		CPPUNIT_ASSERT_EQUAL((int) 2000, (int) aDate.GetYear());
		}

	CPPUNIT_TEST_SUITE(CDateTest);
		CPPUNIT_TEST(testCDate);
		CPPUNIT_TEST(testOperatorMinus);
		CPPUNIT_TEST(testOperatorMinusEqual);
		CPPUNIT_TEST(testOperatorPlus);
		CPPUNIT_TEST(testOperatorPlusEqual);
		CPPUNIT_TEST(testSetFirstDayOfMonth);
	CPPUNIT_TEST_SUITE_END();
	};

class CYearTest : public CppUnit::TestFixture
	{
	public:
	void setUp() { }
	void tearDown() { }

	void testCYear()
		{
		CPPUNIT_ASSERT(!CYear(1995).IsLeapYear());
		CPPUNIT_ASSERT(CYear(1996).IsLeapYear());
		CPPUNIT_ASSERT(CYear(2000).IsLeapYear());
		CPPUNIT_ASSERT(!CYear(2100).IsLeapYear());
		CPPUNIT_ASSERT(!CYear(2200).IsLeapYear());
		CPPUNIT_ASSERT(CYear(2400).IsLeapYear());
		CPPUNIT_ASSERT(CYear(2404).IsLeapYear());
		}

	CPPUNIT_TEST_SUITE(CYearTest);
		CPPUNIT_TEST(testCYear);
	CPPUNIT_TEST_SUITE_END();
	};

class CMonthIteratorFixture : public CppUnit::TestFixture
	{
	public:
	void setUp() { }
	void tearDown() { }

	void testCMonthIterator()
		{
		CMonthIterator mi(2000, 1);

		mi.Set(1999, 1);
		++mi;
		CPPUNIT_ASSERT(mi.GetMonth() == CMonthIterator::FEB);

		mi.Set(1999, 12);
		++mi;
		CPPUNIT_ASSERT(mi.GetMonth() == CMonthIterator::JAN);

		mi.Set(2000, 1);
		mi += 0;
		CPPUNIT_ASSERT(mi.GetMonth() == CMonthIterator::JAN);

		mi.Set(2000, 1);
		mi += 1;
		CPPUNIT_ASSERT(mi.GetMonth() == CMonthIterator::FEB);

		mi.Set(1999, 12);
		mi += 1;
		CPPUNIT_ASSERT(mi.GetMonth() == CMonthIterator::JAN);

		mi.Set(2000, 1);
		mi += 12;
		CPPUNIT_ASSERT(mi.GetMonth() == CMonthIterator::JAN);

		mi.Set(2000, 1);
		mi += -1;
		CPPUNIT_ASSERT(mi.GetMonth() == CMonthIterator::DEC);
		}

	CPPUNIT_TEST_SUITE(CMonthIteratorFixture);
		CPPUNIT_TEST(testCMonthIterator);
	CPPUNIT_TEST_SUITE_END();
	};

class CDateRepeaterMonthlyTest : public CppUnit::TestFixture
	{
	public:
	void setUp() { }
	void tearDown() { }

	void TestDefaultConstruction()
		{
		CDateRepeaterMonthly oRep;
		CPPUNIT_ASSERT(CDateRepeaterMonthly::INFINITE_REPEAT == oRep.Count());
		}

	void TestDaysTo28()
		{
		CDateRepeaterMonthly oRep(15);
		for (int i = 0; i < 100; ++i)
			{
			CDate oNext = oRep.Next(CDate(2000, 1, 1), i);
			CPPUNIT_ASSERT_EQUAL((UINT) 15, oNext.GetDay());
			}
		}

	void TestDaysAbove28()
		{
		CDateRepeaterMonthly oRep(29);
		for (int i = 0; i < 100; ++i)
			{
			CDate oNext = oRep.Next(CDate(2000, 1, 1), i);
			CPPUNIT_ASSERT_EQUAL(oNext.GetDay(), 
				CMonthIterator(oNext.GetYear(), oNext.GetMonth()).Days());
			}
		}
	
	CPPUNIT_TEST_SUITE(CDateRepeaterMonthlyTest);
		CPPUNIT_TEST(TestDefaultConstruction);
		CPPUNIT_TEST(TestDaysTo28);
		CPPUNIT_TEST(TestDaysAbove28);
	CPPUNIT_TEST_SUITE_END();
	};

CPPUNIT_TEST_SUITE_REGISTRATION(CDateRepeaterMonthlyTest);
CPPUNIT_TEST_SUITE_REGISTRATION(CDateTest);
CPPUNIT_TEST_SUITE_REGISTRATION(CMonthIteratorFixture);
CPPUNIT_TEST_SUITE_REGISTRATION(CYearTest);

#endif
