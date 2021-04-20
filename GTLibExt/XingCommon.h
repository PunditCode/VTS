#pragma once


enum XingDataType
{
	xdt_string = 0,		// 문자열
	xdt_long,			// 정수
	xdt_float,			// 실수
	xdt_double,
	xdt_float_dot,		// 실수( 소숫점을 가지고 있음 )
	xdt_currency,		// 통화(,로 구분)
};
#define DATA_TYPE_DOUBLE	DATA_TYPE_FLOAT
#define DATA_TYPE_DOT		DATA_TYPE_FLOAT_DOT

CString GetDispData( char* psData, int nSize, XingDataType nType, int nDotPos = 2 );
void SetPacketData( char* psData, int nSize, CWnd* pWnd, int nID, XingDataType nType, int nDotPos = 2);
void SetPacketData( char* psData, int nSize, LPCTSTR pszSrc, XingDataType nType, int nDotPos = 2);

#define WM_XING_BEGIN		0x0800
#define STR(__field__, __type__) __field__ = GetDispData(p->##__field__, sizeof(p->##__field__), xdt_##__type__)
#define STR_FD(__field__, __dotpos__) __field__ = GetDispData(p->##__field__, sizeof(p->##__field__), xdt_float_dot, __dotpos__)
