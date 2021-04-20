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

// VTS.h : VTS ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.
#include "Session.h"
#include "LogInDlg.h"
#include "AutoOrder.h"
#include "ServerEventReceiver.h"

// CMainApp:
// �� Ŭ������ ������ ���ؼ��� VirtualTradingSystem.cpp�� �����Ͻʽÿ�.
//

class CMainApp : public CWinAppEx
{
public:
	CMainApp();

// �������Դϴ�.
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