// FastTradeDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "FastTradeDlg.h"

#include "afxdialogex.h"
#include "VTS.h"
#include "ServerEventReceiver.h"
#include "FuturesCodesDlg.h"
#include "OptionsTableDlg.h"
#include "ForeignFuturesCodeDlg.h"
#include "OvernightRequestDlg.h"

#include "DKMsgBox.h"
// CFastTradeDlg 대화 상자입니다.
static COLORREF CR_BUY_BG = RGB(252, 214, 203);
static COLORREF CR_SELL_BG = RGB(214, 235, 240);
static COLORREF CR_SELL_BID_INFO_BG = RGB(205, 207, 232);
static COLORREF CR_BUY_BID_INFO_BG = RGB(234, 208, 207);
static COLORREF crContBack = RGB(253, 234, 218);
static COLORREF crOutstdBack = RGB(215, 228, 189);
static COLORREF crMitBack = RGB(230, 224, 236);


static COLORREF CR_DEF_GRID_HEADER_BG = RGB(231, 238, 216); //RGB(88,90,98); //RGB(234,234,234) 
static COLORREF CR_DEF_GRID_HEADER_TEXT = Colors::Black; //RGB(221,221,221);

static COLORREF CR_SELL = RGB(1, 96, 245);
static COLORREF CR_BUY = Colors::Red;
static COLORREF CR_ALL = RGB(153, 193, 71);
static COLORREF CR_MERCH_TAB_SEL = RGB(247, 150, 70);
static COLORREF CR_MERCH_NONE_SEL = RGB(217, 217, 217);

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
	{ _T("체결수량"), 0.17 },
	{ _T("평균단가"), 0.17 },
	{ _T("평가손익"), 0.17 },
	{ _T("평가손익합"), 0.17 },
	{ _T("실시간손익합"), 0.16 },
	{ _T("실현손익"), 0.16 },
};

enum FieldAccBal
{
	fld_ab_EvalBalance,
	fld_ab_Losscut,
	fld_ab_FuturesOver,
	fld_ab_OptionsOver,
	fld_ab_OverSum,
	fld_ab_OverSet,
};

static GridColumnItem g_colsAccBal[] =
{
	{ _T("평가담보금"), 0.17 },
	{ _T("로스컷"), 0.17 },
	{ _T("선물오버가능"), 0.17 },
	{ _T("옵션오버가능"), 0.17 },
	{ _T("오버가능액합계"), 0.16 },
	{ _T("오버나잇"), 0.16 },
};

enum FieldOrderPanel
{
	fld_op_Type,
	fld_op_Code,
	fld_op_Price,
	fld_op_Pos,
	fld_op_Vol,
	fld_op_Profit,
#ifdef OP_ORDNUM
	fld_op_num,
#endif
	fld_op_Cancel,
};

enum OrderPanelItemType
{
	opt_Contract,
	opt_Outstanding,
	opt_MIT,
};

static GridColumnItem g_colsOP[] =
{
	{ _T("타입"), 0.12 },
	{ _T("종목"), 0.20 },
	{ _T("가격"), 0.15 },
	{ _T("구분"), 0.09 },
	{ _T("수량"), 0.09 },
	{ _T("평가손익"), 0.22 },
#ifdef OP_ORDNUM
	{ _T("주문#"), 0.12 },
#endif
	{ _T("취소"), 0.07 },
};
const int ORDER_PANEL_GRID_BTN_COL = sizeof(g_colsOP) / sizeof(GridColumnItem);


enum MerchandiseTab
{
	mt_futures,
	mt_options,
	mt_night_futures,
	mt_night_options,
	mt_foreign_futures
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OrderPanelGrid::OnSetup()
{
	m_nButtonIndex = AddCellType(&m_button);
}
int OrderPanelGrid::OnCellTypeNotify(long ID, int col, long row, long msg, long param){

	if (ID == m_nButtonIndex){
		return OnPushButton(ID, col, row, msg, param);
	}
	return TRUE;
}

int OrderPanelGrid::OnPushButton(long ID, int col, long row, long msg, long param){

	CUGCell cell;
	GetCell(col, row, &cell);
	int nCellTypeIndex = cell.GetCellType();
	int nParam = cell.GetParam();

	if (msg == UGCT_BUTTONUP)
	{
		m_pOwner->PostMessage(WM_ORDER_PANEL_GRID_BUTTON_CLICKED, (WPARAM)col, (LPARAM)row);
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int AccountBalanceGrid::OnPushButton(long ID, int col, long row, long msg, long param){

	CUGCell cell;
	GetCell(col, row, &cell);
	int nCellTypeIndex = cell.GetCellType();
	int nParam = cell.GetParam();

	if (msg == UGCT_BUTTONUP)
	{
		m_pOwner->PostMessage(WM_ACCOUNT_BALANCE_GRID_BUTTON_CLICKED, (WPARAM)col, (LPARAM)row);
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNCREATE(CFastTradeDlg, VTSFrmWndChildResizableDlg)

CFastTradeDlg::CFastTradeDlg(CWnd* pParent /*=NULL*/)
: VTSFrmWndChildResizableDlg(CFastTradeDlg::IDD, pParent)
, m_nPriceType(1),
m_bCurrentInit(FALSE), m_bHogaInit(FALSE),
m_pSelCode(NULL), m_bExpand(FALSE), m_pOptionsTableDlg(NULL),
m_nSizeMode(sz_small), m_bOneClick(FALSE), m_curMerchandiseType(ct_none),
m_pForeignFuturesCodeDlg(NULL), m_pAutoOrder(NULL), m_pCurBase(NULL)
{
	m_spinEditEarnCut.SetEditReadOnly(TRUE);
	m_spinEditLossCut.SetEditReadOnly(TRUE);
}

CFastTradeDlg::~CFastTradeDlg()
{
}

BOOL CFastTradeDlg::Create(CWnd* pParentWnd)
{
	return VTSFrmWndChildResizableDlg::Create(IDD, pParentWnd);
}


void CFastTradeDlg::DoDataExchange(CDataExchange* pDX)
{
	VTSFrmWndChildResizableDlg::DoDataExchange(pDX);
#ifdef _WNTAB
	DDX_Control(pDX, ID_TABCTRL_MERCHANDISE, m_wndTabMerchandise);
	DDX_Control(pDX, IDC_TABCTRL_STOP, m_wndTabStop);
	DDX_Control(pDX, IDC_TABCTRL_CURRENT_INFO, m_wndTabCurrentInfo);
	DDX_Control(pDX, IDC_TABCTRL_CONTRACTS, m_wndTabContracts);
#else
	DDX_Control(pDX, IDC_TAB_MERCHANDISE, m_tabMerchandise);
	DDX_Control(pDX, IDC_TAB_CURRENT_INFO, m_tabCurrentInfo);
	DDX_Control(pDX, IDC_TAB_SET_DISPLAY, m_tabSetDisplay);
	DDX_Control(pDX, IDC_TAB_MARKET_CONTRACT, m_tabMarketContract);
#endif

	DDX_Control(pDX, IDC_BUTTON_EXPAND, m_buttonExpand);

	DDX_Control(pDX, IDC_BUTTON_MARKET_SELL, m_buttons[BTN_MARKET_SELL]);
	DDX_Control(pDX, IDC_BUTTON_ALL_LIQUID, m_buttons[BTN_ALL_LIQUID]);
	DDX_Control(pDX, IDC_BUTTON_MARKET_BUY, m_buttons[BTN_MARKET_BUY]);
	DDX_Control(pDX, IDC_BUTTON_SELL_MIT_CANCEL, m_buttons[BTN_SELL_MIT_CANCEL]);
	DDX_Control(pDX, IDC_BUTTON_SELL_CANCEL, m_buttons[BTN_SELL_CANCEL]);
	DDX_Control(pDX, IDC_BUTTON_ALL_CANCEL, m_buttons[BTN_ALL_CANCEL]);
	DDX_Control(pDX, IDC_BUTTON_BUY_CANCEL, m_buttons[BTN_BUY_CANCEL]);
	DDX_Control(pDX, IDC_BUTTON_BUY_MIT_CANCEL, m_buttons[BTN_BUY_MIT_CANCEL]);
	DDX_Control(pDX, IDC_BUTTON_SELECT_CODE, m_buttonSelCode);
	DDX_Control(pDX, IDC_BUTTON_VOL_1, m_extButtons[BTN_VOL_1]);
	DDX_Control(pDX, IDC_BUTTON_VOL_2, m_extButtons[BTN_VOL_2]);
	DDX_Control(pDX, IDC_BUTTON_VOL_3, m_extButtons[BTN_VOL_3]);
	DDX_Control(pDX, IDC_BUTTON_VOL_4, m_extButtons[BTN_VOL_4]);
	DDX_Control(pDX, IDC_BUTTON_VOL_5, m_extButtons[BTN_VOL_5]);

	DDX_Control(pDX, IDC_STATIC_1, m_labels[LBL_1]);
	DDX_Control(pDX, IDC_STATIC_SEL_CODE, m_labels[LBL_SEL_CODE]);
	DDX_Control(pDX, IDC_STATIC_SELL, m_labels[LBL_SELL]);
	DDX_Control(pDX, IDC_STATIC_ENABLE_SELL, m_labels[LBL_ENABLE_SELL]);
	DDX_Control(pDX, IDC_STATIC_BUY, m_labels[LBL_BUY]);
	DDX_Control(pDX, IDC_STATIC_ENABLE_BUY, m_labels[LBL_ENABLE_BUY]);
	DDX_Control(pDX, IDC_STATIC_2, m_labels[LBL_2]);

	DDX_Control(pDX, IDC_CHECK_ONECLICK_ORDER, m_checkOneClick);
	DDX_Control(pDX, IDC_CHECK_ALWAYS_ON_TOP, m_checkAlwaysOnTop);

	DDX_Control(pDX, IDC_GRID_MARKET_CONTRACT, m_gridMarketContract);
	
	DDX_Control(pDX, IDC_CHECK_LOSSCUT, m_checkLosscut);
	DDX_Control(pDX, IDC_CHECK_EARNCUT, m_checkEarncut);

	DDX_Control(pDX, IDC_EDIT_VOLUME, m_editVol);
	DDX_Control(pDX, IDC_SPIN_VOL, m_spinVol);
	/*DDX_Control(pDX, IDC_EDIT_LOSSCUT, m_editLosscut);
	DDX_Control(pDX, IDC_EDIT_EARNCUT, m_editEarncut);*/
	/*DDX_Control(pDX, IDC_SPIN_LOSSCUT, m_spinLosscut);
	DDX_Control(pDX, IDC_SPIN_EARNCUT, m_spinEarncut);*/
	DDX_Control(pDX, IDC_DKSPIN_EARNCUT, m_spinEditEarnCut);
	DDX_Control(pDX, IDC_DKSPIN_LOSSCUT, m_spinEditLossCut);
	
}

BEGIN_MESSAGE_MAP(CFastTradeDlg, VTSFrmWndChildResizableDlg)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_SELECT_CODE, &CFastTradeDlg::OnBnClickedSelectCode)
	ON_MESSAGE(WM_ORDER_OUTSTANDING, &CFastTradeDlg::OnOrderOutstanding)
	ON_MESSAGE(WM_CONTRACTED_ORDER, &CFastTradeDlg::OnContractedOrder)
	ON_MESSAGE(WM_ORDER_CONTRACTS, &CFastTradeDlg::OnOrderContracts)
	ON_MESSAGE(WM_PROFIT_UPDATED, &CFastTradeDlg::OnProfitUpdated)
	ON_BN_CLICKED(IDC_BUTTON_MARKET_SELL, &CFastTradeDlg::OnBnClickedButtonMarketSell)
	ON_BN_CLICKED(IDC_BUTTON_MARKET_BUY, &CFastTradeDlg::OnBnClickedButtonMarketBuy)
	ON_BN_CLICKED(IDC_BUTTON_ALL_LIQUID, &CFastTradeDlg::OnBnClickedButtonAllLiquid)
	ON_BN_CLICKED(IDC_BUTTON_SELL_MIT_CANCEL, &CFastTradeDlg::OnBnClickedButtonSellMitCancel)
	ON_BN_CLICKED(IDC_BUTTON_BUY_MIT_CANCEL, &CFastTradeDlg::OnBnClickedButtonBuyMitCancel)
	ON_BN_CLICKED(IDC_BUTTON_SELL_CANCEL, &CFastTradeDlg::OnBnClickedButtonSellCancel)
	ON_BN_CLICKED(IDC_BUTTON_BUY_CANCEL, &CFastTradeDlg::OnBnClickedButtonBuyCancel)
	ON_BN_CLICKED(IDC_BUTTON_ALL_CANCEL, &CFastTradeDlg::OnBnClickedButtonAllCancel)
	ON_BN_CLICKED(IDC_CHECK_BIG_HOGA_HEIGHT, &CFastTradeDlg::OnBnClickedCheckBigHogaHeight)
	ON_BN_CLICKED(IDC_CHECK_ONECLICK_ORDER, &CFastTradeDlg::OnBnClickedCheckOneclickOrder)
	ON_BN_CLICKED(IDC_CHECK_LOSSCUT, &CFastTradeDlg::OnBnClickedCheckLosscut)
	ON_BN_CLICKED(IDC_CHECK_EARNCUT, &CFastTradeDlg::OnBnClickedCheckEarncut)
	ON_NOTIFY(UGN_SELCHANGED, IDC_GRID_HOGA, OnHogaGridEndSelChange)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_VOL_1, IDC_BUTTON_VOL_5, &CFastTradeDlg::OnBnClickedVolume)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_EXPAND, &CFastTradeDlg::OnBnClickedExpand)
	ON_CONTROL(DKTABN_CHANGED, IDC_TAB_MERCHANDISE, &CFastTradeDlg::OnMerchandiseTabSelChange)
	ON_MESSAGE(WM_PREFERENCES_CHANGED, &CFastTradeDlg::OnPreferencesChanged)
	ON_MESSAGE(WM_OPTIONS_TABLE_SELECT_CODE_CHANGED, &CFastTradeDlg::OnOptionsTableSelectCodeChanged)
	ON_MESSAGE(WM_ORDER_RESULT, &CFastTradeDlg::OnOrderResult)
	ON_WM_SIZING()
	ON_WM_SIZE()
	ON_MESSAGE(WM_VTS_MODELESS_DLG_DESTROYED, &CFastTradeDlg::OnVTSModelessDlgDestroyed)
	ON_MESSAGE(WM_ENABLE_ORDER_COUNT_RECEIVED, &CFastTradeDlg::OnEnableOrderCountReceived)
	ON_MESSAGE(WM_CLIENT_INFO_UPDATED, &CFastTradeDlg::OnClientInfoUpdated)
	ON_BN_CLICKED(IDC_CHECK_ALWAYS_ON_TOP, &CFastTradeDlg::OnBnClickedCheckAlwaysOnTop)
	ON_BN_CLICKED(IDC_RADIO1, &CFastTradeDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CFastTradeDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CFastTradeDlg::OnBnClickedRadio3)
	ON_NOTIFY(UGN_SELCHANGED, IDC_GRID_ORDER_PANEL, &CFastTradeDlg::OnOrderPanelGridSelChange)
	ON_MESSAGE(WM_VTSFRM_SHOW_COMPLETED, &CFastTradeDlg::OnVTSFrmShowCompleted)
	ON_MESSAGE(WM_ORDER_PANEL_GRID_BUTTON_CLICKED, &CFastTradeDlg::OnOrderPanelGridButtonClicked)
	ON_MESSAGE(WM_ACCOUNT_BALANCE_GRID_BUTTON_CLICKED, &CFastTradeDlg::OnAccountBalanceGridButtonClicked)
	//ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOSSCUT, &CFastTradeDlg::OnDeltaposSpinLosscut)
	//ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_EARNCUT, &CFastTradeDlg::OnDeltaposSpinEarncut)
	ON_CONTROL(DK_SPIN_CHANGED, IDC_DKSPIN_EARNCUT, &CFastTradeDlg::OnEarnCutChanged)
	ON_CONTROL(DK_SPIN_CHANGED, IDC_DKSPIN_LOSSCUT, &CFastTradeDlg::OnLossCutChanged)
END_MESSAGE_MAP()

// CFastTradeDlg 메시지 처리기입니다.
int CFastTradeDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (VTSFrmWndChildResizableDlg::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}


BOOL CFastTradeDlg::OnInitDialog()
{
	VTSFrmWndChildResizableDlg::OnInitDialog();

	m_gridHoga.AttachGrid(this, IDC_GRID_HOGA);
#ifdef _MARKET_CONT_1	
	m_gridMarketContract.AttachGrid(this, IDC_GRID_MARKET_CONTRACT);
#endif
	m_gridOrderPanel.AttachGrid(this, IDC_GRID_ORDER_PANEL);
	m_gridOrderPanel.m_pOwner = this;

	m_gridCurrent.AttachGrid(this, IDC_GRID_CUR_INFO);
	m_gridTotHoga.AttachGrid(this, IDC_GRID_TOT_HOGA);
	m_gridContractProfit.AttachGrid(this, IDC_GRID_CONTRACT_PROFIT);
	m_gridAccountBalance.AttachGrid(this, IDC_GRID_ACCOUNT_BALANCE);
	
	m_gridHoga.SetDefFont(theApp.GetFontDefault());
	m_gridOrderPanel.SetDefFont(theApp.GetFontDefault());
	m_gridCurrent.SetDefFont(theApp.GetFontDefault());
	m_gridTotHoga.SetDefFont(theApp.GetFontDefault());
	m_gridContractProfit.SetDefFont(theApp.GetFontDefault());
	m_gridAccountBalance.SetDefFont(theApp.GetFontDefault());

	//VTSFrmWndChildResizableDlg::AddAnchor(IDC_GRID_HOGA, __RDA_LT, __RDA_RB);

	if (NULL == (HFONT)m_fontGridBold)
	{
		CFont* pFont = m_gridHoga.GetFont(0);
		LOGFONT logFont;
		pFont->GetLogFont(&logFont);
		logFont.lfWeight = 1000;
		m_fontGridBold.CreateFontIndirect(&logFont);
	}

	InitControls();
	m_tabMerchandise.SetCurSel(0);

	SER()->RegisterOrderOutstanding(this);
	SER()->RegisterOrderContracted(this);
	SER()->RegisterClientInfo(this);



	/////////////////////////////////////////////////////
	CRect rt;
	GetClientRect(rt);

	CRect rtHoga;
	m_gridHoga.GetClientRect(rtHoga);

	m_nSmallRight = rtHoga.right +
#if THEME == THEME_DEFAULT || THEME == THEME_LINETRADE
		0;
#else
		3;
#endif

	m_nCurRight = m_nSmallRight;
	m_nExpandAreaWidth = rt.right - m_nSmallRight;

	rt.right = m_nSmallRight;

	m_nSmallHeight = rt.bottom;
	m_nCurHeight = m_nSmallHeight;

	MoveWindow(rt);

	OnOrderContracts(0, 0);
	OnOrderOutstanding(0, -1);

	/////////////////////////////////////////////////////
	GetAutoOrderManager()->AttachObserver(dynamic_cast<AutoOrderManagerObserver*>(this));
	OnPreferencesChanged(0, 0);
	return TRUE;
}
LRESULT CFastTradeDlg::OnVTSFrmShowCompleted(WPARAM wParam, LPARAM lParam)
{
	OnMerchandiseTabSelChange();
	OnClientInfoUpdated(0, 0);

	return 0;
}

void CFastTradeDlg::SetResize(SizeMode nMode)
{
	//오른쪽 숨겨져 있던 영역의 컨트롤 들을 모두 재배치 한다.
	if (m_nSizeMode == nMode)
		return;

	int nPrevRight = m_nCurRight;
	int nPrevHeight = m_nCurHeight;

	int nHogaHeight = 0;
	switch (nMode)
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

	MOVE(IDC_BUTTON_SELECT_CODE)
		MOVE(IDC_TAB_SET_DISPLAY);
	MOVE(IDC_CHECK_ALWAYS_ON_TOP);
	MOVE(IDC_CHECK_ONECLICK_ORDER);
	MOVE(IDC_RADIO1);
	MOVE(IDC_RADIO2);
	MOVE(IDC_RADIO3);
	MOVE(IDC_TAB_CURRENT_INFO);
	MOVE(IDC_GRID_CUR_INFO);
	MOVE(IDC_TAB_MARKET_CONTRACT);
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

	for (int nCol = 0; nCol < nColCount; ++nCol)
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

		for (int nCol = 0; nCol < COL_COUNT; ++nCol)
		{
			if (nCol == COL_COUNT - 1)
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

		for (int nCol = 0; nCol < COL_COUNT; ++nCol)
		{
			if (nCol == COL_COUNT - 1)
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
		for (int nCol = 0; nCol < COL_COUNT; ++nCol)
		{
			if (nCol == COL_COUNT - 1)
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

	CSize sz(0, 80);

	AddAnchor(IDC_GRID_HOGA, __RDA_LT, sz);
	AddAnchor(IDC_GRID_MARKET_CONTRACT, __RDA_LT, __RDA_LB);

	//사이즈 변경없이 아래쪽으로 움직일때 따라서 움직인다.
	AddAnchor(IDC_GRID_TOT_HOGA, sz);
	AddAnchor(IDC_BUTTON_SELL_MIT_CANCEL, sz);
	AddAnchor(IDC_BUTTON_SELL_CANCEL, sz);
	AddAnchor(IDC_BUTTON_ALL_CANCEL, sz);
	AddAnchor(IDC_BUTTON_BUY_CANCEL, sz);
	AddAnchor(IDC_BUTTON_BUY_MIT_CANCEL, sz);
	AddAnchor(IDC_GRID_ORDER_PANEL, sz, __RDA_LB);

	AddAnchor(IDC_STATIC_REAL_EVAL, __RDA_LB);
	AddAnchor(IDC_STATIC_PROFIT_REALIZE, __RDA_LB);
	AddAnchor(IDC_STATIC_EVAL, __RDA_LB);
	AddAnchor(IDC_STATIC_PROFIT_EVAL, __RDA_LB);
	AddAnchor(IDC_GRID_OUTSTANDING_ORDER, __RDA_LB);
	AddAnchor(IDC_GRID_MIT, __RDA_LB);

	GetClientRect(rt);
	rt.right += nMove;
	rt.bottom = m_nCurHeight;
	//MoveWindow(rt);
	GetParent()->SendMessage(WM_VTSFRM_CHILD_RESIZE_REQUEST, (WPARAM)rt.Width(), (LPARAM)rt.Height());

	m_nSizeMode = nMode;
}


void CFastTradeDlg::InitControls()
{
	m_penGridHeaderBorder.CreatePen(PS_SOLID, 1, RGB(45, 46, 54));

	DWORD dwAddExBorderStyles = WS_EX_STATICEDGE;
	DWORD dwRemoveExBorderStyles = WS_EX_STATICEDGE;

	m_tabMerchandise.AddTab(_T("선물"));
	m_tabMerchandise.AddTab(_T("옵션"));
	m_tabMerchandise.AddTab(_T("야간선물"));
	m_tabMerchandise.AddTab(_T("야간옵션"));
	m_tabMerchandise.AddTab(_T("해외선물"));

	m_tabSetDisplay.SetSelectTabColor(RGB(247, 150, 70));
	m_tabSetDisplay.AddTab(_T("화면설정"));

	m_tabCurrentInfo.SetSelectTabColor(RGB(146, 208, 80));
	m_tabCurrentInfo.AddTab(_T("가격정보"));

	m_tabMarketContract.SetSelectTabColor(RGB(0, 176, 240));
	m_tabMarketContract.AddTab(_T("체결정보"));

	m_labels[LBL_SELL].SetBkColor(CR_SELL);
	m_labels[LBL_SELL].SetTextColor(true, Colors::White);
	m_labels[LBL_BUY].SetBkColor(CR_BUY);
	m_labels[LBL_BUY].SetTextColor(true, Colors::White);
	
	m_labels[LBL_SEL_CODE].SetFontBold();

	m_labels[LBL_ENABLE_SELL].SetBkColor(Colors::White);
	m_labels[LBL_ENABLE_SELL].SetTextColor(true, Colors::Black);
	m_labels[LBL_ENABLE_BUY].SetBkColor(Colors::White);
	m_labels[LBL_ENABLE_BUY].SetTextColor(true, Colors::Black);

	CString strImgRoot = ENV().GetImgRoot();

	m_buttonSelCode.SetSkin(strImgRoot + _T("BTN_SELECT_CODE_N.png"), strImgRoot + _T("BTN_SELECT_CODE_C.png"), strImgRoot + _T("BTN_SELECT_CODE_H.png"), NULL, NULL, COLOR_IMG_TRANSPARENT, 0, 0);

	m_buttons[BTN_MARKET_SELL].SetBkColor(RGB(135,184,250));
	m_buttons[BTN_MARKET_SELL].SetTextColorNormal(RGB(0,10,37));

	m_buttons[BTN_MARKET_BUY].SetBkColor(RGB(235,177,176));
	m_buttons[BTN_MARKET_BUY].SetTextColorNormal(RGB(0,10,37));

	m_buttons[BTN_ALL_LIQUID].SetBkColor(RGB(229,187,173));
	m_buttons[BTN_ALL_LIQUID].SetTextColorNormal(RGB(0,10,37));
	m_buttons[BTN_ALL_CANCEL].SetBkColor(RGB(229,187,173));
	m_buttons[BTN_ALL_CANCEL].SetTextColorNormal(RGB(0,10,37));


	m_buttons[BTN_SELL_CANCEL].SetBkColor(RGB(135,184,250));
	m_buttons[BTN_SELL_CANCEL].SetTextColorNormal(RGB(1,28,115));
	m_buttons[BTN_SELL_MIT_CANCEL].SetBkColor(RGB(135,184,250));
	m_buttons[BTN_SELL_MIT_CANCEL].SetTextColorNormal(RGB(1,28,115));

	m_buttons[BTN_BUY_CANCEL].SetBkColor(RGB(234,178,177));
	m_buttons[BTN_BUY_CANCEL].SetTextColorNormal(RGB(157,47,50));
	m_buttons[BTN_BUY_MIT_CANCEL].SetBkColor(RGB(234,178,177));
	m_buttons[BTN_BUY_MIT_CANCEL].SetTextColorNormal(RGB(157,47,50));


	InitHoga();
	InitContractProfitGrid();
	InitAccountBalanceGrid();
	InitMarketContract();
	InitOrderPanel();
	InitCurrentInfo();

	CSize sz(0, 80);

	AddAnchor(IDC_GRID_HOGA, __RDA_LT, sz);
	AddAnchor(IDC_GRID_MARKET_CONTRACT, __RDA_LT, __RDA_LB);
	//사이즈 변경없이 아래쪽으로 움직일때 따라서 움직인다.
	AddAnchor(IDC_GRID_TOT_HOGA, sz);
	AddAnchor(IDC_BUTTON_SELL_MIT_CANCEL, sz);
	AddAnchor(IDC_BUTTON_SELL_CANCEL, sz);
	AddAnchor(IDC_BUTTON_ALL_CANCEL, sz);
	AddAnchor(IDC_BUTTON_BUY_CANCEL, sz);
	AddAnchor(IDC_BUTTON_BUY_MIT_CANCEL, sz);
	AddAnchor(IDC_GRID_ORDER_PANEL, sz, __RDA_LB);

	AddAnchor(IDC_STATIC_REAL_EVAL, __RDA_LB);
	AddAnchor(IDC_STATIC_PROFIT_REALIZE, __RDA_LB);
	AddAnchor(IDC_STATIC_EVAL, __RDA_LB);
	AddAnchor(IDC_STATIC_PROFIT_EVAL, __RDA_LB);
	AddAnchor(IDC_GRID_OUTSTANDING_ORDER, __RDA_LB);
	AddAnchor(IDC_GRID_MIT, __RDA_LB);


	m_spinVol.SetRange(1, 100);
	m_editVol.SetWindowText(_T("1"));

	m_spinEditEarnCut.SetEditReadOnly(TRUE);
	m_spinEditLossCut.SetEditReadOnly(TRUE);

	m_spinEditEarnCut.SetNumber(5);
	m_spinEditLossCut.SetNumber(5);

	/*m_spinLosscut.SetRange(1, 100);
	m_spinEarncut.SetRange(1, 100);
	m_editLosscut.SetWindowText(_T("5"));
	m_editEarncut.SetWindowText(_T("5"));

	m_editLosscut.SetReadOnly(TRUE);
	m_editEarncut.SetReadOnly(TRUE);*/

	m_buttonExpand.SetTooltipText(_T("창을 확장합니다"));

	m_buttonExpand.SetBkColor(Colors::Orange);
	m_buttonExpand.SetTextColorNormal(Colors::Black);

#if THEME == THEME_DEFAULT || THEME == THEME_LINETRADE
	for (int i = 0; i < EXT_BTN_COUNT; ++i)
		m_extButtons[i].SetTextColorNormal(Colors::White);

	//	GetDlgItem(IDC_CHECK_ALWAYS_ON_TOP)->ShowWindow(SW_HIDE);
#endif

	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(BST_CHECKED);
}

void CFastTradeDlg::InitContractProfitGrid()
{

	ASSERT(fld_cp_count == sizeof(g_colsContProfit) / sizeof(GridColumnItem));
	const int COL_COUNT = fld_cp_count;

	WINDOWPLACEMENT wp;
	CRect rt;

	m_gridContractProfit.GetWindowPlacement(&wp);
	wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + 38;
	m_gridContractProfit.SetWindowPlacement(&wp);

	m_gridContractProfit.SetDoubleBufferMode(FALSE);
	m_gridContractProfit.SetDefRowHeight(18);
	m_gridContractProfit.m_GI->m_userSizingMode = FALSE;
	m_gridContractProfit.SetCurrentCellMode(4);

#if THEME == THEME_DEFAULT || THEME == THEME_LINETRADE
	delete m_gridContractProfit.m_GI->m_defBorderPen;
	m_gridContractProfit.m_GI->m_defBorderPen = new CPen(PS_SOLID, 1, RGB(192, 192, 192));
	m_gridContractProfit.m_threeDLightPen.DeleteObject();
	m_gridContractProfit.m_threeDDarkPen.DeleteObject();
	m_gridContractProfit.m_threeDLightPen.CreatePen(PS_SOLID, 1, RGB(129, 131, 140));
	m_gridContractProfit.m_threeDDarkPen.CreatePen(PS_SOLID, 1, RGB(45, 46, 54));
#endif


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
	//cell.SetBackColor(CR_DEF_GRID_HEADER_BG);
	//cell.SetTextColor(CR_DEF_GRID_HEADER_TEXT);
	cell.SetBackColor(RGB(227,227,227));
	cell.SetTextColor(RGB(0,0,0));
	cell.SetFont(theApp.GetFontDefault());
	m_gridContractProfit.SetHeadingDefault(&cell);

	m_gridContractProfit.GetClientRect(rt);
	int w = rt.Width();
	int nLeft = 0;
	int nWidth = 0;

	for (int nCol = 0; nCol < COL_COUNT; ++nCol)
	{
		m_gridContractProfit.QuickSetText(nCol, -1, g_colsContProfit[nCol].n);
		if (nCol == COL_COUNT - 1)
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

	m_buttons[BTN_ALL_LIQUID].SetTooltipText(_T("보유중인 모든 포지션을 시장가로 일괄청산합니다."));
	m_buttons[BTN_ALL_CANCEL].SetTooltipText(_T("모든 종목의 미체결 주문과 현재 선택된 종목의 MIT주문을 취소합니다."));
}

void CFastTradeDlg::InitAccountBalanceGrid()
{
	const int COL_COUNT = sizeof(g_colsAccBal) / sizeof(GridColumnItem);

	WINDOWPLACEMENT wpCP;
	m_gridContractProfit.GetWindowPlacement(&wpCP);

	WINDOWPLACEMENT wp;
	CRect rt;
	m_gridAccountBalance.GetWindowPlacement(&wp);
	wp.rcNormalPosition.top = wpCP.rcNormalPosition.bottom + 2;
	wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + 38;
	m_gridAccountBalance.SetWindowPlacement(&wp);

	m_gridAccountBalance.SetDoubleBufferMode(FALSE);
	m_gridAccountBalance.SetDefRowHeight(18);
	m_gridAccountBalance.m_GI->m_userSizingMode = FALSE;
	m_gridAccountBalance.SetCurrentCellMode(4);

#if THEME == THEME_DEFAULT || THEME == THEME_LINETRADE
	delete m_gridAccountBalance.m_GI->m_defBorderPen;
	m_gridAccountBalance.m_GI->m_defBorderPen = new CPen(PS_SOLID, 1, RGB(192, 192, 192));
	m_gridAccountBalance.m_threeDLightPen.DeleteObject();
	m_gridAccountBalance.m_threeDDarkPen.DeleteObject();
	m_gridAccountBalance.m_threeDLightPen.CreatePen(PS_SOLID, 1, RGB(129, 131, 140));
	m_gridAccountBalance.m_threeDDarkPen.CreatePen(PS_SOLID, 1, RGB(45, 46, 54));
#endif

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
	//cell.SetBackColor(CR_DEF_GRID_HEADER_BG);
	//cell.SetTextColor(CR_DEF_GRID_HEADER_TEXT);
	//cell.SetXPStyle(XPCellTypeBorder);
	/*
	cell.SetXPStyle(XPCellTypeData);
	cell.SetCellType(m_gridAccountBalance.m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	*/
	cell.SetBackColor(RGB(227,227,227));
	cell.SetTextColor(RGB(0,0,0));
	cell.SetFont(theApp.GetFontDefault());
	m_gridAccountBalance.SetHeadingDefault(&cell);

	m_gridAccountBalance.GetClientRect(rt);
	int w = rt.Width();
	int nLeft = 0;
	int nWidth = 0;

	for (int nCol = 0; nCol < COL_COUNT; ++nCol)
	{
		m_gridAccountBalance.QuickSetText(nCol, -1, g_colsAccBal[nCol].n);
		if (nCol == COL_COUNT - 1)
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

	m_gridAccountBalance.m_pOwner = this;
	m_gridAccountBalance.GetCell(5, 0, &cell);
	cell.SetText(GetSession()->enableOvernight ? _T("신청") : _T("해제"));
	cell.SetCellType(m_gridAccountBalance.m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetTextColor(Colors::Black);
	cell.SetBackColor(Colors::Khaki);
	m_gridAccountBalance.SetCell(fld_ab_OverSet, 0, &cell);
}

const int nCurrentInfoRowCount = 7;
void CFastTradeDlg::InitCurrentInfo()
{
	const int nColCount = 1;
	const int nRowHeight = 18;

	CRect rt;
	m_gridCurrent.GetClientRect(rt);

	WINDOWPLACEMENT wp;
	m_gridCurrent.GetWindowPlacement(&wp);
	wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + (nRowHeight * nCurrentInfoRowCount);
	m_gridCurrent.SetWindowPlacement(&wp);

	m_gridCurrent.SetDoubleBufferMode(FALSE);
	m_gridCurrent.SetCurrentCellMode(4);
	m_gridCurrent.SetDefRowHeight(18);
	m_gridCurrent.m_GI->m_userSizingMode = FALSE;
	m_gridCurrent.m_GI->m_hdgDefaults->SetBorder(UG_BDR_RECESSED);

#if THEME == THEME_DEFAULT || THEME == THEME_LINETRADE
	delete m_gridCurrent.m_GI->m_defBorderPen;
	m_gridCurrent.m_GI->m_defBorderPen = new CPen(PS_SOLID, 1, RGB(192, 192, 192));
	m_gridCurrent.m_threeDLightPen.DeleteObject();
	m_gridCurrent.m_threeDDarkPen.DeleteObject();
	m_gridCurrent.m_threeDLightPen.CreatePen(PS_SOLID, 1, RGB(192, 192, 192));
	m_gridCurrent.m_threeDDarkPen.CreatePen(PS_SOLID, 1, RGB(192, 192, 192));;//RGB(192, 192, 192) );
#endif

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
	cell.SetFont(theApp.GetFontDefault());
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

void CFastTradeDlg::InitHoga()
{
	CUGCell cell;

	m_gridHoga.m_pTradeDlg = this;
	m_gridHoga.Init();

	m_gridHoga.GetHeadingDefault(&cell);
	cell.SetFont(theApp.GetFontDefault());
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

	for (int nCol = 0; nCol < nColCount; ++nCol)
	{
		int nWidth = m_gridHoga.GetColWidth(nCol);
		m_gridTotHoga.SetColWidth(nCol, nWidth);
		nLeft += nWidth;
	}
	int nRightHoga = rtHoga.right - m_gridHoga.m_GI->m_vScrollWidth;

	//호가창 상단 버튼
	int nBtnWidth = 0;
	GetDlgItem(IDC_BUTTON_MARKET_SELL)->GetWindowRect(rt); ScreenToClient(rt); nBtnWidth = rt.Width();

	int nBottom = rtHoga.top - 2;
	int nTop = nBottom - rt.Height();

	rt.left = rtHoga.left; rt.right = rt.left + nBtnWidth; rt.top = nTop; rt.top = nTop; rt.bottom = nBottom;
	GetDlgItem(IDC_BUTTON_MARKET_SELL)->MoveWindow(rt);

	GetDlgItem(IDC_BUTTON_ALL_LIQUID)->GetWindowRect(rt); ScreenToClient(rt); nBtnWidth = rt.Width();
	rt.left = rtHoga.left + ((nRightHoga - rtHoga.left) / 2) - (nBtnWidth / 2);
	rt.right = rt.left + nBtnWidth;  rt.top = nTop; rt.bottom = nBottom;
	GetDlgItem(IDC_BUTTON_ALL_LIQUID)->MoveWindow(rt);

	GetDlgItem(IDC_BUTTON_MARKET_BUY)->GetWindowRect(rt); ScreenToClient(rt); nBtnWidth = rt.Width();
	rt.left = nRightHoga - nBtnWidth;
	rt.right = nRightHoga;  rt.top = nTop; rt.bottom = nBottom;
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
	m_gridTotHoga.QuickSetText(c_buy, 0, _T("0")); m_gridTotHoga.RedrawCell(c_buy, 0);
	m_gridTotHoga.QuickSetText(c_mit_buy, 0, _T("0")); m_gridTotHoga.RedrawCell(c_mit_buy, 0);
}

void CFastTradeDlg::InitMarketContract()
{
	const int nColCount = 3;
	m_gridMarketContract.SetDefaultStyle(VTSGrid::sel_unable, false);
	m_gridMarketContract.DefaultRowHeightSet(18);

	VTSGridColDefs defs;
	defs.push_back(std::make_pair(_T("시간"), 0.36));
	defs.push_back(std::make_pair(_T("체결가"), 0.30));
	defs.push_back(std::make_pair(_T("수량"), 0.25));

	m_gridMarketContract.SetColumDef(defs, false, 0, COLOR_GRID_HEADER_BACK1);
	m_gridMarketContract.RowAdd(GTCurrent::MAX_QUEUE);

	for (int i = 0; i < GTCurrent::MAX_QUEUE; ++i)
	{
		m_gridMarketContract.QuickSetBackColor(1, i, RGB(255, 255, 204));
		m_gridMarketContract.SetAlign(1, i, VTSGrid::align_center);
		m_gridMarketContract.SetAlign(2, i, VTSGrid::align_center);
	}
}

void CFastTradeDlg::InitOrderPanel()
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

	m_gridOrderPanel.GetColDefault(5, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	cell.SetFont(&m_fontGridBold);
	m_gridOrderPanel.SetColDefault(5, &cell);

	// Header색
	m_gridOrderPanel.GetHeadingDefault(&cell);
	cell.SetBackColor(COLOR_GRID_HEADER_BACK1);
	//cell.SetTextColor(CR_DEF_GRID_HEADER_TEXT);
	cell.SetFont(theApp.GetFontDefault());
	m_gridOrderPanel.SetHeadingDefault(&cell);

	CRect rt;
	m_gridOrderPanel.GetClientRect(rt);
	int w = rt.Width() - m_gridOrderPanel.m_GI->m_vScrollWidth;
	int nLeft = 0;
	int nWidth = 0;

	for (int nCol = 0; nCol < COL_COUNT; ++nCol)
	{
		m_gridOrderPanel.QuickSetText(nCol, -1, g_colsOP[nCol].n);
		if (nCol == COL_COUNT - 1)
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

void CFastTradeDlg::SetEnableOrderCount()
{
	if (NULL == m_pSelCode)
		return;
	GetSession()->RequestEnableOrderVolume(GetSafeHwnd(), m_pSelCode, m_curMarketType, m_pMerch->GetCurrent()->price.ToString());
}

LRESULT CFastTradeDlg::OnEnableOrderCountReceived(WPARAM wParam, LPARAM lParam)
{
	EanbleOrderItem* pItem = (EanbleOrderItem*)wParam;

	if (m_pSelCode == pItem->pCode)
	{
		CString s;
		s.Format(_T("%d "), pItem->nSellCount);
		m_labels[LBL_ENABLE_SELL].SetWindowText(s);
		s.Format(_T("%d "), pItem->nBuyCount);
		m_labels[LBL_ENABLE_BUY].SetWindowText(s);
	}

	delete pItem;
	return 0;
}

void CFastTradeDlg::OnBnClickedSelectCode()
{
	BOOL bDeleteOptionsTable = TRUE;
	Code* pCode = NULL;
	if (ct_futures == m_curMerchandiseType)
	{
	}
	else if (ct_options == m_curMerchandiseType)
	{
		bDeleteOptionsTable = FALSE;
		if (NULL == m_pOptionsTableDlg)
		{
			m_pOptionsTableDlg = new COptionsTableDlg(this, mt_EUREX == m_curMarketType);
			m_pOptionsTableDlg->m_pOwner = this;
			m_pOptionsTableDlg->Create(COptionsTableDlg::IDD, this);
		}
		m_pOptionsTableDlg->ShowWindow(SW_SHOW);
	}
	else if (ct_foreign_futures == m_curMerchandiseType)
	{
		if (NULL == m_pForeignFuturesCodeDlg)
		{
			m_pForeignFuturesCodeDlg = new CForeignFuturesCodeDlg(this);
			m_pForeignFuturesCodeDlg->m_pOwner = this;
			m_pForeignFuturesCodeDlg->Create(CForeignFuturesCodeDlg::IDD, this);
		}
		m_pForeignFuturesCodeDlg->ShowWindow(SW_SHOW);
	}

	if (NULL == pCode)
		return;

	SetMerchandise(pCode, m_curMarketType);
}


void CFastTradeDlg::OnCurrentInitUpdated(GTCurrentPtr p)
{
	if (m_bCurrentInit)
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

	m_gridCurrent.RedrawCell(0, 5);
	m_gridCurrent.RedrawCell(0, 6);

	SetEnableOrderCount();
	UpdateGridAccountPanel(ap_contract);
	OnCurrentUpdated(p);
	MITTotalUpdate();
	m_bCurrentInit = TRUE;

	m_pMerch->RunHogaRecv();
}

void CFastTradeDlg::UpdateGridAccountPanel(int nCase, OrderContracted* pOrderUpdated)
{
	// 0:체결됐을때, 1:손익 바뀌었을때, 2:계좌 잔고 바뀌었을때
	const OrderContracts& conts = SER()->GetContracts();
	Profit* pProfit = SER()->GetProfit();

	if (nCase & ap_contract)
	{
		BOOL bMatched = FALSE;
		for (auto pos = conts.begin(); pos != conts.end(); ++pos)
		{
			OrderContracted* pOrder = (*pos).get();
			if (pOrder->pCode == m_pSelCode)
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

				bMatched = TRUE;
				break;
			}
		}

		if (FALSE == bMatched)
		{
			m_gridContractProfit.QuickSetText(fld_cp_ContratedVolume, 0, _T(""));
			m_gridContractProfit.QuickSetText(fld_cp_PriceAverage, 0, _T(""));
			m_gridContractProfit.QuickSetText(fld_cp_EvalProfit, 0, _T(""));

			m_gridContractProfit.RedrawCell(fld_cp_ContratedVolume, 0);
			m_gridContractProfit.RedrawCell(fld_cp_PriceAverage, 0);
			m_gridContractProfit.RedrawCell(fld_cp_EvalProfit, 0);
		}
		if (pProfit)
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


			//평가담보금
			GT_CURRENCY accountBalanceEval = GetSession()->get_bankBalance() + pProfit->profitEvalSum;
			m_gridAccountBalance.QuickSetText(fld_ab_EvalBalance, 0, ::ToThousand(accountBalanceEval));
			m_gridAccountBalance.RedrawCell(fld_ab_EvalBalance, 0);

			//로스컷
			m_gridAccountBalance.QuickSetText(fld_ab_Losscut, 0, ::ToThousand(pProfit->lossCut));
			m_gridAccountBalance.RedrawCell(fld_ab_Losscut, 0);

			m_gridAccountBalance.QuickSetText(fld_ab_FuturesOver, 0, ::ToThousand(pProfit->depositForFuturesOvernightTot));
			m_gridAccountBalance.RedrawCell(fld_ab_FuturesOver, 0);

			m_gridAccountBalance.QuickSetText(fld_ab_OptionsOver, 0, ::ToThousand(pProfit->depositForOptionsOvernightTot));
			m_gridAccountBalance.RedrawCell(fld_ab_OptionsOver, 0);

			m_gridAccountBalance.QuickSetText(fld_ab_OverSum, 0, ::ToThousand(pProfit->depositForFuturesOvernightTot + pProfit->depositForOptionsOvernightTot + pProfit->depositForForeignFuturesOvernightTot));
			m_gridAccountBalance.RedrawCell(fld_ab_OverSum, 0);
		}
		else
		{
			m_gridContractProfit.QuickSetText(fld_cp_EvalProfitSum, 0, _T(""));
			m_gridContractProfit.QuickSetTextColor(fld_cp_EvalProfitSum, 0, Colors::Black);
			m_gridContractProfit.RedrawCell(fld_cp_EvalProfitSum, 0);
		}

	}

	if (nCase & ap_profit)
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
		m_gridAccountBalance.QuickSetText(fld_ab_EvalBalance, 0, ::ToThousand(accountBalanceEval));
		m_gridAccountBalance.RedrawCell(fld_ab_EvalBalance, 0);

		m_gridAccountBalance.QuickSetText(fld_ab_FuturesOver, 0, ::ToThousand(pProfit->depositForFuturesOvernightTot));
		m_gridAccountBalance.RedrawCell(fld_ab_FuturesOver, 0);

		m_gridAccountBalance.QuickSetText(fld_ab_OptionsOver, 0, ::ToThousand(pProfit->depositForOptionsOvernightTot));
		m_gridAccountBalance.RedrawCell(fld_ab_OptionsOver, 0);

		m_gridAccountBalance.QuickSetText(fld_ab_OverSum, 0, ::ToThousand(pProfit->depositForFuturesOvernightTot + pProfit->depositForOptionsOvernightTot + pProfit->depositForForeignFuturesOvernightTot));
		m_gridAccountBalance.RedrawCell(fld_ab_OverSum, 0);

		if (m_pSelCode == pOrderUpdated->pCode)
		{
			m_gridContractProfit.QuickSetText(fld_cp_EvalProfit, 0, ::ToThousand(pOrderUpdated->profitEval));
			m_gridContractProfit.QuickSetTextColor(fld_cp_EvalProfit, 0, ::GetProfitColor(pOrderUpdated->profitEval));
			m_gridContractProfit.RedrawCell(fld_cp_EvalProfit, 0);
		}
	}

	if (nCase & ap_balance)
	{
		GT_CURRENCY accountBalanceEval = GetSession()->get_bankBalance();
		if (pProfit)
		{
			//ASSERT(pProfit->profitRealize == GetSession()->todayProfitRealized);
			m_gridAccountBalance.QuickSetText(fld_ab_Losscut, 0, ::ToThousand(pProfit->lossCut));
			m_gridAccountBalance.RedrawCell(fld_ab_Losscut, 0);

			accountBalanceEval += pProfit->profitEvalSum;
		}

		m_gridContractProfit.QuickSetText(fld_cp_RealizedProfit, 0, ::ToThousand(GetSession()->todayProfitRealized));
		m_gridContractProfit.QuickSetTextColor(fld_cp_RealizedProfit, 0, ::GetProfitColor(GetSession()->todayProfitRealized));
		m_gridContractProfit.RedrawCell(fld_cp_RealizedProfit, 0);

		m_gridAccountBalance.QuickSetText(fld_ab_EvalBalance, 0, ::ToThousand(accountBalanceEval));
		m_gridAccountBalance.RedrawCell(fld_ab_EvalBalance, 0);
	}

	if (pProfit)
	{

	}
}

void CFastTradeDlg::OnAutoOrderContractChanged(int nRowOld, int nRowNew, PositionType position)
{

}

void CFastTradeDlg::OnAutoOrderSetLossCut(BOOL bEnable, int nTick, int nRowOld, int nRowNew)
{
	int nCheck = bEnable ? BST_CHECKED : BST_UNCHECKED;
	//m_editLosscut.SetWindowText(::ToString(nTick));
	m_spinEditLossCut.SetNumber(nTick);
	((CButton*)GetDlgItem(IDC_CHECK_LOSSCUT))->SetCheck(nCheck);
}

void CFastTradeDlg::OnAutoOrderSetEarnCut(BOOL bEnable, int nTick, int nRowOld, int nRowNew)
{
	int nCheck = bEnable ? BST_CHECKED : BST_UNCHECKED;
	//m_editEarncut.SetWindowText(::ToString(nTick));
	m_spinEditEarnCut.SetNumber(nTick);
	((CButton*)GetDlgItem(IDC_CHECK_EARNCUT))->SetCheck(nCheck);
}

void CFastTradeDlg::OnLossCutChanged()
{
	int nCheck = m_checkLosscut.GetCheck();
	if (BST_CHECKED == nCheck)
	{
		OnBnClickedCheckLosscut();
	}
}

void CFastTradeDlg::OnEarnCutChanged()
{
	int nCheck = m_checkEarncut.GetCheck();
	if (BST_CHECKED == nCheck)
	{
		OnBnClickedCheckEarncut();
	}
}

//void HogaGrid::OnMITOrderUpdated(PositionType position, const CString& price, const CString& volume)
void CFastTradeDlg::OnAutoOrderMITChanged(PositionType posType, const CString& strPrice, GT_VOLUME volume)
{
	MITTotalUpdate();
}

void CFastTradeDlg::OnAutoOrderManagerOnMITUpdated(const AutoOrders& autoOrders)
{
	UpdateOrderPanel();
}


BOOL CFastTradeDlg::UpdateMarketContracts(GTCurrentPtr p)
{
	int nRow = 0;

#ifdef _MARKET_CONT_1 
	for (auto pos = p->m_listContracts.rbegin(); pos != p->m_listContracts.rend(); ++pos, ++nRow)
	{
		GTCurrent::ContractItem* pItem = (*pos).get();

		m_gridMarketContract.QuickSetText(0, nRow, pItem->time);

		m_gridMarketContract.QuickSetTextColor(1, nRow, pItem->pos == PT_BUY ? Colors::Red : Colors::Blue);
		m_gridMarketContract.QuickSetText(1, nRow, pItem->price);

		m_gridMarketContract.QuickSetTextColor(2, nRow, pItem->pos == PT_BUY ? Colors::Red : Colors::Blue);
		m_gridMarketContract.QuickSetText(2, nRow, pItem->volume);

		/*m_gridMarketContract.RedrawCell(0, nRow);
		m_gridMarketContract.RedrawCell(1, nRow);
		m_gridMarketContract.RedrawCell(2, nRow);*/
	}
	m_gridMarketContract.RedrawAll();
#else
	for (auto pos = p->m_listContracts.rbegin(); pos != p->m_listContracts.rend(); ++pos, ++nRow)
	{
		GTCurrent::ContractItem* pItem = (*pos).get();

		m_gridMarketContract.QuickSetText(0, nRow, pItem->time);
		if (m_pCurBase->IsUpdated())
			return FALSE;

		m_gridMarketContract.QuickSetTextColor(1, nRow, pItem->pos == PT_BUY ? Colors::Red : Colors::Blue);
		m_gridMarketContract.QuickSetText(1, nRow, pItem->price);
		if (m_pCurBase->IsUpdated())
			return FALSE;

		m_gridMarketContract.QuickSetTextColor(2, nRow, pItem->pos == PT_BUY ? Colors::Red : Colors::Blue);
		m_gridMarketContract.QuickSetText(2, nRow, pItem->volume);
		if (m_pCurBase->IsUpdated())
			return FALSE;
	}
	m_gridMarketContract.OnSwInvalidate(false);
#endif
	return TRUE;
}

void CFastTradeDlg::OnCurrentUpdated(GTCurrentPtr p)
{
	m_gridHoga.OnCurrentUpdated(p);

	if (FALSE == m_bExpand)
		return;

	if (!UpdateMarketContracts(p))
		return;
	UpdateCurrent(p);
}

void CFastTradeDlg::UpdateCurrent(GTCurrentPtr p)
{
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

	if (m_pCurBase->IsUpdated())
		return;

	m_gridCurrent.RedrawCell(0, 0);
	m_gridCurrent.RedrawCell(0, 1);
	m_gridCurrent.RedrawCell(0, 2);
	m_gridCurrent.RedrawCell(0, 3);
	m_gridCurrent.RedrawCell(0, 4);
}

void CFastTradeDlg::OnHogaInitUpdated(GTHogaPtr p)
{
	if (m_bHogaInit)
	{
		return;
	}

	m_gridHoga.OnHogaInitUpdated(p);

	m_bHogaInit = TRUE;

	UpdateTotalHoga(p);
	ContractApplyToHoga();
	OutstandingApplyToHoga();
}

void CFastTradeDlg::OnHogaUpdated(GTHogaPtr p)
{
	if (FALSE == m_bHogaInit)
	{
		return;
	}

	m_gridHoga.OnHogaUpdated(p);
	UpdateTotalHoga(p);
}

void CFastTradeDlg::UpdateTotalHoga(GTHogaPtr p)
{
	m_gridTotHoga.QuickSetText(c_sell_number, 0, ::ToThousand(p->totoffercnt.ToINT()));
	m_gridTotHoga.QuickSetText(c_sell_remain, 0, ::ToThousand(p->totofferrem.ToINT()));

	m_gridTotHoga.QuickSetText(c_buy_number, 0, ::ToThousand(p->totbidcnt.ToINT()));
	m_gridTotHoga.QuickSetText(c_buy_remain, 0, ::ToThousand(p->totbidrem.ToINT()));

	int nSub = p->totbidrem.ToINT() - p->totofferrem.ToINT();
	m_gridTotHoga.QuickSetTextColor(c_price, 0, ::GetProfitColor(nSub));
	m_gridTotHoga.QuickSetText(c_price, 0, ::ToThousand(abs(nSub)));
	m_gridTotHoga.RedrawRow(0);
}

void CFastTradeDlg::OnClose()
{
	ASSERT(FALSE);
	__super::OnClose();
}

CString CFastTradeDlg::GetCurVolume()
{
	CString s;
	m_editVol.GetWindowText(s);
	return s;
}

void CFastTradeDlg::OnHogaClickOrder(int col, int row, const CString& price)
{
	CString strVolume = GetCurVolume();

	if (c_buy == col || c_sell == col)
	{
		Order(c_sell == col ? PT_SELL : PT_BUY, ht_JiJeongGa, price, strVolume,
			m_bOneClick ? om_click : om_dbl_click);
	}
	else if (c_mit_buy == col || c_mit_sell == col)
	{
		int nOrder = ::ToINT(strVolume);
		PositionType posType = c_mit_sell == col ? PT_SELL : PT_BUY;
		CString strMsg;
		if (m_pAutoOrder->MIT_New(posType, price, nOrder, strMsg))
		{
			ORDER_ACTOR actor;
			CString strAction;
			strAction.Format(_T("%s: MIT신규"), OrderMethodStr(m_bOneClick ? om_click : om_dbl_click, actor));
			GetSession()->SendOrderAction(strAction, m_pSelCode->GetCode(), price, posType, strVolume);
		}
		else
		{
			ErrMsgBox(strMsg);
		}
	}
}

BOOL CFastTradeDlg::_Order(Code* pCode, MarketType marketType, PositionType posType, HogaType hoga,
	const CString& price, const CString& vol, OrderMethod method)
{
#ifdef _SHOW_CONFIRM_MSG
	if (ENV().Get(ENV_ORDER_CONFIRM_SHOW).ToINT() == TRUE && (method != om_mit && method != om_stop_losscut && method != om_stop_earncut))
	{
		CString str;
		str.Format(_T("%s(%s) %s 신규주문: 가격(%s), 수량(%s)\n주문 하시겠습니까?"),
			::MerchandiseTypeStr(pCode->GetType()), pCode->GetCode(), ::PositionTypeStr(posType),
			price, vol);

		if (MessageBox(str, _T("신규주문"), MB_ICONQUESTION | MB_OKCANCEL) != IDOK)
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

BOOL CFastTradeDlg::Order(PositionType posType, HogaType hoga, const CString& price, const CString& vol, OrderMethod method)
{
	if (NULL == m_pSelCode)
		return FALSE;
	return _Order(m_pSelCode, m_curMarketType, posType, hoga, price, vol, method);
}

BOOL CFastTradeDlg::OrderCorrection(PositionType posType, CString srcPrice, CString destPrice, CString vol, OrderMethod method)
{
	const OrderOutstandings& outstds = SER()->GetOutstandings();

#ifdef _SHOW_CONFIRM_MSG
	if (ENV().Get(ENV_ORDER_CONFIRM_SHOW).ToINT() == TRUE)
	{
		CString str;
		str.Format(_T("%s(%s) %s정정주문:\n수량(%s) 원주문가격(%s), 정정주문가격(%s)\n주문 하시겠습니까?"),
			::MerchandiseTypeStr(m_pSelCode->GetType()), m_pSelCode->GetCode(), ::PositionTypeStr(posType),
			vol, srcPrice, destPrice);

		if (MessageBox(str, _T("정정주문"), MB_ICONQUESTION | MB_OKCANCEL) != IDOK)
			return FALSE;
	}
#endif

	GT_VOLUME nTotVol = 0;
	vector<pair<int, GT_VOLUME> > ordNums;
	for (auto pos = outstds.begin(); pos != outstds.end(); ++pos)
	{
		OrderOutstanding*	pOrder = *pos;
		if (pOrder->position == posType && pOrder->strPrice == srcPrice)
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

	for (UINT i = 0; i < ordNums.size(); ++i)
	{
		if (!GetSession()->OrderCorrection(GetSafeHwnd(), ordNums[i].first, ht_JiJeongGa, destPrice, ::ToString(ordNums[i].second)))
			return FALSE;
	}

	ASSERT(vol == ::ToString(nTotVol));
	return TRUE;
}
BOOL CFastTradeDlg::OrderCancel(PositionType posType, CString srcPrice, CString vol, OrderMethod method)
{
	const OrderOutstandings& outstds = SER()->GetOutstandings();
#ifdef _SHOW_CONFIRM_MSG
	if (ENV().Get(ENV_ORDER_CONFIRM_SHOW).ToINT() == TRUE)
	{
		CString str;
		str.Format(_T("%s(%s) %s취소주문: 수량(%s) 가격(%s)\n주문을 취소하시겠습니까?"),
			::MerchandiseTypeStr(m_pSelCode->GetType()), m_pSelCode->GetCode(), ::PositionTypeStr(posType),
			vol, srcPrice);

		if (MessageBox(str, _T("취소주문"), MB_ICONQUESTION | MB_OKCANCEL) != IDOK)
			return FALSE;
	}
#endif
	GT_VOLUME nTotVol = 0;
	vector<pair<int, GT_VOLUME> > ordNums;
	for (auto pos = outstds.begin(); pos != outstds.end(); ++pos)
	{
		OrderOutstanding*	pOrder = *pos;
		if (pOrder->position == posType && pOrder->strPrice == srcPrice)
		{
			nTotVol += pOrder->nVolumeRemain;
			ordNums.push_back(make_pair(pOrder->nOrderNum, pOrder->nVolumeRemain));
		}
	}

	ORDER_ACTOR actor;
	CString strAction;
	strAction.Format(_T("%s: 취소주문"), ::OrderMethodStr(method, actor));
	GetSession()->SendOrderAction(strAction, m_pSelCode->GetCode(), srcPrice, posType, vol);

	for (UINT i = 0; i < ordNums.size(); ++i)
	{
		if (!GetSession()->OrderCancel(GetSafeHwnd(), ordNums[i].first, ::ToString(ordNums[i].second)))
			return FALSE;
	}

	ASSERT(vol == ::ToString(nTotVol));
	return TRUE;
}

LRESULT CFastTradeDlg::OnOrderOutstanding(WPARAM wParam, LPARAM lParam)
{
	//DLOG(_T(""));
	const OrderOutstandings& outstds = SER()->GetOutstandings();

	m_opOutstandings.clear();

	const int nNewIndex = (int)lParam;

	int nIndex = 0;
	OrderOutstanding* pOrderNew = NULL;
	OrderOutstanding* pOrderCancel = NULL;

	for (auto pos = outstds.begin(); pos != outstds.end(); ++pos, ++nIndex)
	{
		OrderOutstanding*	pOrder = *pos;

		if (pOrder->nVolumeRemain == 0)
		{
			pOrderCancel = pOrder;
			continue;
		}

		OrderPanelItem* pOPItem = new OrderPanelItem;

		pOPItem->pCode = pOrder->pCode;
		pOPItem->marketType = pOrder->marketType;
		pOPItem->strPrice = pOrder->strPrice;
		pOPItem->position = pOrder->position;
		pOPItem->nVol = pOrder->nVolume;
//		pOPItem->profitEval = 
		pOPItem->nOrdNum = pOrder->nOrderNum;
		pOPItem->nVolRemain = pOrder->nVolumeRemain;

		if (nIndex == nNewIndex)
		{
			pOrderNew = pOrder;
		}
		m_opOutstandings.push_back(OrderPanelItemPtr(pOPItem));
	}

#ifdef ENABLE_HOGA_FAKE_UPDATE
	if (pOrderNew && pOrderNew->pCode == m_pSelCode)
	{
		if (pOrderNew->bHogaFakeUpdate)
		{
			m_gridHoga.ApplyHogaFakeUpdate(m_pMerch->GetHoga(), pOrderNew);
		}
		else if (pOrderNew->bRealOrderFailed)
		{
			m_gridHoga.ApplyRealOrderFailed(m_pMerch->GetHoga(), pOrderNew);
		}
	}

	if (pOrderCancel && pOrderCancel->pCode == m_pSelCode)
	{
		if (pOrderCancel->bHogaFakeUpdate)
		{
			m_gridHoga.ApplyHogaFakeUpdate(m_pMerch->GetHoga(), pOrderCancel);
		}
		else if (pOrderCancel->bRealOrderFailed)
		{
			m_gridHoga.ApplyRealOrderFailed(m_pMerch->GetHoga(), pOrderCancel);
		}
	}
#endif

	OutstandingApplyToHoga();
	UpdateOrderPanel();
	SetEnableOrderCount();
	//DLOG(_T(""));
	return 0;
}

void CFastTradeDlg::OutstandingApplyToHoga()
{
	if (FALSE == m_bHogaInit)
		return;
	if (NULL == m_pSelCode)
		return;

	const OrderOutstandings& outstds = SER()->GetOutstandings();

	typedef map<pair<PositionType, CString>, GT_VOLUME>		OutstdMap;
	OutstdMap	outMap;
	int nBuy = 0, nSell = 0;
	for (auto pos = outstds.begin(); pos != outstds.end(); ++pos)
	{
		OrderOutstanding*	pOrder = *pos;

		if (m_pSelCode != pOrder->pCode)
			continue;

		pair<PositionType, CString> key(pOrder->position, pOrder->strPrice);
		auto posOut = outMap.find(key);
		if (outMap.end() == posOut)
			outMap.insert(make_pair(key, pOrder->nVolumeRemain));
		else
			posOut->second += pOrder->nVolumeRemain;

		if (pOrder->nVolumeRemain == 0)
			continue;

		if (PT_BUY == pOrder->position)
			nBuy += pOrder->nVolumeRemain;
		else
			nSell += pOrder->nVolumeRemain;
	}

	CString s;
	for (auto pos = outMap.begin(); pos != outMap.end(); ++pos)
	{
		if (pos->second == 0)
			s = _T("");
		else
			s = ::ToString(pos->second);

		m_gridHoga.OnOrderUpdated(pos->first.first, pos->first.second, s);
	}
	s = ::ToString(nSell);
	m_gridTotHoga.QuickSetText(c_sell, 0, s); m_gridTotHoga.RedrawCell(c_sell, 0);
	s = ::ToString(nBuy);
	m_gridTotHoga.QuickSetText(c_buy, 0, s); m_gridTotHoga.RedrawCell(c_buy, 0);
}
LRESULT CFastTradeDlg::OnContractedOrder(WPARAM wParam, LPARAM lParam)
{
	ContractedOrder* pCont = (ContractedOrder*)wParam;

	if (pCont->pCode == m_pSelCode)
	{
		GTCurrentPtr pCurrent = m_pMerch->GetCurrent();
		if (!pCurrent->price.ToString().IsEmpty())
		{
			COleDateTime tm = pCont->time;

			if (pCont->pCode->GetType() == ct_foreign_futures)
			{
				CodeForeignFutures* pCode = dynamic_cast<CodeForeignFutures*>(pCont->pCode);
				ASSERT(pCode);
				tm += COleDateTimeSpan(0, pCode->nHourGap, 0, 0);
			}
			pCurrent->AddContract(tm.Format(_T("%H:%M:%S")),
				pCont->strPrice, ::ToString(pCont->nVolume), pCont->posType);

			UpdateMarketContracts(pCurrent);
		}
	}

	return 0;
}

LRESULT CFastTradeDlg::OnOrderContracts(WPARAM wParam, LPARAM lParam)
{
	OrderContractNotifyItem* pNotf = (OrderContractNotifyItem*)wParam;

	const OrderContracts& conts = SER()->GetContracts();
	Profit* pProfit = SER()->GetProfit();

	m_opContracts.clear();
	int nRow = 0;
	for (auto pos = conts.begin(); pos != conts.end(); ++pos)
	{
		OrderContracted* pOrder = (*pos).get();
		OrderPanelItem* pOPItem = new OrderPanelItem;

		pOPItem->pCode = pOrder->pCode;
		pOPItem->marketType = pOrder->marketType;
		pOPItem->strPrice = pOrder->strPrice;
		pOPItem->position = pOrder->position;
		pOPItem->nVol = pOrder->nVolume;
		pOPItem->profitEval = pOrder->profitEval;
		pOPItem->nOrdNum = nRow++;

		m_opContracts.push_back(OrderPanelItemPtr(pOPItem));
	}

	SetEnableOrderCount();
	ContractApplyToHoga();
	UpdateGridAccountPanel(ap_contract | ap_balance);
	UpdateOrderPanel();

	return 0;
}

void CFastTradeDlg::ContractApplyToHoga()
{

}

LRESULT CFastTradeDlg::OnProfitUpdated(WPARAM wParam, LPARAM lParam)
{
	OrderContracted* pOrder = (OrderContracted*)wParam;

	auto pos = m_ordPanelContRowIdx.find(pOrder->pCode);
	ASSERT(pos != m_ordPanelContRowIdx.end());
	int nRow = pos->second;
	m_gridOrderPanel.QuickSetText(fld_op_Profit, nRow, ::ToThousand(pOrder->profitEval));
	m_gridOrderPanel.QuickSetTextColor(fld_op_Profit, nRow, ::GetProfitColor(pOrder->profitEval));
	m_gridOrderPanel.RedrawCell(fld_op_Profit, nRow);

	for (auto pos = m_opContracts.begin(); pos != m_opContracts.end(); ++pos)
	{
		OrderPanelItem* p = (*pos).get();
		if (p->pCode == pOrder->pCode)
		{
			p->profitEval = pOrder->profitEval;
			break;
		}
	}

	UpdateGridAccountPanel(ap_profit, pOrder);
	return 0;
}

void CFastTradeDlg::UpdateOrderPanel()
{
	m_gridOrderPanel.EnableUpdate(FALSE);
	m_ordPanelContRowIdx.clear();
	for (int nRow = 0; nRow < m_gridOrderPanel.GetNumberRows(); ++nRow)
		m_gridOrderPanel.DeleteRow(nRow);

	m_gridOrderPanel.SetNumberRows(m_opContracts.size() + m_opOutstandings.size());
	int nRow = 0;

#define STR(_str_, c) m_gridOrderPanel.QuickSetText(c, nRow, _str_); 
#define CLRT(_c_, c) m_gridOrderPanel.QuickSetTextColor(c, nRow, _c_);
#define CLRB(_c_, c) m_gridOrderPanel.QuickSetBackColor(c, nRow, _c_);
	CUGCell cell;

	for (auto pos = m_opContracts.begin(); pos != m_opContracts.end(); ++pos)
	{
		const OrderPanelItem* p = (*pos).get();

		m_gridOrderPanel.GetCell(fld_op_Type, nRow, &cell);
		cell.SetParam((LONG)opt_Contract);
		m_gridOrderPanel.SetCell(fld_op_Type, nRow, &cell);

		STR(_T("체결"), fld_op_Type); CLRB(crContBack, fld_op_Type);
		STR(p->pCode->GetCode(), fld_op_Code); CLRB(crContBack, fld_op_Code);
		STR(p->strPrice, fld_op_Price); CLRB(crContBack, fld_op_Price);
		STR(::PositionTypeStr(p->position), fld_op_Pos); CLRB(crContBack, fld_op_Pos);
		CLRT(p->position == PT_SELL ? Colors::Blue : Colors::Red, fld_op_Pos);
		STR(::ToString(p->nVol), fld_op_Vol); CLRB(crContBack, fld_op_Vol);;
		STR(::ToThousand(p->profitEval), fld_op_Profit); CLRB(crContBack, fld_op_Profit);
		CLRT(GetProfitColor(p->profitEval), fld_op_Profit);

#ifdef OP_ORDNUM
		STR(_T(""), fld_op_num); CLRB(crContBack, fld_op_num); ++nCol;
#endif
		m_gridOrderPanel.GetCell(fld_op_Cancel, nRow, &cell);
		cell.SetText(_T("청산"));
		cell.SetCellType(m_gridOrderPanel.m_nButtonIndex);
		cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
		cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
		cell.SetTextColor(RGB(0, 0, 128));
		cell.SetBackColor(Colors::Orange);
		cell.SetParam((long)p);
		m_gridOrderPanel.SetCell(fld_op_Cancel, nRow, &cell);

		m_ordPanelContRowIdx.insert(make_pair(p->pCode, nRow));

		++nRow;
	}

	for (auto pos = m_opOutstandings.begin(); pos != m_opOutstandings.end(); ++pos)
	{
		const OrderPanelItem* p = (*pos).get();

		m_gridOrderPanel.GetCell(fld_op_Type, nRow, &cell);
		cell.SetParam((LONG)opt_Outstanding);
		m_gridOrderPanel.SetCell(fld_op_Type, nRow, &cell);

		STR(_T("미체결"), fld_op_Type); CLRB(crOutstdBack, fld_op_Type);
		STR(p->pCode->GetCode(), fld_op_Code); CLRB(crOutstdBack, fld_op_Code);;
		STR(p->strPrice, fld_op_Price); CLRB(crOutstdBack, fld_op_Price);;
		STR(::PositionTypeStr(p->position), fld_op_Pos); CLRB(crOutstdBack, fld_op_Pos);
		CLRT(p->position == PT_SELL ? Colors::Blue : Colors::Red, fld_op_Pos);;
		STR(::ToString(p->nVol), fld_op_Vol); CLRB(crOutstdBack, fld_op_Vol);
		STR(_T(""), fld_op_Profit); CLRB(crOutstdBack, fld_op_Profit);
#ifdef OP_ORDNUM
		STR(::ToString(p->nOrdNum), fld_op_num); CLRB(crOutstdBack, fld_op_num); ++nCol;
#endif

		m_gridOrderPanel.GetCell(fld_op_Cancel, nRow, &cell);
		cell.SetText(_T("취소"));
		cell.SetCellType(m_gridOrderPanel.m_nButtonIndex);
		cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
		cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
		cell.SetTextColor(RGB(0, 0, 128));
		cell.SetBackColor(Colors::Violet);
		cell.SetParam((long)p);
		m_gridOrderPanel.SetCell(fld_op_Cancel, nRow, &cell);

		++nRow;
	}

	const AutoOrders& autoOrders = GetAutoOrderManager()->GetAutoOrders();
	for (auto posOrds = autoOrders.begin(); posOrds != autoOrders.end(); ++posOrds)
	{
		AutoOrder* pAutoOrder = posOrds->second.second;
		const AutoOrder::MITs& mits = pAutoOrder->GetMITs();
		if (mits.empty())
			continue;

		for (auto pos = mits.begin(); pos != mits.end(); ++pos)
		{
			const AutoOrder::MITItem& item = pos->second;

			ASSERT(item.nVolume > 0);

			m_gridOrderPanel.SetNumberRows(nRow + 1);

			m_gridOrderPanel.GetCell(fld_op_Type, nRow, &cell);
			cell.SetParam((LONG)opt_MIT);
			m_gridOrderPanel.SetCell(fld_op_Type, nRow, &cell);

			STR(_T("MIT"), fld_op_Type); CLRB(crMitBack, fld_op_Type);
			STR(posOrds->first.first->GetCode(), fld_op_Code); CLRB(crMitBack, fld_op_Code);
			STR(item.strPrice, fld_op_Price); CLRB(crMitBack, fld_op_Price);
			STR(::PositionTypeStr(item.position), fld_op_Pos); CLRB(crMitBack, fld_op_Pos); CLRT(item.position == PT_BUY ? Colors::Red : Colors::Blue, fld_op_Pos);
			STR(::ToString(item.nVolume), fld_op_Vol); CLRB(crMitBack, fld_op_Vol);
			STR(_T(""), fld_op_Profit); CLRB(crMitBack, fld_op_Profit);
#ifdef OP_ORDNUM
			STR(_T(""), fld_op_num); CLRB(crMitBack, fld_op_num);
#endif

			m_gridOrderPanel.GetCell(fld_op_Cancel, nRow, &cell);
			cell.SetText(_T("취소"));
			cell.SetCellType(m_gridOrderPanel.m_nButtonIndex);
			cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
			cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
			cell.SetTextColor(RGB(0, 0, 128));
			cell.SetBackColor(Colors::SpringGreen);
			cell.SetParam((long)pAutoOrder);
			m_gridOrderPanel.SetCell(fld_op_Cancel, nRow, &cell);

			m_gridOrderPanel.GetCell(fld_op_Code, nRow, &cell);
			cell.SetParam((LONG)posOrds->first.first);
			m_gridOrderPanel.SetCell(fld_op_Code, nRow, &cell);

			m_gridOrderPanel.GetCell(fld_op_Price, nRow, &cell);
			cell.SetParam((LONG)posOrds->first.second);
			m_gridOrderPanel.SetCell(fld_op_Price, nRow, &cell);

			++nRow;
			
		}
	}

	m_gridOrderPanel.EnableUpdate(TRUE);
	m_gridOrderPanel.RedrawAll();
}

LRESULT CFastTradeDlg::OnOrderPanelGridButtonClicked(WPARAM wParam, LPARAM lParam)
{
	const int nCol = (int)wParam;
	const int nRow = (int)lParam;

	CUGCell cell;
	m_gridOrderPanel.GetCell(fld_op_Type, nRow, &cell);
	OrderPanelItemType type = (OrderPanelItemType)cell.GetParam();

	if (opt_Contract == type)
	{
		m_gridOrderPanel.GetCell(fld_op_Cancel, nRow, &cell);
		OrderPanelItem* p = (OrderPanelItem*)cell.GetParam();
#ifdef _SHOW_CONFIRM_MSG
		if (ENV().Get(ENV_ORDER_CONFIRM_SHOW).ToINT() == TRUE)
		{
			CString str;
			str.Format(_T("%s(%s) %s 주문: 시장가 수량(%d)\n주문 하시겠습니까?"),
				::MerchandiseTypeStr(p->pCode->GetType()), p->pCode->GetCode(), ::PositionTypeStr(p->position),
				p->nVol);

			if (MessageBox(str, _T("청산주문"), MB_ICONQUESTION | MB_OKCANCEL) != IDOK)
				return 0;
		}
#endif

		GetSession()->OrderLiquid(GetSafeHwnd(), p->pCode);
		/*MarketType marketType = p->marketType;
		_Order(p->pCode, marketType, p->position == PT_SELL ? PT_BUY : PT_SELL, ht_SiJangGa, _T("0"), ::ToString(p->nVol), om_each_liquid_button);*/
	}
	else if (opt_Outstanding == type)
	{
		m_gridOrderPanel.GetCell(fld_op_Cancel, nRow, &cell);
		OrderPanelItem* p = (OrderPanelItem*)cell.GetParam();

		CString strVol = ::ToString(p->nVolRemain);
#if 0
		if (ENV().Get(ENV_ORDER_CONFIRM_SHOW).ToINT() == TRUE)
		{
			CString str;
			str.Format(_T("%s(%s) %s취소주문: 수량(%s) 가격(%s)\n주문을 취소하시겠습니까?"),
				::MerchandiseTypeStr(p->pCode->GetType()), p->pCode->GetCode(), ::PositionTypeStr(p->position),
				strVol, p->strPrice);

			if (MessageBox(str, _T("취소주문"), MB_ICONQUESTION | MB_OKCANCEL) != IDOK)
				return 0;
		}
#endif
		ORDER_ACTOR actor;
		CString strAction;
		strAction.Format(_T("%s: 취소주문"), ::OrderMethodStr(om_each_cancel_button, actor));

		GetSession()->SendOrderAction(strAction, m_pSelCode->GetCode(), p->strPrice, p->position, strVol);
		GetSession()->OrderCancel(GetSafeHwnd(), p->nOrdNum, strVol);
	}
	else if (opt_MIT == type)
	{
		CUGCell cell;

		m_gridOrderPanel.GetCell(fld_op_Cancel, nRow, &cell);
		AutoOrder* pAutoOrder = (AutoOrder*)cell.GetParam();

		CString strPrice = m_gridOrderPanel.QuickGetText(2, nRow);
		CString strPosType = m_gridOrderPanel.QuickGetText(3, nRow);
		PositionType posType = strPosType == _T("매도") ? PT_SELL : PT_BUY;
		CString strVol = m_gridOrderPanel.QuickGetText(4, nRow);


		ORDER_ACTOR actor;
		CString strAction;
		strAction.Format(_T("%s: MIT취소"), ::OrderMethodStr(om_each_cancel_button, actor));
		GetSession()->SendOrderAction(strAction, pAutoOrder->GetCodeID().first->GetCode(), strPrice, posType, strVol);

		pAutoOrder->MIT_Cancel(posType, strPrice);
	}
	return 0;
}

LRESULT CFastTradeDlg::OnAccountBalanceGridButtonClicked(WPARAM wParam, LPARAM lParam)
{
	const int nCol = (int)wParam;
	const int nRow = (int)lParam;

	if (fld_ab_OverSet == nCol)
	{
		COvernightRequestDlg dlg;
		dlg.DoModal();
	}
	return 0;
}

void CFastTradeDlg::SetMerchandise(Code* pCode, MarketType marketType)
{
	if (m_pMerch)
	{
		if (m_pMerch->GetCode() == pCode && m_curMarketType == marketType)
			return;

		//DetachAutoOrderObserver를 호출한 후에 확인해야 한다. 아주 조심해야 하는부분
		if (m_pAutoOrder->IsEmpty())
		{
			m_pMerch->StopCurrentRecv();
			m_pMerch->StopBidRecv();
		}

		GetAutoOrderManager()->DetachAutoOrderObserver(make_pair(m_pMerch->GetCode(), m_curMarketType), dynamic_cast<AutoOrderObserver*>(this));
		GetAutoOrderManager()->DetachAutoOrderObserver(make_pair(m_pMerch->GetCode(), m_curMarketType), dynamic_cast<AutoOrderObserver*>(&m_gridHoga));
		
		m_pMerch->GetCurrent()->DetachObserver(dynamic_cast<GTCurrentObserver*>(this));
		m_pMerch->GetHoga()->DetachObserver(dynamic_cast<GTHogaObserver*>(this));

		m_pAutoOrder = NULL;
		m_gridHoga.m_pAutoOrder = NULL;
	}

	m_bCurrentInit = FALSE;
	m_bHogaInit = FALSE;

	/*if(FALSE == m_mits.empty())
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

#ifdef _MARKET_CONT_1
	for (int nRow = 0; nRow < GTCurrent::MAX_QUEUE; ++nRow)
	{
		m_gridMarketContract.QuickSetText(0, nRow, _T(""));
		m_gridMarketContract.QuickSetText(1, nRow, _T(""));
		m_gridMarketContract.QuickSetText(2, nRow, _T(""));
	}
	m_gridMarketContract.RedrawAll();
#else
	for (int nRow = 0; nRow < GTCurrent::MAX_QUEUE; ++nRow)
	{
		m_gridMarketContract.QuickSetText(0, nRow, _T(""));
		m_gridMarketContract.QuickSetText(1, nRow, _T(""));
		m_gridMarketContract.QuickSetText(2, nRow, _T(""));
	}
	m_gridMarketContract.OnSwInvalidate(true);
#endif

	m_pSelCode = pCode;

	m_pMerch->RunCurrentRecv();

	/// 호가 수신은 OnCurrentInitUpdated가 호출되면 호출한다.
	//m_pMerch->RunHogaRecv(); 

	CString strLabel;
	if (pCode->GetType() == ct_futures)
	{
		strLabel = _T("KOSPI선물");
	}
	else if (pCode->GetType() == ct_options)
	{
		CodeOptions* pOPCode = dynamic_cast<CodeOptions*>(pCode);
		if (pOPCode)
		{
			if (pOPCode->GetOptionsType() == ot_call)
			{
				strLabel.Format(_T("콜 %s"), pOPCode->GetPrice());
			}
			else
			{
				strLabel.Format(_T("풋 %s"), pOPCode->GetPrice());
			}
		}
	}
	else if (pCode->GetType() == ct_foreign_futures)
	{
		CodeForeignFutures* pFFCode = dynamic_cast<CodeForeignFutures*>(pCode);
		if (pFFCode)
		{
			strLabel = pFFCode->korName;
		}
	}
	m_labels[LBL_SEL_CODE].SetWindowText(strLabel);
	m_labels[LBL_SEL_CODE].Invalidate();

	CString strAction;
	strAction.Format(_T("종목선택: %s"), strLabel);
	if (marketType != mt_default)
	{
		if (mt_CME == marketType)
			strAction += _T("(CME)");
		else if (mt_EUREX == marketType)
			strAction += _T("(EUREX)");
		else
		{
			ASSERT(FALSE);
		}
	}
	GetSession()->SendOrderAction(strAction);
	m_curMarketType = marketType;

	m_pCurBase = SER()->GetCurrentBase(codeID);

	CString strTitle;
	//	strLabel.Replace(_T("&&"), _T("&")); // S&P 500은 내부적으로 S&&500으로 저장되어있다.
	strTitle.Format(_T("%s - 빠른주문(%s)"), strLabel, GetSession()->id);
	GetParent()->SendMessage(WM_VTSFRM_CHILD_SET_TITLE_REQUEST, (WPARAM)(LPCTSTR)strTitle);
}

void CFastTradeDlg::OnBnClickedButtonMarketSell()
{
	if (NULL == m_pSelCode)
		return;
	/// 시장가매도
	CString s;
	s.Format(_T("버튼: 시장가매도버튼(%s)"), m_pSelCode->GetCode());
	GetSession()->SendOrderAction(s);
	Order(PT_SELL, ht_SiJangGa, _T("0"), GetCurVolume(), om_button);
}

void CFastTradeDlg::OnBnClickedButtonMarketBuy()
{
	if (NULL == m_pSelCode)
		return;

	/// 시장가매수
	CString s;
	s.Format(_T("버튼: 시장가매수버튼(%s)"), m_pSelCode->GetCode());
	GetSession()->SendOrderAction(s);
	Order(PT_BUY, ht_SiJangGa, _T("0"), GetCurVolume(), om_button);
}

void CFastTradeDlg::OnBnClickedButtonAllLiquid()
{
	if (NULL == m_pSelCode)
		return;

#ifdef _SHOW_CONFIRM_MSG
	if (GetParent()->MessageBox(_T("현재종목의 보유중인 모든 포지션을 시장가로 청산합니다"), _T("청산주문"), MB_ICONQUESTION | MB_OKCANCEL) != IDOK)
		return;

	CString s;
	s.Format(_T("버튼: 전체청산(%s)"), m_pSelCode->GetCode());
	GetSession()->SendOrderAction(s);
	GetSession()->OrderLiquid(GetSafeHwnd(), m_pSelCode);
#else


	OnBnClickedButtonAllCancel();
	GetSession()->SendOrderAction(_T("버튼: 전체청산"));
	GetSession()->OrderLiquid(GetSafeHwnd(), NULL);
#endif
}

void CFastTradeDlg::MITAllCancel()
{
	if (NULL == m_pSelCode)
		return;

	m_pAutoOrder->MIT_AllCancel();
}

void CFastTradeDlg::OnBnClickedButtonSellMitCancel()
{
	if (NULL == m_pSelCode)
		return;

	CString s;
	s.Format(_T("버튼: MIT매도전체취소(%s)"), m_pSelCode->GetCode());
	GetSession()->SendOrderAction(s);

	m_pAutoOrder->MIT_SellCancel();
}

void CFastTradeDlg::OnBnClickedButtonBuyMitCancel()
{
	if (NULL == m_pSelCode)
		return;

	CString s;
	s.Format(_T("버튼: MIT매수전체취소(%s)"), m_pSelCode->GetCode());
	GetSession()->SendOrderAction(s);

	m_pAutoOrder->MIT_BuyCancel();
}

void CFastTradeDlg::OnBnClickedButtonSellCancel()
{
	if (NULL == m_pSelCode)
		return;

	CString s;
	s.Format(_T("버튼: 매도전체취소(%s)"), m_pSelCode->GetCode());
	GetSession()->SendOrderAction(s);

	GetSession()->OrderCancelAllByPosition(GetSafeHwnd(), m_pSelCode, PT_SELL);
}

void CFastTradeDlg::OnBnClickedButtonBuyCancel()
{
	if (NULL == m_pSelCode)
		return;

	CString s;
	s.Format(_T("버튼: 매수전체취소(%s)"), m_pSelCode->GetCode());
	GetSession()->SendOrderAction(s);

	GetSession()->OrderCancelAllByPosition(GetSafeHwnd(), m_pSelCode, PT_BUY);

}

void CFastTradeDlg::OnBnClickedButtonAllCancel()
{
	if (NULL == m_pSelCode)
		return;

#ifdef _SHOW_CONFIRM_MSG
	CString s;
	s.Format(_T("버튼: 전체취소(%s)"), m_pSelCode->GetCode());
	GetSession()->SendOrderAction(s);
	GetSession()->OrderCancelAllByPosition(GetSafeHwnd(), m_pSelCode, PT_NONE);
#else
	GetSession()->SendOrderAction(_T("버튼: 전체취소"));
	GetSession()->OrderCancelAllByPosition(GetSafeHwnd(), NULL, PT_NONE);
#endif
	MITAllCancel();
}

void CFastTradeDlg::MITTotalUpdate()
{
	int nBuy = 0, nSell = 0;
	const AutoOrder::MITs& mits = m_pAutoOrder->GetMITs();

	for (auto pos = mits.begin(); pos != mits.end(); ++pos)
	{
		const AutoOrder::MITItem& item = pos->second;

		if (item.position == PT_SELL)
			nSell += item.nVolume;
		else if (item.position == PT_BUY)
			nBuy += item.nVolume;
	}

	CString s;
	s = ::ToString(nSell);
	m_gridTotHoga.QuickSetText(c_mit_sell, 0, s); m_gridTotHoga.RedrawCell(c_mit_sell, 0);

	s = ::ToString(nBuy);
	m_gridTotHoga.QuickSetText(c_mit_buy, 0, s); m_gridTotHoga.RedrawCell(c_mit_buy, 0);
}

void CFastTradeDlg::OnBnClickedCheckBigHogaHeight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_BIG_HOGA_HEIGHT))->GetCheck();
	m_gridHoga.SetHighRowHeight(bCheck);
}


void CFastTradeDlg::OnBnClickedCheckOneclickOrder()
{
	m_bOneClick = ((CButton*)GetDlgItem(IDC_CHECK_ONECLICK_ORDER))->GetCheck() == BST_CHECKED;
	m_gridHoga.SetOneClickOrder(m_bOneClick);

	CString strAction;
	strAction.Format(_T("원클릭주문 %s"), m_bOneClick ? _T("설정") : _T("해제"));
	GetSession()->SendOrderAction(strAction);
}


void CFastTradeDlg::OnBnClickedCheckLosscut()
{
	if (NULL == m_pAutoOrder)
		return;
	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_LOSSCUT))->GetCheck() == BST_CHECKED;

	/*CString s;
	m_editLosscut.GetWindowText(s);
	INT nTick = ::ToINT(s);*/
	INT nTick = m_spinEditLossCut.GetNumber();

	CString strMsg;
	if (!m_pAutoOrder->SetLosscut(bCheck, nTick, dynamic_cast<AutoOrderObserver*>(this), strMsg))
	{
		((CButton*)GetDlgItem(IDC_CHECK_LOSSCUT))->SetCheck(bCheck ? BST_UNCHECKED : BST_CHECKED);
		CString str;
		str.Format(_T("손실제한설정실패: %s"), strMsg);
		GetSession()->SendOrderAction(str);
		MessageBox(strMsg, _T("설정실패"), MB_ICONERROR);
		return;
	}

	if (bCheck)
	{
		CString str;
		str.Format(_T("손실제한설정: %d틱(%s)"), nTick, m_pSelCode->GetCode());
		GetSession()->SendOrderAction(str);
	}
	else
	{
		GetSession()->SendOrderAction(_T("손실제한설정해제"));
	}
}


void CFastTradeDlg::OnBnClickedCheckEarncut()
{
	if (NULL == m_pAutoOrder)
		return;

	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_EARNCUT))->GetCheck() == BST_CHECKED;

	/*CString s;
	m_editEarncut.GetWindowText(s);
	INT nTick = ::ToINT(s);*/
	INT nTick = m_spinEditEarnCut.GetNumber();

	CString strMsg;
	if (!m_pAutoOrder->SetEarncut(bCheck, nTick, dynamic_cast<AutoOrderObserver*>(this), strMsg))
	{
		((CButton*)GetDlgItem(IDC_CHECK_EARNCUT))->SetCheck(bCheck ? BST_UNCHECKED : BST_CHECKED);
		CString str;
		str.Format(_T("이익실현설정실패: %s"), strMsg);
		GetSession()->SendOrderAction(str);
		MessageBox(strMsg, _T("설정실패"), MB_ICONERROR);
		return;
	}

	if (bCheck)
	{
		CString str;
		str.Format(_T("이익실현설정: %d틱(%s)"), nTick, m_pSelCode->GetCode());
		GetSession()->SendOrderAction(str);
	}
	else
	{
		GetSession()->SendOrderAction(_T("이익실현설정해제"));
	}
}


void CFastTradeDlg::OnHogaGridEndSelChange(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_UG* pItem = (NM_UG*)pNotifyStruct;

	pItem->col;
	pItem->row;
}

void CFastTradeDlg::OnOrderPanelGridSelChange(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_UG* pItem = (NM_UG*)pNotifyStruct;
	pItem->col;
	pItem->row;

	if (pItem->col == ORDER_PANEL_GRID_BTN_COL - 1)
		return;

	if (pItem->col != -1 && pItem->row != -1)
	{
		Code* pCode = NULL;
		MarketType marketType = mt_none;

		CUGCell cell;
		m_gridOrderPanel.GetCell(fld_op_Type, pItem->row, &cell);
		OrderPanelItemType type = (OrderPanelItemType)cell.GetParam();

		if (opt_Contract == type || opt_Outstanding == type)
		{
			m_gridOrderPanel.GetCell(fld_op_Cancel, pItem->row, &cell);
			OrderPanelItem* pOPItem = (OrderPanelItem*)cell.GetParam();
			ASSERT(pOPItem);
			pCode = pOPItem->pCode;
			marketType = pOPItem->marketType;
		}
		else if (opt_MIT == type)
		{
			m_gridOrderPanel.GetCell(fld_op_Code, pItem->row, &cell);
			pCode = (Code*)cell.GetParam();

			m_gridOrderPanel.GetCell(fld_op_Price, pItem->row, &cell);
			marketType = (MarketType)cell.GetParam();
		}
		ASSERT(pCode && marketType != mt_none);

		if (m_pOptionsTableDlg)
		{
			m_pOptionsTableDlg->ShowWindow(SW_HIDE);
			m_pOptionsTableDlg->SendMessage(WM_CLOSE);
		}

		if (m_pForeignFuturesCodeDlg)
		{
			m_pForeignFuturesCodeDlg->ShowWindow(SW_HIDE);
			m_pForeignFuturesCodeDlg->SendMessage(WM_CLOSE);
		}

		int nSelTab = NUL_IDX;
		BOOL bShowBtn = FALSE;

		switch (pCode->GetType())
		{
		case ct_futures:
			if (marketType == mt_CME)
				nSelTab = mt_night_futures;
			else
				nSelTab = mt_futures;
			break;
		case ct_options:
			bShowBtn = TRUE;
			if (marketType == mt_EUREX)
			{
				nSelTab = mt_night_options;
			}
			else
				nSelTab = mt_options;
			break;
		case ct_foreign_futures:
			nSelTab = mt_foreign_futures;
			bShowBtn = TRUE;
			break;
		default:
			ASSERT(FALSE);
		}
		m_tabMerchandise.SetCurSel(nSelTab);
		if (m_pSelCode != pCode)
			SetMerchandise(pCode, marketType);

		m_buttonSelCode.ShowWindow(bShowBtn ? SW_SHOW : SW_HIDE);
		m_curMerchandiseType = pCode->GetType();
		m_curMarketType = marketType;


		UpdateGridAccountPanel(ap_contract);
	}
}

void CFastTradeDlg::OnMerchandiseTabSelChange()
{
	//현재 띄워져 있는 종목선택 다이얼로그를 해제한다.
	if (m_pOptionsTableDlg)
	{
		m_pOptionsTableDlg->ShowWindow(SW_HIDE);
		m_pOptionsTableDlg->SendMessage(WM_CLOSE);
	}

	if (m_pForeignFuturesCodeDlg)
	{
		m_pForeignFuturesCodeDlg->ShowWindow(SW_HIDE);
		m_pForeignFuturesCodeDlg->SendMessage(WM_CLOSE);
	}

	int nTab = m_tabMerchandise.GetCurSel();

	MerchandiseType mtype = ct_none;
	MarketType marketType = mt_default;

	BOOL bShowBtn = FALSE;
	Code* pCode = NULL;

	if (false == InstCM().GetFutures().empty())
	{
		if (mt_futures == nTab)
		{
			mtype = ct_futures;
			pCode = InstCM().GetFutures().begin()->second;
		}
		else if (mt_night_futures == nTab)
		{
			mtype = ct_futures;
			marketType = mt_CME;

			pCode = InstCM().GetFutures().begin()->second;
			if (pCode->bExpireDay)
				pCode = (++InstCM().GetFutures().begin())->second; //만기일이라면 차월물을 근월물로 셋팅
		}
		else if (mt_foreign_futures == nTab)
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

			if (m_pForeignFuturesCodeDlg->s_pLastSelCode)
				pCode = m_pForeignFuturesCodeDlg->s_pLastSelCode;

			mtype = ct_foreign_futures;
			bShowBtn = TRUE;
		}
		else if (mt_options == nTab)
		{
			ASSERT(NULL == m_pOptionsTableDlg);
			m_pOptionsTableDlg = new COptionsTableDlg(this, FALSE);
			m_pOptionsTableDlg->m_pOwner = this;
			m_pOptionsTableDlg->Create(COptionsTableDlg::IDD, this);
			m_pOptionsTableDlg->ShowWindow(SW_SHOW);

			mtype = ct_options;
			bShowBtn = TRUE;
		}
		else if (mt_night_options == nTab)
		{
			ASSERT(NULL == m_pOptionsTableDlg);
			m_pOptionsTableDlg = new COptionsTableDlg(this, TRUE);
			m_pOptionsTableDlg->m_pOwner = this;
			m_pOptionsTableDlg->Create(COptionsTableDlg::IDD, this);
			m_pOptionsTableDlg->ShowWindow(SW_SHOW);

			mtype = ct_options;
			marketType = mt_EUREX;
			bShowBtn = TRUE;
		}
	}

	m_curMerchandiseType = mtype;

	if (pCode)
	{
		SetMerchandise(pCode, marketType);
		m_curMarketType = marketType;
	}
	else
	{
		m_curMarketType = mt_none;
	}

	m_buttonSelCode.ShowWindow(bShowBtn ? SW_SHOW : SW_HIDE);
	UpdateGridAccountPanel(ap_contract);

	//	*pResult = 0;
}

void CFastTradeDlg::OnDestroy()
{
	if (m_pOptionsTableDlg)
	{
		m_pOptionsTableDlg->SendMessage(WM_CLOSE);
	}

	if (m_pForeignFuturesCodeDlg)
	{
		m_pForeignFuturesCodeDlg->SendMessage(WM_CLOSE);
	}
	if (m_pMerch)
	{
		//DetachAutoOrderObserver를 호출한 후에 확인해야 한다. 아주 조심해야 하는부분
		if (m_pAutoOrder && m_pAutoOrder->IsEmpty())
		{
			m_pMerch->StopCurrentRecv();
			m_pMerch->StopBidRecv();
		}

		m_pMerch->GetCurrent()->DetachObserver(dynamic_cast<GTCurrentObserver*>(this));
		m_pMerch->GetHoga()->DetachObserver(dynamic_cast<GTHogaObserver*>(this));
		m_pMerch = GTMerchandisePtr();
	}

	SER()->UnregisterOrderOutstanding(this);
	SER()->UnregisterOrderContracted(this);
	SER()->UnregisterClientInfo(this);
	
	if (NULL != m_pSelCode)
	{
		GetAutoOrderManager()->DetachAutoOrderObserver(make_pair(m_pSelCode, m_curMarketType), dynamic_cast<AutoOrderObserver*>(this));
		GetAutoOrderManager()->DetachAutoOrderObserver(make_pair(m_pSelCode, m_curMarketType), dynamic_cast<AutoOrderObserver*>(&m_gridHoga));
	}

	GetAutoOrderManager()->DetachObserver(dynamic_cast<AutoOrderManagerObserver*>(this));
	m_pAutoOrder = NULL;
	m_gridHoga.m_pAutoOrder = NULL;
	m_gridHoga.OnFinalize();

	VTSFrmWndChildResizableDlg::OnDestroy();
}


void CFastTradeDlg::OnBnClickedVolume(UINT nID)
{
	CString vol;
	GetDlgItemText(nID, vol);
	m_editVol.SetWindowText(vol);

	CString strAction;
	strAction.Format(_T("수량설정 %s개"), vol);
	GetSession()->SendOrderAction(strAction);


}

void CFastTradeDlg::OnBnClickedExpand()
{
	m_bExpand = !m_bExpand;

	CRect rt;
	GetClientRect(rt);

	CString s;
	if (m_bExpand)
	{
		m_buttonExpand.SetTooltipText(_T("창을 축소합니다"));
		s = _T("<닫힘");
		rt.right = m_nCurRight + m_nExpandAreaWidth;
	}
	else
	{
		m_buttonExpand.SetTooltipText(_T("창을 확장합니다"));
		s = _T("펼침>");
		rt.right = m_nCurRight;
	}

	m_buttonExpand.SetWindowText(s);
	GetParent()->SendMessage(WM_VTSFRM_CHILD_RESIZE_REQUEST, (WPARAM)rt.Width(), (LPARAM)rt.Height());

	if (m_bExpand && m_pMerch)
	{
		GTCurrentPtr p = m_pMerch->GetCurrent();
		if (!UpdateMarketContracts(p))
			return;
		UpdateCurrent(p);
	}
}

LRESULT CFastTradeDlg::OnPreferencesChanged(WPARAM wParam, LPARAM lParam)
{
	CString str;
	str = ENV().Get(ENV_VOL_1);
	m_extButtons[BTN_VOL_1].SetWindowText(str);
	str = ENV().Get(ENV_VOL_2);
	m_extButtons[BTN_VOL_2].SetWindowText(str);
	str = ENV().Get(ENV_VOL_3);
	m_extButtons[BTN_VOL_3].SetWindowText(str);
	str = ENV().Get(ENV_VOL_4);
	m_extButtons[BTN_VOL_4].SetWindowText(str);
	str = ENV().Get(ENV_VOL_5);
	m_extButtons[BTN_VOL_5].SetWindowText(str);

	return 0;
}
LRESULT CFastTradeDlg::OnOptionsTableSelectCodeChanged(WPARAM wParam, LPARAM lParam)
{
	Code* pCode = (Code*)wParam;
	MarketType marketType = (MarketType)lParam;
	ASSERT(pCode);
	SetMerchandise(pCode, marketType);
	return 0;
}

LRESULT CFastTradeDlg::OnVTSModelessDlgDestroyed(WPARAM wParam, LPARAM lParam)
{
	if (m_pOptionsTableDlg == (COptionsTableDlg*)wParam)
		m_pOptionsTableDlg = NULL;
	else if (m_pForeignFuturesCodeDlg == (CForeignFuturesCodeDlg*)wParam)
		m_pForeignFuturesCodeDlg = NULL;

	return 0;
}

void CFastTradeDlg::OnSizing(UINT fwSide, LPRECT pRect)
{


	VTSFrmWndChildResizableDlg::OnSizing(fwSide, pRect);
}

void CFastTradeDlg::OnSize(UINT nType, int cx, int cy)
{
	VTSFrmWndChildResizableDlg::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (IsWindow(m_gridHoga.GetSafeHwnd()))
	{
#if 0
		int nHogaSizeType = ENV().Get(ENV_HOGA_RESIZE_TYPE).ToINT();
		if (nHogaSizeType == hst_count_fix)
		{
			CRect rect;
			m_gridHoga.GetClientRect(rect);
			int nNewHeight = rect.Height() / 19;
			m_gridHoga.SetRowHeight(0, nNewHeight);
			m_gridHoga.RedrawAll();
		}
#endif
	}
}


LRESULT CFastTradeDlg::OnOrderResult(WPARAM wParam, LPARAM lParam)
{
	OrderResult* p = (OrderResult*)wParam;
	TRACE(_T("CFastTradeDlg::OnOrderResult(0x%x)\n"), p);
	ASSERT(AfxIsValidAddress(p, sizeof(OrderResult)));
	if (OS_REJECT == p->orderStatus)
	{
		::MessageBox(NULL, p->strMsg, _T("주문거부"), MB_ICONERROR);
	}

	return 0;
}

void CFastTradeDlg::OnBnClickedCheckAlwaysOnTop()
{
	BOOL bCheck = BST_CHECKED == ((CButton*)GetDlgItem(IDC_CHECK_ALWAYS_ON_TOP))->GetCheck();
	GetParent()->SendMessage(WM_VTSFRM_SET_TOP_MOST, (WPARAM)bCheck);
}

//작게
void CFastTradeDlg::OnBnClickedRadio1()
{
	SetResize(sz_small);
}

//중간
void CFastTradeDlg::OnBnClickedRadio2()
{
	SetResize(sz_medium);
}

//크게
void CFastTradeDlg::OnBnClickedRadio3()
{
	SetResize(sz_large);
}

LRESULT CFastTradeDlg::OnClientInfoUpdated(WPARAM wParam, LPARAM lParam)
{
	TRACE_LINE;
	SetEnableOrderCount();

	UpdateGridAccountPanel(ap_balance);

	m_gridAccountBalance.QuickSetText(fld_ab_OverSet, 0, GetSession()->enableOvernight ? _T("신청") : _T("해제"));
	m_gridAccountBalance.RedrawCell(fld_ab_OverSet, 0);
	return 0;
}
