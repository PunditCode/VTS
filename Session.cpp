// MySocket.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VTS.h"
#include "Session.h"
#include <boost/thread.hpp>
#include <VTSGuids.h>

#define RECV_BUF_LEN 10240

#define MAX_RECONNECT_COUNT		2
// Session

Session::Session() : m_bReconnectTry(FALSE), m_nReconnectCount(0), m_bForceLogout(FALSE), m_bLogout(FALSE), m_bConnectToSub(FALSE)
{
}

Session::~Session()
{
	TRACE(_T("%s Session deleted\n"), id);
}

BOOL Session::Login(HWND hWnd, CString id, CString pwd, CString strParam)
{
	if(!VTSSocket::Login(hWnd, id, pwd, strParam))
		return FALSE;

	this->id = id;
	this->password = pwd;
	return TRUE;
}

BOOL Session::LogOut()
{
	m_bLogout = TRUE;
	return VTSSocket::LogOut();
}

#define ENABLE_RECONNECT

void Session::_OnClose()
{
	if(m_bLogout)
		return;

//	VTSSocket::OnClose();
#ifdef ENABLE_RECONNECT
	if(m_nReconnectCount >= MAX_RECONNECT_COUNT || m_bForceLogout)
	{
		if(FALSE == m_bConnectToSub)
		{
			m_bConnectToSub = TRUE;

			if (ConnectToServer(ENV().GetPEMPath(), theApp.m_ipSub, theApp.m_nPort, TRUE))
				return;
		}

		ErrMsgBox(_T("서버와의 연결이 끊겼습니다.\r\n관리자에게 문의해주시기 바랍니다."));
		if (theApp.m_pMainWnd && ::IsWindow(theApp.m_pMainWnd->GetSafeHwnd()))
			theApp.m_pMainWnd->PostMessage(WM_SERVER_DISCONNECTED);
		return;
	}
	if(IsLogin() || m_bReconnectTry)
	{
		SetLogin(FALSE);
		m_bReconnectTry = TRUE;

		while(m_nReconnectCount++ < MAX_RECONNECT_COUNT)
		{
			TRACE(_T("Reconnect to server...%d\n"), m_nReconnectCount);
			if (!ConnectToServer(ENV().GetPEMPath(), GetServerIP(), GetServerPort(), TRUE))
				continue;
			return;
		}
	}
	else
	{
		if(FALSE == m_bConnectToSub)
		{
			//VTSUpdater가 이미 ipSub로 접속이 되어서 들어오기 때문에 처음로그인 실패후 다시 접속할 필요가 없다.
			//디버깅 모드가 아니고서는 절대 들어오지 않는 코드
			m_bConnectToSub = TRUE;
			if (ConnectToServer(ENV().GetPEMPath(), theApp.m_ipSub, theApp.m_nPort, TRUE))
				return;
		}

		ErrMsgBox(_T("불편을끼쳐드려 죄송합니다. 서버연결이 실패했습니다. \r\n관리자에게 문의해주시기 바랍니다."));
		if (theApp.m_pMainWnd && ::IsWindow(theApp.m_pMainWnd->GetSafeHwnd()))
			theApp.m_pMainWnd->PostMessage(WM_SERVER_DISCONNECTED);
	}
#else
	theApp.m_pMainWnd->PostMessage(WM_SERVER_DISCONNECTED);
#endif
}

BOOL Session::OnAuthentication(DKBuffer& dbuf, UINT nPacketSize)
{
	AUTHENTICATION_TYPE authType;
	if(!dbuf.RD(authType))
		return FALSE;

	switch(authType)
	{
	case AUTH_PROGRAM_GUID_COMPLETED: 
		{
			if(m_bReconnectTry)
			{
				Login(0, id, password, _T(""));
			}
			else
			{
				DLOG(_T("Recv AUTH_PROGRAM_GUID_COMPLETED"));
				AfxGetApp()->PostThreadMessage(WM_CONNECTION_ESTABLISHED, 0, 0);
			}
		}
		break;
	case AUTH_LOGIN_RESULT:
		{
			DLOG(_T("Recv Login Result"));
			int hWnd;
			dbuf.RD(hWnd);

			VTSS_LOGIN_RESULT resultCode;
			dbuf.RD(resultCode);
			
			CString* pStrMsg = new CString;
			dbuf.RD(*pStrMsg);
			
			LOG_INF(*pStrMsg);

			if(resultCode == VTSS_LOGIN_SUCCESS)
			{
				ReadAccount(dbuf);
				SetLogin(TRUE);

				if(m_bReconnectTry)
				{
					//재접속이 성공했다.
					theApp.m_pMainWnd->PostMessage(WM_RECONNECTED_TO_SERVER);
				}
			}
			else
			{
				///로그인이 실패했다.
				SetLogin(FALSE);
			}
			if(FALSE == m_bReconnectTry)
				AfxGetApp()->PostThreadMessage(WM_LOGIN_RESULT, (WPARAM)resultCode, (LPARAM)(LPCTSTR)pStrMsg);

			if(resultCode == VTSS_LOGIN_SUCCESS)
			{
				m_bReconnectTry = FALSE;
				m_nReconnectCount = 0;
			}
			/*if(::IsWindow((HWND)hWnd))
			{
				SendMessage((HWND)hWnd, WM_REQUEST_RECEIVED, (WPARAM)resultCode, (LPARAM)(LPCTSTR)pStrMsg);
			}*/
		}
		break;
	}
	return TRUE;
}

