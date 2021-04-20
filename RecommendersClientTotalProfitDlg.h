#pragma once

#include <RequestRecvDlg.h>
#include "VTSGrid.h"

// CRecommendersClientTotalProfitDlg 대화 상자입니다.

class CRecommendersClientTotalProfitDlg : public CRequestRecvDlg
{
	DECLARE_DYNAMIC(CRecommendersClientTotalProfitDlg)

public:
	CRecommendersClientTotalProfitDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CRecommendersClientTotalProfitDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_RECOMMENDERS_CLIENT_TOTAL_PROFIT };

	VTSGrid		m_grid;
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	void InitGrid();

	OVERRIDE void OnRequestReceived(DKBuffer& dbuf, LPVOID lpData);
	OVERRIDE void OnRequestError(int nRequestID, const CString& strMsg);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonQuery();
public:
	CExtComboBox m_comboQueryField;
	CDateTimeCtrl m_dateBegin;
	CDateTimeCtrl m_dateEnd;
	afx_msg void OnBnClickedButtonToExcel();
	afx_msg void OnCbnSelchangeComboLeague();
};
