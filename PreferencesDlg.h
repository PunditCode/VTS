#pragma once

#include "resource.h"
#include "VTSFrmWndChildBase.h"

// CPreferencesDlg 대화 상자입니다.
namespace PFDLGID
{
enum 
{
	C_ORDER_CONFIRM_SHOW       ,    
	C_TOOLTIP_SHOW             ,
	C_CONTRACT_POPUP_SHOW      ,

	C_VOICE_BUY_ORDER          ,
	C_VOICE_BUY_CANCEL         ,
	C_VOICE_BUY_CORRECTION     ,
	C_VOICE_BUY_CONTRACT       ,

	C_VOICE_SELL_ORDER         ,
	C_VOICE_SELL_CANCEL        ,
	C_VOICE_SELL_CORRECTION    ,
	C_VOICE_SELL_CONTRACT      ,

	C_ACC_ORDER_ALL            ,
	C_ACC_STOP_ORDER_CANCEL    ,
	C_ACC_MIT_ORDER_CANCEL     ,
	C_ACC_ALL_LIQUID           ,
	C_ACC_SIJANGA_BUY      ,
	C_ACC_SIJANGA_SELL              ,
	C_COUNT
};

enum
{
	CB_SYSKEY_ORDER_ALL          ,
	CB_SYSKEY_STOP_ORDER_CANCEL  ,
	CB_SYSKEY_MIT_ORDER_CANCEL   ,
	CB_SYSKEY_ALL_LIQUID         ,
	CB_SYSKEY_IMMEDIATE_ORDER    ,
	CB_SYSKEY_REFRESH            ,
	CB_KEY_ORDER_ALL             ,
	CB_KEY_STOP_ORDER_CANCEL     ,
	CB_KEY_MIT_ORDER_CANCEL      ,
	CB_KEY_ALL_LIQUID            ,
	CB_KEY_IMMEDIATE_ORDER       ,
	CB_KEY_REFRESH               ,
	CB_COUNT
};

enum
{
	B_VOL_1             ,
	B_VOL_2             ,
	B_VOL_3             ,
	B_VOL_4             ,
	B_VOL_5             ,
	B_APPLY             ,
	B_CLOSE             ,
	B_COUNT
};

enum
{
	E_VOL_1             ,
	E_VOL_2             ,
	E_VOL_3             ,
	E_VOL_4             ,
	E_VOL_5             ,
	E_COUNT
};

enum
{
	G_1             ,
	G_2             ,
	G_3             ,
	G_4             ,
	G_COUNT
};
}; // namespace PFDLGID


class CPreferencesDlg : public CExtNCW<CExtResizableDialog>
{
	DECLARE_DYNAMIC(CPreferencesDlg)

public:
	CPreferencesDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CPreferencesDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_PREFERENCES };


	CExtCheckBox		m_checks[PFDLGID::C_COUNT];
	CExtComboBox		m_combos[PFDLGID::CB_COUNT];
	CExtButton			m_buttons[PFDLGID::B_COUNT];
	CExtEdit			m_edits[PFDLGID::E_COUNT];
	CExtGroupBox		m_groups[PFDLGID::G_COUNT];

	std::map<CString, INT>	m_accKeys;
	std::set<pair<CString, CString> > m_usedKeys;
protected:
	void Set();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonClose();
};
