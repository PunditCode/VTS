// CFastTradeDlg_DSVIP 대화 상자입니다.

#pragma once

#if COMPANY==COMPANY_DSVIP

#include <VTSFrmWndChildBase.h>
#include "resource.h"
#include "UltimateGrid/UGCtrl.h"
#include "GTMerchandisePool.h"
#include "ColorStatic.h"
#include "HogaGrid_DSVIP.h"
#include "Entabctrl.h"
#include "AutoOrder.h"
#include <UltimateGrid/ugctbutn.h>
#include "xSkinButton.h"

class CFastTradeDlg_DSVIP;

class CMyTabWndFlat : public CExtTabFlatWnd
{
	int m_nShift;
	int m_nSize;
public:
	CFastTradeDlg_DSVIP*		m_pOwner;
	CMyTabWndFlat() : m_nShift( 2 ), m_nSize( 2 ) { }
	int GetShift()
	{
		CRect rc;
		GetWindowRect(rc);
		return _CalcRgnShift( OrientationIsHorizontal(), rc );
	}
	int GetSize() { return OnFlatTabWndGetSize( OrientationIsHorizontal() ); }
	void SetShift( int nShift ) { m_nShift = nShift; }
	void SetSize( int nSize ) { m_nSize = nSize; }


	virtual bool OnTabWndSelectionChange(
      LONG nOldItemIndex,
      LONG nNewItemIndex,
	  bool bPreSelectionTest){return TRUE;}

protected:
	virtual int _CalcRgnShift( bool bHorz, const CRect & rc )
	{
		int nShift = 0;
		if( m_nShift >= 0 )
			nShift = m_nShift;
		else
			nShift = ::MulDiv( bHorz ? rc.Height() : rc.Width(), 1, 4 );
		return nShift;
	}
	virtual int OnFlatTabWndGetSize( bool bHorz )
	{
		int nSize = 0;
		if(bHorz)
			if( m_nSize > 0 )
				nSize = m_nSize;
			else
				nSize = 16;//::GetSystemMetrics( SM_CXHSCROLL );
		else
			if( m_nSize > 0 )
				nSize = m_nSize;
			else
				nSize = 16;//::GetSystemMetrics( SM_CYHSCROLL );
		return nSize;
	}
}; // class CMyTabWndFlat	

class OrderPanelGrid : public CUGCtrl
{
public:
	CUGButtonType		m_button;
	int					m_nButtonIndex;
	class CFastTradeDlg_DSVIP*		m_pOwner;

	virtual void OnSetup();
	int OnPushButton(long ID,int col,long row,long msg,long param);
	override int OnCellTypeNotify(long ID,int col,long row,long msg, LONG_PTR param);
};


class CFastTradeDlg_DSVIP : public VTSFrmWndChildResizableDlg, public GTCurrentObserver, public GTHogaObserver, public AutoOrderObserver, public AutoOrderManagerObserver
{
	DECLARE_DYNCREATE(CFastTradeDlg_DSVIP)

public:
	CFastTradeDlg_DSVIP(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CFastTradeDlg_DSVIP();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_FAST_TRADE_DSVIP };
	BOOL Create(CWnd* pParentWnd);

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	
	void InitControls();
	void InitContractProfitGrid();
	void InitAccountBalanceGrid();
	void InitCurrentInfo();
	void InitHoga();
	void InitMarketContract();
	void InitOrderPanel();
	void InitOutstanding();

	void OnHogaClickOrder(int col, int row, const CString& price);

	

	BOOL Order(PositionType posType, HogaType hoga, const CString& price, const CString& vol, OrderMethod method);
	BOOL OrderCorrection(PositionType posType, CString srcPrice, CString destPrice, CString vol, OrderMethod method);
	BOOL OrderCancel(PositionType posType, CString srcPrice, CString vol, OrderMethod method);

	BOOL _Order(Code* pCode, MarketType marketType, PositionType posType, HogaType hoga, const CString& price, const CString& vol, OrderMethod method);

