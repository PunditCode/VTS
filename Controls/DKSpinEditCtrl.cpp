// DKSpinEditCtrl.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "DKSpinEditCtrl.h"
#define DKSPINEDIT_ID			444555

BOOL DKSpinEditCtrl::RegisterWindowClass()
{
    WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetResourceHandle();

    if (!(::GetClassInfo(hInst, DKTABCTRL_CLASSNAME, &wndcls)))
    {
        // otherwise we need to register a new class
        wndcls.style            = CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = DKTABCTRL_CLASSNAME;

        if (!AfxRegisterClass(&wndcls))
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}

// DKSpinEditCtrl

IMPLEMENT_DYNAMIC(DKSpinEditCtrl, CWnd)

DKSpinEditCtrl::DKSpinEditCtrl()
{
	static BOOL bDummy = RegisterWindowClass();
	m_nNumber = 1;
	m_bSubclassing = TRUE;

	m_crLine = COLOR_CONTROL_LINE;
	m_crArrow = RGB(221, 221, 221);
	m_crArrowBack = RGB(111, 113, 124);
	m_crEdit = Colors::White;

	m_penLine.CreatePen(PS_SOLID, 1, m_crLine);
	
	m_brushArrow.CreateSolidBrush(m_crArrow);
	m_brushArrowBack.CreateSolidBrush(m_crArrowBack);
	m_brushEdit.CreateSolidBrush(m_crEdit);
	
	m_brushArrowBackMouseMove.CreateSolidBrush(RGB(200,200,200));
	m_brushArrowBackClick.CreateSolidBrush(RGB(20, 20, 20));

	m_crText = RGB(45, 46, 54);
	m_Brush.CreateStockObject(HOLLOW_BRUSH);
	m_mouseState = ms_none;
	m_bTrackMouse = FALSE;
	m_bEditReadOnly = FALSE;
}

DKSpinEditCtrl::~DKSpinEditCtrl()
{
}


BEGIN_MESSAGE_MAP(DKSpinEditCtrl, CWnd)
	ON_WM_CREATE()
    ON_WM_PAINT()
	ON_WM_ERASEBKGND()
    ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_EN_CHANGE(DKSPINEDIT_ID, &DKSpinEditCtrl::OnEditChanged)
END_MESSAGE_MAP()



// DKSpinEditCtrl 메시지 처리기입니다.

void DKSpinEditCtrl::Init()
{
	GetClientRect(m_rtClient);

	int nWidth = m_rtClient.Width();
	if(nWidth % 2 != 0)
		--nWidth;
	int nHeight = m_rtClient.Height();
	if(nHeight % 2 != 1)
		++nHeight;
	SetWindowPos(NULL, 0, 0, nWidth, nHeight, SWP_NOMOVE);


	m_font.CreateFont(12, 0, 0, 0, 400, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("굴림"));

	SetFont(&m_font, FALSE);

	SetRegion();

	if(FALSE == m_bEditReadOnly)
	{
		m_edit.Create(WS_CHILD|WS_VISIBLE | ES_NUMBER | ES_CENTER, m_rtEditCtrl, this, DKSPINEDIT_ID);
		m_edit.SetFont(&m_font, FALSE);
		m_edit.SetWindowText(::ToString(m_nNumber));
		m_edit.SetEventMask(m_edit.GetEventMask() | ENM_CHANGE);
	}
}

BOOL DKSpinEditCtrl::Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwStyle)
{
	m_bSubclassing = FALSE;
    ASSERT(pParentWnd->GetSafeHwnd());
	if (!CWnd::Create(DKTABCTRL_CLASSNAME, NULL, dwStyle, rect, pParentWnd, nID))
        return FALSE;
	Init();
    return TRUE;
}

void DKSpinEditCtrl::PreSubclassWindow()
{
	CWnd::PreSubclassWindow();
    if (::IsWindow(m_hWnd))
	{
		if(m_bSubclassing)
			Init();
	}
}

int DKSpinEditCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CWnd::OnCreate(lpCreateStruct);
	Init();

	
	return 1;
}

void DKSpinEditCtrl::OnPaint()
{
    CPaintDC dc(this);      // device context for painting
	OnDraw(&dc);
}

BOOL DKSpinEditCtrl::OnEraseBkgnd(CDC* pDC) 
{
	// bitmap drawing을 하기 위해서 재정의 해야 한다.
	//pDC->FillSolidRect(CRect(CHART_TOOLBAR_WIDTH, 0, m_rtWnd.right, CHART_TOOLBAR_HEIGHT), Colors::White);
	pDC->FillSolidRect(m_rtClient, COLOR_DEFAULT_BACK);
	return FALSE;
}


