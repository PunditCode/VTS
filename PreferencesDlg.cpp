// PreferencesDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PreferencesDlg.h"
#include "afxdialogex.h"
#include "VTS.h"

using namespace PFDLGID;
// CPreferencesDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPreferencesDlg, CExtNCW<CExtResizableDialog>)

CPreferencesDlg::CPreferencesDlg(CWnd* pParent /*=NULL*/)
	: CExtNCW<CExtResizableDialog>(CPreferencesDlg::IDD, pParent)
{

}

CPreferencesDlg::~CPreferencesDlg()
{
}

void CPreferencesDlg::DoDataExchange(CDataExchange* pDX)
{
	CExtNCW<CExtResizableDialog>::DoDataExchange(pDX);

#define _CHECK(n) DDX_Control(pDX, IDC_CHECK_##n, m_checks[C_##n]);

	_CHECK(ORDER_CONFIRM_SHOW       );    
	_CHECK(TOOLTIP_SHOW             );
	_CHECK(CONTRACT_POPUP_SHOW      );

	_CHECK(VOICE_BUY_ORDER          );
	_CHECK(VOICE_BUY_CANCEL         );
	_CHECK(VOICE_BUY_CORRECTION     );
	_CHECK(VOICE_BUY_CONTRACT       );

	_CHECK(VOICE_SELL_ORDER         );
	_CHECK(VOICE_SELL_CANCEL        );
	_CHECK(VOICE_SELL_CORRECTION    );
	_CHECK(VOICE_SELL_CONTRACT      );

	_CHECK(ACC_ORDER_ALL            );
	_CHECK(ACC_STOP_ORDER_CANCEL    );
	_CHECK(ACC_MIT_ORDER_CANCEL     );
	_CHECK(ACC_ALL_LIQUID           );
	_CHECK(ACC_SIJANGA_BUY      );
	_CHECK(ACC_SIJANGA_SELL              );

#define _COMBO(n) DDX_Control(pDX, IDC_COMBO_##n, m_combos[CB_##n]);
	_COMBO(SYSKEY_ORDER_ALL          );
	_COMBO(SYSKEY_STOP_ORDER_CANCEL  );
	_COMBO(SYSKEY_MIT_ORDER_CANCEL   );
	_COMBO(SYSKEY_ALL_LIQUID         );
	_COMBO(SYSKEY_IMMEDIATE_ORDER    );
	_COMBO(SYSKEY_REFRESH            );
	_COMBO(KEY_ORDER_ALL             );
	_COMBO(KEY_STOP_ORDER_CANCEL     );
	_COMBO(KEY_MIT_ORDER_CANCEL      );
	_COMBO(KEY_ALL_LIQUID            );
	_COMBO(KEY_IMMEDIATE_ORDER       );
	_COMBO(KEY_REFRESH               );

#define _BUTTON(n) DDX_Control(pDX, IDC_BUTTON_##n, m_buttons[B_##n]);

	_BUTTON(VOL_1             );
	_BUTTON(VOL_2             );
	_BUTTON(VOL_3             );
	_BUTTON(VOL_4             );
	_BUTTON(VOL_5             );
	_BUTTON(APPLY             );
//	_BUTTON(CLOSE             );

#define _EDIT(n) DDX_Control(pDX, IDC_EDIT_##n, m_edits[E_##n]);

	_EDIT(VOL_1             );
	_EDIT(VOL_2             );
	_EDIT(VOL_3             );
	_EDIT(VOL_4             );
	_EDIT(VOL_5             );

#define _GROUP(n) DDX_Control(pDX, IDC_STATIC_GROUP_##n, m_groups[G_##n]);
	_GROUP(1             );
	_GROUP(2             );
	_GROUP(3             );
	_GROUP(4             );
}


BEGIN_MESSAGE_MAP(CPreferencesDlg, CExtNCW<CExtResizableDialog>)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CPreferencesDlg::OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CPreferencesDlg::OnBnClickedButtonClose)
END_MESSAGE_MAP()


