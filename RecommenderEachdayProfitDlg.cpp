// RecommenderEachdayProfitDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "VTS.h"
#include "RecommenderEachdayProfitDlg.h"
#include "afxdialogex.h"


// CRecommenderEachdayProfitDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNCREATE(CRecommenderEachdayProfitDlg, CRequestRecvDlg)

CRecommenderEachdayProfitDlg::CRecommenderEachdayProfitDlg(CWnd* pParent /*=NULL*/)
	: CRequestRecvDlg(CRecommenderEachdayProfitDlg::IDD, pParent)
{
}

CRecommenderEachdayProfitDlg::~CRecommenderEachdayProfitDlg()
{
}

void CRecommenderEachdayProfitDlg::DoDataExchange(CDataExchange* pDX)
{
	CRequestRecvDlg::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_DATEBEGIN, m_dateBegin);
	DDX_Control(pDX, IDC_DATEEND, m_dateEnd);
	DDX_Control(pDX, IDC_GRID, m_grid);

}


BEGIN_MESSAGE_MAP(CRecommenderEachdayProfitDlg, CRequestRecvDlg)
	ON_BN_CLICKED(IDC_BUTTON_QUERY, &CRecommenderEachdayProfitDlg::OnBnClickedButtonQuery)
END_MESSAGE_MAP()


// CRecommenderEachdayProfitDlg �޽��� ó�����Դϴ�.


