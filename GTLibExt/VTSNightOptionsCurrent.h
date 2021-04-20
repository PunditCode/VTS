#pragma once

#include "GTCurrent.h"
#include "VTSCurrentBase.h"

class VTSNightOptionsCurrent : public COptionsCurrent, public VTSCurrentBase
{
public:
	VTSNightOptionsCurrent(void);
	~VTSNightOptionsCurrent(void);

	OVERRIDE BOOL RunRecv();
	OVERRIDE void StopRecv();

protected:
	OVERRIDE BOOL OnInitReceive(CHAR* pPacket, UINT nDataLength);
	OVERRIDE BOOL OnReceive(CHAR* pPacket, UINT nDataLength);
};