void Session::SendReady()
{
	BEGIN_BUF();
	BUF_WT(PRTC_AUTHENTICATION);
	BUF_WT(AUTH_READY);
	END_BUF(this);
}

BOOL Session::SendProgramGUID()
{
	vector<CString> cont;
	::GetMACaddress(cont);

	BEGIN_BUF();
	BUF_WT(PRTC_AUTHENTICATION);
	BUF_WT(AUTH_PROGRAM_GUID);
	BUF_WT(VTS_PROGRAM_GUID);
	BUF_WT(cont[0]);
	END_BUF(this);
	return TRUE;
}

BOOL Session::ReadAccount(DKBuffer& dbuf)
{
	ClientBase::FromBuffer(dbuf);

	VTSCodeManager& cm = dynamic_cast<VTSCodeManager&>(InstCM());
	if(!cm.Init(dbuf))
	{
		ErrMsgBox(_T("코드정보를 수신하지 못했습니다."));
		PostQuitMessage(0);
	}
	//theApp.m_pMainWnd->SendMessage(WM_CODE_LIST_RECEIVED);
	return TRUE;
}

BOOL Session::RequestMerchandiseCurrent(GTMerchandise::IDType idCode, BOOL bRelease)
{
	VTS_REQUEST_TYPE reqType = bRelease ? VTS_REQ_RELEASE_CURRENT : VTS_REQ_CURRENT;

	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_REQUEST);
	BUF_WT(reqType);
	BUF_WT(idCode.first->GetID()); 
	BUF_WT(idCode.second);
	END_BUF(this);

	return TRUE;
}

BOOL Session::RequestMerchandiseHoga(GTMerchandise::IDType idCode, BOOL bRelease)
{
	VTS_REQUEST_TYPE reqType = bRelease ? VTS_REQ_RELEASE_HOGA : VTS_REQ_HOGA;

	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_REQUEST);
	BUF_WT(reqType);
	BUF_WT(idCode.first->GetID()); 
	BUF_WT(idCode.second);
	END_BUF(this);
	return TRUE;
}

BOOL Session::RequestJango(BOOL bRelease)
{
	VTS_REQUEST_TYPE reqType = bRelease ? VTS_REQ_RELEASE_JANGO : VTS_REQ_JANGO;

	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_REQUEST);
	BUF_WT(reqType);
	END_BUF(this);

	return TRUE;
}

BOOL Session::RequestDeposit(GT_CURRENCY amount, const CString& name)
{
	CString strAction;
	strAction.Format(_T("입금요청: %s원"), ::ToThousand(amount));
	SendOrderAction(strAction);

	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_REQUEST);
	BUF_WT(VTS_REQ_DESPOSIT);
	BUF_WT(amount);
	BUF_WT(bankOwner);
	BUF_WT(m_vtssBank);
	BUF_WT(m_vtssBankAccount);
	BUF_WT(m_vtssBankOwner);
	BUF_WT(CString(_T("")));

	END_BUF(this);
	return TRUE;
}

BOOL Session::RequestWithdraw(GT_CURRENCY amount, const CString& strMemo)
{
	CString strAction;
	strAction.Format(_T("출금요청: %s원"), ::ToThousand(amount));
	SendOrderAction(strAction);

	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_REQUEST);
	BUF_WT(VTS_REQ_WITHDRAW);
	BUF_WT(amount);
	BUF_WT(bankOwner);
	BUF_WT(bank);
	BUF_WT(bankAccount);
	BUF_WT(bankOwner);
	BUF_WT(strMemo);
	
	END_BUF(this);
	return TRUE;
}


BOOL Session::RequestOrderHistory(HWND hWnd, COleDateTime tmBegin, COleDateTime tmEnd, BOOL bContractOnly)
{
	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_REQUEST);
	BUF_WT(VTS_REQ_ORDER_HISTORY);
	BUF_WT((INT)hWnd);
	BUF_WT(tmBegin.m_dt);
	BUF_WT(tmEnd.m_dt);
	BUF_WT(bContractOnly);
	END_BUF(this);

	/*m_nLastRequest = VTS_REQ_ORDER_HISTORY;
	m_timer->expires_from_now(boost::posix_time::seconds(3));
	m_timer->async_wait(boost::bind(&Session::DeadLineTimer, this, ASIO::placeholders::error));*/
	
	return TRUE;
}

