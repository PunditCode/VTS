#pragma once

#include "GTCurrent.h"
#include "GTHoga.h"
#include "VTSCurrentBase.h"

class VTSOptionsHoga : public GTHoga, public VTSCurrentBase
{
public:
	VTSOptionsHoga(void);

	OVERRIDE BOOL RunRecv();
	OVERRIDE void StopRecv();

protected:
	OVERRIDE BOOL OnInitReceive(CHAR* pPacket, UINT nDataLength);
	OVERRIDE BOOL OnReceive(CHAR* pPacket, UINT nDataLength);
};

