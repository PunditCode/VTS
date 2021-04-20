#pragma once

#include <DKEnvironment.h>
#include <EnumString.h>

enum HogaSizeType
{
	hst_height_fix,
	hst_count_fix,
};

enum ENV_ITEM
{
	ENV_ORDER_CONFIRM_SHOW        ,    
	ENV_TOOLTIP_SHOW              ,
	ENV_CONTRACT_POPUP_SHOW       ,
						     
	ENV_HOGA_RESIZE_TYPE          ,

	ENV_VOICE_BUY_ORDER           ,
	ENV_VOICE_BUY_CANCEL          ,
	ENV_VOICE_BUY_CORRECTION      ,
	ENV_VOICE_BUY_CONTRACT        ,
						     
	ENV_VOICE_SELL_ORDER          ,
	ENV_VOICE_SELL_CANCEL         ,
	ENV_VOICE_SELL_CORRECTION     ,
	ENV_VOICE_SELL_CONTRACT       ,
						     
	ENV_ACC_ORDER_ALL             ,
	ENV_ACC_STOP_ORDER_CANCEL     ,
	ENV_ACC_MIT_ORDER_CANCEL      ,
	ENV_ACC_ALL_LIQUID            ,
	ENV_ACC_SIJANGA_BUY           ,
	ENV_ACC_SIJANGA_SELL          ,
						     
	ENV_SYSKEY_ORDER_ALL          ,
	ENV_SYSKEY_STOP_ORDER_CANCEL  ,
	ENV_SYSKEY_MIT_ORDER_CANCEL   ,
	ENV_SYSKEY_ALL_LIQUID         ,
	ENV_SYSKEY_IMMEDIATE_ORDER    ,
	ENV_SYSKEY_REFRESH            ,
	ENV_KEY_ORDER_ALL             ,
	ENV_KEY_STOP_ORDER_CANCEL     ,
	ENV_KEY_MIT_ORDER_CANCEL      ,
	ENV_KEY_ALL_LIQUID            ,
	ENV_KEY_IMMEDIATE_ORDER       ,
	ENV_KEY_REFRESH               ,

	ENV_VOL_1                     ,
	ENV_VOL_2                     ,
	ENV_VOL_3                     ,
	ENV_VOL_4                     ,
	ENV_VOL_5                     ,
};

Begin_Enum_String(ENV_ITEM)
{
	Enum_String(ENV_ORDER_CONFIRM_SHOW        );    
	Enum_String(ENV_TOOLTIP_SHOW              );
	Enum_String(ENV_CONTRACT_POPUP_SHOW       );
						     
	Enum_String(ENV_HOGA_RESIZE_TYPE          );

	Enum_String(ENV_VOICE_BUY_ORDER           );
	Enum_String(ENV_VOICE_BUY_CANCEL          );
	Enum_String(ENV_VOICE_BUY_CORRECTION      );
	Enum_String(ENV_VOICE_BUY_CONTRACT        );
						     
	Enum_String(ENV_VOICE_SELL_ORDER          );
	Enum_String(ENV_VOICE_SELL_CANCEL         );
	Enum_String(ENV_VOICE_SELL_CORRECTION     );
	Enum_String(ENV_VOICE_SELL_CONTRACT       );
						     
	Enum_String(ENV_ACC_ORDER_ALL             );
	Enum_String(ENV_ACC_STOP_ORDER_CANCEL     );
	Enum_String(ENV_ACC_MIT_ORDER_CANCEL      );
	Enum_String(ENV_ACC_ALL_LIQUID            );
	Enum_String(ENV_ACC_SIJANGA_BUY           );
	Enum_String(ENV_ACC_SIJANGA_SELL          );
	
	Enum_String(ENV_SYSKEY_ORDER_ALL          );
	Enum_String(ENV_SYSKEY_STOP_ORDER_CANCEL  );
	Enum_String(ENV_SYSKEY_MIT_ORDER_CANCEL   );
	Enum_String(ENV_SYSKEY_ALL_LIQUID         );
	Enum_String(ENV_SYSKEY_IMMEDIATE_ORDER    );
	Enum_String(ENV_SYSKEY_REFRESH            );
	Enum_String(ENV_KEY_ORDER_ALL             );
	Enum_String(ENV_KEY_STOP_ORDER_CANCEL     );
	Enum_String(ENV_KEY_MIT_ORDER_CANCEL      );
	Enum_String(ENV_KEY_ALL_LIQUID            );
	Enum_String(ENV_KEY_IMMEDIATE_ORDER       );
	Enum_String(ENV_KEY_REFRESH               );

	Enum_String(ENV_VOL_1                     );
	Enum_String(ENV_VOL_2                     );
	Enum_String(ENV_VOL_3                     );
	Enum_String(ENV_VOL_4                     );
	Enum_String(ENV_VOL_5                     );
}
End_Enum_String;


class Environment
{
public:
	
	Environment(void);
	~Environment(void);
	
	BOOL Set(ENV_ITEM item, INT val, BOOL bFlush = TRUE);
	BOOL Set(ENV_ITEM item, const CString& val, BOOL bFlush = TRUE);
	BOOL Set(ENV_ITEM item, const DKStr2Val& val, BOOL bFlush = TRUE);
	DKStr2Val Get(ENV_ITEM item);
	
	void FlushSettings();
	const CString& GetRoot() { return m_strRootDir; }
	const CString& GetImgRoot() { return m_strImgRoot; }
	CString GetPEMPath();
private:
	

	BOOL Init();
	BOOL Init(LPCTSTR szIniFile);

	void Check(ENV_ITEM item, INT defVal = 0, BOOL bFlush = TRUE);
	void Check(ENV_ITEM item, const CString& defVal = _T(""), BOOL bFlush = TRUE);

	CString		m_strRootDir;
	CString		m_strImgRoot;
	typedef std::hash_map<ENV_ITEM, DKStr2Val> MyItems;
	MyItems			m_items;
	CString			m_strSettingFile;
	
};


inline Environment& ENV() 
{ 
	return SingletonHolder<Environment, CreateUsingNew, DeletableSingleton>::Instance(); 
}



