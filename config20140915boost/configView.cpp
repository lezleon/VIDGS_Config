// configView.cpp : CconfigView 类的实现
//

#include "stdafx.h"
#include "config.h"

#include "configDoc.h"
#include "configView.h"
#include "CXMemDC.h"
#include "Paramini.h"

#include "cv.h"
#include "highgui.h"
#include "MainFrm.h"
#include "boost/shared_ptr.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CconfigView

IMPLEMENT_DYNCREATE(CconfigView, CScrollView)

BEGIN_MESSAGE_MAP(CconfigView, CScrollView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, & CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, & CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, & CScrollView::OnFilePrintPreview)
	
	ON_BN_CLICKED(IDC_BTN_OPENIMAGE, &CconfigView::OnBnClickedBtnOpenimage)
	ON_BN_CLICKED(IDC_BTN_EXIT, &CconfigView::OnBnClickedBtnExit)
	ON_BN_CLICKED(IDC_BTN_DrawSelRect, &CconfigView::OnBnClickedBtnDrawselrect)
	ON_UPDATE_COMMAND_UI(IDC_BTN_DrawSelRect, OnUpdateDrawRectBtn)
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_BN_CLICKED(IDC_BTN_DelSelRect, &CconfigView::OnBnClickedBtnDelselrect)
	ON_UPDATE_COMMAND_UI(IDC_BTN_DelSelRect,OnUpdateDelRectBtn)
	ON_BN_CLICKED(IDC_CaptureRect, &CconfigView::OnBnClickedCapturerect)
//	ON_WM_LBUTTONUP()
ON_WM_LBUTTONUP()
ON_BN_CLICKED(IDC_DrawLine, &CconfigView::OnBnClickedDrawline)
//ON_WM_MOUSEHOVER()
//ON_WM_MOUSEMOVE()
ON_BN_CLICKED(IDC_LineConfirm, &CconfigView::OnBnClickedLineconfirm)
ON_BN_CLICKED(IDC_Track, &CconfigView::OnBnClickedTrack)
ON_BN_CLICKED(IDC_StopLine, &CconfigView::OnBnClickedStopline)
ON_BN_CLICKED(IDC_Bridge, &CconfigView::OnBnClickedBridge)
ON_BN_CLICKED(IDC_DelLine, &CconfigView::OnBnClickedDelline)
ON_UPDATE_COMMAND_UI(IDC_DelLine,OnUpdateDelLineBtn)

ON_BN_CLICKED(IDC_PlaneType, &CconfigView::OnBnClickedPlanetype)
ON_WM_ERASEBKGND()
ON_BN_CLICKED(IDC_DrawPoint, &CconfigView::OnBnClickedDrawpoint)
ON_BN_CLICKED(IDC_DelPoint, &CconfigView::OnBnClickedDelpoint)
ON_UPDATE_COMMAND_UI(IDC_DelPoint, OnUpdateDelPointBtn)
ON_BN_CLICKED(IDC_PointConfim, &CconfigView::OnBnClickedPointconfim)
ON_BN_CLICKED(IDC_Enlarge, &CconfigView::OnBnClickedEnlarge)
ON_BN_CLICKED(IDC_Shrink, &CconfigView::OnBnClickedShrink)
ON_COMMAND(ID_StartVideo, &CconfigView::OnStartvideo)
ON_COMMAND(ID_StopVideo, &CconfigView::OnStopvideo)
ON_COMMAND(ID_ExitVideo, &CconfigView::OnExitvideo)
ON_COMMAND(ID_ExitImg, &CconfigView::OnExitimg)
ON_COMMAND(ID_OpenImg, &CconfigView::OnOpenimg)
ON_COMMAND(ID_station, &CconfigView::Onstation)
ON_COMMAND(ID_Laster, &CconfigView::OnLaster)
ON_WM_MOUSEMOVE()
ON_COMMAND(ID_LastConfig, &CconfigView::OnLastconfig)
ON_COMMAND(ID_ReConfig, &CconfigView::OnReconfig)
ON_COMMAND(ID_SaveImage, &CconfigView::OnSaveimage)
ON_COMMAND(ID_PlaneData, &CconfigView::OnPlanedata)
ON_COMMAND(ID_32798, &CconfigView::OnCOMSetting)
END_MESSAGE_MAP()

// CconfigView 构造/析构

CconfigView::CconfigView()
: m_ptOrigin(0)
, m_CameraMode(0)
{
	// TODO: 在此处添加构造代码
	m_pBmpInfo_out = new BITMAPINFO;
	m_pBits_out = new BYTE[443686400];

	static_image = NULL;
	static_image_one=NULL;
	SetScrollSizes(MM_TEXT, CSize(0, 0));
	g_ParamIni.Init(_T("\\1-1.ini"));
	PvInitialize();
	memset(&m_Camera,0,sizeof(tCamera));

	m_ptOrigin=0;
	m_start=0;
	m_end=0;
	m_bDraw=FALSE;
	m_DrawPoint=FALSE;
	m_DrawPoint1=FALSE;
	count=0;
	m_zoom = 0.5;

	m_ExitImg=FALSE;
	m_CameraMode = 0;
	
	CString   path=AfxGetApp()->m_pszHelpFilePath;   
	CString   str=AfxGetApp()->m_pszExeName;   
	path=path.Left(path.GetLength()-str.GetLength()-4);  
	//routename=".\\test.xml";
	CString Xmlpath;
	Xmlpath.Format("%s%s",path,"test.xml");
	routename = LPCSTR(Xmlpath);

	buttonup=FALSE;
	m_Imagesave=FALSE;

}

CconfigView::~CconfigView()
{
	if (m_pBmpInfo_out)
	{
		delete m_pBmpInfo_out;
	}
	if (m_pBits_out)
	{
		delete [] m_pBits_out;
	}
	if(static_image)
	{
		cvReleaseImage(&static_image);
	}
	if(static_image_one)
	{
		cvReleaseImage(&static_image_one);
	}
	long i;
	for(i=0;i<m_TrackerArray.GetCount();i++)
	{
		CRectTracker* pTracker = m_TrackerArray[i];
		if(pTracker)
			delete pTracker;
	}
}

BOOL CconfigView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	if(!CScrollView::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;	
	return TRUE;
}

void CconfigView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	/*BITMAPINFO* pBmpInfo = NULL;
	void * pBits = NULL;

	pBmpInfo = m_pBmpInfo_out;
	pBits = m_pBits_out;*/

	CSize sizeScroll(800,600);
	SetScrollSizes(MM_TEXT,sizeScroll);
}
// CconfigView 绘制

void CconfigView::OnDraw(CDC* pDC)
{


	CXMemDC dc(pDC);

	CRect rcClient;
	GetClientRect(rcClient);

	dc->FillSolidRect(rcClient,RGB(40,40,40));
	////
	//CconfigDoc* pDoc = GetDocument();
	//ASSERT_VALID(pDoc);
	//if (!pDoc)
	//	return;

	if(m_CameraMode == 0)
	{
		BITMAPINFO* pBmpInfo = NULL;
		void * pBits = NULL;

		if (!m_ExitImg)
		{
			pBmpInfo = m_pBmpInfo_out;
			pBits = m_pBits_out;
		

			dc.SetStretchBltMode(COLORONCOLOR);

			CSize sizeImg(pBmpInfo->bmiHeader.biWidth, ::abs( pBmpInfo->bmiHeader.biHeight ));
			CPoint viewportorg(0,0);

			if(rcClient.right<=(long)(GetZoom()*sizeImg.cx))
				viewportorg.x=0;
			else
				viewportorg.x=(long)((rcClient.right-GetZoom()*sizeImg.cx)/2);

			if(rcClient.bottom<=(long)(GetZoom()*sizeImg.cy))
				viewportorg.y=0;
			else
				viewportorg.y=(long)((rcClient.bottom - GetZoom()*sizeImg.cy)/2);

			///////////////////////////////////////////////////////
			int nLines = StretchDIBits( 
				dc.GetSafeHdc(),// Handle to the device
				viewportorg.x, 
				viewportorg.y,
				GetZoom()*sizeImg.cx,	// Dest. rectangle width.
				GetZoom()*sizeImg.cy,	// Dest. rectangle height.
				0,				// X-coordinate of lower-left corner of the source rect.
				0,				// Y-coordinate of lower-left corner of the source rect.
				pBmpInfo->bmiHeader.biWidth,	// Source rectangle width.
				::abs( pBmpInfo->bmiHeader.biHeight ), // Number of scan lines.
				pBits,	// Modified address of array with DIB bits.
				pBmpInfo,	// Address of structure with bitmap info.
				DIB_RGB_COLORS,	// RGB or palette indices.
				SRCCOPY
				);
		}
	}
	else
	{
		BITMAPINFO* pBmpInfo = NULL;
		void * pBits = NULL;

		
	//BmpInfo=camoperate.info;
		
	//Bits=camoperate.image_one.data;
		pBmpInfo = m_pBmpInfo_out;
		pBits = m_pBits_out;
		
		dc.SetStretchBltMode(COLORONCOLOR);

		CSize sizeImg(pBmpInfo->bmiHeader.biWidth, ::abs( pBmpInfo->bmiHeader.biHeight ));
		CPoint viewportorg(0,0);

		if(rcClient.right<=(long)(GetZoom()*sizeImg.cx))
			viewportorg.x=0;
		else
			viewportorg.x=(long)((rcClient.right-GetZoom()*sizeImg.cx)/2);

		if(rcClient.bottom<=(long)(GetZoom()*sizeImg.cy))
			viewportorg.y=0;
		else
			viewportorg.y=(long)((rcClient.bottom - GetZoom()*sizeImg.cy)/2);

			/////////////////////////////////////////////////////
			int nLines = StretchDIBits( 
				dc.GetSafeHdc(),// Handle to the device
				viewportorg.x, 
				viewportorg.y,
				GetZoom()*sizeImg.cx,	// Dest. rectangle width.
				GetZoom()*sizeImg.cy,	// Dest. rectangle height.
				0,				// X-coordinate of lower-left corner of the source rect.
				0,				// Y-coordinate of lower-left corner of the source rect.
				pBmpInfo->bmiHeader.biWidth,	// Source rectangle width.
				::abs( pBmpInfo->bmiHeader.biHeight ), // Number of scan lines.
				pBits,	// Modified address of array with DIB bits.
				pBmpInfo,	// Address of structure with bitmap info.
				DIB_RGB_COLORS,	// RGB or palette indices.
				SRCCOPY
				);
		

	}

	long i;
	for(i=0;i<m_TrackerArray.GetCount();i++)
	{
		if ( m_TrackerArray[i]->m_rect.Width()>0 && m_TrackerArray[i]->m_rect.Height()>0 )
			m_TrackerArray[i]->Draw(&dc);
	}
	//Invalidate();

}


