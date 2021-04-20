// LogInWnd.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VTS.h"
#include "LogInWnd.h"

#define LOGINWND_CLASSNAME    _T("VTSLOGINWND")  // Window class name

// CLogInWnd

BOOL CLogInWnd::RegisterWindowClass()
{
    WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetResourceHandle();

    if (!(::GetClassInfo(hInst, LOGINWND_CLASSNAME, &wndcls)))
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
        wndcls.lpszClassName    = LOGINWND_CLASSNAME;

        if (!AfxRegisterClass(&wndcls))
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}


IMPLEMENT_DYNAMIC(CLogInWnd, CWnd)

CLogInWnd::CLogInWnd()
{
	 static BOOL bDummy = RegisterWindowClass();
}

CLogInWnd::~CLogInWnd()
{
}


BEGIN_MESSAGE_MAP(CLogInWnd, CWnd)
	ON_WM_CREATE()
    ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CLogInWnd 메시지 처리기입니다.

BOOL CLogInWnd::Create()
{
//	m_bSubclassing = FALSE;
	m_rtWnd.right = 600;
	m_rtWnd.bottom = 600;

	DWORD dwStyle = WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE|WS_THICKFRAME;

   // ASSERT(pParentWnd->GetSafeHwnd());
	if (!CWnd::Create(LOGINWND_CLASSNAME, NULL, dwStyle, m_rtWnd, GetDesktopWindow(), 0))
        return FALSE;
	Init();
    return TRUE;
}

int CLogInWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CWnd::OnCreate(lpCreateStruct);
	Init();
	return 1;
}

void CLogInWnd::PreSubclassWindow()
{
	CWnd::PreSubclassWindow();
    if (::IsWindow(m_hWnd))
	{
		Init();
	}
}

BOOL CLogInWnd::Init()
{
	
//	m_imgBack.LoadBMP_Resource(MAKEINTRESOURCE( IDB_BITMAP_LOGINDLG_MAIN ));

	CSize size = m_imgBack.GetSize();
	m_rtWnd.right = size.cx;
	m_rtWnd.bottom = size.cy;

	return TRUE;
}


BOOL CLogInWnd::OnEraseBkgnd(CDC* pDC) 
{
	// bitmap drawing을 하기 위해서 재정의 해야 한다.
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	memDC.SelectObject(&m_imgBack);

	pDC->BitBlt(m_rtWnd.left, m_rtWnd.top, m_rtWnd.Width(), m_rtWnd.Height(),
          &memDC, m_rtWnd.left, m_rtWnd.top, SRCCOPY);

	pDC->FillSolidRect(m_rtWnd, COLOR_WHITE);
	return TRUE;
}
void CLogInWnd::OnPaint()
{
    CPaintDC dc(this);      // device context for painting
	OnDraw(&dc);
}
void CLogInWnd::OnDraw(CDC* pDC)
{
	//CDC memDC;
	//memDC.CreateCompatibleDC(pDC);
	//memDC.SelectObject(m_pBitmap);

 //   pDC->BitBlt(m_rtWnd.left, m_rtWnd.top, m_rtWnd.Width(), m_rtWnd.Height(),
 //         &memDC, m_rtWnd.left, m_rtWnd.top, SRCCOPY);
}

