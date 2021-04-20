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

// VTS.h : VirtualTradingSystem ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.
#include "Socket.h"

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
	virtual int ExitInstance();
	
	BOOL Start();
	Socket* GetSocket() { return m_pSocket; }
private:
	Socket*	m_pSocket;
// �����Դϴ�.
protected:
	HMENU  m_hMDIMenu;
	HACCEL m_hMDIAccel;
	
public:
	UINT  m_nAppLook;
	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	afx_msg void OnSecureConnectionEstablished(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMessageFromServer(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

extern CMainApp theApp;

inline Socket* GetSocket()
{
	return theApp.GetSocket();
}
