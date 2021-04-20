
// MainDlg.h : ��� ����
//

#pragma once

#include "VEdit.h"
#include "EditTrans.h"
#include "StaticTrans.h"
#include "resource.h"
#include "xSkinButton.h"
extern CFont g_font;

// CLogInDlg ��ȭ ����
class CLogInDlg : public CDialog
{
// �����Դϴ�.
public:
	CLogInDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_LOGIN };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.
private:
	CRect		m_rtClient;
	CPoint		m_ptDown;

	CImage		m_imgBack;
	HICON		m_hIcon;

	CxSkinButton	m_buttonConnect, m_buttonCancel, m_buttonRegister;

// �����Դϴ�.
protected:
	CEdit m_editID;
	CEdit m_editPassword;

	// ������ �޽��� �� �Լ�
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
