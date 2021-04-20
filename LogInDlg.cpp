
// MainDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "VTS.h"
#include "LogInDlg.h"
#include "RegisterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CFont g_font;
// CLogInDlg 대화 상자

CLogInDlg::CLogInDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogInDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLogInDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ID, m_editID);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_editPassword);
	DDX_Control(pDX, IDC_STATIC_LOG, m_staticLog);
	DDX_Control(pDX, IDC_CHECK_SAVE_ID, m_checkSaveID);

	DDX_Control(pDX, IDC_BUTTON_CONNECT, m_buttonConnect);
	DDX_Control(pDX, IDC_BUTTON_CANCEL, m_buttonCancel);
	DDX_Control(pDX, IDC_BUTTON_REGISTER, m_buttonRegister);
}

BEGIN_MESSAGE_MAP(CLogInDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CLogInDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CLogInDlg::OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_REGISTER, &CLogInDlg::OnBnClickedButtonRegister)
	ON_MESSAGE(WM_CONNECTION_ESTABLISHED, &CLogInDlg::OnConnectionEstablished)
	ON_EN_SETFOCUS(IDC_EDIT_ID, &CLogInDlg::OnEnSetfocusEditId)
	ON_EN_SETFOCUS(IDC_EDIT_PASSWORD, &CLogInDlg::OnEnSetfocusEditPassword)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_EDIT_ID, &CLogInDlg::OnEnChangeEditId)
END_MESSAGE_MAP()

void CLogInDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	m_imgBack.BitBlt(dc.GetSafeHdc(), 0, 0, m_rtClient.Width(), m_rtClient.Height(), 0, 0, SRCCOPY);
	//if (IsIconic())
	//{
	//	SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

	//	// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
	//	int cxIcon = GetSystemMetrics(SM_CXICON);
	//	int cyIcon = GetSystemMetrics(SM_CYICON);
	//	CRect rect;
	//	GetClientRect(&rect);
	//	int x = (rect.Width() - cxIcon + 1) / 2;
	//	int y = (rect.Height() - cyIcon + 1) / 2;

	//	// 아이콘을 그립니다.
	//	dc.DrawIcon(x, y, m_hIcon);
	//}
	//else
	//{
	//	CDialog::OnPaint();
	//}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CLogInDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CLogInDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.
		
	const CString& root = ENV().GetImgRoot();

	g_font.CreateFont(20, 0, 0, 0, 700, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("굴림"));

	CString strImgPath(root + _T("Login_WIZ.png"));
	HRESULT hr = m_imgBack.Load(strImgPath);
	GetClientRect(m_rtClient);

	if(SUCCEEDED(hr))
	{
		BITMAP bm;
		CBitmap* pBitmap = CBitmap::FromHandle((HBITMAP)m_imgBack);
		pBitmap->GetObject(sizeof(BITMAP), &bm);

		m_rtClient.right = bm.bmWidth;
		m_rtClient.bottom = bm.bmHeight;
		SetWindowPos(NULL, 0, 0, m_rtClient.Width(), m_rtClient.Height(), SWP_NOMOVE | SWP_NOZORDER);
		//MoveWindow(m_rtClient);
		m_editID.MoveWindow(CRect(144, 170, 277, 196), FALSE);
		m_editPassword.MoveWindow(CRect(144, 204, 277, 230), FALSE);
	}

	m_editID.SetFont(theApp.GetFontDefault());
	m_editPassword.SetFont(theApp.GetFontDefault());
	m_checkSaveID.SetFont(theApp.GetFontDefault());

	m_buttonConnect.SetWindowText(_T(""));
	m_buttonCancel.SetWindowText(_T(""));
	m_buttonRegister.SetWindowText(_T(""));


	m_buttonConnect.SetSkin(root + _T("BTN_WIZ_CONNECT_N.png"), root + _T("BTN_WIZ_CONNECT_N.png"), NULL, root + _T("BTN_WIZ_CONNECT_N.png"), NULL, Colors::Fuchsia, 0);
	m_buttonCancel.SetSkin(root + _T("BTN_WIZ_CANCEL_N.png"), root + _T("BTN_WIZ_CANCEL_N.png"), NULL, root + _T("BTN_WIZ_CANCEL_N.png"), NULL, Colors::Fuchsia, 0);
	m_buttonRegister.SetSkin(root + _T("BTN_WIZ_REGISTER_N.png"), root + _T("BTN_WIZ_REGISTER_N.png"), NULL, root + _T("BTN_WIZ_REGISTER_N.png"), NULL, Colors::Fuchsia, 0);

	m_buttonConnect.SetWindowPos(NULL, 60, 280, 0, 0, SWP_NOSIZE);
	m_buttonCancel.SetWindowPos(NULL, 578, 12, 0, 0, SWP_NOSIZE);
	m_buttonRegister.SetWindowPos(NULL, 266, 240, 0, 0, SWP_NOSIZE);
	m_checkSaveID.MoveWindow(70, 245, 13, 13);
	m_staticLog.SetWindowPos(NULL, 14, 7, 0, 0, SWP_NOSIZE);
	m_staticLog.SetTextColor(Colors::White);


	int nSaveID = AfxGetApp()->GetProfileInt(_T("LoginSettings"), _T("SaveIDCheck"), 1);
	CString id = AfxGetApp()->GetProfileString(_T("LoginSettings"), _T("ID"));
	((CButton*)GetDlgItem(IDC_CHECK_SAVE_ID))->SetCheck(nSaveID == 1 ? BST_CHECKED : BST_UNCHECKED);
	m_editID.SetWindowText(id);

	m_editID.EnableWindow(FALSE);
	m_editPassword.EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_REGISTER)->EnableWindow(FALSE);
	
	if (!GetSession()->ConnectToServer(ENV().GetPEMPath(), theApp.m_bConnectToSub ? theApp.m_ipSub : theApp.m_ipMain, theApp.m_nPort))
	{
		ErrMsgBox(_T("서버접속에 실패했습니다. 관리자에게 문의해주세요"));
		return FALSE;
	}

	m_staticLog.SetWindowText(_T("연결중입니다. 잠시만 기다려주세요..."));
	m_staticLog.UpdateCtrl();
		
	SetWindowText(CString(theApp.m_pszAppName) + _T("-로그인"));
	
	ModifyStyleEx(0, WS_EX_APPWINDOW);
	SetForegroundWindow();
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}


