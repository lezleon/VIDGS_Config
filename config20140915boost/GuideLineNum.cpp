// GuideLineNum.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "config.h"
#include "GuideLineNum.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CGuideLineNum �Ի���

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


// CGuideLineNum ��Ϣ�������


void CGuideLineNum::OnCbnSelchangeComboguideline()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	long index = m_guideLineNum.GetCurSel();

	m_guideLineNum.GetLBText(index,str_guide);
}


BOOL CGuideLineNum::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_guideLineNum.AddString(_T("1"));
	m_guideLineNum.AddString(_T("2"));
	m_guideLineNum.AddString(_T("3"));
	m_guideLineNum.AddString(_T("4"));
	m_guideLineNum.AddString(_T("5"));
	m_guideLineNum.SetCurSel(0);

	str_guide='1';

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CGuideLineNum::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	guidenum=atoi(str_guide);
	guidenumFlag=TRUE;
	CDialog::OnOK();
}


void CGuideLineNum::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	guidenumFlag=FALSE;
	CDialog::OnCancel();
}
