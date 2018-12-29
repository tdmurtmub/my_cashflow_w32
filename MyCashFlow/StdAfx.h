// (C) 2004 Wesley Steiner

#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER // Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0400 // Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#if defined(_WIN32_WCE) && (_WIN32_WCE >= 211) && (_AFXDLL)
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <string>

#include <boost\smart_ptr.hpp>

#include <bldenv.h>
#include <lib.h>
#include <unittest.h>
#include <Utility.h>
#include <win\api\Exception.h>
#include <win\mfc.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
