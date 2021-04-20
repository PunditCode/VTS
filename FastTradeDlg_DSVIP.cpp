// FastTradeDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#if COMPANY==COMPANY_DSVIP

#include "FastTradeDlg_DSVIP.h"
#include "afxdialogex.h"
#include "VTS.h"
#include "ServerEventReceiver.h"
#include "FuturesCodesDlg.h"
#include "OptionsTableDlg.h"
#include "ForeignFuturesCodeDlg.h"

// CFastTradeDlg_DSVIP 대화 상자입니다.
static COLORREF CR_BUY_BG = RGB(252,214,203);
static COLORREF CR_SELL_BG = RGB(214,235,240);
static COLORREF CR_SELL_BID_INFO_BG = RGB(205, 207, 232);
static COLORREF CR_BUY_BID_INFO_BG = RGB(234, 208, 207);
static COLORREF crContBack = RGB(253, 234, 218);
static COLORREF crOutstdBack = RGB(215, 228, 189);
static COLORREF crMitBack = RGB(230, 224, 236);

static COLORREF CR_SELL = RGB(1, 96, 245);
static COLORREF CR_BUY = Colors::Red;
static COLORREF CR_ALL = RGB(150, 212, 17);
static COLORREF CR_MERCH_TAB_SEL = RGB(255, 136, 16);
static COLORREF CR_MERCH_NONE_SEL = COLOR_CONTROL_BACK;
static COLORREF CR_DEF_GRID_HEADER_BG = COLOR_CONTROL_BACK;
static COLORREF CR_DEF_GRID_HEADER_TEXT = Colors::White; //RGB(221,221,221);

enum FieldContractProfit
{
	fld_cp_ContratedVolume,
	fld_cp_PriceAverage,
	fld_cp_EvalProfit,
	fld_cp_EvalProfitSum,
	fld_cp_RealtimeProfitTot,
	fld_cp_RealizedProfit,
	fld_cp_count
};
static GridColumnItem g_colsContProfit[] = 
{
	{_T("체결수량"),		0.13},
	{_T("평균단가"),		0.13},
	{_T("평가손익"),		0.17},
	{_T("평가손익합"),	0.17},
	{_T("실시간손익합"),  0.19},
	{_T("실현손익"),     0.18},
};


static GridColumnItem g_colsAccBal[] = 
{
	{_T("평가담보금"),			0.19},
	{_T("로스컷"),				0.16},
	{_T("선물오버가능액"),		0.21},
	{_T("옵션오버가능액"),		0.21},
	{_T("오버가능금액합계"),		0.21},
};


static GridColumnItem g_colsOP[] = 
{
	{_T("구분"), 0.12},
	{_T("종목"), 0.16},
	{_T("가격"), 0.15},
	{_T("수량"), 0.16},
	{_T("평가손익"), 0.18},
	{_T("주문번호"), 0.13},
#if 0
	{_T("잔량"), 0.07},
#else
	{_T("취소"), 0.07}, //순서가 바뀌면 COL_OP_CANCEL도 같이 바꿔줘야 함
#endif
};

enum
{
	COL_OP_SEP,
	COL_OP_CODE,
	COL_OP_PRICE,
	COL_OP_VOL,
	COL_OP_EVAL,
	COL_OP_NUM,
	COL_OP_CANCEL,
};


enum MerchandiseTab
{
	mt_futures,
	mt_options,
	mt_night_futures,
	mt_night_options,
	mt_foreign_futures
};



void OrderPanelGrid::OnSetup()
{
	m_nButtonIndex = AddCellType(&m_button);
}
int OrderPanelGrid::OnCellTypeNotify(long ID,int col,long row,long msg,long param){

	if(ID == m_nButtonIndex){
		return OnPushButton( ID, col, row, msg, param);
	}
	return TRUE;
}

int OrderPanelGrid::OnPushButton(long ID,int col,long row,long msg,long param){
	
	CUGCell cell;
	GetCell(col,row,&cell);
	int nCellTypeIndex = cell.GetCellType();
	int nParam = cell.GetParam();

	if(msg == UGCT_BUTTONUP)
	{
		m_pOwner->PostMessage(WM_ORDER_PANEL_GRID_BUTTON_CLICKED, (WPARAM)col, (LPARAM)row);
	}
	
	return TRUE;
}



IMPLEMENT_DYNCREATE(CFastTradeDlg_DSVIP, VTSFrmWndChildResizableDlg)

CFastTradeDlg_DSVIP::CFastTradeDlg_DSVIP(CWnd* pParent /*=NULL*/)
	: VTSFrmWndChildResizableDlg(CFastTradeDlg_DSVIP::IDD, pParent)
	, m_nPriceType(1), 
	m_bCurrentInit(FALSE), m_bHogaInit(FALSE),
	m_pSelCode(NULL), m_bExpand(FALSE), m_pCurContract(NULL), m_pOptionsTableDlg(NULL),
	m_nSizeMode(sz_small), m_bOneClick(FALSE), m_curMerchandiseType(ct_none),
	m_pForeignFuturesCodeDlg(NULL), m_pSelContractCode(NULL), m_pSelContractOPItem(NULL), m_pAutoOrder(NULL)
{
	//m_pMerch= NULL;

}

CFastTradeDlg_DSVIP::~CFastTradeDlg_DSVIP()
{
}

BOOL CFastTradeDlg_DSVIP::Create(CWnd* pParentWnd)
{
	return VTSFrmWndChildResizableDlg::Create(IDD, pParentWnd);
}


void CFastTradeDlg_DSVIP::DoDataExchange(CDataExchange* pDX)
{
	VTSFrmWndChildResizableDlg::DoDataExchange(pDX);
#ifdef _WNTAB
	DDX_Control( pDX, ID_TABCTRL_MERCHANDISE     , m_wndTabMerchandise );
	DDX_Control( pDX, IDC_TABCTRL_STOP           , m_wndTabStop        );
	DDX_Control( pDX, IDC_TABCTRL_CURRENT_INFO   , m_wndTabCurrentInfo );
	DDX_Control( pDX, IDC_TABCTRL_CONTRACTS      , m_wndTabContracts   );
#else
	DDX_Control(pDX, IDC_TAB_MERCHANDISE, m_tabMerchandise);
//	DDX_Control(pDX, IDC_TAB_CURRENT_INFO, m_tabCurrentInfo);
//	DDX_Control(pDX, IDC_TAB_STOP_ORDER, m_tabStopOrder);
//	DDX_Control(pDX, IDC_TAB_MARKET_CONTRACT, m_tabMarketContract);
#endif
	
	DDX_Control( pDX, IDC_EDIT_VOLUME       , m_editVol );
	DDX_Control( pDX, IDC_SPIN_VOL          , m_spinVol );
	DDX_Control( pDX, IDC_BUTTON_EXPAND     , m_buttonExpand);
	
	
	DDX_Control( pDX, IDC_BUTTON_MARKET_SELL     , m_buttons[BTN_MARKET_SELL     ]);
	DDX_Control( pDX, IDC_BUTTON_ALL_LIQUID      , m_buttons[BTN_ALL_LIQUID      ]);
	DDX_Control( pDX, IDC_BUTTON_MARKET_BUY      , m_buttons[BTN_MARKET_BUY      ]);
	DDX_Control( pDX, IDC_BUTTON_SELL_MIT_CANCEL , m_buttons[BTN_SELL_MIT_CANCEL ]);
	DDX_Control( pDX, IDC_BUTTON_SELL_CANCEL     , m_buttons[BTN_SELL_CANCEL     ]);
	DDX_Control( pDX, IDC_BUTTON_ALL_CANCEL      , m_buttons[BTN_ALL_CANCEL      ]);
	DDX_Control( pDX, IDC_BUTTON_BUY_CANCEL      , m_buttons[BTN_BUY_CANCEL      ]);
	DDX_Control( pDX, IDC_BUTTON_BUY_MIT_CANCEL  , m_buttons[BTN_BUY_MIT_CANCEL  ]);
	DDX_Control( pDX, IDC_BUTTON_SELECT_CODE     , m_buttons[BTN_SELECT_CODE     ]);


	DDX_Control( pDX, IDC_TAB_DISPLAY_SETTING     , m_btnTabDisplay);
	DDX_Control( pDX, IDC_TAB_CURRENT_PRICE, m_btnTabCurrent);
	DDX_Control( pDX, IDC_TAB_CONTRACTS         , m_btnTabContract);

	
	


	DDX_Control( pDX, IDC_STATIC_1               , m_labels[LBL_1                ]);
	DDX_Control( pDX, IDC_STATIC_SEL_CODE        , m_labels[LBL_SEL_CODE         ]);
	DDX_Control( pDX, IDC_STATIC_SELL            , m_labels[LBL_SELL             ]);
	DDX_Control( pDX, IDC_STATIC_ENABLE_SELL     , m_labels[LBL_ENABLE_SELL      ]);
	DDX_Control( pDX, IDC_STATIC_BUY             , m_labels[LBL_BUY              ]);
	DDX_Control( pDX, IDC_STATIC_ENABLE_BUY      , m_labels[LBL_ENABLE_BUY       ]);
	DDX_Control( pDX, IDC_STATIC_2               , m_labels[LBL_2                ]);
	
	DDX_Control( pDX, IDC_CHECK_LOSSCUT     , m_checkLosscut      );
	DDX_Control( pDX, IDC_CHECK_EARNCUT     , m_checkEarncut      );
	DDX_Control( pDX, IDC_EDIT_LOSSCUT      , m_editLosscut       );
	DDX_Control( pDX, IDC_EDIT_EARNCUT      , m_editEarncut       );
	DDX_Control( pDX, IDC_SPIN_LOSSCUT      , m_spinLosscut       );
	DDX_Control( pDX, IDC_SPIN_EARNCUT      , m_spinEarncut       );

}


BEGIN_MESSAGE_MAP(CFastTradeDlg_DSVIP, VTSFrmWndChildResizableDlg)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_SELECT_CODE, &CFastTradeDlg_DSVIP::OnBnClickedSelectCode)
	ON_MESSAGE(WM_ORDER_OUTSTANDING, &CFastTradeDlg_DSVIP::OnOrderOutstanding)
	ON_MESSAGE(WM_CONTRACTED_ORDER, &CFastTradeDlg_DSVIP::OnContractedOrder)
	ON_MESSAGE(WM_ORDER_CONTRACTS, &CFastTradeDlg_DSVIP::OnOrderContracts)
	ON_MESSAGE(WM_PROFIT_UPDATED, &CFastTradeDlg_DSVIP::OnProfitUpdated)
	ON_BN_CLICKED(IDC_BUTTON_MARKET_SELL, &CFastTradeDlg_DSVIP::OnBnClickedButtonMarketSell)
	ON_BN_CLICKED(IDC_BUTTON_MARKET_BUY, &CFastTradeDlg_DSVIP::OnBnClickedButtonMarketBuy)
	ON_BN_CLICKED(IDC_BUTTON_ALL_LIQUID, &CFastTradeDlg_DSVIP::OnBnClickedButtonAllLiquid)
	ON_BN_CLICKED(IDC_BUTTON_SELL_MIT_CANCEL, &CFastTradeDlg_DSVIP::OnBnClickedButtonSellMitCancel)
	ON_BN_CLICKED(IDC_BUTTON_BUY_MIT_CANCEL, &CFastTradeDlg_DSVIP::OnBnClickedButtonBuyMitCancel)
	ON_BN_CLICKED(IDC_BUTTON_SELL_CANCEL, &CFastTradeDlg_DSVIP::OnBnClickedButtonSellCancel)
	ON_BN_CLICKED(IDC_BUTTON_BUY_CANCEL, &CFastTradeDlg_DSVIP::OnBnClickedButtonBuyCancel)
	ON_BN_CLICKED(IDC_BUTTON_ALL_CANCEL, &CFastTradeDlg_DSVIP::OnBnClickedButtonAllCancel)
	ON_BN_CLICKED(IDC_CHECK_BIG_HOGA_HEIGHT, &CFastTradeDlg_DSVIP::OnBnClickedCheckBigHogaHeight)
	ON_BN_CLICKED(IDC_CHECK_ONECLICK_ORDER, &CFastTradeDlg_DSVIP::OnBnClickedCheckOneclickOrder)
	ON_BN_CLICKED(IDC_CHECK_LOSSCUT, &CFastTradeDlg_DSVIP::OnBnClickedCheckLosscut)
	ON_BN_CLICKED(IDC_CHECK_EARNCUT, &CFastTradeDlg_DSVIP::OnBnClickedCheckEarncut)
	ON_NOTIFY(UGN_SELCHANGED, IDC_GRID_HOGA, OnHogaGridEndSelChange)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_VOL_1, IDC_BUTTON_VOL_5, &CFastTradeDlg_DSVIP::OnBnClickedVolume)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_EXPAND, &CFastTradeDlg_DSVIP::OnBnClickedExpand)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MERCHANDISE, &CFastTradeDlg_DSVIP::OnMerchandiseTabSelChange)
	ON_MESSAGE(WM_PREFERENCES_CHANGED, &CFastTradeDlg_DSVIP::OnPreferencesChanged)
	ON_MESSAGE(WM_OPTIONS_TABLE_SELECT_CODE_CHANGED, &CFastTradeDlg_DSVIP::OnOptionsTableSelectCodeChanged)
	ON_MESSAGE(WM_ORDER_RESULT, &CFastTradeDlg_DSVIP::OnOrderResult)
	ON_WM_SIZING()
	ON_WM_SIZE()
	ON_MESSAGE(WM_VTS_MODELESS_DLG_DESTROYED, &CFastTradeDlg_DSVIP::OnVTSModelessDlgDestroyed)
	ON_MESSAGE(WM_ENABLE_ORDER_COUNT_RECEIVED, &CFastTradeDlg_DSVIP::OnEnableOrderCountReceived)
	ON_MESSAGE(WM_CLIENT_INFO_UPDATED, &CFastTradeDlg_DSVIP::OnClientInfoUpdated)
	ON_BN_CLICKED(IDC_CHECK_ALWAYS_ON_TOP, &CFastTradeDlg_DSVIP::OnBnClickedCheckAlwaysOnTop)
	ON_BN_CLICKED(IDC_CHECK_STANDALONE, &CFastTradeDlg_DSVIP::OnBnClickedCheckStandalone)
	ON_BN_CLICKED(IDC_RADIO1, &CFastTradeDlg_DSVIP::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CFastTradeDlg_DSVIP::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CFastTradeDlg_DSVIP::OnBnClickedRadio3)
	ON_NOTIFY(UGN_SELCHANGED, IDC_GRID_ORDER_PANEL, &CFastTradeDlg_DSVIP::OnOrderPanelGridSelChange)
	ON_MESSAGE(WM_VTSFRM_SHOW_COMPLETED, &CFastTradeDlg_DSVIP::OnVTSFrmShowCompleted)
	ON_MESSAGE(WM_ORDER_PANEL_GRID_BUTTON_CLICKED, &CFastTradeDlg_DSVIP::OnOrderPanelGridButtonClicked)
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_MOVE()
END_MESSAGE_MAP()

// CFastTradeDlg_DSVIP 메시지 처리기입니다.
int CFastTradeDlg_DSVIP::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (VTSFrmWndChildResizableDlg::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}


BOOL CFastTradeDlg_DSVIP::OnInitDialog()
{
	VTSFrmWndChildResizableDlg::OnInitDialog();

	m_gridHoga.AttachGrid(this, IDC_GRID_HOGA);
	m_gridMarketContract.AttachGrid(this, IDC_GRID_MARKET_CONTRACT);
	m_gridOrderPanel.AttachGrid(this, IDC_GRID_ORDER_PANEL);
	m_gridOrderPanel.m_pOwner = this;

	m_gridCurrent.AttachGrid(this, IDC_GRID_CUR_INFO);
	m_gridTotHoga.AttachGrid(this, IDC_GRID_TOT_HOGA);
	m_gridContractProfit.AttachGrid(this, IDC_GRID_CONTRACT_PROFIT);
	m_gridAccountBalance.AttachGrid(this, IDC_GRID_ACCOUNT_BALANCE);
	
	
	//VTSFrmWndChildResizableDlg::AddAnchor(IDC_GRID_HOGA, __RDA_LT, __RDA_RB);

	if(NULL == (HFONT)m_fontGridBold)
	{
		CFont* pFont = m_gridHoga.GetFont(0);
		LOGFONT logFont; 
		pFont->GetLogFont(&logFont);
		logFont.lfWeight = 1000;
		m_fontGridBold.CreateFontIndirect(&logFont);
	}

	InitControls();
	m_tabMerchandise.SetCurSel(0);

	SER().RegisterOrderOutstanding(this);
	SER().RegisterOrderContracted(this);
	SER().RegisterClientInfo(this);
		
	

	/////////////////////////////////////////////////////
	CRect rt;
	GetClientRect(rt);

	CRect rtHoga;
	m_gridHoga.GetClientRect(rtHoga);
	
	m_nSmallRight = rtHoga.right + 10;
	m_nCurRight = m_nSmallRight;
	m_nExpandAreaWidth = rt.right - m_nSmallRight;
		
	rt.right = m_nSmallRight;
	
	m_nSmallHeight = rt.bottom;
	m_nCurHeight = m_nSmallHeight;

	MoveWindow(rt);

	OnOrderContracts(0, 0);
	OnOrderOutstanding(0, -1);

	/////////////////////////////////////////////////////
	/*((CButton*)GetDlgItem(IDC_CHECK_STANDALONE))->SetCheck(BST_CHECKED);
	SendMessage(WM_COMMAND, MAKEWPARAM(IDC_CHECK_STANDALONE, BN_CLICKED));*/
//	OnBnClickedCheckStandalone();
	GetAutoOrderManager()->AttachObserver(dynamic_cast<AutoOrderManagerObserver*>(this));
	OnPreferencesChanged(0, 0);
	return TRUE;
}
LRESULT CFastTradeDlg_DSVIP::OnVTSFrmShowCompleted(WPARAM wParam, LPARAM lParam)
{
	OnMerchandiseTabSelChange(NULL, NULL);
	OnClientInfoUpdated(0, 0);
#if 0
	((CButton*)GetDlgItem(IDC_CHECK_STANDALONE))->SetCheck(BST_CHECKED);
//	SendMessage(WM_COMMAND, MAKEWPARAM(IDC_CHECK_STANDALONE, BN_CLICKED));
	OnBnClickedCheckStandalone();
#endif
	return 0;
}

