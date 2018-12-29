// (C) 2004 Wesley Steiner

#ifndef _MODULE_UTILITY_FIXEDARRAY_UNIT_TEST_
#define _MODULE_UTILITY_FIXEDARRAY_UNIT_TEST_

#include <cppunit\extensions\HelperMacros.h>
#include <Utility\FixedArray.h>

using namespace std;

namespace Utility {

class FixedArrayTest : public CppUnit::TestFixture
	{
	public:
	void setUp() { }
	void tearDown() { }

	void testFixedArray()
		{
			{
			FixedArray<int> aArray(0);
			CPPUNIT_ASSERT_EQUAL((size_t) 0, aArray.Size());
			}

			{
			FixedArray<int> aArray(100);
			CPPUNIT_ASSERT_EQUAL((size_t) 100, aArray.Size());
			aArray[30] = 123;
			CPPUNIT_ASSERT_EQUAL(123, aArray[30]);
			}
		}
	
	void testCopy()
		{
		static const char* str_p = "Test String";
		FixedArray<char> aArray(strlen(str_p));
		aArray.Copy(str_p);
		CPPUNIT_ASSERT(memcmp(str_p, aArray, aArray.Size()) == 0);
		}

	CPPUNIT_TEST_SUITE(FixedArrayTest);
		CPPUNIT_TEST(testCopy);
		CPPUNIT_TEST(testFixedArray);
	CPPUNIT_TEST_SUITE_END();
	};

}

#endif 
