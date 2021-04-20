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

// VTS.h : VTS 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.
#include "Session.h"
#include "LogInDlg.h"
#include "AutoOrder.h"
#include "ServerEventReceiver.h"

// CMainApp:
// 이 클래스의 구현에 대해서는 VirtualTradingSystem.cpp을 참조하십시오.
//

class CMainApp : public CWinAppEx
{
public:
	CMainApp();

// 재정의입니다.
public:
	virtual BOOL InitInstance();
	virtual int Run();
	BOOL Init();

	virtual int ExitInstance();
	Session* GetSession() { return m_pSession; }
	void ApplyUpdateVTSStarter();
	AutoOrderManager* GetAutoOrderManager() { return m_pAutoOrderManager; }
	CFont* GetFontDefault() { return &m_fontDefault; }
	ServerEventReceiver* GetServerEventRecevier() { return m_pServerEventRecevier; }
	CString		m_ipMain;
	CString		m_ipSub;
	UINT		m_nPort;
	BOOL		m_bConnectToSub;
	CLogInDlg*	m_pLoginDlg;
		
	CString m_strManagerID;
	CString m_strManagerPassword;
	CString m_strClientID;
	CString m_strClientPassword;

	
	BOOL IsManagerMode() { return m_bManagerMode; }
private:
	Session*			m_pSession;
	AutoOrderManager*	m_pAutoOrderManager;
	CFont	m_fontDefault;
	ServerEventReceiver*	m_pServerEventRecevier;
protected:
#ifdef _MDI_
	HMENU  m_hMDIMenu;
	HACCEL m_hMDIAccel;
	UINT  m_nAppLook;
#endif
	BOOL	m_bManagerMode;

	DECLARE_MESSAGE_MAP()
public:
#ifdef _MDI_
	afx_msg void OnFileNew();
#endif
	afx_msg void OnConnectionEstablished(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMessageFromServer(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLoginResult(WPARAM wParam, LPARAM lParam);
};

extern CMainApp theApp;

inline Session* GetSession()
{
	return theApp.GetSession();
}

inline AutoOrderManager* GetAutoOrderManager()
{
	return theApp.GetAutoOrderManager();
}

inline ServerEventReceiver* SER()
{
	return theApp.GetServerEventRecevier();
}


extern GTMerchandisePool* g_pMP;