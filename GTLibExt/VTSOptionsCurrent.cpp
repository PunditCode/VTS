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
		//�̹� �ٸ�â�� ���ִ� ���̴�
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

	GTXD(hname         , string		); //  �ѱ۸�                      
    GTXD(price         , float		); //  ���簡                      
    GTXD(sign          , string		); //  ���ϴ�񱸺�                 
    GTXD(change        , float		); //  ���ϴ��                     
    GTXD(jnilclose     , float		); //  ��������                     
    drate.Set(p->diff, sizeof(p->diff), xdt_float);
    GTXD(volume        , long		); //  �ŷ���                      
    GTXD(value         , long		); //  �ŷ����
	openyak.Set(p->mgjv, sizeof(p->mgjv), xdt_long);
	openyakcha.Set(p->mgjvdiff, sizeof(p->mgjvdiff), xdt_long);
    //GTXD(mgjv          , long		); //  �̰�����                     
    //GTXD(mgjvdiff      , long		); //  �̰�������                   
    GTXD(open          , float		); //  �ð�                        
    GTXD(high          , float		); //  ��                        
    GTXD(low           , float		); //  ����                        
    GTXD(uplmtprice    , float		); //  ���Ѱ�                      
    GTXD(dnlmtprice    , float		); //  ���Ѱ�                      
	GTXD(high52w       , float		); //  52�ְ�                    
    GTXD(low52w        , float		); //  52������                    
    GTXD(basis         , float		); //  ���̽ý�                     
    GTXD(recprice      , float		); //  ���ذ�                      
    GTXD(lastmonth     , string		); //  ������                      
    GTXD(jandatecnt    , long		); //  �ܿ���                      
	k200jisu.Set(p->kospijisu, sizeof(p->kospijisu), xdt_float);
    //GTXD(kospijisu     , float		); //  KOSPI200����                
    GTXD(kospisign     , string		); //  KOSPI200���ϴ�񱸺�         
    GTXD(kospichange   , float		); //  KOSPI200���ϴ��             
    GTXD(kospidiff     , float		); //  KOSPI200�����              
    GTXD(listhprice    , float		); //  �����ְ�                   
    GTXD(listlprice    , float		); //  ����������                   
    GTXD_FD(delt       , 4); //  ��Ÿ
    GTXD_FD(gama       , 4); //  ����
    GTXD_FD(ceta       , 4); //  ��Ÿ
    GTXD_FD(vega       , 4); //  ����
    GTXD_FD(rhox       , 4); //  �ο�
    GTXD(gmprice       , float		); //  �ٿ������簡                 
    GTXD(gmsign        , string		); //  �ٿ������ϴ�񱸺�            
    GTXD(gmchange      , float		); //  �ٿ������ϴ��                
    GTXD(gmdiff        , float		); //  �ٿ��������                 
    GTXD(theorypriceg  , float		); //  �̷а�                      
    GTXD(histimpv      , float		); //  ������������                 
    GTXD(impv          , float		); //  ���纯����                   
    GTXD(gmfutcode     , string		); //  �ٿ��������ڵ�                
    GTXD(actprice      , float		); //  ��簡                      
    GTXD(greeks_time   , string		); //  �ŷ��ҹΰ������Žð�          
    GTXD(greeks_confirm, string		); //  �ŷ��ҹΰ���Ȯ������          
    GTXD(danhochk      , string		); //  ���ϰ�ȣ������                
    GTXD(yeprice       , float		); //  ����ü�ᰡ                   
    GTXD(jnilysign     , string		); //  ����ü�ᰡ����������񱸺�     
    GTXD(jnilychange   , float		); //  ����ü�ᰡ�����������         
    GTXD(jnilydrate    , float		); //  ����ü�ᰡ�������������       
    GTXD(alloc_gubun   , string		); //  ��б���(1:��а���2:�������0:

	NotifyInit();

	m_lastPrice = price.ToString();
	return TRUE;
}

BOOL VTSOptionsCurrent::OnReceive(CHAR* pPacket, UINT nDataLength)
{
	LPOC0_OutBlock p = (LPOC0_OutBlock)pPacket;
	
	GTXD(chetime    , time);    // ü��ð�          
	GTXD(sign       , string);    // ���ϴ�񱸺�      
	GTXD(change     , float );    // ���ϴ��          
	GTXD(drate      , float );    // �����            
	GTXD(price      , float );    // ���簡            
	GTXD(open       , float );    // �ð�              
	GTXD(high       , float );    // ��              
	GTXD(low        , float );    // ����              
	GTXD(cgubun     , string);    // ü�ᱸ��          
	GTXD(cvolume    , long  );    // ü�ᷮ   
#if 0
	GTXD(volume     , long  );    // �����ŷ���        
	GTXD(value      , long  );    // �����ŷ����      
	GTXD(mdvolume   , long  );    // �ŵ�����ü�ᷮ    
	GTXD(mdchecnt   , long  );    // �ŵ�����ü��Ǽ�  
	GTXD(msvolume   , long  );    // �ż�����ü�ᷮ    
	GTXD(mschecnt   , long  );    // �ż�����ü��Ǽ�  
	GTXD(cpower     , float );    // ü�ᰭ��          
	GTXD(offerho1   , float );    // �ŵ�ȣ��1         
	GTXD(bidho1     , float );    // �ż�ȣ��1         
	GTXD(openyak    , long  );    // �̰�����������    
	GTXD(k200jisu   , float );    // KOSPI200����      
//	GTXD(kasis      , float );    // ������            
	GTXD(openyakcha , long  );    // �̰�����������    
	GTXD(jgubun     , string);    // ������        
	GTXD(jnilvolume , long  );    // ���ϵ��ð���ŷ���
	//GTXD(futcode    , string);    // �����ڵ�          
#endif
	AddContract(::GetStockTimeStr(chetime.ToINT()), price.ToString(), cvolume.ToString(), p->cgubun[0] == _T('-') ? PT_SELL : PT_BUY);

	return TRUE;
}


