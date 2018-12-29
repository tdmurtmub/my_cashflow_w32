// An MFC-based Date Class
// Copyright (C) 1995 Wesley Steiner. All rights reserved.

#pragma once

#include <utility.h>
#include <hms.h>

class CYear
	{
	public:
	CYear(const size_t aYear) : myYear(aYear) { }
	BOOL IsLeapYear() const
	{
		if (myYear % 4 == 0)
			{
			if (myYear % 100 == 0)
				// A century must be evenly divisible by 400.
				return (myYear % 400 == 0);
			else
				return TRUE;
			}
		return FALSE;
	}

	private:
	size_t myYear;
	};

class CMonth 
	{
	public:
	enum tag {JAN = 1, FEB, MAR, APR, MAY, JUN, JUL, AUG, SEP, OCT, NOV, DEC};
	static const TCHAR * Name(tag month);
	private:
	static bool Initialized;
	};

class CDate
	{
	private:

	// time of the day used for comparison functions
	SCM(int, ZERO_HOUR, 10);
	SCM(int, ZERO_MINUTE, 0);
	SCM(int, ZERO_SECOND, 0);

	public:

	static CDate Today() { return CDate(); }

	CDate() // Defaults to today's date.
		{
		CTime today = CTime::GetCurrentTime();
		m_aTime = CTime(today.GetYear(), today.GetMonth(), today.GetDay(), ZERO_HOUR, ZERO_MINUTE, ZERO_SECOND);
		ASSERT(m_aTime.GetHour() == ZERO_HOUR);
		ASSERT(m_aTime.GetMinute() == ZERO_MINUTE);
		ASSERT(m_aTime.GetSecond() == ZERO_SECOND);
		}

	CDate(size_t year, size_t month = 1, size_t day = 1) 
		{ 
		m_aTime = CTime(year, month, day, ZERO_HOUR, ZERO_MINUTE, ZERO_SECOND);
		}

	size_t GetYear() const { return m_aTime.GetYear(); }
	CMonth::tag GetMonth() const { return (CMonth::tag) m_aTime.GetMonth(); }
	size_t GetDay() const { return m_aTime.GetDay(); }

	void SetFirstDayOfMonth();

	size_t GetDays() const { return static_cast <size_t> (m_aTime.GetTime() / HMS::SECONDS_PER_DAY); }
	// Returns # of days since 1/1/1970.

	CString ToString() const
		{
		ASSERT(m_aTime.GetHour() == ZERO_HOUR);
		return m_aTime.Format(_T("%x"));
		}

	BOOL operator<=(const CDate& aDate) const
		{
		ASSERT(m_aTime.GetHour() == ZERO_HOUR);
		ASSERT(m_aTime.GetMinute() == ZERO_MINUTE);
		ASSERT(m_aTime.GetSecond() == ZERO_SECOND);
		return m_aTime <= aDate.m_aTime;
		}

	BOOL operator<(const CDate& aDate) const
		{
		ASSERT(m_aTime.GetHour() == ZERO_HOUR);
		ASSERT(m_aTime.GetMinute() == ZERO_MINUTE);
		ASSERT(m_aTime.GetSecond() == ZERO_SECOND);
		return m_aTime < aDate.m_aTime;
		}

	BOOL operator==(const CDate& aDate) const
		{
		ASSERT(m_aTime.GetHour() == ZERO_HOUR);
		ASSERT(m_aTime.GetMinute() == ZERO_MINUTE);
		ASSERT(m_aTime.GetSecond() == ZERO_SECOND);
		ASSERT(((CTime&) aDate).GetHour() == ZERO_HOUR);
		ASSERT(((CTime&) aDate).GetMinute() == ZERO_MINUTE);
		ASSERT(((CTime&) aDate).GetSecond() == ZERO_SECOND);
		return m_aTime == aDate.m_aTime;
		}

	BOOL operator!=(const CDate& aDate) const
		{
		ASSERT(m_aTime.GetHour() == ZERO_HOUR);
		ASSERT(m_aTime.GetMinute() == ZERO_MINUTE);
		ASSERT(m_aTime.GetSecond() == ZERO_SECOND);
		return m_aTime != aDate.m_aTime;
		}

	BOOL operator>=(const CDate& aDate) const
		{
		ASSERT(m_aTime.GetHour() == ZERO_HOUR);
		ASSERT(m_aTime.GetMinute() == ZERO_MINUTE);
		ASSERT(m_aTime.GetSecond() == ZERO_SECOND);
		return m_aTime >= aDate.m_aTime;
		}

	BOOL operator>(const CDate& aDate) const
		{
		ASSERT(m_aTime.GetHour() == ZERO_HOUR);
		ASSERT(m_aTime.GetMinute() == ZERO_MINUTE);
		ASSERT(m_aTime.GetSecond() == ZERO_SECOND);
		return m_aTime > aDate.m_aTime;
		}

	CDate& operator++() // next day
		{
		ASSERT(m_aTime.GetHour() == ZERO_HOUR);
		ASSERT(m_aTime.GetMinute() == ZERO_MINUTE);
		ASSERT(m_aTime.GetSecond() == ZERO_SECOND);
		CTime aTime(m_aTime);
		aTime += CTimeSpan(1, 0, 0, 0); // 1 day
		m_aTime = CTime(aTime.GetYear(), aTime.GetMonth(), aTime.GetDay(), ZERO_HOUR, ZERO_MINUTE, ZERO_SECOND);
		return *this;
		}

	CDate& operator+=(size_t n) // next day
		{
		ASSERT(m_aTime.GetHour() == ZERO_HOUR);
		ASSERT(m_aTime.GetMinute() == ZERO_MINUTE);
		ASSERT(m_aTime.GetSecond() == ZERO_SECOND);
		CTime aTime(m_aTime);
		aTime += CTimeSpan(n, 0, 0, 0); // 1 day
		m_aTime = CTime(aTime.GetYear(), aTime.GetMonth(), aTime.GetDay(), ZERO_HOUR, ZERO_MINUTE, ZERO_SECOND);

		return *this;
		}

	CDate operator+(size_t n)
		{
		ASSERT(m_aTime.GetHour() == ZERO_HOUR);
		ASSERT(m_aTime.GetMinute() == ZERO_MINUTE);
		ASSERT(m_aTime.GetSecond() == ZERO_SECOND);
		return CDate(m_aTime + CTimeSpan(n, 0, 0, 0));
		}

	CDate operator-(size_t n)
		{
		ASSERT(m_aTime.GetHour() == ZERO_HOUR);
		ASSERT(m_aTime.GetMinute() == ZERO_MINUTE);
		ASSERT(m_aTime.GetSecond() == ZERO_SECOND);
		return CDate(m_aTime - CTimeSpan(n, 0, 0, 0));
		}

	CDate& operator-=(size_t n)
		{
		ASSERT(m_aTime.GetHour() == ZERO_HOUR);
		ASSERT(m_aTime.GetMinute() == ZERO_MINUTE);
		ASSERT(m_aTime.GetSecond() == ZERO_SECOND);

		CTime aTime(m_aTime);
		aTime -= CTimeSpan(n, 0, 0, 0); // 1 day
		m_aTime = CTime(aTime.GetYear(), aTime.GetMonth(), aTime.GetDay(), ZERO_HOUR, ZERO_MINUTE, ZERO_SECOND);

		ASSERT(m_aTime.GetHour() == ZERO_HOUR);
		ASSERT(m_aTime.GetMinute() == ZERO_MINUTE);
		ASSERT(m_aTime.GetSecond() == ZERO_SECOND);

		return *this;
		}

	CDate& operator--() // previous day
		{
		ASSERT(m_aTime.GetHour() == ZERO_HOUR);
		ASSERT(m_aTime.GetMinute() == ZERO_MINUTE);
		ASSERT(m_aTime.GetSecond() == ZERO_SECOND);

		CTime aTime(m_aTime);
		aTime -= CTimeSpan(1, 0, 0, 0); // 1 day
		m_aTime = CTime(aTime.GetYear(), aTime.GetMonth(), aTime.GetDay(), ZERO_HOUR, ZERO_MINUTE, ZERO_SECOND);

		ASSERT(m_aTime.GetHour() == ZERO_HOUR);
		ASSERT(m_aTime.GetMinute() == ZERO_MINUTE);
		ASSERT(m_aTime.GetSecond() == ZERO_SECOND);

		return *this;
		}

	operator CTime&()
		{
		ASSERT(m_aTime.GetHour() == ZERO_HOUR);
		ASSERT(m_aTime.GetMinute() == ZERO_MINUTE);
		ASSERT(m_aTime.GetSecond() == ZERO_SECOND);
		return m_aTime;
		}
	// 01-17-04: Added this built-in conversion operator for implicit conversion in 
	// MFC DDX controls and dialogs.

	private:
	CTime m_aTime; // Is-implemented-in-terms-of a CTime.

	CDate(CTime aTime)
		{ 
		m_aTime = CTime(aTime.GetYear(), aTime.GetMonth(), aTime.GetDay(), ZERO_HOUR, ZERO_MINUTE, ZERO_SECOND);
		ASSERT(m_aTime.GetHour() == ZERO_HOUR);
		ASSERT(m_aTime.GetMinute() == ZERO_MINUTE);
		ASSERT(m_aTime.GetSecond() == ZERO_SECOND);
		}
	};

