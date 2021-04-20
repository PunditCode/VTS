#pragma once

#include "resource.h"
#include "UltimateGrid/UGCtrl.h"
#include <RequestRecvDlg.h>
#include "GTMerchandisePool.h"
#include "VTSReqeustRecvWndBase.h"

class COptionsTableItem : public GTCurrentObserver
{ 
	friend class COptionsTableDlg;
public:
	virtual ~COptionsTableItem();
private:
	COptionsTableItem(CodeOptions* pCode, int nRow, CUGCtrl& grid);
	
	void RunRecv();
	OVERRIDE void OnCurrentInitUpdated(GTCurrentPtr pCurrent);
	OVERRIDE void OnCurrentUpdated(GTCurrentPtr pCurrent);
	void Set(GTCurrentPtr pCurrent);

	CodeOptions*		m_pCode;
	int					m_nRow;
	CUGCtrl&			m_grid;
	GTMerchandisePtr	m_pMerch;
};


class COptionsTableGrid : public CUGCtrl
{
public:
	class COptionsTableDlg* m_pOwner;

	OVERRIDE void OnLClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed);
	OVERRIDE void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
};

//typedef  VTSRequestRecvWndBase<CExtNCW<CExtResizableDialog> > OTDBaseDlg;
typedef  VTSModelessDlg<CRequestRecvDlg> OTDBaseDlg;

class COptionsTableDlg : public VTSModelessDlg<CRequestRecvDlg>
{
	DECLARE_DYNAMIC(COptionsTableDlg)

public:
	COptionsTableDlg(CWnd* pParent = NULL, BOOL bNight = FALSE);   // 표준 생성자입니다.
	virtual ~COptionsTableDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_OPTIONS_TABLE };

	BOOL m_bNight;
	CExtRadioButton    m_buttons[4];
	std::vector<std::pair<UINT, UINT> >	m_months;
	static CodeOptions*		s_pLastSelCode;

	CExtLabel		m_labelCode;
	COptionsTableGrid	m_grid;
	CExtButton		m_buttonUpdate;
	void InitGrid();

	void SetMonth(UINT nYear, UINT nMonth);

	typedef boost::shared_ptr<COptionsTableItem> COptionsTableItemPtr;

	struct Item
	{
		int		nRow;
		COptionsTableItemPtr	pCall;
		COptionsTableItemPtr	pPut;
	};

	std::map<CString, Item>	m_items;
	CFont	m_fontPrice;
	volatile BOOL m_bClose;
	void RunRecvAllFnc();

	void OnGridLClicked(int nCol, int nRow, int updn);
	void OnGridLDblClicked(int nCol, int nRow);

	int		m_nLastSelectRow;
	PositionType m_selPos;
	class CFastTradeDlg*		m_pOwner;
protected:
	void SetCode(CodeOptions* pCode);
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	
	OVERRIDE void OnRequestReceived(DKBuffer& dbuf, LPVOID lpData);
	OVERRIDE void OnRequestError(int nRequestID, const CString& strMsg);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedButton(UINT nID);
	afx_msg LRESULT OnOrderContracts(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonUpdate();
};
