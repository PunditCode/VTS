#pragma once
#include "resource.h"

// CRegisterDlg 대화 상자입니다.

class CRegisterDlg : public CExtResizableDialog
{
	DECLARE_DYNAMIC(CRegisterDlg)

public:
	CRegisterDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CRegisterDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_REGISTER };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
