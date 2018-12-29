// Copyright (C) 2006 by Wesley Steiner. All rights reserved.

#include "stdafx.h"

#include <my.h>
#include <win/mfc/MyWinApp.h>

using namespace my::win::app;

BEGIN_MESSAGE_MAP(CMyWinApp, CWinApp)
	//{{AFX_MSG_MAP(CMyWinApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CMyWinApp::InitInstance()
	{
	SetRegistryKey(REGISTRY_KEY);
	return FALSE;
	}

