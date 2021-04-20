// RecommenderMembersActionLogDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VTS.h"
#include "RecommenderMembersActionLogDlg.h"

void ClientOrderActionLogGrid::OnGbwSelectionChanged()
{
	CRect rtSel = SelectionGet(0, -1);
	if(rtSel.left != -1 && rtSel.top != -1)
		m_pOwner->OnGridSelChanged(rtSel.left, rtSel.top);
}


// CRecommenderMembersActionLogDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRecommenderMembersActionLogDlg, CRequestRecvDlg)

CRecommenderMembersActionLogDlg::CRecommenderMembersActionLogDlg(CWnd* pParent /*=NULL*/)
	: CRequestRecvDlg(CRecommenderMembersActionLogDlg::IDD, pParent)
{
}

CRecommenderMembersActionLogDlg::~CRecommenderMembersActionLogDlg()
{
}

void CRecommenderMembersActionLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CRequestRecvDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_QUERY_FIELD, m_comboQueryField);
	DDX_Control(pDX, IDC_DATE, m_date);
	DDX_Control(pDX, IDC_GRID, m_grid);
	DDX_Control(pDX, IDC_GRID_CLIENT, m_gridClient);
}


BEGIN_MESSAGE_MAP(CRecommenderMembersActionLogDlg, CRequestRecvDlg)
	ON_BN_CLICKED(IDC_BUTTON_QUERY, &CRecommenderMembersActionLogDlg::OnBnClickedButtonQuery)
END_MESSAGE_MAP()


// CRecommenderMembersActionLogDlg 메시지 처리기입니다.

