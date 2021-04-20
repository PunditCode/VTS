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

// VTS.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "VTS.h"

#ifdef _MDI_
	#include "MainFrm.h"
	#include "ChildFrm.h"
#else
	#if THEME == THEME_DEFAULT
		#include "VTSMainWnd.h"
	#elif THEME == THEME_LINETRADE
		#include "WizMainWnd.h"
	
	#else
		#include "MainDlg.h"
	#endif
#endif

//#include <DKExceptHandler.h>
#include "MsgDlg.h"
#include "GTLibExt\VTSObjectFactory.h"

#include "RegisterDlg.h"
#include "..\VTSStarter\VTSNames.h"
#include "VTSError.h"
#include "ExtVTSPaintManager.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define _NEED_LOGIN

// CMainApp

BEGIN_MESSAGE_MAP(CMainApp, CWinAppEx)
#ifdef _MDI_
	ON_COMMAND(ID_FILE_NEW, &CMainApp::OnFileNew)
#endif
	ON_THREAD_MESSAGE(WM_CONNECTION_ESTABLISHED, &CMainApp::OnConnectionEstablished)
	ON_THREAD_MESSAGE(WM_MESSAGE_FROM_SERVER, &CMainApp::OnMessageFromServer)
	ON_THREAD_MESSAGE(WM_LOGIN_RESULT, &CMainApp::OnLoginResult)
END_MESSAGE_MAP()


GTMerchandisePool* g_pMP = NULL;

// CMainApp ����

CMainApp::CMainApp()
{
	SetAppID(_T("VTS.AppID.NoVersion"));
	m_bManagerMode = FALSE;
	m_nPort = 0;
	m_pLoginDlg = NULL;
	m_pAutoOrderManager = NULL;
	m_bConnectToSub = FALSE;
	m_pServerEventRecevier = NULL;
}

// ������ CMainApp ��ü�Դϴ�.

CMainApp theApp;


BOOL CMainApp::InitInstance()
{
	ApplyUpdateVTSStarter();

	CString id, password;
	m_bConnectToSub = FALSE;
	//	ErrMsgBox(_T(""));
	if (__argc > 1)
	{
		// �Ķ���� ������ ������ �ٲ�� �Ǹ� ������Ʈ�� ���������� �ٲ�� �ǹǷ� ������ �����ϰ� �ȴ�. 
		const CString parent = __targv[1];
		if (parent == VTSGUID_CLIENT_CERT)
		{
			if (__argc != 6)
			{
				ErrMsgBox(_T("�߸��� �����Դϴ�."));
				PostQuitMessage(0);
			}

			m_ipMain = __targv[2];
			m_nPort = ::ToINT(__targv[3]);
			m_ipSub = __targv[4];
			m_bConnectToSub = ::ToINT(__targv[5]);
		}
		else if (parent == VTSGUID_MANAGER_CERT)
		{
			if (__argc != 8)
			{
				ErrMsgBox(_T("�߸��� �����Դϴ�."));
				PostQuitMessage(0);
			}

			m_ipMain = __targv[2];
			m_nPort = ::ToINT(__targv[3]);
			m_strClientID = __targv[4];
			m_strClientPassword = __targv[5];

			m_strManagerID = __targv[6];
			m_strManagerPassword = __targv[7];

			/*CString s;
			s.Format(_T("%s %d %s %s %s %s"), m_ipMain, m_nPort, m_strClientID, m_strClientPassword,
			m_strManagerID, m_strManagerPassword);

			AfxMessageBox(s);*/
			m_bManagerMode = TRUE;

		}
		else
		{
			ErrMsgBox(_T("�߸��� �����Դϴ�."));
			PostQuitMessage(0);
		}
	}
#ifndef _DEBUG
	else
	{
		ErrMsgBox(_T("�߸��� �����Դϴ�."));
		PostQuitMessage(0);
	}
#else
	if (m_ipMain.IsEmpty())
	{
		m_ipMain = _T("localhost");
//		m_ipMain = _T("neverasset");
		m_nPort = 2911;
	}
#endif
	/*if(__argc == 6)
	{
	const CString cmd = __targv[1];

	if(cmd == VTSSTARTER_GUID)
	{
	m_ipMain = __targv[2];
	m_nPort = ::ToINT(__targv[3]);
	m_strClientID = __targv[4];
	m_strClientPassword = __targv[5];
	m_bManagerMode = FALSE;
	}
	else if(cmd == _T("/R"))
	{
	m_strManagerID = __targv[2];
	m_strManagerPassword = __targv[3];
	m_strClientID = __targv[4];
	m_strClientPassword = __targv[5];
	m_bManagerMode = TRUE;
	}
	}*/
#if 1//ndef _DEBUG

#ifdef _DISABLE_DWM
	HRESULT hr = S_OK;
	// Disable DWM Composition 
	hr = DwmEnableComposition(DWM_EC_DISABLECOMPOSITION);
	if (SUCCEEDED(hr))
	{
	}
	else
	{
		AfxMessageBox(_T("DWM Failed"));
	}
#endif

#endif

	CString strAppName(m_pszAppName);

	strAppName = _T("ACROASSET");

	if (strAppName != m_pszAppName)
	{
		free((void*)m_pszAppName);
		m_pszAppName = _tcsdup(strAppName);
	}

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// OLE ���̺귯���� �ʱ�ȭ�մϴ�.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxInitRichEdit2();
	AfxEnableControlContainer();
	EnableTaskbarInteraction(FALSE);
	SetRegistryKey(strAppName);

	if (m_pszProfileName != NULL)
		free((void*)m_pszProfileName);
	m_pszProfileName = _tcsdup(strAppName);
	ASSERT(m_pszProfileName != NULL);
	VERIFY(g_CmdManager->ProfileSetup(m_pszProfileName));

#ifdef _MDI_
	CMDIFrameWnd* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;

	m_pMainWnd = pFrame;
#else

#if THEME == THEME_DEFAULT || THEME == THEME_LINETRADE
	InitContextMenuManager();
	InitKeyboardManager();
	InitTooltipManager();

#if THEME == THEME_LINETRADE
	WizMainWnd* pWnd = new WizMainWnd;
#else
	VTSMainWnd* pWnd = new VTSMainWnd;
#endif
	m_pMainWnd = pWnd;
#else
	CMainDlg* pDlg = new CMainDlg;
	pDlg->Create(CMainDlg::IDD, NULL);
	m_pMainWnd = pDlg;
#endif

#endif

	return TRUE;
}
	
