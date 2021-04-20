/*===========================================================================
====                                                                     ====
====    File name           :  EditTrans.cpp                             ====
====    Creation date       :  7/10/2001                                 ====
====    Author(s)           :  Dany Cantin                               ====
====                                                                     ====
===========================================================================*/

#include "stdafx.h"
#include "EditTrans.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditTrans

CEditTrans::CEditTrans()
{
    m_TextColor = RGB(0, 0, 0);
    m_BackColor = TRANS_BACK;
}

CEditTrans::~CEditTrans()
{
}


BEGIN_MESSAGE_MAP(CEditTrans, CEdit)
	//{{AFX_MSG_MAP(CEditTrans)
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT(EN_UPDATE, OnUpdate)
	ON_WM_LBUTTONDOWN()
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	//}}AFX_MSG_MAP
	ON_WM_NCPAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditTrans message handlers

HBRUSH CEditTrans::CtlColor(CDC* pDC, UINT nCtlColor) 
{
    m_Brush.DeleteObject();

    if (m_BackColor == TRANS_BACK) {
        m_Brush.CreateStockObject(HOLLOW_BRUSH);
        pDC->SetBkMode(TRANSPARENT);
    }
    else {
        m_Brush.CreateSolidBrush(m_BackColor);
        pDC->SetBkColor(m_BackColor);
    }

    pDC->SetTextColor(m_TextColor);

    return (HBRUSH)m_Brush;
}


void CEditTrans::OnKillfocus() 
{
    UpdateCtrl();
}


void CEditTrans::OnUpdate() 
{
    UpdateCtrl();
}


void CEditTrans::OnLButtonDown(UINT nFlags, CPoint point) 
{
    UpdateCtrl();
    
    CEdit::OnLButtonDown(nFlags, point);
}


void CEditTrans::UpdateCtrl()
{
	return;
    CWnd* pParent = GetParent();
    CRect rect;
    
    GetWindowRect(rect);
    pParent->ScreenToClient(rect);
    rect.DeflateRect(2, 2);
    
    pParent->InvalidateRect(rect, FALSE);    
}


void CEditTrans::OnNcPaint()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CEdit::OnNcPaint()을(를) 호출하지 마십시오.
}
