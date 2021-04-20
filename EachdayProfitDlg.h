#pragma once

#include <RequestRecvDlg.h>
#include "VTSGrid.h"
#include "xSkinButton.h"

// CEachdayProfitDlg ��ȭ �����Դϴ�.
class CEachdayProfitDlg : public CRequestRecvDlg
{
	DECLARE_DYNCREATE(CEachdayProfitDlg)

public:
	CEachdayProfitDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CEachdayProfitDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EACHDAY_PROFIT };

	VTSGrid				m_grid;
	CExtDateTimeWnd		m_dateBegin, m_dateEnd;
	CExtCheckBox		m_checkContract;

	CExtGridCellHeader * ColumnRegister(const CString& strColumnName,
										const CString& strCategoryName,
										INT nColumnExtent,
										INT nColumnExtentMin = 5,
										INT nColumnExtentMax= 32000);

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	void InitGrid();

	OVERRIDE void OnRequestReceived(DKBuffer& dbuf, LPVOID lpData);
	OVERRIDE void OnRequestError(int nRequestID, const CString& strMsg);


	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonQuery();
	afx_msg LRESULT OnRequestReceived(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonExcel();
};
