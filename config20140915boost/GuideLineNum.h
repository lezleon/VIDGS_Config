#pragma once
#include "afxwin.h"


// CGuideLineNum �Ի���

class CGuideLineNum : public CDialog
{
	DECLARE_DYNAMIC(CGuideLineNum)

public:
	CGuideLineNum(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CGuideLineNum();

// �Ի�������
	enum { IDD = IDD_GuideLineNum };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_guideLineNum;
	afx_msg void OnCbnSelchangeComboguideline();
	virtual BOOL OnInitDialog();

public:
	CString str_guide;
	int guidenum;

	BOOL guidenumFlag;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
