// DKMsgBox.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DKMsgBox.h"
#include "afxdialogex.h"


// DKMsgBox ��ȭ �����Դϴ�.

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


// DKMsgBox �޽��� ó�����Դϴ�.


BOOL DKMsgBox::ContinueModal()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	return CDialog::ContinueModal();
}
