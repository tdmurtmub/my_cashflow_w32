#ifndef MODULE_MYWINAPP
#define MODULE_MYWINAPP

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

class CMyWinApp : public CWinApp 
	{ 
	typedef CWinApp inherited;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyWinApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMyWinApp)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	}; 

#endif

