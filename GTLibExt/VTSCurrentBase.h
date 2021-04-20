#pragma once

#include <DKMsgOnlyWnd.h>
#include <list>

class VTSCurrentBase : public DKMsgOnlyWnd
{
public:
	VTSCurrentBase();
	virtual ~VTSCurrentBase();

	BOOL InitReceive(CHAR* pPacket, UINT nDataLength);
	BOOL Receive(CHAR* pPacket, UINT nDataLength);

	CHAR* GetBuffer(UINT nDataLength);

	BOOL IsUpdated() 
	{
		return m_nID > m_nNotifyID;
	}
protected:

	std::list<std::pair<CHAR*, UINT> >		m_buffers;

	virtual BOOL OnInitReceive(CHAR* pPacket, UINT nDataLength) = 0;
	virtual BOOL OnReceive(CHAR* pPacket, UINT nDataLength) = 0;

	BOOL OnRecv(WPARAM wParam, LPARAM lParam);
	BOOL OnInitRecv(WPARAM wParam, LPARAM lParam);

	CCriticalSection m_sync;
	CCriticalSection m_syncBuffer;
	CHAR* m_pPacket;
	UINT m_nLen;
	int m_nNotifyID;
	
	volatile int m_nID;
};
