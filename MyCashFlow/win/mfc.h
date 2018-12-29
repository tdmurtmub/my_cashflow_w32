// (C) 2004 Wesley Steiner

#ifndef _MODULE_WIN_MFC
#define _MODULE_WIN_MFC

#include <win\mfc\mfcext.h>
#include <win\mfc\wndinfo.h>
#include <utility.h>

#ifndef WINCE
#include <string>
#endif

namespace win 
	{ 
	interface IDirectory
		{
		virtual bool Exists() const = 0;
		virtual bool IsReadOnly() const = 0;
		virtual TCHAR const * Name() const = 0;
		};

	namespace mfc 
		{ 
		inline CString ToCString(int value)
		{
				CString cs; 
				cs.Format(_T("%d"), value);
				return cs;
		}

		class SelectorBase : DECLARE_NO_COPY
			{
			protected:
			CDC & myDC;
			SelectorBase(CDC & dc) : myDC(dc) { }
			};

		class FontSelector : protected SelectorBase 
			{
			CFont * myPreviousObject;
			public:
			FontSelector(CDC & dc, CFont * aFont) : SelectorBase(dc) { myPreviousObject = myDC.SelectObject(aFont); }
			~FontSelector() { myDC.SelectObject(myPreviousObject); }
			};

		class PenSelector : protected SelectorBase 
			{
			CPen * myPreviousObject;
			public:
			PenSelector(CDC & dc, CPen * aPen) : SelectorBase(dc) { myPreviousObject = myDC.SelectObject(aPen); }
			~PenSelector() { myDC.SelectObject(myPreviousObject); }
			};

		#ifndef WINCE
		inline std::string CStringToString(const CString & aString) { return std::string((LPCTSTR) aString); }
		#endif

		class Directory : public IDirectory
			{ 
			public:
			Directory(CString const & name) :myName(name) 
				{ 
//				myExists = myFinder.FindFile(myName) && !myFinder.FindNextFile() && myFinder.IsDirectory(); 
				myExists = myFinder.FindFile(myName) && !myFinder.FindNextFile() && (GetFileAttributes(myName) & FILE_ATTRIBUTE_DIRECTORY); 
				}
			virtual bool Exists() const { return myExists; }
			virtual bool IsReadOnly() const { return Exists() && (GetFileAttributes(myName) & FILE_ATTRIBUTE_READONLY); } //myFinder.IsReadOnly(); }
			virtual TCHAR const * Name() const { return myName; }
			private:
			CString myName;
			bool myExists;
			mutable CFileFind myFinder;
			};

		class CWndWrapper : public CWndInfo
			{
			public:
			CWndWrapper(CWnd& wnd) : CWndInfo(wnd) { }
			CWndWrapper(CWnd* wnd) : CWndInfo(wnd) { }
			};

		namespace dlg {
			static const BOOL XFER_CONTROLS_TO_MEMBERS = FROM_CONTROLS_TO_MEMBERS;
			static const BOOL XFER_MEMBERS_TO_CONTROLS = FROM_MEMBERS_TO_CONTROLS;
			}
		}
	}

#endif
