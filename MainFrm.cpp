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

// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "MainFrm.h"

#ifdef _MDI_
#include "VTS.h"


#include "Session.h"
#include "LogDlg.h"
#include "StandaloneWnd.h"
#include "ExtVTSPaintManager.h"
#include "PreferencesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, MainFrameBase)

BEGIN_MESSAGE_MAP(CMainFrame, MainFrameBase)
	ON_WM_CREATE()
	ON_COMMAND_RANGE(IDC_STATIC_WND_NUMBER, ID_TOOLBAR_BUTTON_RESERVED, &CMainFrame::OnRibbonCommands)
	ON_UPDATE_COMMAND_UI_RANGE(IDC_STATIC_WND_NUMBER, ID_TOOLBAR_BUTTON_RESERVED, &CMainFrame::OnUpdateRibbonCommands)
	ON_MESSAGE(WM_NOTICES_RECEIVED, &CMainFrame::OnNoticesReceived)
	ON_MESSAGE(WM_NEW_NOTICE, &CMainFrame::OnNewNotice)
	ON_MESSAGE(WM_STANDALONE_WND_TO_MDI_CHILD, &CMainFrame::OnStandaloneWndToMDIChild)
	ON_MESSAGE(WM_VTS_MDI_CHILD_FRAME_CLOSED, &CMainFrame::OnVTSMDIChildFrameClosed)
	ON_MESSAGE(WM_STANDALONE_WND_CLOSED, &CMainFrame::OnStandaloneWndClosed)
	ON_MESSAGE(WM_PREFERENCES_CHANGED, &CMainFrame::OnPreferencesChanged)
	ON_REGISTERED_MESSAGE(CExtPopupMenuWnd::g_nMsgPrepareMenu, OnExtMenuPrepare)
	ON_MESSAGE(WM_CODE_LIST_RECEIVED, &CMainFrame::OnCodeListReceived)
	ON_REGISTERED_MESSAGE( CExtPopupMenuWnd::g_nMsgPopupDrawItem, &CMainFrame::OnDrawPopupMenuItem )
	ON_MESSAGE(WM_SERVER_DISCONNECTED, &CMainFrame::OnServerDisconnected)
END_MESSAGE_MAP()

// CMainFrame 생성/소멸

CMainFrame::CMainFrame() : m_pChildNotices(NULL), m_pChildLog(NULL), m_pPreferences(NULL)
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.

	CWinApp * pApp = ::AfxGetApp();
	//if(!g_PaintManager.PaintManagerStateLoad(pApp->m_pszRegistryKey, pApp->m_pszProfileName, pApp->m_pszProfileName))
	//if(theApp.IsManagerMode())
	//	g_PaintManager.InstallPaintManager(RUNTIME_CLASS(CExtPaintManagerForManagerMode));//CExtVTSPaintManager
	//else
	g_PaintManager.InstallPaintManager(RUNTIME_CLASS(CExtVTSPaintManager));//CExtVTSPaintManager CExtPaintManagerStudio2005

	// window placement persistence
	::memset( &m_dataFrameWP, 0, sizeof(WINDOWPLACEMENT) );
	m_dataFrameWP.length = sizeof(WINDOWPLACEMENT);
	m_dataFrameWP.showCmd = SW_HIDE;
}

CMainFrame::~CMainFrame()
{
}

LRESULT CMainFrame::OnDrawPopupMenuItem(WPARAM wParam,LPARAM lParam)
{

CExtPopupMenuWnd::DRAWITEMDATA * pDrawItemData =reinterpret_cast< CExtPopupMenuWnd::DRAWITEMDATA * >( lParam );

    ASSERT( pDrawItemData != NULL );
    pDrawItemData->PaintDefault(
        true, // no paint icon
        false, // no paint text
        false, // no paint check/radio mark
        false, // no paint as enabled
        false // no paint as unselected
        );
    // returns a non-zero value if the item is repainted
    // by this message handler
    return (!0);
}

