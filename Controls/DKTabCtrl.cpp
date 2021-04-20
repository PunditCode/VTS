// DKTabCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "DKTab.h"
#include "DKTabCtrl.h"

static COLORREF Darker(COLORREF crBase, float fFactor)
{
	ASSERT (fFactor < 1.0f && fFactor > 0.0f);

	fFactor = min(fFactor, 1.0f);
	fFactor = max(fFactor, 0.0f);

	BYTE bRed, bBlue, bGreen;
	BYTE bRedShadow, bBlueShadow, bGreenShadow;

	bRed = GetRValue(crBase);
	bBlue = GetBValue(crBase);
	bGreen = GetGValue(crBase);

	bRedShadow = (BYTE)(bRed * fFactor);
	bBlueShadow = (BYTE)(bBlue * fFactor);
	bGreenShadow = (BYTE)(bGreen * fFactor);

	return RGB(bRedShadow, bGreenShadow, bBlueShadow);
}

static COLORREF Lighter(COLORREF crBase, float fFactor) 
{
	ASSERT (fFactor > 1.0f);

	fFactor = max(fFactor, 1.0f);

	BYTE bRed, bBlue, bGreen;
	BYTE bRedHilite, bBlueHilite, bGreenHilite;

	bRed = GetRValue(crBase);
	bBlue = GetBValue(crBase);
	bGreen = GetGValue(crBase);

	bRedHilite = (BYTE)min((int)(bRed * fFactor), 255);
	bBlueHilite = (BYTE)min((int)(bBlue * fFactor), 255);
	bGreenHilite = (BYTE)min((int)(bGreen * fFactor), 255);

	return RGB(bRedHilite, bGreenHilite, bBlueHilite);
}

// DKTabCtrl

IMPLEMENT_DYNAMIC(DKTabCtrl, CStatic)

DKTabCtrl::DKTabCtrl()
{
	m_bSubclassing = TRUE;
	m_nSelect = 0;

	m_crLine = RGB(45, 46, 54);
	m_crLine2 = RGB(106, 108, 107);
	m_crLine3 = RGB(137, 139, 149);

	
#if COMPANY == COMPANY_NC
	m_crTab = RGB(212, 208, 200);
	SetSelectTabColor(RGB(207, 32, 40));
#else
	m_crTab = RGB(115, 118, 129);
	SetSelectTabColor(RGB(207, 34, 40));
#endif
	m_crTabHover = RGB(200,200,200);//Colors::DarkSeaGreen;

/*
	m_crLineSel = RGB(81, 13, 16);
	m_crLine2Sel = RGB(145, 75, 86);
	m_crLine3Sel = RGB(231, 57, 74);
*/
	m_penLine.CreatePen(PS_SOLID, 1, m_crLine);
	m_penLine2.CreatePen(PS_SOLID, 1, m_crLine2);
	m_penLine3.CreatePen(PS_SOLID, 1, m_crLine3);
		
	m_crText = Colors::White;
	m_nHeight = 25;
	m_nItemWidth = 68;
	m_Brush.CreateStockObject(HOLLOW_BRUSH);

	m_font.CreateFont(12, 0, 0, 0, 400, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
	CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("굴림"));

	m_bTrackMouse = FALSE;
	m_nMouseHover = NUL_IDX;
}

void DKTabCtrl::SetSelectTabColor(COLORREF color)
{
	m_crTabSel = color;

	m_crLineSel = Darker(m_crTabSel, 0.4f);
	m_crLine2Sel = Darker(m_crTabSel, 0.7f);
	m_crLine3Sel = Lighter(m_crTabSel, 1.3f);

	m_penLineSel.DeleteObject();
	m_penLine2Sel.DeleteObject();
	m_penLine3Sel.DeleteObject();

	m_penLineSel.CreatePen(PS_SOLID, 1, m_crLineSel);
	m_penLine2Sel.CreatePen(PS_SOLID, 1, m_crLine2Sel);
	m_penLine3Sel.CreatePen(PS_SOLID, 1, m_crLine3Sel);
}

DKTabCtrl::~DKTabCtrl()
{
}


BEGIN_MESSAGE_MAP(DKTabCtrl, CStatic)
	ON_WM_CREATE()
    ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()



// DKTabCtrl 메시지 처리기입니다.

void DKTabCtrl::Init()
{
	GetClientRect(m_rtClient); 
	SetFont(&m_font, FALSE);
}

HBRUSH DKTabCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	Draw(pDC);
	return (HBRUSH)m_Brush;
}

void DKTabCtrl::Draw(CDC* pDC)
{
	pDC->SelectObject(&m_font);
	GetClientRect(m_rtClient); 
//	pDC->FillSolidRect(m_rtClient, RGB(0,255,0));

	for(UINT i = 0 ; i < m_items.size() ; ++i)
	{
		DrawTab(pDC, i, (i == m_nSelect) ? _select : _normal);
	}

	int nTop = 0;
	pDC->SelectObject(&m_penLine);
	pDC->MoveTo((m_nItemWidth - 1) * m_items.size(), nTop + m_nHeight);
	pDC->LineTo(m_rtClient.right, nTop + m_nHeight);
}