	override void OnAutoOrderSetLossCut(BOOL bEnable, int nTick, int nRowOld, int nRowNew);
	override void OnAutoOrderSetEarnCut(BOOL bEnable, int nTick, int nRowOld, int nRowNew);
	override void OnAutoOrderMITChanged(PositionType posType, const CString& strPrice, GT_VOLUME volume);
	override void OnAutoOrderContractChanged(int nRowOld, int nRowNew);
	override void OnAutoOrderManagerOnMITUpdated(const AutoOrders& autoOrders);

	BOOL MITOrder(PositionType posType, CString price, CString vol, BOOL bCorrection, OrderMethod method);
	BOOL MITCorrection(PositionType posType, CString srcPrice, CString destPrice, CString vol, OrderMethod method);
	BOOL MITCancel(PositionType posType, CString srcPrice, CString vol, OrderMethod method);

	void MITTotalUpdate();
	void MITAllCancel();

	override void OnCurrentInitUpdated(GTCurrent* pCurrent);
	override void OnCurrentUpdated(GTCurrent* pCurrent);
	override void OnHogaInitUpdated(GTHoga* pCurrent);
	override void OnHogaUpdated(GTHoga* pHoga);
	void UpdateTotalHoga(GTHoga* pHoga);
	void SetMerchandise(Code* pCode, MarketType marketType);


	void ContractApplyToHoga();
	void OutstandingApplyToHoga();
	void SetMerchandiseTab(MerchandiseType mtype, MarketType marketType, BOOL bSelectDefault = FALSE);
	void SetEnableOrderCount();

	CString GetCurVolume();

	enum { ap_contract = 0x00000001, ap_profit = 0x00000002, ap_balance = 0x00000004, 
		ap_all = ap_contract | ap_profit | ap_balance };

	void UpdateGridAccountPanel(int nCase, OrderContracted* pOrderUpdated = NULL); // 0:체결됐을때, 1:손익 바뀌었을때, 2:계좌 잔고 바뀌었을때
private:
	friend class HogaGrid_DSVIP;

	enum
	{
		LBL_1                  ,
		LBL_SEL_CODE           ,
		LBL_SELL               ,
		LBL_ENABLE_SELL        ,
		LBL_BUY                ,
		LBL_ENABLE_BUY         ,
		LBL_2					,
		////////////////////////
		LBL_COUNT              ,
	};

	enum 
	{
		BTN_MARKET_SELL     ,
		BTN_ALL_LIQUID      ,
		BTN_MARKET_BUY      ,
		BTN_SELL_MIT_CANCEL ,
		BTN_SELL_CANCEL     ,
		BTN_ALL_CANCEL      ,
		BTN_BUY_CANCEL      ,
		BTN_BUY_MIT_CANCEL  ,
		BTN_SELECT_CODE     ,
		////////////////////////
		BTN_COUNT
	};

	CEnTabCtrl		m_tabMerchandise;
	CEnTabCtrl		m_tabCurrentInfo;
	CEnTabCtrl		m_tabStopOrder;
	CEnTabCtrl		m_tabMarketContract;


	CMyTabWndFlat		m_wndTabMerchandise;
	CExtLabel			m_labels[LBL_COUNT];
	CxSkinButton		m_buttons[BTN_COUNT];

	CxSkinButton		m_btnTabDisplay;
	CxSkinButton		m_btnTabCurrent;
	CxSkinButton		m_btnTabContract;

	CExtSpinWnd			m_spinVol;
	CExtEdit			m_editVol;
	CExtButton			m_buttonExpand;

	HogaGrid_DSVIP		m_gridHoga;
	CUGCtrl				m_gridMarketContract;
	OrderPanelGrid		m_gridOrderPanel;

	CUGCtrl				m_gridOutstd;
	CFont				m_fontGridBold;
	int m_nPriceType;
	BOOL m_bOneClick;

	CMyTabWndFlat		m_wndTabStop, m_wndTabCurrentInfo, m_wndTabContracts;
	CExtSpinWnd			m_spinEarncut, m_spinLosscut;
	CExtCheckBox		m_checkLosscut, m_checkEarncut;
	CExtEdit			m_editLosscut, m_editEarncut;
	

	CUGCtrl m_gridCurrent;
	CUGCtrl m_gridTotHoga;
	CUGCtrl m_gridContractProfit; // 체결손익
	CUGCtrl m_gridAccountBalance;

