// DKSkinButton.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DKSkinButton.h"
#include <DKcommon.h>

// DKSkinButton

IMPLEMENT_DYNAMIC(DKSkinButton, CStatic)

DKSkinButton::DKSkinButton()
{
	m_mouseState = ms_normal;
	m_bTrackMouse = FALSE;
	m_brush.CreateStockObject(HOLLOW_BRUSH);
	m_crTransparent = Colors::Fuchsia;
	m_pImageBG = NULL;
	m_nMenuID = 0;
}

DKSkinButton::~DKSkinButton()
{
}

BEGIN_MESSAGE_MAP(DKSkinButton, CStatic)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_SHOWWINDOW()
//	ON_WM_PAINT()
ON_WM_KILLFOCUS()
END_MESSAGE_MAP()



// DKSkinButton 메시지 처리기입니다.
void DKSkinButton::PreSubclassWindow()
{
	CStatic::PreSubclassWindow();

	ModifyStyle(0, SS_NOTIFY);
	ModifyStyleEx(0, WS_EX_TRANSPARENT);
}

BOOL DKSkinButton::SetImage(LPCTSTR szNormal, LPCTSTR szHover, LPCTSTR szClick, LPCTSTR szDisable, bool bHasAlphaChannel, 
							CImage* pImageBG, COLORREF crTransparent)
{
	if(NULL == szNormal)
		return FALSE;

	if(SUCCEEDED(m_image[bs_normal].Load(szNormal)))
	{
		m_image[bs_normal].SetHasAlphaChannel(bHasAlphaChannel);

		if(-1 != crTransparent)
			m_image[bs_normal].SetTransparentColor(crTransparent);
	}
	else
		return FALSE;
	
	if(szHover)
	{
		if(SUCCEEDED(m_image[bs_hover].Load(szHover)))
		{
			m_image[bs_hover].SetHasAlphaChannel(bHasAlphaChannel);

			if(-1 != crTransparent)
				m_image[bs_hover].SetTransparentColor(crTransparent);
		}
		else
			return FALSE;
	}
	
	if(szClick)
	{
		if(SUCCEEDED(m_image[bs_click].Load(szClick)))
		{
			m_image[bs_click].SetHasAlphaChannel(bHasAlphaChannel);
			if(-1 != crTransparent)
				m_image[bs_click].SetTransparentColor(crTransparent);
		}
		else
			return FALSE;
	}
	if(szDisable)
	{
		if(SUCCEEDED(m_image[bs_disable].Load(szDisable)))
		{
			m_image[bs_disable].SetHasAlphaChannel(bHasAlphaChannel);

			if(-1 != crTransparent)
				m_image[bs_disable].SetTransparentColor(crTransparent);
		}
		else
			return FALSE;
	}
	m_pImageBG = pImageBG;
	SetWindowPos(NULL, 0, 0, m_image[bs_normal].GetWidth(), m_image[bs_normal].GetHeight(), SWP_NOZORDER|SWP_NOMOVE);

	m_bitmap.CreateCompatibleBitmap(GetDC(), m_image[bs_normal].GetWidth(), m_image[bs_normal].GetHeight());

	m_str = szNormal;
	return TRUE;
}

HBRUSH DKSkinButton::CtlColor(CDC* pDC, UINT nCtlColor)
{
	/*CRect rt;
	GetClientRect(rt);
	Draw(pDC, rt);*/
	UpdateCtrl();
	return (HBRUSH)m_brush;
}

void DKSkinButton::UpdateCtrl()
{
	if(m_image[bs_hover].IsNull())
		return;

	CRect rt;
	GetClientRect(rt);

	CDC* pDC = GetDC();

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	memDC.SelectObject(&m_bitmap);
	
	Draw(&memDC, rt);
	pDC->BitBlt(0, 0, rt.Width(), rt.Height(), &memDC, 0, 0, SRCCOPY);
}

