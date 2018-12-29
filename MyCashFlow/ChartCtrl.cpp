// (C) 2004 Wesley Steiner

#include "stdafx.h"

#include "ChartCtrl.h"

#include <limits>
#include <line.h>
#include <numeric\converter.h>
#include <win\gdi.h>
#include "MyMoneyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;
using namespace win;

xy::Rectangle CChartCtrl::thePlotArea;

CChartCtrl::CChartCtrl(CCashFlowListCtrl & aCashFlowListCtrl) : CWnd(),
	myCashFlowListCtrl(aCashFlowListCtrl)
	{
	}

CChartCtrl::~CChartCtrl()
	{
	}

BEGIN_MESSAGE_MAP(CChartCtrl, CWnd)
	//{{AFX_MSG_MAP(CChartCtrl)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

#define GRID_COLOR RGB(96, 96, 96)

CPen CChartCtrl::the_pos_balance_pen(PS_SOLID, 1, win::GDI::RGBColor::BLACK);
CPen CChartCtrl::the_neg_balance_pen(PS_SOLID, 1, win::GDI::RGBColor::RED);
CPen CChartCtrl::the_zeroline_pen(PS_SOLID, 1, win::GDI::RGBColor::BLUE);
CPen CChartCtrl::theGridLinePen(PS_DOT, 1, GRID_COLOR);
CPen CChartCtrl::theGrayPen(PS_SOLID, 1, GRID_COLOR);

#include "CashFlowCtrl.h" //?
#include ".\chartctrl.h"

CChartCtrl::PlotDataCache::PlotDataCache(ICashflowListCtrl *cashflow) :
	myMinDayBalance(numeric_limits<appl::Dollar::Value::type>::max()),
	myMaxDayBalance(numeric_limits<appl::Dollar::Value::type>::min())
//	my_selected_item((((CCashFlowListCtrl*)cashflow)->GetSelectedItemIndex() == -1) ? -1 : cashflow->GetItemDate(((CCashFlowListCtrl*)cashflow)->GetSelectedItemIndex()).GetDays()) //? 
	{ 
	for (size_t i = 0; i < cashflow->GetItemCount(); ++i) 
		{
		myEodDataPoints[cashflow->GetItemDate(i).GetDays()] = cashflow->GetItemBalance(i);
		}

	// discover the eod min and max values ...
	//POSITION pos = myEodDataPoints.GetStartPosition();
	//while (pos != NULL)
	//{
	//	size_t day;
	//	int amount;
	//	myEodDataPoints.GetNextAssoc(pos, day, pPerson);
	//}
	for (TDataPoints::const_iterator it = myEodDataPoints.begin(); it != myEodDataPoints.end(); ++it)
		{
		myMinDayBalance = min(myMinDayBalance, (*it).second);
		myMaxDayBalance = max(myMaxDayBalance, (*it).second);
		}

	// ensure chart bottoms or tops out at zero ...
	myMinDayBalance = min(0, myMinDayBalance);
	myMaxDayBalance = max(0, myMaxDayBalance);
	}

void CChartCtrl::DrawMonth(CDC & dc, const CRect & rect, CString const & month) const
	{
	int x = rect.left;
	if ((x >= ThePlotArea().Left()) && (x <= ThePlotArea().Right())) dc.TextOut(x, rect.top, month);
	}

inline const CRect & RectangleToCRect(const xy::Rectangle & rectangle)
	{
	return (CRect &) rectangle;
	}

