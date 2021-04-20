#pragma once
#ifdef _CLIENT_ACTION

#include <RequestRecvDlg.h>
#include "UltimateGrid/UGCtrl.h"

// ClientActionLog ��ȭ �����Դϴ�.
class CClientActionLogDlg : public CRequestRecvDlg
{
	DECLARE_DYNCREATE(CClientActionLogDlg)

public:
	CClientActionLogDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CClientActionLogDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_USER_ACTION_LOG };

	CUGCtrl		m_grid;
	CExtLabel			m_label1, m_label2;
	CExtDateTimeWnd		m_dateBegin, m_dateEnd;
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	void InitGrid();

	OVERRIDE void OnRequestReceived(DKBuffer& dbuf, LPVOID lpData);
	OVERRIDE void OnRequestError(int nRequestID, const CString& strMsg);
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonQuery();
};

#endif