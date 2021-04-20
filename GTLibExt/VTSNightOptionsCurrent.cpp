#include "stdafx.h"
#include "VTSNightOptionsCurrent.h"
#include "EC0.h"
#include "t2830.h"
#include "../VTS.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


VTSNightOptionsCurrent::VTSNightOptionsCurrent(void)
{
}

VTSNightOptionsCurrent::~VTSNightOptionsCurrent(void)
{
	SER()->UnregisterCurrent(m_pCode, mt_EUREX, this);
}

BOOL VTSNightOptionsCurrent::RunRecv()
{
	int nRet = SER()->RegisterCurrent(m_pCode, mt_EUREX, this);

	if(nRet == 1)
		return TRUE;
	else if(nRet == 0)
	{
		//이미 다른창이 떠있는 것이다
		NotifyInit();
		return TRUE;
	}
	return FALSE;
}

void VTSNightOptionsCurrent::StopRecv()
{
	SER()->UnregisterCurrent(m_pCode, mt_EUREX, this);
}

BOOL VTSNightOptionsCurrent::OnInitReceive(CHAR* pPacket, UINT nDataLength)
{
	if(nDataLength != sizeof(t2830OutBlock))
		return FALSE;

	LPt2830OutBlock p = (LPt2830OutBlock)pPacket;
	
	GTXD(hname         , string		); //  한글명                      
    GTXD(price         , float		); //  현재가                      
    GTXD(sign          , string		); //  전일대비구분                 
    GTXD(change        , float		); //  전일대비                     
    GTXD(jnilclose     , float		); //  전일종가                     
    drate.Set(p->diff, sizeof(p->diff), xdt_float);                    
    GTXD(volume        , long		); //  거래량                      
    GTXD(value         , long		); //  거래대금
	
    GTXD(open          , float		); //  시가                        
    GTXD(high          , float		); //  고가                        
    GTXD(low           , float		); //  저가                        
    GTXD(uplmtprice    , float		); //  상한가                      
    GTXD(dnlmtprice    , float		); //  하한가                      
    GTXD(recprice      , float		); //  기준가 
	theorypriceg.Set(p->theoryprice, sizeof(p->theoryprice), xdt_float);
	k200jisu.Set(p->kospijisu, sizeof(p->kospijisu), xdt_float);
    GTXD(kospisign     , string		); //  KOSPI200전일대비구분         
    GTXD(kospichange   , float		); //  KOSPI200전일대비             
    GTXD(kospidiff     , float		); //  KOSPI200등락율              

	NotifyInit();

	m_lastPrice = price.ToString();
	return TRUE;
}

BOOL VTSNightOptionsCurrent::OnReceive(CHAR* pPacket, UINT nDataLength)
{
	LPEC0_OutBlock p = (LPEC0_OutBlock)pPacket;
	
	GTXD(chetime    , time);    // 체결시간          
	GTXD(sign       , string);    // 전일대비구분      
	GTXD(change     , float );    // 전일대비          
	GTXD(drate      , float );    // 등락율            
	GTXD(price      , float );    // 현재가            
	GTXD(open       , float );    // 시가              
	GTXD(high       , float );    // 고가              
	GTXD(low        , float );    // 저가              
	GTXD(cgubun     , string);    // 체결구분          
	GTXD(cvolume    , long  );    // 체결량   
#if 0
	GTXD(volume     , long  );    // 누적거래량        
	GTXD(value      , long  );    // 누적거래대금      
	GTXD(mdvolume   , long  );    // 매도누적체결량    
	GTXD(mdchecnt   , long  );    // 매도누적체결건수  
	GTXD(msvolume   , long  );    // 매수누적체결량    
	GTXD(mschecnt   , long  );    // 매수누적체결건수  
	GTXD(cpower     , float );    // 체결강도          
	GTXD(offerho1   , float );    // 매도호가1         
	GTXD(bidho1     , float );    // 매수호가1         
	GTXD(openyak    , long  );    // 미결제약정수량    
	GTXD(k200jisu   , float );    // KOSPI200지수
	theorypriceg.Set(p->theoryprice, sizeof(p->theoryprice), xdt_float);
	GTXD(openyakcha , long  );    // 미결제약정증감    
	GTXD(jgubun     , string);    // 장운영정보        
	GTXD(jnilvolume , long  );    // 전일동시간대거래량
	//GTXD(futcode    , string);    // 단축코드          
#endif

	AddContract(::GetStockTimeStr(chetime.ToINT()), price.ToString(), cvolume.ToString(), p->cgubun[0] == _T('-') ? PT_SELL : PT_BUY);
	return TRUE;
}


