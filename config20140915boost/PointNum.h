#pragma once
#include "Paramini.h"

// CPointNum �Ի���

class CPointNum : public CDialog
{
	DECLARE_DYNAMIC(CPointNum)

public:
	CPointNum(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPointNum();
	int insertnum;
	CString dis;
	float distance;
	// �Ի�������
	enum { IDD = IDD_Pointnum };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
