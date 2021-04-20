#pragma once

#include "GTObjectFactory.h"


class VTSObjectFactory :  public GTObjectFactory
{
public:
	VTSObjectFactory(void);
	virtual ~VTSObjectFactory(void);

	OVERRIDE GTCurrentPtr GetCurrent(MerchandiseType mType, MarketType marketType);
	OVERRIDE GTHogaPtr GetHoga(MerchandiseType mType, MarketType marketType);
	OVERRIDE GTTrader* GetTrader(TradeCompany tc);
	OVERRIDE GTCodeManager* GetCodeManager();
private:
	GTCodeManager*	m_pCodeManager;
};

