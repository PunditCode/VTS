// ForeignFuturesCodeDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ForeignFuturesCodeDlg.h"
#include "VTS.h"
#include "afxdialogex.h"
#include "FastTradeDlg.h"

Code* CForeignFuturesCodeDlg::s_pLastSelCode = NULL;

// CForeignFuturesCodeDlg 대화 상자입니다.


IMPLEMENT_DYNAMIC(CForeignFuturesCodeDlg, VTSModelessDlg<CDialog>)

CForeignFuturesCodeDlg::CForeignFuturesCodeDlg(CWnd* pParent /*=NULL*/)
	: VTSModelessDlg<CDialog>(CForeignFuturesCodeDlg::IDD, pParent)
{

}

CForeignFuturesCodeDlg::~CForeignFuturesCodeDlg()
{
}

void CForeignFuturesCodeDlg::DoDataExchange(CDataExchange* pDX)
{
	VTSModelessDlg<CDialog>::DoDataExchange(pDX);
}

// CForeignFuturesCodeDlg 메시지 처리기입니다.
BEGIN_MESSAGE_MAP(CForeignFuturesCodeDlg, VTSModelessDlg<CDialog>)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_FOREIGN_FUTURES_1, IDC_BUTTON_FOREIGN_FUTURES_R, &CForeignFuturesCodeDlg::OnBnClickedButton)
END_MESSAGE_MAP()

// CForeignFuturesCodeDlg 메시지 처리기입니다.


BOOL CForeignFuturesCodeDlg::OnInitDialog()
{
	VTSModelessDlg<CDialog>::OnInitDialog();
	SetIcon( AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

	const Codes& codes = InstCM().GetForeignFutures();

	int i = 0;
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON|BS_LEFT;

	int nMaxWidth = INT_MIN;

	CDC* pDC = GetDC();
	pDC->SelectObject(GetFont());
	for(auto pos = codes.rbegin() ; pos != codes.rend() ; ++pos, ++i)
	{
		CodeForeignFutures* pFFCode = dynamic_cast<CodeForeignFutures*>(pos->second);

		CString str;
		str.Format(_T(" %s - [ %s ] %s"), pFFCode->korName, pFFCode->GetCode(), pFFCode->GetName());

		CSize szCaption = pDC->GetTextExtent(str);
		if(szCaption.cx > nMaxWidth)
		{
			nMaxWidth = szCaption.cx;
		}

		boost::shared_ptr<CExtButton> pButton(new CExtButton);

		pButton->Create(str, dwStyle, CRect(0, 0, 0, 0), this, IDC_BUTTON_FOREIGN_FUTURES_1 + i);
		pButton->SetFont(theApp.GetFontDefault());
		m_codes.push_back(pos->second);
		m_buttons.push_back(pButton);
		if(i == FFT_COUNT)
			break;
	}
	nMaxWidth += 10;
	int nTop = 3;
	int nHeight = 23;
	int nGap = 5;
	for(auto pos = m_buttons.begin() ; pos != m_buttons.end() ; ++pos)
	{
		boost::shared_ptr<CExtButton> pButton = *pos;

		pButton->SetWindowPos(this, 3, nTop, nMaxWidth, nHeight, SWP_NOZORDER);
		nTop += nHeight + nGap;

#if THEME == THEME_DEFAULT || THEME == THEME_LINETRADE
		pButton->SetTextColorNormal(Colors::GreenYellow);
		pButton->SetTextColorHover(Colors::GreenYellow);
#endif
	}
		
	CRect rt;
	GetClientRect(rt);

	CRect rtWnd;
	GetWindowRect(rtWnd);

	int nNCHeight = rtWnd.Height() - rt.Height();
	
	CRect rectParent, rect;
	m_pOwner->GetParent()->GetWindowRect(rectParent);
	SetWindowPos(NULL, rectParent.right, rectParent.top, nMaxWidth + 10, nTop + nNCHeight, SWP_NOZORDER);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CForeignFuturesCodeDlg::OnBnClickedButton(UINT nID)
{
	int i = nID - IDC_BUTTON_FOREIGN_FUTURES_1;
	s_pLastSelCode = m_codes[i];
	m_pOwner->SendMessage(WM_OPTIONS_TABLE_SELECT_CODE_CHANGED, (WPARAM)s_pLastSelCode);
}