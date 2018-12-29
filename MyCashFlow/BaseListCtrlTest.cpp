// (C) 2004 Wesley Steiner

#include "stdafx.h"

#ifdef TEST

#include "BaseListCtrlTest.h"

#include <win/mfc.h>
#include <cppunit\extensions\HelperMacros.h>

#include "BaseListCtrl.h"
#include "mfc\ConfigDataManager.h"

using namespace win::mfc;

namespace wm
	{
	class NotifyTesting : public CppUnit::TestFixture 
		{
		void TestCode()
			{
			NMHDR aNmHdr;
			aNmHdr.code = 123;
			Notify aNotify(0, (long) &aNmHdr, 0);
			CPPUNIT_ASSERT_EQUAL((UINT) 123, aNotify.Code());
			}

		CPPUNIT_TEST_SUITE(NotifyTesting);
			CPPUNIT_TEST(TestCode); 
		CPPUNIT_TEST_SUITE_END();
		};

	CPPUNIT_TEST_SUITE_REGISTRATION(NotifyTesting);
	}

class CBaseListCtrlTestFixture : public CppUnit::TestFixture 
	{
	void test_TryDeleteEntry ()
		{
		class CTester : public CBaseListCtrl
			{
			public: 
			CTester() : CBaseListCtrl(fake::TestConfigDataManager()) { }
			bool DeleteEntryWasCalled;
			virtual void DeleteEntry(UINT nIndx) { DeleteEntryWasCalled = true; } 

			virtual void SaveColumnWidth(int aColIndex) const { }
			virtual CString MySettingsCategory() const { return ""; }

			bool ConfirmDeleteEntryResult;
			virtual bool ConfirmDeleteEntry() const { return ConfirmDeleteEntryResult; }

			virtual UINT DefaultColumnWidth(UINT aColumnIndex) const { return 0; }
			};

		CTester aTester;

		aTester.ConfirmDeleteEntryResult = true;
		aTester.DeleteEntryWasCalled = false;
		aTester.TryDeleteEntry(0);
		CPPUNIT_ASSERT(aTester.DeleteEntryWasCalled);

		aTester.ConfirmDeleteEntryResult = false;
		aTester.DeleteEntryWasCalled = false;
		aTester.TryDeleteEntry(0);
		CPPUNIT_ASSERT(!aTester.DeleteEntryWasCalled);
		}

	void TestOnNotifyImpl()
		{
		class CTester : public CBaseListCtrl
			{
			public:
			CTester() : CBaseListCtrl(fake::TestConfigDataManager()) { }

			mutable fake::MethodInfo mSaveColumnWidthInfo;

			protected: 
			virtual void DeleteEntry(UINT nItem) { };
			virtual CString MySettingsCategory() const { return ""; }

			virtual void SaveColumnWidth(int aColIndex) const
				{
				mSaveColumnWidthInfo.Enter();
				}
			virtual UINT DefaultColumnWidth(UINT aColumnIndex) const { return 0; }
			};

		CTester aTester;
		NMHDR aNmHdr;
		aNmHdr.code = HDN_ENDTRACKW;
		wm::Notify aNotifyMsg(0, (long) &aNmHdr, 0);
		aTester.OnNotifyImpl(aNotifyMsg);
		CPPUNIT_ASSERT(aTester.mSaveColumnWidthInfo.WasCalled());
		}

	void TestGetColumnWidthRegKey()
		{
		CPPUNIT_ASSERT_EQUAL(string("ColumnWidth_7"), CStringToString(CBaseListCtrl::GetColumnWidthRegKey(7)));
		CPPUNIT_ASSERT_EQUAL(string("ColumnWidth_0"), CStringToString(CBaseListCtrl::GetColumnWidthRegKey(0)));		}

	void TestColumnWidth()
		{
		class CTester : public CBaseListCtrl
			{
			public:
			CTester(IConfigDataManager & rConfigDataManager) : CBaseListCtrl(rConfigDataManager) { }
			virtual void DeleteEntry(UINT nItem) { };
			virtual CString MySettingsCategory() const { return "CATEGORY"; }
			virtual UINT DefaultColumnWidth(UINT aColumnIndex) const { return 110; }
			};

		fake::TestConfigDataManager aFakeConfig;
		CTester aTester(aFakeConfig);
		aFakeConfig.SetKeyValue(aTester.MySettingsCategory(), aTester.GetColumnWidthRegKey(1), 826);
		CPPUNIT_ASSERT_EQUAL((UINT) 826, aTester.ColumnWidth(1));
		}

	void test_OnLButtonDblClk()
	{
		struct CTester : public CBaseListCtrl 
		{
			CTester () : CBaseListCtrl(fake::ConfigDataManager()) { }
			bool EditEntry_was_called;
			int GetSelectedItemIndex_result;
			virtual void EditEntry(UINT nIndex) { EditEntry_was_called = true; }
			virtual void DeleteEntry(UINT nIndex) { }
			virtual int GetSelectedItemIndex() const { return GetSelectedItemIndex_result; }
			virtual CString MySettingsCategory() const { return ""; }
			virtual UINT DefaultColumnWidth(UINT aColumnIndex) const { return 0; }
		};

		CTester aList;

		aList.GetSelectedItemIndex_result = -1;
		aList.EditEntry_was_called = false;
		aList.OnLButtonDblClk(MK_LBUTTON, CPoint(0, 0));
		CPPUNIT_ASSERT(!aList.EditEntry_was_called);

		aList.GetSelectedItemIndex_result = 0;
		aList.EditEntry_was_called = false;
		aList.OnLButtonDblClk(MK_LBUTTON, CPoint(0, 0));
		CPPUNIT_ASSERT(aList.EditEntry_was_called);
	}

	CPPUNIT_TEST_SUITE(CBaseListCtrlTestFixture);
		CPPUNIT_TEST(test_TryDeleteEntry);
		CPPUNIT_TEST(TestOnNotifyImpl); 
		CPPUNIT_TEST(TestGetColumnWidthRegKey); 
		CPPUNIT_TEST(TestColumnWidth); 
		CPPUNIT_TEST(test_OnLButtonDblClk);
	CPPUNIT_TEST_SUITE_END();
	};

CPPUNIT_TEST_SUITE_REGISTRATION(CBaseListCtrlTestFixture);

#endif

