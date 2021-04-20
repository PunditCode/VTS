// ChangePasswordDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ChangePasswordDlg.h"
#include "VTS.h"
#include "VTSError.h"

// CChangePasswordDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CChangePasswordDlg, CExtResizableDialog)

CChangePasswordDlg::CChangePasswordDlg(CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(CChangePasswordDlg::IDD, pParent)
{
	SetAutoSubclassChildControls();
}

CChangePasswordDlg::~CChangePasswordDlg()
{
}

void CChangePasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CExtResizableDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CChangePasswordDlg, CExtResizableDialog)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, &CChangePasswordDlg::OnBnClickedButtonModify)
	ON_MESSAGE(WM_MESSAGE_FROM_SERVER, &CChangePasswordDlg::OnMessageFromServer)
	ON_EN_SETFOCUS(IDC_EDIT_PREV, &CChangePasswordDlg::OnEnSetfocusEditPrev)
	ON_EN_SETFOCUS(IDC_EDIT_NEW, &CChangePasswordDlg::OnEnSetfocusEditNew)
	ON_EN_SETFOCUS(IDC_EDIT_CONFIRM, &CChangePasswordDlg::OnEnSetfocusEditConfirm)
END_MESSAGE_MAP()


// CChangePasswordDlg �޽��� ó�����Դϴ�.


void CChangePasswordDlg::OnBnClickedButtonModify()
{
	CString strPrev, strNew, strConfirm;

	GetDlgItemText(IDC_EDIT_PREV, strPrev);
	GetDlgItemText(IDC_EDIT_NEW, strNew);
	GetDlgItemText(IDC_EDIT_CONFIRM, strConfirm);

	if(strNew.IsEmpty())
	{
		ErrMsgBox(_T("����й�ȣ�� �Է��ϼ���."));
		return;
	}
	
	if(strNew != strConfirm)
	{
		ErrMsgBox(_T("����й�ȣ�� ����й�ȣȮ�ΰ��� Ʋ���ϴ�."));
		return;
	}
	
	if(strNew.GetLength() < 4)
	{
		ErrMsgBox(_T("��й�ȣ�� 4�� �̻��̾�� �մϴ�"));
		return;
	}

	for(int i = 0 ; i < strNew.GetLength() ; ++i)
	{
		if(FALSE == __isascii(strNew[i]))
		{
			ErrMsgBox(_T("�н����忡�� ����, ����, Ư����ȣ�� �����մϴ�."));
			return;
		}
	}

	GetSession()->RequestChangePassword(GetSafeHwnd(), strPrev, strNew);
}


afx_msg LRESULT CChangePasswordDlg::OnMessageFromServer(WPARAM wParam, LPARAM lParam)
{
	std::auto_ptr<MsgItem> pMsg((MsgItem*)wParam);
	MessageBox(pMsg->strBody, pMsg->strTitle, pMsg->nCode == VRES_SUCCESS ? MB_ICONINFORMATION : MB_ICONERROR);

	if(pMsg->nCode == VRES_SUCCESS)
		EndDialog(IDOK);

	return 0;
}


BOOL CChangePasswordDlg::OnInitDialog()
{
	CExtResizableDialog::OnInitDialog();

	SetIcon( AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CChangePasswordDlg::OnEnSetfocusEditPrev()
{
	SetKeyboard(GetDlgItem(IDC_EDIT_PREV)->GetSafeHwnd(), TRUE);
}

void CChangePasswordDlg::OnEnSetfocusEditNew()
{
	SetKeyboard(GetDlgItem(IDC_EDIT_NEW)->GetSafeHwnd(), TRUE);
}

void CChangePasswordDlg::OnEnSetfocusEditConfirm()
{
	SetKeyboard(GetDlgItem(IDC_EDIT_CONFIRM)->GetSafeHwnd(), TRUE);
}
