// Converter Unit Testing
// Copyright (C) 2004 Wesley Steiner

#ifndef _MODULE_NUMERIC_CONVERTER_UNIT_TEST_
#define _MODULE_NUMERIC_CONVERTER_UNIT_TEST_

#include <cppunit\extensions\HelperMacros.h>
#include <Numeric\Converter.h>

using namespace std;

using namespace Numeric;

class ConverterTest : public CppUnit::TestFixture
	{
	public:
	void setUp() { }
	void tearDown() { }

	void testConverter()
		{
		CPPUNIT_ASSERT_EQUAL(string("-123"), Converter<int>::ToString(-123));
		CPPUNIT_ASSERT_EQUAL(string("123"), Converter<size_t>::ToString(123));
		CPPUNIT_ASSERT_EQUAL(string("123.45"), Converter<float>::ToString(123.45f));
		CPPUNIT_ASSERT_EQUAL(string("0.45"), Converter<float>::ToString(.45f));
		CPPUNIT_ASSERT_EQUAL(string("0.45"), Converter<float>::ToString(0.45f));
		CPPUNIT_ASSERT_EQUAL(string("0.45"), Converter<float>::ToString(0.450000f));
		CPPUNIT_ASSERT_EQUAL(string("0.45"), Converter<float>::ToString(00.450000f));
		CPPUNIT_ASSERT_EQUAL(string("0.45"), Converter<float>::ToString(000.450000f));
		CPPUNIT_ASSERT_EQUAL(string("0.45"), Converter<float>::ToString(0000.450000f));
		CPPUNIT_ASSERT_EQUAL(string("123.45"), Converter<double>::ToString(123.45));
		}
	
	void testFromString()
		{
		// plain conditions ...

		CPPUNIT_ASSERT_EQUAL(123, FromString::ToInt("123"));
		CPPUNIT_ASSERT_EQUAL(-123, FromString::ToInt("-123"));
		CPPUNIT_ASSERT_EQUAL(123, FromString::ToInt(" 123 "));
		CPPUNIT_ASSERT_EQUAL(123, FromString::ToInt("\t123\t"));

		CPPUNIT_ASSERT_EQUAL((size_t) 123, FromString::ToUInt("123"));
		CPPUNIT_ASSERT_EQUAL((size_t) 123, FromString::ToUInt(" 123 "));
		CPPUNIT_ASSERT_EQUAL((size_t) -1, FromString::ToUInt("-1"));

		// edge conditions ...

		CPPUNIT_ASSERT_EQUAL(0, FromString::ToInt(""));
		CPPUNIT_ASSERT_EQUAL(0, FromString::ToInt("  "));
		CPPUNIT_ASSERT_EQUAL(0, FromString::ToInt(" "));
		CPPUNIT_ASSERT_EQUAL(0, FromString::ToInt("\t"));
		CPPUNIT_ASSERT_EQUAL(0, FromString::ToInt("\n"));
		CPPUNIT_ASSERT_EQUAL(0, FromString::ToInt("ABC"));
		CPPUNIT_ASSERT_EQUAL(0, FromString::ToInt("*&^$@#*&("));
		}
	
	CPPUNIT_TEST_SUITE(ConverterTest);
		CPPUNIT_TEST(testFromString);
		CPPUNIT_TEST(testConverter);
	CPPUNIT_TEST_SUITE_END();
	};

#endif 
