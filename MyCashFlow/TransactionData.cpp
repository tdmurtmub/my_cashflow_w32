// Copyright (C) 2004 Wesley Steiner

#include "stdafx.h"

#include "TransactionData.h"

#include <Win\MFC\CDate.h>

TransactionData::~TransactionData() 
{ 
	delete m_pRepeater; 
}
