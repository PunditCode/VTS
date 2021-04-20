// MainDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VTS.h"
#include "MainDlg.h"

#include <LogDlg.h>

#include "PreferencesDlg.h"
#include "ChangePasswordDlg.h"
#include "ServerEventReceiver.h"
#include "OvernightRequestDlg.h"

// CMainDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMainDlg, CExtNCW<CExtResizableDialog>)

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CExtNCW<CExtResizableDialog>(CMainDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetAutoSubclassChildControls();

	m_pWndNotices = NULL;
	m_pWndLog = NULL;
}

CMainDlg::~CMainDlg()
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CExtNCW<CExtResizableDialog>::DoDataExchange(pDX);
	DDX_Control( pDX, IDC_MENU_BAR, m_wndMenuBar );
	DDX_Control( pDX, IDC_TOOL_BAR, m_wndToolBar );
	
	DDX_Control( pDX, IDC_STATIC1, m_label1);
	DDX_Control( pDX, IDC_STATIC_KOSPI, m_labelKospi);
	DDX_Control( pDX, IDC_STATIC2, m_label2);
	DDX_Control( pDX, IDC_STATIC_USD, m_labelUSD);
}


BEGIN_MESSAGE_MAP(CMainDlg, CExtResizableDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_MESSAGE(WM_LOGIN_COMPLETED, &CMainDlg::OnLoginCompleted)
	ON_COMMAND_RANGE(IDC_STATIC_WND_NUMBER, ID_TOOLBAR_BUTTON_RESERVED, &CMainDlg::OnCommandAll)
	ON_MESSAGE(WM_NOTICES_RECEIVED, &CMainDlg::OnNoticesReceived)
	ON_MESSAGE(WM_NEW_NOTICE, &CMainDlg::OnNewNotice)
	ON_MESSAGE(WM_PREFERENCES_CHANGED, &CMainDlg::OnPreferencesChanged)
	ON_MESSAGE(WM_SERVER_DISCONNECTED, &CMainDlg::OnServerDisconnected)
	ON_MESSAGE(WM_CHILD_WND_CLOSED, &CMainDlg::OnChildWndClosed)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_ORDER_RESULT, &CMainDlg::OnOrderResult)
	ON_MESSAGE(WM_MARKET_EYE, &CMainDlg::OnMarketEye)
	ON_MESSAGE(WM_RECONNECTED_TO_SERVER, &CMainDlg::OnReconnectedToServer)
END_MESSAGE_MAP()


// CMainDlg 메시지 처리기입니다.
static UINT g_statBasicCommands[] =
{
//	ID_APP_ABOUT,
//	ID_APP_EXIT,
//	ID_VIEW_MENUBAR,
//	IDR_TOOLBAR_MAIN,
	IDM_FAST_TRADING                ,
	IDM_BANKING_WITHDRAW            ,
	IDM_BANKING_DEPOSIT             ,
	IDM_BANKING_IO_HISTORY          ,
	IDM_NOTICES                     ,
	IDM_PREFERENCES                 ,
	IDM_RCMD_MEMBERS_PROFIT         ,
	IDM_RCMD_PERIOD_PROFIT          ,
	IDM_RCMD_MEMBER_ACTION_LOG      ,
	IDM_RCMD_CLIENT_INFO,
	IDM_RCMD_CLIENT_TOTAL_PROFIT,
	IDM_ORDER_ACTION_LOG            ,
	IDM_ORDER_HISTORY,
	IDM_EACH_DAY_PROFIT,
	IDM_OVERNIGHT_REQUEST,
	IDM_CHANGE_PASSWORD,

//	ID_VIEW_STATUS_BAR,
	0, // end of list
};

int CMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CExtNCW<CExtResizableDialog>::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}



