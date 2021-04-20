#pragma once

#include "resource.h"

// CChangePasswordDlg 대화 상자입니다.

class CChangePasswordDlg : public CExtResizableDialog
{
	DECLARE_DYNAMIC(CChangePasswordDlg)

public:
	CChangePasswordDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CChangePasswordDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CHANGE_PASSWORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonModify();
protected:
	afx_msg LRESULT OnMessageFromServer(WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL OnInitDialog();

	afx_msg void OnEnSetfocusEditPrev();
	afx_msg void OnEnSetfocusEditNew();
	afx_msg void OnEnSetfocusEditConfirm();
};
