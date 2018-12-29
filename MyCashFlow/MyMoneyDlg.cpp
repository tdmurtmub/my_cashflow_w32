// Copyright (C) 2004 by Wesley Steiner. All rights reserved.

#include "stdafx.h"

#include "MyMoneyDlg.h"

#include <std.h>
#include <NumExt.h>
#include "api\Exception.h"
#include "mfc\Markup.h"
#include "mfc\wndinfo.h"

#include "AccountEditDlg.h"
#include "AccountsCtrl.h"
#include "MyMoney.h"
#include "mfc\ConfigDataManager.h"
#include "XEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;
using namespace win::mfc;
using namespace app;

const bool CMyCashflowDlg::WITH_CANCEL = true;
 
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_aVersion = _T("Version ") + CMyCashflowApp::GetVersionStr();
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_STATIC_VERSION, m_aVersion);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg) 
	ON_BN_CLICKED(IDC_HOMEPAGE, OnHomePage)
	ON_BN_CLICKED(IDC_DONATE, OnBnClickedDonate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CMyCashflowDlg, CMyCashflowDlgBase)

CMyCashflowDlg::CMyCashflowDlg(IConfigDataManager & rConfigDataManager, size_t duration, const CRect & rect /* = CRect(0, 0, 0, 0) */) : 
	CMyCashflowDlgBase(IDS_APPTITLE),
	initial_frame(rect),
	accounts_dialog(rConfigDataManager, &cashflow_dialog.myList),
	transactions_dialog(rConfigDataManager, &cashflow_dialog.myList),
	cashflow_dialog(rConfigDataManager, &accounts_dialog.m_aList, &transactions_dialog.m_aList, duration),
	chart(cashflow_dialog.myList),
	m_rConfigDataManager(rConfigDataManager)
	{
	//{{AFX_DATA_INIT(CMyCashflowDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	AddPage(&this->cashflow_dialog); ASSERT(tab::index::CASHFLOW == 0);
	AddPage(&this->accounts_dialog); ASSERT(tab::index::ACCOUNTS == 1);
	AddPage(&this->transactions_dialog); ASSERT(tab::index::TRANSACTIONS == 2);
	}

void CMyCashflowDlg::DoDataExchange(CDataExchange* pDX)
{
	inherited::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyCashflowDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMyCashflowDlg, CMyCashflowDlgBase)
	//{{AFX_MSG_MAP(CMyCashflowDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DRAWITEM()
	ON_COMMAND(ID_EDIT_ADDTRANX, OnEditAddTranx)
	ON_COMMAND(ID_EDIT_ADDACCOUNT, OnEditAddAccount)
	ON_COMMAND(ID_EDIT_SELECTEDITEM, OnEditSelItem)
	ON_COMMAND(ID_EDIT_DELSELENTRY, OnEditDelSelEntry)
	ON_WM_INITMENUPOPUP()
	ON_WM_DESTROY()
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_WM_SIZE()
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
	ON_WM_GETMINMAXINFO()
	ON_COMMAND(ID_FILE_AUTOSAVEONEXIT, OnFileAutosaveonexit)
END_MESSAGE_MAP()

