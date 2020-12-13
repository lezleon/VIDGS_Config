// LineNum.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "config.h"
#include "LineNum.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CLineNum �Ի���

IMPLEMENT_DYNAMIC(CLineNum, CDialog)

CLineNum::CLineNum(CWnd* pParent /*=NULL*/)
	: CDialog(CLineNum::IDD, pParent)
{
	linenumFlag=FALSE;
}

CLineNum::~CLineNum()
{
}

void CLineNum::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_Lineguide, m_lineguidenum);
	DDX_Control(pDX, IDC_COMBO_Linestop, m_linestopnum);
}


BEGIN_MESSAGE_MAP(CLineNum, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_Lineguide, &CLineNum::OnCbnSelchangeComboLineguide)
	ON_CBN_SELCHANGE(IDC_COMBO_Linestop, &CLineNum::OnCbnSelchangeComboLinestop)
	ON_BN_CLICKED(IDOK, &CLineNum::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CLineNum::OnBnClickedCancel)
END_MESSAGE_MAP()


// CLineNum ��Ϣ�������


BOOL CLineNum::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_lineguidenum.AddString(_T("1"));
	m_lineguidenum.AddString(_T("2"));
	m_lineguidenum.AddString(_T("3"));
	m_lineguidenum.AddString(_T("4"));
	m_lineguidenum.AddString(_T("5"));
	m_lineguidenum.SetCurSel(0);

	m_linestopnum.AddString(_T("1"));
	m_linestopnum.AddString(_T("2"));
	m_linestopnum.AddString(_T("3"));
	m_linestopnum.AddString(_T("4"));
	m_linestopnum.AddString(_T("5"));
	m_linestopnum.AddString(_T("6"));
	m_linestopnum.AddString(_T("7"));
	m_linestopnum.AddString(_T("8"));
	m_linestopnum.AddString(_T("9"));
	m_linestopnum.AddString(_T("A"));
	m_linestopnum.AddString(_T("B"));
	m_linestopnum.AddString(_T("C"));
	m_linestopnum.AddString(_T("D"));
	m_linestopnum.AddString(_T("E"));
	m_linestopnum.AddString(_T("F"));
	m_linestopnum.SetCurSel(0);

	str_guidenum='1';
	str_stopnum='1';
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CLineNum::OnCbnSelchangeComboLineguide()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	long index = m_lineguidenum.GetCurSel();
	m_lineguidenum.GetLBText(index,str_guidenum);
}


void CLineNum::OnCbnSelchangeComboLinestop()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	long index = m_linestopnum.GetCurSel();
	m_linestopnum.GetLBText(index,str_stopnum);
}


void CLineNum::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	guidenum=atoi(str_guidenum);
	stopnum=atoi(str_stopnum);
	//GetDlgItem(IDC_LaserY)->GetWindowText(LaserY);
	
	linenumFlag=TRUE;
	CDialog::OnOK();
}


void CLineNum::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	linenumFlag=FALSE;
	CDialog::OnCancel();
}
