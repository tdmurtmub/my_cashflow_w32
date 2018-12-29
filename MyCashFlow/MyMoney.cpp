// (C) 2004 Wesley Steiner

#include "stdafx.h"

#include "MyMoney.h"

#include <shlobj.h>

#include <FixedArray.h>
#include "mfc\ConfigDataManager.h"
#include "mfc\wndinfo.h"

#include "MyMoneyDlg.h"

#ifdef TEST
#include <cppunit\extensions\HelperMacros.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;
using namespace lib;
using namespace win;
using namespace win::mfc;
using namespace app;
using namespace app::data;

BEGIN_MESSAGE_MAP(CMyCashflowApp, CMyWinApp)
	//{{AFX_MSG_MAP(CMyCashflowApp)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CMyWinApp::OnHelp)
	ON_COMMAND(ID_HELP_HOMEPAGE, OnHelpHomePage)
END_MESSAGE_MAP()

const CString CMyCashflowApp::PERSISTENCE_EXTENSION = "MyCashflow";

CMyCashflowApp theApp;

CMyCashflowApp::CMyCashflowApp()
	{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	setlocale(LC_ALL, "");
	}

CRect CMyCashflowApp::InitialFrameRect()
	{
	CRect aFrameRect;
	aFrameRect.left = ConfigDataManager::Instance().GetKeyValue(cfg::cat::FRAME, Settings::category::frame::key::LEFT, 0);
	aFrameRect.top = ConfigDataManager::Instance().GetKeyValue(cfg::cat::FRAME, Settings::category::frame::key::TOP, 0);
	aFrameRect.right = aFrameRect.left + ConfigDataManager::Instance().GetKeyValue(cfg::cat::FRAME, Settings::category::frame::key::WIDTH, 0);
	aFrameRect.bottom = aFrameRect.top + ConfigDataManager::Instance().GetKeyValue(cfg::cat::FRAME, Settings::category::frame::key::HEIGHT, 0);
	return aFrameRect;
	}

CString CMyCashflowApp::InitializeStorageDir(IDirectory const & directory)
	{
	return (directory.Exists() && !directory.IsReadOnly()) ? directory.Name() : the_storage_dir;
	}

void CMyCashflowApp::RenameLegacyFile()
	{
	try
		{
		CString old_name = the_storage_dir + "\\default.MyCa$hflow";
		hooks->CFileRename(old_name, GetDefaultFilePath());
		}
	catch(CFileException *e )
		{
		e->Delete();
		}	
	}

BOOL CMyCashflowApp::InitInstance()
	{
	CMyWinApp::InitInstance();
	the_storage_dir = InitializeStorageDir(Directory(ConfigDataManager::Instance().GetKeyValue(cfg::cat::ROOT, cfg::cat::root::STORAGEDIR, the_storage_dir)));
	RenameLegacyFile();
	main_dialog = new CMyCashflowDlg(ConfigDataManager::Instance(),Settings::Instance()->GetKeyValueInt(cfg::cat::CASHFLOW,Settings::category::cashflow::Key::DURATION), InitialFrameRect());
	m_pMainWnd = main_dialog;
	main_dialog->DoModal();
	delete main_dialog;

	// Since the dialog has been closed, return FALSE so that we exit the application, rather than start the application's message pump.
	return FALSE;
	}

void CMyCashflowApp::Chart::Refresh()
{
	theApp.main_dialog->chart.Refresh();
}