BOOL CMyCashflowDlg::OnInitDialog()
{
	inherited::OnInitDialog();
	HideButton(IDOK);
	HideButton(IDCANCEL);
	HideButton(IDHELP);
	HideButton(ID_APPLY_NOW);
	main_menu.DestroyMenu();
	WAXONZ(main_menu.LoadMenu(IDR_MAINFRAME));
	VERIFY(SetMenu(NULL));
	VERIFY(SetMenu(&main_menu));
	if (!this->initial_frame.IsRectEmpty()) MoveWindow(initial_frame);
	
	bool state = (bool) ConfigDataManager::Instance().GetKeyValue(cfg::cat::ROOT, Settings::Root::Key::AUTOSAVE, true);
	CMenu * sub_menu = main_menu.GetSubMenu(0);
	sub_menu->CheckMenuItem(ID_FILE_AUTOSAVEONEXIT,  MF_BYCOMMAND | (state ? MF_CHECKED : MF_UNCHECKED));

	// Force OnInitDialog to be called for all pages first.
	for (int iPage = 0; iPage < GetPageCount(); ++iPage) SetActivePage(iPage);
	// then set the default page.
	SetActivePage(0);

	// AddEntry "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	this->chart.Create(this, GetChartRect());
	Load();
	#ifndef _DEBUG
	PostMessage(WM_COMMAND, ID_APP_ABOUT);
	#endif
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMyCashflowDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if (nID == SC_CLOSE)
		{
		if (ConditionalSave()) inherited::OnSysCommand(nID, lParam);
		}
	else
		{
		inherited::OnSysCommand(nID, lParam);
		}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMyCashflowDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		inherited::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMyCashflowDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMyCashflowDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: AddEntry your message handler code here and/or call default
	
	inherited::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CMyCashflowDlg::OnEditAddTranx() 
	{
	CTransEditDlg dialog;
	if (dialog.DoModal() == IDOK) 
		{
		CDateRepeater* repeater = dialog.CreateDateRepeater(); ASSERT(repeater);
		VERIFY(this->transactions_dialog.m_aList.AddEntry(dialog.m_aStartDate, dialog.my_description,
			(dialog.m_iDebitGrp == 0) ? -(int) dialog.m_nAmt : dialog.m_nAmt, repeater) 
			!= -1);
		}
	}

void CMyCashflowDlg::OnEditAddAccount() 
	{
	CAccountEditDlg dialog;
	if (dialog.DoModal() == IDOK) VERIFY(this->accounts_dialog.m_aList.AddEntry(dialog.myDescription, dialog.m_nBalance, true) != -1);
	}

void CMyCashflowDlg::OnEditSelItem() 
	{
	CListCtrl* pList = NULL;
	switch (GetActiveIndex())
		{
		case CMyCashflowDlg::tab::index::ACCOUNTS:
			{
			int iSelected = this->accounts_dialog.m_aList.GetSelectedItemIndex(); ASSERT(iSelected != -1);
			this->accounts_dialog.m_aList.EditEntry(iSelected);
			break;
			}
		case CMyCashflowDlg::tab::index::TRANSACTIONS:
			{
			int iSelected = this->transactions_dialog.m_aList.GetSelectedItemIndex(); ASSERT(iSelected != -1);
			this->transactions_dialog.m_aList.EditEntry(iSelected);
			break;
			}
		case CMyCashflowDlg::tab::index::CASHFLOW:
			{
			int iSelected = this->cashflow_dialog.myList.GetSelectedItemIndex(); ASSERT(iSelected != -1);
			this->cashflow_dialog.myList.EditEntry((UINT) iSelected);
			break;
			}
		}
	}

void CMyCashflowDlg::OnEditDelSelEntry() 
{
	switch (OverridableGetActiveIndex())
	{
		case CMyCashflowDlg::tab::index::ACCOUNTS:
		{
			AccountsPageDlg().OnEditDelSelEntry();
			break;
		}
		case CMyCashflowDlg::tab::index::TRANSACTIONS:
		{
			TransactionsPageDlg().OnEditDelSelEntry();
			break;
		}
	}
}

bool CMyCashflowDlg::CanEditSelectedItem() const
	{
	bool bEnable = false;
	switch (GetActiveIndex())
		{
		case CMyCashflowDlg::tab::index::ACCOUNTS:
			{
			CAccountsListCtrl& rList = (r_cast<CAccountsPageDialog*> (GetActivePage()))->m_aList;
			ASSERT(rList);
			bEnable = rList.GetSelectedCount() > 0;
			break;
			}
		case CMyCashflowDlg::tab::index::TRANSACTIONS:
			{
			CTransListCtrl& rList = (r_cast<CTransactionsPageDialog*> (GetActivePage()))->m_aList; ASSERT(rList);
			bEnable = rList.GetSelectedCount() > 0;
			break;
			}
		case CMyCashflowDlg::tab::index::CASHFLOW:
			{
			CCashFlowListCtrl& rList = (r_cast<CCashflowPageDialog*> (GetActivePage()))->myList; ASSERT(rList);
			bEnable = (rList.GetSelectedCount() > 0) && rList.IsTransaction(rList.GetSelectedItemIndex());
			break;
			}
		}
	return bEnable;
	}

bool CMyCashflowDlg::CanDeleteSelectedItem() const
	{
	bool bEnable = false;
	switch (GetActiveIndex())
		{
		case CMyCashflowDlg::tab::index::ACCOUNTS:
			{
			CAccountsListCtrl & rList = (r_cast<CAccountsPageDialog*> (GetActivePage()))->m_aList; ASSERT(rList);
			bEnable = rList.GetSelectedCount() > 0;
			break;
			}
		case CMyCashflowDlg::tab::index::TRANSACTIONS:
			{
			CTransListCtrl& rList = (r_cast<CTransactionsPageDialog*> (GetActivePage()))->m_aList; ASSERT(rList);
			bEnable = rList.GetSelectedCount() > 0;
			break;
			}
		}
	return bEnable;
	}

void CMyCashflowDlg::UpdateMenuItem(CMenu* pPopupMenu, UINT nCmd) 
	{
	switch (nCmd)
		{
		case ID_EDIT_SELECTEDITEM:
			{
			VERIFY(pPopupMenu->EnableMenuItem(nCmd, MF_BYCOMMAND | (CanEditSelectedItem() ? MF_ENABLED : MF_GRAYED)) != -1);
			break;
			}
		case ID_EDIT_DELSELENTRY:
			{
			VERIFY(pPopupMenu->EnableMenuItem(nCmd, MF_BYCOMMAND | (CanDeleteSelectedItem() ? MF_ENABLED : MF_GRAYED)) != -1);
			break;
			}
		}
	}

void CMyCashflowDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
	if (!bSysMenu)
	{
		// iterate over the menu items and update each one before display
		for (UINT iMenuItem = 0; iMenuItem < pPopupMenu->GetMenuItemCount(); ++iMenuItem)
		{
			UINT nId = pPopupMenu->GetMenuItemID(iMenuItem);
			if (nId != ID_SEPARATOR) UpdateMenuItem(pPopupMenu, nId);
		}
	}
	inherited::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
}

