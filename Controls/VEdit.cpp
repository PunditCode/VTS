// VEdit.cpp : implementation file
//

#include "stdafx.h"
#include "VEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define VE_TOP_MARGIN 4

/////////////////////////////////////////////////////////////////////////////
// CVEdit

CVEdit::CVEdit() 
	: m_rectNCBottom(0, 0, 0, 0)
	, m_rectNCTop(0, 0, 0, 0)
{
}

CVEdit::~CVEdit()
{
}


BEGIN_MESSAGE_MAP(CVEdit, CEdit)
	//{{AFX_MSG_MAP(CVEdit)
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT(EN_UPDATE, OnUpdate)
	ON_WM_LBUTTONDOWN()
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVEdit message handlers

void CVEdit::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	CRect rectWnd, rectClient;
	
	//calculate client area height needed for a font
	CFont *pFont = GetFont();
	CRect rectText;
	rectText.SetRectEmpty();
	
	CDC *pDC = GetDC();
	
	CFont *pOld = pDC->SelectObject(pFont);
	pDC->DrawText(_T("Ky"), rectText, DT_CALCRECT | DT_LEFT);
	UINT uiVClientHeight = rectText.Height();
	
	pDC->SelectObject(pOld);
	ReleaseDC(pDC);
	
	//calculate NC area to center text.
	
	GetClientRect(rectClient);
	GetWindowRect(rectWnd);

	ClientToScreen(rectClient);

	UINT uiCenterOffset = (rectClient.Height() - uiVClientHeight) / 2;
	UINT uiCY = (rectWnd.Height() - rectClient.Height()) / 2;
	UINT uiCX = (rectWnd.Width() - rectClient.Width()) / 2;

	rectWnd.OffsetRect(-rectWnd.left, -rectWnd.top);
	m_rectNCTop = rectWnd;

	m_rectNCTop.DeflateRect(uiCX, uiCY, uiCX, uiCenterOffset + uiVClientHeight + uiCY);

	m_rectNCBottom = rectWnd;

	m_rectNCBottom.DeflateRect(uiCX, uiCenterOffset + uiVClientHeight + uiCY, uiCX, uiCY);

	lpncsp->rgrc[0].top +=uiCenterOffset;
	lpncsp->rgrc[0].bottom -= uiCenterOffset;

	lpncsp->rgrc[0].left +=uiCX;
	lpncsp->rgrc[0].right -= uiCY;

}

void CVEdit::OnNcPaint() 
{
	//Default();
	//CWindowDC dc(this);
	//CBrush Brush(GetSysColor(COLOR_WINDOW));
	//dc.FillRect(m_rectNCBottom, &Brush);
	//dc.FillRect(m_rectNCTop, &Brush);
}

HBRUSH CVEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if(m_rectNCTop.IsRectEmpty())
	{
		SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
	}

	m_Brush.DeleteObject();
    m_Brush.CreateStockObject(HOLLOW_BRUSH);
    pDC->SetBkMode(TRANSPARENT);
    pDC->SetTextColor(RGB(0,0,0));
    return (HBRUSH)m_Brush;
}
void CVEdit::OnKillfocus() 
{
    UpdateCtrl();
}

void CVEdit::OnLButtonDown(UINT nFlags, CPoint point) 
{
    UpdateCtrl();
    
    CEdit::OnLButtonDown(nFlags, point);
}


void CVEdit::OnUpdate() 
{
    UpdateCtrl();
}

void CVEdit::UpdateCtrl()
{
    CWnd* pParent = GetParent();
    CRect rect;
    
    GetWindowRect(rect);
    pParent->ScreenToClient(rect);
    rect.DeflateRect(2, 2);
    pParent->InvalidateRect(rect, FALSE);    
}

