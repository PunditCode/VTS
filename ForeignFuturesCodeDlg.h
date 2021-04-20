#pragma once
#include "resource.h"


// CForeignFuturesCodeDlg 대화 상자입니다.

class CForeignFuturesCodeDlg : public VTSModelessDlg<CDialog>
{
	DECLARE_DYNAMIC(CForeignFuturesCodeDlg)

public:
	CForeignFuturesCodeDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CForeignFuturesCodeDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_FOREIGN_FUTURES_CODE };

	list<boost::shared_ptr<CExtButton> >	m_buttons;
	vector<Code*>		m_codes;
	static Code*		s_pLastSelCode;
	class CFastTradeDlg*		m_pOwner;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton(UINT nID);
};
