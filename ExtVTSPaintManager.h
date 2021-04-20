#pragma once

class CExtVTSPaintManager : public CExtPaintManagerOffice2003NoThemes
{
public:
	DECLARE_DYNCREATE( CExtVTSPaintManager );

	void SetThemeColors(COLORREF clrFillHint,COLORREF clrAccentHint);

	COLORREF m_clrFillHint;
	COLORREF m_clrAccentHint;

	CExtVTSPaintManager();

protected:
	virtual void InitTranslatedColors();
	virtual bool OnQueryThemeColors(COLORREF *pclrFillHint, COLORREF *pclrAccentHint = NULL);

	virtual bool OnQueryUseThemeColors() const
	{
		return true;
	}

	virtual e_system_theme_t OnQuerySystemTheme() const
	{
		return ThemeUnknown;
	}

	virtual COLORREF QueryObjectTextColor(
		CDC & dc,
		bool bEnabled,
		bool bFocused,
		bool bHovered,
		bool bPressed,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);

};

class VTSButton : public CExtButton
{

};

class CExtPaintManagerForManagerMode : public CExtPaintManagerOffice2003NoThemes
{
public:
	DECLARE_DYNCREATE( CExtPaintManagerForManagerMode );
	CExtPaintManagerForManagerMode(void);

	void SetThemeColors(COLORREF clrFillHint,COLORREF clrAccentHint);

	COLORREF m_clrFillHint;
	COLORREF m_clrAccentHint;

protected:
	virtual void InitTranslatedColors();
	virtual bool OnQueryThemeColors(COLORREF *pclrFillHint, COLORREF *pclrAccentHint = NULL);
	virtual bool OnQueryUseThemeColors() const;
	virtual e_system_theme_t OnQuerySystemTheme() const;
	virtual COLORREF QueryObjectTextColor(
		CDC & dc,
		bool bEnabled,
		bool bFocused,
		bool bHovered,
		bool bPressed,
		CObject * pHelperSrc,
		LPARAM lParam = 0L
		);

};
