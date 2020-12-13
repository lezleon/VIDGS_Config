#pragma once
#include "Laster.h"


// CTypeChoose �Ի���

typedef enum
{
	PROGTYPE_Vision,
	PROGTYPE_Laser,
} PROGTYPE;

class CTypeChoose : public CDialog
{
	DECLARE_DYNAMIC(CTypeChoose)

public:
	CTypeChoose(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTypeChoose();

// �Ի�������
	enum { IDD = IDD_TYPE };
	PROGTYPE m_ProgType;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedVision();
	afx_msg void OnBnClickedLaser();
	virtual BOOL OnInitDialog();
};
extern CTypeChoose g_Type;