// CconfigView 打印

BOOL CconfigView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CconfigView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CconfigView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CconfigView 诊断

#ifdef _DEBUG
void CconfigView::AssertValid() const
{
	CView::AssertValid();
}

void CconfigView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CconfigDoc* CconfigView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CconfigDoc)));
	return (CconfigDoc*)m_pDocument;
}
#endif //_DEBUG


// CconfigView 消息处理程序

void CconfigView::OnBnClickedBtnOpenimage()
{
	// TODO: 在此添加控件通知处理程序代码
	//m_ExitImg=FALSE;
	//TCHAR szFilter[] = _T("bmp (*.bmp)|*.bmp|jpg (*.jpg)|*.jpg||");
	//CFileDialog dlgFile(
	//	TRUE,
	//	NULL, 
	//	NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, 
	//	(LPCTSTR)szFilter,
	//	NULL
	//	);
	//CString title;
	//VERIFY(title.LoadString(AFX_IDS_OPENFILE));



	//dlgFile.m_ofn.lpstrTitle = title;
	//dlgFile.m_ofn.lpstrDefExt = _T("bmp");
	//dlgFile.m_ofn.lpstrFile = filename.GetBuffer(_MAX_PATH);

	//if(dlgFile.DoModal() != IDOK)
	//	return;

	//filename.ReleaseBuffer();
	//static_image=cvLoadImage(filename,CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
	////static_image=static_image;
	////if (static_image->width != 1280)
	////{
	////	AfxMessageBox("请选择符合要求的图像！");
	////	return;
	////}
	//
	//Ipl2Bitmap(static_image);
	////cvReleaseImage(&static_image);
	//
	//
	//Invalidate();

}
void CconfigView::Ipl2Bitmap(IplImage *Iplmg)
{
	IplImage* Iplmg_tmp = cvCreateImage(cvSize(Iplmg->width,Iplmg->height),Iplmg->depth,Iplmg->nChannels);

	cvConvertImage(Iplmg,Iplmg_tmp,CV_CVTIMG_FLIP);

	//m_pBmpInfo_out->bmiHeader.biSize =sizeof(BITMAPINFOHEADER);

	//m_pBmpInfo_out->bmiHeader.biBitCount =Iplmg_tmp->nChannels*8;
	m_pBmpInfo_out=(BITMAPINFO *)malloc(sizeof(BITMAPINFO)+sizeof(RGBQUAD)*256);
	m_pBmpInfo_out->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);

	m_pBmpInfo_out->bmiHeader.biBitCount =Iplmg_tmp->nChannels*8;
	m_pBmpInfo_out->bmiHeader.biClrUsed =0;
	m_pBmpInfo_out->bmiHeader.biCompression =BI_RGB;
	m_pBmpInfo_out->bmiHeader.biHeight =Iplmg_tmp->height;
	m_pBmpInfo_out->bmiHeader.biWidth =Iplmg_tmp->width;
	m_pBmpInfo_out->bmiHeader.biPlanes =1;
	m_pBmpInfo_out->bmiHeader.biSizeImage =0;
	m_pBmpInfo_out->bmiHeader.biXPelsPerMeter=0;
	m_pBmpInfo_out->bmiHeader.biYPelsPerMeter=0;
		for (int k=0;k<256;k++)
		{
			m_pBmpInfo_out->bmiColors[k].rgbBlue=k;
			m_pBmpInfo_out->bmiColors[k].rgbGreen=k;
			m_pBmpInfo_out->bmiColors[k].rgbRed=k;
			m_pBmpInfo_out->bmiColors[k].rgbReserved=0;
		}



	/*int ImgSize=(((m_pBmpInfo_out)->bmiHeader.biWidth  *  (m_pBmpInfo_out)->bmiHeader.biBitCount +  31 )/32) * 4  * (m_pBmpInfo_out)->bmiHeader.biHeight;
	(m_pBits_out)=new BYTE[ImgSize+1];
	memcpy(m_pBits_out,Iplmg_tmp->imageData,ImgSize);*/
	memcpy(m_pBits_out,Iplmg_tmp->imageData,Iplmg_tmp->imageSize);
	
	cvReleaseImage(&Iplmg_tmp);


}
IplImage* CconfigView::Bitmap2Ipl(BITMAPINFO* pBmpInfo,void* pBits)
{

	int channel_tmp = pBmpInfo->bmiHeader.biBitCount/8;
	IplImage* m_IplFrame = cvCreateImageHeader(cvSize(pBmpInfo->bmiHeader.biWidth,::abs(pBmpInfo->bmiHeader.biHeight)),IPL_DEPTH_8U,channel_tmp);
	cvSetData(m_IplFrame,pBits,pBmpInfo->bmiHeader.biWidth*channel_tmp);
	//静态图像需要倒置图像数据
	IplImage* Iplmg_tmp = cvCreateImage(cvSize(pBmpInfo->bmiHeader.biWidth,::abs(pBmpInfo->bmiHeader.biHeight)),IPL_DEPTH_8U,channel_tmp);

	cvConvertImage(m_IplFrame,Iplmg_tmp,CV_CVTIMG_FLIP);
	cvReleaseImageHeader(&m_IplFrame);

	//return m_IplFrame;
	return Iplmg_tmp;

}

void CconfigView::OnBnClickedBtnExit()
{
	// TODO: 在此添加控件通知处理程序代码
	if(AfxMessageBox(_T("确认退出程序？"),MB_OKCANCEL)== IDOK)
	{
		if (m_CameraMode==1)
		{
			camoperate.CameraStop(&m_Camera);
		}
		GetParent()->PostMessage(WM_CLOSE);
	}
}

double CconfigView::GetZoom()
{
	//m_zoom = 0.5;
	return m_zoom;
}
void CconfigView::SetZoom(double zoom)
{
	m_zoom = zoom;
}



void CconfigView::OnBnClickedBtnDrawselrect()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_TrackerArray.GetCount()<NUM_RECTTRACKER)
	{
		SetDrawState(TRUE);
	}
	else
	{
		SetDrawState(FALSE);
	}
}

void CconfigView::OnUpdateDrawRectBtn(CCmdUI* pCmdUI)
{
	//long nMaxRectCount = g_MethodParamCfg.GetInt(
	//	CMethodParamIni::g_strSection,
	//	CMethodParamIni::g_strSelRectCount,
	//	3);
	pCmdUI->Enable(m_TrackerArray.GetCount()>=NUM_RECTTRACKER?FALSE:TRUE);
}

void CconfigView::OnLButtonDown(UINT nFlags, CPoint point)
{
	buttonup=FALSE;
	m_ptOrigin=point;
	//m_bDraw=TRUE;
	count++;

	CRectTracker* pTracker = HitTestTracker(point);
	m_pCurRect = pTracker;

	if(pTracker)
	{
		if (pTracker->Track(this, point, true))
		{
			pTracker->m_rect.NormalizeRect();
			//update your selection coordinate

			//Invalidate();
		} 
	}
	else
	{
		if(m_TrackerArray.GetCount()< NUM_RECTTRACKER && GetDrawState() == TRUE)
		{			
			CRectTracker* track = new CRectTracker;
			track->m_rect = CRect(0,0,0,0);
			track->m_nStyle = 0;
			track->m_nStyle |= CRectTracker::hatchedBorder;
			track->m_nStyle |= CRectTracker::resizeOutside;

			if (track->TrackRubberBand(this, point, true)) 
			{
				track->m_rect.NormalizeRect();				
				m_TrackerArray.Add(track);				

				m_pCurRect = track;
			} 			
			else
				delete track;

			Invalidate();
		} 	
	}

	/*m_ptOrigin=point;*/

	CScrollView::OnLButtonDown(nFlags, point);
}

CRectTracker* CconfigView::HitTestTracker(const CPoint& pt)
{
	long i;
	for(i=0;i<m_TrackerArray.GetCount();i++)
	{
		if (m_TrackerArray[i]->HitTest(pt) >= 0)
		{
			return m_TrackerArray.GetAt(i);			 
		} 
	}
	return NULL;
}

BOOL CconfigView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	long i;
	for(i=0;i<m_TrackerArray.GetCount();i++)
	{
		if (m_TrackerArray[i]->SetCursor( pWnd, nHitTest )) 
			return TRUE;
	}

	return CScrollView::OnSetCursor(pWnd, nHitTest, message);
}
void CconfigView::OnBnClickedBtnDelselrect()
{
	// TODO: 在此添加控件通知处理程序代码
	long i;
	for(i=0;i<m_TrackerArray.GetCount();i++)
	{
		if(m_pCurRect == m_TrackerArray.GetAt(i))
		{
			delete m_pCurRect;
			m_TrackerArray.RemoveAt(i);

			m_pCurRect = NULL;
			break;
		}
	}
	Invalidate();
}

void CconfigView::OnUpdateDelRectBtn(CCmdUI* pCmdUI)
{	
	pCmdUI->Enable(m_TrackerArray.GetCount()>0?TRUE:FALSE);
}

void CconfigView::OnBnClickedCapturerect()
{
	// TODO: 在此添加控件通知处理程序代码
	BITMAPINFO* pBmpInfo = NULL;
	void * pBits = NULL;

	if (m_CameraMode == 0)
	{
		pBmpInfo = m_pBmpInfo_out;
		pBits = m_pBits_out;
	}
	else
	{
		pBmpInfo = &camoperate.m_pBmpInfo;
		pBits = camoperate.buffer;
	}
	int i;

	CRect Rect_search;
	//
	for(i = 0;i<m_TrackerArray.GetCount();i++)
	{
		CRect rcSel(m_TrackerArray[i]->m_rect);

		CPoint ptTopLeft(rcSel.TopLeft()),ptTopRight(rcSel.right, rcSel.top),ptBtmLeft(rcSel.left, rcSel.bottom), ptBtmRight(rcSel.BottomRight());

		CanvasToVirtual(ptTopLeft);
		CanvasToVirtual(ptTopRight);
		CanvasToVirtual(ptBtmLeft);
		CanvasToVirtual(ptBtmRight);

		//
		CRect rcImgSel(ptTopLeft.x,ptTopLeft.y, ptBtmRight.x,ptBtmRight.y);
		CRect rcImage;

		rcImage = CRect(0,0,pBmpInfo->bmiHeader.biWidth, ::abs(pBmpInfo->bmiHeader.biHeight));	


		CRect rc;
		rc.IntersectRect(rcImage, rcImgSel);
		if(rc != rcImgSel)
			continue;

		Rect_search.left = rc.left;
		Rect_search.top= rc.top;
		Rect_search.right = rc.right;
		Rect_search.bottom = rc.bottom;
	}
	CaptureRect(Rect_search);

	
	for(i=0;i<m_TrackerArray.GetCount();i++)
	{
		CRectTracker* pTracker = m_TrackerArray.GetAt(i);
		if(pTracker)
		{
			delete pTracker;
			m_TrackerArray.RemoveAt(i);
			m_pCurRect = NULL;
		}
	}

	m_TrackerArray.RemoveAll();

	CvRect searchrect;
	searchrect.x = Rect_search.left;
	searchrect.y = Rect_search.top;
	searchrect.width = Rect_search.Width();
	searchrect.height = Rect_search.Height();
	cvRectangle(static_image,cvPoint(searchrect.x,searchrect.y),cvPoint(searchrect.x+searchrect.width,searchrect.y+searchrect.height),CV_RGB(0,255,0),2,8,0);
	CvFont font;  
	cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 1.0, 1.0, 0, 2, 8); 
	cvPutText(static_image,"CaptureRect",cvPoint(searchrect.x+0.3*searchrect.width,searchrect.y+0.5*searchrect.height), &font,CV_RGB(0,255,0));
	Ipl2Bitmap(static_image);

	Invalidate();
}