BOOL CMainApp::Init()
{
	/*CString strErrorTxt;
	strErrorTxt.Format(_T("%s\\VTS_error_%s.txt"), ENV().GetRoot(), CTime::GetCurrentTime().Format(_T("%Y%m%d_%H%M%S")));
	InstEH().SetLogFileName(strErrorTxt);*/
	m_pServerEventRecevier = new ServerEventReceiver;
	
//	InstLog().Init(ENV().GetRoot() + _T("\\LOG"));

	
	GTInit(new VTSObjectFactory);
	m_pSession = new Session;

	if(m_bManagerMode)
	{
		if (!GetSession()->ConnectToServer(ENV().GetPEMPath(), m_ipMain, m_nPort))
		{
			ErrMsgBox(_T("�������ӿ� �����߽��ϴ�. �����ڿ��� �������ּ���"));
			return FALSE;
		}
	}
	else
	{
		m_pLoginDlg = new CLogInDlg;
		m_pLoginDlg->Create(CLogInDlg::IDD, m_pMainWnd);
		m_pLoginDlg->ShowWindow(SW_SHOW);
	}
	
	return TRUE;
}
int CMainApp::Run()
{
	if (FALSE == Init())
		return FALSE;

	BOOL bRun = TRUE;
	BOOL bExit = FALSE;
	while (!bExit)
	{
		bRun = CWinApp::Run();
		bExit = TRUE;
	}

	return bRun;
}


void CMainApp::OnConnectionEstablished(WPARAM wParam, LPARAM lParam)
{
	if(m_bManagerMode)
	{
		CString strParam;
		strParam.Format(_T("%s;%s"), m_strManagerID, m_strManagerPassword);
		GetSession()->Login(0, m_strClientID, m_strClientPassword, strParam);
	}
	else
	{
		ASSERT(m_pLoginDlg);
		m_pLoginDlg->SendMessage(WM_CONNECTION_ESTABLISHED);
	}
}

void CMainApp::ApplyUpdateVTSStarter()
{
	CString strPath;
	CString strNewPath;

	strPath.Format(_T("%s\\%s"), ENV().GetRoot(), MODULENAME_CLIENT_STARTER);
	strNewPath.Format(_T("%s\\%s.NEW"), ENV().GetRoot(), MODULENAME_CLIENT_STARTER);

	if(IsExistFile(strNewPath))
	{
		if(IsExistFile(strPath))
		{
			if(!DeleteFile(strPath))
			{
				ErrMsgBox( _T("������Ʈ ���������� �����߽��ϴ�(VTSStarter.exe)"));
			}
		}

		MoveFile(strNewPath, strPath);
	}
}

