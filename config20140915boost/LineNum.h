#pragma once
#include "afxwin.h"
#include "resource.h"

// CLineNum �Ի���

class CLineNum : public CDialog
{
	DECLARE_DYNAMIC(CLineNum)

public:
	CLineNum(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLineNum();

// �Ի�������
	enum { IDD = IDD_LineNum };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_lineguidenum;
	CComboBox m_linestopnum;
	CString str_guidenum;
	CString str_stopnum;
	int guidenum;
	int stopnum;

	BOOL linenumFlag;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboLineguide();
	afx_msg void OnCbnSelchangeComboLinestop();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	//CString LaserY;
};
