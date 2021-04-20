#pragma once

#include "resource.h"
#include <RequestRecvDlg.h>
#include "VTSGrid.h"

// CRecommendersClientInfoDlg 대화 상자입니다.

class CRecommendersClientInfoDlg : public CRequestRecvDlg
{
	DECLARE_DYNAMIC(CRecommendersClientInfoDlg)

public:
	CRecommendersClientInfoDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CRecommendersClientInfoDlg();
	void OnGridSelChanged(int nCol, int nRow);
	void OnGridLbuttonDoubleClicked(int nCol, int nRow);


	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_RECOMMENDERS_CLIENT_INFO };
	VTSGrid		m_grid;

	CExtComboBox m_comboQueryField;
	vector<ClientBasePtr>	m_clients;

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	void InitGrid();
	void SetRow(int nRow, ClientManagedPtr pClient);
	OVERRIDE void OnRequestReceived(DKBuffer& dbuf, LPVOID lpData);
	OVERRIDE void OnRequestError(int nRequestID, const CString& strMsg);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonQuery();
	afx_msg void OnBnClickedButtonToExcel();
};
