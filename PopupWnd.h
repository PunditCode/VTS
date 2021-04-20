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


// CPopupWnd 프레임입니다.
class CPopupWnd : public VTSPopupBaseWnd
{
	DECLARE_DYNCREATE(CPopupWnd)

	CPopupWnd();           // 동적 만들기에 사용되는 protected 생성자입니다.
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


