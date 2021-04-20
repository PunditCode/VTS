#pragma once

#include <RequestRecvDlg.h>
#include "VTSGrid.h"
#include "xSkinButton.h"

// COrderHistoryDlg ��ȭ �����Դϴ�.
class COrderHistoryDlg : public CRequestRecvDlg
{
	DECLARE_DYNCREATE(COrderHistoryDlg)

public:
	COrderHistoryDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~COrderHistoryDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_ORDER_HISTORY };

	VTSGrid		m_grid;
	CExtDateTimeWnd		m_dateBegin, m_dateEnd;
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	void InitGrid();

	OVERRIDE void OnRequestReceived(DKBuffer& dbuf, LPVOID lpData);
	OVERRIDE void OnRequestError(int nRequestID, const CString& strMsg);
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonQuery();

	afx_msg LRESULT OnRequestReceived(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonExcel();
	afx_msg void OnBnClickedCheckContract();
};
