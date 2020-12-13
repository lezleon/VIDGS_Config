// GuideLineNum.cpp : 实现文件
//

#include "stdafx.h"
#include "config.h"
#include "GuideLineNum.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CGuideLineNum 对话框

IMPLEMENT_DYNAMIC(CGuideLineNum, CDialog)

CGuideLineNum::CGuideLineNum(CWnd* pParent /*=NULL*/)
	: CDialog(CGuideLineNum::IDD, pParent)
{
	guidenumFlag=FALSE;

}

CGuideLineNum::~CGuideLineNum()
{
}

void CGuideLineNum::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBOGuideLine, m_guideLineNum);
}


BEGIN_MESSAGE_MAP(CGuideLineNum, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBOGuideLine, &CGuideLineNum::OnCbnSelchangeComboguideline)
	ON_BN_CLICKED(IDOK, &CGuideLineNum::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CGuideLineNum::OnBnClickedCancel)
END_MESSAGE_MAP()


// CGuideLineNum 消息处理程序


void CGuideLineNum::OnCbnSelchangeComboguideline()
{
	// TODO: 在此添加控件通知处理程序代码
	long index = m_guideLineNum.GetCurSel();

	m_guideLineNum.GetLBText(index,str_guide);
}


BOOL CGuideLineNum::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_guideLineNum.AddString(_T("1"));
	m_guideLineNum.AddString(_T("2"));
	m_guideLineNum.AddString(_T("3"));
	m_guideLineNum.AddString(_T("4"));
	m_guideLineNum.AddString(_T("5"));
	m_guideLineNum.SetCurSel(0);

	str_guide='1';

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CGuideLineNum::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	guidenum=atoi(str_guide);
	guidenumFlag=TRUE;
	CDialog::OnOK();
}


void CGuideLineNum::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	guidenumFlag=FALSE;
	CDialog::OnCancel();
}
