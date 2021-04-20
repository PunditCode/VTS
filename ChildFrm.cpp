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

// ChildFrm.cpp : CChildFrame Ŭ������ ����
//

#include "stdafx.h"
#include "ChildFrm.h"

#ifdef _MDI_

#include "VTS.h"
#include <StandaloneWnd.h>
#include <LogDlg.h>
#include "FastTradeDlg.h"
#include "BankingIORequestPage.h"
#include "NoticesDlg.h"
#include "OrderHistoryDlg.h"
#include "EachdayProfitDlg.h"
#include "BankingHistoryDlg.h"
#include "ClientActionLogDlg.h"
#include "NoticesDlg.h"
#include "OptionsTableDlg.h"
#include "BankingIORequestDlg.h"
#include "RecommenderMembersProfitDlg.h"
#include "RecommenderEachdayProfitDlg.h"
#include "RecommenderMembersActionLogDlg.h"
#include "OrderActionLogDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HWND CChildFrame::m_TopWindow = NULL;	// There can only be one top-most window

// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, MDIChildWndBase)

BEGIN_MESSAGE_MAP(CChildFrame, MDIChildWndBase)
	ON_COMMAND(ID_FILE_CLOSE, &CChildFrame::OnFileClose)
	ON_WM_SIZING()
	ON_WM_SIZE()
	ON_COMMAND(ID_COMMAND_TO_STANDALONE_WND, &CChildFrame::OnToStandaloneWindow)
	ON_WM_SYSCOMMAND()
	ON_MESSAGE(WM_VTSFRM_CHILD_RESIZE_REQUEST, &CChildFrame::OnChildResizeRequest)
	ON_MESSAGE(WM_VTSFRM_SET_TOP_MOST, &CChildFrame::OnSetTopMost)
	ON_WM_NCRBUTTONDOWN()
	ON_REGISTERED_MESSAGE(CExtPopupMenuWnd::g_nMsgPrepareMenu, OnExtMenuPrepare)
	ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

// CChildFrame ����/�Ҹ�


CChildFrame::CChildFrame()
{
	m_nCaptionHeight = 0;
	m_nFrameThick = 0;
}

CChildFrame::~CChildFrame()
{
}

void CChildFrame::PreCreateView(int nViewID, int x, int y)
{
	m_p.nWndID = nViewID;
	//m_p.xInit = 50;
	//m_p.yInit = 10;


	switch(nViewID)
	{
	case ID_FAST_TRADING		    :
		m_p.xInit = 100;
		m_p.yInit = 10;
		m_p.bMaximizeBox = FALSE;
		break;
	case ID_BANKING_WITHDRAW	    :
	case ID_BANKING_DEPOSIT		    :
	case ID_PREFERENCES			    :
		m_p.bMaximizeBox = FALSE;
		break;
	case ID_SYSTEM_LOG              :
	case ID_NOTICES                 :
	case ID_ORDER_HISTORY		    :
	case ID_EACH_DAY_PROFIT		    :
	case ID_BANKING_IO_HISTORY		:
	case ID_USER_ACTION_LOG		    :
	case ID_OPTIONS_TABLE		    :
	case ID_RCMD_MEMBERS_PROFIT	    :
	case ID_RCMD_PERIOD_PROFIT	    :
	case ID_RCMD_MEMBER_ACTION_LOG  :
	case ID_ORDER_ACTION_LOG        :
		m_p.bMaximizeBox = TRUE;
		break;
	default:
		ASSERT(FALSE);
	}

	/*if(ID_SYSTEM_LOG == m_p.nWndID || ID_NOTICES == m_p.nWndID)
		m_p.bEnableStandalone = FALSE;*/
}


