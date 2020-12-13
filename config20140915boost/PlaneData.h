#pragma once
#include "Paramini.h"
#include "resource.h"
#include "cxcore.h"
#include "Doc.h"
using namespace cv;  
using namespace std; 

// CPlaneData �Ի���

class CPlaneData : public CDialog
{
	DECLARE_DYNAMIC(CPlaneData)

public:
	CPlaneData(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPlaneData();

	String routename;
	Document Doc;
	PlaneInfo planeinfo;
// �Ի�������
	enum { IDD = IDD_PlaneData };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};