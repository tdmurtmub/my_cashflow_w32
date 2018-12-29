// (C) 2004 Wesley Steiner

#pragma once

#include <win\mfc\cdate.h>
#include "resource.h"
#include "SharedItemEditDialog.h"

#define GROUP struct

class CTransEditDlg : public CSharedItemEditDialog
{
public:
	GROUP Mode { enum type {ADD, UPDATE}; };

public:
	GROUP Repeat { 
		GROUP Index	{
			SCM(UINT, ONCE, 0);
			SCM(UINT, REPEAT, 1);
		};
		GROUP Ends {
			GROUP Index	{
				SCM(UINT, NEVER, 0);
				SCM(UINT, AFTER, 1);
			};
			GROUP After { GROUP Spin {
				SCM(UINT, MIN_VAL, 2);
				SCM(UINT, MAX_VAL, UD_MAXVAL);
			}; };
		};
		GROUP Interval { 
			GROUP Freq { GROUP Index {
				enum {EVERY, EVERY_OTHER, EVERY_THIRD, EVERY_FOURTH};
			};};
			GROUP Unit { GROUP Index {
				enum {DAY, WEEK, MONTH, YEAR};
			};};
		};
	};

private:
	Mode::type m_eMode;

	void DoRepGrpSel(int iSel);
	void DoEndGrpSel(int iSel);

	bool IsDefaultMode() const { return m_eMode == Mode::ADD; }
	// Returns true if dialog is in default mode.

public:
	CTransEditDlg(Mode::type mode = Mode::ADD, CWnd* pParent = NULL);
	void InitializeControls(const CDate& start_date, LPCTSTR description, appl::Dollar::Value::type amount, CDateRepeater* repeater = new CDateRepeaterOnce());
	CDateRepeater* CreateDateRepeater();

// Dialog Data
	//{{AFX_DATA(CTransEditDlg)
	enum { IDD = IDD_ADD_TRANSACTION };
	CString my_description;
	UINT m_nAmt;
	CDate m_aStartDate;
	int			m_iDebitGrp;
	int			m_iRepGrp;
	CButton		m_aRepGrpOnce;
	CButton		m_aRepGrpEvery;
	int			m_iRep1Interval;
	CComboBox	m_aRep1Interval;
	int			m_iRep1Freq;
	CComboBox	m_aRep1Freq;
	int			m_iRepEndGrp;
	UINT		m_nOccur;
	CEdit		m_aOccur;
	CButton		m_aEndGrpNever;
	CButton		m_aEndGrpAfter;
	CSpinButtonCtrl	m_aOccurSpin;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	test_protected:

	// Generated message map functions
	//{{AFX_MSG(CTransEditDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRepeatsOnce();
	afx_msg void OnRepeatsEvery();
	afx_msg void OnEndsNever();
	afx_msg void OnEndsAfter();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// unit testing ...
private:
	friend class CTranxEditDlgTest;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