BOOL CRecommenderMembersActionLogDlg::OnInitDialog()
{
	CRequestRecvDlg::OnInitDialog();

	InitClientGrid();
	InitGrid();

	int n = 0;
	m_comboQueryField.InsertString(n++, _T("ID"));
	m_comboQueryField.InsertString(n++, _T("이름"));
	m_comboQueryField.InsertString(n++, _T("휴대폰"));
	m_comboQueryField.InsertString(n++, _T("이메일"));
	m_comboQueryField.SetCurSel(1);

	m_date.SetTimeFormat( CExtDateTimeWnd::h24 );
	m_date.SetMode( CExtDateTimeWnd::date );
	m_date.SetDateTime(COleDateTime::GetCurrentTime());
	m_date.m_lParamCookie = LPARAM(m_date.GetDlgCtrlID());

	AddAnchor(IDC_GRID_CLIENT, __RDA_LT, __RDA_LB);
	AddAnchor(IDC_GRID, __RDA_LT, __RDA_RB);

	OnBnClickedButtonQuery();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


enum QueryType
{
	qt_client_info,
	qt_profit,
};

void CRecommenderMembersActionLogDlg::InitClientGrid()
{
	m_gridClient.m_pOwner = this;
	m_gridClient.SetDefaultStyle(VTSGrid::sel_full_row);
		
	VTSGridColDefs defs;
	defs.push_back(make_pair(_T("아이디"),		0.6)); 
	defs.push_back(make_pair(_T("이름"),			0.4)); //1	

	m_gridClient.SetColumDef(defs, false);
}

enum
{
	c_client = 1,
	c_system = 4,

	c_no = 5,
	c_fee = 10,
	c_profit,
	c_deposit,
	c_contract = c_deposit + 1,
	c_outstanding,
};

void CRecommenderMembersActionLogDlg::InitGrid()
{
	m_grid.SetDefaultStyle(VTSGrid::sel_full_row);
	m_grid.OuterRowCountTopSet(2, false);

	VTSGridColDefs defs;
	defs.push_back(make_pair(_T("시간"), 0.12));

	defs.push_back(make_pair(_T("고객"), 0.20)); //1
	defs.push_back(make_pair(_T("관리자"), 0.06));
	defs.push_back(make_pair(_T("거래소"), 0.07));
	defs.push_back(make_pair(_T("시스템"), 0.06));

	defs.push_back(make_pair(_T("주문#"), 0.05)); //5
	defs.push_back(make_pair(_T("종목"), 0.06));
	defs.push_back(make_pair(_T("가격"), 0.06));
	defs.push_back(make_pair(_T("구분"), 0.06));
	defs.push_back(make_pair(_T("수량"), 0.06));
	defs.push_back(make_pair(_T("수수료"), 0.06));
	defs.push_back(make_pair(_T("손익"), 0.06)); //11 
	defs.push_back(make_pair(_T("담보금"), 0.08)); //12
	defs.push_back(make_pair(_T("체결"), 0.10)); //13
	defs.push_back(make_pair(_T("미체결"), 0.10));

	CRect rt;
	m_grid.GetClientRect(rt);
	int w = rt.Width();
	int nLeft = 0;
	int nWidth = 0;
	for (UINT nCol = 0; nCol < defs.size(); ++nCol)
	{
		nWidth = (int)(w * defs[nCol].second);

		CString strCategory;

		if (c_client <= nCol && nCol <= c_system)
			strCategory = _T("행위자");
		else if (c_no <= nCol && nCol <= c_profit)
			strCategory = _T("주문내역");
		else if (c_contract == nCol || c_outstanding == nCol)
			strCategory = _T("포지션");
		else
			strCategory = defs[nCol].first;

		m_grid.ColumnRegister(defs[nCol].first, strCategory, nWidth);

		nLeft += nWidth;
	}
#if 0
	for (UINT nCol = 0; nCol < defs.size(); ++nCol)
	{
		m_grid.QuickSetHeaderBackColor(nCol, 0, RGB(253, 234, 218));
		m_grid.QuickSetHeaderBackColor(nCol, 1, RGB(253, 234, 218));
	}
#endif
}


void CRecommenderMembersActionLogDlg::OnBnClickedButtonQuery()
{
	BeginWaitCursor();
	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(FALSE);

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


void CRecommenderMembersActionLogDlg::QueryPeriod()
{
	CRect rtSel = m_gridClient.SelectionGet(0, -1);
	if(rtSel.left == -1 || rtSel.top == -1)
		return;

	BeginWaitCursor();
	CString id = m_gridClient.QuickGetText(0, rtSel.top);

	BeginWaitCursor();
	
	COleDateTime tm = m_date.GetDateTime();

	GetSession()->RequestRecommenderMembersActionLog(GetSafeHwnd(), id, 
					tm.Format(_T("%Y%m%d")));
	BeginCheckTimer(qt_profit);
}

void CRecommenderMembersActionLogDlg::OnRequestReceived(DKBuffer& dbuf, LPVOID lpData)
{
	EndWaitCursor();
	if(qt_client_info == (UINT)lpData)
	{
		m_clients.clear();

		UINT nColCount, nRowCount;
		dbuf.RD(nColCount);
		dbuf.RD(nRowCount);

		m_gridClient.RowRemoveAll(false);
		m_gridClient.RowAdd(nRowCount);

		for(UINT nRow = 0 ; nRow < nRowCount ; ++nRow)
		{
			ClientManagedPtr pClient(new ClientManaged);
			if(!pClient->FromBufferOnlyBase(dbuf))
				return;

			m_clients.push_back(pClient);

			int nCol = 0;
			m_gridClient.QuickSetText(nCol, nRow, pClient->id); ++nCol;
			m_gridClient.QuickSetText(nCol, nRow, pClient->name); ++nCol;
		}

		m_gridClient.OnSwUpdateScrollBars();
		m_gridClient.OnSwInvalidate(false);

		GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(TRUE);
	}
	else if(qt_profit == (UINT)lpData)
	{
		UINT nColCount, nRowCount;
		dbuf.RD(nColCount);
		dbuf.RD(nRowCount);

		m_grid.RowRemoveAll(false);
		m_grid.RowAdd(nRowCount);

		LONGLONG llVal;
		GT_CURRENCY nLastBalance = 0;
		CString strLastCont, strLastOutstd;

		for (UINT nRow = 0; nRow < nRowCount; ++nRow)
		{
			GT_CURRENCY nFee = 0;
			for (UINT nCol = 0; nCol < nColCount; ++nCol)
			{
				CString str;

				if (0 == nCol)
				{
					DATE d;
					dbuf.RD(d);
					COleDateTime tm(d);

					str = tm.Format(FMT_DATE_TIME);
				}
				else if (c_fee == nCol)
				{
					dbuf.RD(llVal);
					nFee = llVal;
					if (nFee != 0)
						str = ::ToThousand(llVal);
				}
				else if (c_profit == nCol)
				{
					dbuf.RD(llVal);
					if (llVal != 0 || nFee != 0)
					{
						str = ::ToThousand(llVal);
						m_grid.QuickSetTextColor(nCol, nRow, ::GetProfitColor(llVal));
					}
				}
				else if (c_deposit == nCol)
				{
					dbuf.RD(llVal);
					if (llVal != nLastBalance)
					{
						nLastBalance = llVal;
						str = ::ToThousand(llVal);
					}
				}
				else
				{
					dbuf.RD(str);

					if (c_contract == nCol)
					{
						if (str == strLastCont)
							str.Empty();
						else
							strLastCont = str;
					}
					else if (c_outstanding == nCol)
					{
						if (str == strLastOutstd)
							str.Empty();
						else
							strLastOutstd = str;
					}
				}

				CExtGridCellString* pCell = m_grid.QuickSetText(nCol, nRow, str);
				if (10 <= nCol && nCol <= 13)
					pCell->ModifyStyle(pCell->GetStyle() | __EGCS_TA_HORZ_RIGHT);
			}
		}

		m_grid.OnSwUpdateScrollBars();
		m_grid.OnSwInvalidate(false);
	}

}


void CRecommenderMembersActionLogDlg::OnRequestError(int nRequestID, const CString& strMsg)
{
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

void CRecommenderMembersActionLogDlg::OnGridSelChanged(int nCol, int nRow)
{
	QueryPeriod();
}

