#include "stdafx.h"

#include "SharedItemEditDialog.h"

IMPLEMENT_DYNAMIC(CSharedItemEditDialog, CDialog)
CSharedItemEditDialog::CSharedItemEditDialog(UINT resourceId, CWnd* parent /*=NULL*/) : CDialog(resourceId, parent)
{
}

CSharedItemEditDialog::~CSharedItemEditDialog()
{
}

void CSharedItemEditDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSharedItemEditDialog, CDialog)
END_MESSAGE_MAP()
