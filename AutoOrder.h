#pragma once

#include <map>
#include <GTMerchandisePool.h>
#include "DKMsgOnlyWnd.h"
#include <GTTypes.h>


class AutoOrderObserver : public DKObserver
{
public:
	virtual void OnAutoOrderSetLossCut(BOOL bEnable, int nTick, int nRowOld, int nRowNew){}
	virtual void OnAutoOrderSetEarnCut(BOOL bEnable, int nTick, int nRowOld, int nRowNew){}
	virtual void OnAutoOrderMITChanged(PositionType posType, const CString& strPrice, GT_VOLUME volume){}
	virtual void OnAutoOrderContractChanged(int nRowOld, int nRowNew, PositionType position){}
};


class AutoOrder : public GTCurrentObserver, protected DKNotifier
{
	friend class AutoOrderManager;

public:
	AutoOrder(GTCurrentPtr pCurrent, AutoOrderManager* pOwner);
	~AutoOrder(void);

	void InitHoga(GTCurrentPtr pCurrent);
	BOOL IsInit() { return m_bInit; }

	BOOL SetLosscut(BOOL bEnable, int nTick, AutoOrderObserver* pCaller, CString& strMsg);
	BOOL SetEarncut(BOOL bEnable, int nTick, AutoOrderObserver* pCaller, CString& strMsg);

	BOOL MIT_New(PositionType position, const CString& strPrice, GT_VOLUME nVolume, OUT CString& strMsg);
	BOOL MIT_Correction(PositionType position, const CString& strPriceOrg, const CString& strPriceNew, OUT CString& strMsg);
	BOOL MIT_Cancel(PositionType position, const CString& strPrice);
	void MIT_AllCancel();
	void MIT_SellCancel();
	void MIT_BuyCancel();

	enum Direction { dir_none = -1, dir_forward, dir_backward};
	struct MITItem
	{
		CString		strPrice;
		PositionType position;
		GT_VOLUME	nVolume;
		CTime		tm;
		Direction	dir;
		MITItem() : nVolume(0), dir(dir_none){}
	};
	typedef std::map<INT, MITItem>	MITs;

	const MITs& GetMITs() { return m_mits; }

	int GetRowIndex(int nPrice)
	{
		if (NULL == m_pFncGetRowIndex)
			return NUL_IDX;
		return (this->*m_pFncGetRowIndex)(nPrice);
	}

	GTMerchandise::IDType GetCodeID() { return m_codeID; }

	BOOL IsEmpty()
	{
		return m_nContractRow == NUL_IDX && GetMITs().empty();
	}
protected:
	BOOL Order(PositionType posType, const CString& price, GT_VOLUME nVolume, OrderMethod method);

	OVERRIDE void OnCurrentInitUpdated(GTCurrentPtr pCurrent){}
	OVERRIDE void OnCurrentUpdated(GTCurrentPtr pCurrent);
	
	typedef int (AutoOrder::*pFNC_GetRowIndex)(int /*nPrice*/);
	pFNC_GetRowIndex		m_pFncGetRowIndex;
	pFNC_GetRowIndex		m_pFncGetRowIndexForContract;

	int _GetRowIndex(int nPrice);
	int _GetRowIndexOptions(int nPrice);
	int _GetRowIndexForContract(int nPrice);
	int _GetRowIndexOptionsForContract(int nPrice);

	void OnOrderContracts(AddContractType acType, OrderContracted* pOrderCont);

	void NotifyLiquid();
	void NotifyMITUpdate(PositionType posType, const CString& strPrice, GT_VOLUME volume);
private:
	BOOL		m_bInit;
	GTMerchandise::IDType	m_codeID;
	GTCurrentPtr				m_pCurrent; //객체가 살아있는동안에는 항상 있다고 보장한다.
	AutoOrderManager*		m_pOwner;

	BOOL		m_bLosscut;
	BOOL		m_bEarncut;

	int			m_nLosscutTick;
	int			m_nEarncutTick;
	MITs		m_mits;
	friend class HogaGrid;

	int		m_nMax, m_nMin, m_nHogaHeight, m_nHogaUnit;
	int		m_nHogaRowCount;
	int		m_nLosscutRow;
	int		m_nEarncutRow;
	BOOL	m_bOptionsOverlppedPrice3_0;

	int		m_nContractRow;
	PositionType m_contractPos;
	GT_VOLUME m_nVolumeContract;
};

////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////
typedef std::map<GTMerchandise::IDType, pair<GTMerchandisePtr, AutoOrder*> >	AutoOrders;

class AutoOrderManagerObserver : public DKObserver
{
public:
	virtual void OnAutoOrderManagerOnMITUpdated(const AutoOrders& autoOrders){}
};


class AutoOrderManager : public DKMsgOnlyWnd, public DKNotifier
{
public:
	friend class AutoOrder;

	AutoOrderManager();
	~AutoOrderManager();

	AutoOrder* GetAutoOrder(GTMerchandise::IDType codeID, BOOL bCreateNotExist);
	BOOL RemoveAutoOrder(GTMerchandise::IDType codeID);

	AutoOrder* AttachAutoOrderObserver(GTMerchandise::IDType codeID, AutoOrderObserver* pObserver);
	void DetachAutoOrderObserver(GTMerchandise::IDType codeID, AutoOrderObserver* pObserver);

	const AutoOrders& GetAutoOrders() { return m_orders; }

	BOOL IsAnyMITRemain();
protected:
	BOOL OnOrderContracts(WPARAM wParam, LPARAM lParam);
	void OnMITUpdated(AutoOrder* pAutoOrder);
private:
	AutoOrders m_orders;
};