BOOL Session::RequestEachdayProfit(HWND hWnd, COleDateTime tmBegin, COleDateTime tmEnd)
{
	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_REQUEST);
	BUF_WT(VTS_REQ_EACHDAY_PROFIT);
	BUF_WT((INT)hWnd);
	BUF_WT(tmBegin.m_dt);
	BUF_WT(tmEnd.m_dt);
	END_BUF(this);
	return TRUE;
}
#ifdef _CLIENT_ACTION
BOOL Session::RequestClientActionLog(HWND hWnd, COleDateTime tmBegin, COleDateTime tmEnd)
{
	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_REQUEST);
	BUF_WT(VTS_REQ_CLIENT_ACTION_LOG);
	BUF_WT((INT)hWnd);
	BUF_WT(dateBegin);
	BUF_WT(dateEnd);
	END_BUF(this);
	return TRUE;
}
#endif

BOOL Session::RequestBankingHistory(HWND hWnd, COleDateTime tmBegin, COleDateTime tmEnd, VTSBankingHistoryRequestType type)
{
	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_REQUEST);
	BUF_WT(VTS_REQ_BANKING_HISTORY);
	BUF_WT((INT)hWnd);
	BUF_WT(tmBegin.m_dt);
	BUF_WT(tmEnd.m_dt);
	BUF_WT((int)type);
	END_BUF(this);
	return TRUE;
}


BOOL Session::RequestDummy()
{
	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_REQUEST);
	BUF_WT(VTS_REQ_DUMMY);
	END_BUF(this);
	
	return TRUE;
}


BOOL Session::Order(HWND hWnd, Code* pCode, MarketType marketType, PositionType posType, HogaType hoga, const CString& price, const CString& vol, ORDER_ACTOR actor)
{
	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_ORDER);
	BUF_WT(VTS_ORDER_NEW);
	BUF_WT((int)hWnd);
	BUF_WT(pCode->GetID());
	BUF_WT(marketType);
	BUF_WT(posType);
	BUF_WT(hoga);
	BUF_WT(price);
	BUF_WT(vol);
	BUF_WT(actor);
	END_BUF(this);

	return TRUE;
}

BOOL Session::OrderCorrection(HWND hWnd, int nOrdNumOrg, HogaType hoga, const CString& price, const CString& vol)
{
	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_ORDER);
	BUF_WT(VTS_ORDER_CORRECTION);
	BUF_WT((int)hWnd);
	BUF_WT(nOrdNumOrg);
	BUF_WT(hoga);
	BUF_WT(price);
	BUF_WT(vol);
	END_BUF(this);
	return TRUE;
}

BOOL Session::OrderCancel(HWND hWnd, int nOrdNumOrg, const CString& vol)
{
	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_ORDER);
	BUF_WT(VTS_ORDER_CANCEL);
	BUF_WT((int)hWnd);
	BUF_WT(nOrdNumOrg);
	BUF_WT(vol);
	END_BUF(this);
	return TRUE;
}

BOOL Session::OrderCancelAllByPosition(HWND hWnd, Code* pCode, PositionType posType)
{
	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_ORDER);
	BUF_WT(VTS_ORDER_CANCEL_ALL_BY_POSITION);
	BUF_WT((int)hWnd);
	BUF_WT(pCode == NULL ? -1  : pCode->GetID());
	BUF_WT(posType);
	END_BUF(this);
	return TRUE;
}

BOOL Session::OrderLiquid(HWND hWnd, Code* pCode)
{
	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_ORDER);
	BUF_WT(VTS_ORDER_LIQUID);
	BUF_WT((int)hWnd);
	if(pCode)
	{
		BUF_WT(pCode->GetID());
	}
	else
	{
		BUF_WT(-1);
	}

	END_BUF(this);
	return TRUE;
}
#ifdef _CLIENT_ACTION
BOOL Session::SendAction(const CString& action, const CString& actor)
{
	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_NOTIFY);
	BUF_WT(VTS_NOTF_ADD_USER_ACTION);
	BUF_WT(action);
	BUF_WT(actor);
	END_BUF(this);
	return TRUE;
}
#endif
BOOL Session::SendOrderAction(const CString& strAction, const CString& strCode, const CString& strPrice,
						 PositionType position, const CString& strVolume)
{
	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_NOTIFY);
	BUF_WT(VTS_NOTF_ADD_ORDER_ACTION);
	BUF_WT(strAction);
	BUF_WT(strCode);
	BUF_WT(strPrice);
	BUF_WT(position);
	BUF_WT(strVolume);
	END_BUF(this);
	return TRUE;

}

