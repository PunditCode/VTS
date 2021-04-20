// RecommenderMembersProfitDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VTS.h"
#include "RecommenderMembersProfitDlg.h"

void RecommenderMembersProfitGrid::OnGbwSelectionChanged()
{
	CRect rtSel = SelectionGet(0, -1);
	if(rtSel.left != -1 && rtSel.top != -1)
		m_pOwner->OnGridSelChanged(rtSel.left, rtSel.top);
}

// CRecommenderMembersProfitDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRecommenderMembersProfitDlg, CRequestRecvDlg)

CRecommenderMembersProfitDlg::CRecommenderMembersProfitDlg(CWnd* pParent /*=NULL*/)
	: CRequestRecvDlg(CRecommenderMembersProfitDlg::IDD, pParent)
{
}

CRecommenderMembersProfitDlg::~CRecommenderMembersProfitDlg()
{
}

void CRecommenderMembersProfitDlg::DoDataExchange(CDataExchange* pDX)
{
	CRequestRecvDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_QUERY_FIELD, m_comboQueryField);
	DDX_Control(pDX, IDC_DATEBEGIN, m_dateBegin);
	DDX_Control(pDX, IDC_DATEEND, m_dateEnd);
	DDX_Control(pDX, IDC_STATIC1, m_label1);
	DDX_Control(pDX, IDC_STATIC2, m_label2);
	DDX_Control(pDX, IDC_GRID_PROFIT, m_gridProfit);
	DDX_Control(pDX, IDC_GRID_CLIENT, m_gridClient);
}


BEGIN_MESSAGE_MAP(CRecommenderMembersProfitDlg, CRequestRecvDlg)
	ON_BN_CLICKED(IDC_BUTTON_QUERY, &CRecommenderMembersProfitDlg::OnBnClickedButtonQuery)
	ON_BN_CLICKED(IDC_BUTTON_QUERY_CLIENT, &CRecommenderMembersProfitDlg::OnBnClickedButtonQueryClient)
END_MESSAGE_MAP()


// CRecommenderMembersProfitDlg 메시지 처리기입니다.

