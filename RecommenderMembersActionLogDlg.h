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


// CRecommenderMembersActionLog ��ȭ �����Դϴ�.

class CRecommenderMembersActionLogDlg : public CRequestRecvDlg
{
	DECLARE_DYNAMIC(CRecommenderMembersActionLogDlg)

public:
	CRecommenderMembersActionLogDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CRecommenderMembersActionLogDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_RECOMMENDER_MEMBERS_ACTION_LOG };

	ClientOrderActionLogGrid		m_gridClient;
	VTSGrid		m_grid;

	CExtComboBox m_comboQueryField;
	CExtDateTimeWnd		m_date;
	vector<ClientBasePtr>	m_clients;

	void OnGridSelChanged(int nCol, int nRow);

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
