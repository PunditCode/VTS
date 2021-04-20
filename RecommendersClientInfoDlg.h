#pragma once

#include "resource.h"
#include <RequestRecvDlg.h>
#include "VTSGrid.h"

// CRecommendersClientInfoDlg ��ȭ �����Դϴ�.

class CRecommendersClientInfoDlg : public CRequestRecvDlg
{
	DECLARE_DYNAMIC(CRecommendersClientInfoDlg)

public:
	CRecommendersClientInfoDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CRecommendersClientInfoDlg();
	void OnGridSelChanged(int nCol, int nRow);
	void OnGridLbuttonDoubleClicked(int nCol, int nRow);


	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_RECOMMENDERS_CLIENT_INFO };
	VTSGrid		m_grid;

	CExtComboBox m_comboQueryField;
	vector<ClientBasePtr>	m_clients;

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	void InitGrid();
	void SetRow(int nRow, ClientManagedPtr pClient);
	OVERRIDE void OnRequestReceived(DKBuffer& dbuf, LPVOID lpData);
	OVERRIDE void OnRequestError(int nRequestID, const CString& strMsg);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonQuery();
	afx_msg void OnBnClickedButtonToExcel();
};
