#include "StdAfx.h"
#include "HogaGrid.h"

#include "FastTradeDlg.h"
#include "VTS.h"

static COLORREF CR_MIT_SELL_BG = RGB(219, 238, 244);
static COLORREF CR_MIT_BUY_BG = RGB(253,234,218);

static COLORREF CR_SELL_BG = RGB(192,217,241);
static COLORREF CR_SELL_BID_INFO_BG = RGB(239,238,255);

static COLORREF CR_BUY_BG = RGB(252,213,181);
static COLORREF CR_BUY_BID_INFO_BG = RGB(255,237,237);

//static COLORREF CR_HOGA_IN_MM_BG = RGB(252,250,225);
static COLORREF CR_HOGA_IN_MM_BG = RGB(252,250,223);

static COLORREF CR_PRICE_BG = RGB(234,241,222);
#define DEFAULT_HEIGHT		17
#define DEFAULT_HIGH_HEIGHT		23
// 추가 2014-03-31
static COLORREF CRF_c_mit_sell    = RGB(199,242,197);
static COLORREF CRF_c_sell        = RGB(206,219,238);
static COLORREF CRF_c_sell_number = RGB(238,238,255);
static COLORREF CRF_c_sell_remain = RGB(238,238,255);
static COLORREF CRF_c_price       = RGB(255,255,255);
static COLORREF CRF_c_buy_remain  = RGB(255,236,236);
static COLORREF CRF_c_buy_number  = RGB(255,236,236);
static COLORREF CRF_c_buy         = RGB(248,222,221);
static COLORREF CRF_c_mit_buy     = RGB(199,242,197);
static COLORREF CRF_c_price_sel_txt = RGB(255,0,0);
static COLORREF CRF_c_price_sel_bg  = RGB(255,255,0);

HogaGrid::HogaGrid(void) : m_pTradeDlg(NULL), 
	m_bFixRowToCurrent(FALSE), m_lastPosition(PT_NONE), m_nLastMoveCol(NUL_IDX), m_nLastMoveRow(NUL_IDX), m_bOneClickOrder(FALSE),
	m_pCode(NULL), m_pHogaBase(NULL)
{
	m_penSellBorder.CreatePen (PS_SOLID,1,Colors::Blue);
	m_penBuyBorder.CreatePen (PS_SOLID,1,Colors::Red);
	m_penFixRow.CreatePen(PS_SOLID, 1, Colors::Black);
	m_penLosscut.CreatePen(PS_SOLID, 0, Colors::Blue);
	m_penEarncut.CreatePen(PS_SOLID, 0, Colors::Red);
	
	m_penPriceSide.CreatePen(PS_SOLID, 1, Colors::Black);
	
	m_nDragRow = NUL_IDX;
	m_nDragCol = NUL_IDX;

	m_nLastSellHogaMaxRow = NUL_IDX; 
	m_nLastSellHogaMinRow = NUL_IDX; 
	m_nLastBuyHogaMaxRow = NUL_IDX; 
	m_nLastBuyHogaMinRow = NUL_IDX;

	m_pAutoOrder = NULL;
	
	m_nHigh = 0;
	m_nLow = 0;
	m_nHighRow = NUL_IDX;
	m_nLowRow = NUL_IDX;
	m_nOpenRow = NUL_IDX;
#ifdef ENABLE_HOGA_FAKE_UPDATE
	m_nHogaFakeUpdateCancelTimerID = 1000;
#endif
}


HogaGrid::~HogaGrid(void)
{
}

BEGIN_MESSAGE_MAP(HogaGrid, CUGCtrl)
#ifdef ENABLE_HOGA_FAKE_UPDATE
	ON_WM_TIMER()
#endif
END_MESSAGE_MAP()


void HogaGrid::OnFinalize()
{
#ifdef ENABLE_HOGA_FAKE_UPDATE
	CancelFakeUpdateCancelTimers();
#endif
	ASSERT(m_pAutoOrder == NULL); // CFastTradeDlg에 의해 해제된다.
}

void HogaGrid::Init()
{
	CFont* pFont = GetFont(0);
	LOGFONT logFont; 
	pFont->GetLogFont(&logFont);

	m_fontSmallPr.CreateFont(12, 0, 0, 0, logFont.lfWeight + 200, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, logFont.lfFaceName);

	m_fontMedium.CreateFont(15, 0, 0, 0, logFont.lfWeight, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, logFont.lfFaceName);

	m_fontMediumPr.CreateFont(15, 0, 0, 0, logFont.lfWeight + 300, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, logFont.lfFaceName);

	m_fontLarge.CreateFont(18, 0, 0, 0, logFont.lfWeight, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, logFont.lfFaceName);
	
	m_fontLargePr.CreateFont(18, 0, 0, 0, logFont.lfWeight + 400, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, logFont.lfFaceName);

	SetDefFont(theApp.GetFontDefault());

	SetDoubleBufferMode(FALSE);
	m_GI->m_userSizingMode = FALSE;
	SetUniformRowHeight(TRUE); //Row의 높이가 바뀌면 모두 적용

	//SetHScrollMode(2);
	GetGridDefault(&m_cellTemp);
	/////////////////////////////////////////////////
	// default Cell 
	m_cellTemp.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);  //오른쪽 정렬이 기본값이다.
	delete m_GI->m_defBorderPen;
	m_GI->m_defBorderPen = new CPen(PS_SOLID, 1, RGB(180,180,180));

	/////////////////////////////////////////////////
	SetGridDefault(&m_cellTemp);
	SetDefRowHeight(DEFAULT_HEIGHT);
	m_bmHigh.LoadBitmap(IDB_HIGH);
	m_bmLow.LoadBitmap(IDB_LOW);
	m_bmOpen.LoadBitmap(IDB_OPEN);
	m_bmContractBuy.LoadBitmap(IDB_HOGA_CONTRACT_BUY);
	m_bmContractSell.LoadBitmap(IDB_HOGA_CONTRACT_SELL);
	SetCurrentCellMode(4);

	SetVScrollMode(UG_SCROLLTRACKING);


	GetHeadingDefault(&m_cellTemp);
	m_cellTemp.SetBackColor(COLOR_GRID_HEADER_BACK1);
	SetHeadingDefault(&m_cellTemp);
	SetSH_NumberCols(1);
	SetSH_ColWidth(-1, 0);

	int nColCount = sizeof(g_colsHoga) / sizeof(GridColumnItem);
	SetNumberCols(nColCount);
	
	for(int i = 0 ; i < nColCount ; ++i)
	{
		QuickSetText(i, -1, g_colsHoga[i].n);
//		SetColWidth(i, g_colsHoga[i].w);
	}
	//FitToWindow(0, c_mit_buy);
		
	GetColDefault(c_mit_sell, &m_cellTemp);
	m_cellTemp.SetBackColor(CRF_c_mit_sell);
	SetColDefault (c_mit_sell,&m_cellTemp);
	GetColDefault(c_sell, &m_cellTemp);
	//m_cellTemp.SetBorderColor (UG_SIDE_RIGHT, &m_penSellBorder);
	m_cellTemp.SetBorder (UG_BDR_RTHIN);
	m_cellTemp.SetBackColor(CRF_c_sell);
	SetColDefault (c_sell,&m_cellTemp);

	GetColDefault(c_sell_number, &m_cellTemp);
	m_cellTemp.SetBackColor(CRF_c_sell_number);
	SetColDefault (c_sell_number,&m_cellTemp);

	GetColDefault(c_sell_remain, &m_cellTemp);
	//m_cellTemp.SetBorderColor (UG_SIDE_RIGHT, &m_penPriceSide);
	m_cellTemp.SetBorder (UG_BDR_RTHIN);
	m_cellTemp.SetBackColor(CRF_c_sell_remain);
	SetColDefault (c_sell_remain,&m_cellTemp);
	GetColDefault(c_buy_remain, &m_cellTemp);
	m_cellTemp.SetBackColor(CRF_c_buy_remain);
	SetColDefault (c_buy_remain,&m_cellTemp);
	GetColDefault(c_buy_number, &m_cellTemp);
	//m_cellTemp.SetBorderColor (UG_SIDE_RIGHT, &m_penBuyBorder);
	m_cellTemp.SetBorder (UG_BDR_RTHIN);
	m_cellTemp.SetBackColor(CRF_c_buy_number);
	SetColDefault (c_buy_number,&m_cellTemp);
		
	GetColDefault(c_buy, &m_cellTemp);
	m_cellTemp.SetBackColor(CRF_c_buy);
	SetColDefault (c_buy,&m_cellTemp);
	
	/*
	GetColDefault(c_mit_sell, &m_cellTemp);
	m_cellTemp.SetBackColor(CR_MIT_SELL_BG);
	SetColDefault (c_mit_sell,&m_cellTemp);
	*/

	GetColDefault(c_mit_buy, &m_cellTemp);
	m_cellTemp.SetBackColor(CRF_c_mit_buy);
	SetColDefault (c_mit_buy,&m_cellTemp);

	//UGCELL_LABEL_SET
	m_cellTemp.ClearAll();
	GetCell(c_price, -1, &m_cellTemp);
	//m_cellTemp.SetPropertyFlags(m_cellTemp.GetPropertyFlags() | );
	m_cellTemp.SetCellType(UGCT_CHECKBOX);
	m_cellTemp.SetCellTypeEx(UGCT_CHECKBOXCHECKMARK);
	m_cellTemp.SetLabelText(g_colsHoga[c_price].n);
	m_cellTemp.SetBackColor(CR_PRICE_BG);
	SetCell(c_price, -1, &m_cellTemp);

	SetColumnWidth();
	
	/*
	GetColDefault(c_sell_remain, &m_cellTemp);
	m_cellTemp.SetBorderColor (UG_SIDE_RIGHT, &m_penPriceSide);
	m_cellTemp.SetBorder (UG_BDR_RTHIN);
	SetColDefault (c_sell_remain,&m_cellTemp);
	*/

	///////////////////////////////////////////////////
	// 호가 컬럼
	GetColDefault(c_price, &m_cellTemp);
	//m_cellTemp.SetFont(&m_fontSmallPr);
	m_cellTemp.SetFont(&m_fontSmallPr);
	m_cellTemp.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	m_cellTemp.SetBorder(UG_BDR_RTHIN);
	//m_cellTemp.SetBorderColor(UG_SIDE_RIGHT, &m_penPriceSide);
	m_cellTemp.SetTextColor(RGB(255,0,0));
	SetColDefault(c_price, &m_cellTemp);
	///////////////////////////////////////////////////
}