void CChildFrame::SetParam()
{
	switch(m_p.nWndID)
	{
	case ID_SYSTEM_LOG:
		m_p.strTitle = _T("�ý��� �α�");
		break;
	case ID_FAST_TRADING:
		m_p.strTitle.Format(_T("�����ֹ�(%s)"), GetSession()->id);
		m_p.bEnableWidthResize = FALSE;
		break;
	case ID_BANKING_DEPOSIT: 
		m_p.strTitle = _T("�Աݽ�û");
		m_p.bEnableWidthResize = FALSE;
		m_p.bEnableHeightResize = FALSE;
		break;
	case ID_BANKING_WITHDRAW: 
		m_p.strTitle = _T("��ݽ�û");
		m_p.bEnableWidthResize = FALSE;
		m_p.bEnableHeightResize = FALSE;
		break;
	case ID_EACH_DAY_PROFIT: 
		m_p.strTitle = _T("���� ��ȸ");
		break;

	case ID_ORDER_HISTORY: 
		m_p.strTitle = _T("�ŷ�����");
		break;
	
	case ID_BANKING_IO_HISTORY: 
		m_p.strTitle = _T("��/��ݳ��� ��ȸ");
		break;
	case ID_ORDER_ACTION_LOG        :
		m_p.strTitle = _T("�ֹ��α׳���");
		break;
	case ID_USER_ACTION_LOG: 
		m_p.strTitle = _T("���α� ����");
		break;

	case ID_NOTICES: 
		m_p.strTitle = _T("��������");
		break;
	case ID_RCMD_MEMBERS_PROFIT	    :
		m_p.strTitle = _T("��õȸ�� �Ⱓ�� ����");
		break;
	case ID_RCMD_PERIOD_PROFIT	    :
		m_p.strTitle = _T("��¥�� ��õȸ�� ��������");
		break;
	case ID_RCMD_MEMBER_ACTION_LOG  :
		m_p.strTitle = _T("��õȸ�� ���α�");
		break;
	
	default:
		ASSERT(FALSE);
	}

	if(theApp.IsManagerMode())
		m_p.strTitle += _T("(����!! �� ȭ���Դϴ�)");
}
CWnd* CChildFrame::GetView(int nViewID)
{
	CDialog* pDlg = NULL;
#define NEWDLG(_class_) pDlg = new _class_; pDlg->Create(_class_::IDD, this);

	switch(m_p.nWndID)
	{
	case ID_SYSTEM_LOG:
		NEWDLG(CLogDlg);
		break;
	case ID_FAST_TRADING:
		NEWDLG(CFastTradeDlg);
		break;
	case ID_BANKING_DEPOSIT: 
		pDlg = new CBankingIORequestDlg(biot_deposit); pDlg->Create(CBankingIORequestDlg::IDD, this);
		break;
	case ID_BANKING_WITHDRAW: 
		pDlg = new CBankingIORequestDlg(biot_withdraw); pDlg->Create(CBankingIORequestDlg::IDD, this);
		break;
	case ID_ORDER_HISTORY: 
		NEWDLG(COrderHistoryDlg);
		break;
	case ID_EACH_DAY_PROFIT: 
		NEWDLG(CEachdayProfitDlg);
		break;
	case ID_BANKING_IO_HISTORY: 
		NEWDLG(CBankingHistoryDlg);
		break;
	case ID_USER_ACTION_LOG: 
		NEWDLG(CClientActionLogDlg);
		break;
	case ID_NOTICES: 
		NEWDLG(CNoticesDlg);
		break;
	case ID_OPTIONS_TABLE:
		NEWDLG(COptionsTableDlg);
		break;
	case ID_RCMD_MEMBERS_PROFIT:
		NEWDLG(CRecommenderMembersProfitDlg);
		break;
	case ID_RCMD_PERIOD_PROFIT:
		NEWDLG(CRecommenderEachdayProfitDlg);
		break;
	case ID_RCMD_MEMBER_ACTION_LOG:
		NEWDLG(CRecommenderMembersActionLogDlg);
		break;
	case ID_ORDER_ACTION_LOG:
		NEWDLG(COrderActionLogDlg);
		break;
	}
	ASSERT(pDlg);
	return pDlg;
}




BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡�� Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.
	if( !MDIChildWndBase::PreCreateWindow(cs) )
		return FALSE;

	cs.x = m_p.xInit;
	cs.y = m_p.yInit;
	cs.cx = 1;
	cs.cy = 1;
	cs.style |= WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_SYSMENU;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	//cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (MDIChildWndBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(m_p.bEnableStandalone)
	{
		HMENU pSysMenu = ::GetSystemMenu(GetSafeHwnd(), FALSE);
		if (pSysMenu)
		{
			int n = 0;
			::InsertMenu(pSysMenu, n, MF_BYPOSITION | MF_STRING, ID_COMMAND_TO_STANDALONE_WND, _T("��������")); ++n;
			::InsertMenu(pSysMenu, n, MF_BYPOSITION | MF_SEPARATOR, 0, _T(""));
		}
	}

	return 0;
}

