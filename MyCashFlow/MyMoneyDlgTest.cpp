// (C) 2006 Wesley Steiner

#include "stdafx.h"

#ifdef TEST

#include "MyMoneyDlg.h"
#include "mfc\ConfigDataManager.h"
#include <cppunit\extensions\HelperMacros.h>

using namespace app;

class CMyCashflowDlgTestFixture : public CppUnit::TestFixture 
	{
	struct CMyCashflowDlgTester : CMyCashflowDlg
		{
		bool file_was_saved;
		mutable UINT messagebox_flags;
		fake::TestConfigDataManager configuration;
		CMyCashflowDlgTester() : CMyCashflowDlg(configuration, 2) { }
		override void Save() { file_was_saved = true; }

		override int AFXAPI CallAfxMessageBox(UINT nIDPrompt, UINT nType = MB_OK, UINT nIDHelp = (UINT) -1) const 
			{ 
			messagebox_flags = nType; return -1245; 
			}
		};

	struct ConditionalSaveTester : CMyCashflowDlgTester
		{
		bool pretend_IsModified;
		bool pretend_AutoSave;
		int pretend_AskToSave;
		override int AskToSave() const { return this->pretend_AskToSave; }
		override bool IsModified() const { return this->pretend_IsModified; }
		override bool AutoSave() const { return this->pretend_AutoSave; }
		};
		
	void Test_ConditionalSave()
		{
		ConditionalSaveTester dialog;
		dialog.pretend_AutoSave = false;
		dialog.file_was_saved = false;
		dialog.pretend_IsModified = true;
		dialog.pretend_AskToSave = IDYES;
		CPPUNIT_ASSERT(dialog.ConditionalSave());
		CPPUNIT_ASSERT(dialog.file_was_saved);
		dialog.file_was_saved = false;
		dialog.pretend_IsModified = false;
		dialog.pretend_AskToSave = IDYES;
		CPPUNIT_ASSERT(dialog.ConditionalSave());
		CPPUNIT_ASSERT(!dialog.file_was_saved);
		dialog.file_was_saved = false;
		dialog.pretend_IsModified = true;
		dialog.pretend_AskToSave = IDNO;
		CPPUNIT_ASSERT(dialog.ConditionalSave());
		CPPUNIT_ASSERT(!dialog.file_was_saved);
		dialog.file_was_saved = false;
		dialog.pretend_IsModified = false;
		dialog.pretend_AskToSave = IDNO;
		CPPUNIT_ASSERT(dialog.ConditionalSave());
		CPPUNIT_ASSERT(!dialog.file_was_saved);
		dialog.file_was_saved = false;
		dialog.pretend_IsModified = true;
		dialog.pretend_AskToSave = IDCANCEL;
		CPPUNIT_ASSERT(!dialog.ConditionalSave());
		CPPUNIT_ASSERT(!dialog.file_was_saved);
		dialog.file_was_saved = false;
		dialog.pretend_AutoSave = true;
		dialog.pretend_IsModified = true;
		dialog.ConditionalSave();
		CPPUNIT_ASSERT(dialog.file_was_saved);
		}

	struct IsModifiedTester : CMyCashflowDlgTester
		{
		CMarkup current_markup;
		override CMarkup GetMarkup() const { return current_markup; }
		};
		
	void Test_IsModified()
		{
		IsModifiedTester dialog;
		dialog.last_saved_state = CMarkup();
		dialog.last_saved_state.AddElem(xml::HEAD);
		dialog.current_markup = CMarkup();
		dialog.current_markup.AddElem(xml::HEAD);
		CPPUNIT_ASSERT(dialog.IsModified() == false);
		dialog.current_markup.AddAttrib("FOO", "BAR");
		CPPUNIT_ASSERT(dialog.IsModified() == true);
		}

	struct OnFileExitTester : public CMyCashflowDlgTester
		{
		bool EndDialog_was_called;
		bool pretend_ConditionalSave;
		override bool ConditionalSave() { return pretend_ConditionalSave; }
		override void CallEndDialog(int nEndID) { EndDialog_was_called = true; }
		};

	void Test_OnFileExit_respects_cancel()
		{
		OnFileExitTester dialog;
		dialog.EndDialog_was_called = false;
		dialog.pretend_ConditionalSave = true;
		dialog.OnFileExit();
		CPPUNIT_ASSERT(dialog.EndDialog_was_called);
		dialog.EndDialog_was_called = false;
		dialog.pretend_ConditionalSave = false;
		dialog.OnFileExit();
		CPPUNIT_ASSERT(!dialog.EndDialog_was_called);
		}
		
	void Test_OnFileAutosaveonexit()
		{
		CMyCashflowDlgTester dialog;
		dialog.Create();
		CMenu * sub_menu = dialog.MainMenu().GetSubMenu(0);
		sub_menu->CheckMenuItem(ID_FILE_AUTOSAVEONEXIT, MF_BYCOMMAND | MF_CHECKED);
		dialog.OnFileAutosaveonexit();
		CPPUNIT_ASSERT_EQUAL((UINT) MF_UNCHECKED, sub_menu->GetMenuState(ID_FILE_AUTOSAVEONEXIT,  MF_BYCOMMAND));
		CPPUNIT_ASSERT_EQUAL(0, dialog.configuration.GetKeyValue(cfg::cat::ROOT, Settings::Root::Key::AUTOSAVE, 1));
		dialog.OnFileAutosaveonexit();
		CPPUNIT_ASSERT_EQUAL((UINT) MF_CHECKED, sub_menu->GetMenuState(ID_FILE_AUTOSAVEONEXIT,  MF_BYCOMMAND));
		CPPUNIT_ASSERT_EQUAL(1, dialog.configuration.GetKeyValue(cfg::cat::ROOT, Settings::Root::Key::AUTOSAVE, 0));
		}

	void Test_AutoSave()
		{
		CMyCashflowDlgTester dialog;
		dialog.Create();
		CMenu * sub_menu = dialog.MainMenu().GetSubMenu(0);
		sub_menu->CheckMenuItem(ID_FILE_AUTOSAVEONEXIT, MF_BYCOMMAND | MF_CHECKED);
		CPPUNIT_ASSERT(dialog.AutoSave());
		sub_menu->CheckMenuItem(ID_FILE_AUTOSAVEONEXIT, MF_BYCOMMAND | MF_UNCHECKED);
		CPPUNIT_ASSERT(!dialog.AutoSave());
		}
				
	CPPUNIT_TEST_SUITE(CMyCashflowDlgTestFixture);
		CPPUNIT_TEST(Test_ConditionalSave); 
		CPPUNIT_TEST(Test_IsModified); 
		CPPUNIT_TEST(Test_OnFileExit_respects_cancel);
		CPPUNIT_TEST(Test_OnFileAutosaveonexit);
		CPPUNIT_TEST(Test_AutoSave);
	CPPUNIT_TEST_SUITE_END();
	};

CPPUNIT_TEST_SUITE_REGISTRATION(CMyCashflowDlgTestFixture);

#endif
