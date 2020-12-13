// PointNum.cpp : 实现文件
//

#include "stdafx.h"
#include "config.h"
#include "PointNum.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPointNum 对话框

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


// CPointNum 消息处理程序


void CPointNum::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str1,str2;
	GetDlgItem(IDC_EDITnum)->GetWindowText(str1);
	GetDlgItem(IDC_distance)->GetWindowText(str2);
	insertnum=atoi(str1);
	distance=atof(str2);
	
	CDialog::OnOK();
}
