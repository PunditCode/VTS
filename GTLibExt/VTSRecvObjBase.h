
#pragma once

class VTSRecvObjBase
{
public:
	VTSRecvObjBase(){}
	virtual ~VTSRecvObjBase(){}
	virtual BOOL OnInitReceive(CHAR* pPacket, UINT nDataLength) = 0;
	virtual BOOL OnReceive(CHAR* pPacket, UINT nDataLength) = 0;
	/*virtual void OnInitReceive(BYTE* pPacket, UINT nDataLength) = 0;
	virtual void OnReceive(BYTE* pPacket, UINT nDataLength) = 0;*/
};