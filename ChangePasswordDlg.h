#pragma once

#include "resource.h"

// CChangePasswordDlg ��ȭ �����Դϴ�.

class CChangePasswordDlg : public CExtResizableDialog
{
	DECLARE_DYNAMIC(CChangePasswordDlg)

public:
	CChangePasswordDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CChangePasswordDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_CHANGE_PASSWORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