void CChartCtrl::DrawXAxis(CDC & dc, vector<int> & vertGridLines, size_t nominalWidth) const
	{
	ASSERT(vertGridLines.size() > 0);
	FontSelector fs(dc, XAxis::TheFont());
	PenSelector ps(dc, TheTickLinePen());
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextAlign(TA_TOP + TA_CENTER);
	CDate currentDay = myCashFlowListCtrl.GetStartDate();
	currentDay.SetFirstDayOfMonth();
	CMonthIterator month(currentDay);
	if (NeedToDrawPreMonth(vertGridLines[0])) 
		{
		DrawMonth(dc, XAxis::IntervalRect(RectangleToCRect(ThePlotArea()), vertGridLines, -1, nominalWidth), XAxis::Label(month.GetYear(), month.GetMonth(), XAxis::CharsWide(dc,nominalWidth)));
		++month;
		}
	for (size_t i = 0; i < vertGridLines.size(); ++i)
		{
		DrawMonth(dc, XAxis::IntervalRect(RectangleToCRect(ThePlotArea()), vertGridLines, i, nominalWidth), XAxis::Label(month.GetYear(), month.GetMonth(), XAxis::CharsWide(dc, nominalWidth)));
		++month;
		}
	};

void CChartCtrl::DrawZeroLine(CDC & dc, int atY) const
	{
	PenSelector ps(dc, &the_zeroline_pen);
	dc.MoveTo(ThePlotArea().Left(), ThePlotArea().Top() + atY);
	dc.LineTo(ThePlotArea().Right(), ThePlotArea().Top() + atY);
	}

vector<int> CChartCtrl::DrawVertGridLines(CDC &dc, size_t days) const
	{
	vector<int> lines;
	CDate currentDay = myCashFlowListCtrl.GetStartDate();
	size_t iNextDay = 0;
	PenSelector ps(dc, &theGridLinePen);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextAlign(TA_TOP + TA_CENTER);
	while (currentDay < myCashFlowListCtrl.GetEndDate())
		{
		if (currentDay.GetDay() == 1)
			{
			int x = MulDiv(the_plot_size.cx, iNextDay, days);
			ASSERT(x >= 0 && x < the_plot_size.cx);
			lines.push_back(x);
			dc.MoveTo(ThePlotArea().Left() + x, ThePlotArea().Top());
			dc.LineTo(ThePlotArea().Left() + x, ThePlotArea().Bottom());
			}
		++currentDay;
		++iNextDay;
		}
	return lines;
	}

class CDrawingSlate : public CDC, utility_declare_no_copy
	{
	private:
	CBitmap my_bitmap, *my_prev_bitmap;

	public:
	CDrawingSlate(CDC *dc, number w, number h) : CDC() 
		{ 
		WAXONZ(CreateCompatibleDC(dc));
		WAXONZ(my_bitmap.CreateCompatibleBitmap(dc, w, h));
		my_prev_bitmap = (CBitmap*) SelectObject(&my_bitmap);
		FillSolidRect(0, 0, w, h, GDI::RGBColor::WHITE);
		}

	~CDrawingSlate()
		{
		SelectObject(my_prev_bitmap);
		}
	};

void CChartCtrl::DrawPlotData(CDC &dc, CChartCtrl::PlotDataCache const &plot_data, uint days, int baseline, std::vector<int> &vertGridLines, HGridInfo &hGridInfo) const
	{
	CDrawingSlate slate(&dc, the_plot_size.cx, the_plot_size.cy);
	int delta_from_baseline = 0;
	for (int x = 0; x < the_plot_size.cx; ++x)
		{
		// what date does this x coord represent ...
		CDate date = myCashFlowListCtrl.GetStartDate() + MulDiv(days, x, the_plot_size.cx);
		ASSERT(MulDiv(days, x, the_plot_size.cx) != -1);

		// if there is a matching balance for this date use it as the new bar delta ...
		PlotDataCache::TDataPoints::const_iterator it = plot_data.myEodDataPoints.lower_bound(date.GetDays());
		if (it != plot_data.myEodDataPoints.end()) delta_from_baseline = DeltaFromBaseline((*it).second, baseline, plot_data.myMinDayBalance, plot_data.myMaxDayBalance);

		if (delta_from_baseline != 0)
			{
			PenSelector ps(slate, (delta_from_baseline < 0) ? &the_neg_balance_pen : &the_pos_balance_pen);
			int y = baseline;
			if (delta_from_baseline > 0) --y;
			slate.MoveTo(x, y);
			slate.LineTo(x, y - delta_from_baseline);
			}
			
		// prototype selection marker	
		//if ((plot_data.my_selected_item != -1) && (date.GetDays() == plot_data.my_selected_item))
		//	{
		//	PenSelector ps(slate, &the_zeroline_pen);
		//	slate.MoveTo(x, 0);
		//	slate.LineTo(x, the_plot_size.cy);
		//	}
		}
	dc.BitBlt(ThePlotArea().Left(), ThePlotArea().Top(), the_plot_size.cx, the_plot_size.cy, &slate, 0, 0, SRCCOPY);
	vertGridLines = DrawVertGridLines(dc, days);
	hGridInfo = DrawHorzGridLines(dc, plot_data.myMinDayBalance, plot_data.myMaxDayBalance);
	}

