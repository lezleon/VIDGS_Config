// configView.h : CconfigView 类的接口
//


#pragma once

#define  RECOG_EXPORTS
#ifdef RECOG_EXPORTS
#define RECOG_API extern "C" __declspec(dllexport)
#else
#define RECOG_API extern "C" __declspec(dllimport)
#endif

#include <afxpriv.h>
#include <afxtempl.h>
#include "Digistatic\Digistatic.h"
#include "ButtonST\BtnST.h"

#include "cv.h"
#include "highgui.h"
#include "cxcore.h"
#include "configDoc.h"
#include "atltypes.h"
#include "PlaneType.h"
#include "StationNum.h"
#include "GuideLineNum.h"
#include "LineNum.h"
#include "PointNum.h"
#include "CameraOperate.h"
#include "Doc.h"
#include "Laster.h"
#include"ChoosePoint.h"
#include "PlaneData.h"
#include "DLG_SETTING.h"
#include "LED.h"
#include "TCPIPclient.h"

//#include <vcclr.h>

using namespace cv;  
using namespace std;  
//using namespace System;
#define NUM_RECTTRACKER 7
//#define _AFXDLL

typedef CArray<CRectTracker*, CRectTracker*> CTrackerArray;
class CconfigView : public CScrollView
{
protected: // 仅从序列化创建
	CconfigView();
	DECLARE_DYNCREATE(CconfigView)

// 属性
public:
	CconfigDoc* GetDocument() const;
	virtual void OnInitialUpdate();
	

public:
	IplImage *static_image;
	IplImage*static_image_one;
	CString filename;
	BITMAPINFO* m_pBmpInfo_out;
	BYTE* m_pBits_out;
	double m_zoom;
	
public:
	void Ipl2Bitmap(IplImage *Iplmg);
	IplImage* Bitmap2Ipl(BITMAPINFO* pBmpInfo,void* pBits);
	double GetZoom();
	void SetZoom(double zoom);
	void UpdateWnd();
	void Readini();
	void CanvasToVirtual( CPoint& point );
	void CaptureRect(CRect Rect_input);
	void TrackRect(CRect Rect_input);
	void BridgeRect(CRect Rect_input);
	//void IdentifyRect(CRect Rect_input);

	CTrackerArray m_TrackerArray;

	BOOL m_bDrawState;
	void SetDrawState(BOOL bDraw)
	{
		m_bDrawState = bDraw;
	}
	BOOL GetDrawState()const
	{
		return m_bDrawState;
	}

	CRectTracker* m_pCurRect;
	CRectTracker* HitTestTracker(const CPoint& pt);

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);


