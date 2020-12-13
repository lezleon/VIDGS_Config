// config.h : config 应用程序的主头文件
//
#pragma once

//#define  RECOG_EXPORTS
//#ifdef RECOG_EXPORTS
//#define RECOG_API extern "C" __declspec(dllexport)
//#else
//#define RECOG_API extern "C" __declspec(dllimport)
//#endif

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号


// CconfigApp:
// 有关此类的实现，请参阅 config.cpp
//

class CconfigApp : public CWinApp
{
public:
	CconfigApp();


// 重写
public:
	virtual BOOL InitInstance();

// 实现
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CconfigApp theApp;

//RECOG_API CLaster* _stdcall classLaserInit();