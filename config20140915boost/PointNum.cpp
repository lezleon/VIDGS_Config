// PointNum.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "config.h"
#include "PointNum.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPointNum �Ի���

IMPLEMENT_DYNAMIC(CPointNum, CDialog)

	CPointNum::CPointNum(CWnd* pParent /*=NULL*/)
	: CDialog(CPointNum::IDD, pParent)
{

}

CPointNum::~CPointNum()
{
}

void CPointNum::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPointNum, CDialog)
	ON_BN_CLICKED(IDOK, &CPointNum::OnBnClickedOk)
END_MESSAGE_MAP()


// CPointNum ��Ϣ�������


void CPointNum::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str1,str2;
	GetDlgItem(IDC_EDITnum)->GetWindowText(str1);
	GetDlgItem(IDC_distance)->GetWindowText(str2);
	insertnum=atoi(str1);
	distance=atof(str2);
	
	CDialog::OnOK();
}
