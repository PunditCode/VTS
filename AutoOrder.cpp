#include "StdAfx.h"
#include "VTS.h"
#include "AutoOrder.h"
#include "ServerEventReceiver.h"
#include <boost\foreach.hpp>

#define _AVR_PRICE_PLUS_1

AutoOrder::AutoOrder(GTCurrentPtr pCurrent, AutoOrderManager* pOwner) : m_nMax(0), m_nMin(0), m_nHogaHeight(0), m_nHogaUnit(0)
{
	ASSERT(pCurrent);
	m_bInit = FALSE;
	m_pCurrent = pCurrent;
	m_codeID = make_pair(pCurrent->GetCode(), pCurrent->GetMerchandise()->GetMarketType());
	m_pOwner = pOwner;

	m_pCurrent->AttachObserver(this);

	m_bLosscut = FALSE;
	m_bEarncut = FALSE;

	m_nLosscutTick = 5;
	m_nEarncutTick = 5;
		
	m_pFncGetRowIndex = NULL;
	m_pFncGetRowIndexForContract = NULL;

	m_nContractRow = NUL_IDX;

	m_contractPos = PT_NONE;
	m_nLosscutRow = NUL_IDX;
	m_nEarncutRow = NUL_IDX;
	

	TRACE(_T("AutoOrder Created(%s)\n"), m_codeID.first->GetCode());
}


AutoOrder::~AutoOrder(void)
{
	
	m_pCurrent->DetachObserver(this);

	TRACE(_T("AutoOrder deleted(%s)\n"), m_codeID.first->GetCode());
}

int AutoOrder::_GetRowIndex(int nPrice)
{
	ASSERT(m_nMax != 0 && m_nHogaUnit != 0);
	return (m_nMax - nPrice) / m_nHogaUnit;
}

int AutoOrder::_GetRowIndexOptions(int nPrice)
{
	int nRow = 0;
	if(nPrice >= 300)
	{
		nRow = (m_nMax - nPrice) / 5;
	}
	else
	{
		nRow = (m_nMax - 300) / 5 + 1;
		nRow += 299 - nPrice;
	}
	return nRow;
}

int AutoOrder::_GetRowIndexForContract(int nPrice)
{
	ASSERT(m_nMax != 0 && m_nHogaUnit != 0);
#ifdef _AVR_PRICE_PLUS_1
	//체결가는 평균가로서 1자리수가 더해서 넘어온다. 그래서 m_nMax에 10을곱해 한자리수를 더 만든 상태에서 인덱스를 구한다.
	int nRow = (m_nMax * 10 - nPrice) / (m_nHogaUnit * 10);
	if((m_nMax * 10 - nPrice) % (m_nHogaUnit * 10) > 5) //반올림
		nRow += 1;
#else
	int nRow = (m_nMax - nPrice) / (m_nHogaUnit);
	DOUBLE dHalfHoga = m_nHogaUnit * 0.5;

	//선물 250.02 일때는 250으로 250.03일때는 250.05로

	int nRest = (nPrice % m_nHogaUnit);

	if(nRest != 0 && nRest < dHalfHoga)
		++nRow;
#endif
	return nRow;
}

int AutoOrder::_GetRowIndexOptionsForContract(int nPrice)
{
	int nRow = 0;
#ifdef _AVR_PRICE_PLUS_1
	if(nPrice >= 3000)
	{
		nRow = (m_nMax * 10 - nPrice) / (5 * 10);
		if((m_nMax * 10 - nPrice) % (5 * 10) > 5) //반올림
			nRow += 1;
	}
	else
	{
		nRow = (m_nMax - 300) / 5 + 1; // 3.00의 인덱스에 +1
		nRow += (2990 - nPrice) / 10; //2.99부터 현재가까지 인덱스 카운트

		if((2990 - nPrice) % 10 > 5)
			nRow += 1;
	}
#else
	if(nPrice >= 300)
	{
		nRow = (m_nMax - nPrice) / 5;
		
		int nRest = (nPrice % 5);
		if(nRest != 0 && nRest < 2.5)
			++nRow;
	}
	else
	{
		nRow = (m_nMax - 300) / 5 + 1; // 3.00의 인덱스에 +1
		nRow += (299 - nPrice); //2.99부터 현재가까지 인덱스 카운트
	}
#endif

	return nRow;
}

