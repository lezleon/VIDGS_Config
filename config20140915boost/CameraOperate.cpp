#pragma once
#include "stdafx.h"
#include "CameraOperate.h"
#include "configView.h"
#include "configDoc.h"
#include "MainFrm.h"

//void __stdcall FrameDoneCB(tPvFrame* pFrame);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void __stdcall FrameDoneCB(tPvFrame* pFrame);
CameraOperate::CameraOperate( )
{
	m_Buffer = NULL;
	frame = 0;
	exposuretime=1000;
	gain=0;
	 //BMP文件的信息头结构
	m_pBmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	m_pBmpInfo.bmiHeader.biWidth=W;
	m_pBmpInfo.bmiHeader.biHeight=H;
	m_pBmpInfo.bmiHeader.biPlanes=1;
	m_pBmpInfo.bmiHeader.biXPelsPerMeter=0;
	m_pBmpInfo.bmiHeader.biYPelsPerMeter=0;
	m_pBmpInfo.bmiHeader.biClrUsed=0;
	m_pBmpInfo.bmiHeader.biClrUsed=0;
	m_pBmpInfo.bmiHeader.biClrImportant=0;
	m_pBmpInfo.bmiHeader.biCompression=0;
	m_pBmpInfo.bmiHeader.biBitCount=24;
	//为图像信息存储分配内存
	m_Buffer=(BYTE *)malloc((((W * 3) + 3) & ~3) * H );
	buffer=(BYTE *)malloc((((W * 3) + 3) & ~3) * H);
	buffer1=(BYTE *)malloc((((W * 3) + 3) & ~3) * H);
	//定义帧结构
	m_Frames=(tPvFrame*)malloc(FRAMESCOUNT*sizeof(tPvFrame));
	if(m_Frames)
		memset(m_Frames,0,sizeof(tPvFrame) * FRAMESCOUNT);

	info=(BITMAPINFO *)malloc(sizeof(BITMAPINFO)+sizeof(RGBQUAD)*256);
	info->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	info->bmiHeader.biPlanes=1;
	info->bmiHeader.biWidth=W;
	info->bmiHeader.biHeight=H;
	info->bmiHeader.biXPelsPerMeter=0;
	info->bmiHeader.biYPelsPerMeter=0;
	info->bmiHeader.biClrUsed=0;
	info->bmiHeader.biClrImportant=0;
	info->bmiHeader.biCompression=0;
    info->bmiHeader.biBitCount=8;
	for (int k=0;k<256;k++)
	{
		info->bmiColors[k].rgbBlue=k;
		info->bmiColors[k].rgbGreen=k;
		info->bmiColors[k].rgbRed=k;
		info->bmiColors[k].rgbReserved=0;
	}

}


CameraOperate::~CameraOperate( )
{
	for(UINT i=0;i<FRAMESCOUNT;i++)
		//delete [] m_Frames[i].ImageBuffer;
		free(m_Frames[i].ImageBuffer);
	//delete [] m_Frames;
	free(m_Frames);
	free(m_Buffer); 
	free(buffer);
	free(buffer1);
	free(info);
}

/*************************************************************************
 *
 * 函数名称：
 *   CameraGet
 *
 * 参数:
 *  tCamera* camera		相机结构
 *  
 *
 * 返回值:
 * true		得到相机信息，返回true
 * false	未找到相机，返回false
 *
 * 说明:
 *   该函数主要用来查找并得到相机的信息
 *
 ************************************************************************/

bool CameraOperate::CameraGet(tCamera* camera)
{
	tPvUint32 count,connected;
	WaitForCamera();
	count = PvCameraListEx(&list,1,&connected, sizeof(tPvCameraInfoEx));
	if(count == 1)
	{
		camera->UID = list.UniqueId;
		return true;
	}
	else
	{

		MessageBox(NULL,"没有找到相机！","提示",MB_OK);
		return false;
	}
	return false;
}



bool CameraOperate::CameraSetup(tCamera* camera)
{
	if ((Err = PvCameraOpen(camera->UID,ePvAccessMaster,&(camera->Handle))) != ePvErrSuccess)
	{
		if (Err == ePvErrAccessDenied)
			MessageBox(NULL,"相机打开失败！","提示",MB_OK);
		return false;
	}
	//Err=PvAttrEnumSet(camera->Handle,"PixelFormat","Bayer8");
	Err=PvAttrEnumSet(camera->Handle,"PixelFormat","Mono8");
	Err = PvAttrEnumSet(camera->Handle, "AcquisitionMode", "Continuous");
	Err = PvAttrEnumSet(camera->Handle, "FrameStartTriggerMode", "Freerun");
	PvAttrUint32Set(camera->Handle,"ExposureValue",exposuretime);
	PvAttrUint32Set(camera->Handle,"GainValue",gain);
	PvAttrEnumSet(camera->Handle,"IrisMode","DCIris");
	PvAttrUint32Set(camera->Handle,"IrisVideoLevel",100);
	PvAttrUint32Set(camera->Handle,"IrisAutoTarget",50);
	PvAttrUint32Set(camera->Handle,"LensDCDriveStrength",10);

	return true;
}


