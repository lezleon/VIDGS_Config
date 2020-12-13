// MainFrm.cpp : CMainFrame 类的实现
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
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	//ModifyStyle(WS_CAPTION,0);//取消标题栏
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	//添加菜单栏
	SetMenu(0);
	m_hMenu1=LoadMenu(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MENU1));
	::SetMenu(this->GetSafeHwnd(),m_hMenu1);
	EnableDocking(CBRS_ALIGN_ANY);
	
	m_wndDrawBar.Create (this,(UINT)(IDD_DLG_CheckPanel), WS_CHILD|WS_VISIBLE|CBRS_RIGHT,0);//添加对话框
	
	//ShowWindow(SW_SHOWMAXIMIZED);
	ShowWindow(SW_MAXIMIZE);

	//设置标题栏图标
	HICON m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);  
	SetIcon(m_hIcon,TRUE);  
	SetIcon(m_hIcon,FALSE);  


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);

	return TRUE;
}


// CMainFrame 诊断

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


// CMainFrame 消息处理程序



