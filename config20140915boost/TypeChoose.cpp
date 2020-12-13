// TypeChoose.cpp : 实现文件
//

#include "stdafx.h"
#include "config.h"
#include "TypeChoose.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CTypeChoose 对话框
CTypeChoose g_Type;

IMPLEMENT_DYNAMIC(CTypeChoose, CDialog)

CTypeChoose::CTypeChoose(CWnd* pParent /*=NULL*/)
	: CDialog(CTypeChoose::IDD, pParent)
{
	m_ProgType = PROGTYPE_Vision;

}

CTypeChoose::~CTypeChoose()
{
}

void CTypeChoose::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTypeChoose, CDialog)
	ON_BN_CLICKED(IDC_Vision, &CTypeChoose::OnBnClickedVision)
	ON_BN_CLICKED(IDC_Laser, &CTypeChoose::OnBnClickedLaser)
END_MESSAGE_MAP()


// CTypeChoose 消息处理程序


void CTypeChoose::OnBnClickedVision()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ProgType = PROGTYPE_Vision;
	OnOK();
}


void CTypeChoose::OnBnClickedLaser()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ProgType = PROGTYPE_Laser;
	/*CLaster laster;
	laster.DoModal();*/
	OnOK();
}


BOOL CTypeChoose::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CMenu*   menu   =   this->GetSystemMenu(FALSE);   
	menu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