void CameraOperate::CameraStart(tCamera* camera)
{
	ULONG   Capturing;

	if(!(Err = PvCaptureQuery(camera->Handle, &Capturing)))    // is the camera IDLE?
	{
		if(!Capturing)
		{
			if(!(Err = PvCaptureStart(camera->Handle)))   
			{
				 if(!(Err = PvAttrUint32Get(camera->Handle,"TotalBytesPerFrame",&m_BytesPerFrame)))
				 {
					// PvAttrUint32Set(camera->Handle,"ExposureValue",4*1000);
					 if(m_BytesPerFrame)
					 {
						  for(int i=0;i<FRAMESCOUNT && !Err;i++)
						  {
							  //m_Frames[i].ImageBuffer = new BYTE[m_BytesPerFrame];
							  m_Frames[i].ImageBuffer = (BYTE*)malloc(m_BytesPerFrame);
							  if(!m_Frames[i].ImageBuffer)
								  Err = ePvErrResources;
							  else
							  {
								  m_Frames[i].ImageBufferSize = m_BytesPerFrame;

							  }
						  }

					 }
					 if(!Err)
					 {
						 for(int i=0;i<FRAMESCOUNT && !Err;i++)
							 Err = PvCaptureQueueFrame(camera->Handle,&m_Frames[i],FrameDoneCB);
						 if(!Err)
						 {
							 Err = PvCommandRun(camera->Handle,"AcquisitionStart");
							 if(Err && Err != ePvErrForbidden)
								 MessageBox(NULL,"CameraStart: failed to set camera attributes！","提示",MB_OK);

						 }
						 else
						 {
							 PvCaptureQueueClear(camera->Handle);
						 }

					 }

				 }
			}
		}
	}
}