void AutoOrder::InitHoga(GTCurrentPtr pCurrent)
{
	if(FALSE == m_bInit)
	{
		ASSERT(m_pCurrent == pCurrent);
		m_nContractRow = NUL_IDX;
		m_contractPos = PT_NONE;

		m_nMax = pCurrent->uplmtprice.DoubleToInt();
		m_nMin = pCurrent->dnlmtprice.DoubleToInt();
	

		m_nHogaHeight = m_nMax - m_nMin;

		m_nHogaUnit = 0;
		m_bOptionsOverlppedPrice3_0 = FALSE;

		m_pFncGetRowIndex = &AutoOrder::_GetRowIndex;
		m_pFncGetRowIndexForContract = &AutoOrder::_GetRowIndexForContract;
		switch(pCurrent->GetCode()->GetType())
		{
		case ct_futures: 
			m_nHogaUnit = 5; break;
		case ct_options:
			if(300 >= m_nMax)
			{
				m_nHogaUnit = 1;
				m_bOptionsOverlppedPrice3_0 = FALSE;
			}
			else if(m_nMin >= 300)
			{
				m_nHogaUnit = 5;
				m_bOptionsOverlppedPrice3_0 = FALSE;
			}
			else
			{
				m_pFncGetRowIndex = &AutoOrder::_GetRowIndexOptions;
				m_pFncGetRowIndexForContract = &AutoOrder::_GetRowIndexOptionsForContract;

				m_bOptionsOverlppedPrice3_0 = TRUE;
				m_nHogaUnit = 0;
			}
			break;
		case ct_foreign_futures:
			{
				CodeForeignFutures* pCode = dynamic_cast<CodeForeignFutures*>(m_codeID.first);
				ASSERT(pCode);
				m_nHogaUnit = pCode->hogaUnitPrice.DoubleToInt();
			}
			break;
		default:
			ASSERT(FALSE);
		}

		m_nHogaRowCount = 0;
		if(m_bOptionsOverlppedPrice3_0)
			m_nHogaRowCount = (((m_nMax - 300) / 5) + (300 - m_nMin)) + 1;
		else
			m_nHogaRowCount = (m_nHogaHeight / m_nHogaUnit) + 1;
	}


	const OrderContracts& conts = SER()->GetContracts();
	OrderContractedPtr pOrder;
	for(auto posCont = conts.begin() ; posCont != conts.end() ; ++posCont)
	{
		if((*posCont)->pCode == pCurrent->GetCode())
		{
			pOrder = *posCont;
			OnOrderContracts(act_none, pOrder.get());
			break;
		}
	}
	m_bInit = TRUE;
}

void AutoOrder::NotifyLiquid()
{
	if(m_contractPos == PT_NONE)
		return;

	BEGIN_OBSERVER_ITERATION(AutoOrderObserver)
	{
		pObserver->OnAutoOrderSetLossCut(m_bLosscut, m_nLosscutTick, m_nLosscutRow, NUL_IDX);
		pObserver->OnAutoOrderSetEarnCut(m_bEarncut, m_nEarncutTick, m_nEarncutRow, NUL_IDX);
		pObserver->OnAutoOrderContractChanged(m_nContractRow, NUL_IDX, PT_NONE);
	}
	END_OBSERVER_ITERATION()

	m_nLosscutRow = NUL_IDX;
	m_nEarncutRow = NUL_IDX;
	m_nContractRow = NUL_IDX;
	m_contractPos = PT_NONE;	
}
void AutoOrder::OnOrderContracts(AddContractType acType, OrderContracted* pOrderCont)
{
	if (act_all_liquid == acType || act_reverse == acType)
	{
		NotifyLiquid();
		if(act_all_liquid == acType)
			return;
	}

	CHECK_RET(pOrderCont, );

	// 매도일때 익절:BOTTOM Row+Tick, 손절:TOP Row-Tick
	// 매수일때 익절:TOP Row-Tick, 손절: BOTTOM Row + Tick
	m_contractPos = pOrderCont->position;
	m_nVolumeContract = pOrderCont->nVolume;
	CString strPrice = pOrderCont->strPrice;
#ifdef _AVR_PRICE_PLUS_1
	int nDotPos = strPrice.ReverseFind(_T('.'));
	if(NUL_IDX == nDotPos)
		nDotPos = 0;
	else
		nDotPos -= 1;

	int nDotDiff = nDotPos - pOrderCont->pCode->nPricePointPos + 1;
#endif

	strPrice.Remove(_T('.'));
	INT nPrice = ::ToINT(strPrice);
#ifdef _AVR_PRICE_PLUS_1
	if(nDotDiff != 0)
	{
		int nOffset = (int)pow((double)10, 0);
		nPrice *= nOffset;
	}
#endif
	ASSERT(m_pFncGetRowIndexForContract);

	int nNewContRow = (this->*m_pFncGetRowIndexForContract)(nPrice);

	BEGIN_OBSERVER_ITERATION(AutoOrderObserver)
	{
		pObserver->OnAutoOrderContractChanged(m_nContractRow, nNewContRow, m_contractPos);
	} 
	END_OBSERVER_ITERATION()
	m_nContractRow = nNewContRow;

	CString s;
	SetLosscut(m_bLosscut, m_nLosscutTick, NULL, s);
	SetEarncut(m_bEarncut, m_nEarncutTick, NULL, s);
}