void DKSpinEditCtrl::OnDraw(CDC* pDC)
{
	DrawEdit(pDC);
	DrawUpArrow(pDC, ms_none);
	DrawDownArrow(pDC, ms_none);
}

void DKSpinEditCtrl::DrawEdit(CDC* pDC)
{
	pDC->SelectObject(&m_font);
	pDC->SelectObject(&m_penLine);
	pDC->SelectObject(&m_brushEdit);
	pDC->Rectangle(m_rtEdit);

	if(m_bEditReadOnly)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(Colors::DimGray);
		pDC->DrawText(::ToString(m_nNumber), m_rtEdit, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
}

void DKSpinEditCtrl::DrawUpArrow(CDC* pDC, MouseState mouseState)
{
	if(ms_upArrow == mouseState)
	{
		pDC->SelectObject(&m_brushArrowBackMouseMove);
	}
	else if(ms_upArrowClick == mouseState)
	{
		pDC->SelectObject(&m_brushArrowBackClick);
	}
	else
	{
		pDC->SelectObject(&m_brushArrowBack);
	}
	pDC->SelectObject(&m_penLine);
	pDC->Rectangle(m_rtUp);
	pDC->SelectStockObject(NULL_PEN);
	pDC->SelectObject(&m_brushArrow);
	pDC->Polygon(m_ptUpArrow, 3);
}

void DKSpinEditCtrl::DrawDownArrow(CDC* pDC, MouseState mouseState)
{
	if(ms_downArrow == mouseState)
	{
		pDC->SelectObject(&m_brushArrowBackMouseMove);
	}
	else if(ms_downArrowClick == mouseState)
	{
		pDC->SelectObject(&m_brushArrowBackClick);
	}
	else
	{
		pDC->SelectObject(&m_brushArrowBack);
	}

	pDC->SelectObject(&m_penLine);
	pDC->Rectangle(m_rtDown);
	pDC->SelectStockObject(NULL_PEN);
	pDC->SelectObject(&m_brushArrow);
	pDC->Polygon(m_ptDownArrow, 3);
}

void DKSpinEditCtrl::OnSize(UINT nType, int cx, int cy)
{  
	SetRegion();
	CWnd::OnSize(nType, cx, cy);
}

void DKSpinEditCtrl::SetRegion()
{
	GetClientRect(m_rtClient);
	
	m_rtEdit = m_rtClient;
	m_rtEdit.right = (INT)(m_rtClient.right * 0.69);

	m_rtUp.left = m_rtEdit.right - 1;
	m_rtUp.top = 0;
	m_rtUp.right = m_rtClient.right;
	m_rtUp.bottom = m_rtClient.Height() / 2 + 1;

	m_rtDown = m_rtUp;
	m_rtDown.top = m_rtUp.bottom - 1;
	m_rtDown.bottom = m_rtClient.bottom;

	int nTriCenter = m_rtUp.left + (INT)(m_rtUp.Width() / 2.);
	int nTriHeight = (INT)(m_rtUp.Height() / 3.);
	int nTriWidth = (INT)(m_rtUp.Width() * (5/12.));
	if(nTriWidth % 2 != 1)
		nTriWidth++;
		
	m_ptUpArrow[0].x = nTriCenter;
	m_ptUpArrow[0].y = (INT)((m_rtUp.Height() / 2.) - (nTriHeight / 2.)) - 1;

	m_ptUpArrow[1].x = (INT)((DOUBLE)m_ptUpArrow[0].x - (nTriWidth / 2.));
	m_ptUpArrow[1].y = m_ptUpArrow[0].y + nTriHeight + 1;

	m_ptUpArrow[2].x = m_ptUpArrow[1].x + nTriWidth + 1;
	m_ptUpArrow[2].y = m_ptUpArrow[1].y;


	m_ptDownArrow[0].x = nTriCenter;
	m_ptDownArrow[0].y = m_rtDown.bottom - m_ptUpArrow[0].y - 1;

	m_ptDownArrow[1].x = m_ptUpArrow[2].x;
	m_ptDownArrow[1].y = m_ptDownArrow[0].y - nTriHeight;

	m_ptDownArrow[2].x = m_ptUpArrow[1].x + 1;
	m_ptDownArrow[2].y = m_ptDownArrow[1].y;


	CDC* pDC = GetDC();
	CSize size = pDC->GetTextExtent(_T("0"));


	m_rtEditCtrl = m_rtEdit;
	int nGap = (m_rtEditCtrl.Height() - size.cy);
	++m_rtEditCtrl.left;
	--m_rtEditCtrl.right;
	m_rtEditCtrl.top += 3;
	--m_rtEditCtrl.bottom;
}

void DKSpinEditCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	TRACKMOUSEEVENT MouseEvent;
	::ZeroMemory( &MouseEvent, sizeof( MouseEvent ) );
	MouseEvent.cbSize       = sizeof( MouseEvent );
	MouseEvent.dwFlags      = TME_LEAVE;
	MouseEvent.hwndTrack    = m_hWnd;
	MouseEvent.dwHoverTime  = 0;
	m_bTrackMouse = ::_TrackMouseEvent( &MouseEvent );

	MouseState newMouseState = ms_none;
	if(m_rtUp.PtInRect(point))
	{
		newMouseState = ms_upArrow;
	}
	else if(m_rtDown.PtInRect(point))
	{
		newMouseState = ms_downArrow;
	}
	else 
	{
		newMouseState = ms_edit;
	}

	if(newMouseState != m_mouseState)
	{
		if(ms_upArrow == m_mouseState)
		{
			DrawUpArrow(GetDC(), ms_none);
		}

		if(ms_downArrow == m_mouseState)
		{
			m_mouseState = ms_downArrow;
			DrawDownArrow(GetDC(), ms_none);
		}
	}

	m_mouseState = newMouseState;
	
	if(ms_upArrow == m_mouseState)
	{
		DrawUpArrow(GetDC(), ms_upArrow);
	}
	else if(ms_downArrow == m_mouseState)
	{
		DrawDownArrow(GetDC(), ms_downArrow);
	}

	CWnd::OnMouseMove(nFlags, point);
}


