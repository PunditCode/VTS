#pragma once

#include <RequestRecvDlg.h>
#include "VTSGrid.h"

// CRecommenderEachdayProfitDlg ��ȭ �����Դϴ�.

class CRecommenderEachdayProfitDlg : public CRequestRecvDlg
{
	DECLARE_DYNCREATE(CRecommenderEachdayProfitDlg)

public:
	CRecommenderEachdayProfitDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CRecommenderEachdayProfitDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_RECOMMENDER_EACHDAY_PROFIT };

	VTSGrid				m_grid;
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
};
