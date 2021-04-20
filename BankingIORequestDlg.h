#pragma once


// CBankingIORequestDlg 대화 상자입니다.
#include <VTSFrmWndChildBase.h>
#include "DKTabCtrl.h"

class CBankingIORequestDlg : public VTSFrmWndChildResizableDlg
{
	DECLARE_DYNCREATE(CBankingIORequestDlg)

public:
	CBankingIORequestDlg(BankingIOType mode = biot_deposit, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CBankingIORequestDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_DEPOSIT_REQUEST };

	DKTabCtrl		m_tab;

	BankingIOType	m_mode;
	
	CExtEditSystemCurrency		m_editAmount;
	CExtEdit					m_editName;
	CExtEditSystemCurrency		m_editAmountRequest;

	CExtEdit		m_editBank;
	CExtEdit		m_editBankAccount;
	CExtEdit		m_editBankOwner;
	CExtEdit		m_editMemo;

	CExtGroupBox	m_groupBank;
	CExtGroupBox	m_groupMemo;

	CExtLabel		m_labels[5];
	CExtButton		m_buttonRequest;
	CExtButton		m_buttonCancel;
protected:
	void SetControls(BankingIOType mode);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonRequest();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedCancel();
	afx_msg LRESULT OnClientInfoUpdated(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonAll();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnTabSelChange();
	afx_msg LRESULT OnVTSFrmShowCompleted(WPARAM wParam, LPARAM lParam);

};
