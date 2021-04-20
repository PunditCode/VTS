// OvernightRequestDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VTS.h"
#include "OvernightRequestDlg.h"
#include "afxdialogex.h"
#include "VTSError.h"

// COvernightRequestDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(COvernightRequestDlg, CExtResizableDialog)

COvernightRequestDlg::COvernightRequestDlg(CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(COvernightRequestDlg::IDD, pParent)
{
	m_nRequest = GetSession()->enableOvernight == TRUE ? 0 : 1;

	SetAutoSubclassChildControls();
}

COvernightRequestDlg::~COvernightRequestDlg()
{
}

void COvernightRequestDlg::DoDataExchange(CDataExchange* pDX)
{
	CExtResizableDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, m_nRequest);
}


BEGIN_MESSAGE_MAP(COvernightRequestDlg, CExtResizableDialog)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &COvernightRequestDlg::OnBnClickedButtonApply)
	ON_MESSAGE(WM_MESSAGE_FROM_SERVER, &COvernightRequestDlg::OnMessageFromServer)
END_MESSAGE_MAP()


// COvernightRequestDlg 메시지 처리기입니다.


BOOL COvernightRequestDlg::OnInitDialog()
{
	CExtResizableDialog::OnInitDialog();

	SetDlgItemText(IDC_EDIT_F, ::ToThousand(GetSession()->m_nOvernightDepositFutures));
	SetDlgItemText(IDC_EDIT_FF, ::ToThousand(GetSession()->m_nOvernightDepositForeignFutures));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void COvernightRequestDlg::OnBnClickedButtonApply()
{
	UpdateData(TRUE);

	BOOL bEnable = m_nRequest == 0;

	GetSession()->RequestChangeEnableOvernight(GetSafeHwnd(), bEnable);
}

LRESULT COvernightRequestDlg::OnMessageFromServer(WPARAM wParam, LPARAM lParam)
{
	std::auto_ptr<MsgItem> pMsg((MsgItem*)wParam);
	MessageBox(pMsg->strBody, pMsg->strTitle, pMsg->nCode == VRES_SUCCESS ? MB_ICONINFORMATION : MB_ICONERROR);

	if (pMsg->nCode == VRES_SUCCESS)
		EndDialog(IDOK);

	return 0;
}
