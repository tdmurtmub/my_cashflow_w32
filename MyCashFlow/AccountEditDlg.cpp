#include "stdafx.h"

#include "AccountEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace win;

CAccountEditDlg::DescList CAccountEditDlg::theDescriptionListCache;

CAccountEditDlg::CAccountEditDlg(Mode::type mode /* = Mode::ADD */, CWnd* pParent /*=NULL*/)
	: CSharedItemEditDialog(CAccountEditDlg::IDD, pParent),
	m_eMode(mode)
{
	//{{AFX_DATA_INIT(CAccountEditDlg)
	this->myDescription = _T("");
	this->m_nBalance = 0;
	//}}AFX_DATA_INIT
}

void CAccountEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CSharedItemEditDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CAccountEditDlg)
	DDX_CBString(pDX, IDC_DESC, myDescription);
	DDV_MaxChars(pDX, myDescription, 50);
	DDX_Text(pDX, IDC_BALANCE, m_nBalance);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		myDescription.TrimLeft();
		myDescription.TrimRight();
	}
}

BEGIN_MESSAGE_MAP(CAccountEditDlg, CSharedItemEditDialog)
	//{{AFX_MSG_MAP(CAccountEditDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAccountEditDlg::OnInitDialog() 
{
	CSharedItemEditDialog::OnInitDialog();
	
	// Update the button text if not default mode.
	if (!IsDefaultMode())
	{
		if (m_eMode == Mode::UPDATE) 
		{
			CString str;
			VERIFY(str.LoadString(IDS_ACCNTDLGMODEUPDATE));
			ASSERT(GetDlgItem(IDOK));
			GetDlgItem(IDOK)->SetWindowText(str);	
		}
	}

	// Load the drop down combo box list with saved descriptions.
	CComboBox *combo = s_cast<CComboBox*> (GetDlgItem(IDC_DESC)); ASSERT(combo);
	for (DescList::const_iterator cit = theDescriptionListCache.begin(); cit != theDescriptionListCache.end(); ++cit)
	{
		VERIFY(combo->AddString(*cit) >= 0);
	}

	if (m_eMode == Mode::UPDATE)
	{
		// set the focus to the amount field ...
		CEdit* pEdit = s_cast<CEdit*> (GetDlgItem(IDC_BALANCE));
		pEdit->SetFocus();
		pEdit->SetSel(0, -1);
		return FALSE;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAccountEditDlg::OnOK() 
{
	UpdateData(mfc::dlg::XFER_CONTROLS_TO_MEMBERS);
	theDescriptionListCache.insert(s_cast<const TCHAR *> (myDescription));
	CSharedItemEditDialog::OnOK();
}
