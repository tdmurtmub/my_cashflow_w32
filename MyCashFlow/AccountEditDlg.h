#pragma once

#include <set>
#include "resource.h"
#include "SharedItemEditDialog.h"

class CAccountEditDlg : public CSharedItemEditDialog
{
public:
	struct Mode { enum type {ADD, UPDATE}; };

private:
	Mode::type m_eMode;
	typedef std::set<CString> DescList;
	static DescList theDescriptionListCache;

	bool IsDefaultMode() const { return m_eMode == Mode::ADD; }
	// Returns true if dialog is in default mode.

public:
	CAccountEditDlg(Mode::type mode = Mode::ADD, CWnd* pParent = NULL);

	//{{AFX_DATA(CAccountEditDlg)
	enum { IDD = IDD_ADD_ACCOUNT };
	CString	myDescription;
	INT	m_nBalance;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAccountEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	// Generated message map functions
	//{{AFX_MSG(CAccountEditDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	friend class CAccntEditDlgTest;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
