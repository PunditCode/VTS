#pragma once

#include "resource.h"
#include "PopupWnd.h"
#include <list>
#include <TaskbarNotifier.h>

// CMainDlg 대화 상자입니다.
class CExtToolControlBarNoRightBtn : public CExtToolControlBar
{
    virtual CExtBarContentExpandButton * OnCreateBarRightBtn(){ 
        return NULL; 
    }
};

class CMainDlg : public CExtNCW<CExtResizableDialog>
{
	DECLARE_DYNAMIC(CMainDlg)

public:
	CMainDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMainDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MAIN };

	HICON m_hIcon;
	CExtMenuControlBar				m_wndMenuBar;
	CExtToolControlBarNoRightBtn    m_wndToolBar;
protected:
	CPopupWnd*	m_pWndNotices;
	CPopupWnd*	m_pWndLog;
	std::list<CPopupWnd*>	m_wndPopups;

	void ClosePopups();
	
	CTaskbarNotifier	m_wndTaskbarNotf;

	CFont	m_fontStatic;
	CExtLabel	m_labelKospi, m_labelUSD, m_label1, m_label2;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	void ShowChildWindow(UINT nWndID, int x = 0, int y = 0);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCommandAll(UINT nID);
	afx_msg LRESULT OnLoginCompleted(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNoticesReceived(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNewNotice(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPreferencesChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnServerDisconnected(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	virtual void PostNcDestroy();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg LRESULT OnChildWndClosed(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOrderResult(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMarketEye(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReconnectedToServer(WPARAM wParam, LPARAM lParam);
};
