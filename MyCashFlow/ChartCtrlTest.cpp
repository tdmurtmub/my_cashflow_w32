// (C) 2006 Wesley Steiner

#include "stdafx.h"

#ifdef TEST

#include <cppunit\extensions\HelperMacros.h>
#include <ansi.h>
#include "mfc\IConfigDataManager.h"
#include "CashflowCtrl.h" 
#include "ChartCtrl.h" 
#include "IAccountsListCtrl.h"
#include "ITransListCtrl.h"
#include "ICashflowListCtrl.h" 

using namespace std;

inline string CStringToString(const CString & rCString) { return string((LPCTSTR) rCString); }

class ChartCtrlFixture : public CppUnit::TestFixture 
	{
	void SetYAxisWidth(uint width) const { YAxis::theCachedWidth = width; }

	public:
	void TestIntervalWidth()
		{
		CPPUNIT_ASSERT_EQUAL((size_t) 100, CChartCtrl::IntervalWidth(13, 1300));
		}

	void TestFrameRectangle()
		{
		CPPUNIT_ASSERT(CRect(0, 1, 4, 5) == CChartCtrl::FrameRectangle(xy::Rectangle(1, 2, 3, 4)));
		}

	void testOnSize()
		{
		CCashFlowListCtrl aCashflowListCtrl(fake::ConfigDataManager(), new fake::AccountsListCtrl(), new fake::TransListCtrl());
		SetYAxisWidth(10);
		CChartCtrl aChartCtrl(aCashflowListCtrl);

		// OnSize should set the thePlotSize ...
		aChartCtrl.OnSize(SIZE_RESTORED, 123, 456);
		CPPUNIT_ASSERT_EQUAL((int) (123-2-10*2), (int) aChartCtrl.the_plot_size.cx);
		CPPUNIT_ASSERT_EQUAL((int) 456-2-15, (int) aChartCtrl.the_plot_size.cy);

		// ignored for SIZE_MINIMIZED
		aChartCtrl.OnSize(SIZE_MINIMIZED, 1, 2);
		CPPUNIT_ASSERT_EQUAL((int) (123-2-10*2), (int) aChartCtrl.the_plot_size.cx);
		CPPUNIT_ASSERT_EQUAL((int) 456-2-15, (int) aChartCtrl.the_plot_size.cy);
		}

	class CChartCtrlTester : public CChartCtrl
		{
		public:
		CChartCtrlTester () : CChartCtrl(CCashFlowListCtrl(fake::ConfigDataManager(), new fake::AccountsListCtrl(), new fake::TransListCtrl())) { }
		virtual void InitializeAxis () { YAxis::theCachedWidth = 1; }
		};

	void test_OnSize_initializes_Y_axis ()
		{
		CChartCtrlTester tester;
		YAxis::theCachedWidth = YAxis::NOT_INITIALIZED;
		tester.OnSize(SIZE_RESTORED, 450, 550);
		CPPUNIT_ASSERT(YAxis::IsInitialized());
		}

	void testDeltaFromBaseline()
		{
		fake::AccountsListCtrl aFakeAccntsListCtrl;
		fake::TransListCtrl aFakeTranxListCtrl;
		CCashFlowListCtrl aCashflowListCtrl(fake::ConfigDataManager(), &aFakeAccntsListCtrl, &aFakeTranxListCtrl);
		CChartCtrl aChartCtrl(aCashflowListCtrl);

		aChartCtrl.the_plot_size = CSize(256, 128);

		// Case 2: lo=0, hi>0 ...

		// a $ balance of $0 should match the baseline ...
		CPPUNIT_ASSERT_EQUAL(0, aChartCtrl.DeltaFromBaseline(0, 127, 0, 999));

		// a positive balance should be a positive value ...
		CPPUNIT_ASSERT(aChartCtrl.DeltaFromBaseline(100, 127, 0, 999) > 0);

		// a 50% positive balance should be 1/2 the Y scale ...
		CPPUNIT_ASSERT_EQUAL(64, aChartCtrl.DeltaFromBaseline(500, 127, 0, 999));

		// Case 3: lo<0, hi>0 ...

		// a $ balance of $0 should match the baseline ...
		CPPUNIT_ASSERT_EQUAL(0, aChartCtrl.DeltaFromBaseline(0, 64, -999, 999));

		// a positive balance should be a positive value ...
		CPPUNIT_ASSERT(aChartCtrl.DeltaFromBaseline(100, 64, -999, 999) > 0);

		// a 50% positive balance should be 1/4 the Y scale ...
		CPPUNIT_ASSERT_EQUAL(32, aChartCtrl.DeltaFromBaseline(500, 64, -999, 999));

		// a negative balance should be a negative value ...
		CPPUNIT_ASSERT(aChartCtrl.DeltaFromBaseline(-100, 64, -999, 999) < 0);

		// a 50% negative balance should be 3/4 the Y scale ...
		CPPUNIT_ASSERT_EQUAL(-32, aChartCtrl.DeltaFromBaseline(-500, 64, -999, 999));

		// Case 4: lo<0, hi=0 ...

		// a $ balance of $0 should match the baseline ...
		CPPUNIT_ASSERT_EQUAL(0, aChartCtrl.DeltaFromBaseline(0, 0, -999, 0));

		// a negative balance should be a negative value ...
		CPPUNIT_ASSERT(aChartCtrl.DeltaFromBaseline(-100, 0, -999, 0) < 0);

		// a 50% negative balance should be 1/2 the Y scale ...
		CPPUNIT_ASSERT_EQUAL(-64, aChartCtrl.DeltaFromBaseline(-500, 0, -999, 0));
		}
		
	void testConstructor()
		{
		fake::AccountsListCtrl aFakeAccntsListCtrl;
		fake::TransListCtrl aFakeTranxListCtrl;
		CCashFlowListCtrl aCashflowListCtrl(fake::ConfigDataManager(), &aFakeAccntsListCtrl, &aFakeTranxListCtrl);
		CChartCtrl aChartCtrl(aCashflowListCtrl);
		}

	void testNeedToDrawPreMonth()
		{
		CPPUNIT_ASSERT(CChartCtrl::NeedToDrawPreMonth(0) == false);
		CPPUNIT_ASSERT(CChartCtrl::NeedToDrawPreMonth(4) == true);
		}
	
	void test_CalcYGridDollarDelta ()
		{
		CPPUNIT_ASSERT_EQUAL(1, (int) CChartCtrl::CalcYGridDollarDelta(9));
		CPPUNIT_ASSERT_EQUAL(10, (int) CChartCtrl::CalcYGridDollarDelta(10));
		CPPUNIT_ASSERT_EQUAL(10, (int) CChartCtrl::CalcYGridDollarDelta(60));
		CPPUNIT_ASSERT_EQUAL(10, (int) CChartCtrl::CalcYGridDollarDelta(99));
		CPPUNIT_ASSERT_EQUAL(100, (int) CChartCtrl::CalcYGridDollarDelta(100));
		CPPUNIT_ASSERT_EQUAL(100, (int) CChartCtrl::CalcYGridDollarDelta(101));
		CPPUNIT_ASSERT_EQUAL(1000, (int) CChartCtrl::CalcYGridDollarDelta(9999));
		CPPUNIT_ASSERT_EQUAL(10000, (int) CChartCtrl::CalcYGridDollarDelta(10000));
		CPPUNIT_ASSERT_EQUAL(10000, (int) CChartCtrl::CalcYGridDollarDelta(10001));
		}

	void test_CalcStartingYGridDollarValue ()
		{
		CPPUNIT_ASSERT_EQUAL(90, (int) CChartCtrl::CalcStartingYGridDollarValue(99, 10));
		CPPUNIT_ASSERT_EQUAL(100, (int) CChartCtrl::CalcStartingYGridDollarValue(100, 10));
		CPPUNIT_ASSERT_EQUAL(100, (int) CChartCtrl::CalcStartingYGridDollarValue(101, 10));
		CPPUNIT_ASSERT_EQUAL(-900, (int) CChartCtrl::CalcStartingYGridDollarValue(-999, 100));
		CPPUNIT_ASSERT_EQUAL(-1000, (int) CChartCtrl::CalcStartingYGridDollarValue(-1000, 100));
		CPPUNIT_ASSERT_EQUAL(-1000, (int) CChartCtrl::CalcStartingYGridDollarValue(-1001, 100));
		CPPUNIT_ASSERT_EQUAL(0, (int) CChartCtrl::CalcStartingYGridDollarValue(12345, 0));
		CPPUNIT_ASSERT_EQUAL(0, (int) CChartCtrl::CalcStartingYGridDollarValue(0, 0));
		}

	CPPUNIT_TEST_SUITE(ChartCtrlFixture);
		CPPUNIT_TEST(TestIntervalWidth); 
		CPPUNIT_TEST(TestFrameRectangle); 
		CPPUNIT_TEST(testOnSize);
		CPPUNIT_TEST(testDeltaFromBaseline);
		CPPUNIT_TEST(testConstructor);
		CPPUNIT_TEST(testNeedToDrawPreMonth);
		CPPUNIT_TEST(test_CalcYGridDollarDelta);
		CPPUNIT_TEST(test_CalcStartingYGridDollarValue);
		CPPUNIT_TEST(test_OnSize_initializes_Y_axis);
	CPPUNIT_TEST_SUITE_END();
	};

