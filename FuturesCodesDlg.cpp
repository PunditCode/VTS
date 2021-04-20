// FuturesCodesDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "FuturesCodesDlg.h"
#include "afxdialogex.h"



// CFuturesCodesDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CFuturesCodesDlg, CResizableDialogBase)

CFuturesCodesDlg::CFuturesCodesDlg(BOOL bNight, int x, int y, CWnd* pParent /*=NULL*/)
	: CResizableDialogBase(CFuturesCodesDlg::IDD, pParent)
{
	m_x = x;
	m_y = y;
	m_bNight = bNight;
}

CFuturesCodesDlg::~CFuturesCodesDlg()
{
}

void CFuturesCodesDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialogBase::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BUTTON_VOL_1, m_buttons[0]);
	DDX_Control(pDX, IDC_BUTTON_VOL_2, m_buttons[1]);
	DDX_Control(pDX, IDC_BUTTON_VOL_3, m_buttons[2]);
	DDX_Control(pDX, IDC_BUTTON_VOL_4, m_buttons[3]);

}


BEGIN_MESSAGE_MAP(CFuturesCodesDlg, CResizableDialogBase)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_VOL_1, IDC_BUTTON_VOL_4, &CFuturesCodesDlg::OnBnClickedButton)
END_MESSAGE_MAP()


// CFuturesCodesDlg �޽��� ó�����Դϴ�.


BOOL CFuturesCodesDlg::OnInitDialog()
{
	CResizableDialogBase::OnInitDialog();
	SetIcon( AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);
	
	CRect rt;
	GetClientRect(rt);

	for(int i = 0 ; i < 4 ; ++i)
	{
		m_buttons[i].EnableWindow(FALSE);
		m_buttons[i].SetWindowText(_T(""));
	}

	const Codes& codes = InstCM().GetFutures();
	int i = 0;
	for(auto pos = codes.begin() ; pos != codes.end() ; ++pos, ++i)
	{
		m_buttons[i].EnableWindow(TRUE);
		m_buttons[i].SetWindowText(pos->second->GetName());
		m_codes[i] = pos->second;
		if(i == 3)
			break;
	}
	
	SetWindowText(m_bNight ? _T("CME �߰�����") : _T("KOSPI����"));
	
	CPoint pt(m_x, m_y);
	ScreenToClient(&pt);

	SetWindowPos(NULL, pt.x, pt.y, rt.Width(), rt.Height(), SWP_NOSIZE);


	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CFuturesCodesDlg::OnBnClickedButton(UINT nID)
{
	int i = nID - IDC_BUTTON_VOL_1;
	m_pSelCode = m_codes[i];
	EndDialog(IDOK);
}