CString CMyCashflowApp::GetVersionStr()
{
	CString aExeName(theApp.m_pszExeName);
	aExeName += ".exe";
	DWORD dwHandle;
	DWORD dwSize = GetFileVersionInfoSize(aExeName.GetBuffer(aExeName.GetLength()), &dwHandle); ASSERT(dwSize > 0);
	Utility::FixedArray<BYTE> bData(dwSize);
	VERIFY(GetFileVersionInfo(aExeName.GetBuffer(aExeName.GetLength()), dwHandle, dwSize, bData));

	UINT dwLen;
	LPVOID pBuffer;
	VERIFY(VerQueryValue((const LPVOID) (const BYTE*) bData, TEXT("\\"), &pBuffer, &dwLen) > 0); 

	VS_FIXEDFILEINFO * pFixedFileInfo = (VS_FIXEDFILEINFO *) pBuffer;

	VERIFY(VerQueryValue(bData, TEXT("\\StringFileInfo\\040904b0\\ProductVersion"), &pBuffer, &dwLen) > 0); 

	int aBuildNo = pFixedFileInfo->dwProductVersionLS & 0x0000FFFF;
	CString csVersionStr;
	if (aBuildNo > 1)
	{
		csVersionStr.Format("%s Build #%d", (const char*) pBuffer, aBuildNo);
	}
	else
	{
		csVersionStr.Format("%s", (const char*) pBuffer);
	}

	return csVersionStr;
}

const TCHAR * Settings::category::frame::key::LEFT = "Left";
const TCHAR * Settings::category::frame::key::TOP = "Top";
const TCHAR * Settings::category::frame::key::WIDTH = "Width";
const TCHAR * Settings::category::frame::key::HEIGHT = "Height";
const TCHAR * Settings::category::cashflow::Key::DURATION = "Duration";
const TCHAR * Settings::Root::Key::AUTOSAVE = _T("Autosave");

using namespace std;

CString Settings::GetKeyValueStr(const CString &category, const CString &key) const
	{ 
	CString value;
	KeyValuePairs *kvp;
	if (!theData.Lookup(category, (CObject*&) kvp)) theData[category] = (kvp = new KeyValuePairs());
	if (!kvp->Lookup(key, value))
		{
		if (category == cfg::cat::CASHFLOW)
			{
			if (key == category::cashflow::Key::DURATION) 
				{
					value.Format("%d", CCashFlowListCtrl::Limits::Duration::DEFAULT);
				}
			}
		kvp->SetAt(key, value.GetBuffer());
		}
	return value;
	}

int Settings::GetKeyValueInt(const CString &category, const CString &key) const
	{
	return atoi((LPCSTR) GetKeyValueStr(category, key));
	}

Settings::IStorage * Settings::storage = NULL;
CMapStringToOb Settings::theData;

void Settings::SetKeyValueInt(const CString &category, const CString &key, int value)
	{
	SetKeyValue(category, key, value);
	}

void Settings::SetKeyValue(const CString &category, const CString &key, int value)
	{
	SetKeyValueStr(category, key, ToCString(value));
	}

void Settings::SetKeyValueStr(const CString &category, const CString &key, const CString &value)
	{
		SetKeyValue(category, key, value);
	}

void Settings::SetKeyValue(const CString &category, const CString &key, const CString &value)
	{
	KeyValuePairs *kvp;
	if (!theData.Lookup(category, (CObject*&) kvp)) theData[category] = (kvp = new KeyValuePairs());
	kvp->SetAt(key, value);
	}

void Settings::Storage::LoadConfig(ISettings * settings) 
	{
	ASSERT(settings);

	CString csXml(GetXmlString());
	if (csXml.IsEmpty()) return;	 

	CMarkup aMarkup;
	if (!aMarkup.SetDoc(csXml)) throw new Exception::BadlyFormedXml;

	if (aMarkup.FindElem(xml::HEAD))
		{
		if (aMarkup.FindChildElem("CASHFLOW"))
			{
			CString csString = aMarkup.GetChildAttrib(Settings::category::cashflow::Key::DURATION);
			if (csString.GetLength() > 0) 
				{
				// ensure the persisted value is within range ...
				size_t aDuration = atoi(csString);
				if (aDuration < CCashFlowListCtrl::Limits::Duration::MINIMUM || aDuration > CCashFlowListCtrl::Limits::Duration::MAXIMUM) aDuration = CCashFlowListCtrl::Limits::Duration::DEFAULT;
				settings->SetKeyValueInt(cfg::cat::CASHFLOW, Settings::category::cashflow::Key::DURATION, aDuration);
				}
			}
		}
	}

