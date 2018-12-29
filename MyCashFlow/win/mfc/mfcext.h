// Extensions to MFC {C++}
// Copyright (C) 2000-2004 by Wesley Steiner. All rights reserved.

#ifndef _MODULE_MFCEXT
#define _MODULE_MFCEXT

#define MAINFRAME ((CMainFrame*) AfxGetMainWnd())
#define APP AfxGetApp()
#define FRAMEWND ((CFrameWnd*) AfxGetMainWnd())
#define MAINVIEW (MAINFRAME->GetActiveView())

// DDX direction constants
#define FROM_CONTROLS_TO_MEMBERS TRUE
#define FROM_MEMBERS_TO_CONTROLS FALSE

#define SET_BIG_ICON TRUE
#define SET_SMALL_ICON FALSE

// Print message on entry and exit from block.
#ifdef _DEBUG
class CTraceInOut 
	{
	const TCHAR * mMsg;
	public:
	CTraceInOut(const TCHAR * msg) : mMsg(msg) { TRACE(_T(">> %s\n"), mMsg); }
	~CTraceInOut() { TRACE(_T("<< %s\n"), mMsg); }
	};
#define TRACE_IN_OUT(s) CTraceInOut traceInOut(s);
#else // _DEBUG
#define TRACE_IN_OUT(s) TRACE(s);
#endif // _DEBUG
	// 01-03-01: Added this tracing macro from www.wd-mag.com tech tips.

#endif


