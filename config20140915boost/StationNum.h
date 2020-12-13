#pragma once
#include "Paramini.h"
#include "cxcore.h"

using namespace cv;  
using namespace std; 
// CStationNum 对话框

class CStationNum : public CDialog
{
	DECLARE_DYNAMIC(CStationNum)

public:
	CStationNum(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStationNum();

// 对话框数据
	enum { IDD = IDD_Station };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	BOOL station;
	int StationNum;
	int PlacementNum;
	int stopdistance;
	int toofardistance;
	int speed;
	int pctime;
	
	
};