BOOL AutoOrder::SetLosscut(BOOL bEnable, int nTick, AutoOrderObserver* pCaller, CString& strMsg)
{
	// 매도일때 익절:BOTTOM Row+Tick, 손절:TOP Row-Tick
	// 매수일때 익절:TOP Row-Tick, 손절: BOTTOM Row + Tick
	m_nLosscutTick = nTick;
	m_bLosscut = bEnable;
	
	int nCutRow = NUL_IDX;
	if(bEnable && m_contractPos != PT_NONE)
	{
		nCutRow = m_nContractRow + ((m_contractPos == PT_SELL) ? -nTick : nTick);
		int nPrice = m_pCurrent->price.DoubleToInt();
		int nRow = GetRowIndex(nPrice);

		if((m_contractPos == PT_BUY && nRow > nCutRow) || (m_contractPos == PT_SELL && nRow < nCutRow))
		{
			strMsg = _T("범위를 벗어났습니다.");
			return FALSE;
		}
	}

	BEGIN_OBSERVER_ITERATION(AutoOrderObserver)
	{
		if (pCaller == pObserver)
			continue;
		pObserver->OnAutoOrderSetLossCut(bEnable, nTick, m_nLosscutRow, nCutRow);
	}
	END_OBSERVER_ITERATION()

	m_nLosscutRow = nCutRow;
	return TRUE;
}

BOOL AutoOrder::SetEarncut(BOOL bEnable, int nTick, AutoOrderObserver* pCaller, CString& strMsg)
{
	// 매도일때 익절:BOTTOM Row+Tick, 손절:TOP Row-Tick
	// 매수일때 익절:TOP Row-Tick, 손절: BOTTOM Row + Tick
	int nCutRow = NUL_IDX;
	if(bEnable && m_contractPos != PT_NONE)
	{
		nCutRow = m_nContractRow + ((m_contractPos == PT_SELL) ? nTick : -nTick);

		int nPrice = m_pCurrent->price.DoubleToInt();
		int nRow = GetRowIndex(nPrice);

		if((m_contractPos == PT_BUY && nRow < nCutRow) || (m_contractPos == PT_SELL && nRow > nCutRow))
		{
			strMsg = _T("범위를 벗어났습니다.");
			return FALSE;
		}
	}

	m_nEarncutTick = nTick;
	m_bEarncut = bEnable;

	BEGIN_OBSERVER_ITERATION(AutoOrderObserver)
	{
		if (pCaller == pObserver)
			continue;
		pObserver->OnAutoOrderSetEarnCut(bEnable, nTick, m_nEarncutRow, nCutRow);
	}
	END_OBSERVER_ITERATION()

	m_nEarncutRow = nCutRow;
	return TRUE;
}