void CMainApp::OnLoginResult(WPARAM wParam, LPARAM lParam)
{
	VTSS_LOGIN_RESULT resultCode = (VTSS_LOGIN_RESULT)wParam;
	CString* pStrMsg = (CString*)lParam;
	auto_ptr<CString> spStrMsg(pStrMsg);
	if (VTSS_LOGIN_SUCCESS != resultCode)
	{
		ErrMsgBox(*pStrMsg);
		return;
	}
	if (m_pLoginDlg)
	{
		m_pLoginDlg->ShowWindow(SW_HIDE);
		delete m_pLoginDlg;
	}

	g_pMP = new GTMerchandisePool;
	m_pAutoOrderManager = new AutoOrderManager;

#ifdef _MDI_
	//VERIFY(TerminateProcess(m_hStarter, 0));
	CMDIFrameWnd* pFrame = dynamic_cast<CMDIFrameWnd*>(m_pMainWnd);
	// �� MDI ������ â�� ����ϴ�.
	if (!pFrame->LoadFrame(IDR_MAINFRAME))
		return;

	HINSTANCE hInst = AfxGetResourceHandle();
	m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_VTSTYPE));
	m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_VTSTYPE));

	((CMainFrame*)m_pMainWnd)->LoginSuccess();

	InitContextMenuManager();
	InitKeyboardManager();
	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// ���� MDI �޴� �� �׼��������� Ű ���̺��� �ε带 �õ��մϴ�.
	//TODO: �߰� ��� ������ �߰��ϰ� ���� ���α׷��� �ʿ��� �߰� �޴� ���Ŀ� ����
	//	ȣ���� �ε��մϴ�.
	// �� â�� �ʱ�ȭ�Ǿ����Ƿ� �̸� ǥ���ϰ� ������Ʈ�մϴ�.
	pFrame->ShowWindow(SW_SHOWMAXIMIZED);//m_nCmdShow);
	pFrame->UpdateWindow();
#else

	if (m_bManagerMode)
		g_PaintManager.InstallPaintManager(RUNTIME_CLASS(CExtPaintManagerForManagerMode));
	else
	{
		g_PaintManager.InstallPaintManager(RUNTIME_CLASS(CExtPaintManagerOffice2003NoThemes));
	}


	m_fontDefault.CreateFont(12, 0, 0, 0, 400, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("����"));

#if THEME == THEME_DEFAULT || THEME == THEME_LINETRADE

	#if THEME == THEME_DEFAULT
	VTSMainWnd* pWnd = (VTSMainWnd*)m_pMainWnd;
	#elif THEME == THEME_LINETRADE
	WizMainWnd* pWnd = (WizMainWnd*)m_pMainWnd;
	#endif
	pWnd->LoadFrame(IDR_MAINFRAME, WS_POPUP, NULL,NULL);
	pWnd->GetMenu()->Detach();
	pWnd->SetMenu(NULL);
#endif

	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->SendMessage(WM_LOGIN_COMPLETED);
#endif
	GetSession()->SendReady();
}


int CMainApp::ExitInstance()
{
	m_pMainWnd->DestroyWindow();
	delete m_pAutoOrderManager;
	
	m_pSession->LogOut();
	m_pSession->Close();
	delete m_pServerEventRecevier;
	delete g_pMP;
//	VTSSocket::StopIOService();
//	delete m_pSession;

	
#ifdef _MDI_

	if (m_hMDIMenu != NULL)
		FreeResource(m_hMDIMenu);
	if (m_hMDIAccel != NULL)
		FreeResource(m_hMDIAccel);
#endif
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CMainApp �޽��� ó����
#ifdef _MDI_
void CMainApp::OnFileNew() 
{

	CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
	// �� MDI �ڽ� â�� ����ϴ�.
	pFrame->CreateNewChild(
		RUNTIME_CLASS(CChildFrame), IDR_VTSTYPE, m_hMDIMenu, m_hMDIAccel);

}
#endif

// CMainApp �޽��� ó����

void CMainApp::OnMessageFromServer(WPARAM wParam, LPARAM lParam)
{
	std::auto_ptr<MsgItem> pMsg((MsgItem*)wParam);
	if(VTSS_MSG_NOTICE == pMsg->nMsgType || VTSS_MSG_POPUP == pMsg->nMsgType || VTSS_MSG_REQ_RESULT == pMsg->nMsgType)
	{
		CMsgDlg* pDlg = new CMsgDlg(VTSS_MSG_NOTICE == pMsg->nMsgType);
		pDlg->m_strTitle = pMsg->strTitle;
		pDlg->m_strBody = pMsg->strBody;
		pDlg->Create(CMsgDlg::IDD, m_pMainWnd);
		pDlg->ShowWindow(SW_SHOW);

		::PlayWave(ENV().GetRoot() + _T("\\wave\\NEW_MESSAGE.WAV"));
	}
	else if(VTSS_MSG_ERR == pMsg->nMsgType)
	{
		if(pMsg->nCode == VRES_LOGIN_ANOTHER_PLACE)
		{
			m_pSession->LogOut();
			MessageBox(m_pMainWnd->GetSafeHwnd(), pMsg->strBody, pMsg->strTitle, MB_ICONINFORMATION);
			PostQuitMessage(0);
		}
		else
		{
			if(::IsWindow((HWND)pMsg->hWnd))
				MessageBox((HWND)pMsg->hWnd, pMsg->strBody, pMsg->strTitle, MB_ICONERROR);
			else
				MessageBox(m_pMainWnd->GetSafeHwnd(), pMsg->strBody, pMsg->strTitle, MB_ICONERROR);
		}
	}
}