void CChartCtrl::Refresh()
	{
	TRACE("CChartCtrl::Refresh()\n");
	if (m_hWnd) InvalidateRect(NULL);
	}

CSize CChartCtrl::the_plot_size;

int CChartCtrl::DeltaFromBaseline(appl::Dollar::Value::type aBalance, int iBaseLnY, int iMinBalance, int iMaxBalance) 
// Returns the number of pixels above (+) or below (-) iBaseLnY that represents
// the plot point on the chart for aBalance.
	{ 
	ASSERT(aBalance >= iMinBalance);
	ASSERT(aBalance <= iMaxBalance);
	if (aBalance < 0)
		{
		if (iMinBalance == 0) return 0;
		return MulDiv(the_plot_size.cy - iBaseLnY, aBalance, -iMinBalance);
		}
	else
		{
		if (iMaxBalance == 0) return 0;
		return MulDiv(iBaseLnY, aBalance, iMaxBalance);
		}
	}

CFont ChartAxis::theFont;

void CChartCtrl::CachePlotSize(int cx, int cy) const
	{ //TRACE("CChartCtrl::CachePlotSize(%d,%d)\n", cx, cy);
	the_plot_size.cx = cx - 2 - YAxis::Width() * 2;
	ASSERT(the_plot_size.cx > 0);
	the_plot_size.cy = cy - 7 - XAxis::Height();
	ASSERT(the_plot_size.cy > 0);
	}

void CChartCtrl::OnSize(UINT nType, int cx, int cy) 
	{ //TRACE("CChartCtrl::OnSize(type=%d,x=%d,y=%d)\n", nType, cx, cy);
	ASSERT(cx > 0);
	ASSERT(cy > 0);
	CWnd::OnSize(nType, cx, cy);
	if (!YAxis::IsInitialized()) InitializeAxis();
	if (nType != SIZE_MINIMIZED) CachePlotSize(cx,cy);
	}

const xy::Rectangle & CChartCtrl::ThePlotArea()
	{ 
	thePlotArea.mLeft = YAxis::Width();
	thePlotArea.mTop = 5;
	thePlotArea.mRight = thePlotArea.mLeft + the_plot_size.cx;
	thePlotArea.mBottom = thePlotArea.mTop + the_plot_size.cy;
	return thePlotArea; 
	}

size_t CChartCtrl::IntervalWidth(size_t months, size_t plotAreaWidth)
	{
	return plotAreaWidth / months;
	}

CRect CChartCtrl::FrameRectangle(const xy::Rectangle & plotArea)
	{
	CRect rect((const RECT &) plotArea);
	rect.InflateRect(1, 1);
	return rect;
	}

bool CChartCtrl::NeedToDrawPreMonth(int firstVertGridLineOffset) 
	{
	ASSERT(firstVertGridLineOffset >= 0);
	return (firstVertGridLineOffset != 0); 
	}

