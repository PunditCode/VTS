#pragma once

// DKSkinButton
#define DKSKIN_BN_CLICKED		(NM_FIRST+1)
#define ON_DKSKIN_BN_CLICKED(id, memberFxn)		ON_CONTROL(DKSKIN_BN_CLICKED, id, memberFxn)

class DKSkinButton : public CStatic
{
	DECLARE_DYNAMIC(DKSkinButton)

public:
	DKSkinButton();
	virtual ~DKSkinButton();

	
	BOOL SetImage(LPCTSTR szNormal, LPCTSTR szHover, LPCTSTR szClick, LPCTSTR szDisable, bool bHasAlphaChannel, 
					CImage* pImageBG = NULL, COLORREF crTransparent = -1);

	void SetMenu(UINT nID) { m_nMenuID = nID; }
private:
	enum MouseState
	{
		ms_normal,
		ms_hover,
		ms_click,
	};


	MouseState	m_mouseState;
	CBrush		m_brush;
	BOOL		m_bTrackMouse;
	COLORREF	m_crTransparent;

	enum ButtonState
	{
		bs_normal,
		bs_hover,
		bs_click,
		bs_disable,
	///////////////////
		bs_count,
	};
	
	CImage*		m_pImageBG;
	CImage		m_image[bs_count];
	CBitmap		m_bitmap;
	UINT		m_nMenuID;

	CString		m_str;
protected:
	void Draw(CDC* pDC, const CRect& rtClient);
	void DrawBG(CDC *pDC, const CRect& rtClient);

	void UpdateCtrl();

	virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnPaint();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};