void CFastTradeDlg_DSVIP::SetResize(SizeMode nMode)
{
	//오른쪽 숨겨져 있던 영역의 컨트롤 들을 모두 재배치 한다.
	if(m_nSizeMode == nMode)
		return;

	int nPrevRight = m_nCurRight;
	int nPrevHeight = m_nCurHeight;

	int nHogaHeight = 0;
	switch(nMode)
	{
	case sz_small:
		m_nCurRight = m_nSmallRight;
		m_nCurHeight = m_nSmallHeight;
		nHogaHeight = 17;
		break;
	case sz_medium:
		m_nCurRight = m_nSmallRight + 40;
		m_nCurHeight = m_nSmallHeight + 40;
		nHogaHeight = 21;
		break;
	case sz_large:
		m_nCurRight = m_nSmallRight + 80;
		m_nCurHeight = m_nSmallHeight + 80;
		nHogaHeight = 25;
		break;
	}

	m_gridHoga.SetFontHeight(nMode);

	int nMove = m_nCurRight - nPrevRight;

	CRect rt;
#define EXPAND(nID) GetDlgItem(nID)->GetWindowRect(rt); ScreenToClient(rt); rt.right += nMove; GetDlgItem(nID)->MoveWindow(rt);
#define MOVE(nID) GetDlgItem(nID)->GetWindowRect(rt); ScreenToClient(rt); rt.left += nMove ; rt.right += nMove; GetDlgItem(nID)->MoveWindow(rt);

	RemoveAnchor(IDC_GRID_HOGA);
	RemoveAnchor(IDC_GRID_MARKET_CONTRACT);
	RemoveAnchor(IDC_GRID_TOT_HOGA); 
	RemoveAnchor(IDC_BUTTON_SELL_MIT_CANCEL); 
	RemoveAnchor(IDC_BUTTON_SELL_CANCEL); 
	RemoveAnchor(IDC_BUTTON_ALL_CANCEL); 
	RemoveAnchor(IDC_BUTTON_BUY_CANCEL); 
	RemoveAnchor(IDC_BUTTON_BUY_MIT_CANCEL); 
	RemoveAnchor(IDC_STATIC_REAL_EVAL); 
	RemoveAnchor(IDC_STATIC_PROFIT_REALIZE); 
	RemoveAnchor(IDC_STATIC_EVAL); 
	RemoveAnchor(IDC_STATIC_PROFIT_EVAL); 
	RemoveAnchor(IDC_GRID_ORDER_PANEL); 
	RemoveAnchor(IDC_GRID_OUTSTANDING_ORDER); 
	RemoveAnchor(IDC_GRID_MIT); 

	MOVE(IDC_BUTTON_SELECT_CODE);
	MOVE(IDC_TAB_DISPLAY_SETTING);
	MOVE(IDC_CHECK_ALWAYS_ON_TOP);
	MOVE(IDC_CHECK_STANDALONE);
	MOVE(IDC_RADIO1);
	MOVE(IDC_RADIO2);
	MOVE(IDC_RADIO3);
	MOVE(IDC_TAB_CURRENT_PRICE);
	MOVE(IDC_GRID_CUR_INFO);
	MOVE(IDC_TAB_CONTRACTS);
	MOVE(IDC_GRID_MARKET_CONTRACT);
	
	EXPAND(IDC_GRID_CONTRACT_PROFIT);
	EXPAND(IDC_GRID_ACCOUNT_BALANCE);
	MOVE(IDC_BUTTON_EXPAND);
	
	EXPAND(IDC_GRID_HOGA);
	EXPAND(IDC_GRID_TOT_HOGA);
	
	EXPAND(IDC_GRID_ORDER_PANEL);
	
	m_gridHoga.SetDefRowHeight(nHogaHeight);
	m_gridHoga.SetColumnWidth();
	m_gridHoga.RedrawAll();

#define MVB(nID)  GetDlgItem(nID)->GetWindowRect(rt); ScreenToClient(rt); rt.left = nLeft; rt.right = rt.left + nWidth; GetDlgItem(nID)->MoveWindow(rt);

	CRect rtHoga;
	m_gridHoga.GetWindowRect(rtHoga); ScreenToClient(rtHoga);
	int nLeft = rtHoga.left;
	const int nColCount = m_gridHoga.GetNumberCols();

	for(int nCol = 0 ; nCol < nColCount ; ++nCol)
	{
		int nWidth = m_gridHoga.GetColWidth(nCol);
		m_gridTotHoga.SetColWidth(nCol, nWidth);
		nLeft += nWidth;
	}
	int nRightHoga = rtHoga.right - m_gridHoga.m_GI->m_vScrollWidth;

	//호가창 상단 버튼
	int nBtnWidth = 0;
	GetDlgItem(IDC_BUTTON_MARKET_SELL)->GetWindowRect(rt); ScreenToClient(rt); nBtnWidth = rt.Width();
	rt.left = rtHoga.left; rt.right = rt.left + nBtnWidth; 
	GetDlgItem(IDC_BUTTON_MARKET_SELL)->MoveWindow(rt);

	GetDlgItem(IDC_BUTTON_ALL_LIQUID)->GetWindowRect(rt); ScreenToClient(rt); nBtnWidth = rt.Width(); 
	rt.left = rtHoga.left + ((nRightHoga - rtHoga.left) / 2) - (nBtnWidth / 2);
	rt.right = rt.left + nBtnWidth;
	GetDlgItem(IDC_BUTTON_ALL_LIQUID)->MoveWindow(rt);

	GetDlgItem(IDC_BUTTON_MARKET_BUY)->GetWindowRect(rt); ScreenToClient(rt); nBtnWidth = rt.Width(); 
	rt.left = nRightHoga - nBtnWidth;
	rt.right = nRightHoga;
	GetDlgItem(IDC_BUTTON_MARKET_BUY)->MoveWindow(rt);

	//호가창 하단의 각 버튼
	nLeft = rtHoga.left;
	GetDlgItem(IDC_BUTTON_SELL_MIT_CANCEL)->GetWindowRect(rt); 
	nBtnWidth = rt.Width();

	ScreenToClient(rt); rt.left = nLeft; rt.right = rt.left + nBtnWidth; GetDlgItem(IDC_BUTTON_SELL_MIT_CANCEL)->MoveWindow(rt);
	nLeft += nBtnWidth + 2;
	rt.left = nLeft; rt.right = nLeft + nBtnWidth; GetDlgItem(IDC_BUTTON_SELL_CANCEL)->MoveWindow(rt);

	int nRight = nRightHoga;
	rt.right = nRight; rt.left = nRight - nBtnWidth; GetDlgItem(IDC_BUTTON_BUY_MIT_CANCEL)->MoveWindow(rt);
	nRight -= (nBtnWidth + 2);
	rt.right = nRight; rt.left = nRight - nBtnWidth; GetDlgItem(IDC_BUTTON_BUY_CANCEL)->MoveWindow(rt);


	GetDlgItem(IDC_BUTTON_ALL_CANCEL)->GetWindowRect(rt); ScreenToClient(rt); nBtnWidth = rt.Width(); 
	rt.left = rtHoga.left + ((nRightHoga - rtHoga.left) / 2) - (nBtnWidth / 2);
	rt.right = rt.left + nBtnWidth;
	GetDlgItem(IDC_BUTTON_ALL_CANCEL)->MoveWindow(rt);

	{
		const int COL_COUNT = sizeof(g_colsContProfit) / sizeof(GridColumnItem);
		m_gridContractProfit.GetClientRect(rt);
		int w = rt.Width();
		int nLeft = 0;
		int nWidth = 0;

		for(int nCol = 0 ; nCol < COL_COUNT ; ++nCol)
		{
			if(nCol == COL_COUNT - 1)
			{
				nWidth = w - nLeft;
			}
			else
			{
				nWidth = (int)(w * g_colsContProfit[nCol].w);
			}
			m_gridContractProfit.SetColWidth(nCol, nWidth); 
			nLeft += nWidth;
		}
	}

	{
		const int COL_COUNT = sizeof(g_colsAccBal) / sizeof(GridColumnItem);
		m_gridAccountBalance.GetClientRect(rt);
		int w = rt.Width();
		int nLeft = 0;
		int nWidth = 0;

		for(int nCol = 0 ; nCol < COL_COUNT ; ++nCol)
		{
			if(nCol == COL_COUNT - 1)
			{
				nWidth = w - nLeft;
			}
			else
			{
				nWidth = (int)(w * g_colsAccBal[nCol].w);
			}
			m_gridAccountBalance.SetColWidth(nCol, nWidth); 
			nLeft += nWidth;
		}
	}

	{
		const int COL_COUNT = sizeof(g_colsOP) / sizeof(GridColumnItem);
		m_gridOrderPanel.GetClientRect(rt);
		int w = rt.Width() - m_gridOrderPanel.m_GI->m_vScrollWidth;
		int nLeft = 0;
		int nWidth = 0;
		for(int nCol = 0 ; nCol < COL_COUNT ; ++nCol)
		{
			if(nCol == COL_COUNT - 1)
			{
				nWidth = w - nLeft;
			}
			else
			{
				nWidth = (int)(w * g_colsOP[nCol].w);
			}
			m_gridOrderPanel.SetColWidth(nCol, nWidth); 
			nLeft += nWidth;
		}
	}

	CSize sz(0,80);

	AddAnchor(IDC_GRID_HOGA, __RDA_LT, sz);
	AddAnchor(IDC_GRID_MARKET_CONTRACT, __RDA_LT, __RDA_LB);

	//사이즈 변경없이 아래쪽으로 움직일때 따라서 움직인다.
	AddAnchor(IDC_GRID_TOT_HOGA,  sz); 
	AddAnchor(IDC_BUTTON_SELL_MIT_CANCEL, sz); 
	AddAnchor(IDC_BUTTON_SELL_CANCEL,sz); 
	AddAnchor(IDC_BUTTON_ALL_CANCEL, sz); 
	AddAnchor(IDC_BUTTON_BUY_CANCEL,sz); 
	AddAnchor(IDC_BUTTON_BUY_MIT_CANCEL, sz); 
	AddAnchor(IDC_GRID_ORDER_PANEL,  sz, __RDA_LB); 

	AddAnchor(IDC_STATIC_REAL_EVAL, __RDA_LB); 
	AddAnchor(IDC_STATIC_PROFIT_REALIZE, __RDA_LB); 
	AddAnchor(IDC_STATIC_EVAL, __RDA_LB); 
	AddAnchor(IDC_STATIC_PROFIT_EVAL, __RDA_LB); 
	AddAnchor(IDC_GRID_OUTSTANDING_ORDER, __RDA_LB); 
	AddAnchor(IDC_GRID_MIT, __RDA_LB); 
	
	m_nSizeMode = nMode;

	GetClientRect(rt);
	rt.right += nMove;
	rt.bottom =m_nCurHeight;
	//MoveWindow(rt);
	
	
	GetParent()->SendMessage(WM_VTSFRM_CHILD_RESIZE_REQUEST, (WPARAM)rt.Width(), (LPARAM)rt.Height());

}


