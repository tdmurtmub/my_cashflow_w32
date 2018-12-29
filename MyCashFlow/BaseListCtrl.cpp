// (C) 2004 Wesley Steiner

#include "stdafx.h"

#include "BaseListCtrl.h"

#include "mfc\ConfigDataManager.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBaseListCtrl::CBaseListCtrl(IConfigDataManager & rConfigDataManager)
	: m_rConfigDataManager(rConfigDataManager)
	{
	}

CBaseListCtrl::~CBaseListCtrl()
	{
	}

BEGIN_MESSAGE_MAP(CBaseListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CBaseListCtrl)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CBaseListCtrl::GetSelectedItemIndex() const 
	{ 
	POSITION pos = GetFirstSelectedItemPosition();
	return (pos == NULL) ? -1 : GetNextSelectedItem(pos);
	}

bool CBaseListCtrl::ConfirmDeleteEntry() const
	{
	CString aText, aTitle;
	VERIFY(aTitle.LoadString(IDS_CONFIRMDELETETITLE));
	VERIFY(aText.LoadString(IDS_CONFIRMDELETE));
	return GetParent()->MessageBox(aText, aTitle, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES;
	}

void CBaseListCtrl::TryDeleteEntry(UINT nItem)
	{
	ASSERT(nItem >= 0);
	if (ConfirmDeleteEntry()) DeleteEntry(nItem);
	}

void CBaseListCtrl::PreSubclassWindow() 
	{
	SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	CListCtrl::PreSubclassWindow();
	}

void CBaseListCtrl::OnNotifyImpl(const wm::Notify & aNotifyMsg) 
	{
	if (aNotifyMsg.Code() == HDN_ENDTRACKW) SaveColumnWidth(((LPNMHEADER) aNotifyMsg.lParam())->iItem);
	}

BOOL CBaseListCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
	{
	OnNotifyImpl(wm::Notify(wParam, lParam, pResult));
	return CListCtrl::OnNotify(wParam, lParam, pResult);
	}

CString CBaseListCtrl::GetColumnWidthRegKey(int aColIndex)
	{
	ASSERT(aColIndex >= 0);
	CString aString;
	aString.Format("ColumnWidth_%d", aColIndex);
	return aString;
	}

void CBaseListCtrl::SaveColumnWidth(int aColIndex) const
	{
	ASSERT(aColIndex >= 0);
	m_rConfigDataManager.SetKeyValue(MySettingsCategory(), CBaseListCtrl::GetColumnWidthRegKey(aColIndex), GetColumnWidth(aColIndex));
	}

UINT CBaseListCtrl::ColumnWidth(uint aColumnIndex) const
	{
//	ASSERT(aColumnIndex < column::COUNT);
	return m_rConfigDataManager.GetKeyValue(MySettingsCategory(), GetColumnWidthRegKey(aColumnIndex), DefaultColumnWidth(aColumnIndex));
	}

void CBaseListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
	{
	if (GetSelectedItemIndex() >= 0) EditEntry(GetSelectedItemIndex()); 
	else CListCtrl::OnLButtonDblClk(nFlags, point);
	}