void HogaGrid::SetFontHeight(int n)
{
	CUGCell colPrice;
	CFont* pFont = NULL;
	if(n == 0)
	{
		pFont = &m_fontSmallPr;
		SetDefFont(theApp.GetFontDefault());
	}
	else if(n == 1)
	{
		pFont = &m_fontMediumPr;
		SetDefFont(&m_fontMedium);
	}
	else if(n == 2)
	{
		pFont = &m_fontLargePr;
		SetDefFont(&m_fontLarge);
	}

	GetColDefault(c_price, &m_cellTemp);
	m_cellTemp.SetFont(pFont);
	SetColDefault(c_price, &m_cellTemp);

	int nColCount = sizeof(g_colsHoga) / sizeof(GridColumnItem);
	for(int i = 0 ; i < nColCount ; ++i)
	{
		m_cellTemp.ClearAll();
		GetCell(i, -1, &m_cellTemp);
		m_cellTemp.SetFont(theApp.GetFontDefault());
		SetCell(i, -1, &m_cellTemp);
	}
}


void HogaGrid::SetColumnWidth()
{
	
#if 0
| 20 |10|10|10|10| 20 |10|10|
#endif

	CRect rt;
	GetClientRect(rt);
	int w = rt.Width() - m_GI->m_vScrollWidth;
	int nLeft = 0;
	int nCol = 0;
	int nWidth = 0;

	DOUBLE dWidth = 0.09;
	DOUBLE dWidth2 = 0.12;
	nWidth = (int)(w * dWidth2); SetColWidth(nCol++, nWidth); nLeft += nWidth;//MIT
	nWidth = (int)(w * dWidth2); SetColWidth(nCol++, nWidth); nLeft += nWidth;//매도
	nWidth = (int)(w * dWidth); SetColWidth(nCol++, nWidth); nLeft += nWidth;//건수
	nWidth = (int)(w * dWidth); SetColWidth(nCol++, nWidth); nLeft += nWidth;//잔량
	nCol++;//고정
	nWidth = (int)(w * dWidth); SetColWidth(nCol++, nWidth); nLeft += nWidth;//잔량
	nWidth = (int)(w * dWidth); SetColWidth(nCol++, nWidth); nLeft += nWidth;//건수
	nWidth = (int)(w * dWidth2); SetColWidth(nCol++, nWidth); nLeft += nWidth;//매수
	nWidth = (int)(w * dWidth2); SetColWidth(nCol++, nWidth); nLeft += nWidth;//MIT

	nWidth = w - nLeft;
	SetColWidth(c_price, nWidth);
}

