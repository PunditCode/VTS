
// MainDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "VTS.h"
#include "LogInDlg.h"
#include "RegisterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CFont g_font;
// CLogInDlg ��ȭ ����

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

	//	// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
	//	int cxIcon = GetSystemMetrics(SM_CXICON);
	//	int cyIcon = GetSystemMetrics(SM_CYICON);
	//	CRect rect;
	//	GetClientRect(&rect);
	//	int x = (rect.Width() - cxIcon + 1) / 2;
	//	int y = (rect.Height() - cyIcon + 1) / 2;

	//	// �������� �׸��ϴ�.
	//	dc.DrawIcon(x, y, m_hIcon);
	//}
	//else
	//{
	//	CDialog::OnPaint();
	//}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CLogInDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CLogInDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.
		
	const CString& root = ENV().GetImgRoot();

	g_font.CreateFont(20, 0, 0, 0, 700, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("����"));

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
		ErrMsgBox(_T("�������ӿ� �����߽��ϴ�. �����ڿ��� �������ּ���"));
		return FALSE;
	}

	m_staticLog.SetWindowText(_T("�������Դϴ�. ��ø� ��ٷ��ּ���..."));
	m_staticLog.UpdateCtrl();
		
	SetWindowText(CString(theApp.m_pszAppName) + _T("-�α���"));
	
	ModifyStyleEx(0, WS_EX_APPWINDOW);
	SetForegroundWindow();
	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}


void CLogInDlg::OnTimer(UINT_PTR nIDEvent)
{
}

afx_msg LRESULT CLogInDlg::OnConnectionEstablished(WPARAM wParam, LPARAM lParam)
{
	m_staticLog.SetWindowText(_T("����ƽ��ϴ�. �α��� ���ּ���."));
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.z
	m_ptDown = point;

	CDialog::OnLButtonDown(nFlags, point);
}


void CLogInDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	

	CDialog::OnLButtonUp(nFlags, point);
}


void CLogInDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if(nFlags & MK_LBUTTON)
	{
		//���� ���콺�� ��ġ�� Screen��ǥ�� ȯ���ؼ�
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
		ErrMsgBox(_T("���̵� �Է����ּ���."));
		return;
	}

	if(password.IsEmpty())
	{
		ErrMsgBox(_T("�н����带 �Է����ּ���."));
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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
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
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CDialog::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}
