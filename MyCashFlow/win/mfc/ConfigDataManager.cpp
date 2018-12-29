// (C) 2006 Wesley Steiner

#include "stdafx.h"

#include "ConfigDataManager.h"

//#define DEFAULT_INT_AS_TEXT ""

using namespace app;

class ConfigDataStorage : public IConfigDataStorage
	{
	public:
	virtual void WriteData(const CString & category, const CString & key, const TCHAR * value) const
		{ 
		AfxGetApp()->WriteProfileString(category, key, value);
		};
	virtual void WriteData(const CString & category, const CString & key, int value) const
		{
		AfxGetApp()->WriteProfileInt(category, key, value);
		}
	};

ConfigDataStorage s_aConfigDataStorage;

ConfigDataManager & ConfigDataManager::Instance()
	{
	static ConfigDataManager s_aConfigDataManager(s_aConfigDataStorage);
	return s_aConfigDataManager;
	}

void ConfigDataManager::SetKeyValue(const CString & category, const CString & key, const TCHAR * value)
	{
	ASSERT(category.GetLength() > 0);
	ASSERT(key.GetLength() > 0);
/*	CMapStringToString * pKeys;
	if (!m_aCategoryMap.Lookup(category, (CObject * &) pKeys))
		{
		pKeys = new CMapStringToString();
		}
	pKeys->SetAt(key, value);
	m_aCategoryMap[category] = pKeys;
*/	AfxGetApp()->WriteProfileString(category, key, value);
	}

void ConfigDataManager::SetKeyValue(const CString & category, const CString & key, int value)
	{
//	CString aIntAsStr;
//	aIntAsStr.Format("%d", value);
//	SetKeyValue(category, key, aIntAsStr);
	AfxGetApp()->WriteProfileInt(category, key, value);
	}

CString ConfigDataManager::GetKeyValue(const CString & category, const CString & key, const TCHAR * default_value) const
	{
	ASSERT(default_value != NULL);
/*	CMapStringToString * pKeys;
	CString value;
	if ((m_aCategoryMap.Lookup(category, (CObject * &) pKeys)) && (pKeys->Lookup(key, value))) return value;
	else return default_value;
*/	return AfxGetApp()->GetProfileString(category, key, default_value);
	}

int ConfigDataManager::GetKeyValue(const CString & category, const CString & key, int default_value) const
	{
/*	CString value = GetKeyValue(category, key, DEFAULT_INT_AS_TEXT);
	return  (value == DEFAULT_INT_AS_TEXT) ? default_value : Numeric::StringToInt((LPCTSTR) value);
*/	return AfxGetApp()->GetProfileInt(category, key, default_value);
	}

ConfigDataManager::ConfigDataManager(IConfigDataStorage & rStorage) : 
	m_rStorage(rStorage)
	{
	}
