#pragma once
#include "Paramini.h"

// CPointNum 对话框

class CPointNum : public CDialog
{
	DECLARE_DYNAMIC(CPointNum)

public:
	CPointNum(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPointNum();
	int insertnum;
	CString dis;
	float distance;
	// 对话框数据
	enum { IDD = IDD_Pointnum };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
