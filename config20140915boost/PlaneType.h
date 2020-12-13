#pragma once
#include "afxwin.h"
#include "Paramini.h"
#include "resource.h"
#include "cxcore.h"
#include "Doc.h"
using namespace cv;  
using namespace std; 


// CPlaneType �Ի���

class CPlaneType : public CDialog
{
	DECLARE_DYNAMIC(CPlaneType)

public:
	CPlaneType(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPlaneType();

// �Ի�������
	enum { IDD = IDD_PlaneType };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_comboChoose;
	CComboBox m_comboChoose2;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnCbnSelchangeCombo2();
public:
	CString str_guide;
	CString str_stop;
	int stopNum;
	int guidenum;
	afx_msg void OnBnClickedCancel();
	BOOL planeType;
	Document Doc;
	String routename;

	vector<string>VS;
};
