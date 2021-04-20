#include "stdafx.h"
#include "Session.h"
#include "ServerEventReceiver.h"
#include "MsgDlg.h"
#include "VTS.h"
#include "VTSError.h"

void Session::OnMsg(DKBuffer& dbuf, UINT nPacketSize)
{
	MsgItem* pMsg = new MsgItem;

	dbuf.RD(pMsg->nMsgType);
	dbuf.RD(pMsg->hWnd);
	dbuf.RD(pMsg->nCode);
	dbuf.RD(pMsg->strTitle);
	dbuf.RD(pMsg->strBody);
		
	LOG_SYS(pMsg->strTitle + _T(":") + pMsg->strBody);	
	if(VTSS_MSG_POPUP == pMsg->nMsgType || VTSS_MSG_ERR == pMsg->nMsgType)
	{
		AfxGetApp()->PostThreadMessage(WM_MESSAGE_FROM_SERVER, (WPARAM)pMsg, 0);
	}
	else if(VTSS_MSG_REQ_RESULT == pMsg->nMsgType)
	{
		if(pMsg->hWnd != 0 && ::IsWindow((HWND)pMsg->hWnd))
		{
			::PostMessage((HWND)pMsg->hWnd, WM_MESSAGE_FROM_SERVER, (WPARAM)pMsg, 0);
		}
		else
		{
			AfxGetApp()->PostThreadMessage(WM_MESSAGE_FROM_SERVER, (WPARAM)pMsg, 0);
		}
	}
	else if (VTSS_MSG_REQ_ERROR == pMsg->nMsgType)
	{
		if (::IsWindow((HWND)pMsg->hWnd))
			::PostMessage((HWND)pMsg->hWnd, WM_REQUEST_ERROR, (WPARAM)(LPCTSTR)pMsg->strBody, (LPARAM)(LPCTSTR)pMsg->strTitle);
		else
			::MessageBox(CWnd::GetActiveWindow()->GetSafeHwnd(), pMsg->strBody, pMsg->strTitle, MB_ICONERROR);
	}
}

void Session::OnData(DKBuffer& dbuf, UINT nPacketSize)
{
	STREAM_DATA_TYPE streamType;
	dbuf.RD(streamType);
	if(SDT_NOTIFY == streamType)
	{
		OnNotify(dbuf, nPacketSize);
		return;
	}
	else if(SDT_ORDER_EVENT == streamType)
	{
		OnOrderEvent(dbuf);
		return;
	}
	ASSERT(FALSE);
}

void Session::OnNotify(DKBuffer& dbuf, UINT nPacketSize)
{
	VTSS_NOTIFY_TYPE notifyType;
	dbuf.RD(notifyType);

	switch(notifyType)
	{
	case VTSS_NOTF_CLIENT_INFO_UPDATED:
		{
			ClientBase info;
			info.FromBuffer(dbuf);
			ClientBase::operator = (info);
			SER()->OnClientInfoUpdated();
		}
		break;
	case VTSS_NOTF_SERVER_PARAMS:
		{
		CString str;
		dbuf.RD(str);

		Tokenizer tok(str, _T(";"));

		for (UINT i = 0; i < tok.size(); ++i)
		{
			CString name, val;
			::ParseLine(tok[i], name, val, _T('='));

			if (_T("ENV_BANK") == name)
			{
				m_vtssBank = val;
			}
			else if (_T("ENV_BANK_ACCOUNT") == name)
			{
				m_vtssBankAccount = val;
			}
			else if (_T("ENV_BANK_OWNER") == name)
			{
				m_vtssBankOwner = val;
			}
			else if (_T("ENV_OVERNIGHT_DEPOSIT_FUTURES") == name)
			{
				m_nOvernightDepositFutures = ::ToLONGLONG(val);
			}
			else if (_T("ENV_OVERNIGHT_DEPOSIT_FOREIGN_FUTURES") == name)
			{
				m_nOvernightDepositForeignFutures = ::ToLONGLONG(val);
			}
		}
		}
		break;
	case VTSS_NOTF_REGISTER_RESULT:
		{
			int hWnd;
			dbuf.RD(hWnd);
			int nErr;
			CString msg;
			dbuf.RD(nErr);
			dbuf.RD(msg);

			if(!::IsWindow((HWND)hWnd))
				hWnd = (int)theApp.m_pMainWnd->GetSafeHwnd();

			if(VRES_REGISTER_SUCCESS == nErr)
				::MessageBox((HWND)hWnd, msg, _T("성공"), MB_ICONINFORMATION | MB_OK);
			else
				::MessageBox((HWND)hWnd, msg, _T("실패"), MB_ICONERROR | MB_OK);
		}
		break;
	case VTSS_NOTF_CODE: 
		{
			if(dynamic_cast<VTSCodeManager&>(InstCM()).Init(dbuf))
			{
				LOG_DIAG(_T("종목코드 수신완료"));
				theApp.m_pMainWnd->SendMessage(WM_CODE_LIST_RECEIVED);
			}
			else
				LOG_NET(_T("종목코드 수신 실패"));
		}
		break;
	case VTSS_NOTF_MERCHANDISE_CURRENT_INIT: 
	case VTSS_NOTF_MERCHANDISE_CURRENT: 
		SER()->OnCurrentUpdated(dbuf, notifyType);
		break;
	case VTSS_NOTF_MERCHANDISE_HOGA_INIT: 
		SER()->OnHogaInitUpdated(dbuf);
		break;
	case VTSS_NOTF_MERCHANDISE_HOGA: 
		SER()->OnHogaUpdated(dbuf);
		break;
	case VTSS_NOTF_CONTRACTED_ORDER:
		SER()->OnContractedOrder(dbuf);
		break;
	case VTSS_NOTF_ORDER_CONTRACTS:
		SER()->OnOrderContracts(dbuf);
		break;
	case VTSS_NOTF_ORDER_OUTSTANDING:
		SER()->OnOrderOutstandings(dbuf);
		break;
	case VTSS_NOTF_PROFIT_UPDATED:
		SER()->OnProfitUpdated(dbuf);
		break;
	case VTSS_NOTF_NOTICES:
		{
			UINT nDummy;
			UINT nColCount, nRowCount;
			dbuf.RD(nDummy);
			dbuf.RD(nColCount);
			ASSERT(nColCount == 3);
			dbuf.RD(nRowCount);
			
			for(UINT i = 0 ; i < nRowCount ; ++i)
			{
				NoticeItemPtr pNotice(new NoticeItem);

				dbuf.RD(pNotice->date);
				dbuf.RD(pNotice->header);
				dbuf.RD(pNotice->body);

				m_notices.push_back(pNotice);
			}
#ifdef NDEBUG
			if(theApp.m_pMainWnd && IsWindow(theApp.m_pMainWnd->GetSafeHwnd()))
				theApp.m_pMainWnd->PostMessage(WM_NOTICES_RECEIVED, 0, 0);
#endif
		}
		break;
	case VTSS_NOTF_ORDER_HISTORY:
	case VTSS_NOTF_EACHDAY_PROFIT:
	case VTSS_NOTF_USER_ACTION_LOG:
	case VTSS_NOTF_BANKING_HISTORY:
	case VTSS_NOTF_CLIENT_INFO_LIST:
	case VTSS_NOTF_RCMD_EACHDAY_PROFIT:
	case VTSS_NOTF_OPTIONS_TABLE:
	case VTSS_NOTF_CLIENT_TOTAL_PROFIT:
		{
			LOG_INF(_T("조회내역 수신(%d)"), notifyType);
			UINT hWnd;
			dbuf.RD(hWnd);
			if(::IsWindow((HWND)hWnd))
			{
				UINT nSize = nPacketSize - (dbuf.GetPos() - BUF_HEADER_SIZE);
				DKBuffer* pDbuf = new DKBuffer(nSize);
				pDbuf->WT(dbuf.GetCur(), nSize);
				pDbuf->SetPos(0);

				::PostMessage((HWND)hWnd, WM_REQUEST_RECEIVED, (WPARAM)pDbuf, (LPARAM)0);
			}
			break;
		}
		break;
	case VTSS_NOTF_CLIENT_EACH_DAY_PROFIT:	
	case VTSS_NOTF_CLIENT_ACTION_LOG:
	case VTSS_NOTF_ORDER_ACTION_LOG:
		{
			UINT hWnd;
			dbuf.RD(hWnd);
			if(::IsWindow((HWND)hWnd))
			{
				UINT nSize = nPacketSize - (dbuf.GetPos() - BUF_HEADER_SIZE);
				DKBuffer* pDbuf = new DKBuffer(nSize);
				pDbuf->WT(dbuf.GetCur(), nSize);
				pDbuf->SetPos(0);

				::PostMessage((HWND)hWnd, WM_REQUEST_RECEIVED, (WPARAM)pDbuf, (LPARAM)1);
			}
			break;
		}
		break;
	case VTSS_NOTF_NEW_NOTICE:
		{
			NoticeItemPtr pNotice(new NoticeItem);

			dbuf.RD(pNotice->date);
			dbuf.RD(pNotice->header);
			dbuf.RD(pNotice->body);

			m_notices.push_front(pNotice);
			theApp.m_pMainWnd->SendMessage(WM_NEW_NOTICE, 0, 0);

			MsgItem* pMsg = new MsgItem;
			pMsg->nMsgType = VTSS_MSG_NOTICE;
			pMsg->nCode = 0;
			pMsg->strTitle = _T("[공지]") + pNotice->header;
			pMsg->strBody = pNotice->body;
		
			theApp.PostThreadMessage(WM_MESSAGE_FROM_SERVER, (WPARAM)pMsg, 0);
		}
		break;
	case VTSS_NOTF_ENABLE_ORDER_VOLUME:
		{
			INT hWnd;
			dbuf.RD(hWnd);

			if(::IsWindow((HWND)hWnd))
			{
				EanbleOrderItem* pItem = new EanbleOrderItem;
				INT nCode;

				dbuf.RD(nCode);
				dbuf.RD(pItem->marketType);
				dbuf.RD(pItem->nBuyCount);
				dbuf.RD(pItem->nSellCount);

				pItem->pCode = InstCM().GetCode(nCode);
				::PostMessage((HWND)hWnd, WM_ENABLE_ORDER_COUNT_RECEIVED, (WPARAM)pItem, (LPARAM)0);
			}
		}
		break;
	case VTSS_NOTF_MARKET_EYE:
		{
			MarketEye* pME = new MarketEye;
			m_marketEye = *pME;

			dbuf.RD(pME->kospi.jisu);
			dbuf.RD(pME->kospi.sign);
			dbuf.RD(pME->kospi.change);
			dbuf.RD(pME->kospi.diff);

			dbuf.RD(pME->USD.jisu);
			dbuf.RD(pME->USD.sign);
			dbuf.RD(pME->USD.change);
			dbuf.RD(pME->USD.diff);

			if(::IsWindow(theApp.m_pMainWnd->GetSafeHwnd()))
				theApp.m_pMainWnd->PostMessage(WM_MARKET_EYE, (WPARAM)pME);
		}
		break;
	case VTSS_NOTF_CLOSE:
		m_bForceLogout = TRUE;
		Close();
		_OnClose();
		break;
	case VTSS_NOTF_ENABLE_WITHDRAW_AMOUNT:
		{
			GT_CURRENCY amountEnable;
			dbuf.RD(amountEnable);
		}
		break;
	default:
		ASSERT(FALSE);
	}
}