void CFastTradeDlg_DSVIP::InitControls()
{
	m_penGridHeaderBorder.CreatePen(PS_SOLID,1,RGB(45,46,54));

	DWORD dwAddExBorderStyles = WS_EX_STATICEDGE;
	DWORD dwRemoveExBorderStyles = WS_EX_STATICEDGE;
#ifdef _WNDTAB
	m_wndTabMerchandise.ModifyTabWndStyle(		0, __ETWS_ORIENT_TOP);//	| __ETWS_GROUPED );
	m_wndTabMerchandise.ModifyStyleEx( dwRemoveExBorderStyles, dwAddExBorderStyles );
	m_wndTabMerchandise.ItemInsert(_T("선물"));
	m_wndTabMerchandise.ItemInsert(_T("옵션"));
	m_wndTabMerchandise.ItemInsert(_T("야간선물"));
	m_wndTabMerchandise.ItemInsert(_T("야간옵션"));
	m_wndTabMerchandise.ItemInsert(_T("해외선물"));
	
	m_wndTabMerchandise.ModifyTabWndStyle(0, __ETWS_EQUAL_WIDTHS);
	m_wndTabMerchandise.ModifyTabWndStyle(0, __ETWS_BOLD_SELECTION);
	m_wndTabMerchandise.SelectionSet(0, true);
	m_wndTabMerchandise.ModifyStyleEx( 0, WS_EX_CONTROLPARENT );
	m_wndTabMerchandise.m_pOwner = this;

	m_wndTabStop.ItemInsert(_T("STOP주문"));
	m_wndTabStop.ModifyTabWndStyle(0, __ETWS_EQUAL_WIDTHS);
	m_wndTabStop.ModifyTabWndStyle(0, __ETWS_BOLD_SELECTION);
	m_wndTabStop.SelectionSet(0, true);

	m_wndTabCurrentInfo.ItemInsert(_T("가격정보"));
	m_wndTabCurrentInfo.ModifyTabWndStyle(0, __ETWS_EQUAL_WIDTHS);
	m_wndTabCurrentInfo.ModifyTabWndStyle(0, __ETWS_BOLD_SELECTION);
	m_wndTabCurrentInfo.SelectionSet(0, true);

	m_wndTabContracts.ItemInsert(_T("체결정보"));
	m_wndTabContracts.ModifyTabWndStyle(0, __ETWS_EQUAL_WIDTHS);
	m_wndTabContracts.ModifyTabWndStyle(0, __ETWS_BOLD_SELECTION);
	m_wndTabContracts.SelectionSet(0, true);

#else


	DWORD dwFlags = ETC_SELECTION | ETC_COLOR;

	m_tabMerchandise.EnableDraw(BTC_TABS); 
	m_tabMerchandise.SetSelectTabColor(CR_MERCH_TAB_SEL);
	m_tabMerchandise.SetSelectTextColor(Colors::White);
	m_tabMerchandise.EnableCustomLook(dwFlags, dwFlags);
	m_tabMerchandise.SetNoneSelectTabColor(CR_MERCH_NONE_SEL);
	m_tabMerchandise.SetBkgndColor(RGB(198,198,198));


	m_tabMerchandise.InsertItem(0, _T("선물"));
	m_tabMerchandise.InsertItem(1, _T("옵션"));
	m_tabMerchandise.InsertItem(2, _T("야간선물"));
	m_tabMerchandise.InsertItem(3, _T("야간옵션"));
	m_tabMerchandise.InsertItem(4, _T("해외선물"));

	
	/*m_tabStopOrder.EnableDraw(BTC_TABS); 
	m_tabStopOrder.SetSelectTabColor(RGB(247,150,70));
	m_tabStopOrder.SetSelectTextColor(Colors::White);
	m_tabStopOrder.SetBkgndColor(RGB(198,198,198));*/

	//m_tabStopOrder.EnableCustomLook(dwFlags, dwFlags);
	//m_tabStopOrder.InsertItem(0, _T("화면설정"));
	//m_tabStopOrder.SetCurSel(0);


	//m_tabCurrentInfo.EnableDraw(BTC_TABS); 
	//m_tabCurrentInfo.SetSelectTabColor(RGB(146,208,80));
	//m_tabCurrentInfo.SetSelectTextColor(Colors::White);
	//m_tabCurrentInfo.SetBkgndColor(COLOR_DEFAULT_BACK);

	//m_tabCurrentInfo.EnableCustomLook(dwFlags, dwFlags);
	//m_tabCurrentInfo.InsertItem(0, _T("가격정보"));
	//m_tabCurrentInfo.SetCurSel(0);

	/*m_tabMarketContract.EnableDraw(BTC_TABS); 
	m_tabMarketContract.SetSelectTabColor(RGB(0,176,240));
	m_tabMarketContract.SetSelectTextColor(Colors::White);
	m_tabMarketContract.SetBkgndColor(COLOR_DEFAULT_BACK);

	m_tabMarketContract.EnableCustomLook(dwFlags, dwFlags);
	m_tabMarketContract.InsertItem(0, _T("체결정보"));
	m_tabMarketContract.SetCurSel(0);*/
#endif


	m_labels[LBL_SELL].SetBkColor(CR_SELL);
	m_labels[LBL_SELL].SetTextColor(true, Colors::White);
	m_labels[LBL_BUY].SetBkColor(CR_BUY);
	m_labels[LBL_BUY].SetTextColor(true, Colors::White);
	m_labels[LBL_SEL_CODE].SetTextColor(true, RGB(255, 246, 1));

	m_labels[LBL_ENABLE_SELL].SetBkColor(Colors::White);
	m_labels[LBL_ENABLE_SELL].SetTextColor(true, Colors::Black);
	m_labels[LBL_ENABLE_BUY].SetBkColor(Colors::White);
	m_labels[LBL_ENABLE_BUY].SetTextColor(true, Colors::Black);

	
		CString strImgRoot = ENV().GetImgRoot();

#define BTN_SET(__n__) m_buttons[BTN_##__n__].SetSkin(strImgRoot + _T(#__n__)_T("_N.png"), strImgRoot + _T(#__n__)_T("_P.png"), strImgRoot + _T(#__n__)_T("_O.png"), 0, 0, COLOR_IMG_TRANSPARENT, 0, 0);

	BTN_SET(MARKET_SELL     );
	BTN_SET(ALL_LIQUID      );
	BTN_SET(MARKET_BUY      );
	BTN_SET(SELL_MIT_CANCEL );
	BTN_SET(SELL_CANCEL     );
	BTN_SET(ALL_CANCEL      );
	BTN_SET(BUY_CANCEL      );
	BTN_SET(BUY_MIT_CANCEL  );

	m_btnTabDisplay.SetSkin(strImgRoot + _T("TAB_DISPLAY.png"), strImgRoot + _T("TAB_DISPLAY.png"), NULL, strImgRoot + _T("TAB_DISPLAY.png"), NULL, COLOR_IMG_TRANSPARENT, 0, 0);
	m_btnTabCurrent.SetSkin(strImgRoot + _T("TAB_CURRENT.png"), strImgRoot + _T("TAB_CURRENT.png"), NULL, strImgRoot + _T("TAB_CURRENT.png"), NULL, COLOR_IMG_TRANSPARENT, 0, 0);
	m_btnTabContract.SetSkin(strImgRoot + _T("TAB_CONTRACTS.png"), strImgRoot + _T("TAB_CONTRACTS.png"), NULL, strImgRoot + _T("TAB_CONTRACTS.png"), NULL, COLOR_IMG_TRANSPARENT, 0, 0);



	//CExtBitmap  bitmap;
	//bitmap.LoadBMP_Resource(MAKEINTRESOURCE(IDB_BITMAP_FIND));
	//bitmap.Make32();
	////bitmap.AlphaColor();
	//
	//m_buttons[BTN_SELECT_CODE     ].SetIcon(bitmap.CreateHICON());
	//m_buttons[BTN_SELECT_CODE     ].SetFlat(TRUE);
		
	InitHoga();
	InitContractProfitGrid();
	InitAccountBalanceGrid();
	InitMarketContract();
	InitOrderPanel();
	InitCurrentInfo();

	CSize sz(0,80);

	AddAnchor(IDC_GRID_HOGA, __RDA_LT, sz);
	AddAnchor(IDC_GRID_MARKET_CONTRACT, __RDA_LT, __RDA_LB);

	//사이즈 변경없이 아래쪽으로 움직일때 따라서 움직인다.
	AddAnchor(IDC_GRID_TOT_HOGA,  sz); 
	AddAnchor(IDC_BUTTON_SELL_MIT_CANCEL, sz); 
	AddAnchor(IDC_BUTTON_SELL_CANCEL,sz); 
	AddAnchor(IDC_BUTTON_ALL_CANCEL, sz); 
	AddAnchor(IDC_BUTTON_BUY_CANCEL,sz); 
	AddAnchor(IDC_BUTTON_BUY_MIT_CANCEL, sz); 
	AddAnchor(IDC_GRID_ORDER_PANEL,  sz, __RDA_LB); 

	AddAnchor(IDC_STATIC_REAL_EVAL, __RDA_LB); 
	AddAnchor(IDC_STATIC_PROFIT_REALIZE, __RDA_LB); 
	AddAnchor(IDC_STATIC_EVAL, __RDA_LB); 
	AddAnchor(IDC_STATIC_PROFIT_EVAL, __RDA_LB); 
	AddAnchor(IDC_GRID_OUTSTANDING_ORDER, __RDA_LB); 
	AddAnchor(IDC_GRID_MIT, __RDA_LB); 

	m_spinVol.SetRange(1, 100);
	m_editVol.SetWindowText(_T("1"));
	
	m_spinLosscut.SetRange(1, 100);
	m_spinEarncut.SetRange(1, 100);
	m_editLosscut.SetWindowText(_T("5"));
	m_editEarncut.SetWindowText(_T("5"));

	m_buttonExpand.SetTooltipText(_T("창을 확장합니다"));
	m_buttonExpand.SetTextColorNormal(Colors::White);

	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(BST_CHECKED);


}

