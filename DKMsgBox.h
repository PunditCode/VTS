#pragma once

#include "resource.h"

// DKMsgBox 대화 상자입니다.

class DKMsgBox : public CDialog
{
	DECLARE_DYNAMIC(DKMsgBox)

public:
	DKMsgBox(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~DKMsgBox();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_DKMSG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL ContinueModal();
};