BOOL AutoOrder::MIT_New(PositionType posType, const CString& strPrice, GT_VOLUME nVolume, CString& strMsg)
{
	if(m_pCurrent->price.ToString() == strPrice)
	{
		//현재가에 MIT를 설정할수 없다.
		strMsg = _T("현재가 MIT주문은 불가합니다. 시장가 주문을 이용해주시기 바랍니다");
		return FALSE;
	}

	int nPriceNoPoint = DKStr2Val(strPrice).DoubleToInt();
	auto pos = m_mits.find(nPriceNoPoint);

	if(pos == m_mits.end())
	{
		MITItem item;
		item.position = posType;
		item.tm = CTime::GetCurrentTime();
		item.strPrice = strPrice;
		pos = m_mits.insert(m_mits.begin(), std::make_pair(nPriceNoPoint, item));
	}

	MITItem& item = pos->second;
	if (posType != item.position)
	{
		strMsg = _T("같은가격의 서로다른 포지션주문은 불가합니다.");
		return FALSE;
	}

	if(posType == PT_SELL)
	{
		item.dir = (nPriceNoPoint > m_pCurrent->price.DoubleToInt()) ? dir_forward : dir_backward;
	}
	else
	{
		item.dir = (nPriceNoPoint < m_pCurrent->price.DoubleToInt()) ? dir_forward : dir_backward;
	}

	item.nVolume += nVolume;
	NotifyMITUpdate(posType, strPrice, item.nVolume); 
	return TRUE;
}

BOOL AutoOrder::MIT_Correction(PositionType posType, const CString& strPriceOrg, const CString& strPriceNew, OUT CString& strMsg)
{
	//////////////////////////////////////////////
	// 원 MIT주문은 취소한다.
	int nPriceNoPoint = DKStr2Val(strPriceOrg).DoubleToInt();
	auto pos = m_mits.find(nPriceNoPoint);
	CHECK(pos != m_mits.end());

	MITItem item = pos->second;
	ASSERT(item.position == posType);
	m_mits.erase(pos);

	NotifyMITUpdate(posType, strPriceOrg, 0); 
	//////////////////////////////////////////////

	return MIT_New(posType, strPriceNew, item.nVolume, strMsg);
}

BOOL AutoOrder::MIT_Cancel(PositionType posType, const CString& strPrice)
{
	//////////////////////////////////////////////
	// 원 MIT주문은 취소한다.
	int nPriceNoPoint = DKStr2Val(strPrice).DoubleToInt();
	auto pos = m_mits.find(nPriceNoPoint);
	CHECK(pos != m_mits.end());

	MITItem item = pos->second;
	ASSERT(item.position == posType);
	m_mits.erase(pos);

	NotifyMITUpdate(posType, strPrice, 0); 
	
	return TRUE;
	//////////////////////////////////////////////
}

void AutoOrder::MIT_AllCancel()
{
	MITs mits = m_mits; // CFastTradeDlg에서 UpdateOrderPanel()호출할때 비어있게 하기 위해서
	m_mits.clear();

	for(auto pos = mits.begin() ; pos != mits.end() ; ++pos)
	{
		MITItem& item = pos->second;
		NotifyMITUpdate(item.position, pos->second.strPrice, 0); 
	}
}

void AutoOrder::MIT_SellCancel()
{
	vector<INT> delKeys;
	BOOL bDelete = FALSE;
	for(auto pos = m_mits.begin() ; pos != m_mits.end() ; ++pos)
	{
		MITItem& item = pos->second;
		if (item.position == PT_SELL)
		{
			bDelete = TRUE;
			delKeys.push_back(pos->first);
		}
	}
	if(FALSE == bDelete)
		return;

	for (auto posDel = delKeys.begin(); posDel != delKeys.end(); ++posDel)
	{
		auto pos = m_mits.find(*posDel);
		MITItem item = pos->second;
		m_mits.erase(pos);
		NotifyMITUpdate(item.position, item.strPrice, 0);
	}
}

void AutoOrder::MIT_BuyCancel()
{
	vector<INT> delKeys;
	BOOL bDelete = FALSE;
	for(auto pos = m_mits.begin() ; pos != m_mits.end() ; ++pos)
	{
		MITItem& item = pos->second;
		if (item.position == PT_BUY)
		{
			bDelete = TRUE;
			delKeys.push_back(pos->first);
		}
	}
	if(FALSE == bDelete)
		return;

	for (auto posDel = delKeys.begin(); posDel != delKeys.end(); ++posDel)
	{
		auto pos = m_mits.find(*posDel);
		MITItem item = pos->second;
		m_mits.erase(pos);
		NotifyMITUpdate(item.position, item.strPrice, 0);
	}
}

