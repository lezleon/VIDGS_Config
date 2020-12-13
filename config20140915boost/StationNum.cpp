// StationNum.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "config.h"
#include "StationNum.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CStationNum �Ի���

IMPLEMENT_DYNAMIC(CStationNum, CDialog)

CStationNum::CStationNum(CWnd* pParent /*=NULL*/)
	: CDialog(CStationNum::IDD, pParent)
{
	station=FALSE;

}

CStationNum::~CStationNum()
{
}

void CStationNum::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStationNum, CDialog)
	ON_BN_CLICKED(IDOK, &CStationNum::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CStationNum::OnBnClickedCancel)
END_MESSAGE_MAP()


// CStationNum ��Ϣ�������


void CStationNum::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	station=TRUE;
	CString str1,str2;
	GetDlgItem(IDC_StationNum)->GetWindowText(str1);
	GetDlgItem(IDC_placement)->GetWindowText(str2);

	CString str3,str4,str5,str6;
	GetDlgItem(IDC_stopdistance)->GetWindowText(str3);
	GetDlgItem(IDC_toofardistance)->GetWindowText(str4);
	GetDlgItem(IDC_speed)->GetWindowText(str5);
	GetDlgItem(IDC_PCtime)->GetWindowText(str6);
	StationNum=atoi(str1);
	PlacementNum=atoi(str2);
	stopdistance=atoi(str3);
	toofardistance=atoi(str4);
	speed=atoi(str5);
	pctime=atoi(str6);  


	/*FileStorage fs(".\\test.xml",FileStorage::APPEND);
	if (str1!="")
	{
	fs <<"StationNum"<<StationNum;
	}
	if (str2!="")
	{
	fs <<"PlacementNum"<<PlacementNum;
	}
	if (str3!="")
	{
	fs <<"stopdistance"<<stopdistance;
	}
	if (str4!="")
	{
	fs <<"toofardistance"<<toofardistance;
	}
	if (str5!="")
	{
	fs <<"speed"<<speed;
	}
	if (str6!="")
	{
	fs <<"pctime"<<pctime;
	}*/

	CDialog::OnOK();
}


void CStationNum::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	station=FALSE;
	CDialog::OnCancel();
}