void HogaGrid::InitHoga(GTCurrentPtr pCurrent)
{
	ASSERT(m_pAutoOrder);
	m_nHigh = pCurrent->high.DoubleToInt();
	m_nLow = pCurrent->low.DoubleToInt();
	int nOpen = pCurrent->open.DoubleToInt();
	if(0 != nOpen)
		m_nOpenRow = m_pAutoOrder->GetRowIndex(nOpen);

	if(0 != m_nHigh)
		m_nHighRow = m_pAutoOrder->GetRowIndex(m_nHigh);

	if(0 != m_nLow)
		m_nLowRow = m_pAutoOrder->GetRowIndex(m_nLow);


	m_nLastSellHogaMaxRow = NUL_IDX; 
	m_nLastSellHogaMinRow = NUL_IDX; 
	m_nLastBuyHogaMaxRow = NUL_IDX; 
	m_nLastBuyHogaMinRow = NUL_IDX;

	EnableUpdate(FALSE);
	DeleteAllRows();
	m_pCode = pCurrent->GetCode();

	int nColCount = sizeof(g_colsHoga) / sizeof(GridColumnItem);
	for(int i = 0 ; i < nColCount ; ++i)
	{
		QuickSetText(i, -1, g_colsHoga[i].n);
		QuickSetTextColor(i, -1, RGB(60,60,60));
		QuickSetBackColor(i, -1, RGB(255,255,255));
		QuickSetCellType(i,-1,0);
	}
	m_cellTemp.ClearAll();
	GetCell(c_price, -1, &m_cellTemp);
	m_cellTemp.SetCellType(UGCT_CHECKBOX);
	m_cellTemp.SetCellTypeEx(UGCT_CHECKBOXCHECKMARK);
	m_cellTemp.SetLabelText(g_colsHoga[c_price].n);
	m_cellTemp.SetBool(m_bFixRowToCurrent);
	SetCell(c_price, -1, &m_cellTemp);
	
	SetNumberRows(m_pAutoOrder->m_nHogaRowCount);
	
	CString s;
	int nPrice = m_pAutoOrder->m_nMax;
	int nPricePointPos = pCurrent->GetCode()->nPricePointPos;
	
	if(m_pAutoOrder->m_bOptionsOverlppedPrice3_0)
	{
		int _nRowCount = (m_pAutoOrder->m_nMax - 300) / 5;
		int nRow = 0;
		for(; nRow <= _nRowCount ; ++nRow, nPrice -= 5)
		{
			s = ::ToString(nPrice);
			s.Insert(s.GetLength() - 2, _T('.'));

			m_cellTemp.ClearAll();
			GetCell(c_price, nRow, &m_cellTemp);
			m_cellTemp.SetParam(nPrice);
			m_cellTemp.SetText(s);
		
			if(m_nHigh >= nPrice && nPrice >= m_nLow)
				m_cellTemp.SetBackColor(CR_HOGA_IN_MM_BG);
				//->m_cellTemp.SetBackColor(Colors::Beige);
			else
				m_cellTemp.SetBackColor(Colors::White);
				//->m_cellTemp.SetBackColor(Colors::LightSteelBlue);

			m_cellTemp.SetBorder(UG_BDR_RTHIN);
			//m_cellTemp.SetBorderColor(UG_SIDE_RIGHT, &m_penPriceSide);
			SetCell(c_price, nRow, &m_cellTemp);

			m_cellTemp.ClearAll();
			GetCell(c_sell_remain, nRow, &m_cellTemp);
			m_cellTemp.SetBorder (UG_BDR_RTHIN);
			//m_cellTemp.SetBorderColor (UG_SIDE_RIGHT, &m_penPriceSide);
			SetCell(c_sell_remain, nRow, &m_cellTemp);

			//RedrawCell(c_price, nRow);
			m_cellTemp.ClearAll();
			GetCell(c_sell, nRow, &m_cellTemp);
			m_cellTemp.SetBorder (UG_BDR_RTHIN);
			//m_cellTemp.SetBorderColor (UG_SIDE_RIGHT, &m_penSellBorder);
			SetCell(c_sell, nRow, &m_cellTemp);

			m_cellTemp.ClearAll();
			GetCell(c_buy_number, nRow, &m_cellTemp);
			m_cellTemp.SetBorder (UG_BDR_RTHIN);
			//m_cellTemp.SetBorderColor (UG_SIDE_RIGHT, &m_penBuyBorder);
			SetCell(c_buy_number, nRow, &m_cellTemp);
			
		}

		int nPrice = 299;
		for( ; nRow < m_pAutoOrder->m_nHogaRowCount ; ++nRow, nPrice -= 1)
		{
			if(nPrice < 100)
			{
				s.Format(_T("0.%02d"), nPrice);
			}
			else
			{
				s = ::ToString(nPrice);
				s.Insert(s.GetLength() - 2, _T('.'));
			}
			m_cellTemp.ClearAll();
			GetCell(c_price, nRow, &m_cellTemp);
			m_cellTemp.SetParam(nPrice);
			m_cellTemp.SetText(s);
		
			if(m_nHigh >= nPrice && nPrice >= m_nLow)
				m_cellTemp.SetBackColor(CR_HOGA_IN_MM_BG);
			else
				m_cellTemp.SetBackColor(Colors::White);

			m_cellTemp.SetBorder(UG_BDR_RTHIN);
			//m_cellTemp.SetBorderColor(UG_SIDE_RIGHT, &m_penPriceSide);
			SetCell(c_price, nRow, &m_cellTemp);

			m_cellTemp.ClearAll();
			GetCell(c_sell_remain, nRow, &m_cellTemp);
			m_cellTemp.SetBorder (UG_BDR_RTHIN);
			//m_cellTemp.SetBorderColor (UG_SIDE_RIGHT, &m_penPriceSide);
			SetCell(c_sell_remain, nRow, &m_cellTemp);

			//RedrawCell(c_price, nRow);
			m_cellTemp.ClearAll();
			GetCell(c_sell, nRow, &m_cellTemp);
			m_cellTemp.SetBorder (UG_BDR_RTHIN);
			//m_cellTemp.SetBorderColor (UG_SIDE_RIGHT, &m_penSellBorder);
			SetCell(c_sell, nRow, &m_cellTemp);
			m_cellTemp.ClearAll();
			GetCell(c_buy_number, nRow, &m_cellTemp);
			m_cellTemp.SetBorder (UG_BDR_RTHIN);
			//m_cellTemp.SetBorderColor (UG_SIDE_RIGHT, &m_penBuyBorder);
			SetCell(c_buy_number, nRow, &m_cellTemp);
		}
	}
	else
	{
		int n1Point = (INT)pow(10.0, (DOUBLE)nPricePointPos);
		for(int nRow = 0 ; nRow < m_pAutoOrder->m_nHogaRowCount ; ++nRow, nPrice -= m_pAutoOrder->m_nHogaUnit)
		{
			if(nPrice < n1Point)
			{
				CString fmt;
				fmt.Format(_T("0.%%0%dd"), nPricePointPos);

				s.Format(fmt, nPrice);
			}
			else
			{
				s = ::ToString(nPrice);
				s.Insert(s.GetLength() - nPricePointPos, _T('.'));
			}
			m_cellTemp.ClearAll();
			GetCell(c_price, nRow, &m_cellTemp);
			m_cellTemp.SetParam(nPrice);
			m_cellTemp.SetText(s);
		
			if(m_nHigh >= nPrice && nPrice >= m_nLow)
				m_cellTemp.SetBackColor(CR_HOGA_IN_MM_BG);
			else
				m_cellTemp.SetBackColor(Colors::White);

			m_cellTemp.SetBorder(UG_BDR_RTHIN);
			//m_cellTemp.SetBorderColor(UG_SIDE_RIGHT, &m_penPriceSide);

			SetCell(c_price, nRow, &m_cellTemp);
			//RedrawCell(c_price, nRow);

			m_cellTemp.ClearAll();
			GetCell(c_sell_remain, nRow, &m_cellTemp);
			m_cellTemp.SetBorder (UG_BDR_RTHIN);
			//m_cellTemp.SetBorderColor (UG_SIDE_RIGHT, &m_penPriceSide);
			SetCell(c_sell_remain, nRow, &m_cellTemp);


			m_cellTemp.ClearAll();
			GetCell(c_sell, nRow, &m_cellTemp);
			m_cellTemp.SetBorder (UG_BDR_RTHIN);
			//m_cellTemp.SetBorderColor (UG_SIDE_RIGHT, &m_penSellBorder);
			SetCell(c_sell, nRow, &m_cellTemp);

			m_cellTemp.ClearAll();
			GetCell(c_buy_number, nRow, &m_cellTemp);
			m_cellTemp.SetBorder (UG_BDR_RTHIN);
			//m_cellTemp.SetBorderColor (UG_SIDE_RIGHT, &m_penBuyBorder);
			SetCell(c_buy_number, nRow, &m_cellTemp);
		}
	}

	if(NUL_IDX != m_nOpenRow)
		QuickSetBitmap(c_price, m_nOpenRow, &m_bmOpen, FALSE);

	if(NUL_IDX != m_nHighRow)
		QuickSetBitmap(c_price, m_nHighRow, &m_bmHigh, FALSE);

	if(NUL_IDX != m_nLowRow)
		QuickSetBitmap(c_price, m_nLowRow, &m_bmLow, FALSE);
	
	int nCurPrice = pCurrent->price.DoubleToInt();
	int nRow = m_pAutoOrder->GetRowIndex(nCurPrice);
	

	int nVisible = m_GI->m_bottomRow - m_GI->m_topRow;
	SetTopRow(nRow - (nVisible / 2));
//	FitToWindow(c_mit_sell, c_mit_buy);

	PositionType pos = pCurrent->cgubun.ToString() == _T("+") ? PT_BUY : PT_SELL;
	QuickSetBackColor(c_price, nRow, CRF_c_price_sel_bg);
	QuickSetTextColor(c_price, nRow, CRF_c_price_sel_txt);
	m_nLastCurRow = nRow;

	const AutoOrder::MITs& mits = m_pAutoOrder->GetMITs();
	for(auto pos = mits.begin() ; pos != mits.end() ; ++pos)
	{
		const AutoOrder::MITItem& item = pos->second;
		OnAutoOrderMITChanged(item.position, item.strPrice, item.nVolume);
	}
	if(m_pAutoOrder->m_contractPos != PT_NONE)
	{
		ASSERT(m_pAutoOrder->m_nContractRow != NUL_IDX);
		OnAutoOrderContractChanged(NUL_IDX, m_pAutoOrder->m_nContractRow, m_pAutoOrder->m_contractPos);

		if(NUL_IDX != m_pAutoOrder->m_nLosscutRow)
		{
			ASSERT(m_pAutoOrder->m_contractPos != PT_NONE);
			OnAutoOrderSetLossCut(m_pAutoOrder->m_bLosscut, 0, NUL_IDX, m_pAutoOrder->m_nLosscutRow);
		}
		if(NUL_IDX != m_pAutoOrder->m_nEarncutRow)
		{
			ASSERT(m_pAutoOrder->m_contractPos != PT_NONE);
			OnAutoOrderSetEarnCut(m_pAutoOrder->m_bEarncut, 0, NUL_IDX, m_pAutoOrder->m_nEarncutRow);
		}
	}
#ifdef _DEBUG
	else
	{
		ASSERT(NUL_IDX == m_pAutoOrder->m_nLosscutRow && NUL_IDX == m_pAutoOrder->m_nEarncutRow);
	}
#endif
		
	QuickSetBackColor(4, -1, RGB(229,229,230));
	QuickSetTextColor(4, -1, RGB(0,0,0));
	EnableUpdate(TRUE);
	RedrawAll();
}

