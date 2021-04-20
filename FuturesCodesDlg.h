#pragma once

#include "resource.h"

// CFuturesCodesDlg 대화 상자입니다.
typedef CExtNCW < CExtResizableDialog >	CResizableDialogBase;

class CFuturesCodesDlg : public CResizableDialogBase
{
	DECLARE_DYNAMIC(CFuturesCodesDlg)

public:
	CFuturesCodesDlg(BOOL bNight, int x, int y, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CFuturesCodesDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_FUTURES_CODE };
	int m_x, m_y;
	BOOL m_bNight;
	CExtButton	m_buttons[4];
	Code*		m_codes[4];
	Code*		m_pSelCode;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton(UINT nID);
};
