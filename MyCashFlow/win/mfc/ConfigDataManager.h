// Copyright (C) 2006 by Wesley Steiner. All rights reserved.

#pragma once

#include "IConfigDataManager.h"

class IConfigDataStorage
	{
	public:
	virtual void WriteData(const CString & category, const CString & key, const TCHAR * value) const = 0;
	virtual void WriteData(const CString & category, const CString & key, int value) const = 0;
	};

namespace app {

class ConfigDataManager : utility_declare_no_copy, public IConfigDataManager
	{
	public:
	static ConfigDataManager & Instance();

	virtual void SetKeyValue(const CString & category, const CString & key, const TCHAR * value);
	virtual void SetKeyValue(const CString & category, const CString & key, int value);
	virtual CString GetKeyValue(const CString & category, const CString & key, const TCHAR * default_value) const;
	virtual int GetKeyValue(const CString & category, const CString & key, int default_value) const;

	test_private:
	ConfigDataManager(IConfigDataStorage & rStorage);
	const IConfigDataStorage & Storage() const { return m_rStorage; }

	private:
	IConfigDataStorage & m_rStorage;

	#ifdef TEST
	public:
	void SetStorage(IConfigDataStorage & rStorage) { m_rStorage = rStorage; }
	#endif
	};

} // app

#ifdef TEST

#include <numeric/Conversion.h>
#include <win/mfc.h>

using namespace std;

namespace fake 
	{
	class ConfigDataStorage : public IConfigDataStorage
		{
		public:
		mutable MethodInfo WriteDataInfo;
		mutable CString m_aLastCategory;
		mutable CString m_aLastKey;
		mutable CString m_aLastValue;
		virtual void WriteData(const CString & category, const CString & key, const TCHAR * value) const 
			{ 
			WriteDataInfo.Enter(); 
			m_aLastCategory = category;
			m_aLastKey = key;
			m_aLastValue = value;
			}
		virtual void WriteData(const CString & category, const CString & key, int value) const
			{
			}
		};
	
	class TestConfigDataManager : public IConfigDataManager
		{
		public:
		virtual void SetKeyValue(const CString & category, const CString & key, int value) 
		{ 
			SetKeyValue(category, key, numeric::ToString(value).c_str()); 
		}
		
		virtual void SetKeyValue(const CString & category, const CString & key, const TCHAR * value) 
		{ 
			myMap[category + key] = value; 
		}
		
		virtual CString GetKeyValue(const CString & category, const CString & key, const TCHAR * default_value) const 
		{ 
			CString value;
			if (myMap.Lookup(category + key, value))
				return value;
			else
				return default_value;
		}	

		virtual int GetKeyValue(const CString & category, const CString & key, int default_value) const 
		{ 
			return numeric::StringToInt(win::mfc::CStringToString(GetKeyValue(category, key, numeric::ToString(default_value).c_str()))); 
		}

		mutable CMapStringToString myMap;
 		};
	}

#endif
