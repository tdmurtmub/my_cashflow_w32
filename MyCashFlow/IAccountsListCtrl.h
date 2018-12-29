// (C) 2006 Wesley Steiner

#pragma once

#include "MyMoney.h"

interface IAccountsListCtrl
	{
	virtual appl::Dollar::Value::type GetBalance() const = 0;
	};

#ifdef TEST

namespace fake
	{
	class AccountsListCtrl : public IAccountsListCtrl
		{
		virtual appl::Dollar::Value::type GetBalance() const { return 0; }
		};
	}

#endif TEST

