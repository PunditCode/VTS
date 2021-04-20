// OptionsTableDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "OptionsTableDlg.h"
#include "afxdialogex.h"
#include "VTS.h"
#include "FastTradeDlg.h"
#include "ServerEventReceiver.h"


static GridColumnItem g_cols[] = 
{
{_T(""),		0.17},
{_T("거래량"),	0.25},
{_T("등락률"),	0.17},
{_T("대비"),		0.17},
{_T("현재가"),	0.17},
};

const int COL_HALF_COUNT = sizeof(g_cols) / sizeof(GridColumnItem);
const int NCOL_COUNT = (COL_HALF_COUNT * 2) + 1;



void COptionsTableGrid::OnLClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed)
{
	m_pOwner->OnGridLClicked(col, row, updn);
}
void COptionsTableGrid::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	m_pOwner->OnGridLDblClicked(col, row);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
COptionsTableItem::COptionsTableItem(CodeOptions* pCode, int nRow, CUGCtrl& grid) : m_pCode(pCode), m_nRow(nRow), m_grid(grid)
{
	/*m_pMerch = g_pMP->GetObject(m_pCode);
	m_pMerch->GetCurrent()->AttachObserver(this);
	*/
}

COptionsTableItem::~COptionsTableItem()
{
	/*if(m_pMerch)
	{
		m_pMerch->GetCurrent()->DetachObserver(this);
	}*/
}

void COptionsTableItem::RunRecv()
{
	m_pMerch->RunCurrentRecv();
}

void COptionsTableItem::OnCurrentInitUpdated(GTCurrentPtr pCurrent)
{
	Set(pCurrent);
}

void COptionsTableItem::OnCurrentUpdated(GTCurrentPtr pCurrent)
{
	Set(pCurrent);
}

void COptionsTableItem::Set(GTCurrentPtr pCurrent)
{
	CompareStateItem compItem = CompareStateColorFromXing(pCurrent->sign.ToString());
	CString change;
	change.Format(_T("%s %s"), get<2>(compItem), pCurrent->change);
	COLORREF cr = get<0>(compItem);

	if(m_pCode->GetOptionsType() == ot_call)
	{
		int nCol = 1;
	
		m_grid.QuickSetText(nCol, m_nRow, ::ToThousand(pCurrent->volume.ToINT())); ++nCol;
		m_grid.QuickSetText(nCol, m_nRow, pCurrent->drate); ++nCol;
		m_grid.QuickSetText(nCol, m_nRow, change); ++nCol;
		m_grid.QuickSetText(nCol, m_nRow, pCurrent->price); ++nCol;

		nCol = 2;
		m_grid.QuickSetTextColor(nCol, m_nRow,cr);++nCol;
		m_grid.QuickSetTextColor(nCol, m_nRow,cr);++nCol;
		m_grid.QuickSetTextColor(nCol, m_nRow,cr);++nCol;

		m_grid.RedrawRange(0, m_nRow, COL_HALF_COUNT, m_nRow);
	}
	else
	{
		int nCol = NCOL_COUNT - 2;
	
		m_grid.QuickSetText(nCol, m_nRow, ::ToThousand(pCurrent->volume.ToINT())); --nCol;
		m_grid.QuickSetText(nCol, m_nRow, pCurrent->drate); --nCol;
		m_grid.QuickSetText(nCol, m_nRow, change); --nCol;
		m_grid.QuickSetText(nCol, m_nRow, pCurrent->price); --nCol;

		nCol = COL_HALF_COUNT + 1;
		m_grid.QuickSetTextColor(nCol, m_nRow,cr);++nCol;
		m_grid.QuickSetTextColor(nCol, m_nRow,cr);++nCol;
		m_grid.QuickSetTextColor(nCol, m_nRow,cr);++nCol;

		m_grid.RedrawRange(COL_HALF_COUNT + 1, m_nRow, NCOL_COUNT - 1, m_nRow);
	}
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CodeOptions*		COptionsTableDlg::s_pLastSelCode = NULL;
// COptionsTableDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(COptionsTableDlg, OTDBaseDlg)

COptionsTableDlg::COptionsTableDlg(CWnd* pParent /*=NULL*/, BOOL bNight)
	: OTDBaseDlg(COptionsTableDlg::IDD, pParent), m_bClose(FALSE), m_nLastSelectRow(NUL_IDX), m_selPos(PT_NONE)
{
	m_bNight = bNight;
	m_pOwner = NULL;
}

COptionsTableDlg::~COptionsTableDlg()
{
}

void COptionsTableDlg::DoDataExchange(CDataExchange* pDX)
{
	OTDBaseDlg::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_BUTTON_VOL_1, m_buttons[0]);
	//DDX_Control(pDX, IDC_BUTTON_VOL_2, m_buttons[1]);
	//DDX_Control(pDX, IDC_BUTTON_VOL_3, m_buttons[2]);
	//DDX_Control(pDX, IDC_BUTTON_VOL_4, m_buttons[3]);
	DDX_Control(pDX, IDC_STATIC1, m_labelCode);
	DDX_Control(pDX, IDC_BUTTON_UPDATE, m_buttonUpdate);
	
	
}


