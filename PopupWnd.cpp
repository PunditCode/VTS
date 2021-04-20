// BaseWnd.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "VTS.h"
#include "PopupWnd.h"
#include "resource.h"

#include <LogDlg.h>
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
#include "PreferencesDlg.h"
#include "FastTradeDlg.h"
#include "RecommendersClientInfoDlg.h"
#include "RecommendersClientTotalProfitDlg.h"
#ifdef ENABLE_WEB_BROWSER
#include "WebBrowserDlg.h"
#endif

// CPopupWnd

IMPLEMENT_DYNCREATE(CPopupWnd, VTSPopupBaseWnd)

CPopupWnd::CPopupWnd() : m_nWndID(NUL_IDX)
{
}

CPopupWnd::~CPopupWnd()
{
}


BEGIN_MESSAGE_MAP(CPopupWnd, VTSPopupBaseWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int CPopupWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (VTSPopupBaseWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetIcon( AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);
	return 0;
}


#define NEWDLG(_class_) pDlg = new _class_; pDlg->Create(_class_::IDD);

BOOL CPopupWnd::SetChildWnd(int nWndID, int x, int y)
{
	CDialog* pDlg = NULL;

#ifdef ENABLE_WEB_BROWSER
	if(ID_TRDSTD_1 <= nWndID && nWndID <= ID_TRDSTD_5)
	{
		switch(nWndID)
		{
		case ID_TRDSTD_1:
			m_strTitle = _T("�ŷ����� - �ŷ��ð�/����� �ð� �ȳ�"); 
			break;
		case ID_TRDSTD_2:
			m_strTitle = _T("�ŷ����� - �ν��� ���� �ȳ�"); 
			break;
		case ID_TRDSTD_3:
			m_strTitle = _T("�ŷ����� - �ֹ����ɼ��� �ȳ�"); 
			break;
		case ID_TRDSTD_4:
			m_strTitle = _T("�ŷ����� - �߰���û�� �������� �ȳ�"); 
			break;
		case ID_TRDSTD_5:
			m_strTitle = _T("�ŷ����� - �ս�����/���ͽ���, MIT�ֹ� �ȳ�"); 
			break;
		}

		CWebBrowserDlg* pDlg = new CWebBrowserDlg;
		pDlg->SetID(nWndID);
		pDlg->Create(CWebBrowserDlg::IDD);
		
		if(theApp.IsManagerMode())
			m_strTitle += _T("(<���!>��ȭ���Դϴ�)");

		m_pWndChild = pDlg;
		m_nWndID = nWndID;
		m_xInit = x;
		m_yInit = y;
		return TRUE;
	}
#endif

	switch(nWndID)
	{
	case ID_SYSTEM_LOG:
		m_strTitle = _T("�ý��� �α�");
		NEWDLG(CLogDlg);
		break;
	case ID_FAST_TRADING:
		m_strTitle.Format(_T("�����ֹ�(%s)"), GetSession()->id);
		m_bEnableWidthResize = FALSE;
		m_bMaxButton = FALSE;
		NEWDLG(CFastTradeDlg);
		break;
	case ID_BANKING_DEPOSIT: 
		m_strTitle = _T("�Աݽ�û");
		m_bEnableWidthResize = FALSE;
		m_bEnableHeightResize = FALSE;
		m_bMaxButton = FALSE;
		pDlg = new CBankingIORequestDlg(biot_deposit, this); pDlg->Create(CBankingIORequestDlg::IDD);
		break;
	case ID_BANKING_WITHDRAW: 
		m_strTitle = _T("��ݽ�û");
		m_bEnableWidthResize = FALSE;
		m_bEnableHeightResize = FALSE;
		m_bMaxButton = FALSE;
		pDlg = new CBankingIORequestDlg(biot_withdraw, this); pDlg->Create(CBankingIORequestDlg::IDD);
		break;
	case ID_EACH_DAY_PROFIT: 
		m_strTitle = _T("���� ��ȸ");
		//NEWDLG(CMainDlg_WIZ);
		NEWDLG(CEachdayProfitDlg);
		break;
	case ID_ORDER_HISTORY: 
		m_strTitle = _T("�ŷ�����");
		NEWDLG(COrderHistoryDlg);
		break;
	case ID_BANKING_IO_HISTORY: 
		m_strTitle = _T("��/��ݳ��� ��ȸ");
		NEWDLG(CBankingHistoryDlg);
		break;
	case ID_ORDER_ACTION_LOG        :
		m_strTitle = _T("�ֹ��α׳���");
		NEWDLG(COrderActionLogDlg);
		break;
#ifdef _CLIENT_ACTION
	case ID_USER_ACTION_LOG: 
		m_strTitle = _T("���α� ����");
		NEWDLG(CClientActionLogDlg);
		break;
#endif
	case ID_NOTICES: 
		m_strTitle = _T("��������");
		NEWDLG(CNoticesDlg);
		break;
	case ID_RCMD_MEMBERS_PROFIT	    :
		m_strTitle = _T("��õȸ�� �Ⱓ�� ����");
		NEWDLG(CRecommenderMembersProfitDlg);
		break;
	case ID_RCMD_PERIOD_PROFIT	    :
		m_strTitle = _T("��¥�� ��õȸ�� ��������");
		NEWDLG(CRecommenderEachdayProfitDlg);
		break;
	case ID_RCMD_MEMBER_ACTION_LOG  :
		m_strTitle = _T("��õȸ�� ���α�");
		NEWDLG(CRecommenderMembersActionLogDlg);
		break;
	case ID_RCMD_CLIENT_INFO :
		m_strTitle = _T("��õȸ������");
		NEWDLG(CRecommendersClientInfoDlg);
		break;
	case ID_RCMD_CLIENT_TOTAL_PROFIT:
		m_strTitle = _T("��õȸ�� �Ѽ������賻��");
		NEWDLG(CRecommendersClientTotalProfitDlg);
		break;
	default:
		ASSERT(FALSE);
		return FALSE;
	}
	
	if(theApp.IsManagerMode())
		m_strTitle += _T("(<���!>��ȭ���Դϴ�)");

	m_pWndChild = pDlg;
	m_nWndID = nWndID;
	m_xInit = x;
	m_yInit = y;

	return TRUE;
}
