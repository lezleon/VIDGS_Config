// TypeChoose.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "config.h"
#include "TypeChoose.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CTypeChoose �Ի���
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


// CTypeChoose ��Ϣ�������


void CTypeChoose::OnBnClickedVision()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_ProgType = PROGTYPE_Vision;
	OnOK();
}


void CTypeChoose::OnBnClickedLaser()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_ProgType = PROGTYPE_Laser;
	/*CLaster laster;
	laster.DoModal();*/
	OnOK();
}


BOOL CTypeChoose::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CMenu*   menu   =   this->GetSystemMenu(FALSE);   
	menu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
