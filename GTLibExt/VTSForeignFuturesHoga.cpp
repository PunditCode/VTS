#include "stdafx.h"
#include "VTSForeignFuturesHoga.h"
#include "OVH.h"
#include "o3106.h"
#include "../VTS.h"

///////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


VTSForeignFuturesHoga::VTSForeignFuturesHoga(void) 
{
	
}

BOOL VTSForeignFuturesHoga::RunRecv()
{
	int nRet = SER()->RegisterHoga(m_pCode, mt_default, this);

	if(nRet == 1)
		return TRUE;
	else if(nRet == 0)
	{
		//이미 다른창이 떠있는 것이다
		Notify();
		return TRUE;
	}
	return FALSE;
}

void VTSForeignFuturesHoga::StopRecv()
{
	SER()->UnregisterHoga(m_pCode, mt_default, this);
}


#define BID_SELL(idx) \
sellHoga[idx - 1].price.Set(p->offerho##idx, sizeof(p->offerho##idx), xdt_float_dot, m_pCode->nPricePointPos);\
sellHoga[idx - 1].remain.Set(p->offerrem##idx, sizeof(p->offerrem##idx), xdt_long);\
sellHoga[idx - 1].number.Set(p->offercnt##idx, sizeof(p->offercnt##idx), xdt_long);

#define BID_BUY(idx) \
buyHoga[idx - 1].price.Set(p->bidho##idx, sizeof(p->bidho##idx), xdt_float_dot, m_pCode->nPricePointPos);\
buyHoga[idx - 1].remain.Set(p->bidrem##idx, sizeof(p->bidrem##idx), xdt_long);\
buyHoga[idx - 1].number.Set(p->bidcnt##idx, sizeof(p->bidcnt##idx), xdt_long);

BOOL VTSForeignFuturesHoga::OnInitReceive(CHAR* pPacket, UINT nDataLength)
{
	if(nDataLength != sizeof(o3106OutBlock))
		return FALSE;

	LPo3106OutBlock p = (LPo3106OutBlock)pPacket;
		
	BID_SELL(1);
	BID_SELL(2);
	BID_SELL(3);
	BID_SELL(4);
	BID_SELL(5);

	BID_BUY(1);
	BID_BUY(2);
	BID_BUY(3);
	BID_BUY(4);
	BID_BUY(5);
	
	totofferrem.Set(p->offer, sizeof(p->offer), xdt_long);
	totbidrem.Set(p->bid, sizeof(p->bid), xdt_long); 
	totoffercnt.Set(p->offercnt, sizeof(p->offercnt), xdt_long);
	totbidcnt.Set(p->bidcnt, sizeof(p->bidcnt), xdt_long); 

	NotifyInit();
	return TRUE;
}

#undef BID_SELL
#undef BID_BUY

#define BID_SELL(idx) \
sellHoga[idx - 1].price.Set(p->offerho##idx, sizeof(p->offerho##idx), xdt_float_dot, m_pCode->nPricePointPos);  \
sellHoga[idx - 1].remain.Set(p->offerrem##idx, sizeof(p->offerrem##idx), xdt_long);								\
sellHoga[idx - 1].number.Set(p->offerno##idx, sizeof(p->offerno##idx), xdt_long);								

#define BID_BUY(idx) \
buyHoga[idx - 1].price.Set(p->bidho##idx, sizeof(p->bidho##idx), xdt_float_dot, m_pCode->nPricePointPos);		\
buyHoga[idx - 1].remain.Set(p->bidrem##idx, sizeof(p->bidrem##idx), xdt_long);									\
buyHoga[idx - 1].number.Set(p->bidno##idx, sizeof(p->bidno##idx), xdt_long);									

BOOL VTSForeignFuturesHoga::OnReceive(CHAR* pPacket, UINT nDataLength)
{
	LPOVH_OutBlock p = (LPOVH_OutBlock)pPacket;
	
	BID_SELL(1);
	BID_SELL(2);
	BID_SELL(3);
	BID_SELL(4);
	BID_SELL(5);

	BID_BUY(1);
	BID_BUY(2);
	BID_BUY(3);
	BID_BUY(4);
	BID_BUY(5);

	GTXD(hotime     , string);  // 호가시간     
	GTXD(totofferrem, long  );  // 매도호가총수량
	GTXD(totbidrem  , long  );  // 매수호가총수량
	GTXD(totoffercnt, long  );  // 매도호가총건수
	GTXD(totbidcnt  , long  );  // 매수호가총건수
	//GTXD(futcode    , string);  // 단축코드     

	Notify();
	
	return TRUE;
}