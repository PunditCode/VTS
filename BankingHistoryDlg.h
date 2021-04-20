#pragma once

#include <RequestRecvDlg.h>
#include "VTSGrid.h"
#include "xSkinButton.h"

// CBankingHistoryDlg 대화 상자입니다.
class CBankingHistoryDlg : public CRequestRecvDlg
{
	DECLARE_DYNCREATE(CBankingHistoryDlg)

public:
	CBankingHistoryDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CBankingHistoryDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_BANKING_HISTORY };

	VTSGrid		m_grid;
	CExtDateTimeWnd		m_dateBegin, m_dateEnd;
	CExtRadioButton		m_radio[3];
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
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
