#pragma once
#ifdef _CLIENT_ACTION

#include <RequestRecvDlg.h>
#include "UltimateGrid/UGCtrl.h"

// ClientActionLog 대화 상자입니다.
class CClientActionLogDlg : public CRequestRecvDlg
{
	DECLARE_DYNCREATE(CClientActionLogDlg)

public:
	CClientActionLogDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CClientActionLogDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_USER_ACTION_LOG };

	CUGCtrl		m_grid;
	CExtLabel			m_label1, m_label2;
	CExtDateTimeWnd		m_dateBegin, m_dateEnd;
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	void InitGrid();

	OVERRIDE void OnRequestReceived(DKBuffer& dbuf, LPVOID lpData);
	OVERRIDE void OnRequestError(int nRequestID, const CString& strMsg);
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonQuery();
};

#endif