void CFastTradeDlg_DSVIP::InitContractProfitGrid()
{
	
	ASSERT(fld_cp_count == sizeof(g_colsContProfit) / sizeof(GridColumnItem));
	const int COL_COUNT = fld_cp_count;

	WINDOWPLACEMENT wp;
	CRect rt;
	m_gridContractProfit.GetWindowPlacement(&wp);
	wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + 40;
	m_gridContractProfit.SetWindowPlacement(&wp);
	
	m_gridContractProfit.SetDoubleBufferMode(FALSE);
	m_gridContractProfit.SetDefRowHeight(18);
	m_gridContractProfit.m_GI->m_userSizingMode = FALSE;
	m_gridContractProfit.SetCurrentCellMode(4);
	
	delete m_gridContractProfit.m_GI->m_defBorderPen;
	m_gridContractProfit.m_GI->m_defBorderPen = new CPen(PS_SOLID, 1, COLOR_DEFAULT_BACK);
	//cell.SetBorderColor(&m_penGridHeaderBorder, &m_penGridHeaderBorder, &m_penGridHeaderBorder, &m_penGridHeaderBorder);
	
	//////////////////////////
	// Default cell
	CUGCell cell;
	m_gridContractProfit.GetGridDefault(&cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	//cell.SetFont(&m_fontGridBold);
	m_gridContractProfit.SetGridDefault(&cell);

	//////////////////////////
	// Col, Row
	m_gridContractProfit.SetNumberCols(COL_COUNT);
	m_gridContractProfit.SetNumberRows(1);
	m_gridContractProfit.SetTH_NumberRows(1);
	m_gridContractProfit.SetSH_NumberCols(1);
	m_gridContractProfit.SetSH_ColWidth(-1, 0);

	m_gridContractProfit.GetHeadingDefault(&cell);
	cell.SetBackColor(CR_DEF_GRID_HEADER_BG);
	cell.SetTextColor(CR_DEF_GRID_HEADER_TEXT);
	cell.SetFont(GetFont());
	m_gridContractProfit.SetHeadingDefault(&cell);

	m_gridContractProfit.GetClientRect(rt);
	int w = rt.Width();
	int nLeft = 0;
	int nWidth = 0;

	for(int nCol = 0 ; nCol < COL_COUNT ; ++nCol)
	{
		m_gridContractProfit.QuickSetText(nCol, -1, g_colsContProfit[nCol].n);
		if(nCol == COL_COUNT - 1)
		{
			nWidth = w - nLeft;
		}
		else
		{
			nWidth = (int)(w * g_colsContProfit[nCol].w);
		}
		m_gridContractProfit.SetColWidth(nCol, nWidth); 
		nLeft += nWidth;
	}
}

void CFastTradeDlg_DSVIP::InitAccountBalanceGrid()
{
	const int COL_COUNT = sizeof(g_colsAccBal) / sizeof(GridColumnItem);

	WINDOWPLACEMENT wpCP;
	m_gridContractProfit.GetWindowPlacement(&wpCP);

	WINDOWPLACEMENT wp;
	CRect rt;
	m_gridAccountBalance.GetWindowPlacement(&wp);
	wp.rcNormalPosition.top = wpCP.rcNormalPosition.bottom;
	wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + 40;
	m_gridAccountBalance.SetWindowPlacement(&wp);
	
	m_gridAccountBalance.SetDoubleBufferMode(FALSE);
	m_gridAccountBalance.SetDefRowHeight(18);
	m_gridAccountBalance.m_GI->m_userSizingMode = FALSE;
	m_gridAccountBalance.SetCurrentCellMode(4);
	
	//////////////////////////
	// Default cell
	CUGCell cell;
	m_gridAccountBalance.GetGridDefault(&cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	//cell.SetFont(&m_fontGridBold);
	m_gridAccountBalance.SetGridDefault(&cell);

	//////////////////////////
	// Col, Row
	m_gridAccountBalance.SetNumberCols(COL_COUNT);
	m_gridAccountBalance.SetNumberRows(1);
	m_gridAccountBalance.SetTH_NumberRows(1);
	m_gridAccountBalance.SetSH_NumberCols(1);
	m_gridAccountBalance.SetSH_ColWidth(-1, 0);

	m_gridAccountBalance.GetHeadingDefault(&cell);
	cell.SetBackColor(CR_DEF_GRID_HEADER_BG);
	cell.SetTextColor(CR_DEF_GRID_HEADER_TEXT);
	cell.SetFont(GetFont());
	m_gridAccountBalance.SetHeadingDefault(&cell);

	m_gridAccountBalance.GetClientRect(rt);
	int w = rt.Width();
	int nLeft = 0;
	int nWidth = 0;

	for(int nCol = 0 ; nCol < COL_COUNT ; ++nCol)
	{
		m_gridAccountBalance.QuickSetText(nCol, -1, g_colsAccBal[nCol].n);
		if(nCol == COL_COUNT - 1)
		{
			nWidth = w - nLeft;
		}
		else
		{
			nWidth = (int)(w * g_colsAccBal[nCol].w);
		}
		m_gridAccountBalance.SetColWidth(nCol, nWidth); 
		nLeft += nWidth;
	}
}

const int nCurrentInfoRowCount = 7;
void CFastTradeDlg_DSVIP::InitCurrentInfo()
{
	const int nColCount = 1;
	
	const int nRowHeight = 18;


	CRect rt;
	m_gridCurrent.GetClientRect(rt);
	
	WINDOWPLACEMENT wp;
	m_gridCurrent.GetWindowPlacement(&wp);
	wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + (nRowHeight * nCurrentInfoRowCount) + 2;
	m_gridCurrent.SetWindowPlacement(&wp);


	m_gridCurrent.SetDoubleBufferMode(FALSE);
	m_gridCurrent.SetCurrentCellMode(4);
	m_gridCurrent.SetDefRowHeight(18);
	m_gridCurrent.m_GI->m_userSizingMode = FALSE;

	
	//////////////////////////
	// Default cell
	CUGCell cell;
	m_gridCurrent.GetGridDefault(&cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	m_gridCurrent.SetGridDefault(&cell);

	//////////////////////////
	// Col, Row
	m_gridCurrent.SetNumberCols(1);
	m_gridCurrent.SetNumberRows(nCurrentInfoRowCount);
	m_gridCurrent.SetTH_NumberRows(1);
	m_gridCurrent.SetTH_RowHeight(-1, 0);
	m_gridCurrent.SetSH_NumberCols(1);

	m_gridCurrent.GetHeadingDefault(&cell);
	cell.SetBackColor(CR_DEF_GRID_HEADER_BG);
	cell.SetTextColor(CR_DEF_GRID_HEADER_TEXT);
	cell.SetFont(GetFont());
	m_gridCurrent.SetHeadingDefault(&cell);

	int nLeft = (int)(rt.Width() * 0.4);
	m_gridCurrent.SetSH_ColWidth(-1, nLeft);
	m_gridCurrent.SetColWidth(0, rt.Width() - nLeft);

	int nRow = 0;
	m_gridCurrent.QuickSetText(-1, nRow, _T("현재가")); ++nRow;
	m_gridCurrent.QuickSetText(-1, nRow, _T("전일대비")); ++nRow;
	m_gridCurrent.QuickSetText(-1, nRow, _T("시가")); ++nRow;
	m_gridCurrent.QuickSetText(-1, nRow, _T("고가")); ++nRow;
	m_gridCurrent.QuickSetText(-1, nRow, _T("저가")); ++nRow;
	m_gridCurrent.QuickSetText(-1, nRow, _T("상한가")); ++nRow;
	m_gridCurrent.QuickSetText(-1, nRow, _T("하한가")); ++nRow;
}

void CFastTradeDlg_DSVIP::InitHoga()
{
	CUGCell cell;

	m_gridHoga.m_pTradeDlg = this;
	m_gridHoga.Init();

	m_gridHoga.GetHeadingDefault(&cell);
	cell.SetFont(GetFont());
	m_gridHoga.SetHeadingDefault(&cell);
	
	const int nColCount = m_gridHoga.GetNumberCols();

	WINDOWPLACEMENT wp;
	CRect rt;
	m_gridTotHoga.GetWindowPlacement(&wp);
	wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + 20;
	m_gridTotHoga.SetWindowPlacement(&wp);

	m_gridTotHoga.SetDoubleBufferMode(FALSE);
	m_gridTotHoga.SetCurrentCellMode(0);
	m_gridTotHoga.SetDefRowHeight(18);
	m_gridTotHoga.m_GI->m_userSizingMode = FALSE;

	//////////////////////////
	// Default cell
	
	m_gridTotHoga.GetGridDefault(&cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	m_gridTotHoga.SetGridDefault(&cell);
	m_gridTotHoga.SetCurrentCellMode(4);
	//////////////////////////
	// Col, Row
	m_gridTotHoga.SetNumberCols(nColCount);
	m_gridTotHoga.SetNumberRows(1);
	m_gridTotHoga.SetTH_NumberRows(1);
	m_gridTotHoga.SetTH_RowHeight(-1, 0);
	m_gridTotHoga.SetSH_NumberCols(1);
	m_gridTotHoga.SetSH_ColWidth(-1, 0);
	
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	m_gridTotHoga.SetCell(c_price, 0, &cell);

	CRect rtHoga;
	m_gridHoga.GetWindowRect(rtHoga); ScreenToClient(rtHoga);
	int nLeft = rtHoga.left;

	for(int nCol = 0 ; nCol < nColCount ; ++nCol)
	{
		int nWidth = m_gridHoga.GetColWidth(nCol);
		m_gridTotHoga.SetColWidth(nCol, nWidth);
		nLeft += nWidth;
	}
	int nRightHoga = rtHoga.right - m_gridHoga.m_GI->m_vScrollWidth;

	//호가창 상단 버튼
	int nBtnWidth = 0;
	GetDlgItem(IDC_BUTTON_MARKET_SELL)->GetWindowRect(rt); ScreenToClient(rt); nBtnWidth = rt.Width();
	rt.left = rtHoga.left; rt.right = rt.left + nBtnWidth; 
	GetDlgItem(IDC_BUTTON_MARKET_SELL)->MoveWindow(rt);

	GetDlgItem(IDC_BUTTON_ALL_LIQUID)->GetWindowRect(rt); ScreenToClient(rt); nBtnWidth = rt.Width(); 
	rt.left = rtHoga.left + ((nRightHoga - rtHoga.left) / 2) - (nBtnWidth / 2);
	rt.right = rt.left + nBtnWidth;
	GetDlgItem(IDC_BUTTON_ALL_LIQUID)->MoveWindow(rt);

	GetDlgItem(IDC_BUTTON_MARKET_BUY)->GetWindowRect(rt); ScreenToClient(rt); nBtnWidth = rt.Width(); 
	rt.left = nRightHoga - nBtnWidth;
	rt.right = nRightHoga;
	GetDlgItem(IDC_BUTTON_MARKET_BUY)->MoveWindow(rt);

	//호가창 하단의 각 버튼
	nLeft = rtHoga.left;
	GetDlgItem(IDC_BUTTON_SELL_MIT_CANCEL)->GetWindowRect(rt); 
	nBtnWidth = rt.Width();

	ScreenToClient(rt); rt.left = nLeft; rt.right = rt.left + nBtnWidth; GetDlgItem(IDC_BUTTON_SELL_MIT_CANCEL)->MoveWindow(rt);
	nLeft += nBtnWidth + 2;
	rt.left = nLeft; rt.right = nLeft + nBtnWidth; GetDlgItem(IDC_BUTTON_SELL_CANCEL)->MoveWindow(rt);

	int nRight = nRightHoga;
	rt.right = nRight; rt.left = nRight - nBtnWidth; GetDlgItem(IDC_BUTTON_BUY_MIT_CANCEL)->MoveWindow(rt);
	nRight -= (nBtnWidth + 2);
	rt.right = nRight; rt.left = nRight - nBtnWidth; GetDlgItem(IDC_BUTTON_BUY_CANCEL)->MoveWindow(rt);


	GetDlgItem(IDC_BUTTON_ALL_CANCEL)->GetWindowRect(rt); ScreenToClient(rt); nBtnWidth = rt.Width(); 
	rt.left = rtHoga.left + ((nRightHoga - rtHoga.left) / 2) - (nBtnWidth / 2);
	rt.right = rt.left + nBtnWidth;
	GetDlgItem(IDC_BUTTON_ALL_CANCEL)->MoveWindow(rt);


	m_gridTotHoga.QuickSetText(c_mit_sell, 0, _T("0")); m_gridTotHoga.RedrawCell(c_mit_sell, 0);
	m_gridTotHoga.QuickSetText(c_sell, 0, _T("0")); m_gridTotHoga.RedrawCell(c_sell, 0);
	m_gridTotHoga.QuickSetText(c_buy, 0, _T("0"));m_gridTotHoga.RedrawCell(c_buy, 0);
	m_gridTotHoga.QuickSetText(c_mit_buy, 0, _T("0"));m_gridTotHoga.RedrawCell(c_mit_buy, 0);
}

void CFastTradeDlg_DSVIP::InitMarketContract()
{
	const int nColCount = 3;

	m_gridMarketContract.SetDoubleBufferMode(FALSE);
	m_gridMarketContract.SetCurrentCellMode(4);
	m_gridMarketContract.SetDefRowHeight(18);
	m_gridMarketContract.m_GI->m_userSizingMode = FALSE;

	//////////////////////////
	// Default cell
	CUGCell cell;
	m_gridMarketContract.GetGridDefault(&cell);
	cell.SetAlignment(UG_ALIGNVCENTER | UG_ALIGNCENTER);
	m_gridMarketContract.SetGridDefault(&cell);

	//////////////////////////
	// Col, Row
	m_gridMarketContract.SetNumberCols(nColCount);
	m_gridMarketContract.SetNumberRows(GTCurrent::MAX_QUEUE);
	m_gridMarketContract.SetTH_NumberRows(1);
	m_gridMarketContract.SetSH_NumberCols(1);
	m_gridMarketContract.SetSH_ColWidth(-1, 0);
	
	
	//////////////////////////
	// Header
	m_gridMarketContract.GetHeadingDefault(&cell);
	cell.SetBackColor(CR_DEF_GRID_HEADER_BG);
	cell.SetTextColor(CR_DEF_GRID_HEADER_TEXT);
	cell.SetFont(GetFont());
	m_gridMarketContract.SetHeadingDefault(&cell);

	m_gridMarketContract.QuickSetText(0, -1, _T("시간"));
	m_gridMarketContract.QuickSetText(1, -1, _T("체결가"));
	m_gridMarketContract.QuickSetText(2, -1, _T("수량"));

	m_gridMarketContract.GetColDefault(0, &cell);
	cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
	m_gridMarketContract.SetColDefault (0,&cell);

	m_gridMarketContract.GetColDefault(1, &cell);
	cell.SetBackColor(RGB(255,255,204));
	m_gridMarketContract.SetColDefault (1,&cell);
/*
	m_gridMarketContract.GetColDefault(2, &cell);
	cell.SetBackColor(RGB(245,234,218));
	m_gridMarketContract.SetColDefault (2,&cell);*/

	CRect rt;
	m_gridMarketContract.GetClientRect(rt);
	int w = rt.Width() - m_gridMarketContract.m_GI->m_vScrollWidth;
	int nLeft = 0;
	int nCol = 0;
	int nWidth = (int)(w * 0.40);
	m_gridMarketContract.SetColWidth(nCol++, nWidth); nLeft += nWidth;
	nWidth = (int)(w * 0.35);
	m_gridMarketContract.SetColWidth(nCol++, nWidth); nLeft += nWidth;
	nWidth = w - nLeft - 2;
	m_gridMarketContract.SetColWidth(nCol++, nWidth);
}

void CFastTradeDlg_DSVIP::InitOrderPanel()
{
	const int COL_COUNT = sizeof(g_colsOP) / sizeof(GridColumnItem);

	m_gridOrderPanel.SetDoubleBufferMode(FALSE);
	m_gridOrderPanel.SetDefRowHeight(18);
	m_gridOrderPanel.SetCurrentCellMode(4);
	m_gridOrderPanel.m_GI->m_lockRowHeight = TRUE;
	m_gridOrderPanel.m_GI->m_highlightRowFlag = TRUE; 
	m_gridOrderPanel.m_GI->m_currentCellMode = 1; 

	CUGCell cell;
	m_gridOrderPanel.GetGridDefault(&cell);
	cell.SetAlignment(UG_ALIGNVCENTER);
	m_gridOrderPanel.SetGridDefault(&cell);
		
	m_gridOrderPanel.SetNumberCols(COL_COUNT);
	m_gridOrderPanel.SetNumberRows(0);
	m_gridOrderPanel.SetTH_NumberRows(1);
	m_gridOrderPanel.SetSH_NumberCols(1);
	m_gridOrderPanel.SetSH_ColWidth(-1, 0);
	

	m_gridOrderPanel.GetColDefault(COL_OP_PRICE, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	m_gridOrderPanel.SetColDefault(COL_OP_PRICE, &cell);
	
	m_gridOrderPanel.GetColDefault(COL_OP_VOL, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	m_gridOrderPanel.SetColDefault(COL_OP_VOL, &cell);

	m_gridOrderPanel.GetColDefault(COL_OP_EVAL, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	cell.SetFont(&m_fontGridBold);
	m_gridOrderPanel.SetColDefault(COL_OP_EVAL, &cell);
	
	// Header색
	m_gridOrderPanel.GetHeadingDefault(&cell);
	cell.SetBackColor(CR_DEF_GRID_HEADER_BG);
	cell.SetTextColor(CR_DEF_GRID_HEADER_TEXT);
	cell.SetFont(GetFont());
	m_gridOrderPanel.SetHeadingDefault(&cell);

	CRect rt;
	m_gridOrderPanel.GetClientRect(rt);
	int w = rt.Width() - m_gridOrderPanel.m_GI->m_vScrollWidth;
	int nLeft = 0;
	int nWidth = 0;

	for(int nCol = 0 ; nCol < COL_COUNT ; ++nCol)
	{
		m_gridOrderPanel.QuickSetText(nCol, -1, g_colsOP[nCol].n);
		if(nCol == COL_COUNT - 1)
		{
			nWidth = w - nLeft;
		}
		else
		{
			nWidth = (int)(w * g_colsOP[nCol].w);
		}
		m_gridOrderPanel.SetColWidth(nCol, nWidth); 
		nLeft += nWidth;
	}
}

struct OrderedCount
{
	GT_VOLUME	nBuyCont;
	GT_VOLUME	nSellCont;
	GT_VOLUME	nBuyOutstd;
	GT_VOLUME	nSellOutstd;
};


void CFastTradeDlg::SetEnableOrderCount()
{
	if(NULL == m_pSelCode)
		return;

	const OrderContracts& conts = SER().GetContracts();
	const OrderOutstandings& outstds = SER().GetOutstandings();

	GT_VOLUME nBuyMaxContract = 0;
	GT_VOLUME nSellMaxContract = 0;

	if(GetSession()->maxDepositFrom0)
	{
		if(ct_futures == m_curMerchandiseType)
		{
			nBuyMaxContract = nSellMaxContract = GetSession()->futuresMaxContract;
		}
		else if(ct_foreign_futures == m_curMerchandiseType)
		{
			nBuyMaxContract = nSellMaxContract = GetSession()->foreignFuturesMaxContract;
		}
		else if(ct_options == m_curMerchandiseType)
		{
			nBuyMaxContract = GetSession()->optionsBuyMaxContract;
			nSellMaxContract = GetSession()->optionsSellMaxContract;
		}

		GT_VOLUME	nDestBuyCont = 0;
		GT_VOLUME	nDestBuyContMulti = 0; //옵션매수 3.0 이상일때는 5를 곱해야 하기 때문에 따로 수량을 기억한다.
		GT_VOLUME	nDestBuyOutstdMulti = 0;

		GT_VOLUME	nDestSellCont = 0;
		GT_VOLUME	nDestSellContMulti = 0;
		GT_VOLUME	nDestSellOutstd = 0;

		GT_VOLUME	nOtherBuyContMulti = 0;
		GT_VOLUME	nOtherBuyOutstdMulti = 0;

		GT_VOLUME	nOtherSellCont = 0;
		GT_VOLUME	nOtherSellOutstd = 0;

	
		set<MerchandiseType> merchTypes;
		for(auto pos = outstds.begin() ; pos != outstds.end() ; ++pos)
		{
			OrderOutstanding* pOrder = (*pos);

			if(m_pSelCode == pOrder->pCode)
			{
				const GT_VOLUME nVol = pOrder->nVolumeRemain;

				if(pOrder->position == PT_BUY)
				{
					if(pOrder->pCode->GetType() == ct_options && pOrder->dPrice > OPTIONS_BUY_MULTIPLE_POINT)
						nDestBuyOutstdMulti += (OPTIONS_BUY_MORE_THAN_3POINT_MULTIPLE * nVol);
					else
						nDestBuyOutstdMulti += nVol;
				}
				else
					nDestSellOutstd += nVol;
			}
			else if(m_pSelCode->GetType() == pOrder->pCode->GetType())
			{
				GT_VOLUME nVolInPos = 0;
				for(auto pos = conts.begin() ; pos != conts.end() ; ++pos)
				{
					OrderContracted* pCont = (*pos).get();
					if(pCont->position != pOrder->position && pCont->pCode == pOrder->pCode)
					{
						// posOut이 청산을 위한 미체결이라면
						nVolInPos = pCont->nVolume;
						break;
					}
				}

				GT_VOLUME nVol =  pOrder->nVolumeRemain;

				if(nVolInPos >= nVol)
					nVol = 0;
				else
					nVol -= nVolInPos;

				if(pOrder->position == PT_BUY)
				{
					if(pOrder->pCode->GetType() == ct_options && pOrder->dPrice > OPTIONS_BUY_MULTIPLE_POINT)
						nOtherBuyOutstdMulti += (OPTIONS_BUY_MORE_THAN_3POINT_MULTIPLE * nVol);
					else
						nOtherBuyOutstdMulti += nVol;
				}
				else
					nOtherSellOutstd += nVol;
			} 
		}
		
		for(auto pos = conts.begin() ; pos != conts.end() ; ++pos)
		{
			OrderContracted* pOrder = (*pos).get();
			merchTypes.insert(pOrder->pCode->GetType());
			//ASSERT(pOrder->dPrice.is_zero() == false);
			if(m_pSelCode == pOrder->pCode)
			{
				if(pOrder->position == PT_BUY)
				{
					nDestBuyCont += pOrder->nVolume;

					if(pOrder->pCode->GetType() == ct_options && pOrder->dPrice > OPTIONS_BUY_MULTIPLE_POINT)
						nDestBuyContMulti += (OPTIONS_BUY_MORE_THAN_3POINT_MULTIPLE * pOrder->nVolume);
					else
						nDestBuyContMulti += pOrder->nVolume;
				}
				else
				{
					nDestSellCont += pOrder->nVolume;
					if(pOrder->pCode->GetType() == ct_options && pOrder->dPrice > OPTIONS_BUY_MULTIPLE_POINT)
						nDestSellContMulti += (OPTIONS_BUY_MORE_THAN_3POINT_MULTIPLE * pOrder->nVolume);
					else
						nDestSellContMulti += pOrder->nVolume;
				}
			}
			else if(m_pSelCode->GetType() == pOrder->pCode->GetType())
			{
				if(pOrder->position == PT_BUY)
				{
					if(pOrder->pCode->GetType() == ct_options && pOrder->dPrice > OPTIONS_BUY_MULTIPLE_POINT)
						nOtherBuyContMulti += (OPTIONS_BUY_MORE_THAN_3POINT_MULTIPLE * pOrder->nVolume);
					else
						nOtherBuyContMulti += pOrder->nVolume;
				}
				else
					nOtherSellCont += pOrder->nVolume;
			} 
		}

		nBuyMaxContract = nBuyMaxContract 
						- nDestBuyContMulti 
						- nDestBuyOutstdMulti 
						- nOtherBuyContMulti 
						- nOtherBuyOutstdMulti 
						+ nDestSellContMulti
						;

		nSellMaxContract = nSellMaxContract 
						- nDestSellCont 
						- nDestSellOutstd 
						- nOtherSellCont 
						- nOtherSellOutstd
						+ nDestBuyCont
						;

		if(m_curMerchandiseType == ct_options)
		{
			DK_DECIMAL dPrice = m_pMerch->GetCurrent()->price.ToDecimal();
			if(dPrice > 5.0)
			{
				nBuyMaxContract = 0;
				nSellMaxContract = 0;
			}
			else if(dPrice > OPTIONS_BUY_MULTIPLE_POINT)
			{
				nBuyMaxContract = nBuyMaxContract/OPTIONS_BUY_MORE_THAN_3POINT_MULTIPLE; //3.0이상일때는 20%로 줄인다.
			}
		}

		if(merchTypes.find(m_curMerchandiseType) == merchTypes.end())
		{
			//현재 보유포지션에 없는 상품을 주문할때 개시증거금이 만족하는지
			GT_CURRENCY minDeposit = 0;
			for(auto pos = merchTypes.begin() ; pos != merchTypes.end() ; ++pos)
			{
				minDeposit += GetMinimumDeposit(*pos);
			}
			minDeposit += GetMinimumDeposit(m_curMerchandiseType);

			if(minDeposit > GetSession()->maxDepositFrom0)
			{
				nBuyMaxContract = 0;
				nSellMaxContract = 0;
			}
		}
	}
	CString s;
	s.Format(_T("%d "), nSellMaxContract);
	m_labels[LBL_ENABLE_SELL].SetWindowText(s);
	s.Format(_T("%d "), nBuyMaxContract);
	m_labels[LBL_ENABLE_BUY].SetWindowText(s);
}


LRESULT CFastTradeDlg_DSVIP::OnEnableOrderCountReceived(WPARAM wParam, LPARAM lParam)
{
	EanbleOrderItem* pItem = (EanbleOrderItem*)wParam;
	delete pItem;
	return 0;
}

void CFastTradeDlg_DSVIP::OnBnClickedSelectCode()
{
	BOOL bDeleteOptionsTable = TRUE;
	Code* pCode = NULL;
	if(ct_futures == m_curMerchandiseType)
	{
	}
	else if(ct_options == m_curMerchandiseType)
	{
		bDeleteOptionsTable = FALSE;
		if(NULL == m_pOptionsTableDlg)
		{
			m_pOptionsTableDlg = new COptionsTableDlg(this, mt_EUREX == m_curMarketType);
			m_pOptionsTableDlg->m_pOwner = this;
			m_pOptionsTableDlg->Create(COptionsTableDlg::IDD, this);
		}
		m_pOptionsTableDlg->ShowWindow(SW_SHOW);
	}
	else if(ct_foreign_futures == m_curMerchandiseType)
	{
		if(NULL == m_pForeignFuturesCodeDlg)
		{
			m_pForeignFuturesCodeDlg = new CForeignFuturesCodeDlg(this);
			m_pForeignFuturesCodeDlg->m_pOwner = this;
			m_pForeignFuturesCodeDlg->Create(CForeignFuturesCodeDlg::IDD, this);
		}
		m_pForeignFuturesCodeDlg->ShowWindow(SW_SHOW);
	}

	if(NULL == pCode)
		return;

	SetMerchandise(pCode, m_curMarketType);
}


void CFastTradeDlg_DSVIP::OnCurrentInitUpdated(GTCurrent* p)
{
	if(m_bCurrentInit)
		return;

	m_pAutoOrder->InitHoga(p);
	m_gridHoga.InitHoga(p);

	//m_gridCurrent.QuickSetText(-1, nRow, _T("현재가")); ++nRow;
	//m_gridCurrent.QuickSetText(-1, nRow, _T("전일대비")); ++nRow;
	//m_gridCurrent.QuickSetText(-1, nRow, _T("시가")); ++nRow;
	//m_gridCurrent.QuickSetText(-1, nRow, _T("고가")); ++nRow;
	//m_gridCurrent.QuickSetText(-1, nRow, _T("저가")); ++nRow;
	//m_gridCurrent.QuickSetText(-1, nRow, _T("상한가")); ++nRow;
	//m_gridCurrent.QuickSetText(-1, nRow, _T("하한가")); ++nRow;

	m_gridCurrent.QuickSetTextColor(0, 5, Colors::Red);
	m_gridCurrent.QuickSetText(0, 5, p->uplmtprice);
	m_gridCurrent.QuickSetTextColor(0, 6, Colors::Blue);
	m_gridCurrent.QuickSetText(0, 6, p->dnlmtprice);

	m_gridCurrent.RedrawCell(0 ,5);
	m_gridCurrent.RedrawCell(0, 6);


	const OrderContracts& conts = SER().GetContracts();
	for(auto pos = conts.begin() ; pos != conts.end() ; ++pos)
	{
		OrderContracted* pOrder = (*pos).get();
		if(pOrder->pCode == p->GetCode())
		{
			m_pSelContractCode = pOrder->pCode;
			m_pCurContract = pOrder;
			break;
		}
	}
	
	SetEnableOrderCount();
	UpdateGridAccountPanel(ap_contract);
	OnCurrentUpdated(p);
	MITTotalUpdate();
	m_bCurrentInit = TRUE;

	m_pMerch->RunHogaRecv();
}

void CFastTradeDlg_DSVIP::UpdateGridAccountPanel(int nCase, OrderContracted* pOrderUpdated)
{
	// 0:체결됐을때, 1:손익 바뀌었을때, 2:계좌 잔고 바뀌었을때
	const OrderContracts& conts = SER().GetContracts();
	Profit* pProfit = SER().GetProfit();

	if(nCase & ap_contract)
	{
		
		if(conts.empty() || (m_pSelContractCode != m_pSelCode))
		{
			m_gridContractProfit.QuickSetText(fld_cp_ContratedVolume, 0, _T(""));
			m_gridContractProfit.QuickSetText(fld_cp_PriceAverage, 0,_T(""));
			m_gridContractProfit.QuickSetText(fld_cp_EvalProfit, 0,_T(""));

			m_gridContractProfit.RedrawCell(fld_cp_ContratedVolume, 0);
			m_gridContractProfit.RedrawCell(fld_cp_PriceAverage, 0);
			m_gridContractProfit.RedrawCell(fld_cp_EvalProfit, 0);
		}
		else
		{
			for(auto pos = conts.begin() ; pos != conts.end() ; ++pos)
			{
				OrderContracted* pOrder = (*pos).get();
				if(pOrder->pCode == m_pSelContractCode)
				{
					CString s;
					s.Format(_T("%s %d"), ::PositionTypeStr(pOrder->position), pOrder->nVolume);
					m_gridContractProfit.QuickSetText(fld_cp_ContratedVolume, 0, s);
					m_gridContractProfit.QuickSetTextColor(fld_cp_ContratedVolume, 0, pOrder->position == PT_SELL ? Colors::Blue : Colors::Red);

					m_gridContractProfit.QuickSetText(fld_cp_PriceAverage, 0, pOrder->strPrice);

					m_gridContractProfit.QuickSetText(fld_cp_EvalProfit, 0, ::ToThousand(pOrder->profitEval));
					m_gridContractProfit.QuickSetTextColor(fld_cp_EvalProfit, 0, ::GetProfitColor(pOrder->profitEval));

					m_gridContractProfit.RedrawCell(fld_cp_ContratedVolume, 0);
					m_gridContractProfit.RedrawCell(fld_cp_PriceAverage, 0);
					m_gridContractProfit.RedrawCell(fld_cp_EvalProfit, 0);
					break;
				}
			}
		}
		if(pProfit)
		{
			//평가손익합
			m_gridContractProfit.QuickSetText(fld_cp_EvalProfitSum, 0, ::ToThousand(pProfit->profitEvalSum));
			m_gridContractProfit.QuickSetTextColor(fld_cp_EvalProfitSum, 0, ::GetProfitColor(pProfit->profitEvalSum));
			m_gridContractProfit.RedrawCell(fld_cp_EvalProfitSum, 0);
			// 실시간손익합
			GT_CURRENCY realTimeProfitTot = pProfit->profitRealize + pProfit->profitEvalSum;
			m_gridContractProfit.QuickSetText(fld_cp_RealtimeProfitTot, 0, ::ToThousand(realTimeProfitTot));
			m_gridContractProfit.QuickSetTextColor(fld_cp_RealtimeProfitTot, 0, ::GetProfitColor(realTimeProfitTot));
			m_gridContractProfit.RedrawCell(fld_cp_RealtimeProfitTot, 0);

			//로스컷
			m_gridAccountBalance.QuickSetText(1, 0, ::ToThousand(pProfit->lossCut));
			m_gridAccountBalance.RedrawCell(1, 0);

			//평가담보금
			GT_CURRENCY accountBalanceEval = GetSession()->get_bankBalance() + pProfit->profitEvalSum;
			m_gridAccountBalance.QuickSetText(0, 0, ::ToThousand(accountBalanceEval));
			m_gridAccountBalance.RedrawCell(0, 0);

			m_gridAccountBalance.QuickSetText(2, 0, ::ToThousand(pProfit->depositForFuturesOvernightTot));
			m_gridAccountBalance.RedrawCell(2, 0);

			m_gridAccountBalance.QuickSetText(3, 0, ::ToThousand(pProfit->depositForOptionsOvernightTot));
			m_gridAccountBalance.RedrawCell(3, 0);

			m_gridAccountBalance.QuickSetText(4, 0, ::ToThousand(pProfit->depositForFuturesOvernightTot + pProfit->depositForOptionsOvernightTot));
			m_gridAccountBalance.RedrawCell(4, 0);
		}
		else
		{
			m_gridContractProfit.QuickSetText(fld_cp_EvalProfitSum, 0, _T(""));
			m_gridContractProfit.QuickSetTextColor(fld_cp_EvalProfitSum, 0, Colors::Black);
			m_gridContractProfit.RedrawCell(fld_cp_EvalProfitSum, 0);
		}

	}
	
	if(nCase & ap_profit)
	{
		ASSERT(pOrderUpdated);
		ASSERT(pProfit);
		//평가손익합
		m_gridContractProfit.QuickSetText(fld_cp_EvalProfitSum, 0, ::ToThousand(pProfit->profitEvalSum));
		m_gridContractProfit.QuickSetTextColor(fld_cp_EvalProfitSum, 0, ::GetProfitColor(pProfit->profitEvalSum));
		m_gridContractProfit.RedrawCell(fld_cp_EvalProfitSum, 0);
		//실시간손익합
		GT_CURRENCY realTimeProfitTot = pProfit->profitRealize + pProfit->profitEvalSum;
		m_gridContractProfit.QuickSetText(fld_cp_RealtimeProfitTot, 0, ::ToThousand(realTimeProfitTot));
		m_gridContractProfit.QuickSetTextColor(fld_cp_RealtimeProfitTot, 0, ::GetProfitColor(realTimeProfitTot));
		m_gridContractProfit.RedrawCell(fld_cp_RealtimeProfitTot, 0);

		//평가담보금
		GT_CURRENCY accountBalanceEval = GetSession()->get_bankBalance() + pProfit->profitEvalSum;
		m_gridAccountBalance.QuickSetText(0, 0, ::ToThousand(accountBalanceEval));
		m_gridAccountBalance.RedrawCell(0, 0);

		m_gridAccountBalance.QuickSetText(2, 0, ::ToThousand(pProfit->depositForFuturesOvernightTot));
		m_gridAccountBalance.RedrawCell(2, 0);

		m_gridAccountBalance.QuickSetText(3, 0, ::ToThousand(pProfit->depositForOptionsOvernightTot));
		m_gridAccountBalance.RedrawCell(3, 0);

		m_gridAccountBalance.QuickSetText(4, 0, ::ToThousand(pProfit->depositForFuturesOvernightTot + pProfit->depositForOptionsOvernightTot));
		m_gridAccountBalance.RedrawCell(4, 0);
		if(m_pSelContractCode == pOrderUpdated->pCode)
		{
			m_gridContractProfit.QuickSetText(fld_cp_EvalProfit, 0, ::ToThousand(pOrderUpdated->profitEval));
			m_gridContractProfit.QuickSetTextColor(fld_cp_EvalProfit, 0, ::GetProfitColor(pOrderUpdated->profitEval));
			m_gridContractProfit.RedrawCell(fld_cp_EvalProfit, 0);
		}
	}
	
	if(nCase & ap_balance)
	{
		GT_CURRENCY accountBalanceEval = GetSession()->get_bankBalance();
		if(pProfit)
		{
			//ASSERT(pProfit->profitRealize == GetSession()->todayProfitRealized);
			m_gridAccountBalance.QuickSetText(1, 0, ::ToThousand(pProfit->lossCut));
			m_gridAccountBalance.RedrawCell(1, 0);

			accountBalanceEval += pProfit->profitEvalSum;
		}

		m_gridContractProfit.QuickSetText(fld_cp_RealizedProfit, 0, ::ToThousand(GetSession()->todayProfitRealized));
		m_gridContractProfit.QuickSetTextColor(fld_cp_RealizedProfit, 0, ::GetProfitColor(GetSession()->todayProfitRealized));
		m_gridContractProfit.RedrawCell(fld_cp_RealizedProfit, 0);

		m_gridAccountBalance.QuickSetText(0, 0, ::ToThousand(accountBalanceEval));
		m_gridAccountBalance.RedrawCell(0, 0);
	}

	if(pProfit)
	{

	}
}

void CFastTradeDlg_DSVIP::OnAutoOrderContractChanged(int nRowOld, int nRowNew)
{
	
}

void CFastTradeDlg_DSVIP::OnAutoOrderSetLossCut(BOOL bEnable, int nTick, int nRowOld, int nRowNew)
{
	int nCheck = bEnable ? BST_CHECKED : BST_UNCHECKED;
	GetDlgItem(IDC_EDIT_LOSSCUT)->EnableWindow(!bEnable);
	GetDlgItem(IDC_SPIN_LOSSCUT)->EnableWindow(!bEnable);
	SetDlgItemText(IDC_EDIT_LOSSCUT, ::ToString(nTick));
	((CButton*)GetDlgItem(IDC_CHECK_LOSSCUT))->SetCheck(nCheck);
}

void CFastTradeDlg_DSVIP::OnAutoOrderSetEarnCut(BOOL bEnable, int nTick, int nRowOld, int nRowNew)
{
	int nCheck = bEnable ? BST_CHECKED : BST_UNCHECKED;
	GetDlgItem(IDC_EDIT_EARNCUT)->EnableWindow(!bEnable);
	GetDlgItem(IDC_SPIN_EARNCUT)->EnableWindow(!bEnable);
	SetDlgItemText(IDC_EDIT_EARNCUT, ::ToString(nTick));
	((CButton*)GetDlgItem(IDC_CHECK_EARNCUT))->SetCheck(nCheck);
}

//void HogaGrid::OnMITOrderUpdated(PositionType position, const CString& price, const CString& volume)
void CFastTradeDlg_DSVIP::OnAutoOrderMITChanged(PositionType posType, const CString& strPrice, GT_VOLUME volume)
{
	MITTotalUpdate();
}

void CFastTradeDlg_DSVIP::OnAutoOrderManagerOnMITUpdated(const AutoOrders& autoOrders)
{
	UpdateOrderPanel();
}

void CFastTradeDlg_DSVIP::UpdateMarketContracts(GTCurrent* p)
{
	int nRow = 0;
	for(auto pos = p->m_listContracts.rbegin() ; pos != p->m_listContracts.rend() ; ++pos, ++nRow)
	{
		GTCurrent::ContractItem* pItem = (*pos).get();

		m_gridMarketContract.QuickSetText(0, nRow, pItem->time);

		m_gridMarketContract.QuickSetTextColor(2, nRow, pItem->pos == PT_BUY ? Colors::Red : Colors::Blue);
		m_gridMarketContract.QuickSetText(1, nRow, pItem->price);

		m_gridMarketContract.QuickSetTextColor(2, nRow, pItem->pos == PT_BUY ? Colors::Red : Colors::Blue);
		m_gridMarketContract.QuickSetText(2, nRow, pItem->volume);
	}
	m_gridMarketContract.RedrawAll();
}

void CFastTradeDlg_DSVIP::OnCurrentUpdated(GTCurrent* p)
{
	m_gridHoga.OnCurrentUpdated(p);
	//CFuturesOptionsCurrentCommon* p = dynamic_cast<CFuturesOptionsCurrentCommon*>(p);

	UpdateMarketContracts(p);

	CompareStateItem compItem = CompareStateColorFromXing(p->sign.ToString());
	COLORREF cr = get<0>(compItem);

	m_gridCurrent.QuickSetText(0, 0, p->price.ToString());
	m_gridCurrent.QuickSetTextColor(0, 0, cr);

	CString s;
	s.Format(_T("%s %s(%s%%)"), get<2>(compItem), p->change.ToString(), p->drate.ToString());
	m_gridCurrent.QuickSetText(0, 1, s);
	m_gridCurrent.QuickSetTextColor(0, 1, cr);
		
	m_gridCurrent.QuickSetTextColor(0, 2, p->open > p->jnilclose ? Colors::Red : Colors::Blue);
	m_gridCurrent.QuickSetText(0, 2, p->open);

	m_gridCurrent.QuickSetTextColor(0, 3, p->high > p->jnilclose ? Colors::Red : Colors::Blue);
	m_gridCurrent.QuickSetText(0, 3, p->high);

	m_gridCurrent.QuickSetTextColor(0, 4, p->low> p->jnilclose ? Colors::Red : Colors::Blue);
	m_gridCurrent.QuickSetText(0, 4, p->low);

	m_gridCurrent.RedrawCell(0, 0);
	m_gridCurrent.RedrawCell(0, 1);
	m_gridCurrent.RedrawCell(0, 2);
	m_gridCurrent.RedrawCell(0, 3);
	m_gridCurrent.RedrawCell(0, 4);
}

void CFastTradeDlg_DSVIP::OnHogaInitUpdated(GTHoga* p)
{
	if(m_bHogaInit)
	{
		return;
	}

	m_gridHoga.OnHogaInitUpdated(p);

	m_bHogaInit = TRUE;

	UpdateTotalHoga(p);
	ContractApplyToHoga();
	OutstandingApplyToHoga();
}

void CFastTradeDlg_DSVIP::OnHogaUpdated(GTHoga* p)
{
	if(FALSE == m_bHogaInit)
	{
		return;
	}

	m_gridHoga.OnHogaUpdated(p);
	UpdateTotalHoga(p);	
}

void CFastTradeDlg_DSVIP::UpdateTotalHoga(GTHoga* p)
{
	CString s;
	if(mt_EUREX == m_curMarketType)
		s = p->totofferrem.ToString();
	else
		s.Format(_T("(%4s)%4s"), p->totoffercnt.ToString(), p->totofferrem.ToString());
	m_gridTotHoga.QuickSetText(c_sell_remain, 0, s);

	if(mt_EUREX == m_curMarketType)
		s = p->totbidrem.ToString();
	else
		s.Format(_T("%-4s(%-4s)"), p->totbidrem.ToString(), p->totbidcnt.ToString());

	m_gridTotHoga.QuickSetText(c_buy_remain, 0, s);

	int nSub = p->totbidrem.ToINT() - p->totofferrem.ToINT();
	m_gridTotHoga.QuickSetTextColor(c_price, 0, ::GetProfitColor(nSub));
	m_gridTotHoga.QuickSetText(c_price, 0, ::ToThousand(abs(nSub)));
	m_gridTotHoga.RedrawRow(0);
}

void CFastTradeDlg_DSVIP::OnClose()
{
	ASSERT(FALSE);
	__super::OnClose();
}

CString CFastTradeDlg_DSVIP::GetCurVolume()
{
	CString s;
	m_editVol.GetWindowText(s);
	return s;
}

void CFastTradeDlg_DSVIP::OnHogaClickOrder(int col, int row, const CString& price)
{
	CString strVolume = GetCurVolume();

	if(c_buy == col || c_sell == col)
	{
		Order(c_sell == col ? PT_SELL : PT_BUY, ht_JiJeongGa, price, strVolume, 
			m_bOneClick ? om_click : om_dbl_click);
	}
	else if(c_mit_buy == col || c_mit_sell == col)
	{
		PositionType posType = c_mit_sell == col ? PT_SELL : PT_BUY;
		CString strMsg;
		if(m_pAutoOrder->MIT_New(posType, price, ::ToINT(strVolume), strMsg))
		{
			ORDER_ACTOR actor;
			CString strAction;
			strAction.Format(_T("%s: MIT신규"),OrderMethodStr(m_bOneClick ? om_click : om_dbl_click, actor));
			GetSession()->SendOrderAction(strAction, m_pSelCode->GetCode(), price, posType, strVolume);
		}
		else
		{
			ErrMsgBox(strMsg);
		}
	}
}

HHOOK g_hhookCBTProc = 0;
CPoint g_ptMsgBox;
LRESULT CALLBACK pfnCBTMsgBoxHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HCBT_CREATEWND)
	{
		CREATESTRUCT *pcs = ((CBT_CREATEWND *)lParam)->lpcs;

		if ((pcs->style & WS_DLGFRAME) || (pcs->style & WS_POPUP))
		{
			HWND hwnd = (HWND)wParam;

			// At this point you have the hwnd of the newly created 
			// message box that so you can position it at will
			SetWindowPos(hwnd, NULL, g_ptMsgBox.x, g_ptMsgBox.y, 500,500, SWP_NOZORDER);
		}
	}

	return (CallNextHookEx(g_hhookCBTProc, nCode, wParam, lParam));
}


BOOL CFastTradeDlg_DSVIP::_Order(Code* pCode, MarketType marketType, PositionType posType, HogaType hoga, 
						   const CString& price, const CString& vol, OrderMethod method)
{
#ifdef _SHOW_CONFIRM_MSG
	if(ENV().Get(ENV_ORDER_CONFIRM_SHOW).ToINT() == TRUE && 
		(method != om_mit && method != om_stop_losscut && method != om_stop_earncut))
	{
		CString str;
		str.Format(_T("%s(%s) %s 신규주문: 가격(%s), 수량(%s)\n주문 하시겠습니까?"), 
			::MerchandiseTypeStr(pCode->GetType()), pCode->GetCode(), ::PositionTypeStr(posType),
			price, vol);

		CRect rtWnd;
		GetParent()->GetWindowRect(rtWnd);
		g_ptMsgBox.x = rtWnd.right + 5;
		g_ptMsgBox.y = rtWnd.top + rtWnd.Height() - 30;

		g_hhookCBTProc = SetWindowsHookEx(WH_CBT, 
                                pfnCBTMsgBoxHook, 
                                0, GetCurrentThreadId());

		int nRet = ::MessageBox(GetSafeHwnd(), str, _T("신규주문"), MB_ICONQUESTION|MB_OKCANCEL);
		UnhookWindowsHookEx(g_hhookCBTProc);

		if(nRet != IDOK)
			return FALSE;
	}
#endif

	ORDER_ACTOR actor;
	CString strAction;
	strAction.Format(_T("%s: 신규주문[%s]"), ::OrderMethodStr(method, actor), ::HogaTypeStr(hoga));

	GetSession()->SendOrderAction(strAction, pCode->GetCode(), price, posType, vol);

	
	BOOL bRet = GetSession()->Order(GetSafeHwnd(), pCode, marketType, posType, hoga, price, vol, actor);
	return bRet;
}

BOOL CFastTradeDlg_DSVIP::Order(PositionType posType, HogaType hoga, const CString& price, const CString& vol, OrderMethod method)
{
	ASSERT(m_pSelCode);
	return _Order(m_pSelCode, m_curMarketType, posType, hoga, price, vol, method);
}


BOOL CFastTradeDlg_DSVIP::OrderCorrection(PositionType posType, CString srcPrice, CString destPrice, CString vol, OrderMethod method)
{
	const OrderOutstandings& outstds = SER().GetOutstandings();
#ifdef _SHOW_CONFIRM_MSG
	if(ENV().Get(ENV_ORDER_CONFIRM_SHOW).ToINT() == TRUE)
	{
		CString str;
		str.Format(_T("%s(%s) %s정정주문: 수량(%s) 원주문가격(%s), 정정주문가격(%s)\n주문 하시겠습니까?"), 
			::MerchandiseTypeStr(m_pSelCode->GetType()), m_pSelCode->GetCode(), ::PositionTypeStr(posType),
			vol, srcPrice, destPrice);
		
		int nRet = MessageBox(str, _T("정정주문"), MB_ICONQUESTION|MB_OKCANCEL);
		UnhookWindowsHookEx(g_hhookCBTProc);
		if(nRet != IDOK)
			return FALSE;
	}
#endif

	GT_VOLUME nTotVol = 0;
	vector<pair<int, GT_VOLUME> > ordNums;
	for(auto pos = outstds.begin() ; pos != outstds.end() ; ++pos)
	{
		OrderOutstanding*	pOrder = *pos;
		if(pOrder->position == posType && pOrder->strPrice == srcPrice)
		{
			nTotVol += pOrder->nVolumeRemain;
			ordNums.push_back(make_pair(pOrder->nOrderNum, pOrder->nVolumeRemain));
		}
	}
	
	ORDER_ACTOR actor;
	CString strAction;
	strAction.Format(_T("%s: 정정주문"), ::OrderMethodStr(method, actor));
	CString strMethod = ::OrderMethodStr(method, actor);
	GetSession()->SendOrderAction(strAction, m_pSelCode->GetCode(), destPrice, posType, vol);

	for(UINT i = 0 ; i < ordNums.size() ; ++i)
	{
		if(!GetSession()->OrderCorrection(GetSafeHwnd(), ordNums[i].first, ht_JiJeongGa, destPrice, ::ToString(ordNums[i].second)))
			return FALSE;
	}

	ASSERT(vol == ::ToString(nTotVol));
	return TRUE;
}
BOOL CFastTradeDlg_DSVIP::OrderCancel(PositionType posType, CString srcPrice, CString vol, OrderMethod method)
{
	const OrderOutstandings& outstds = SER().GetOutstandings();
#ifdef _SHOW_CONFIRM_MSG
	if(ENV().Get(ENV_ORDER_CONFIRM_SHOW).ToINT() == TRUE)
	{
		CString str;
		str.Format(_T("%s(%s) %s취소주문: 수량(%s) 가격(%s)\n주문을 취소하시겠습니까?"), 
			::MerchandiseTypeStr(m_pSelCode->GetType()), m_pSelCode->GetCode(), ::PositionTypeStr(posType),
			vol, srcPrice);

		if(MessageBox(str, _T("취소주문"), MB_ICONQUESTION|MB_OKCANCEL) != IDOK)
			return FALSE;
	}
#endif

	GT_VOLUME nTotVol = 0;
	vector<pair<int, GT_VOLUME> > ordNums;
	for(auto pos = outstds.begin() ; pos != outstds.end() ; ++pos)
	{
		OrderOutstanding*	pOrder = *pos;
		if(pOrder->position == posType && pOrder->strPrice == srcPrice)
		{
			nTotVol += pOrder->nVolumeRemain;
			ordNums.push_back(make_pair(pOrder->nOrderNum, pOrder->nVolumeRemain));
		}
	}

	ORDER_ACTOR actor;
	CString strAction;
	strAction.Format(_T("%s: 취소주문"), ::OrderMethodStr(method, actor));
	GetSession()->SendOrderAction(strAction, m_pSelCode->GetCode(), srcPrice, posType,  vol);
	
	for(UINT i = 0 ; i < ordNums.size() ; ++i)
	{
		if(!GetSession()->OrderCancel(GetSafeHwnd(), ordNums[i].first, ::ToString(ordNums[i].second)))
			return FALSE;
	}

	ASSERT(vol == ::ToString(nTotVol));
	return TRUE;
}

LRESULT CFastTradeDlg_DSVIP::OnOrderOutstanding(WPARAM wParam, LPARAM lParam)
{
	//DLOG(_T(""));
	const OrderOutstandings& outstds = SER().GetOutstandings();

	m_opOutstandings.clear();

	const int nNewIndex = (int)lParam;

	int nIndex = 0;
	OrderOutstanding* pOrderNew = NULL;
	OrderOutstanding* pOrderCancel = NULL;

	for(auto pos = outstds.begin() ; pos != outstds.end() ; ++pos, ++nIndex)
	{
		OrderOutstanding*	pOrder = *pos;
		
		if(pOrder->nVolumeRemain == 0)
		{
			pOrderCancel = pOrder;
			continue;
		}

		OrderPanelItem* pOPItem = new OrderPanelItem;

		pOPItem->pCode           = pOrder->pCode;
		pOPItem->marketType      = pOrder->marketType;
		pOPItem->strPrice        = pOrder->strPrice;
		pOPItem->position        = pOrder->position;
		pOPItem->nVol            = pOrder->nVolume;
//		pOPItem->profitEval      = 
		pOPItem->nOrdNum         = pOrder->nOrderNum;
		pOPItem->nVolRemain      = pOrder->nVolumeRemain;

		if(nIndex == nNewIndex)
		{
			pOrderNew = pOrder;
		}
		m_opOutstandings.push_back(OrderPanelItemPtr(pOPItem));
	}

	if(pOrderNew && pOrderNew->pCode == m_pSelCode)
	{
		if(pOrderNew->bHogaFakeUpdate)
		{
			m_gridHoga.ApplyHogaFakeUpdate(m_pMerch->GetHoga(), pOrderNew);
		}
		else if(pOrderNew->bRealOrderFailed)
		{
			m_gridHoga.ApplyRealOrderFailed(m_pMerch->GetHoga(), pOrderNew);
		}
	}

	if(pOrderCancel && pOrderCancel->pCode == m_pSelCode)
	{
		if(pOrderCancel->bHogaFakeUpdate)
		{
			m_gridHoga.ApplyHogaFakeUpdate(m_pMerch->GetHoga(), pOrderCancel);
		}
		else if(pOrderCancel->bRealOrderFailed)
		{
			m_gridHoga.ApplyRealOrderFailed(m_pMerch->GetHoga(), pOrderCancel);
		}
	}


	OutstandingApplyToHoga();
	UpdateOrderPanel();
	SetEnableOrderCount();
	//DLOG(_T(""));
	return 0;
}

void CFastTradeDlg_DSVIP::OutstandingApplyToHoga()
{
	if(FALSE == m_bHogaInit)
		return;
	if(NULL == m_pSelCode)
		return;
	
	const OrderOutstandings& outstds = SER().GetOutstandings();

	typedef map<pair<PositionType, CString>, GT_VOLUME>		OutstdMap;
	OutstdMap	outMap;
	int nBuy = 0, nSell = 0;
	for(auto pos = outstds.begin() ; pos != outstds.end() ; ++pos)
	{
		OrderOutstanding*	pOrder = *pos;

		if(m_pSelCode != pOrder->pCode)
			continue;
		
		pair<PositionType, CString> key(pOrder->position, pOrder->strPrice);
		auto posOut = outMap.find(key);
		if(outMap.end() == posOut)
			outMap.insert(make_pair(key, pOrder->nVolumeRemain));
		else
			posOut->second += pOrder->nVolumeRemain;

		if(pOrder->nVolumeRemain == 0)
			continue;

		if(PT_BUY == pOrder->position)
			nBuy += pOrder->nVolumeRemain;
		else
			nSell += pOrder->nVolumeRemain;
	}

	CString s;
	for(auto pos = outMap.begin() ; pos != outMap.end() ; ++pos)
	{
		if(pos->second == 0)
			s = _T("");
		else
			s = ::ToString(pos->second);

		m_gridHoga.OnOrderUpdated(pos->first.first, pos->first.second, s);
	}
	s = ::ToString(nSell);
	m_gridTotHoga.QuickSetText(c_sell, 0, s); m_gridTotHoga.RedrawCell(c_sell, 0);
	s = ::ToString(nBuy);
	m_gridTotHoga.QuickSetText(c_buy, 0, s);m_gridTotHoga.RedrawCell(c_buy, 0);
}
LRESULT CFastTradeDlg_DSVIP::OnContractedOrder(WPARAM wParam, LPARAM lParam)
{
	ContractedOrder* pCont = (ContractedOrder*)wParam;

	if(pCont->pCode == m_pSelCode)
	{
		CTime tm(CTime::GetCurrentTime());
		GTCurrent* pCurrent = m_pMerch->GetCurrent();
		if(!pCurrent->price.ToString().IsEmpty())
		{
			pCurrent->AddContract(tm.Format(_T("%H:%M:%S")), 
				pCont->strPrice, ::ToString(pCont->nVolume), pCont->posType);

			UpdateMarketContracts(pCurrent);
		}
	}

	return 0;
}

LRESULT CFastTradeDlg_DSVIP::OnOrderContracts(WPARAM wParam, LPARAM lParam)
{
	OrderContractNotifyItem* pNotf = (OrderContractNotifyItem*)wParam;

	const OrderContracts& conts = SER().GetContracts();
	Profit* pProfit = SER().GetProfit();
		
	m_opContracts.clear();
	m_pCurContract = NULL;

	for(auto pos = conts.begin() ; pos != conts.end() ; ++pos)
	{
		OrderContracted* pOrder = (*pos).get();
		OrderPanelItem* pOPItem = new OrderPanelItem;

		pOPItem->pCode           = pOrder->pCode;
		pOPItem->marketType      = pOrder->marketType;
		pOPItem->strPrice        = pOrder->strPrice;
		pOPItem->position        = pOrder->position;
		pOPItem->nVol            = pOrder->nVolume;
		pOPItem->profitEval      = pOrder->profitEval;
//		pOPItem->nOrdNum         =
//		pOPItem->nVolRemain      =
		if(pOrder->pCode == m_pSelCode)
			m_pCurContract = pOrder;

		m_opContracts.push_back(OrderPanelItemPtr(pOPItem));
	}

	if(pNotf && (m_pSelCode == pNotf->pCode && m_curMarketType == pNotf->marketType && FALSE == pNotf->bAllLiquid))
		m_pSelContractCode = m_pSelCode;
	else
		m_pSelContractCode = NULL;

	SetEnableOrderCount();
	ContractApplyToHoga();
	UpdateGridAccountPanel(ap_contract|ap_balance);
	UpdateOrderPanel();

	return 0;
}

void CFastTradeDlg_DSVIP::ContractApplyToHoga()
{
	
}

LRESULT CFastTradeDlg_DSVIP::OnProfitUpdated(WPARAM wParam, LPARAM lParam)
{
	OrderContracted* pOrder = (OrderContracted*)wParam;
	
	int nRowCount = m_gridOrderPanel.GetNumberRows();
	for(int nRow = 0 ; nRow < nRowCount ; ++nRow)
	{
		CString stype = m_gridOrderPanel.QuickGetText(COL_OP_SEP, nRow);
		CString code = m_gridOrderPanel.QuickGetText(COL_OP_CODE, nRow);
		
		if(stype == _T("체결") && code == pOrder->pCode->GetCode())
		{
			m_gridOrderPanel.QuickSetText(COL_OP_EVAL, nRow, ::ToThousand(pOrder->profitEval));
			m_gridOrderPanel.QuickSetTextColor(COL_OP_EVAL, nRow, ::GetProfitColor(pOrder->profitEval));
			m_gridOrderPanel.RedrawCell(COL_OP_EVAL, nRow);
			break;
		}
	}
	UpdateGridAccountPanel(ap_profit, pOrder);
	return 0;
}

void CFastTradeDlg_DSVIP::UpdateOrderPanel()
{
	m_gridOrderPanel.EnableUpdate(FALSE);

	for(int nRow = 0 ; nRow < m_gridOrderPanel.GetNumberRows() ; ++nRow)
		m_gridOrderPanel.DeleteRow(nRow);

	m_gridOrderPanel.SetNumberRows(m_opContracts.size() + m_opOutstandings.size());

	int nRow = 0;

#define STR(_str_) m_gridOrderPanel.QuickSetText(nCol, nRow, _str_); 
#define CLRT(_c_) m_gridOrderPanel.QuickSetTextColor(nCol, nRow, _c_);
#define CLRB(_c_) m_gridOrderPanel.QuickSetBackColor(nCol, nRow, _c_);
	CUGCell cell;

	CString s;
	for(auto pos = m_opContracts.begin() ; pos != m_opContracts.end() ; ++pos, ++nRow)
	{
		OrderPanelItem* p = (*pos).get();

		int nCol = 0;

		STR(_T("체결")); CLRB(crContBack) ; ++nCol;
		STR(p->pCode->GetCode()); CLRB(crContBack) ; ++nCol;
		STR(p->strPrice); CLRB(crContBack) ; ++nCol;
		s.Format(_T("%s %d"), ::PositionTypeStr(p->position), p->nVol);
		STR(s); CLRB(crContBack) ; CLRT(p->position == PT_SELL ? Colors::Blue : Colors::Red) ; ++nCol;
		STR(::ToThousand(p->profitEval)); CLRB(crContBack) ; CLRT(GetProfitColor(p->profitEval)); ++nCol;
		STR(_T("")); CLRB(crContBack); ++nCol;
#if 0
		STR(_T("")); CLRB(crContBack); ++nCol;
#else
		m_gridOrderPanel.GetCell(nCol, nRow, &cell);
		cell.SetText(_T("청산"));
		cell.SetCellType(m_gridOrderPanel.m_nButtonIndex);
		cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
		cell.SetAlignment(UG_ALIGNCENTER|UG_ALIGNVCENTER);
		cell.SetTextColor(RGB(0,0,128));
		cell.SetBackColor(Colors::Orange);
		cell.SetParam((long)p);
		m_gridOrderPanel.SetCell(nCol, nRow, &cell);
#endif
	}

	for(auto pos = m_opOutstandings.begin() ; pos != m_opOutstandings.end() ; ++pos, ++nRow)
	{
		OrderPanelItem* p = (*pos).get();

		int nCol = 0;

		STR(_T("미체결")); CLRB(crOutstdBack) ; ++nCol;
		STR(p->pCode->GetCode()); CLRB(crOutstdBack) ; ++nCol;
		STR(p->strPrice); CLRB(crOutstdBack) ; ++nCol;
		s.Format(_T("%s %d"), ::PositionTypeStr(p->position), p->nVol);
		STR(s); CLRB(crOutstdBack) ; CLRT(p->position == PT_SELL ? Colors::Blue : Colors::Red); ++nCol;
		STR(_T("")); CLRB(crOutstdBack); ++nCol; ;//STR(::ToThousand(p->profitEval)); CLRB(crOutstdBack) ; CLRT(GetProfitColor(p->profitEval));++nCol;
		STR(::ToString(p->nOrdNum)); CLRB(crOutstdBack) ; ++nCol;
#if 0
		STR(::ToString(p->nVolRemain)); CLRB(crOutstdBack) ; ++nCol;
#else
		m_gridOrderPanel.GetCell(nCol, nRow, &cell);
		cell.SetText(_T("취소"));
		cell.SetCellType(m_gridOrderPanel.m_nButtonIndex);
		cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
		cell.SetAlignment(UG_ALIGNCENTER|UG_ALIGNVCENTER);
		cell.SetTextColor(RGB(0,0,128));
		cell.SetBackColor(Colors::Violet);
		cell.SetParam((long)p);
		m_gridOrderPanel.SetCell(nCol, nRow, &cell);
#endif
	}

	const AutoOrders& autoOrders = GetAutoOrderManager()->GetAutoOrders();
	for(auto posOrds = autoOrders.begin() ; posOrds != autoOrders.end() ; ++posOrds)
	{
		AutoOrder* pAutoOrder = posOrds->second.second;
		const AutoOrder::MITs& mits = pAutoOrder->GetMITs();
		if(mits.empty())
			continue;
		
		for(auto pos = mits.begin() ; pos != mits.end() ; ++pos)
		{
			const AutoOrder::MITItem& item = pos->second;

			if(item.volBuy > 0)
			{
				m_gridOrderPanel.SetNumberRows(nRow + 1);
				int nCol = 0;
				STR(_T("MIT")); CLRB(crMitBack) ; ++nCol;
				STR(posOrds->first.first->GetCode()); CLRB(crMitBack) ; ++nCol;
				STR(pos->first); CLRB(crMitBack) ; ++nCol;
				s.Format(_T("%s %d"), ::PositionTypeStr(PT_BUY), item.volBuy);
				STR(s); CLRB(crMitBack) ; CLRT(Colors::Red); ++nCol;
				STR(_T("")); CLRB(crMitBack); ++nCol; ;//STR(::ToThousand(p->profitEval)); CLRB(crOutstdBack) ; CLRT(GetProfitColor(p->profitEval));++nCol;
				STR(_T("")); CLRB(crMitBack) ; ++nCol;
#if 0
				STR(_T("")); CLRB(crMitBack) ; ++nCol;
#else
				m_gridOrderPanel.GetCell(nCol, nRow, &cell);
				cell.SetText(_T("취소"));
				cell.SetCellType(m_gridOrderPanel.m_nButtonIndex);
				cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
				cell.SetAlignment(UG_ALIGNCENTER|UG_ALIGNVCENTER);
				cell.SetTextColor(RGB(0,0,128));
				cell.SetBackColor(Colors::DarkGreen);
				cell.SetParam((long)pAutoOrder);
				m_gridOrderPanel.SetCell(nCol, nRow, &cell);
#endif

				m_gridOrderPanel.GetCell(COL_OP_SEP, nRow, &cell);
				cell.SetParam((LONG)posOrds->first.first); // Code*
				m_gridOrderPanel.SetCell(COL_OP_SEP, nRow, &cell);

				m_gridOrderPanel.GetCell(COL_OP_CODE, nRow, &cell);
				cell.SetParam((LONG)posOrds->first.second); // MarketType
				m_gridOrderPanel.SetCell(COL_OP_CODE, nRow, &cell);

				m_gridOrderPanel.GetCell(COL_OP_VOL, nRow, &cell);
				cell.SetParam((LONG)item.volBuy); 
				m_gridOrderPanel.SetCell(COL_OP_VOL, nRow, &cell);

				++nRow;
			}

			if(item.volSell > 0)
			{
				m_gridOrderPanel.SetNumberRows(nRow + 1);
				int nCol = 0;
				STR(_T("MIT")); CLRB(crMitBack) ; ++nCol;
				STR(posOrds->first.first->GetCode()); CLRB(crMitBack) ; ++nCol;
				STR(pos->first); CLRB(crMitBack) ; ++nCol;

				s.Format(_T("%s %d"), ::PositionTypeStr(PT_SELL), item.volSell);
				STR(s); CLRB(crMitBack) ; CLRT(Colors::Blue); ++nCol;
				STR(_T("")); CLRB(crMitBack); ++nCol; ;//STR(::ToThousand(p->profitEval)); CLRB(crOutstdBack) ; CLRT(GetProfitColor(p->profitEval));++nCol;
				STR(_T("")); CLRB(crMitBack) ; ++nCol;
#if 0
				STR(_T("")); CLRB(crMitBack) ; ++nCol;
#else
				m_gridOrderPanel.GetCell(nCol, nRow, &cell);
				cell.SetText(_T("취소"));
				cell.SetCellType(m_gridOrderPanel.m_nButtonIndex);
				cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
				cell.SetAlignment(UG_ALIGNCENTER|UG_ALIGNVCENTER);
				cell.SetTextColor(RGB(0,0,128));
				cell.SetBackColor(Colors::DarkGreen);
				cell.SetParam((long)pAutoOrder);
				m_gridOrderPanel.SetCell(nCol, nRow, &cell);
#endif

				m_gridOrderPanel.GetCell(COL_OP_SEP, nRow, &cell);
				cell.SetParam((LONG)posOrds->first.first); // Code*
				m_gridOrderPanel.SetCell(COL_OP_SEP, nRow, &cell);

				m_gridOrderPanel.GetCell(COL_OP_CODE, nRow, &cell);
				cell.SetParam((LONG)posOrds->first.second); // MarketType
				m_gridOrderPanel.SetCell(COL_OP_CODE, nRow, &cell);

				m_gridOrderPanel.GetCell(COL_OP_VOL, nRow, &cell);
				cell.SetParam((LONG)PT_SELL); 
				m_gridOrderPanel.SetCell(COL_OP_VOL, nRow, &cell);

				m_gridOrderPanel.GetCell(COL_OP_EVAL, nRow, &cell);
				cell.SetParam((LONG)item.volSell); 
				m_gridOrderPanel.SetCell(COL_OP_EVAL, nRow, &cell);

				++nRow;
			}
		}
	}
		
	m_gridOrderPanel.EnableUpdate(TRUE);
	m_gridOrderPanel.RedrawAll();
}

LRESULT CFastTradeDlg_DSVIP::OnOrderPanelGridButtonClicked(WPARAM wParam, LPARAM lParam)
{
	const int nCol = (int)wParam;
	const int nRow = (int)lParam;
	ASSERT(nCol == COL_OP_CANCEL);

	CString str = m_gridOrderPanel.QuickGetText(COL_OP_SEP, nRow);
	if(str == _T("체결"))
	{
		CUGCell cell;
		m_gridOrderPanel.GetCell(nCol, nRow, &cell);
		OrderPanelItem* p = (OrderPanelItem*)cell.GetParam();

		GetSession()->OrderLiquid(GetSafeHwnd(), p->pCode);
		/*MarketType marketType = p->marketType;
		_Order(p->pCode, marketType, p->position == PT_SELL ? PT_BUY : PT_SELL, ht_SiJangGa, _T("0"), ::ToString(p->nVol), om_each_liquid_button);*/
	}
	else if(str == _T("미체결"))
	{
		CUGCell cell;
		m_gridOrderPanel.GetCell(nCol, nRow, &cell);
		OrderPanelItem* p = (OrderPanelItem*)cell.GetParam();
		
		CString strVol = ::ToString(p->nVolRemain);
#ifdef _SHOW_CONFIRM_MSG
		if(ENV().Get(ENV_ORDER_CONFIRM_SHOW).ToINT() == TRUE)
		{
			CString str;
			str.Format(_T("%s(%s) %s취소주문: 수량(%s) 가격(%s)\n주문을 취소하시겠습니까?"), 
				::MerchandiseTypeStr(p->pCode->GetType()), p->pCode->GetCode(), ::PositionTypeStr(p->position),
				strVol, p->strPrice);

			if(MessageBox(str, _T("취소주문"), MB_ICONQUESTION|MB_OKCANCEL) != IDOK)
				return 0;
		}
#endif
		ORDER_ACTOR actor;
		CString strAction;
		strAction.Format(_T("%s: 취소주문"), ::OrderMethodStr(om_each_cancel_button, actor));

		GetSession()->SendOrderAction(strAction, m_pSelCode->GetCode(), p->strPrice, p->position, strVol);
		GetSession()->OrderCancel(GetSafeHwnd(), p->nOrdNum, strVol);
	}
	else if(str == _T("MIT"))
	{
		CUGCell cell;

		m_gridOrderPanel.GetCell(nCol, nRow, &cell);
		AutoOrder* pAutoOrder = (AutoOrder*)cell.GetParam();
		
		CString strPrice = m_gridOrderPanel.QuickGetText(COL_OP_PRICE, nRow);
		m_gridOrderPanel.GetCell(COL_OP_VOL, nRow, &cell);
		PositionType posType = (PositionType)cell.GetParam(); 
		m_gridOrderPanel.GetCell(COL_OP_EVAL, nRow, &cell);
		CString strVol = ::ToString(cell.GetParam());

		ORDER_ACTOR actor;
		CString strAction;
		strAction.Format(_T("%s: MIT취소"), ::OrderMethodStr(om_each_cancel_button, actor));
		GetSession()->SendOrderAction(strAction, pAutoOrder->GetCodeID().first->GetCode(), strPrice, posType, strVol);

		pAutoOrder->MIT_Cancel(posType, strPrice);
	}
	return 0;
}


void CFastTradeDlg_DSVIP::SetMerchandise(Code* pCode, MarketType marketType)
{
	if(m_pMerch)
	{
		if(m_pMerch->GetCode() == pCode && m_curMarketType == marketType)
			return;

		m_pMerch->GetCurrent()->DetachObserver(dynamic_cast<GTCurrentObserver*>(this));
		m_pMerch->GetHoga()->DetachObserver(dynamic_cast<GTHogaObserver*>(this));

		GetAutoOrderManager()->DetachAutoOrderObserver(make_pair(m_pMerch->GetCode(), m_curMarketType), dynamic_cast<AutoOrderObserver*>(this));
		GetAutoOrderManager()->DetachAutoOrderObserver(make_pair(m_pMerch->GetCode(), m_curMarketType), dynamic_cast<AutoOrderObserver*>(&m_gridHoga));

		m_pAutoOrder = NULL;
		m_gridHoga.m_pAutoOrder = NULL;
	}

	m_bCurrentInit = FALSE;
	m_bHogaInit = FALSE;

	/*if(!m_mits.empty())
	{
		if(MessageBox(_T("MIT주문설정이 되어 있습니다. 종목을 변경하면 주문이 취소 됩니다.\r\n취소하시겠습니까?"), _T("확인"), MB_ICONASTERISK | MB_YESNO) == IDNO)
			return;

		MITAllCancel();
	}*/

	GTMerchandise::IDType codeID(pCode, marketType);

	m_pMerch = g_pMP->GetObject(codeID);

	m_pAutoOrder = GetAutoOrderManager()->AttachAutoOrderObserver(
				codeID, dynamic_cast<AutoOrderObserver*>(this));
	m_gridHoga.m_pAutoOrder = GetAutoOrderManager()->AttachAutoOrderObserver(
				codeID, dynamic_cast<AutoOrderObserver*>(&m_gridHoga));

	m_pMerch->GetCurrent()->AttachObserver(dynamic_cast<GTCurrentObserver*>(this));
	m_pMerch->GetHoga()->AttachObserver(dynamic_cast<GTHogaObserver*>(this));

	for(int nRow = 0 ; nRow < GTCurrent::MAX_QUEUE ; ++nRow)
	{
		m_gridMarketContract.QuickSetText(0, nRow, _T(""));
		m_gridMarketContract.QuickSetText(1, nRow, _T(""));
		m_gridMarketContract.QuickSetText(2, nRow, _T(""));
	}
	m_gridMarketContract.RedrawAll();
	
	m_pSelCode = pCode;

	m_pMerch->RunCurrentRecv();

	/// 호가 수신은 OnCurrentInitUpdated가 호출되면 호출한다.
	//m_pMerch->RunHogaRecv(); 
	
	CString strLabel;
	COLORREF crText = Colors::Black;
	//strLabel.Format(_T("%s %s"), ::MerchandiseTypeStr(pCode->GetType()), pCode->GetCode());
	if(pCode->GetType() == ct_futures)
	{
		strLabel = _T("KOSPI선물");
		crText = Colors::Black;
	}
	else if(pCode->GetType() == ct_options)
	{
		CodeOptions* pOPCode = dynamic_cast<CodeOptions*>(pCode);
		if(pOPCode)
		{
			if(pOPCode->GetOptionsType() == ot_call)
			{
				crText = Colors::Red;
				strLabel.Format(_T("콜 %s"), pOPCode->GetPrice());
			}
			else
			{
				crText = Colors::Blue;
				strLabel.Format(_T("풋 %s"), pOPCode->GetPrice());
			}
		}
	}
	else if(pCode->GetType() == ct_foreign_futures)
	{
		CodeForeignFutures* pFFCode = dynamic_cast<CodeForeignFutures*>(pCode);
		if(pFFCode)
		{
			strLabel = pFFCode->korName;
		}
	}

	m_labels[LBL_SEL_CODE].SetTextColor(true, crText);
	m_labels[LBL_SEL_CODE].SetWindowText(strLabel);

	CString strTitle;
	strTitle.Format(_T("[%s]빠른주문 %s"), GetSession()->id, strLabel);
		
	CString strAction;
	strAction.Format(_T("종목선택: %s"), strLabel);
	GetSession()->SendOrderAction(strAction);
}

void CFastTradeDlg_DSVIP::OnBnClickedButtonMarketSell()
{
	/// 시장가매도
	Order(PT_SELL, ht_SiJangGa, _T("0"), GetCurVolume(), om_button);
}

void CFastTradeDlg_DSVIP::OnBnClickedButtonMarketBuy()
{
	/// 시장가매수
	Order(PT_BUY, ht_SiJangGa, _T("0"), GetCurVolume(), om_button);
}

void CFastTradeDlg_DSVIP::OnBnClickedButtonAllLiquid()
{
	ASSERT(m_pSelCode);
	
	GetSession()->SendOrderAction(_T("버튼: 전체청산"));

	GetSession()->OrderLiquid(GetSafeHwnd(), NULL);
}

void CFastTradeDlg_DSVIP::MITAllCancel()
{
	m_pAutoOrder->MIT_AllCancel();
}

void CFastTradeDlg_DSVIP::OnBnClickedButtonSellMitCancel()
{
	m_pAutoOrder->MIT_SellCancel();	
	GetSession()->SendOrderAction(_T("버튼: MIT매도전체취소"));
}

void CFastTradeDlg_DSVIP::OnBnClickedButtonBuyMitCancel()
{
	m_pAutoOrder->MIT_BuyCancel();	
	GetSession()->SendOrderAction(_T("버튼: MIT매수전체취소"));
}

void CFastTradeDlg_DSVIP::OnBnClickedButtonSellCancel()
{
	GetSession()->SendOrderAction(_T("버튼: 매도전체취소"));
		
	GetSession()->OrderCancelAllByPosition(GetSafeHwnd(), m_pSelCode, PT_SELL);
}

void CFastTradeDlg_DSVIP::OnBnClickedButtonBuyCancel()
{
	GetSession()->SendOrderAction(_T("버튼: 매수전체취소"));
		
	GetSession()->OrderCancelAllByPosition(GetSafeHwnd(), m_pSelCode, PT_BUY);

}

void CFastTradeDlg_DSVIP::OnBnClickedButtonAllCancel()
{
	GetSession()->SendOrderAction(_T("버튼: 전체취소"));

	GetSession()->OrderCancelAllByPosition(GetSafeHwnd(), NULL, PT_NONE);
	MITAllCancel();
}

void CFastTradeDlg_DSVIP::MITTotalUpdate()
{
	int nBuy = 0, nSell = 0;
	const AutoOrder::MITs& mits = m_pAutoOrder->GetMITs();

	for(auto pos = mits.begin() ; pos != mits.end() ; ++pos)
	{
		const AutoOrder::MITItem& item = pos->second;
		nSell += item.volSell;
		nBuy += item.volBuy;
	}

	CString s;
	s = ::ToString(nSell);
	m_gridTotHoga.QuickSetText(c_mit_sell, 0, s); m_gridTotHoga.RedrawCell(c_mit_sell, 0);

	s = ::ToString(nBuy);
	m_gridTotHoga.QuickSetText(c_mit_buy, 0, s); m_gridTotHoga.RedrawCell(c_mit_buy, 0);
}

void CFastTradeDlg_DSVIP::OnBnClickedCheckBigHogaHeight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_BIG_HOGA_HEIGHT))->GetCheck();
	m_gridHoga.SetHighRowHeight(bCheck);
}


void CFastTradeDlg_DSVIP::OnBnClickedCheckOneclickOrder()
{
	m_bOneClick = ((CButton*)GetDlgItem(IDC_CHECK_ONECLICK_ORDER))->GetCheck() == BST_CHECKED;
	m_gridHoga.SetOneClickOrder(m_bOneClick);

	CString strAction;
	strAction.Format(_T("원클릭주문 %s"), m_bOneClick ? _T("설정") : _T("해제"));
	GetSession()->SendOrderAction(strAction);
}


void CFastTradeDlg_DSVIP::OnBnClickedCheckLosscut()
{
	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_LOSSCUT))->GetCheck() == BST_CHECKED;
	
	CString s;
	GetDlgItemText(IDC_EDIT_LOSSCUT, s);

	CString strMsg;
	if(!m_pAutoOrder->SetLosscut(bCheck, ::ToINT(s), dynamic_cast<AutoOrderObserver*>(this), strMsg))
	{
		((CButton*)GetDlgItem(IDC_CHECK_LOSSCUT))->SetCheck(bCheck ? BST_UNCHECKED : BST_CHECKED);
		CString str;
		str.Format(_T("손실제한설정실패: %s"), strMsg);
		GetSession()->SendOrderAction(str);
		MessageBox(strMsg, _T("설정실패"), MB_ICONERROR);
		return;
	}

	GetDlgItem(IDC_EDIT_LOSSCUT)->EnableWindow(!bCheck);
	GetDlgItem(IDC_SPIN_LOSSCUT)->EnableWindow(!bCheck);

	if(bCheck)
	{
		CString str;
		str.Format(_T("손실제한설정: %s틱"), s);
		GetSession()->SendOrderAction(str);
	}
	else
	{
		GetSession()->SendOrderAction(_T("손실제한설정해제"));
	}
}


