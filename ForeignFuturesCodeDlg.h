#pragma once
#include "resource.h"


// CForeignFuturesCodeDlg ��ȭ �����Դϴ�.

class CForeignFuturesCodeDlg : public VTSModelessDlg<CDialog>
{
	DECLARE_DYNAMIC(CForeignFuturesCodeDlg)

public:
	CForeignFuturesCodeDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CForeignFuturesCodeDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_FOREIGN_FUTURES_CODE };

	list<boost::shared_ptr<CExtButton> >	m_buttons;
	vector<Code*>		m_codes;
	static Code*		s_pLastSelCode;
	class CFastTradeDlg*		m_pOwner;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton(UINT nID);
};
