// BankingIORequestDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VTS.h"
#include "BankingIORequestDlg.h"
#include "afxdialogex.h"
#include "ServerEventReceiver.h"


// CBankingIORequestDlg 대화 상자입니다.

IMPLEMENT_DYNCREATE(CBankingIORequestDlg, VTSFrmWndChildResizableDlg)

CBankingIORequestDlg::CBankingIORequestDlg(BankingIOType mode, CWnd* pParent /*=NULL*/)
	: VTSFrmWndChildResizableDlg(CBankingIORequestDlg::IDD, pParent), m_mode(mode)
{
	
}

CBankingIORequestDlg::~CBankingIORequestDlg()
{
	if(biot_withdraw == m_mode)
		SER()->UnregisterClientInfo(this);
}

void CBankingIORequestDlg::DoDataExchange(CDataExchange* pDX)
{
	VTSFrmWndChildResizableDlg::DoDataExchange(pDX);
	
	DDX_Control(pDX, IDC_TAB, m_tab);
	DDX_Control( pDX, IDC_EDIT_AMOUNT        , m_editAmount);
	DDX_Control( pDX, IDC_EDIT_NAME          , m_editName);
	DDX_Control( pDX, IDC_EDIT_AMOUNT_REQUEST, m_editAmountRequest);

	DDX_Control( pDX, IDC_EDIT_BANK          , m_editBank);
	DDX_Control( pDX, IDC_EDIT_BANK_ACCOUNT  , m_editBankAccount);
	DDX_Control( pDX, IDC_EDIT_BANK_OWNER    , m_editBankOwner);
	DDX_Control( pDX, IDC_EDIT_MEMO          , m_editMemo);

	DDX_Control( pDX, IDC_STATIC_GROUP_BANK        , m_groupBank);
	DDX_Control( pDX, IDC_STATIC_GROUP_MEMO        , m_groupMemo);

	DDX_Control( pDX, IDC_STATIC1         , m_labels[0]);
	DDX_Control( pDX, IDC_STATIC2         , m_labels[1]);
	DDX_Control( pDX, IDC_STATIC3         , m_labels[2]);
	DDX_Control( pDX, IDC_STATIC4         , m_labels[3]);
	DDX_Control( pDX, IDC_STATIC5         , m_labels[4]);

	DDX_Control( pDX, IDC_BUTTON_REQUEST        , m_buttonRequest);
	DDX_Control( pDX, IDCANCEL			       , m_buttonCancel);
	
}


BEGIN_MESSAGE_MAP(CBankingIORequestDlg, VTSFrmWndChildResizableDlg)
	ON_BN_CLICKED(IDC_BUTTON_REQUEST, &CBankingIORequestDlg::OnBnClickedButtonRequest)
	ON_BN_CLICKED(IDCANCEL, &CBankingIORequestDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_ALL, &CBankingIORequestDlg::OnBnClickedButtonAll)
	ON_MESSAGE(WM_CLIENT_INFO_UPDATED, &CBankingIORequestDlg::OnClientInfoUpdated)
	ON_WM_CLOSE()
	ON_WM_SETFOCUS()
	ON_CONTROL(DKTABN_CHANGED, IDC_TAB, &CBankingIORequestDlg::OnTabSelChange)
	ON_MESSAGE(WM_VTSFRM_SHOW_COMPLETED, &CBankingIORequestDlg::OnVTSFrmShowCompleted)
END_MESSAGE_MAP()


// CBankingIORequestDlg 메시지 처리기입니다.


