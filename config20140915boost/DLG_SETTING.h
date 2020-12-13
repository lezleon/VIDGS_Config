#pragma once
#include "afxpropertygridctrl.h"
#include "Doc.h"

// DLG_SETTING �Ի���

class DLG_SETTING : public CDialogEx
{
	DECLARE_DYNAMIC(DLG_SETTING)

public:
	DLG_SETTING(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DLG_SETTING();
	virtual BOOL OnInitDialog();

// �Ի�������
	enum { IDD = IDD_DLG_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
