#pragma once


// CChoosePoint �Ի���

class CChoosePoint : public CDialog
{
	DECLARE_DYNAMIC(CChoosePoint)

public:
	CChoosePoint(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CChoosePoint();
	int Pointflag;

// �Ի�������
	enum { IDD = IDD_choose };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPointinsert();
	afx_msg void OnBnClickedPointrenew();
};
