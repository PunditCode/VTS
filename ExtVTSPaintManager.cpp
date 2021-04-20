#include "StdAfx.h"
#include "ExtVTSPaintManager.h"
#include "MainFrm.h"
#include "resource.h"

#if 1
//IMPLEMENT_SERIAL( CExtVTSPaintManager,  CExtPaintManagerOffice2003NoThemes, VERSIONABLE_SCHEMA|2 );
IMPLEMENT_DYNCREATE( CExtVTSPaintManager,  CExtPaintManagerOffice2003NoThemes);

CExtVTSPaintManager::CExtVTSPaintManager()
{

#if THEME == THEME_NATIVE
	m_clrFillHint =  RGB(212, 208, 200);
	m_clrAccentHint =  RGB(100, 100, 100);
#elif THEME == THEME_DEFAULT || THEME == THEME_LINETRADE
	m_clrFillHint =  RGB(200,200,200);
	m_clrAccentHint =  RGB(40,40,40);
#else
	//m_clrFillHint =  RGB(200,200,200);
	//m_clrAccentHint =  RGB(40,40,40);
#endif
}

void CExtVTSPaintManager::InitTranslatedColors()
{
	CExtPaintManagerOffice2003NoThemes::InitTranslatedColors();


//	m_mapColorTranslate[CLR_TEXT_OUT]				=  InstallColor(Colors::White);
//	m_mapColorTranslate[CLR_TEXT_IN]				= InstallColor(Colors::White);
//	m_mapColorTranslate[CLR_TEXT_DISABLED]			= Colors::GrayTEXT;
//	m_mapColorTranslate[XPCLR_PUSHEDHOVERTEXT]		= InstallColor(Colors::White);;

	// Menu text colors
//	m_mapColorTranslate[CLR_MENUTEXT_OUT]			= InstallColor(Colors::White);
//	m_mapColorTranslate[CLR_MENUTEXT_IN]			= InstallColor(Colors::White);
//	m_mapColorTranslate[CLR_MENUTEXT_DISABLED]		= Colors::GrayTEXT;
/*
	m_mapColorTranslate[XPCLR_TEXT_FIELD_BORDER_NORMAL]		= COLOR_3DFACE;
	m_mapColorTranslate[XPCLR_TEXT_FIELD_BORDER_DISABLED]	= InstallColor( xpclr_Separator );*/

	//XPCLR_HILIGHT_BORDER : COLOR_3DSHADOW

#if THEME == THEME_NATIVE
#else
	m_mapColorTranslate[_2003CLR_GRADIENT_LIGHT] = InstallColor(COLOR_DEFAULT_BACK);
	m_mapColorTranslate[_2003CLR_GRADIENT_DARK] = InstallColor(COLOR_DEFAULT_BACK);
	m_colors[COLOR_3DFACE]      = COLOR_CONTROL_BACK;
	m_colors[COLOR_3DSHADOW]    = COLOR_CONTROL_LINE;
	m_mapColorTranslate[_2003CLR_EXPBTN_LIGHT]  = InstallColor(Colors::White);
	m_mapColorTranslate[_2003CLR_EXPBTN_DARK]  = InstallColor(Colors::White);
	m_mapColorTranslate[_2003CLR_EXPBTN_HOVER_LIGHT]  = InstallColor(Colors::White);
#endif
	//m_colors[COLOR_BTNTEXT] = Colors::White;
	
	/*_2003CLR_EXPBTN_LIGHT			
	_2003CLR_EXPBTN_DARK			
	_2003CLR_EXPBTN_HOVER_LIGHT		
	_2003CLR_EXPBTN_HOVER_DARK		
	_2003CLR_EXPBTN_PRESSED_LIGHT	
	_2003CLR_EXPBTN_PRESSED_DARK	*/

}

bool CExtVTSPaintManager::OnQueryThemeColors(COLORREF *pclrFillHint, COLORREF *pclrAccentHint)
{
	if( pclrFillHint != NULL )
		*pclrFillHint = m_clrFillHint;
	if( pclrAccentHint != NULL )
		*pclrAccentHint = m_clrAccentHint;
	return true;
}

