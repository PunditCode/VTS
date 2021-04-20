#pragma once

#include "resource.h"

// CFuturesCodesDlg ��ȭ �����Դϴ�.
typedef CExtNCW < CExtResizableDialog >	CResizableDialogBase;

class CFuturesCodesDlg : public CResizableDialogBase
{
	DECLARE_DYNAMIC(CFuturesCodesDlg)

public:
	CFuturesCodesDlg(BOOL bNight, int x, int y, CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CFuturesCodesDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_FUTURES_CODE };
	int m_x, m_y;
	BOOL m_bNight;
	CExtButton	m_buttons[4];
	Code*		m_codes[4];
	Code*		m_pSelCode;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton(UINT nID);
};