BEGIN_MESSAGE_MAP(COptionsTableDlg, OTDBaseDlg)
	ON_WM_TIMER()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_VOL_1, IDC_BUTTON_VOL_4, &COptionsTableDlg::OnBnClickedButton)
	ON_MESSAGE(WM_ORDER_CONTRACTS, &COptionsTableDlg::OnOrderContracts)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, &COptionsTableDlg::OnBnClickedButtonUpdate)
END_MESSAGE_MAP()


// COptionsTableDlg 메시지 처리기입니다.
BOOL COptionsTableDlg::OnInitDialog()
{
	OTDBaseDlg::OnInitDialog();
	SetIcon( AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);
	ShowSizeGrip(FALSE);
	
	InstCM().GetCallOptionsDate(m_months);
	if(m_months.empty())
	{
		ErrMsgBox(_T("옵션코드가 존재하지 않습니다"));
		return FALSE;
	}
	SetWindowText(m_bNight ? _T("야간옵션 전광판") : _T("옵션전광판"));
	InitGrid();


	int nMonth = 0;
	if(m_bNight)
	{
		std::vector<CodeOptions*>* pCalls = InstCM().GetCodesCallOptions(m_months[0].first, m_months[0].second);
		// 당일이 옵션만기일일때 야간옵션의 근월물은 차월물로 셋팅한다.
		if(pCalls->at(0)->IsExpireDay())
			nMonth = 1;
	}

	SetMonth(m_months[nMonth].first, m_months[nMonth].second);
#if 0
	int i = 0;
	while(true)
	{
		CString str;
		str.Format(_T("%04d년%02d월"), m_months[i].first, m_months[i].second);
		m_buttons[i].SetWindowText(str);
		m_buttons[i].EnableWindow(TRUE);
		++i;
		if(i == 4 || m_months.size() == i)
			break;
	}
#endif
	AddAnchor(IDC_GRID, __RDA_LT, __RDA_RB);

	SER()->RegisterOrderContracted(this);
	OnOrderContracts(0, 0);

	CRect rectParent, rect;
	m_pOwner->GetParent()->GetWindowRect(rectParent);
	SetWindowPos(NULL, rectParent.right, rectParent.top, 0, 0, SWP_NOSIZE);

#if THEME == THEME_DEFAULT || THEME == THEME_LINETRADE
	m_labelCode.SetBkColor(Colors::White);
	m_buttonUpdate.SetTextColorNormal(Colors::White);
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void COptionsTableDlg::OnBnClickedButton(UINT nID)
{
	m_bClose = TRUE;
	int i = nID - IDC_BUTTON_VOL_1;
	SetMonth(m_months[i].first, m_months[i].second);
}
#define GRID_ROW_HEIGHT		18
#define GRID_TOP_HEADER_HEIGHT 40

void COptionsTableDlg::InitGrid()
{
	m_grid.m_pOwner = this;
	m_grid.AttachGrid(this, IDC_GRID);
	m_grid.m_GI->m_bHeaderFillGradient = FALSE;
	//////////////////////////
	// Default cell
	CUGCell cell;
	m_grid.GetGridDefault(&cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	m_grid.SetGridDefault(&cell);
	m_grid.SetCurrentCellMode(4);
	m_grid.m_GI->m_userSizingMode = FALSE;
//	m_grid.m_GI->m_highlightRowFlag = TRUE; 
//	m_grid.m_GI->m_currentCellMode = 2; 
	//////////////////////////
	// Col, Row
	m_grid.SetNumberCols(NCOL_COUNT);
	m_grid.SetNumberRows(0);
	m_grid.SetTH_NumberRows(2);
	m_grid.SetSH_NumberCols(1);
	m_grid.SetSH_ColWidth(-1, 0);
	m_grid.SetDefRowHeight(GRID_ROW_HEIGHT);
	m_grid.GetHeadingDefault(&cell);
	cell.SetBackColor(CR_GRID_HEADER_BG);
	m_grid.SetHeadingDefault(&cell);

	m_grid.SetTH_Height(GRID_TOP_HEADER_HEIGHT);
	m_grid.SetTH_RowHeight(-1,20);
	m_grid.SetTH_RowHeight(-2,20);

	m_grid.GetColDefault(COL_HALF_COUNT, &cell);
	if(NULL == (HFONT)m_fontPrice)
	{
		CFont* pFont = m_grid.GetFont(0);
		LOGFONT logFont; 
		pFont->GetLogFont(&logFont);
		logFont.lfWeight = 1000;
		m_fontPrice.CreateFontIndirect(&logFont);
	}
	cell.SetBackColor(RGB(232,249,249));
	cell.SetFont(&m_fontPrice);
	cell.SetAlignment(UG_ALIGNCENTER| UG_ALIGNVCENTER);
	m_grid.SetColDefault(COL_HALF_COUNT, &cell);

	m_grid.GetColDefault(0, &cell);
	cell.SetBackColor(RGB(192,217,241));
	m_grid.SetColDefault(0, &cell);

	m_grid.GetColDefault(NCOL_COUNT - 1, &cell);
	cell.SetBackColor(RGB(252,213,181));
	m_grid.SetColDefault(NCOL_COUNT - 1, &cell);

	CRect rt;
	m_grid.GetClientRect(rt);
	int w = rt.Width();// - m_grid.m_GI->m_vScrollWidth;
	int nLeft = 0;
	int nWidth = 0;

	const int w_price = 50;
	int w_half = (w - w_price) / 2;

	for(int nCol = 0 ; nCol < COL_HALF_COUNT ; ++nCol)
	{
		m_grid.QuickSetText(nCol, -1, g_cols[nCol].n);
		nWidth = (int)(w_half * g_cols[nCol].w);
		m_grid.SetColWidth(nCol, nWidth); 
		nLeft += nWidth;
	}

	int nRight = w;
	int _nCol = 0;
	for(int nCol = NCOL_COUNT - 1 ; nCol >= COL_HALF_COUNT + 1 ; --nCol, ++_nCol)
	{
		m_grid.QuickSetText(nCol, -1, g_cols[_nCol].n);
		nWidth = (int)(w_half * g_cols[_nCol].w);
		m_grid.SetColWidth(nCol, nWidth); 
		nRight -= nWidth;
	}

	//행사가
	//m_grid.QuickSetText(COL_HALF_COUNT, -1, _T("행사가"));
	m_grid.SetColWidth(COL_HALF_COUNT, nRight - nLeft); 

	m_grid.JoinCells(0, -2, 0, -1);
	m_grid.QuickSetText(0, -2, _T("체결"));

	m_grid.JoinCells(1, -2, COL_HALF_COUNT - 1, -2);
	m_grid.QuickSetText(1, -2, _T("콜옵션"));
	m_grid.JoinCells(COL_HALF_COUNT, -2, COL_HALF_COUNT, -1);
	m_grid.QuickSetText(COL_HALF_COUNT, -2, _T("행사가"));
	m_grid.JoinCells(COL_HALF_COUNT + 1, -2, NCOL_COUNT - 2, -2);
	m_grid.QuickSetText(COL_HALF_COUNT + 1, -2, _T("풋옵션"));

	m_grid.JoinCells(NCOL_COUNT - 1, -2, NCOL_COUNT - 1, -1);
	m_grid.QuickSetText(NCOL_COUNT - 1, -2, _T("체결"));

}


void COptionsTableDlg::OnGridLClicked(int nCol, int nRow, int updn)
{
	if (nCol == -1 || nRow == -1 || updn != 0)
		return;

	if(NUL_IDX != m_nLastSelectRow && m_selPos != PT_NONE)
	{
		int nBeginCol = m_selPos == PT_SELL ? 1 : COL_HALF_COUNT + 1;
		int nEndCol = m_selPos == PT_SELL ? COL_HALF_COUNT - 1 : NCOL_COUNT - 2;

		for(int nCol = nBeginCol ; nCol <= nEndCol ; ++nCol)
		{
			m_grid.QuickSetBackColor(nCol, m_nLastSelectRow, Colors::White);
			m_grid.RedrawCell(nCol, m_nLastSelectRow);
		}
	}
	

	int nParamCol = NUL_IDX;

	if(0 <= nCol && nCol <= COL_HALF_COUNT)
	{
		nParamCol = 0;
		m_selPos = PT_SELL;
	}
	else if(COL_HALF_COUNT < nCol  && nCol < NCOL_COUNT)
	{
		nParamCol = NCOL_COUNT - 1;
		m_selPos = PT_BUY;
	}

	if(nParamCol != NUL_IDX)
	{
		CUGCell cell;
		m_grid.GetCell(nParamCol, nRow, &cell);

		CodeOptions* pCode = (CodeOptions*)cell.GetParam();
		
		m_labelCode.SetTextColor(true, pCode->GetOptionsType() == ot_call ? Colors::Red : Colors::Blue);
		CString str;
		str.Format(_T("%s %s %s"), pCode->GetCode(), pCode->GetPrice(), pCode->GetOptionsType() == ot_call ? _T("콜") : _T("풋"));
		m_labelCode.SetWindowText(str);

		SetCode(pCode);
	}

	if(m_selPos != PT_NONE)
	{
		m_nLastSelectRow = nRow;
		int nBeginCol = m_selPos == PT_SELL ? 1 : COL_HALF_COUNT + 1;
		int nEndCol = m_selPos == PT_SELL ? COL_HALF_COUNT - 1 : NCOL_COUNT - 2;

		for(int nCol = nBeginCol ; nCol <= nEndCol ; ++nCol)
		{
			m_grid.QuickSetBackColor(nCol, m_nLastSelectRow, Colors::Yellow);
			m_grid.RedrawCell(nCol, m_nLastSelectRow);
		}
	}
}
void COptionsTableDlg::OnGridLDblClicked(int nCol, int nRow)
{

}
void COptionsTableDlg::SetCode(CodeOptions* pCode)
{
	ASSERT(m_pOwner);
	s_pLastSelCode = pCode;
	m_pOwner->SendMessage(WM_OPTIONS_TABLE_SELECT_CODE_CHANGED, (WPARAM)s_pLastSelCode, (LPARAM)(m_bNight == TRUE ? mt_EUREX : mt_default));
}



void COptionsTableDlg::SetMonth(UINT nYear, UINT nMonth)
{
	m_items.clear();

	std::vector<CodeOptions*>* pCalls = InstCM().GetCodesCallOptions(nYear, nMonth);
	std::vector<CodeOptions*>* pPuts = InstCM().GetCodesPutOptions(nYear, nMonth);

	ASSERT(pCalls->size() == pPuts->size());

	const int nRowCount = pCalls->size();
	m_grid.SetNumberRows(nRowCount);
	int n = nRowCount - 1;

	for(int nRow = 0 ; nRow < nRowCount ; ++nRow, --n)
	{
		CodeOptions* pCodeCall = pCalls->at(n);
		CodeOptions* pCodePut = pPuts->at(n);

		ASSERT(pCodeCall->GetPrice() == pCodePut->GetPrice());

		CUGCell cell;
		m_grid.GetCell(0, nRow, &cell);
		cell.SetParam((long)pCodeCall);
		m_grid.SetCell(0, nRow, &cell);

		cell.ClearAll();
		m_grid.GetCell(NCOL_COUNT - 1, nRow, &cell);
		cell.SetParam((long)pCodePut);
		m_grid.SetCell(NCOL_COUNT - 1, nRow, &cell);


		m_grid.QuickSetText(COL_HALF_COUNT, nRow, pCodeCall->GetPrice());

		Item item;
		item.nRow = nRow;
		item.pCall = COptionsTableItemPtr(new COptionsTableItem(pCalls->at(n), nRow, m_grid));
		item.pPut = COptionsTableItemPtr(new COptionsTableItem(pPuts->at(n), nRow, m_grid));

		m_items.insert(make_pair(pCodeCall->GetPrice(), item));
	}

	WINDOWPLACEMENT wp;
	
	m_grid.GetWindowPlacement(&wp);
	wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + (nRowCount * (GRID_ROW_HEIGHT+ 1)) + GRID_TOP_HEADER_HEIGHT - 12;
	m_grid.SetWindowPlacement(&wp);

	CRect rt;
	GetClientRect(rt);

	CRect rtWnd;
	GetWindowRect(rtWnd);
	rt.bottom = (rtWnd.Height() - rt.Height()) + wp.rcNormalPosition.bottom + 5;
	rt.right += (rtWnd.Width() - rt.Width());
	MoveWindow(rt);

	m_buttonUpdate.EnableWindow(FALSE);
	GetSession()->RequestOptionsTable(GetSafeHwnd(), nYear, nMonth, m_bNight);
	BeginCheckTimer();
}

void COptionsTableDlg::OnRequestReceived(DKBuffer& dbuf, LPVOID lpData)
{
	m_buttonUpdate.EnableWindow(TRUE);

	INT yyyy, mm;
	BOOL bNight;

	dbuf.RD(yyyy);
	dbuf.RD(mm);
	dbuf.RD(bNight);

	CString		recentPrice;
	CString		recentSign;
	CString		recentChange;
	CString		recentDiff;

	dbuf.RD(recentPrice);
	dbuf.RD(recentSign);
	dbuf.RD(recentChange);
	dbuf.RD(recentDiff);

	//::CompareStateColorFromXing

	///요청한 월물이 맞는지 확인해야함
	CString str;
	UINT nCount;

	dbuf.RD(nCount);

	CString    actprice   ;    // [float ,  6.2] 행사가        
	CString    optcode    ;    // [string,    8] 콜옵션코드     
	CString    price      ;    // [float ,  6.2] 현재가        
	CString    sign       ;    // [string,    1] 전일대비구분   
	CString    change     ;    // [float ,  6.2] 전일대비      
	CString    drate       ;    // [float ,  6.2] 등락율        
	GT_VOLUME  volume     ;    // [long  ,   12] 거래량        

	DOUBLE dRecentPrice = ::ToDOUBLE(recentPrice);
	DOUBLE dMinDiff = DBL_MAX;
	int nRecentRow = NUL_IDX;
	int nLastSel = NUL_IDX;
	for(UINT i = 0 ; i < nCount ; ++i)
	{
		dbuf.RD(actprice);
		dbuf.RD(optcode);
		dbuf.RD(price);
		dbuf.RD(sign);
		dbuf.RD(change);
		dbuf.RD(drate);
		dbuf.RD(volume);

		auto pos = m_items.find(actprice);
		if(pos != m_items.end())
		{
			int nRow = pos->second.nRow;
			CompareStateItem compItem = CompareStateColorFromXing(sign);
			str.Format(_T("%s %s"), get<2>(compItem), change);
			COLORREF cr = get<0>(compItem);

			int nCol = 4;
			m_grid.QuickSetText(nCol, nRow, price); m_grid.QuickSetTextColor(nCol, nRow,cr); --nCol;
			m_grid.QuickSetText(nCol, nRow, str); m_grid.QuickSetTextColor(nCol, nRow,cr); --nCol;
			m_grid.QuickSetText(nCol, nRow, drate); m_grid.QuickSetTextColor(nCol, nRow,cr); --nCol;
			m_grid.QuickSetText(nCol, nRow, ::ToThousand(volume));

			DOUBLE dPrice = ::ToDOUBLE(actprice);
			DOUBLE dDiff = fabs(dPrice - dRecentPrice);
			if(dDiff < dMinDiff)
			{
				nRecentRow = nRow;
				dMinDiff = dDiff;
			}

			if(s_pLastSelCode && s_pLastSelCode->GetPrice() == actprice)
				nLastSel = nRow;
		}
	}

	dbuf.RD(nCount);
	for(UINT i = 0 ; i < nCount ; ++i)
	{
		dbuf.RD(actprice);
		dbuf.RD(optcode);
		dbuf.RD(price);
		dbuf.RD(sign);
		dbuf.RD(change);
		dbuf.RD(drate);
		dbuf.RD(volume);

		auto pos = m_items.find(actprice);
		if(pos != m_items.end())
		{
			int nRow = pos->second.nRow;
			CompareStateItem compItem = CompareStateColorFromXing(sign);
			str.Format(_T("%s %s"), get<2>(compItem), change);
			COLORREF cr = get<0>(compItem);

			int nCol = 6;
			m_grid.QuickSetText(nCol, nRow, price); m_grid.QuickSetTextColor(nCol, nRow,cr); ++nCol;
			m_grid.QuickSetText(nCol, nRow, str); m_grid.QuickSetTextColor(nCol, nRow,cr); ++nCol; 
			m_grid.QuickSetText(nCol, nRow, drate); m_grid.QuickSetTextColor(nCol, nRow,cr); ++nCol;
			m_grid.QuickSetText(nCol, nRow, ::ToThousand(volume));
		}
	}

	m_grid.RedrawAll();

	
	
	if(m_nLastSelectRow == NUL_IDX)
	{
		m_grid.QuickSetBackColor(5, nRecentRow, Colors::Red);
		m_grid.QuickSetTextColor(5, nRecentRow, Colors::White);
		m_grid.RedrawCell(5, nRecentRow);

		if(nLastSel != NUL_IDX)
			OnGridLClicked(s_pLastSelCode->GetOptionsType() == ot_call ? 1 : COL_HALF_COUNT + 1, nLastSel, 0);
		else
			OnGridLClicked(1, nRecentRow, 0);
	}
	
}

void COptionsTableDlg::OnRequestError(int nRequestID, const CString& strMsg)
{
	ErrMsgBox(_T("옵션전광판요청이 실패했습니다."));
}

void COptionsTableDlg::OnTimer(UINT_PTR nIDEvent)
{
	KillTimer(nIDEvent);
	EndWaitCursor();
	OTDBaseDlg::OnTimer(nIDEvent);
}

void COptionsTableDlg::RunRecvAllFnc()
{
	for(auto pos = m_items.begin() ; pos != m_items.end() ; ++pos)
	{
		Item& item = pos->second;

		Sleep(300);
		if(m_bClose)
		{
			TRACE(_T("Break recv\n"));
			return;
		}

		item.pCall->RunRecv();
		item.pPut->RunRecv();
	}
}


LRESULT COptionsTableDlg::OnOrderContracts(WPARAM wParam, LPARAM lParam)
{
	int nCol = 0;
	for(int nRow = 0 ; nRow < m_grid.GetNumberRows() ; ++nRow)
	{
		nCol = 0;
		CString str = m_grid.QuickGetText(nCol, nRow);
		if(FALSE == str.IsEmpty())
		{
			m_grid.QuickSetText(nCol, nRow, _T(""));
			m_grid.RedrawCell(nCol, nRow);
		}

		nCol = NCOL_COUNT - 1;
		str = m_grid.QuickGetText( nCol, nRow);
		if(FALSE == str.IsEmpty())
		{
			m_grid.QuickSetText( nCol, nRow, _T(""));
			m_grid.RedrawCell( nCol, nRow);
		}
	}

	const OrderContracts& contracts = SER()->GetContracts();
	for(auto pos = contracts.begin() ; pos != contracts.end() ; ++pos)
	{
		OrderContracted* pOrder = (*pos).get();

		if(pOrder->pCode->GetType() == ct_options)
		{
			CodeOptions* pCode = dynamic_cast<CodeOptions*>(pOrder->pCode);

			auto posItem = m_items.find(pCode->GetPrice());
			ASSERT(posItem != m_items.end());

			int nCol = NUL_IDX;
			if(posItem->second.pCall->m_pCode == pCode)
			{
				nCol = 0;
				
			}
			else if(posItem->second.pPut->m_pCode == pCode)
			{
				nCol = NCOL_COUNT - 1;
			}

			if(NUL_IDX != nCol)
			{
				m_grid.QuickSetText(nCol, posItem->second.nRow, ::ToString(pOrder->nVolume));
				m_grid.RedrawCell(nCol, posItem->second.nRow);
			}
		}
	}
	return 0;
}

void COptionsTableDlg::OnClose()
{
	SER()->UnregisterOrderContracted(this);
	OTDBaseDlg::OnClose();
}

void COptionsTableDlg::PostNcDestroy()
{
	m_bClose = TRUE;
	OTDBaseDlg::PostNcDestroy();
}

void COptionsTableDlg::OnBnClickedButtonUpdate()
{
	GetSession()->RequestOptionsTable(GetSafeHwnd(), m_months[0].first, m_months[0].second, m_bNight);
	BeginCheckTimer();
	
	m_buttonUpdate.EnableWindow(FALSE);
}

