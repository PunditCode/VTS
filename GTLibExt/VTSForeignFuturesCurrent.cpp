#include "stdafx.h"
#include "VTSForeignFuturesCurrent.h"
#include "o3105.h"
#include "OVC.h"
#include "../VTS.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


VTSForeignFuturesCurrent::VTSForeignFuturesCurrent(void)
{
}

VTSForeignFuturesCurrent::~VTSForeignFuturesCurrent(void)
{
	SER()->UnregisterCurrent(m_pCode, mt_default, this);
}

BOOL VTSForeignFuturesCurrent::RunRecv()
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

void VTSForeignFuturesCurrent::StopRecv()
{
	SER()->UnregisterCurrent(m_pCode, mt_default, this);
}

#undef GTXD
#undef GTXD_FD

#define GTXD(__field__, __member__, __type__) __member__.Set(p->##__field__, sizeof(p->##__field__), xdt_##__type__)
#define GTXD_FD(__field__, __member__, __dotpos__) __member__.Set(p->##__field__, sizeof(p->##__field__), xdt_float_dot, __dotpos__)

BOOL VTSForeignFuturesCurrent::OnInitReceive(CHAR* pPacket, UINT nDataLength)
{
	if(nDataLength != sizeof(o3105OutBlock))
		return FALSE;

	/*uplmtprice.DoubleToInt();
	dnlmtprice.DoubleToInt();*/

	LPo3105OutBlock p = (LPo3105OutBlock)pPacket;
	
	//hname      
	GTXD_FD(TrdP,		price,		m_pCode->nPricePointPos);
	GTXD(YdiffSign,	sign,		string);
	GTXD_FD(YdiffP,	change,		m_pCode->nPricePointPos);
	GTXD_FD(CloseP,	jnilclose,	m_pCode->nPricePointPos);
	GTXD_FD(Diff,		drate,		2); //  등락율         
	GTXD(TotQ,			volume,		long); //  거래량                      
	GTXD(TotAmt,		value,		long); //  거래대금
	GTXD_FD(OpenP,		open,		m_pCode->nPricePointPos);
	GTXD_FD(HighP,		high,		m_pCode->nPricePointPos);
	GTXD_FD(LowP,		low,		m_pCode->nPricePointPos);
	GTXD(Cgubun,		cgubun,		string);    // 체결구분   
	GTXD(RcvTm,			chetime,	string);    // 체결시간      
	GTXD_FD(EcPrc,	jnilclose,	m_pCode->nPricePointPos);    // 전일종가      

	CodeForeignFutures* pCode = dynamic_cast<CodeForeignFutures*>(m_pCode);
	DK_DECIMAL hogaUnit = pCode->hogaUnitPrice.ToDecimal();

	DK_DECIMAL maxTemp = jnilclose.ToDecimal() * 1.15;
	DK_DECIMAL sub = maxTemp - jnilclose.ToDecimal();
	int nCount = DK_DECIMAL(sub / hogaUnit).convert_to<int>();
	maxTemp = jnilclose.ToDecimal() + (hogaUnit * nCount);

	DK_DECIMAL minTemp = jnilclose.ToDecimal() * 0.85;
	sub =  jnilclose.ToDecimal() - minTemp;
	nCount = DK_DECIMAL(sub / hogaUnit).convert_to<int>();
	minTemp = jnilclose.ToDecimal() - (hogaUnit * nCount);

	CStringA strMax(::ToString(maxTemp, m_pCode->nPricePointPos));
	CStringA strMin(::ToString(minTemp, m_pCode->nPricePointPos));

	uplmtprice.Set((LPSTR)(LPCSTR)strMax, strMax.GetLength(), xdt_float_dot, 4);
	dnlmtprice.Set((LPSTR)(LPCSTR)strMin, strMin.GetLength(), xdt_float_dot, 4);
	NotifyInit();

	m_lastPrice = price.ToString();
	return TRUE;
}

BOOL VTSForeignFuturesCurrent::OnReceive(CHAR* pPacket, UINT nDataLength)
{
	LPOVC_OutBlock p = (LPOVC_OutBlock)pPacket;

	GTXD(trdtm,			chetime,	time);    // 체결시간          
	GTXD(ydiffSign,		sign,		string);    // 전일대비구분      
	GTXD_FD(ydiffpr,	change,		m_pCode->nPricePointPos);
	GTXD_FD(chgrate,	drate,		2);    // 등락율           
	GTXD_FD(curpr,		price,		m_pCode->nPricePointPos);
	GTXD_FD(open,		open,		m_pCode->nPricePointPos);
	GTXD_FD(high,		high,		m_pCode->nPricePointPos);
	GTXD_FD(low,		low,		m_pCode->nPricePointPos);
	GTXD(trdq,			cvolume,	long);    // 체결량            
	GTXD(totq,			volume,		long);    // 누적거래량        
#if 0
	GTXD(mdvolume,		mdvolume,	long);    // 매도누적체결량    
	GTXD(msvolume,		msvolume,	long);    // 매수누적체결량    
#endif
	AddContract(::GetStockTimeStr(chetime.ToINT()), price.ToString(), cvolume.ToString(), p->cgubun[0] == _T('-') ? PT_SELL : PT_BUY);
	
	return TRUE;
}


