#pragma once

// Session 명령 대상입니다.

#include "GTLibExt/VTSCodeManager.h"
#include "VTSSocket.h"
#include "ClientBase.h"

class Session : public VTSSocket, public ClientBase
{
public:
	Session();
	virtual ~Session();

public:
	OVERRIDE BOOL Login(HWND hWnd, CString id, CString pwd, CString strParam = _T(""));
	OVERRIDE BOOL LogOut();
	OVERRIDE void _OnClose();
	OVERRIDE BOOL OnAuthentication(DKBuffer& dbuf, UINT nPacketSize);

	void SendReady();
	BOOL RequestMerchandiseCurrent(GTMerchandise::IDType idCode, BOOL bRelease = FALSE); //서버에게 현재가 정보를 요청/해제 한다.
	BOOL RequestMerchandiseHoga(GTMerchandise::IDType idCode, BOOL bRelease = FALSE); //서버에게 호가 정보를 요청/해제 한다.
	BOOL RequestJango(BOOL bRelease = FALSE); //서버에게 잔고 정보를 요청/해제 한다.
	BOOL RequestDummy();
	BOOL RequestDeposit(GT_CURRENCY amount, const CString& name); //입금신청
	BOOL RequestWithdraw(GT_CURRENCY amount, const CString& strMemo); //출금신청
	BOOL RequestOrderHistory(HWND hWnd, COleDateTime tmBegin, COleDateTime tmEnd, BOOL bContractOnly);
	BOOL RequestEachdayProfit(HWND hWnd, COleDateTime tmBegin, COleDateTime tmEnd);
	
	BOOL RequestBankingHistory(HWND hWnd, COleDateTime tmBegin, COleDateTime tmEnd, VTSBankingHistoryRequestType type);
	BOOL RequestOptionsTable(HWND hWnd, INT yyyy, INT mm, BOOL bNight);
	BOOL Order(HWND hWnd, Code* pCode, MarketType marketType, PositionType posType, HogaType hoga, const CString& price, const CString& vol, ORDER_ACTOR actor = OA_CLIENT);
	BOOL OrderCorrection(HWND hWnd, int nOrdNumOrg, HogaType hoga, const CString& price, const CString& vol);
	BOOL OrderCancel(HWND hWnd, int nOrdNumOrg, const CString& vol);
	BOOL OrderCancelAllByPosition(HWND hWnd, Code* pCode, PositionType posType);
	BOOL OrderLiquid(HWND hWnd, Code* pCode); //해당 포지션을 모두 청산
	
	void SendDummy();

	BOOL RegisterNewClient(HWND hWnd, ClientBase* pClient);
#ifdef _CLIENT_ACTION
	BOOL SendAction(const CString& action, const CString& actor);
	BOOL RequestClientActionLog(HWND hWnd, COleDateTime tmBegin, COleDateTime tmEnd);
#endif
	BOOL SendOrderAction(const CString& strAction, const CString& strCode = _T(""), const CString& strPrice = _T(""),
						 PositionType position = PT_NONE, const CString& strVolume = _T(""));
	
	///////////////////////////////////////////////////
	// 추천인 조회용
	BOOL RequestClientInfoList(HWND hWnd, int nWhereField, CString strWhereKey, BOOL bTodayOrder = FALSE, BOOL bConnected = FALSE);
	BOOL RequestClientEachdayProfit(HWND hWnd, CString id, COleDateTime tmBegin, COleDateTime tmEnd);
	BOOL RequestRecommenderEachdayProfit(HWND hWnd, COleDateTime tmBegin, COleDateTime tmEnd);
	BOOL RequestRecommenderMembersActionLog(HWND hWnd, CString id, CString date);
	BOOL RequestRecommenderClientTotalProfit(HWND hWnd, int nQueryField,
		CString strQuery, COleDateTime tmBegin, COleDateTime tmEnd);
	///////////////////////////////////////////////////

	BOOL RequestEnableOrderVolume(HWND hWnd, Code* pCode, MarketType marketType, CString strPrice);
	BOOL RequestOrderActionLog(HWND hWnd, CString date);
	
	BOOL RequestChangePassword(HWND hWnd, CString strPrev, CString strNew);
	BOOL RequestChangeEnableOvernight(HWND hWnd, BOOL bEnableOvernight);

protected:
	OVERRIDE BOOL SendProgramGUID();
	OVERRIDE BOOL ReadAccount(DKBuffer& dbuf);
	OVERRIDE void OnData(DKBuffer& dbuf, UINT nPacketSize);
	OVERRIDE void OnNotify(DKBuffer& dbuf, UINT nPacketSize);
	OVERRIDE void OnMsg(DKBuffer& dbuf, UINT nPacketSize);

	void OnOrderEvent(DKBuffer& dbuf);
	
	OVERRIDE void set_bankBalance(GT_CURRENCY balance);

public:
	
	Notices		m_notices;
	CString		m_vtssBank, m_vtssBankAccount, m_vtssBankOwner;
	GT_CURRENCY	m_nOvernightDepositFutures, m_nOvernightDepositForeignFutures;

	MarketEye	m_marketEye;
	BOOL	m_bReconnectTry;
	int		m_nReconnectCount;
	BOOL	m_bForceLogout;
	BOOL	m_bLogout;
	BOOL	m_bConnectToSub;
	list<OrderResult*>		m_orderEvents;
};


