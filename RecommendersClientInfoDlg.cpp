// RecommendersClientInfoDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "VTS.h"
#include "RecommendersClientInfoDlg.h"


// CRecommendersClientInfoDlg ��ȭ �����Դϴ�.
IMPLEMENT_DYNAMIC(CRecommendersClientInfoDlg, CRequestRecvDlg)

CRecommendersClientInfoDlg::CRecommendersClientInfoDlg(CWnd* pParent /*=NULL*/)
: CRequestRecvDlg(CRecommendersClientInfoDlg::IDD, pParent)
{

}

CRecommendersClientInfoDlg::~CRecommendersClientInfoDlg()
{
}

void CRecommendersClientInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CRequestRecvDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_QUERY_FIELD, m_comboQueryField);
	DDX_Control(pDX, IDC_GRID, m_grid);
}


BEGIN_MESSAGE_MAP(CRecommendersClientInfoDlg, CRequestRecvDlg)
	ON_BN_CLICKED(IDC_BUTTON_QUERY, &CRecommendersClientInfoDlg::OnBnClickedButtonQuery)
	ON_BN_CLICKED(IDC_BUTTON_TO_EXCEL, &CRecommendersClientInfoDlg::OnBnClickedButtonToExcel)
END_MESSAGE_MAP()


// CRecommendersClientInfoDlg �޽��� ó�����Դϴ�.