class XAxisFixture : public CppUnit::TestFixture 
	{
	public:
	void test_IntervalRect_with_one_grid_line()
		{
		vector<int> lines;
		lines.push_back(10);
		CRect aPlotArea(55, 77, 150, 160);
		CRect rect = XAxis::IntervalRect(aPlotArea, lines, 0, 45);
		CPPUNIT_ASSERT_EQUAL((int) 66, (int) rect.left);
		CPPUNIT_ASSERT_EQUAL((int) 161, (int) rect.top);
		CPPUNIT_ASSERT_EQUAL((int) 45, (int) rect.Width());
		CPPUNIT_ASSERT_EQUAL((int) XAxis::Height(), (int) rect.Height());
		}

	void test_IntervalRect_with_multiple_grid_lines()
		{
		vector<int> lines;
		lines.push_back(11);
		lines.push_back(14);
		CRect aPlotArea(80, 21, 250, 67);
		CRect rect0 = XAxis::IntervalRect(aPlotArea, lines, 0, 31);
		CPPUNIT_ASSERT_EQUAL((int) 2, (int) rect0.Width());
		CRect rect1 = XAxis::IntervalRect(aPlotArea, lines, 1, 31);
		CPPUNIT_ASSERT_EQUAL((int) 31, (int) rect1.Width());
		}

	void test_IntervalRect_for_previous_month()
		{
		vector<int> lines;
		lines.push_back(12);
		CRect aPlotArea(20, 80, 35, 95);
		CRect rect = XAxis::IntervalRect(aPlotArea, lines, -1, 28);
		CPPUNIT_ASSERT_EQUAL((int) 4, (int) rect.left);
		CPPUNIT_ASSERT_EQUAL((int) 96, (int) rect.top);
		CPPUNIT_ASSERT_EQUAL((int) 28, (int) rect.Width());
		CPPUNIT_ASSERT_EQUAL((int) XAxis::Height(), (int) rect.Height());
		}

	CPPUNIT_TEST_SUITE(XAxisFixture);
		CPPUNIT_TEST(test_IntervalRect_with_one_grid_line);
		CPPUNIT_TEST(test_IntervalRect_with_multiple_grid_lines);
		CPPUNIT_TEST(test_IntervalRect_for_previous_month);
	CPPUNIT_TEST_SUITE_END();
	};

