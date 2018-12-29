#include "stdafx.h"
#include "MyMoney.h"
#include "XEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace app;

CTransEditDlg::CTransEditDlg(Mode::type mode /* = Mode::ADD */,
	CWnd* pParent /*=NULL*/)
	: CSharedItemEditDialog(CTransEditDlg::IDD, pParent),
	m_eMode(mode)
{
	//{{AFX_DATA_INIT(CTransEditDlg)
	m_nAmt = 0;
	m_aStartDate = CDate::Today();
	m_iDebitGrp = 0;
	m_iRepGrp = Repeat::Index::ONCE;
	my_description = _T("");
	m_iRep1Interval = 0;
	m_iRep1Freq = 0;
	m_iRepEndGrp = Repeat::Ends::Index::NEVER;
	m_nOccur = 2;
	//}}AFX_DATA_INIT
}

void CTransEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CSharedItemEditDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransEditDlg)
	DDX_Control(pDX, IDC_ENDOCCURSPIN, m_aOccurSpin);
	DDX_CBString(pDX, IDC_DESC, my_description);
	DDX_Control(pDX, IDC_REP1FREQ, m_aRep1Freq);
	DDX_Text(pDX, IDC_AMT, m_nAmt);
	DDX_DateTimeCtrl(pDX, IDC_STARTDATE, m_aStartDate);
	DDX_Radio(pDX, IDC_DEBIT, m_iDebitGrp);
	DDX_Radio(pDX, IDC_REPGRPONCE, m_iRepGrp);
	DDX_CBIndex(pDX, IDC_REP1INTERVAL, m_iRep1Interval);
	DDX_Control(pDX, IDC_REP1INTERVAL, m_aRep1Interval);
	DDX_CBIndex(pDX, IDC_REP1FREQ, m_iRep1Freq);
	DDX_Radio(pDX, IDC_ENDGRPNEVER, m_iRepEndGrp);
	DDX_Text(pDX, IDC_OCCURRENCES, m_nOccur);
	DDV_MinMaxUInt(pDX, m_nOccur, 2, 9999);
	DDX_Control(pDX, IDC_OCCURRENCES, m_aOccur);
	DDX_Control(pDX, IDC_REPGRPONCE, m_aRepGrpOnce);
	DDX_Control(pDX, IDC_REPGRPEVERY, m_aRepGrpEvery);
	DDX_Control(pDX, IDC_ENDGRPNEVER, m_aEndGrpNever);
	DDX_Control(pDX, IDC_ENDGRPAFTER, m_aEndGrpAfter);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		my_description.TrimLeft();
		my_description.TrimRight();
	}
}


