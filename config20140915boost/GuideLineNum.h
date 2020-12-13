#pragma once
#include "afxwin.h"


// CGuideLineNum 对话框

class CGuideLineNum : public CDialog
{
	DECLARE_DYNAMIC(CGuideLineNum)

public:
	CGuideLineNum(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGuideLineNum();

// 对话框数据
	enum { IDD = IDD_GuideLineNum };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_guideLineNum;
	afx_msg void OnCbnSelchangeComboguideline();
	virtual BOOL OnInitDialog();

public:
	CString str_guide;
	int guidenum;

	BOOL guidenumFlag;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
