// (C) 2004 Wesley Steiner

#if !defined(AFX_MYMONEYDLG_H__905CF30C_1A00_46BB_B7AE_B3E02A8CEA96__INCLUDED_)
#define AFX_MYMONEYDLG_H__905CF30C_1A00_46BB_B7AE_B3E02A8CEA96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AccountsPgDlg.h"
#include "CashFlowCtrl.h"
#include "CashFlowPgDlg.h"
#include "ChartCtrl.h"
#include "MyCashFlowDlgBase.h"
#include "MyMoney.h"
#include "TransactionsPgDlg.h"

class IConfigDataManager;

using namespace win::mfc;

class CAboutDlg : public CDialog
	{
	public: CAboutDlg();

	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_aVersion;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	protected:
	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnHomePage();
	afx_msg void OnBnClickedDonate();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	};

class CMyCashflowDlg : public CMyCashflowDlgBase
	{
	DECLARE_DYNAMIC(CMyCashflowDlg)

	public:
	struct tab { struct index { enum 
		{
		CASHFLOW = 0,	
		ACCOUNTS,
		TRANSACTIONS,
		COUNT
		};};};
	
	CAccountsPageDialog accounts_dialog;
	CTransactionsPageDialog transactions_dialog;
	CCashflowPageDialog cashflow_dialog;
	CChartCtrl chart;

	CMyCashflowDlg(IConfigDataManager & rConfigDataManager, size_t duration, const CRect & rect = CRect(0, 0, 0, 0));

	virtual int DoModal(); 
	test_virtual CAccountsPageDialog & AccountsPageDlg() { return this->accounts_dialog; }
	test_virtual CTransactionsPageDialog & TransactionsPageDlg() { return this->transactions_dialog; }

	//{{AFX_DATA(CMyCashflowDlg)
	enum { IDD = IDD_MYMONEY_DIALOG };
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CMyCashflowDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	test_protected:
	HICON m_hIcon;

	//{{AFX_MSG(CMyCashflowDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnEditAddTranx();
	afx_msg void OnEditAddAccount();
	afx_msg void OnEditSelItem();
	afx_msg void OnEditDelSelEntry();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnFileExit();
	afx_msg void OnFileSave();
	afx_msg void OnAppAbout();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnFileAutosaveonexit();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	private: 
	CRect initial_frame;
	IConfigDataManager & m_rConfigDataManager;
	CMenu main_menu;
	void HideButton(UINT id) const;
	int EdgeSpace() const;
	CRect GetChartRect() const;
	CRect GetTabControlRect() const;
	void ResizeDlgPage(CWnd & page, const CRect & inside);
	void ResizePropertyPages(const CRect & inside);
	virtual void UpdateMenuItem(CMenu* pPopupMenu, UINT nCmd);
	void LoadXmlData(const CString & rXmlData);
	void Load();
	static int CALLBACK XmnPropSheetCallback(HWND hWnd, UINT message, LPARAM lParam);
	
	test_private: 
	static const bool WITH_CANCEL;
	CMarkup last_saved_state;
	test_virtual int AskToSave() const;
	test_virtual int AFXAPI CallAfxMessageBox(UINT nIDPrompt, UINT nType = MB_OK, UINT nIDHelp = (UINT) -1) const;
	test_virtual bool AutoSave() const;
	test_virtual bool CanEditSelectedItem() const;
	test_virtual bool CanDeleteSelectedItem() const;
	test_virtual bool ConditionalSave();
	test_virtual bool IsModified() const;
	test_virtual BOOL CallPostMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
	test_virtual CMarkup GetMarkup() const;
	test_virtual void CallEndDialog(int nEndID);
	test_virtual void Save();
	test_virtual void SetupFirstRun();
	CMenu const & MainMenu() const { return main_menu; }
	};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYMONEYDLG_H__905CF30C_1A00_46BB_B7AE_B3E02A8CEA96__INCLUDED_)
