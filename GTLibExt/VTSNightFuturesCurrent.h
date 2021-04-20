#pragma once

#include "GTCurrent.h"
#include "VTSCurrentBase.h"

class VTSNightFuturesCurrent : public CFuturesCurrent, public VTSCurrentBase
{
public:
	VTSNightFuturesCurrent(void);
	~VTSNightFuturesCurrent(void);

	OVERRIDE BOOL RunRecv();
	OVERRIDE void StopRecv();

protected:
	OVERRIDE BOOL OnInitReceive(CHAR* pPacket, UINT nDataLength);
	OVERRIDE BOOL OnReceive(CHAR* pPacket, UINT nDataLength);
};