void HogaGrid::OnCurrentUpdated(GTCurrentPtr p)
{
	if(FALSE == p->IsPriceChanged())
		return;

	int nPrice = p->price.DoubleToInt();
	int nRow = m_pAutoOrder->GetRowIndex(nPrice);

	//CFuturesOptionsCurrentCommon* p = dynamic_cast<CFuturesOptionsCurrentCommon*>(pCurrent);
	PositionType pos = p->cgubun.ToString() == _T("+") ? PT_BUY : PT_SELL;
	
	QuickSetBackColor(c_price, m_nLastCurRow, CR_HOGA_IN_MM_BG);
	QuickSetTextColor(c_price, m_nLastCurRow, Colors::Black);
	RedrawCell(c_price, m_nLastCurRow);

	QuickSetBackColor(c_price, nRow, CRF_c_price_sel_bg);
	QuickSetTextColor(c_price, nRow, CRF_c_price_sel_txt);
	RedrawCell(c_price, nRow);

	m_lastPosition = pos;
	m_nLastCurRow = nRow;

	if(p->high.DoubleToInt() != m_nHigh)
	{
		if(NUL_IDX != m_nHighRow)
		{
			QuickSetBitmap(c_price, m_nHighRow, NULL);
			RedrawCell(c_price, m_nHighRow);
		}

		m_nHigh = p->high.DoubleToInt();
		m_nHighRow = m_pAutoOrder->GetRowIndex(m_nHigh);
				
		QuickSetBitmap(c_price, m_nHighRow, &m_bmHigh, FALSE);
		RedrawCell(c_price, m_nHighRow);
	}

	if(p->low.DoubleToInt() != m_nLow)
	{
		if(NUL_IDX != m_nLowRow)
		{
			QuickSetBitmap(c_price, m_nLowRow, NULL);
			RedrawCell(c_price, m_nLowRow);
		}

		m_nLow = p->low.DoubleToInt();
		m_nLowRow = m_pAutoOrder->GetRowIndex(m_nLow);

		QuickSetBitmap(c_price, m_nLowRow, &m_bmLow, FALSE);
		RedrawCell(c_price, m_nLowRow);
	}
}

void HogaGrid::OnHogaInitUpdated(GTHogaPtr p)
{
#ifdef ENABLE_HOGA_FAKE_UPDATE
	CancelFakeUpdateCancelTimers();
#endif

	ASSERT(m_pAutoOrder);
	if(FALSE == m_pAutoOrder->IsInit())
	{
		return;
	}

	m_pHogaBase = SER()->GetHogaBase(p->GetMerchandise()->GetCodeID());
	ASSERT(m_pHogaBase);


	m_nLastSellHogaMaxRow = NUL_IDX; 
	m_nLastSellHogaMinRow = NUL_IDX; 
	m_nLastBuyHogaMaxRow = NUL_IDX; 
	m_nLastBuyHogaMinRow = NUL_IDX;

#if 0
	for(int nHoga = 0 ; nHoga < GTHoga::HOGA_COUNT ; ++nHoga)
	{
		int nRow = p->sellHoga[nHoga].price.ToString().IsEmpty() || p->sellHoga[nHoga].price.DoubleToInt() == 0? NUL_IDX : 
					m_pAutoOrder->GetRowIndex(p->sellHoga[nHoga].price.DoubleToInt());
		
		if(NUL_IDX != nRow)
		{
			m_nLastSellHogaMaxRow = m_nLastSellHogaMaxRow == NUL_IDX ? nRow : max(m_nLastSellHogaMaxRow, nRow);
			m_nLastSellHogaMinRow = m_nLastSellHogaMinRow == NUL_IDX ? nRow : min(m_nLastSellHogaMinRow, nRow);
		}

		nRow = p->buyHoga[nHoga].price.ToString().IsEmpty() || p->buyHoga[nHoga].price.DoubleToInt() == 0 ? NUL_IDX : 
					m_pAutoOrder->GetRowIndex(p->buyHoga[nHoga].price.DoubleToInt());

		if(NUL_IDX != nRow)
		{
			m_nLastBuyHogaMaxRow = m_nLastBuyHogaMaxRow == NUL_IDX ? nRow : max(m_nLastBuyHogaMaxRow, nRow);
			m_nLastBuyHogaMinRow = m_nLastBuyHogaMinRow == NUL_IDX ? nRow : min(m_nLastBuyHogaMinRow, nRow);
		}
	}
#endif
	UGXPThemes::UseThemes(false);
	UGXPThemes::DrawBorderEdges(false);

	OnHogaUpdated(p);
}

