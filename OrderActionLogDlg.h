#pragma once

#include <RequestRecvDlg.h>
#include "VTSGrid.h"
#include "xSkinButton.h"


// COrderActionLogDlg ��ȭ �����Դϴ�.
class COrderActionLogDlg : public CRequestRecvDlg
{
	DECLARE_DYNCREATE(COrderActionLogDlg)

	void OnDateSelChanged()
	{
		OnBnClickedButtonQuery();
	}

	class CExtMyDateTimeWnd : public CExtDateTimeWnd
	{
	public:
		COrderActionLogDlg*	m_pOwner;
		virtual bool OnValueChanged(const COleDateTime & dtDateTimeOld,const COleDateTime & dtDateTimeNew) const
		{
			m_pOwner->OnDateSelChanged();
			return true;
		}
	};

public:
	COrderActionLogDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~COrderActionLogDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_ORDER_ACTION_LOG };

	VTSGrid				m_grid;
	CExtMyDateTimeWnd	m_date;
	CExtCheckBox		m_checkContract;
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