void CFastTradeDlg_DSVIP::OnBnClickedCheckEarncut()
{
	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_EARNCUT))->GetCheck() == BST_CHECKED;
	
	CString s;
	GetDlgItemText(IDC_EDIT_EARNCUT, s);

	CString strMsg;
	if(!m_pAutoOrder->SetEarncut(bCheck, ::ToINT(s), dynamic_cast<AutoOrderObserver*>(this), strMsg))
	{
		((CButton*)GetDlgItem(IDC_CHECK_EARNCUT))->SetCheck(bCheck ? BST_UNCHECKED : BST_CHECKED);
		CString str;
		str.Format(_T("이익실현설정실패: %s"), strMsg);
		GetSession()->SendOrderAction(str);
		MessageBox(strMsg, _T("설정실패"), MB_ICONERROR);
		return;
	}

	GetDlgItem(IDC_EDIT_EARNCUT)->EnableWindow(!bCheck);
	GetDlgItem(IDC_SPIN_EARNCUT)->EnableWindow(!bCheck);

	if(bCheck)
	{
		CString str;
		str.Format(_T("이익실현설정: %s틱"), s);
		GetSession()->SendOrderAction(str);
	}
	else
	{
		GetSession()->SendOrderAction(_T("이익실현설정해제"));
	}
}


