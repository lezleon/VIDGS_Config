// config.h : config Ӧ�ó������ͷ�ļ�
//
#pragma once

//#define  RECOG_EXPORTS
//#ifdef RECOG_EXPORTS
//#define RECOG_API extern "C" __declspec(dllexport)
//#else
//#define RECOG_API extern "C" __declspec(dllimport)
//#endif

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CconfigApp:
// �йش����ʵ�֣������ config.cpp
//

class CconfigApp : public CWinApp
{
public:
	CconfigApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CconfigApp theApp;

//RECOG_API CLaster* _stdcall classLaserInit();