static UINT g_statBasicCommands[] =
{
//	ID_APP_ABOUT,
//	ID_APP_EXIT,
	ID_VIEW_MENUBAR,
	IDR_TOOLBAR_MAIN,
	IDM_FAST_TRADING                ,
	IDM_BANKING_WITHDRAW            ,
	IDM_BANKING_DEPOSIT             ,
	IDM_BANKING_IO_HISTORY          ,
	IDM_USER_ACTION_LOG             ,
	IDM_NOTICES                     ,
	IDM_PREFERENCES                 ,
	IDM_RCMD_MEMBERS_PROFIT         ,
	IDM_RCMD_PERIOD_PROFIT          ,
	IDM_RCMD_MEMBER_ACTION_LOG      ,
	IDM_ORDER_ACTION_LOG            ,
//	ID_VIEW_STATUS_BAR,
	0, // end of list
};



int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (MainFrameBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	HICON hIcon = (::AfxGetApp())->LoadIcon(IDR_MAINFRAME);
	ASSERT( hIcon != NULL );
	SetIcon( hIcon, TRUE );
	SetIcon( hIcon, FALSE );
	
	/*CExtMenuControlBar::g_bHideMenuBarExpandContentButton = true;
	CExtPopupMenuWnd::g_bMenuWithShadows = false;
	CExtPopupMenuWnd::g_bMenuDelayExpanding = false;
	CExtPopupMenuWnd::g_bMenuLargeIcons = false;
	*/

	CWinApp * pApp = ::AfxGetApp();
	g_CmdManager->ProfileSetup(pApp->m_pszProfileName, GetSafeHwnd());
	VERIFY(g_CmdManager->UpdateFromMenu(pApp->m_pszProfileName, IDR_MAINFRAME));
//	VERIFY(g_CmdManager->UpdateFromToolBar(pApp->m_pszProfileName,IDR_MAINFRAME));
	VERIFY(g_CmdManager->UpdateFromToolBar(pApp->m_pszProfileName,IDR_TOOLBAR_MAIN));
	//VERIFY(g_CmdManager->UpdateFromMenu(pApp->m_pszProfileName, IDR_VTSTYPE));
	
	m_wndMenuBar.SetMdiWindowPopupName( _T("VTS메뉴") );
	if((!m_wndMenuBar.Create(NULL, this, ID_VIEW_MENUBAR)))
    {
        TRACE0("Failed to create menubar\n");
        return -1;      // failed to create
    }
	m_wndMenuBar._FlatTrackingSet(true);

	if((!m_wndToolBar.Create(_T("바로가기"), this, AFX_IDW_TOOLBAR)) || (! m_wndToolBar.LoadToolBar( IDR_TOOLBAR_MAIN)))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
#if 0
	if(	!m_wndLabelWndNumber.CreateEx(0, _T("STATIC"), NULL, WS_CHILD|WS_VISIBLE|SS_SIMPLE|SS_CENTER|SS_CENTERIMAGE,
			CRect( 0, 0, 90, 25 ), &m_wndToolBar, IDC_STATIC_WND_NUMBER))
	{
		ASSERT( FALSE );
		return FALSE;
	}

	if(NULL == (HFONT)m_fontWndNumber)
	{
		CFont* pFont = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
		LOGFONT logFont; 
		pFont->GetLogFont(&logFont);
		logFont.lfWeight = 1000;
		logFont.lfHeight *= 1.5;
		m_fontWndNumber.CreateFontIndirect(&logFont);
	}

	VERIFY(m_wndToolBar.SetButtonCtrl(m_wndToolBar.CommandToIndex(IDC_STATIC_WND_NUMBER), &m_wndLabelWndNumber));
	m_wndLabelWndNumber.SetFont(&m_fontWndNumber);
	m_wndLabelWndNumber.SetWindowText( _T("화면번호") );
	
	if(!m_wndEditWndFind.Create(WS_CHILD|WS_VISIBLE|ES_LEFT|ES_AUTOHSCROLL, CRect(0,0,100,25),&m_wndToolBar, ID_TOOLEDIT_WND_FIND))
	{
		TRACE0("Failed to create ID_TOOLEDIT_WND_FIND\n");
		return -1;      // fail to create
	}
	VERIFY(m_wndToolBar.SetButtonCtrl(m_wndToolBar.CommandToIndex(ID_TOOLEDIT_WND_FIND), &m_wndEditWndFind));
	m_wndToolBar.GetButton(m_wndToolBar.CommandToIndex(ID_TOOLEDIT_WND_FIND))->SetCtrlVisibleVertically(TRUE);
	m_wndEditWndFind.SetFont(&m_fontWndNumber);
	m_wndEditWndFind.SetCueBanner(_T("화면번호"));
	m_wndToolBar.EnableToolTips(TRUE);
#else
	m_wndToolBar.RemoveButton(m_wndToolBar.CommandToIndex(IDC_STATIC_WND_NUMBER));
	m_wndToolBar.RemoveButton(m_wndToolBar.CommandToIndex(ID_TOOLEDIT_WND_FIND));
#endif

#ifndef _DEBUG
	m_wndToolBar.RemoveButton(m_wndToolBar.CommandToIndex(ID_SYSTEM_LOG));
#endif

#if 0
	if((!m_wndStatusBar.Create(this)))// || (!m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT))))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
#endif

    m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndMenuBar.m_bAllowProfMainFrameSysmenu = false;
	m_wndMenuBar.m_bAllowProfChildFrameSysmenu = false;

	if( ! CExtControlBar::FrameEnableDocking(this) )
	{
		ASSERT( FALSE );
		return -1;
	}
	
	VERIFY(g_CmdManager->SetBasicCommands(pApp->m_pszProfileName, g_statBasicCommands));

	if(	! CExtControlBar::ProfileBarStateLoad(
			this,
			pApp->m_pszRegistryKey,
			pApp->m_pszProfileName,
			pApp->m_pszProfileName,
			&m_dataFrameWP
			)
		)
	{
		DockControlBar( &m_wndMenuBar );
		DockControlBar( &m_wndToolBar );
		RecalcLayout();
	}

	/*g_CmdManager->SerializeState(
		pApp->m_pszProfileName,
		pApp->m_pszRegistryKey,
		pApp->m_pszProfileName,
		false
		);*/
	

#ifdef _DEBUG
//	OnRibbonCommands(ID_SYSTEM_LOG);
#else
	
#endif
	return 0;
}