BOOL CRecommenderEachdayProfitDlg::OnInitDialog()
{
	CRequestRecvDlg::OnInitDialog();

	InitGrid();
	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	AddAnchor(IDC_GRID, __RDA_LT, __RDA_RB);
	
	m_dateBegin.SetTimeFormat( CExtDateTimeWnd::h24 );
	m_dateBegin.SetMode( CExtDateTimeWnd::date );
	m_dateBegin.SetDateTime(COleDateTime::GetCurrentTime());
	m_dateBegin.m_lParamCookie = LPARAM(m_dateBegin.GetDlgCtrlID());

	m_dateEnd.SetTimeFormat( CExtDateTimeWnd::h24 );
	m_dateEnd.SetMode( CExtDateTimeWnd::date );
	m_dateEnd.SetDateTime(COleDateTime::GetCurrentTime());
	m_dateEnd.m_lParamCookie = LPARAM(m_dateEnd.GetDlgCtrlID());


	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CRecommenderEachdayProfitDlg::InitGrid()
{
	m_grid.SetDefaultStyle(VTSGrid::sel_full_row);

	m_grid.OuterRowCountTopSet(3, false);

	VTSGridColDefs defs;
	defs.push_back(make_pair(_T("��¥"),				0.08)); 
	defs.push_back(make_pair(_T("����"),			0.10));
	defs.push_back(make_pair(_T("�ɼ�"),			0.10));
	defs.push_back(make_pair(_T("�߰�����"),		0.10));
	defs.push_back(make_pair(_T("�߰��ɼ�"),		0.10));
	defs.push_back(make_pair(_T("�ؿܼ���"),		0.10));
	defs.push_back(make_pair(_T("�հ�"),			0.10)); //6
	defs.push_back(make_pair(_T("����"),		0.10));
	defs.push_back(make_pair(_T("�ɼ�"),		0.10));
	defs.push_back(make_pair(_T("�߰�����"),	0.10));
	defs.push_back(make_pair(_T("�߰��ɼ�"),	0.10));
	defs.push_back(make_pair(_T("�ؿܼ���"),		0.10));
	defs.push_back(make_pair(_T("�հ�"),		0.10)); //12
	defs.push_back(make_pair(_T("��������"),			0.10));
	
	CRect rt;
	m_grid.GetClientRect(rt);
	int w = rt.Width();
	int nLeft = 0;
	int nWidth = 0;
	for(UINT nCol = 0 ; nCol < defs.size() ; ++nCol)
	{
		nWidth = (int)(w * defs[nCol].second);

		CString strCategory;
		if(nCol == 0)
			strCategory = _T("��¥");
		else if(1 <= nCol && nCol <= 6)
			strCategory = _T("��������");
		else if(7 <= nCol && nCol <= 12)
			strCategory = _T("������");
		else if(13 == nCol)
			strCategory = _T("��������");

		m_grid.ColumnRegister(defs[nCol].first, strCategory, nWidth);

		nLeft += nWidth;
	}

	//m_grid.SetColumDef(defs, true, 1, RGB(253, 234, 218));

	for(UINT nCol = 0 ; nCol < defs.size() ; ++nCol)
	{
		CExtGridCellHeader * pHeader = m_grid.QuickSetHeaderText(nCol, 2, nCol == 0 ? _T("�հ�") : _T(""));
		pHeader	-> ModifyStyle(nCol == 0  ? __EGCS_TA_HORZ_CENTER : __EGCS_TA_HORZ_RIGHT);
		pHeader	-> ModifyStyleEx(__EGCS_EX_DISABLE_SORT);

		if(0 <= nCol && nCol <= 6)
			m_grid.QuickSetHeaderBackColor(nCol, 2, RGB(215, 228, 189));
		else if(7 <= nCol &&  nCol <= 12)
			m_grid.QuickSetHeaderBackColor(nCol, 2, RGB(252, 213, 181));
		else if(13 == nCol)
			m_grid.QuickSetHeaderBackColor(nCol, 2, RGB(255, 255, 153));
	}
}
//}
//
//void CRecommenderEachdayProfitDlg::OnDtnDatetimechangeDate(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
//	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
//	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(TRUE);
//	*pResult = 0;
//}


void CRecommenderEachdayProfitDlg::OnBnClickedButtonQuery()
{
	COleDateTime tmBegin = m_dateBegin.GetDateTime();
	COleDateTime tmEnd = m_dateEnd.GetDateTime();
	if (tmBegin > tmEnd)
		std::swap(tmBegin, tmEnd);
	COleDateTimeSpan tmSpan = tmEnd - tmBegin;
	if (tmSpan.GetDays() > MAX_QUERY_PERIOD_DAY)
	{
		ErrMsgBox(_T("��ȸ�Ⱓ�� %d�����Ϸ� �����Ͻñ� �ٶ��ϴ�."), MAX_QUERY_PERIOD_DAY);
		return;
	}
	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(FALSE);
	GetSession()->RequestRecommenderEachdayProfit(GetSafeHwnd(), tmBegin, tmEnd);
	BeginCheckTimer();
}

void CRecommenderEachdayProfitDlg::OnRequestReceived(DKBuffer& dbuf, LPVOID lpData)
{
	UINT nColCount, nRowCount;
	dbuf.RD(nColCount);
	dbuf.RD(nRowCount);

	m_grid.RowRemoveAll(false);
	m_grid.RowAdd(nRowCount - 1);

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
					m_grid.QuickSetTextColor(nCol, nRow, ::GetProfitColor(llVal));
				else if(nCol == 12)
					m_grid.QuickSetTextColor(nCol, nRow, Colors::Blue);
			}

			CExtGridCellString* pCell = m_grid.QuickSetText(nCol, nRow, str);
			pCell->ModifyStyle(pCell->GetStyle() | __EGCS_TA_HORZ_RIGHT);
			if(nCol == 6)
				pCell->BackColorSet(CExtGridCell::__ECS_ALL, RGB(242, 246, 234));
			else if(nCol == 12)
				pCell->BackColorSet(CExtGridCell::__ECS_ALL, RGB(254, 246, 240));
			else if(nCol == 13)
				pCell->BackColorSet(CExtGridCell::__ECS_ALL, RGB(255, 255, 231));
		}
	}

	// ������ row���� ������ ����ִ�.
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
				m_grid.GridCellGetOuterAtTop(nCol, 2, RUNTIME_CLASS(CExtGridCellHeader)));

			dbuf.RD(llVal);

			pHeader->TextSet(::ToThousand(llVal));

			if(nCol == 6 || nCol == 13)
				pHeader->TextColorSet(CExtGridCell::__ECS_ALL, ::GetProfitColor(llVal));
			else if(nCol == 12 && llVal > 0)
				pHeader->TextColorSet(CExtGridCell::__ECS_ALL, Colors::Blue);

			if(nCol == nColCount - 1)
			{
				//�Ѽ��͸� ���̶���Ʈ���ش�.
				pHeader->FontWeightSet(FW_BOLD);
			}
		}
	}

	m_grid.OnSwUpdateScrollBars();
	m_grid.OnSwInvalidate(false);
	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(TRUE);
}

void CRecommenderEachdayProfitDlg::OnRequestError(int nRequestID, const CString& strMsg)
{
	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(TRUE);
	if (NUL_IDX != nRequestID)
		ErrMsgBox(_T("�Ϻ����ͳ��� ��û�� �����߽��ϴ�."));
}