void CameraOperate::CameraStop(tCamera* camera)
{
	PvCommandRun(camera->Handle,"AcquisitionStop");
	PvCaptureEnd(camera->Handle);
	PvCaptureQueueClear(camera->Handle);
}
int i=0;
void __stdcall FrameDoneCB(tPvFrame* pFrame)
{
	//CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	//CconfigView* Window = (CconfigView*)pMainFrm->GetActiveView();
	CconfigView* Window=(CconfigView*)((CFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveView(); 

	switch(pFrame->Format)
	{
	case ePvFmtBayer8:
		{
			Window->camoperate.remainder = ULONG_PADDING(W * 3);

			PvUtilityColorInterpolate(pFrame, &Window->camoperate.m_Buffer[2],  &Window->camoperate.m_Buffer[1], &Window->camoperate.m_Buffer[0], 2, Window->camoperate.remainder);

			break;
		}
	case ePvFmtMono8:
		{
			Mat image(pFrame->Height,pFrame->Width,CV_8UC1,pFrame->ImageBuffer);
			
			Window->camoperate.image_one=image.clone();
		}
	}
	//int nWidthBytes = WIDTHBYTES((Window->camoperate.info->bmiHeader.biWidth)*(Window->camoperate.info->bmiHeader.biBitCount));//图像每行的数据信息量
	//for (int i=0;i<H;i++)
	//	{
	//		for (int j=0;j<nWidthBytes;j++)
	//		{
	//			//对图像数据进行坐标转换，使显示的图像符合人的视觉习惯
	//			Window->camoperate.buffer[i*nWidthBytes+j]=Window->camoperate.m_Buffer[(H-i-1)*nWidthBytes+j];
	//				
	//		}
	//	}
	//Mat image_one(pFrame->Height,pFrame->Width,CV_8UC3,Window->camoperate.m_Buffer);
	
	CDC* pDC = Window->GetDC();
	SetStretchBltMode(pDC->GetSafeHdc(),COLORONCOLOR);
	CRect rcClient;
	Window->GetClientRect(&rcClient);

	flip(Window->camoperate.image_one,Window->camoperate.image_one,0);
	Window->m_Camera.Counter++;
	/*PvAttrEnumSet(Window->m_Camera.Handle,"IrisMode","DCIris");
	PvAttrUint32Set(Window->m_Camera.Handle,"IrisVideoLevel",20);
	PvAttrUint32Set(Window->m_Camera.Handle,"IrisAutoTarget",50);
	PvAttrUint32Set(Window->m_Camera.Handle,"LensDCDriveStrength",10);*/

	Window->camoperate.evaluate(Window->camoperate.image_one);


	/*if (i>200)
	{
		PvAttrEnumSet(Window->m_Camera.Handle,"IrisMode","Disabled");
	}*/

	CSize sizeImg(Window->camoperate.m_pBmpInfo.bmiHeader.biWidth, ::abs( Window->camoperate.m_pBmpInfo.bmiHeader.biHeight ));
	CPoint viewportorg(0,0);

	if(rcClient.right<=(long)(Window->GetZoom()*sizeImg.cx))
		viewportorg.x=0;
	else
		viewportorg.x=(long)((rcClient.right-Window->GetZoom()*sizeImg.cx)/2);

	if(rcClient.bottom<=(long)(Window->GetZoom()*sizeImg.cy))
		viewportorg.y=0;
	else
		viewportorg.y=(long)((rcClient.bottom - Window->GetZoom()*sizeImg.cy)/2);
		

	StretchDIBits(pDC->GetSafeHdc(),
		/*0,						
		0,
		rcClient.Width(),
		rcClient.Height(),*/
		viewportorg.x, 
		viewportorg.y,
		Window->GetZoom()*sizeImg.cx,	// Dest. rectangle width.
		Window->GetZoom()*sizeImg.cy,	// Dest. rectangle height.
		0,
		0,
		pFrame->Width,					//图像宽度
		pFrame->Height,					//图像高度
		Window->camoperate.image_one.data,//Window->camoperate.buffer,			//图像缓冲区
		//&Window->camoperate.m_pBmpInfo,				//BMP图像描述信息
		Window->camoperate.info,
		DIB_RGB_COLORS,
		SRCCOPY
		);
	if (Window->m_Imagesave)
	{
		Mat temImage;
		flip(Window->camoperate.image_one,temImage,0);
		sprintf_s(Window->m_Camera.Filename,sizeof(Window->m_Camera.Filename),".\\snap.bmp");
		imwrite(Window->m_Camera.Filename,temImage);
		Window->m_Imagesave=FALSE;
	}
	
		//调用回调函数进行下一帧的处理
	PvCaptureQueueFrame(Window->m_Camera.Handle,pFrame,FrameDoneCB);
}

void CameraOperate::WaitForCamera()
{
	while(PvCameraCount() == 0)
	{
		Sleep(20);
	}
}

void CameraOperate::evaluate(Mat mat)
{
	CconfigView* Window=(CconfigView*)((CFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveView(); 
	frame=Window->m_Camera.Counter;
	if (frame<=200)
	{
		meanStdDev(mat,mean,sdv);
		TRACE("%d %f %f\n",frame,mean.ptr<double>(0)[0],sdv.ptr<double>(0)[0]);
	}
	/*else
	{
	PvAttrEnumSet(Window->m_Camera.Handle,"IrisMode","Disabled");
	}
	

	double gray=mean.ptr<double>(0)[0];\

	if (mean.ptr<double>(0)[0]<=100&&mean.ptr<double>(0)[0]>=30)
	{
	PvAttrEnumSet(Window->m_Camera.Handle,"IrisMode","Disabled");
	}*/

	////PvAttrUint32Get(m_Camera.Handle,"ExposureValue",&exposuretime);
	////PvAttrUint32Get(m_Camera.Handle,"GainValue",&gain);

	
	CString str;
	if (mean.ptr<double>(0)[0]<60)
	{
		//低照度情况
		str=_T("低照度");

		if (frame<=200)
		{
			//自动增加曝光时间或增益来调整图像亮度
			if (exposuretime>=40000)
			{
				exposuretime=40000;
				if (gain<10)
				{
					gain++;
				}
				else
					gain=10;
			}
			else if (exposuretime<=10)
			{
				exposuretime=10;
			}
			else if(10000<=exposuretime&&exposuretime<40000) 
			{
				exposuretime=exposuretime+1000;
				gain=0;
			}
			else if(1000<=exposuretime&&exposuretime<10000) 
			{
				exposuretime=exposuretime+100;
				gain=0;
			}
			else if(exposuretime>=100&&exposuretime<1000)
			{
				exposuretime=exposuretime+10;
				gain=0;
			}
			else if(exposuretime>=10&&exposuretime<100)
			{
				exposuretime=exposuretime+1;
				gain=0;
			}
		}
		if (exposuretime==40000&&gain==10)
		{
			//当曝光时间和增益调整到最大值后，对低照度图像进行非线性变换调整图像质量
			if (mean.ptr<double>(0)[0]<20)
			{
				Window->MessageBox("图像过暗，请调整光圈！");
			}
			else
				Trans(mat);

		}

	}
	else if (mean.ptr<double>(0)[0]>120)
	{
		//强光照情况
		str=_T("强光");

		if (frame<=200)
		{
			//自动降低曝光时间和增益来降低图像亮度
			if (gain>0)
			{
				gain=0;
			}
			else
			{
				if (exposuretime>40000)
				{
					exposuretime=40000;
				}
				else if (exposuretime<=10)
				{
					exposuretime=10;
				}
				else if(10000<exposuretime&&exposuretime<=40000)
				{
					exposuretime=exposuretime-1000;
				}
				else if(1000<exposuretime&&exposuretime<=10000)
				{
					exposuretime=exposuretime-100;
				}
				else if(exposuretime>100&&exposuretime<=1000)
				{
					exposuretime=exposuretime-10;
				}
				else if(exposuretime>10&&exposuretime<=100)
				{
					exposuretime=exposuretime-1;
				}

			}

		}
		if (exposuretime==10&&gain==0)
		{
			//当无法对曝光时间和增益进行调整时对图像进行gamma变换来调整图像亮度
			//Window->process.gamma(image_second,8.0,0.9,2.0);
			if (mean.ptr<double>(0)[0]>160)
			{
				Window->MessageBox("图像过亮，请调整光圈！");
			}
			else
				gamma(mat,2.0,0.9,1.5);

		}

	}
	else if (mean.ptr<double>(0)[0]<=120&&mean.ptr<double>(0)[0]>=60)
	{
		//正常光照情况，不对图像进行任何处理
		str=_T("正常光照");

	}
	PvAttrUint32Set(Window->m_Camera.Handle,"ExposureValue",exposuretime);
	PvAttrUint32Set(Window->m_Camera.Handle,"GainValue",gain);

}

/*************************************************************************
 *
 * 函数名称：
 *	Trans
 *
 * 参数:
 *  BITMAPINFO* m_pBmpInfo	图像信息头
 *  BYTE* buffer			图像数据
 *  
 * 返回值:
 *	buffer				经过变换校正之后的图像数据
 *
 * 说明:
 *   该函数主要用来对得到的图像数据进行非线性变换。
 ************************************************************************/
void CameraOperate::Trans(Mat mat)
{

	//for (int i=0;i<mat.rows;i++)
	//{
	//	for (int j=0;j<mat.cols;j++)
	//	{
	//		Vec3i bgr = mat.at<Vec3b>(i,j);
	//		for(int k=0;k<3;k++)
	//		{	float t=bgr.val[k];
	//		
	//		t = t+0.02*t*(255-t);
	//		if(t > 255)
	//			t = 255;
	//		else if(t < 0)
	//			t = 0;
	//		bgr.val[k]=(unsigned char)t;
	//		}
	//		mat.at<Vec3b>(i,j)=bgr;  

	//	}
	//	
	//}
	Mat lookUpTable=Mat(256,1,CV_8U);
	for(int i=0;i<256;i++)
	{
		float t=i+0.01*i*(255-i);
		if(t>255) t=255;
		if(t<0) t=0;
		lookUpTable.at<uchar>(i,0)=t;
	}
	LUT(mat,lookUpTable,mat);
	//equalizeHist(mat,mat);
	//medianBlur(mat,mat,3);

}

/*************************************************************************
 *
 * 函数名称：
 *  gamma
 *
 * 参数:
 *  BITMAPINFO* m_pBmpInfo	图像信息头
 *  BYTE* buffer			图像数据
 *  double b				参数b
 *	double c				参数c
 *	double r				参数r
 * 返回值:
 *	buffer				经过gamma校正之后的图像数据
 *
 * 说明:
 *   该函数主要用来对得到的图像数据进行gamma校正
 ************************************************************************/
void CameraOperate::gamma(Mat mat,double b,double c,double r)
{
	//for (int i=0;i<mat.rows;i++)
	//{
	//	for (int j=0;j<mat.cols;j++)
	//	{
	//		Vec3i bgr = mat.at<Vec3b>(i,j);
	//		for(int k=0;k<3;k++)
	//		{	float t=bgr.val[k];
	//		
	//		//强光
	//		t = c* pow(t / 255.0,r) * 255.0 -b;
	//		if(t > 255)
	//			t = 255;
	//		else if(t < 0)
	//			t = 0;
	//		bgr.val[k]=(unsigned char)t;

	//		}
	//		mat.at<Vec3b>(i,j)=bgr;  

	//	}
	//}


	Mat lookUpTable=Mat(256,1,CV_8U);
	for(int i=0;i<256;i++)
	{
		uchar t=(c* pow(i / 255.0,r) * 255.0 -b);
		if(t>255) t=255;
		if(t<0) t=0;
		lookUpTable.at<uchar>(i,0)=t;
	}
	LUT(mat,lookUpTable,mat);

//equalizeHist(mat,mat);
}
