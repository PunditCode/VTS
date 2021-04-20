#pragma once

#include "resource.h"
#include <RequestRecvDlg.h>
#include "VTSGrid.h"

class ClientOrderActionLogGrid : public VTSGrid
{
public:
	class CRecommenderMembersActionLogDlg*		m_pOwner;
	virtual void OnGbwSelectionChanged();
};


// CRecommenderMembersActionLog 대화 상자입니다.

class CRecommenderMembersActionLogDlg : public CRequestRecvDlg
{
	DECLARE_DYNAMIC(CRecommenderMembersActionLogDlg)

public:
	CRecommenderMembersActionLogDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CRecommenderMembersActionLogDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_RECOMMENDER_MEMBERS_ACTION_LOG };

	ClientOrderActionLogGrid		m_gridClient;
	VTSGrid		m_grid;

	CExtComboBox m_comboQueryField;
	CExtDateTimeWnd		m_date;
	vector<ClientBasePtr>	m_clients;

	void OnGridSelChanged(int nCol, int nRow);

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	void QueryPeriod();
	void InitClientGrid();
	void InitGrid();

	OVERRIDE void OnRequestReceived(DKBuffer& dbuf, LPVOID lpData);
	OVERRIDE void OnRequestError(int nRequestID, const CString& strMsg);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonQuery();
	afx_msg void OnGridClientSelChange(NMHDR *pNotifyStruct, LRESULT* /*pResult*/);
public:
};
