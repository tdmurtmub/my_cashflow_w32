// (C) 2004 Wesley Steiner

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#define Appl CMyCashflowApp

#include <my.h>
#include <win/mfc/CDate.h>
#include <win/mfc/Markup.h>
#include <win/mfc/MyWinApp.h>

#include "resource.h"

class CAboutDlg;
class CCashFlowListCtrl;
class SettingsTest;
class CMarkup;
class CMyCashflowDlg;
class StorageTest;

class AutoPersistTest;
class IConfigDataManager;

struct os 
	{ 
	static bool OpenHtmlPage(const CString & aUrl) { return ((int) ShellExecute(NULL, "open", aUrl, "", ".\\", SW_SHOW) > 32); }

	struct file 
		{ 
		static bool Exists(const TCHAR * pPath) { return FileExists(pPath); } 
		};
	};

#define HOMEPAGE_FOLDER MY_DOMAIN_URL _T("/mycashflow/")

namespace app 
	{
	namespace data 
		{
		static const TCHAR * HOMEPAGE_URL = HOMEPAGE_FOLDER _T("main.html");
		static const TCHAR * DONATE_URL = HOMEPAGE_FOLDER _T("donate.html");
		static const TCHAR * REGISTRY_KEY = my::win::app::REGISTRY_KEY;
		};

	namespace cfg
		{
		namespace cat 
			{
			static const TCHAR * ROOT = _T("");
			static const TCHAR * FRAME = _T("Frame");
			static const TCHAR * CASHFLOW = _T("CashFlow");
			namespace root 
				{ 
				static const TCHAR * STORAGEDIR = _T("StorageDirectory");
				}
			}
		}
	
	namespace xml
		{
		static const TCHAR * HEAD = _T("MYCA$HFLOW");
		}

	interface IMarkupPersistence
		{
		virtual void PutMarkupData(CMarkup& rMarkup) const = 0;
		// Puts, or writes, persistent data from the object into rMarkup.

		virtual void GetMarkupData(CMarkup& rMarkup) = 0;
		// Gets, or reads, persistent data from rMarkup into the object.
		};

	interface ISettings
		{ 
		virtual void SetKeyValue(const CString &category, const CString &key, int value) = 0;
		virtual void SetKeyValue(const CString &category, const CString &key, const CString & value) = 0;
		virtual void SetKeyValueInt(const CString &category, const CString &key, int value) = 0; // OBSOLETE: use SetKeyValue
		virtual void SetKeyValueStr(const CString &category, const CString &key, const CString & value) = 0; // OBSOLETE: use SetKeyValue
		virtual int GetKeyValueInt(const CString &category, const CString &key) const = 0;
		virtual CString GetKeyValueStr(const CString &category, const CString &key) const = 0;
		};

	class BooleanSetting
		{
		};
		
	class Settings : utility_declare_no_copy, public ISettings
		{
		public:
		struct Root { struct Key
			{
			static const TCHAR *AUTOSAVE;
			};};
		struct category 
			{
			struct frame { struct key 
				{
				static const TCHAR * LEFT;
				static const TCHAR * TOP;
				static const TCHAR * WIDTH;
				static const TCHAR * HEIGHT;
				};};
			struct cashflow { struct Key 
				{
				static const TCHAR * DURATION;
				};};
			};

		interface IStorage 
			{
			virtual void LoadConfig(ISettings * settings) = 0;
			};

		class Storage : public IStorage
			{
			public:
			struct Exception 
				{
				class BadlyFormedXml : public CException
					{
					};
				};

			// IStorage ...
			void LoadConfig(ISettings * settings);

			private: 
			virtual CString GetXmlString() const;
			};

		static Settings * Instance();

		// ISettings ...
		public:
		void SetKeyValue(const CString &category, const CString &key, int value);
		void SetKeyValue(const CString &category, const CString &key, const CString &value);
		void SetKeyValueInt(const CString &category, const CString &key, int value);
		void SetKeyValueStr(const CString &category, const CString &key, const CString &value);
		int GetKeyValueInt(const CString &category, const CString &key) const;
		CString GetKeyValueStr(const CString &category, const CString &key) const;

		private:
		Settings();
		static Settings * instance;
		static IStorage * storage;
		typedef CMapStringToString KeyValuePairs;
		static CMapStringToOb theData; // category to  KeyValuePairs
	
		#ifdef TEST
		private:
		friend class SettingsTest;
		friend class StorageTest;
		#endif TEST
		};

	namespace convert
		{
		CDate TextToDate(const CString & date);
		CString DateToText(const CDate & date);
		}

	void InitDateTimePickerControl(HWND hWnd);
	void LaunchWebPage(CString const & url);
	void LaunchHomePage();
	CString GetBrowserNotFoundMessage(); 
	CString GetBrowserNotFoundTitle();
	}

class CMyCashflowAppHooks 
	{
	test_private: test_virtual void PASCAL CFileRename(LPCTSTR lpszOldName, LPCTSTR lpszNewName) { CFile::Rename(lpszOldName, lpszNewName); }
	friend class CMyCashflowApp;
	};

#define appl CMyCashflowApp

class CMyCashflowApp : public CMyWinApp
	{
	public:
		struct Dollar { struct Value 
			{ 
			typedef int type; 

			SCM(type, ZERO, 0);

			static CString ToStr(type aDollarValue)
			// Returns the default character version of a dollar value.
				{
				CString str;
				str.Format("%d", aDollarValue);
				return str;
				}

			static type FromStr(const TCHAR * pDollarValueAsText)
				{
				ASSERT(pDollarValueAsText);
				return atoi(pDollarValueAsText);
				}
			}; };

	public:
	struct column
	{
		struct Width
		// Defines the width of list columns.
		{
			SCM(UINT, DATE, 70);
			SCM(UINT, AMOUNT, 60);
			SCM(UINT, BALANCE, 80);
		};
	};

	struct Chart 
		{
		static void Refresh();
		};

	CMyCashflowApp();

	static CString GetDefaultFileName();
	static CString GetDefaultFilePath();
	static CString GetCurrentFilePath();

	CMyCashflowDlg * main_dialog;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyCashflowApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	private:
		static const CString AUTOSAVE_FILENAME;
 		friend class CAboutDlg;

	public:
		//{{AFX_MSG(CMyCashflowApp)
		afx_msg void OnHelpHomePage();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	test_private: 
		void RenameLegacyFile();
		static CString DiscoverPersistenceFolderPath();
		static CString the_storage_dir;
		static CRect InitialFrameRect();
		static const CString PERSISTENCE_EXTENSION;
		static CString GetVersionStr();
		static CMyCashflowAppHooks * hooks;
		static CString InitializeStorageDir(win::IDirectory const & directory);
	};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
