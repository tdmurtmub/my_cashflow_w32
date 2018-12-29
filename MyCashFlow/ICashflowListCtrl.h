// (C) 2004 Wesley Steiner

#pragma once

#include "MyMoney.h"
#include "TransactionCookie.h"

class CDate; 

interface ICashflowListCtrl 
	{
	virtual int AddEntry(const CDate &date, LPCTSTR descripton, appl::Dollar::Value::type amount, TTransactionCookie cookie = NULL_TRANSACTION_COOKIE, BOOL bUpdateBal = TRUE) = 0;
	virtual CDate GetEndDate() const = 0;
	virtual quantity GetItemCount() const = 0;
	virtual appl::Dollar::Value::type GetItemBalance(int nItemIndex) const = 0;
	virtual CDate GetItemDate(UINT nItemIndex) const = 0;
	virtual TTransactionCookie GetItemCookie(int nItemIndex) const = 0;
	virtual void RefreshBalance(UINT nStart = 0) = 0;
	virtual void RemoveItem(int nItemIndex) = 0;
	};

#ifdef TEST

namespace fake
	{
	class CashflowListCtrl : public ICashflowListCtrl
		{
		public:
		virtual void RefreshBalance(UINT nStart = 0) { }
		TTransactionCookie GetItemCookie(int aItemIndex) const { return 0; }
		CDate GetEndDate() const { return CDate(); }
		void RemoveItem(int aItemIndex) { }
		virtual quantity GetItemCount() const { return 0; }
		virtual appl::Dollar::Value::type GetItemBalance(int iIndex) const { return 0; }
		virtual CDate GetItemDate(UINT iIndex) const { return CDate(); };
		virtual int AddEntry(const CDate& date, LPCTSTR description, appl::Dollar::Value::type amount, TTransactionCookie aTransactionCookie = NULL_TRANSACTION_COOKIE, BOOL bUpdateBal = TRUE) { return 0; }
		};
	}

#endif

