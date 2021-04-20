#include "stdafx.h"
#include "VTSCurrentBase.h"

#define WM_RECV				WM_USER + 1
#define WM_INIT_RECV		WM_USER + 2


VTSCurrentBase::VTSCurrentBase() : m_nID(0), m_nNotifyID(0)
{
	ADD_HANDLER(WM_RECV, VTSCurrentBase::OnRecv);
	ADD_HANDLER(WM_INIT_RECV, VTSCurrentBase::OnInitRecv);
}

VTSCurrentBase::~VTSCurrentBase()
{
	for(auto pos = m_buffers.begin() ; pos != m_buffers.end() ; ++pos)
		::HeapFree(GetProcessHeap(), 0, pos->first);
}

BOOL VTSCurrentBase::InitReceive(CHAR* pPacket, UINT nDataLength)
{
	CHAR* p = GetBuffer(nDataLength);
	memcpy(p, pPacket, nDataLength);
	memcpy(p + nDataLength, (void*)&m_nID, sizeof(int));

	PostMessage(WM_INIT_RECV, (WPARAM)p, (LPARAM)nDataLength);
	return TRUE;
}

BOOL VTSCurrentBase::Receive(CHAR* pPacket, UINT nDataLength)
{
	++m_nID;

	CHAR* p = GetBuffer(nDataLength);
	memcpy(p, pPacket, nDataLength);
	memcpy(p+nDataLength, (void*)&m_nID, sizeof(int));
	
	PostMessage(WM_RECV, (WPARAM)p, (LPARAM)nDataLength);
	return TRUE;
}

BOOL VTSCurrentBase::OnRecv(WPARAM wParam, LPARAM lParam)
{
	CHAR* p = (CHAR*)wParam;
	UINT nDataLength = (UINT)lParam;
	m_nNotifyID = *((int*)(p + nDataLength));
	OnReceive(p, nDataLength);

#if 0
	LARGE_INTEGER	tick;
	QueryPerformanceCounter(&tick);
	TRACE(_T("OnRecv(%d) %I64d\n"), m_nNotifyID, tick.QuadPart);
#endif

	SCOPE_LOCK(m_syncBuffer);
	m_buffers.push_back(std::make_pair((CHAR*)wParam, (UINT)lParam));
//	TRACE(_T("Buf size(%d)\n"), m_buffers.size());
	return TRUE;
}

BOOL VTSCurrentBase::OnInitRecv(WPARAM wParam, LPARAM lParam)
{
	return OnInitReceive((CHAR*)wParam, (UINT)lParam);
}

CHAR* VTSCurrentBase::GetBuffer(UINT nDataLength)
{
	SCOPE_LOCK(m_syncBuffer);
	for(auto pos = m_buffers.begin() ; pos != m_buffers.end() ; ++pos)
	{
		if(pos->second >= nDataLength)
		{
			CHAR* p = pos->first;
			m_buffers.erase(pos);
			return p;
		}
	}

	CHAR* p = (CHAR*)::HeapAlloc(::GetProcessHeap(), 0, nDataLength + sizeof(int));
	return p;
}