void CMyCashflowDlg::LoadXmlData(const CString & rXmlData)
	{
	CMarkup markup;
	if (markup.SetDoc(rXmlData))
		{
		if (markup.FindElem("MYCA$HFLOW"))
			{
			this->cashflow_dialog.GetMarkupData(markup);

			if (markup.FindChildElem("ACCOUNTS"))
				{
				markup.IntoElem();
				while (markup.FindChildElem("ACCOUNT"))
					{
					markup.IntoElem();
					CString aDesc;
					appl::Dollar::Value::type aAmnt = 0;
					if (markup.FindChildElem("DESCRIPTION"))
						{
						aDesc = markup.GetChildData();
						}
					if (markup.FindChildElem("AMOUNT"))
						{
						CString aString = markup.GetChildData();
						aAmnt = appl::Dollar::Value::FromStr(aString);
						}
					this->accounts_dialog.m_aList.AddEntry(aDesc, aAmnt);
					markup.OutOfElem();
					}
				markup.OutOfElem();
				}

			if (markup.FindChildElem("TRANSACTIONS"))
				{
				markup.IntoElem();
				while (markup.FindChildElem("TRANSACTION"))
					{
					markup.IntoElem();
					CString csDesc;
					CDate aDate;
					appl::Dollar::Value::type aAmnt = 0;
					if (markup.FindChildElem("DESCRIPTION"))
						{
						csDesc = markup.GetChildData();
						}
					if (markup.FindChildElem("DATE"))
						{
						CString csString = markup.GetChildData();
						aDate = convert::TextToDate(csString);
						}
					if (markup.FindChildElem("AMOUNT"))
						{
						CString csString = markup.GetChildData();
						aAmnt = appl::Dollar::Value::FromStr(csString);
						}
					CDateRepeater* pRepeater = NULL;
					if (markup.FindChildElem("REPEAT"))
						{
						CString csType = markup.GetChildAttrib("Type");
						size_t nDelta = atoi(markup.GetChildAttrib("Delta"));
						size_t nCount = atoi(markup.GetChildAttrib("Count"));
						if (csType == "Once") pRepeater = new CDateRepeaterOnce();
						else if (csType == "Daily") pRepeater = new CDateRepeaterDaily(nDelta, nCount);
						else if (csType == "Weekly") pRepeater = new CDateRepeaterWeekly(nDelta, nCount);
						else if (csType == "Monthly") 
							{
							int iDay = atoi(markup.GetChildAttrib("Day"));
							pRepeater = new CDateRepeaterMonthly(iDay, nDelta, nCount);
							}
						else if (csType == "Yearly") pRepeater = new CDateRepeaterYearly(nDelta, nCount);
						}
					if (pRepeater == NULL) pRepeater = new CDateRepeaterOnce();
					this->transactions_dialog.m_aList.AddEntry(aDate, csDesc, aAmnt, pRepeater);
					markup.OutOfElem();
					}
				markup.OutOfElem();
				}
			}
		}
	last_saved_state = GetMarkup();
	}

void CMyCashflowDlg::SetupFirstRun()
	{
	this->accounts_dialog.m_aList.AddEntry("Checking", 0);
	this->accounts_dialog.m_aList.AddEntry("Savings", 0);
	this->transactions_dialog.m_aList.AddEntry(CDate::Today(), "Payday", 400, new CDateRepeaterWeekly(2)); 
	this->transactions_dialog.m_aList.AddEntry(CDate::Today(), "Rent/Mortgage", -600, new CDateRepeaterMonthly(1)); 
	}

