// DKMsgBox.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DKMsgBox.h"
#include "afxdialogex.h"


// DKMsgBox 대화 상자입니다.

IMPLEMENT_DYNAMIC(DKMsgBox, CDialog)

DKMsgBox::DKMsgBox(CWnd* pParent /*=NULL*/)
	: CDialog(DKMsgBox::IDD, pParent)
{

}

DKMsgBox::~DKMsgBox()
{
}

void DKMsgBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DKMsgBox, CDialog)
END_MESSAGE_MAP()


// DKMsgBox 메시지 처리기입니다.


BOOL DKMsgBox::ContinueModal()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialog::ContinueModal();
}
