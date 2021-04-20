// MsgDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MsgDlg.h"


// CMsgDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMsgDlg, CExtResizableDialog)

CMsgDlg::CMsgDlg(BOOL bLarageWidth, CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(CMsgDlg::IDD, pParent)
	, m_strTitle(_T(""))
	, m_strBody(_T(""))
	, m_bLargeWidth(bLarageWidth)
{
	SetAutoSubclassChildControls();
}

CMsgDlg::~CMsgDlg()
{
}

void CMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CExtResizableDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MSG_TITLE, m_strTitle);
	DDX_Text(pDX, IDC_EDIT_MSG_BODY, m_strBody);
	DDX_Control(pDX, IDC_EDIT_MSG_TITLE, m_editTitle);
	DDX_Control(pDX, IDC_EDIT_MSG_BODY, m_editBody);
}


BEGIN_MESSAGE_MAP(CMsgDlg, CExtResizableDialog)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CMsgDlg::OnInitDialog()
{
	CExtResizableDialog::OnInitDialog();

	SetIcon( AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

	AddAnchor(IDC_EDIT_MSG_TITLE, __RDA_KEEP, __RDA_X);
	AddAnchor(IDC_EDIT_MSG_BODY, __RDA_LT, __RDA_RB);
	AddAnchor(IDOK, __RDA_X);

	if(m_bLargeWidth)
	{
		CRect rt;
		GetWindowRect(rt);
		SetWindowPos(NULL, 0, 0, rt.Width() + 160, rt.Height() + 50, SWP_NOMOVE);
	}

	SetForegroundWindow();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

// CMsgDlg 메시지 처리기입니다.
void CMsgDlg::PostNcDestroy()
{
	CExtResizableDialog::PostNcDestroy();
	delete this;
}


void CMsgDlg::OnOK()
{
	CExtResizableDialog::OnOK();

	DestroyWindow();
}


void CMsgDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CExtResizableDialog::OnClose();

	DestroyWindow();
}


