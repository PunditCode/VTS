// RecommendersClientTotalProfitDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VTS.h"
#include "RecommendersClientTotalProfitDlg.h"
#include "afxdialogex.h"


// CRecommendersClientTotalProfitDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRecommendersClientTotalProfitDlg, CRequestRecvDlg)

CRecommendersClientTotalProfitDlg::CRecommendersClientTotalProfitDlg(CWnd* pParent /*=NULL*/)
: CRequestRecvDlg(CRecommendersClientTotalProfitDlg::IDD, pParent)
{
}

CRecommendersClientTotalProfitDlg::~CRecommendersClientTotalProfitDlg()
{
}

void CRecommendersClientTotalProfitDlg::DoDataExchange(CDataExchange* pDX)
{
	CRequestRecvDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRID, m_grid);
	DDX_Control(pDX, IDC_COMBO_QUERY_FIELD, m_comboQueryField);
	DDX_Control(pDX, IDC_DATETIMEPICKER_BEGIN, m_dateBegin);
	DDX_Control(pDX, IDC_DATETIMEPICKER_END, m_dateEnd);
}


BEGIN_MESSAGE_MAP(CRecommendersClientTotalProfitDlg, CRequestRecvDlg)
	ON_BN_CLICKED(IDC_BUTTON_QUERY, &CRecommendersClientTotalProfitDlg::OnBnClickedButtonQuery)
	ON_BN_CLICKED(IDC_BUTTON_TO_EXCEL, &CRecommendersClientTotalProfitDlg::OnBnClickedButtonToExcel)
END_MESSAGE_MAP()


// CRecommendersClientTotalProfitDlg 메시지 처리기입니다.

