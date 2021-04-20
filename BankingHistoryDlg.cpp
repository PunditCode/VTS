// BankingHistoryDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VTS.h"
#include "BankingHistoryDlg.h"
#include "afxdialogex.h"


// CBankingHistoryDlg 대화 상자입니다.

IMPLEMENT_DYNCREATE(CBankingHistoryDlg, CRequestRecvDlg)

CBankingHistoryDlg::CBankingHistoryDlg(CWnd* pParent /*=NULL*/)
	: CRequestRecvDlg(CBankingHistoryDlg::IDD, pParent)
	, m_nRequestType(2)
{
	// 0: 입금, 1:출금, 2:전체
}

CBankingHistoryDlg::~CBankingHistoryDlg()
{
}

void CBankingHistoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CRequestRecvDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRID, m_grid);
	DDX_Control(pDX, IDC_DATEBEGIN, m_dateBegin);
	DDX_Control(pDX, IDC_DATEEND, m_dateEnd);
	
	DDX_Control(pDX, IDC_RADIO1, m_radio[0]);
	DDX_Control(pDX, IDC_RADIO2, m_radio[1]);
	DDX_Control(pDX, IDC_RADIO3, m_radio[2]);
	
	DDX_Radio(pDX, IDC_RADIO1, m_nRequestType);
}


BEGIN_MESSAGE_MAP(CBankingHistoryDlg, CRequestRecvDlg)
	ON_BN_CLICKED(IDC_BUTTON_QUERY, &CBankingHistoryDlg::OnBnClickedButtonQuery)
	ON_BN_CLICKED(IDC_BUTTON_EXCEL, &CBankingHistoryDlg::OnBnClickedButtonExcel)
END_MESSAGE_MAP()


// CBankingHistoryDlg 메시지 처리기입니다.


BOOL CBankingHistoryDlg::OnInitDialog()
{
	CRequestRecvDlg::OnInitDialog();

	InitGrid();
	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	AddAnchor(IDC_GRID, __RDA_LT, __RDA_RB);
	//AddAnchor(IDC_BUTTON_QUERY, __RDA_RT);

	COleDateTime tm(COleDateTime::GetCurrentTime());
	if(tm.GetHour() < 7)
	{
		tm -= COleDateTimeSpan(1, 0, 0, 0);
	}

	m_dateBegin.SetTimeFormat( CExtDateTimeWnd::h24 );
	m_dateBegin.SetMode( CExtDateTimeWnd::date );
	m_dateBegin.SetDateTime(tm);
	m_dateBegin.m_lParamCookie = LPARAM(m_dateBegin.GetDlgCtrlID());

	m_dateEnd.SetTimeFormat( CExtDateTimeWnd::h24 );
	m_dateEnd.SetMode( CExtDateTimeWnd::date );
	m_dateEnd.SetDateTime(tm);
	m_dateEnd.m_lParamCookie = LPARAM(m_dateEnd.GetDlgCtrlID());
		
	AddAnchor(IDC_BUTTON_EXCEL, __RDA_X);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


static GridColumnItem g_cols[] = 
{
{_T("요청시간"),		0.20},
{_T("구분"),			0.07},
{_T("요청금액"),		0.15},
{_T("처리시간"),		0.20},
{_T("처리금액"),		0.15},
{_T("메모"),			0.20},
};

const int COL_COUNT = sizeof(g_cols) / sizeof(GridColumnItem);

void CBankingHistoryDlg::InitGrid()
{
	m_grid.SetDefaultStyle(VTSGrid::sel_full_row);

	VTSGridColDefs defs;
	defs.push_back(std::make_pair(_T("요청시간"),		0.20)); 
	defs.push_back(std::make_pair(_T("구분"),		0.07));
	defs.push_back(std::make_pair(_T("요청금액"),		0.15));
	defs.push_back(std::make_pair(_T("처리시간"),		0.20));
	defs.push_back(std::make_pair(_T("처리금액"),		0.15));
	defs.push_back(std::make_pair(_T("메모"),		0.20));
	
	m_grid.SetColumDef(defs, true);
	//CR_GRID_HEADER_BG
}

void CBankingHistoryDlg::OnBnClickedButtonQuery()
{
	UpdateData(TRUE);

	COleDateTime tmBegin = m_dateBegin.GetDateTime();
	COleDateTime tmEnd = m_dateEnd.GetDateTime();

	if (tmBegin > tmEnd)
		std::swap(tmBegin, tmEnd);
	COleDateTimeSpan tmSpan = tmEnd - tmBegin;
	if (tmSpan.GetDays() > MAX_QUERY_PERIOD_DAY)
	{
		ErrMsgBox(_T("조회기간을 %d일이하로 설정하시기 바랍니다."), MAX_QUERY_PERIOD_DAY);
		return;
	}

	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(FALSE);

	GetSession()->RequestBankingHistory(GetSafeHwnd(), tmBegin, tmEnd, (VTSBankingHistoryRequestType)m_nRequestType);

	BeginCheckTimer();
}

void CBankingHistoryDlg::OnRequestReceived(DKBuffer& dbuf, LPVOID lpData)
{
	UINT nColCount, nRowCount;
	dbuf.RD(nColCount);
	dbuf.RD(nRowCount);

	m_grid.RowRemoveAll(false);
	m_grid.RowAdd(nRowCount);
	for(UINT nRow = 0 ; nRow < nRowCount ; ++nRow)
	{
		CString str;
		COLORREF cr = 0;
		for(UINT nCol = 0 ; nCol < nColCount ; ++nCol)
		{
			dbuf.RD(str);
			m_grid.QuickSetText(nCol, nRow, str);
			if(1 == nCol)
			{
				cr = (str == _T("입금")) ? Colors::Red : Colors::Blue;
			}
			else if(2 == nCol || 4 == nCol)
				m_grid.SetAlign(nCol, nRow, VTSGrid::align_right);
		}
		m_grid.QuickSetTextColor(1, nRow, cr);
		m_grid.QuickSetTextColor(2, nRow, cr);
		m_grid.QuickSetTextColor(4, nRow, cr);
	}

	m_grid.OnSwUpdateScrollBars();
	m_grid.OnSwInvalidate( false );

	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(TRUE);
}

void CBankingHistoryDlg::OnRequestError(int nRequestID, const CString& strMsg)
{
	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(TRUE);
	if (NUL_IDX != nRequestID)
		ErrMsgBox(_T("거래내역 요청이 실패했습니다."));
}


void CBankingHistoryDlg::OnBnClickedButtonExcel()
{
	m_grid.ToExcel();
}
