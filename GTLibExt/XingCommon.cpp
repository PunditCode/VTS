#include "stdafx.h"
#include "XingCommon.h"

CString GetDispData( char* psData, int nSize, XingDataType nType, int nDotPos )
{
	CString strData( psData, nSize );

	//-----------------------------------------------------------------------
	// ���ڿ� : �޺κ��� Space ����
	if( nType == xdt_string )
	{
		strData.TrimRight();
	}

	//-----------------------------------------------------------------------
	// ���� : ���� 0 �� ����. ���� �ϱ� ���� ������ ��ȯ�� �ٽ� ���ڷ� ��ȯ
	else if( nType == xdt_long )
	{
		CString strTemp;
		strTemp.Format( "%d", atoi( strData ) );
		strData = strTemp;
	}

	//-----------------------------------------------------------------------
	// �Ǽ� : ���� 0 �� ����. ���� �ϱ� ���� �Ǽ��� ��ȯ�� �ٽ� nDotPos ��ŭ ǥ��
	else if( nType == xdt_float || nType == xdt_double)
	{
		// �Ҽ����� ������ ���� �������̹Ƿ� nDotPos ��ġ�� ���� ��´�.
		strData.Insert( strData.GetLength() - nDotPos, '.' );

		// �Ҽ��� ���ϸ� nDotPos ��ŭ ǥ���ϱ� ���� format�� �����.
		CString strFormat;
		strFormat.Format( "%%.%df", nDotPos );
		CString strTemp;
		strTemp.Format( strFormat, atof( strData ) );
		strData = strTemp;
	}

	//-----------------------------------------------------------------------
	// �Ǽ� : ���� 0 �� ����. ���� �ϱ� ���� �Ǽ��� ��ȯ�� �ٽ� nDotPos ��ŭ ǥ��
	else if( nType == xdt_float_dot )
	{
		// �Ҽ����� �����ִ�
		int nFind = strData.Find( "." );
		nDotPos = strData.GetLength() - nFind - 1;
			
		// �Ҽ��� ���ϸ� nDotPos ��ŭ ǥ���ϱ� ���� format�� �����.
		CString strFormat;
		strFormat.Format( "%%.%df", nDotPos );
		CString strTemp;
		strTemp.Format( strFormat, atof( strData ) );
		strData = strTemp;
	}
	else if(xdt_currency == nType)
	{
		INT n = atoi( strData );

		CString strTemp;
		strTemp.Format( "%d", n );
		strData = strTemp;

		if(strData.GetLength() <= 3)
			return strData;
		else
		{

			int nComma = strData.GetLength() / 3;
			if(nComma > 1 && (strData.GetLength() % 3 == 0))
				nComma -= 1;

			int nLen = strData.GetLength() + nComma;
			TCHAR* s = new TCHAR[nLen + 1];

			int pos = nLen - 1;
			for(int i = strData.GetLength() - 1 ; i >= 0 ; --i)
			{
				s[pos] = strData[i];
				if((strData.GetLength() - i) % 3 == 0 && pos > 0)
				{
					--pos;
					s[pos] = _T(',');
				}
				--pos;
			}
			s[nLen] = _T('\0');
			strData = s;
			delete [] s;
		}
	}


	return strData;
}

void SetPacketData( char* psData, int nSize, CWnd* pWnd, int nID, XingDataType nType, int nDotPos )
{
	CString str;
	pWnd->GetDlgItemText( nID, str );
	SetPacketData( psData, nSize, str, nType, nDotPos );
}

