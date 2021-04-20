// �� MFC ���� �ҽ� �ڵ�� MFC Microsoft Office Fluent ����� �������̽�("Fluent UI")�� 
// ����ϴ� ����� ���� �ָ�, MFC C++ ���̺귯�� ����Ʈ��� ���Ե� 
// Microsoft Foundation Classes Reference �� ���� ���� ������ ���� 
// �߰������� �����Ǵ� �����Դϴ�.  
// Fluent UI�� ����, ��� �Ǵ� �����ϴ� �� ���� ��� ����� ������ �����˴ϴ�.  
// Fluent UI ���̼��� ���α׷��� ���� �ڼ��� ������ 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.h : CMainFrame Ŭ������ �������̽�
//

#pragma once

#ifdef _MDI_
#include "ChildFrm.h"
class CMyToolControlBar : public CExtToolControlBar
{
    virtual CExtBarContentExpandButton * OnCreateBarRightBtn(){ 
        return NULL; 
    }
};


class CToolbarEdit : public CExtEdit
{

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};

typedef CExtNCW < CMDIFrameWnd > MainFrameBase;



class CMainFrame : public MainFrameBase
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();
	virtual ~CMainFrame();

	void LoginSuccess();
protected:
	CChildFrame* CreateChildFrame(int nChildViewID, int x = 0, int y = 0);
	
protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
	WINDOWPLACEMENT m_dataFrameWP;


	CExtMenuControlBar    m_wndMenuBar;
	CMyToolControlBar    m_wndToolBar;
	CExtLabel			m_wndLabelWndNumber;
	CToolbarEdit		m_wndEditWndFind;

	CExtStatusControlBar  m_wndStatusBar;
	
	CFont	m_fontWndNumber;

	CChildFrame*	m_pChildLog;
	CChildFrame*	m_pChildNotices;
	CChildFrame*	m_pPreferences;

	std::list<std::pair<int, CWnd*>	>	m_children;
// ������ �޽��� �� �Լ�
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL DestroyWindow();
	virtual void ActivateFrame(int nCmdShow = -1);

	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);

	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnAnyCommand();
	afx_msg void OnAnyCommandRange(UINT nID);
	afx_msg void OnUpdateAnyCommand(CCmdUI* pCmdUI);
	afx_msg LRESULT OnNoticesReceived(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNewNotice(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStandaloneWndToMDIChild(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRibbonCommands(UINT nID);
	afx_msg void OnUpdateRibbonCommands(CCmdUI* pCmdUI);
	afx_msg LRESULT OnVTSMDIChildFrameClosed(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStandaloneWndClosed(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPreferencesChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCodeListReceived(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnExtMenuPrepare(WPARAM wParam, LPARAM lParam);
	BOOL OnCmdMsg(UINT nID, int nCode,void* pExtra,AFX_CMDHANDLERINFO* pHandlerInfo);
	afx_msg void OnUpdateControlBarMenu(CCmdUI* pCmdUI);
	afx_msg BOOL OnBarCheck(UINT nID);
	afx_msg LRESULT OnDrawPopupMenuItem( WPARAM wParam,LPARAM lParam ); 
	afx_msg LRESULT OnServerDisconnected(WPARAM wParam, LPARAM lParam);
};


#endif