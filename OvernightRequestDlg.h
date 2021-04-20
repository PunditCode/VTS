#pragma once

// COvernightRequestDlg 대화 상자입니다.

class COvernightRequestDlg : public CExtResizableDialog
{
	DECLARE_DYNAMIC(COvernightRequestDlg)

public:
	COvernightRequestDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~COvernightRequestDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_OVERNIGHT_REQUEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonApply();
	int m_nRequest;
	afx_msg LRESULT OnMessageFromServer(WPARAM wParam, LPARAM lParam);
};
