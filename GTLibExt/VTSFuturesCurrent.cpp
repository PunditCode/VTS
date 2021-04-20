#include "stdafx.h"
#include "VTSFuturesCurrent.h"
#include "FC0.h"
#include "t2101.h"
#include "../VTS.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VTSFuturesCurrent::VTSFuturesCurrent(void) 
{
}

VTSFuturesCurrent::~VTSFuturesCurrent(void)
{
	SER()->UnregisterCurrent(m_pCode, mt_default, this);
}

BOOL VTSFuturesCurrent::RunRecv()
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

void VTSFuturesCurrent::StopRecv()
{
	SER()->UnregisterCurrent(m_pCode, mt_default, this);
}

BOOL VTSFuturesCurrent::OnInitReceive(CHAR* pPacket, UINT nDataLength)
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
    GTXD(theoryprice   , float		); //  �̷а�                      
    GTXD_FD(glyl       , 3); //  ������
    GTXD(cbhprice      , float		); //  CB���Ѱ�                    
    GTXD(cblprice      , float		); //  CB���Ѱ�                    
    GTXD(lastmonth     , string		); //  ������                      
    GTXD(jandatecnt    , long		); //  �ܿ���                      
    GTXD(pricejisu     , float		); //  ��������                     
    GTXD(jisusign      , string		); //  �����������ϴ�񱸺�          
    GTXD(jisuchange    , float		); //  �����������ϴ��              
    GTXD(jisudiff      , float		); //  �������������
	k200jisu.Set(p->kospijisu, sizeof(p->kospijisu), xdt_float);
    //GTXD(kospijisu     , float		); //  KOSPI200����                
    GTXD(kospisign     , string		); //  KOSPI200���ϴ�񱸺�         
    GTXD(kospichange   , float		); //  KOSPI200���ϴ��             
    GTXD(kospidiff     , float		); //  KOSPI200�����              
    GTXD(listhprice    , float		); //  �����ְ�                   
    GTXD(listlprice    , float		); //  ����������                   
    GTXD(sbasis        , float		); //  ����BASIS                   
    GTXD(ibasis        , float		); //  �̷�BASIS                   
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

BOOL VTSFuturesCurrent::OnReceive(CHAR* pPacket, UINT nDataLength)
{
	LPFC0_OutBlock p = (LPFC0_OutBlock)pPacket;
	
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
	GTXD(theoryprice, float );    // �̷а�            
	GTXD(kasis      , float );    // ������            
	GTXD(sbasis     , float );    // ����BASIS         
	GTXD(ibasis     , float );    // �̷�BASIS         
	GTXD(openyakcha , long  );    // �̰�����������    
	GTXD(jgubun     , string);    // ������        
	GTXD(jnilvolume , long  );    // ���ϵ��ð���ŷ���
#endif
	//GTXD(futcode    , string);    // �����ڵ�          

	AddContract(::GetStockTimeStr(chetime.ToINT()), price.ToString(), cvolume.ToString(), p->cgubun[0] == _T('-') ? PT_SELL : PT_BUY);
	return TRUE;
}