void CMyCashflowDlg::Load()
{
	TRY
	{
		CStdioFile file(CMyCashflowApp::GetDefaultFilePath(), CFile::modeRead | CFile::shareDenyWrite);
		CString aDoc;
		CString aToken;
		while (file.ReadString(aToken)) aDoc += aToken;
		LoadXmlData(aDoc);
	}
	CATCH (CFileException, e) 
	{
		if (e->m_cause == CFileException::fileNotFound) SetupFirstRun();
	}
	END_CATCH
}

void CMyCashflowDlg::CallEndDialog(int nEndID)
	{
	EndDialog(nEndID);	
	}

void CMyCashflowDlg::OnFileExit() 
	{
	if (ConditionalSave()) CallEndDialog(IDCANCEL);	
	}

void CMyCashflowDlg::OnFileSave() 
	{
	Save();
	}

CMarkup CMyCashflowDlg::GetMarkup() const
	{
	CMarkup markup;
	VERIFY(markup.AddElem(xml::HEAD));
	VERIFY(markup.AddAttrib("Format", "1"));
	this->cashflow_dialog.PutMarkupData(markup);
	this->accounts_dialog.PutMarkupData(markup);
	this->transactions_dialog.PutMarkupData(markup);
	return markup;
	}

void CMyCashflowDlg::Save()
	{
	CMarkup markup = this->GetMarkup();
	try
		{
		CStdioFile file(CMyCashflowApp::GetDefaultFilePath(), CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive);
		file.WriteString(markup.GetDoc());
		this->last_saved_state = markup;
		}
	catch (CFileException *)
		{
		// ...
		}
	}

void CMyCashflowDlg::OnAppAbout() 
	{
	CAboutDlg oDlg;
	oDlg.DoModal();
	}

void CAboutDlg::OnHomePage() 
	{
	LaunchHomePage();
	}

BOOL CMyCashflowDlg::CallPostMessage(UINT message, WPARAM wParam, LPARAM lParam)
	{
	return PostMessage(message, wParam, lParam);
	}

void CMyCashflowDlg::ResizeDlgPage(CWnd & page, const CRect & inside) 
	{
	if (page.m_hWnd != 0) 
		{
		CRect page_window;
		page.GetWindowRect(page_window);
		CRect window(inside);
		ClientToScreen(window);
		page.SetWindowPos(NULL, 0, 0, 
			inside.Width() - (page_window.left - window.left) - CWndInfo(*GetTabControl()).LeftBorderSize() - 1,
			inside.Height() - (page_window.top - window.top) - CWndInfo(*GetTabControl()).BottomBorderSize() - 1,
			SWP_NOZORDER | SWP_NOMOVE);
		}
	}

void CMyCashflowDlg::ResizePropertyPages(const CRect & inside) 
	{
	ResizeDlgPage(this->cashflow_dialog, inside);
	ResizeDlgPage(this->accounts_dialog, inside);
	ResizeDlgPage(this->transactions_dialog, inside);
	}

void CMyCashflowDlg::OnSize(UINT nType, int cx, int cy) 
	{
	TRACE("CMyCashflowDlg::OnSize(UINT nType, %d, %d)\n", cx, cy);
	inherited::OnSize(nType, cx, cy);
	if ((GetTabControl() != NULL) && (GetTabControl()->m_hWnd != 0)) 
		{
		m_rConfigDataManager.SetKeyValue(cfg::cat::FRAME, Settings::category::frame::key::WIDTH, CWndInfo(this).Width());
		m_rConfigDataManager.SetKeyValue(cfg::cat::FRAME, Settings::category::frame::key::HEIGHT, CWndInfo(this).Height());
		CRect aTabControlRect = GetTabControlRect();
		GetTabControl()->MoveWindow(aTabControlRect);
		ResizePropertyPages(aTabControlRect);
		if (this->chart.m_hWnd != 0) 
			{
			this->chart.MoveWindow(&GetChartRect(), FALSE);
			this->chart.InvalidateRect(NULL, FALSE);
			}
		}
	}

int CMyCashflowDlg::EdgeSpace() const
	{
	CRect aRect = CWndInfo(*GetTabControl()).WindowRect();
	ScreenToClient(aRect);
	return aRect.left;
	}

CRect CMyCashflowDlg::GetChartRect () const
	{
	//TRACE("CMyCashflowDlg::GetChartRect\n");
	CRect client;
	GetClientRect(client);
	CRect window(client);
	ClientToScreen(window);
	CRect client_tab;
	GetTabControl()->GetClientRect(client_tab);
	CRect window_tab;
	GetTabControl()->GetWindowRect(window_tab);
	return CRect(0, (window_tab.top - window.top) + window_tab.Height(), client.Width(), client.bottom);
	}