void CLogInDlg::OnTimer(UINT_PTR nIDEvent)
{
}

afx_msg LRESULT CLogInDlg::OnConnectionEstablished(WPARAM wParam, LPARAM lParam)
{
	m_staticLog.SetWindowText(_T("연결됐습니다. 로그인 해주세요."));
	m_staticLog.UpdateCtrl();

	m_editID.EnableWindow(TRUE);
	m_editPassword.EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_CANCEL)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_REGISTER)->EnableWindow(TRUE);
	
	CString id;
	m_editID.GetWindowText(id);
	if(id.IsEmpty())
		m_editID.SetFocus();
	else
		m_editPassword.SetFocus();

	return 0;
}


BOOL CLogInDlg::OnEraseBkgnd(CDC* pDC)
{
//	m_imgBack.BitBlt(pDC->GetSafeHdc(), 0, 0, m_rtClient.Width(), m_rtClient.Height(), 0, 0, SRCCOPY);
	return TRUE;
//	return CDialog::OnEraseBkgnd(pDC);
}


void CLogInDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.z
	m_ptDown = point;

	CDialog::OnLButtonDown(nFlags, point);
}


void CLogInDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	

	CDialog::OnLButtonUp(nFlags, point);
}


void CLogInDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(nFlags & MK_LBUTTON)
	{
		//현재 마우스의 위치를 Screen좌표로 환산해서
		CPoint ptScreen = point;
		ClientToScreen(&ptScreen);

		ptScreen.x -= m_ptDown.x;
		ptScreen.y -= m_ptDown.y;
		

		SetWindowPos(NULL, ptScreen.x, ptScreen.y, 0, 0, SWP_NOSIZE);
		//MoveWindow(rt, FALSE);
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CLogInDlg::OnBnClickedButtonConnect()
{
	CString id, password;
	m_editID.GetWindowText(id);
	m_editPassword.GetWindowText(password);

	if(id.IsEmpty())
	{
		ErrMsgBox(_T("아이디를 입력해주세요."));
		return;
	}

	if(password.IsEmpty())
	{
		ErrMsgBox(_T("패스워드를 입력해주세요."));
		return;
	}
	GetSession()->Login(0, id, password);


	int nCheck = m_checkSaveID.GetCheck();
	AfxGetApp()->WriteProfileInt(_T("LoginSettings"), _T("SaveIDCheck"), nCheck == BST_CHECKED ? 1 : 0);
	if(FALSE == nCheck)
		id = _T("");
	AfxGetApp()->WriteProfileString(_T("LoginSettings"), _T("ID"), id);
}


void CLogInDlg::OnBnClickedButtonCancel()
{
	PostQuitMessage(0);
}


void CLogInDlg::OnBnClickedButtonRegister()
{
	CRegisterDlg dlg;
	dlg.DoModal();
}

BOOL CLogInDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message == WM_KEYDOWN)
	{
		if ( pMsg->wParam == VK_RETURN )
		{
			OnBnClickedButtonConnect();
			return TRUE;
		}
		else if( pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CLogInDlg::OnEnSetfocusEditId()
{
	SetKeyboard(m_editID.GetSafeHwnd(), TRUE);
}


void CLogInDlg::OnEnSetfocusEditPassword()
{
	SetKeyboard(m_editID.GetSafeHwnd(), TRUE);
}




void CLogInDlg::OnEnChangeEditId()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