// CMainFrame 메시지 처리기
void CMainFrame::LoginSuccess()
{
	tClientLevelType level = GetSession()->ClientLevelTypeID;

	if(clt_partner1 != level && clt_partner2 != level)
	{
		CMenu* pMenu = m_wndMenuBar.GetMenu();
		ASSERT(pMenu);
		pMenu->RemoveMenu(4, MF_BYPOSITION); 
		//m_wndMenuBar.RemoveButton(m_wndMenuBar.CommandToIndex(IDM_FAST_TRADING));
		m_wndToolBar.RemoveButton(m_wndToolBar.CommandToIndex(ID_RECOMMENDER));

		/*m_wndToolBar.RemoveButton(m_wndToolBar.CommandToIndex(ID_RCMD_MEMBERS_PROFIT));
		m_wndToolBar.RemoveButton(m_wndToolBar.CommandToIndex(ID_RCMD_PERIOD_PROFIT));
		m_wndToolBar.RemoveButton(m_wndToolBar.CommandToIndex(ID_RCMD_MEMBER_ACTION_LOG));*/
	}
	else
	{
		CMenu _menu;
		VERIFY( _menu.LoadMenu(IDR_MENU_RECOMMENDER));
		VERIFY(	m_wndToolBar.SetButtonMenu(m_wndToolBar.CommandToIndex(ID_RECOMMENDER),_menu.Detach(),TRUE));

	}
}


CChildFrame* CMainFrame::CreateChildFrame(int nChildViewID, int x, int y)
{
	CChildFrame* pFrame = (CChildFrame*)(RUNTIME_CLASS(CChildFrame)->CreateObject());
	pFrame->PreCreateView(nChildViewID, x, y);
	
	// load the frame
	CCreateContext context;
	context.m_pCurrentFrame = this;

	pFrame->SetHandles(NULL, NULL);
	if (!pFrame->LoadFrame(IDR_MAINFRAME,
			WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, &context))
	{
		TRACE(traceAppMsg, 0, "Couldn't load frame window.\n");
		return NULL;
	}

	CString strFullString, strTitle;
	if (strFullString.LoadString(IDR_MAINFRAME))
		AfxExtractSubString(strTitle, strFullString, CDocTemplate::docName);

	// redraw the frame and parent
	pFrame->SetTitle(strTitle);
	pFrame->InitialUpdateFrame(NULL, TRUE);
	pFrame->CreateView();

	m_children.push_back(make_pair(nChildViewID, pFrame->GetChildWnd()));
	return pFrame;
}

