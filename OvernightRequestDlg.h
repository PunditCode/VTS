#pragma once

// COvernightRequestDlg ��ȭ �����Դϴ�.

class COvernightRequestDlg : public CExtResizableDialog
{
	DECLARE_DYNAMIC(COvernightRequestDlg)

public:
	COvernightRequestDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~COvernightRequestDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_OVERNIGHT_REQUEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonApply();
	int m_nRequest;
	afx_msg LRESULT OnMessageFromServer(WPARAM wParam, LPARAM lParam);
};
