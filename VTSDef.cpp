
#include "stdafx.h"
#include "VTSDef.h"

CString OrderMethodStr(OrderMethod method, ORDER_ACTOR& actor)
{
	CString strMethod;
	switch(method)
	{
	case om_click:
		strMethod = _T("호가클릭");
		actor = OA_CLIENT;
		break;
	case om_dbl_click:
		strMethod = _T("호가더블클릭");
		actor = OA_CLIENT;
		break;
	case om_button:
		strMethod = _T("버튼");
		actor = OA_CLIENT;
		break;

	case om_each_cancel_button:
		strMethod = _T("개별취소버튼");
		actor = OA_CLIENT;
		break;
	case om_each_liquid_button:
		strMethod = _T("개별청산버튼");
		actor = OA_CLIENT;
		break;
	case om_drag:
		strMethod = _T("드래그");
		actor = OA_CLIENT;
		break;
	case om_mit:
		strMethod = _T("MIT");
		actor = OA_MIT;
		break;
	case om_stop_losscut:
		strMethod = _T("손실제한");
		actor = OA_STOP_LOSSCUT;
		break;
	case om_stop_earncut:
		strMethod = _T("이익실현");
		actor = OA_STOP_EARNCUT;
		break;
	default:
		ASSERT(FALSE);
	};
	return strMethod;
};
