#pragma once

#include "resource.h"
#include "UltimateGrid/UGCtrl.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "VTSFrmWndChildBase.h"

class CNoticesDlg : public VTSFrmWndChildResizableDlg//CDialog
{
	DECLARE_DYNCREATE(CNoticesDlg)
public:
	CNoticesDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CNoticesDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_NOTICES };

	CUGCtrl	m_grid;

	void InitGrid();
	void UpdateGrid();
protected:
	void SelChange(int row);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	afx_msg void OnGridEndSelChange(NMHDR *pNotifyStruct, LRESULT* /*pResult*/);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnNewNotice(WPARAM wParam, LPARAM lParam);
	CExtLabel m_wndLabelTitle;
	CExtEdit m_editTitle;
	CExtEdit m_editBody;
};