void SetPacketData( char* psData, int nSize, LPCTSTR pszSrc, XingDataType nType, int nDotPos )
{
	//-----------------------------------------------------------------------
	// ���ڿ�
	if( nType == xdt_string )
	{
		// ���� ����
		// ���� ���ڸ��� ' ' �� ä��

		// ����ũ�Ⱑ �� �����ͺ��� �۴ٸ� �����ʹ� ©���� �ϹǷ� �����߻�
		int nSrcLen = strlen( pszSrc );

		// ���� Space�� ä���
		FillMemory( psData, nSize, ' ' );

		// �տ����� �����͸� �ִ´�.
		// �������Ͱ� ũ�ٸ� �޺κ��� ������.
		CopyMemory( psData, pszSrc, min( nSize, nSrcLen ) );
	}

	//-----------------------------------------------------------------------
	// ����
	else if( nType == xdt_long )
	{
		// ������ ����
		// ���� ���ڸ��� '0' ���� ä��

		// ����ũ�Ⱑ �� �����ͺ��� �۴ٸ� �����ʹ� ©���� �ϹǷ� �����߻�
		int nSrcLen = strlen( pszSrc );
		ASSERT( nSize >= nSrcLen );

		// ���� 0 ���� ä���
		FillMemory( psData, nSize, '0' );

		// �ڿ������� �����͸� �ִ´�.
		if( nSize >= nSrcLen )
		{
			CopyMemory( psData+nSize-nSrcLen, pszSrc, nSrcLen );
		}
		// �������Ͱ� ũ�ٸ� ���������� �޺κ��� ������.
		else
		{
			CopyMemory( psData, pszSrc, nSize );
		}
	}

	//-----------------------------------------------------------------------
	// �Ǽ� : �Ҽ����� ���� �ʴ´�.
	else if( nType == xdt_float || nType == xdt_double)
	{
		// �Ҽ��� ��ġ�� �������� ����
		// �Ҽ����� ���� ������ �������� ���ڸ��� �Ҽ����� ���ڸ��� 0���� ä��

		int nSrcLen = strlen( pszSrc );

		// ���� 0 ���� ä���
		FillMemory( psData, nSize, '0' );

		// �������Ϳ��� �Ҽ����� ��ġ�� ã�Ƽ�
		// ���������� �������� ���̿� �Ҽ����� ���̸� ���Ѵ�.
		int nSrcIntLen;
		int nSrcDotLen;
		LPCTSTR psz = _tcschr( pszSrc, '.' );
		if( psz == NULL )		// �Ҽ��ΰ� ����.
		{
			nSrcIntLen = strlen( pszSrc );
			nSrcDotLen = 0;
		}
		else					// �Ҽ��ΰ� �ִ�.
		{
			nSrcIntLen = psz - pszSrc;
			nSrcDotLen = strlen( pszSrc ) - nSrcIntLen - 1;
		}

		// �����θ� �ִ´�.
		if( nSize-nDotPos >= nSrcIntLen )
		{
			CopyMemory( psData+nSize-nDotPos-nSrcIntLen, pszSrc, nSrcIntLen );
		}
		else
		{
			// ���������� ������ ���̰� �� �� ��� �������� ���ڸ��� �����ȴ�.
			ASSERT( FALSE );
			CopyMemory( psData, pszSrc, nSize-nDotPos );
		}

		// �Ҽ��θ� �ִµ� ���������� �Ҽ��� ���̰� �� �� ��� �Ҽ����� ���ڸ��� �����ȴ�.
		ASSERT( nDotPos >= nSrcDotLen );
		CopyMemory( psData+nSize-nDotPos, pszSrc + strlen( pszSrc ) - nSrcDotLen, min( nDotPos, nSrcDotLen ) );
	}

	//-----------------------------------------------------------------------
	// �Ǽ� : �Ҽ����� ����
	else if( nType == xdt_float_dot )
	{
		// �Ҽ��� ��ġ�� �������� ����
		// �Ҽ����� ���� ������ �������� ���ڸ��� �Ҽ����� ���ڸ��� 0���� ä��
		
		int nSrcLen = strlen( pszSrc );
		
		// ���� 0 ���� ä���
		FillMemory( psData, nSize, '0' );
		
		// �������Ϳ��� �Ҽ����� ��ġ�� ã�Ƽ�
		// ���������� �������� ���̿� �Ҽ����� ���̸� ���Ѵ�.
		int nSrcIntLen;
		int nSrcDotLen;
		LPCTSTR psz = _tcschr( pszSrc, '.' );
		if( psz == NULL )		// �Ҽ��ΰ� ����.
		{
			nSrcIntLen = strlen( pszSrc );
			nSrcDotLen = 0;
		}
		else					// �Ҽ��ΰ� �ִ�.
		{
			nSrcIntLen = psz - pszSrc;
			nSrcDotLen = strlen( pszSrc ) - nSrcIntLen - 1;
		}
		
		// �����θ� �ִ´�.
		if( nSize-nDotPos-1 >= nSrcIntLen )
		{
			CopyMemory( psData+nSize-nDotPos-nSrcIntLen-1, pszSrc, nSrcIntLen );
		}
		else
		{
			// ���������� ������ ���̰� �� �� ��� �������� ���ڸ��� �����ȴ�.
			ASSERT( FALSE );
			CopyMemory( psData, pszSrc, nSize-nDotPos-1 );
		}

		// �Ҽ����� ��´�.
		psData[nSize-nDotPos-1] = '.';
		
		// �Ҽ��θ� �ִµ� ���������� �Ҽ��� ���̰� �� �� ��� �Ҽ����� ���ڸ��� �����ȴ�.
		ASSERT( nDotPos >= nSrcDotLen );
		CopyMemory( psData+nSize-nDotPos, pszSrc + strlen( pszSrc ) - nSrcDotLen, min( nDotPos, nSrcDotLen ) );
	}
}