	CRect m_rect;

	CPen	m_penGridHeaderBorder;
		
	BOOL				m_bCurrentInit;
	BOOL				m_bHogaInit;
	Code*				m_pSelCode;
	GTMerchandisePtr	m_pMerch;

	OrderContracts*		m_pOrderContracts;
	OrderContracted*	m_pCurContract;


	BOOL	m_bExpand;

	enum SizeMode{sz_small, sz_medium, sz_large };

	SizeMode	m_nSizeMode;
	int			m_nSmallRight;
	int			m_nExpandAreaWidth;
	int			m_nCurRight;
	int			m_nSmallHeight;
	int			m_nCurHeight;

	MerchandiseType		m_curMerchandiseType;
	MarketType			m_curMarketType;
	
	void SetResize(SizeMode nMode);

	struct OrderPanelItem
	{
		Code*			pCode           ;
		MarketType		marketType		;
		CString			strPrice        ;
		PositionType	position        ;
		GT_VOLUME		nVol            ;
		GT_CURRENCY		profitEval		;
		INT  			nOrdNum         ;
		GT_VOLUME		nVolRemain      ;

		OrderPanelItem() : pCode(NULL), marketType(mt_none), position(PT_SELL), profitEval(0), nOrdNum(NUL_IDX), nVolRemain(0) {}
	};
	typedef boost::shared_ptr<OrderPanelItem>	OrderPanelItemPtr;
	
	std::vector<OrderPanelItemPtr>		m_opContracts;
	std::vector<OrderPanelItemPtr>		m_opOutstandings;
	std::vector<OrderPanelItemPtr>		m_opMits;

	class COptionsTableDlg*	m_pOptionsTableDlg;
	class CForeignFuturesCodeDlg* m_pForeignFuturesCodeDlg;
	void UpdateOrderPanel();
	void UpdateMarketContracts(GTCurrent* p);
	Code*				m_pSelContractCode;
	OrderPanelItem*		m_pSelContractOPItem;

	AutoOrder*			m_pAutoOrder;
public:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnBnClickedSelectCode();
	afx_msg void OnBnClickedButtonCorrection();
	afx_msg void OnCbnSelchangeComboCode();
	afx_msg void OnBnClickedButtonMarketSell();
	afx_msg void OnBnClickedButtonMarketBuy();
	afx_msg void OnBnClickedButtonAllLiquid();
	afx_msg void OnBnClickedButtonSellMitCancel();
	afx_msg void OnBnClickedButtonBuyMitCancel();
	afx_msg void OnBnClickedButtonSellCancel();
	afx_msg void OnBnClickedButtonBuyCancel();
	afx_msg void OnBnClickedButtonAllCancel();
	afx_msg void OnBnClickedCheckBigHogaHeight();
	afx_msg void OnBnClickedCheckOneclickOrder();
	afx_msg void OnBnClickedCheckLosscut();
	afx_msg void OnBnClickedCheckEarncut();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedVolume(UINT nID);
	afx_msg void OnBnClickedExpand();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedCheckAlwaysOnTop();
	afx_msg void OnBnClickedCheckStandalone();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();

	afx_msg void OnOrderPanelGridSelChange(NMHDR *pNotifyStruct, LRESULT* /*pResult*/);
	afx_msg void OnHogaGridEndSelChange(NMHDR *pNotifyStruct, LRESULT* /*pResult*/);
	afx_msg void OnMerchandiseTabSelChange(NMHDR *pNMHDR, LRESULT *pResult);
	
	afx_msg LRESULT OnOrderOutstanding(WPARAM wParam, LPARAM lParam); // wParam : OrderOutstandings*
	afx_msg LRESULT OnContractedOrder(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOrderContracts(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnProfitUpdated(WPARAM wParam, LPARAM lParam); // wParam: OrderContracted*
	afx_msg LRESULT OnPreferencesChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOptionsTableSelectCodeChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnVTSModelessDlgDestroyed(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOrderResult(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEnableOrderCountReceived(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClientInfoUpdated(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnVTSFrmShowCompleted(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOrderPanelGridButtonClicked(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

#endif // #if COMPANY==COMPANY_DSVIP