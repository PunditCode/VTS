#pragma once

#include "GTCodeManager.h"
#include <DKMsgOnlyWnd.h>
#include "GTLibExt/VTSCurrentBase.h"

struct OrderContractNotifyItem
{
	Code* pCode;
	MarketType marketType;
	AddContractType acType;
	OrderContracted* pContract;
};

struct ContractedOrder
{
	CString id, name;
	UINT hWndRet;
	Code* pCode;
	int nCodeID;
	MarketType marketType;
	CString strPrice;
	PositionType posType;
	GT_VOLUME nVolume;
	COleDateTime time;
};


class ServerEventReceiver : public DKMsgOnlyWnd
{
	friend class Session;
public:
	ServerEventReceiver(void);
	virtual ~ServerEventReceiver(void);

	int RegisterCurrent(Code* pCode, MarketType marketType, VTSCurrentBase* pCurrent);
	BOOL UnregisterCurrent(Code* pCode, MarketType marketType, VTSCurrentBase* pCurrent);
	int RegisterHoga(Code* pCode, MarketType marketType, VTSCurrentBase* pCurrent);
	BOOL UnregisterHoga(Code* pCode, MarketType marketType, VTSCurrentBase* pCurrent);

	BOOL RegisterOrderContracted(CWnd* pWnd);
	BOOL UnregisterOrderContracted(CWnd* pWnd);

	BOOL RegisterOrderOutstanding(CWnd* pWnd);
	BOOL UnregisterOrderOutstanding(CWnd* pWnd);

	BOOL RegisterClientInfo(CWnd* pWnd);
	BOOL UnregisterClientInfo(CWnd* pWnd);

	const OrderOutstandings& GetOutstandings() { return m_orderOutstds; }
	const OrderContracts& GetContracts() { return m_orderContracts; }
	Profit* GetProfit() { return m_pProfit; }

	BOOL ReRegisterCurrentHoga();

	VTSCurrentBase* GetCurrentBase(GTMerchandise::IDType idCode);
	VTSCurrentBase* GetHogaBase(GTMerchandise::IDType idCode);

protected:
	void OnCurrentUpdated(DKBuffer& dbuf, VTSS_NOTIFY_TYPE notifyType);

	void OnHogaInitUpdated(DKBuffer& dbuf);
	void OnHogaUpdated(DKBuffer& dbuf);

	void OnContractedOrder(DKBuffer& dbuf);
	void OnOrderContracts(DKBuffer& dbuf);
	void OnOrderOutstandings(DKBuffer& dbuf);
	void OnProfitUpdated(DKBuffer& dbuf);
	void OnClientInfoUpdated();
private:
	// 요소가 3개까지는 vector가 가장빠르다.
	struct Item
	{
		GTMerchandise::IDType	id;
		VTSCurrentBase*	pCurrent;
		int	nCount;
		GTMerchandisePtr	pMerch;

		Item() : nCount(0) {}
	};
	typedef std::vector<Item>	Currents;

	Currents m_currents;
	Currents m_hogas;
	
	typedef set<CWnd*>		_Windows;
	_Windows	m_wndsOrderContracted;
	_Windows	m_wndsOrderOutstanding;
	_Windows	m_wndsClientInfo;

	OrderOutstandings	m_orderOutstds;
	OrderContracts		m_orderContracts;
	Profit*				m_pProfit;

	CCriticalSection	m_syncCurrent, m_syncHoga;

	std::map<GTMerchandise::IDType, GTMerchandisePtr>	m_merchandises;
};