Settings * Settings::instance = NULL;

Settings * Settings::Instance()
	{
	if (instance == NULL)
		{
		if (storage == NULL) storage = new Storage();
		instance = new Settings;
		}

	return instance;
	}

Settings::Settings()
	{
	storage->LoadConfig(this);
	}

CString Settings::Storage::GetXmlString() const 
	{ 
	CString aDoc;
	TRY
		{
		CStdioFile aFile(CMyCashflowApp::GetDefaultFilePath(), CFile::modeRead | CFile::shareDenyWrite);
		CString aToken;
		while (aFile.ReadString(aToken)) aDoc += aToken;
		}
	CATCH(CFileException, pFileException) 
		{
		// ignore if file does not exist
		}
	END_CATCH
	return aDoc;
	}

CMyCashflowAppHooks * CMyCashflowApp::hooks = new CMyCashflowAppHooks;

CString CMyCashflowApp::DiscoverPersistenceFolderPath()
	{
	#if defined(TEST) || defined (_DEBUG)
	return ".\\";
	#else
	TCHAR szPath[MAX_PATH];
	SHGetSpecialFolderPath(NULL, szPath, CSIDL_PERSONAL, 0);
	return szPath;
	#endif
	}

CString CMyCashflowApp::the_storage_dir = CMyCashflowApp::DiscoverPersistenceFolderPath();

CString CMyCashflowApp::GetDefaultFilePath()
	{ 
	ASSERT(hooks != NULL);
	CString result(the_storage_dir);
	result.TrimRight("\\ ");
	result += '\\';
	result += GetDefaultFileName();
	return result; 
	}

CString CMyCashflowApp::GetCurrentFilePath()
	{
	return GetDefaultFilePath();
	}

CString CMyCashflowApp::GetDefaultFileName()
	{ 
	return "default." + PERSISTENCE_EXTENSION;
	}

namespace app
	{
	void InitDateTimePickerControl(HWND hWnd)
		{
		ASSERT(hWnd);
		SYSTEMTIME aSysTimeArray [2] = { 1970, 1, 0, 1, 0, 0, 0, 0 };
		VERIFY(DateTime_SetRange(hWnd, GDTR_MIN , &aSysTimeArray));
		}

	CDate convert::TextToDate(const CString& date)
		{
		// highly optimized conversion of mm/dd/yyyy into a CDate
		TCHAR buf[11];
		ASSERT(date.GetLength() == 10);
		memcpy(buf, s_cast<const TCHAR *> (date), sizeof(buf));
		ASSERT(buf[2] == '/');
		buf[2] = '\0';
		ASSERT(buf[5] == '/');
		buf[5] = '\0';
		return CDate(atoi(&buf[6]), atoi(&buf[0]), atoi(&buf[3]));
		}

	CString convert::DateToText(const CDate & date)
		{
		CString str;
		str.Format("%02d/%02d/%d", date.GetMonth(), date.GetDay(), date.GetYear());
		return str;
		}
	}

namespace app 
	{
	void LaunchWebPage(CString const & url)
		{
		if (!os::OpenHtmlPage(url))
			{
				MessageBox(theApp.m_pMainWnd->m_hWnd, GetBrowserNotFoundMessage(), GetBrowserNotFoundTitle(), MB_OK | MB_ICONWARNING);
			}
		}

	void LaunchHomePage()
		{
		LaunchWebPage(app::data::HOMEPAGE_URL);
		}

	CString GetBrowserNotFoundMessage()
		{
		CString aString;
		VERIFY(aString.LoadString(IDS_NOBROWSERTEXT));
		CString aMsg;
		aMsg.Format("%s\n\n%s", aString, data::HOMEPAGE_URL);
		return aMsg;
		}
		
	CString GetBrowserNotFoundTitle()
		{
		CString aString;
		VERIFY(aString.LoadString(IDS_ERRORMSGTITLE));
		return aString;
		}
	}

