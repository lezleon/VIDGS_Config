// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// config.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

CString GetAppPath(HMODULE hModule)
{
	TCHAR buf[MAX_PATH] = {'\0'};
	CString strDir, strTemp;

	::GetModuleFileName( hModule, buf, MAX_PATH);
	strTemp = buf;
	strDir = strTemp.Left( strTemp.ReverseFind('\\') + 1 );

	return strDir;
}

