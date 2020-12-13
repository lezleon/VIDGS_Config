// PlaneType.cpp : 实现文件
//

#include "stdafx.h"
#include "config.h"
#include "PlaneType.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPlaneType 对话框

IMPLEMENT_DYNAMIC(CPlaneType, CDialog)

CPlaneType::CPlaneType(CWnd* pParent /*=NULL*/)
	: CDialog(CPlaneType::IDD, pParent)
{
	planeType=FALSE;
	CString   path=AfxGetApp()->m_pszHelpFilePath;   
	CString   str=AfxGetApp()->m_pszExeName;   
	path=path.Left(path.GetLength()-str.GetLength()-4);  
	//routename=".\\test.xml";
	CString Xmlpath;
	Xmlpath.Format("%s%s",path,"test.xml");
	routename = LPCSTR(Xmlpath);

}

CPlaneType::~CPlaneType()
{
}

void CPlaneType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_comboChoose);
	DDX_Control(pDX, IDC_COMBO2, m_comboChoose2);
}


BEGIN_MESSAGE_MAP(CPlaneType, CDialog)
	ON_BN_CLICKED(IDOK, &CPlaneType::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CPlaneType::OnCbnSelchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CPlaneType::OnCbnSelchangeCombo2)
	ON_BN_CLICKED(IDCANCEL, &CPlaneType::OnBnClickedCancel)
END_MESSAGE_MAP()


// CPlaneType 消息处理程序