void HogaGrid::OnHogaUpdated(GTHogaPtr p)
{
	ASSERT(m_pAutoOrder);
	if(FALSE == m_pAutoOrder->IsInit())
	{
		return;
	}

	int		nSellHogaMaxRow = NUL_IDX, nSellHogaMinRow = NUL_IDX;
	int		nBuyHogaMaxRow = NUL_IDX, nBuyHogaMinRow = NUL_IDX;

	for(int nHoga = 0 ; nHoga < GTHoga::HOGA_COUNT ; ++nHoga)
	{
		int nRow = p->sellHoga[nHoga].price.DoubleToInt() == 0? NUL_IDX : 
					m_pAutoOrder->GetRowIndex(p->sellHoga[nHoga].price.DoubleToInt());
		
		if(NUL_IDX != nRow)
		{
			INT geonsu = p->sellHoga[nHoga].number.ToINT();
			GT_VOLUME nRemain = p->sellHoga[nHoga].remain.ToINT();

#ifdef ENABLE_HOGA_FAKE_UPDATE
			auto pos = m_hogaFakeUpdates.find(p->sellHoga[nHoga].price.ToString());
			if(pos != m_hogaFakeUpdates.end())
			{
				if(geonsu > 0) //야간옵션일경우는 건수가 0이므로 더해주진 않는다
					geonsu += pos->second.first;
				nRemain += pos->second.second;
			}
#endif
			if (!m_pHogaBase->IsUpdated())
			{
				QuickSetText(c_sell_remain, nRow, ::ToString(abs(nRemain))); //절대값 수정
				QuickSetBackColor(c_sell_remain, nRow, CR_SELL_BID_INFO_BG);
				if (geonsu > 0)
				{
					QuickSetText(c_sell_number, nRow, ::ToString(geonsu));
					QuickSetBackColor(c_sell_number, nRow, CR_SELL_BID_INFO_BG);
				}
			}
			nSellHogaMaxRow = nSellHogaMaxRow == NUL_IDX ? nRow : max(nSellHogaMaxRow, nRow);
			nSellHogaMinRow = nSellHogaMinRow == NUL_IDX ? nRow : min(nSellHogaMinRow, nRow);
		}

		nRow = p->buyHoga[nHoga].price.DoubleToInt() == 0 ? NUL_IDX : 
					m_pAutoOrder->GetRowIndex(p->buyHoga[nHoga].price.DoubleToInt());

		if(NUL_IDX != nRow)
		{
			INT geonsu = p->buyHoga[nHoga].number.ToINT();
			GT_VOLUME nRemain = p->buyHoga[nHoga].remain.ToINT();
#ifdef ENABLE_HOGA_FAKE_UPDATE
			auto pos = m_hogaFakeUpdates.find(p->buyHoga[nHoga].price.ToString());
			if(pos != m_hogaFakeUpdates.end())
			{
				if(geonsu > 0) //야간옵션일경우는 건수가 0이므로 더해주진 않는다
					geonsu += pos->second.first;
				nRemain += pos->second.second;
			}
#endif

			if (!m_pHogaBase->IsUpdated())
			{
				QuickSetText(c_buy_remain, nRow, ::ToString(abs(nRemain))); //절대값 수정
				QuickSetBackColor(c_buy_remain, nRow, CR_BUY_BID_INFO_BG);
				if (geonsu > 0)
				{
					QuickSetText(c_buy_number, nRow, ::ToString(geonsu));
					QuickSetBackColor(c_buy_number, nRow, CR_BUY_BID_INFO_BG);
				}
			}
			nBuyHogaMaxRow = nBuyHogaMaxRow == NUL_IDX ? nRow : max(nBuyHogaMaxRow, nRow);
			nBuyHogaMinRow = nBuyHogaMinRow == NUL_IDX ? nRow : min(nBuyHogaMinRow, nRow);
		}
	}

	if (m_bFixRowToCurrent)
	{
		if(m_nLastSellHogaMaxRow != nSellHogaMaxRow)
		{
			if(NUL_IDX != m_nLastSellHogaMaxRow)
			{
				for(int c = 0 ; c <= c_mit_buy ; ++c)
				{
					m_cellTemp.ClearAll();
					GetCell(c, m_nLastSellHogaMaxRow, &m_cellTemp);

					int nBorder = m_cellTemp.GetBorder() & ~UG_BDR_BTHIN;
					m_cellTemp.SetBorder(nBorder);
					m_cellTemp.SetBorderColorClear(UG_SIDE_BOTTOM);
					SetCell(c, m_nLastSellHogaMaxRow, &m_cellTemp);
				}
				RedrawRow(m_nLastSellHogaMaxRow);
			}
		
			for(int c = 0 ; c <= c_mit_buy ; ++c)
			{
				m_cellTemp.ClearAll();
				GetCell(c, nSellHogaMaxRow, &m_cellTemp);
			
				int nBorder = m_cellTemp.GetBorder() | UG_BDR_BTHIN;
				m_cellTemp.SetBorder(nBorder);

				m_cellTemp.SetBorderColor(UG_SIDE_BOTTOM, &m_penFixRow);
				SetCell(c, nSellHogaMaxRow, &m_cellTemp);
			}
		
			SetTopRow(nSellHogaMaxRow - ((m_GI->m_bottomRow - m_GI->m_topRow) / 2));
			RedrawRow(nSellHogaMaxRow);
		}
		else
		{
			long nCurTopRow = GetTopRow();
			long nOrgTopRow = nSellHogaMaxRow - ((m_GI->m_bottomRow - m_GI->m_topRow) / 2);
			if(nCurTopRow != nOrgTopRow)
				SetTopRow(nOrgTopRow);
		}
	}
	
	
	if(m_nLastSellHogaMinRow != NUL_IDX && m_nLastSellHogaMinRow < nSellHogaMinRow)
	{
		for(int nRow = m_nLastSellHogaMinRow ; nRow < nSellHogaMinRow ; ++nRow)
		{
			QuickSetText(c_sell_number, nRow, _T(""));
			QuickSetBackColor(c_sell_number, nRow, CRF_c_sell_number);
			QuickSetText(c_sell_remain, nRow, _T(""));
			QuickSetBackColor(c_sell_remain, nRow, CRF_c_sell_remain);

			RedrawCell(c_sell_number, nRow);
			RedrawCell(c_sell_remain, nRow);
		}
	}
	if(m_nLastSellHogaMaxRow != NUL_IDX && m_nLastSellHogaMaxRow > nSellHogaMaxRow)
	{
		for(int nRow = nSellHogaMaxRow + 1 ; nRow <= m_nLastSellHogaMaxRow ; ++nRow)
		{
			QuickSetText(c_sell_number, nRow, _T(""));
			QuickSetBackColor(c_sell_number, nRow, CRF_c_sell_number);
			QuickSetText(c_sell_remain, nRow, _T(""));
			QuickSetBackColor(c_sell_remain, nRow, CRF_c_sell_remain);

			RedrawCell(c_sell_number, nRow);
			RedrawCell(c_sell_remain, nRow);
		}
	}
	

	if(m_nLastBuyHogaMinRow != NUL_IDX && m_nLastBuyHogaMinRow < nBuyHogaMinRow)
	{
		for(int nRow = m_nLastBuyHogaMinRow ; nRow < nBuyHogaMinRow; ++nRow)
		{
			QuickSetText(c_buy_remain, nRow, _T(""));
			QuickSetBackColor(c_buy_remain, nRow, CRF_c_buy_remain);
			QuickSetText(c_buy_number, nRow, _T(""));
			QuickSetBackColor(c_buy_number, nRow, CRF_c_buy_number);

			RedrawCell(c_buy_remain, nRow);
			RedrawCell(c_buy_number, nRow);
		}
	}
	if(m_nLastBuyHogaMaxRow != NUL_IDX && m_nLastBuyHogaMaxRow > nBuyHogaMaxRow)
	{
		for(int nRow = nBuyHogaMaxRow + 1; nRow <= m_nLastBuyHogaMaxRow  ; ++nRow)
		{
			QuickSetText(c_buy_remain, nRow, _T(""));
			QuickSetBackColor(c_buy_remain, nRow, CRF_c_buy_remain);
			QuickSetText(c_buy_number, nRow, _T(""));
			QuickSetBackColor(c_buy_number, nRow, CRF_c_buy_number);

			RedrawCell(c_buy_remain, nRow);
			RedrawCell(c_buy_number, nRow);
		}
	}

	m_nLastSellHogaMaxRow = nSellHogaMaxRow;
	m_nLastSellHogaMinRow = nSellHogaMinRow;
	m_nLastBuyHogaMaxRow = nBuyHogaMaxRow;
	m_nLastBuyHogaMinRow = nBuyHogaMinRow;

	if (!m_pHogaBase->IsUpdated())
	{
		RedrawRange(c_sell_number, nSellHogaMinRow, c_sell_remain, nSellHogaMaxRow);
		RedrawRange(c_buy_remain, nBuyHogaMinRow, c_buy_number, nBuyHogaMaxRow);

		if (m_bFixRowToCurrent)
			RedrawRow(m_nLastSellHogaMaxRow);
	}
}


