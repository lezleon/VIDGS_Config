#pragma once


// CChoosePoint 对话框

class CChoosePoint : public CDialog
{
	DECLARE_DYNAMIC(CChoosePoint)

public:
	CChoosePoint(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CChoosePoint();
	int Pointflag;

// 对话框数据
	enum { IDD = IDD_choose };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPointinsert();
	afx_msg void OnBnClickedPointrenew();
};
