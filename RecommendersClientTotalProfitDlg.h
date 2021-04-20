#pragma once

#include <RequestRecvDlg.h>
#include "VTSGrid.h"

// CRecommendersClientTotalProfitDlg ��ȭ �����Դϴ�.

class CRecommendersClientTotalProfitDlg : public CRequestRecvDlg
{
	DECLARE_DYNAMIC(CRecommendersClientTotalProfitDlg)

public:
	CRecommendersClientTotalProfitDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CRecommendersClientTotalProfitDlg();

	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_RECOMMENDERS_CLIENT_TOTAL_PROFIT };

	VTSGrid		m_grid;
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
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