void CconfigView::CaptureRect(CRect Rect_input)
{
	
	CvRect Rect_search;
	Rect_search.x = Rect_input.left;
	Rect_search.y = Rect_input.top;
	Rect_search.width = Rect_input.Width();
	Rect_search.height = Rect_input.Height();
	
	int TrackerNumber;
	TrackerNumber = m_TrackerArray.GetCount();
	if (Rect_search.width != 0 && TrackerNumber == 1)
	{
		//FileStorage fs(".\\test.xml", FileStorage::APPEND);
		/*fs << "SearchRect_x" <<Rect_search.x;
		fs << "SearchRect_y" <<Rect_search.y;
		fs << "SearchRect_Width" <<Rect_search.width;
		fs << "SearchRect_Height" <<Rect_search.height;
		fs.release();*/

		/*Mat searchrect_mat = (Mat_<int>(4,1) << Rect_search.x, Rect_search.y, Rect_search.width, Rect_search.height);
		fs << "SearchRect" <<searchrect_mat;*/
		
		Doc.writedata(routename,"SearchRect",Rect_search);

		MessageBox("捕获区域绘制成功！");
	}
	else
	{
		MessageBox("捕获区域绘制失败！");
	}
}
void CconfigView::Readini()
{
	int temp_int,temp_double;
}

void CconfigView::CanvasToVirtual( CPoint& point )
{
	CPoint ptScrl = GetScrollPosition();//获取滚动条的位置
	point.x += ptScrl.x;
	point.y += ptScrl.y;

	CSize sizeImg;

	BITMAPINFO* pBmpInfo = NULL;
	void * pBits = NULL;

	if (m_CameraMode == 0)
	{
		pBmpInfo = m_pBmpInfo_out;
		pBits = m_pBits_out;
	}
	else
	{
		pBmpInfo = &camoperate.m_pBmpInfo;
		pBits = camoperate.buffer;
	}

	sizeImg = CSize(pBmpInfo->bmiHeader.biWidth,::abs(pBmpInfo->bmiHeader.biHeight));




	CRect viewclientRect;
	GetClientRect(&viewclientRect);
	CPoint viewportorg(0,0);

	if(viewclientRect.right<=(long)(GetZoom()*sizeImg.cx))
		viewportorg.x=0;
	else
		viewportorg.x=(long)((viewclientRect.right-GetZoom()*sizeImg.cx)/2);

	if(viewclientRect.bottom<=(long)(GetZoom()*sizeImg.cy))
		viewportorg.y=0;
	else
		viewportorg.y=(long)((viewclientRect.bottom - GetZoom()*sizeImg.cy)/2);

	point.x -= viewportorg.x;
	point.y -= viewportorg.y;

	double zm = 1./GetZoom();
	point.x = point.x*zm + 0.5;
	point.y = point.y*zm + 0.5;
}

void CconfigView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	/*HDC hdc;
	hdc=::GetDC(m_hWnd);
	MoveToEx(hdc,m_ptOrigin.x,m_ptOrigin.y,NULL);
	LineTo(hdc,point.x,point.y);
	::ReleaseDC(m_hWnd,hdc);*/
	buttonup=TRUE;

	if (m_bDraw)
	{
		CPen pen(PS_SOLID,5,RGB(255,255,0));
		CClientDC dc(this);
		CPen *pOldPen=dc.SelectObject(&pen);
		dc.MoveTo(m_ptOrigin);
		m_start=m_ptOrigin;
		dc.LineTo(point);
		m_end=point;
		dc.SelectObject(pOldPen);
	}
	//if (m_DrawPoint)
	//{
	//	/*CPen pen(PS_SOLID,5,RGB(255,0,0));
	//	CClientDC dc(this);
	//	CPen *pOldPen=dc.SelectObject(&pen);
	//	dc.MoveTo(m_ptOrigin);
	//	m_start=m_ptOrigin;
	//	dc.LineTo(point);
	//	m_end=point;
	//	dc.SelectObject(pOldPen);*/
	//	CClientDC dc(this);
	//	//SetPixel(dc,point.x , point.y,RGB(255,0,0));
	//	m_start=m_ptOrigin;
	//	CBrush *oldBrush;//定义画刷
	//	CBrush newBrush(RGB(255,0,0));
	//	oldBrush = dc.SelectObject(&newBrush);
	//	dc.Ellipse(point.x-4,point.y-4,point.x+4,point.y+4);//半径是4

	//	CanvasToVirtual(m_start);
	//	/*	FileStorage fs(".\\test.xml",FileStorage::APPEND);
	//	fs << "Point"<<count;
	//	fs <<"point_x"<<m_start.x;
	//	fs <<"point_y"<<m_start.y;*/
	//	Point point;
	//	point.x=m_start.x;
	//	point.y=m_start.y;
	//	switch(count)
	//	{
	//	case 1:
	//		Doc.writedata(routename,"Point1",point);
	//		break;
	//	case 2:
	//		Doc.writedata(routename,"Point2",point);
	//		break;
	//	case 3:
	//		Doc.writedata(routename,"Point3",point);
	//		break;
	//	case 4:
	//		Doc.writedata(routename,"Point4",point);
	//		break;
	//	case 5:
	//		Doc.writedata(routename,"Point5",point);
	//		break;
	//	case 6:
	//		Doc.writedata(routename,"Point6",point);
	//		break;
	//	case 7:
	//		Doc.writedata(routename,"Point7",point);
	//		break;
	//	case 8:
	//		Doc.writedata(routename,"Point8",point);
	//		break;
	//	case 9:
	//		Doc.writedata(routename,"Point9",point);
	//		break;
	//	case 10:
	//		Doc.writedata(routename,"Point10",point);
	//		break;
	//	case 11:
	//		Doc.writedata(routename,"Point11",point);
	//		break;
	//	case 12:
	//		Doc.writedata(routename,"Point12",point);
	//		break;
	//	case 13:
	//		Doc.writedata(routename,"Point13",point);
	//		break;
	//	case 14:
	//		Doc.writedata(routename,"Point14",point);
	//		break;
	//	case 15:
	//		Doc.writedata(routename,"Point15",point);
	//		break;
	//	case 16:
	//		Doc.writedata(routename,"Point16",point);
	//		break;
	//	case 17:
	//		Doc.writedata(routename,"Point17",point);
	//		break;
	//	case 18:
	//		Doc.writedata(routename,"Point18",point);
	//		break;
	//	case 19:
	//		Doc.writedata(routename,"Point19",point);
	//		break;
	//	case 20:
	//		Doc.writedata(routename,"Point20",point);
	//		break;
	//	default:
	//		return;
	//	}
	//}
	//
	if (m_DrawPoint1)
	{
		try
		{
		/*CPen pen(PS_SOLID,5,RGB(255,0,0));
		CClientDC dc(this);
		CPen *pOldPen=dc.SelectObject(&pen);
		dc.MoveTo(m_ptOrigin);
		m_start=m_ptOrigin;
		dc.LineTo(point);
		m_end=point;
		dc.SelectObject(pOldPen);*/
		m_start=m_ptOrigin;
		CClientDC dc(this);
		//SetPixel(dc,point.x , point.y,RGB(255,0,0));
		CBrush *oldBrush;//定义画刷
		CBrush newBrush(RGB(255,0,0));
		oldBrush = dc.SelectObject(&newBrush);
		dc.Ellipse(point.x-4,point.y-4,point.x+4,point.y+4);//半径是4


		CanvasToVirtual(m_start);
		Point point;
		point.x=m_start.x;
		point.y=m_start.y;

		CPointNum pointnum;
		INT_PTR nResponse = pointnum.DoModal();
		insertnum=pointnum.insertnum;
		/*FileStorage fs(".\\test.xml",FileStorage::APPEND);
		fs << "Point"<<insertnum;
		fs <<"point_x"<<m_start.x;
		fs <<"point_y"<<m_start.y;
		fs<<"distance"<<pointnum.distance;*/
		
		switch(insertnum)
		{
		case 1:
			VLinePoints1.push_back(make_pair(pointnum.distance,point));
			break;
		case 2:
			VLinePoints2.push_back(make_pair(pointnum.distance,point));
			break;
		case 3:
			VLinePoints3.push_back(make_pair(pointnum.distance,point));
			break;
		case 4:
			VLinePoints4.push_back(make_pair(pointnum.distance,point));
			break;
		case 5:
			VLinePoints5.push_back(make_pair(pointnum.distance,point));
			break;
		default:
			break;
		}
		}
		catch(Exception &e)
		{
		}
		
	//Invalidate();
	}

	CScrollView::OnLButtonUp(nFlags, point);
}

void CconfigView::OnBnClickedDrawline()
{
	// TODO: 在此添加控件通知处理程序代码
	SetDrawState(FALSE);
	m_bDraw=true;
	m_DrawPoint=FALSE;
	m_DrawPoint1=FALSE;
}




