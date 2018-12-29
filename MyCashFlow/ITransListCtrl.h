// (C) 2004 Wesley Steiner

#pragma once

#include "TransactionCookie.h"

interface ITransListCtrl : public IBaseListCtrl
{
	virtual int FindItemByCookie(TTransactionCookie cookie) const = 0;
	// Returns the index of the transaction that matches aTransactionCookie or -1 if not found.
	virtual void InsertAllCashFlowEntries() const = 0;
};

#ifdef TEST

namespace fake
	{
	class TransListCtrl : public ITransListCtrl
		{
		public:
		mutable bool InsertAllCashFlowEntries_wasCalled;
		void InsertAllCashFlowEntries() const { InsertAllCashFlowEntries_wasCalled = true; }
		bool EditEntryWasCalled;
		virtual void EditEntry(UINT nIndex) { EditEntryWasCalled = true; }
		virtual int FindItemByCookie(TTransactionCookie cookie) const { return -1; }
		};
	}

#endif TEST

