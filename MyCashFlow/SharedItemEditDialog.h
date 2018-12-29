#pragma once

class CSharedItemEditDialog : public CDialog
	{
	DECLARE_DYNAMIC(CSharedItemEditDialog)

	public:
	CSharedItemEditDialog(UINT resourceId, CWnd* parent = NULL);
	virtual ~CSharedItemEditDialog();

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	};