void CconfigView::OnBnClickedLineconfirm()
{
	// TODO: 在此添加控件通知处理程序代码
	guideLine.DoModal();

	CanvasToVirtual(m_start);
	CanvasToVirtual(m_end);
	
	////CString linenum=guideLine.str_guide;
	//FileStorage fs(".\\test.xml", FileStorage::APPEND);
	//
	//if (m_start!=m_end)
	//{
	//	/*fs << "GuideLine"<<guideLine.guidenum ;
	//	fs <<"start_x"<<m_start.x;
	//	fs <<"start_x"<<m_start.y;
	//	fs <<"end_x"<<m_end.x;
	//	fs <<"end_y"<<m_end.y;*/
	//	Mat guideline_mat = (Mat_<int>(5,1) << guideLine.guidenum,m_start.x,m_start.y,m_end.x,m_end.y);
	//	fs << "Guideline" <<guideline_mat;

	//}
	
	Point start;
	Point end;
	start.x=m_start.x;
	start.y=m_start.y;
	end.x=m_end.x;
	end.y=m_end.y;


	if(m_start!=m_end&&guideLine.guidenumFlag)
	{
		CString str_start;
		CString str_end;
		str_start.Format("%s%s%s","guide",guideLine.str_guide,"_start");
		str_end.Format("%s%s%s","guide",guideLine.str_guide,"_end");
		String Str_start=LPCTSTR(str_start);
		String Str_end=LPCTSTR(str_end);
		Doc.writedata(routename,Str_start,start);
		Doc.writedata(routename,Str_end,end);

		MessageBox("引导线绘制成功！");
		cvLine(static_image,start,end,CV_RGB(0,0,0),5,8,0);
	}
	else
		MessageBox("引导线绘制失败！");


	/*switch(guideLine.guidenum)
	{
	case 1:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide1_start",start);
	Doc.writedata(routename,"guide1_end",end);

	MessageBox("引导线1绘制成功！");
	}
	else
	MessageBox("引导线1绘制失败！");

	break;
	case 2:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide2_start",start);
	Doc.writedata(routename,"guide2_end",end);

	MessageBox("引导线2绘制成功！");
	}
	else
	MessageBox("引导线2绘制失败！");
	break;
	case 3:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide3_start",start);
	Doc.writedata(routename,"guide3_end",end);

	MessageBox("引导线3绘制成功！");
	}
	else
	MessageBox("引导线3绘制失败！");
	break;
	case 4:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide4_start",start);
	Doc.writedata(routename,"guide4_end",end);
	MessageBox("引导线4绘制成功！");
	}
	else
	MessageBox("引导线4绘制失败！");
	break;
	case 5:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide5_start",start);
	Doc.writedata(routename,"guide5_end",end);
	MessageBox("引导线5绘制成功！");
	}
	else
	MessageBox("引导线5绘制失败！");
	break;
	default:
	break;
	}*/

	
	Ipl2Bitmap(static_image);
	for(int i=0;i<m_TrackerArray.GetCount();i++)
	{
		CRectTracker* pTracker = m_TrackerArray.GetAt(i);
		if(pTracker)
		{
			delete pTracker;
			m_TrackerArray.RemoveAt(i);
			m_pCurRect = NULL;
		}
	}

	m_TrackerArray.RemoveAll();
	Invalidate();
	m_bDraw=FALSE;
	
	
}


void CconfigView::OnBnClickedTrack()
{
	try
	{
	// TODO: 在此添加控件通知处理程序代码
	BITMAPINFO* pBmpInfo = NULL;
	void * pBits = NULL;

	if (m_CameraMode == 0)
	{
		pBmpInfo = m_pBmpInfo_out;
		pBits = m_pBits_out;
	}
	else
	{
		pBmpInfo = &camoperate.m_pBmpInfo;
		pBits = camoperate.buffer;
	}
	int i;

	CRect Rect_search;
	//
	for(i = 0;i<m_TrackerArray.GetCount();i++)
	{
		CRect rcSel(m_TrackerArray[i]->m_rect);

		CPoint ptTopLeft(rcSel.TopLeft()),ptTopRight(rcSel.right, rcSel.top),ptBtmLeft(rcSel.left, rcSel.bottom), ptBtmRight(rcSel.BottomRight());

		CanvasToVirtual(ptTopLeft);
		CanvasToVirtual(ptTopRight);
		CanvasToVirtual(ptBtmLeft);
		CanvasToVirtual(ptBtmRight);

		//
		CRect rcImgSel(ptTopLeft.x,ptTopLeft.y, ptBtmRight.x,ptBtmRight.y);
		CRect rcImage;

		rcImage = CRect(0,0,pBmpInfo->bmiHeader.biWidth, ::abs(pBmpInfo->bmiHeader.biHeight));	


		CRect rc;
		rc.IntersectRect(rcImage, rcImgSel);
		if(rc != rcImgSel)
			continue;

		Rect_search.left = rc.left;
		Rect_search.top= rc.top;
		Rect_search.right = rc.right;
		Rect_search.bottom = rc.bottom;
	}
	TrackRect(Rect_search);

	CvRect searchrect;
	searchrect.x = Rect_search.left;
	searchrect.y = Rect_search.top;
	searchrect.width = Rect_search.Width();
	searchrect.height = Rect_search.Height();
	cvRectangle(static_image,cvPoint(searchrect.x,searchrect.y),cvPoint(searchrect.x+searchrect.width,searchrect.y+searchrect.height),CV_RGB(0,255,0),2,8,0);
	CvFont font;  
	cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 1.0, 1.0, 0, 2, 8); 
	cvPutText(static_image,"TrackRect",cvPoint(searchrect.x+0.3*searchrect.width,searchrect.y+0.5*searchrect.height), &font,CV_RGB(0,255,0));
	Ipl2Bitmap(static_image);

	for(i=0;i<m_TrackerArray.GetCount();i++)
	{
		CRectTracker* pTracker = m_TrackerArray.GetAt(i);
		if(pTracker)
		{
			delete pTracker;
			m_TrackerArray.RemoveAt(i);
			m_pCurRect = NULL;
		}
	}

	m_TrackerArray.RemoveAll();
	Invalidate();
	}
	catch(Exception e)
	{
	}
}

void CconfigView::TrackRect(CRect Rect_input)
{
	CvRect Rect_search;
	Rect_search.x = Rect_input.left;
	Rect_search.y = Rect_input.top;
	Rect_search.width = Rect_input.Width();
	Rect_search.height = Rect_input.Height();

	int TrackerNumber;
	TrackerNumber = m_TrackerArray.GetCount();
	if (Rect_search.width != 0 && TrackerNumber == 1)
	{
		//FileStorage fs(".\\test.xml", FileStorage::APPEND);
		/*fs << "TrackRect_x" <<Rect_search.x;
		fs << "TrackRect_y" <<Rect_search.y;
		fs << "TrackRect_Width" <<Rect_search.width;
		fs << "TrackRect_Height" <<Rect_search.height;
		fs.release();*/
		//Mat trackrect_mat=(Mat_<int>(4,1) << Rect_search.x, Rect_search.y, Rect_search.width, Rect_search.height);
		//fs << "TrackRect" <<trackrect_mat;
		Doc.writedata(routename,"TrackRect",Rect_search);

		MessageBox("跟踪定位区域绘制成功！");
	}
	else
	{
		MessageBox("跟踪定位区域绘制失败！");
	}
}

void CconfigView::OnBnClickedStopline()
{
	// TODO: 在此添加控件通知处理程序代码

	linenum.DoModal();
	CanvasToVirtual(m_start);
	CanvasToVirtual(m_end);
	//FileStorage fs(".\\test.xml", FileStorage::APPEND);
	//
	//if (m_start!=m_end)
	//{
	//	/*fs << "guideLine"<<linenum.guidenum;
	//	fs << "StopLine"<<linenum.stopnum;
	//	fs <<"start_x"<<m_start.x;
	//	fs <<"start_x"<<m_start.y;
	//	fs <<"end_x"<<m_end.x;
	//	fs <<"end_y"<<m_end.y;*/
	//	Mat stopline_mat = (Mat_<int>(6,1) << guideLine.guidenum,linenum.stopnum,m_start.x,m_start.y,m_end.x,m_end.y);
	//	fs << "Stopline" <<stopline_mat;
	//}
	Point start;
	Point end;
	start.x=m_start.x;
	start.y=m_start.y;
	end.x=m_end.x;
	end.y=m_end.y;

	if (m_start!=m_end&&linenum.linenumFlag)
	{
		CString str_start;
		CString str_end;
		str_start.Format("%s%s%s%s%s","guide",linenum.str_guidenum,"_stop",linenum.str_stopnum,"_start");
		str_end.Format("%s%s%s%s%s","guide",linenum.str_guidenum,"_stop",linenum.str_stopnum,"_end");
		String Str_start=LPCTSTR(str_start);
		String Str_end=LPCTSTR(str_end);
		Doc.writedata(routename,Str_start,start);
		Doc.writedata(routename,Str_end,end);
		MessageBox("停止线绘制成功！");
		cvLine(static_image,start,end,CV_RGB(0,0,0),5,8,0);
	}
	else
		MessageBox("停止线绘制失败！");

	/*if (linenum.str_guidenum=='1')
	{
	switch(linenum.stopnum)
	{
	case 1:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide1_stop1_start",start);
	Doc.writedata(routename,"guide1_stop1_end",end);

	MessageBox("停止线11绘制成功！");
	}
	else
	MessageBox("停止线11绘制失败！");
	break;
	case 2:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide1_stop2_start",start);
	Doc.writedata(routename,"guide1_stop2_end",end);
	MessageBox("停止线12绘制成功！");
	}
	else
	MessageBox("停止线12绘制失败！");
	break;
	case 3:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide1_stop3_start",start);
	Doc.writedata(routename,"guide1_stop3_end",end);
	MessageBox("停止线13绘制成功！");
	}
	else
	MessageBox("停止线13绘制失败！");
	break;
	case 4:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide1_stop4_start",start);
	Doc.writedata(routename,"guide1_stop4_end",end);
	MessageBox("停止线14绘制成功！");
	}
	else
	MessageBox("停止线14绘制失败！");
	break;
	case 5:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide1_stop5_start",start);
	Doc.writedata(routename,"guide1_stop5_end",end);
	MessageBox("停止线15绘制成功！");
	}
	else
	MessageBox("停止线15绘制失败！");
	break;
	case 6:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide1_stop6_start",start);
	Doc.writedata(routename,"guide1_stop6_end",end);
	MessageBox("停止线16绘制成功！");
	}
	else
	MessageBox("停止线16绘制失败！");
	break;
	default:
	break;

	}
	}
	else if (linenum.str_guidenum=='2')
	{
	switch(linenum.stopnum)
	{
	case 1:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide2_stop1_start",start);
	Doc.writedata(routename,"guide2_stop1_end",end);
	MessageBox("停止线21绘制成功！");
	}
	else
	MessageBox("停止线21绘制失败！");
	break;
	case 2:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide2_stop2_start",start);
	Doc.writedata(routename,"guide2_stop2_end",end);
	MessageBox("停止线22绘制成功！");
	}
	else
	MessageBox("停止线22绘制失败！");
	break;
	case 3:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide2_stop3_start",start);
	Doc.writedata(routename,"guide2_stop3_end",end);
	MessageBox("停止线23绘制成功！");
	}
	else
	MessageBox("停止线23绘制失败！");
	break;
	case 4:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide2_stop4_start",start);
	Doc.writedata(routename,"guide2_stop4_end",end);
	MessageBox("停止线24绘制成功！");
	}
	else
	MessageBox("停止线24绘制失败！");
	break;
	case 5:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide2_stop5_start",start);
	Doc.writedata(routename,"guide2_stop5_end",end);
	MessageBox("停止线25绘制成功！");
	}
	else
	MessageBox("停止线25绘制失败！");
	break;
	case 6:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide2_stop6_start",start);
	Doc.writedata(routename,"guide2_stop6_end",end);
	MessageBox("停止线26绘制成功！");
	}
	else
	MessageBox("停止线26绘制失败！");
	break;
	default:
	break;

	}
	}
	else if (linenum.str_guidenum=='3')
	{
	switch(linenum.stopnum)
	{
	case 1:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide3_stop1_start",start);
	Doc.writedata(routename,"guide3_stop1_end",end);
	MessageBox("停止线31绘制成功！");
	}
	else
	MessageBox("停止线31绘制失败！");
	break;
	case 2:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide3_stop2_start",start);
	Doc.writedata(routename,"guide3_stop2_end",end);
	MessageBox("停止线32绘制成功！");
	}
	else
	MessageBox("停止线32绘制失败！");
	break;
	case 3:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide3_stop3_start",start);
	Doc.writedata(routename,"guide3_stop3_end",end);
	MessageBox("停止线33绘制成功！");
	}
	else
	MessageBox("停止线33绘制失败！");
	break;
	case 4:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide3_stop4_start",start);
	Doc.writedata(routename,"guide3_stop4_end",end);
	MessageBox("停止线34绘制成功！");
	}
	else
	MessageBox("停止线34绘制失败！");
	break;
	case 5:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide3_stop5_start",start);
	Doc.writedata(routename,"guide3_stop5_end",end);
	MessageBox("停止线35绘制成功！");
	}
	else
	MessageBox("停止线35绘制失败！");
	break;
	case 6:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide3_stop6_start",start);
	Doc.writedata(routename,"guide3_stop6_end",end);
	MessageBox("停止线36绘制成功！");
	}
	else
	MessageBox("停止线36绘制失败！");
	break;
	default:
	break;

	}
	}
	else if (linenum.str_guidenum=='4')
	{
	switch(linenum.stopnum)
	{
	case 1:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide4_stop1_start",start);
	Doc.writedata(routename,"guide4_stop1_end",end);
	MessageBox("停止线41绘制成功！");
	}
	else
	MessageBox("停止线41绘制失败！");
	break;
	case 2:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide4_stop2_start",start);
	Doc.writedata(routename,"guide4_stop2_end",end);
	MessageBox("停止线42绘制成功！");
	}
	else
	MessageBox("停止线42绘制失败！");
	break;
	case 3:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide4_stop3_start",start);
	Doc.writedata(routename,"guide4_stop3_end",end);
	MessageBox("停止线43绘制成功！");
	}
	else
	MessageBox("停止线43绘制失败！");
	break;
	case 4:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide4_stop4_start",start);
	Doc.writedata(routename,"guide4_stop4_end",end);
	MessageBox("停止线44绘制成功！");
	}
	else
	MessageBox("停止线44绘制失败！");
	break;
	case 5:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide4_stop5_start",start);
	Doc.writedata(routename,"guide4_stop5_end",end);
	MessageBox("停止线45绘制成功！");
	}
	else
	MessageBox("停止线45绘制失败！");
	break;
	case 6:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide4_stop6_start",start);
	Doc.writedata(routename,"guide4_stop6_end",end);
	MessageBox("停止线46绘制成功！");
	}
	else
	MessageBox("停止线46绘制失败！");
	break;
	default:
	break;

	}
	}
	else if (linenum.str_guidenum=='5')
	{
	switch(linenum.stopnum)
	{
	case 1:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide5_stop1_start",start);
	Doc.writedata(routename,"guide5_stop1_end",end);
	MessageBox("停止线51绘制成功！");
	}
	else
	MessageBox("停止线51绘制失败！");
	break;
	case 2:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide5_stop2_start",start);
	Doc.writedata(routename,"guide5_stop2_end",end);
	MessageBox("停止线52绘制成功！");
	}
	else
	MessageBox("停止线52绘制失败！");
	break;
	case 3:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide5_stop3_start",start);
	Doc.writedata(routename,"guide5_stop3_end",end);
	MessageBox("停止线53绘制成功！");
	}
	else
	MessageBox("停止线53绘制失败！");
	break;
	case 4:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide5_stop4_start",start);
	Doc.writedata(routename,"guide5_stop4_end",end);
	MessageBox("停止线54绘制成功！");
	}
	else
	MessageBox("停止线54绘制失败！");
	break;
	case 5:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide5_stop5_start",start);
	Doc.writedata(routename,"guide5_stop5_end",end);
	MessageBox("停止线55绘制成功！");
	}
	else
	MessageBox("停止线55绘制失败！");
	break;
	case 6:
	if(m_start!=m_end)
	{
	Doc.writedata(routename,"guide5_stop6_start",start);
	Doc.writedata(routename,"guide5_stop6_end",end);
	MessageBox("停止线56绘制成功！");
	}
	else
	MessageBox("停止线56绘制失败！");
	break;
	default:
	break;

	}
	}*/
	
	Ipl2Bitmap(static_image);

	for(int i=0;i<m_TrackerArray.GetCount();i++)
	{
		CRectTracker* pTracker = m_TrackerArray.GetAt(i);
		if(pTracker)
		{
			delete pTracker;
			m_TrackerArray.RemoveAt(i);
			m_pCurRect = NULL;
		}
	}

	m_TrackerArray.RemoveAll();
	Invalidate();
	m_bDraw=FALSE;


}


//void CconfigView::OnBnClickedButton5()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	IplImage*ima=cvCreateImage(cvSize(m_pBmpInfo_out->bmiHeader.biWidth,m_pBmpInfo_out->bmiHeader.biHeight),IPL_DEPTH_8U,3);
//	cvCvtColor(static_image,ima,CV_RGB2Lab);
//
//	for (int i=0;i<ima->height;i++)
//	{
//		uchar*ptr=(uchar*)(ima->imageData+i*ima->widthStep);
//
//		for (int j=0;j<ima->width;j++)
//		{
//			ptr[3*i+1]=255;
//			ptr[3*i+2]=255;
//		}
//	}
//
//}


void CconfigView::OnBnClickedBridge()
{
	// TODO: 在此添加控件通知处理程序代码
	BITMAPINFO* pBmpInfo = NULL;
	void * pBits = NULL;

	if (m_CameraMode == 0)
	{
		pBmpInfo = m_pBmpInfo_out;
		pBits = m_pBits_out;
	}
	else
	{
		pBmpInfo = &camoperate.m_pBmpInfo;
		pBits = camoperate.buffer;
	}
	int i;

	CRect Rect_search;
	//
	for(i = 0;i<m_TrackerArray.GetCount();i++)
	{
		CRect rcSel(m_TrackerArray[i]->m_rect);

		CPoint ptTopLeft(rcSel.TopLeft()),ptTopRight(rcSel.right, rcSel.top),ptBtmLeft(rcSel.left, rcSel.bottom), ptBtmRight(rcSel.BottomRight());

		CanvasToVirtual(ptTopLeft);
		CanvasToVirtual(ptTopRight);
		CanvasToVirtual(ptBtmLeft);
		CanvasToVirtual(ptBtmRight);

		//
		CRect rcImgSel(ptTopLeft.x,ptTopLeft.y, ptBtmRight.x,ptBtmRight.y);
		CRect rcImage;

		rcImage = CRect(0,0,pBmpInfo->bmiHeader.biWidth, ::abs(pBmpInfo->bmiHeader.biHeight));	


		CRect rc;
		rc.IntersectRect(rcImage, rcImgSel);
		if(rc != rcImgSel)
			continue;

		Rect_search.left = rc.left;
		Rect_search.top= rc.top;
		Rect_search.right = rc.right;
		Rect_search.bottom = rc.bottom;
	}
	BridgeRect(Rect_search);

	CvRect searchrect;
	searchrect.x = Rect_search.left;
	searchrect.y = Rect_search.top;
	searchrect.width = Rect_search.Width();
	searchrect.height = Rect_search.Height();
	cvRectangle(static_image,cvPoint(searchrect.x,searchrect.y),cvPoint(searchrect.x+searchrect.width,searchrect.y+searchrect.height),CV_RGB(0,255,0),2,8,0);
	CvFont font;  
	cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 1.0, 1.0, 0, 2, 8); 
	cvPutText(static_image,"BridgeRect",cvPoint(searchrect.x+0.3*searchrect.width,searchrect.y+0.5*searchrect.height), &font,CV_RGB(0,255,0));
	Ipl2Bitmap(static_image);

	for(i=0;i<m_TrackerArray.GetCount();i++)
	{
		CRectTracker* pTracker = m_TrackerArray.GetAt(i);
		if(pTracker)
		{
			delete pTracker;
			m_TrackerArray.RemoveAt(i);
			m_pCurRect = NULL;
		}
	}

	m_TrackerArray.RemoveAll();
	Invalidate();
}

void CconfigView::BridgeRect(CRect Rect_input)
{

	CvRect Rect_search;
	Rect_search.x = Rect_input.left;
	Rect_search.y = Rect_input.top;
	Rect_search.width = Rect_input.Width();
	Rect_search.height = Rect_input.Height();

	int TrackerNumber;
	TrackerNumber = m_TrackerArray.GetCount();
	if (Rect_search.width != 0 && TrackerNumber == 1)
	{
		//FileStorage fs(".\\test.xml", FileStorage::APPEND);
		///*fs << "BridgeRect_x" <<Rect_search.x;
		//fs << "BridgeRect_y" <<Rect_search.y;
		//fs << "BridgeRect_Width" <<Rect_search.width;
		//fs << "BridgeRect_Height" <<Rect_search.height;
		//fs.release();*/
		//Mat bridgerect_mat=(Mat_<int>(4,1) << Rect_search.x, Rect_search.y, Rect_search.width, Rect_search.height);
		//fs << "BridgeRect" <<bridgerect_mat;
		Doc.writedata(routename,"BridgeRect",Rect_search);

		MessageBox("登机桥区域绘制成功！");
	}
	else
	{
		MessageBox("登机桥区域绘制失败！");
	}
}



void CconfigView::OnBnClickedDelline()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bDraw=FALSE;
	Invalidate();
}

void CconfigView::OnUpdateDelLineBtn(CCmdUI* pCmdUI)
{	
	pCmdUI->Enable(m_bDraw?TRUE:FALSE);
	
}


