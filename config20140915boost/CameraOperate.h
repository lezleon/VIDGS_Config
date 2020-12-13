#pragma once
#include"stdafx.h"
#include "PvApi.h"
#include <opencv2\opencv.hpp>
#include "ImageLib.h"
#include "opencv2/highgui/highgui.hpp"
using namespace cv;
using namespace std;

#define FRAMESCOUNT 3//֡��
#define ULONG_PADDING(x) (((x+3) & ~3) - x)
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
const int W =1936;//ͼ����
const int H = 1456;//ͼ��߶�


typedef struct 
{
	unsigned long   UID;
	tPvHandle       Handle;
	tPvFrame        Frame;
	tPvUint32       Counter;
	char            Filename[20];

} tCamera;//�����֡�ṹ
class CameraOperate
{
public:
	CameraOperate( );
	~CameraOperate( );
	bool CameraGet(tCamera* camera);
	bool CameraSetup(tCamera* camera);
	void WaitForCamera();
	void CameraStart(tCamera* camera);
	void CameraStop(tCamera* camera);
	IplImage* Bitmap2Ipl(BITMAPINFO* pBmpInfo,BYTE* pBits);//bmpͼ��ת��ΪIplImage��ʽ
	void evaluate(Mat mat);
	void Trans(Mat mat);
	void gamma(Mat mat,double b,double c,double r);


public:
	tPvErr Err;
	BYTE* m_Buffer;
	BYTE* buffer;
	BYTE *buffer1;
	BITMAPINFO m_pBmpInfo;
	tPvFrame* m_Frames;
	BITMAPINFO *info;
	int frame;
	tPvCameraInfoEx list;
	tPvUint32 m_BytesPerFrame;
	ULONG remainder;
	Mat image_one;
	Mat mean,sdv;
	tPvUint32 exposuretime;;
	tPvUint32 gain;
};
