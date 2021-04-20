
#include "stdafx.h"
#include "VTSDef.h"

CString OrderMethodStr(OrderMethod method, ORDER_ACTOR& actor)
{
	CString strMethod;
	switch(method)
	{
	case om_click:
		strMethod = _T("ȣ��Ŭ��");
		actor = OA_CLIENT;
		break;
	case om_dbl_click:
		strMethod = _T("ȣ������Ŭ��");
		actor = OA_CLIENT;
		break;
	case om_button:
		strMethod = _T("��ư");
		actor = OA_CLIENT;
		break;

	case om_each_cancel_button:
		strMethod = _T("������ҹ�ư");
		actor = OA_CLIENT;
		break;
	case om_each_liquid_button:
		strMethod = _T("����û���ư");
		actor = OA_CLIENT;
		break;
	case om_drag:
		strMethod = _T("�巡��");
		actor = OA_CLIENT;
		break;
	case om_mit:
		strMethod = _T("MIT");
		actor = OA_MIT;
		break;
	case om_stop_losscut:
		strMethod = _T("�ս�����");
		actor = OA_STOP_LOSSCUT;
		break;
	case om_stop_earncut:
		strMethod = _T("���ͽ���");
		actor = OA_STOP_EARNCUT;
		break;
	default:
		ASSERT(FALSE);
	};
	return strMethod;
};