BOOL CRecommendersClientTotalProfitDlg::OnInitDialog()
{
	CRequestRecvDlg::OnInitDialog();
	ShowSizeGrip(FALSE);
	InitGrid();

	int n = 0;
	m_comboQueryField.InsertString(n++, _T("ID"));
	m_comboQueryField.InsertString(n++, _T("이름"));
	m_comboQueryField.InsertString(n++, _T("휴대폰"));
	m_comboQueryField.InsertString(n++, _T("이메일"));
	m_comboQueryField.SetCurSel(1);


	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	AddAnchor(IDC_GRID, __RDA_LT, __RDA_RB);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CRecommendersClientTotalProfitDlg::InitGrid()
{
	m_grid.SetDefaultStyle(VTSGrid::sel_full_row);
	m_grid.OuterRowCountTopSet(2, false);
	//m_grid.OuterColumnCountLeftSet(1);
	//m_grid.OuterColumnWidthSet(true, 0, 20);

	VTSGridColDefs defs;
	defs.push_back(std::make_pair(_T("가입일자"), 0.12));
	defs.push_back(std::make_pair(_T("아이디"), 0.07));
	defs.push_back(std::make_pair(_T("이름"), 0.07));
	defs.push_back(std::make_pair(_T("추천인"), 0.08));
	defs.push_back(std::make_pair(_T("버전"), 0.06)); //4
	defs.push_back(std::make_pair(_T("매매손익"), 0.08)); //5
	defs.push_back(std::make_pair(_T("선물손익"), 0.08));
	defs.push_back(std::make_pair(_T("옵션손익"), 0.08));
	defs.push_back(std::make_pair(_T("야간선물손익"), 0.09));
	defs.push_back(std::make_pair(_T("야간옵션손익"), 0.09));
	defs.push_back(std::make_pair(_T("해외선물손익"), 0.09));
	defs.push_back(std::make_pair(_T("총손익합"), 0.08)); //11
	defs.push_back(std::make_pair(_T("선물수수료"), 0.09)); //12
	defs.push_back(std::make_pair(_T("옵션수수료"), 0.09));
	defs.push_back(std::make_pair(_T("야간선물수수료"), 0.09));
	defs.push_back(std::make_pair(_T("야간옵션수수료"), 0.09));
	defs.push_back(std::make_pair(_T("해외선물수수료"), 0.09));
	defs.push_back(std::make_pair(_T("총수수료합"), 0.09)); //17
	defs.push_back(std::make_pair(_T("거래횟수"), 0.07));
	defs.push_back(std::make_pair(_T("거래일"), 0.06));

	CRect rt;
	m_grid.GetClientRect(rt);
	int w = rt.Width();
	int nLeft = 0;
	int nWidth = 0;
	for (UINT nCol = 0; nCol < defs.size(); ++nCol)
	{
		nWidth = (int)(w * defs[nCol].second);

		CString strCategory;

		if (0 <= nCol && nCol <= 4)
			strCategory = _T("고객정보");
		else if (6 <= nCol && nCol <= 11)
			strCategory = _T("순손익");
		else if (12 <= nCol && nCol <= 17)
			strCategory = _T("수수료");
		else
			strCategory = defs[nCol].first;

		m_grid.ColumnRegister(defs[nCol].first, strCategory, nWidth);
		nLeft += nWidth;
	}

	for (UINT nCol = 0; nCol < defs.size(); ++nCol)
	{
		m_grid.QuickSetHeaderBackColor(nCol, 0, CR_GRID_HEADER_BG);
		m_grid.QuickSetHeaderBackColor(nCol, 1, CR_GRID_HEADER_BG);
	}
}


void CRecommendersClientTotalProfitDlg::OnBnClickedButtonQuery()
{
	COleDateTime tmBegin, tmEnd;
	m_dateBegin.GetTime(tmBegin);
	m_dateEnd.GetTime(tmEnd);

	if (tmBegin > tmEnd)
		std::swap(tmBegin, tmEnd);

	CString strQuery;
	GetDlgItemText(IDC_EDIT_QUERY, strQuery);

	int nQueryField = m_comboQueryField.GetCurSel();

	BeginWaitCursor();
	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(FALSE);
	GetSession()->RequestRecommenderClientTotalProfit(GetSafeHwnd(), nQueryField, strQuery, tmBegin, tmEnd);
	BeginCheckTimer();
}

void CRecommendersClientTotalProfitDlg::OnCbnSelchangeComboLeague()
{
	OnBnClickedButtonQuery();
}

void CRecommendersClientTotalProfitDlg::OnRequestReceived(DKBuffer& dbuf, LPVOID lpData)
{
	EndWaitCursor();


	UINT nColCount, nRowCount;
	dbuf.RD(nColCount);
	dbuf.RD(nRowCount);

	m_grid.RowRemoveAll(false);
	m_grid.RowAdd(nRowCount);

	CString str;
	LONGLONG llVal;
	int nVal;

	for (UINT nRow = 0; nRow < nRowCount; ++nRow)
	{
		for (UINT nCol = 0; nCol < nColCount; ++nCol)
		{
			if (0 <= nCol && nCol <= 4)
			{
				dbuf.RD(str);
				if (nCol == 4)
					continue; //skip version
			}
			else if (5 <= nCol && nCol <= 17)
			{
				dbuf.RD(llVal);
				str = ::ToThousand(llVal);

				if (nCol == 5 || nCol == 11)
				{
					m_grid.QuickSetTextColor(nCol, nRow, ::GetProfitColor(llVal));
				}
				else if (nCol == 17)
					m_grid.QuickSetTextColor(nCol, nRow, Colors::Blue);

				m_grid.SetAlign(nCol, nRow, VTSGrid::align_right);
			}
			else
			{
				dbuf.RD(nVal);
				str = ::ToString(nVal);
			}

			m_grid.QuickSetText(nCol, nRow, str);
		}
	}

	m_grid.OnSwUpdateScrollBars();
	m_grid.OnSwInvalidate(false);
	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(TRUE);
}

void CRecommendersClientTotalProfitDlg::OnRequestError(int nRequestID, const CString& strMsg)
{
	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(TRUE);
	if (NUL_IDX != nRequestID)
		ErrMsgBox(_T("조회요청이 실패했습니다."));
}

void CRecommendersClientTotalProfitDlg::OnBnClickedButtonToExcel()
{
	m_grid.ToExcel();
}
