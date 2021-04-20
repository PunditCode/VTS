#pragma once


// DKSampleCtrl
#define DKSampleCtrl_CLASSNAME    _T("DKSampleCtrl1.0")  // Window class name


class DKSampleCtrl : public CWnd
{
	DECLARE_DYNAMIC(DKSampleCtrl)

public:
	DKSampleCtrl();
	virtual ~DKSampleCtrl();

	 BOOL Create(const RECT& rect, CWnd* parent, UINT nID,
                DWORD dwStyle = WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE|WS_THICKFRAME);
private:
	BOOL		m_bSubclassing;
	CRect		m_rtClient;

	COLORREF	m_crLine;
	COLORREF	m_crText;
	COLORREF	m_crArrow, m_crArrowBack;

	CPen		m_penLine;
	CBrush		m_Brush;
	CFont		m_font;
protected:
	BOOL RegisterWindowClass();

	void Init();
	virtual void PreSubclassWindow();
public:
	virtual void  OnDraw(CDC* pDC);

	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


