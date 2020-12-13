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
	 //BMP�ļ�����Ϣͷ�ṹ
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
	//Ϊͼ����Ϣ�洢�����ڴ�
	m_Buffer=(BYTE *)malloc((((W * 3) + 3) & ~3) * H );
	buffer=(BYTE *)malloc((((W * 3) + 3) & ~3) * H);
	buffer1=(BYTE *)malloc((((W * 3) + 3) & ~3) * H);
	//����֡�ṹ
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
 * �������ƣ�
 *   CameraGet
 *
 * ����:
 *  tCamera* camera		����ṹ
 *  
 *
 * ����ֵ:
 * true		�õ������Ϣ������true
 * false	δ�ҵ����������false
 *
 * ˵��:
 *   �ú�����Ҫ�������Ҳ��õ��������Ϣ
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

		MessageBox(NULL,"û���ҵ������","��ʾ",MB_OK);
		return false;
	}
	return false;
}



bool CameraOperate::CameraSetup(tCamera* camera)
{
	if ((Err = PvCameraOpen(camera->UID,ePvAccessMaster,&(camera->Handle))) != ePvErrSuccess)
	{
		if (Err == ePvErrAccessDenied)
			MessageBox(NULL,"�����ʧ�ܣ�","��ʾ",MB_OK);
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
								 MessageBox(NULL,"CameraStart: failed to set camera attributes��","��ʾ",MB_OK);

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
	//int nWidthBytes = WIDTHBYTES((Window->camoperate.info->bmiHeader.biWidth)*(Window->camoperate.info->bmiHeader.biBitCount));//ͼ��ÿ�е�������Ϣ��
	//for (int i=0;i<H;i++)
	//	{
	//		for (int j=0;j<nWidthBytes;j++)
	//		{
	//			//��ͼ�����ݽ�������ת����ʹ��ʾ��ͼ������˵��Ӿ�ϰ��
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
		pFrame->Width,					//ͼ����
		pFrame->Height,					//ͼ��߶�
		Window->camoperate.image_one.data,//Window->camoperate.buffer,			//ͼ�񻺳���
		//&Window->camoperate.m_pBmpInfo,				//BMPͼ��������Ϣ
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
	
		//���ûص�����������һ֡�Ĵ���
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
		//���ն����
		str=_T("���ն�");

		if (frame<=200)
		{
			//�Զ������ع�ʱ�������������ͼ������
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
			//���ع�ʱ���������������ֵ�󣬶Ե��ն�ͼ����з����Ա任����ͼ������
			if (mean.ptr<double>(0)[0]<20)
			{
				Window->MessageBox("ͼ��������������Ȧ��");
			}
			else
				Trans(mat);

		}

	}
	else if (mean.ptr<double>(0)[0]>120)
	{
		//ǿ�������
		str=_T("ǿ��");

		if (frame<=200)
		{
			//�Զ������ع�ʱ�������������ͼ������
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
			//���޷����ع�ʱ���������е���ʱ��ͼ�����gamma�任������ͼ������
			//Window->process.gamma(image_second,8.0,0.9,2.0);
			if (mean.ptr<double>(0)[0]>160)
			{
				Window->MessageBox("ͼ��������������Ȧ��");
			}
			else
				gamma(mat,2.0,0.9,1.5);

		}

	}
	else if (mean.ptr<double>(0)[0]<=120&&mean.ptr<double>(0)[0]>=60)
	{
		//�����������������ͼ������κδ���
		str=_T("��������");

	}
	PvAttrUint32Set(Window->m_Camera.Handle,"ExposureValue",exposuretime);
	PvAttrUint32Set(Window->m_Camera.Handle,"GainValue",gain);

}

/*************************************************************************
 *
 * �������ƣ�
 *	Trans
 *
 * ����:
 *  BITMAPINFO* m_pBmpInfo	ͼ����Ϣͷ
 *  BYTE* buffer			ͼ������
 *  
 * ����ֵ:
 *	buffer				�����任У��֮���ͼ������
 *
 * ˵��:
 *   �ú�����Ҫ�����Եõ���ͼ�����ݽ��з����Ա任��
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
 * �������ƣ�
 *  gamma
 *
 * ����:
 *  BITMAPINFO* m_pBmpInfo	ͼ����Ϣͷ
 *  BYTE* buffer			ͼ������
 *  double b				����b
 *	double c				����c
 *	double r				����r
 * ����ֵ:
 *	buffer				����gammaУ��֮���ͼ������
 *
 * ˵��:
 *   �ú�����Ҫ�����Եõ���ͼ�����ݽ���gammaУ��
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
	//		//ǿ��
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
