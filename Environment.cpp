#include "StdAfx.h"
#include "Environment.h"
#include "StdioFileEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



Environment::Environment(void)
{
	Init();
}


Environment::~Environment(void)
{
	
}

BOOL Environment::Init(LPCTSTR szIniFile)
{
	m_strSettingFile = szIniFile;
	CStdioFile file;
	TRY
	{
		CFileException ex;
		if(file.Open(m_strSettingFile, CFile::modeRead, &ex))
		{
			CString str;
			while(file.ReadString(str))
			{
				CString key, val;
				::ParseLine(str, key, val, _T('='));
				key.MakeUpper();

				ENV_ITEM item;
				const bool bResult = EnumString<ENV_ITEM>::To(item, key);
				m_items.insert(make_pair(item, val));
			}
			if(m_items.empty())
			{
				//AfxMessageBox(_T("설정이 비었습니다"));
				return FALSE;
			}
			return TRUE;
		}
		/*CString str;
		ex.GetErrorMessage(str.GetBuffer(256), 256);
		ErrMsgBox(_T("파일열기 실패 : %s"), szIniFile, str);*/
		return FALSE;
	}
	CATCH(CException, pEx)
	{
		TCHAR sz[256];
		pEx->GetErrorMessage(sz, sizeof(TCHAR) * 256);
		throw DKSimpleException(sz);
	}
	END_CATCH
	return TRUE;
}

void Environment::FlushSettings()
{
	CStdioFile file;
	if(file.Open(m_strSettingFile, CFile::modeCreate|CFile::modeReadWrite))
	{
		for(auto pos = m_items.begin() ; pos != m_items.end() ; ++pos)
		{
			const CString& strKey = EnumString<ENV_ITEM>::From(pos->first);
			CString str;
			str.Format(_T("%s=%s\n"), strKey, pos->second.ToString());
			file.WriteString(str);
		}
	}
}

BOOL Environment::Set(ENV_ITEM item, INT val, BOOL bFlush)
{
	return Set(item, DKStr2Val(val), bFlush);
}
BOOL Environment::Set(ENV_ITEM item, const CString& val, BOOL bFlush)
{
	return Set(item, DKStr2Val(val), bFlush);
}

BOOL Environment::Set(ENV_ITEM item, const DKStr2Val& val, BOOL bFlush)
{
	auto pos = m_items.find(item);
	if(pos != m_items.end())
	{
		if(pos->second == val)
			return FALSE;

		pos->second = val;
	}
	else
		m_items.insert(make_pair(item, val));

	if(bFlush)
		FlushSettings();

	return TRUE;
}

DKStr2Val Environment::Get(ENV_ITEM item)
{
	auto pos = m_items.find(item);
	if(pos != m_items.end())
	{
		return pos->second;
	}
	return DKStr2Val();
}


void Environment::Check(ENV_ITEM item, INT defVal, BOOL bFlush)
{
	auto pos = m_items.find(item);
	if(pos == m_items.end())
	{
		m_items.insert(make_pair(item, DKStr2Val(defVal)));
		if(bFlush)
			FlushSettings();
	}
}

void Environment::Check(ENV_ITEM item, const CString& defVal, BOOL bFlush)
{
	auto pos = m_items.find(item);
	if(pos == m_items.end())
	{
		m_items.insert(make_pair(item, DKStr2Val(defVal)));
		if(bFlush)
			FlushSettings();
	}
}

BOOL Environment::Init()
{
	TCHAR szPath[MAX_PATH];
	::GetModuleFileName(AfxGetInstanceHandle(), szPath, MAX_PATH);
	m_strRootDir = ::GetDirectoryFromFilePath(szPath);

	if(m_strRootDir.IsEmpty())
	{
		AfxMessageBox(_T("루트를 읽어올수가 없습니다!"));
		return FALSE;
	}

#ifdef _DEBUG
	ExpandEnvironmentStrings(_T("%VTS_SRC%"), szPath, MAX_PATH);
	CString strSrcPath = szPath;
	m_strImgRoot = strSrcPath + _T("install_shield_include\\img_TOPVALUE\\");
#else
	m_strImgRoot = m_strRootDir + _T("\\img\\");
#endif


	CString initPath = m_strRootDir + _T("\\settings\\VTSSetting.ini");

	BOOL bInitFileLoad = FALSE;
	try
	{
		bInitFileLoad = Init(initPath);
	}
	catch(DKSimpleException& ex)
	{
		AfxMessageBox(ex.GetMessage());
	}

	if(FALSE == bInitFileLoad)
	{
	#define _SETI(key, val) Check(key, val, FALSE);
	#define _SETS(key, val) Check(key, _T(#val), FALSE);

		_SETI(ENV_ORDER_CONFIRM_SHOW        , 0);    
		_SETI(ENV_TOOLTIP_SHOW              , 0);
		_SETI(ENV_CONTRACT_POPUP_SHOW       , 0);
		
		_SETI(ENV_HOGA_RESIZE_TYPE          , 0);
		
		_SETI(ENV_VOICE_BUY_ORDER           , 1);
		_SETI(ENV_VOICE_BUY_CANCEL          , 1);
		_SETI(ENV_VOICE_BUY_CORRECTION      , 1);
		_SETI(ENV_VOICE_BUY_CONTRACT        , 1);
						     
		_SETI(ENV_VOICE_SELL_ORDER          , 1);
		_SETI(ENV_VOICE_SELL_CANCEL         , 1);
		_SETI(ENV_VOICE_SELL_CORRECTION     , 1);
		_SETI(ENV_VOICE_SELL_CONTRACT       , 1);
						     
		_SETI(ENV_ACC_ORDER_ALL             , 1);
		_SETI(ENV_ACC_STOP_ORDER_CANCEL     , 1);
		_SETI(ENV_ACC_MIT_ORDER_CANCEL      , 1);
		_SETI(ENV_ACC_ALL_LIQUID            , 1);
		_SETI(ENV_ACC_SIJANGA_BUY           , 1);
		_SETI(ENV_ACC_SIJANGA_SELL          , 1);

		_SETS(ENV_SYSKEY_ORDER_ALL          , CTRL);
		_SETS(ENV_SYSKEY_STOP_ORDER_CANCEL  , CTRL);
		_SETS(ENV_SYSKEY_MIT_ORDER_CANCEL   , CTRL);
		_SETS(ENV_SYSKEY_ALL_LIQUID         , CTRL);
		_SETS(ENV_SYSKEY_IMMEDIATE_ORDER    , CTRL);
		_SETS(ENV_SYSKEY_REFRESH            , CTRL);
		_SETS(ENV_KEY_ORDER_ALL             , A);
		_SETS(ENV_KEY_STOP_ORDER_CANCEL     , B);
		_SETS(ENV_KEY_MIT_ORDER_CANCEL      , C);
		_SETS(ENV_KEY_ALL_LIQUID            , D);
		_SETS(ENV_KEY_IMMEDIATE_ORDER       , E);
		_SETS(ENV_KEY_REFRESH               , F);

		_SETS(ENV_VOL_1                     , 1);
		_SETS(ENV_VOL_2                     , 2);
		_SETS(ENV_VOL_3                     , 3);
		_SETS(ENV_VOL_4                     , 4);
		_SETS(ENV_VOL_5                     , 5);

		FlushSettings();
	}


	return TRUE;
}


CString Environment::GetPEMPath()
{
	return GetRoot() + _T("tv_file0001.dk");
}