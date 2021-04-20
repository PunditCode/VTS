#pragma once

#if THEME == THEME_DEFAULT
	#include "VTSDefPopupWnd.h"
	typedef VTSDefPopupWnd VTSPopupBaseWnd;
#elif THEME == THEME_LINETRADE	
	#include "WizPopupWnd.h"
	typedef WizPopupWnd VTSPopupBaseWnd;
#else
	#include "VTSPopupWnd.h"
	typedef VTSPopupWnd VTSPopupBaseWnd;
#endif


// CPopupWnd �������Դϴ�.
class CPopupWnd : public VTSPopupBaseWnd
{
	DECLARE_DYNCREATE(CPopupWnd)

	CPopupWnd();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CPopupWnd();
public:
	BOOL SetChildWnd(int nWndID, int x = 0, int y = 0);
	int GetWndID() { return m_nWndID; }
private:
	int			m_nWndID;
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


