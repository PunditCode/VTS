#pragma once

// DKSpinEditCtrl
#define DKTABCTRL_CLASSNAME    _T("DKSpinEditCtrl1.0")  // Window class name

#define DK_SPIN_CHANGED		(NM_FIRST+1)

class DKSpinEditCtrl : public CWnd
{
	DECLARE_DYNAMIC(DKSpinEditCtrl)

public:
	DKSpinEditCtrl();
	virtual ~DKSpinEditCtrl();

	 BOOL Create(const RECT& rect, CWnd* parent, UINT nID,
                DWORD dwStyle = WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE|WS_THICKFRAME);

	 void SetNumber(INT nNum);
	 INT GetNumber() { return m_nNumber; }
	 void SetEditReadOnly(BOOL bReadOnly)
	 {
		 m_bEditReadOnly = bReadOnly;
		 if(::IsWindow(m_edit.GetSafeHwnd()))
		 {
			 m_edit.SetReadOnly(bReadOnly);
		 }
	 }
private:
	INT			m_nNumber;
	BOOL		m_bSubclassing;
	BOOL		m_bEditReadOnly;
	CRect		m_rtClient;
	CRect		m_rtEdit, m_rtEditCtrl, m_rtUp, m_rtDown;

	COLORREF	m_crLine;
	COLORREF	m_crText;
	COLORREF	m_crArrow, m_crArrowBack;
	COLORREF	m_crEdit;

	CPen		m_penLine;
	CBrush		m_brushArrow, m_brushArrowBack, m_brushEdit;
	CBrush		m_brushArrowBackMouseMove, m_brushArrowBackClick;

	CBrush		m_Brush;
	CFont		m_font;


	POINT		m_ptUpArrow[3];
	POINT		m_ptDownArrow[3];

	CRichEditCtrl	m_edit;
	BOOL		m_bTrackMouse;

	enum MouseState
	{
		ms_none,
		ms_edit,
		ms_upArrow,
		ms_downArrow,
		ms_upArrowClick,
		ms_downArrowClick,
	};
	MouseState	m_mouseState;

protected:
	BOOL RegisterWindowClass();

	void Init();
	virtual void PreSubclassWindow();
	void SetRegion();
public:
	virtual void  OnDraw(CDC* pDC);
	void DrawEdit(CDC* pDC);
	void DrawUpArrow(CDC* pDC, MouseState mouseState);
	void DrawDownArrow(CDC* pDC, MouseState mouseState);

	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnEditChanged();
};


