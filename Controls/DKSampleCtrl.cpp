// DKSampleCtrl.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "DKSampleCtrl.h"

BOOL DKSampleCtrl::RegisterWindowClass()
{
    WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetResourceHandle();

    if (!(::GetClassInfo(hInst, DKSampleCtrl_CLASSNAME, &wndcls)))
    {
        // otherwise we need to register a new class
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = DKSampleCtrl_CLASSNAME;

        if (!AfxRegisterClass(&wndcls))
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}

// DKSampleCtrl

IMPLEMENT_DYNAMIC(DKSampleCtrl, CWnd)

DKSampleCtrl::DKSampleCtrl()
{
	static BOOL bDummy = RegisterWindowClass();

	m_bSubclassing = TRUE;

	m_crLine = RGB(45, 46, 54);
	m_penLine.CreatePen(PS_SOLID, 1, m_crLine);
	m_crText = Colors::White;
	m_Brush.CreateStockObject(HOLLOW_BRUSH);
}

DKSampleCtrl::~DKSampleCtrl()
{
}


BEGIN_MESSAGE_MAP(DKSampleCtrl, CWnd)
	ON_WM_CREATE()
    ON_WM_PAINT()
	ON_WM_ERASEBKGND()
    ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// DKSampleCtrl 메시지 처리기입니다.

void DKSampleCtrl::Init()
{
	GetClientRect(m_rtClient); 

	m_font.CreateFont(12, 0, 0, 0, 400, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("굴림"));

	SetFont(&m_font, FALSE);
}

BOOL DKSampleCtrl::Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwStyle)
{
	m_bSubclassing = FALSE;
    ASSERT(pParentWnd->GetSafeHwnd());
	if (!CWnd::Create(DKSampleCtrl_CLASSNAME, NULL, dwStyle, rect, pParentWnd, nID))
        return FALSE;
	Init();
    return TRUE;
}

void DKSampleCtrl::PreSubclassWindow()
{
	CWnd::PreSubclassWindow();
    if (::IsWindow(m_hWnd))
	{
		if(m_bSubclassing)
			Init();
	}
}

int DKSampleCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CWnd::OnCreate(lpCreateStruct);
	Init();

	
	return 1;
}

void DKSampleCtrl::OnPaint()
{
    CPaintDC dc(this);      // device context for painting
	OnDraw(&dc);
}

BOOL DKSampleCtrl::OnEraseBkgnd(CDC* pDC) 
{
	// bitmap drawing을 하기 위해서 재정의 해야 한다.
	//pDC->FillSolidRect(CRect(CHART_TOOLBAR_WIDTH, 0, m_rtWnd.right, CHART_TOOLBAR_HEIGHT), Colors::White);
	pDC->FillSolidRect(m_rtClient, COLOR_DEFAULT_BACK);
	return FALSE;
}


void DKSampleCtrl::OnDraw(CDC* pDC)
{
	pDC->SelectObject(&m_font);
	pDC->SelectObject(&m_penLine);
}


void DKSampleCtrl::OnSize(UINT nType, int cx, int cy)
{  
	GetClientRect(m_rtClient);

	CWnd::OnSize(nType, cx, cy);
}

void DKSampleCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
}
