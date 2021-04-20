#pragma once

#include "resource.h"

// CMsgDlg ��ȭ �����Դϴ�.

class CMsgDlg : public CExtResizableDialog
{
	DECLARE_DYNAMIC(CMsgDlg)

public:
	CMsgDlg(BOOL bLarageWidth = FALSE, CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CMsgDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_MSG };

	CString m_strTitle;
	CString m_strBody;

	CExtEdit	m_editTitle, m_editBody;
	BOOL m_bLargeWidth;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

public:
	DECLARE_MESSAGE_MAP()

	virtual void PostNcDestroy();
	virtual void OnOK();
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
};
