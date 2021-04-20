#pragma once

#include "GTCurrent.h"
#include "VTSCurrentBase.h"

class VTSForeignFuturesCurrent : public CForeignFuturesCurrent, public VTSCurrentBase
{
public:
	VTSForeignFuturesCurrent(void);
	~VTSForeignFuturesCurrent(void);

	OVERRIDE BOOL RunRecv();
	OVERRIDE void StopRecv();
protected:
	OVERRIDE BOOL OnInitReceive(CHAR* pPacket, UINT nDataLength);
	OVERRIDE BOOL OnReceive(CHAR* pPacket, UINT nDataLength);

	OVERRIDE CandleNode* GetStick(GT_DATE a_date, GT_TIME a_time) const {return NULL;}
	OVERRIDE CandleNode* GetUpdatedStick(CandleNode* pOldStick) const {return NULL;}
};