COLORREF CExtVTSPaintManager::QueryObjectTextColor(
		CDC & dc,
		bool bEnabled,
		bool bFocused,
		bool bHovered,
		bool bPressed,
		CObject * pHelperSrc,
		LPARAM lParam)
{
	if(bEnabled)
	{
		return COLOR_FONT_NORMAL;
	}
	if(bFocused)
	{
		return COLOR_FONT_NORMAL;
	}
	if(bHovered)
	{
		return Colors::White;
	}

	return COLOR_FONT_NORMAL;
}
void CExtVTSPaintManager::SetThemeColors(COLORREF clrFillHint,COLORREF clrAccentHint)
{
	if(m_clrFillHint == clrFillHint && m_clrAccentHint == clrAccentHint)
		return;

	m_clrFillHint = clrFillHint;
	m_clrAccentHint = clrAccentHint;
	InitTranslatedColors();
	InitHelperBrushes();
#ifdef _MDI_
	CMainFrame* pMainFrame = (STATIC_DOWNCAST(CMainFrame,AfxGetMainWnd()));
	ASSERT( pMainFrame != NULL );
	ASSERT_VALID( pMainFrame );
	pMainFrame->RedrawWindow(
		NULL,
		NULL,
		RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE
		| RDW_FRAME | RDW_ALLCHILDREN
		);

	CExtControlBar::stat_RedrawFloatingFrames( pMainFrame );
	CExtControlBar::stat_RecalcBarMetrics( pMainFrame );
#endif
}



IMPLEMENT_DYNCREATE( CExtPaintManagerForManagerMode,  CExtPaintManagerOffice2003NoThemes);

CExtPaintManagerForManagerMode::CExtPaintManagerForManagerMode()
{
	m_clrFillHint =  RGB(200,200,200);
	m_clrAccentHint =  Colors::Blue;
}
bool CExtPaintManagerForManagerMode::OnQueryUseThemeColors() const
{
	return true;
}

CExtPaintManager::e_system_theme_t CExtPaintManagerForManagerMode::OnQuerySystemTheme() const
{
	return ThemeUnknown;
}

void CExtPaintManagerForManagerMode::InitTranslatedColors()
{
	CExtPaintManagerOffice2003NoThemes::InitTranslatedColors();
	m_mapColorTranslate[_2003CLR_GRADIENT_LIGHT] = InstallColor(COLOR_MANAGER_MODE_BACK);
	m_mapColorTranslate[_2003CLR_GRADIENT_DARK] = InstallColor(COLOR_MANAGER_MODE_BACK);
	m_colors[COLOR_3DFACE]      = COLOR_CONTROL_BACK;
}
bool CExtPaintManagerForManagerMode::OnQueryThemeColors(COLORREF *pclrFillHint, COLORREF *pclrAccentHint)
{
	if( pclrFillHint != NULL )
		*pclrFillHint = m_clrFillHint;
	if( pclrAccentHint != NULL )
		*pclrAccentHint = m_clrAccentHint;
	return true;
}

COLORREF CExtPaintManagerForManagerMode::QueryObjectTextColor(
		CDC & dc,
		bool bEnabled,
		bool bFocused,
		bool bHovered,
		bool bPressed,
		CObject * pHelperSrc,
		LPARAM lParam)
{
	if(bEnabled)
	{
		return COLOR_FONT_NORMAL;
	}
	if(bFocused)
	{
		return COLOR_FONT_NORMAL;
	}
	if(bHovered)
	{
		return Colors::White;
	}

	return COLOR_FONT_NORMAL;
}
void CExtPaintManagerForManagerMode::SetThemeColors(COLORREF clrFillHint,COLORREF clrAccentHint)
{
	if(m_clrFillHint == clrFillHint && m_clrAccentHint == clrAccentHint)
		return;

	m_clrFillHint = clrFillHint;
	m_clrAccentHint = clrAccentHint;
	InitTranslatedColors();
	InitHelperBrushes();
}

