BOOL CMainDlg::OnInitDialog()
{
	CExtNCW<CExtResizableDialog>::OnInitDialog();
	
	SetIcon( m_hIcon, TRUE );
	SetIcon( m_hIcon, FALSE );
	
#if 0
	m_fontStatic.CreateFont(16, 8, 0, 0, 590, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("굴림"));

	m_label1.SetFont(&m_fontStatic);
	m_labelKospi.SetFont(&m_fontStatic);
	m_label2.SetFont(&m_fontStatic);
	m_labelUSD.SetFont(&m_fontStatic);
#endif

	m_label1.SetTextColor(true, COLOR_FONT_NORMAL);
	m_label2.SetTextColor(true, COLOR_FONT_NORMAL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

LRESULT CMainDlg::OnLoginCompleted(WPARAM wParam, LPARAM lParam)
{
	CWinApp * pApp = ::AfxGetApp();
	VERIFY( g_CmdManager->ProfileWndAdd( pApp->m_pszProfileName, GetSafeHwnd() ) );

	tClientLevelType level = GetSession()->ClientLevelTypeID;

	if(	! m_wndMenuBar.LoadMenuBar( IDR_MAIN_MENU) )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	m_wndMenuBar._FlatTrackingSet(true);
	m_wndMenuBar.SetBarStyle( m_wndMenuBar.GetBarStyle() & (~CBRS_GRIPPER) );



	if (!m_wndToolBar.LoadToolBar(IDR_TOOLBAR_DSVIP))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
#ifndef _DEBUG
	m_wndToolBar.RemoveButton(m_wndToolBar.CommandToIndex(ID_SYSTEM_LOG));
#endif

	
	if(clt_partner1 != level && clt_partner2 != level)
	{
		CMenu* pMenu = m_wndMenuBar.GetMenu();
		ASSERT(pMenu);
		pMenu->RemoveMenu(4, MF_BYPOSITION); 
		m_wndMenuBar.UpdateMenuBar();
		m_wndToolBar.RemoveButton(m_wndToolBar.CommandToIndex(ID_RECOMMENDER));
	}
	else
	{
		CMenu _menu;
		VERIFY( _menu.LoadMenu(IDR_MENU_RECOMMENDER));
		VERIFY(	m_wndToolBar.SetButtonMenu(m_wndToolBar.CommandToIndex(ID_RECOMMENDER),_menu.Detach(),TRUE));
	}


	m_wndToolBar.SetBarStyle( m_wndToolBar.GetBarStyle() & (~CBRS_GRIPPER) );

	VERIFY( g_CmdManager->SetBasicCommands( pApp->m_pszProfileName, g_statBasicCommands ) );
	
//	EnableSaveRestore( _T("Dialog Positions"), _T("MainDlg") );
	CWnd::RepositionBars( 0, 0xFFFF, 0 );

	
	m_wndTaskbarNotf.Create(GetDesktopWindow());
	m_wndTaskbarNotf.SetSkin(IDB_TASKBAR_ALARM);
	m_wndTaskbarNotf.SetTextFont(_T("굴림"),90,TN_TEXT_NORMAL,TN_TEXT_UNDERLINE);
 	m_wndTaskbarNotf.SetTextColor(RGB(0,0,0),RGB(0,0,200));
	m_wndTaskbarNotf.SetTextRect(CRect(10,40,m_wndTaskbarNotf.m_nSkinWidth-10,m_wndTaskbarNotf.m_nSkinHeight-25));


	CRect rtWnd;
	GetWindowRect(rtWnd);

	CRect rtClient;
	GetClientRect(rtClient);

	CString strPath = ENV().GetRoot() + _T("\\settings\\WndPos.ini");
	CStdioFile file;
	if(file.Open(strPath, CFile::modeRead))
	{
		CString s;
		CString strWnd, strPos;
		while(file.ReadString(s))
		{
			if(!::ParseLine(s, strWnd, strPos, _T('=')))
				break;

			Tokenizer tok(strPos, _T(";"));
			if(tok.size() != 4)
				break;

			CRect rect(::ToINT(tok[0]), ::ToINT(tok[1]), ::ToINT(tok[2]), ::ToINT(tok[3]));

			strWnd.MakeUpper();
			if(strWnd == _T("MAIN"))
			{
				SetWindowPos(NULL, rect.left, rect.top, rtWnd.Width(), rtWnd.Height(), SWP_NOZORDER);
			}
			else
			{
				int nWndID = ::ToINT(strWnd);
				if (nWndID == ID_FAST_TRADING)
					ShowChildWindow(nWndID, rect.left, rect.top);
			}
		}
		file.Close();
	}
	else
	{
		SetWindowPos(NULL, 10, 10, rtWnd.Width(), rtWnd.Height(), SWP_NOZORDER );
		ShowChildWindow(ID_FAST_TRADING, 10, 10 + rtWnd.Height() + 10);
	}

#ifdef _DEBUG
	//ShowChildWindow(ID_SYSTEM_LOG);
#endif

	SetWindowText(theApp.m_pszAppName);
	return 0;
}


void CMainDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CExtNCW < CExtResizableDialog > :: OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMainDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMainDlg::OnSize(UINT nType, int cx, int cy) 
{
	CExtNCW < CExtResizableDialog > :: OnSize( nType, cx, cy );

	if( nType != SIZE_MINIMIZED )
		CWnd::RepositionBars(0,0xFFFF,0);
}


void CMainDlg::OnCommandAll(UINT nID)
{
#define MENU_2_TOOL(_n_) case IDM_##_n_: nID = ID_##_n_; break;
	switch(nID)
	{
		MENU_2_TOOL(FAST_TRADING);
		MENU_2_TOOL(BANKING_WITHDRAW);
		MENU_2_TOOL(BANKING_DEPOSIT);
		MENU_2_TOOL(EACH_DAY_PROFIT);
		MENU_2_TOOL(ORDER_HISTORY);
		MENU_2_TOOL(BANKING_IO_HISTORY);
		MENU_2_TOOL(USER_ACTION_LOG);
		MENU_2_TOOL(NOTICES);
		MENU_2_TOOL(PREFERENCES);
		MENU_2_TOOL(OPTIONS_TABLE);
		MENU_2_TOOL(RCMD_MEMBERS_PROFIT);
		MENU_2_TOOL(RCMD_PERIOD_PROFIT);
		MENU_2_TOOL(RCMD_MEMBER_ACTION_LOG);
		MENU_2_TOOL(RCMD_CLIENT_INFO);
		MENU_2_TOOL(RCMD_CLIENT_TOTAL_PROFIT);
		MENU_2_TOOL(DUMMY);
		MENU_2_TOOL(ORDER_ACTION_LOG);
		MENU_2_TOOL(SYSTEM_LOG);
		MENU_2_TOOL(TRDSTD_1);
		MENU_2_TOOL(TRDSTD_2);
		MENU_2_TOOL(TRDSTD_3);
		MENU_2_TOOL(TRDSTD_4);
		MENU_2_TOOL(TRDSTD_5);
	}
	ShowChildWindow(nID);
}

void CMainDlg::ShowChildWindow(UINT nWndID, int x, int y)
{
	switch(nWndID)
	{
	case ID_NOTICES				    :
		if(NULL == m_pWndNotices)
		{
			CRect rect;
			GetWindowRect(rect);

			x = rect.right + 10;
			y = rect.top;

			m_pWndNotices = (CPopupWnd*)RUNTIME_CLASS(CPopupWnd)->CreateObject();
			m_pWndNotices->SetChildWnd(nWndID, x, y);
			m_pWndNotices->Create(NULL, _T(""));
			m_pWndNotices->ShowWindow(SW_SHOW);
		}
		else
			m_pWndNotices->ShowWindow(m_pWndNotices->IsWindowVisible() ? SW_HIDE : SW_SHOW);
		return;
#ifdef _DEBUG
	case ID_SYSTEM_LOG			    :
		if(NULL == m_pWndLog)
		{
			m_pWndLog = (CPopupWnd*)RUNTIME_CLASS(CPopupWnd)->CreateObject();
			m_pWndLog->SetChildWnd(nWndID, x , y);
			m_pWndLog->Create(NULL, _T(""));
			m_pWndLog->ShowWindow(SW_SHOW);
			InstLog().SetNotifyFnc(Logger::NotifyFnc(dynamic_cast<CLogDlg*>(m_pWndLog->GetChildWnd()), &CLogDlg::OnLog));
		}
		else
			m_pWndLog->ShowWindow(m_pWndLog->IsWindowVisible() ? SW_HIDE : SW_SHOW);
		return;
#endif
	case ID_PREFERENCES			    :
		{
			CPreferencesDlg dlg;
			dlg.DoModal();
		}
		return;
	case IDM_CHANGE_PASSWORD:
		{
			CChangePasswordDlg dlg;
			dlg.DoModal();
		}
		return;
	case ID_FAST_TRADING:
		{
		int nCount = 1;
		for (auto pos = m_wndPopups.begin(); pos != m_wndPopups.end(); ++pos)
		{
			if ((*pos)->GetWndID() == ID_FAST_TRADING)
			{
				++nCount;
#ifndef _DEBUG
				if (nCount > 3)
				{
					ErrMsgBox(_T("빠른주문창의 최대갯수는 3개입니다."));
					return;
				}
#endif
			}
		}
		CString s;
		s.Format(_T("빠른주문창실행(창갯수:%d)"), nCount);
		GetSession()->SendOrderAction(s);
		}
		break;
	case IDM_OVERNIGHT_REQUEST:
	{
		COvernightRequestDlg dlg;
		dlg.DoModal();
	}
		return;

	}

	
	CPopupWnd* pWnd = (CPopupWnd*)RUNTIME_CLASS(CPopupWnd)->CreateObject();
	pWnd->SetChildWnd(nWndID, x, y);

	if(pWnd->Create(NULL, _T("")))
	{
		pWnd->ShowWindow(SW_SHOW);
		m_wndPopups.push_back(pWnd);
	}
}

LRESULT CMainDlg::OnNoticesReceived(WPARAM wParam, LPARAM lParam)
{
	if(NULL == m_pWndNotices || FALSE == m_pWndNotices->IsWindowVisible())
		ShowChildWindow(ID_NOTICES);
	return 0;
}	

LRESULT CMainDlg::OnNewNotice(WPARAM wParam, LPARAM lParam)
{
	if(NULL == m_pWndNotices)
		ShowChildWindow(ID_NOTICES);
	else
	{
		m_pWndNotices->GetChildWnd()->SendMessage(WM_NEW_NOTICE);
		if(m_pWndNotices->IsIconic())
			m_pWndNotices->ShowWindow(SW_RESTORE);
		m_pWndNotices->SetForegroundWindow();
	}
	return 0;
}	
LRESULT CMainDlg::OnPreferencesChanged(WPARAM wParam, LPARAM lParam)
{
	for(auto pos = m_wndPopups.begin() ; pos != m_wndPopups.end() ; ++pos)
	{
		(*pos)->GetChildWnd()->SendMessage(WM_PREFERENCES_CHANGED, wParam, lParam);
	}

	return 0;
}


LRESULT CMainDlg::OnServerDisconnected(WPARAM wParam, LPARAM lParam)
{
	if(::IsWindow(GetSafeHwnd()))
	{
#ifndef _DEBUG
		::PlayWave(ENV().GetRoot() + _T("\\wave\\DISCONNECT.WAV"));
		ErrMsgBox(_T("서버와의 접속이 끊겼습니다."));
#endif
	}
	PostQuitMessage(0);
	return 0;
}


void CMainDlg::OnClose()
{
	CString strMsg;
	if (theApp.GetAutoOrderManager()->IsAnyMITRemain())
		strMsg = _T("현재 MIT주문이 남아있습니다. 종료하시곘습니까?");
	else
		strMsg = _T("종료하시겠습니까?");
	if (AfxMessageBox(strMsg, MB_YESNO) == IDNO)
		return;
//	SaveWindowRect();
	ClosePopups();
	PostQuitMessage(0);
//	CExtNCW<CExtResizableDialog>::OnClose();
}


void CMainDlg::PostNcDestroy()
{
	
	ClosePopups();
	CExtNCW<CExtResizableDialog>::PostNcDestroy();
}

void CMainDlg::ClosePopups()
{
	if(::IsWindow(GetSafeHwnd()))
	{
		CString strPath = ENV().GetRoot() + _T("\\settings\\WndPos.ini");
		CStdioFile file;
		if(file.Open(strPath, CFile::modeCreate|CFile::modeReadWrite))
		{
			CRect rect;
			GetWindowRect(rect);

			CString s;
			s.Format(_T("MAIN=%d;%d;%d;%d\n"), rect.left, rect.top, rect.right, rect.bottom);
			file.WriteString(s);

			for(auto pos = m_wndPopups.begin() ; pos != m_wndPopups.end() ; ++pos)
			{
				CPopupWnd* pWnd = *pos;

				int nID = pWnd->GetWndID();

				if (nID != ID_FAST_TRADING)
					continue;
				
				if(::IsWindow(pWnd->GetSafeHwnd()))
				{
					pWnd->GetWindowRect(rect);
					int nID = pWnd->GetWndID();

					s.Format(_T("%d=%d;%d;%d;%d\n"), nID, rect.left, rect.top, rect.right, rect.bottom);
					file.WriteString(s);
				}
			} 
		}
		file.Close();
	}

	while(true)
	{
		if(m_wndPopups.empty())
			break;
		
		CPopupWnd* pWnd = *m_wndPopups.begin();
		m_wndPopups.pop_front();
		pWnd->DestroyWindow();
	}

	if(m_pWndNotices)
		m_pWndNotices->DestroyWindow();
	if(m_pWndLog)
		m_pWndLog->DestroyWindow();
}
void CMainDlg::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	return;
	CExtNCW<CExtResizableDialog>::OnCancel();
}


void CMainDlg::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	return;
	CExtNCW<CExtResizableDialog>::OnOK();
}