void CconfigView::OnBnClickedPlanetype()
{
	// TODO: 在此添加控件通知处理程序代码
	CPlaneType planetype;
	INT_PTR nResponse = planetype.DoModal();
	if (planetype.planeType)
	{
		//Doc.writedata(routename,"PlaneType",planetype.VS);
		CString str;
		str.Format("%s%s%s","PlaneType",planetype.str_guide,planetype.str_stop);
		String Str=LPCTSTR(str);
		Doc.writedata(routename,Str,planetype.VS);

		/*if (planetype.str_guide =='1')
		{
		switch(planetype.stopNum)
		{
		case 1:
		Doc.writedata(routename,"PlaneType11",planetype.VS);
		break;
		case 2:
		Doc.writedata(routename,"PlaneType12",planetype.VS);
		break;
		case 3:
		Doc.writedata(routename,"PlaneType13",planetype.VS);
		break;
		case 4:
		Doc.writedata(routename,"PlaneType14",planetype.VS);
		break;
		case 5:
		Doc.writedata(routename,"PlaneType15",planetype.VS);
		break;
		case 6:
		Doc.writedata(routename,"PlaneType16",planetype.VS);
		break;
		default:
		break;

		}
		}
		else if (planetype.str_guide =='2')
		{
		switch(planetype.stopNum)
		{
		case 1:
		Doc.writedata(routename,"PlaneType21",planetype.VS);
		break;
		case 2:
		Doc.writedata(routename,"PlaneType22",planetype.VS);
		break;
		case 3:
		Doc.writedata(routename,"PlaneType23",planetype.VS);
		break;
		case 4:
		Doc.writedata(routename,"PlaneType24",planetype.VS);
		break;
		case 5:
		Doc.writedata(routename,"PlaneType25",planetype.VS);
		break;
		case 6:
		Doc.writedata(routename,"PlaneType26",planetype.VS);
		break;
		default:
		break;

		}
		}
		else if (planetype.str_guide =='3')
		{
		switch(planetype.stopNum)
		{
		case 1:
		Doc.writedata(routename,"PlaneType31",planetype.VS);
		break;
		case 2:
		Doc.writedata(routename,"PlaneType32",planetype.VS);
		break;
		case 3:
		Doc.writedata(routename,"PlaneType33",planetype.VS);
		break;
		case 4:
		Doc.writedata(routename,"PlaneType34",planetype.VS);
		break;
		case 5:
		Doc.writedata(routename,"PlaneType35",planetype.VS);
		break;
		case 6:
		Doc.writedata(routename,"PlaneType36",planetype.VS);
		break;
		default:
		break;

		}
		}
		else if (planetype.str_guide =='4')
		{
		switch(planetype.stopNum)
		{
		case 1:
		Doc.writedata(routename,"PlaneType41",planetype.VS);
		break;
		case 2:
		Doc.writedata(routename,"PlaneType42",planetype.VS);
		break;
		case 3:
		Doc.writedata(routename,"PlaneType43",planetype.VS);
		break;
		case 4:
		Doc.writedata(routename,"PlaneType44",planetype.VS);
		break;
		case 5:
		Doc.writedata(routename,"PlaneType45",planetype.VS);
		break;
		case 6:
		Doc.writedata(routename,"PlaneType46",planetype.VS);
		break;
		default:
		break;

		}
		}
		else if (planetype.str_guide =='5')
		{
		switch(planetype.stopNum)
		{
		case 1:
		Doc.writedata(routename,"PlaneType51",planetype.VS);
		break;
		case 2:
		Doc.writedata(routename,"PlaneType52",planetype.VS);
		break;
		case 3:
		Doc.writedata(routename,"PlaneType53",planetype.VS);
		break;
		case 4:
		Doc.writedata(routename,"PlaneType54",planetype.VS);
		break;
		case 5:
		Doc.writedata(routename,"PlaneType55",planetype.VS);
		break;
		case 6:
		Doc.writedata(routename,"PlaneType56",planetype.VS);
		break;
		default:
		break;

		}
		}*/
	}

}

BOOL CconfigView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//return CScrollView::OnEraseBkgnd(pDC);
	return TRUE;
}


void CconfigView::OnBnClickedDrawpoint()
{
	// TODO: 在此添加控件通知处理程序代码
	//m_DrawPoint=TRUE;
	switch(insertnum)
	{
	//case 1:
	//	Doc.writedata(routename,"guide1",VLinePoints1,choosepoint.Pointflag);
	//	break;
	//case 2:
	//	Doc.writedata(routename,"guide2",VLinePoints2,choosepoint.Pointflag);
	//	break;
	//case 3:
	//	Doc.writedata(routename,"guide3",VLinePoints3,choosepoint.Pointflag);
	//	break;
	//case 4:
	//	Doc.writedata(routename,"guide4",VLinePoints4,choosepoint.Pointflag);
	//	break;
	//case 5:
	//	Doc.writedata(routename,"guide5",VLinePoints5,choosepoint.Pointflag);
	//	break;
	default:
		break;
	}
	MessageBox("标记点绘制成功！");
}


void CconfigView::OnBnClickedDelpoint()
{
	// TODO: 在此添加控件通知处理程序代码
	m_DrawPoint=FALSE;
	m_DrawPoint1=FALSE;
	
	Invalidate();
}
void CconfigView::OnUpdateDelPointBtn(CCmdUI* pCmdUI)
{	
	//pCmdUI->Enable(m_DrawPoint?TRUE:FALSE);
	pCmdUI->Enable(m_DrawPoint1?TRUE:FALSE);

}

void CconfigView::OnBnClickedPointconfim()
{
	// TODO: 在此添加控件通知处理程序代码
	//插入单个点;
	m_DrawPoint1=TRUE;
	m_DrawPoint=FALSE;
	m_bDraw=FALSE;

	choosepoint.DoModal();




}


void CconfigView::OnBnClickedEnlarge()
{
	// TODO: 在此添加控件通知处理程序代码
	SetZoom(GetZoom()*1.2);
	 UpdateWnd( );
}

void CconfigView::UpdateWnd()
{
	//get image from the video window
	
	CSize sizeScroll(m_pBmpInfo_out->bmiHeader.biWidth*GetZoom(),::abs(m_pBmpInfo_out->bmiHeader.biHeight)*GetZoom());
	SetScrollSizes(MM_TEXT,sizeScroll);
	Invalidate();

}

void CconfigView::OnBnClickedShrink()
{
	// TODO: 在此添加控件通知处理程序代码
	SetZoom(GetZoom()*0.75);
	UpdateWnd();
}


void CconfigView::OnStartvideo()
{
	// TODO: 在此添加命令处理程序代码
	camoperate.CameraGet(&m_Camera);
	camoperate.CameraSetup(&m_Camera);
	camoperate.CameraStart(&m_Camera);
	m_CameraMode = 1;
}


void CconfigView::OnStopvideo()
{
	// TODO: 在此添加命令处理程序代码
	camoperate.CameraStop(&m_Camera);
	static_image=Bitmap2Ipl(camoperate.info,camoperate.image_one.data);
	Ipl2Bitmap(static_image);

}


void CconfigView::OnExitvideo()
{
	// TODO: 在此添加命令处理程序代码
	camoperate.CameraStop(&m_Camera);
	Invalidate( );
	CconfigView* Window=(CconfigView*)((CFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveView();
	CDC* pDC = Window->GetDC();
	CXMemDC dc(pDC);

	CRect rcClient;
	GetClientRect(rcClient);
	dc->FillSolidRect(rcClient,RGB(40,40,40));
	m_CameraMode = 0;
}


void CconfigView::OnExitimg()
{
	// TODO: 在此添加命令处理程序代码
	m_ExitImg=TRUE;
	//m_pBmpInfo_out = NULL;
	//m_pBits_out = NULL;

	Invalidate( );
}


void CconfigView::OnOpenimg()
{
	// TODO: 在此添加命令处理程序代码
	m_ExitImg=FALSE;
	TCHAR szFilter[] = _T("bmp (*.bmp)|*.bmp|jpg (*.jpg)|*.jpg||");
	CFileDialog dlgFile(
		TRUE,
		NULL, 
		NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, 
		(LPCTSTR)szFilter,
		NULL
		);
	CString title;
	VERIFY(title.LoadString(AFX_IDS_OPENFILE));



	dlgFile.m_ofn.lpstrTitle = title;
	dlgFile.m_ofn.lpstrDefExt = _T("bmp");
	dlgFile.m_ofn.lpstrFile = filename.GetBuffer(_MAX_PATH);

	if(dlgFile.DoModal() != IDOK)
		return;

	filename.ReleaseBuffer();
	//static_image=cvLoadImage(filename,CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
	static_image=cvLoadImage(filename,CV_LOAD_IMAGE_GRAYSCALE);
	static_image_one=cvLoadImage(filename,CV_LOAD_IMAGE_GRAYSCALE);
	//static_image=static_image;
	//if (static_image->width != 1280)
	//{
	//	AfxMessageBox("请选择符合要求的图像！");
	//	return;
	//}

	Ipl2Bitmap(static_image);
	//cvReleaseImage(&static_image);


	Invalidate();
}


void CconfigView::Onstation()
{
	// TODO: 在此添加命令处理程序代码
	CStationNum stationnum;
	INT_PTR nResponse = stationnum.DoModal();
	if (stationnum.station)
	{
		Doc.writedata(routename,"StationNum",stationnum.StationNum);
		Doc.writedata(routename,"PlacementNum",stationnum.PlacementNum);
		Doc.writedata(routename,"stopdistance",stationnum.stopdistance);
		Doc.writedata(routename,"toofardistance",stationnum.toofardistance);
		Doc.writedata(routename,"speed",stationnum.speed);
		Doc.writedata(routename,"pctime",stationnum.pctime);
	}
	

}


void CconfigView::OnLaster()
{
	// TODO: 在此添加命令处理程序代码
	//MessageBox("准备激光配置窗口");
	//g_laster.DoModal();
	//if (g_laster.laster_flag)
	//{
	//	laser_parameters lasers;
	//	CString str;
	//	str.Format("%s%s%s","Laser",g_laster.linenum.str_guidenum,g_laster.linenum.str_stopnum);
	//	String Str=LPCTSTR(str);
	//	lasers.guideline_stopline_name=Str;
	//	//lasers.install_high=g_laster.install_high;
	//	lasers.object_dis=g_laster.object_dis;
	//	lasers.guide_line_k=g_laster.guide_line_k;
	//	lasers.guide_line_b=g_laster.guide_line_b;
	//	lasers.guide_line_x=g_laster.guide_line_x;
	//	lasers.guide_line_y=g_laster.guide_line_y;
	//	//lasers.HrzConfirmSteps=g_laster.HrzConfirmSteps;
	//	//lasers.VertConfirmSteps=g_laster.VertConfirmSteps;
	//	Doc.writedata(routename,Str,lasers);
	//	MessageBox("激光场景配置成功！");
	//	/*Doc.writedata(routename,"install_high",g_laster.install_high);
	//	Doc.writedata(routename,"object_dis",g_laster.object_dis);
	//	Doc.writedata(routename,"guide_line_k",g_laster.guide_line_k);
	//	Doc.writedata(routename,"guide_line_b",g_laster.guide_line_b);
	//	Doc.writedata(routename,"guide_line_x",g_laster.guide_line_x);
	//	Doc.writedata(routename,"guide_line_y",g_laster.guide_line_y);
	//	Doc.writedata(routename,"HrzConfirmSteps",g_laster.HrzConfirmSteps);
	//	Doc.writedata(routename,"VertConfirmSteps",g_laster.VertConfirmSteps);*/
	//}
	/*else
		MessageBox("激光场景配置失败！");*/


}



void CconfigView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bDraw&&!buttonup)
	{
		if(m_ptOrigin.x!=0||m_ptOrigin.y!=0)
		{
			CPen pen(PS_SOLID,3,RGB(255,255,0));
			CClientDC dc(this);
			CPen *pOldPen=dc.SelectObject(&pen);
			dc.MoveTo(m_ptOrigin);
			m_start=m_ptOrigin;
			dc.LineTo(point);
			m_end=point;
			dc.SelectObject(pOldPen);
			Invalidate();

		}
	}

	CScrollView::OnMouseMove(nFlags, point);
}


