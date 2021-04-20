#include "StdAfx.h"
#include "VTSCodeManager.h"

#include "t9943.h" //선물코드
#include "t9944.h" //옵션코드
#include "o3101.h" //해외선물코드

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VTSCodeManager::VTSCodeManager(void) : m_bInit(FALSE)
{
}


VTSCodeManager::~VTSCodeManager(void)
{
}


inline void RDCode(DKBuffer& dbuf, Code* pCode)
{
	dbuf.RD(pCode->name); 
	dbuf.RD(pCode->code); 
	dbuf.RD(pCode->expCode);  
	dbuf.RD(pCode->nYear);
	dbuf.RD(pCode->nMonth);
	dbuf.RD(pCode->nPricePointPos);
	dbuf.RD(pCode->nID);
	dbuf.RD(pCode->bExpireDay);
	__time64_t tm;
	dbuf.RD(tm);
	pCode->tmExpire = tm;
}

inline void RDCode(DKBuffer& dbuf, CodeOptions* pCode)
{
	RDCode(dbuf, (Code*)pCode);
	dbuf.RD(pCode->otype);
	dbuf.RD(pCode->price);
}

inline void RDCode(DKBuffer& dbuf, CodeForeignFutures* pCode)
{
	RDCode(dbuf, (Code*)pCode);
	CString str;
	dbuf.RD(str); pCode->applyDate				=  str;
	dbuf.RD(str); pCode->basicMerchandiseCode	=  str;
	dbuf.RD(str); pCode->basicMerchandiseName	=  str;
	dbuf.RD(str); pCode->exchangeCode			=  str;
	dbuf.RD(str); pCode->exchangeName			=  str;
	dbuf.RD(str); pCode->stdCurrencyCode		=  str;
	dbuf.RD(str); pCode->notaionCode			=  str;
	dbuf.RD(str); pCode->hogaUnitPrice			=  str;
	dbuf.RD(str); pCode->minChangeAmount		=  str;
	dbuf.RD(str); pCode->priceAdjust			=  str;
	dbuf.RD(str); pCode->amountPerContract		=  str;
	dbuf.RD(str); pCode->merchandiseCode		=  str;
	dbuf.RD(str); pCode->listingYear			=  str;
	dbuf.RD(str); pCode->listingMonth			=  str;
	dbuf.RD(str); pCode->makingUpPrice			=  str;
	dbuf.RD(str); pCode->tmStart				=  str;
	dbuf.RD(str); pCode->tmEnd					=  str;
	dbuf.RD(str); pCode->orderEnalbeCode		=  str;
	dbuf.RD(str); pCode->depositCode			=  str;
	dbuf.RD(str); pCode->depositOpen			=  str;
	dbuf.RD(str); pCode->depsoitContinue		=  str;
	dbuf.RD(str); pCode->depositOpenRate		=  str;
	dbuf.RD(str); pCode->depsoitContinueRate	=  str;
	dbuf.RD(str); pCode->pricePointPos			=  str;
	dbuf.RD(str); pCode->korName			=  str;
	dbuf.RD(pCode->nHourGap);
}


BOOL VTSCodeManager::Init(DKBuffer& dbuf)
{
	if(TRUE == m_bInit)
	{
		//서버가 끊기고 재접속하는 경우
		ASSERT(m_futures.empty() == false);
		return TRUE;
	}

	VTSS_NOTIFY_TYPE cmd;
	dbuf.RD(cmd);
	if(cmd != VTSS_NOTF_CODE)
	{
		return FALSE;
	}

	int nCount;
	////////////////////////////////////////////////////////////////
	//선물
	dbuf.RD(nCount);
	for(int i = 0 ; i < nCount ; ++i)
	{
		Code* c = new Code(ct_futures);
		RDCode(dbuf, c);

		m_futures.insert(make_pair(c->code, c));
		m_dic.insert(make_pair(c->code, c));
		m_dic.insert(make_pair(c->name, c));
		m_codesByID.insert(make_pair(c->nID, c));
	}
	////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////
	//옵션
	int nMonthCount;
	dbuf.RD(nMonthCount);
	for(int nMon = 0 ; nMon < nMonthCount ; ++nMon)
	{
		CodesOptionsPerMonth* pCallPerMonth = new CodesOptionsPerMonth;
		dbuf.RD(pCallPerMonth->nYear);
		dbuf.RD(pCallPerMonth->nMonth);
		m_codesOptsPerMonCalls.push_back(pCallPerMonth);

		CodesOptionsPerMonth* pPutPerMonth = new CodesOptionsPerMonth;
		pPutPerMonth->nYear = pCallPerMonth->nYear;
		pPutPerMonth->nMonth = pCallPerMonth->nMonth;
		m_codesOptsPerMonPuts.push_back(pPutPerMonth);

		dbuf.RD(nCount);
		for(int i = 0 ; i < nCount ; ++i)
		{
			CodeOptions* c = new CodeOptions(ct_options);
			RDCode(dbuf, c);

			m_options.insert(make_pair(c->code, c));
			m_dic.insert(make_pair(c->code, c));
			m_dic.insert(make_pair(c->name, c));
			m_codesByID.insert(make_pair(c->nID, c));
			pCallPerMonth->codes.push_back(c);
		}

		dbuf.RD(nCount);
		for(int i = 0 ; i < nCount ; ++i)
		{
			CodeOptions* c = new CodeOptions(ct_options);
			RDCode(dbuf, c);

			m_options.insert(make_pair(c->code, c));
			m_dic.insert(make_pair(c->code, c));
			m_dic.insert(make_pair(c->name, c));
			m_codesByID.insert(make_pair(c->nID, c));
			pPutPerMonth->codes.push_back(c);
		}
	}
	////////////////////////////////////////////////////////////////
		
	////////////////////////////////////////////////////////////////
	//해외선물
	dbuf.RD(nCount);
	for(int i = 0 ; i < nCount ; ++i)
	{
		CodeForeignFutures* c = new CodeForeignFutures;
		RDCode(dbuf, c);
		//c->nPricePointPos = c->pricePointPos.ToINT();

		m_foreign_futures.insert(make_pair(c->code, c));
		m_dic.insert(make_pair(c->code, c));
		m_dic.insert(make_pair(c->name, c));
		m_codesByID.insert(make_pair(c->nID, c));
	}
	////////////////////////////////////////////////////////////////
	
#ifdef _DEBUG
	DLOG(_T("선물코드 ========================================="));
	for(auto pos = m_futures.begin() ; pos != m_futures.end() ; ++pos)
	{
		DLOG(_T("\tCODE(%s)\tNAME(%s)\tEXPCODE(%s)"), pos->second->code, pos->second->name, pos->second->expCode);
	}
	DLOG(_T("================================================="));
#endif
	m_bInit = TRUE;
	return TRUE;
}