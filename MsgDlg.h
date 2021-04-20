#pragma once

#include "resource.h"

// CMsgDlg 대화 상자입니다.

class CMsgDlg : public CExtResizableDialog
{
	DECLARE_DYNAMIC(CMsgDlg)

public:
	CMsgDlg(BOOL bLarageWidth = FALSE, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMsgDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MSG };

	CString m_strTitle;
	CString m_strBody;

	CExtEdit	m_editTitle, m_editBody;
	BOOL m_bLargeWidth;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

public:
	DECLARE_MESSAGE_MAP()

	virtual void PostNcDestroy();
	virtual void OnOK();
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
};