// CPreferencesDlg 메시지 처리기입니다.


BOOL CPreferencesDlg::OnInitDialog()
{
	CExtNCW<CExtResizableDialog>::OnInitDialog();
	SetIcon( AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	for(int i = 0 ; i < G_COUNT ; ++i)
		m_groups[i].SetStyle(CExtGroupBox::STYLE_CAPTION);
		
	CString s;

	int nIndex = 0;
	for(TCHAR nChar = _T('0') ; nChar <= _T('9') ; ++nChar, ++nIndex)
	{
		s.Format(_T("%c"), nChar);
		m_accKeys.insert(make_pair(s, nIndex));
	}
	for(TCHAR nChar = _T('A') ; nChar <= _T('Z') ; ++nChar, ++nIndex)
	{
		s.Format(_T("%c"), nChar);
		m_accKeys.insert(make_pair(s, nIndex));
	}

	for(int i = CB_KEY_ORDER_ALL ; i <= CB_KEY_REFRESH ; ++i)
	{
		for(auto pos = m_accKeys.begin() ; pos != m_accKeys.end() ; ++pos)
		{
			m_combos[i].InsertString(pos->second, pos->first);
		}
	}
	Set();

	WINDOWPLACEMENT wp;
	GetDlgItem(IDC_BUTTON_APPLY)->GetWindowPlacement(&wp);
	
	CRect rtWnd;
	GetWindowRect(rtWnd);

	CRect rtClient;
	GetClientRect(rtClient);

	rtWnd.bottom = rtWnd.top + wp.rcNormalPosition.bottom + rtWnd.Height() - rtClient.Height() + 5;
	SetWindowPos(NULL, 0, 0, rtWnd.Width(), rtWnd.Height(), SWP_NOMOVE | SWP_NOZORDER );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CPreferencesDlg::Set()
{
	m_usedKeys.clear();

	CString str, strSyskey;
	DKStr2Val val, valSyskey;

#define _CHECKSET(env, mem) val = ENV().Get(env); m_checks[C_##mem].SetCheck(val.ToINT() == 1 ? BST_CHECKED : BST_UNCHECKED);
#define _COMBOSET(env, mem) val = ENV().Get(env); m_combos[CB_##mem].SetCurSel(m_combos[CB_##mem].FindItemExact(val.ToString()));
#define _CBKEYSET(envSys, syskey, envKey, key) \
	valSyskey = ENV().Get(envSys); \
	m_combos[CB_##syskey].SetCurSel(m_combos[CB_##syskey].FindItemExact(valSyskey.ToString())); \
	val = ENV().Get(envKey); \
	if(m_usedKeys.find(make_pair(valSyskey, val.ToString())) == m_usedKeys.end())\
	{\
		m_usedKeys.insert(make_pair(valSyskey.ToString(), val.ToString()));\
		m_combos[CB_##key].SetCurSel(m_combos[CB_##key].FindItemExact(val.ToString()));\
	}
#define __EDITSET(env, mem) val = ENV().Get(env); m_edits[E_##mem].SetWindowText(val.ToString()); m_buttons[B_##mem].SetWindowText(val.ToString());

	_CHECKSET(ENV_ORDER_CONFIRM_SHOW        , ORDER_CONFIRM_SHOW        );    
	_CHECKSET(ENV_TOOLTIP_SHOW              , TOOLTIP_SHOW              );
	_CHECKSET(ENV_CONTRACT_POPUP_SHOW       , CONTRACT_POPUP_SHOW       );
	
		
	_CHECKSET(ENV_VOICE_BUY_ORDER           , VOICE_BUY_ORDER           );
	_CHECKSET(ENV_VOICE_BUY_CANCEL          , VOICE_BUY_CANCEL          );
	_CHECKSET(ENV_VOICE_BUY_CORRECTION      , VOICE_BUY_CORRECTION      );
	_CHECKSET(ENV_VOICE_BUY_CONTRACT        , VOICE_BUY_CONTRACT        );
						     			  		     
	_CHECKSET(ENV_VOICE_SELL_ORDER          , VOICE_SELL_ORDER          );
	_CHECKSET(ENV_VOICE_SELL_CANCEL         , VOICE_SELL_CANCEL         );
	_CHECKSET(ENV_VOICE_SELL_CORRECTION     , VOICE_SELL_CORRECTION     );
	_CHECKSET(ENV_VOICE_SELL_CONTRACT       , VOICE_SELL_CONTRACT       );
						     			  		     
	_CHECKSET(ENV_ACC_ORDER_ALL             , ACC_ORDER_ALL             );
	_CHECKSET(ENV_ACC_STOP_ORDER_CANCEL     , ACC_STOP_ORDER_CANCEL     );
	_CHECKSET(ENV_ACC_MIT_ORDER_CANCEL      , ACC_MIT_ORDER_CANCEL      );
	_CHECKSET(ENV_ACC_ALL_LIQUID            , ACC_ALL_LIQUID            );
	_CHECKSET(ENV_ACC_SIJANGA_BUY       , ACC_SIJANGA_BUY       );
	_CHECKSET(ENV_ACC_SIJANGA_SELL               , ACC_SIJANGA_SELL               );

	_CBKEYSET(ENV_SYSKEY_ORDER_ALL          , SYSKEY_ORDER_ALL          , ENV_KEY_ORDER_ALL             , KEY_ORDER_ALL             );
	_CBKEYSET(ENV_SYSKEY_STOP_ORDER_CANCEL  , SYSKEY_STOP_ORDER_CANCEL  , ENV_KEY_STOP_ORDER_CANCEL     , KEY_STOP_ORDER_CANCEL     );
	_CBKEYSET(ENV_SYSKEY_MIT_ORDER_CANCEL   , SYSKEY_MIT_ORDER_CANCEL   , ENV_KEY_MIT_ORDER_CANCEL      , KEY_MIT_ORDER_CANCEL      );
	_CBKEYSET(ENV_SYSKEY_ALL_LIQUID         , SYSKEY_ALL_LIQUID         , ENV_KEY_ALL_LIQUID            , KEY_ALL_LIQUID            );
	_CBKEYSET(ENV_SYSKEY_IMMEDIATE_ORDER    , SYSKEY_IMMEDIATE_ORDER    , ENV_KEY_IMMEDIATE_ORDER       , KEY_IMMEDIATE_ORDER       );
	_CBKEYSET(ENV_SYSKEY_REFRESH            , SYSKEY_REFRESH            , ENV_KEY_REFRESH               , KEY_REFRESH               );

	__EDITSET(ENV_VOL_1                     , VOL_1                     );
	__EDITSET(ENV_VOL_2                     , VOL_2                     );
	__EDITSET(ENV_VOL_3                     , VOL_3                     );
	__EDITSET(ENV_VOL_4                     , VOL_4                     );
	__EDITSET(ENV_VOL_5                     , VOL_5                     );

	
}


void CPreferencesDlg::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	int nTemp;
	CString str, strSyskey;
	BOOL bChanged = FALSE;

#define _CHECKSET(env, mem) nTemp = m_checks[C_##mem].GetCheck(); nTemp = nTemp == BST_CHECKED ? 1: 0; bChanged |= ENV().Set(env, nTemp, FALSE) ; 
#define _COMBOSET(env, mem) m_combos[CB_##mem].GetWindowText(str); bChanged |= ENV().Set(env, str, FALSE);
#define _CBKEYSET(envSys, syskey, envKey, key) m_combos[CB_##syskey].GetWindowText(str); bChanged |= ENV().Set(envSys, str, FALSE);m_combos[CB_##key].GetWindowText(str); bChanged |= ENV().Set(envKey, str, FALSE);
#define __EDITSET(env, mem) m_edits[E_##mem].GetWindowText(str); bChanged |= ENV().Set(env, str, FALSE); m_buttons[B_##mem].SetWindowText(str);

	_CHECKSET(ENV_ORDER_CONFIRM_SHOW        , ORDER_CONFIRM_SHOW        );    
	_CHECKSET(ENV_TOOLTIP_SHOW              , TOOLTIP_SHOW              );
	_CHECKSET(ENV_CONTRACT_POPUP_SHOW       , CONTRACT_POPUP_SHOW       );
			
	_CHECKSET(ENV_VOICE_BUY_ORDER           , VOICE_BUY_ORDER           );
	_CHECKSET(ENV_VOICE_BUY_CANCEL          , VOICE_BUY_CANCEL          );
	_CHECKSET(ENV_VOICE_BUY_CORRECTION      , VOICE_BUY_CORRECTION      );
	_CHECKSET(ENV_VOICE_BUY_CONTRACT        , VOICE_BUY_CONTRACT        );
						     			  		     
	_CHECKSET(ENV_VOICE_SELL_ORDER          , VOICE_SELL_ORDER          );
	_CHECKSET(ENV_VOICE_SELL_CANCEL         , VOICE_SELL_CANCEL         );
	_CHECKSET(ENV_VOICE_SELL_CORRECTION     , VOICE_SELL_CORRECTION     );
	_CHECKSET(ENV_VOICE_SELL_CONTRACT       , VOICE_SELL_CONTRACT       );
						     			  		     
	_CHECKSET(ENV_ACC_ORDER_ALL             , ACC_ORDER_ALL             );
	_CHECKSET(ENV_ACC_STOP_ORDER_CANCEL     , ACC_STOP_ORDER_CANCEL     );
	_CHECKSET(ENV_ACC_MIT_ORDER_CANCEL      , ACC_MIT_ORDER_CANCEL      );
	_CHECKSET(ENV_ACC_ALL_LIQUID            , ACC_ALL_LIQUID            );
	_CHECKSET(ENV_ACC_SIJANGA_BUY       , ACC_SIJANGA_BUY       );
	_CHECKSET(ENV_ACC_SIJANGA_SELL               , ACC_SIJANGA_SELL               );

	_CBKEYSET(ENV_SYSKEY_ORDER_ALL          , SYSKEY_ORDER_ALL          , ENV_KEY_ORDER_ALL             , KEY_ORDER_ALL             );
	_CBKEYSET(ENV_SYSKEY_STOP_ORDER_CANCEL  , SYSKEY_STOP_ORDER_CANCEL  , ENV_KEY_STOP_ORDER_CANCEL     , KEY_STOP_ORDER_CANCEL     );
	_CBKEYSET(ENV_SYSKEY_MIT_ORDER_CANCEL   , SYSKEY_MIT_ORDER_CANCEL   , ENV_KEY_MIT_ORDER_CANCEL      , KEY_MIT_ORDER_CANCEL      );
	_CBKEYSET(ENV_SYSKEY_ALL_LIQUID         , SYSKEY_ALL_LIQUID         , ENV_KEY_ALL_LIQUID            , KEY_ALL_LIQUID            );
	_CBKEYSET(ENV_SYSKEY_IMMEDIATE_ORDER    , SYSKEY_IMMEDIATE_ORDER    , ENV_KEY_IMMEDIATE_ORDER       , KEY_IMMEDIATE_ORDER       );
	_CBKEYSET(ENV_SYSKEY_REFRESH            , SYSKEY_REFRESH            , ENV_KEY_REFRESH               , KEY_REFRESH               );

	__EDITSET(ENV_VOL_1                     , VOL_1                     );
	__EDITSET(ENV_VOL_2                     , VOL_2                     );
	__EDITSET(ENV_VOL_3                     , VOL_3                     );
	__EDITSET(ENV_VOL_4                     , VOL_4                     );
	__EDITSET(ENV_VOL_5                     , VOL_5                     );

	if(bChanged)
	{
		ENV().FlushSettings();
		theApp.m_pMainWnd->PostMessage(WM_PREFERENCES_CHANGED);
	}
	EndDialog(IDOK);
}


void CPreferencesDlg::OnBnClickedButtonClose()
{
	EndDialog(IDCANCEL);
}

