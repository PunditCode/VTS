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
		//�̹� �ٸ�â�� ���ִ� ���̴�
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
	
	GTXD(hname         , string		); //  �ѱ۸�                      
    GTXD(price         , float		); //  ���簡                      
    GTXD(sign          , string		); //  ���ϴ�񱸺�                 
    GTXD(change        , float		); //  ���ϴ��                     
    GTXD(jnilclose     , float		); //  ��������                     
    drate.Set(p->diff, sizeof(p->diff), xdt_float);                    
    GTXD(volume        , long		); //  �ŷ���                      
    GTXD(value         , long		); //  �ŷ����
	
    GTXD(open          , float		); //  �ð�                        
    GTXD(high          , float		); //  ��                        
    GTXD(low           , float		); //  ����                        
    GTXD(uplmtprice    , float		); //  ���Ѱ�                      
    GTXD(dnlmtprice    , float		); //  ���Ѱ�                      
    GTXD(recprice      , float		); //  ���ذ� 
	theorypriceg.Set(p->theoryprice, sizeof(p->theoryprice), xdt_float);
	k200jisu.Set(p->kospijisu, sizeof(p->kospijisu), xdt_float);
    GTXD(kospisign     , string		); //  KOSPI200���ϴ�񱸺�         
    GTXD(kospichange   , float		); //  KOSPI200���ϴ��             
    GTXD(kospidiff     , float		); //  KOSPI200�����              

	NotifyInit();

	m_lastPrice = price.ToString();
	return TRUE;
}

BOOL VTSNightOptionsCurrent::OnReceive(CHAR* pPacket, UINT nDataLength)
{
	LPEC0_OutBlock p = (LPEC0_OutBlock)pPacket;
	
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
	theorypriceg.Set(p->theoryprice, sizeof(p->theoryprice), xdt_float);
	GTXD(openyakcha , long  );    // �̰�����������    
	GTXD(jgubun     , string);    // ������        
	GTXD(jnilvolume , long  );    // ���ϵ��ð���ŷ���
	//GTXD(futcode    , string);    // �����ڵ�          
#endif

	AddContract(::GetStockTimeStr(chetime.ToINT()), price.ToString(), cvolume.ToString(), p->cgubun[0] == _T('-') ? PT_SELL : PT_BUY);
	return TRUE;
}


