// ChoosePoint.cpp : 实现文件
//

#include "stdafx.h"
#include "config.h"
#include "ChoosePoint.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChoosePoint 对话框

IMPLEMENT_DYNAMIC(CChoosePoint, CDialog)

CChoosePoint::CChoosePoint(CWnd* pParent /*=NULL*/)
	: CDialog(CChoosePoint::IDD, pParent)
{
	Pointflag=0;
}

CChoosePoint::~CChoosePoint()
{
}

void CChoosePoint::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CChoosePoint, CDialog)
	ON_BN_CLICKED(ID_PointInsert, &CChoosePoint::OnBnClickedPointinsert)
	ON_BN_CLICKED(ID_PointRenew, &CChoosePoint::OnBnClickedPointrenew)
END_MESSAGE_MAP()


// CChoosePoint 消息处理程序


void CChoosePoint::OnBnClickedPointinsert()
{
	// TODO: 在此添加控件通知处理程序代码
	Pointflag=1;
	CDialog::OnOK();
}


void CChoosePoint::OnBnClickedPointrenew()
{
	// TODO: 在此添加控件通知处理程序代码
	Pointflag=0;
	CDialog::OnOK();
}
