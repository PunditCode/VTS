#if !defined(AFX_VEDIT_H__FC76C0E2_1328_4CE5_87EE_215424543C38__INCLUDED_)
#define AFX_VEDIT_H__FC76C0E2_1328_4CE5_87EE_215424543C38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVEdit window

class CVEdit : public CEdit
{
// Construction
public:
	CVEdit();

// Attributes
public:
protected:
	CRect m_rectNCBottom;
	CRect m_rectNCTop;
	CBrush   m_Brush;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVEdit)
	public:
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVEdit();
	void UpdateCtrl();
	// Generated message map functions
protected:
	//{{AFX_MSG(CVEdit)
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnUpdate();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKillfocus();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VEDIT_H__FC76C0E2_1328_4CE5_87EE_215424543C38__INCLUDED_)
