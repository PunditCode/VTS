#include "StdAfx.h"
#include "VTS.h"
#include "ServerEventReceiver.h"
#include <DKBuffer.h>

using namespace std;


ServerEventReceiver::ServerEventReceiver(void) : m_pProfit(NULL)
{
}


ServerEventReceiver::~ServerEventReceiver(void)
{
	m_currents.clear();
	m_hogas.clear();
}

int ServerEventReceiver::RegisterCurrent(Code* pCode, MarketType marketType, VTSCurrentBase* pCurrent)
{
	GTMerchandise::IDType idCode(pCode, marketType);
	BOOL bFound = FALSE;
	
	for(auto pos = m_currents.begin() ; pos != m_currents.end() ; ++pos)
	{
		Item& item = *pos;
		if(item.id != idCode)
			continue;
		
		++item.nCount;
		bFound = TRUE;
		break;
	}
	if(!bFound)
	{
		TRACE(_T("ServerEventReceiver::RegisterCurrent(%s)\n"), pCode->GetCode());
		SCOPE_LOCK(m_syncCurrent);
		
		Item item;
		item.id = idCode;
		item.pCurrent = pCurrent;
		item.nCount = 1;
		item.pMerch = g_pMP->GetObject(idCode);

		m_currents.push_back(item);
	}

	if(!GetSession()->RequestMerchandiseCurrent(idCode))
		return -1;

	return 1;
}

BOOL ServerEventReceiver::UnregisterCurrent(Code* pCode, MarketType marketType, VTSCurrentBase* pCurrent)
{
	GTMerchandise::IDType idCode(pCode, marketType);
	BOOL bFound = FALSE;
	
	for(auto pos = m_currents.begin() ; pos != m_currents.end() ; ++pos)
	{
		Item& item = *pos;
		if(item.id != idCode)
			continue;

		--item.nCount;
		if(item.nCount == 0)
		{
			TRACE(_T("ServerEventReceiver::UnregisterCurrent(%s)\n"), pCode->GetCode());
			SCOPE_LOCK(m_syncCurrent);
			m_currents.erase(pos);
			GetSession()->RequestMerchandiseCurrent(idCode, TRUE);
			return TRUE;
		} 
	}
	return FALSE;
}

int ServerEventReceiver::RegisterHoga(Code* pCode, MarketType marketType, VTSCurrentBase* pCurrent)
{
	GTMerchandise::IDType idCode(pCode, marketType);
	
	BOOL bFound = FALSE;
	for (auto pos = m_hogas.begin(); pos != m_hogas.end(); ++pos)
	{
		Item& item = *pos;
		if (item.id != idCode)
			continue;

		++item.nCount;
		bFound = TRUE;
		break;
	}
	if (!bFound)
	{
		TRACE(_T("ServerEventReceiver::RegisterHoga(%s)\n"), pCode->GetCode());
		SCOPE_LOCK(m_syncHoga);

		Item item;
		item.id = idCode;
		item.pCurrent = pCurrent;
		item.nCount = 1;
		item.pMerch = g_pMP->GetObject(idCode);
		m_hogas.push_back(item);
	}
		
	if(!GetSession()->RequestMerchandiseHoga(idCode))
		return -1;
	return 1;
}