BOOL CRecommendersClientInfoDlg::OnInitDialog()
{
	CRequestRecvDlg::OnInitDialog();
	AddAnchor(IDC_GRID, __RDA_LT, __RDA_RB);
	ShowSizeGrip(FALSE);
	InitGrid();

	int n = 0;
	m_comboQueryField.InsertString(n++, _T("ID"));
	m_comboQueryField.InsertString(n++, _T("�̸�"));
	m_comboQueryField.InsertString(n++, _T("�޴���"));
	m_comboQueryField.InsertString(n++, _T("�̸���"));
	m_comboQueryField.SetCurSel(1);

	OnBnClickedButtonQuery();
	SetKeyboard(GetDlgItem(IDC_EDIT_QUERY)->GetSafeHwnd(), FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

enum Column
{
	c_registerDate,
	c_id,
	c_name,
	c_mobile,
	c_email,
	c_connection,
	c_enableOvernight,
	c_evalBalance,
	c_fee_Futures,
	c_fee_Options,
#ifndef FEE_NIGHT_SAME
	c_fee_NF,
	c_fee_NO,
#endif
	c_fee_Foreign,

};

void CRecommendersClientInfoDlg::InitGrid()
{
	m_grid.SetDefaultStyle(VTSGrid::sel_full_row);
//	m_grid.OuterColumnCountLeftSet(1L, false);
//	m_grid.OuterColumnWidthSet(true, 0L, 40);

	//m_grid.OuterColumnCountLeftSet(1);
	//m_grid.OuterColumnWidthSet(true, 0, 20);

	VTSGridColDefs defs;
	defs.push_back(std::make_pair(_T("�����Ͻ�"), 0.10)); //1
	defs.push_back(std::make_pair(_T("���̵�"), 0.08));
	defs.push_back(std::make_pair(_T("�̸�"), 0.06));
	defs.push_back(std::make_pair(_T("�޴���"), 0.09));
	defs.push_back(std::make_pair(_T("�̸���"), 0.10)); //6
	defs.push_back(std::make_pair(_T("���ӻ���"), 0.05));
	defs.push_back(std::make_pair(_T("�������ռ���"), 0.05)); //10
	defs.push_back(std::make_pair(_T("�򰡴㺸��"), 0.08)); //11
	defs.push_back(std::make_pair(_T("����"), 0.05));
	defs.push_back(std::make_pair(_T("�ɼ�"), 0.05));
#ifndef FEE_NIGHT_SAME
	defs.push_back(std::make_pair(_T("�߰�����"), 0.05));
	defs.push_back(std::make_pair(_T("�߰��ɼ�"), 0.05));
#endif
	defs.push_back(std::make_pair(_T("�ؿܼ���"), 0.05)); //16

	m_grid.SetColumDef(defs, true);
}

void CRecommendersClientInfoDlg::OnBnClickedButtonQuery()
{
	BeginWaitCursor();
	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(FALSE);

	CString query;
	GetDlgItemText(IDC_EDIT_QUERY, query);

	int nQueryField = m_comboQueryField.GetCurSel();

	tClientCol col = cc_none;
	switch (nQueryField)
	{
	case 0: col = cc_id; break;
	case 1:	col = cc_name; break;
	case 2:	col = cc_mobile; break;
	case 3:	col = cc_email; break;
	}

	GetSession()->RequestClientInfoList(GetSafeHwnd(), nQueryField, query);
}

void CRecommendersClientInfoDlg::OnRequestReceived(DKBuffer& dbuf, LPVOID lpData)
{
	EndWaitCursor();
	m_clients.clear();

	UINT nColCount, nRowCount;
	dbuf.RD(nColCount);
	dbuf.RD(nRowCount);

	m_grid.RowRemoveAll(false);
	m_grid.RowAdd(nRowCount);

	for (UINT nRow = 0; nRow < nRowCount; ++nRow)
	{
		ClientManagedPtr pClient(new ClientManaged);
		if (!pClient->FromBufferOnlyBase(dbuf))
			return;

		m_clients.push_back(pClient);
		SetRow(nRow, pClient);
	}
	
	m_grid.BestFitColumns();
	m_grid.OnSwUpdateScrollBars();
	m_grid.OnSwInvalidate(false);
	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(TRUE);
}

void CRecommendersClientInfoDlg::SetRow(int nRow, ClientManagedPtr pClient)
{
	int nCol = 0;
	m_grid.QuickSetText(nCol, nRow, pClient->registerDate.Format(_T("%Y-%m-%d %H:%M"))); ++nCol;
	m_grid.QuickSetText(nCol, nRow, pClient->id); ++nCol;
	m_grid.QuickSetText(nCol, nRow, pClient->name); ++nCol;
	m_grid.QuickSetText(nCol, nRow, pClient->mobile); ++nCol;
	m_grid.QuickSetText(nCol, nRow, pClient->email); ++nCol;
	m_grid.QuickSetText(nCol, nRow, pClient->is_login ? _T("������") : _T("")); ++nCol;
	m_grid.QuickSetText(nCol, nRow, pClient->enableOvernight ? _T("����") : _T("�Ұ�")); ++nCol;

	m_grid.QuickSetText(nCol, nRow, ::ToThousand(pClient->get_bankBalance())); ++nCol;
	CString s;
	s.Format(_T("%s(%d)"), ::ToString(pClient->feeFutures, 5), pClient->futuresMaxContract);
	m_grid.QuickSetText(nCol, nRow, s); ++nCol;

	s.Format(_T("%s(%d/%d)"), ::ToString(pClient->feeOptions, 5), pClient->optionsBuyMaxContract, pClient->optionsSellMaxContract);
	m_grid.QuickSetText(nCol, nRow, s); ++nCol;
#ifndef FEE_NIGHT_SAME
	s.Format(_T("%s(%d)"), ::ToString(pClient->feeNightFutures, 5), pClient->futuresMaxContract);
	m_grid.QuickSetText(nCol, nRow, s); ++nCol;

	s.Format(_T("%s(%d/%d)"), ::ToString(pClient->feeNightOptions, 5), pClient->optionsBuyMaxContract, pClient->optionsSellMaxContract);
	m_grid.QuickSetText(nCol, nRow, s); ++nCol;
#endif
	s.Format(_T("%s(%d)"), ::ToString(pClient->feeForeignFutures, 5), pClient->foreignFuturesMaxContract);
	m_grid.QuickSetText(nCol, nRow, s); ++nCol;
}

void CRecommendersClientInfoDlg::OnRequestError(int nRequestID, const CString& strMsg)
{
	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(TRUE);
	if (NUL_IDX != nRequestID)
		ErrMsgBox(_T("������ ��ȸ��û�� �����߽��ϴ�."));
}

void CRecommendersClientInfoDlg::OnBnClickedButtonToExcel()
{
	m_grid.ToExcel();
}