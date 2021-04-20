#pragma once

#include "GTHoga.h"
#include "VTSCurrentBase.h"

class VTSNightFuturesHoga : public GTHoga, public VTSCurrentBase
{
public:
	VTSNightFuturesHoga(void);

	OVERRIDE BOOL RunRecv();
	OVERRIDE void StopRecv();

protected:
	OVERRIDE BOOL OnInitReceive(CHAR* pPacket, UINT nDataLength);
	OVERRIDE BOOL OnReceive(CHAR* pPacket, UINT nDataLength);
};

