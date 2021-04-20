// OrderHistoryDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VTS.h"
#include "OrderHistoryDlg.h"
#include "afxdialogex.h"


// COrderHistoryDlg 대화 상자입니다.

IMPLEMENT_DYNCREATE(COrderHistoryDlg, CRequestRecvDlg)

COrderHistoryDlg::COrderHistoryDlg(CWnd* pParent /*=NULL*/)
	: CRequestRecvDlg(COrderHistoryDlg::IDD, pParent)
{
}

COrderHistoryDlg::~COrderHistoryDlg()
{
}

void COrderHistoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CRequestRecvDlg::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_DATEBEGIN, m_dateBegin);
	DDX_Control(pDX, IDC_DATEEND, m_dateEnd);
	DDX_Control(pDX, IDC_GRID, m_grid);
}


BEGIN_MESSAGE_MAP(COrderHistoryDlg, CRequestRecvDlg)
	ON_BN_CLICKED(IDC_BUTTON_QUERY, &COrderHistoryDlg::OnBnClickedButtonQuery)
	ON_BN_CLICKED(IDC_BUTTON_EXCEL, &COrderHistoryDlg::OnBnClickedButtonExcel)
	ON_BN_CLICKED(IDC_CHECK_CONTRACT, &COrderHistoryDlg::OnBnClickedCheckContract)
END_MESSAGE_MAP()


// COrderHistoryDlg 메시지 처리기입니다.


BOOL COrderHistoryDlg::OnInitDialog()
{
	CRequestRecvDlg::OnInitDialog();

	InitGrid();
	
	AddAnchor(IDC_GRID, __RDA_LT, __RDA_RB);
	/*AddAnchor(IDC_BUTTON_QUERY, __RDA_RT);
	AddAnchor(IDC_CHECK_CONTRACT, __RDA_RT);
	AddAnchor(IDC_DATE, __RDA_RT);
	AddAnchor(IDC_STATIC_DATE, __RDA_RT);*/

	//m_date.ShowDropDownButton( FALSE );
	//m_date.ShowSpinButton( FALSE );
	//m_date.NoHideSelectionSet( TRUE );
	m_dateBegin.SetTimeFormat( CExtDateTimeWnd::h24 );
	m_dateBegin.SetMode( CExtDateTimeWnd::date );
	m_dateBegin.SetDateTime(COleDateTime::GetCurrentTime());
	m_dateBegin.m_lParamCookie = LPARAM(m_dateBegin.GetDlgCtrlID());

	m_dateEnd.SetTimeFormat( CExtDateTimeWnd::h24 );
	m_dateEnd.SetMode( CExtDateTimeWnd::date );
	m_dateEnd.SetDateTime(COleDateTime::GetCurrentTime());
	m_dateEnd.m_lParamCookie = LPARAM(m_dateEnd.GetDlgCtrlID());

	AddAnchor(IDC_BUTTON_EXCEL, __RDA_X);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void COrderHistoryDlg::InitGrid()
{
	m_grid.SetDefaultStyle(VTSGrid::sel_full_row);
	m_grid.OuterRowCountTopSet(2, false);

	VTSGridColDefs defs;
	defs.push_back(make_pair(_T("날짜"),			0.10)); 
	defs.push_back(make_pair(_T("시간"),			0.10)); 
	defs.push_back(make_pair(_T("주문번호"),		0.09)); 
	defs.push_back(make_pair(_T("종목"),			0.08)); 
	defs.push_back(make_pair(_T("구분"),			0.07)); 
	defs.push_back(make_pair(_T("상태"),			0.07)); 
	defs.push_back(make_pair(_T("유형"),			0.07)); 
	defs.push_back(make_pair(_T("주문량"),		0.08)); 
	defs.push_back(make_pair(_T("체결량"),		0.08)); 
	defs.push_back(make_pair(_T("주문가"),		0.08)); 
	defs.push_back(make_pair(_T("체결가"),		0.08)); 
	defs.push_back(make_pair(_T("손익"),			0.09)); // 11
	defs.push_back(make_pair(_T("수수료"),		0.09)); // 12
	defs.push_back(make_pair(_T("총손익"),		0.09)); // 13
	defs.push_back(make_pair(_T("원번호"),		0.08)); // 14
	defs.push_back(make_pair(_T("호출"),			0.08)); 
	m_grid.SetColumDef(defs, true, 0, RGB(253, 234, 218));

	for(UINT nCol = 0 ; nCol < defs.size() ; ++nCol)
	{
		CExtGridCellHeader * pHeader = m_grid.QuickSetHeaderText(nCol, 1, nCol == 0 ? _T("합계") : _T(""));
		pHeader	-> ModifyStyle(nCol == 0  ? __EGCS_TA_HORZ_CENTER : __EGCS_TA_HORZ_RIGHT);
		pHeader	-> ModifyStyleEx(__EGCS_EX_DISABLE_SORT);
		pHeader	->BackColorSet(CExtGridCell::__ECS_ALL, RGB(252, 213, 181));
	}
}
//
//void COrderHistoryDlg::OnDtnDatetimechangeDate(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(TRUE);
//	*pResult = 0;
//}

void COrderHistoryDlg::OnBnClickedButtonQuery()
{
	COleDateTime tmBegin = m_dateBegin.GetDateTime();
	COleDateTime tmEnd = m_dateEnd.GetDateTime();

	if (tmBegin > tmEnd)
		std::swap(tmBegin, tmEnd);
	COleDateTimeSpan tmSpan = tmEnd - tmBegin;
	if (tmSpan.GetDays() > 6)
	{
		ErrMsgBox(_T("조회기간을 6일이하로 설정하시기 바랍니다."));
		return;
	}

	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_CONTRACT))->GetCheck() == BST_CHECKED;

	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(FALSE);
	GetSession()->RequestOrderHistory(GetSafeHwnd(), tmBegin, tmEnd, bCheck);
	BeginCheckTimer();
}

