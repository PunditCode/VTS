#pragma once

#include "resource.h"

// DKMsgBox ��ȭ �����Դϴ�.

class DKMsgBox : public CDialog
{
	DECLARE_DYNAMIC(DKMsgBox)

public:
	DKMsgBox(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~DKMsgBox();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_DKMSG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL ContinueModal();
};