void HogaGrid::OnLClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed)
{
	if(m_bOneClickOrder && 0 == updn)
	{
		if((NUL_IDX == m_nDragRow && NUL_IDX == m_nDragCol) ||
			(row == m_nDragRow && col == m_nDragCol))
			Click(col, row);
	}

	if(updn == 0)
	{
		if(NUL_IDX != m_nDragRow && NUL_IDX != m_nDragCol)
		{
			if(m_nDragCol == col && m_nDragRow == row)
			{

			}
			else
			{
				CString strVol = QuickGetText(m_nDragCol, m_nDragRow);
				CString srcPrice = QuickGetText(c_price, m_nDragRow);

				if(col == m_nDragCol) //정정
				{
					CString destPrice = QuickGetText(c_price, row);
										
					if(c_buy == col || c_sell == col)
					{	
						m_pTradeDlg->OrderCorrection(c_sell == col ? PT_SELL : PT_BUY, srcPrice, destPrice, strVol, om_drag);
					}
					else if(c_mit_buy == col || c_mit_sell == col)
					{
						PositionType posType = c_mit_sell == col ? PT_SELL : PT_BUY;
						CString strMsg;
						if(m_pAutoOrder->MIT_Correction(posType, srcPrice, destPrice, strMsg))
						{
							ORDER_ACTOR actor;
							CString strAction;
							strAction.Format(_T("%s: MIT정정"), OrderMethodStr(om_drag, actor));
							GetSession()->SendOrderAction(strAction, m_pCode->GetCode(), destPrice, posType, strVol);
						}
						else
						{
							ErrMsgBox(strMsg);
						}
					} 
				}
				else //취소
				{
					if(c_buy == m_nDragCol || c_sell == m_nDragCol)
					{
						m_pTradeDlg->OrderCancel(c_sell == m_nDragCol ? PT_SELL : PT_BUY, srcPrice, strVol, om_drag);
					}
					else if(c_mit_buy == m_nDragCol || c_mit_sell == m_nDragCol)
					{
						PositionType posType = c_mit_sell == m_nDragCol ? PT_SELL : PT_BUY;
						m_pAutoOrder->MIT_Cancel(posType, srcPrice);

						ORDER_ACTOR actor;
						CString strAction;
						strAction.Format(_T("%s: MIT취소"), OrderMethodStr(om_drag, actor));
						GetSession()->SendOrderAction(strAction, m_pCode->GetCode(), srcPrice, posType, strVol);
					} 
				}
			}
			
		}
		m_nDragRow = NUL_IDX;
		m_nDragCol = NUL_IDX;
	}
	else
	{
		if(col == c_mit_sell || col == c_sell || col == c_buy || col == c_mit_buy)
		{
			CString str = QuickGetText(col, row);
			if(FALSE == str.IsEmpty())
			{
				m_nDragRow = row;
				m_nDragCol = col;

				SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_ORDER_CORRECTION));
				//this->StartDragDrop();
			}
		}
	}
}
void HogaGrid::OnRClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed)
{

}
void HogaGrid::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	if(FALSE == m_bOneClickOrder)
		Click(col, row);
}

void HogaGrid::Click(int col,long row)
{
	m_pTradeDlg->OnHogaClickOrder(col, row, QuickGetText(c_price, row));
}

void HogaGrid::OnMouseLeave()
{
	if(NUL_IDX != m_nDragRow && NUL_IDX != m_nDragCol)
	{
		SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_ORDER_CANCEL));
	}
}

#if 0
DROPEFFECT HogaGrid::OnDragEnter(COleDataObject* pDataObject,int col,long row) 
{
	TRACE(_T("Drag start\n"));
	if(col == c_mit_sell || col == c_sell || col == c_buy || col == c_mit_buy)
	{
		return DROPEFFECT_MOVE;
	}
	return DROPEFFECT_NONE;
}

DROPEFFECT HogaGrid::OnDragOver(COleDataObject* pDataObject,int col,long row) 
{
	ASSERT(m_nDragCol != NUL_IDX);
	if(m_nDragCol == col)
	{
		return DROPEFFECT_MOVE;
	}
	return DROPEFFECT_NONE;
}


DROPEFFECT HogaGrid::OnDragDrop(COleDataObject* pDataObject,int col,long row) 
{
	TRACE(_T("OnDragDrop\n"));
	if(col == c_mit_sell || col == c_sell || col == c_buy || col == c_mit_buy)
	{
		if(m_nDragCol == col && m_nDragRow == row)
		{
			TRACE(_T("draged same cell\n"));
		}
		m_nDragCol = NUL_IDX;
		m_nDragRow = NUL_IDX;
		return DROPEFFECT_MOVE;
	}

	return DROPEFFECT_NONE;
}
#endif


void HogaGrid::OnMouseMove(int col,long row,POINT *point,UINT nFlags,BOOL processed)
{
	//TRACE(_T("col %d row %d\n"), col, row);
	if(nFlags & MK_LBUTTON)
	{
		if(NUL_IDX == m_nDragCol && NUL_IDX == m_nDragRow)
		{
			
		}
		else
		{
			// Drag중이다.
			// 같은 컬럼이면 Move준비하고 다른 컬럼이면 삭제
			if(col == m_nDragCol)
				SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_ORDER_CORRECTION));
			else
				SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_ORDER_CANCEL));
		}
	}

	if(c_mit_buy == col || c_mit_sell == col || c_buy == col || c_sell == col)
	{
		m_CUGGrid->m_drawHint.AddHint(m_nLastMoveCol, m_nLastMoveRow);
		m_CUGGrid->m_drawHint.AddHint(c_price, m_nLastMoveRow);
		m_CUGGrid->m_drawHint.AddHint(col, row);
		m_CUGGrid->m_drawHint.AddHint(c_price, row);
		
		m_cellTemp.ClearAll();
		GetCell(m_nLastMoveCol, m_nLastMoveRow, &m_cell);
		LONG prop = m_cell.GetPropertyFlags();
		m_cell.SetPropertyFlags(prop &= ~UGCELL_FOCUS_RECT_SET);
		SetCell(m_nLastMoveCol, m_nLastMoveRow, &m_cell);

		m_cellTemp.ClearAll();
		GetCell(col, row, &m_cell);
		prop = m_cell.GetPropertyFlags();
		m_cell.SetPropertyFlags(prop |= UGCELL_FOCUS_RECT_SET);
		SetCell(col, row, &m_cell);

		m_cellTemp.ClearAll();
		GetCell(c_price, m_nLastMoveRow, &m_cell);
		prop = m_cell.GetPropertyFlags();
		m_cell.SetPropertyFlags(prop &= ~UGCELL_FOCUS_RECT_SET);
		SetCell(c_price, m_nLastMoveRow, &m_cell);

		m_cellTemp.ClearAll();
		GetCell(c_price, row, &m_cell);
		prop = m_cell.GetPropertyFlags();
		m_cell.SetPropertyFlags(prop |= UGCELL_FOCUS_RECT_SET);
		SetCell(c_price, row, &m_cell);

		
		/*SetCurrent2(c_price, row);	
		GotoCell(col,row);*/

		m_nLastMoveCol = col;
		m_nLastMoveRow = row;

		Moved();
	}
	else
	{
		m_CUGGrid->m_drawHint.AddHint(m_nLastMoveCol, m_nLastMoveRow);
		m_CUGGrid->m_drawHint.AddHint(c_price, m_nLastMoveRow);
		m_CUGGrid->m_drawHint.AddHint(c_price, row);
		
		m_cellTemp.ClearAll();
		GetCell(m_nLastMoveCol, m_nLastMoveRow, &m_cell);
		LONG prop = m_cell.GetPropertyFlags();
		m_cell.SetPropertyFlags(prop &= ~UGCELL_FOCUS_RECT_SET);
		SetCell(m_nLastMoveCol, m_nLastMoveRow, &m_cell);

		m_cellTemp.ClearAll();
		GetCell(c_price, m_nLastMoveRow, &m_cell);
		prop = m_cell.GetPropertyFlags();
		m_cell.SetPropertyFlags(prop &= ~UGCELL_FOCUS_RECT_SET);
		SetCell(c_price, m_nLastMoveRow, &m_cell);

		m_cellTemp.ClearAll();
		GetCell(c_price, row, &m_cell);
		prop = m_cell.GetPropertyFlags();
		m_cell.SetPropertyFlags(prop |= UGCELL_FOCUS_RECT_SET);
		SetCell(c_price, row, &m_cell);

		m_nLastMoveCol = c_price;
		m_nLastMoveRow = row;

		Moved();
	}
}

