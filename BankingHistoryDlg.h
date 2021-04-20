#pragma once

#include <RequestRecvDlg.h>
#include "VTSGrid.h"
#include "xSkinButton.h"

// CBankingHistoryDlg ��ȭ �����Դϴ�.
class CBankingHistoryDlg : public CRequestRecvDlg
{
	DECLARE_DYNCREATE(CBankingHistoryDlg)

public:
	CBankingHistoryDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CBankingHistoryDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_BANKING_HISTORY };

	VTSGrid		m_grid;
	CExtDateTimeWnd		m_dateBegin, m_dateEnd;
	CExtRadioButton		m_radio[3];
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	void InitGrid();

	OVERRIDE void OnRequestReceived(DKBuffer& dbuf, LPVOID lpData);
	OVERRIDE void OnRequestError(int nRequestID, const CString& strMsg);
	
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButtonQuery();
	afx_msg void OnDtnDatetimechangeDate(NMHDR *pNMHDR, LRESULT *pResult);
	int m_nRequestType;
public:
	afx_msg void OnBnClickedButtonExcel();
};