class CMonthIterator : public CMonth
{
public:
	CMonthIterator(const size_t a_year, const size_t a_month)
		{
		Set(a_year, a_month);
		}

	CMonthIterator(const CDate & date)
		{
		Set(date.GetYear(), date.GetMonth());
		}

	void Set(const size_t a_year, const size_t a_month)
		{
		myYear = a_year;
		m_nMonth = a_month;
		}

	int GetYear() const { return myYear; }
	CMonth::tag GetMonth() const { return (CMonth::tag) m_nMonth; }
	const TCHAR * Name() const { return CMonth::Name((CMonth::tag) m_nMonth); }

	size_t Days() const 
	// Returns the number of days in the month.
		{ 
		switch (m_nMonth)
			{
			case JAN:
			case MAR:
			case MAY:
			case JUL:
			case AUG:
			case OCT:
			case DEC:
				return 31;
			case APR:
			case JUN:
			case SEP:
			case NOV:
				return 30;
			case FEB:
				return CYear(myYear).IsLeapYear() ? 29 : 28;
			default:
				ASSERT(0);
				return 0;
			}
		}

	void operator--() // previous month
	{
		--m_nMonth;
		if (m_nMonth == 0)
		{
			--myYear;
			m_nMonth = 12;
		}
	}

	void operator++() // next month
		{
		++m_nMonth;
		if (m_nMonth == 13)
			{
			++myYear;
			m_nMonth = 1;
			}
		}