CRect XAxis::IntervalRect(const CRect & aPlotArea, std::vector<int> intervalLines, int intervalIndex, size_t defaultWidth)
	{
	ASSERT(intervalLines.size() > 0);
	ASSERT((intervalIndex >= -1) || (intervalIndex < (int) intervalLines.size()));
	ASSERT(defaultWidth > 0);
	int width = defaultWidth;
	if (intervalIndex == -1) 
		{
		return CRect(CPoint(aPlotArea.left + intervalLines[0] - defaultWidth, aPlotArea.bottom + 1), CSize(defaultWidth, XAxis::Height())); 
		}
	int left = aPlotArea.left + intervalLines[intervalIndex] + 1;
	if ((intervalLines.size() > 1) && (((size_t) intervalIndex) < (intervalLines.size() - 1)))
		{
		width = intervalLines[intervalIndex + 1] - intervalLines[intervalIndex] - 1;
		}
	return CRect(CPoint(left, aPlotArea.bottom + 1), CSize(width, XAxis::Height()));
	}

size_t CChartCtrl::CalcYGridDollarDelta(size_t aDollarRange)
	{ //TRACE("CChartCtrl::CalcYGridDollarDelta(aDollarRange=%d)\n", aDollarRange);
	return static_cast <size_t> (pow(10,(int) floor(log10((double) aDollarRange))));
	}

int CChartCtrl::CalcStartingYGridDollarValue (int aBalanceHigh, size_t aDeltaInDollars)
	{
	return (aDeltaInDollars == 0) ? 0 : (aBalanceHigh / (int) aDeltaInDollars * aDeltaInDollars);
	}

CChartCtrl::HGridInfo CChartCtrl::DrawHorzGridLines(CDC & dc, int aMinBalance, int aMaxBalance) const
	{ //TRACE("CChartCtrl::DrawHorzGridLines(dc,aMinBalance=%d,aMaxBalance=%d)\n", aMinBalance, aMaxBalance);
	PenSelector ps(dc, &theGridLinePen);
	size_t deltaInDollars = CalcYGridDollarDelta(MAX(abs(aMaxBalance), abs(aMinBalance)));
	HGridInfo results;
	int yDollars = CalcStartingYGridDollarValue(aMaxBalance, deltaInDollars);
	while (yDollars > aMinBalance) 
		{
		int y = MulDiv(ThePlotArea().Height(), aMaxBalance - yDollars, aMaxBalance - aMinBalance);
		dc.MoveTo(ThePlotArea().Left(), ThePlotArea().Top() + y);
		dc.LineTo(ThePlotArea().Right(), ThePlotArea().Top() + y);
		results.push_back(make_pair(y, yDollars));
		yDollars -= deltaInDollars;
		}
	return results;
	}

void CChartCtrl::DrawYAxis(CDC & dc, HGridInfo const & hGridInfo) const
	{
	FontSelector fontSelector(dc, ChartAxis::TheFont());
	PenSelector ps(dc, TheTickLinePen());
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	dc.SetBkMode(TRANSPARENT);
	for (size_t i = 0; i < hGridInfo.size(); ++i) 
		{
		dc.SetTextAlign(TA_BASELINE + TA_RIGHT);
		dc.TextOut(ThePlotArea().Left() - 3, hGridInfo[i].first + tm.tmAscent / 2, YAxis::Label(hGridInfo[i].second));
		dc.SetTextAlign(TA_BASELINE + TA_LEFT);
		dc.TextOut(ThePlotArea().Right() + 3, hGridInfo[i].first + tm.tmAscent / 2, YAxis::Label(hGridInfo[i].second));
		}
	}

CString YAxis::Label(int value)
	{
	CString result;
	if (abs(value) > 999999) result.Format("%dM", value / 1000000); else if (abs(value) > 999) result.Format("%dK", value / 1000); else result.Format("%d", value);
	return result;
	}

uint XAxis::Height() 
	{
	#ifdef TEST
	return 10;
	#else
	LOGFONT lf;
	theFont.GetLogFont(&lf);
	return -lf.lfHeight + 5; 
	#endif
	}

