// PlaneData.cpp : 实现文件
//

#include "stdafx.h"
#include "config.h"
#include "PlaneData.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPlaneData 对话框

IMPLEMENT_DYNAMIC(CPlaneData, CDialog)

CPlaneData::CPlaneData(CWnd* pParent /*=NULL*/)
	: CDialog(CPlaneData::IDD, pParent)
{
	CString   path=AfxGetApp()->m_pszHelpFilePath;   
	CString   str=AfxGetApp()->m_pszExeName;   
	path=path.Left(path.GetLength()-str.GetLength()-4);  
	//routename=".\\test.xml";
	CString Xmlpath;
	Xmlpath.Format("%s%s",path,"PlaneData.xml");
	routename = LPCSTR(Xmlpath);
}

CPlaneData::~CPlaneData()
{
}

void CPlaneData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPlaneData, CDialog)
	ON_BN_CLICKED(IDOK, &CPlaneData::OnBnClickedOk)
END_MESSAGE_MAP()


// CPlaneData 消息处理程序


void CPlaneData::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	CString cabin_width,wing_width,head_height,head_wheel_horidis,engine_centerdis,enginr_radius,center_floor_dis;
	CString numhorizontalcurvefactor,horzatalxmaxfromhead,minaverage,maxaverage;
	CString horCur1,horCur2,horCur3,horCur4,horCur5,horCur6,horCur7,horCur8,horCur9,horCur10;

	CString planetype;
	GetDlgItem(IDC_type)->GetWindowText(planetype);
	
	planeinfo.plane_Type=LPCSTR(planetype);

	GetDlgItem(IDC_Cabin_Width)->GetWindowText(cabin_width);
	GetDlgItem(IDC_Wing_Width)->GetWindowText(wing_width);
	GetDlgItem(IDC_Head_Height)->GetWindowText(head_height);
	GetDlgItem(IDC_Head_Wheel_HoriDis)->GetWindowText(head_wheel_horidis);
	GetDlgItem(IDC_Engine_Centerdis)->GetWindowText(engine_centerdis);
	GetDlgItem(IDC_Engine_Radius)->GetWindowText(enginr_radius);
	GetDlgItem(IDC_Center_floor_Dis)->GetWindowText(center_floor_dis);
	planeinfo.cabin_Width=atof(cabin_width);
	planeinfo.wing_Width=atof(wing_width);
	planeinfo.head_Height=atof(head_height);
	planeinfo.head_wheel_HoriDis=atof(head_wheel_horidis);
	planeinfo.engine_Centerdis=atof(engine_centerdis);
	planeinfo.engine_Radius=atof(enginr_radius);
	planeinfo.center_floor_Dis=atof(center_floor_dis);

	GetDlgItem(IDC_NumHorizontalCurveFactor)->GetWindowText(numhorizontalcurvefactor);
	GetDlgItem(IDC_NorzatalXmaxfromHead)->GetWindowText(horzatalxmaxfromhead);
	GetDlgItem(IDC_minAverage)->GetWindowText(minaverage);
	GetDlgItem(IDC_maxAverage)->GetWindowText(maxaverage);
	planeinfo.numHorizontalCurveFactor=atoi(numhorizontalcurvefactor);
	planeinfo.horizontalXMaxFromHead=atoi(horzatalxmaxfromhead);
	planeinfo.minAverage=atoi(minaverage);
	planeinfo.maxAverage=atoi(maxaverage);

	GetDlgItem(IDC_NorizontalCurveFactor1)->GetWindowText(horCur1);
	GetDlgItem(IDC_NorizontalCurveFactor2)->GetWindowText(horCur2);
	GetDlgItem(IDC_NorizontalCurveFactor3)->GetWindowText(horCur3);
	GetDlgItem(IDC_NorizontalCurveFactor4)->GetWindowText(horCur4);
	GetDlgItem(IDC_NorizontalCurveFactor5)->GetWindowText(horCur5);
	GetDlgItem(IDC_NorizontalCurveFactor6)->GetWindowText(horCur6);
	GetDlgItem(IDC_NorizontalCurveFactor7)->GetWindowText(horCur7);
	GetDlgItem(IDC_NorizontalCurveFactor8)->GetWindowText(horCur8);
	GetDlgItem(IDC_NorizontalCurveFactor9)->GetWindowText(horCur9);
	GetDlgItem(IDC_NorizontalCurveFactor10)->GetWindowText(horCur10);

	planeinfo.horizontalCurveFactor.push_back(atof(horCur1));
	planeinfo.horizontalCurveFactor.push_back(atof(horCur2));
	planeinfo.horizontalCurveFactor.push_back(atof(horCur3));
	planeinfo.horizontalCurveFactor.push_back(atof(horCur4));
	planeinfo.horizontalCurveFactor.push_back(atof(horCur5));
	planeinfo.horizontalCurveFactor.push_back(atof(horCur6));
	planeinfo.horizontalCurveFactor.push_back(atof(horCur7));
	planeinfo.horizontalCurveFactor.push_back(atof(horCur8));
	planeinfo.horizontalCurveFactor.push_back(atof(horCur9));
	planeinfo.horizontalCurveFactor.push_back(atof(horCur10));
	
	
	Doc.writedata(routename,planeinfo.plane_Type,planeinfo);



	CDialog::OnOK();
}