void CChildFrame::CreateView()
{
	ASSERT(m_p.nWndID != NUL_IDX);
	m_p.pWnd = GetView(m_p.nWndID);
	CHECK_RET(NULL != m_p.pWnd, );

	SetParam();
	m_strTitle = m_p.strTitle;

	SetWindowText(m_p.strTitle);
	CDocument* pDocument = GetActiveDocument();
	if( pDocument )
	{
		pDocument->SetTitle(m_p.strTitle);
	}
	

	CRect rtClient, rtWindow;

	GetClientRect(rtClient);
	GetWindowRect(rtWindow);

	m_nCaptionHeight = rtWindow.Height() - rtClient.Height();
	m_nFrameThick = (rtWindow.Width() - rtClient.Width()) / 2;

	m_p.pWnd->GetWindowRect(m_p.rtOrg);
	ScreenToClient(m_p.rtOrg);
		
	int nWidth = m_p.rtOrg.Width();
	int nHeight = m_p.rtOrg.Height();

	m_p.nMinWidth = nWidth + (m_nFrameThick * 2);
	m_p.nMinHeight = m_nCaptionHeight + nHeight;

	SetWindowPos(NULL, 0, 0, m_p.nMinWidth, m_p.nMinHeight, SWP_NOMOVE | SWP_NOZORDER );
	
	
	m_p.pWnd->MoveWindow(0, 0, m_p.rtOrg.Width(), m_p.rtOrg.Height(), FALSE);
	m_p.pWnd->ShowWindow(SW_SHOW);
	m_p.pWnd->PostMessage(WM_VTSFRM_SHOW_COMPLETED);
}
void CChildFrame::OnSize(UINT nType, int cx, int cy)
{
	if(m_p.pWnd)
	{
		int nDlgNewWidth = cx;
		int nDlgNewHeight = cy;

		//m_pCaption->MoveWindow(0, 1, nDlgNewWidth, m_nCaptionHeight);
		m_p.pWnd->MoveWindow(0, 0 , nDlgNewWidth, nDlgNewHeight);
	}
	MDIChildWndBase::OnSize(nType, cx, cy);
}

void CChildFrame::OnSizing(UINT fwSide, LPRECT pRect)
{
	if(m_p.pWnd)
	{
		if(m_p.bEnableWidthResize)
		{
			if(pRect->right - pRect->left < m_p.nMinWidth)
				pRect->right = pRect->left + m_p.nMinWidth;
		}
		else
		{
			pRect->right = pRect->left + m_p.nMinWidth;
		}

		if(m_p.bEnableHeightResize)
		{
			if(pRect->bottom - pRect->top < m_p.nMinHeight)
				pRect->bottom = pRect->top + m_p.nMinHeight;
		}
		else
		{
			pRect->bottom = pRect->top + m_p.nMinHeight;
		}

		int nDlgNewWidth = (pRect->right - pRect->left)  - (m_nFrameThick * 2);
		int nDlgNewHeight = (pRect->bottom - pRect->top) - m_nCaptionHeight;

		//m_pCaption->MoveWindow(0, 1, nDlgNewWidth, m_nCaptionHeight);
		m_p.pWnd->MoveWindow(0, 0 , nDlgNewWidth, nDlgNewHeight);
	}

	MDIChildWndBase::OnSizing(fwSide, pRect);
}

LRESULT CChildFrame::OnChildResizeRequest(WPARAM wParam, LPARAM lParam)
{
	int nWidth = (int)wParam;
	int nHeight = (int)lParam;
	
	m_p.nMinWidth = nWidth + (m_nFrameThick * 2);
		
	m_p.pWnd->MoveWindow(0, 0, nWidth, nHeight);
	SetWindowPos(NULL, 0, 0, m_p.nMinWidth, m_nCaptionHeight + nHeight, SWP_NOMOVE | SWP_NOZORDER );

	return 0;
}

void CChildFrame::OnToStandaloneWindow()
{
	CStandaloneWnd* pWnd = (CStandaloneWnd*)RUNTIME_CLASS(CStandaloneWnd)->CreateObject();

	GetWindowRect(pWnd->m_rtInit); //����â�� rect�� �⺻ũ����Ѵ�.
	pWnd->m_rtInit.right -= (m_nFrameThick * 2);
	pWnd->m_rtInit.bottom -= m_nCaptionHeight;

	// CStandaloneWnd���� �ٽ� ��� �Ǿ�� �Ѵ�.
	m_p.nMinWidth -= (m_nFrameThick * 2);
	m_p.nMinHeight -= m_nCaptionHeight;


	pWnd->SetParam(m_p);
	pWnd->Create(NULL, m_p.strTitle);

	m_p.pWnd->SetParent(pWnd);
	pWnd->ShowWindow(SW_SHOW);
	m_p.pWnd = NULL;

	AfxGetMainWnd()->SendMessage(WM_STANDALONE_WND_CREATED, (WPARAM)pWnd);
	
	MDIDestroy();
}