void CconfigView::OnLastconfig()
{
	try
	{
		if(static_image == NULL) return;
	// TODO: 在此添加命令处理程序代码
	//显示引导线
	CString cs_guide_start[5];
	CString cs_guide_end[5];
	String s_guide_start[5];
	String s_guide_end[5];
	Point point_guide_start[5];
	Point point_guide_end[5];

	for (int i=0;i<5;i++)
	{
		point_guide_start[i].x=0;
		point_guide_start[i].y=0;
		point_guide_end[i].x=0;
		point_guide_end[i].y=0;

		cs_guide_start[i].Format("%s%d%s","guide",i+1,"_start");
		s_guide_start[i]=LPCTSTR(cs_guide_start[i]);
		cs_guide_end[i].Format("%s%d%s","guide",i+1,"_end");
		s_guide_end[i]=LPCTSTR(cs_guide_end[i]);
		point_guide_start[i]=Doc.readpointdata(routename,s_guide_start[i]);
		point_guide_end[i]=Doc.readpointdata(routename,s_guide_end[i]);
		if (point_guide_start[i].x!=0&&point_guide_start[i].y!=0&&point_guide_end[i].x!=0&&point_guide_end[i].y!=0)
		{
			cvLine(static_image,point_guide_start[i],point_guide_end[i],CV_RGB(0,0,0),5,8,0);
			
		}

	}
	//显示停止线
	CString cs_stop_start[5][6];
	CString cs_stop_end[5][6];
	String s_stop_start[5][6];
	String s_stop_end[5][6];
	Point point_stop_start[5][6];
	Point point_stop_end[5][6];

	for (int i=0;i<5;i++)
	{
		for (int j=0;j<6;j++)
		{
			point_stop_start[i][j].x=0;
			point_stop_start[i][j].y=0;
			point_stop_end[i][j].x=0;
			point_stop_end[i][j].y=0;
			cs_stop_start[i][j].Format("%s%d%s%d%s","guide",i+1,"_stop",j+1,"_start");
			s_stop_start[i][j]=LPCTSTR(cs_stop_start[i][j]);
			cs_stop_end[i][j].Format("%s%d%s%d%s","guide",i+1,"_stop",j+1,"_end");
			s_stop_end[i][j]=LPCTSTR(cs_stop_end[i][j]);
			point_stop_start[i][j]=Doc.readpointdata(routename,s_stop_start[i][j]);
			point_stop_end[i][j]=Doc.readpointdata(routename,s_stop_end[i][j]);
			if (point_stop_start[i][j].x!=0&&point_stop_start[i][j].y!=0&&point_stop_end[i][j].x!=0&&point_stop_end[i][j].y!=0)
			{
				cvLine(static_image,point_stop_start[i][j],point_stop_end[i][j],CV_RGB(0,0,0),5,8,0);
				
			}
		}
	}
	//显示捕获区域
	Rect rect_capture;
	rect_capture=Doc.readrectdata(routename,"SearchRect");
	cvRectangle(static_image,cvPoint(rect_capture.x,rect_capture.y),cvPoint(rect_capture.x+rect_capture.width,rect_capture.y+rect_capture.height),CV_RGB(0,255,0),2,8,0);
	CvFont font;  
	cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 1.0, 1.0, 0, 2, 8); 
	cvPutText(static_image,"CaptureRect",cvPoint(rect_capture.x+0.3*rect_capture.width,rect_capture.y+0.5*rect_capture.height), &font,CV_RGB(0,255,0));
	//显示跟踪区域
	Rect rect_track;
	rect_track=Doc.readrectdata(routename,"TrackRect");
	cvRectangle(static_image,cvPoint(rect_track.x,rect_track.y),cvPoint(rect_track.x+rect_track.width,rect_track.y+rect_track.height),CV_RGB(0,255,0),2,8,0);
	//CvFont font;  
	//cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 1.0, 1.0, 0, 2, 8); 
	cvPutText(static_image,"TrackRect",cvPoint(rect_track.x+0.3*rect_track.width,rect_track.y+0.5*rect_track.height), &font,CV_RGB(0,255,0));
	//显示登机桥区域
	Rect rect_bridge;
	rect_bridge=Doc.readrectdata(routename,"BridgeRect");
	cvRectangle(static_image,cvPoint(rect_bridge.x,rect_bridge.y),cvPoint(rect_bridge.x+rect_bridge.width,rect_bridge.y+rect_bridge.height),CV_RGB(0,255,0),2,8,0);
	//CvFont font;  
	//cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 1.0, 1.0, 0, 2, 8); 
	cvPutText(static_image,"BridgeRect",cvPoint(rect_bridge.x+0.3*rect_bridge.width,rect_bridge.y+0.5*rect_bridge.height), &font,CV_RGB(0,255,0));
	//绘制标记点
	
	vector<pair<double,Point>>VLinePoints[5];

	CString cs_guidepoint;
	String s_guidepoint;
	for (int i=0;i<5;i++)
	{
		cs_guidepoint.Format("%s%d","guide",i+1);
		s_guidepoint=LPCTSTR(cs_guidepoint);
		//VLinePoints[i]=Doc.readVOrientationdata(routename,s_guidepoint);
		int iter;
		iter = VLinePoints[i].end()-VLinePoints[i].begin();
		for (iter = 0; iter <= VLinePoints[i].end()-VLinePoints[i].begin()-1; iter++)
		{
			Point p_guide;
			p_guide.x=VLinePoints[i].at(iter).second.x;
			p_guide.y=VLinePoints[i].at(iter).second.y;
			double num=VLinePoints[i].at(iter).first;
			CString dis;
			dis.Format("%f",num);

			cvCircle(static_image,cvPoint(p_guide.x,p_guide.y),2,CV_RGB(0,0,0),20,8,0);
			cvPutText(static_image,dis,cvPoint(p_guide.x+5,p_guide.y), &font,CV_RGB(0,255,0));
			
		}
		
	}
	}
	catch(Exception &e)
	{
	}

	try
	{
	Ipl2Bitmap(static_image);
	Invalidate();
	}
	catch(Exception e)
	{

	}
}


void CconfigView::OnReconfig()
{
	// TODO: 在此添加命令处理程序代码
	static_image=static_image_one;
	Ipl2Bitmap(static_image);
	cvReleaseImage(&static_image_one);
	Invalidate();
}


void CconfigView::OnSaveimage()
{
	// TODO: 在此添加命令处理程序代码
	m_Imagesave=TRUE;
}


void CconfigView::OnPlanedata()
{
	// TODO: 在此添加命令处理程序代码
	CPlaneData planedata;
	 planedata.DoModal();

}


void CconfigView::OnCOMSetting()
{
	DLG_SETTING dlg_setting;
	dlg_setting.DoModal();
	// TODO: 在此添加命令处理程序代码
}

CLaster* _stdcall classLaserInit()
{
	return new CLaster();
}

void _stdcall classLaserDestroy(CLaster *p)
{
	p->LaserDestory();
	delete p;
	/*g_laster.~CLaster();
	delete g_laster;
	p->~CLaster();
	p->thread3.~thread_group();
	p->thread2.~thread_group();
	p->thread1.~thread_group();*/
	//delete p;
	/*glb_thread_group.remove_thread(boost::bind(&CLaster::Com_Laser_Read,this));
	glb_thread_group.remove_thread(boost::bind(&CLaster::Com_Laser_Write,this));
	glb_thread_group.remove_thread(boost::bind(&CLaster::OnCommand,this));
	glb_thread_group.remove_thread(boost::CLaster::OnCommand);*/
}

void _stdcall OnBnClickedConnect_std(CLaster *p)
{
	p->OnBnClickedConnect();
	//p->OnBnClickedConnect_agent();
}

