// (C) 2004 Wesley Steiner

#pragma once

#include <string>

#include <win\mfc\cdate.h>

#include "MyMoney.h"

class CDateRepeater;

struct TransactionData : _WSM_DECLARE_NONCOPYABLE
	// Data object pointed to by the data field (CListCtrl::SetItemData) of a transaction list entry.
	{ 
	CDate myStartDate;
	CString myDescription;
	appl::Dollar::Value::type m_aAmount;
	CDateRepeater * m_pRepeater;
	TransactionData(const CDate & startDate, const TCHAR * description, appl::Dollar::Value::type amount, CDateRepeater * repeater) : 
		myStartDate(startDate),
		myDescription(description),
		m_aAmount(amount),
		m_pRepeater(repeater) { }
	~TransactionData();
	};
