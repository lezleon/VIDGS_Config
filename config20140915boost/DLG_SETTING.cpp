// DLG_SETTING.cpp : 实现文件
//

#include "stdafx.h"
#include "config.h"
#include "DLG_SETTING.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// DLG_SETTING 对话框

IMPLEMENT_DYNAMIC(DLG_SETTING, CDialogEx)

DLG_SETTING::DLG_SETTING(CWnd* pParent /*=NULL*/)
	: CDialogEx(DLG_SETTING::IDD, pParent)
{
	CString   path=AfxGetApp()->m_pszHelpFilePath;   
	CString   str=AfxGetApp()->m_pszExeName;   
	path=path.Left(path.GetLength()-str.GetLength()-4);
	CString Xmlpath;
	Xmlpath.Format("%s%s",path,"test.xml");
	routename = LPCSTR(Xmlpath);
}

BOOL DLG_SETTING::OnInitDialog()
{
	CDialog::OnInitDialog();
	HDITEM item; 
	item.cxy=100; 
	item.mask=HDI_WIDTH; 
	/*m_propertyGrid_COM.GetHeaderCtrl().SetItem(0, new HDITEM(item)); 
	m_propertyGrid_COM.GetHeaderCtrl().SetItem(0, new HDITEM(item)); */

	//vision_machine_parameters para=Doc.readvisiondatas(routename);

	//CMFCPropertyGridProperty * group1 = new CMFCPropertyGridProperty(_T("激光模块")); 
	//CMFCPropertyGridProperty * group2 = new CMFCPropertyGridProperty(_T("LED模块")); 
	//CMFCPropertyGridProperty * group3 = new CMFCPropertyGridProperty(_T("HMI模块")); 

	//CString laserPaud,ledPaud,hmiPaud;
	//laserPaud.Format("%d",para.LASERPAUD);
	//ledPaud.Format("%d",para.LEDPAUD);
	//hmiPaud.Format("%d",para.MODBUSPAUD);

	//CMFCPropertyGridProperty * pProp1 = new CMFCPropertyGridProperty( 
	//	_T("COM口"),  
	//	_T(para.LASERCOM.c_str()),  
	//	_T("请填写连接到激光控制器的COM端口号，如COM1。"));
	//CMFCPropertyGridProperty * pProp2 = new CMFCPropertyGridProperty( 
	//	_T("波特率"),  
	//	laserPaud,  
	//	_T("请填写连接到激光控制器的COM端口波特率，如115200。")); 
	//CMFCPropertyGridProperty * pProp3 = new CMFCPropertyGridProperty( 
	//	_T("COM口"),  
	//	_T(para.LEDCOM.c_str()),  
	//	_T("请填写连接到LED的COM端口号，如COM2。"));
	//CMFCPropertyGridProperty * pProp4 = new CMFCPropertyGridProperty( 
	//	_T("波特率"),  
	//	ledPaud,  
	//	_T("请填写连接到LED的COM端口波特率，如57600。")); 
	//CMFCPropertyGridProperty * pProp5 = new CMFCPropertyGridProperty( 
	//	_T("COM口"),  
	//	_T(para.MODBUSCOM.c_str()),  
	//	_T("请填写连接到触摸屏的COM端口号，如COM4。"));
	//CMFCPropertyGridProperty * pProp6 = new CMFCPropertyGridProperty( 
	//	_T("波特率"),  
	//	hmiPaud,  
	//	_T("请填写连接到触摸屏的COM端口波特率，如115200。")); 


	//group1->AddSubItem(pProp1); 
	//group1->AddSubItem(pProp2); 
	//group2->AddSubItem(pProp3); 
	//group2->AddSubItem(pProp4); 
	//group3->AddSubItem(pProp5); 
	//group3->AddSubItem(pProp6);

	//m_propertyGrid_COM.AddProperty(group1); 
	//m_propertyGrid_COM.AddProperty(group2);
	//m_propertyGrid_COM.AddProperty(group3); 

	return true;
}

DLG_SETTING::~DLG_SETTING()
{
}

void DLG_SETTING::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_MFCPROPERTYGRID1, m_COM_SETTING);
	//DDX_Control(pDX, IDC_MFCPROPERTYGRID2, m_para_setting);
	DDX_Control(pDX, IDC_MFCPROPERTYGRID3, m_propertyGrid_COM);
	DDX_Control(pDX, IDC_MFCPROPERTYGRID4, m_propertyGrid_Para);
}


BEGIN_MESSAGE_MAP(DLG_SETTING, CDialogEx)
	ON_BN_CLICKED(IDOK, &DLG_SETTING::OnBnClickedOk)
END_MESSAGE_MAP()


// DLG_SETTING 消息处理程序


void DLG_SETTING::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
	system_parameters para=Doc.readsystemdatas(routename);
	para.LASERCOM = (CString)m_propertyGrid_COM.GetProperty(0)->GetSubItem(0)->GetValue();
	para.LASERPAUD = atoi((CString)m_propertyGrid_COM.GetProperty(0)->GetSubItem(1)->GetValue());
	para.LEDCOM = (CString)m_propertyGrid_COM.GetProperty(1)->GetSubItem(0)->GetValue();
	para.LEDPAUD = atoi((CString)m_propertyGrid_COM.GetProperty(1)->GetSubItem(1)->GetValue());
	para.MODBUSCOM = (CString)m_propertyGrid_COM.GetProperty(2)->GetSubItem(0)->GetValue();
	para.MODBUSPAUD = atoi((CString)m_propertyGrid_COM.GetProperty(2)->GetSubItem(1)->GetValue());
	Doc.writedata(routename,para);
}
