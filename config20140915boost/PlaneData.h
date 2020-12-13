#pragma once
#include "Paramini.h"
#include "resource.h"
#include "cxcore.h"
#include "Doc.h"
using namespace cv;  
using namespace std; 

// CPlaneData 对话框

class CPlaneData : public CDialog
{
	DECLARE_DYNAMIC(CPlaneData)

public:
	CPlaneData(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPlaneData();

	String routename;
	Document Doc;
	PlaneInfo planeinfo;
// 对话框数据
	enum { IDD = IDD_PlaneData };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};