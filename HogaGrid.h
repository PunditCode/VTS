#pragma once

#include "UltimateGrid/UGCtrl.h"
#include "GTMerchandise.h"
#include "AutoOrder.h"
#include "ServerEventReceiver.h"

enum HogaColumn
{
	c_mit_sell,
	c_sell,
	c_sell_number,
	c_sell_remain,
	c_price,
	c_buy_remain,
	c_buy_number,
	c_buy,
	c_mit_buy,
};

const GridColumnItem g_colsHoga[] = 
{
	{_T("MIT"),	0.105},
	{_T("매도"),	0.105},
	{_T("건수"),	0.105},
	{_T("잔량"),	0.105},
	{_T("중앙정렬"),	0.16},
	{_T("잔량"),	0.105},
	{_T("건수"),	0.105},
	{_T("매수"),	0.105},
	{_T("MIT"),	0.105},
};


class HogaGrid : public CUGCtrl, public AutoOrderObserver
{
	
public:
	HogaGrid(void);
	~HogaGrid(void);

	void Init();
	void InitHoga(GTCurrentPtr pCurrent);

	void OnCurrentUpdated(GTCurrentPtr pCurrent);
	void OnHogaInitUpdated(GTHogaPtr p);
	void OnHogaUpdated(GTHogaPtr p);

	OVERRIDE void OnLClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed);
	OVERRIDE void OnRClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed);
	OVERRIDE void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
	OVERRIDE void OnMouseMove(int col,long row,POINT *point,UINT nFlags,BOOL processed=0);
	OVERRIDE void OnMouseLeave();

	OVERRIDE int OnCellTypeNotify(long ID,int col,long row,long msg,LONG_PTR param);
	OVERRIDE void OnMenuCommand(int col,long row,int section,int item);
	OVERRIDE int  OnMenuStart(int col,long row,int section);
	/*OVERRIDE DROPEFFECT OnDragEnter(COleDataObject* pDataObject,int col,long row);
	OVERRIDE DROPEFFECT OnDragOver(COleDataObject* pDataObject,int col,long row);
	OVERRIDE DROPEFFECT OnDragDrop(COleDataObject* pDataObject,int col,long row);*/

	void OnOrderUpdated(PositionType position, const CString& price, const CString& volume);

	void SetHighRowHeight(BOOL bFlag);
	void SetOneClickOrder(BOOL bFlag);
	void Click(int col,long row);

	void SetHorizontalLine(int nRow, CPen& pen, BOOL bBottom);
	void SetHorizontalLineRemove(int nRow, BOOL bBottom);
	void SetColumnWidth();
	void SetFontHeight(int n);

	OVERRIDE void OnAutoOrderSetLossCut(BOOL bEnable, int nTick, int nRowOld, int nRowNew);
	OVERRIDE void OnAutoOrderSetEarnCut(BOOL bEnable, int nTick, int nRowOld, int nRowNew);
	OVERRIDE void OnAutoOrderMITChanged(PositionType posType, const CString& strPrice, GT_VOLUME volume);
	OVERRIDE void OnAutoOrderContractChanged(int nRowOld, int nRowNew, PositionType position);

	void OnFinalize(); //CFastTradeDlg에 의해 호출됨 

#ifdef ENABLE_HOGA_FAKE_UPDATE
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void ApplyHogaFakeUpdate(GTHogaPtr pHoga, OrderOutstanding* pOutstanding);
	void ApplyRealOrderFailed(GTHogaPtr pHoga, OrderOutstanding* pOutstanding);
	std::map<CString, pair<INT, GT_VOLUME> > m_hogaFakeUpdates;
	std::map<INT, pair<CString, GT_VOLUME> > m_hogaFakeUpdateCancels;
	INT		m_nHogaFakeUpdateCancelTimerID;
	void CancelFakeUpdateCancelTimers();
#endif

private:
	friend class CFastTradeDlg;
	CFastTradeDlg*		m_pTradeDlg;
	AutoOrder*			m_pAutoOrder;
	Code*	m_pCode;
	
	int		m_nHigh, m_nLow, m_nHighRow, m_nLowRow, m_nOpenRow;

	int		m_nLastCurRow;
	int		m_nLastSellHogaMaxRow, m_nLastSellHogaMinRow;
	int		m_nLastBuyHogaMaxRow, m_nLastBuyHogaMinRow;
	CBitmap m_bmHigh, m_bmLow, m_bmOpen, m_bmContractBuy, m_bmContractSell;
	
	CFont	m_fontPrice;
	CFont	m_fontMedium, m_fontLarge;
	CFont	m_fontSmallPr, m_fontMediumPr, m_fontLargePr;
	CPen	m_penSellBorder, m_penBuyBorder;
	CPen	m_penFixRow;
	CPen	m_penPriceSide;

	BOOL	m_bFixRowToCurrent;
	CUGCell	m_cellTemp;
	PositionType	m_lastPosition;

	int		m_nLastMoveCol, m_nLastMoveRow;
	BOOL	m_bOneClickOrder;
	CPen	m_penLosscut, m_penEarncut;

	int		m_nDragRow, m_nDragCol;

	VTSCurrentBase*	m_pHogaBase;
	
	
public:
	DECLARE_MESSAGE_MAP()
	
};
