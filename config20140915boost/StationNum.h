#pragma once
#include "Paramini.h"
#include "cxcore.h"

using namespace cv;  
using namespace std; 
// CStationNum �Ի���

class CStationNum : public CDialog
{
	DECLARE_DYNAMIC(CStationNum)

public:
	CStationNum(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CStationNum();

// �Ի�������
	enum { IDD = IDD_Station };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
