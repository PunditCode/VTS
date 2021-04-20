// ChangePasswordDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ChangePasswordDlg.h"
#include "VTS.h"
#include "VTSError.h"

// CChangePasswordDlg 대화 상자입니다.

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


// CChangePasswordDlg 메시지 처리기입니다.


void CChangePasswordDlg::OnBnClickedButtonModify()
{
	CString strPrev, strNew, strConfirm;

	GetDlgItemText(IDC_EDIT_PREV, strPrev);
	GetDlgItemText(IDC_EDIT_NEW, strNew);
	GetDlgItemText(IDC_EDIT_CONFIRM, strConfirm);

	if(strNew.IsEmpty())
	{
		ErrMsgBox(_T("새비밀번호를 입력하세요."));
		return;
	}
	
	if(strNew != strConfirm)
	{
		ErrMsgBox(_T("새비밀번호와 새비밀번호확인값이 틀립니다."));
		return;
	}
	
	if(strNew.GetLength() < 4)
	{
		ErrMsgBox(_T("비밀번호는 4자 이상이어야 합니다"));
		return;
	}

	for(int i = 0 ; i < strNew.GetLength() ; ++i)
	{
		if(FALSE == __isascii(strNew[i]))
		{
			ErrMsgBox(_T("패스워드에는 영문, 숫자, 특수기호만 가능합니다."));
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
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
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