void DKSkinButton::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	CRect rect;
	GetClientRect(rect);
	Draw(&dc, rect);
}

void DKSkinButton::Draw(CDC* pDC, const CRect& rtClient)
{
//	TRACE(_T("0x%x draw\n"), this);
	DrawBG(pDC, rtClient);

	if(ms_hover == m_mouseState)
	{
		if(FALSE == m_image[bs_hover].IsNull())
		{
			m_image[bs_hover].Draw(pDC->GetSafeHdc(), rtClient);
			return;
		}
	}
	else if(ms_click == m_mouseState)
	{
		if(FALSE == m_image[bs_click].IsNull())
		{
			m_image[bs_click].Draw(pDC->GetSafeHdc(), rtClient);
			return;
		}
	}

	if(FALSE == m_image[bs_normal].IsNull())
		m_image[bs_normal].Draw(pDC->GetSafeHdc(), rtClient);
}

void DKSkinButton::DrawBG(CDC *pDC, const CRect& rtClient)
{
	if(NULL == m_pImageBG)
		return;

	CRect wndRect;
	GetWindowRect(&wndRect);
	GetParent()->ScreenToClient(&wndRect);

	//m_pImageBG->BitBlt(dc.GetSafeHdc(), wndRect, CPoint(wndRect.left, wndRect.top));
	m_pImageBG->BitBlt(pDC->GetSafeHdc(), rtClient, CPoint(wndRect.left, wndRect.top));
}

void DKSkinButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_mouseState = ms_click;
	UpdateCtrl();

	if(m_nMenuID > 0)
	{
		m_mouseState = ms_normal;
		UpdateCtrl();

		CMenu menu;
		menu.LoadMenu(m_nMenuID);
		CMenu* pMenu = menu.GetSubMenu(0);


		CRect rt;
		GetWindowRect(rt);
		
		pMenu->TrackPopupMenu(TPM_LEFTALIGN, rt.left, rt.bottom, GetParent());
	}

	CStatic::OnLButtonDown(nFlags, point);
}


void DKSkinButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_mouseState = ms_hover;
	UpdateCtrl();

	CWnd *pOwner = GetOwner();
	if (m_nMenuID == 0 && pOwner && IsWindow(pOwner->m_hWnd))
		pOwner->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(),DKSKIN_BN_CLICKED), (LPARAM)GetSafeHwnd());

//	CStatic::OnLButtonUp(nFlags, point);
}


void DKSkinButton::OnMouseMove(UINT nFlags, CPoint point)
{
	TRACKMOUSEEVENT MouseEvent;
	::ZeroMemory( &MouseEvent, sizeof( MouseEvent ) );
	MouseEvent.cbSize       = sizeof( MouseEvent );
	MouseEvent.dwFlags      = TME_LEAVE;
	MouseEvent.hwndTrack    = m_hWnd;
	MouseEvent.dwHoverTime  = 0;
	m_bTrackMouse = ::_TrackMouseEvent( &MouseEvent );

	if(nFlags & MK_LBUTTON)
	{

	}
	else
	{
		if(ms_hover != m_mouseState)
		{
			m_mouseState = ms_hover;
			UpdateCtrl();
		}
	}
	CStatic::OnMouseMove(nFlags, point);
}


void DKSkinButton::OnMouseLeave()
{
	m_bTrackMouse = FALSE;
	m_mouseState = ms_normal;
	UpdateCtrl();
	CStatic::OnMouseLeave();
}


BOOL DKSkinButton::PreTranslateMessage(MSG* pMsg)
{
	return CStatic::PreTranslateMessage(pMsg);
}


void DKSkinButton::OnKillFocus(CWnd* pNewWnd)
{
	CStatic::OnKillFocus(pNewWnd);

	m_bTrackMouse = FALSE;
	m_mouseState = ms_normal;
	UpdateCtrl();
}

void DKSkinButton::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CStatic::OnShowWindow(bShow, nStatus);
	UpdateCtrl();

}