	CMonthIterator& operator+=(int n)
		{
		for (int i = 0; i < abs(n); ++i) 
			{
			if (n < 0) -- (*this); else ++(*this);
			}
		return *this;
		}

	private:
	size_t myYear;
	size_t m_nMonth;
	};

class CDateRepeater
	// Base class for deriving specialized date repeater classes.
	{
	private:
	size_t m_nRepCnt;

	protected:
	size_t m_nRepDelta;

	public:
	enum { INFINITE_REPEAT = 0 };

	struct Interval { enum type {NONE, DAY, WEEK, MONTH, YEAR};	};

	CDateRepeater(size_t nRepCnt = 1, size_t nRepDelta = 1) : 
		m_nRepCnt(nRepCnt), 
		m_nRepDelta(nRepDelta) 
	{ }

	virtual ~CDateRepeater() { }

	size_t Count() const { return m_nRepCnt; }

	virtual CDate Next(const CDate & aStartDate, size_t ith) const = 0;
	// Returns the ith (zero based) date in the sequence.

	virtual Interval::type IntervalType() const = 0;

	virtual size_t IntervalDelta() const { return m_nRepDelta; }
	// Returns the number of interval units between occurences.
	};	

class CDateRepeaterOnce : public CDateRepeater
	{
	CDate Next(const CDate & aStartDate, size_t ith) const { return aStartDate; }
	virtual Interval::type IntervalType() const { return Interval::NONE; }
	};

class CDateRepeaterDaily : public CDateRepeater
	{
	public:
	CDateRepeaterDaily(size_t nRepDelta = 1, size_t nRepCnt = INFINITE_REPEAT) : 
		CDateRepeater(nRepCnt, nRepDelta)
		{ 
		}

	CDate Next(const CDate & aStartDate, size_t ith) const 
		{ 
		return CDate(aStartDate) += m_nRepDelta * ith; 
		}

	virtual Interval::type IntervalType() const { return Interval::DAY; }
	};

class CDateRepeaterWeekly: public CDateRepeaterDaily
	{
	public:
	CDateRepeaterWeekly(size_t nRepDelta = 1, size_t nRepCnt = INFINITE_REPEAT) : 
		CDateRepeaterDaily(nRepDelta * 7, nRepCnt) { }
	virtual size_t IntervalDelta() const { return CDateRepeaterDaily::IntervalDelta() / 7; }
	virtual Interval::type IntervalType() const { return Interval::WEEK; }
	};

class CDateRepeaterMonthly : public CDateRepeater
	{
	public:
	struct SpecialDay {
		SCM(size_t, LAST_DAY, 0);
	};

	CDateRepeaterMonthly(size_t nDayNo = 1, size_t nRepDelta = 1, size_t nRepCnt = INFINITE_REPEAT) : 
		CDateRepeater(nRepCnt, nRepDelta), m_nDayNo((nDayNo > 28) ? SpecialDay::LAST_DAY : nDayNo)
		{ 
		}

	CDate Next(const CDate & aStartDate, size_t ith) const 
		{ 
		CMonthIterator oMonth(aStartDate.GetYear(), aStartDate.GetMonth());
		for (size_t i = 0; i < ith; ++i) oMonth += m_nRepDelta;
		size_t nDayNo;
		if (m_nDayNo == SpecialDay::LAST_DAY)
			nDayNo = oMonth.Days();
		else
			nDayNo = m_nDayNo; 
		return CDate(oMonth.GetYear(), oMonth.GetMonth(), nDayNo);
		}

	virtual Interval::type IntervalType() const { return Interval::MONTH; }

	int DayOfMonth() const { return m_nDayNo; }

	private:
	int m_nDayNo;
	};

class CDateRepeaterYearly : public CDateRepeater
	{
	public:
	CDateRepeaterYearly(size_t nRepDelta = 1, size_t nRepCnt = INFINITE_REPEAT) : 
		CDateRepeater(nRepCnt, nRepDelta)
		{ 
		}

	CDate Next(const CDate & aStartDate, size_t ith) const 
		{ 
		return CDate(aStartDate.GetYear() + ith * m_nRepDelta, aStartDate.GetMonth(), aStartDate.GetDay());
		}
	virtual Interval::type IntervalType() const { return Interval::YEAR; }
	};