BOOL CBankingIORequestDlg::OnInitDialog()
{
	VTSFrmWndChildResizableDlg::OnInitDialog();

	m_tab.AddTab(_T("입금"));
	m_tab.AddTab(_T("출금"));
	
	m_groupBank.SetStyle(CExtGroupBox::STYLE_CAPTION);
	//m_groupBank.SetBkColor(Colors::Yellow);
	m_groupMemo.SetStyle(CExtGroupBox::STYLE_CAPTION);
	m_editAmount.IntSet(0);

	
	
#if THEME == THEME_DEFAULT || THEME == THEME_LINETRADE
	m_buttonRequest.SetBkColor(Colors::Red);
	m_buttonRequest.SetTextColorNormal(COLOR_FONT_NORMAL);
	m_buttonCancel.SetTextColorNormal(COLOR_FONT_NORMAL);

	/*m_editBank.SetTextColor(Colors::White);
	m_editBankAccount.SetTextColor(Colors::White);
	m_editBankOwner.SetTextColor(Colors::White);*/
#endif
	m_tab.SetCurSel(m_mode == biot_withdraw ? 1 : 0);
	SetControls(m_mode);
	
	//VTSFrmWndChildResizableDlg::SetBkColor(Colors::Pink);
	return FALSE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
	
void CBankingIORequestDlg::OnTabSelChange()
{
	INT nSel = m_tab.GetCurSel();
	m_mode = nSel == 0 ? biot_deposit : biot_withdraw;
	SetControls(m_mode);
}

void CBankingIORequestDlg::SetControls(BankingIOType mode)
{
	if(biot_deposit == mode)
	{
		m_labels[0].SetWindowText(_T("신청금액"));
		m_labels[1].SetWindowText(_T("입금자명"));

		m_editAmount.SetTextColor(Colors::Black);
		m_editAmount.SetWindowText(_T(""));
		m_editAmount.SetReadOnly(FALSE);
		m_editAmount.SetLimitText(20);

		m_editName.SetWindowText(GetSession()->name);

		m_groupBank.SetWindowText(_T("입금계좌"));
		
		m_editBank.SetWindowText(GetSession()->m_vtssBank);
		m_editBankAccount.SetWindowText(GetSession()->m_vtssBankAccount);
		m_editBankOwner.SetWindowText(GetSession()->m_vtssBankOwner);
		
		m_editAmountRequest.ShowWindow(SW_HIDE);
		m_editName.ShowWindow(SW_SHOW);
		m_editMemo.ShowWindow(SW_HIDE);
		m_groupMemo.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_ALL)->ShowWindow(SW_HIDE);

		SER()->UnregisterClientInfo(this);

		m_editAmount.SetFocus();
	}
	else
	{
		m_labels[0].SetWindowText(_T("출금가능금액"));
		m_labels[1].SetWindowText(_T("출금신청금액"));
		
		m_editAmount.SetTextColor(Colors::Red);
		m_editAmount.SetWindowText(::ToString(GetSession()->get_bankBalance()));
		m_editAmount.SetReadOnly(TRUE);
						
		m_groupBank.SetWindowText(_T("출금계좌"));
				
		m_editBank.SetWindowText(GetSession()->bank);
		m_editBankAccount.SetWindowText(GetSession()->bankAccount);
		m_editBankOwner.SetWindowText(GetSession()->bankOwner);

		m_editAmountRequest.ShowWindow(SW_SHOW);
		m_editName.ShowWindow(SW_HIDE);

		m_editMemo.ShowWindow(SW_SHOW);
		m_groupMemo.ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_ALL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_NAME)->SetFocus();
		GotoDlgCtrl(GetDlgItem(IDC_EDIT_NAME));

		SER()->RegisterClientInfo(this);
	}
	OnVTSFrmShowCompleted(0, 0);
}

LRESULT CBankingIORequestDlg::OnVTSFrmShowCompleted(WPARAM wParam, LPARAM lParam)
{
	CRect rt;
	GetClientRect(rt);
		
	WINDOWPLACEMENT wp;
	m_groupMemo.GetWindowPlacement(&wp);

	if(biot_deposit == m_mode)
	{
		rt.bottom = wp.rcNormalPosition.top - 5;
	}
	else
	{
		rt.bottom = wp.rcNormalPosition.bottom + 5;
	}

	GetParent()->PostMessage(WM_VTSFRM_CHILD_RESIZE_REQUEST, (WPARAM)rt.Width(), (LPARAM)rt.Height());
	return 0;
}


void CBankingIORequestDlg::OnBnClickedButtonRequest()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if(biot_deposit == m_mode)
	{
		GT_CURRENCY amount = m_editAmount.IntGet();
		if(amount == 0)
		{
			MessageBox(_T("금액을 다시 입력해주세요"), _T("확인"),MB_ICONEXCLAMATION);
			return;
		}

		CString strName;
		m_editName.GetWindowText(strName);
		if(strName.IsEmpty())
		{
			MessageBox(_T("입금자명을 입력해 주세요"), _T("확인"), MB_ICONEXCLAMATION);
			return;
		}
	
		GetSession()->RequestDeposit(amount, strName);
	}
	else
	{
		GT_CURRENCY request = m_editAmountRequest.IntGet();
		if(request == 0)
		{
			MessageBox(_T("금액을 다시 입력해주세요"), _T("확인"), MB_ICONEXCLAMATION);
			return;
		}

		CString strMemo;
		m_editMemo.GetWindowText(strMemo);
		GetSession()->RequestWithdraw(request, strMemo);
	}
	GetParent()->DestroyWindow();
}

void CBankingIORequestDlg::OnBnClickedButtonAll()
{
	ASSERT(biot_withdraw == m_mode);

	if(((CButton*)GetDlgItem(IDC_BUTTON_ALL))->GetCheck() == BST_CHECKED)
	{
		CString str;
		m_editAmount.GetWindowText(str);
		m_editAmountRequest.SetWindowText(str);
	}
}


void CBankingIORequestDlg::OnOK()
{
	//Prevent Enter key
	//VTSFrmWndChildResizableDlg::OnOK();
}

LRESULT CBankingIORequestDlg::OnClientInfoUpdated(WPARAM wParam, LPARAM lParam)
{
	m_editAmount.SetWindowText(::ToString(GetSession()->get_bankBalance()));
	return 0;
}

void CBankingIORequestDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetParent()->DestroyWindow();
	//VTSFrmWndChildResizableDlg::OnCancel();
}

void CBankingIORequestDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(biot_withdraw == m_mode)
		SER()->UnregisterClientInfo(this);
	VTSFrmWndChildResizableDlg::OnClose();
}



void CBankingIORequestDlg::OnSetFocus(CWnd* pOldWnd)
{
	VTSFrmWndChildResizableDlg::OnSetFocus(pOldWnd);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	
	if(biot_deposit == m_mode)
	{
		GotoDlgCtrl(&m_editAmount);
	}
	else
	{
		GotoDlgCtrl(&m_editName);
	}

}