LRESULT CMainDlg::OnChildWndClosed(WPARAM wParam, LPARAM lParam)
{
	CPopupWnd* pChild = (CPopupWnd*)wParam;

	if(m_pWndNotices == pChild)
		m_pWndNotices = NULL;
#ifdef _DEBUG
	else if(m_pWndLog == pChild)
		m_pWndLog = NULL;
#endif
	else
	{
		auto pos = std::find(m_wndPopups.begin(), m_wndPopups.end(), pChild);
		if(pos != m_wndPopups.end())
		{
			m_wndPopups.erase(pos);
		}

	}
	return 0;
}

LRESULT CMainDlg::OnOrderResult(WPARAM wParam, LPARAM lParam)
{
	OrderResult* pResult = (OrderResult*)wParam;

	CString strAlarm;
	CString strWaveFile;
	switch(pResult->orderStatus)
	{
	case OS_NEW:
		if(pResult->position == PT_SELL)
		{
			if(ENV().Get(ENV_VOICE_SELL_ORDER).ToINT() == 1)
				strWaveFile = _T("SELL_RECEIPT.WAV");

			strAlarm.Format(_T("매도주문접수\r\n%s 수량 %d개"), pResult->pCode->GetCode(), pResult->nVolume); 
		}
		else
		{
			if(ENV().Get(ENV_VOICE_BUY_ORDER).ToINT() == 1)
				strWaveFile = _T("BUY_RECEIPT.WAV");

			strAlarm.Format(_T("매수주문접수\r\n%s 수량 %d개"), pResult->pCode->GetCode(), pResult->nVolume); 
		}
		break;
	case OS_CORRECTION:
		if((pResult->position == PT_SELL && ENV().Get(ENV_VOICE_SELL_CORRECTION).ToINT() == 1) ||
			(pResult->position == PT_BUY && ENV().Get(ENV_VOICE_BUY_CORRECTION).ToINT() == 1))
			strWaveFile = _T("MODIFY_RECEIPT.WAV");

		strAlarm.Format(_T("정정주문접수\r\n%s 수량 %d개"), pResult->pCode->GetCode(), pResult->nVolume); 
		break;
	case OS_CANCEL:
		if((pResult->position == PT_SELL && ENV().Get(ENV_VOICE_SELL_CANCEL).ToINT() == 1) ||
			(pResult->position == PT_BUY && ENV().Get(ENV_VOICE_BUY_CANCEL).ToINT() == 1))
			strWaveFile = _T("CANCEL_RECEIPT.WAV");
		strAlarm.Format(_T("취소주문접수\r\n%s 수량 %d개"), pResult->pCode->GetCode(), pResult->nVolume); 
		break;
	case OS_CONTRACT:
		if(pResult->position == PT_SELL)
		{
			if(ENV().Get(ENV_VOICE_SELL_CONTRACT).ToINT() == 1)
				strWaveFile = _T("SELL_CONTRACT.WAV");

			strAlarm.Format(_T("매도주문체결\r\n%s 수량 %d개"), pResult->pCode->GetCode(), pResult->nVolume); 
		}
		else
		{
			if(ENV().Get(ENV_VOICE_BUY_CONTRACT).ToINT() == 1)
				strWaveFile = _T("BUY_CONTRACT.WAV");
			strAlarm.Format(_T("매수주문체결\r\n%s 수량 %d개"), pResult->pCode->GetCode(), pResult->nVolume); 
		}
		break;
	case OS_REJECT:
	{
		if (0 == lParam)
		{
			ErrMsgBox(pResult->strMsg);
		}
	}
	return 0;
	default:
		ASSERT(FALSE);
	}

	if(FALSE == strWaveFile.IsEmpty())
		::PlayWave(ENV().GetRoot() + _T("\\wave\\") + strWaveFile);
	if(FALSE == strAlarm.IsEmpty() && ENV().Get(ENV_CONTRACT_POPUP_SHOW).ToINT() == 1)
		m_wndTaskbarNotf.Show(strAlarm);

	return 0;
}


