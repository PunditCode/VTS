// NoticesDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VTS.h"
#include "NoticesDlg.h"
#include "afxdialogex.h"


// CNoticesDlg 대화 상자입니다.

IMPLEMENT_DYNCREATE(CNoticesDlg, VTSFrmWndChildResizableDlg)

CNoticesDlg::CNoticesDlg(CWnd* pParent /*=NULL*/)
	: VTSFrmWndChildResizableDlg(CNoticesDlg::IDD, pParent)
{
}

CNoticesDlg::~CNoticesDlg()
{
}

void CNoticesDlg::DoDataExchange(CDataExchange* pDX)
{
	VTSFrmWndChildResizableDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TITLE, m_wndLabelTitle);
	//DDX_Control(pDX, IDC_STATIC_BODY, m_wndLabelBody);
	DDX_Control(pDX, IDC_EDIT_HEADER, m_editTitle);
	DDX_Control(pDX, IDC_EDIT_BODY, m_editBody);
}


BEGIN_MESSAGE_MAP(CNoticesDlg, VTSFrmWndChildResizableDlg)
	ON_NOTIFY(UGN_SELCHANGED, IDC_GRID, OnGridEndSelChange)
	ON_MESSAGE(WM_NEW_NOTICE, &CNoticesDlg::OnNewNotice)
END_MESSAGE_MAP()


// CNoticesDlg 메시지 처리기입니다.


BOOL CNoticesDlg::OnInitDialog()
{
	VTSFrmWndChildResizableDlg::OnInitDialog();
	SetAutoSubclassChildControls(true);

	ShowSizeGrip(FALSE);
	
	InitGrid();
	UpdateGrid();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
void CNoticesDlg::InitGrid()
{
	m_grid.AttachGrid(this, IDC_GRID);

	AddAnchor(m_grid.GetSafeHwnd(),  __RDA_LT, __RDA_RT);
	AddAnchor(IDC_EDIT_HEADER, __RDA_LT, __RDA_RT);
	AddAnchor(IDC_EDIT_BODY, __RDA_LT, __RDA_RB);

	/*AddAnchor(IDC_GRID, __RDA_LT, __RDA_RT);
	AddAnchor(IDC_EDIT_HEADER, __RDA_LT, __RDA_RT);
	AddAnchor(IDC_EDIT_BODY, __RDA_LT, __RDA_RB);*/
	
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
	m_grid.SetNumberCols(2);
	
	m_grid.SetTH_NumberRows(1);
	m_grid.SetSH_NumberCols(1);
	m_grid.SetSH_ColWidth(-1, 0);

	m_grid.GetHeadingDefault(&cell);
	cell.SetBackColor(CR_GRID_HEADER_BG);
	m_grid.SetHeadingDefault(&cell);

	m_grid.QuickSetText(0, -1, _T("일자"));
	m_grid.QuickSetText(1, -1, _T("공지사항"));
	
	CRect rt;
	m_grid.GetClientRect(rt);
	int w = rt.Width() - m_grid.m_GI->m_vScrollWidth;
	int nLeft = 0;
	int nCol = 0;
	int nWidth = (int)(w * 0.15);
	m_grid.SetColWidth(nCol++, nWidth); nLeft += nWidth;
	nWidth = w - nLeft - 2;
	m_grid.SetColWidth(nCol++, nWidth);
}
void CNoticesDlg::UpdateGrid()
{
	Notices& notices =  GetSession()->m_notices;
	m_grid.SetNumberRows(notices.size());
	UINT i = 0;
	CUGCell cell;
	for(auto pos = notices.begin() ; pos != notices.end() ; ++pos, ++i)
	{
		NoticeItem* pItem = (*pos).get();
		
		m_grid.GetCell(0, i, &cell);
		cell.SetParam((long)pItem);
		cell.SetText(pItem->date);
		m_grid.SetCell(0, i, &cell);

		m_grid.QuickSetText(1, i, pItem->header);
	}
	if(!notices.empty())
		SelChange(0);
}
void CNoticesDlg::OnGridEndSelChange(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
    NM_UG* pItem = (NM_UG*) pNotifyStruct;
	pItem->col;
	pItem->row;
	if(pItem->row != -1)
		SelChange(pItem->row);
}

void CNoticesDlg::SelChange(int row)
{
	CUGCell cell;
	m_grid.GetCell(0, row, &cell);
	NoticeItem* pItem = (NoticeItem*)cell.GetParam();
	
	SetDlgItemText(IDC_EDIT_HEADER, pItem->header);
	SetDlgItemText(IDC_EDIT_BODY, pItem->body);
}


LRESULT CNoticesDlg::OnNewNotice(WPARAM wParam, LPARAM lParam)
{
	m_grid.EnableUpdate(FALSE);
	UpdateGrid();
	m_grid.EnableUpdate(TRUE);
	m_grid.RedrawAll();
	return 0;
}	
