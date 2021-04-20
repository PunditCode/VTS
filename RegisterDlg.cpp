// RegisterDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RegisterDlg.h"
#include "afxdialogex.h"
#include "VTS.h"

// CRegisterDlg 대화 상자입니다.
IMPLEMENT_DYNAMIC(CRegisterDlg, CExtResizableDialog)

CRegisterDlg::CRegisterDlg(CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(CRegisterDlg::IDD, pParent)
{
	SetAutoSubclassChildControls();
}

CRegisterDlg::~CRegisterDlg()
{
}

void CRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CExtResizableDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRegisterDlg, CExtResizableDialog)
	ON_BN_CLICKED(IDC_BUTTON_REGISTER, &CRegisterDlg::OnBnClickedButtonRegister)
	ON_EN_SETFOCUS(IDC_EDIT_ID, &CRegisterDlg::OnEnSetfocusEditId)
	ON_EN_SETFOCUS(IDC_EDIT_NAME, &CRegisterDlg::OnEnSetfocusEditName)
	ON_EN_SETFOCUS(IDC_EDIT_PASSWORD, &CRegisterDlg::OnEnSetfocusEditPassword)
	ON_EN_SETFOCUS(IDC_EDIT_PASSWORD2, &CRegisterDlg::OnEnSetfocusEditPassword2)
END_MESSAGE_MAP()


// CRegisterDlg 메시지 처리기입니다.


BOOL CRegisterDlg::OnInitDialog()
{
	CExtResizableDialog::OnInitDialog();
	SetIcon( AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_BANK);

	for(INT i = 0 ; i < BANK_COUNT ; ++i)
	{
		pCombo->InsertString(i, g_bank[i]);
	}
	pCombo->SetCurSel(0);

#ifdef _DEBUG
	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetDlgItemText(IDC_EDIT_ID, _T("tester1"));
	SetDlgItemText(IDC_EDIT_NAME, _T("테스터"));
	SetDlgItemText(IDC_EDIT_PASSWORD, _T("test24"));
	SetDlgItemText(IDC_EDIT_PASSWORD2, _T("test24"));
	SetDlgItemText(IDC_EDIT_MOBILE, _T("010-1234-5689"));
	SetDlgItemText(IDC_EDIT_EMAIL, _T("test5@gmail.com"));
	SetDlgItemText(IDC_EDIT_ACCOUNT, _T("124-567-8009"));
	SetDlgItemText(IDC_EDIT_BANK_OWNER, _T("테스터5"));
#endif

	SetDlgItemText(IDC_STATIC_RECOMMENDER, _T("추천인"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CRegisterDlg::OnBnClickedButtonRegister()
{
	CString password, password2;

	GetDlgItemText(IDC_EDIT_PASSWORD, password);
	GetDlgItemText(IDC_EDIT_PASSWORD2, password2);
	if(password != password2)
	{
		ErrMsgBox(_T("비밀번호와 비밀번호 확인값이 틀립니다."));
		return;
	}

	if(password.GetLength() < 4)
	{
		ErrMsgBox(_T("비밀번호는 4자 이상이어야 합니다"));
		return;
	}

	ClientBase client;
	client.password = password;

	GetDlgItemText(IDC_EDIT_ID, client.id);

	for(int i = 0 ; i < client.id.GetLength() ; ++i)
	{
		if(!__isascii(client.id[i]))
		{
			ErrMsgBox(_T("아이디에는 영문, 숫자, 특수기호만 가능합니다."));
			return;
		}
	}

	for(int i = 0 ; i < client.password.GetLength() ; ++i)
	{
		if(!__isascii(client.password[i]))
		{
			ErrMsgBox(_T("패스워드에는 영문, 숫자, 특수기호만 가능합니다."));
			return;
		}
	}
	GetDlgItemText(IDC_EDIT_NAME, client.name);
	if(client.name.IsEmpty())
	{
		ErrMsgBox(_T("이름을 입력해주세요"));
		return;
	}

	GetDlgItemText(IDC_EDIT_RECOMMENDER, client.recommender);
	GetDlgItemText(IDC_EDIT_EMAIL, client.email);
	GetDlgItemText(IDC_EDIT_MOBILE, client.mobile);
	GetDlgItemText(IDC_COMBO_BANK, client.bank);
	GetDlgItemText(IDC_EDIT_ACCOUNT, client.bankAccount);
	GetDlgItemText(IDC_EDIT_BANK_OWNER, client.bankOwner);
	
	theApp.GetSession()->RegisterNewClient(GetParent()->GetSafeHwnd(), &client);
	EndDialog(IDOK);
}


void CRegisterDlg::OnEnChangeEditPhone()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CExtResizableDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
void CRegisterDlg::OnEnSetfocusEditId()
{
	SetKeyboard(GetDlgItem(IDC_EDIT_ID)->GetSafeHwnd(), TRUE);
}

void CRegisterDlg::OnEnSetfocusEditName()
{
	SetKeyboard(GetDlgItem(IDC_EDIT_NAME)->GetSafeHwnd(), FALSE);
}

void CRegisterDlg::OnEnSetfocusEditPassword()
{
	SetKeyboard(GetDlgItem(IDC_EDIT_PASSWORD)->GetSafeHwnd(), TRUE);
}

void CRegisterDlg::OnEnSetfocusEditPassword2()
{
	SetKeyboard(GetDlgItem(IDC_EDIT_PASSWORD2)->GetSafeHwnd(), TRUE);
}