BOOL AutoOrder::Order(PositionType posType, const CString& strPrice, GT_VOLUME nVolume, OrderMethod method)
{
	CString strVolume = ::ToString(nVolume);

	ORDER_ACTOR actor;
	CString strAction;

	if (om_mit == method)
		strAction.Format(_T("%s: 신규주문(지정가:%s, 신호가:%s)"), OrderMethodStr(method, actor), strPrice, m_pCurrent->price.ToString());
	else
		strAction.Format(_T("%s: 신규주문"), OrderMethodStr(method, actor));

	GetSession()->SendOrderAction(strAction, m_pCurrent->GetCode()->GetCode(), _T("0"), posType, strVolume);

	BOOL bRet = GetSession()->Order(NULL, m_pCurrent->GetCode(), 
		m_pCurrent->GetMerchandise()->GetMarketType(), posType, ht_SiJangGa, _T("0"), strVolume, actor);
	return bRet;
}

void AutoOrder::OnCurrentUpdated(GTCurrentPtr pCurrent)
{
	//if(!pCurrent->IsPriceChanged())
	//	return;
	
	int nPrice = pCurrent->price.DoubleToInt();
	int nRow = GetRowIndex(nPrice);

	if(NUL_IDX != m_nLosscutRow)
	{
		if((m_contractPos == PT_BUY && m_nLosscutRow <= nRow) || (m_contractPos == PT_SELL && m_nLosscutRow >= nRow))
		{
			//기존 MIT주문과 해당종목의 미체결은 모두 취소한다.
			MIT_AllCancel();
			GetSession()->OrderCancelAllByPosition(NULL, m_pCurrent->GetCode(), PT_NONE);
			
			//매수 주문일때 로스컷은 현재가가 매수가 보다 낮을경우 즉 로스컷보다 더 내려간경우(index는 증가) 손절
			Order(m_contractPos == PT_SELL ? PT_BUY : PT_SELL, _T("0"), m_nVolumeContract, om_stop_losscut);
			NotifyLiquid();
			return;
		}
	}

	if(NUL_IDX != m_nEarncutRow)
	{
		if((m_contractPos == PT_BUY && m_nEarncutRow >= nRow) || (m_contractPos == PT_SELL && m_nEarncutRow <= nRow))
		{
			MIT_AllCancel();
			GetSession()->OrderCancelAllByPosition(NULL, m_pCurrent->GetCode(), PT_NONE);
			
			Order(m_contractPos == PT_SELL ? PT_BUY : PT_SELL, _T("0"), m_nVolumeContract, om_stop_earncut);
			NotifyLiquid();
			return;
		}
	}

	if (false == m_mits.empty())
	{
		const CString& strPrice = pCurrent->price.ToString();
		INT nPriceNoPoint = pCurrent->price.DoubleToInt();

		auto pos = m_mits.find(nPriceNoPoint);

		list<INT> delKeys;

		for (auto pos = m_mits.begin(); pos != m_mits.end(); ++pos)
		{
			MITItem& item = pos->second;
			
			BOOL bOrder = FALSE;
			
			if (pos->first == nPriceNoPoint)
				bOrder = TRUE;

			if (FALSE == bOrder)
			{
				if (item.position == PT_BUY)
				{
					if (item.dir == dir_forward)
					{
						if (nPriceNoPoint < pos->first)
							bOrder = TRUE;
					}
					else if (item.dir == dir_backward)
					{
						if (nPriceNoPoint > pos->first)
							bOrder = TRUE;
					}
				}
				else if (item.position == PT_SELL)
				{
					if (item.dir == dir_forward)
					{
						if (nPriceNoPoint > pos->first)
							bOrder = TRUE;
					}
					else if (item.dir == dir_backward)
					{
						if (nPriceNoPoint < pos->first)
							bOrder = TRUE;
					}
				}
			}
			if (FALSE == bOrder)
				continue;

			Order(item.position, item.strPrice, item.nVolume, om_mit);
			
			delKeys.push_back(pos->first);
		}

		for (auto posDel = delKeys.begin(); posDel != delKeys.end(); ++posDel)
		{
			auto pos = m_mits.find(*posDel);
			MITItem item = pos->second;
			m_mits.erase(*posDel);
			NotifyMITUpdate(item.position, item.strPrice, 0);
		}
	}
}

