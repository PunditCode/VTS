// RegisterDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "RegisterDlg.h"
#include "afxdialogex.h"
#include "VTS.h"

// CRegisterDlg ��ȭ �����Դϴ�.
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


// CRegisterDlg �޽��� ó�����Դϴ�.


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
	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	SetDlgItemText(IDC_EDIT_ID, _T("tester1"));
	SetDlgItemText(IDC_EDIT_NAME, _T("�׽���"));
	SetDlgItemText(IDC_EDIT_PASSWORD, _T("test24"));
	SetDlgItemText(IDC_EDIT_PASSWORD2, _T("test24"));
	SetDlgItemText(IDC_EDIT_MOBILE, _T("010-1234-5689"));
	SetDlgItemText(IDC_EDIT_EMAIL, _T("test5@gmail.com"));
	SetDlgItemText(IDC_EDIT_ACCOUNT, _T("124-567-8009"));
	SetDlgItemText(IDC_EDIT_BANK_OWNER, _T("�׽���5"));
#endif

	SetDlgItemText(IDC_STATIC_RECOMMENDER, _T("��õ��"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CRegisterDlg::OnBnClickedButtonRegister()
{
	CString password, password2;

	GetDlgItemText(IDC_EDIT_PASSWORD, password);
	GetDlgItemText(IDC_EDIT_PASSWORD2, password2);
	if(password != password2)
	{
		ErrMsgBox(_T("��й�ȣ�� ��й�ȣ Ȯ�ΰ��� Ʋ���ϴ�."));
		return;
	}

	if(password.GetLength() < 4)
	{
		ErrMsgBox(_T("��й�ȣ�� 4�� �̻��̾�� �մϴ�"));
		return;
	}

	ClientBase client;
	client.password = password;

	GetDlgItemText(IDC_EDIT_ID, client.id);

	for(int i = 0 ; i < client.id.GetLength() ; ++i)
	{
		if(!__isascii(client.id[i]))
		{
			ErrMsgBox(_T("���̵𿡴� ����, ����, Ư����ȣ�� �����մϴ�."));
			return;
		}
	}

	for(int i = 0 ; i < client.password.GetLength() ; ++i)
	{
		if(!__isascii(client.password[i]))
		{
			ErrMsgBox(_T("�н����忡�� ����, ����, Ư����ȣ�� �����մϴ�."));
			return;
		}
	}
	GetDlgItemText(IDC_EDIT_NAME, client.name);
	if(client.name.IsEmpty())
	{
		ErrMsgBox(_T("�̸��� �Է����ּ���"));
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
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CExtResizableDialog::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