void CMainFrame::OnRibbonCommands(UINT nID)
{

#define MENU_2_TOOL(_n_) case IDM_##_n_: nID = ID_##_n_; break;
	switch(nID)
	{
	MENU_2_TOOL(FAST_TRADING            );
	MENU_2_TOOL(BANKING_WITHDRAW        );
	MENU_2_TOOL(BANKING_DEPOSIT         );
	MENU_2_TOOL(EACH_DAY_PROFIT         );
	MENU_2_TOOL(ORDER_HISTORY           );
	MENU_2_TOOL(BANKING_IO_HISTORY      );
	MENU_2_TOOL(USER_ACTION_LOG         );
	MENU_2_TOOL(NOTICES                 );
	MENU_2_TOOL(PREFERENCES             );
	MENU_2_TOOL(OPTIONS_TABLE           );
	MENU_2_TOOL(RCMD_MEMBERS_PROFIT     );
	MENU_2_TOOL(RCMD_PERIOD_PROFIT      );
	MENU_2_TOOL(RCMD_MEMBER_ACTION_LOG  );
	MENU_2_TOOL(DUMMY                   );
	MENU_2_TOOL(ORDER_ACTION_LOG        );
	MENU_2_TOOL(SYSTEM_LOG              );
	}


	int nPosX = 0, nPosY = 0;
	switch(nID)
	{
	case ID_EDIT_WINDOWS_NUMBER			    :
		break	;						   
	case ID_NOTICES				    :
		if(NULL == m_pChildNotices)
			m_pChildNotices = CreateChildFrame(nID, 500, 10);
		else
			m_pChildNotices->ShowWindow(m_pChildNotices->IsWindowVisible() ? SW_HIDE : SW_SHOW);
		return;
	case ID_SYSTEM_LOG			    :
		if(NULL == m_pChildLog )
		{
			m_pChildLog = CreateChildFrame(nID, 10, 400);
			InstLog().SetNotifyFnc(Logger::NotifyFnc(dynamic_cast<CLogDlg*>(m_pChildLog->GetChildWnd()), &CLogDlg::OnLog));
		}
		else
			m_pChildLog->ShowWindow(m_pChildLog->IsWindowVisible() ? SW_HIDE : SW_SHOW);
		return;
	case ID_PREFERENCES			    :
		{
			CPreferencesDlg dlg;
			dlg.DoModal();
		}
		return;
	case ID_FAST_TRADING		    :
	case ID_ORDER_HISTORY		    :
	case ID_EACH_DAY_PROFIT		    :
	case ID_BANKING_IO_HISTORY		:
	case ID_USER_ACTION_LOG		    :
	case ID_BANKING_WITHDRAW	    :
	case ID_BANKING_DEPOSIT		    :
	case ID_OPTIONS_TABLE		    :
	case ID_RCMD_MEMBERS_PROFIT	    :
	case ID_RCMD_PERIOD_PROFIT	    :
	case ID_RCMD_MEMBER_ACTION_LOG  :
	case ID_ORDER_ACTION_LOG        :
		CreateChildFrame(nID);
		return;
	default:
		ASSERT(FALSE);
	}
}
void CMainFrame::OnUpdateRibbonCommands(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

LRESULT CMainFrame::OnNoticesReceived(WPARAM wParam, LPARAM lParam)
{
	OnRibbonCommands(ID_NOTICES);
	return 0;
}	

LRESULT CMainFrame::OnNewNotice(WPARAM wParam, LPARAM lParam)
{
	if(NULL == m_pChildNotices)
		OnRibbonCommands(ID_NOTICES);
	else
	{
		m_pChildNotices->GetChildWnd()->SendMessage(WM_NEW_NOTICE);
	}
	return 0;
}	

LRESULT CMainFrame::OnStandaloneWndToMDIChild(WPARAM wParam, LPARAM lParam)
{
	CStandaloneWnd* pStdWnd = (CStandaloneWnd*)wParam;
	CChildFrame* pFrame = (CChildFrame*)CreateNewChild(RUNTIME_CLASS(CChildFrame), IDR_MAINFRAME);
	pFrame->SetFromStandaloneWnd(pStdWnd);
	
	pFrame->ShowWindow(SW_SHOW);
	return 0;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if( m_wndMenuBar.TranslateMainFrameMessage( pMsg ) )
		return TRUE;

	return MainFrameBase::PreTranslateMessage(pMsg);
}

BOOL CMainFrame::DestroyWindow()
{
	InstLog().SetNotifyFnc(Logger::NotifyDummy);

	CWinApp * pApp = ::AfxGetApp();
	
	VERIFY(CExtControlBar::ProfileBarStateSave(this, pApp->m_pszRegistryKey, pApp->m_pszProfileName, pApp->m_pszProfileName));
	VERIFY(g_CmdManager->SerializeState(pApp->m_pszProfileName, pApp->m_pszRegistryKey, pApp->m_pszProfileName, true));
	VERIFY(g_PaintManager.PaintManagerStateSave(pApp->m_pszRegistryKey, pApp->m_pszProfileName, pApp->m_pszProfileName));

	g_CmdManager->ProfileWndRemove( GetSafeHwnd() );

	return MainFrameBase::DestroyWindow();
}


void CMainFrame::ActivateFrame(int nCmdShow)
{
	if( m_dataFrameWP.showCmd != SW_HIDE )
	{
		SetWindowPlacement( &m_dataFrameWP );
		CExtNCW < CMDIFrameWnd > :: ActivateFrame( m_dataFrameWP.showCmd );
		m_dataFrameWP.showCmd = SW_HIDE;
		return;
	}
	MainFrameBase::ActivateFrame(nCmdShow);
}

BEGIN_MESSAGE_MAP(CToolbarEdit, CExtEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()


void CToolbarEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(nChar == VK_RETURN)
	{
		int a = 0;
		return;
	}
	CExtEdit::OnChar(nChar, nRepCnt, nFlags);
}

LRESULT CMainFrame::OnVTSMDIChildFrameClosed(WPARAM wParam, LPARAM lParam)
{
	CChildFrame* pChild = (CChildFrame*)wParam;
	
	if(NULL != pChild->GetChildWnd())
	{
		auto pos = std::find(m_children.begin(), m_children.end(), 
			make_pair(pChild->GetParam().nWndID, pChild->GetChildWnd()));
		if(pos != m_children.end())
		{
			if(m_pChildNotices == pChild)
				m_pChildNotices = NULL;

			m_children.erase(pos);
		}
	}
	return 0;
}

LRESULT CMainFrame::OnStandaloneWndClosed(WPARAM wParam, LPARAM lParam)
{
	CStandaloneWnd* pChild = (CStandaloneWnd*)wParam;
	if(NULL == pChild->GetChildWnd())
	{
		//MDIChild로 옮겨지는것이다.
	}
	else
	{
		//Main밖에서 닫혀지는 경우
		auto pos = std::find(m_children.begin(), m_children.end(), 
			make_pair(pChild->GetParam().nWndID, pChild->GetChildWnd()));
		if(pos != m_children.end())
		{
			m_children.erase(pos);
		}
	}

	return 0;
}

LRESULT CMainFrame::OnPreferencesChanged(WPARAM wParam, LPARAM lParam)
{
	for(auto pos = m_children.begin() ; pos != m_children.end() ; ++pos)
	{
		pos->second->SendMessage(WM_PREFERENCES_CHANGED, wParam, lParam);
	}

	return 0;
}

LRESULT CMainFrame::OnCodeListReceived(WPARAM wParam, LPARAM lParam)
{
	OnRibbonCommands(ID_FAST_TRADING);
	return 0;
}


LRESULT CMainFrame::OnExtMenuPrepare(WPARAM wParam, LPARAM lParam)
{
	wParam;
	lParam;
	
	//////////////////////////////////////////////////////////////////////////
	// Add "Windows..." command
	//////////////////////////////////////////////////////////////////////////
	
	CExtPopupMenuWnd::MsgPrepareMenuData_t * pData =
		reinterpret_cast
		< CExtPopupMenuWnd::MsgPrepareMenuData_t * >
		( wParam );
	ASSERT( pData != NULL );
	CExtPopupMenuWnd * pPopup = pData->m_pPopup;
	ASSERT( pPopup != NULL );
	
	INT nItemPos;
	INT nNewPos = -1;
	
	nItemPos = pPopup->ItemFindPosForCmdID( __ID_MDIWND_DLGWINDOWS );
	if( nItemPos > 0 )
	{
		// "More Windows..." command found
		pPopup->ItemRemove( nItemPos );
		nNewPos = nItemPos;
	}
	else
	{
		int nMaxCmdID = 0;
		for( int nCmdID = __ID_MDIWNDLIST_FIRST; nCmdID <= __ID_MDIWNDLIST_LAST; nCmdID++ ){
			nItemPos = pPopup->ItemFindPosForCmdID( nCmdID );
			if( nItemPos > 0 ){
				if( nCmdID > nMaxCmdID ){
					nMaxCmdID = nCmdID;
					nNewPos = nItemPos;
				}
			}
		}
		if( nNewPos > 0 ){
			pPopup->ItemInsert(
				(UINT)CExtPopupMenuWnd::TYPE_SEPARATOR,
				++nNewPos
				);
			nNewPos++;
		}
	}
	if( nNewPos > 0 )
	{
		UINT nCmdID = ID_WINDOWS_LIST;
		CExtCmdItem * pCmdItem =
			g_CmdManager->CmdGetPtr(
			g_CmdManager->ProfileNameFromWnd( this->GetSafeHwnd() ),
			nCmdID
			);
		if( pCmdItem == NULL ){
			pCmdItem = 
				g_CmdManager->CmdAllocPtr( 
				g_CmdManager->ProfileNameFromWnd( this->GetSafeHwnd() ), 
				nCmdID 
				);
		}
		ASSERT( pCmdItem != NULL );
		if( pCmdItem != NULL )
		{
			CExtSafeString sMoreWindows(_T("Windows..."));
			CExtSafeString sManageWindows(_T("Manages the currently open windows"));
			pCmdItem->m_sMenuText = sMoreWindows;
			pCmdItem->m_sToolbarText = pCmdItem->m_sMenuText;
			pCmdItem->m_sTipTool = sManageWindows;
			pCmdItem->m_sTipStatus = pCmdItem->m_sTipTool;
			pCmdItem->StateSetBasic( true );
			
			pPopup->ItemInsert(
				nCmdID,
				nNewPos,
				sMoreWindows,
				NULL,
				m_hWnd
				);
		}
	}
	return 1;
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode,void* pExtra,AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if( nCode == CN_COMMAND )
	{
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)

#ifndef __EXT_MFC_NO_MDI_WINDOWS_LIST_DIALOG            
		if( nID == ID_WINDOWS_LIST )
		{
			CExtMdiWindowsListDlg dlg( this );
			dlg.DoModal();
		}
#endif // #ifndef __EXT_MFC_NO_MDI_WINDOWS_LIST_DIALOG           
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
	}
	return CMDIFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}




void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	if(theApp.IsManagerMode())
	{
		AfxSetWindowText(m_hWnd, _T("Virtual Trading System(주의!! 고객 화면입니다)"));
	}
	else
	{
		CMDIFrameWnd::OnUpdateFrameTitle(bAddToTitle);
	}
	
}

LRESULT CMainFrame::OnServerDisconnected(WPARAM wParam, LPARAM lParam)
{
	if(::IsWindow(GetSafeHwnd()))
	{
		ErrMsgBox(_T("서버와의 접속이 끊겼습니다."));
	}
	PostQuitMessage(0);
	return 0;
}


#endif //#ifdef _MDI_