void CFastTradeDlg_DSVIP::OnHogaGridEndSelChange(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
    NM_UG* pItem = (NM_UG*) pNotifyStruct;

	pItem->col;
	pItem->row;
}

void CFastTradeDlg_DSVIP::OnOrderPanelGridSelChange(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
    NM_UG* pItem = (NM_UG*) pNotifyStruct;
	pItem->col;
	pItem->row;
	
	if(pItem->col == COL_OP_CANCEL)
		return;

	if(pItem->col != -1 && pItem->row != -1)
	{
		CString str = m_gridOrderPanel.QuickGetText(COL_OP_SEP, pItem->row);
		Code* pCode = NULL;
		MarketType marketType = mt_none;

		if(str == _T("체결") || str == _T("미체결"))
		{
			CUGCell cell;
			m_gridOrderPanel.GetCell(COL_OP_CANCEL, pItem->row, &cell);
			OrderPanelItem* pOPItem = (OrderPanelItem*)cell.GetParam();
			ASSERT(pOPItem);
			pCode = pOPItem->pCode;
			marketType = pOPItem->marketType;
		}
		else if(str == _T("MIT"))
		{
			CUGCell cell;
			m_gridOrderPanel.GetCell(COL_OP_SEP, pItem->row, &cell);
			pCode = (Code*)cell.GetParam();

			m_gridOrderPanel.GetCell(COL_OP_CODE, pItem->row, &cell);
			marketType = (MarketType)cell.GetParam();
		}
		ASSERT(pCode && marketType != mt_none);

		if(m_pOptionsTableDlg)
		{
			m_pOptionsTableDlg->ShowWindow(SW_HIDE);
			m_pOptionsTableDlg->SendMessage(WM_CLOSE);
		}

		if(m_pForeignFuturesCodeDlg)
		{
			m_pForeignFuturesCodeDlg->ShowWindow(SW_HIDE);
			m_pForeignFuturesCodeDlg->SendMessage(WM_CLOSE);
		}

		int nSelTab = NUL_IDX;
		CString strCodeBtn;
		switch(pCode->GetType())
		{
		case ct_futures:
			if(marketType == mt_CME)
				nSelTab = 2;
			else
				nSelTab = 0;
			break;
		case ct_options:
			if(marketType == mt_EUREX)
			{
				strCodeBtn = _T("야간옵션행사가격선택");
				nSelTab = 3;
			}
			else
			{
				strCodeBtn = _T("옵션행사가격선택");
				nSelTab = 1;
			}
			break;
		case ct_foreign_futures:
			nSelTab = 4;
			strCodeBtn = _T("해외선물종목선택");
			break;
		default:
			ASSERT(FALSE);
		}
		m_tabMerchandise.SetCurSel(nSelTab);
		if(m_pSelCode != pCode)
			SetMerchandise(pCode, marketType);

		//if(!strCodeBtn.IsEmpty())
		//	m_buttons[BTN_SELECT_CODE].SetWindowText(strCodeBtn);
		m_buttons[BTN_SELECT_CODE].ShowWindow(strCodeBtn.IsEmpty() ? SW_HIDE : SW_SHOW);
		m_curMerchandiseType = pCode->GetType();
		m_curMarketType = marketType;
			
		if(str == _T("체결"))
			m_pSelContractCode = pCode;

		UpdateGridAccountPanel(ap_contract);
	}
}

