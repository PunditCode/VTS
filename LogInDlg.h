
// MainDlg.h : 헤더 파일
//

#pragma once

#include "VEdit.h"
#include "EditTrans.h"
#include "StaticTrans.h"
#include "resource.h"
#include "xSkinButton.h"
extern CFont g_font;

// CLogInDlg 대화 상자
class CLogInDlg : public CDialog
{
// 생성입니다.
public:
	CLogInDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_LOGIN };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.
private:
	CRect		m_rtClient;
	CPoint		m_ptDown;

	CImage		m_imgBack;
	HICON		m_hIcon;

	CxSkinButton	m_buttonConnect, m_buttonCancel, m_buttonRegister;

// 구현입니다.
protected:
	CEdit m_editID;
	CEdit m_editPassword;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonRegister();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	afx_msg LRESULT OnConnectionEstablished(WPARAM wParam, LPARAM lParam);
public:
	CStaticTrans m_staticLog;
	CButton m_checkSaveID;

	afx_msg void OnEnSetfocusEditId();
	afx_msg void OnEnSetfocusEditPassword();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnEnChangeEditId();
};