void COrderHistoryDlg::OnRequestReceived(DKBuffer& dbuf, LPVOID lpData)
{
	UINT nColCount, nRowCount;
	dbuf.RD(nColCount);
	dbuf.RD(nRowCount);

	GT_CURRENCY totalFee, totalProfit, totalRealProfit;
	dbuf.RD(totalFee);
	dbuf.RD(totalProfit);
	dbuf.RD(totalRealProfit);

	m_grid.RowRemoveAll(false);
	m_grid.RowAdd(nRowCount);

	CString str;
	str = ::ToThousand(totalProfit);

	m_grid.QuickSetHeaderText(11, 1, str);
	m_grid.QuickSetHeaderTextColor(11, 1, ::GetProfitColor(totalProfit));
	
	str = ::ToThousand(totalFee);
	m_grid.QuickSetHeaderText(12, 1, str);
	m_grid.QuickSetHeaderTextColor(12, 1, Colors::Blue);
	
	str = ::ToThousand(totalRealProfit);
	m_grid.QuickSetHeaderText(13, 1, str);
	m_grid.QuickSetHeaderTextColor(13, 1, ::GetProfitColor(totalRealProfit));
	

	for(UINT nRow = 0 ; nRow < nRowCount ; ++nRow)
	{
		for(UINT nCol = 0 ; nCol < nColCount ; ++nCol)
		{
			if(nCol == 11 || nCol == 12 || nCol == 13)
			{
				LONGLONG llVal;
				dbuf.RD(llVal);

				COLORREF crText;
				if(nCol == 11 || nCol == 13) //손익, 총손익
				{
					crText = ::GetProfitColor(llVal);
					
				}
				else if(nCol == 12) //수수료
				{
					crText = Colors::Blue;
				} 

				str = ::ToThousand(llVal);

				CExtGridCellString* pCell = m_grid.QuickSetText(nCol, nRow, str);
				pCell->ModifyStyle(pCell->GetStyle() | __EGCS_TA_HORZ_RIGHT);
				m_grid.QuickSetTextColor(nCol, nRow, crText);
				
			}
			else
			{
				
				dbuf.RD(str);

				if(nCol == 4) //구분
				{
					COLORREF cr = str == _T("매수") ? Colors::Red : Colors::Blue;

					m_grid.QuickSetTextColor(nCol, nRow, cr);
					m_grid.QuickSetTextColor(nCol + 1, nRow, cr);
				}
				m_grid.QuickSetText(nCol, nRow, str);
			
			}
		}
	}
	m_grid.OnSwUpdateScrollBars();
	m_grid.BestFitColumns();
	m_grid.OnSwInvalidate(false);

	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(TRUE);
}

void COrderHistoryDlg::OnRequestError(int nRequestID, const CString& strMsg)
{
	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(TRUE);
	if (NUL_IDX != nRequestID)
		ErrMsgBox(_T("거래내역 요청이 실패했습니다."));
}

void COrderHistoryDlg::OnBnClickedButtonExcel()
{
	m_grid.ToExcel();
}


void COrderHistoryDlg::OnBnClickedCheckContract()
{
	OnBnClickedButtonQuery();
}