//호출전에 m_mit가 업데이트 되어야 한다.
void AutoOrder::NotifyMITUpdate(PositionType posType, const CString& strPrice, GT_VOLUME volume)
{
	BEGIN_OBSERVER_ITERATION(AutoOrderObserver)
	{
		pObserver->OnAutoOrderMITChanged(posType, strPrice, volume);
	}
	END_OBSERVER_ITERATION()

	m_pOwner->OnMITUpdated(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
AutoOrderManager::AutoOrderManager()
{
	ADD_HANDLER(WM_ORDER_CONTRACTS, AutoOrderManager::OnOrderContracts);
	SER()->RegisterOrderContracted(this);
}

AutoOrderManager::~AutoOrderManager()
{
	SER()->UnregisterOrderContracted(this);
}


BOOL AutoOrderManager::OnOrderContracts(WPARAM wParam, LPARAM lParam)
{
	OrderContractNotifyItem* pNotf = (OrderContractNotifyItem*)wParam;

	for(auto pos = m_orders.begin() ; pos != m_orders.end() ; ++pos)
	{
		if(pos->first.first != pNotf->pCode || pos->first.second != pNotf->marketType)
			continue;
		
		AutoOrder* pAutoOrder = pos->second.second;
		pAutoOrder->OnOrderContracts(pNotf->acType, pNotf->pContract);
		break;
	}
	
	return TRUE;
}

AutoOrder* AutoOrderManager::GetAutoOrder(GTMerchandise::IDType codeID, BOOL bCreateNotExist)
{
	auto pos = m_orders.find(codeID);
	if(m_orders.end() == pos)
	{
		if(FALSE == bCreateNotExist)
			return NULL;
		GTMerchandisePtr pMerch = g_pMP->GetObject(codeID);
		AutoOrder* pAutoOrder = new AutoOrder(pMerch->GetCurrent(), this);
		m_orders.insert(make_pair(codeID, make_pair(pMerch, pAutoOrder)));
		return pAutoOrder;
	}

	return pos->second.second;
}

BOOL AutoOrderManager::RemoveAutoOrder(GTMerchandise::IDType codeID)
{
	auto pos = m_orders.find(codeID);
	if(m_orders.end() == pos)
		return FALSE;

	delete pos->second.second;
	m_orders.erase(pos);
	return TRUE;
}


AutoOrder* AutoOrderManager::AttachAutoOrderObserver(GTMerchandise::IDType codeID, AutoOrderObserver* pObserver)
{
	AutoOrder* pOrder = GetAutoOrder(codeID, TRUE);
	pOrder->AttachObserver(pObserver);
	return pOrder;
}
void AutoOrderManager::DetachAutoOrderObserver(GTMerchandise::IDType codeID, AutoOrderObserver* pObserver)
{
	auto pos = m_orders.find(codeID);
	if(m_orders.end() == pos)
		return;

	AutoOrder* pOrder = pos->second.second;
	if(NULL == pOrder)
		return;

	pOrder->DetachObserver(pObserver);
	if (pOrder->IsEmpty() && pOrder->m_observers.empty())
	{
		delete pOrder;
		m_orders.erase(pos);
	}
}

void AutoOrderManager::OnMITUpdated(AutoOrder* pAutoOrder)
{
	BEGIN_OBSERVER_ITERATION(AutoOrderManagerObserver)
	{
		pObserver->OnAutoOrderManagerOnMITUpdated(m_orders);
	}
	END_OBSERVER_ITERATION()
}


BOOL AutoOrderManager::IsAnyMITRemain()
{
	for (auto pos = m_orders.begin(); pos != m_orders.end(); ++pos)
	{
		AutoOrder* pAutoOrder = pos->second.second;
		if (!pAutoOrder->m_mits.empty())
			return TRUE;
	}
	return FALSE;
}