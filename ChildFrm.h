// �� MFC ���� �ҽ� �ڵ�� MFC Microsoft Office Fluent ����� �������̽�("Fluent UI")�� 
// ����ϴ� ����� ���� �ָ�, MFC C++ ���̺귯�� ����Ʈ��� ���Ե� 
// Microsoft Foundation Classes Reference �� ���� ���� ������ ���� 
// �߰������� �����Ǵ� �����Դϴ�.  
// Fluent UI�� ����, ��� �Ǵ� �����ϴ� �� ���� ��� ����� ������ �����˴ϴ�.  
// Fluent UI ���̼��� ���α׷��� ���� �ڼ��� ������ 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// ChildFrm.h : CChildFrame Ŭ������ �������̽�
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