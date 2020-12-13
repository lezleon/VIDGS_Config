#pragma once
#include "Laster.h"


// CTypeChoose 对话框

typedef enum
{
	PROGTYPE_Vision,
	PROGTYPE_Laser,
} PROGTYPE;

class CTypeChoose : public CDialog
{
	DECLARE_DYNAMIC(CTypeChoose)

public:
	CTypeChoose(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTypeChoose();

// 对话框数据
	enum { IDD = IDD_TYPE };
	PROGTYPE m_ProgType;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedVision();
	afx_msg void OnBnClickedLaser();
	virtual BOOL OnInitDialog();
};
extern CTypeChoose g_Type;