BOOL ServerEventReceiver::UnregisterHoga(Code* pCode, MarketType marketType, VTSCurrentBase* pCurrent)
{
	GTMerchandise::IDType idCode(pCode, marketType);
	BOOL bFound = FALSE;

	
	for (auto pos = m_hogas.begin(); pos != m_hogas.end(); ++pos)
	{
		Item& item = *pos;
		if (item.id != idCode)
			continue;

		--item.nCount;
		if (item.nCount == 0)
		{
			TRACE(_T("ServerEventReceiver::UnregisterHoga(%s)\n"), pCode->GetCode());
			SCOPE_LOCK(m_syncHoga);
			m_hogas.erase(pos);
			GetSession()->RequestMerchandiseHoga(idCode, TRUE);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ServerEventReceiver::ReRegisterCurrentHoga()
{
	{
		SCOPE_LOCK(m_syncCurrent);
		for (auto pos = m_currents.begin(); pos != m_currents.end(); ++pos)
		{
			Item& item = *pos;
			GetSession()->RequestMerchandiseCurrent(item.id);
		}
	}

	{
		SCOPE_LOCK(m_syncHoga);
		for (auto pos = m_hogas.begin(); pos != m_hogas.end(); ++pos)
		{
			Item& item = *pos;
			GetSession()->RequestMerchandiseHoga(item.id);
		}
	}
	
	return TRUE;
}

void ServerEventReceiver::OnCurrentUpdated(DKBuffer& dbuf, VTSS_NOTIFY_TYPE notifyType)
{
	int nCode = NUL_IDX;
	dbuf.RD(nCode);
	MarketType marketType;
	dbuf.RD(marketType);

	Code* pCode = InstCM().GetCode(nCode);
	ASSERT(pCode);

	UINT nPacketLen;
	dbuf.RD(nPacketLen);

	GTMerchandise::IDType idCode(pCode, marketType);

	SCOPE_LOCK(m_syncCurrent);
	for(auto pos = m_currents.begin() ; pos != m_currents.end() ; ++pos)
	{
		Item& item = *pos;
		if(item.id != idCode)
			continue;

		if (VTSS_NOTF_MERCHANDISE_CURRENT == notifyType)
		{
			item.pCurrent->Receive(dbuf.GetCur(), nPacketLen);
			return;
		}
		//else if (VTSS_NOTF_MERCHANDISE_CURRENT_INIT == notifyType)
		item.pCurrent->InitReceive(dbuf.GetCur(), nPacketLen);
		return;
	}
}

VTSCurrentBase* ServerEventReceiver::GetCurrentBase(GTMerchandise::IDType idCode)
{
	SCOPE_LOCK(m_syncCurrent);
	for(auto pos = m_currents.begin() ; pos != m_currents.end() ; ++pos)
	{
		Item& item = *pos;
		if(item.id != idCode)
			continue;

		return item.pCurrent;
	}
	return NULL;
}

VTSCurrentBase* ServerEventReceiver::GetHogaBase(GTMerchandise::IDType idCode)
{
	SCOPE_LOCK(m_syncHoga);
	for (auto pos = m_hogas.begin(); pos != m_hogas.end(); ++pos)
	{
		Item& item = *pos;
		if (item.id != idCode)
			continue;

		return item.pCurrent;
	}
	return NULL;
}

void ServerEventReceiver::OnHogaInitUpdated(DKBuffer& dbuf)
{
	int nCode = NUL_IDX;
	dbuf.RD(nCode);
	MarketType marketType;
	dbuf.RD(marketType);

	Code* pCode = InstCM().GetCode(nCode);
	ASSERT(pCode);

	UINT nPacketLen;
	dbuf.RD(nPacketLen);

	GTMerchandise::IDType idCode(pCode, marketType);

	SCOPE_LOCK(m_syncHoga);
	for (auto pos = m_hogas.begin(); pos != m_hogas.end(); ++pos)
	{
		Item& item = *pos;
		if (item.id != idCode)
			continue;

		item.pCurrent->InitReceive(dbuf.GetCur(), nPacketLen);
		return;
	}
}


void ServerEventReceiver::OnHogaUpdated(DKBuffer& dbuf)
{
	int nCode = NUL_IDX;
	dbuf.RD(nCode);
	MarketType marketType;
	dbuf.RD(marketType);

	Code* pCode = InstCM().GetCode(nCode);
	ASSERT(pCode);

	UINT nPacketLen;
	dbuf.RD(nPacketLen);

	GTMerchandise::IDType idCode(pCode, marketType);
	SCOPE_LOCK(m_syncHoga);

	for (auto pos = m_hogas.begin(); pos != m_hogas.end(); ++pos)
	{
		Item& item = *pos;
		if (item.id != idCode)
			continue;

		item.pCurrent->Receive(dbuf.GetCur(), nPacketLen);
		return;
	}
}


BOOL ServerEventReceiver::RegisterOrderContracted(CWnd* pWnd)
{
	auto pos = m_wndsOrderContracted.find(pWnd);
	if(m_wndsOrderContracted.end() != pos)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	BOOL bRequest = m_wndsOrderContracted.empty();
	m_wndsOrderContracted.insert(pWnd);

	return TRUE;
}


BOOL ServerEventReceiver::UnregisterOrderContracted(CWnd* pWnd)
{
	auto pos = m_wndsOrderContracted.find(pWnd);
	if(m_wndsOrderContracted.end() == pos)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_wndsOrderContracted.erase(pos);
	return TRUE;
}


BOOL ServerEventReceiver::RegisterOrderOutstanding(CWnd* pWnd)
{
	auto pos = m_wndsOrderOutstanding.find(pWnd);
	if(m_wndsOrderOutstanding.end() != pos)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	BOOL bRequest = m_wndsOrderOutstanding.empty();
	m_wndsOrderOutstanding.insert(pWnd);

	return TRUE;
}

BOOL ServerEventReceiver::UnregisterOrderOutstanding(CWnd* pWnd)
{
	auto pos = m_wndsOrderOutstanding.find(pWnd);
	if(m_wndsOrderOutstanding.end() == pos)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_wndsOrderOutstanding.erase(pos);
	
	return TRUE;
}

void ServerEventReceiver::OnOrderOutstandings(DKBuffer& dbuf)
{
	//DLOG(_T(""));
	for(auto pos = m_orderOutstds.begin() ; pos != m_orderOutstds.end() ; ++pos)
	{
		delete *pos;
	}
	m_orderOutstds.clear();
	
	UINT nCount;
	int nTemp;

	dbuf.RD(nCount);

	for(UINT i = 0 ; i < nCount ; ++i)
	{
		OrderOutstanding* p = new OrderOutstanding;

		dbuf.RD(nTemp);
		
		p->pCode = InstCM().GetCode(nTemp);
		ASSERT(p->pCode);
		dbuf.RD(p->marketType);
		dbuf.RD(p->position);
		dbuf.RD(p->nOrderNum);
		dbuf.RD(p->strPrice);
		dbuf.RD(p->nVolume);
		dbuf.RD(p->nVolumeRemain);
		p->dPrice = ::ToDecimal(p->strPrice);
		dbuf.RD(p->bHogaFakeUpdate);
		dbuf.RD(p->bRealOrderFailed);

		m_orderOutstds.push_back(p);
	}
	int nNewIndex;
	dbuf.RD(nNewIndex);
	for(auto pos = m_wndsOrderOutstanding.begin() ; pos != m_wndsOrderOutstanding.end() ; ++pos)
	{
		(*pos)->SendMessage(WM_ORDER_OUTSTANDING, (WPARAM)&m_orderOutstds, (LPARAM)nNewIndex);
	}
}

void ServerEventReceiver::OnContractedOrder(DKBuffer& dbuf)
{
	ContractedOrder cont;
		
	dbuf.RD(cont.hWndRet);
	dbuf.RD(cont.id);
	dbuf.RD(cont.name);
	
	int nCode;
	dbuf.RD(nCode);
	cont.pCode = InstCM().GetCode(nCode);

	dbuf.RD(cont.marketType);
	dbuf.RD(cont.posType);
	dbuf.RD(cont.strPrice);
	dbuf.RD(cont.nVolume);
	DATE dt;
	dbuf.RD(dt);
	cont.time = dt;
	
	
	for(auto pos = m_wndsOrderContracted.begin() ; pos != m_wndsOrderContracted.end() ; ++pos)
	{
		(*pos)->SendMessage(WM_CONTRACTED_ORDER, (WPARAM)&cont);
	}
}


void ServerEventReceiver::OnOrderContracts(DKBuffer& dbuf)
{
	if(NULL == m_pProfit)
	{
		m_pProfit = new Profit;
	}
	m_pProfit->Empty();

	m_orderContracts.clear();

	CString id, name;
	UINT nCount;
	int nTemp;
	
	MarketType marketType;

	dbuf.RD(id);
	dbuf.RD(name);

	ASSERT(GetSession()->id == id);
	ASSERT(GetSession()->name = name);
		
	dbuf.RD(nTemp); //Code ID
	Code* pCodeContracted = NULL;
	if(nTemp != -1)
		pCodeContracted = InstCM().GetCode(nTemp);

	dbuf.RD(marketType);
	AddContractType acType;
	dbuf.RD(acType); //해당종목이 모두 청산된건지
	dbuf.RD(nCount);

	OrderContracted* pContract = NULL;
	for(UINT i = 0 ; i < nCount ; ++i)
	{
		OrderContractedPtr p(new OrderContracted);
		dbuf.RD(nTemp);
		
		p->pCode = InstCM().GetCode(nTemp);
		ASSERT(p->pCode);
		dbuf.RD(p->marketType);
		dbuf.RD(p->position);
		dbuf.RD(p->strPrice);
		dbuf.RD(p->nVolume);
		dbuf.RD(p->strCurPrice);
		dbuf.RD(p->profitEval);
		p->dPrice = ::ToDecimal(p->strPrice);
		m_orderContracts.push_front(p);
		DLOG(_T("Recv contract %d"), p->nVolume);

		if(pCodeContracted == p->pCode && marketType == p->marketType)
			pContract = p.get();
	}

	dbuf.RD(m_pProfit->profitEvalSum);
	dbuf.RD(m_pProfit->profitRealize);
	dbuf.RD(m_pProfit->lossCut);
	dbuf.RD(m_pProfit->accountBalanceEval);
	dbuf.RD(m_pProfit->depositForFuturesOvernightTot);
	dbuf.RD(m_pProfit->depositForOptionsOvernightTot);
	dbuf.RD(m_pProfit->depositForForeignFuturesOvernightTot);

	OrderContractNotifyItem notf;
	notf.pCode = pCodeContracted;
	notf.marketType = marketType;
	notf.acType = acType;
	notf.pContract = pContract;
	
	for(auto pos = m_wndsOrderContracted.begin() ; pos != m_wndsOrderContracted.end() ; ++pos)
	{
		(*pos)->SendMessage(WM_ORDER_CONTRACTS, (WPARAM)&notf);
	}

}

void ServerEventReceiver::OnProfitUpdated(DKBuffer& dbuf)
{
	OrderContracted obj;
	OrderContracted* p = &obj;//new OrderContracted;

	if(NULL == m_pProfit)
	{
		m_pProfit = new Profit;
	}
	//m_pProfit->Empty();

	int nTemp;
	dbuf.RD(nTemp);

	p->pCode = InstCM().GetCode(nTemp);
	dbuf.RD(p->marketType);
	dbuf.RD(p->position);
	dbuf.RD(p->strPrice);
	dbuf.RD(p->nVolume);
	dbuf.RD(p->strCurPrice);
	dbuf.RD(p->profitEval);

	dbuf.RD(m_pProfit->profitEvalSum);
	dbuf.RD(m_pProfit->accountBalanceEval);
	dbuf.RD(m_pProfit->depositForFuturesOvernightTot);
	dbuf.RD(m_pProfit->depositForOptionsOvernightTot);
	dbuf.RD(m_pProfit->depositForForeignFuturesOvernightTot);

	
	for(auto pos = m_orderContracts.begin() ; pos != m_orderContracts.end() ; ++pos)
	{
		if(p->pCode == (*pos)->pCode)
			*(*pos) = obj;
	}

	for(auto pos = m_wndsOrderContracted.begin() ; pos != m_wndsOrderContracted.end() ; ++pos)
	{
		(*pos)->SendMessage(WM_PROFIT_UPDATED, (WPARAM)p, (LPARAM)m_pProfit);
	}
}

BOOL ServerEventReceiver::RegisterClientInfo(CWnd* pWnd)
{
	auto pos = m_wndsClientInfo.find(pWnd);
	if(m_wndsClientInfo.end() != pos)
	{
		return FALSE;
	}
	BOOL bRequest = m_wndsClientInfo.empty();
	m_wndsClientInfo.insert(pWnd);

	return TRUE;
}

BOOL ServerEventReceiver::UnregisterClientInfo(CWnd* pWnd)
{
	auto pos = m_wndsClientInfo.find(pWnd);
	if(m_wndsClientInfo.end() == pos)
	{
		return FALSE;
	}
	m_wndsClientInfo.erase(pos);
	
	return TRUE;
}

void ServerEventReceiver::OnClientInfoUpdated()
{
	for(auto pos = m_wndsClientInfo.begin() ; pos != m_wndsClientInfo.end() ; ++pos)
	{
		(*pos)->PostMessage(WM_CLIENT_INFO_UPDATED);
	}
}
