#pragma once
#include "resource.h"

// CRegisterDlg ��ȭ �����Դϴ�.

class CRegisterDlg : public CExtResizableDialog
{
	DECLARE_DYNAMIC(CRegisterDlg)

public:
	CRegisterDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CRegisterDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_REGISTER };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonRegister();
	afx_msg void OnEnChangeEditPhone();
	afx_msg void OnEnSetfocusEditId();
	afx_msg void OnEnSetfocusEditName();
	afx_msg void OnEnSetfocusEditPassword();
	afx_msg void OnEnSetfocusEditPassword2();
};