int HogaGrid::OnCellTypeNotify(long ID,int col,long row,long msg,LONG_PTR param)
{
	if(c_price == col && -1 == row)
	{
		ASSERT(ID == UGCT_CHECKBOX);
		m_bFixRowToCurrent = param == 1;

		if(m_bFixRowToCurrent)
		{
			int nVisible = m_GI->m_bottomRow - m_GI->m_topRow;

			SetTopRow(m_nLastSellHogaMaxRow - (nVisible / 2));

			for(int c = 0 ; c <= c_mit_buy ; ++c)
			{
				m_cellTemp.ClearAll();
				GetCell(c, m_nLastSellHogaMaxRow, &m_cellTemp);

				int nBorder = m_cellTemp.GetBorder() | UG_BDR_BTHIN;
				m_cellTemp.SetBorder(nBorder);
				m_cellTemp.SetBorderColor(UG_SIDE_BOTTOM, &m_penFixRow);

				SetCell(c, m_nLastSellHogaMaxRow, &m_cellTemp);
			}
		}
		else
		{
			for(int c = 0 ; c <= c_mit_buy ; ++c)
			{
				m_cellTemp.ClearAll();
				GetCell(c, m_nLastSellHogaMaxRow, &m_cellTemp);

				int nBorder = m_cellTemp.GetBorder() & ~UG_BDR_BTHIN;
				
				m_cellTemp.SetBorder(nBorder);
				m_cellTemp.SetBorderColorClear(UG_SIDE_BOTTOM);
				SetCell(c, m_nLastSellHogaMaxRow, &m_cellTemp);
			}
		}

		RedrawAll();
		//RedrawRow(m_nLastCurRow);
	}
	return 0;
}


void HogaGrid::OnOrderUpdated(PositionType position, const CString& price, const CString& volume)
{
	CString s(price);
	s.Remove(_T('.'));
	int nPrice = ::ToINT(s);
	int nRow = m_pAutoOrder->GetRowIndex(nPrice);

	QuickSetText(PT_SELL == position ? c_sell : c_buy, nRow, volume);
	RedrawCell(PT_SELL == position ? c_sell : c_buy, nRow);
}

void HogaGrid::SetHighRowHeight(BOOL bFlag)
{
	SetRowHeight(0, bFlag ? DEFAULT_HIGH_HEIGHT : DEFAULT_HEIGHT );
	RedrawAll();
}
void HogaGrid::SetOneClickOrder(BOOL bFlag)
{
	m_bOneClickOrder = bFlag;
}

enum MENU_ID
{
	MID_CANCEL,
	MID_ORDER_VOL1,
	MID_ORDER_VOL2,
	MID_ORDER_VOL3,
	MID_ORDER_VOL5,
	MID_ORDER_VOL10,
	MID_ORDER_VOL15,
	MID_ORDER_VOL20,
};
void HogaGrid::OnMenuCommand(int col,long row,int section,int item)
{
	UNREFERENCED_PARAMETER(section);
	switch( item )
	{
	case MID_CANCEL: break;
	case MID_ORDER_VOL1: break;
	case MID_ORDER_VOL2: break;
	case MID_ORDER_VOL3: break;
	case MID_ORDER_VOL5: break;
	case MID_ORDER_VOL10: break;
	case MID_ORDER_VOL15: break;
	case MID_ORDER_VOL20: break;
	default:
		return;
	}
}

/////////////////////////////////////////////////////////////////////////////
//	OnMenuStart
//		Is called when the pop up menu is about to be shown
//	Params:
//		col, row	- the cell coordinates of where the menu originated from
//		setcion		- identify for which portion of the gird the menu is for.
//					possible sections:
//						UG_TOPHEADING, UG_SIDEHEADING,UG_GRID
//						UG_HSCROLL  UG_VSCROLL  UG_CORNERBUTTON
//	Return:
//		TRUE - to allow menu to show
//		FALSE - to prevent the menu from poping up
int HogaGrid::OnMenuStart(int col,long row,int section)
{
	EmptyMenu();

	if ( section == UG_GRID && col >= 0 && row >= 0 )
	{
		CString price = QuickGetText(c_price, row);
		CString s;
		if(c_sell == col || c_buy == col)
		{
			CString pos(c_sell == col ? _T("매도") : _T("매수"));
			CString vol = QuickGetText(col, row);
			if(FALSE == vol.IsEmpty())
			{
				s.Format(_T("%s 수량(%s) %s 취소"), price, vol, pos);
				AddMenuItem(MID_CANCEL, s);
			};

			s.Format(_T("%s %s: 수량 1"), price, pos);
			AddMenuItem(MID_ORDER_VOL1, s);
			s.Format(_T("%s %s: 수량 2"), price, pos);
			AddMenuItem(MID_ORDER_VOL2, s);
			s.Format(_T("%s %s: 수량 3"), price, pos);
			AddMenuItem(MID_ORDER_VOL3, s);
			s.Format(_T("%s %s: 수량 5"), price, pos);
			AddMenuItem(MID_ORDER_VOL5, s);
			s.Format(_T("%s %s: 수량 10"), price, pos);
			AddMenuItem(MID_ORDER_VOL10, s);
			s.Format(_T("%s %s: 수량 15"), price, pos);
			AddMenuItem(MID_ORDER_VOL15, s);
			s.Format(_T("%s %s: 수량 20"), price, pos);
			AddMenuItem(MID_ORDER_VOL20, s);
		}
	}
	else if ( section == UG_TOPHEADING && col >= 0 )
	{
		//AddMenuItem( 1002, "Column defaults" );
	}
	else if ( section == UG_CORNERBUTTON )
	{
		//AddMenuItem( 1003, "Grid defaults" );
	}

	return TRUE;
}


void HogaGrid::SetHorizontalLine(int nRow, CPen& pen, BOOL bBottom)
{
	for(int c = 0 ; c <= c_mit_buy ; ++c)
	{
		m_cellTemp.ClearAll();
		GetCell(c, nRow, &m_cellTemp);

		if(bBottom)
		{
			int nBorder = m_cellTemp.GetBorder() | UG_BDR_BTHIN;
			m_cellTemp.SetBorder(nBorder);
			m_cellTemp.SetBorderColor(UG_SIDE_BOTTOM, &pen);
		}
		else
		{
			int nBorder = m_cellTemp.GetBorder() | UG_BDR_TTHIN;
			m_cellTemp.SetBorder(nBorder);
			m_cellTemp.SetBorderColor(UG_SIDE_TOP, &pen);
		}

		SetCell(c, nRow, &m_cellTemp);
	}
	RedrawRow(nRow);
}

