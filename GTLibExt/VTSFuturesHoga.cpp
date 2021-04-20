#include "stdafx.h"
#include "VTSFuturesHoga.h"
#include "FH0.h"
#include "t2105.h"
#include "../VTS.h"

///////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


VTSFuturesHoga::VTSFuturesHoga(void) 
{
}

BOOL VTSFuturesHoga::RunRecv()
{
	int nRet = SER()->RegisterHoga(m_pCode, mt_default, this);

	if(nRet == 1)
		return TRUE;
	else if(nRet == 0)
	{
		//�̹� �ٸ�â�� ���ִ� ���̴�
		Notify();
		return TRUE;
	}
	return FALSE;
}

void VTSFuturesHoga::StopRecv()
{
	SER()->UnregisterHoga(m_pCode, mt_default, this);
}


#define BID_SELL(idx) \
sellHoga[idx - 1].price.Set(p->offerho##idx, sizeof(p->offerho##idx), xdt_float_dot); \
sellHoga[idx - 1].remain.Set(p->offerrem##idx, sizeof(p->offerrem##idx), xdt_long); \
sellHoga[idx - 1].number.Set(p->dcnt##idx, sizeof(p->dcnt##idx), xdt_long); 

#define BID_BUY(idx) \
buyHoga[idx - 1].price.Set(p->bidho##idx, sizeof(p->bidho##idx), xdt_float_dot); \
buyHoga[idx - 1].remain.Set(p->bidrem##idx, sizeof(p->bidrem##idx), xdt_long); \
buyHoga[idx - 1].number.Set(p->scnt##idx, sizeof(p->scnt##idx), xdt_long); 

BOOL VTSFuturesHoga::OnInitReceive(CHAR* pPacket, UINT nDataLength)
{
	if(nDataLength != sizeof(t2105OutBlock))
		return FALSE;

	LPt2105OutBlock p = (LPt2105OutBlock)pPacket;

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

	totofferrem.Set(p->dvol, sizeof(p->dvol), xdt_long);
	totbidrem.Set(p->svol, sizeof(p->svol), xdt_long); 
	totoffercnt.Set(p->toffernum, sizeof(p->toffernum), xdt_long);
	totbidcnt.Set(p->tbidnum, sizeof(p->tbidnum), xdt_long); 

	NotifyInit();
	return TRUE;
}

#undef BID_SELL
#undef BID_BUY

#define BID_SELL(idx) \
sellHoga[idx - 1].price.Set(p->offerho##idx, sizeof(p->offerho##idx), xdt_float);		\
sellHoga[idx - 1].remain.Set(p->offerrem##idx, sizeof(p->offerrem##idx), xdt_long);		\
sellHoga[idx - 1].number.Set(p->offercnt##idx, sizeof(p->offercnt##idx), xdt_long);		

#define BID_BUY(idx) \
buyHoga[idx - 1].price.Set(p->bidho##idx, sizeof(p->bidho##idx), xdt_float);			\
buyHoga[idx - 1].remain.Set(p->bidrem##idx, sizeof(p->bidrem##idx), xdt_long);			\
buyHoga[idx - 1].number.Set(p->bidcnt##idx, sizeof(p->bidcnt##idx), xdt_long);			

BOOL VTSFuturesHoga::OnReceive(CHAR* pPacket, UINT nDataLength)
{
	LPFH0_OutBlock p = (LPFH0_OutBlock)pPacket;

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

	GTXD(hotime     , string);  // ȣ���ð�     
	GTXD(totofferrem, long  );  // �ŵ�ȣ���Ѽ���
	GTXD(totbidrem  , long  );  // �ż�ȣ���Ѽ���
	GTXD(totoffercnt, long  );  // �ŵ�ȣ���ѰǼ�
	GTXD(totbidcnt  , long  );  // �ż�ȣ���ѰǼ�
	//GTXD(futcode    , string);  // �����ڵ�     
	GTXD(danhochk   , string);  // ���ϰ�ȣ������
	GTXD(alloc_gubun, string);  // ������뱸��  

	Notify();
	return TRUE;
}