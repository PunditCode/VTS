#pragma once

#include <RequestRecvDlg.h>
#include "VTSGrid.h"

// CRecommenderEachdayProfitDlg 대화 상자입니다.

class CRecommenderEachdayProfitDlg : public CRequestRecvDlg
{
	DECLARE_DYNCREATE(CRecommenderEachdayProfitDlg)

public:
	CRecommenderEachdayProfitDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CRecommenderEachdayProfitDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_RECOMMENDER_EACHDAY_PROFIT };

	VTSGrid				m_grid;
	CExtDateTimeWnd		m_dateBegin, m_dateEnd;
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	void InitGrid();

	OVERRIDE void OnRequestReceived(DKBuffer& dbuf, LPVOID lpData);
	OVERRIDE void OnRequestError(int nRequestID, const CString& strMsg);


	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonQuery();
	afx_msg LRESULT OnRequestReceived(WPARAM wParam, LPARAM lParam);
};