void HogaGrid::SetHorizontalLineRemove(int nRow, BOOL bBottom)
{
	if(m_nLastSellHogaMaxRow == nRow && TRUE == bBottom)
	{
		SetHorizontalLine(m_nLastSellHogaMaxRow, m_penFixRow, TRUE);
	}
	else
	{
	
		for(int c = 0 ; c <= c_mit_buy ; ++c)
		{
			m_cellTemp.ClearAll();
			GetCell(c, nRow, &m_cellTemp);

			if(bBottom)
			{
				int nBorder = m_cellTemp.GetBorder() & ~UG_BDR_BTHIN;
				m_cellTemp.SetBorder(nBorder);
				m_cellTemp.SetBorderColorClear(UG_SIDE_BOTTOM);
			}
			else
			{
				int nBorder = m_cellTemp.GetBorder() & ~UG_BDR_TTHIN;
				m_cellTemp.SetBorder(nBorder);
				m_cellTemp.SetBorderColorClear(UG_SIDE_TOP);
			}

			SetCell(c, nRow, &m_cellTemp);
		}
	}
	RedrawRow(nRow);
}

void HogaGrid::OnAutoOrderContractChanged(int nRowOld, int nRowNew, PositionType position)
{
	if(NUL_IDX != nRowOld)
	{
		// 이전 * 표시를 없앤다.
		QuickSetBitmap(c_price, nRowOld, NULL);
		RedrawCell(c_price, nRowOld);
	}

	if(NUL_IDX != nRowNew)
	{
		// 이전 * 표시를 없앤다.
		QuickSetBitmap(c_price, nRowNew, position == PT_BUY ? &m_bmContractBuy : &m_bmContractSell,  FALSE, position == PT_BUY);
		RedrawCell(c_price, nRowNew);
	}
}

void HogaGrid::OnAutoOrderSetLossCut(BOOL bEnable, int nTick, int nRowOld, int nRowNew)
{
	if(NUL_IDX != nRowOld)
		SetHorizontalLineRemove(nRowOld, m_pAutoOrder->m_contractPos == PT_BUY);

	if(bEnable && NUL_IDX != nRowNew)
		SetHorizontalLine(nRowNew, m_penLosscut, m_pAutoOrder->m_contractPos == PT_BUY);
}

void HogaGrid::OnAutoOrderSetEarnCut(BOOL bEnable, int nTick, int nRowOld, int nRowNew)
{
	if(NUL_IDX != nRowOld)
		SetHorizontalLineRemove(nRowOld, m_pAutoOrder->m_contractPos == PT_SELL);

	if(bEnable && NUL_IDX != nRowNew)
		SetHorizontalLine(nRowNew, m_penEarncut, m_pAutoOrder->m_contractPos == PT_SELL);
}

//void HogaGrid::OnMITOrderUpdated(PositionType position, const CString& price, const CString& volume)
void HogaGrid::OnAutoOrderMITChanged(PositionType posType, const CString& strPrice, GT_VOLUME volume)
{
	CString s(strPrice);
	s.Remove(_T('.'));
	int nPrice = ::ToINT(s);
	int nRow = m_pAutoOrder->GetRowIndex(nPrice);

	CString strVolume = volume == 0 ? _T("") : ::ToString(volume);

	QuickSetText(PT_SELL == posType ? c_mit_sell : c_mit_buy, nRow, strVolume);
	RedrawCell(PT_SELL == posType ? c_mit_sell : c_mit_buy, nRow);
}

#ifdef ENABLE_HOGA_FAKE_UPDATE
void HogaGrid::OnTimer(UINT_PTR nIDEvent)
{
	KillTimer(nIDEvent);
	
	auto posCancel = m_hogaFakeUpdateCancels.find(nIDEvent);
	if(posCancel == m_hogaFakeUpdateCancels.end())
		return;

	auto pos = m_hogaFakeUpdates.find(posCancel->second.first);
	if(pos == m_hogaFakeUpdates.end())
		return;
	
	if (posCancel->second.second < 0)
	{
		/// //////////////////////////////////////
		/// Cancel Fake
		//취소 FAKE를 다시 되돌릴때
		++pos->second.first;
		/// //////////////////////////////////////
	}
	else
	{
		--pos->second.first;
	}
	
	pos->second.second -= posCancel->second.second;
	OnHogaUpdated(m_pTradeDlg->m_pMerch->GetHoga());
	m_hogaFakeUpdateCancels.erase(posCancel);

	if (pos->second.second == 0)
		m_hogaFakeUpdates.erase(pos);

	CUGCtrl::OnTimer(nIDEvent);
}
void HogaGrid::CancelFakeUpdateCancelTimers()
{
	for(auto pos = m_hogaFakeUpdateCancels.begin() ; pos != m_hogaFakeUpdateCancels.end() ; ++pos)
	{
		KillTimer(pos->first);
	}
	m_hogaFakeUpdateCancels.clear();
}

void HogaGrid::ApplyRealOrderFailed(GTHogaPtr pHoga, OrderOutstanding* pOutstanding)
{
	auto pos = m_hogaFakeUpdates.find(pOutstanding->strPrice);

	if(pOutstanding->nVolumeRemain == 0)
	{
		if(pos != m_hogaFakeUpdates.end())
			m_hogaFakeUpdates.erase(pos);
		else
		{
			/// //////////////////////////////////////
			/// Cancel Fake
			if (pOutstanding->bHogaFakeUpdate == TRUE)
			{
				if (pos == m_hogaFakeUpdates.end())
				{
					pos = m_hogaFakeUpdates.insert(m_hogaFakeUpdates.begin(), make_pair(pOutstanding->strPrice, make_pair(0, 0)));
				}
				--pos->second.first;
				pos->second.second -= pOutstanding->nVolume;
			}
			/// //////////////////////////////////////
		}
	}
	else
	{
		if(pos == m_hogaFakeUpdates.end())
		{
			pos = m_hogaFakeUpdates.insert(m_hogaFakeUpdates.begin(), make_pair(pOutstanding->strPrice, make_pair(0, 0)));
		}
		++pos->second.first;
		pos->second.second += pOutstanding->nVolumeRemain;
	}

	OnHogaUpdated(pHoga);
}

void HogaGrid::ApplyHogaFakeUpdate(GTHogaPtr pHoga, OrderOutstanding* pOutstanding)
{
	ApplyRealOrderFailed(pHoga, pOutstanding);

	if (pOutstanding->nVolumeRemain == 0)
	{
		for (auto pos = m_hogaFakeUpdateCancels.begin(); pos != m_hogaFakeUpdateCancels.end(); ++pos)
		{
			if (pos->second.first == pOutstanding->strPrice)
			{
				KillTimer(pos->first);
				m_hogaFakeUpdateCancels.erase(pos);
				return;
			}
		}

		/// //////////////////////////////////////
		/// Cancel Fake
		/// 잔량을 빼준다.
		m_nHogaFakeUpdateCancelTimerID = SetTimer(m_nHogaFakeUpdateCancelTimerID++, 3000, NULL);

		m_hogaFakeUpdateCancels.insert(make_pair(m_nHogaFakeUpdateCancelTimerID,
			make_pair(pOutstanding->strPrice, -pOutstanding->nVolume)));
		/// //////////////////////////////////////
	}
	else
	{
		m_nHogaFakeUpdateCancelTimerID = SetTimer(m_nHogaFakeUpdateCancelTimerID++, 3000, NULL);

		m_hogaFakeUpdateCancels.insert(make_pair(m_nHogaFakeUpdateCancelTimerID,
			make_pair(pOutstanding->strPrice, pOutstanding->nVolumeRemain)));
	}
}
#endif