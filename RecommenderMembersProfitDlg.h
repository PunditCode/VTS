#pragma once

#include "resource.h"
#include <RequestRecvDlg.h>
#include "VTSGrid.h"

class RecommenderMembersProfitGrid : public VTSGrid
{
public:
	class CRecommenderMembersProfitDlg*		m_pOwner;
	virtual void OnGbwSelectionChanged();
};


// CRecommenderMembersProfitDlg 대화 상자입니다.

class CRecommenderMembersProfitDlg : public CRequestRecvDlg
{
	DECLARE_DYNAMIC(CRecommenderMembersProfitDlg)

public:
	CRecommenderMembersProfitDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CRecommenderMembersProfitDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_RECOMMENDER_MEMBERS_PROFIT };

	RecommenderMembersProfitGrid		m_gridClient;
	VTSGrid		m_gridProfit;

	CExtComboBox		m_comboQueryField;
	CExtLabel			m_label1, m_label2;
	CExtDateTimeWnd		m_dateBegin, m_dateEnd;
	vector<ClientBasePtr>	m_clients;
	void OnGridSelChanged(int nCol, int nRow);

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	void InitClientGrid();
	void InitGrid();

	OVERRIDE void OnRequestReceived(DKBuffer& dbuf, LPVOID lpData);
	OVERRIDE void OnRequestError(int nRequestID, const CString& strMsg);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonQuery();
	afx_msg void OnGridClientSelChange(NMHDR *pNotifyStruct, LRESULT* /*pResult*/);
public:
	afx_msg void OnBnClickedButtonQueryClient();
};