void CChildFrame::SetFromStandaloneWnd(CStandaloneWnd* pStdWnd)
{
	m_p = pStdWnd->m_p;
	m_p.pWnd->SetParent(this);
	pStdWnd->m_p.pWnd = NULL;
	m_strTitle = m_p.strTitle;
	SetWindowText(m_p.strTitle);

	m_p.nMinWidth -= (pStdWnd->m_nFrameThick * 2);
	m_p.nMinHeight -= pStdWnd->m_nCaptionHeight;


	CRect rtClient, rtWindow;
	GetClientRect(rtClient);
	GetWindowRect(rtWindow);
	m_nCaptionHeight = rtWindow.Height() - rtClient.Height();
	m_nFrameThick = (rtWindow.Width() - rtClient.Width()) / 2;


	m_p.nMinWidth += (m_nFrameThick * 2);
	m_p.nMinHeight += m_nCaptionHeight;
	
	CRect rtDlg;
	m_p.pWnd->GetWindowRect(rtDlg);
	
	int nWidth = rtDlg.Width() + (m_nFrameThick * 2);
	int nHeight = rtDlg.Height() + m_nCaptionHeight;

	SetWindowPos(NULL, 10, 10, nWidth, nHeight, SWP_NOMOVE | SWP_NOZORDER );

	pStdWnd->DestroyWindow();
}

void CChildFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	switch (nID)
	{
		case ID_COMMAND_TO_STANDALONE_WND :
			PostMessage(WM_COMMAND, nID, 0);
			return;
	}

	MDIChildWndBase::OnSysCommand(nID, lParam);
}

// CChildFrame �޽��� ó����
void CChildFrame::OnFileClose() 
{
	// �������� �������� WM_CLOSE�� ������ �˴ϴ�.
	// �ý��� �޴����� �ݱ⸦ �����ϴ� �Ͱ� �����մϴ�.
	SendMessage(WM_CLOSE);
}

void CChildFrame::PostNcDestroy()
{
	if (m_hWnd == m_TopWindow)
		m_TopWindow = NULL;

	AfxGetMainWnd()->SendMessage(WM_VTS_MDI_CHILD_FRAME_CLOSED, (WPARAM)this);
	MDIChildWndBase::PostNcDestroy();
}

void CChildFrame::NcFrameImpl_OnNcPaint(CDC & dcPaint, bool bOuterEmulationMode)
{
	MDIChildWndBase::NcFrameImpl_OnNcPaint(dcPaint, bOuterEmulationMode);
}

void CChildFrame::OnNcRButtonDown(UINT nHitTest, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	MDIChildWndBase::OnNcRButtonDown(nHitTest, point);
}

LRESULT CChildFrame::OnExtMenuPrepare(WPARAM wParam, LPARAM lParam)
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

	int nItemPos = pPopup->ItemGetCount();
	pPopup->ItemInsert((UINT)CExtPopupMenuWnd::TYPE_SEPARATOR, -1);
	++nItemPos;
	//pPopup->ItemInsert(ID_COMMAND_TO_STANDALONE_WND, -1, _T("��������"), NULL, m_hWnd);

	
	return 1;
}


bool CChildFrame::NcFrameImpl_PreWindowProc( LRESULT & lResult, UINT message, WPARAM wParam, LPARAM lParam )
{
#if 0
	CWnd * pWndFrameImpl = NcFrameImpl_GetFrameWindow();
	switch( message )
	{
	case WM_LBUTTONDOWN:
		TRACE(_T("CChildFrm::WM_LBUTTONDOWN\n"));
		break;
	case WM_LBUTTONUP:
		TRACE(_T("CChildFrm::WM_LBUTTONUP\n"));
		break;
	case WM_MOUSEMOVE:
		TRACE(_T("CChildFrm::WM_MOUSEMOVE\n"));
		break;
	case WM_NCLBUTTONDOWN:
		TRACE(_T("CChildFrm::WM_NCLBUTTONDOWN\n"));
		break;
	case WM_NCLBUTTONUP:
		TRACE(_T("CChildFrm::WM_NCLBUTTONUP\n"));
		break;
	case WM_NCMOUSEMOVE:
		TRACE(_T("CChildFrm::WM_NCMOUSEMOVE\n"));
		break;
	}
#endif
	return __super::NcFrameImpl_PreWindowProc(lResult, message, wParam, lParam);
}

LRESULT CChildFrame::OnSetTopMost(WPARAM wParam, LPARAM lParam)
{
	if(wParam == 1)
		m_TopWindow = m_hWnd;
	else
		m_TopWindow = NULL;
	return 1;
}

void CChildFrame::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	// This message-handler is sent to the window being activated (Changing position, size or z-order)
	// Check if another window has the status of being top-most, instead of this one
	if (m_TopWindow!=NULL && m_TopWindow!=m_hWnd && ::IsWindow(m_TopWindow))
	{
		::SetWindowPos(m_TopWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	}
	MDIChildWndBase::OnWindowPosChanged(lpwndpos);
}

#endif //#ifdef _MDI_