void CFastTradeDlg_DSVIP::OnMerchandiseTabSelChange(NMHDR *pNMHDR, LRESULT *pResult)
{
	//현재 띄워져 있는 종목선택 다이얼로그를 해제한다.
	if(m_pOptionsTableDlg)
	{
		m_pOptionsTableDlg->ShowWindow(SW_HIDE);
		m_pOptionsTableDlg->SendMessage(WM_CLOSE);
	}

	if(m_pForeignFuturesCodeDlg)
	{
		m_pForeignFuturesCodeDlg->ShowWindow(SW_HIDE);
		m_pForeignFuturesCodeDlg->SendMessage(WM_CLOSE);
	}

	int nTab = m_tabMerchandise.GetCurSel();
	
	MerchandiseType mtype = ct_none;
	MarketType marketType = mt_default;

	CString strCodeBtn;
	Code* pCode = NULL;

	if(mt_futures == nTab)
	{
		mtype = ct_futures;
		pCode = InstCM().GetFutures().begin()->second;
	}
	else if(mt_night_futures == nTab)
	{
		mtype = ct_futures;
		marketType = mt_CME;

		pCode = InstCM().GetFutures().begin()->second;
		if(pCode->bExpireDay)
			pCode = (++InstCM().GetFutures().begin())->second; //만기일이라면 차월물을 근월물로 셋팅
	}
	else if(mt_foreign_futures == nTab)
	{
		ASSERT(NULL == m_pForeignFuturesCodeDlg);

		std::vector<Code*> codesExact;
		std::vector<Code*> codesPartial;
		InstCM().MatchedCodes(_T("URO"), codesExact, codesPartial, ct_foreign_futures);
		ASSERT(!codesPartial.empty());
		pCode = *codesPartial.begin();
		
		m_pForeignFuturesCodeDlg = new CForeignFuturesCodeDlg(this);
		m_pForeignFuturesCodeDlg->m_pOwner = this;
		m_pForeignFuturesCodeDlg->Create(CForeignFuturesCodeDlg::IDD, this);
		m_pForeignFuturesCodeDlg->ShowWindow(SW_SHOW);

		mtype = ct_foreign_futures;
		strCodeBtn = _T("해외선물종목선택");
	}
	else if(mt_options == nTab)
	{
		ASSERT(NULL == m_pOptionsTableDlg);
		m_pOptionsTableDlg = new COptionsTableDlg(this, FALSE);
		m_pOptionsTableDlg->m_pOwner = this;
		m_pOptionsTableDlg->Create(COptionsTableDlg::IDD, this);
		m_pOptionsTableDlg->ShowWindow(SW_SHOW);

		mtype = ct_options;
		strCodeBtn = _T("옵션행사가선택");
	}
	else if(mt_night_options == nTab)
	{
		ASSERT(NULL == m_pOptionsTableDlg);
		m_pOptionsTableDlg = new COptionsTableDlg(this, TRUE);
		m_pOptionsTableDlg->m_pOwner = this;
		m_pOptionsTableDlg->Create(COptionsTableDlg::IDD, this);
		m_pOptionsTableDlg->ShowWindow(SW_SHOW);

		mtype = ct_options;
		marketType = mt_EUREX;
		strCodeBtn = _T("야간옵션행사가선택");
	}

	if(pCode)
		SetMerchandise(pCode, marketType);

//	if(!strCodeBtn.IsEmpty())
//		m_buttons[BTN_SELECT_CODE].SetWindowText(strCodeBtn);

	m_buttons[BTN_SELECT_CODE].ShowWindow(strCodeBtn.IsEmpty() ? SW_HIDE : SW_SHOW);
	m_curMerchandiseType = mtype;
	m_curMarketType = marketType;
	UpdateGridAccountPanel(ap_contract);

//	*pResult = 0;
}