void DKSpinEditCtrl::OnMouseLeave()
{
	m_bTrackMouse = FALSE;

	if(ms_upArrow == m_mouseState)
	{
		m_mouseState = ms_none;
		DrawUpArrow(GetDC(), ms_none);
	}
	else if(ms_downArrow == m_mouseState)
	{
		m_mouseState = ms_none;
		DrawDownArrow(GetDC(), ms_none);
	}
	
	CWnd::OnMouseLeave();
}


void DKSpinEditCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_rtUp.PtInRect(point))
	{
		INT nNumber = m_nNumber;
		if(FALSE == m_bEditReadOnly)
		{
			CString str;
			m_edit.GetWindowText(str);
			nNumber = ::ToINT(str);
		}
		++nNumber;

		DrawUpArrow(GetDC(), ms_upArrowClick);
		SetNumber(nNumber);
		CWnd *pOwner = GetOwner();
		if (pOwner && IsWindow(pOwner->m_hWnd))
		{
			pOwner->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(),DK_SPIN_CHANGED), (LPARAM)GetSafeHwnd());
		}
	}
	else if(m_rtDown.PtInRect(point))
	{
		INT nNumber = m_nNumber;
		if(FALSE == m_bEditReadOnly)
		{
			CString str;
			m_edit.GetWindowText(str);
			nNumber = ::ToINT(str);
		}
		if(nNumber == 1)
			return;

		--nNumber;
		
		DrawDownArrow(GetDC(), ms_downArrowClick);
		SetNumber(nNumber);
		CWnd *pOwner = GetOwner();
		if (pOwner && IsWindow(pOwner->m_hWnd))
		{
			pOwner->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(),DK_SPIN_CHANGED), (LPARAM)GetSafeHwnd());
		}
	}
	CWnd::OnLButtonDown(nFlags, point);
}


void DKSpinEditCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_rtUp.PtInRect(point))
	{
		DrawUpArrow(GetDC(), ms_none);
	}
	else if(m_rtDown.PtInRect(point))
	{
		DrawDownArrow(GetDC(), ms_none);
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void DKSpinEditCtrl::OnEditChanged()
{
	CString s;
	m_edit.GetWindowText(s);
	m_nNumber = ::ToINT(s);

	CWnd *pOwner = GetOwner();
	if (pOwner && IsWindow(pOwner->m_hWnd))
	{
		pOwner->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(),DK_SPIN_CHANGED), (LPARAM)GetSafeHwnd());
	}
}

void DKSpinEditCtrl::SetNumber(INT nNum)
{
	m_nNumber = nNum; 
	DrawEdit(GetDC());
	if(FALSE == m_bEditReadOnly)
		m_edit.SetWindowText(::ToString(m_nNumber));
}