BOOL CPlaneType::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	m_comboChoose.AddString(_T("1"));
	m_comboChoose.AddString(_T("2"));
	m_comboChoose.AddString(_T("3"));
	m_comboChoose.AddString(_T("4"));
	m_comboChoose.AddString(_T("5"));
	
	m_comboChoose.SetCurSel(0);
	m_comboChoose2.AddString(_T("1"));
	m_comboChoose2.AddString(_T("2"));
	m_comboChoose2.AddString(_T("3"));
	m_comboChoose2.AddString(_T("4"));
	m_comboChoose2.AddString(_T("5"));
	m_comboChoose2.AddString(_T("6"));
	m_comboChoose2.AddString(_T("7"));
	m_comboChoose2.AddString(_T("8"));
	m_comboChoose2.AddString(_T("9"));
	m_comboChoose2.AddString(_T("A"));
	m_comboChoose2.AddString(_T("B"));
	m_comboChoose2.AddString(_T("C"));
	m_comboChoose2.AddString(_T("D"));
	m_comboChoose2.AddString(_T("E"));
	m_comboChoose2.AddString(_T("F"));
	m_comboChoose2.SetCurSel(0);
	str_guide='1';
	str_stop='1';

	CString str;
	str.Format("%s%s%s","PlaneType",str_guide,str_stop);
	String Str=LPCSTR(str);
	vector<string>vplanetypes = Doc.read_Vplanetypes(routename,Str);
	int N = vplanetypes.size();
	CString type[16];
	for (int i=0;i<N;i++)
	{
		type[i]=vplanetypes.at(i).c_str();
	}
	for (int i=N;i<15;i++)
	{
		type[i]="";
	}
	GetDlgItem(IDC_type1)->SetWindowText(type[0]);
	GetDlgItem(IDC_type2)->SetWindowText(type[1]);
	GetDlgItem(IDC_type3)->SetWindowText(type[2]);
	GetDlgItem(IDC_type4)->SetWindowText(type[3]);
	GetDlgItem(IDC_type5)->SetWindowText(type[4]);
	GetDlgItem(IDC_type6)->SetWindowText(type[5]);
	GetDlgItem(IDC_type7)->SetWindowText(type[6]);
	GetDlgItem(IDC_type8)->SetWindowText(type[7]);
	GetDlgItem(IDC_type9)->SetWindowText(type[8]);
	GetDlgItem(IDC_type10)->SetWindowText(type[9]);
	GetDlgItem(IDC_type11)->SetWindowText(type[10]);
	GetDlgItem(IDC_type12)->SetWindowText(type[11]);
	GetDlgItem(IDC_type13)->SetWindowText(type[12]);
	GetDlgItem(IDC_type14)->SetWindowText(type[13]);
	GetDlgItem(IDC_type15)->SetWindowText(type[14]);
	GetDlgItem(IDC_type16)->SetWindowText(type[15]);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CPlaneType::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	planeType=TRUE;
	CString str1,str2,str3,str4;
	CString str5,str6,str7,str8;
	CString str9,str10,str11,str12;
	CString str13,str14,str15,str16;
	GetDlgItem(IDC_type1)->GetWindowText(str1);
	GetDlgItem(IDC_type2)->GetWindowText(str2);
	GetDlgItem(IDC_type3)->GetWindowText(str3);
	GetDlgItem(IDC_type4)->GetWindowText(str4);
	GetDlgItem(IDC_type5)->GetWindowText(str5);
	GetDlgItem(IDC_type6)->GetWindowText(str6);
	GetDlgItem(IDC_type7)->GetWindowText(str7);
	GetDlgItem(IDC_type8)->GetWindowText(str8);
	GetDlgItem(IDC_type9)->GetWindowText(str9);
	GetDlgItem(IDC_type10)->GetWindowText(str10);
	GetDlgItem(IDC_type11)->GetWindowText(str11);
	GetDlgItem(IDC_type12)->GetWindowText(str12);
	GetDlgItem(IDC_type13)->GetWindowText(str13);
	GetDlgItem(IDC_type14)->GetWindowText(str14);
	GetDlgItem(IDC_type15)->GetWindowText(str15);
	GetDlgItem(IDC_type16)->GetWindowText(str16);
	//LPCSTR(str1);
	stopNum=atoi(str_stop);
	guidenum=atoi(str_guide);
	//FileStorage fs(".\\test.xml",FileStorage::APPEND);
	///*Mat plane_mat = (Mat_<string>(18,1) << str_guide,str_stop,str1,str2,str3,str4,str5,
	//	str6,str7,str8,str9,str10,str11,str12,str13,str14,str15,str16);
	//fs << "PlaneType" <<plane_mat;*/

	//fs<<"guidenum"<<guidenum;
	//fs<<"stopNum"<<stopNum;
	//if (str1!="")
	//{
	//	fs<<"num1"<<LPCSTR(str1);
	//}
	//if (str2!="")
	//{
	//	fs<<"num2"<<LPCSTR(str2);
	//}
	//if (str3!="")
	//{
	//	fs<<"num3"<<LPCSTR(str3);
	//}
	//if (str4!="")
	//{
	//	fs<<"num4"<<LPCSTR(str4);
	//}
	//if (str5!="")
	//{
	//	fs<<"num5"<<LPCSTR(str5);
	//}
	//if (str6!="")
	//{
	//	fs<<"num6"<<LPCSTR(str6);
	//}
	//if (str7!="")
	//{
	//	fs<<"num7"<<LPCSTR(str7);
	//}
	//if (str8!="")
	//{
	//	fs<<"num8"<<LPCSTR(str8);
	//}
	//if (str9!="")
	//{
	//	fs<<"num9"<<LPCSTR(str9);
	//}
	//if (str10!="")
	//{
	//	fs<<"num10"<<LPCSTR(str10);
	//}
	//if (str11!="")
	//{
	//	fs<<"num11"<<LPCSTR(str11);
	//}
	//if (str12!="")
	//{
	//	fs<<"num12"<<LPCSTR(str12);
	//}
	//if (str13!="")
	//{
	//	fs<<"num13"<<LPCSTR(str13);
	//}
	//if (str14!="")
	//{
	//	fs<<"num14"<<LPCSTR(str14);
	//}
	//if (str15!="")
	//{
	//	fs<<"num15"<<LPCSTR(str15);
	//}
	//if (str16!="")
	//{
	//	fs<<"num16"<<LPCSTR(str16);
	//}

	
	CString g_guide=_T("guide")+str_guide;
	CString g_stop=_T("stop")+str_stop;

	VS.push_back(LPCSTR(g_guide));
	VS.push_back(LPCSTR(g_stop));
	if (str1!="")
	{
		VS.push_back(LPCSTR(str1));
	}
	if (str2!="")
	{
		VS.push_back(LPCSTR(str2));
	}
	if (str3!="")
	{
		VS.push_back(LPCSTR(str3));
	}
	if (str4!="")
	{
		VS.push_back(LPCSTR(str4));
	}
	if (str5!="")
	{
		VS.push_back(LPCSTR(str5));
	}
	if (str6!="")
	{
		VS.push_back(LPCSTR(str6));
	}
	if (str7!="")
	{
		VS.push_back(LPCSTR(str7));
	}
	if (str8!="")
	{
		VS.push_back(LPCSTR(str8));
	}
	if (str9!="")
	{
		VS.push_back(LPCSTR(str9));
	}
	if (str10!="")
	{
		VS.push_back(LPCSTR(str10));
	}
	if (str11!="")
	{
		VS.push_back(LPCSTR(str11));
	}
	if (str12!="")
	{
		VS.push_back(LPCSTR(str12));
	}
	if (str13!="")
	{
		VS.push_back(LPCSTR(str13));
	}
	if (str14!="")
	{
		VS.push_back(LPCSTR(str14));
	}
	if (str15!="")
	{
		VS.push_back(LPCSTR(str15));
	}
	if (str16!="")
	{
		VS.push_back(LPCSTR(str16));
	}

	CDialog::OnOK();
}


