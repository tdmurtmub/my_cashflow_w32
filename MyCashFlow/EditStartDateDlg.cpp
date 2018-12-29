#include "stdafx.h"

#include "EditStartDateDlg.h"

#include "MyMoney.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace app;

CEditStartDateDlg::CEditStartDateDlg(const CDate & aStartDate, CWnd* pParent /*=NULL*/)
	: CDialog(CEditStartDateDlg::IDD, pParent)
	{
	//{{AFX_DATA_INIT(CEditStartDateDlg)
	m_aStartDate = aStartDate;
	//}}AFX_DATA_INIT
	}

void CEditStartDateDlg::DoDataExchange(CDataExchange* pDX)
	{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditStartDateDlg)
	DDX_DateTimeCtrl(pDX, IDC_STARTDATE, m_aStartDate);
	//}}AFX_DATA_MAP
	}

BEGIN_MESSAGE_MAP(CEditStartDateDlg, CDialog)
	//{{AFX_MSG_MAP(CEditStartDateDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CEditStartDateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	InitDateTimePickerControl(GetDlgItem(IDC_STARTDATE)->GetSafeHwnd());
	return TRUE;  // return TRUE unless you set the focus to a control
}