BEGIN_MESSAGE_MAP(CTransEditDlg, CSharedItemEditDialog)
	//{{AFX_MSG_MAP(CTransEditDlg)
	ON_BN_CLICKED(IDC_REPGRPONCE, OnRepeatsOnce)
	ON_BN_CLICKED(IDC_REPGRPEVERY, OnRepeatsEvery)
	ON_BN_CLICKED(IDC_ENDGRPNEVER, OnEndsNever)
	ON_BN_CLICKED(IDC_ENDGRPAFTER, OnEndsAfter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CTransEditDlg::OnInitDialog() 
{
	CSharedItemEditDialog::OnInitDialog();
	
	// Update the button text if not default mode ...
	if (!IsDefaultMode())
	{
		if (m_eMode == Mode::UPDATE) 
		{
			CString str;
			VERIFY(str.LoadString(IDS_TRANXDLGMODEUPDATE));
			ASSERT(GetDlgItem(IDOK));
			GetDlgItem(IDOK)->SetWindowText(str);	
		}
	}

	// Populate the description list control ...
//	CComboBox* pCB = (CComboBox*) GetDlgItem(IDC_DESC); ASSERT(pCB);

	InitDateTimePickerControl(GetDlgItem(IDC_STARTDATE)->GetSafeHwnd());
	DoRepGrpSel(m_iRepGrp); 
	DoEndGrpSel(m_iRepEndGrp); 
	m_aOccurSpin.SetRange(Repeat::Ends::After::Spin::MIN_VAL, Repeat::Ends::After::Spin::MAX_VAL);
	if (m_eMode == Mode::UPDATE)
	{
		// set the focus to the amount field ...
		CEdit* pEdit = s_cast<CEdit*> (GetDlgItem(IDC_AMT));
		pEdit->SetFocus();
		pEdit->SetSel(0, -1);
		return FALSE;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTransEditDlg::OnEndsNever() 
// User clicked the never-ends radio button. 
{
	DoEndGrpSel(Repeat::Ends::Index::NEVER);
}

void CTransEditDlg::OnEndsAfter() 
// User clicked the ends-after radio button. 
{
	DoEndGrpSel(Repeat::Ends::Index::AFTER);
}

void CTransEditDlg::OnRepeatsOnce()
{
	DoRepGrpSel(Repeat::Index::ONCE);
}

void CTransEditDlg::OnRepeatsEvery()
{
	DoRepGrpSel(Repeat::Index::REPEAT);
}

void CTransEditDlg::DoRepGrpSel(int iSel)
// Resets controls based on a repeat group radio button selection.
{
	BOOL bEnable;
	switch (iSel) 
	{
		case Repeat::Index::ONCE:
			bEnable = FALSE;
			break;
		case Repeat::Index::REPEAT:
			bEnable = TRUE;
			break;
		default:
			ASSERT(FALSE);
	}
	m_aRep1Freq.EnableWindow(bEnable);
	m_aRep1Interval.EnableWindow(bEnable);
	m_aEndGrpNever.EnableWindow(bEnable);
	m_aEndGrpAfter.EnableWindow(bEnable);
	if (bEnable) {
		CDataExchange DX(this, win::mfc::dlg::XFER_CONTROLS_TO_MEMBERS);
		DDX_Radio(&DX, IDC_ENDGRPNEVER, m_iRepEndGrp);
		DoEndGrpSel(m_iRepEndGrp);
	}
	else {
		m_aOccur.EnableWindow(bEnable);
		m_aOccurSpin.EnableWindow(bEnable);
	}
}

void CTransEditDlg::DoEndGrpSel(int iSel) 
// Resets controls based on a repeat ends group radio button selection.
{
	BOOL bEnable;
	switch (iSel) 
	{
		case Repeat::Ends::Index::NEVER:
			bEnable = FALSE;
			break;
		case Repeat::Ends::Index::AFTER:
			bEnable = TRUE;
			break;
		default:
			ASSERT(FALSE);
	}
	m_aOccur.EnableWindow(bEnable);
	m_aOccurSpin.EnableWindow(bEnable);
}

CDateRepeater* CTransEditDlg::CreateDateRepeater()
// Returns a pointer to a new instance of a CDateRepeater object based on the current 
// contents of the dialog's data members.
// Caller is responsible for deleting the resource.
{
	CDateRepeater* repeater;

	if (m_iRepGrp == Repeat::Index::ONCE) repeater = new CDateRepeaterOnce();
	else
	{
		size_t nDelta;
		switch (m_iRep1Freq)
		{
			case Repeat::Interval::Freq::Index::EVERY:
				nDelta = 1;
				break;
			case Repeat::Interval::Freq::Index::EVERY_OTHER:
				nDelta = 2;
				break;
			case Repeat::Interval::Freq::Index::EVERY_THIRD:
				nDelta = 3;
				break;
			case Repeat::Interval::Freq::Index::EVERY_FOURTH:
				nDelta = 4;
				break;
			default:
				ASSERT(0); // sanity check
		}

		size_t nCount;
		switch (m_iRepEndGrp)
		{
			case Repeat::Ends::Index::NEVER:
				nCount = CDateRepeater::INFINITE_REPEAT;
				break;
			case Repeat::Ends::Index::AFTER:
				nCount = m_nOccur;
				break;
			default:
				ASSERT(0); // sanity check
		}

		switch (m_iRep1Interval) 
		{
			case Repeat::Interval::Unit::Index::DAY:
				repeater = new CDateRepeaterDaily(nDelta, nCount);
				break;
			case Repeat::Interval::Unit::Index::WEEK:
				repeater = new CDateRepeaterWeekly(nDelta, nCount);
				break;
			case Repeat::Interval::Unit::Index::MONTH:
				repeater = new CDateRepeaterMonthly(m_aStartDate.GetDay(), nDelta, nCount);
				break;
			case Repeat::Interval::Unit::Index::YEAR:
				repeater = new CDateRepeaterYearly(nDelta, nCount);
				break;
			default:
				ASSERT(0); // sanity check
		}
	}

	return repeater;
}

void CTransEditDlg::InitializeControls(const CDate &start_date, LPCTSTR description, 
	appl::Dollar::Value::type amount,
	CDateRepeater* repeater /* = new CDateRepeaterOnce() */ )
{
	ASSERT(description);
	ASSERT(repeater);

	m_aStartDate = start_date;
	my_description = description;
	m_nAmt = abs(amount);
	m_iDebitGrp = (amount < 0) ? 0 : 1 ;

	m_iRepGrp = (repeater->Count() == 1) ? CTransEditDlg::Repeat::Index::ONCE : CTransEditDlg::Repeat::Index::REPEAT ;

	switch (repeater->IntervalDelta())
	{	
		case 0: // ignore non repeaters
		case 1:
			m_iRep1Freq = CTransEditDlg::Repeat::Interval::Freq::Index::EVERY;
			break;
		case 2:
			m_iRep1Freq = CTransEditDlg::Repeat::Interval::Freq::Index::EVERY_OTHER;
			break;
		case 3:
			m_iRep1Freq = CTransEditDlg::Repeat::Interval::Freq::Index::EVERY_THIRD;
			break;
		case 4:
			m_iRep1Freq = CTransEditDlg::Repeat::Interval::Freq::Index::EVERY_FOURTH;
			break;
		default:
			ASSERT(FALSE);
	};

	switch (repeater->IntervalType())
	{
		case CDateRepeater::Interval::NONE:
			break;
		case CDateRepeater::Interval::DAY:
			m_iRep1Interval = CTransEditDlg::Repeat::Interval::Unit::Index::DAY;
			break;
		case CDateRepeater::Interval::WEEK:
			m_iRep1Interval = CTransEditDlg::Repeat::Interval::Unit::Index::WEEK;
			break;
		case CDateRepeater::Interval::MONTH:
			m_iRep1Interval = CTransEditDlg::Repeat::Interval::Unit::Index::MONTH;
			break;
		case CDateRepeater::Interval::YEAR:
			m_iRep1Interval = CTransEditDlg::Repeat::Interval::Unit::Index::YEAR;
			break;
		default:
			ASSERT(FALSE);
	}

	if ((repeater->IntervalType() == CDateRepeater::Interval::NONE) || (repeater->Count() == CDateRepeater::INFINITE_REPEAT)) 
		{
		m_iRepEndGrp = CTransEditDlg::Repeat::Ends::Index::NEVER;
		}
	else 
		{
		m_iRepEndGrp = CTransEditDlg::Repeat::Ends::Index::AFTER;
		m_nOccur = repeater->Count();
		}
}