CRect CMyCashflowDlg::GetTabControlRect() const
	{
	CRect client;
	GetClientRect(client);
	return CRect(
		EdgeSpace(), EdgeSpace(),
		client.Width() - EdgeSpace(), 
		client.Height() - PercentAdjust(client.Height(), 25) - EdgeSpace());
	}

void CMyCashflowDlg::OnMove(int x, int y)
	{
	//TRACE("CMyCashflowDlg::OnMove(%d,%d)\n", x, y);
	inherited::OnMove(x, y);
	m_rConfigDataManager.SetKeyValue(cfg::cat::FRAME, Settings::category::frame::key::LEFT, CWndInfo(this).Left());
	m_rConfigDataManager.SetKeyValue(cfg::cat::FRAME, Settings::category::frame::key::TOP, CWndInfo(this).Top());
	}

void CMyCashflowDlg::HideButton(UINT id) const
	{
	GetDlgItem(id)->EnableWindow(FALSE);
	GetDlgItem(id)->ShowWindow(SW_HIDE);
	}

int CALLBACK CMyCashflowDlg::XmnPropSheetCallback(HWND hWnd, UINT message, LPARAM lParam)
// see: http://support.microsoft.com/kb/300606
{
	extern int CALLBACK AfxPropSheetCallback(HWND, UINT message, LPARAM lParam);
	// XMN: Call MFC's callback
	int nRes = AfxPropSheetCallback(hWnd, message, lParam);
	switch (message)
	{
	case PSCB_PRECREATE:
	  // Set our own window styles
	  ((LPDLGTEMPLATE)lParam)->style |= (DS_3DLOOK | DS_SETFONT | WS_THICKFRAME | WS_SYSMENU | WS_POPUP | WS_VISIBLE | WS_CAPTION);
	  break;
	}
	return nRes;
}
 
#include <AFXDLGS.H>
#include ".\mymoneydlg.h"

int CMyCashflowDlg::DoModal() 
// see: http://support.microsoft.com/kb/300606
{
	// Hook into property sheet creation code
	m_psh.dwFlags |= PSH_USECALLBACK;
	m_psh.pfnCallback = XmnPropSheetCallback;
	return CPropertySheet::DoModal();
}

void CAboutDlg::OnBnClickedDonate()
{
	LaunchWebPage(data::DONATE_URL);
}

void CMyCashflowDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
	{
	lpMMI->ptMinTrackSize.x = 300;
	lpMMI->ptMinTrackSize.y = 300; 
	}

bool CMyCashflowDlg::IsModified() const 
	{ 
	return  (GetMarkup().GetDoc() != last_saved_state.GetDoc()); 
	}

int AFXAPI CMyCashflowDlg::CallAfxMessageBox(UINT nIDPrompt, UINT nType, UINT nIDHelp) const
	{
	return AfxMessageBox(nIDPrompt, nType, nIDHelp);
	}

int CMyCashflowDlg::AskToSave() const
	{
	return CallAfxMessageBox(IDS_SAVECHANGES, MB_YESNOCANCEL | MB_ICONQUESTION);
	}

bool CMyCashflowDlg::ConditionalSave()
	{
	if (IsModified()) 
		{
		if (AutoSave()) Save(); else switch (AskToSave()) 
			{
			case IDCANCEL: return false;
			case IDYES: Save(); 
			}
		}
	return true;
	}

void CMyCashflowDlg::OnFileAutosaveonexit()
	{
	CMenu * sub_menu = main_menu.GetSubMenu(0);
	uint state = sub_menu->GetMenuState(ID_FILE_AUTOSAVEONEXIT,  MF_BYCOMMAND); ASSERT(state != 0xFFFFFFFF);
	sub_menu->CheckMenuItem(ID_FILE_AUTOSAVEONEXIT,  MF_BYCOMMAND | ((state & MF_CHECKED) ? MF_UNCHECKED : MF_CHECKED));
	m_rConfigDataManager.SetKeyValue(cfg::cat::ROOT, Settings::Root::Key::AUTOSAVE, (state & MF_CHECKED) ? 0 : 1);
	}

bool CMyCashflowDlg::AutoSave() const
	{
	return ((MainMenu().GetSubMenu(0)->GetMenuState(ID_FILE_AUTOSAVEONEXIT,  MF_BYCOMMAND) & MF_CHECKED) != 0);
	}

