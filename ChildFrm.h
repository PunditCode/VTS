// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를 
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된 
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해 
// 추가적으로 제공되는 내용입니다.  
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.  
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// ChildFrm.h : CChildFrame 클래스의 인터페이스
//

#pragma once

#ifdef _MDI_

#include "VTSChildFrm.h"

typedef CExtNCW < CMDIChildWnd >	MDIChildWndBase;

class CStandaloneWnd;

class CChildFrame : public MDIChildWndBase
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();
	virtual ~CChildFrame();
	
	OVERRIDE void PreCreateView(int nViewID, int x, int y); 
	OVERRIDE CWnd* GetView(int nViewID);
	OVERRIDE void SetParam();
	
	void CreateView();
	void SetFromStandaloneWnd(CStandaloneWnd* pStdWnd);
	CWnd* GetChildWnd() { return m_p.pWnd; }
	const ChildViewParam& GetParam() { return m_p; }

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	ChildViewParam	m_p;

	int m_nCaptionHeight;
	int m_nFrameThick;

	static HWND m_TopWindow;
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnFileClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnToStandaloneWindow();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	virtual void PostNcDestroy();
	afx_msg LRESULT OnChildResizeRequest(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetTopMost(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
	LRESULT OnExtMenuPrepare(WPARAM wParam, LPARAM lParam);

	OVERRIDE bool NcFrameImpl_PreWindowProc( LRESULT & lResult, UINT message, WPARAM wParam, LPARAM lParam );
	OVERRIDE void NcFrameImpl_OnNcPaint(CDC & dcPaint, bool bOuterEmulationMode);
	
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
};

#endif //#ifdef _MDI_