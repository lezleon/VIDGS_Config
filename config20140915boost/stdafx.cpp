// stdafx.cpp : 只包括标准包含文件的源文件
// config.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

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