BOOL CRecommenderMembersProfitDlg::OnInitDialog()
{
	CRequestRecvDlg::OnInitDialog();

	ShowSizeGrip(FALSE);
	InitClientGrid();
	InitGrid();

	int n = 0;
	m_comboQueryField.InsertString(n++, _T("ID"));
	m_comboQueryField.InsertString(n++, _T("이름"));
	m_comboQueryField.InsertString(n++, _T("휴대폰"));
	m_comboQueryField.InsertString(n++, _T("이메일"));
	m_comboQueryField.SetCurSel(1);

	m_dateBegin.SetTimeFormat( CExtDateTimeWnd::h24 );
	m_dateBegin.SetMode( CExtDateTimeWnd::date );
	m_dateBegin.SetDateTime(COleDateTime::GetCurrentTime());
	m_dateBegin.m_lParamCookie = LPARAM(m_dateBegin.GetDlgCtrlID());

	m_dateEnd.SetTimeFormat( CExtDateTimeWnd::h24 );
	m_dateEnd.SetMode( CExtDateTimeWnd::date );
	m_dateEnd.SetDateTime(COleDateTime::GetCurrentTime());
	m_dateEnd.m_lParamCookie = LPARAM(m_dateEnd.GetDlgCtrlID());


	AddAnchor(IDC_GRID_CLIENT, __RDA_LT, __RDA_LB);
	AddAnchor(IDC_GRID_PROFIT, __RDA_LT, __RDA_RB);

	OnBnClickedButtonQueryClient();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

enum QueryType
{
	qt_client_info,
	qt_profit,
};

void CRecommenderMembersProfitDlg::InitClientGrid()
{
	m_gridClient.m_pOwner = this;
	m_gridClient.SetDefaultStyle(VTSGrid::sel_full_row);
		
	VTSGridColDefs defs;
	defs.push_back(make_pair(_T("아이디"),		0.4)); 
	defs.push_back(make_pair(_T("이름"),			0.3)); //1	
	defs.push_back(make_pair(_T("평가담보금"),	0.3)); //1	

	m_gridClient.SetColumDef(defs, false);
}

void CRecommenderMembersProfitDlg::InitGrid()
{
	m_gridProfit.SetDefaultStyle(VTSGrid::sel_full_row);

	m_gridProfit.OuterRowCountTopSet(3, false);

	VTSGridColDefs defs;
	defs.push_back(make_pair(_T("날짜"),				0.08)); 
	defs.push_back(make_pair(_T("선물"),			0.10));
	defs.push_back(make_pair(_T("옵션"),			0.10));
	defs.push_back(make_pair(_T("야간선물"),		0.10));
	defs.push_back(make_pair(_T("야간옵션"),		0.10));
	defs.push_back(make_pair(_T("해외선물"),		0.10));
	defs.push_back(make_pair(_T("합계"),			0.10)); //6
	defs.push_back(make_pair(_T("선물"),		0.10));
	defs.push_back(make_pair(_T("옵션"),		0.10));
	defs.push_back(make_pair(_T("야간선물"),	0.10));
	defs.push_back(make_pair(_T("야간옵션"),	0.10));
	defs.push_back(make_pair(_T("해외선물"),		0.10));
	defs.push_back(make_pair(_T("합계"),		0.10)); //12
	defs.push_back(make_pair(_T("실제손익"),			0.10));
	
	CRect rt;
	m_gridProfit.GetClientRect(rt);
	int w = rt.Width();
	int nLeft = 0;
	int nWidth = 0;
	for(UINT nCol = 0 ; nCol < defs.size() ; ++nCol)
	{
		nWidth = (int)(w * defs[nCol].second);

		CString strCategory;
		if(nCol == 0)
			strCategory = _T("날짜");
		else if(1 <= nCol && nCol <= 6)
			strCategory = _T("실현손익");
		else if(7 <= nCol && nCol <= 12)
			strCategory = _T("수수료");
		else if(13 == nCol)
			strCategory = _T("실제손익");

		m_gridProfit.ColumnRegister(defs[nCol].first, strCategory, nWidth);

		nLeft += nWidth;
	}

	//m_gridProfit.SetColumDef(defs, true, 1, RGB(253, 234, 218));

	for(UINT nCol = 0 ; nCol < defs.size() ; ++nCol)
	{
		CExtGridCellHeader * pHeader = m_gridProfit.QuickSetHeaderText(nCol, 2, nCol == 0 ? _T("합계") : _T(""));
		pHeader	-> ModifyStyle(nCol == 0  ? __EGCS_TA_HORZ_CENTER : __EGCS_TA_HORZ_RIGHT);
		pHeader	-> ModifyStyleEx(__EGCS_EX_DISABLE_SORT);

		if(0 <= nCol && nCol <= 6)
			m_gridProfit.QuickSetHeaderBackColor(nCol, 2, RGB(215, 228, 189));
		else if(7 <= nCol &&  nCol <= 12)
			m_gridProfit.QuickSetHeaderBackColor(nCol, 2, RGB(252, 213, 181));
		else if(13 == nCol)
			m_gridProfit.QuickSetHeaderBackColor(nCol, 2, RGB(255, 255, 153));
	}
}


void CRecommenderMembersProfitDlg::OnBnClickedButtonQueryClient()
{
	BeginWaitCursor();
	GetDlgItem(IDC_BUTTON_QUERY_CLIENT)->EnableWindow(FALSE);

	CString query;
	GetDlgItemText(IDC_EDIT_QUERY, query);

	int nQueryField = m_comboQueryField.GetCurSel();

	tClientCol col = cc_none;
	switch(nQueryField)
	{
	case 0: col = cc_id; break;
	case 1:	col = cc_name; break;
	case 2:	col = cc_mobile; break;
	case 3:	col = cc_email; break;
	}

	GetSession()->RequestClientInfoList(GetSafeHwnd(), nQueryField, query);
	BeginCheckTimer(qt_client_info);
}


void CRecommenderMembersProfitDlg::OnBnClickedButtonQuery()
{
	CRect rtSel = m_gridClient.SelectionGet(0, -1);
	if(rtSel.left == -1 || rtSel.top == -1)
		return;

	CString id = m_gridClient.QuickGetText(0, rtSel.top);

	BeginWaitCursor();
	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(FALSE);

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
	
	GetSession()->RequestClientEachdayProfit(GetSafeHwnd(), id, tmBegin, tmEnd);
	BeginCheckTimer(qt_profit);
}

void CRecommenderMembersProfitDlg::OnRequestReceived(DKBuffer& dbuf, LPVOID lpData)
{
	EndWaitCursor();
	if(qt_client_info == (UINT)lpData)
	{
		m_clients.clear();

		UINT nColCount, nRowCount;
		dbuf.RD(nColCount);
		ASSERT(nColCount == cc_base_count);
		dbuf.RD(nRowCount);

		m_gridClient.RowRemoveAll(false);
		m_gridClient.RowAdd(nRowCount);
		for(UINT nRow = 0 ; nRow < nRowCount ; ++nRow)
		{
			ClientBasePtr pClient(new ClientBase);
			if(!pClient->FromBuffer(dbuf))
				return;

			m_clients.push_back(pClient);

			int nCol = 0;
			m_gridClient.QuickSetText(nCol, nRow, pClient->id); ++nCol;
			m_gridClient.QuickSetText(nCol, nRow, pClient->name); ++nCol;
			m_gridClient.QuickSetText(nCol, nRow, ::ToThousand(pClient->get_bankBalance())); ++nCol;
		}

		m_gridClient.OnSwUpdateScrollBars();
		m_gridClient.OnSwInvalidate(false);

		GetDlgItem(IDC_BUTTON_QUERY_CLIENT)->EnableWindow(TRUE);
	}
	else if(qt_profit == (UINT)lpData)
	{
		UINT nColCount, nRowCount;
		dbuf.RD(nColCount);
		dbuf.RD(nRowCount);

		m_gridProfit.RowRemoveAll(false);
		m_gridProfit.RowAdd(nRowCount - 1);

		LONGLONG llVal;
		CString str;
		for(UINT nRow = 0 ; nRow < nRowCount - 1 ; ++nRow)
		{
			for(UINT nCol = 0 ; nCol < nColCount ; ++nCol)
			{
				if(0 == nCol)
				{
					DATE d;
					dbuf.RD(d);
					COleDateTime tm(d);

					str = tm.Format(FMT_DATE);
				}
				else
				{
					dbuf.RD(llVal);
					str = ::ToThousand(llVal);

					if(nCol == 6 || nCol == 13)
						m_gridProfit.QuickSetTextColor(nCol, nRow, ::GetProfitColor(llVal));
					else if(nCol == 12)
						m_gridProfit.QuickSetTextColor(nCol, nRow, Colors::Blue);
				}

				CExtGridCellString* pCell = m_gridProfit.QuickSetText(nCol, nRow, str);
				pCell->ModifyStyle(pCell->GetStyle() | __EGCS_TA_HORZ_RIGHT);
				if(nCol == 6)
					pCell->BackColorSet(CExtGridCell::__ECS_ALL, RGB(242, 246, 234));
				else if(nCol == 12)
					pCell->BackColorSet(CExtGridCell::__ECS_ALL, RGB(254, 246, 240));
				else if(nCol == 13)
					pCell->BackColorSet(CExtGridCell::__ECS_ALL, RGB(255, 255, 231));
			}
		}

		// 마지막 row에는 총합이 들어있다.
		for(UINT nCol = 0 ; nCol < nColCount ; ++nCol)
		{
			if(0 == nCol)
			{
				DATE d;
				dbuf.RD(d);
			}
			else
			{
				CExtGridCellHeader * pHeader = dynamic_cast<CExtGridCellHeader*>(
					m_gridProfit.GridCellGetOuterAtTop(nCol, 2, RUNTIME_CLASS(CExtGridCellHeader)));

				dbuf.RD(llVal);

				pHeader->TextSet(::ToThousand(llVal));

				if(nCol == 6 || nCol == 13)
					pHeader->TextColorSet(CExtGridCell::__ECS_ALL, ::GetProfitColor(llVal));
				else if(nCol == 12 && llVal > 0)
					pHeader->TextColorSet(CExtGridCell::__ECS_ALL, Colors::Blue);

				if(nCol == nColCount - 1)
				{
					//총손익만 하이라이트해준다.
					pHeader->FontWeightSet(FW_BOLD);
				}
			}
		}

		m_gridProfit.OnSwUpdateScrollBars();
		m_gridProfit.OnSwInvalidate(false);

		GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(TRUE);
	}

}


void CRecommenderMembersProfitDlg::OnRequestError(int nRequestID, const CString& strMsg)
{
	GetDlgItem(IDC_BUTTON_QUERY_CLIENT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(TRUE);
	if (qt_client_info == nRequestID)
	{
		ErrMsgBox(_T("조회요청이 실패했습니다."));
	}
	else if (qt_profit == nRequestID)
	{
		ErrMsgBox(_T("조회요청이 실패했습니다."));
	}
}

void CRecommenderMembersProfitDlg::OnGridSelChanged(int nCol, int nRow)
{
   OnBnClickedButtonQuery();
}

