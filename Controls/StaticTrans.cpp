/*===========================================================================
====                                                                     ====
====    File name           :  StaticTrans.cpp                           ====
====    Creation date       :  7/10/2001                                 ====
====    Author(s)           :  Dany Cantin                               ====
====                                                                     ====
===========================================================================*/

#include "stdafx.h"
#include "StaticTrans.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStaticTrans

CStaticTrans::CStaticTrans()
{
    m_TextColor = RGB(0, 0, 0);
    m_BackColor = TRANS_BACK;    
}

CStaticTrans::~CStaticTrans()
{
}


BEGIN_MESSAGE_MAP(CStaticTrans, CStatic)
	//{{AFX_MSG_MAP(CStaticTrans)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticTrans message handlers

HBRUSH CStaticTrans::CtlColor(CDC* pDC, UINT nCtlColor) 
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

void CStaticTrans::UpdateCtrl()
{
    CWnd* pParent = GetParent();
    CRect rect;
    
    GetWindowRect(rect);
    pParent->ScreenToClient(rect);
    pParent->InvalidateRect(rect, FALSE);    
}

BOOL CStaticTrans::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message == WM_SETTEXT)
	{
		CStatic::PreTranslateMessage(pMsg);
		UpdateCtrl();
		return TRUE;
	}
	return CStatic::PreTranslateMessage(pMsg);
}
