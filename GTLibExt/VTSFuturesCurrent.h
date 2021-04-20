#pragma once

#include "GTCurrent.h"
#include "VTSCurrentBase.h"

class VTSFuturesCurrent : public CFuturesCurrent, public VTSCurrentBase
{
public:
	VTSFuturesCurrent(void);
	~VTSFuturesCurrent(void);

	OVERRIDE BOOL RunRecv();
	OVERRIDE void StopRecv();

protected:
	OVERRIDE BOOL OnInitReceive(CHAR* pPacket, UINT nDataLength);
	OVERRIDE BOOL OnReceive(CHAR* pPacket, UINT nDataLength);
};
