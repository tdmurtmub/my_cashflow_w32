// Copyright (C) 2006 by Wesley Steiner. All rights reserved.

#pragma once

class IConfigDataManager
	{ 
	public:
	virtual void SetKeyValue(const CString & category, const CString & key, const TCHAR * value) = 0;
	virtual void SetKeyValue(const CString & category, const CString & key, int value) = 0;
	virtual CString GetKeyValue(const CString & category, const CString & key, const TCHAR * default_value) const = 0;
	virtual int GetKeyValue(const CString & category, const CString & key, int default_value) const = 0;
	};

#ifdef TEST

namespace fake 
	{
	class ConfigDataManager : public IConfigDataManager
		{
		public:
		virtual void SetKeyValue(const CString & category, const CString & key, int value) { }
		virtual void SetKeyValue(const CString & category, const CString & key, const TCHAR * value) { }
		virtual CString GetKeyValue(const CString & category, const CString & key, const TCHAR * default_value) const { return CString(); }	
		virtual int GetKeyValue(const CString & category, const CString & key, int default_value) const { return 0; }
 		};
	}

#endif