BOOL Session::RegisterNewClient(HWND hWnd, ClientBase* pClient)
{
	BEGIN_BUF();
	BUF_WT(PRTC_AUTHENTICATION);
	BUF_WT(AUTH_REGISTER);
	BUF_WT((INT)hWnd);
	pClient->ToBuffer(*pSendBuf);
	END_BUF(this);
	return TRUE;
}


BOOL Session::RequestClientInfoList(HWND hWnd, int nWhereField, CString strWhereKey, BOOL bTodayOrder, BOOL bConnected)
{
	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_REQUEST);
	BUF_WT(VTS_REQ_RCMD_CLIENT_INFO_LIST);
	BUF_WT((INT)hWnd);
	BUF_WT(nWhereField);
	BUF_WT(strWhereKey);
	BUF_WT(bTodayOrder);
	BUF_WT(bConnected);
	END_BUF(this);

	return TRUE;
}

BOOL Session::RequestClientEachdayProfit(HWND hWnd, CString id, COleDateTime tmBegin, COleDateTime tmEnd)
{
	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_REQUEST);
	BUF_WT(VTS_REQ_RCMD_CLIENT_EACH_DAY_PROFIT);
	BUF_WT((INT)hWnd);
	BUF_WT(id);
	BUF_WT(tmBegin.m_dt);
	BUF_WT(tmEnd.m_dt);
	END_BUF(this);

	return TRUE;
}

BOOL Session::RequestRecommenderEachdayProfit(HWND hWnd, COleDateTime tmBegin, COleDateTime tmEnd)
{
	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_REQUEST);
	BUF_WT(VTS_REQ_RCMD_EACH_DAY_PROFIT);
	BUF_WT((INT)hWnd);
	BUF_WT(tmBegin.m_dt);
	BUF_WT(tmEnd.m_dt);
	END_BUF(this);
	return TRUE;
}

BOOL Session::RequestRecommenderMembersActionLog(HWND hWnd, CString id, CString date)
{
	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_REQUEST);
	BUF_WT(VTS_REQ_RCMD_MEMBERS_ACTION_LOG);
	BUF_WT((INT)hWnd);
	BUF_WT(id);
	BUF_WT(date);
	END_BUF(this);

	return TRUE;
}

BOOL Session::RequestRecommenderClientTotalProfit(HWND hWnd, int nQueryField,
							CString strQuery, COleDateTime tmBegin, COleDateTime tmEnd)
{
	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_REQUEST);
	BUF_WT(VTS_REQ_RCMD_CLIENT_TOTAL_PROFIT);
	BUF_WT((INT)hWnd);
	BUF_WT(nQueryField);
	BUF_WT(strQuery);
	BUF_WT(tmBegin.m_dt);
	BUF_WT(tmEnd.m_dt);
	END_BUF(this);

	return TRUE;
}


BOOL Session::RequestOptionsTable(HWND hWnd, INT yyyy, INT mm, BOOL bNight)
{
	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_REQUEST);
	BUF_WT(VTS_REQ_OPTIONS_TABLE);
	BUF_WT((INT)hWnd);
	BUF_WT(yyyy);
	BUF_WT(mm);
	BUF_WT(bNight);
	END_BUF(this);

	return TRUE;
}

BOOL Session::RequestEnableOrderVolume(HWND hWnd, Code* pCode, MarketType marketType, CString strPrice)
{
	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_REQUEST);
	BUF_WT(VTS_REQ_ENABLE_ORDER_VOLUME);
	BUF_WT((INT)hWnd);
	BUF_WT(pCode->GetID());
	BUF_WT(marketType);
	BUF_WT(strPrice);
	END_BUF(this);

	return TRUE;
}

BOOL Session::RequestOrderActionLog(HWND hWnd, CString date)
{
	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_REQUEST);
	BUF_WT(VTS_REQ_ORDER_ACTION_LOG);
	BUF_WT((INT)hWnd);
	BUF_WT(date);
	END_BUF(this);
	return TRUE;
}


BOOL Session::RequestChangePassword(HWND hWnd, CString strPrev, CString strNew)
{
	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_REQUEST);
	BUF_WT(VTS_REQ_CHANGE_PASSWORD);
	BUF_WT((INT)hWnd);
	BUF_WT(strPrev);
	BUF_WT(strNew);
	END_BUF(this);
	return TRUE;
}

BOOL Session::RequestChangeEnableOvernight(HWND hWnd, BOOL bEnableOvernight)
{
	BEGIN_BUF();
	BUF_WT(PRTC_DATA);
	BUF_WT(SDT_REQUEST);
	BUF_WT(VTS_REQ_CHANGE_ENABLE_OVERNIGHT);
	BUF_WT((INT)hWnd);
	BUF_WT(bEnableOvernight);
	END_BUF(this);
	return TRUE;
}