void CMyCashflowApp::OnHelpHomePage()
	{
	LaunchHomePage();
	}

#ifdef TEST

#include <cppunit\extensions\HelperMacros.h>
#include ".\mymoney.h"

class CMyCashflowAppFixture : public CppUnit::TestFixture 
	{
	void TestInitialFrameRect()
		{
		ConfigDataManager::Instance().SetKeyValue(cfg::cat::FRAME, Settings::category::frame::key::LEFT, 12);
		ConfigDataManager::Instance().SetKeyValue(cfg::cat::FRAME, Settings::category::frame::key::TOP, 23);
		ConfigDataManager::Instance().SetKeyValue(cfg::cat::FRAME, Settings::category::frame::key::WIDTH, 10);
		ConfigDataManager::Instance().SetKeyValue(cfg::cat::FRAME, Settings::category::frame::key::HEIGHT, 20);
		CPPUNIT_ASSERT(CRect(12, 23, 22, 43) == CMyCashflowApp::InitialFrameRect());
		}

	class FakeDataManager : public fake::ConfigDataManager
		{
		public:
		CString myKeyValue; 
		virtual void SetKeyValue(const CString & category, const CString & key, const TCHAR * value) { myKeyValue = value; }
		virtual CString GetKeyValue(const CString & category, const CString & key, const TCHAR * default_value) const { return myKeyValue.IsEmpty() ? default_value : myKeyValue; }	
		};

	class FakeDirectory : public IDirectory
		{ 
		public:
			CString myName;
			FakeDirectory(CString const & name) : myName(name) { }
			bool ExistsResult, IsReadOnlyResult;
			virtual bool Exists() const { return ExistsResult; }
			virtual bool IsReadOnly() const { return IsReadOnlyResult; }
			virtual TCHAR const * Name() const { return myName; }
		};

	void Test_InitializeStorageDir()
		{
		CMyCashflowApp::the_storage_dir = "the default";
		FakeDirectory directory("override");
		directory.ExistsResult = true;
		directory.IsReadOnlyResult = false;
		CPPUNIT_ASSERT_EQUAL(string("override"), CStringToString(CMyCashflowApp::InitializeStorageDir(directory)));
		directory.ExistsResult = true;
		directory.IsReadOnlyResult = true;
		CPPUNIT_ASSERT_EQUAL(string("the default"), CStringToString(CMyCashflowApp::InitializeStorageDir(directory)));
		directory.ExistsResult = false;
		directory.IsReadOnlyResult = true;
		CPPUNIT_ASSERT_EQUAL(string("the default"), CStringToString(CMyCashflowApp::InitializeStorageDir(directory)));
		}

		void test_Date_FromStr()
		{
			CPPUNIT_ASSERT(CDate(2000, 1, 31) == convert::TextToDate("01/31/2000"));
		}

		void test_DateToText()
		{
			CPPUNIT_ASSERT_EQUAL(string("11/28/2001"), CStringToString(convert::DateToText(CDate(2001, 11, 28))));
			CPPUNIT_ASSERT_EQUAL(string("01/02/2000"), CStringToString(convert::DateToText(CDate(2000, 1, 2))));
		}

	CPPUNIT_TEST_SUITE(CMyCashflowAppFixture);
		CPPUNIT_TEST(TestInitialFrameRect); 
		CPPUNIT_TEST(Test_InitializeStorageDir); 
		CPPUNIT_TEST(test_Date_FromStr);
		CPPUNIT_TEST(test_DateToText);
	CPPUNIT_TEST_SUITE_END();
	};

CPPUNIT_TEST_SUITE_REGISTRATION(CMyCashflowAppFixture);

#endif
