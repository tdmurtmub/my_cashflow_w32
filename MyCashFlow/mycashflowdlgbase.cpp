// (C) 2004 Wesley Steiner

#include "stdafx.h"

#include "mycashflowdlgbase.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CMyCashflowDlgBase, CPropertySheet)

CMyCashflowDlgBase::CMyCashflowDlgBase(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
	{
	myAcceleratorTable = ::LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));
	ASSERT(myAcceleratorTable);
	}

CMyCashflowDlgBase::~CMyCashflowDlgBase()
	{
	} 

BEGIN_MESSAGE_MAP(CMyCashflowDlgBase, CPropertySheet)
	//{{AFX_MSG_MAP(CMyCashflowDlgBase)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CMyCashflowDlgBase::PreTranslateMessage(MSG* pMsg) 
	{
	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST) 
		{
		if (myAcceleratorTable && ::TranslateAccelerator(m_hWnd, myAcceleratorTable, pMsg)) return TRUE;
		}
	return CPropertySheet::PreTranslateMessage(pMsg);
	}