class YAxisFixture : public CppUnit::TestFixture 
	{
	public:

	void test_Label ()
		{
		CPPUNIT_ASSERT_EQUAL(string("999M"), CStringToString(YAxis::Label(999000000)));
		CPPUNIT_ASSERT_EQUAL(string("999K"), CStringToString(YAxis::Label(999000)));
		CPPUNIT_ASSERT_EQUAL(string("1K"), CStringToString(YAxis::Label(1000)));
		CPPUNIT_ASSERT_EQUAL(string("900"), CStringToString(YAxis::Label(900)));
		CPPUNIT_ASSERT_EQUAL(string("0"), CStringToString(YAxis::Label(0)));
		CPPUNIT_ASSERT_EQUAL(string("-900"), CStringToString(YAxis::Label(-900)));
		CPPUNIT_ASSERT_EQUAL(string("-1K"), CStringToString(YAxis::Label(-1000)));
		CPPUNIT_ASSERT_EQUAL(string("-999M"), CStringToString(YAxis::Label(-999000000)));
		}

	void test_IsInitialized ()
		{
		YAxis::theCachedWidth = YAxis::NOT_INITIALIZED;
		CPPUNIT_ASSERT(!YAxis::IsInitialized());
		YAxis::theCachedWidth = YAxis::NOT_INITIALIZED + 1;
		CPPUNIT_ASSERT(YAxis::IsInitialized());
		}

	CPPUNIT_TEST_SUITE(YAxisFixture);
		CPPUNIT_TEST(test_Label);
		CPPUNIT_TEST(test_IsInitialized);
	CPPUNIT_TEST_SUITE_END();
	};

appl::Dollar::Value::type the_item_balances[] = {-100, 300, 200};

class PlotDataCacheFixture : public CppUnit::TestFixture 
	{
	class MockCashflow : public fake::CashflowListCtrl
		{
		virtual quantity GetItemCount() const { return ARRAYSIZE(the_item_balances); }
		virtual CDate GetItemDate(UINT nItemIndex) const { return CDate::Today() + nItemIndex; }
		virtual appl::Dollar::Value::type GetItemBalance(int iIndex) const { return the_item_balances[iIndex]; }
		};

	public:

	void test_constructor()
		{
		MockCashflow cashflow;
		CChartCtrl::PlotDataCache cache(&cashflow);
		CPPUNIT_ASSERT_EQUAL(-100, cache.myMinDayBalance);
		CPPUNIT_ASSERT_EQUAL(300, cache.myMaxDayBalance);
//		CPPUNIT_ASSERT_EQUAL(-1, cache.my_selected_item);
		}

	CPPUNIT_TEST_SUITE(PlotDataCacheFixture);
		CPPUNIT_TEST(test_constructor);
	CPPUNIT_TEST_SUITE_END();
	};

CPPUNIT_TEST_SUITE_REGISTRATION(ChartCtrlFixture);
CPPUNIT_TEST_SUITE_REGISTRATION(XAxisFixture);
CPPUNIT_TEST_SUITE_REGISTRATION(YAxisFixture);
CPPUNIT_TEST_SUITE_REGISTRATION(PlotDataCacheFixture);

#endif
