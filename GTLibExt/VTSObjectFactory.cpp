#include "StdAfx.h"
#include "VTSObjectFactory.h"
#include "VTSCodeManager.h"
#include "VTSFuturesCurrent.h"
#include "VTSOptionsCurrent.h"
#include "VTSFuturesHoga.h"
#include "VTSOptionsHoga.h"
#include "VTSNightFuturesCurrent.h"
#include "VTSNightFuturesHoga.h"
#include "VTSForeignFuturesCurrent.h"
#include "VTSForeignFuturesHoga.h"
#include "VTSNightOptionsCurrent.h"
#include "VTSNightOptionsHoga.h"


GTObjectFactory* g_pFactory = new VTSObjectFactory;

VTSObjectFactory::VTSObjectFactory(void) : m_pCodeManager(NULL)
{

}


VTSObjectFactory::~VTSObjectFactory(void)
{

}


GTCurrentPtr VTSObjectFactory::GetCurrent(MerchandiseType mType, MarketType marketType)
{
	switch(mType)
	{
	case ct_futures:
		if (mt_default == marketType)
			return GTCurrentPtr(new VTSFuturesCurrent());
		else if (mt_CME == marketType)
			return GTCurrentPtr(new VTSNightFuturesCurrent());
		else
		{
			ASSERT(FALSE);
		}
	case ct_options:
		if (mt_default == marketType)
			return GTCurrentPtr(new VTSOptionsCurrent());
		else if (mt_EUREX == marketType)
			return GTCurrentPtr(new VTSNightOptionsCurrent());
		else
		{
			ASSERT(FALSE);
		}
	case ct_foreign_futures:
		return GTCurrentPtr(new VTSForeignFuturesCurrent());
	default:
		ASSERT(FALSE);
		return NULL;
	}
	return NULL;
}

GTHogaPtr VTSObjectFactory::GetHoga(MerchandiseType mType, MarketType marketType)
{
	switch(mType)
	{
	case ct_futures:
		if(mt_default == marketType)
			return GTHogaPtr(new VTSFuturesHoga());
		else if(mt_CME == marketType)
			return GTHogaPtr(new VTSNightFuturesHoga());
		else
		{
			ASSERT(FALSE);
		}
	case ct_options:
		if(mt_default == marketType)
			return GTHogaPtr(new VTSOptionsHoga());
		else if(mt_EUREX == marketType)
			return GTHogaPtr(new VTSNightOptionsHoga());
		else
		{
			ASSERT(FALSE);
		}
	case ct_foreign_futures:
		return GTHogaPtr(new VTSForeignFuturesHoga());
	default:
		ASSERT(FALSE);
		return NULL;
	}
	return NULL;
}

GTTrader* VTSObjectFactory::GetTrader(TradeCompany tc)
{
	GTTrader* pTrader= NULL;

	if(tc_xing == tc)
	{
//		pTrader = new XingTrader;
	}
	return NULL;
}

GTCodeManager* VTSObjectFactory::GetCodeManager()
{
	if(NULL == m_pCodeManager)
		m_pCodeManager = new VTSCodeManager;
	return m_pCodeManager;
}