void _stdcall OnBnClickedButtonReadtype_std(CLaster *p)//由机型读取引导线
{p->OnBnClickedButtonReadtype();}
void _stdcall OnBnClickedStopline_std(CLaster *p)//编号
{p->OnBnClickedStopline();}
void _stdcall OnBnClickedButtonDetect_std(CLaster *p)//校准
{p->OnBnClickedButtonDetect();}
void _stdcall OnBnClickedButtonReadedge_std(CLaster *p)//读取边框
{p->OnBnClickedButtonReadedge();}
void _stdcall OnBnClickedButtonLeft_std(CLaster *p)//左移1步
{p->OnBnClickedButtonLeft();}
void _stdcall OnBnClickedButtonRight_std(CLaster *p)//右移1步
{p->OnBnClickedButtonRight();}
void _stdcall OnBnClickedButtonDown_std(CLaster *p)//下移1步
{p->OnBnClickedButtonDown();}
void _stdcall OnBnClickedButtonUp_std(CLaster *p)//上移1步
{p->OnBnClickedButtonUp();}
void _stdcall OnBnClickedButtonLocate_std(CLaster *p)//单点测量
{p->OnBnClickedButtonLocate();}
void _stdcall OnBnClickedButtonRedlaseron_std(CLaster *p)//打开红色激光
{p->OnBnClickedButtonRedlaseron();}
void _stdcall OnBnClickedButtonRedlaseroff_std(CLaster *p)//关闭红色激光
{p->OnBnClickedButtonRedlaseroff();}
void _stdcall OnBnClickedButtonScanall_std(CLaster *p)//窗口扫描
{p->OnBnClickedButtonScanall();}
void _stdcall OnBnClickedButtonEdge_std(CLaster *p)//确定边框
{p->OnBnClickedButtonEdge();}
void _stdcall OnBnClickedButtonSetzero_std(CLaster *p)//置为零点
{p->OnBnClickedButtonSetzero();}
void _stdcall OnBnClickedButtonReset_std(CLaster *p)//回到零点
{p->OnBnClickedButtonReset();}
void _stdcall OnBnClickedButtonTwod_std(CLaster *p)//二维扫描
{p->OnBnClickedButtonTwod();}
void _stdcall OnBnClickedButtonUpten_std(CLaster *p)//上移10步
{p->OnBnClickedButtonUpten();}
void _stdcall OnBnClickedButtonLeftten_std(CLaster *p)//向左10步
{p->OnBnClickedButtonLeftten();}
void _stdcall OnBnClickedButtonDownten_std(CLaster *p)//下移10步
{p->OnBnClickedButtonDownten();}
void _stdcall OnBnClickedButtonRightten_std(CLaster *p)//向右10步
{p->OnBnClickedButtonRightten();}
void _stdcall OnBnClickedButtonHrzcalibration_std(CLaster *p)//水平标定
{p->OnBnClickedButtonHrzcalibration();}
void _stdcall OnBnClickedButtonCalibration_std(CLaster *p)//垂直标定
{p->OnBnClickedButtonCalibration();}
void _stdcall OnBnClickedButtonScanYard_std(CLaster *p)//站坪扫描
{p->OnBnClickedScanyard();}
void _stdcall OnBnClickedButtonScanYardToAdjust_std(CLaster *p)//站坪扫描后校准
{p->OnBnClickedScanyardToAdjust();}
void _stdcall OnBnClickedWalkset_std(CLaster *p)//走步设置
{p->OnBnClickedWalkset();}
void _stdcall OnBnClickedWalktest_std(CLaster *p)//走步测试
{p->OnBnClickedWalktest();}
void _stdcall OnBnClickedBtnClear_std(CLaster *p)//停止
{p->OnBnClickedBtnClear();}
void _stdcall OnBnClickedButtonConfirmwalkset_std(CLaster *p)//确认
{p->OnBnClickedButtonConfirmwalkset();}
void _stdcall OnBnClickedButtonClearwalkset_std(CLaster *p)//清空
{p->OnBnClickedButtonClearwalkset();}
//void _stdcall OnBnClickedStopline_std(CLaster *p)//编号
//{p->OnBnClickedStopline();}
void _stdcall OnBnClickedButtonReadlinenum_std(CLaster *p)//由编号读取停止线
{p->OnBnClickedButtonReadlinenum();}
void _stdcall OnBnClickedButtonComfirmtype_std(CLaster *p)//验证停止点
{p->OnBnClickedButtonComfirmtype();}
void _stdcall OnBnClickedSave_std(CLaster *p)//保存
{p->OnBnClickedSave();}
void _stdcall OnBnClickedButtonOffLineTest_std(CLaster *p)//离线测试
{p->OnBnClickedButtonOffLineTest();}
void _stdcall OnBnClickedButtonGuideLineMove_std(CLaster *p)//引导线整体移动
{p->OnBnClickedButtonGuideLineMove();}
void _stdcall OnBnClickedButtonStopLineMoveSingle_std(CLaster *p)//停止线单独移动
{p->OnBnClickedButtonStopLineMoveSingle();}
void _stdcall OnBnClickedButtonStopLineMove_std(CLaster *p)//停止线整体移动
{p->OnBnClickedButtonStopLineMove();}
void _stdcall OnBnClickedButtonBoostOn_std(CLaster *p)//Boost串口打开
{p->OpenLaserCOM();}
void _stdcall OnBnClickedButtonBoostClose_std(CLaster *p)//Boost串口关闭
{p->CloseLaserCOM();}

void _stdcall WriteLedMessage(int led_code,float led_dis,const char* led_time,const char* led_type)//Boost串口关闭
{
	if(glb_led_obj_ptr!=nullptr) glb_led_obj_ptr->sendmessage(led_code, led_dis, led_time, led_type);
}

bool _stdcall GetTCPIPStatus()//TCPIP连通状态
{
	if(glb_ptr_client!=NULL)
	{
		bool status = (glb_ptr_client->connected && glb_ptr_client->error_time==0);
		return status;
	}
	else return false;
}

bool _stdcall GetLEDStatus()//TCPIP连通状态
{
	if(glb_led_obj_ptr!= NULL && glb_led_obj_ptr->active_time != boost::posix_time::not_a_date_time)
	{
		bool status = (boost::posix_time::microsec_clock::local_time() - glb_led_obj_ptr->active_time < boost::posix_time::seconds(15));
		if(glb_led_obj_ptr!=nullptr) return status;
	}
	else
	{
		return false;
	}
}

//void _stdcall _std(CLaster *p)//
//{p->();}

void _stdcall transform1_2Struct_std(CLaster *p, needTransform1_2 *structDataCs, bool mode)
{
	//p->m_strRXData="co各方nte封二nt";p->walktext_deflection="De分为非tai分为非l2";//test
	//p->PlaneType4Confirm = "abc123";
	if(!mode)//Cpp-C#上行
	{
		structDataCs->install_high = p->install_high;
		structDataCs->object_high = p->object_high;
		structDataCs->object_dis = p->object_dis;
		structDataCs->Edit_Start_x = p->Edit_Start_x;
		structDataCs->Edit_End_x = p->Edit_End_x;
		structDataCs->Edit_StandHigh = p->Edit_StandHigh;
		structDataCs->Auto_StandHigh = p->Auto_StandHigh;
		structDataCs->mid_num = p->mid_num;
		structDataCs->guide_line_y = p->guide_line_y;
		//structDataCs->PlaneType4Confirm = p->PlaneType4Confirm;
		//structDataCs->LineNum4Confirm = p->LineNum4Confirm;
		if(structDataCs->PlaneType4Confirm) strcpy_s(structDataCs->PlaneType4Confirm , 20, p->PlaneType4Confirm);
		if(structDataCs->LineNum4Confirm) strcpy_s(structDataCs->LineNum4Confirm , 20, p->LineNum4Confirm);

		//以下都不需要下行
		//structDataCs->m_strRXData = p->m_strRXData;
		//structDataCs->walktext_deflection = p->walktext_deflection;
		if(structDataCs->m_strRXData) strcpy_s(structDataCs->m_strRXData, 50, p->m_strRXData);
		if(structDataCs->walktext_deflection) strcpy_s(structDataCs->walktext_deflection , 200,  p->walktext_deflection);

		structDataCs->guide_line_k = p->guide_line_k;
		structDataCs->guide_line_b = p->guide_line_b;
		structDataCs->guide_line_x = p->guide_line_x;

		structDataCs->x_walk = p->x_walk;//走步测试的位置，可以只上行
		structDataCs->y_walk = p->y_walk;
	}
	else//C#-Cpp下行
	{
		p->install_high = structDataCs->install_high;
		p->object_high = structDataCs->object_high;
		p->object_dis = structDataCs->object_dis;
		p->Edit_Start_x = structDataCs->Edit_Start_x;
		p->Edit_End_x = structDataCs->Edit_End_x;
		p->Edit_StandHigh = structDataCs->Edit_StandHigh;
		p->mid_num = structDataCs->mid_num;
		p->guide_line_y = structDataCs->guide_line_y;
		//以下字符串传递，待验证
		p->PlaneType4Confirm = structDataCs->PlaneType4Confirm;
		p->LineNum4Confirm = structDataCs->LineNum4Confirm;
		//strcpy(p->PlaneType4Confirm, structDataCs->PlaneType4Confirm);
		//strcpy(p->LineNum4Confirm, structDataCs->LineNum4Confirm );

		//以下只下行
		p->linenum.str_guidenum = structDataCs->str_guidenum;
		p->linenum.str_stopnum = structDataCs->str_stopnum;
		p->offsetforline = structDataCs->offsetforline;
	}

}
void _stdcall transform3Struct_std(CLaster *p, needTransform3 *structDataCs, bool mode)
{
	if(!mode)//Cpp-C#只上行
	{
		structDataCs->data_h = p->data_h;
		for(int i=0;i<p->data_h;i++)
		{
			structDataCs->save_data[i]=p->save_data[i];
		}
	}
	else//C#-Cpp
	{	}//int *a=new int[32];int b[]={1,2,3};
}

void _stdcall trans1pointerInt_std(CLaster* p, int *a, int idx)
{
	//p->pInt = a;
	//*p->pInt = 123;
	/*bool b = (p->refleshFlag2pointer);
	b = (NULL==p->refleshFlag2pointer);*/
	//int[] a=new int[10];
	switch(idx)
	{
	case 1:
		p->refleshFlag1pointer=a;//*p->refleshFlag1pointer=111;
		break;
	case 2:
		p->refleshFlag2pointer=a;//*p->refleshFlag2pointer=222;
		break;
	case 3:
		p->refleshFlag3pointer=a;//*p->refleshFlag3pointer=333;
		break;
	case 4:
		p->refleshFlag4pointer=a;//*p->refleshFlag4pointer=444;
		break;
	}
}

int _stdcall trans1Flag_std(CLaster* p, int idx)
{
	//p->pInt = a;
	//*p->pInt = 123;
	/*bool b = (p->refleshFlag2pointer);
	b = (NULL==p->refleshFlag2pointer);*/
	//int[] a=new int[10];
	switch(idx)
	{
	case 1:
		return p->refleshFlag1;//*p->refleshFlag1pointer=111;
		break;
	case 2:
		return p->refleshFlag2;//*p->refleshFlag2pointer=222;
		break;
	case 3:
		return p->refleshFlag3;//*p->refleshFlag3pointer=333;
		break;
	case 4:
		return p->refleshFlag4;//*p->refleshFlag4pointer=444;
		break;
	}
}

void _stdcall setFlagTo_std(CLaster* p, int flagValue, int idx)
{
	//p->pInt = a;
	//*p->pInt = 123;
	/*bool b = (p->refleshFlag2pointer);
	b = (NULL==p->refleshFlag2pointer);*/
	//int[] a=new int[10];
	switch(idx)
	{
	case 1:
		p->refleshFlag1=flagValue;//*p->refleshFlag1pointer=111;
		break;
	case 2:
		p->refleshFlag2=flagValue;//*p->refleshFlag2pointer=222;
		break;
	case 3:
		p->refleshFlag3=flagValue;//*p->refleshFlag3pointer=333;
		break;
	case 4:
		p->refleshFlag4=flagValue;//*p->refleshFlag4pointer=444;
		break;
	}
}
//namespace ManageCppDll
//{
	/*Laster_manage::Laster_manage()
	{
		m_pImp = new CLaster();
	}
	Laster_manage::~Laster_manage()
	{
		delete m_pImp;
	}

	Laster_manage::OnBnClickedButtonReadtype_manage()
	{
		m_pImp->OnBnClickedButtonReadtype();
	}*/

//};