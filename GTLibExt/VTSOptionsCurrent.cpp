#include "stdafx.h"
#include "VTSOptionsCurrent.h"
#include "OC0.h"
#include "t2101.h"
#include "../VTS.h"

VTSOptionsCurrent::VTSOptionsCurrent(void) 
{
}

VTSOptionsCurrent::~VTSOptionsCurrent(void)
{
	SER()->UnregisterCurrent(m_pCode, mt_default, this);
}

BOOL VTSOptionsCurrent::RunRecv()
{
	int nRet = SER()->RegisterCurrent(m_pCode, mt_default, this);

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

void VTSOptionsCurrent::StopRecv()
{
	SER()->UnregisterCurrent(m_pCode, mt_default, this);
}

BOOL VTSOptionsCurrent::OnInitReceive(CHAR* pPacket, UINT nDataLength)
{
	if(nDataLength != sizeof(t2101OutBlock))
		return FALSE;

	LPt2101OutBlock p = (LPt2101OutBlock)pPacket;

	GTXD(hname         , string		); //  한글명                      
    GTXD(price         , float		); //  현재가                      
    GTXD(sign          , string		); //  전일대비구분                 
    GTXD(change        , float		); //  전일대비                     
    GTXD(jnilclose     , float		); //  전일종가                     
    drate.Set(p->diff, sizeof(p->diff), xdt_float);
    GTXD(volume        , long		); //  거래량                      
    GTXD(value         , long		); //  거래대금
	openyak.Set(p->mgjv, sizeof(p->mgjv), xdt_long);
	openyakcha.Set(p->mgjvdiff, sizeof(p->mgjvdiff), xdt_long);
    //GTXD(mgjv          , long		); //  미결제량                     
    //GTXD(mgjvdiff      , long		); //  미결제증감                   
    GTXD(open          , float		); //  시가                        
    GTXD(high          , float		); //  고가                        
    GTXD(low           , float		); //  저가                        
    GTXD(uplmtprice    , float		); //  상한가                      
    GTXD(dnlmtprice    , float		); //  하한가                      
	GTXD(high52w       , float		); //  52최고가                    
    GTXD(low52w        , float		); //  52최저가                    
    GTXD(basis         , float		); //  베이시스                     
    GTXD(recprice      , float		); //  기준가                      
    GTXD(lastmonth     , string		); //  만기일                      
    GTXD(jandatecnt    , long		); //  잔여일                      
	k200jisu.Set(p->kospijisu, sizeof(p->kospijisu), xdt_float);
    //GTXD(kospijisu     , float		); //  KOSPI200지수                
    GTXD(kospisign     , string		); //  KOSPI200전일대비구분         
    GTXD(kospichange   , float		); //  KOSPI200전일대비             
    GTXD(kospidiff     , float		); //  KOSPI200등락율              
    GTXD(listhprice    , float		); //  상장최고가                   
    GTXD(listlprice    , float		); //  상장최저가                   
    GTXD_FD(delt       , 4); //  델타
    GTXD_FD(gama       , 4); //  감마
    GTXD_FD(ceta       , 4); //  세타
    GTXD_FD(vega       , 4); //  베가
    GTXD_FD(rhox       , 4); //  로우
    GTXD(gmprice       , float		); //  근월물현재가                 
    GTXD(gmsign        , string		); //  근월물전일대비구분            
    GTXD(gmchange      , float		); //  근월물전일대비                
    GTXD(gmdiff        , float		); //  근월물등락율                 
    GTXD(theorypriceg  , float		); //  이론가                      
    GTXD(histimpv      , float		); //  역사적변동성                 
    GTXD(impv          , float		); //  내재변동성                   
    GTXD(gmfutcode     , string		); //  근월물종목코드                
    GTXD(actprice      , float		); //  행사가                      
    GTXD(greeks_time   , string		); //  거래소민감도수신시간          
    GTXD(greeks_confirm, string		); //  거래소민감도확정여부          
    GTXD(danhochk      , string		); //  단일가호가여부                
    GTXD(yeprice       , float		); //  예상체결가                   
    GTXD(jnilysign     , string		); //  예상체결가전일종가대비구분     
    GTXD(jnilychange   , float		); //  예상체결가전일종가대비         
    GTXD(jnilydrate    , float		); //  예상체결가전일종가등락율       
    GTXD(alloc_gubun   , string		); //  배분구분(1:배분개시2:배분해제0:

	NotifyInit();

	m_lastPrice = price.ToString();
	return TRUE;
}

BOOL VTSOptionsCurrent::OnReceive(CHAR* pPacket, UINT nDataLength)
{
	LPOC0_OutBlock p = (LPOC0_OutBlock)pPacket;
	
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
//	GTXD(kasis      , float );    // 괴리율            
	GTXD(openyakcha , long  );    // 미결제약정증감    
	GTXD(jgubun     , string);    // 장운영정보        
	GTXD(jnilvolume , long  );    // 전일동시간대거래량
	//GTXD(futcode    , string);    // 단축코드          
#endif
	AddContract(::GetStockTimeStr(chetime.ToINT()), price.ToString(), cvolume.ToString(), p->cgubun[0] == _T('-') ? PT_SELL : PT_BUY);

	return TRUE;
}