void DKTabCtrl::DrawTab(CDC* pDC, int nIndex, State state)
{
	int nLeft = nIndex * (m_nItemWidth - 1);
	int nTop = 0;

	pDC->SelectObject(_select == state ? &m_penLineSel : &m_penLine);

	//왼쪽 모서리
	pDC->SetPixel(nLeft + 1, nTop + 1, _select == state ? m_crLineSel : m_crLine);

	//top line
	pDC->MoveTo(nLeft + 2, nTop);
	pDC->LineTo(nLeft + m_nItemWidth - 2, nTop);

	//오른쪽 모서리
	pDC->SetPixel(nLeft + m_nItemWidth - 2, nTop + 1, _select == state ? m_crLineSel : m_crLine);

	//right line
	pDC->MoveTo(nLeft + m_nItemWidth - 1, nTop + 2);
	pDC->LineTo(nLeft + m_nItemWidth - 1, nTop + m_nHeight);

	//left line
	pDC->MoveTo(nLeft, nTop + 2);
	pDC->LineTo(nLeft, nTop + m_nHeight);
	//bottom line
	pDC->LineTo(nLeft + m_nItemWidth, nTop + m_nHeight);
	
	
	pDC->SelectObject(_select == state ? &m_penLine2Sel : &m_penLine2);

	//왼쪽 모서리
	pDC->SetPixel(nLeft + 2, nTop + 1, _select == state ? m_crLine2Sel : m_crLine2);

	//left line
	pDC->MoveTo(nLeft + 1, nTop + 2);
	pDC->LineTo(nLeft + 1, nTop + m_nHeight);

	pDC->SelectObject(_select == state ? &m_penLine3Sel : &m_penLine3);

	//top line
	pDC->MoveTo(nLeft + 3, nTop + 1);
	pDC->LineTo(nLeft + m_nItemWidth - 2, nTop + 1);

	CRect rt(nLeft + 2, nTop + 2, nLeft + m_nItemWidth - 1, nTop + m_nHeight);

	COLORREF crFrom = _select == state ? m_crTabSel : (_normal == state ? m_crTab : m_crTabHover);
	COLORREF crTo = Darker(crFrom, 0.5f);
	int nROrg = GetRValue(crFrom);
	int nGOrg = GetGValue(crFrom);
	int nBOrg = GetBValue(crFrom);
	int nRDiff = GetRValue(crTo) - nROrg;
	int nGDiff = GetGValue(crTo) - nGOrg;
	int nBDiff = GetBValue(crTo) - nBOrg;
		
	int nHeight = rt.Height();
	for (int nLine = rt.top ; nLine < rt.bottom; ++nLine)
	{
		int nRed = nROrg + (nLine * nRDiff) / nHeight;
		int nGreen = nGOrg + (nLine * nGDiff) / nHeight;
		int nBlue = nBOrg + (nLine * nBDiff) / nHeight;
		
		CPen pen;
		pen.CreatePen(PS_SOLID, 1, RGB(nRed, nGreen, nBlue));
		pDC->SelectObject(&pen);

		pDC->MoveTo(rt.left, nLine);
		pDC->LineTo(rt.right, nLine);
	}

//	pDC->FillSolidRect(rt, RGB(255, 0, 0));
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_crText);
	pDC->DrawText(m_items[nIndex].m_name, rt, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void DKTabCtrl::OnSize(UINT nType, int cx, int cy)
{  
	GetClientRect(m_rtClient);
}

void DKTabCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(point.y > m_nHeight)
		return;

	int x = point.x;

	int nIndex = x / m_nItemWidth;
	SetCurSel(nIndex);

	CWnd *pOwner = GetOwner();
	if (pOwner && IsWindow(pOwner->m_hWnd))
		pOwner->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(),DKTABN_CHANGED), (LPARAM)GetSafeHwnd());
}

void DKTabCtrl::AddTab(const CString& strName)
{
	Item item;
	item.m_name = strName;
	m_items.push_back(item);
}

void DKTabCtrl::SetCurSel(int nIndex)
{
	if(m_nSelect == nIndex || nIndex >= (int)m_items.size())
		return;

	CDC* pDC = GetDC();
	pDC->SelectObject(&m_font);

	DrawTab(pDC, m_nSelect, _normal);
	DrawTab(pDC, nIndex, _select);
	m_nSelect = nIndex;
}


void DKTabCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	TRACKMOUSEEVENT MouseEvent;
	::ZeroMemory( &MouseEvent, sizeof( MouseEvent ) );
	MouseEvent.cbSize       = sizeof( MouseEvent );
	MouseEvent.dwFlags      = TME_LEAVE;
	MouseEvent.hwndTrack    = m_hWnd;
	MouseEvent.dwHoverTime  = 0;
	m_bTrackMouse = ::_TrackMouseEvent( &MouseEvent );

	if(point.y <= m_nHeight)
	{
		int x = point.x;
		int nIndex = x / m_nItemWidth;

		if( nIndex < (int)m_items.size() && nIndex != m_nSelect && nIndex != m_nMouseHover)
		{
			CDC* pDC = GetDC();
			pDC->SelectObject(&m_font);

			if(NUL_IDX != m_nMouseHover && m_nSelect != m_nMouseHover)
			{
				DrawTab(pDC, m_nMouseHover, _normal);
			}
			DrawTab(pDC, nIndex, _hover);
			m_nMouseHover = nIndex;
		}
	}

	CStatic::OnMouseMove(nFlags, point);
}

void DKTabCtrl::OnMouseLeave()
{
	m_bTrackMouse = FALSE;
	if(NUL_IDX != m_nMouseHover && m_nSelect != m_nMouseHover)
	{
		CDC* pDC = GetDC();
		pDC->SelectObject(&m_font);

		DrawTab(pDC, m_nMouseHover, _normal);
		m_nMouseHover = NUL_IDX;
	}
	CStatic::OnMouseLeave();
}

void DKTabCtrl::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CStatic::OnShowWindow(bShow, nStatus);
	CDC* pDC = GetDC();
	Draw(pDC);
}
