// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "config.h"

#include "MainFrm.h"
#include "configView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



//CDrawDlgBar
IMPLEMENT_DYNAMIC(CDrawDlgBar, CDialogBar)
CDrawDlgBar::CDrawDlgBar()
{

}

CDrawDlgBar::~CDrawDlgBar()
{
}


BEGIN_MESSAGE_MAP(CDrawDlgBar, CDialogBar)
	ON_WM_PAINT()
	ON_MESSAGE(WM_INITDIALOG, OnInitDialog )
END_MESSAGE_MAP()


// CCmdDlgBar message handlers

void CDrawDlgBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialogBar::OnPaint() for painting messages
}

void CDrawDlgBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);

}

LONG CDrawDlgBar::OnInitDialog ( WPARAM wParam, LPARAM lParam)
{
	LRESULT bRet = HandleInitDialog(wParam, lParam);

	if (!UpdateData(FALSE))
	{
		TRACE0("Warning: UpdateData failed during dialog init.\n");
	}

	CFont* font = GetFont();
	LOGFONT lgfont;
	font->GetLogFont(&lgfont);
	return bRet;

	// m_hMenu1=LoadMenu(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MENU1));
	// ::SetMenu(this->GetSafeHwnd(),m_hMenu1);
}







// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ״̬��ָʾ��
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	// TODO: �ڴ���ӳ�Ա��ʼ������
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	//ModifyStyle(WS_CAPTION,0);//ȡ��������
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	//��Ӳ˵���
	SetMenu(0);
	m_hMenu1=LoadMenu(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MENU1));
	::SetMenu(this->GetSafeHwnd(),m_hMenu1);
	EnableDocking(CBRS_ALIGN_ANY);
	
	m_wndDrawBar.Create (this,(UINT)(IDD_DLG_CheckPanel), WS_CHILD|WS_VISIBLE|CBRS_RIGHT,0);//��ӶԻ���
	
	//ShowWindow(SW_SHOWMAXIMIZED);
	ShowWindow(SW_MAXIMIZE);

	//���ñ�����ͼ��
	HICON m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);  
	SetIcon(m_hIcon,TRUE);  
	SetIcon(m_hIcon,FALSE);  


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);

	return TRUE;
}


// CMainFrame ���

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame ��Ϣ�������



