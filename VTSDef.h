#pragma once

#include <VTSCommon.h>
////////////////////////////////////////////////////////////////////////////////////////////////////
enum VTS_MESSAGE
{
	WM_MERCH_CURRENT_INIT = WM_VTS_MSG_BEGIN + 1000,
	WM_MERCH_CURRENT,
	WM_MERCH_HOGA_INIT,
	WM_MERCH_HOGA,
	WM_ORDER_OUTSTANDING,
	WM_CONTRACTED_ORDER,
	WM_ORDER_CONTRACTS,
	WM_ORDER_RESULT,
	WM_PROFIT_UPDATED,
	WM_DUMMY_TEST,
	WM_PACKET_ADDED,
	WM_MESSAGE_FROM_SERVER,
	WM_NOTICES_RECEIVED,
	WM_NEW_NOTICE,
	WM_PREFERENCES_CHANGED,
	WM_OPTIONS_TABLE_SELECT_CODE_CHANGED,
	WM_OPTIONS_TABLE_DESTORYED,
	WM_CODE_LIST_RECEIVED,
	WM_ENABLE_ORDER_COUNT_RECEIVED,
	WM_CLIENT_INFO_UPDATED,
	WM_ORDER_PANEL_GRID_BUTTON_CLICKED,
	WM_CURRENT_UPDATED,
	WM_HOGA_UPDATED,
	WM_MARKET_EYE,
	WM_ACCOUNT_BALANCE_GRID_BUTTON_CLICKED,
};
////////////////////////////////////////////////////////////////////////////////////////////////////
enum SERVER_EVENT_TYPE
{
	SET_CURRENT,
	SET_HOGA,
	SET_JANGO,
};


////////////////////////////////////////////////////////////////////////////////////////////////////
struct OrderOutstanding
{
	Code*			pCode;
	MarketType		marketType;
	PositionType	position;
	int				nOrderNum;
	CString			strPrice;
	GT_VOLUME		nVolume;
	GT_VOLUME		nVolumeRemain;
	DK_DECIMAL		dPrice;
	BOOL			bHogaFakeUpdate; //bHogaFakeUpdate
	BOOL			bRealOrderFailed; //bRealOrderFailed
};

typedef std::list<OrderOutstanding*> OrderOutstandings;

////////////////////////////////////////////////////////////////////////////////////////////////////
struct OrderContracted
{
	Code*			pCode;
	MarketType		marketType;
	PositionType	position;
	CString			strPrice;
	GT_VOLUME		nVolume;
	CString			strCurPrice;

	GT_CURRENCY		profitEval;
	DK_DECIMAL		dPrice;
};
////////////////////////////////////////////////////////////////////////////////////////////////////
struct Profit
{
	GT_CURRENCY			profitEvalSum;
	GT_CURRENCY			profitRealize;
	GT_CURRENCY			lossCut;
	GT_CURRENCY			accountBalanceEval;
	GT_CURRENCY			depositForFuturesOvernightTot;
	GT_CURRENCY			depositForOptionsOvernightTot;
	GT_CURRENCY			depositForForeignFuturesOvernightTot;
	Profit()
	{
		Empty();
	}
	void Empty()
	{
		profitEvalSum = 0L;
		profitRealize = 0L;
		lossCut = 0L;
		accountBalanceEval = 0L;
		depositForFuturesOvernightTot = 0L;
		depositForOptionsOvernightTot = 0L;
		depositForForeignFuturesOvernightTot = 0L;
	}
};
////////////////////////////////////////////////////////////////////////////////////////////////////
struct OrderResult
{
	ORDER_STATUS    orderStatus;
	int             nOrdNum;
	int             nOrdNumOrg;
	Code*           pCode;
	PositionType    position;
	HogaType        hoga;
	CString         strPrice;
	GT_VOLUME       nVolume;
	CString         strContPrice;
	CString         strMsg;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
struct EanbleOrderItem
{
	Code*			pCode;
	MarketType		marketType;
	GT_VOLUME		nBuyCount;
	GT_VOLUME		nSellCount;
};
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef boost::shared_ptr<OrderContracted>	OrderContractedPtr;
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef list<OrderContractedPtr> OrderContracts;
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
enum OrderMethod
{
	om_click,
	om_dbl_click,
	om_button,
	om_each_cancel_button,
	om_each_liquid_button,
	om_drag,
	om_mit,
	om_stop_losscut,
	om_stop_earncut,
};

CString OrderMethodStr(OrderMethod method, ORDER_ACTOR& actor);

struct CurrentJisu
{
	CString		jisu;
	CString		sign;
	CString		change;
	CString		diff;
};

struct MarketEye
{
	CurrentJisu		kospi;
	CurrentJisu		USD;
};

#if THEME == THEME_NATIVE
const COLORREF COLOR_DEFAULT_BACK       = RGB(234, 233, 229);
const COLORREF COLOR_CONTROL_BACK       = RGB(208, 211, 220);
const COLORREF COLOR_FONT_NORMAL        = Colors::Black;
const COLORREF COLOR_CONTROL_LINE       = RGB(45, 46, 54);
#elif THEME == THEME_LINETRADE
const COLORREF COLOR_DEFAULT_BACK       = RGB(65, 67, 74);
const COLORREF COLOR_CONTROL_BACK       = RGB(111, 113, 124);
const COLORREF COLOR_FONT_NORMAL        = RGB(221, 221, 221);
const COLORREF COLOR_CONTROL_LINE       = RGB(45, 46, 54);
#else
const COLORREF COLOR_DEFAULT_BACK       = RGB(65, 67, 74);
const COLORREF COLOR_CONTROL_BACK       = RGB(111, 113, 124);
const COLORREF COLOR_FONT_NORMAL        = RGB(221, 221, 221);
const COLORREF COLOR_CONTROL_LINE       = RGB(45, 46, 54);
#endif

const COLORREF COLOR_IMG_TRANSPARENT    = RGB(255, 0, 255);
const COLORREF COLOR_GRID_HEADER_BACK1  = RGB(234,234,234);
const COLORREF COLOR_MANAGER_MODE_BACK  = RGB(128,64,64);