#if 0
void CExtVTSPaintManager::NcFrame_Paint(
										CDC & dc,
										const CExtCmdIcon * pIcon,
										__EXT_MFC_SAFE_LPCTSTR strCaption,
										UINT nDrawTextAlignFlags,
										const RECT & rcFrame,
										const RECT & rcClient,
										const RECT & rcIcon,
										const RECT & rcText,
										const RECT & rcHelp,
										const RECT & rcMinimize,
										const RECT & rcMaximizeRestore,
										const RECT & rcClose,
										bool bFrameActive,
										bool bFrameEnabled,
										bool bFrameMaximized,
										CExtPaintManager::e_nc_button_state_t eStateButtonHelp,
										CExtPaintManager::e_nc_button_state_t eStateButtonMinimize,
										CExtPaintManager::e_nc_button_state_t eStateButtonMaximizeRestore,
										CExtPaintManager::e_nc_button_state_t eStateButtonClose,
										const CWnd * pWnd,
										LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( IsHighContrast() )
	{
		CExtPaintManagerOffice2003NoThemes::NcFrame_Paint(
			dc, pIcon, strCaption, nDrawTextAlignFlags, rcFrame, rcClient, rcIcon, rcText,
			rcHelp, rcMinimize, rcMaximizeRestore, rcClose, bFrameActive, bFrameEnabled, bFrameMaximized,
			eStateButtonHelp, eStateButtonMinimize, eStateButtonMaximizeRestore, eStateButtonClose,
			pWnd, lParam
			);
		return;
	}

	int nOldStretchBltMode = ::GetStretchBltMode( dc.m_hDC );
	::SetStretchBltMode( dc.m_hDC, ( g_PaintManager.m_bIsWinNT ) ? HALFTONE : COLORONCOLOR );
	const CExtBitmap & _bmpNcFrame = bFrameActive ? m_bmpNcFrameActive : m_bmpNcFrameInactive;
	const CRect & rcPadding = bFrameActive ? m_rcNcFrameBordersActive : m_rcNcFrameBordersInactive;
	//const CRect & rcBorders = bFrameActive ? m_rcNcFrameBordersActive : m_rcNcFrameBordersInactive;
	const INT nCaptionHeightSrc = bFrameActive ? m_nCaptionHeightActive : m_nCaptionHeightInactive;
	const INT nCaptionHeightDst = UiScalingDo( nCaptionHeightSrc, __EUIST_Y );
	CRect _rcFrame = rcFrame;
	CRect _rcText = rcText;
	bool bZoomed = pWnd->IsZoomed()? true : false;
	if( bZoomed && ( ! g_PaintManager.m_DWM.IsCompositionEnabled() ) )
	{
		_rcText.top += rcPadding.top;
		_rcText.bottom += rcPadding.top;
		_rcFrame.top -= rcPadding.top;
		_rcFrame.left -= rcPadding.left;
		_rcFrame.right += rcPadding.right;
	}
	//	if( bZoomed )
	//		_rcFrame.InflateRect( rcBorders.left, rcBorders.top, rcBorders.right, rcBorders.bottom );
	_bmpNcFrame.DrawNcFrame( dc.m_hDC, _rcFrame, rcPadding, nCaptionHeightDst, nCaptionHeightSrc );
	
	if(pIcon != NULL &&	(! pIcon->IsEmpty() ) && (!::IsRectEmpty( &rcIcon)) &&	dc.RectVisible( &rcIcon ))
	{
		CSize _sizeIconArea( rcIcon.right - rcIcon.left, rcIcon.bottom - rcIcon.top );
		CSize _sizeIconSurface = pIcon->GetSize();
		bool bDrawDefaultIconSize = true;
		if( _sizeIconSurface != _sizeIconArea )
		{
			CExtCmdIcon _icon;
			_icon = *pIcon;
			if( ! _icon.IsEmpty() )
			{
				_icon.Scale( _sizeIconArea );
				if( ! _icon.IsEmpty() )
				{
					bDrawDefaultIconSize = false;
					_icon.Paint( const_cast <CExtVTSPaintManager*> (this),
						        dc.m_hDC,rcIcon.left, rcIcon.top, _sizeIconArea.cx, _sizeIconArea.cy);
				} // if( ! _icon.IsEmpty() )
			} // if( ! _icon.IsEmpty() )
		} // if( _sizeIconSurface != _sizeIconArea )
		if( bDrawDefaultIconSize )
		{
			pIcon->Paint(const_cast<CExtVTSPaintManager*>(this), dc.m_hDC, rcIcon.left, rcIcon.top, 1, -1);
		}
	}

	COLORREF colorFrameText = bFrameActive ? m_clrNcFrameTextActive : m_clrNcFrameTextInactive;
	if( colorFrameText != COLORREF(-1L) )
	{
		int nLenCaption = int( ( strCaption == NULL ) ? 0 : _tcslen( strCaption ) );
		if(nLenCaption > 0 && (!::IsRectEmpty( &_rcText)) && dc.RectVisible( &rcText ))
		{
			UINT _nDrawTextAlignFlags = nDrawTextAlignFlags;
			if(		pWnd->GetSafeHwnd() != NULL
				&&	(pWnd->GetExStyle()&WS_EX_LAYOUT_RTL) != 0
				&&	(nDrawTextAlignFlags&DT_CENTER) == 0
				)
			{
				_nDrawTextAlignFlags &= ~(DT_LEFT|DT_CENTER|DT_RIGHT);
				if( (nDrawTextAlignFlags&DT_RIGHT) != 0 )
					_nDrawTextAlignFlags |= DT_LEFT;
				else
					_nDrawTextAlignFlags |= DT_RIGHT;
				_nDrawTextAlignFlags |= DT_RTLREADING;
			}

			COLORREF clrOldText = dc.SetTextColor( colorFrameText );
			CFont * pOldFont = dc.SelectObject( (CFont*)&m_FontCaption );
			int nOldBkMode = dc.SetBkMode( TRANSPARENT );
			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC,
				strCaption, nLenCaption,
				(LPRECT)&_rcText,
				_nDrawTextAlignFlags|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX, 0
				);
			dc.SetBkMode( nOldBkMode );
			dc.SelectObject( pOldFont );
			dc.SetTextColor( clrOldText );
		}
	} // if( colorFrameText != COLORREF(-1L) )

	static const RECT rcZeroPadding = { 0, 0, 0, 0 };
	if(( ! ::IsRectEmpty( &rcHelp ) ) && dc.RectVisible( &rcHelp ))
	{
		//LPCTSTR strGlyph = stat_NcFrameButton_NameFromState( eStateButtonHelp );
		//const CExtSkinGlyph & glyph = m_Skin[_T("Button")][_T("FrameNcButton")][_T("Help")][strFrameActive].Glyph(strGlyph);
		//glyph.Draw( dc.m_hDC, rcHelp );
		INT nIndexInArray = bFrameActive ? m_nNcBtnIdxNormalActive : m_nNcBtnIdxNormalInactive;
		if( eStateButtonHelp == __ENCBS_HOVER )
		{
			m_bmpNcButtonBkHover.AlphaBlendSkinParts( dc.m_hDC, rcHelp, m_rcNcButtonBkPadding, CExtBitmap::__EDM_STRETCH );
			nIndexInArray = m_nNcBtnIdxHover;
		}
		else if( eStateButtonHelp == __ENCBS_PRESSED )
		{
			m_bmpNcButtonBkPressed.AlphaBlendSkinParts( dc.m_hDC, rcHelp, m_rcNcButtonBkPadding, CExtBitmap::__EDM_STRETCH );
			nIndexInArray = m_nNcBtnIdxPressed;
		}
		else if( eStateButtonHelp == __ENCBS_DISABLED )
			nIndexInArray = m_nNcBtnIdxDisabled;
		CRect rcSrc( 0, 0, m_sizeNcButtonShapeInArr.cx, m_sizeNcButtonShapeInArr.cy );
		rcSrc.OffsetRect( 0, m_sizeNcButtonShapeInArr.cy * nIndexInArray );
		m_bmpArrNcButtonsHelp.AlphaBlendSkinParts(
			dc.m_hDC,
			rcHelp,
			rcSrc,
			rcZeroPadding,
			( g_PaintManager.m_nLPZ == 96 ) ? ( CExtBitmap::__EDM_CENTER ) : ( CExtBitmap::__EDM_STRETCH ),
			true,
			true
			);
	}
	if((!::IsRectEmpty( &rcMinimize ))&& dc.RectVisible( &rcMinimize ))
	{
		//LPCTSTR strGlyph = stat_NcFrameButton_NameFromState( eStateButtonMinimize );
		//const CExtSkinGlyph & glyph = m_Skin[_T("Button")][_T("FrameNcButton")][_T("Minimize")][strFrameActive].Glyph(strGlyph);
		//glyph.Draw( dc.m_hDC, rcMinimize );
		INT nIndexInArray = bFrameActive ? m_nNcBtnIdxNormalActive : m_nNcBtnIdxNormalInactive;
		if( eStateButtonMinimize == __ENCBS_HOVER )
		{
			m_bmpNcButtonBkHover.AlphaBlendSkinParts( dc.m_hDC, rcMinimize, m_rcNcButtonBkPadding, CExtBitmap::__EDM_STRETCH );
			nIndexInArray = m_nNcBtnIdxHover;
		}
		else if( eStateButtonMinimize == __ENCBS_PRESSED )
		{
			m_bmpNcButtonBkPressed.AlphaBlendSkinParts( dc.m_hDC, rcMinimize, m_rcNcButtonBkPadding, CExtBitmap::__EDM_STRETCH );
			nIndexInArray = m_nNcBtnIdxPressed;
		}
		else if( eStateButtonMinimize == __ENCBS_DISABLED )
			nIndexInArray = m_nNcBtnIdxDisabled;
		CRect rcSrc( 0, 0, m_sizeNcButtonShapeInArr.cx, m_sizeNcButtonShapeInArr.cy );
		rcSrc.OffsetRect( 0, m_sizeNcButtonShapeInArr.cy * nIndexInArray );
		const CExtBitmap & _bmpMin = ( pWnd != NULL && pWnd->IsIconic() ) ? m_bmpArrNcButtonsRestore : m_bmpArrNcButtonsMinimize;
		_bmpMin.AlphaBlendSkinParts(
			dc.m_hDC,
			rcMinimize,
			rcSrc,
			rcZeroPadding,
			( g_PaintManager.m_nLPZ == 96 ) ? ( CExtBitmap::__EDM_CENTER ) : ( CExtBitmap::__EDM_STRETCH ),
			true,
			true
			);
	}
	if((!::IsRectEmpty( &rcMaximizeRestore ) ) && dc.RectVisible( &rcMaximizeRestore ))
	{
		//LPCTSTR strGlyph = stat_NcFrameButton_NameFromState( eStateButtonMaximizeRestore );
		//const CExtSkinGlyph & glyph = m_Skin[_T("Button")][_T("FrameNcButton")][ bFrameMaximized ? _T("Restore") : _T("Maximize") ][strFrameActive].Glyph(strGlyph);
		//glyph.Draw( dc.m_hDC, rcMaximizeRestore );
		INT nIndexInArray = bFrameActive ? m_nNcBtnIdxNormalActive : m_nNcBtnIdxNormalInactive;
		if( eStateButtonMaximizeRestore == __ENCBS_HOVER )
		{
			m_bmpNcButtonBkHover.AlphaBlendSkinParts( dc.m_hDC, rcMaximizeRestore, m_rcNcButtonBkPadding, CExtBitmap::__EDM_STRETCH );
			nIndexInArray = m_nNcBtnIdxHover;
		}
		else if( eStateButtonMaximizeRestore == __ENCBS_PRESSED )
		{
			m_bmpNcButtonBkPressed.AlphaBlendSkinParts( dc.m_hDC, rcMaximizeRestore, m_rcNcButtonBkPadding, CExtBitmap::__EDM_STRETCH );
			nIndexInArray = m_nNcBtnIdxPressed;
		}
		else if( eStateButtonMaximizeRestore == __ENCBS_DISABLED )
			nIndexInArray = m_nNcBtnIdxDisabled;
		CRect rcSrc( 0, 0, m_sizeNcButtonShapeInArr.cx, m_sizeNcButtonShapeInArr.cy );
		rcSrc.OffsetRect( 0, m_sizeNcButtonShapeInArr.cy * nIndexInArray );
		if( bFrameMaximized )
			m_bmpArrNcButtonsRestore.AlphaBlendSkinParts(
			dc.m_hDC,
			rcMaximizeRestore,
			rcSrc,
			rcZeroPadding,
			( g_PaintManager.m_nLPZ == 96 ) ? ( CExtBitmap::__EDM_CENTER ) : ( CExtBitmap::__EDM_STRETCH ),
			true,
			true
			);
		else
			m_bmpArrNcButtonsMaximize.AlphaBlendSkinParts(
			dc.m_hDC,
			rcMaximizeRestore,
			rcSrc,
			rcZeroPadding,
			( g_PaintManager.m_nLPZ == 96 ) ? ( CExtBitmap::__EDM_CENTER ) : ( CExtBitmap::__EDM_STRETCH ),
			true,
			true
			);
	}
	if((!::IsRectEmpty( &rcClose ) ) &&	dc.RectVisible( &rcClose ))
	{
		//LPCTSTR strGlyph = stat_NcFrameButton_NameFromState( eStateButtonClose );
		//const CExtSkinGlyph & glyph = m_Skin[_T("Button")][_T("FrameNcButton")][_T("Close")][strFrameActive].Glyph(strGlyph);
		//glyph.Draw( dc.m_hDC, rcClose );
		INT nIndexInArray = bFrameActive ? m_nNcBtnIdxNormalActive : m_nNcBtnIdxNormalInactive;
		if( eStateButtonClose == __ENCBS_HOVER )
		{
			const CExtBitmap * pBmp = &m_bmpNcButtonBkHover;
			if( ! m_bmpNcButtonBkHoverX.IsEmpty() )
				pBmp = &m_bmpNcButtonBkHoverX;
			pBmp->AlphaBlendSkinParts( dc.m_hDC, rcClose, m_rcNcButtonBkPadding, CExtBitmap::__EDM_STRETCH );
			nIndexInArray = m_nNcBtnIdxHover;
		}
		else if( eStateButtonClose == __ENCBS_PRESSED )
		{
			const CExtBitmap * pBmp = &m_bmpNcButtonBkPressed;
			if( ! m_bmpNcButtonBkPressedX.IsEmpty() )
				pBmp = &m_bmpNcButtonBkPressedX;
			pBmp->AlphaBlendSkinParts( dc.m_hDC, rcClose, m_rcNcButtonBkPadding, CExtBitmap::__EDM_STRETCH );
			nIndexInArray = m_nNcBtnIdxPressed;
		}
		else if( eStateButtonClose == __ENCBS_DISABLED )
			nIndexInArray = m_nNcBtnIdxDisabled;
		CRect rcSrc( 0, 0, m_sizeNcButtonShapeInArr.cx, m_sizeNcButtonShapeInArr.cy );
		rcSrc.OffsetRect( 0, m_sizeNcButtonShapeInArr.cy * nIndexInArray );
		m_bmpArrNcButtonsClose.AlphaBlendSkinParts(
			dc.m_hDC,
			rcClose,
			rcSrc,
			rcZeroPadding,
			( g_PaintManager.m_nLPZ == 96 ) ? ( CExtBitmap::__EDM_CENTER ) : ( CExtBitmap::__EDM_STRETCH ),
			true,
			true
			);
	}
	::SetStretchBltMode( dc.m_hDC, nOldStretchBltMode );
}
#endif
#endif