void Session::OnOrderEvent(DKBuffer& dbuf)
{
	int hWnd;
	UINT nCode;

	OrderResult* pResult = new OrderResult; //삭제는 m_pMainWnd에서

	CString strMsg;
	dbuf.RD(pResult->orderStatus);
	dbuf.RD(hWnd);
	dbuf.RD(pResult->nOrdNum);
	dbuf.RD(pResult->nOrdNumOrg);
	dbuf.RD(nCode);
	pResult->pCode = InstCM().GetCode(nCode);
	VERIFY(pResult->pCode);
	dbuf.RD(pResult->position);
	dbuf.RD(pResult->hoga);
	dbuf.RD(pResult->strPrice);
	dbuf.RD(pResult->nVolume);
	dbuf.RD(pResult->strContPrice);
	dbuf.RD(pResult->strMsg);

	m_orderEvents.push_back(pResult);
	TRACE(_T("OnOrderEvent status(0x%x-%d)\n"), pResult, pResult->orderStatus);

	if (::IsWindow((HWND)hWnd) && OS_REJECT == pResult->orderStatus)
	{
		//빠른주문창에서 메세지박스를 띄우면 그동안 현재가 수신이 안된다.
		::PostMessage((HWND)hWnd, WM_ORDER_RESULT, (WPARAM)pResult, 0);
	}
		// pResult의 삭제는 MainWnd에서
	theApp.m_pMainWnd->PostMessage(WM_ORDER_RESULT, (WPARAM)pResult, (LPARAM)hWnd);
}


void Session::set_bankBalance(GT_CURRENCY balance)
{

	ClientBase::set_bankBalance(balance);
}
