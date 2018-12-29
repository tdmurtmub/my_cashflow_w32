// (C) 2004 Wesley Steiner

#pragma once

#include "MyMoney.h"

#include <map>
#include <vector>
#include <xy.h>

class CCashFlowListCtrl;

interface ICashflowListCtrl;

class ChartAxis
	{
	protected:
	static CFont theFont;
	public:
	ChartAxis () 
		{ 
		#ifndef TEST
		WAXONZ(theFont.CreatePointFont(80, "Arial")); 
		#endif
		}
	static CFont * TheFont() { return &theFont; }
	};

class XAxis : public ChartAxis
	{
	public:
	static CRect IntervalRect(const CRect & aPlotArea, std::vector<int> intervalLines, int intervalIndex, size_t defaultWidth);
	static uint Height(); 
	static CString Label(size_t year, CMonth::tag month, size_t nChars);
	static size_t CharsWide(CDC & cdc, int pixelsWide);
	};

class CChartCtrl;

class YAxis : public ChartAxis
	{
	test_private:
	static uint theCachedWidth;
	public:
	enum { NOT_INITIALIZED = 0 };
	static CString Label (int value);
	static uint Width (); 
	static void Initialize (CDC & dc);
	static bool IsInitialized () { return theCachedWidth != NOT_INITIALIZED; }
	friend class CChartCtrl;
	};

class CChartCtrl : public CWnd
	{
	private:
	CCashFlowListCtrl & myCashFlowListCtrl;

	static CPen the_pos_balance_pen;
	static CPen the_neg_balance_pen;
	static CPen the_zeroline_pen;
	static CPen theGridLinePen;
	static CPen theGrayPen;
	static CSize the_plot_size;

	ChartAxis definitionRequiredToInitializeTheFont;

	static CPen * TheTickLinePen() { return &theGrayPen; }
	static xy::Rectangle thePlotArea;
	static const xy::Rectangle & ThePlotArea();
	static CRect FrameRectangle(const xy::Rectangle & plotArea);

	static int DeltaFromBaseline (appl::Dollar::Value::type aBalance, int iBaseLnY, int iMinBalance, int iMaxBalance);
	static bool NeedToDrawPreMonth (int firstVertGridLineOffset);
	static size_t CalcYGridDollarDelta (size_t aDollarRange);
	static int CalcStartingYGridDollarValue (int aBalanceHigh, size_t aDeltaInDollars);
	
	struct PlotDataCache
		{	 
		PlotDataCache(ICashflowListCtrl * pICashFlowListCtrl);
		appl::Dollar::Value::type myMinDayBalance;
		appl::Dollar::Value::type myMaxDayBalance;
		typedef std::map< size_t /* CDate */, appl::Dollar::Value::type, std::greater<size_t> > TDataPoints;
//		typedef CMap<size_t, size_t, int, int> TDataPoints;
		TDataPoints myEodDataPoints;
		int my_selected_item;
		};

	static size_t IntervalWidth(size_t months, size_t plotAreaWidth);

	typedef std::vector< std::pair<int,int> > HGridInfo;
	HGridInfo DrawHorzGridLines(CDC & dc, int aMinBalance, int aMaxBalance) const;
	std::vector<int> DrawVertGridLines(CDC &dc, size_t days) const;
	void DrawXAxis(CDC &dc, std::vector<int> &vertGridLines, size_t nominalWidth) const;
	void DrawYAxis(CDC &dc, HGridInfo const &hGridInfo) const;
	void DrawZeroLine(CDC &dc, int atY) const;
	void DrawMonth(CDC &dc, const CRect &rect, CString const &month) const;
	void CachePlotSize(int cx, int cy) const;
	void DrawPlotData(CDC &dc, PlotDataCache const &plot_data, uint days, int baseline, std::vector<int> &vertGridLines, HGridInfo &hGridInfo) const;

	public:
	CChartCtrl(CCashFlowListCtrl & rCashFlowListCtrl);

	BOOL Create(CWnd * poParent, const CRect & rRect);
	void Refresh();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChartCtrl)
	public:
	//}}AFX_VIRTUAL

	virtual ~CChartCtrl();

	protected:
	//{{AFX_MSG(CChartCtrl)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	test_private:
	test_virtual void InitializeAxis ();

	friend class CChartCtrlTest;
	friend class ChartCtrlFixture;
	friend class PlotDataCacheFixture;
	};
