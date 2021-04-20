#pragma once

#include <vector>

// DKTabCtrl

#define DKTABN_CHANGED		(NM_FIRST+1)

class DKTabCtrl : public CStatic
{
	DECLARE_DYNAMIC(DKTabCtrl)

public:
	DKTabCtrl();
	virtual ~DKTabCtrl();

	 void AddTab(const CString& strName);

	 void SetCurSel(int nIndex);
	 INT GetCurSel() { return m_nSelect; }

	 void SetSelectTabColor(COLORREF color);
private:
	BOOL		m_bSubclassing;
	CRect		m_rtClient;
	INT			m_nHeight;
	INT			m_nItemWidth;

	struct Item
	{
		CString		m_name;
	};
	
	std::vector<Item>	m_items;
	INT		m_nSelect;
	
	COLORREF	m_crLine, m_crLine2, m_crLine3;
	COLORREF	m_crLineSel, m_crLine2Sel, m_crLine3Sel;
	COLORREF	m_crText;
	COLORREF	m_crTab, m_crTabSel, m_crTabHover;

	CPen		m_penLine, m_penLine2, m_penLine3;
	CPen		m_penLineSel, m_penLine2Sel, m_penLine3Sel;
	
	CBrush		m_Brush;
	CFont		m_font;
	BOOL		m_bTrackMouse;
	INT			m_nMouseHover;
protected:
	void Init();
	
	enum State { _normal, _select, _hover };
	void DrawTab(CDC* pDC, int nIndex, State state);
public:
	void Draw(CDC* pDC);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};