uint YAxis::theCachedWidth = NOT_INITIALIZED;

uint YAxis::Width() 
	{
	return theCachedWidth;
	}

void YAxis::Initialize(CDC & dc)
	{
	FontSelector fs(dc, TheFont());
	theCachedWidth = dc.GetTextExtent("-999M-").cx; 
	}

CString XAxis::Label(size_t year, CMonth::tag month, size_t nChars)
	{
	ASSERT(nChars > 0);
	switch (month)
		{
		case CMonth::JAN:
			{
			CString cs;
			switch (nChars)
				{
				case 1:
					break;
				case 2:
					{
					cs.Format("%02d", year % 100);
					return cs;
					}
				case 3:
					{
					cs.Format("%c%02d", *CMonth::Name(month), year % 100);
					return cs;
					}
				case 4:
					{
					cs.Format("%04d", year);
					return cs;
					}
				case 5:
					{
					cs.Format("%0.3s%02d", CMonth::Name(month), year % 100);
					return cs;
					}
				case 6:
					{
					cs.Format("%0.3s/%02d", CMonth::Name(month), year % 100);
					return cs;
					}
				case 7:
					{
					cs.Format("%0.3s%04d", CMonth::Name(month), year);
					return cs;
					}
				default:
					{
					cs.Format("%0.3s/%04d", CMonth::Name(month), year);
					return cs;
					}
				}
			}
		}
		return CString(CMonth::Name(month)).Left(MIN(MIN(nChars, 3),nChars));
	}

size_t XAxis::CharsWide(CDC & cdc, int pixelsWide)
	{
	FontSelector fs(cdc, TheFont());
	return MAX(1, pixelsWide / cdc.GetTextExtent("A").cx); 
	}

BOOL CChartCtrl::Create(CWnd * poParent, const CRect & rRect) 
	{
	return CWnd::Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_DISABLED, rRect, poParent, 1);
	}

void CChartCtrl::InitializeAxis ()
	{
	CDC * cdc = GetDC();
	YAxis::Initialize(*cdc);
	ReleaseDC(cdc);
	}

void CChartCtrl::OnPaint()
	{
	CPaintDC dc(this);
	PlotDataCache plot_data(&myCashFlowListCtrl);
	if (plot_data.myEodDataPoints.empty()) return;
	// imaginary line for which the bottom edge of the pixel represents 0
	int zero_y = MulDiv(the_plot_size.cy, plot_data.myMaxDayBalance, (plot_data.myMaxDayBalance - plot_data.myMinDayBalance + 1));
	size_t days = myCashFlowListCtrl.GetEndDate().GetDays() - myCashFlowListCtrl.GetStartDate().GetDays();
	HGridInfo hGridInfo;
	CDrawingSlate slate(&dc, CWndWrapper(*this).ClientRect().Width(), CWndWrapper(*this).ClientRect().Height());
	slate.FillSolidRect(CWndWrapper(*this).ClientRect(), GetSysColor(COLOR_BTNFACE));
	vector<int> vertGridLines;
	DrawPlotData(slate, plot_data, days, zero_y, vertGridLines, hGridInfo);
	slate.FrameRect(FrameRectangle(ThePlotArea()), dynamic_cast<CBrush*> (slate.SelectStockObject(GRAY_BRUSH)));
	DrawXAxis(slate, vertGridLines, IntervalWidth(myCashFlowListCtrl.GetDuration(), the_plot_size.cx));
	DrawYAxis(slate, hGridInfo);
	if (zero_y >= 0 || zero_y < the_plot_size.cy) DrawZeroLine(slate, zero_y);
	dc.BitBlt(0, 0, CWndWrapper(*this).ClientRect().Width(), CWndWrapper(*this).ClientRect().Height(), &slate, 0, 0, SRCCOPY);
	}

BOOL CChartCtrl::OnEraseBkgnd(CDC *pDC)
	{
	return TRUE;
	}
