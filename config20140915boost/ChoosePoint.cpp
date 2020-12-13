// ChoosePoint.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "config.h"
#include "ChoosePoint.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChoosePoint �Ի���

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


// CChoosePoint ��Ϣ�������


void CChoosePoint::OnBnClickedPointinsert()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	Pointflag=1;
	CDialog::OnOK();
}


void CChoosePoint::OnBnClickedPointrenew()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	Pointflag=0;
	CDialog::OnOK();
}
