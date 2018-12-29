// (C) 2004 Wesley Steiner

#pragma once

#include "MyMoney.h"

struct AccountData
// Data object pointed to by the data field (CListCtrl::SetItemData) of an account list entry.
	{ 
	CString myDescription;
	appl::Dollar::Value::type myAmount;
	AccountData(const char* description, appl::Dollar::Value::type amount) : myDescription(description), myAmount(amount) { }
	};