void CPlaneType::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	long index = m_comboChoose.GetCurSel();

	m_comboChoose.GetLBText(index,str_guide);

	CString str;
	str.Format("%s%s%s","PlaneType",str_guide,str_stop);
	String Str=LPCSTR(str);
	vector<string>vplanetypes = Doc.read_Vplanetypes(routename,Str);
	int N = vplanetypes.size();
	CString type[16];
	for (int i=0;i<N;i++)
	{
		type[i]=vplanetypes.at(i).c_str();
	}
	for (int i=N;i<15;i++)
	{
		type[i]="";
	}
	
	GetDlgItem(IDC_type1)->SetWindowText(type[0]);
	GetDlgItem(IDC_type2)->SetWindowText(type[1]);
	GetDlgItem(IDC_type3)->SetWindowText(type[2]);
	GetDlgItem(IDC_type4)->SetWindowText(type[3]);
	GetDlgItem(IDC_type5)->SetWindowText(type[4]);
	GetDlgItem(IDC_type6)->SetWindowText(type[5]);
	GetDlgItem(IDC_type7)->SetWindowText(type[6]);
	GetDlgItem(IDC_type8)->SetWindowText(type[7]);
	GetDlgItem(IDC_type9)->SetWindowText(type[8]);
	GetDlgItem(IDC_type10)->SetWindowText(type[9]);
	GetDlgItem(IDC_type11)->SetWindowText(type[10]);
	GetDlgItem(IDC_type12)->SetWindowText(type[11]);
	GetDlgItem(IDC_type13)->SetWindowText(type[12]);
	GetDlgItem(IDC_type14)->SetWindowText(type[13]);
	GetDlgItem(IDC_type15)->SetWindowText(type[14]);
	GetDlgItem(IDC_type16)->SetWindowText(type[15]);
	
}
void CPlaneType::OnCbnSelchangeCombo2()
{
	// TODO: 在此添加控件通知处理程序代码
	long index = m_comboChoose2.GetCurSel();

	m_comboChoose2.GetLBText(index,str_stop);

	CString str;
	str.Format("%s%s%s","PlaneType",str_guide,str_stop);
	String Str=LPCSTR(str);
	vector<string>vplanetypes = Doc.read_Vplanetypes(routename,Str);
	int N = vplanetypes.size();
	CString type[16];
	if (N>0)
	{
		for (int i=0;i<N;i++)
		{
			type[i]=vplanetypes.at(i).c_str();
		}
		for (int i=N;i<15;i++)
		{
			type[i]="";
		}


	}
	else
	{
		for (int i=0;i<16;i++)
		{
			type[i]="";
		}
	}
	
	GetDlgItem(IDC_type1)->SetWindowText(type[0]);
	GetDlgItem(IDC_type2)->SetWindowText(type[1]);
	GetDlgItem(IDC_type3)->SetWindowText(type[2]);
	GetDlgItem(IDC_type4)->SetWindowText(type[3]);
	GetDlgItem(IDC_type5)->SetWindowText(type[4]);
	GetDlgItem(IDC_type6)->SetWindowText(type[5]);
	GetDlgItem(IDC_type7)->SetWindowText(type[6]);
	GetDlgItem(IDC_type8)->SetWindowText(type[7]);
	GetDlgItem(IDC_type9)->SetWindowText(type[8]);
	GetDlgItem(IDC_type10)->SetWindowText(type[9]);
	GetDlgItem(IDC_type11)->SetWindowText(type[10]);
	GetDlgItem(IDC_type12)->SetWindowText(type[11]);
	GetDlgItem(IDC_type13)->SetWindowText(type[12]);
	GetDlgItem(IDC_type14)->SetWindowText(type[13]);
	GetDlgItem(IDC_type15)->SetWindowText(type[14]);
	GetDlgItem(IDC_type16)->SetWindowText(type[15]);
	
}


void CPlaneType::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	planeType=FALSE;
	CDialog::OnCancel();
}
