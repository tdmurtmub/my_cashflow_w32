// (C) 2004 Wesley Steiner

#if !defined(AFX_BASELISTCTRL_H__1535B582_17BE_4E31_84C9_65971D7A71C0__INCLUDED_)
#define AFX_BASELISTCTRL_H__1535B582_17BE_4E31_84C9_65971D7A71C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <lib.h>

class IConfigDataManager;

namespace wm 
	{
	class Message
		{
		public:
		Message(WPARAM wParam, LPARAM lParam, LRESULT* pResult) : m_wParam(wParam), m_lParam(lParam), m_pResult(pResult) { }
		LPARAM lParam() const { return m_lParam; }
		protected:
		WPARAM m_wParam; 
		LPARAM m_lParam;
		LRESULT * m_pResult;
		};

	class Notify : public Message
		{
		public:
		Notify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) : Message(wParam, lParam, pResult) { }
		uint Code() const { return ((LPNMHDR) m_lParam)->code; }
		};
	}

interface IBaseListCtrl 
	{
	virtual void EditEntry(uint nIndex) = 0;
	};

class CBaseListCtrl : public CListCtrl, public IBaseListCtrl 
	// Serves as a base class for all list controls in the app.
	{
	public:
	CBaseListCtrl(IConfigDataManager & rConfigDataManager);

	static CString GetColumnWidthRegKey(int aColIndex);

	virtual int GetSelectedItemIndex() const;
	// Returns the index of the currently selected list item or -1 if nothing selected.

	void TryDeleteEntry(uint nItem);
	virtual ~CBaseListCtrl();

	protected: 
	virtual void DeleteEntry(uint nItem) = 0;
	virtual void EditEntry(uint nIndex) { }
	virtual CString MySettingsCategory() const = 0;
	virtual uint DefaultColumnWidth(uint aColumnIndex) const = 0;
	test_virtual void SaveColumnWidth(int aColIndex) const;
	IConfigDataManager & ConfigDataManager() const { return m_rConfigDataManager; }
	uint ColumnWidth(uint aColumnIndex) const;

	//{{AFX_VIRTUAL(CBaseListCtrl)
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CBaseListCtrl)
	afx_msg void OnLButtonDblClk(uint nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	private:
	IConfigDataManager & m_rConfigDataManager;

	test_private: virtual bool ConfirmDeleteEntry() const;
	test_private: void OnNotifyImpl(const wm::Notify & aNotifyMsg);

	friend
	class CBaseListCtrlTestFixture;
	};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
