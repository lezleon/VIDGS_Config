// DLG_SETTING.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "config.h"
#include "DLG_SETTING.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// DLG_SETTING �Ի���

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

	//CMFCPropertyGridProperty * group1 = new CMFCPropertyGridProperty(_T("����ģ��")); 
	//CMFCPropertyGridProperty * group2 = new CMFCPropertyGridProperty(_T("LEDģ��")); 
	//CMFCPropertyGridProperty * group3 = new CMFCPropertyGridProperty(_T("HMIģ��")); 

	//CString laserPaud,ledPaud,hmiPaud;
	//laserPaud.Format("%d",para.LASERPAUD);
	//ledPaud.Format("%d",para.LEDPAUD);
	//hmiPaud.Format("%d",para.MODBUSPAUD);

	//CMFCPropertyGridProperty * pProp1 = new CMFCPropertyGridProperty( 
	//	_T("COM��"),  
	//	_T(para.LASERCOM.c_str()),  
	//	_T("����д���ӵ������������COM�˿ںţ���COM1��"));
	//CMFCPropertyGridProperty * pProp2 = new CMFCPropertyGridProperty( 
	//	_T("������"),  
	//	laserPaud,  
	//	_T("����д���ӵ������������COM�˿ڲ����ʣ���115200��")); 
	//CMFCPropertyGridProperty * pProp3 = new CMFCPropertyGridProperty( 
	//	_T("COM��"),  
	//	_T(para.LEDCOM.c_str()),  
	//	_T("����д���ӵ�LED��COM�˿ںţ���COM2��"));
	//CMFCPropertyGridProperty * pProp4 = new CMFCPropertyGridProperty( 
	//	_T("������"),  
	//	ledPaud,  
	//	_T("����д���ӵ�LED��COM�˿ڲ����ʣ���57600��")); 
	//CMFCPropertyGridProperty * pProp5 = new CMFCPropertyGridProperty( 
	//	_T("COM��"),  
	//	_T(para.MODBUSCOM.c_str()),  
	//	_T("����д���ӵ���������COM�˿ںţ���COM4��"));
	//CMFCPropertyGridProperty * pProp6 = new CMFCPropertyGridProperty( 
	//	_T("������"),  
	//	hmiPaud,  
	//	_T("����д���ӵ���������COM�˿ڲ����ʣ���115200��")); 


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


// DLG_SETTING ��Ϣ�������


void DLG_SETTING::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
