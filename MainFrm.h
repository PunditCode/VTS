// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를 
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된 
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해 
// 추가적으로 제공되는 내용입니다.  
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.  
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.h : CMainFrame 클래스의 인터페이스
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
	
protected:  // 컨트롤 모음이 포함된 멤버입니다.
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
// 생성된 메시지 맵 함수
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