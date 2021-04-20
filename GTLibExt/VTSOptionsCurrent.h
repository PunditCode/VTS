#pragma once

#include "GTCurrent.h"
#include "VTSCurrentBase.h"

class VTSOptionsCurrent : public COptionsCurrent, public VTSCurrentBase
{
public:
	VTSOptionsCurrent(void);
	~VTSOptionsCurrent(void);

	OVERRIDE BOOL RunRecv();
	OVERRIDE void StopRecv();

protected:
	OVERRIDE BOOL OnInitReceive(CHAR* pPacket, UINT nDataLength);
	OVERRIDE BOOL OnReceive(CHAR* pPacket, UINT nDataLength);
};
