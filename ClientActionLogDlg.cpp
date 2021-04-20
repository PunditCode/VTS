// ClientActionLogDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#ifdef _CLIENT_ACTION
#include "VTS.h"
#include "ClientActionLogDlg.h"
#include "afxdialogex.h"


// CClientActionLogDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNCREATE(CClientActionLogDlg, CRequestRecvDlg)

CClientActionLogDlg::CClientActionLogDlg(CWnd* pParent /*=NULL*/)
	: CRequestRecvDlg(CClientActionLogDlg::IDD, pParent)
{
}

CClientActionLogDlg::~CClientActionLogDlg()
{
}

void CClientActionLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CRequestRecvDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATEBEGIN, m_dateBegin);
	DDX_Control(pDX, IDC_DATEEND, m_dateEnd);
	DDX_Control(pDX, IDC_STATIC1, m_label1);
	DDX_Control(pDX, IDC_STATIC2, m_label2);
}


BEGIN_MESSAGE_MAP(CClientActionLogDlg, CRequestRecvDlg)
	ON_BN_CLICKED(IDC_BUTTON_QUERY, &CClientActionLogDlg::OnBnClickedButtonQuery)
END_MESSAGE_MAP()


// CClientActionLogDlg �޽��� ó�����Դϴ�.


BOOL CClientActionLogDlg::OnInitDialog()
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

static GridColumnItem g_cols[] = 
{
{_T("�̿��Ͻ�"),		0.20},
{_T("����"),			0.70},
{_T("���"),			0.10},
};

const int COL_COUNT = sizeof(g_cols) / sizeof(GridColumnItem);

void CClientActionLogDlg::InitGrid()
{
	m_grid.AttachGrid(this, IDC_GRID);

	//////////////////////////
	// Default cell
	CUGCell cell;
	m_grid.GetGridDefault(&cell);
	cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
	m_grid.SetGridDefault(&cell);
	m_grid.m_GI->m_highlightRowFlag = TRUE; 
	m_grid.m_GI->m_currentCellMode = 2; 
	//////////////////////////
	// Col, Row
	m_grid.SetNumberCols(COL_COUNT);
	m_grid.SetNumberRows(0);
	m_grid.SetTH_NumberRows(1);
	m_grid.SetSH_NumberCols(1);
	m_grid.SetSH_ColWidth(-1, 0);

	m_grid.GetHeadingDefault(&cell);
	cell.SetBackColor(CR_GRID_HEADER_BG);
	m_grid.SetHeadingDefault(&cell);

	CRect rt;
	m_grid.GetClientRect(rt);
	int w = rt.Width();
	int nLeft = 0;
	int nWidth = 0;
	for(int nCol = 0 ; nCol < COL_COUNT ; ++nCol)
	{
		m_grid.QuickSetText(nCol, -1, g_cols[nCol].n);
			nWidth = (int)(w * g_cols[nCol].w);
		m_grid.SetColWidth(nCol, nWidth); 
		nLeft += nWidth;
	}

}

void CClientActionLogDlg::OnBnClickedButtonQuery()
{
	COleDateTime tmBegin = m_dateBegin.GetDateTime();
	COleDateTime tmEnd = m_dateEnd.GetDateTime();
	
	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(FALSE);

	GetSession()->RequestClientActionLog(GetSafeHwnd(), tmBegin.Format(_T("%Y%m%d")),  tmEnd.Format(_T("%Y%m%d")));
	BeginCheckTimer();
}

void CClientActionLogDlg::OnRequestReceived(DKBuffer& dbuf, LPVOID lpData)
{
	UINT nColCount, nRowCount;
	dbuf.RD(nColCount);
	dbuf.RD(nRowCount);

	m_grid.SetNumberRows(nRowCount);
	for(UINT nRow = 0 ; nRow < nRowCount ; ++nRow)
	{
		CString str;
		for(UINT nCol = 0 ; nCol < nColCount ; ++nCol)
		{
			dbuf.RD(str);
			m_grid.QuickSetText(nCol, nRow, str);
		}
	}

	m_grid.RedrawAll();
	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(TRUE);
}


void CClientActionLogDlg::OnRequestError(int nRequestID, const CString& strMsg)
{
	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(TRUE);
	if (NUL_IDX != nRequestID)
		ErrMsgBox(_T("���α׳��� ��û�� �����߽��ϴ�."));
}

#endif //#ifdef _CLIENT_ACTION