// 实现
public:
	virtual ~CconfigView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnOpenimage();
	afx_msg void OnBnClickedBtnExit();
	afx_msg void OnBnClickedBtnDrawselrect();
	afx_msg void OnUpdateDrawRectBtn(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnBnClickedBtnDelselrect();
	afx_msg void OnUpdateDelRectBtn(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDelLineBtn(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDelPointBtn(CCmdUI* pCmdUI);
	afx_msg void OnBnClickedCapturerect();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
public:
	BOOL m_bDraw;
	BOOL m_DrawPoint;
	BOOL m_DrawPoint1;
	CPoint m_ptOrigin;
	CPoint m_start;
	CPoint m_end;
	int count;
	BOOL m_ExitImg;
	int m_CameraMode;

	Document Doc;
	string routename;
	BOOL buttonup;
	BOOL m_Imagesave;

public:
	afx_msg void OnBnClickedDrawline();
	afx_msg void OnBnClickedLineconfirm();
	afx_msg void OnBnClickedTrack();
	afx_msg void OnBnClickedStopline();
	afx_msg void OnBnClickedBridge();
	afx_msg void OnBnClickedDelline();
	afx_msg void OnBnClickedPlanetype();

public:
	tCamera m_Camera;
	CameraOperate camoperate;

public:
	CGuideLineNum guideLine;
	CLineNum linenum;
	CPointNum pointnum;
	int insertnum;
	CChoosePoint choosepoint;
	vector<pair<double,Point>>VLinePoints1;
	vector<pair<double,Point>>VLinePoints2;
	vector<pair<double,Point>>VLinePoints3;
	vector<pair<double,Point>>VLinePoints4;
	vector<pair<double,Point>>VLinePoints5;

	CvPoint guide1_start,guide1_end,guide2_start,guide2_end;
	CvPoint stop11_start,stop11_end,stop12_start,stop12_end,stop13_start,stop13_end;
	CvPoint stop21_start,stop21_end,stop22_start,stop22_end,stop23_start,stop23_end;
	CvRect searchrect,trackrect,bridgerect;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedDrawpoint();
	afx_msg void OnBnClickedDelpoint();
	afx_msg void OnBnClickedPointconfim();
	afx_msg void OnBnClickedEnlarge();
	afx_msg void OnBnClickedShrink();
	afx_msg void OnStartvideo();
	afx_msg void OnStopvideo();
	afx_msg void OnExitvideo();
	afx_msg void OnExitimg();
	afx_msg void OnOpenimg();
	afx_msg void Onstation();
	afx_msg void OnLaster();

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLastconfig();
	afx_msg void OnReconfig();
	afx_msg void OnSaveimage();
	afx_msg void OnPlanedata();
	afx_msg void OnCOMSetting();
};

#ifndef _DEBUG  // configView.cpp 中的调试版本
inline CconfigDoc* CconfigView::GetDocument() const
   { return reinterpret_cast<CconfigDoc*>(m_pDocument); }
#endif

//namespace ManageCppDll
//{
	//public ref class Laster_manage
	//{
	//public:
	//	Laster_manage();
	//	~Laster_manage();

	//	property int Age;
	//	OnBnClickedButtonReadtype_manage();

	//private:
	//	CLaster *m_pImp;
	//};
//};






RECOG_API CLaster* _stdcall classLaserInit();//{return new CLaster();};//类对象
RECOG_API void _stdcall classLaserDestroy(CLaster *p);//{return new CLaster();};//类对象
RECOG_API void _stdcall OnBnClickedConnect_std(CLaster *p);//{p->OnBnClickedConnect();};//启动按钮
RECOG_API void _stdcall OnBnClickedButtonReadtype_std(CLaster *p);//由机型读取停止线
RECOG_API void _stdcall OnBnClickedButtonReadlinenum_std(CLaster *p);//由编号读取停止线
RECOG_API void _stdcall OnBnClickedStopline_std(CLaster *p);//编号

RECOG_API void _stdcall OnBnClickedButtonDetect_std(CLaster *p);//校准
RECOG_API void _stdcall OnBnClickedButtonReadedge_std(CLaster *p);//读取边框
RECOG_API void _stdcall OnBnClickedButtonLeft_std(CLaster *p);//左移1步
RECOG_API void _stdcall OnBnClickedButtonRight_std(CLaster *p);//右移1步
RECOG_API void _stdcall OnBnClickedButtonDown_std(CLaster *p);//下移1步
RECOG_API void _stdcall OnBnClickedButtonUp_std(CLaster *p);//上移1步
RECOG_API void _stdcall OnBnClickedButtonLocate_std(CLaster *p);//单点测量
RECOG_API void _stdcall OnBnClickedButtonRedlaseron_std(CLaster *p);//打开红色激光
RECOG_API void _stdcall OnBnClickedButtonRedlaseroff_std(CLaster *p);//关闭红色激光
RECOG_API void _stdcall OnBnClickedButtonScanall_std(CLaster *p);//窗口扫描
RECOG_API void _stdcall OnBnClickedButtonEdge_std(CLaster *p);//确定边框
RECOG_API void _stdcall OnBnClickedButtonSetzero_std(CLaster *p);//置为零点
RECOG_API void _stdcall OnBnClickedButtonReset_std(CLaster *p);//回到零点
RECOG_API void _stdcall OnBnClickedButtonTwod_std(CLaster *p);//二维扫描
RECOG_API void _stdcall OnBnClickedButtonUpten_std(CLaster *p);//上移10步
RECOG_API void _stdcall OnBnClickedButtonLeftten_std(CLaster *p);//向左10步
RECOG_API void _stdcall OnBnClickedButtonDownten_std(CLaster *p);//下移10步
RECOG_API void _stdcall OnBnClickedButtonRightten_std(CLaster *p);//向右10步
RECOG_API void _stdcall OnBnClickedButtonHrzcalibration_std(CLaster *p);//水平标定
RECOG_API void _stdcall OnBnClickedButtonCalibration_std(CLaster *p);//垂直标定
RECOG_API void _stdcall OnBnClickedButtonScanYard_std(CLaster *p);//站坪扫描
RECOG_API void _stdcall OnBnClickedButtonScanYardToAdjust_std(CLaster *p);//站坪扫描后校准
RECOG_API void _stdcall OnBnClickedWalkset_std(CLaster *p);//走步设置
RECOG_API void _stdcall OnBnClickedWalktest_std(CLaster *p);//走步测试
RECOG_API void _stdcall OnBnClickedBtnClear_std(CLaster *p);//停止
RECOG_API void _stdcall OnBnClickedButtonConfirmwalkset_std(CLaster *p);//确认
RECOG_API void _stdcall OnBnClickedButtonClearwalkset_std(CLaster *p);//清空
//RECOG_API void _stdcall OnBnClickedStopline_std(CLaster *p);//编号
RECOG_API void _stdcall OnBnClickedButtonComfirmtype_std(CLaster *p);//验证停止点
RECOG_API void _stdcall OnBnClickedSave_std(CLaster *p);//保存
//RECOG_API void _stdcall _std(CLaster *p);//

RECOG_API void _stdcall transform1_2Struct_std(CLaster *p, needTransform1_2 *structDataCs, bool mode);
RECOG_API void _stdcall transform3Struct_std(CLaster *p, needTransform3 *structDataCs, bool mode);

RECOG_API void _stdcall trans1pointerInt_std(CLaster *p, int *a, int idx);
RECOG_API int _stdcall trans1Flag_std(CLaster *p, int idx);
RECOG_API void _stdcall setFlagTo_std(CLaster *p, int flagValue, int idx);
RECOG_API void _stdcall OnBnClickedButtonOffLineTest_std(CLaster *p);//离线测试
RECOG_API void _stdcall OnBnClickedButtonGuideLineMove_std(CLaster *p);//引导线整体移动
RECOG_API void _stdcall OnBnClickedButtonStopLineMoveSingle_std(CLaster *p);//停止线单独移动
RECOG_API void _stdcall OnBnClickedButtonStopLineMove_std(CLaster *p);//停止线整体移动

RECOG_API void _stdcall OnBnClickedButtonBoostOn_std(CLaster *p);//Boost串口打开
RECOG_API void _stdcall OnBnClickedButtonBoostClose_std(CLaster *p);//Boost串口关闭

RECOG_API void _stdcall ReadConfirmProbability(CLaster *p);//Boost串口关闭
RECOG_API void _stdcall WriteLedMessage(int led_code,float led_dis,const char* led_time,const char* led_type);//Boost串口关闭

RECOG_API bool _stdcall GetTCPIPStatus();//TCPIP连通状态
RECOG_API bool _stdcall GetLEDStatus();//LED连通状态 