#pragma once
#include "afxpropertygridctrl.h"
#include "Doc.h"

// DLG_SETTING 对话框

class DLG_SETTING : public CDialogEx
{
	DECLARE_DYNAMIC(DLG_SETTING)

public:
	DLG_SETTING(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DLG_SETTING();
	virtual BOOL OnInitDialog();

// 对话框数据
	enum { IDD = IDD_DLG_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	//CMFCPropertyGridCtrl m_COM_SETTING;
	//CMFCPropertyGridCtrl m_para_setting;
	CMFCPropertyGridCtrl m_propertyGrid_COM;
	CMFCPropertyGridCtrl m_propertyGrid_Para;
	Document Doc;
	String routename;
	afx_msg void OnBnClickedOk();
};