void CFastTradeDlg_DSVIP::OnDestroy()
{
	if(m_pOptionsTableDlg)
	{
		m_pOptionsTableDlg->SendMessage(WM_CLOSE);
	}

	if(m_pForeignFuturesCodeDlg)
	{
		m_pForeignFuturesCodeDlg->SendMessage(WM_CLOSE);
	}
	
	if(m_pMerch)
	{
		m_pMerch->GetCurrent()->DetachObserver(dynamic_cast<GTCurrentObserver*>(this));
		m_pMerch->GetHoga()->DetachObserver(dynamic_cast<GTHogaObserver*>(this));
		m_pMerch.Clear();
	}

	SER().UnregisterOrderOutstanding(this);
	SER().UnregisterOrderContracted(this);
	SER().UnregisterClientInfo(this);

	if(NULL != m_pSelCode)
	{
		GetAutoOrderManager()->DetachAutoOrderObserver(make_pair(m_pSelCode, m_curMarketType), dynamic_cast<AutoOrderObserver*>(this));
		GetAutoOrderManager()->DetachAutoOrderObserver(make_pair(m_pSelCode, m_curMarketType), dynamic_cast<AutoOrderObserver*>(&m_gridHoga));

		m_pAutoOrder = NULL;
		m_gridHoga.m_pAutoOrder = NULL;
	}

	GetAutoOrderManager()->DetachObserver(dynamic_cast<AutoOrderManagerObserver*>(this));
	m_gridHoga.OnFinalize();

	VTSFrmWndChildResizableDlg::OnDestroy();
}


void CFastTradeDlg_DSVIP::OnBnClickedVolume(UINT nID)
{
	CString vol;
	GetDlgItemText(nID, vol);
	m_editVol.SetWindowText(vol);

	CString strAction;
	strAction.Format(_T("수량설정 %s개"), vol);
	GetSession()->SendOrderAction(strAction);
}

void CFastTradeDlg_DSVIP::OnBnClickedExpand()
{
	m_bExpand = !m_bExpand;

	CRect rt;
	GetClientRect(rt);

	CString s;
	if(m_bExpand)
	{
		m_buttonExpand.SetTooltipText(_T("창을 축소합니다"));
		s = _T("<<");
		rt.right = m_nCurRight + m_nExpandAreaWidth;
	}
	else
	{
		m_buttonExpand.SetTooltipText(_T("창을 확장합니다"));
		s = _T(">>");
		rt.right = m_nCurRight;
	}

	m_buttonExpand.SetWindowText(s);
	GetParent()->SendMessage(WM_VTSFRM_CHILD_RESIZE_REQUEST, (WPARAM)rt.Width(), (LPARAM)rt.Height());
}

LRESULT CFastTradeDlg_DSVIP::OnPreferencesChanged(WPARAM wParam, LPARAM lParam)
{
	CString str;
	str = ENV().Get(ENV_VOL_1);
	SetDlgItemText(IDC_BUTTON_VOL_1, str);
	str = ENV().Get(ENV_VOL_2);
	SetDlgItemText(IDC_BUTTON_VOL_2, str);
	str = ENV().Get(ENV_VOL_3);
	SetDlgItemText(IDC_BUTTON_VOL_3, str);
	str = ENV().Get(ENV_VOL_4);
	SetDlgItemText(IDC_BUTTON_VOL_4, str);
	str = ENV().Get(ENV_VOL_5);
	SetDlgItemText(IDC_BUTTON_VOL_5, str);
	return 0;
}
LRESULT CFastTradeDlg_DSVIP::OnOptionsTableSelectCodeChanged(WPARAM wParam, LPARAM lParam)
{
	Code* pCode = (Code*)wParam;
	MarketType marketType = (MarketType)lParam;
	ASSERT(pCode);
	SetMerchandise(pCode, marketType);
	return 0;
}

LRESULT CFastTradeDlg_DSVIP::OnVTSModelessDlgDestroyed(WPARAM wParam, LPARAM lParam)
{
	if(m_pOptionsTableDlg == (COptionsTableDlg*)wParam)
		m_pOptionsTableDlg = NULL;
	else if(m_pForeignFuturesCodeDlg == (CForeignFuturesCodeDlg*)wParam)
		m_pForeignFuturesCodeDlg = NULL;

	return 0;
}

void CFastTradeDlg_DSVIP::OnSizing(UINT fwSide, LPRECT pRect)
{
	

	VTSFrmWndChildResizableDlg::OnSizing(fwSide, pRect);
}

void CFastTradeDlg_DSVIP::OnSize(UINT nType, int cx, int cy)
{
	VTSFrmWndChildResizableDlg::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(IsWindow(m_gridHoga.GetSafeHwnd()))
	{
#if 0
		int nHogaSizeType = ENV().Get(ENV_HOGA_RESIZE_TYPE).ToINT();
		if(nHogaSizeType == hst_count_fix)
		{
			CRect rect;
			m_gridHoga.GetClientRect(rect);
			int nNewHeight = rect.Height() / 19;
			m_gridHoga.SetRowHeight(0, nNewHeight );
			m_gridHoga.RedrawAll();
		}
#endif
	}
}


LRESULT CFastTradeDlg_DSVIP::OnOrderResult(WPARAM wParam, LPARAM lParam)
{
	OrderResult* p = (OrderResult*)wParam;

	if(OS_REJECT == p->orderStatus)
	{
		MessageBox(p->strMsg, _T("주문거부"), MB_ICONERROR);
	}

	return 0;
}

void CFastTradeDlg_DSVIP::OnBnClickedCheckAlwaysOnTop()
{
	BOOL bCheck = BST_CHECKED == ((CButton*)GetDlgItem(IDC_CHECK_ALWAYS_ON_TOP))->GetCheck();
	GetParent()->SendMessage(WM_VTSFRM_SET_TOP_MOST, (WPARAM)bCheck);
}

void CFastTradeDlg_DSVIP::OnBnClickedCheckStandalone()
{
	BOOL bCheck = BST_CHECKED == ((CButton*)GetDlgItem(IDC_CHECK_STANDALONE))->GetCheck();
	GetParent()->SendMessage(WM_COMMAND, bCheck ? ID_COMMAND_TO_STANDALONE_WND : ID_COMMAND_STANDALONE_WND_TO_MDI_CHILD, 0);
}

//작게
void CFastTradeDlg_DSVIP::OnBnClickedRadio1()
{
	SetResize(sz_small);
}

//중간
void CFastTradeDlg_DSVIP::OnBnClickedRadio2()
{
	SetResize(sz_medium);
}

//크게
void CFastTradeDlg_DSVIP::OnBnClickedRadio3()
{
	SetResize(sz_large);
}

LRESULT CFastTradeDlg_DSVIP::OnClientInfoUpdated(WPARAM wParam, LPARAM lParam)
{
	SetEnableOrderCount();

	UpdateGridAccountPanel(ap_balance);

	return 0;
}


BOOL CFastTradeDlg_DSVIP::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		BOOL bAlt = HIWORD(pMsg->lParam) & KF_ALTDOWN;
		TCHAR vkTCHAR = (TCHAR)pMsg->wParam;


	}
	return VTSFrmWndChildResizableDlg::PreTranslateMessage(pMsg);
}

#endif //#if COMPANY==COMPANY_DSVIP