LRESULT CMainDlg::OnMarketEye(WPARAM wParam, LPARAM lParam)
{
	MarketEye* pME = (MarketEye*)wParam;
	if (NULL == pME)
	{
		ASSERT(FALSE);
		return 0;
	}

	{
		CurrentJisu& kospi = pME->kospi;
		CompareStateItem compItem = CompareStateColorFromXing(kospi.sign);
		COLORREF cr = get<0>(compItem);
		m_labelKospi.SetTextColor(true, cr);
		CString s;
		s.Format(_T("%s %s%s %s%%"), kospi.jisu, get<2>(compItem), kospi.change, kospi.diff);
		m_labelKospi.SetWindowText(s);
	}

	{
		CurrentJisu& USD = pME->USD;
		CompareStateItem compItem = CompareStateColorFromXing(USD.sign);
		COLORREF cr = get<0>(compItem);
		m_labelUSD.SetTextColor(true, cr);
		CString s;
		s.Format(_T("%s %s%s %s%%"), USD.jisu, get<2>(compItem), USD.change, USD.diff);
		m_labelUSD.SetWindowText(s);
	}
	delete pME;
	return 0;
}

afx_msg LRESULT CMainDlg::OnReconnectedToServer(WPARAM wParam, LPARAM lParam)
{
	SER()->ReRegisterCurrentHoga();
	return 0;
}