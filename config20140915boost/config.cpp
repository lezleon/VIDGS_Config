// config.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "config.h"
#include "MainFrm.h"

#include "configDoc.h"
#include "configView.h"
#include "TypeChoose.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CconfigApp

BEGIN_MESSAGE_MAP(CconfigApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CconfigApp::OnAppAbout)
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// ��׼��ӡ��������
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CconfigApp ����

CconfigApp::CconfigApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CconfigApp ����

CconfigApp theApp;

//testSVN
// CconfigApp ��ʼ��

BOOL CconfigApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	LoadStdProfileSettings(4);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)

	//g_Type.DoModal();
	// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	//if (g_Type.m_ProgType==PROGTYPE_Vision)
	//{
		CSingleDocTemplate* pDocTemplate;
		pDocTemplate = new CSingleDocTemplate(
			IDR_MAINFRAME,
			RUNTIME_CLASS(CconfigDoc),
			RUNTIME_CLASS(CMainFrame),       // �� SDI ��ܴ���
			RUNTIME_CLASS(CconfigView));
		if (!pDocTemplate)
			return FALSE;
		AddDocTemplate(pDocTemplate);



		// ������׼������DDE�����ļ�������������
		CCommandLineInfo cmdInfo;
		ParseCommandLine(cmdInfo);


		// ��������������ָ����������
		// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
		if (!ProcessShellCommand(cmdInfo))
			return FALSE;

		// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
		m_pMainWnd->ShowWindow(SW_SHOW);
		m_pMainWnd->UpdateWindow();
		// �������к�׺ʱ�ŵ��� DragAcceptFiles
		//  �� SDI Ӧ�ó����У���Ӧ�� ProcessShellCommand ֮����
		return TRUE;
	//}
	//else
	//{
	//	g_laster.DoModal();
	//	return TRUE;
	//}
}



// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// �������жԻ����Ӧ�ó�������
void CconfigApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CconfigApp ��Ϣ�������

