//将此文件内的所有 this-> 替换成 this-> （本文件中本身不含任何 this->）
//boost未解决：缓冲区长度；短命令返回
// Laster.cpp : 实现文件
//使用5月15日11:30的文件
//缺：-7；校准完成的显示；“原程序为10”

#include "stdafx.h"
#include "config.h"
#include "Laster.h"
#include "afxdialogex.h"
#include <math.h>
#include <Windows.h>
#include <time.h>
#include <direct.h>
#include <limits.h>
#include "VIDGS_serial.h"
#include "LED.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define  RECOG_EXPORTS

#define PI       3.14159265358979323846
#define END_LINE 123456
#define MAXDISTANCE 300000
#define VERTICAL 2     //垂直扫描	
#define HORIZONTAL 1   //水平扫描
#define CHANGE 3       //转换扫描模式
#define VERTICAL_STEP 70 //垂直扫描步数
#define CATCH_DIS_MIN 1000//捕获范围最小值
#define CATCH_DIS_MAX 1000//捕获范围最大值
#define DRAW_10M 10000//10m
#define TEST_THREAD 0//测试启动线程时为1，正常使用为0
#define UpdateData() TRUE;//禁用
#define OFFLINE_SML_MODE 1//离线数据模拟成真实数据的模式，1（上层）在OnComm上模拟，2（底层）在LaserRead上模拟
#define SAVE_DATA_WHEN_OFFLINE 0//离线测试中是否保存数据文件
//#define //GetDlgItem() TRUE;//禁用
#define AUTO_STOP 0//自动停止走不设置和走不测试
#define OFFLINE_SLEEP_TIME 100
#define XSTEPANGLE 0.1125
#define YSTEPANGLE 0.05625
#define USE_BACK 0

boost::asio::io_service laser_iosev, tcpip_iosev;
void laser_iosev_run() {
  laser_iosev.run();
}
extern boost::asio::io_service led_iosev;
boost::thread* laser_iosev_thread;
boost::shared_ptr<boost::asio::serial_port > glb_sp_ptr;
commu_interface glb_sp_data;//commu_interface定义在VIDGS_serial.h
boost::shared_ptr<CLaster> glb_laser_ptr;
boost::thread_group glb_thread_group;

// CLaster 对话框
IMPLEMENT_DYNAMIC(CLaster, CDialog)

CLaster::CLaster(CWnd* pParent /*=NULL*/)
  : CDialog(CLaster::IDD, pParent)
  , PlaneType4Confirm(_T(""))//这一行代码是怎么来的？？？
{
  //UpdateData(0);//test
  //MessageBox("初始化开始");
  //**********  dll ***********
  refleshFlag[0] = 1;
  refleshFlag[1] = refleshFlag[2] = refleshFlag[3] = 0;
  refleshFlag1 = refleshFlag2 = refleshFlag3 = refleshFlag4 = 0;//*refleshFlag4pointer=0;
  refleshFlag1pointer = &refleshFlag1;
  refleshFlag2pointer = &refleshFlag2;
  refleshFlag3pointer = &refleshFlag3;
  refleshFlag4pointer = &refleshFlag4;
  guide_line_y = guide_line_x = 0;
  guide_line_k = guide_line_b = 0;
  offset_angle=0;
  x_walk = y_walk = 0;
  countWalkSetCount = 0;
  //**********  boost ***********
  stopped = 0;
  frame_length = 0;
  //**********  boost over ***********
  sum = 0;  //校验位
  //设置初始命令“s00000”
  //format="s00"; 
  for (int iiii = 0; iiii < 100000; iiii++)
    save_data[iiii] = 0;
  Edit_format = "s00";
  string str_guide = "guide1";
  //else 
  //str_guide="guide"+linenum.str_guidenum;
  //if(linenum.str_stopnum=="")
  string str_stop = "stop1";
  //else str_stop="stop"+linenum.str_stopnum;

  laser_machine_parameters lp = Doc.readlaserparamters(".\\test.xml");
  Edit_x_Offset = offset_angle;
  Edit_Start_x = -6.2 + Edit_x_Offset;
  Edit_End_x = 6.2 + Edit_x_Offset;
  Edit_Start_y = 0;
  Edit_End_y = 0;
  Edit_StandHigh = 0;

  End_order = "05 00 03 01 ca";
  save_start_x = '0';
  save_end_x = '0';
  save_start_y = '0';
  save_end_y = '0';
  //设置初始目录“D:\\1.txt”
  lo = "D:\\plane\\";
  ca = "1";
  te = ".txt";

  temp = 0;
  object_high = 1000;
  object_dis = 0;
  object_width = 300;
  avg_order = 0;
  data_h = 0; //存取数据长度
  NUM = 2;    //保存文件名
  reset_order = "42 05 00 00 ca";  //复位命令
  loop = 1;   //循环
  Calib_fuction = 0;//校准功能，0表示不进行校准，1表示进行校准
  calib_num = 0;  //水平校准步数
  redlight = 0;   //红色激光开关闭
  line_num = 0;   //起始行数
  ci = 0;
  install_high = 0;  //初始化安装高度
  mid_num = 0;      //初始化中点坐标
  next_frame = 5;  //有无后续数据帧
  stan_catch = 0;  //判断是否进行水平捕获
  auto_catch = 0; //判断是否进行自动捕获
  auto_num = 4;   //判断是否继续进行捕获
  catch_plane.Format("未捕获");
  walksetup = 0;  //初始设置不进行走步设置
  stop_lp = FALSE;
  walktext = 0;   //初始设置不进行走步测试
  scanyard = 0;   //初始设置不进行站坪扫描
  scanyardToAdjust = 0;
  walksetup_num = 0; //初始设置走步设置数据点数为0
  stip_h = 2; //设置垂直镜子单次移动2步
  judge_direct = 0;
  first_long = 0;
  //***********站坪扫描初始化*********
  Detect_Thing_Init = 1;
  DtcTEdit_StandHigh_min = 0;
  NUM_NEW = 1;
  Scan_Cate = 3;
  Up_Y_Step = 0;
  yard_scan_range = 0;//站坪扫描范围（1，2，3，4，5）
  yard_scan_obstacle_high = 0;//站坪扫描障碍物的高度
  yard_scan_obstacle_width = 0;//站坪扫描障碍物的宽度
  Y_All.clear();
  Z_All.clear();
  obstacle_dis.clear();
  adjustAlpha_ySum = 0;//用于站坪扫描后的校准
  adjustTimes = 0;//用于站坪扫描后的校准
  adjustEdit_StandHigh = 0;//用于站坪扫描后的校准，正数为往上抬的步数
  //**********  初始化设置 ***********
  divid_x = 0;
  divid_y = 0;
  presacl = 2;
  insert_num = 1;
  expan = 0;

  ForHrzCalib = FALSE;	//水平标定处理函数的参数
  OrdDistance = 0;	//普通测距参数，主要用于校准功能内
  distPrcs = 0;		//精确测距参数
  DistMin = 0;		//最小值，这个值用来和distanccePreces比较

  CString   path = AfxGetApp()->m_pszHelpFilePath;
  CString   str = AfxGetApp()->m_pszExeName;
  //MessageBox("初始化1");
  path = path.Left(path.GetLength() - str.GetLength() - 4);
  //routename=".\\test.xml";
  //MessageBox("初始化2");
  CString Xmlpath;
  Xmlpath.Format("%s%s", path, "test.xml");
  routename = LPCSTR(Xmlpath);
  //MessageBox("初始化3");
  //*********	 自动寻找地面点	*****************/
  prepare_measure = FALSE;
  //**********  验证停止点 ***********
  comfirm_type = 0;
  scan_all = 0;
  //**********  画图 ***********
  //CPaintDC dc(this); // device context for painting
  //pWnd=//GetDlgItem(IDC_STATIC_Draw);//(IDC_STATIC_Draw); IDC_STATIC_Draw
  //pControlDC=pWnd->GetDC(); 
  //pWnd->Invalidate(); 
  //pWnd->UpdateWindow(); 
  bDrawInit = 0;
  bDrawGetReadyXY = 0;
  draw_status = 1;
  DrawRct_top = 0;
  DrawRct_bottom = 441;
  DrawRct_left = 0;
  DrawRct_right = 275;
  //pWnd->GetClientRect(rct); 
  GreenPen.CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
  BluePen.CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
  OrangePen.CreatePen(PS_SOLID, 2, RGB(255, 150, 0));
  RedPen.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
  BlackPen.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
  YellowPen.CreatePen(PS_SOLID, 1, RGB(255, 255, 0));
  for (int i = 0; i < 128; i++) {
    colormap1024[i][0] = 0;
    colormap1024[i][1] = 0;
    colormap1024[i][2] = i + 128;

    colormap1024[i + 896][0] = 255 - i;
    colormap1024[i + 896][1] = 0;
    colormap1024[i + 896][2] = 0;
  }
  for (int i = 0; i < 256; i++) {
    colormap1024[i + 128][0] = 0;
    colormap1024[i + 128][1] = 128 + i;
    colormap1024[i + 128][2] = 255;

    colormap1024[i + 384][0] = 128 + i;
    colormap1024[i + 384][1] = 255;
    colormap1024[i + 384][2] = 255 - i;

    colormap1024[i + 640][0] = 255;
    colormap1024[i + 640][1] = 255 - i;
    colormap1024[i + 640][2] = 0;
  }
  //MessageBox("初始化结束");
  OnCommandThread = nullptr;
  ComReadThread = nullptr;
  ComWriteThread = nullptr;
}

CLaster::~CLaster() {}


void CLaster::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  //DDX_Control(pDX, IDC_MSCOMM1, m_ctrlComm);


  DDX_Text(pDX, IDC_EDIT_Start_x, Edit_Start_x);
  DDX_Text(pDX, IDC_EDIT_End_x, Edit_End_x);
  DDX_Text(pDX, IDC_EDIT_InstallHigh, install_high);
  DDX_Text(pDX, IDC_STATIC_WalkText, walktext_deflection);
  DDX_Text(pDX, IDC_EDIT_StandHigh, Edit_StandHigh);
  DDX_Text(pDX, IDC_EDIT_ObjectHigh, object_high);
  DDX_Text(pDX, IDC_EDIT_ObjectDis, object_dis);
  DDX_Text(pDX, IDC_EDIT_Mid, mid_num);
  DDX_Text(pDX, IDC_STATIC_WalkSet, m_strRXData);
  DDX_Text(pDX, IDC_EDIT_GuideLineY, guide_line_y);
  //DDX_Text(pDX, IDC_EDIT_PlaneType,planeType);
  //DDX_Text(pDX, IDC_EDIT_PlaneType, planeType4Confirm);
  DDX_Text(pDX, IDC_EDIT_PlaneType, PlaneType4Confirm);
  DDX_Text(pDX, IDC_EDIT_LineNum, LineNum4Confirm);
}

BEGIN_MESSAGE_MAP(CLaster, CDialog)
  ON_BN_CLICKED(IDOK, &CLaster::OnBnClickedOk)
  ON_BN_CLICKED(IDC_Connect, &CLaster::OnBnClickedConnect)
  ON_BN_CLICKED(IDC_Scanyard, &CLaster::OnBnClickedScanyard)
  ON_BN_CLICKED(IDC_Scanyard, &CLaster::OnBnClickedScanyardToAdjust)
  ON_BN_CLICKED(IDC_WalkSet, &CLaster::OnBnClickedWalkset)
  ON_BN_CLICKED(IDC_WalkTest, &CLaster::OnBnClickedWalktest)
  ON_BN_CLICKED(IDC_BTN_Clear, &CLaster::OnBnClickedBtnClear)
  ON_BN_CLICKED(IDCANCEL, &CLaster::OnBnClickedCancel)
  ON_BN_CLICKED(IDC_StopLine, &CLaster::OnBnClickedStopline)
  ON_BN_CLICKED(IDC_BUTTON_SetZero, &CLaster::OnBnClickedButtonSetzero)
  ON_BN_CLICKED(IDC_BUTTON_Calibration, &CLaster::OnBnClickedButtonCalibration)
  ON_BN_CLICKED(IDC_BUTTON_Locate, &CLaster::OnBnClickedButtonLocate)
  ON_BN_CLICKED(IDC_BUTTON_Left, &CLaster::OnBnClickedButtonLeft)
  ON_BN_CLICKED(IDC_BUTTON_Right, &CLaster::OnBnClickedButtonRight)
  ON_BN_CLICKED(IDC_BUTTON_Up, &CLaster::OnBnClickedButtonUp)
  ON_BN_CLICKED(IDC_BUTTON_Down, &CLaster::OnBnClickedButtonDown)
  ON_BN_CLICKED(IDC_BUTTON_DETECT, &CLaster::OnBnClickedButtonDetect)
  ON_BN_CLICKED(IDC_BUTTON_HRZCALIBRATION, &CLaster::OnBnClickedButtonHrzcalibration)
  ON_BN_CLICKED(IDC_BUTTON_EDGE, &CLaster::OnBnClickedButtonEdge)
  ON_BN_CLICKED(IDC_BUTTON_TWOD, &CLaster::OnBnClickedButtonTwod)
  ON_BN_CLICKED(IDC_BUTTON_LEFTTEN, &CLaster::OnBnClickedButtonLeftten)
  ON_BN_CLICKED(IDC_BUTTON_UPTEN, &CLaster::OnBnClickedButtonUpten)
  ON_BN_CLICKED(IDC_BUTTON_RIGHTTEN, &CLaster::OnBnClickedButtonRightten)
  ON_BN_CLICKED(IDC_BUTTON_DOWNTEN, &CLaster::OnBnClickedButtonDownten)
  ON_BN_CLICKED(IDC_BUTTON_RESET, &CLaster::OnBnClickedButtonReset)
  ON_BN_CLICKED(IDC_BUTTON_PREPARE, &CLaster::OnBnClickedButtonPrepare)
  ON_BN_CLICKED(IDC_BUTTON8, &CLaster::OnBnClickedButton8)
  ON_BN_CLICKED(IDSAVE, &CLaster::OnBnClickedSave)
  //ON_BN_CLICKED(IDC_SetY, &CLaster::OnBnClickedSety)
  ON_BN_CLICKED(IDC_BUTTON_READEDGE, &CLaster::OnBnClickedButtonReadedge)
  ON_BN_CLICKED(IDC_BUTTON_READGUIDELINE, &CLaster::OnBnClickedButtonReadguideline)
  ON_BN_CLICKED(IDC_BUTTON10, &CLaster::OnBnClickedButton10)
  ON_BN_CLICKED(IDC_BUTTON_CLC, &CLaster::OnBnClickedButtonClc)
  ON_BN_CLICKED(IDC_BUTTON_2DSWITCH3D, &CLaster::OnBnClickedButton2dswitch3d)
  ON_BN_CLICKED(IDC_BUTTON_COMFIRMTYPE, &CLaster::OnBnClickedButtonComfirmtype)
  ON_BN_CLICKED(IDC_BUTTON_READTYPE, &CLaster::OnBnClickedButtonReadtype)
  ON_BN_CLICKED(IDC_BUTTON_READLINENUM, &CLaster::OnBnClickedButtonReadlinenum)
  ON_BN_CLICKED(IDC_BUTTON_ConfirmWalkSet, &CLaster::OnBnClickedButtonConfirmwalkset)
  ON_BN_CLICKED(IDC_BUTTON_ClearWalkSet, &CLaster::OnBnClickedButtonClearwalkset)
  ON_BN_CLICKED(IDC_BUTTON_RedLaserOn, &CLaster::OnBnClickedButtonRedlaseron)
  ON_BN_CLICKED(IDC_BUTTON_RedLaserOff, &CLaster::OnBnClickedButtonRedlaseroff)
  //ON_COMMAND(ID_123, &CLaster::On123)
  ON_BN_CLICKED(IDC_BUTTON_ScanAll, &CLaster::OnBnClickedButtonScanall)
  ON_BN_CLICKED(IDC_BUTTON11, &CLaster::OnBnClickedButtonOffLineTest)
  ON_BN_CLICKED(IDC_BUTTON13, &CLaster::OnBnClickedButtonGuideLineMove)
  ON_BN_CLICKED(IDC_BUTTON14, &CLaster::OnBnClickedButtonStopLineMove)
  ON_BN_CLICKED(IDC_BUTTON15, &CLaster::OnBnClickedButtonStopLineMoveSingle)
END_MESSAGE_MAP()


// CLaster 消息处理程序


BOOL CLaster::OnInitDialog() {
  CDialog::OnInitDialog();

  // TODO:  在此添加额外的初始化

  return TRUE;  // return TRUE unless you set the focus to a control
  // 异常: OCX 属性页应返回 FALSE
}


void CLaster::OnBnClickedOk() {
  // TODO: 在此添加控件通知处理程序代码
  laster_flag = TRUE;

  CDialog::OnOK();
}

BEGIN_EVENTSINK_MAP(CLaster, CDialog)
  ON_EVENT(CLaster, IDC_MSCOMM1, 1, CLaster::OnComm, VTS_NONE)
END_EVENTSINK_MAP()

unsigned char change[20];
int stip;
int data[2000];
int min_num = 800000;
BYTE rxdata[2048]; //设置BYTE数组 An 8-bit integerthat is not signed.  
unsigned char data_temp[1024];
int save_data[100000];
int line_data[100000];
int check_num[1000];
int calib_data1[1000];
int St_point[1000];
int Sm_point[1000];
float walksetup_data[3000][7];

void CLaster::OnComm() {
  // TODO: 在此处添加消息处理程序代码
  //VARIANT variant_inp;     
  //COleSafeArray safearray_inp; 
  //int length_data = (data_length-data_order)/2;//一行数据个数
  //LONG len,k;     

  ////CString strFileName2 = "D:\\2.txt"; 

  //CString strtemp;  

  //if(m_ctrlComm.get_CommEvent()==2) //事件值为2表示接收缓冲区内有字符     
  //{            
  //	////////以下你可以根据自己的通信协议加入处理代码         
  //	variant_inp=m_ctrlComm.get_Input(); //读缓冲区         
  //	safearray_inp=variant_inp;
  //	len=safearray_inp.GetOneDimSize(); //得到有效数据长度

  //	for(k=0;k<len;k++) 
  //	{		  
  //		safearray_inp.GetElement(&k,rxdata+k);//转换为BYTE型数组         
  //	}

  //	CString charar1;		
  //	CString Num;

  //	for(k=0;k<len;k++) //将数组转换为Cstring型变量         
  //	{             
  //		data_temp[k]=rxdata[k];
  //		charar1.Format("%d",data_temp[k]);
  //		data[k] = atoi(charar1);
  //		sum+=rxdata[k];
  //	}

  //	//************原来此处有很多注释掉的代码*************

  //	if (sum==0 && data[next_frame] == 1 && len >7)
  //	{
  //		if (data[positive]==0)
  //		{
  //			if (scan_mode==HORIZONTAL)
  //			{
  //				for (int i=data_order-1;i<data_length-2;)
  //				{
  //					if (data_h-2>=0)
  //					{
  //						save_data[data_h-2]=(data[i]*256+data[i+1])*4;
  //					}
  //					else
  //					{
  //						save_data[data_h]=(data[i]*256+data[i+1])*4;
  //					}

  //					data_h++;
  //					i=i+2;
  //				}
  //			}
  //			else
  //			{
  //				for (int i=data_order-1;i<data_length-2;)
  //				{
  //					if (data_h-3>=0)
  //					{
  //						save_data[data_h-3]=(data[i]*256+data[i+1])*4;
  //					}
  //					else
  //					{
  //						save_data[data_h]=(data[i]*256+data[i+1])*4;
  //					}

  //					data_h++;
  //					i=i+2;
  //				}
  //			}
  //		}
  //		else
  //		{
  //			for (int i=data_length-3;i>data_order-2;)
  //			{

  //				save_data[data_h+3]=(data[i]*256+data[i+1])*4;
  //				data_h++;
  //				i=i-2;
  //			}
  //		}


  //		line = data[5]*256+data[6]+1;
  //		line_data[line_num] = line;
  //		line_num++;
  //	}

  //	if (sum == 0 && data[next_frame] == 0 && len >7)
  //	{
  //		if (data[positive]==0)
  //		{
  //			if (scan_mode==HORIZONTAL)
  //			{
  //				for (int i=data_order-1;i<data_length-2;)
  //				{
  //					if (data_h-2>=0)
  //					{
  //						save_data[data_h-2]=(data[i]*256+data[i+1])*4;
  //					}
  //					else
  //					{
  //						save_data[data_h]=(data[i]*256+data[i+1])*4;
  //					}

  //					data_h++;
  //					i=i+2;
  //				}
  //			}
  //			else
  //			{
  //				for (int i=data_order-1;i<data_length-2;)
  //				{
  //					if (data_h-3>=0)
  //					{
  //						save_data[data_h-3]=(data[i]*256+data[i+1])*4;
  //					}
  //					else
  //					{
  //						save_data[data_h]=(data[i]*256+data[i+1])*4;
  //					}

  //					data_h++;
  //					i=i+2;
  //				}
  //			}
  //		}
  //		else
  //		{
  //			for (int i=data_length-3;i>data_order-2;)
  //			{

  //				save_data[data_h+3]=(data[i]*256+data[i+1])*4;
  //				data_h++;
  //				i=i-2;
  //			}
  //		}
  //		line = data[5]*256+data[6]+1;
  //		line_data[line_num] = line;
  //		line_num++;
  //		MedFilter(length_data,5);

  //		if (Calib_fuction == TRUE)
  //		{
  //			Calibration();
  //		}
  //		else if (walksetup == TRUE)
  //		{
  //			WalkSetUp();
  //		}
  //		else if (walktext == TRUE)
  //		{
  //			WalkText();
  //		}
  //		else if(avg_order == TRUE)
  //		{
  //			Avg();
  //		}
  //		else if (ForHrzCalib == TRUE)
  //		{
  //			save_hrzCali_record("水平标定-OnComm-if(ForHrzCalib == TRUE)将调用CalibHriz()");
  //			CalibHriz();
  //		}
  //		else if (distPrcs == TRUE)
  //		{
  //			save_hrzCali_record("水平标定-OnComm-if(distPrcs == TRUE)将调用AVGprecise()");
  //			AVGprecise();
  //		}
  //		else if (OrdDistance ==TRUE)
  //		{
  //			normalMeasure();
  //		}
  //		else if (comfirm_type >=1)
  //		{
  //			ComfirmType();
  //		}
  //		else if (scan_all == 1)
  //		{
  //			ScanAll();
  //		}
  //		else if(ConfirmFunct == TRUE)
  //		{
  //			ConfirmEdge();
  //		}  
  //		else if (HrzScnProc == TRUE)
  //		{
  //			save_hrzCali_record("水平标定-OnComm-if(HrzScnProc == TRUE)将调用HrzProFunct()");
  //			HrzProFunct();
  //		}
  //		/*else if(hrizmeasure == TRUE)
  //		{
  //		HrizDisAvg();
  //		}*/
  //	}
  //	else
  //	{
  //		sum = 0;
  //	}

  //	if (len <7)
  //	{
  //		/*if(prepare_measure== FALSE )
  //		{
  //		if(prepare_complete == FALSE)
  //		{
  //		actions(NULL,NULL,NULL,NULL,FIXPOINT,JPPREPAREMEASSURE);
  //		}
  //		if(prepare_complete == TRUE)
  //		{
  //		m_strPrepare="初始完成，可以开始标定";
  //		UpdateData(FALSE);
  //		}
  //		}*/

  //		if (auto_catch == 1)
  //		{
  //			if (auto_num>20000 && auto_num != 4)
  //			{
  //				OnAutocatch();

  //			}
  //			else
  //			{
  //				auto_catch = 0;

  //			}

  //		}

  //		else if (ForHrzCalib == TRUE)
  //		{
  //			save_hrzCali_record("水平标定-OnComm-else if(ForHrzCalib == TRUE)将调用CalibHriz()");
  //			CalibHriz();
  //		}

  //		else if (Calib_fuction == TRUE)
  //		{
  //			Sleep(4000);
  //			Calibration();
  //		}

  //		else if(ConfirmFunct == TRUE)
  //		{
  //			ConfirmEdge();
  //		}
  //		//else if(hrizmeasure == TRUE)
  //		//{
  //		//	 HrizDisAvg();
  //		//}
  //	}
  //}

  ////m_strRXData.Format("%d",data_h);
  ////Edit_line.Format("%d",line);
  ////UpdateData(FALSE); //更新编辑框内容
}
//void CLaster::OnComm()
//{
//	// TODO: 在此处添加消息处理程序代码
//	VARIANT variant_inp;     
//	COleSafeArray safearray_inp; 
//	int length_data = (data_length-data_order)/2;//一行数据个数
//	LONG len,k;     
//
//	//CString strFileName2 = "D:\\2.txt"; 
//
//	CString strtemp;  
//
//	if(m_ctrlComm.get_CommEvent()==2) //事件值为2表示接收缓冲区内有字符     
//	{            
//		////////以下你可以根据自己的通信协议加入处理代码         
//		variant_inp=m_ctrlComm.get_Input(); //读缓冲区         
//		safearray_inp=variant_inp;
//		len=safearray_inp.GetOneDimSize(); //得到有效数据长度
//
//		for(k=0;k<len;k++) 
//		{		  
//			safearray_inp.GetElement(&k,rxdata+k);//转换为BYTE型数组         
//		}
//
//		CString charar1;		
//		CString Num;
//
//		for(k=0;k<len;k++) //将数组转换为Cstring型变量         
//		{             
//			data_temp[k]=rxdata[k];
//			charar1.Format("%d",data_temp[k]);
//			data[k] = atoi(charar1);
//			sum+=rxdata[k];
//		}
//
//		//************原来此处有很多注释掉的代码*************
//
//		if (sum==0 && data[next_frame] == 1 && len >7)
//		{
//			if (data[positive]==0)
//			{
//				if (scan_mode==HORIZONTAL)
//				{
//					for (int i=data_order-1;i<data_length-2;)
//					{
//						if (data_h-2>=0)
//						{
//							save_data[data_h-2]=(data[i]*256+data[i+1])*4;
//						}
//						else
//						{
//							save_data[data_h]=(data[i]*256+data[i+1])*4;
//						}
//
//						data_h++;
//						i=i+2;
//					}
//				}
//				else
//				{
//					for (int i=data_order-1;i<data_length-2;)
//					{
//						if (data_h-3>=0)
//						{
//							save_data[data_h-3]=(data[i]*256+data[i+1])*4;
//						}
//						else
//						{
//							save_data[data_h]=(data[i]*256+data[i+1])*4;
//						}
//
//						data_h++;
//						i=i+2;
//					}
//				}
//			}
//			else
//			{
//				for (int i=data_length-3;i>data_order-2;)
//				{
//
//					save_data[data_h+3]=(data[i]*256+data[i+1])*4;
//					data_h++;
//					i=i-2;
//				}
//			}
//
//
//			line = data[5]*256+data[6]+1;
//			line_data[line_num] = line;
//			line_num++;
//		}
//
//		if (sum == 0 && data[next_frame] == 0 && len >7)
//		{
//			if (data[positive]==0)
//			{
//				if (scan_mode==HORIZONTAL)
//				{
//					for (int i=data_order-1;i<data_length-2;)
//					{
//						if (data_h-2>=0)
//						{
//							save_data[data_h-2]=(data[i]*256+data[i+1])*4;
//						}
//						else
//						{
//							save_data[data_h]=(data[i]*256+data[i+1])*4;
//						}
//
//						data_h++;
//						i=i+2;
//					}
//				}
//				else
//				{
//					for (int i=data_order-1;i<data_length-2;)
//					{
//						if (data_h-3>=0)
//						{
//							save_data[data_h-3]=(data[i]*256+data[i+1])*4;
//						}
//						else
//						{
//							save_data[data_h]=(data[i]*256+data[i+1])*4;
//						}
//
//						data_h++;
//						i=i+2;
//					}
//				}
//			}
//			else
//			{
//				for (int i=data_length-3;i>data_order-2;)
//				{
//
//					save_data[data_h+3]=(data[i]*256+data[i+1])*4;
//					data_h++;
//					i=i-2;
//				}
//			}
//			line = data[5]*256+data[6]+1;
//			line_data[line_num] = line;
//			line_num++;
//			MedFilter(length_data,5);
//
//			if (Calib_fuction == TRUE)
//			{
//				Calibration();
//			}
//			else if (walksetup == TRUE)
//			{
//				WalkSetUp();
//			}
//			else if (walktext == TRUE)
//			{
//				WalkText();
//			}
//			else if(avg_order == TRUE)
//			{
//				Avg();
//			}
//			else if (ForHrzCalib == TRUE)
//			{
//				save_hrzCali_record("水平标定-OnComm-if(ForHrzCalib == TRUE)将调用CalibHriz()");
//				CalibHriz();
//			}
//			else if (distPrcs == TRUE)
//			{
//				save_hrzCali_record("水平标定-OnComm-if(distPrcs == TRUE)将调用AVGprecise()");
//				AVGprecise();
//			}
//			else if (OrdDistance ==TRUE)
//			{
//				normalMeasure();
//			}
//			else if (comfirm_type >=1)
//			{
//				ComfirmType();
//			}
//			else if (scan_all == 1)
//			{
//				ScanAll();
//			}
//			else if(ConfirmFunct == TRUE)
//			{
//				ConfirmEdge();
//			}  
//			else if (HrzScnProc == TRUE)
//			{
//				save_hrzCali_record("水平标定-OnComm-if(HrzScnProc == TRUE)将调用HrzProFunct()");
//				HrzProFunct();
//			}
//			/*else if(hrizmeasure == TRUE)
//			{
//			HrizDisAvg();
//			}*/
//		}
//		else
//		{
//			sum = 0;
//		}
//
//		if (len <7)
//		{
//			/*if(prepare_measure== FALSE )
//			{
//			if(prepare_complete == FALSE)
//			{
//			actions(NULL,NULL,NULL,NULL,FIXPOINT,JPPREPAREMEASSURE);
//			}
//			if(prepare_complete == TRUE)
//			{
//			m_strPrepare="初始完成，可以开始标定";
//			UpdateData(FALSE);
//			}
//			}*/
//
//			if (auto_catch == 1)
//			{
//				if (auto_num>20000 && auto_num != 4)
//				{
//					OnAutocatch();
//
//				}
//				else
//				{
//					auto_catch = 0;
//
//				}
//
//			}
//
//			else if (ForHrzCalib == TRUE)
//			{
//				save_hrzCali_record("水平标定-OnComm-else if(ForHrzCalib == TRUE)将调用CalibHriz()");
//				CalibHriz();
//			}
//
//			else if (Calib_fuction == TRUE)
//			{
//				Sleep(4000);
//				Calibration();
//			}
//
//			else if(ConfirmFunct == TRUE)
//			{
//				ConfirmEdge();
//			}
//			//else if(hrizmeasure == TRUE)
//			//{
//			//	 HrizDisAvg();
//			//}
//		}
//	}
//
//	//m_strRXData.Format("%d",data_h);
//	//Edit_line.Format("%d",line);
//	//UpdateData(FALSE); //更新编辑框内容
//}
void CLaster::OnCommand() {
  int test_T1 = 3;
  while (TEST_THREAD) {
    test_T1 = test_T1 * 2;
    TRACE("(T1)=%d ", test_T1);
    Sleep(500);
  }
  int length_data;//一行数据个数
  int k;
  CString strtemp;
  CString charar1;
  try {
    while (1) {
      boost::this_thread::interruption_point();
      if (stopped) {
        Sleep(20);
        continue;
      }
      ptrMsgData curSend = NULL;
      {
        mutex::scoped_lock lock1(glb_sp_data.recvMu);
        glb_sp_data.con_recv.wait(glb_sp_data.recvMu);
      }
      while (NULL != (curSend = glb_sp_data.getFromRecvList())) {
        //处理当前包
        //curSend->msgData; //data
        //curSend->lenData;//整体包长度
        if (offline_mode == 1)//offline_mode==1)//【移植】
        {
          //	detectThingMode=0;//目的是，不用每次改宏定义，离线模式的时候就不会扫描站坪
          //	if (FALSE == HrzDetect || FALSE == VertDetect)
          //	{
          //		StrtAutoCatch();
          //		HrzDetect=1;
          //		VertDetect=1;
          //	}
          //	else if(detectBridgeMode==1)
          //	{
          //		offline_row();
          //		DetectBridge();
          //	}						
          //	else if(detectThingMode==1)
          //	{
          //		offline_row();
          //		DetectThing();
          //	}
          //	else if(detectPlaneMode==1)
          //	{
          //		offline_row();
          //		DetectPlane();
          //	}
          //	else
          //	{
          if (offline_mode == 1) {
            Laser_clock = boost::posix_time::microsec_clock::local_time();
            //LOG(INFO)<<ctime()<<endl;	
            offline_row();
            WalkText();
          }
          //	}
        }
        else {
          length_data = (data_length - data_order - 1) / 2;
          for (k = 0; k < curSend->lenData; k++) //将数组转换为Cstring型变量         
          {
            charar1.Format("%d", curSend->msgData[k]);
            data[k] = atoi(charar1);
          }

          if (data[next_frame] == 1 && curSend->lenData > 7) {

            //LOG(INFO)<<"长回复 if (data[next_frame] == 1 && curSend->lenData >7 : "<<curSend->lenData;
            if (data[positive] == 0) {
              if (scan_mode == HORIZONTAL) {
                for (int i = data_order; i < curSend->lenData - 2;) {
                  if (data_h - forward_offset >= 0) {
                    save_data[data_h - forward_offset] = (data[i] * 256 + data[i + 1]) * 4;
                  }
                  else {
                    save_data[data_h] = (data[i] * 256 + data[i + 1]) * 4;
                  }

                  data_h++;
                  i = i + 2;
                }
              }
              else {
                for (int i = data_order; i < curSend->lenData - 2;) {
                  if (data_h - forward_offset >= 0) {
                    save_data[data_h - forward_offset] = (data[i] * 256 + data[i + 1]) * 4;
                  }
                  else {
                    save_data[data_h] = (data[i] * 256 + data[i + 1]) * 4;
                  }

                  data_h++;
                  i = i + 2;
                }
              }
            }
            else {
              for (int i = 0; i < backward_offset; i++) save_data[data_h + i] = 4;
              for (int i = curSend->lenData - 3; i > data_order - 1 + backward_offset * 2; i -= 2, data_h++) {
                save_data[data_h + backward_offset] = (data[i] * 256 + data[i + 1]) * 4;
              }
              data_h += backward_offset;
            }
          }
          else if (data[next_frame] == 0 && curSend->lenData > 7) {
            //LOG(INFO)<<"长回复 if (data[next_frame] == 0 && curSend->lenData >7 : "<<curSend->lenData;
            if (data[positive] == 0) {
              if (scan_mode == HORIZONTAL) {
                for (int i = data_order; i < curSend->lenData - 2;) {
                  if (data_h - forward_offset >= 0) {
                    save_data[data_h - forward_offset] = (data[i] * 256 + data[i + 1]) * 4;
                  }
                  else {
                    save_data[data_h] = (data[i] * 256 + data[i + 1]) * 4;
                  }

                  data_h++;
                  i = i + 2;
                }
              }


              else {
                for (int i = data_order; i < curSend->lenData - 2;) {
                  if (data_h - forward_offset >= 0) {
                    save_data[data_h - forward_offset] = (data[i] * 256 + data[i + 1]) * 4;
                  }
                  else {
                    save_data[data_h] = (data[i] * 256 + data[i + 1]) * 4;
                  }

                  data_h++;
                  i = i + 2;
                }
              }
            }
            else {
              for (int i = 0; i < backward_offset; i++) save_data[data_h + i] = 4;
              for (int i = curSend->lenData - 3; i > data_order - 1 + backward_offset * 2; i -= 2, data_h++) {
                save_data[data_h + backward_offset] = (data[i] * 256 + data[i + 1]) * 4;
              }
              data_h += backward_offset;
            }
            MedFilter(length_data, 3);
            //if (FALSE == HrzDetect || FALSE == VertDetect)
            //	normalMeasure();
            //else if(detectBridgeMode==1)
            //{
            //	//offline_row();
            //	DetectBridge();
            //}
            //else if(detectThingMode==1)
            //{
            //	//offline_row();
            //	DetectThing();
            //}
            //else if(detectPlaneMode==1)
            //{
            //	//offline_row();
            //	DetectPlane();
            //}
            //else
            //{
            //	Laser_clock=boost::posix_time::microsec_clock::local_time();
            //	//LOG(INFO)<<ctime()<<endl;
            //	if(forcedVerify==1 && forcedVerifyTimes1==0 && forcedVerifyTimes2==0)//强制机型验证初始化
            //	{
            //		verifyCountRow=0;//累计行数
            //		verifyAverageSum=0;//各行平均数的和
            //		verifyCountVirtical=0;//垂直扫描次数
            //		verifyHeightSum=0;//高度累加
            //		verifyComplete=0;//机型验证是否完成
            //	}
            //	if( (planeType.at(0)=='H'||planeType.at(0)=='h') && planeType.at(1)=='u' )
            //	{
            //		object_high = 1000;
            //		object_width = 400;
            //		head_wheel_dis = 0;//放在这里可能不行
            //		GuidePlane_WalkText();
            //	}
            //	else//正常引导
            //	{
            //		GuidePlane();
            //	}
            //}

            if (Calib_fuction == TRUE) {
              Calibration();
            }
            else if (walksetup == TRUE) {
              WalkSetUp();
            }
            else if (walktext == TRUE) {
              WalkText();
            }
            else if (scanyard == TRUE) {
              ScanYard();
            }
            else if (scanyardToAdjust == TRUE) {
              ScanYardToAdjust();
            }
            else if (avg_order == TRUE) {
              Avg();
            }
            else if (ForHrzCalib == TRUE) {
              save_hrzCali_record("水平标定-OnComm-if(ForHrzCalib == TRUE)将调用CalibHriz()");
              CalibHriz();
            }
            else if (distPrcs == TRUE) {
              save_hrzCali_record("水平标定-OnComm-if(distPrcs == TRUE)将调用AVGprecise()");
              AVGprecise();
            }
            else if (OrdDistance == TRUE) {
              normalMeasure();
            }
            else if (comfirm_type >= 1) {
              ComfirmType();
            }
            else if (scan_all == 1) {
              ScanAll();
            }
            else if (ConfirmFunct == TRUE) {
              ConfirmEdge();
            }
            else if (HrzScnProc == TRUE) {
              save_hrzCali_record("水平标定-OnComm-if(HrzScnProc == TRUE)将调用HrzProFunct()");
              HrzProFunct();
            }
            /*else if(hrizmeasure == TRUE)
            {
            HrizDisAvg();
            }*/
          }
          else if (curSend->lenData < 7) {
            //LOG(INFO)<<"短回复 else if (curSend->lenData <7) : "<<curSend->lenData;
            //Sleep(100);
            if (auto_catch == 1) {
              if (auto_num > 20000 && auto_num != 4) {
                OnAutocatch();

              }
              else {
                auto_catch = 0;

              }

            }

            else if (ForHrzCalib == TRUE) {
              save_hrzCali_record("水平标定-OnComm-else if(ForHrzCalib == TRUE)将调用CalibHriz()");
              CalibHriz();
            }

            else if (Calib_fuction == TRUE) {
              Sleep(4000);
              Calibration();
            }

            else if (ConfirmFunct == TRUE) {
              ConfirmEdge();
            }
            //else if(hrizmeasure == TRUE)
            //{
            //	 HrizDisAvg();
            //}
          }
        }

      }//
    }
  }

  catch (std::exception& e) {
    TRACE("\n%s\n", e.what());
  }
}

void CLaster::OnBnClickedConnect() {
  LaserInit();
  //draw_OnLaserPaintInit();
  //if(m_ctrlComm.get_PortOpen())
  //	m_ctrlComm.put_PortOpen(FALSE);  
  //m_ctrlComm.put_CommPort(1); //选择com 
  //if( !m_ctrlComm.get_PortOpen()) 
  //	m_ctrlComm.put_PortOpen(TRUE);//打开串口 
  //else AfxMessageBox("cannot open serial port");  
  //m_ctrlComm.put_Settings("115200,n,8,1"); //波特率115200，无校验，8个数据位，1个停止位 
  //m_ctrlComm.put_InputMode(1);  // 以二进制方式检取数据


}


void CLaster::OnBnClickedWalkset() {
  refleshFlag1 = -1; *refleshFlag1pointer = -1;
  refleshFlag2 = 0; *refleshFlag2pointer = 0;
  refleshFlag3 = 0; *refleshFlag3pointer = 0;
  refleshFlag4 = 0; *refleshFlag4pointer = 0;
  HrzScnProc = FALSE;
  // TODO: 在此添加控件通知处理程序代码
  UpdateData(TRUE);
  if (object_dis <= 0)object_dis = 40000;
  UpdateData(FALSE);
  refleshFlag1 = 2; *refleshFlag1pointer = 2;//走步设置初始
  avg_order = FALSE;
  scan_mode = HORIZONTAL;
  loop = 0;
  float alpha_y_temp = ((float)install_high - (float)object_high - 100) / (float)object_dis;//-100使扫描位置抬高一点
  float angle = atan(alpha_y_temp);
  Edit_StandHigh_Org = Edit_StandHigh;
  Edit_StandHigh = -angle * 180 / YSTEPANGLE / PI;
  if (Edit_StandHigh % 2 != 0) {
    Edit_StandHigh++;
  }
  stepMoveNoSet0((Edit_StandHigh - Edit_StandHigh_Org) / 2, UP, VERTICAL);
  Sleep(300);
  start_height = Edit_StandHigh;
  walksetup = TRUE;
  CString change;
  stop_lp = FALSE;
  short int sx, ex, sy, ey;
  data_h = 0;
  data_order = 16;
  line_num = 0;
  positive = 6;
  next_frame = 5;
  stan_catch = 0;
  auto_catch = 0;
  walktext = 0;
  scanyard = 0;
  scanyardToAdjust = 0;

  frame_length = 7;
  // 	itoa(Edit_hundred,&hundred,10);
  // 	itoa(Edit_ten,&ten,10);
  // 	itoa(Edit_ge,&ge,10);

  /*CString number;
  if (format=="s0")
  {
  data_length=772;
  }
  number = "0000";*/
  // TODO: 在此添加控件通知处理程序代码

  /*m_strTXData=format+number;*/
  save_start_x.Format("%d", Edit_Start_x);
  save_end_x.Format("%d", Edit_End_x);
  save_start_y.Format("%d", Edit_Start_y);
  save_end_y.Format("%d", Edit_End_y);
  sx = Edit_Start_x / XSTEPANGLE;
  ex = Edit_End_x / XSTEPANGLE;
  sy = Edit_Start_y / YSTEPANGLE;
  ey = Edit_End_y / YSTEPANGLE;
  int standhigh = Edit_StandHigh / 2;
  sx = sx / 2;
  ex = ex / 2;
  sy = sy / 2;
  ey = ey / 2;
  int standheight;
  if (presacl != 0) {
    standheight = standhigh;
    data_length = abs(ex - sx) * 2 * presacl + data_order + 2;
  }
  else {
    standheight = Edit_StandHigh;
    data_length = abs(ex - sx) * 2 + data_order + 2;
  }

  // format.Format("44 02 00 08 %c%c ",Edit_format[1],Edit_format[2]);
  format.Format("53 01 00 0a 02 ");
  change.Format("%08x", standheight);
  Stand_High.Format("%c%c %c%c ", change[4], change[5], change[6], change[7]);



  change.Format("%08x", sx);
  start_x.Format("%c%c %c%c ", change[4], change[5], change[6], change[7]);

  change.Format("%08x", ex);
  end_x.Format("%c%c %c%c ", change[4], change[5], change[6], change[7]);

  change.Format("%08x", sy);
  start_y.Format("%c%c %c%c ", change[4], change[5], change[6], change[7]);

  change.Format("%08x", ey);
  end_y.Format("%c%c %c%c ", change[4], change[5], change[6], change[7]);

  m_strTXData = format + Stand_High + start_x + end_x + "00 03 01 ca";
  CByteArray hexdata;
  //int len=String2Hex(m_strTXData,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
  //m_ctrlComm.put_Output(COleVariant(hexdata)); //发送十六进制数据 

  //m_ctrlComm.put_RThreshold(data_length);  //参数1表示每当串口接收缓冲区中有多于或等于1个字符时将引发一个接收数据的OnComm事件 
  //m_ctrlComm.put_InputLen(0); //设置当前接收区数据长度为0 
  //m_ctrlComm.get_Input();//先预读缓冲区以清除残留数据
  int len = String2Hex(m_strTXData, senddata);
  if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
  m_strRXData = "走步设置:启动";
  UpdateData(FALSE);
  refleshFlag1 = 2; *refleshFlag1pointer = 2;//走步设置：启动
  char led_distance[10];
  sprintf(led_distance, "%04.1f", ((float)object_dis) / 1000);
  if (glb_led_obj_ptr != nullptr) glb_led_obj_ptr->sendmessage(6, 0, "", led_distance);
  //m_strRXData=m_strTXData;
  //UpdateData(FALSE); //更新编辑框内容
  //GetDlgItem(IDC_BTN_Clear)->EnableWindow(TRUE);
  //GetDlgItem(IDC_BUTTON_ConfirmWalkSet)->EnableWindow(TRUE);
  //GetDlgItem(IDC_BUTTON_ClearWalkSet)->EnableWindow(TRUE);
}

//由于这个转换函数的格式限制，在发送框中的十六制字符应该每两个字符之间插入一个空隔 
//如：A1 23 45 0B 00 29 
//CByteArray是一个动态字节数组，可参看MSDN帮助 
int CLaster::String2Hex(CString str, CByteArray &senddata)//【重载，config原本函数】
{
  int hexdata, lowhexdata;
  int hexdatalen = 0;
  int len = str.GetLength();
  senddata.SetSize(len / 2);
  for (int i = 0; i < len;) {
    char lstr, hstr = str[i];
    if (hstr == ' ') {
      i++;
      continue;
    }
    i++;
    if (i >= len)
      break;
    lstr = str[i];
    hexdata = ConvertHexChar(hstr);
    lowhexdata = ConvertHexChar(lstr);
    if ((hexdata == 16) || (lowhexdata == 16))
      break;
    else
      hexdata = hexdata * 16 + lowhexdata;
    i++;
    senddata[hexdatalen] = (char)hexdata;
    hexdatalen++;
  }
  senddata.SetSize(hexdatalen);
  return hexdatalen;
}
int CLaster::String2Hex(CString str, unsigned char senddata[])//【重载，来自VIDGS】
{
  int hexdata, lowhexdata;
  int hexdatalen = 0;
  int len = str.GetLength();
  for (int i = 0; i < len;) {
    char lstr, hstr = str[i];
    if (hstr == ' ') {
      i++;
      continue;
    }
    i++;
    if (i >= len)
      break;
    lstr = str[i];
    hexdata = ConvertHexChar(hstr);
    lowhexdata = ConvertHexChar(lstr);
    if ((hexdata == 16) || (lowhexdata == 16))
      break;
    else
      hexdata = hexdata * 16 + lowhexdata;
    i++;
    senddata[hexdatalen] = (char)hexdata;
    hexdatalen++;
  }

  return hexdatalen;
}

//这是一个将字符转换为相应的十六进制值的函数 
//好多C语言书上都可以找到
//功能：若是在0-F之间的字符，则转换为相应的十六进制字符，否则返回-1 

char CLaster::ConvertHexChar(char ch) {
  if ((ch >= '0') && (ch <= '9'))
    return ch - 0x30;
  else if ((ch >= 'A') && (ch <= 'F'))
    return ch - 'A' + 10;
  else if ((ch >= 'a') && (ch <= 'f'))
    return ch - 'a' + 10;
  else return (-1);
}
void CLaster::Manualsend() {
  // TODO: Add your control notification handler code here 
  UpdateData(TRUE); //读取编辑框内容 

  CByteArray hexdata;
  //int len=String2Hex(m_strTXData,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
  //m_ctrlComm.put_Output(COleVariant(hexdata)); //发送十六进制数据 
  int len = String2Hex(m_strTXData, senddata);
  if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);

}

void CLaster::OnBnClickedScanyard() {
  refleshFlag1 = -1; *refleshFlag1pointer = -1;
  refleshFlag2 = 0; *refleshFlag2pointer = 0;
  refleshFlag4 = 0; *refleshFlag4pointer = 0;
  refleshFlag3 = 0; *refleshFlag3pointer = 0;
  HrzScnProc = FALSE;
  x_walk = 0;
  y_walk = 0;
  string now_time = to_iso_extended_string(boost::posix_time::second_clock::local_time());
  int position = now_time.find(":"); // find first period
  while (position != string::npos) {
    now_time.replace(position, 1, ".");
    position = now_time.find(":", position + 1);
  }
  //CString temp_time_str;//=now_time.GetBuffer(0);
  time_str.Format("%s", now_time.c_str());
  system_parameters para = Doc.readsystemdatas(".\\test.xml");
  string image_save_root = para.LogSaveRoot;
  string position_num = para.StationNumber;
  _mkdir((image_save_root + position_num + "-" + now_time + "yard").c_str());
  D_Plan_Time.Format("%s", (image_save_root + position_num + "-" + now_time + "yard\\yard\\").c_str());
  DetectThing_SaveModle = 0;
  // TODO: 在此添加控件通知处理程序代码
  countWalkSetCount = 0;
  UpdateData(TRUE);
  if (object_dis <= 0)object_dis = 40000;
  if (guide_line_k != 0)
    guide_line_x = float(guide_line_y - guide_line_b) / guide_line_k;
  UpdateData(FALSE);
  refleshFlag1 = 2; *refleshFlag1pointer = 2;//站坪扫描初始
  //refleshFlag2=1;*refleshFlag2pointer=1;//走步测试初始
  walktext = FALSE;
  scanyard = TRUE;
  Detect_Thing_Init = 1;
  DtcTEdit_StandHigh_min = 0;
  NUM_NEW = 1;
  Scan_Cate = 3;
  Up_Y_Step = 0;
  yard_scan_range = 0;//站坪扫描范围（1，2，3，4，5）
  yard_scan_obstacle_high = 0;//站坪扫描障碍物的高度
  yard_scan_obstacle_width = 0;//站坪扫描障碍物的宽度
  Y_All.clear();
  Z_All.clear();
  obstacle_dis.clear();
  CString change;

  short int sx, ex, sy, ey;
  data_h = 0;
  data_order = 16;
  line_num = 0;
  positive = 6;
  next_frame = 5;
  walksetup = 0;
  stan_catch = 0;
  auto_catch = 0;
  stop_lp = FALSE;

  scan_mode = HORIZONTAL;
  loop = 0;
  frame_length = 7;
  /*float alpha_y_temp=((float)install_high-(float)object_high-100)/(float)object_dis;//-100使扫描位置抬高一点
  float angle=atan (alpha_y_temp);
  Edit_StandHigh = - angle*180/YSTEPANGLE/PI;
  if (Edit_StandHigh%2 != 0)
  {
    Edit_StandHigh++;
  }
  start_height = Edit_StandHigh;*/

  save_start_x.Format("%d", Edit_Start_x);
  save_end_x.Format("%d", Edit_End_x);
  save_start_y.Format("%d", Edit_Start_y);
  save_end_y.Format("%d", Edit_End_y);
  sx = Edit_Start_x / XSTEPANGLE;
  ex = Edit_End_x / XSTEPANGLE;
  sy = Edit_Start_y / YSTEPANGLE;
  ey = Edit_End_y / YSTEPANGLE;
  int standhigh = Edit_StandHigh / 2;
  sx = sx / 2;
  ex = ex / 2;
  sy = sy / 2;
  ey = ey / 2;
  int standheight;
  if (presacl != 0) {
    standheight = standhigh;
    data_length = abs(ex - sx) * 2 * presacl + data_order + 2;
  }
  else {
    standheight = Edit_StandHigh;
    data_length = abs(ex - sx) * 2 + data_order + 2;
  }


  // format.Format("44 02 00 08 %c%c ",Edit_format[1],Edit_format[2]);
  /*format.Format("53 01 00 0a 02 ");
  change.Format("%08x",standheight);
  Stand_High.Format("%c%c %c%c ",change[4],change[5],change[6],change[7]);



  change.Format("%08x", sx);
  start_x.Format("%c%c %c%c ",change[4],change[5],change[6],change[7]);

  change.Format("%08x", ex);
  end_x.Format("%c%c %c%c ",change[4],change[5],change[6],change[7]);

  change.Format("%08x", sy);
  start_y.Format("%c%c %c%c ",change[4],change[5],change[6],change[7]);

  change.Format("%08x", ey);
  end_y.Format("%c%c %c%c ",change[4],change[5],change[6],change[7]);

  m_strTXData = format+Stand_High+start_x+end_x+"00 03 01 ca";
  CByteArray hexdata;
  int len = String2Hex(m_strTXData, senddata);
  if (senddata[0]==0x43)frame_length=2;else if (senddata[0]==0x53)frame_length=7;glb_sp_data.deliver2send(senddata,len);*/
  int ver_two = 2;
  if (Detect_Thing_Init) {
    laser_machine_parameters lp = Doc.readlaserparamters(".\\test.xml");
    yard_scan_range = lp.yard_scan_range;
    yard_scan_obstacle_high = lp.yard_scan_obstacle_high;
    yard_scan_obstacle_width = lp.yard_scan_obstacle_width;

    laser_parameters lpy = Doc.readlaserdatas(".\\test.xml", Doc.guaidlinename, Doc.stoplinename);
    float guide_line_y_temp = lpy.guide_line_y;
    if (guide_line_y_temp < 20000)guide_line_y_temp = 20000;
    Edit_StandHigh_Org = Edit_StandHigh;//Org为全局变量，org为局部变量		
    Edit_Start_x_Org = Edit_Start_x;
    Edit_End_x_Org = Edit_End_x;
    if (yard_scan_range == 2) {
      Edit_Start_x = Edit_Start_x * 2;
      Edit_End_x = Edit_End_x * 2;
    }
    else if (yard_scan_range == 3) {
      Edit_Start_x = Edit_Start_x * 3;
      Edit_End_x = Edit_End_x * 3;
    }
    else if (yard_scan_range == 4) {
      Edit_Start_x = Edit_Start_x * 4;
      Edit_End_x = Edit_End_x * 4;
    }
    else if (yard_scan_range == 5) {
      Edit_Start_x = Edit_Start_x * 5;
      Edit_End_x = Edit_End_x * 5;
    }
    int DetTobject_dis_max = 120000;//最远扫描距离
    float alpha_y_temp = ((float)install_high) / (float)DetTobject_dis_max;
    float angle = atan(alpha_y_temp);
    Edit_StandHigh = -angle * 180 / (0.1125 / ver_two) / PI;//Edit_StandHigh的起始值

    Edit_StandHigh = 0;

    int Edit_StandHigh_temp = Edit_StandHigh / 2;
    Edit_StandHigh = Edit_StandHigh_temp * 2;
    alpha_y_temp = ((float)install_high) / (float)guide_line_y_temp;
    angle = atan(alpha_y_temp);
    DtcTEdit_StandHigh_min = -angle * 180 / (0.1125 / ver_two) / PI;//Edit_StandHigh的最小值
    int DtcTEdit_StandHigh_min_temp = DtcTEdit_StandHigh_min / 2;
    DtcTEdit_StandHigh_min = DtcTEdit_StandHigh_min_temp * 2;
    length_data = (data_length - data_order) / 2;//一行数据个数
    length_data *= yard_scan_range;
    //int step=(Edit_StandHigh_Org-Edit_StandHigh)/2;
    //stepMoveNoSet0(step, DOWN, VERTICAL);
    //Sleep(1000);
    Scan_Cate = 3;//0;
    Up_Y_Step = -2;
    OnAutocatch();
    DetectThing_SaveModle = 1;
    Detect_Thing_Init = 0;
    NUM_NEW = 1;
    //return;
  }
  m_strRXData = "站坪扫描:启动";
  walktext_deflection = "执行中...";
  if (glb_led_obj_ptr != nullptr) glb_led_obj_ptr->sendmessage(2, 0, "", "");
  UpdateData(FALSE);
  refleshFlag1 = 2; *refleshFlag1pointer = 2;//站坪扫描：启动
}

void CLaster::OnBnClickedScanyardToAdjust() {
  refleshFlag1 = -1; *refleshFlag1pointer = -1;
  refleshFlag2 = 0; *refleshFlag2pointer = 0;
  refleshFlag4 = 0; *refleshFlag4pointer = 0;
  refleshFlag3 = 0; *refleshFlag3pointer = 0;
  HrzScnProc = FALSE;
  x_walk = 0;
  y_walk = 0;
  string now_time = to_iso_extended_string(boost::posix_time::second_clock::local_time());
  int position = now_time.find(":"); // find first period
  while (position != string::npos) {
    now_time.replace(position, 1, ".");
    position = now_time.find(":", position + 1);
  }
  //CString temp_time_str;//=now_time.GetBuffer(0);
  time_str.Format("%s", now_time.c_str());
  system_parameters para = Doc.readsystemdatas(".\\test.xml");
  string image_save_root = para.LogSaveRoot;
  string position_num = para.StationNumber;
  _mkdir((image_save_root + position_num + "-" + now_time + "yard").c_str());
  D_Plan_Time.Format("%s", (image_save_root + position_num + "-" + now_time + "yard\\yard\\").c_str());
  DetectThing_SaveModle = 0;
  // TODO: 在此添加控件通知处理程序代码
  countWalkSetCount = 0;
  UpdateData(TRUE);
  if (object_dis <= 0)object_dis = 40000;
  if (guide_line_k != 0)
    guide_line_x = float(guide_line_y - guide_line_b) / guide_line_k;
  UpdateData(FALSE);
  refleshFlag1 = 2; *refleshFlag1pointer = 2;//站坪扫描初始
  //refleshFlag2=1;*refleshFlag2pointer=1;//走步测试初始
  walktext = FALSE;
  scanyardToAdjust = TRUE;
  Detect_Thing_Init = 1;
  DtcTEdit_StandHigh_min = 0;
  NUM_NEW = 1;
  Scan_Cate = 3;
  Up_Y_Step = 0;
  int NUM_NEW_temp = 0;
  yard_scan_range = 0;//站坪扫描范围（1，2，3，4，5）
  yard_scan_obstacle_high = 0;//站坪扫描障碍物的高度
  yard_scan_obstacle_width = 0;//站坪扫描障碍物的宽度
  Y_All.clear();
  Z_All.clear();
  obstacle_dis.clear();
  adjustAlpha_ySum = 0;//用于站坪扫描后的校准
  adjustTimes = 0;//用于站坪扫描后的校准
  adjustEdit_StandHigh = 0;//用于站坪扫描后的校准，正数为往上抬的步数
  CString change;

  short int sx, ex, sy, ey;
  data_h = 0;
  data_order = 16;
  line_num = 0;
  positive = 6;
  next_frame = 5;
  walksetup = 0;
  stan_catch = 0;
  auto_catch = 0;
  stop_lp = FALSE;

  scan_mode = HORIZONTAL;
  loop = 0;
  frame_length = 7;
  /*float alpha_y_temp=((float)install_high-(float)object_high-100)/(float)object_dis;//-100使扫描位置抬高一点
  float angle=atan (alpha_y_temp);
  Edit_StandHigh = - angle*180/YSTEPANGLE/PI;
  if (Edit_StandHigh%2 != 0)
  {
    Edit_StandHigh++;
  }
  start_height = Edit_StandHigh;*/

  save_start_x.Format("%d", Edit_Start_x);
  save_end_x.Format("%d", Edit_End_x);
  save_start_y.Format("%d", Edit_Start_y);
  save_end_y.Format("%d", Edit_End_y);
  sx = Edit_Start_x / XSTEPANGLE;
  ex = Edit_End_x / XSTEPANGLE;
  sy = Edit_Start_y / YSTEPANGLE;
  ey = Edit_End_y / YSTEPANGLE;
  int standhigh = Edit_StandHigh / 2;
  sx = sx / 2;
  ex = ex / 2;
  sy = sy / 2;
  ey = ey / 2;
  int standheight;
  if (presacl != 0) {
    standheight = standhigh;
    data_length = abs(ex - sx) * 2 * presacl + data_order + 2;
  }
  else {
    standheight = Edit_StandHigh;
    data_length = abs(ex - sx) * 2 + data_order + 2;
  }


  // format.Format("44 02 00 08 %c%c ",Edit_format[1],Edit_format[2]);
  /*format.Format("53 01 00 0a 02 ");
  change.Format("%08x",standheight);
  Stand_High.Format("%c%c %c%c ",change[4],change[5],change[6],change[7]);



  change.Format("%08x", sx);
  start_x.Format("%c%c %c%c ",change[4],change[5],change[6],change[7]);

  change.Format("%08x", ex);
  end_x.Format("%c%c %c%c ",change[4],change[5],change[6],change[7]);

  change.Format("%08x", sy);
  start_y.Format("%c%c %c%c ",change[4],change[5],change[6],change[7]);

  change.Format("%08x", ey);
  end_y.Format("%c%c %c%c ",change[4],change[5],change[6],change[7]);

  m_strTXData = format+Stand_High+start_x+end_x+"00 03 01 ca";
  CByteArray hexdata;
  int len = String2Hex(m_strTXData, senddata);
  if (senddata[0]==0x43)frame_length=2;else if (senddata[0]==0x53)frame_length=7;glb_sp_data.deliver2send(senddata,len);*/
  int ver_two = 2;
  laser_parameters lp_guide = Doc.readlaserdatas(".\\test.xml", "guide1", "stop1");
  guide_line_k = lp_guide.guide_line_k;
  guide_line_b = lp_guide.guide_line_b;
  guide_line_x = lp_guide.guide_line_x;
  guide_line_y = lp_guide.guide_line_y;
  object_dis = lp_guide.object_dis;
  if (Detect_Thing_Init) {
    laser_machine_parameters lp = Doc.readlaserparamters(".\\test.xml");
    yard_scan_range = lp.yard_scan_range;
    yard_scan_obstacle_high = lp.yard_scan_obstacle_high;
    yard_scan_obstacle_width = lp.yard_scan_obstacle_width;

    laser_parameters lpy = Doc.readlaserdatas(".\\test.xml", Doc.guaidlinename, Doc.stoplinename);
    float guide_line_y_temp = lpy.guide_line_y - 10000;

    if (guide_line_y_temp < 20000)guide_line_y_temp = 20000;
    Edit_StandHigh_Org = Edit_StandHigh;//Org为全局变量，org为局部变量		
    Edit_Start_x_Org = Edit_Start_x;
    Edit_End_x_Org = Edit_End_x;
    if (yard_scan_range == 2) {
      Edit_Start_x = Edit_Start_x * 2;
      Edit_End_x = Edit_End_x * 2;
    }
    else if (yard_scan_range == 3) {
      Edit_Start_x = Edit_Start_x * 3;
      Edit_End_x = Edit_End_x * 3;
    }
    else if (yard_scan_range == 4) {
      Edit_Start_x = Edit_Start_x * 4;
      Edit_End_x = Edit_End_x * 4;
    }
    else if (yard_scan_range == 5) {
      Edit_Start_x = Edit_Start_x * 5;
      Edit_End_x = Edit_End_x * 5;
    }
    int DetTobject_dis_max = 120000;//最远扫描距离
    float alpha_y_temp = ((float)install_high) / (float)DetTobject_dis_max;
    float angle = atan(alpha_y_temp);
    Edit_StandHigh = -angle * 180 / (0.1125 / ver_two) / PI;//Edit_StandHigh的起始值

    Edit_StandHigh = 0;

    int Edit_StandHigh_temp = Edit_StandHigh / 2;
    Edit_StandHigh = Edit_StandHigh_temp * 2;
    alpha_y_temp = ((float)install_high) / (float)guide_line_y_temp;
    angle = atan(alpha_y_temp);
    DtcTEdit_StandHigh_min = -angle * 180 / (0.1125 / ver_two) / PI;//Edit_StandHigh的最小值
    int DtcTEdit_StandHigh_min_temp = DtcTEdit_StandHigh_min / 2;
    DtcTEdit_StandHigh_min = DtcTEdit_StandHigh_min_temp * 2;
    length_data = (data_length - data_order) / 2;//一行数据个数
    length_data *= yard_scan_range;
    //int step=(Edit_StandHigh_Org-Edit_StandHigh)/2;
    //stepMoveNoSet0(step, DOWN, VERTICAL);
    //Sleep(1000);
    Scan_Cate = 3;//0;
    Up_Y_Step = -2;
    OnAutocatch();
    DetectThing_SaveModle = 1;
    Detect_Thing_Init = 0;
    NUM_NEW = 1;
    //return;
  }
  m_strRXData = "站坪扫描后校准:启动";
  walktext_deflection = "执行中...";
  if (glb_led_obj_ptr != nullptr) glb_led_obj_ptr->sendmessage(2, 0, "", "");
  UpdateData(FALSE);
  refleshFlag1 = 2; *refleshFlag1pointer = 2;//站坪扫描：启动
}

void CLaster::OnBnClickedWalktest() {
  refleshFlag1 = -1; *refleshFlag1pointer = -1;
  refleshFlag2 = -1; *refleshFlag2pointer = -1;
  refleshFlag4 = -1; *refleshFlag4pointer = -1;
  refleshFlag3 = 0; *refleshFlag3pointer = 0;
  HrzScnProc = FALSE;
  x_walk = 0;
  y_walk = 0;
  // TODO: 在此添加控件通知处理程序代码
  countWalkSetCount = 0;
  UpdateData(TRUE);
  if (object_dis <= 0)object_dis = 40000;
  if (guide_line_k != 0)
    guide_line_x = float(guide_line_y - guide_line_b) / guide_line_k;
  UpdateData(FALSE);
  refleshFlag1 = 2; *refleshFlag1pointer = 2;//走步测试初始
  refleshFlag2 = 2; *refleshFlag2pointer = 2;//走步测试初始
  walktext = TRUE;
  scanyard = 0;
  scanyardToAdjust = 0;
  CString change;

  //画图，画引导线
  /*draw_getready_guideline();
  draw_white();
  draw_guideline();*/

  short int sx, ex, sy, ey;
  data_h = 0;
  data_order = 16;
  line_num = 0;
  positive = 6;
  next_frame = 5;
  walksetup = 0;
  stan_catch = 0;
  auto_catch = 0;
  stop_lp = FALSE;

  scan_mode = HORIZONTAL;
  loop = 0;
  float alpha_y_temp = ((float)install_high - (float)object_high - 100) / (float)object_dis;//-100使扫描位置抬高一点
  float angle = atan(alpha_y_temp);
  Edit_StandHigh_Org = Edit_StandHigh;
  Edit_StandHigh = -angle * 180 / YSTEPANGLE / PI;
  if (Edit_StandHigh % 2 != 0) {
    Edit_StandHigh++;
  }
  start_height = Edit_StandHigh;
  //stepMoveNoSet0((Edit_StandHigh - Edit_StandHigh_Org) / 2, UP, VERTICAL);
  //Sleep(500);
  // 	itoa(Edit_hundred,&hundred,10);
  // 	itoa(Edit_ten,&ten,10);
  // 	itoa(Edit_ge,&ge,10);

  /*CString number;
  if (format=="s0")
  {
  data_length=772;
  }
  number = "0000";*/
  // TODO: 在此添加控件通知处理程序代码

  /*m_strTXData=format+number;*/
  save_start_x.Format("%d", Edit_Start_x);
  save_end_x.Format("%d", Edit_End_x);
  save_start_y.Format("%d", Edit_Start_y);
  save_end_y.Format("%d", Edit_End_y);
  sx = Edit_Start_x / XSTEPANGLE;
  ex = Edit_End_x / XSTEPANGLE;
  sy = Edit_Start_y / YSTEPANGLE;
  ey = Edit_End_y / YSTEPANGLE;
  int standhigh = Edit_StandHigh / 2;
  sx = sx / 2;
  ex = ex / 2;
  sy = sy / 2;
  ey = ey / 2;
  int standheight;
  if (presacl != 0) {
    standheight = standhigh;
    data_length = abs(ex - sx) * 2 * presacl + data_order + 2;
  }
  else {
    standheight = Edit_StandHigh;
    data_length = abs(ex - sx) * 2 + data_order + 2;
  }

  frame_length = 7;
  // format.Format("44 02 00 08 %c%c ",Edit_format[1],Edit_format[2]);
  format.Format("53 01 00 0a 02 ");
  change.Format("%08x", standheight);
  Stand_High.Format("%c%c %c%c ", change[4], change[5], change[6], change[7]);



  change.Format("%08x", sx);
  start_x.Format("%c%c %c%c ", change[4], change[5], change[6], change[7]);

  change.Format("%08x", ex);
  end_x.Format("%c%c %c%c ", change[4], change[5], change[6], change[7]);

  change.Format("%08x", sy);
  start_y.Format("%c%c %c%c ", change[4], change[5], change[6], change[7]);

  change.Format("%08x", ey);
  end_y.Format("%c%c %c%c ", change[4], change[5], change[6], change[7]);

  m_strTXData = format + Stand_High + start_x + end_x + "00 03 01 ca";
  CByteArray hexdata;
  //int len=String2Hex(m_strTXData,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
  //m_ctrlComm.put_Output(COleVariant(hexdata)); //发送十六进制数据 

  //m_ctrlComm.put_RThreshold(data_length);  //参数1表示每当串口接收缓冲区中有多于或等于1个字符时将引发一个接收数据的OnComm事件 
  //m_ctrlComm.put_InputLen(0); //设置当前接收区数据长度为0 
  //m_ctrlComm.get_Input();//先预读缓冲区以清除残留数据
  int len = String2Hex(m_strTXData, senddata);
  if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7;
  glb_sp_data.deliver2send(senddata, len);
  m_strRXData = "走步测试:启动";
  walktext_deflection = "";
  if (glb_led_obj_ptr != nullptr) glb_led_obj_ptr->sendmessage(2, 0, "", "");
  UpdateData(FALSE);
  refleshFlag1 = 2; *refleshFlag1pointer = 2;//走步测试：启动
  //m_strRXData=m_strTXData;
  //UpdateData(FALSE); //更新编辑框内容
  //GetDlgItem(IDC_BTN_Clear)->EnableWindow(TRUE);
}


void CLaster::WalkSetUp() {
  int i, length_data, data_point;
  int ko, St, Sm, SmTemp;
  int locate_first, locate_end, max_num;
  float z, alpha_x, alpha_y = YSTEPANGLE * (-(float)Edit_StandHigh)*PI / 180;
  int dis_predict = (float)(install_high - object_high) / sin(alpha_y);
  int SmBreak;
  int SmStart, SmEnd, SmStartTemp, SmMinTemp = 0, SmMinData = 800000;
  double SmMin = 0.0;
  int direction = 0;

  max_num = 4;
  ci = Sm = 0;
  St = 0;
  locate_first = locate_end = 0;
  char ge;
  length_data = (data_length - data_order) / 2;//一行数据个数
  ge = '0';
  SmTemp = 0; SmBreak = -1;
  SmStart = 0; SmEnd = 0; SmStartTemp = 0;
  SmMin = 0; SmMinTemp = 0;
  SmMinData = 800000;
  Sm = 0;
  for (i = 0; i <= data_h; i++)//开始分析正反两行数据
  {
    if (USE_BACK) {
      SmTemp = 0; SmBreak = -1;
      SmStart = 0; SmEnd = 0; SmStartTemp = 0;
      SmMin = 0; SmMinTemp = 0;
      SmMinData = 800000;
      Sm = 0;
    }
    ko = i % length_data;
    if (ko == 0 && i != 0) {
      if (ge == '0') {
        data_point = i - length_data;
        locate_first = max_num - 3000;
        locate_end = data_point - length_data;
        for (data_point; data_point < i; data_point++)//开始分析反向行的数据
        {
          if (save_data[data_point] > 2000)//正常点
          {
            z = (float)install_high - (float)save_data[data_point] * sin(alpha_y);
            if (save_data[data_point] > dis_predict - 20000 && save_data[data_point] < dis_predict + 5000 && z>500) {//感兴趣的点
              St++;
              if (SmBreak == 1) {
                SmTemp++;
                if (save_data[data_point] < save_data[SmMinTemp]) {
                  SmMinTemp = data_point;
                }
              }
              else if (SmBreak == -1) {
                SmBreak = 1;
                SmTemp++;
                SmStartTemp = data_point;//标【在这一大行的号】
                SmMinTemp = data_point;
              }
              else//SmBreak==0
              {
                SmBreak = 1;
                SmTemp += 2;
                save_data[data_point - 1] = (save_data[data_point - 2] + save_data[data_point]) / 2;
                if (save_data[data_point] < save_data[SmMinTemp]) {
                  SmMinTemp = data_point;
                }
              }
            }
            else//不感兴趣的点
            {
              if (SmBreak == 1) {
                SmBreak = 0;
              }
              else if (SmBreak == 0) {
                SmBreak = -1;
                //Sm=Sm>(SmTemp-1)?Sm:(SmTemp-1);		
                if (save_data[SmMinTemp] < SmMinData)//旧版if(Sm<SmTemp)
                {
                  Sm = SmTemp;
                  SmStart = SmStartTemp;
                  SmEnd = SmStart + Sm - 1;
                  SmMin = SmMinTemp;
                  SmMinData = save_data[SmMinTemp];
                }
                SmTemp = 0;
              }
            }
            //St_point[locate_end] = save_data[data_point];
            //St++;
          }
          if (data_point == i - 1)//在最后一个点，更新Sm
          {
            if (SmBreak != -1 && save_data[SmMinTemp] < SmMinData)//旧版Sm<SmTemp)
            {
              Sm = SmTemp;
              SmStart = SmStartTemp;//【在这一大行的标号】
              SmEnd = SmStart + Sm - 1;//【在这一大行的标号】
              SmMin = SmMinTemp;
              SmMinData = save_data[SmMinTemp];
            }
          }
          locate_end++;
        }//完成分析反向行的数据
        //Sm = FindSm(&locate_end);
        //data_point = ConfirmCentre(locate_end);
        if (USE_BACK) {
          SmMin = (SmStart + SmEnd) / 2.0;
          bool bCaputred = 0;
          bCaputred = JundgeCatch(Sm, St, SmStart, SmMin, 0);
          if (bCaputred) {
            guide_line_k = 0;//SmMin;
          }
          else {
            guide_line_k = 0;
          }
        }

        ge = '5';
      }
      else {
        if (USE_BACK) {
          data_point = i - length_data;
          locate_first = max_num - 3000;
          locate_end = data_point - length_data;
          for (data_point; data_point < i; data_point++)//开始分析反向行的数据
          {
            if (save_data[data_point] > 2000)//正常点
            {
              z = (float)install_high - (float)save_data[data_point] * sin(alpha_y);
              if (save_data[data_point] > dis_predict - 20000 && save_data[data_point] < dis_predict + 5000 && z>500) {//感兴趣的点
                St++;
                if (SmBreak == 1) {
                  SmTemp++;
                  if (save_data[data_point] < save_data[SmMinTemp]) {
                    SmMinTemp = data_point;
                  }
                }
                else if (SmBreak == -1) {
                  SmBreak = 1;
                  SmTemp++;
                  SmStartTemp = data_point;//标【在这一大行的号】
                  SmMinTemp = data_point;
                }
                else//SmBreak==0
                {
                  SmBreak = 1;
                  SmTemp += 2;
                  save_data[data_point - 1] = (save_data[data_point - 2] + save_data[data_point]) / 2;
                  if (save_data[data_point] < save_data[SmMinTemp]) {
                    SmMinTemp = data_point;
                  }
                }
              }
              else//不感兴趣的点
              {
                if (SmBreak == 1) {
                  SmBreak = 0;
                }
                else if (SmBreak == 0) {
                  SmBreak = -1;
                  //Sm=Sm>(SmTemp-1)?Sm:(SmTemp-1);		
                  if (save_data[SmMinTemp] < SmMinData)//旧版if(Sm<SmTemp)
                  {
                    Sm = SmTemp;
                    SmStart = SmStartTemp;
                    SmEnd = SmStart + Sm - 1;
                    SmMin = SmMinTemp;
                    SmMinData = save_data[SmMinTemp];
                  }
                  SmTemp = 0;
                }
              }
              //St_point[locate_end] = save_data[data_point];
              //St++;
            }
            if (data_point == i - 1)//在最后一个点，更新Sm
            {
              if (SmBreak != -1 && save_data[SmMinTemp] < SmMinData)//旧版Sm<SmTemp)
              {
                Sm = SmTemp;
                SmStart = SmStartTemp;//【在这一大行的标号】
                SmEnd = SmStart + Sm - 1;//【在这一大行的标号】
                SmMin = SmMinTemp;
                SmMinData = save_data[SmMinTemp];
              }
            }
            locate_end++;
          }//完成分析反向行的数据
          //Sm = FindSm(&locate_end);
          //data_point = ConfirmCentre(locate_end);
        }//if(USE_BACK)

      }
    }
  }//完成分析正反两行数据
  //data_point=SmMin;//距离最近点
  //JundgeCatch0(Sm,St,data_point,length_data);
  SmMin = (SmStart + SmEnd) / 2.0;
  if (SmMin > SmStart + Sm - 1)SmMin = SmStart + Sm - 1;
  bool bCaputred;
  bCaputred = JundgeCatch(Sm, St, SmStart, SmMin, length_data);
  if (bCaputred) {
    Jundgeheight(Sm, St, SmMin, length_data);
  }
  else if (loop != 0 && loop > 50) {
    Jundgeheight(Sm, St, SmMin, length_data);
  }
  else if (loop != 0) {
    loop++;
  }

  if (length_data != 0) {
    CString strFileName1, str;
    str.Format("数据%d", Edit_StandHigh);
    ca += str;
    strFileName1 = lo + ca + te;
    _mkdir(lo);
    savefile(strFileName1);
    ca.Format("%d", NUM);
    NUM++;
  }



  if (stop_lp == FALSE) {
    OnAutocatch();
  }
  else {
    stop_lp = FALSE;
  }
}

void CLaster::ComfirmType() {
  /*if(comfirm_type==1)
  {
  comfirm_type++;
  OnAutocatch();
  Sleep(200);
  return;
  }*/
  int i, length_data, data_point, SmMinForward = 0;
  int ko, St, Sm, SmTemp;
  int locate_first, locate_end, max_num;
  float /*alpha_x,*/alpha_y = YSTEPANGLE * (-(float)Edit_StandHigh)*PI / 180;
  //int dis_predict=(float)(install_high-object_high)/sin(alpha_y);
  float tempdis = (float(guide_line_x)*(guide_line_x)+float(guide_line_y)*(guide_line_y)+float(install_high)*install_high);
  float dis_predict = sqrt(tempdis);//( float(guide_line_x)*(guide_line_x) + (guide_line_y)*(guide_line_y) +install_high*install_high );
  float z;
  //float alpha_x=(float)locate_step*XSTEPANGLE*PI/180;
  //float alpha_y=YSTEPANGLE*(-(float)Edit_StandHigh)*PI/180;

  int SmBreak;
  int SmStart, SmEnd, SmStartTemp, SmMinTemp = 0, SmMinData = 800000;
  double SmMin = 0.0;
  int direction = 0;
  bool bCaputred = 0, bCaputred2 = 0;

  max_num = 4;
  ci = Sm = 0;
  St = 0;
  locate_first = locate_end = 0;
  char ge;
  length_data = (data_length - data_order) / 2;//一行数据个数
  ge = '0';
  SmTemp = 0; SmBreak = -1;
  SmStart = 0; SmEnd = 0; SmStartTemp = 0;
  SmMin = 0; SmMinTemp = 0;
  Sm = 0;
  for (i = 0; i <= data_h; i++)//开始分析正反两行数据
  {

    ko = i % length_data;
    if (ko == 0 && i != 0) {
      if (ge == '0') {
        data_point = i - length_data;
        locate_first = max_num - 3000;
        locate_end = data_point - length_data;
        for (data_point; data_point < i; data_point++)//开始分析反向行的数据
        {
          if (save_data[data_point] > 2000)//正常点
          {
            z = (float)install_high - (float)save_data[data_point] * sin(alpha_y);
            if (save_data[data_point] > dis_predict - 5000 && save_data[data_point] < dis_predict + 5000 && z>500) {//感兴趣的点
              St++;
              if (SmBreak == 1) {
                SmTemp++;
                if (save_data[data_point] < save_data[SmMinTemp]) {
                  SmMinTemp = data_point;
                }
              }
              else if (SmBreak == -1) {
                SmBreak = 1;
                SmTemp++;
                SmStartTemp = data_point;//标【在这一大行的号】
                SmMinTemp = data_point;
              }
              else//SmBreak==0
              {
                SmBreak = 1;
                SmTemp += 2;
                save_data[data_point - 1] = (save_data[data_point - 2] + save_data[data_point]) / 2;
                if (save_data[data_point] < save_data[SmMinTemp]) {
                  SmMinTemp = data_point;
                }
              }
            }
            else//不感兴趣的点
            {
              if (SmBreak == 1) {
                SmBreak = 0;
              }
              else if (SmBreak == 0) {
                SmBreak = -1;
                //Sm=Sm>(SmTemp-1)?Sm:(SmTemp-1);		
                if (save_data[SmMinTemp] < SmMinData)//旧版if(Sm<SmTemp)
                {
                  Sm = SmTemp;
                  SmStart = SmStartTemp;
                  SmEnd = SmStart + Sm - 1;
                  SmMin = SmMinTemp;
                  SmMinData = save_data[SmMinTemp];
                }
                SmTemp = 0;
              }
            }
            //St_point[locate_end] = save_data[data_point];
            //St++;
          }
          if (data_point == i - 1)//在最后一个点，更新Sm
          {
            if (SmBreak != -1 && save_data[SmMinTemp] < SmMinData)//旧版Sm<SmTemp)
            {
              Sm = SmTemp;
              SmStart = SmStartTemp;//【在这一大行的标号】
              SmEnd = SmStart + Sm - 1;//【在这一大行的标号】
              SmMin = SmMinTemp;
              SmMinData = save_data[SmMinTemp];
            }
          }
          locate_end++;
        }//完成分析反向行的数据
        //Sm = FindSm(&locate_end);
        //data_point = ConfirmCentre(locate_end);
        SmMin = (SmStart + SmEnd) / 2 + 2;
        if (SmMin > SmStart + Sm - 1)SmMin = SmStart + Sm - 1;
        bCaputred = 0;
        if (Sm >= 2)
          bCaputred = TRUE;
        if (bCaputred) {
          SmMinForward = SmMin;
          if (SmMinForward > SmStart + Sm - 1)SmMinForward = SmStart + Sm - 1;
        }
        else {
          SmMinForward = 0;
        }

        SmTemp = 0; SmBreak = -1;
        SmStart = 0; SmEnd = 0; SmStartTemp = 0;
        SmMin = 0; SmMinTemp = 0;
        SmMinData = 800000;
        Sm = 0;

        ge = '5';
      }
      else {
        data_point = i - length_data;
        locate_first = max_num - 3000;
        locate_end = data_point - length_data;
        for (data_point; data_point < i; data_point++)//开始分析反向行的数据
        {
          if (save_data[data_point] > 2000)//正常点
          {
            z = (float)install_high - (float)save_data[data_point] * sin(alpha_y);
            if (save_data[data_point] > dis_predict - 5000 && save_data[data_point] < dis_predict + 5000 && z>500) {//感兴趣的点
              St++;
              if (SmBreak == 1) {
                SmTemp++;
                if (save_data[data_point] < save_data[SmMinTemp]) {
                  SmMinTemp = data_point;
                }
              }
              else if (SmBreak == -1) {
                SmBreak = 1;
                SmTemp++;
                SmStartTemp = data_point;//标【在这一大行的号】
                SmMinTemp = data_point;
              }
              else//SmBreak==0
              {
                SmBreak = 1;
                SmTemp += 2;
                save_data[data_point - 1] = (save_data[data_point - 2] + save_data[data_point]) / 2;
                if (save_data[data_point] < save_data[SmMinTemp]) {
                  SmMinTemp = data_point;
                }
              }
            }
            else//不感兴趣的点
            {
              if (SmBreak == 1) {
                SmBreak = 0;
              }
              else if (SmBreak == 0) {
                SmBreak = -1;
                //Sm=Sm>(SmTemp-1)?Sm:(SmTemp-1);		
                if (save_data[SmMinTemp] < SmMinData)//旧版if(Sm<SmTemp)
                {
                  Sm = SmTemp;
                  SmStart = SmStartTemp;
                  SmEnd = SmStart + Sm - 1;
                  SmMin = SmMinTemp;
                  SmMinData = save_data[SmMinTemp];
                }
                SmTemp = 0;
              }
            }
            //St_point[locate_end] = save_data[data_point];
            //St++;
          }
          if (data_point == i - 1)//在最后一个点，更新Sm
          {
            if (SmBreak != -1 && save_data[SmMinTemp] < SmMinData)//旧版Sm<SmTemp)
            {
              Sm = SmTemp;
              SmStart = SmStartTemp;//【在这一大行的标号】
              SmEnd = SmStart + Sm - 1;//【在这一大行的标号】
              SmMin = SmMinTemp;
              SmMinData = save_data[SmMinTemp];
            }
          }
          locate_end++;
        }//完成分析反向行的数据
        //Sm = FindSm(&locate_end);
        //data_point = ConfirmCentre(locate_end);
      }
    }
  }//完成分析正反两行数据
  //data_point=SmMin;//距离最近点
  //JundgeCatch0(Sm,St,data_point,length_data);

  bCaputred2 = 0;
  SmMin = (SmStart + SmEnd) / 2.0;
  if (SmMin > SmStart + Sm - 1)SmMin = SmStart + Sm - 1;
  if (Sm >= 2)
    bCaputred2 = TRUE;

  if (bCaputred2) {
    direction = JundgeComfirmType(Sm, St, SmMin, length_data, 0);//,SmMinForward);
  }
  /*else if(bCaputred)
  {
    direction = JundgeComfirmType(Sm,St,SmMinForward,0,0);
  }*/
  else {
    m_strRXData = "停止点确认";
    walktext_deflection.Format("停止点位置：\n%d, %d\n未找到目标", guide_line_x, guide_line_y);
    //walktext_deflection.Format("停止点位置：\n%d, %d\n目标位置：\n%d, %d\n相距.2f%m",guide_line_x,guide_line_y,int(x),int(y),dis);
    save_hrzCali_record(walktext_deflection);
    UpdateData(FALSE);
    refleshFlag1 = 1; *refleshFlag1pointer = 1; *refleshFlag1pointer = 1;//验证停止点：未找到目标
  }
  if (length_data != 0) {
    CString strFileName1, str;
    str.Format("数据%d", Edit_StandHigh);
    ca += str;
    strFileName1 = lo + ca + te;
    savefile(strFileName1);
    ca.Format("%d", NUM);
    NUM++;
  }


  //if (stop_lp == FALSE)
  //{
  //	// 		time_end = (double)clock();
  //	// 		time_fin = time_end-time_start;
  //	OnAutocatch();
  //}
  //else
  //{
  //	stop_lp = FALSE;
  //}

  comfirm_type = 0;
  TRACE("comfirm_type=0");
}

void CLaster::ScanAll_org()//由于测试出现蓝屏，把原本的备份为org，把走步测试的代码复制并改写成了现在的ScanAll()
{
  /*if(comfirm_type==1)
  {
  comfirm_type++;
  OnAutocatch();
  Sleep(200);
  return;
  }*/
  save_hrzCali_record("ScanAll()函数");
  countNearPoint[0] = 0;
  countNearPoint[1] = 0;
  int i, length_data, data_point, SmMinForward = 0;
  int ko, St, Sm, SmTemp;
  int locate_first, locate_end, max_num;
  float /*alpha_x,*/alpha_y = YSTEPANGLE * (-(float)Edit_StandHigh)*PI / 180;
  //int dis_predict=(float)(install_high-object_high)/sin(alpha_y);
  float tempdis = (float(guide_line_x)*(guide_line_x)+float(guide_line_y)*(guide_line_y)+float(install_high)*install_high);
  float dis_predict = sqrt(tempdis);//( float(guide_line_x)*(guide_line_x) + (guide_line_y)*(guide_line_y) +install_high*install_high );
  float z;
  //float alpha_x=(float)locate_step*XSTEPANGLE*PI/180;
  //float alpha_y=YSTEPANGLE*(-(float)Edit_StandHigh)*PI/180;

  int SmBreak;
  int SmStart, SmEnd, SmStartTemp, SmMin = 0, SmMinTemp = 0, SmMinData = 800000;
  int direction = 0;
  bool bCaputred = 0, bCaputred2 = 0;

  max_num = 4;
  ci = Sm = 0;
  St = 0;
  locate_first = locate_end = 0;
  char ge;
  length_data = (data_length - data_order) / 2;//一行数据个数
  TRACE("窗口扫描data_length=%d, data_order=%d\n", data_length, data_order);
  ge = '0';
  for (i = 0; i <= data_h; i++)//开始分析正反两行数据
  {
    SmTemp = 0; SmBreak = -1;
    SmStart = 0; SmEnd = 0; SmStartTemp = 0;
    SmMin = 0; SmMinTemp = 0;

    ko = i % length_data;
    if (ko == 0 && i != 0) {
      if (ge == '0') {
        data_point = i - length_data;
        locate_first = max_num - 3000;
        locate_end = data_point - length_data;
        for (data_point; data_point < i; data_point++)//开始分析反向行的数据
        {
          if (save_data[data_point] > 2000)//正常点
          {
            z = (float)install_high - (float)save_data[data_point] * sin(alpha_y);
            if (save_data[data_point] > dis_predict - 5000 && save_data[data_point] < dis_predict + 2000 && z>500) {//感兴趣的点
              St++;
              if (SmBreak == 1) {
                SmTemp++;
                if (save_data[data_point] < save_data[SmMinTemp]) {
                  SmMinTemp = data_point;
                }
              }
              else if (SmBreak == -1) {
                SmBreak = 1;
                SmTemp++;
                SmStartTemp = data_point;//标【在这一大行的号】
                SmMinTemp = data_point;
              }
              else//SmBreak==0
              {
                SmBreak = 1;
                SmTemp += 2;
                save_data[data_point - 1] = (save_data[data_point - 2] + save_data[data_point]) / 2;
                if (save_data[data_point] < save_data[SmMinTemp]) {
                  SmMinTemp = data_point;
                }
              }
            }
            else//不感兴趣的点
            {
              if (SmBreak == 1) {
                SmBreak = 0;
              }
              else if (SmBreak == 0) {
                SmBreak = -1;
                //Sm=Sm>(SmTemp-1)?Sm:(SmTemp-1);		
                if (save_data[SmMinTemp] < SmMinData)//旧版if(Sm<SmTemp)
                {
                  Sm = SmTemp;
                  SmStart = SmStartTemp;
                  SmEnd = SmStart + Sm - 1;
                  SmMin = SmMinTemp;
                  SmMinData = save_data[SmMinTemp];
                }
                SmTemp = 0;
              }
            }
            //St_point[locate_end] = save_data[data_point];
            //St++;
          }
          else if (save_data[data_point] > 4 && save_data[data_point] < 2000)//玻璃上的点
          {
            countNearPoint[0]++;
          }
          if (data_point == i - 1)//在最后一个点，更新Sm
          {
            if (SmBreak != -1 && save_data[SmMinTemp] < SmMinData)//旧版Sm<SmTemp)
            {
              Sm = SmTemp;
              SmStart = SmStartTemp;//【在这一大行的标号】
              SmEnd = SmStart + Sm - 1;//【在这一大行的标号】
              SmMin = SmMinTemp;
              SmMinData = save_data[SmMinTemp];
            }
          }
          locate_end++;
        }//完成分析反向行的数据
        //Sm = FindSm(&locate_end);
        //data_point = ConfirmCentre(locate_end);
        SmMin = (SmStart + SmEnd) / 2;
        bCaputred = 0;
        if (Sm >= 2)
          bCaputred = TRUE;
        if (bCaputred) {
          SmMinForward = SmMin;
          if (SmMinForward > SmStart + Sm - 1)SmMinForward = SmStart + Sm - 1;
        }
        else {
          SmMinForward = 0;
        }

        SmTemp = 0; SmBreak = -1;
        SmStart = 0; SmEnd = 0; SmStartTemp = 0;
        SmMin = 0; SmMinTemp = 0;
        SmMinData = 800000;

        ge = '5';
      }
      else {
        data_point = i - length_data;
        locate_first = max_num - 3000;
        locate_end = data_point - length_data;
        for (data_point; data_point < i; data_point++)//开始分析反向行的数据
        {
          if (save_data[data_point] > 2000)//正常点
          {
            z = (float)install_high - (float)save_data[data_point] * sin(alpha_y);
            if (save_data[data_point] > dis_predict - 5000 && save_data[data_point] < dis_predict + 2000 && z>500) {//感兴趣的点
              St++;
              if (SmBreak == 1) {
                SmTemp++;
                if (save_data[data_point] < save_data[SmMinTemp]) {
                  SmMinTemp = data_point;
                }
              }
              else if (SmBreak == -1) {
                SmBreak = 1;
                SmTemp++;
                SmStartTemp = data_point;//标【在这一大行的号】
                SmMinTemp = data_point;
              }
              else//SmBreak==0
              {
                SmBreak = 1;
                SmTemp += 2;
                if (save_data[data_point] < save_data[SmMinTemp]) {
                  SmMinTemp = data_point;
                }
              }
            }
            else//不感兴趣的点
            {
              if (SmBreak == 1) {
                SmBreak = 0;
              }
              else if (SmBreak == 0) {
                SmBreak = -1;
                //Sm=Sm>(SmTemp-1)?Sm:(SmTemp-1);		
                if (save_data[SmMinTemp] < SmMinData)//旧版if(Sm<SmTemp)
                {
                  Sm = SmTemp;
                  SmStart = SmStartTemp;
                  SmEnd = SmStart + Sm - 1;
                  SmMin = SmMinTemp;
                  SmMinData = save_data[SmMinTemp];
                }
                SmTemp = 0;
              }
            }
            //St_point[locate_end] = save_data[data_point];
            //St++;
          }
          else if (save_data[data_point] > 4 && save_data[data_point] < 2000)//玻璃上的点
          {
            countNearPoint[1]++;
          }
          if (data_point == i - 1)//在最后一个点，更新Sm
          {
            if (SmBreak != -1 && save_data[SmMinTemp] < SmMinData)//旧版Sm<SmTemp)
            {
              Sm = SmTemp;
              SmStart = SmStartTemp;//【在这一大行的标号】
              SmEnd = SmStart + Sm - 1;//【在这一大行的标号】
              SmMin = SmMinTemp;
              SmMinData = save_data[SmMinTemp];
            }
          }
          locate_end++;
        }//完成分析反向行的数据
        //Sm = FindSm(&locate_end);
        //data_point = ConfirmCentre(locate_end);
      }
    }
  }//完成分析正反两行数据
  //data_point=SmMin;//距离最近点
  //JundgeCatch0(Sm,St,data_point,length_data);

  bCaputred2 = 0;
  SmMin = (SmStart + SmEnd) / 2.0;
  if (SmMin > SmStart + Sm - 1)SmMin = SmStart + Sm - 1;
  if (Sm >= 2)
    bCaputred2 = TRUE;

  //if (bCaputred2)
  //{		
  //	direction = JundgeComfirmType(Sm,St,SmMin,length_data,SmMinForward);
  //}
  //else if(bCaputred)
  //{
  //	direction = JundgeComfirmType(Sm,St,SmMinForward,0,0);
  //}
  //else
  //{
  //	m_strRXData="停止点确认";
  //	walktext_deflection.Format("停止点位置：\n%d, %d\n未找到目标",guide_line_x,guide_line_y);
  //	//walktext_deflection.Format("停止点位置：\n%d, %d\n目标位置：\n%d, %d\n相距.2f%m",guide_line_x,guide_line_y,int(x),int(y),dis);
  //	UpdateData(FALSE);
  //}

  if ((countNearPoint[0] > 5 && float(countNearPoint[0]) / length_data > 0.05) || (countNearPoint[1] > 5 && float(countNearPoint[1]) / length_data > 0.05))//玻璃点数过多
  {
    save_hrzCali_record("窗口扫描：已停止");
    m_strRXData = "窗口扫描：已停止";
    walktext_deflection.Format("出现视界阻挡\n可能是\n玻璃前方的障碍物\n或玻璃上的污点");//,guide_line_x,guide_line_y);
    //UpdateData(FALSE);
    save_hrzCali_record(walktext_deflection);

    stop_lp = FALSE;
    scan_all = 0;
  }
  else {
    if (float(Edit_StandHigh) / 2 - 1 > -float(VertConfirmSteps) + 20)//继续扫描
    {
      CString content;
      content.Format("窗口扫描：继续扫描 Edit_StandHigh=%d\ncountNearPoint[0]=%d,countNearPoint[1]=%d,length_data=%d", Edit_StandHigh, countNearPoint[0], countNearPoint[1], length_data);
      save_hrzCali_record(content);
      Edit_StandHigh = Edit_StandHigh - 2;
      //UpdateData(FALSE);
      content.Format("窗口扫描：继续扫描 Edit_StandHigh变为%d\n", Edit_StandHigh);
      save_hrzCali_record(content);
      TRACE(content);
      Sleep(100);
      OnAutocatch();
      Sleep(10);
    }
    else//停止扫描
    {
      CString content;
      content.Format("窗口扫描：完成 Edit_StandHigh=%d", Edit_StandHigh);
      save_hrzCali_record(content);
      m_strRXData = "窗口扫描：完成";
      walktext_deflection.Format("未出现视界阻挡");
      save_hrzCali_record(walktext_deflection);
      //UpdateData(FALSE);
      stop_lp = FALSE;
      scan_all = 0;
    }
  }

  if (length_data != 0) {
    CString strFileName1, str;
    str.Format("数据%d", Edit_StandHigh);
    ca += str;
    strFileName1 = lo + ca + te;
    savefile(strFileName1);
    ca.Format("%d", NUM);
    NUM++;
  }
  //if (stop_lp == FALSE)
  //{
  //	// 		time_end = (double)clock();
  //	// 		time_fin = time_end-time_start;
  //	OnAutocatch();
  //}
  //else
  //{
  //	stop_lp = FALSE;
  //}
  //scan_all = 0;
}
void CLaster::ScanAll()//由于测试出现蓝屏，把原本的备份为org，把走步测试的代码复制并改写成了现在的ScanAll()
{
  countNearPoint[0] = 0;
  countNearPoint[1] = 0;
  int i, length_data, data_point, SmMinForward = 0;
  int ko, St, Sm, SmTemp;
  int locate_first, locate_end, max_num;
  float z,/*alpha_x,*/alpha_y = YSTEPANGLE * (-(float)Edit_StandHigh)*PI / 180;
  int dis_predict = (float)(install_high - object_high) / sin(alpha_y);
  int SmBreak;
  int SmStart, SmEnd, SmStartTemp, SmMin = 0, SmMinTemp = 0, SmMinData = 800000;
  int direction = 0;

  max_num = 4;
  ci = Sm = 0;
  St = 0;
  locate_first = locate_end = 0;
  char ge;
  length_data = (data_length - data_order) / 2;//一行数据个数
  TRACE("走不测试data_length=%d, data_order=%d\n", data_length, data_order);
  ge = '0';
  for (i = 0; i <= data_h; i++)//开始分析正反两行数据
  {
    SmTemp = 0; SmBreak = -1;
    SmStart = 0; SmEnd = 0; SmStartTemp = 0;
    SmMin = 0; SmMinTemp = 0;
    SmMinData = 800000;
    Sm = 0;
    ko = i % length_data;
    if (ko == 0 && i != 0) {
      if (ge == '0') {
        data_point = i - length_data;
        locate_first = max_num - 3000;
        locate_end = data_point - length_data;
        for (data_point; data_point < i; data_point++)//开始分析反向行的数据
        {
          if (save_data[data_point] > 2000)//正常点
          {
            z = (float)install_high - (float)save_data[data_point] * sin(alpha_y);
            if (save_data[data_point] > dis_predict - 20000 && save_data[data_point] < dis_predict + 3000 && z>500) {//感兴趣的点
              St++;
              if (SmBreak == 1) {
                SmTemp++;
                if (save_data[data_point] < save_data[SmMinTemp]) {
                  SmMinTemp = data_point;
                }
              }
              else if (SmBreak == -1) {
                SmBreak = 1;
                SmTemp++;
                SmStartTemp = data_point;//标【在这一大行的号】
                SmMinTemp = data_point;
              }
              else//SmBreak==0
              {
                SmBreak = 1;
                SmTemp += 2;
                save_data[data_point - 1] = (save_data[data_point - 2] + save_data[data_point]) / 2;
                if (save_data[data_point] < save_data[SmMinTemp]) {
                  SmMinTemp = data_point;
                }
              }
            }
            else//不感兴趣的点
            {
              if (SmBreak == 1) {
                SmBreak = 0;
              }
              else if (SmBreak == 0) {
                SmBreak = -1;
                //Sm=Sm>(SmTemp-1)?Sm:(SmTemp-1);		
                if (save_data[SmMinTemp] < SmMinData)//旧版if(Sm<SmTemp)
                {
                  Sm = SmTemp;
                  SmStart = SmStartTemp;
                  SmEnd = SmStart + Sm - 1;
                  SmMin = SmMinTemp;
                  SmMinData = save_data[SmMinTemp];
                }
                SmTemp = 0;
              }
            }
            //St_point[locate_end] = save_data[data_point];
            //St++;
          }
          else if (save_data[data_point] > 4 && save_data[data_point] < 2000)//玻璃上的点
          {
            countNearPoint[0]++;
          }
          if (data_point == i - 1)//在最后一个点，更新Sm
          {
            if (SmBreak != -1 && save_data[SmMinTemp] < SmMinData)//旧版Sm<SmTemp)
            {
              Sm = SmTemp;
              SmStart = SmStartTemp;//【在这一大行的标号】
              SmEnd = SmStart + Sm - 1;//【在这一大行的标号】
              SmMin = SmMinTemp;
              SmMinData = save_data[SmMinTemp];
            }
          }
          locate_end++;
        }//完成分析反向行的数据
        //Sm = FindSm(&locate_end);
        //data_point = ConfirmCentre(locate_end);
        SmMin = (SmStart + SmEnd) / 2 + 2;
        if (SmMin > SmStart + Sm - 1)SmMin = SmStart + Sm - 1;
        bool bCaputred = 0;
        bCaputred = JundgeCatch(Sm, St, SmStart, SmMin, 0);
        if (bCaputred) {
          SmMinForward = SmMin + 2;
          if (SmMinForward > SmStart + Sm - 1)SmMinForward = SmStart + Sm - 1;
        }
        else {
          SmMinForward = 0;
        }
        SmMinData = 800000;
        ge = '5';
      }
      else {
        data_point = i - length_data;
        locate_first = max_num - 3000;
        locate_end = data_point - length_data;
        for (data_point; data_point < i; data_point++)//开始分析反向行的数据
        {
          if (save_data[data_point] > 2000)//正常点
          {
            z = (float)install_high - (float)save_data[data_point] * sin(alpha_y);
            if (save_data[data_point] > dis_predict - 20000 && save_data[data_point] < dis_predict + 3000 && z>500) {//感兴趣的点
              St++;
              if (SmBreak == 1) {
                SmTemp++;
                if (save_data[data_point] < save_data[SmMinTemp]) {
                  SmMinTemp = data_point;
                }
              }
              else if (SmBreak == -1) {
                SmBreak = 1;
                SmTemp++;
                SmStartTemp = data_point;//标【在这一大行的号】
                SmMinTemp = data_point;
              }
              else//SmBreak==0
              {
                SmBreak = 1;
                SmTemp += 2;
                save_data[data_point - 1] = (save_data[data_point - 2] + save_data[data_point]) / 2;
                if (save_data[data_point] < save_data[SmMinTemp]) {
                  SmMinTemp = data_point;
                }
              }
            }
            else//不感兴趣的点
            {
              if (SmBreak == 1) {
                SmBreak = 0;
              }
              else if (SmBreak == 0) {
                SmBreak = -1;
                //Sm=Sm>(SmTemp-1)?Sm:(SmTemp-1);		
                if (save_data[SmMinTemp] < SmMinData)//旧版if(Sm<SmTemp)
                {
                  Sm = SmTemp;
                  SmStart = SmStartTemp;
                  SmEnd = SmStart + Sm - 1;
                  SmMin = SmMinTemp;
                  SmMinData = save_data[SmMinTemp];
                }
                SmTemp = 0;
              }
            }
            //St_point[locate_end] = save_data[data_point];
            //St++;
          }
          else if (save_data[data_point] > 4 && save_data[data_point] < 2000)//玻璃上的点
          {
            countNearPoint[1]++;
          }
          if (data_point == i - 1)//在最后一个点，更新Sm
          {
            if (SmBreak != -1 && save_data[SmMinTemp] < SmMinData)//旧版Sm<SmTemp)
            {
              Sm = SmTemp;
              SmStart = SmStartTemp;//【在这一大行的标号】
              SmEnd = SmStart + Sm - 1;//【在这一大行的标号】
              SmMin = SmMinTemp;
              SmMinData = save_data[SmMinTemp];
            }
          }
          locate_end++;
        }//完成分析反向行的数据
        //Sm = FindSm(&locate_end);
        //data_point = ConfirmCentre(locate_end);
      }
    }
  }//完成分析正反两行数据
  //data_point=SmMin;//距离最近点
  //JundgeCatch0(Sm,St,data_point,length_data);

  bool bCaputred;
  bCaputred = JundgeCatch(Sm, St, SmStart, SmMin, length_data);
  SmMin = (SmStart + SmEnd) / 2;
  if (SmMin > SmStart + Sm - 1)SmMin = SmStart + Sm - 1;

  if (0)//bCaputred)
  {
    direction = Jundgemid(Sm, St, SmMin, length_data, 0);//SmMinForward);
  }
  CString content;
  if ((countNearPoint[0] > 5 && float(countNearPoint[0]) / length_data > 0.05) || (countNearPoint[1] > 5 && float(countNearPoint[1]) / length_data > 0.05))//玻璃点数过多
  {
    content.Format("障碍物过多，%d, %d\n", countNearPoint[0], countNearPoint[1]);
    TRACE(content);
    save_hrzCali_record("窗口扫描：已停止");
    m_strRXData = "窗口扫描：已停止";
    walktext_deflection.Format("出现视界阻挡\n可能是\n玻璃前方的障碍物\n或玻璃上的污点");//,guide_line_x,guide_line_y);
    UpdateData(FALSE);
    //refleshFlag1=1;*refleshFlag1pointer=1;*refleshFlag1pointer=1;//窗口扫描发现障碍物
    save_hrzCali_record(walktext_deflection);

    refleshFlag1 = 1; *refleshFlag1pointer = 1;//窗口扫描完成 或 停止
    scan_all = 0;
  }
  else {
    if (float(Edit_StandHigh) / 2 - 1 > -float(VertConfirmSteps) + 30)//继续扫描
    {
      content.Format("没有障碍物，%d, %d\n", countNearPoint[0], countNearPoint[1]);
      TRACE(content);
      content.Format("窗口扫描：继续扫描 Edit_StandHigh=%d\ncountNearPoint[0]=%d,countNearPoint[1]=%d,length_data=%d", Edit_StandHigh, countNearPoint[0], countNearPoint[1], length_data);
      save_hrzCali_record(content);
      Edit_StandHigh = Edit_StandHigh - 2;
      //if(Edit_StandHigh==0)Edit_StandHigh=-4;else Edit_StandHigh=0;			
      //UpdateData(FALSE);
      content.Format("窗口扫描：继续扫描 Edit_StandHigh变为%d\n", Edit_StandHigh);
      save_hrzCali_record(content);
      TRACE(content);
      refleshFlag1 = 2; *refleshFlag1pointer = 2;//全屏过程中
    }
    else//停止扫描
    {
      CString content;
      content.Format("窗口扫描：完成 Edit_StandHigh=%d", Edit_StandHigh);
      save_hrzCali_record(content);
      m_strRXData = "窗口扫描：完成";
      walktext_deflection.Format("未出现视界阻挡");
      save_hrzCali_record(walktext_deflection);
      UpdateData(FALSE);
      refleshFlag1 = 1; *refleshFlag1pointer = 1;//窗口扫描完成 或 停止
      scan_all = 0;
    }
  }


  if (length_data != 0) {
    CString strFileName1, str;
    str.Format("数据%d", Edit_StandHigh);
    ca += str;
    strFileName1 = lo + ca + te;
    _mkdir(lo);
    savefile(strFileName1);
    ca.Format("%d", NUM);
    NUM++;
  }


  if (stop_lp == FALSE) {
    // 		time_end = (double)clock();
    // 		time_fin = time_end-time_start;
    OnAutocatch();
  }
  else {
    stop_lp = FALSE;

  }
}

int CLaster::JundgeComfirmType(int Sm, int St, int mid, int length_data, int SmMinForward) {
  int judge_direction = 0;
  int hum_stature = 0;
  int walk_y;
  int mid_true;
  float angle, x, y;
  walk_y = Edit_StandHigh;
  float temp_numerator, temp_denominator, temp_sign;
  int pointSize = 3;

  mid_true = mid;

  int locate_step_o = Edit_Start_x / XSTEPANGLE;
  int locate_step = locate_step_o + mid;
  if (USE_BACK) locate_step -= length_data;
  float theta = XSTEPANGLE * (float)Sm*PI / 180;
  float alpha_x = (float)locate_step*XSTEPANGLE*PI / 180;
  float alpha_y = YSTEPANGLE * (-(float)Edit_StandHigh)*PI / 180;
  x = (float)save_data[mid_true] * sin(alpha_x)*cos(alpha_y);
  y = (float)save_data[mid_true] * cos(alpha_x)*cos(alpha_y);
  float xForward = (float)save_data[SmMinForward] * sin(alpha_x)*cos(alpha_y);
  float yForward = (float)save_data[SmMinForward] * cos(alpha_x)*cos(alpha_y);
  if (SmMinForward != 0) {
    x = (x + xForward) / 2;
    y = (y + yForward) / 2;
  }
  float dis = sqrt(float(x - guide_line_x)*(x - guide_line_x) + float(y - guide_line_y)*(y - guide_line_y));

  //用直线方程计算偏转
  if (guide_line_k != 0) {
    temp_numerator = guide_line_k * x + guide_line_b - y;
    temp_denominator = guide_line_k * guide_line_k + 1;
    temp_sign = x - (y - guide_line_b) / guide_line_k;
    temp_sign = temp_sign / abs(temp_sign);
    judge_direction = temp_sign * abs(temp_numerator) / sqrt(temp_denominator);

  }
  else {
    judge_direction = x - guide_line_x;
  }

  if (int(x) == 0 && int(y) == 3) {
    m_strRXData = "停止点确认";
    walktext_deflection.Format("停止点位置：\n%d, %d\n未找到目标.", guide_line_x, guide_line_y);
  }
  else {
    CString temp1, temp2;
    if (judge_direction < -200)
      temp1.Format("偏左%d", -judge_direction);
    else if (judge_direction > 200)
      temp1.Format("偏右%d", judge_direction);
    else temp1.Format("居中");

    if (int(y) - guide_line_y < -200)
      temp2.Format("，靠前%d", guide_line_y - int(y));
    else if (int(y) - guide_line_y > 200)
      temp2.Format("，靠后%d", int(y) - guide_line_y);


    m_strRXData = "停止点确认";
    walktext_deflection.Format("停止点位置：\n%d, %d\n\n目标位置：\n%d, %d\n", guide_line_x, guide_line_y, int(x), int(y));
    //walktext_deflection.Format("停止点位置：\n%d, %d\n\n目标位置：\n%d, %d\n目标偏离：\n%d, %d\n相距%.6fm",guide_line_x,guide_line_y,int(x),int(y),judge_direction,int(y)-guide_line_y,dis/1000);
    walktext_deflection = walktext_deflection + temp1 + temp2;
  }
  save_hrzCali_record(walktext_deflection);
  UpdateData(FALSE);
  refleshFlag1 = 1; *refleshFlag1pointer = 1;//验证停止点：找到目标



  //hum_stature = (float)install_high-(float)save_data[mid_true]*sin(alpha_y);//(max_distance-save_data[mid_true])*install_high/max_distance;
  //if (hum_stature>=object_high && hum_stature<=object_high+200)
  //{

  //	if((float)save_data[mid_true]*tan(theta)>=0.6*(float)object_width)//测量宽度>=输入宽度*0.6
  //	{
  //		/*judge_direction = mid-walksetup_data[abs(walk_y)][0]-length_data/2;*/
  //		catch_plane.Format("已捕获  %f, %f",x,y);
  //		////////////////////////////////////////////////
  //		//求速度

  //		angle = (float)walk_y*YSTEPANGLE*PI/180;


  //		//walk_y = Edit_StandHigh -stip_h;

  //		////////////////////////////////////////////////////

  //		///////////////////////////////////////////////////
  //		//用直线方程计算偏转
  //		if (guide_line_k != 0 )
  //		{
  //			temp_numerator = guide_line_k * x +guide_line_b - y;
  //			temp_denominator = guide_line_k *guide_line_k + 1;
  //			temp_sign = x - (y-guide_line_b)/guide_line_k;
  //			temp_sign = temp_sign/abs(temp_sign);
  //			judge_direction =  temp_sign * abs(temp_numerator)/sqrt(temp_denominator);

  //		}
  //		else
  //		{
  //			judge_direction = x - guide_line_x;
  //		}
  //		judge_direct = judge_direction;
  //		//Edit_StandHigh = walk_y;  //设置垂直镜片角度，暂时用Edit_Start_y代替
  //		if (y <= guide_line_y )
  //		{
  //			stop_lp = TRUE;
  //		}
  //		///////////////////////////////////////////////////


  //		/*judge_direct = judge_direction;
  //		Edit_StandHigh = walk_y -stip_h;  //设置垂直镜片角度，暂时用Edit_Start_y代替
  //		if (walksetup_data[abs(Edit_StandHigh)][0] == END_LINE)
  //		{
  //		stop_lp = TRUE;
  //		}*/
  //	}

  //}
  //else if (hum_stature>object_high+200)
  //{
  //	Edit_StandHigh = walk_y -stip_h;  //设置垂直镜片角度，暂时用Edit_Start_y代替
  //}
  //judge_direction = judge_direct;
  //if (stop_lp == FALSE)
  //{
  //	if (judge_direction>150)
  //	{
  //		walktext_deflection.Format("偏右 %d",judge_direction);
  //		if (judge_direction>300)
  //			pDrawControlDC->SelectObject(&RedPen); 
  //		else if (judge_direction>250)
  //			pDrawControlDC->SelectObject(&OrangePen); 
  //		else //if (judge_direction>100)
  //			pDrawControlDC->SelectObject(&GreenPen); 
  //	}
  //	else if (judge_direction<-150)
  //	{
  //		walktext_deflection.Format("偏左 %d",judge_direction);
  //		if (judge_direction<-300)
  //			pDrawControlDC->SelectObject(&RedPen); 
  //		else if (judge_direction<-250)
  //			pDrawControlDC->SelectObject(&OrangePen); 
  //		else //if (judge_direction>100)
  //			pDrawControlDC->SelectObject(&GreenPen); 
  //	}
  //	else
  //	{
  //		walktext_deflection.Format("居中 %d",judge_direction);
  //		pDrawControlDC->SelectObject(&BluePen); 
  //	}
  //	//画图，走步测试中人走过的点
  //	int x_pixel=draw_x_real2pixel(x);
  //	int y_pixel=draw_y_real2pixel(y);
  //	pDrawControlDC->Ellipse(draw_x_real2pixel(x)-pointSize,draw_y_real2pixel(y)-pointSize,draw_x_real2pixel(x)+pointSize,draw_y_real2pixel(y)+pointSize);
  //}
  //else
  //{
  //	walktext_deflection.Format("停止");
  //}
  //UpdateData(FALSE);
  return judge_direction;
}

void CLaster::Jundgeheight(int Sm, int St, double mid, int length_data) {
  int hum_stature = 0;
  float x, y, z;
  int walk_y;
  int mid_true;
  walk_y = Edit_StandHigh;
  float angle;
  int locate_step_o = Edit_Start_x / XSTEPANGLE;
  double locate_step = locate_step_o + mid;
  if (USE_BACK) locate_step -= length_data;


  loop++;

  mid_true = mid;

  float theta = XSTEPANGLE * (float)Sm*PI / 180;
  float alpha_x = locate_step * XSTEPANGLE*PI / 180;
  float alpha_y = YSTEPANGLE * (-(float)Edit_StandHigh)*PI / 180;
  z = (float)install_high - (float)save_data[mid_true] * sin(alpha_y);
  x = (float)save_data[mid_true] * sin(alpha_x)*cos(alpha_y);
  y = (float)save_data[mid_true] * cos(alpha_x)*cos(alpha_y);
  /*locate_step=locate_step_o+guide_line_k;
  alpha_x=(float)locate_step*XSTEPANGLE*PI/180;
  guide_line_x=(float)save_data[(int)guide_line_k]*sin(alpha_x)*cos(alpha_y);
  guide_line_y=(float)save_data[(int)guide_line_k]*cos(alpha_x)*cos(alpha_y);*/
  guide_line_x = x;
  guide_line_y = y;
  /*if (guide_line_k == 0)
  {*/
  //guide_line_x = x;
  //guide_line_y = y;
//}


  hum_stature = z;//(max_distance-save_data[mid_true])*install_high/max_distance;
  if (walk_y == start_height) {
    loop = 1;
  }
  else if (loop > 50) {

    stop_lp = TRUE;
  }




  if (hum_stature >= object_high && hum_stature <= object_high + 200) {


    walksetup_data[abs(walksetup_num)][0] = mid - length_data / 2.0;
    if (USE_BACK) walksetup_data[abs(walksetup_num)][0] -= length_data;
    angle = (float)walk_y*YSTEPANGLE*PI / 180;

    walksetup_data[abs(walksetup_num)][1] = (float)save_data[mid_true] * cos(angle);
    walksetup_data[abs(walksetup_num)][2] = x;
    walksetup_data[abs(walksetup_num)][3] = y;
    walksetup_data[abs(walksetup_num)][4] = z;
    walksetup_data[abs(walksetup_num)][6] = walk_y;
    loop = 1;
    m_strTXData.Format("走步设置:%.6f", walksetup_data[abs(walksetup_num)][3]);
    //catch_plane.Format("已捕获  %d, %d, %d",x,y,z);
    refleshFlag1 = 2; *refleshFlag1pointer = 2;//走步设置中，得到数据点
    if (walksetup_num < 2999)
      walksetup_num++;

    countWalkSetCount++;
    if (AUTO_STOP && countWalkSetCount >= 125) {
      OnBnClickedBtnClear();
      refleshFlag1 = 1; *refleshFlag1pointer = 1;//走步设置中，得到数据点并停止
    }



  }
  else if (hum_stature > object_high + 200) {

    Edit_StandHigh = walk_y - stip_h;  //设置垂直镜片角度，暂时用Edit_Start_y代替
    //catch_plane.Format("未捕获");
  }
  else if (hum_stature > object_high - 200 && hum_stature < object_high) {
    Edit_StandHigh = walk_y + stip_h;  //设置垂直镜片角度【新增：抬高垂直镜子】
    //catch_plane.Format("未捕获");
  }
  UpdateData(FALSE);
  //refleshFlag1=2;*refleshFlag1pointer=2;//走步设置中，得到数据点
}

void CLaster::savefile(CString file_name) {

  CFile file;
  file.Open(file_name, CFile::modeCreate | CFile::modeReadWrite);
  CString str, str1, last;
  int i, length_data, No1;
  short int ex, sx;
  line_num = 0;
  No1 = 0;
  page = 0;
  char point;
  last.Format("\r\n");
  ex = Edit_End_x / XSTEPANGLE;
  sx = Edit_Start_x / XSTEPANGLE;
  sx = sx / 2 * 2;
  ex = ex / 2 * 2;
  point = 9;//',';
  data_length = abs(ex - sx) * 2 + data_order + 2;
  length_data = (data_length - data_order) / 2;  //一行数据个数

  /*itoa(Edit_hundred,&hundred,10);
  itoa(Edit_ten,&ten,10);
  itoa(Edit_ge,&ge,10);*/
  //str1.Format("%c%c%,d%d%,%c,%d%d\r\n",save_data[0],save_data[1]);
  for (i = 0; i < data_h; i++) {
    CString strImp;
    int ko;
    ko = i % length_data;
    if (ko == 0) {
      file.SeekToEnd();
      file.Write((LPCTSTR)str, str.GetLength());

      /*if (line_data[line_num] == 1)
      {
      page++;
      strImp.Format("第%d幅\r\n\r\n%d",page,line_data[line_num]);
      }
      else
      {
      strImp.Format("%d",line_data[line_num]);
      }*/

      str = last + strImp + last;
      //line_num++;
      //No1++;
      /*if (ten == '9'&&ge =='5')
      {
      hundred++;
      ten ='0';
      ge= '0';
      }
      else if (ge =='5')
      {
      ten++;
      ge='0';
      }
      else
      {
      ge='5';
      }*/
    }
    strImp.Format("%d", save_data[i]);
    str = str + strImp + point;


  }
  //No1--;
  file.SeekToEnd();
  file.Write((LPCTSTR)str, str.GetLength());
  file.SeekToEnd();
  str.Format("\r\n垂直步数：%d	", Edit_StandHigh);
  file.Write((LPCTSTR)str, str.GetLength());
  str.Format("\r\ndata_length:%d length_data:%d data_order:%d", data_length, length_data, data_order);
  file.Write((LPCTSTR)str, str.GetLength());
  CString output_walktest = "\r\n" + walktext_deflection;
  file.Write((LPCTSTR)output_walktest, output_walktest.GetLength());
  CString output_catch_plane = "\r\n" + catch_plane;
  file.Write((LPCTSTR)output_catch_plane, output_catch_plane.GetLength());

  //file.Write(data,data_length);
  //file.Write(data_temp,data_length);

  file.Close();


}

void CLaster::OnAutocatch() {
  save_hrzCali_record("OnAutocatch函数");
  //UpdateData(TRUE); //读取编辑框内容
  CString change;
  //time_start = (double)clock();
  short int sx, ex, sy, ey, upy = 0;
  data_h = 0;
  data_order = 16;
  line_num = 0;
  positive = 6;
  next_frame = 5;
  frame_length = 7;
  memset(save_data, 0, length_data * 4 * sizeof(int));
  // 	itoa(Edit_hundred,&hundred,10);
  // 	itoa(Edit_ten,&ten,10);
  // 	itoa(Edit_ge,&ge,10);

  /*CString number;
  if (format=="s0")
  {
  data_length=772;
  }
  number = "0000";*/
  // TODO: 在此添加控件通知处理程序代码

  /*m_strTXData=format+number;*/

  save_start_x.Format("%d", Edit_Start_x);
  save_end_x.Format("%d", Edit_End_x);
  save_start_y.Format("%d", Edit_Start_y);
  save_end_y.Format("%d", Edit_End_y);
  sx = Edit_Start_x / XSTEPANGLE;
  ex = Edit_End_x / XSTEPANGLE;
  sy = Edit_Start_y / YSTEPANGLE;
  ey = Edit_End_y / YSTEPANGLE;
  int standhigh = Edit_StandHigh / 2;
  int standheight;
  sx = sx / 2;
  ex = ex / 2;
  sy = sy / 2;
  ey = ey / 2;
  if (presacl != 0) {
    standheight = standhigh;
    data_length = abs(ex - sx) * 2 * presacl + data_order + 2;
  }
  else {
    standheight = Edit_StandHigh;
    data_length = abs(ex - sx) * 2 + data_order + 2;
  }

  // format.Format("44 02 00 08 %c%c ",Edit_format[1],Edit_format[2]);
  format.Format("53 01 00 0a 02 ");

  change.Format("%08x", standheight);
  Stand_High.Format("%c%c %c%c ", change[4], change[5], change[6], change[7]);

  change.Format("%08x", sx);
  start_x.Format("%c%c %c%c ", change[4], change[5], change[6], change[7]);

  change.Format("%08x", ex);
  end_x.Format("%c%c %c%c ", change[4], change[5], change[6], change[7]);

  change.Format("%08x", sy);
  start_y.Format("%c%c %c%c ", change[4], change[5], change[6], change[7]);

  change.Format("%08x", ey);
  end_y.Format("%c%c %c%c ", change[4], change[5], change[6], change[7]);

  m_strTXData = format + Stand_High + start_x + end_x + "00 03 01 ca";
  if (Scan_Cate == 0) {
    format.Format("5A 01 00 0b ");
    upy = Up_Y_Step / 2;
    change.Format("%08x", upy);
    up_y.Format("%c%c %c%c ", change[4], change[5], change[6], change[7]);
    m_strTXData = format + up_y + Stand_High + start_x + end_x + "00 03 03 ca";
    //TRACE("\ncode:%s\n",m_strTXData); 
    int len = String2Hex(m_strTXData, senddata); //此处返回的len可以用于计算发送了多少个十六进制数
    Sum_Check(len, senddata);
    glb_sp_data.deliver2send(senddata, len);
    Sleep(10);
    return;
  }
  CByteArray hexdata;
  //int len=String2Hex(m_strTXData,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
  //m_ctrlComm.put_Output(COleVariant(hexdata)); //发送十六进制数据 
  ////m_ctrlComm.put_Output(COleVariant(m_strTXData));//发送数据

  //m_ctrlComm.put_RThreshold(data_length);  //参数1表示每当串口接收缓冲区中有多于或等于1个字符时将引发一个接收数据的OnComm事件 
  //m_ctrlComm.put_InputLen(0); //设置当前接收区数据长度为0 
  //m_ctrlComm.get_Input();//先预读缓冲区以清除残留数据
  int len = String2Hex(m_strTXData, senddata);
  if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);

  //m_strRXData=m_strTXData;
  //UpdateData(FALSE); //更新编辑框内容
}

int CLaster::FindSm(int* point_num /*扫描点总数*/) {
  int j, angle, Sm_sum, Sm_max, angle_1;
  int data_point, Sm_num;
  Sm_num = j = 0;
  Sm_sum = Sm_max = 0;
  angle = 3 / XSTEPANGLE;
  angle_1 = 1 / XSTEPANGLE;
  while (j < *point_num) {

    data_point = j;
    for (j; j < data_point + angle; j++) {
      if (St_point[j] > 2000) {
        Sm_sum++;
      }
    }
    j = data_point + angle_1;
    Sm_point[Sm_num] = Sm_sum;
    Sm_num++;
    Sm_sum = 0;
  }
  for (j = 0; j < Sm_num; j++) {
    if (Sm_point[j] > Sm_max) {
      Sm_max = Sm_point[j];
      Sm_sum = j;
    }
  }
  *point_num = Sm_sum * angle;
  return Sm_max;
}

int CLaster::ConfirmCentre(int Sm_data  /*平均回波数据*/) {
  int angel, i, locate_first, locate_end;
  int mid;
  locate_first = -1;
  locate_end = 1;
  angel = 3 / XSTEPANGLE;
  i = Sm_data;
  for (i; i <= Sm_data + angel; i++) {
    if (locate_first == -1) {
      if (St_point[i] > 2000) {
        locate_first = i;
      }
    }
    else {
      if (St_point[i] < 1000 && St_point[i - 1]>2000) {
        locate_end = i;
      }
    }

  }
  mid = locate_first + (locate_end - locate_first) / 2;
  return mid;
}

//void CLaster::WalkText()
//{
//	int i,length_data,data_point;
//	int ko,St,Sm,SmTemp;
//	int locate_first,locate_end,max_num;
//	float alpha_x,alpha_y=YSTEPANGLE*(-(float)Edit_StandHigh)*PI/180;
//	int dis_predict=(float)(install_high-object_high)/sin(alpha_y);
//	int SmBreak;
//	int SmStart,SmEnd,SmStartTemp,SmMin=0,SmMinTemp=0,SmMinData=800000;
//	int direction=0;
//
//	max_num = 4;
//	ci =  Sm = 0;
//	St = 0;
//	locate_first =locate_end  =0;
//	char ge;
//	length_data = (data_length-data_order)/2;//一行数据个数
//	ge= '0';
//	for (i=0;i<=data_h;i++)//开始分析正反两行数据
//	{
//		SmTemp=0;SmBreak=-1;
//		SmStart=0;SmEnd=0;SmStartTemp=0;
//		ko=i%length_data;
//		if (ko==0 && i!=0)
//		{
//			if (ge == '0')
//			{
//				data_point = i-length_data;
//				for (data_point;data_point<i;data_point++)
//				{
//
//					if (save_data[data_point]>max_num)
//					{
//						max_num = save_data[data_point];
//
//					}
//
//				}
//				ge = '5';
//				auto_num = max_num;
//			}
//			else
//			{
//				data_point = i-length_data;
//				locate_first = max_num - 3000;
//				locate_end = data_point - length_data;
//				for (data_point;data_point<i;data_point++)//开始分析反向行的数据
//				{
//					if ( save_data[data_point]>2000)//正常点
//					{
//						if(save_data[data_point]>dis_predict-20000 && save_data[data_point]<dis_predict+300)
//						{//感兴趣的点
//							St++;
//							if(SmBreak==1)
//							{
//								SmTemp++;
//								if(save_data[data_point]<save_data[SmMinTemp])
//								{
//									SmMinTemp=data_point;
//								}
//							}							
//							else if(SmBreak==-1)
//							{
//								SmBreak=1;
//								SmTemp++;
//								SmStartTemp=data_point;//标【在这一大行的号】
//								SmMinTemp=data_point;
//							}
//							else//SmBreak==0
//							{
//								SmBreak=1;
//								SmTemp+=2;
//								if(save_data[data_point]<save_data[SmMinTemp])
//								{
//									SmMinTemp=data_point;
//								}
//							}
//						}
//						else//不感兴趣的点
//						{
//							if(SmBreak==1)
//							{
//								SmBreak=0;
//							}
//							else if(SmBreak==0)
//							{
//								SmBreak=-1;
//								//Sm=Sm>(SmTemp-1)?Sm:(SmTemp-1);		
//								if(save_data[SmMinTemp]<SmMinData)//旧版if(Sm<SmTemp)
//								{
//									Sm=SmTemp;
//									SmStart=SmStartTemp;
//									SmEnd=SmStart+Sm-1;
//									SmMin=SmMinTemp;
//									SmMinData =save_data[SmMinTemp];
//								}
//								SmTemp=0;
//							}
//						}
//						//St_point[locate_end] = save_data[data_point];
//						//St++;
//					}
//					if(data_point==i-1)//在最后一个点，更新Sm
//					{						
//						if(SmBreak!=-1&&save_data[SmMinTemp]<SmMinData)//旧版Sm<SmTemp)
//						{
//							Sm=SmTemp;
//							SmStart=SmStartTemp;//【在这一大行的标号】
//							SmEnd=SmStart+Sm-1;//【在这一大行的标号】
//							SmMin=SmMinTemp;
//							SmMinData =save_data[SmMinTemp];
//						}
//					}
//					locate_end++;
//				}//完成分析反向行的数据
//				//Sm = FindSm(&locate_end);
//				//data_point = ConfirmCentre(locate_end);
//			}
//		}
//	}//完成分析正反两行数据
//	//data_point=SmMin;//距离最近点
//	//JundgeCatch0(Sm,St,data_point,length_data);
//
//	bool bCaputred;
//	bCaputred=JundgeCatch(Sm,St,SmStart,SmMin,length_data);
//	SmMin = (SmStart+SmEnd)/2;
//	if (bCaputred)
//	{
//		direction = Jundgemid(Sm,St,SmMin,length_data);
//	}
//	if (length_data!=0)
//	{
//		CString strFileName1,str; 
//		str.Format("数据%d",Edit_StandHigh);
//		ca += str;
//		strFileName1 = lo+ca+te;
//		savefile(strFileName1);
//		ca.Format("%d",NUM);
//		NUM++;
//	}
//
//
//	if (stop_lp == FALSE)
//	{
//		// 		time_end = (double)clock();
//		// 		time_fin = time_end-time_start;
//		OnAutocatch();
//	}
//	else
//	{
//		stop_lp = FALSE;
//	}
//
//}


void CLaster::WalkText() {
  int i, length_data, data_point, SmMinForward = 0;
  int ko, St, Sm, SmTemp;
  int locate_first, locate_end, max_num;
  float z,/*alpha_x,*/alpha_y = YSTEPANGLE * (-(float)Edit_StandHigh)*PI / 180;
  int dis_predict = (float)(install_high - object_high) / sin(alpha_y);
  int SmBreak;
  int SmStart, SmEnd, SmStartTemp, SmMinTemp = 0, SmMinData = 800000;
  double SmMin = 0.0;
  int direction = 0;
  //_mkdir("walktestdata.txt");
  //【精细预测距离】dis_predict
  float dis_temp;
  if (x_walk == 0 && y_walk == 0)//|| top_order[1]<0 ||  top_order[1]>MAXDISTANCE || top_order[0]>MAXDISTANCE )
  {
    dis_temp = (float)(install_high - object_high)*(install_high - object_high) + (float)object_dis*(float)object_dis;
    if (dis_temp > 0)dis_predict = sqrt(dis_temp);
    else dis_predict = object_dis;
  }
  else {
    dis_temp = (float)(install_high - object_high)*(install_high - object_high) + (float)x_walk*(float)x_walk + (float)y_walk*(float)y_walk;
    if (dis_temp > 0)dis_predict = sqrt(dis_temp);
    else dis_predict = y_walk;
  }

  max_num = 4;
  ci = Sm = 0;
  St = 0;
  locate_first = locate_end = 0;
  char ge;
  length_data = (data_length - data_order) / 2;//一行数据个数
  TRACE("走步测试data_length=%d, data_order=%d\n", data_length, data_order);
  SmTemp = 0; SmBreak = -1;
  SmStart = 0; SmEnd = 0; SmStartTemp = 0;
  SmMin = 0; SmMinTemp = 0;
  SmMinData = 800000;
  Sm = 0;
  ge = '0';
  for (i = 0; i <= data_h; i++)//开始分析正反两行数据
  {
    if (USE_BACK) {
      SmTemp = 0; SmBreak = -1;
      SmStart = 0; SmEnd = 0; SmStartTemp = 0;
      SmMin = 0; SmMinTemp = 0;
      SmMinData = 800000;
      Sm = 0;
    }
    ko = i % length_data;
    if (ko == 0 && i != 0) {
      if (ge == '0') {
        data_point = i - length_data;
        locate_first = max_num - 3000;
        locate_end = data_point - length_data;
        for (data_point; data_point < i; data_point++)//开始分析反向行的数据
        {
          if (save_data[data_point] > 2000)//正常点
          {
            z = (float)install_high - (float)save_data[data_point] * sin(alpha_y);
            if (save_data[data_point] > dis_predict - 20000 && save_data[data_point] < dis_predict + 10000 && z>500) {//感兴趣的点
              St++;
              if (SmBreak == 1) {
                SmTemp++;
                if (save_data[data_point] < save_data[SmMinTemp]) {
                  SmMinTemp = data_point;
                }
              }
              else if (SmBreak == -1) {
                SmBreak = 1;
                SmTemp++;
                SmStartTemp = data_point;//标【在这一大行的号】
                SmMinTemp = data_point;
              }
              else//SmBreak==0
              {
                SmBreak = 1;
                SmTemp += 2;
                save_data[data_point - 1] = (save_data[data_point - 2] + save_data[data_point]) / 2;
                if (save_data[data_point] < save_data[SmMinTemp]) {
                  SmMinTemp = data_point;
                }
              }
            }
            else//不感兴趣的点
            {
              if (SmBreak == 1) {
                SmBreak = 0;
              }
              else if (SmBreak == 0) {
                SmBreak = -1;
                //Sm=Sm>(SmTemp-1)?Sm:(SmTemp-1);		
                if (save_data[SmMinTemp] < SmMinData)//旧版if(Sm<SmTemp)
                {
                  Sm = SmTemp;
                  SmStart = SmStartTemp;
                  SmEnd = SmStart + Sm - 1;
                  SmMin = SmMinTemp;
                  SmMinData = save_data[SmMinTemp];
                }
                SmTemp = 0;
              }
            }
            //St_point[locate_end] = save_data[data_point];
            //St++;
          }
          if (data_point == i - 1)//在最后一个点，更新Sm
          {
            if (SmBreak != -1 && save_data[SmMinTemp] < SmMinData)//旧版Sm<SmTemp)
            {
              Sm = SmTemp;
              SmStart = SmStartTemp;//【在这一大行的标号】
              SmEnd = SmStart + Sm - 1;//【在这一大行的标号】
              SmMin = SmMinTemp;
              SmMinData = save_data[SmMinTemp];
            }
          }
          locate_end++;
        }//完成分析反向行的数据
        //Sm = FindSm(&locate_end);
        //data_point = ConfirmCentre(locate_end);
        if (USE_BACK) {
          SmMin = (SmStart + SmEnd) / 2.0;
          if (SmMin > SmStart + Sm - 1)SmMin = SmStart + Sm - 1;
          bool bCaputred = 0;
          bCaputred = JundgeCatch(Sm, St, SmStart, SmMin, 0);
          if (bCaputred) {
            SmMinForward = SmMin + 2;
            if (SmMinForward > SmStart + Sm - 1)SmMinForward = SmStart + Sm - 1;
          }
          else {
            SmMinForward = 0;
          }
          SmMinData = 800000;
        }

        ge = '5';
      }
      else {
        if (USE_BACK) {
          data_point = i - length_data;
          locate_first = max_num - 3000;
          locate_end = data_point - length_data;
          for (data_point; data_point < i; data_point++)//开始分析反向行的数据
          {
            if (save_data[data_point] > 2000)//正常点
            {
              z = (float)install_high - (float)save_data[data_point] * sin(alpha_y);
              if (save_data[data_point] > dis_predict - 20000 && save_data[data_point] < dis_predict + 10000 && z>500) {//感兴趣的点
                St++;
                if (SmBreak == 1) {
                  SmTemp++;
                  if (save_data[data_point] < save_data[SmMinTemp]) {
                    SmMinTemp = data_point;
                  }
                }
                else if (SmBreak == -1) {
                  SmBreak = 1;
                  SmTemp++;
                  SmStartTemp = data_point;//标【在这一大行的号】
                  SmMinTemp = data_point;
                }
                else//SmBreak==0
                {
                  SmBreak = 1;
                  SmTemp += 2;
                  save_data[data_point - 1] = (save_data[data_point - 2] + save_data[data_point]) / 2;
                  if (save_data[data_point] < save_data[SmMinTemp]) {
                    SmMinTemp = data_point;
                  }
                }
              }
              else//不感兴趣的点
              {
                if (SmBreak == 1) {
                  SmBreak = 0;
                }
                else if (SmBreak == 0) {
                  SmBreak = -1;
                  //Sm=Sm>(SmTemp-1)?Sm:(SmTemp-1);		
                  if (save_data[SmMinTemp] < SmMinData)//旧版if(Sm<SmTemp)
                  {
                    Sm = SmTemp;
                    SmStart = SmStartTemp;
                    SmEnd = SmStart + Sm - 1;
                    SmMin = SmMinTemp;
                    SmMinData = save_data[SmMinTemp];
                  }
                  SmTemp = 0;
                }
              }
              //St_point[locate_end] = save_data[data_point];
              //St++;
            }
            if (data_point == i - 1)//在最后一个点，更新Sm
            {
              if (SmBreak != -1 && save_data[SmMinTemp] < SmMinData)//旧版Sm<SmTemp)
              {
                Sm = SmTemp;
                SmStart = SmStartTemp;//【在这一大行的标号】
                SmEnd = SmStart + Sm - 1;//【在这一大行的标号】
                SmMin = SmMinTemp;
                SmMinData = save_data[SmMinTemp];
              }
            }
            locate_end++;
          }//完成分析反向行的数据
          //Sm = FindSm(&locate_end);
          //data_point = ConfirmCentre(locate_end);
        }//if(USE_BACK)

      }
    }
  }//完成分析正反两行数据
  //data_point=SmMin;//距离最近点
  //JundgeCatch0(Sm,St,data_point,length_data);

  bool bCaputred;
  bCaputred = JundgeCatch(Sm, St, SmStart, SmMin, length_data);
  SmMin = (SmStart + SmEnd) / 2.0;
  if (SmMin > SmStart + Sm - 1)SmMin = SmStart + Sm - 1;

  if (bCaputred) {
    direction = Jundgemid(Sm, St, SmMin, length_data, 0);//SmMinForward);
  }
  if (length_data != 0) {
    CString strFileName1, str;
    str.Format("数据%d", Edit_StandHigh);
    ca += str;
    strFileName1 = lo + ca + te;
    _mkdir(lo);
    savefile(strFileName1);
    ca.Format("%d", NUM);
    NUM++;
  }



  if (stop_lp == FALSE) {
    // 		time_end = (double)clock();
    // 		time_fin = time_end-time_start;
    OnAutocatch();
  }
  else {
    if (offline_mode == 1) {
      offline_mode = 0;
    }
    else {
      stop_lp = FALSE;
      /*CString reset_laser="42 05 00 00 ca";
      int len = String2Hex(reset_laser,senddata);
      glb_sp_data.deliver2send(senddata,len);
      if(!stopped) stop_lp = FALSE;*/
    }
  }

}
void CLaster::OnBnClickedButtonWalkTestToAdjust() {
  refleshFlag1 = -1; *refleshFlag1pointer = -1;
  refleshFlag2 = -1; *refleshFlag2pointer = -1;
  refleshFlag3 = 0; *refleshFlag3pointer = 0;
  refleshFlag4 = 0; *refleshFlag4pointer = 0;
  HrzScnProc = FALSE;
  // TODO: 在此添加控件通知处理程序代码
  if (walksetup_num == 0) {
    //MessageBox("未进行走步设置！");
    refleshFlag1 = 1; *refleshFlag1pointer = 1;
    refleshFlag2 = 1; *refleshFlag2pointer = 1;
    refleshFlag3 = 0; *refleshFlag3pointer = 0;
    refleshFlag4 = 0; *refleshFlag4pointer = 0;
    m_strRXData = "错误";
    walktext_deflection.Format("未进行走步设置！");//,LineNum4Confirm[0],LineNum4Confirm[1]);
    return;
  }

  UpdateData(TRUE);
  walksetup_data[abs(walksetup_num)][0] = END_LINE;
  float k, b;
  bool least_square_method = CalculateLineKB(k, b);

  if (least_square_method == TRUE) {
    walksetup_data[abs(walksetup_num)][2] = k * 10000;
    walksetup_data[abs(walksetup_num)][3] = b;
    CString strFileName1, str;
    str.Format("走步设置数据");
    ca.Format("%d", NUM);
    ca += str;
    strFileName1 = lo + ca + te;
    savefile2(strFileName1);
  }
  else {
    walksetup_data[abs(walksetup_num)][2] = 0;
    walksetup_data[abs(walksetup_num)][3] = 0;
    CString strFileName1, str;
    str.Format("走步设置数据");
    ca.Format("%d", NUM);
    ca += str;
    strFileName1 = lo + ca + te;
    savefile2(strFileName1);
  }



  guide_line_k = (float)walksetup_data[walksetup_num][2] / 10000;
  guide_line_b = walksetup_data[walksetup_num][3];
  guide_line_x = walksetup_data[walksetup_num - 1][2];
  guide_line_y = walksetup_data[walksetup_num - 1][3];

  if (guide_line_k != 0)
    guide_line_x = float(guide_line_y - guide_line_b) / guide_line_k;


  //walksetup_num = 0;


  m_strRXData = "引导线计算完成";
  walktext_deflection.Format("停止点：\n%d, %d\nk=%.3f\nb=%.0f\n请先编号\n再保存引导线", guide_line_x, guide_line_y, guide_line_k, guide_line_b);
  UpdateData(FALSE);
  refleshFlag1 = 1; *refleshFlag1pointer = 1;//引导线计算完成
  refleshFlag2 = 1; *refleshFlag2pointer = 1;//引导线计算完成

  /*draw_getready_guideline();
  draw_white();
  draw_guideline();

  //GetDlgItem(IDC_WalkTest)->EnableWindow(TRUE);
  //GetDlgItem(IDC_StopLine)->EnableWindow(TRUE);
  //GetDlgItem(IDC_BUTTON_COMFIRMTYPE)->EnableWindow(TRUE);*/
}

void CLaster::WalkTextToAdjust() {
  int i, length_data, data_point;
  int ko, St, Sm, SmTemp;
  int locate_first, locate_end, max_num;
  float z, alpha_x, alpha_y = YSTEPANGLE * (-(float)Edit_StandHigh)*PI / 180;
  int dis_predict = (float)(install_high - object_high) / sin(alpha_y);
  int SmBreak;
  int SmStart, SmEnd, SmStartTemp, SmMinTemp = 0, SmMinData = 800000;
  double SmMin = 0.0;
  int direction = 0;

  max_num = 4;
  ci = Sm = 0;
  St = 0;
  locate_first = locate_end = 0;
  char ge;
  length_data = (data_length - data_order) / 2;//一行数据个数
  ge = '0';
  SmTemp = 0; SmBreak = -1;
  SmStart = 0; SmEnd = 0; SmStartTemp = 0;
  SmMin = 0; SmMinTemp = 0;
  SmMinData = 800000;
  Sm = 0;
  for (i = 0; i <= data_h; i++)//开始分析正反两行数据
  {
    if (USE_BACK) {
      SmTemp = 0; SmBreak = -1;
      SmStart = 0; SmEnd = 0; SmStartTemp = 0;
      SmMin = 0; SmMinTemp = 0;
      SmMinData = 800000;
      Sm = 0;
    }
    ko = i % length_data;
    if (ko == 0 && i != 0) {
      if (ge == '0') {
        data_point = i - length_data;
        locate_first = max_num - 3000;
        locate_end = data_point - length_data;
        for (data_point; data_point < i; data_point++)//开始分析反向行的数据
        {
          if (save_data[data_point] > 2000)//正常点
          {
            z = (float)install_high - (float)save_data[data_point] * sin(alpha_y);
            if (save_data[data_point] > dis_predict - 20000 && save_data[data_point] < dis_predict + 5000 && z>500) {//感兴趣的点
              St++;
              if (SmBreak == 1) {
                SmTemp++;
                if (save_data[data_point] < save_data[SmMinTemp]) {
                  SmMinTemp = data_point;
                }
              }
              else if (SmBreak == -1) {
                SmBreak = 1;
                SmTemp++;
                SmStartTemp = data_point;//标【在这一大行的号】
                SmMinTemp = data_point;
              }
              else//SmBreak==0
              {
                SmBreak = 1;
                SmTemp += 2;
                save_data[data_point - 1] = (save_data[data_point - 2] + save_data[data_point]) / 2;
                if (save_data[data_point] < save_data[SmMinTemp]) {
                  SmMinTemp = data_point;
                }
              }
            }
            else//不感兴趣的点
            {
              if (SmBreak == 1) {
                SmBreak = 0;
              }
              else if (SmBreak == 0) {
                SmBreak = -1;
                //Sm=Sm>(SmTemp-1)?Sm:(SmTemp-1);		
                if (save_data[SmMinTemp] < SmMinData)//旧版if(Sm<SmTemp)
                {
                  Sm = SmTemp;
                  SmStart = SmStartTemp;
                  SmEnd = SmStart + Sm - 1;
                  SmMin = SmMinTemp;
                  SmMinData = save_data[SmMinTemp];
                }
                SmTemp = 0;
              }
            }
            //St_point[locate_end] = save_data[data_point];
            //St++;
          }
          if (data_point == i - 1)//在最后一个点，更新Sm
          {
            if (SmBreak != -1 && save_data[SmMinTemp] < SmMinData)//旧版Sm<SmTemp)
            {
              Sm = SmTemp;
              SmStart = SmStartTemp;//【在这一大行的标号】
              SmEnd = SmStart + Sm - 1;//【在这一大行的标号】
              SmMin = SmMinTemp;
              SmMinData = save_data[SmMinTemp];
            }
          }
          locate_end++;
        }//完成分析反向行的数据
        //Sm = FindSm(&locate_end);
        //data_point = ConfirmCentre(locate_end);
        if (USE_BACK) {
          SmMin = (SmStart + SmEnd) / 2.0;
          bool bCaputred = 0;
          bCaputred = JundgeCatch(Sm, St, SmStart, SmMin, 0);
          if (bCaputred) {
            guide_line_k = 0;//SmMin;
          }
          else {
            guide_line_k = 0;
          }
        }

        ge = '5';
      }
      else {
        if (USE_BACK) {
          data_point = i - length_data;
          locate_first = max_num - 3000;
          locate_end = data_point - length_data;
          for (data_point; data_point < i; data_point++)//开始分析反向行的数据
          {
            if (save_data[data_point] > 2000)//正常点
            {
              z = (float)install_high - (float)save_data[data_point] * sin(alpha_y);
              if (save_data[data_point] > dis_predict - 20000 && save_data[data_point] < dis_predict + 5000 && z>500) {//感兴趣的点
                St++;
                if (SmBreak == 1) {
                  SmTemp++;
                  if (save_data[data_point] < save_data[SmMinTemp]) {
                    SmMinTemp = data_point;
                  }
                }
                else if (SmBreak == -1) {
                  SmBreak = 1;
                  SmTemp++;
                  SmStartTemp = data_point;//标【在这一大行的号】
                  SmMinTemp = data_point;
                }
                else//SmBreak==0
                {
                  SmBreak = 1;
                  SmTemp += 2;
                  save_data[data_point - 1] = (save_data[data_point - 2] + save_data[data_point]) / 2;
                  if (save_data[data_point] < save_data[SmMinTemp]) {
                    SmMinTemp = data_point;
                  }
                }
              }
              else//不感兴趣的点
              {
                if (SmBreak == 1) {
                  SmBreak = 0;
                }
                else if (SmBreak == 0) {
                  SmBreak = -1;
                  //Sm=Sm>(SmTemp-1)?Sm:(SmTemp-1);		
                  if (save_data[SmMinTemp] < SmMinData)//旧版if(Sm<SmTemp)
                  {
                    Sm = SmTemp;
                    SmStart = SmStartTemp;
                    SmEnd = SmStart + Sm - 1;
                    SmMin = SmMinTemp;
                    SmMinData = save_data[SmMinTemp];
                  }
                  SmTemp = 0;
                }
              }
              //St_point[locate_end] = save_data[data_point];
              //St++;
            }
            if (data_point == i - 1)//在最后一个点，更新Sm
            {
              if (SmBreak != -1 && save_data[SmMinTemp] < SmMinData)//旧版Sm<SmTemp)
              {
                Sm = SmTemp;
                SmStart = SmStartTemp;//【在这一大行的标号】
                SmEnd = SmStart + Sm - 1;//【在这一大行的标号】
                SmMin = SmMinTemp;
                SmMinData = save_data[SmMinTemp];
              }
            }
            locate_end++;
          }//完成分析反向行的数据
          //Sm = FindSm(&locate_end);
          //data_point = ConfirmCentre(locate_end);
        }//if(USE_BACK)

      }
    }
  }//完成分析正反两行数据
  //data_point=SmMin;//距离最近点
  //JundgeCatch0(Sm,St,data_point,length_data);
  SmMin = (SmStart + SmEnd) / 2.0;
  if (SmMin > SmStart + Sm - 1)SmMin = SmStart + Sm - 1;
  bool bCaputred;
  bCaputred = JundgeCatch(Sm, St, SmStart, SmMin, length_data);
  if (bCaputred) {
    Jundgeheight(Sm, St, SmMin, length_data);
  }
  else if (loop != 0 && loop > 50) {
    Jundgeheight(Sm, St, SmMin, length_data);
  }
  else if (loop != 0) {
    loop++;
  }

  if (walksetup_num == 0) {
    //MessageBox("未进行走步设置！");
    refleshFlag1 = 1; *refleshFlag1pointer = 1;
    refleshFlag2 = 1; *refleshFlag2pointer = 1;
    refleshFlag3 = 0; *refleshFlag3pointer = 0;
    refleshFlag4 = 0; *refleshFlag4pointer = 0;
    m_strRXData = "错误";
    walktext_deflection.Format("未进行走步设置！");//,LineNum4Confirm[0],LineNum4Confirm[1]);
    return;
  }

  UpdateData(TRUE);
  walksetup_data[abs(walksetup_num)][0] = END_LINE;
  float k, b;
  bool least_square_method = CalculateLineKB(k, b);

  if (least_square_method == TRUE) {
    walksetup_data[abs(walksetup_num)][2] = k * 10000;
    walksetup_data[abs(walksetup_num)][3] = b;
    CString strFileName1, str;
    str.Format("走步设置数据");
    ca.Format("%d", NUM);
    ca += str;
    strFileName1 = lo + ca + te;
    savefile2(strFileName1);
  }
  else {
    walksetup_data[abs(walksetup_num)][2] = 0;
    walksetup_data[abs(walksetup_num)][3] = 0;
    CString strFileName1, str;
    str.Format("走步设置数据");
    ca.Format("%d", NUM);
    ca += str;
    strFileName1 = lo + ca + te;
    savefile2(strFileName1);
  }



  guide_line_k = (float)walksetup_data[walksetup_num][2] / 10000;
  guide_line_b = walksetup_data[walksetup_num][3];
  guide_line_x = walksetup_data[walksetup_num - 1][2];
  guide_line_y = walksetup_data[walksetup_num - 1][3];

  if (guide_line_k != 0)
    guide_line_x = float(guide_line_y - guide_line_b) / guide_line_k;






  if (stop_lp == FALSE) {
    OnAutocatch();
  }
  else {
    stop_lp = FALSE;
  }
}

void CLaster::ScanYard() {
  int ver_two = 2;
  if (Detect_Thing_Init) {
    laser_machine_parameters lp = Doc.readlaserparamters(".\\test.xml");
    yard_scan_range = lp.yard_scan_range;
    yard_scan_obstacle_high = lp.yard_scan_obstacle_high;
    yard_scan_obstacle_width = lp.yard_scan_obstacle_width;
    Edit_StandHigh_Org = Edit_StandHigh;//Org为全局变量，org为局部变量		
    Edit_Start_x_Org = Edit_Start_x;
    Edit_End_x_Org = Edit_End_x;
    if (yard_scan_range == 2) {
      Edit_Start_x = Edit_Start_x * 2;
      Edit_End_x = Edit_End_x * 2;
    }
    else if (yard_scan_range == 3) {
      Edit_Start_x = Edit_Start_x * 3;
      Edit_End_x = Edit_End_x * 3;
    }
    else if (yard_scan_range == 4) {
      Edit_Start_x = Edit_Start_x * 4;
      Edit_End_x = Edit_End_x * 4;
    }
    else if (yard_scan_range == 5) {
      Edit_Start_x = Edit_Start_x * 5;
      Edit_End_x = Edit_End_x * 5;
    }
    int DetTobject_dis_max = 120000;//最远扫描距离
    float alpha_y_temp = ((float)install_high) / (float)DetTobject_dis_max;
    float angle = atan(alpha_y_temp);
    Edit_StandHigh = -angle * 180 / (0.1125 / ver_two) / PI;//Edit_StandHigh的起始值

    Edit_StandHigh = 0;

    int Edit_StandHigh_temp = Edit_StandHigh / 2;
    Edit_StandHigh = Edit_StandHigh_temp * 2;
    int guide_line_y_temp = guide_line_y;
    guide_line_y_temp = guide_line_y_temp > 20000 ? guide_line_y_temp : 20000;
    alpha_y_temp = ((float)install_high) / (float)guide_line_y_temp;
    angle = atan(alpha_y_temp);
    DtcTEdit_StandHigh_min = -angle * 180 / (0.1125 / ver_two) / PI;//Edit_StandHigh的最小值
    int DtcTEdit_StandHigh_min_temp = DtcTEdit_StandHigh_min / 2;
    DtcTEdit_StandHigh_min = DtcTEdit_StandHigh_min_temp * 2;
    length_data = (data_length - data_order) / 2;//一行数据个数
    //int step=(Edit_StandHigh_Org-Edit_StandHigh)/2;
    //stepMoveNoSet0(step, DOWN, VERTICAL);
    //Sleep(500);
    Scan_Cate = 0;
    Up_Y_Step = -2;
    OnAutocatch();
    DetectThing_SaveModle = 1;
    Detect_Thing_Init = 0;
    NUM_NEW = 1;
    return;
  }
  float alpha_y[2], dis_predict[2];//alpha_y角度	
  alpha_y[0] = (0.1125 / ver_two)*(-(float)Edit_StandHigh)*PI / 180;
  alpha_y[1] = (0.1125 / ver_two)*(-(float)(Edit_StandHigh + Up_Y_Step))*PI / 180;
  for (int i = 0; i < 2; i++) {
    dis_predict[i] = (float)(install_high) / sin(alpha_y[i]);//预测距离
  }
  int x = 0, y = 0, z = 0;//y目标距离,z目标高度，
  vector<int>X;
  vector<int>Y;
  vector<int>Z;
  vector<int>Y_Save;
  vector<int>Z_Save;
  vector<int>X_Save_New;
  vector<int>Y_Save_New;
  vector<int>Z_Save_New;
  for (int i = 0; i < 2; i++) {
    int j_temp = i == 0 ? 0 : length_data;
    int j_end = i == 0 ? length_data : length_data * 2;
    int j_middle = i == 0 ? length_data / 2 : (length_data + length_data / 2);
    for (int j = j_temp; j < j_end; j++) {
      z = int((float)install_high - (float)save_data[j] * sin(alpha_y[i]));
      /*if (z<0||save_data[j]<1000)z=0;//原始只检测大于0的点
      y=int((float)save_data[j]*cos(alpha_y[i]));
      Y.push_back(y);
      Z.push_back(z);
      Z_Save.push_back(0);*/
      y = int((float)save_data[j] * cos(alpha_y[i]));
      x = int((float)save_data[j] * sin(0.1125*(j - j_middle)*PI / 180));
      if (save_data[j] < 1000)x = y = z = 0;
      Y.push_back(y);
      if (z >= 0)//新添检测大于0和小于0的点
      {
        Z.push_back(z);
        Z_Save.push_back(0);
      }
      else {
        Z.push_back(0);
        Z_Save.push_back(z);
      }
      X_Save_New.push_back(x);
      Y_Save_New.push_back(y);
      Z_Save_New.push_back(z);
    }
  }
  int obstacle_point_times = 0;//记录障碍物出现的点数
  if (!Y_All.empty()) {
    for (vector< vector<int> >::size_type u = 0; u < Y_All.size(); u++) {
      for (vector<int>::size_type v = 0; v < Y_All[u].size(); v++) {
        int i = v >= length_data ? 0 : 1;
        if (Y_All[u][v] >= dis_predict[i]) {
          if (Z_Save[v] < Z_All[u][v] && Z_All[u][v]>0)//新添 && Z_All[u][v]>0
          {
            Z_Save[v] = Z_All[u][v];
            if (Z_Save[v] > yard_scan_obstacle_high)obstacle_point_times++;
          }
          Y_All[u][v] = 0;
          Z_All[u][v] = 0;
        }
      }
    }
  }
  Y_All.push_back(Y);
  Z_All.push_back(Z);
  if (obstacle_point_times > yard_scan_obstacle_width) {
    int dis_predict_temp = dis_predict[0] / 1000;
    if (obstacle_dis.empty())obstacle_dis.push_back(dis_predict_temp);
    else {
      for (vector<int>::size_type i = 0; i < obstacle_dis.size(); i++) {
        if (obstacle_dis[i] != dis_predict_temp) {
          obstacle_dis.push_back(dis_predict_temp);
          break;
        }
      }
    }
  }
  //for (int i=0;i<2;i++)//存储扫描点
  //{
  //	int j_strat=i==0?0:length_data;
  //	int j_end=i==0?length_data:length_data*2;

  //	CString ycstr,messagestr;
  //	ycstr.Format("");
  //	for(int j = j_strat; j < j_end; j++)
  //	{
  //		messagestr.Format("%d    ",Z_Save[j]);
  //		ycstr+=messagestr;
  //	}
  //	messagestr=ycstr;
  //	CString filename,num;
  //	num.Format("%d",NUM_NEW);
  //	int dis_predict_temp=dis_predict[i];
  //	filename.Format("全扫描记录-距离%d.txt",dis_predict_temp);
  //	filename=num+filename;
  //	//save_laserSuspect_file(content,filename,1);
  //	save_laserSuspect_file(messagestr,filename,0);
  //	NUM_NEW++;
  //}
  for (int i = 0; i < 2; i++)//新存储扫描点
  {
    int j_strat = i == 0 ? 0 : length_data;
    int j_end = i == 0 ? length_data : length_data * 2;

    CString ycstr, messagestr;
    ycstr.Format("");
    for (int j = j_strat; j < j_end; j++) {
      messagestr.Format("(%d,%d,%d)	", X_Save_New[j], Y_Save_New[j], Z_Save_New[j]);
      ycstr += messagestr;
    }
    messagestr = ycstr;
    CString filename, num;
    num.Format("%d", NUM_NEW);
    int dis_predict_temp = dis_predict[i];
    filename.Format("全扫描记录.txt");
    filename = num + filename;
    //save_laserSuspect_file(content,filename,1);
    save_laserSuspect_file(messagestr, filename, 0);
    NUM_NEW++;
  }
  if (Edit_StandHigh < DtcTEdit_StandHigh_min)//站坪扫描结束
  {
    //int step=(Edit_StandHigh_Org-Edit_StandHigh)/2;
    //stepMoveNoSet0(step, UP, VERTICAL);
    //Sleep(500);
    //detectThingMode=0;
    Edit_StandHigh = Edit_StandHigh_Org;//Org为全局变量，org为局部变量

    Edit_Start_x = Edit_Start_x_Org;
    Edit_End_x = Edit_End_x_Org;
    for (vector<int>::iterator it = obstacle_dis.begin(); it != obstacle_dis.end(); )  //new
    {
      if (*it > 120) {
        it = obstacle_dis.erase(it);
      }
      else {
        ++it;
      }
    }
    if (obstacle_dis.empty()) {
      //glb_VIDGS_system.detect_thing(0);
      CString messagestr;
      messagestr = "站坪扫描完成，没有发现障碍物";
      CString filename;
      filename.Format("0全扫描记录-结果.txt");
      save_laserSuspect_file(messagestr, filename, 0);
      walktext_deflection.Format("站坪扫描完成，没有发现障碍物");//在config上显示信息
      save_hrzCali_record(walktext_deflection);
    }
    else {
      //glb_VIDGS_system.detect_thing(1);
      int messageTimes = 0;
      CString ycstr, messagestr, ycstr_screen, messagestr_screen;
      ycstr.Format("");
      ycstr_screen.Format("");
      for (vector<int>::size_type i = 0; i < obstacle_dis.size(); i++) {
        ++messageTimes;
        messagestr.Format("%dm\n", obstacle_dis[i]);
        ycstr += messagestr;
        if (messageTimes <= 20) {
          messagestr_screen.Format("%dm   ", obstacle_dis[i]);
          ycstr_screen += messagestr_screen;
        }

      }
      messagestr = "距离\n" + ycstr + "发现障碍物";
      messagestr_screen = "距离\n" + ycstr_screen + "发现障碍物";
      CString filename;
      filename.Format("0全扫描记录-结果.txt");
      save_laserSuspect_file(messagestr, filename, 0);
      walktext_deflection.Format(messagestr_screen);//在config上显示信息
      save_hrzCali_record(walktext_deflection);

    }
    DetectThing_SaveModle = 0;
    Scan_Cate = 3;
    Up_Y_Step = 0;
    walktext = FALSE;
    scanyard = FALSE;
    scanyardToAdjust = FALSE;
    walksetup = FALSE;
    avg_order = FALSE;
    auto_catch = FALSE;
    Calib_fuction = FALSE;
    ForHrzCalib = FALSE;
    distPrcs = FALSE;
    HrzScnProc = FALSE;
    OrdDistance = FALSE;
    ConfirmFunct = FALSE;
    m_strRXData = "站坪扫描：完成";
    UpdateData(FALSE);
    refleshFlag1 = 1; *refleshFlag1pointer = 1;//站坪扫描：完成
    /*CString reset_laser="42 05 00 00 ca";
    int len = String2Hex(reset_laser,senddata);
    glb_sp_data.deliver2send(senddata,len);
    if(!stopped) stop_lp = FALSE;*/
  }
  else {
    Edit_StandHigh -= 4;
    stepMoveNoSet0(2, DOWN, VERTICAL);
    Sleep(200);
    Scan_Cate = 0;
    refleshFlag1 = 2; *refleshFlag1pointer = 2;//站坪扫描：更新
  }

  if (stop_lp == FALSE) {
    // 		time_end = (double)clock();
    // 		time_fin = time_end-time_start;
    OnAutocatch();
  }
  else {
    stop_lp = FALSE;

  }
}

void CLaster::ScanYardToAdjust() {
  int ver_two = 2;

  if (Detect_Thing_Init) {
    laser_machine_parameters lp = Doc.readlaserparamters(".\\test.xml");
    yard_scan_range = lp.yard_scan_range;
    yard_scan_obstacle_high = lp.yard_scan_obstacle_high;
    yard_scan_obstacle_width = lp.yard_scan_obstacle_width;
    Edit_StandHigh_Org = Edit_StandHigh;//Org为全局变量，org为局部变量		
    Edit_Start_x_Org = Edit_Start_x;
    Edit_End_x_Org = Edit_End_x;

    laser_parameters lp_guide = Doc.readlaserdatas(".\\test.xml", "guide1", "stop1");
    guide_line_k = lp_guide.guide_line_k;
    guide_line_b = lp_guide.guide_line_b;
    guide_line_x = lp_guide.guide_line_x;
    guide_line_y = lp_guide.guide_line_y;
    object_dis = lp_guide.object_dis;


    int guide_line_y_temp = guide_line_y;
    guide_line_y_temp = 20000;

    if (yard_scan_range == 2) {
      Edit_Start_x = Edit_Start_x * 2;
      Edit_End_x = Edit_End_x * 2;
    }
    else if (yard_scan_range == 3) {
      Edit_Start_x = Edit_Start_x * 3;
      Edit_End_x = Edit_End_x * 3;
    }
    else if (yard_scan_range == 4) {
      Edit_Start_x = Edit_Start_x * 4;
      Edit_End_x = Edit_End_x * 4;
    }
    else if (yard_scan_range == 5) {
      Edit_Start_x = Edit_Start_x * 5;
      Edit_End_x = Edit_End_x * 5;
    }
    int DetTobject_dis_max = 120000;//最远扫描距离
    float alpha_y_temp = ((float)install_high) / (float)DetTobject_dis_max;
    float angle = atan(alpha_y_temp);
    Edit_StandHigh = -angle * 180 / (0.1125 / ver_two) / PI;//Edit_StandHigh的起始值

    Edit_StandHigh = 0;

    int Edit_StandHigh_temp = Edit_StandHigh / 2;
    Edit_StandHigh = Edit_StandHigh_temp * 2;

    alpha_y_temp = ((float)install_high) / (float)guide_line_y_temp;
    angle = atan(alpha_y_temp);
    DtcTEdit_StandHigh_min = -angle * 180 / (0.1125 / ver_two) / PI;//Edit_StandHigh的最小值
    int DtcTEdit_StandHigh_min_temp = DtcTEdit_StandHigh_min / 2;
    DtcTEdit_StandHigh_min = DtcTEdit_StandHigh_min_temp * 2;
    length_data = (data_length - data_order) / 2;//一行数据个数
    //int step=(Edit_StandHigh_Org-Edit_StandHigh)/2;
    //stepMoveNoSet0(step, DOWN, VERTICAL);
    //Sleep(500);
    Scan_Cate = 0;
    Up_Y_Step = -2;
    OnAutocatch();
    DetectThing_SaveModle = 1;
    Detect_Thing_Init = 0;
    NUM_NEW = 1;
    return;
  }
  float alpha_y[2], dis_predict[2];//alpha_y角度	
  alpha_y[0] = (0.1125 / ver_two)*(-(float)Edit_StandHigh)*PI / 180;
  alpha_y[1] = (0.1125 / ver_two)*(-(float)(Edit_StandHigh + Up_Y_Step))*PI / 180;
  for (int i = 0; i < 2; i++) {
    dis_predict[i] = (float)(install_high) / sin(alpha_y[i]);//预测距离
  }
  int x = 0, y = 0, z = 0;//y目标距离,z目标高度，
  vector<int>X;
  vector<int>Y;
  vector<int>Z;
  vector<int>Y_Save;
  vector<int>Z_Save;
  vector<int>X_Save_New;
  vector<int>Y_Save_New;
  vector<int>Z_Save_New;
  for (int i = 0; i < 2; i++) {
    int j_temp = i == 0 ? 0 : length_data;
    int j_end = i == 0 ? length_data : length_data * 2;
    int j_middle = i == 0 ? length_data / 2 : (length_data + length_data / 2);
    for (int j = j_temp; j < j_end; j++) {
      z = int((float)install_high - (float)save_data[j] * sin(alpha_y[i]));
      /*if (z<0||save_data[j]<1000)z=0;//原始只检测大于0的点
      y=int((float)save_data[j]*cos(alpha_y[i]));
      Y.push_back(y);
      Z.push_back(z);
      Z_Save.push_back(0);*/
      y = int((float)save_data[j] * cos(alpha_y[i]));
      x = int((float)save_data[j] * sin(0.1125*(j - j_middle)*PI / 180));
      if (save_data[j] < 1000)x = y = z = 0;
      Y.push_back(y);
      if (z >= 0)//新添检测大于0和小于0的点
      {
        Z.push_back(z);
        Z_Save.push_back(0);
      }
      else {
        Z.push_back(0);
        Z_Save.push_back(z);
      }
      X_Save_New.push_back(x);
      Y_Save_New.push_back(y);
      Z_Save_New.push_back(z);
    }
  }
  int obstacle_point_times = 0;//记录障碍物出现的点数
  if (!Y_All.empty()) {
    for (vector< vector<int> >::size_type u = 0; u < Y_All.size(); u++) {
      for (vector<int>::size_type v = 0; v < Y_All[u].size(); v++) {
        int i = v >= length_data ? 0 : 1;
        if (Y_All[u][v] >= dis_predict[i]) {
          if (Z_Save[v] < Z_All[u][v] && Z_All[u][v]>0)//新添 && Z_All[u][v]>0
          {
            Z_Save[v] = Z_All[u][v];
            if (Z_Save[v] > yard_scan_obstacle_high)obstacle_point_times++;
          }
          Y_All[u][v] = 0;
          Z_All[u][v] = 0;
        }
      }
    }
  }
  Y_All.push_back(Y);
  Z_All.push_back(Z);
  if (obstacle_point_times > yard_scan_obstacle_width) {
    int dis_predict_temp = dis_predict[0] / 1000;
    if (obstacle_dis.empty())obstacle_dis.push_back(dis_predict_temp);
    else {
      for (vector<int>::size_type i = 0; i < obstacle_dis.size(); i++) {
        if (obstacle_dis[i] != dis_predict_temp) {
          obstacle_dis.push_back(dis_predict_temp);
          break;
        }
      }
    }
  }
  //for (int i=0;i<2;i++)//存储扫描点
  //{
  //	int j_strat=i==0?0:length_data;
  //	int j_end=i==0?length_data:length_data*2;

  //	CString ycstr,messagestr;
  //	ycstr.Format("");
  //	for(int j = j_strat; j < j_end; j++)
  //	{
  //		messagestr.Format("%d    ",Z_Save[j]);
  //		ycstr+=messagestr;
  //	}
  //	messagestr=ycstr;
  //	CString filename,num;
  //	num.Format("%d",NUM_NEW);
  //	int dis_predict_temp=dis_predict[i];
  //	filename.Format("全扫描记录-距离%d.txt",dis_predict_temp);
  //	filename=num+filename;
  //	//save_laserSuspect_file(content,filename,1);
  //	save_laserSuspect_file(messagestr,filename,0);
  //	NUM_NEW++;
  //}

  int adjustYSum_temp = 0;
  int adjustZSum_temp = 0;
  int adjustTimes_temp = 0;
  int NUM_NEW_temp;
  for (int i = 0; i < 2; i++)//新存储扫描点
  {
    int j_strat = i == 0 ? 0 : length_data;
    int j_end = i == 0 ? length_data : length_data * 2;

    CString ycstr, messagestr;
    ycstr.Format("");

    for (int j = j_strat; j < j_end; j++) {
      messagestr.Format("(%d,%d,%d)	", X_Save_New[j], Y_Save_New[j], Z_Save_New[j]);
      ycstr += messagestr;
      if (Y_Save_New[j] > guide_line_y - 10000 && NUM_NEW_temp == 0) {
        NUM_NEW_temp = NUM_NEW;
      }
      if (Z_Save_New[j] != 0 && Y_Save_New[j] > guide_line_y - 10000 && Y_Save_New[j] < guide_line_y + 10000)//用于分析是否战平扫描的值有异常
      {
        adjustYSum_temp += Y_Save_New[j];//用于站坪扫描后的校准
        adjustZSum_temp += Z_Save_New[j];
        adjustTimes_temp++;

      }

    }
    messagestr = ycstr;
    CString filename, num;
    num.Format("%d", NUM_NEW);
    int dis_predict_temp = dis_predict[i];
    filename.Format("全扫描记录.txt");
    filename = num + filename;
    //save_laserSuspect_file(content,filename,1);
    save_laserSuspect_file(messagestr, filename, 0);
    NUM_NEW++;

    if (adjustTimes_temp > 10)//用于站坪扫描后的校准
    {
      CString messagestr;
      messagestr.Format("%d,%d", adjustZSum_temp / adjustTimes_temp, NUM_NEW - 1);
      CString filename;
      filename.Format("0全扫描记录-结果.txt");
      save_laserSuspect_file(messagestr, filename, 0);
      adjustYSum_temp /= adjustTimes_temp;
      adjustZSum_temp /= adjustTimes_temp;
      float temp = (float)adjustZSum_temp / adjustYSum_temp;
      adjustAlpha_ySum += atan(temp) * 180 / PI;
      z_temp += adjustZSum_temp;
      adjustTimes++;
      /*CString messagestr;
      messagestr.Format("%d,%d",adjustZSum_temp,adjustTimes);
      CString filename;
      filename.Format("0全扫描记录-结果.txt");
      save_laserSuspect_file(messagestr,filename,0);*/
    }


  }

  if (Edit_StandHigh < DtcTEdit_StandHigh_min)//站坪扫描结束
  {
    //int step=(Edit_StandHigh_Org-Edit_StandHigh)/2;
    //stepMoveNoSet0(step, UP, VERTICAL);
    //Sleep(500);
    //detectThingMode=0;
    Edit_StandHigh = Edit_StandHigh_Org;//Org为全局变量，org为局部变量

    Edit_Start_x = Edit_Start_x_Org;
    Edit_End_x = Edit_End_x_Org;
    for (vector<int>::iterator it = obstacle_dis.begin(); it != obstacle_dis.end(); )  //new
    {
      if (*it > 120) {
        it = obstacle_dis.erase(it);
      }
      else {
        ++it;
      }
    }
    if (obstacle_dis.empty()) {
      //glb_VIDGS_system.detect_thing(0);
      CString messagestr;
      messagestr = "站坪扫描完成，没有发现障碍物";
      CString filename;
      filename.Format("0全扫描记录-结果.txt");
      save_laserSuspect_file(messagestr, filename, 0);
      walktext_deflection.Format("站坪扫描完成，没有发现障碍物");//在config上显示信息
      save_hrzCali_record(walktext_deflection);
    }
    else {
      //glb_VIDGS_system.detect_thing(1);
      int messageTimes = 0;
      CString ycstr, messagestr, ycstr_screen, messagestr_screen;
      ycstr.Format("");
      ycstr_screen.Format("");
      for (vector<int>::size_type i = 0; i < obstacle_dis.size(); i++) {
        ++messageTimes;
        messagestr.Format("%dm\n", obstacle_dis[i]);
        ycstr += messagestr;
        if (messageTimes <= 20) {
          messagestr_screen.Format("%dm   ", obstacle_dis[i]);
          ycstr_screen += messagestr_screen;
        }

      }
      messagestr = "距离\n" + ycstr + "发现障碍物";
      messagestr_screen = "距离\n" + ycstr_screen + "发现障碍物";
      CString filename;
      filename.Format("0全扫描记录-结果.txt");
      save_laserSuspect_file(messagestr, filename, 0);
      walktext_deflection.Format(messagestr_screen);//在config上显示信息
      save_hrzCali_record(walktext_deflection);

    }


    if (adjustTimes > 4 && true)//站坪扫描校准
    {
      adjustAlpha_ySum /= adjustTimes;
      adjustEdit_StandHigh = adjustAlpha_ySum * ver_two / 0.1125;
      adjustEdit_StandHigh /= 2;
      z_temp = z_temp / adjustTimes;
      CString messagestr;
      messagestr.Format("%d", z_temp);
      CString filename;
      filename.Format("0全扫描记录-结果.txt");
      save_laserSuspect_file(messagestr, filename, 0);




      if (adjustEdit_StandHigh >= 1 || adjustEdit_StandHigh <= -1) {
        CString fix;
        fix.Format("42 05 00 00 ca");
        int len = String2Hex(fix, senddata);
        //复位命令
        Sum_Check(len, senddata);
        glb_sp_data.deliver2send(senddata, len);
        Sleep(500);

        if (adjustEdit_StandHigh >= -20 && adjustEdit_StandHigh <= 20) {
          if (adjustEdit_StandHigh > 0) {
            stepMove(-adjustEdit_StandHigh, DOWN, VERTICAL);//回到零点后置为零点
          }
          else
            stepMove(abs(adjustEdit_StandHigh), DOWN, VERTICAL);//回到零点后置为零点
          walktext_deflection.Format("站坪扫描校准完成，移动步数：%d", adjustEdit_StandHigh);//在config上显示信息
          save_hrzCali_record(walktext_deflection);

          Sleep(20);
        }
        else {
          walktext_deflection.Format("站坪扫描校准步数过大,步数：%d", adjustEdit_StandHigh);//在config上显示信息
          save_hrzCali_record(walktext_deflection);

        }

      }
      else {
        walktext_deflection.Format("站坪扫描校准完成。不需要移动垂直步数");//在config上显示信息
        save_hrzCali_record(walktext_deflection);
      }
    }
    else {
      walktext_deflection.Format("站坪扫描校准不符合要求:扫描数据过少");//在config上显示信息
      save_hrzCali_record(walktext_deflection);
    }

    DetectThing_SaveModle = 0;
    Scan_Cate = 3;
    Up_Y_Step = 0;
    walktext = FALSE;
    scanyard = FALSE;
    scanyardToAdjust = FALSE;
    walksetup = FALSE;
    avg_order = FALSE;
    auto_catch = FALSE;
    Calib_fuction = FALSE;
    ForHrzCalib = FALSE;
    distPrcs = FALSE;
    HrzScnProc = FALSE;
    OrdDistance = FALSE;
    ConfirmFunct = FALSE;
    m_strRXData = "站坪扫描：完成";
    UpdateData(FALSE);
    refleshFlag1 = 1; *refleshFlag1pointer = 1;//引导线计算完成
    refleshFlag2 = 1; *refleshFlag2pointer = 1;//引导线计算完成
    /*CString reset_laser="42 05 00 00 ca";
    int len = String2Hex(reset_laser,senddata);
    glb_sp_data.deliver2send(senddata,len);
    if(!stopped) stop_lp = FALSE;*/
  }
  else {
    Edit_StandHigh -= 4;
    stepMoveNoSet0(2, DOWN, VERTICAL);
    Sleep(100);
    Scan_Cate = 0;
    refleshFlag1 = 2; *refleshFlag1pointer = 2;//站坪扫描：更新
  }

  if (stop_lp == FALSE) {
    // 		time_end = (double)clock();
    // 		time_fin = time_end-time_start;
    OnAutocatch();
  }
  else {
    stop_lp = FALSE;

  }
}


int CLaster::save_laserSuspect_file(CString str_content, CString filename/*="疑似捕获记录.txt"*/, bool savetime)//保存激光疑似捕获
{
  //_mkdir("D:\\plane");
  //_mkdir(lo);
  _mkdir(D_Plan_Time);
  filename = D_Plan_Time + filename;
  //*CString */filename = lo+filename;//"疑似捕获记录.txt";

  FILE *fp = fopen(filename, "a");
  if (fp == NULL) return -1;
  if (savetime) {
    char pblgtime[20];
    time_t nowtime = time(&nowtime);
    strftime(pblgtime, 20, "%Y-%m-%d %X", localtime(&nowtime));
    fprintf(fp, "%s ", pblgtime);
  }
  fprintf(fp, str_content);
  fprintf(fp, "\n");

  if (ferror(fp) != 0 || fclose(fp) != 0) return -1;
  else return 0;
}

//int CLaster::Jundgemid(int Sm,int St,int mid,int length_data)
//{
//	int judge_direction;
//	int hum_stature = 0;
//	int walk_y;
//	int mid_true;
//	float angle,x,y;
//	walk_y = Edit_StandHigh;
//	float temp_numerator,temp_denominator,temp_sign;
//	int pointSize=3;
//
//	mid_true = mid;
//
//	int locate_step_o = Edit_Start_x/XSTEPANGLE;
//	int locate_step=locate_step_o+mid-length_data;
//	float theta=XSTEPANGLE*(float)Sm*PI/180;
//	float alpha_x=(float)locate_step*XSTEPANGLE*PI/180;
//	float alpha_y=YSTEPANGLE*(-(float)Edit_StandHigh)*PI/180;
//	x=(float)save_data[mid_true]*sin(alpha_x)*cos(alpha_y);
//	y=(float)save_data[mid_true]*cos(alpha_x)*cos(alpha_y);
//
//
//
//
//
//
//
//	hum_stature = (float)install_high-(float)save_data[mid_true]*sin(alpha_y);//(max_distance-save_data[mid_true])*install_high/max_distance;
//	if (hum_stature>=object_high && hum_stature<=object_high+200)
//	{
//
//		if((float)save_data[mid_true]*tan(theta)>=0.6*(float)object_width)//测量宽度>=输入宽度*0.6
//		{
//			/*judge_direction = mid-walksetup_data[abs(walk_y)][0]-length_data/2;*/
//			catch_plane.Format("已捕获  %f, %f",x,y);
//			////////////////////////////////////////////////
//			//求速度
//
//			angle = (float)walk_y*YSTEPANGLE*PI/180;
//
//
//			//walk_y = Edit_StandHigh -stip_h;
//
//			////////////////////////////////////////////////////
//
//			///////////////////////////////////////////////////
//			//用直线方程计算偏转
//			if (guide_line_k != 0 )
//			{
//				temp_numerator = guide_line_k * x +guide_line_b - y;
//				temp_denominator = guide_line_k *guide_line_k + 1;
//				temp_sign = x - (y-guide_line_b)/guide_line_k;
//				temp_sign = temp_sign/abs(temp_sign);
//				judge_direction =  temp_sign * abs(temp_numerator)/sqrt(temp_denominator);
//
//			}
//			else
//			{
//				judge_direction = x - guide_line_x;
//			}
//			judge_direct = judge_direction;
//			//Edit_StandHigh = walk_y;  //设置垂直镜片角度，暂时用Edit_Start_y代替
//			if (y <= guide_line_y )
//			{
//				stop_lp = TRUE;
//			}
//			///////////////////////////////////////////////////
//
//
//			/*judge_direct = judge_direction;
//			Edit_StandHigh = walk_y -stip_h;  //设置垂直镜片角度，暂时用Edit_Start_y代替
//			if (walksetup_data[abs(Edit_StandHigh)][0] == END_LINE)
//			{
//			stop_lp = TRUE;
//			}*/
//		}
//
//	}
//	else if (hum_stature>object_high+200)
//	{
//		Edit_StandHigh = walk_y -stip_h;  //设置垂直镜片角度，暂时用Edit_Start_y代替
//	}
//	judge_direction = judge_direct;
//	if (stop_lp == FALSE)
//	{
//		if (judge_direction>100)
//		{
//			walktext_deflection.Format("偏右 %d",judge_direction);
//			if (judge_direction>300)
//				pDrawControlDC->SelectObject(&RedPen); 
//			else if (judge_direction>200)
//				pDrawControlDC->SelectObject(&OrangePen); 
//			else //if (judge_direction>100)
//				pDrawControlDC->SelectObject(&YellowPen); 
//		}
//		else if (judge_direction<-100)
//		{
//			walktext_deflection.Format("偏左 %d",judge_direction);
//			if (judge_direction<-300)
//				pDrawControlDC->SelectObject(&RedPen); 
//			else if (judge_direction<-200)
//				pDrawControlDC->SelectObject(&OrangePen); 
//			else //if (judge_direction>100)
//				pDrawControlDC->SelectObject(&YellowPen); 
//		}
//		//画图，走步测试中人走过的点
//		pDrawControlDC->Ellipse(x-pointSize,y-pointSize,x+pointSize,y+pointSize);
//	}
//	else
//	{
//		walktext_deflection.Format("停止");
//	}
//	UpdateData(FALSE);
//	return judge_direction;
//
//
//
//
//}



int CLaster::Jundgemid(int Sm, int St, double mid, int length_data, int SmMinForward) {
  int judge_direction;
  int hum_stature = 0;
  int walk_y_local;
  int mid_true;
  float angle, x, y;
  walk_y_local = Edit_StandHigh;
  float temp_numerator, temp_denominator, temp_sign;
  int pointSize = 3;

  mid_true = (int)(mid + 0.5);

  int locate_step_o = Edit_Start_x / XSTEPANGLE;
  float locate_step = locate_step_o + mid;
  if (USE_BACK) locate_step -= length_data;
  float theta = XSTEPANGLE * (float)Sm*PI / 180;
  float alpha_x = locate_step * XSTEPANGLE*PI / 180;
  float alpha_y = YSTEPANGLE * (-(float)Edit_StandHigh)*PI / 180;
  x = (float)save_data[mid_true] * sin(alpha_x)*cos(alpha_y);
  y = (float)save_data[mid_true] * cos(alpha_x)*cos(alpha_y);
  float xForward = (float)save_data[SmMinForward] * sin(alpha_x)*cos(alpha_y);
  float yForward = (float)save_data[SmMinForward] * cos(alpha_x)*cos(alpha_y);
  if (SmMinForward != 0) {
    x = (x + xForward) / 2;
    y = (y + yForward) / 2;
  }

  hum_stature = (float)install_high - (float)save_data[mid_true] * sin(alpha_y);//(max_distance-save_data[mid_true])*install_high/max_distance;
  if (hum_stature >= object_high && hum_stature <= object_high + 200) {

    if ((float)save_data[mid_true] * tan(theta) >= 0.6*(float)object_width)//测量宽度>=输入宽度*0.6
    {
      /*judge_direction = mid-walksetup_data[abs(walk_y)][0]-length_data/2;*/
      catch_plane.Format("已捕获  %.6f, %.6f, angle=%.6f %.6f, mid=%.6f, width=%.6f", x, y, alpha_x, alpha_y, mid, save_data[mid_true] * tan(theta));
      ////////////////////////////////////////////////
      //求速度

      angle = (float)walk_y_local*YSTEPANGLE*PI / 180;


      /*CFile file;
      file.Open("walktestdata.txt",CFile::modeCreate|CFile::modeReadWrite);
      catch_plane =  "\r\n"  + catch_plane;
      file.Write((LPCTSTR)catch_plane,catch_plane.GetLength());*/


      x_walk = x;//用以传递走步测试坐标
      y_walk = y;
      //walk_y_local = Edit_StandHigh -stip_h;

      ////////////////////////////////////////////////////

      ///////////////////////////////////////////////////
      //用直线方程计算偏转
      if (guide_line_k != 0) {
        temp_numerator = guide_line_k * x + guide_line_b - y;
        temp_denominator = guide_line_k * guide_line_k + 1;
        temp_sign = x - (y - guide_line_b) / guide_line_k;
        temp_sign = temp_sign / abs(temp_sign);
        judge_direction = temp_sign * abs(temp_numerator) / sqrt(temp_denominator);

      }
      else {
        judge_direction = x - guide_line_x;
      }
      judge_direct = judge_direction;
      //Edit_StandHigh = walk_y;  //设置垂直镜片角度，暂时用Edit_Start_y代替
      if (y <= guide_line_y) {
        stop_lp = TRUE;
        walktext = 0;
      }
      ///////////////////////////////////////////////////


      /*judge_direct = judge_direction;
      Edit_StandHigh = walk_y -stip_h;  //设置垂直镜片角度，暂时用Edit_Start_y代替
      if (walksetup_data[abs(Edit_StandHigh)][0] == END_LINE)
      {
      stop_lp = TRUE;
      }*/
    }

  }
  else if (hum_stature > object_high + 200) {
    Edit_StandHigh = walk_y_local - stip_h;  //设置垂直镜片角度，暂时用Edit_Start_y代替

    //【精细垂直步数】Edit_StandHigh
    int temp_high = install_high - object_high;
    float alpha_ftemp = (float)temp_high / (float)y;
    int alpha_fnext = atan(alpha_ftemp) * 180 / YSTEPANGLE / PI;
    int walk_y = -alpha_fnext;
    if (abs(Edit_StandHigh - walk_y) < 7) Edit_StandHigh = walk_y;
  }
  judge_direction = judge_direct;
  if (stop_lp == FALSE) {
    float dist = (float)(y - guide_line_y) / 1000.0;
    if (judge_direction > 150) {
      walktext_deflection.Format("偏右 %d", judge_direction);
      if (dist > 16) {
        if (glb_led_obj_ptr != nullptr) glb_led_obj_ptr->sendmessage(24, dist, "", "");
      }
      else if (dist > -1) {
        if (glb_led_obj_ptr != nullptr) glb_led_obj_ptr->sendmessage(27, dist, "", "");
      }
      if (judge_direction > 300) {
        //pDrawControlDC->SelectObject(&RedPen); 
      }
      else if (judge_direction > 250) {
        //pDrawControlDC->SelectObject(&OrangePen); 
      }
      else //if (judge_direction>100)
      {
        //pDrawControlDC->SelectObject(&GreenPen); 
      }
    }
    else if (judge_direction < -150) {
      walktext_deflection.Format("偏左 %d", judge_direction);
      if (dist > 16) {
        if (glb_led_obj_ptr != nullptr) glb_led_obj_ptr->sendmessage(25, dist, "", "");
      }
      else if (dist > -1) {
        if (glb_led_obj_ptr != nullptr) glb_led_obj_ptr->sendmessage(28, dist, "", "");
      }
      if (judge_direction < -300) {
        //pDrawControlDC->SelectObject(&RedPen); 
      }
      else if (judge_direction < -250) {
        //pDrawControlDC->SelectObject(&OrangePen); 
      }
      else //if (judge_direction>100)
      {
        //pDrawControlDC->SelectObject(&GreenPen); 
      }
    }
    else {
      walktext_deflection.Format("居中 %d", judge_direction);
      if (dist > 16) {
        if (glb_led_obj_ptr != nullptr) glb_led_obj_ptr->sendmessage(23, dist, "", "");
      }
      else if (dist > -1) {
        if (glb_led_obj_ptr != nullptr) glb_led_obj_ptr->sendmessage(26, dist, "", "");
      }
      //pDrawControlDC->SelectObject(&BluePen); 
    }
    //画图，走步测试中人走过的点
    int x_pixel = draw_x_real2pixel(x);
    int y_pixel = draw_y_real2pixel(y);
    //pDrawControlDC->Ellipse(draw_x_real2pixel(x)-pointSize,draw_y_real2pixel(y)-pointSize,draw_x_real2pixel(x)+pointSize,draw_y_real2pixel(y)+pointSize);
    refleshFlag1 = 2; *refleshFlag1pointer = 2;//走步测试中得到数据
    refleshFlag4 = 2; *refleshFlag4pointer = 2;//走步测试中得到数据
  }
  else {
    walktext_deflection.Format("停止");
    if (glb_led_obj_ptr != nullptr) glb_led_obj_ptr->sendmessage(10, 0, "", "");
    refleshFlag1 = 1; *refleshFlag1pointer = 1;//走步测试停止
    refleshFlag4 = 0; *refleshFlag4pointer = 0;//走步测试停止
  }
  UpdateData(FALSE);
  return judge_direction;
}

void CLaster::MedFilter(int length_data, int num)//中值滤波
{
  int *mid_data = new int[length_data];
  int *mid_order = new int[num];
  int mid = num / 2;
  int temp_stip = 0;
  int temp;
  for (int i = length_data; i <= data_h;) {

    for (int j = i - length_data; j < i; j++) {
      mid_data[j - temp_stip * length_data] = save_data[j];
    }
    for (int m = mid; m < length_data - mid; m++) {
      for (int n = 0; n < num; n++) {
        mid_order[n] = mid_data[m - mid + n];
      }
      for (int k = 0; k < num - 1; k++) {
        for (int l = k + 1; l < num; l++) {
          if (mid_order[k] > mid_order[l]) {
            temp = mid_order[l];
            mid_order[l] = mid_order[k];
            mid_order[k] = temp;
          }
        }
      }
      save_data[i - length_data + m] = mid_order[mid];
    }

    i += length_data;
    temp_stip++;
  }
  delete[]mid_data;
  delete[]mid_order;
}

bool CLaster::JundgeCatch(int Sm, int St, int SmStart, double SmMin, int length_data)//判断捕获是否成功
{
  float x, y, z, xSmStart, xSmEnd, ySmStart, ySmEnd;//theta=XSTEPANGLE*(float)Sm*PI/180
  double alpha_y = YSTEPANGLE * (-(float)Edit_StandHigh)*PI / 180;
  bool bCaptured = false;
  int mid_true = (int)(SmMin + 0.5);
  int dataMin = save_data[mid_true];//Sm中最小数据的值
  double alpha_x = (Edit_Start_x / XSTEPANGLE + SmStart - length_data)*XSTEPANGLE*PI / 180;//第一个Sm点的
  xSmStart = (float)save_data[SmStart] * sin(alpha_x)*cos(alpha_y);
  ySmStart = (float)save_data[SmStart] * cos(alpha_x)*cos(alpha_y);
  alpha_x = (Edit_Start_x / XSTEPANGLE + SmStart + Sm - length_data)*XSTEPANGLE*PI / 180;//最后一个Sm点的
  xSmEnd = (float)save_data[SmStart] * sin(alpha_x)*cos(alpha_y);
  ySmEnd = (float)save_data[SmStart] * cos(alpha_x)*cos(alpha_y);
  float width_test = sqrt((xSmEnd - xSmStart)*(xSmEnd - xSmStart) + (ySmEnd - ySmStart)*(ySmEnd - ySmStart));

  alpha_x = (Edit_Start_x / XSTEPANGLE + SmMin - length_data)*XSTEPANGLE*PI / 180;//最小数据点的
  y = (float)save_data[mid_true] * cos(alpha_x)*cos(alpha_y);
  z = (float)install_high - (float)dataMin*sin(alpha_y);
  if (z > object_high + 200 && z < object_high + 1000)//+200
  {
    Edit_StandHigh = Edit_StandHigh - 2;
    UpdateData(FALSE);
    refleshFlag1 = 2; *refleshFlag1pointer = 2;//JundgeCatch()中降低垂直步数
    catch_plane.Format("未捕获");
  }//【高于上限，更改垂直角度】		
  else if (z >= object_high/*-200*/ && z <= object_high + 200)//-200.+200
  {
    bCaptured = true;//备用
    x = ((double)dataMin)*sin(alpha_x)*cos(alpha_y);
    y = (double)dataMin*cos(alpha_x)*cos(alpha_y);
    catch_plane.Format("已捕获  %.6f, %.6f, %.6f,角度%.6f %.6f\n宽度%.6f", x, y, z, alpha_x, alpha_y, width_test);
    if (((mid_true - SmStart) >= 2) && ((SmStart + Sm - mid_true) >= 2))//貌似没起作用
    {
      if ((save_data[mid_true - 2] > save_data[mid_true - 1] && save_data[mid_true - 1] > save_data[mid_true] && save_data[mid_true + 2] > save_data[mid_true + 1] && save_data[mid_true + 1] > save_data[mid_true])) {
        x = (double)dataMin*sin(alpha_x)*cos(alpha_y);
        y = (double)dataMin*cos(alpha_x)*cos(alpha_y);
        catch_plane.Format("！已捕获  %.6f, %.6f, %.6f,角度%.6f %.6f\n宽度%.6f", x, y, z, alpha_x, alpha_y, width_test);
        bCaptured = true;

      }
    }
  }



  if (!bCaptured) { catch_plane.Format("未捕获"); }
  return bCaptured;
}

void CLaster::savefile2(CString file_name) {

  CFile file;
  file.Open(file_name, CFile::modeCreate | CFile::modeReadWrite);
  CString str, str1, last;
  int i, No1;
  short int ex, sx;
  line_num = 0;
  No1 = 0;
  page = 0;
  char point;
  last.Format("\r\n");

  point = 9;//',';

  file.SeekToEnd();
  str.Format("数据个数      引导线	      对象距离	      水平偏转	      垂直距离	      对象高度	      速度	      步进数\r\n");
  file.Write((LPCTSTR)str, str.GetLength());

  for (i = 1; i < 1000; i++) {
    CString strImp;
    file.SeekToEnd();

    strImp.Format("第%d步		%.6f		%.6f		%.6f		%.6f		%.6f		%.6f		%.6f", i,
      walksetup_data[i][0], walksetup_data[i][1], walksetup_data[i][2], walksetup_data[i][3], walksetup_data[i][4], walksetup_data[i][5], walksetup_data[i][6]);
    str = strImp + last;
    file.Write((LPCTSTR)str, str.GetLength());


  }



  file.Close();

}

void CLaster::OnBnClickedBtnClear() {
  HrzScnProc = FALSE;
  //	// TODO: 在此添加控件通知处理程序代码
  stop_lp = TRUE;
  avg_order = 0;
  Calib_fuction = 0;
  walksetup = 0;
  walktext = 0;
  scan_all = 0;
  scanyard = 0;
  scanyardToAdjust = 0;
  UpdateData(FALSE);
  m_strRXData = "停止";
  walktext_deflection = "";
  refleshFlag1 = 1; *refleshFlag1pointer = 1;//停止按钮
  refleshFlag2 = 0; *refleshFlag2pointer = 0;//停止按钮
  refleshFlag3 = 0; *refleshFlag3pointer = 0;//停止按钮
  refleshFlag4 = 0; *refleshFlag4pointer = 0;//停止按钮
  if (glb_led_obj_ptr != nullptr) glb_led_obj_ptr->sendmessage(15, 0, "", "");
  //GetDlgItem(IDC_BTN_Clear)->EnableWindow(FALSE);
}

BOOL CLaster::CalculateLineKB(float &k, float &b) {
  //最小二乘法直线拟合
  //m_FoldList为关键点(x,y)的链表
  //拟合直线方程(Y=kX+b)


  int i = 0;
  float mX, mY, mXX, mXY, mYY;
  mX = mY = mXX = mXY = mYY = 0;
  float ky = 0;
  int n = 0;
  while (abs(walksetup_data[i][0] - END_LINE) > 0.1f&&i < 1000) {

    if (abs(walksetup_data[i][1]) > 0.1f) {
      mX += walksetup_data[i][2];
      mY += walksetup_data[i][3];
      mXX += walksetup_data[i][2] * walksetup_data[i][2];
      mXY += walksetup_data[i][2] * walksetup_data[i][3];
      mYY += walksetup_data[i][3] * walksetup_data[i][3];
      n++;
      i++;

    }
    else {
      i++;
    }

  }

  if (mY*mY - mYY * n == 0)//if(abs(mX*mX-mXX*n)<1000*(mY*mX-mXY*n)*FLT_MIN)
  {
    return FALSE;
  }
  else {
    float y_k, y_b;
    y_k = (mY*mX - mXY * n) / (mY*mY - mYY * n);
    y_b = (mX - mY * y_k) / n;
    k = 1 / y_k;
    b = -y_b / y_k;
    //k=(mY*mX-mXY*n)/(mX*mX-mXX*n);
    //b=(mY-mX*k)/n;
    if (abs(k) > FLT_MAX*0.000001) {
      save_hrzCali_record("出现k过大");
      return FALSE;
    }
    else
      return TRUE;
  }

}

void CLaster::OnBnClickedCancel() {
  // TODO: 在此添加控件通知处理程序代码
  //pDrawWnd->ReleaseDC(pDrawControlDC);
  laster_flag = FALSE;
  CDialog::OnCancel();
}


//void CLaster::OnBnClickedStopline()
//{
//	HrzScnProc=FALSE;
//	// TODO: 在此添加控件通知处理程序代码
//	UpdateData(TRUE);
//	walksetup_data[abs(walksetup_num)][0]=END_LINE;
//	float k,b;
//	bool least_square_method = CalculateLineKB(k,b); 
//
//	if (least_square_method == TRUE)
//	{
//		walksetup_data[abs(walksetup_num)][2] = k*10000;
//		walksetup_data[abs(walksetup_num)][3] = b;
//		CString strFileName1,str; 
//		str.Format("走步设置数据");
//		ca.Format("%d",NUM);
//		ca+=str;
//		strFileName1 = lo+ca+te;
//		savefile2(strFileName1);
//	}
//	else
//	{
//		walksetup_data[abs(walksetup_num)][2] = 0;
//		walksetup_data[abs(walksetup_num)][3] = 0;
//		CString strFileName1,str; 
//		str.Format("走步设置数据");
//		ca.Format("%d",NUM);
//		ca+=str;
//		strFileName1 = lo+ca+te;
//		savefile2(strFileName1);
//	}
//
//
//
//	guide_line_k = (float)walksetup_data[walksetup_num][2]/10000;
//	guide_line_b = walksetup_data[walksetup_num][3];
//	guide_line_x = walksetup_data[walksetup_num-1][2];
//	guide_line_y = walksetup_data[walksetup_num-1][3];
//
//
//
//	/*FileStorage fs(".\\test.xml",FileStorage::APPEND);
//	Mat laster_mat = (Mat_<int>(6,1) <<Edit_StandHigh,object_dis,guide_line_k,guide_line_b,guide_line_x,guide_line_y);
//	fs << "Laster" <<laster_mat;*/
//	linenum.DoModal();
//	/*CString str;
//	str.Format("%s%s%s%s","guide",linenum.str_guidenum,"_stop",linenum.str_stopnum);
//	String Str=LPCTSTR(str);
//
//	Doc.writedata(routename,"install_high",this->install_high);
//	Doc.writedata(routename,"object_dis",this->object_dis);
//	Doc.writedata(routename,"guide_line_k",this->guide_line_k);
//	Doc.writedata(routename,"guide_line_b",this->guide_line_b);
//	Doc.writedata(routename,"guide_line_x",this->guide_line_x);
//	Doc.writedata(routename,"guide_line_y",this->guide_line_y);
//	Doc.writedata(routename,"HrzConfirmSteps",this->HrzConfirmSteps);
//	Doc.writedata(routename,"VertConfirmSteps",this->VertConfirmSteps);*/
//	/*if(linenum.LaserY!="")
//	{
//	guide_line_y=atoi(linenum.LaserY);
//	}*/
//	walksetup_num = 0;
//	UpdateData(FALSE);
//}

void CLaster::OnBnClickedStopline()//编号
{
  refleshFlag1 = -1; *refleshFlag1pointer = -1;
  refleshFlag2 = 0; *refleshFlag2pointer = 0;
  refleshFlag3 = 0; *refleshFlag3pointer = 0;
  refleshFlag4 = 0; *refleshFlag4pointer = 0;
  UpdateData(TRUE);
  HrzScnProc = FALSE;
  // TODO: 在此添加控件通知处理程序代码
  /*walksetup_data[abs(walksetup_num)][0]=END_LINE;
  float k,b;
  bool least_square_method = CalculateLineKB(k,b);

  if (least_square_method == TRUE)
  {
  walksetup_data[abs(walksetup_num)][2] = k*10000;
  walksetup_data[abs(walksetup_num)][3] = b;
  CString strFileName1,str;
  str.Format("走步设置数据");
  ca.Format("%d",NUM);
  ca+=str;
  strFileName1 = lo+ca+te;
  savefile2(strFileName1);
  }
  else
  {
  walksetup_data[abs(walksetup_num)][2] = 0;
  walksetup_data[abs(walksetup_num)][3] = 0;
  CString strFileName1,str;
  str.Format("走步设置数据");
  ca.Format("%d",NUM);
  ca+=str;
  strFileName1 = lo+ca+te;
  savefile2(strFileName1);
  }



  guide_line_k = (float)walksetup_data[walksetup_num][2]/10000;
  guide_line_b = walksetup_data[walksetup_num][3];
  guide_line_x = walksetup_data[walksetup_num-1][2];
  guide_line_y = walksetup_data[walksetup_num-1][3];

  */

  /*FileStorage fs(".\\test.xml",FileStorage::APPEND);
  Mat laster_mat = (Mat_<int>(6,1) <<Edit_StandHigh,object_dis,guide_line_k,guide_line_b,guide_line_x,guide_line_y);
  fs << "Laster" <<laster_mat;*/

  if (guide_line_k != 0)
    guide_line_x = float(guide_line_y - guide_line_b) / guide_line_k;

  //linenum.str_guidenum=
  //linenum.str_stopnum=

  linenum.DoModal();
  /*CString str;
  str.Format("%s%s%s%s","guide",linenum.str_guidenum,"_stop",linenum.str_stopnum);
  String Str=LPCTSTR(str);

  Doc.writedata(routename,"install_high",this->install_high);
  Doc.writedata(routename,"object_dis",this->object_dis);
  Doc.writedata(routename,"guide_line_k",this->guide_line_k);
  Doc.writedata(routename,"guide_line_b",this->guide_line_b);
  Doc.writedata(routename,"guide_line_x",this->guide_line_x);
  Doc.writedata(routename,"guide_line_y",this->guide_line_y);
  Doc.writedata(routename,"HrzConfirmSteps",this->HrzConfirmSteps);
  Doc.writedata(routename,"VertConfirmSteps",this->VertConfirmSteps);*/
  /*if(linenum.LaserY!="")
  {
  guide_line_y=atoi(linenum.LaserY);
  }*/
  walksetup_num = 0;
  UpdateData(FALSE);
  refleshFlag1 = 1; *refleshFlag1pointer = 1;
}


void CLaster::OnBnClickedButtonSetzero() {
  refleshFlag1 = -1; *refleshFlag1pointer = -1;
  refleshFlag2 = 0; *refleshFlag2pointer = 0;
  refleshFlag3 = 0; *refleshFlag3pointer = 0;
  refleshFlag4 = 0; *refleshFlag4pointer = 0;
  Edit_StandHigh = 0;
  UpdateData(FALSE);
  refleshFlag1 = 1; *refleshFlag1pointer = 1;//置为零点
  // TODO: 在此添加控件通知处理程序代码
  avg_order = FALSE;
  NUM = 1;
  CString setzero;
  setzero.Format("42 01 00 09 00 00 07 d0 00 00 07 d0 01 ca");
  CByteArray hexdata;
  //	int len=String2Hex(setzero,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
    //	m_ctrlComm.put_Output(COleVariant(hexdata)); //发送十六进制数据 
  int len = String2Hex(setzero, senddata);
  if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
}


void CLaster::OnBnClickedButtonCalibration() {
  refleshFlag1 = -1; *refleshFlag1pointer = -1;
  refleshFlag2 = 0; *refleshFlag2pointer = 0;
  refleshFlag3 = 0; *refleshFlag3pointer = 0;
  refleshFlag4 = 0; *refleshFlag4pointer = 0;
  HrzScnProc = FALSE;
  // TODO: 在此添加控件通知处理程序代码
  UpdateData(TRUE);
  Edit_StandHigh = 0;
  Calib_fuction = TRUE;
  data_h = 0;
  data_order = 8;
  line_num = 0;
  positive = 6;
  data_length = 1008;
  next_frame = 6;
  auto_catch = 0;
  stan_catch = 0;
  walksetup = 0;
  walktext = 0;
  scanyard = 0;
  scanyardToAdjust = 0;
  CString fix;
  fix.Format("43 01 00 09 4e 20 03 01 02 01 f4 00 01 ca");
  CByteArray hexdata;
  //int len=String2Hex(fix,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
  //m_ctrlComm.put_Output(COleVariant(hexdata)); //发送十六进制数据 
  //m_ctrlComm.put_RThreshold(data_length);  //参数1表示每当串口接收缓冲区中有多于或等于1个字符时将引发一个接收数据的OnComm事件 
  //m_ctrlComm.put_InputLen(0); //设置当前接收区数据长度为0 
  //m_ctrlComm.get_Input();//先预读缓冲区以清除残留数据
  int len = String2Hex(fix, senddata);
  if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
  m_strRXData = "垂直标定：启动";
  UpdateData(FALSE);
  refleshFlag1 = 2; *refleshFlag1pointer = 2;//垂直标定：启动

}

void CLaster::Calibration() {
  int i, j;
  j = 0;
  int ko, length_data, data_point;
  int length_long;
  float second_long;
  int sum_n = 0;
  int sum_a = 0;
  length_data = (data_length - data_order) / 2;//一行数据个数
  if (first_long == 0) {
    temp = 1;
    for (i = 0; i <= data_h; i++) {
      ko = i % length_data;
      if (ko == 0 && i != 0) {
        length_long = length_data;
        data_point = i - length_data;
        for (data_point; data_point < i; data_point++) {
          if (save_data[data_point] > 2000) {
            sum_n += save_data[data_point];
          }
          else {
            length_long--;
          }

        }
        if (length_long == 0) {
          length_long = 1;
        }
        sum_a = sum_a + sum_n / length_long;
        sum_n = 0;
        j++;
      }
    }
    if (j == 0) {
      temp = 0;
      first_long = 0;
    }
    else {
      first_long = sum_a / j;
    }

    OnCalibration(temp);
  }
  else if (temp == 1) {
    temp = 2;
    OnCalibration(temp);
  }
  else {
    for (i = 0; i <= data_h; i++) {
      ko = i % length_data;
      if (ko == 0 && i != 0) {

        data_point = i - length_data;
        for (data_point; data_point < i; data_point++) {
          sum_n += save_data[data_point];
        }
        sum_a = sum_a + sum_n / length_data;
        sum_n = 0;
        j++;
      }
    }
    second_long = sum_a / j;

    // a^2 = b^2+c^2-2bc*cos4.5
    //cos(alpha)=(c^2-bc*cos4.5)/(ac)    temp_long_a = a^2; temp_long_b = c^2-bc*cos4.5; temp_long_c = ac
    float alpha_b = 4.5*PI / 180;
    float temp_long_a = pow(first_long, 2) + pow(second_long, 2) - 2 * first_long*second_long*cos(alpha_b);
    float temp_long_b = pow(second_long, 2) - first_long * second_long*cos(alpha_b);
    float temp_long_c = sqrt(temp_long_a)*second_long;
    float alpha_a = temp_long_b / temp_long_c;
    float alpha_end = acos(alpha_a);
    install_high = sin(alpha_end)*second_long;
    int up_step = alpha_end * 180 / YSTEPANGLE / 2 / PI;
    int standheight;
    if (presacl != 0) {
      standheight = up_step;
    }
    else {
      standheight = up_step;
    }
    CString up, change;
    change.Format("%08x", standheight);
    up.Format("42 01 00 09 00 00 07 d0 %c%c %c%c 07 d0 01 ca", change[4], change[5], change[6], change[7]);
    CByteArray hexdata;
    //int len=String2Hex(up,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
    //m_ctrlComm.put_Output(COleVariant(hexdata)); //发送十六进制数据 
    int len = String2Hex(up, senddata);
    if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
    first_long = 0;
    Calib_fuction = 0;
    m_strRXData = "垂直标定：完成";
    Edit_StandHigh = 0;
    UpdateData(FALSE);
    refleshFlag1 = 1; *refleshFlag1pointer = 1;//垂直标定：完成

  }

  HrzScnProc = FALSE;
}

void CLaster::OnCalibration(int num) {

  if (num == 1) {

    data_length = 5;
    int up_step = 20;
    int standheight;
    if (presacl != 0) {
      standheight = up_step;
    }
    else {
      standheight = up_step;
    }
    CString up, change;
    change.Format("%08x", standheight);
    up.Format("42 01 00 09 00 00 07 d0 %c%c %c%c 07 d0 00 ca", change[4], change[5], change[6], change[7]);
    CByteArray hexdata;
    //int len=String2Hex(up,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
    //m_ctrlComm.put_Output(COleVariant(hexdata)); //发送十六进制数据 
    //m_ctrlComm.put_RThreshold(data_length);  //参数1表示每当串口接收缓冲区中有多于或等于1个字符时将引发一个接收数据的OnComm事件 
    //m_ctrlComm.put_InputLen(0); //设置当前接收区数据长度为0 
    //m_ctrlComm.get_Input();//先预读缓冲区以清除残留数据
    int len = String2Hex(up, senddata);
    if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
  }
  else {
    //UpdateData(TRUE);
    Calib_fuction = 1;
    data_h = 0;
    data_order = 8;
    line_num = 0;
    positive = 6;
    data_length = 1008;
    next_frame = 6;
    auto_catch = 0;
    stan_catch = 0;
    CString fix;
    fix.Format("43 01 00 09 4e 20 03 01 02 01 f4 00 01 ca");
    CByteArray hexdata;
    //int len=String2Hex(fix,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
    //m_ctrlComm.put_Output(COleVariant(hexdata)); //发送十六进制数据 
    //m_ctrlComm.put_RThreshold(data_length);  //参数1表示每当串口接收缓冲区中有多于或等于1个字符时将引发一个接收数据的OnComm事件 
    //m_ctrlComm.put_InputLen(0); //设置当前接收区数据长度为0 
    //m_ctrlComm.get_Input();
    int len = String2Hex(fix, senddata);
    if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
  }
}

void CLaster::OnBnClickedButtonLocate() {
  refleshFlag1 = -1; *refleshFlag1pointer = -1;
  refleshFlag2 = 0; *refleshFlag2pointer = 0;
  refleshFlag3 = 0; *refleshFlag3pointer = 0;
  refleshFlag4 = 0; *refleshFlag4pointer = 0;
  // TODO: 在此添加控件通知处理程序代码
  data_h = 0;
  avg_order = TRUE;
  data_order = 8;
  line_num = 0;
  positive = 6;
  data_length = 1008;//1008;
  next_frame = 6;
  auto_catch = 0;
  stan_catch = 0;
  walksetup = 0;
  walktext = 0;
  scanyard = 0;
  scanyardToAdjust = 0;
  CString fix;
  frame_length = 2;
  //fix.Format("43 01 00 09 03 b6 03 01 02 03 e8 00 01 ca");//950
  fix.Format("43 01 00 09 4e 20 03 01 02 01 f4 00 01 ca");//20000
  //CByteArray hexdata; 
  //int len=String2Hex(fix,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
  //m_ctrlComm.put_Output(COleVariant(hexdata)); //发送十六进制数据 
  //m_ctrlComm.put_RThreshold(data_length);  //参数1表示每当串口接收缓冲区中有多于或等于1个字符时将引发一个接收数据的OnComm事件 
  //m_ctrlComm.put_InputLen(0); //设置当前接收区数据长度为0 
  //m_ctrlComm.get_Input();//先预读缓冲区以清除残留数据
  int len = String2Hex(fix, senddata);
  if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
  Sleep(10);
}

void CLaster::Avg() {
  int i, j;
  j = 0;
  int ko, length_data, data_point;
  int length_long;
  int sum_n = 0;
  int sum_a = 0;
  length_data = (data_length - data_order) / 2;//一行数据个数
  for (i = 0; i <= data_h; i++) {
    ko = i % length_data;
    if (ko == 0 && i != 0) {
      length_long = length_data;
      data_point = i - length_data;
      for (data_point; data_point < i; data_point++) {
        if (save_data[data_point] > 2000) {
          sum_n += save_data[data_point];
        }
        else {
          length_long--;
        }

      }
      if (length_long <= 0) {
        length_long = 1;
      }
      sum_a = sum_a + sum_n / length_long;
      sum_n = 0;
      j++;
    }
  }
  if (j == 0) {
    j = 1;
  }
  mid_num = sum_a / j;
  UpdateData(FALSE);
  refleshFlag1 = 1; *refleshFlag1pointer = 1;//单点测距

  FILE * file;
  SYSTEMTIME st;
  CString strData, strTime;
  GetLocalTime(&st);
  _mkdir("D:\\标定扫描");
  strTime.Format("D:\\标定扫描\\单点%2d-%2d-%2d.txt", st.wHour, st.wMinute, st.wSecond);
  file = fopen(strTime, "a+");
  for (int i = 0; i < data_h; i++) {
    fprintf(file, "%d\t", save_data[i]);

  }
  fclose(file);
}

void CLaster::OnBnClickedButtonLeft() {
  // TODO: 在此添加控件通知处理程序代码
  CString left;
  left.Format("42 01 00 09 ff ff 07 d0 00 00 07 d0 00 ca");
  //CByteArray hexdata; 
  //int len=String2Hex(left,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
  //m_ctrlComm.put_Output(COleVariant(hexdata)); //发送十六进制数据 
  int len = String2Hex(left, senddata);
  if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
}


void CLaster::OnBnClickedButtonRight() {
  // TODO: 在此添加控件通知处理程序代码
  CString right;
  right.Format("42 01 00 09 00 01 07 d0 00 00 07 d0 00 ca");
  //CByteArray hexdata; 
  //int len=String2Hex(right,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
  //m_ctrlComm.put_Output(COleVariant(hexdata)); //发送十六进制数据
  int len = String2Hex(right, senddata);
  if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
}


void CLaster::OnBnClickedButtonUp() {
  refleshFlag1 = -1; *refleshFlag1pointer = -1;
  refleshFlag2 = 0; *refleshFlag2pointer = 0;
  refleshFlag3 = 0; *refleshFlag3pointer = 0;
  refleshFlag4 = 0; *refleshFlag4pointer = 0;
  Edit_StandHigh += 2;
  UpdateData(FALSE);
  refleshFlag1 = 1; *refleshFlag1pointer = 1;//上移1步
  // TODO: 在此添加控件通知处理程序代码
  CString up;
  up.Format("42 01 00 09 00 00 07 d0 00 01 07 d0 00 ca");
  //CByteArray hexdata; 
  //int len=String2Hex(up,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
  //m_ctrlComm.put_Output(COleVariant(hexdata)); //发送十六进制数据 
  int len = String2Hex(up, senddata);
  if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
}


void CLaster::OnBnClickedButtonDown() {
  refleshFlag1 = -1; *refleshFlag1pointer = -1;
  refleshFlag2 = 0; *refleshFlag2pointer = 0;
  refleshFlag3 = 0; *refleshFlag3pointer = 0;
  refleshFlag4 = 0; *refleshFlag4pointer = 0;
  Edit_StandHigh -= 2;
  UpdateData(FALSE);
  refleshFlag1 = 1; *refleshFlag1pointer = 1;//下移1步
  // TODO: 在此添加控件通知处理程序代码
  CString down;
  down.Format("42 01 00 09 00 00 07 d0 ff ff 07 d0 00 ca");
  //CByteArray hexdata; 
  ////int len=String2Hex(down,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
  ////m_ctrlComm.put_Output(COleVariant(hexdata)); //发送十六进制数据 
  //int len = String2Hex(down, senddata);
  //if (senddata[0]==0x43)frame_length=2;else if (senddata[0]==0x53)frame_length=7;glb_sp_data.deliver2send(senddata,len);
  int len = String2Hex(down, senddata);
  if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
}


void CLaster::OnBnClickedButtonDetect() {
  refleshFlag1 = -1; *refleshFlag1pointer = -1;
  refleshFlag2 = 0; *refleshFlag2pointer = 0;
  refleshFlag3 = 0; *refleshFlag3pointer = 0;
  refleshFlag4 = 0; *refleshFlag4pointer = 0;
  Edit_StandHigh = 0;
  // TODO: 在此添加控件通知处理程序代码
  //UpdateData(TRUE);
  m_strRXData = "校准：启动";
  walktext_deflection.Format("水平步数：%d\n垂直步数：%d", HrzConfirmSteps, VertConfirmSteps);
  UpdateData(FALSE);
  refleshFlag1 = 2; *refleshFlag1pointer = 2;//校准：启动

  HrzDetect = FALSE;
  VertDetect = FALSE;
  HrzEdgeConfirm = TRUE;
  VertEdgeConfirm = TRUE;
  inSideWindow = LastInside = 0;

  //HrzConfirmSteps = 65;
  //VertConfirmSteps = 63;

  //HrzConfirmSteps = doc.readalonexdata(".\\test.xml", "HrzConfirmSteps");//需要从配置文件读出
  //VertConfirmSteps = doc.readalonexdata(".\\test.xml", "VertConfirmSteps");//需要从配置文件读出

  actions(NULL, NULL, NULL, NULL, FIXPOINT, JPAVG);
}

void CLaster::OnBnClickedButtonHrzcalibration() {
  refleshFlag1 = -1; *refleshFlag1pointer = -1;
  refleshFlag2 = 0; *refleshFlag2pointer = 0;
  refleshFlag3 = 0; *refleshFlag3pointer = 0;
  refleshFlag4 = 0; *refleshFlag4pointer = 0;
  Edit_StandHigh = 0;
  // TODO: 在此添加控件通知处理程序代码
  distArr[0] = distArr[1] = distArr[2] = 0;		//距离值，distArr[0]--最左边十个距离值和  distArr[1]--中间十个距离值和 disArr[2]最右十个距离值和

  ResetOrNot = FALSE;		//step2 复位 然后偏移cntsteps步到大概的中心位置
  jmpAbtCenter = FALSE;		//是否偏移，TRUE=偏移后 False=便宜前
  HrzScnProc = FALSE;
  Calib_fuction = FALSE;
  walktext = FALSE;
  scanyard = 0;
  scanyardToAdjust = 0;
  walksetup = FALSE;
  avg_order = FALSE;
  stan_catch = FALSE;
  ForHrzCalib = TRUE;

  //hrizmeasure    = FALSE;//新标定方法的标志位
  //runnum            =  1;
  //measurenum    =  1;  

  isBalance = 0;			//验证水平零点是否居中
  NumberMove = 0;			//从左向右比较十次，每次NumberMove自加	
  cntSteps = 0;			//记录中心的大概区域距离边缘有多少步
  microMove = 0;			//微步测距，没1步测量一次
  microMoveNum = 0;			//同NumberMove
  microMin = 200000;			//微步测距最小值，和distancePrecese比较
  microEqual = 0;			//若MicroMin = distancePrecese则microEqual自加
  microCntSteps = 0;			//同CntSteps，记录精确的零点偏移有多少步
  DistMin = 200000;
  distancePrecese = DistMin + 1;
  data_length = 224;
  data_order = 16;
  isBalance = 0;
  NumberMove = 0;
  cntSteps = 0;
  inSideWindow = 0;
  m_strRXData = "水平标定：启动";
  //save_hrzCali_record("水平标定-【按钮被点击并显示启动,2dscan置为0】");
  data_h = 0;
  line_num = 0;
  positive = 6;
  next_frame = 5;
  scan_mode = HORIZONTAL;
  TwoDimenScan = FALSE;
  UpdateData(FALSE);
  refleshFlag1 = 2; *refleshFlag1pointer = 2;//水平标定：启动

  CString sendComm, changeL, changeR;
  unsigned short tempStep = 26;
  unsigned short leftAngle = (unsigned short)(0 - tempStep);
  unsigned short rightAngle = tempStep;

  changeL.Format("%04x", leftAngle);
  changeR.Format("%04x", rightAngle);
  sendComm.Format("44 02 00 08 01 %c%c %c%c %c%c %c%c 00 03 01 ca",
    changeL[0], changeL[1], changeL[2], changeL[3], changeR[0], changeR[1], changeR[2], changeR[3]);
  //CByteArray hexdata;
  //int len = String2Hex(sendComm, hexdata);
  //m_ctrlComm.put_Output(COleVariant(hexdata));
  //m_ctrlComm.put_RThreshold(data_length);  //参数1表示每当串口接收缓冲区中有多于或等于1个字符时将引发一个接收数据的OnComm事件 
  //m_ctrlComm.put_InputLen(0);				 //设置当前接收区数据长度为0 
  //m_ctrlComm.get_Input();
  int len = String2Hex(sendComm, senddata);
  //if (senddata[0]==0x43)frame_length=2;else if (senddata[0]==0x53)frame_length=7;glb_sp_data.deliver2send(senddata,len);
  OnAutocatch();
  Sleep(1000);
}


void CLaster::OnBnClickedButtonEdge() {
  refleshFlag1 = -1; *refleshFlag1pointer = -1;
  refleshFlag2 = 0; *refleshFlag2pointer = 0;
  refleshFlag3 = 0; *refleshFlag3pointer = 0;
  refleshFlag4 = 0; *refleshFlag4pointer = 0;
  Edit_StandHigh = 0;
  // TODO: 在此添加控件通知处理程序代码
  //************	初始化	**************
  inSideWindow = 0;	//落在校准窗边缘内的点数
  LastInside = 0;		//上一次落入校准窗内的点数
  HrzEdgeConfirm = FALSE;	//判断是否确定好水平边界
  VertEdgeConfirm = FALSE;//判断是否确定好垂直边界	
  ConfirmFunct = FALSE;
  HrzDetect = TRUE;
  VertDetect = TRUE;
  m_strRXData = "确定边框：启动";
  walktext_deflection.Format("");
  HrzConfirmSteps = 0;
  VertConfirmSteps = 0;
  UpdateData(FALSE);
  refleshFlag1 = 2; *refleshFlag1pointer = 2;
  actions(NULL, NULL, NULL, NULL, FIXPOINT, JPAVG);
  Sleep(1000);
}


void CLaster::actions(int direction, int stepsSrce, int stepsDst, int SetZero, int type, int jumpFunction) {
  walktext = FALSE;
  scanyard = 0;
  scanyardToAdjust = 0;
  walksetup = FALSE;
  avg_order = FALSE;
  auto_catch = FALSE;
  Calib_fuction = FALSE;
  ForHrzCalib = FALSE;
  distPrcs = FALSE;
  HrzScnProc = FALSE;
  OrdDistance = FALSE;
  ConfirmFunct = FALSE;

  //hrizmeasure = FALSE;//////

  CString fix, temp, tempLeft, tempRight;
  unsigned short tempSteps;
  unsigned stepLeft, stepRight;
  CByteArray hexdata;
  int len = 0;

  switch (jumpFunction) {
    case 1:
      auto_catch = TRUE; break;
    case 2:
      Calib_fuction = TRUE; break;
    case 3:
      ForHrzCalib = TRUE; /*save_hrzCali_record("水平标定-actions中ForHrzCalib变为TRUE");*/break;
    case 4:
      distPrcs = TRUE; /*save_hrzCali_record("水平标定-actions中distPrcs变为TRUE");*/break;
    case 5:
      walksetup = TRUE; break;
    case 6:
      walktext = TRUE; break;
    case 7:
      OrdDistance = TRUE; break;
    case 8:
      HrzScnProc = TRUE; save_hrzCali_record("水平标定-actions中HrzScnProc变为TRUE"); break;
    case 9:
      ConfirmFunct = TRUE; break;
      /*case 10:
      hrizmeasure=TRUE;break;*/
    default:
      Calib_fuction = TRUE; break;
  }

  switch (type) {
    case 1:			//水平扫描
      OnAutocatch();
      return;
      /*data_h = 0;
      data_order =16;
      line_num = 0;
      positive = 6;
      next_frame = 5;
      scan_mode = HORIZONTAL;

      tempSteps = stepsSrce;
      stepLeft = (unsigned short)(0 - tempSteps);
      stepRight = stepsDst ;
      tempLeft.Format("%04x", stepLeft);
      tempRight.Format("%04x", stepRight);

      data_length = data_order + 4 *(stepsSrce + stepsDst);
      fix.Format("44 02 00 08 01 %c%c %c%c %c%C %c%c 00 03 01 ca",
      tempLeft[0], tempLeft[1], tempLeft[2],tempLeft[3], tempRight[0], tempRight[1], tempRight[2],tempRight[3]);
      break;*/

    case 2:		//垂直扫描
      data_h = 0;
      data_order = 16;
      line_num = 0;
      positive = 6;
      next_frame = 5;
      scan_mode = VERTICAL;

      tempSteps = stepsDst;
      stepRight = (unsigned short)(0 - tempSteps);
      stepLeft = stepsSrce;

      tempLeft.Format("%04x", stepLeft);
      tempRight.Format("%04x", stepRight);
      data_order = 16;
      data_length = data_order + 4 * (stepsSrce + stepsDst);
      fix.Format("44 03 00 08 %c%c %c%c %c%c %c%c 00 03 01 ca",
        tempLeft[0], tempLeft[1], tempLeft[2], tempLeft[3], tempRight[0], tempRight[1], tempRight[2], tempRight[3]);
      break;

    case 3:			//普通测距 *950

      data_order = 8;
      data_h = 0;
      line_num = 0;
      positive = 6;
      data_length = 2008;
      next_frame = 6;//org？？？
      data_length = 958;
      //next_frame = 5;
      fix.Format("43 01 00 09 03 b6 03 01 02 03 e8 00 01 ca");//950org
      fix.Format("43 01 00 09 03 b6 03 01 02 03 b6 00 01 ca");//950，每帧950
      fix.Format("43 01 00 09 03 e8 03 01 02 01 f4 00 01 ca");//1000，每帧500
      fix.Format("43 01 00 09 05 dc 03 01 02 01 f4 00 01 ca");//1500，每帧500
      fix.Format("43 01 00 09 03 b6 03 01 02 01 db 00 01 ca");//950，每帧475
      //fix.Format("43 01 00 09 03 e8 03 01 02 03 e8 00 01 ca");//1000
      //fix.Format("43 01 00 09 4e 20 03 01 02 01 f4 00 01 ca");//20000
      break;

    case 4:			//水平电机定位
      stepLeft = (unsigned short)(0 - stepsSrce);
      stepRight = stepsDst;
      data_length = 5;
      tempLeft.Format("%04x", stepLeft);
      tempRight.Format("%04x", stepRight);

      if (SETPOSITION == SetZero) {
        if (LEFT == direction)
          fix.Format("42 01 00 09 %c%c %c%c 07 d0 00 00 07 d0 01 ca",
            tempLeft[0], tempLeft[1], tempLeft[2], tempLeft[3]);
        else
          fix.Format("42 01 00 09 %c%c %c%c 07 d0 00 00 07 d0 01 ca",
            tempRight[0], tempRight[1], tempRight[2], tempRight[3]);
      }
      else {
        if (LEFT == direction)
          fix.Format("42 01 00 09 %c%c %c%c 07 d0 00 00 07 d0 00 ca",
            tempLeft[0], tempLeft[1], tempLeft[2], tempLeft[3]);
        else
          fix.Format("42 01 00 09 %c%c %c%c 07 d0 00 00 07 d0 00 ca",
            tempRight[0], tempRight[1], tempRight[2], tempRight[3]);
      }
      break;

    case 5:			//垂直电机定位
      Edit_StandHigh = 0;
      stepLeft = stepsSrce;
      stepRight = (unsigned short)(0 - stepsDst);
      data_length = 5;
      tempLeft.Format("%04x", stepLeft);
      tempRight.Format("%04x", stepRight);
      if (SETPOSITION == SetZero) {
        if (LEFT == direction)
          fix.Format("42 01 00 09 00 00 07 d0 %c%c %c%c 07 d0 01 ca ",
            tempLeft[0], tempLeft[1], tempLeft[2], tempLeft[3]);
        else
          fix.Format("42 01 00 09 00 00 07 d0 %c%c %c%c 07 d0 01 ca ",
            tempRight[0], tempRight[1], tempRight[2], tempRight[3]);
      }
      else {
        if (LEFT == direction)
          fix.Format("42 01 00 09 00 00 07 d0 %c%c %c%c 07 d0 00 ca ",
            tempLeft[0], tempLeft[1], tempLeft[2], tempLeft[3]);
        else
          fix.Format("42 01 00 09 00 00 07 d0 %c%c %c%c 07 d0 00 ca ",
            tempRight[0], tempRight[1], tempRight[2], tempRight[3]);
      }
      break;

    case 6:		//reset
      data_length = 5;
      fix.Format("42 05 00 00 ca");
      break;

    case 7:			//置位
      data_length = 5;
      fix.Format("42 01 00 09 00 00 07 d0 00 00 07 d0 01 ca");
      break;

    case 8:		//精确测距
      data_order = 8;
      data_h = 0;
      line_num = 0;
      positive = 6;
      data_length = 1008;
      next_frame = 6;
      data_length = 1008;
      fix.Format("43 01 00 09 4e 20 03 01 02 01 f4 00 01 ca");
      break;

    default:
      data_length = 5;
      fix.Format("42 05 00 00 ca");
      break;
  }

  //len = String2Hex(fix, hexdata);
  //m_ctrlComm.put_RThreshold(data_length);  //sly add
  //m_ctrlComm.put_Output(COleVariant(hexdata));
  //m_ctrlComm.put_InputLen(0); //设置当前接收区数据长度为0 
  //m_ctrlComm.get_Input();
  ////m_ctrlComm.put_RThreshold(data_length);  //参数1表示每当串口接收缓冲区中有多于或等于1个字符时将引发一个接收数据的OnComm事件 
  len = String2Hex(fix, senddata);
  if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
}

void CLaster::stepMove(int stepNum, unsigned char direction, unsigned char type) {
  CString fix, sendSteps;
  unsigned short tempSteps;
  int len = 0;

  if (direction == LEFT || direction == DOWN)
    tempSteps = (unsigned short)(0 - stepNum);
  else
    tempSteps = stepNum;

  sendSteps.Format("%04x", tempSteps);

  if (type == HORIZONTAL)
    fix.Format("42 01 00 09 %c%c %c%c 07 d0 00 00 07 d0 01 ca",
      sendSteps[0], sendSteps[1], sendSteps[2], sendSteps[3]);
  else if (type == VERTICAL)
    fix.Format("42 01 00 09 00 00 07 d0 %c%c %c%c 07 d0 01 ca",
      sendSteps[0], sendSteps[1], sendSteps[2], sendSteps[3]);

  len = String2Hex(fix, senddata);
  Sum_Check(len, senddata);
  glb_sp_data.deliver2send(senddata, len);
}

void	CLaster::hrzScanningForCalib() {
  data_length = 120;
  data_order = 16;
  walktext = FALSE;
  scanyard = 0;
  scanyardToAdjust = 0;
  walksetup = FALSE;
  avg_order = FALSE;
  distPrcs = FALSE;
  Calib_fuction = FALSE;
  ForHrzCalib = TRUE;
  //save_hrzCali_record("水平标定-hrzScanningForCalib()将ForHrzCalib变为TRUE");

  CString sendComm, changeL, changeR;
  unsigned tempStep = 26;
  unsigned short leftAngle = (unsigned short)(0 - tempStep);
  unsigned short rightAngle = tempStep;
  changeL.Format("%04x", leftAngle);
  changeR.Format("%04x", rightAngle);
  sendComm.Format("44 02 00 08 01 %c%c %c%c %c%c %c%c 00 03 01 ca",
    changeL[0], changeL[1], changeL[2], changeL[3], changeR[0], changeR[1], changeR[2], changeR[3]);
  //CByteArray hexdata;
  //int len = String2Hex(sendComm, hexdata);
  //m_ctrlComm.put_Output(COleVariant(hexdata));
  //m_ctrlComm.put_RThreshold(data_length);  //参数1表示每当串口接收缓冲区中有多于或等于1个字符时将引发一个接收数据的OnComm事件 
  //m_ctrlComm.put_InputLen(0); //设置当前接收区数据长度为0 
  //m_ctrlComm.get_Input();
  int len = String2Hex(sendComm, senddata);
  if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
}

void	CLaster::AVGprecise() {
  int i, j = 0;
  int ko, length_data, data_point;
  int length_long;
  int sum_n = 0;
  int sum_a = 0;
  length_data = (data_length - data_order) / 2;//一行数据个数

  //distPrcs        = FALSE;//sly add
  //ForHrzCalib = FALSE;//sly add

  for (i = 0; i <= data_h; i++) {
    ko = i % length_data;
    if (ko == 0 && i != 0) {
      length_long = length_data;
      data_point = i - length_data;
      for (data_point; data_point < i; data_point++) {
        if (save_data[data_point] > 1000) {
          sum_n += save_data[data_point];
        }
        else {
          length_long--;
        }

      }
      if (length_long == 0) {
        length_long = 1;
      }
      sum_a = sum_a + sum_n / length_long;
      sum_n = 0;
      j++;
    }

  }
  if (j == 0) {
    distancePrecese = 100000;
  }
  else {
    distancePrecese = sum_a / j;
  }
  m_strRXData.Format("水平标定：%d", distancePrecese);
  save_hrzCali_record(m_strRXData);
  //save_hrzCali_record("水平标定2-显示过程数值");
  UpdateData(FALSE);
  refleshFlag1 = 2; *refleshFlag1pointer = 2;//水平标定过程数据
  if (ResetOrNot == FALSE && isBalance == TRUE) {
    //save_hrzCali_record("水平标定-AVGprecise()-actions(RIGHT, NULL, 7, UNSETPOSITION, HRZMOTORMOVE, JPHRZCALIB);");
    actions(RIGHT, NULL, 7, UNSETPOSITION, HRZMOTORMOVE, JPHRZCALIB);
    Sleep(100);
  }
  if (ResetOrNot == TRUE && microMove == TRUE) {
    //save_hrzCali_record("水平标定-AVGprecise()-actions(RIGHT, NULL, 1, UNSETPOSITION, HRZMOTORMOVE, JPHRZCALIB);");
    save_hrzCali_record("水平标定2-AVGprecise()-actions右移1步");
    actions(RIGHT, NULL, 1, UNSETPOSITION, HRZMOTORMOVE, JPHRZCALIB);
    Sleep(100);
  }
}
void	CLaster::normalMeasure() {
  CString sendComm;
  CByteArray hexdata;
  int len;

  int i, j = 0;
  int length_data, data_point;
  length_data = (data_length - data_order) / 2;//一行数据个数
  length_data = 950;//【强制修改！！！】
  inSideWindow = 0;
  for (i = 0; i < length_data; i++) {
    if (save_data[i] > 70 && save_data[i] < 1000) {
      inSideWindow++;
    }
  }
  //************************************************************
  //**********************	垂直校准		**********************
  if (VertDetect == FALSE && HrzDetect == TRUE) {
    if (inSideWindow < 100) {
      actions(DOWN, NULL, 7, UNSETPOSITION, VERTMOTORMOVE, JPCONFIRMEDGE);
      Sleep(500);
    }
    if (inSideWindow > 900) {
      actions(UP, 3, NULL, UNSETPOSITION, VERTMOTORMOVE, JPCONFIRMEDGE);
      Sleep(500);
    }
    if (inSideWindow < 520 && inSideWindow > 480) {
      VertDetect = TRUE;
      actions(UP, VertConfirmSteps, NULL, SETPOSITION, VERTMOTORMOVE, JPCONFIRMEDGE);
      Sleep(500);

      m_strRXData = "校准：完成";
      Edit_StandHigh = 0;
      walktext_deflection.Format("");
      UpdateData(FALSE);
      refleshFlag1 = 1; *refleshFlag1pointer = 1;//校准：完成
    }
    if (inSideWindow <= 480 && inSideWindow >= 100) {
      if (LastInside >= 520) {
        VertDetect = TRUE;
        if (LastInside + inSideWindow <= 1000)
          actions(UP, VertConfirmSteps - 1, NULL, SETPOSITION, VERTMOTORMOVE, JPCONFIRMEDGE);
        else
          actions(UP, VertConfirmSteps, NULL, SETPOSITION, VERTMOTORMOVE, JPCONFIRMEDGE);
        inSideWindow = LastInside = 0;
        Sleep(500);

        m_strRXData = "校准：完成";
        Edit_StandHigh = 0;
        walktext_deflection.Format("");
        UpdateData(FALSE);
        refleshFlag1 = 1; *refleshFlag1pointer = 1;//校准：完成
      }
      else {
        LastInside = inSideWindow;
        actions(DOWN, NULL, 1, SETPOSITION, VERTMOTORMOVE, JPCONFIRMEDGE);
        Sleep(300);
      }
    }

    if (inSideWindow >= 520 && inSideWindow <= 900) {
      if (LastInside <= 480) {
        if (inSideWindow + LastInside >= 1000)
          actions(UP, VertConfirmSteps + 1, NULL, SETPOSITION, VERTMOTORMOVE, JPCONFIRMEDGE);
        else
          actions(UP, VertConfirmSteps, NULL, SETPOSITION, VERTMOTORMOVE, JPCONFIRMEDGE);
        Sleep(500);
        VertDetect = TRUE;
        inSideWindow = LastInside = 0;

        m_strRXData = "校准：完成";
        Edit_StandHigh = 0;
        walktext_deflection.Format("");
        UpdateData(FALSE);
        refleshFlag1 = 1; *refleshFlag1pointer = 1;//校准：完成
      }
      else {
        LastInside = inSideWindow;
        actions(UP, 1, NULL, UNSETPOSITION, VERTMOTORMOVE, JPCONFIRMEDGE);
        Sleep(300);
      }

    }
  }

  //*********************************************************************
  //*********************		水平校准		********************************
  //**********************************************************************
  if (HrzDetect == FALSE && VertEdgeConfirm == TRUE) {
    if (inSideWindow < 100) {
      actions(RIGHT, NULL, 5, UNSETPOSITION, HRZMOTORMOVE, JPCONFIRMEDGE);
      Sleep(500);
    }
    if (inSideWindow > 900) {
      actions(LEFT, 2, NULL, UNSETPOSITION, HRZMOTORMOVE, JPCONFIRMEDGE);
      Sleep(500);
    }
    if (inSideWindow > 450 && inSideWindow < 550) {
      HrzDetect = TRUE;
      actions(LEFT, HrzConfirmSteps, NULL, SETPOSITION, HRZMOTORMOVE, JPCONFIRMEDGE);
      Sleep(500);
    }
    if (inSideWindow <= 450 && inSideWindow >= 100) {
      if (LastInside >= 550) {
        HrzDetect = TRUE;

        if (LastInside + inSideWindow <= 1000)
          actions(LEFT, HrzConfirmSteps - 1, NULL, SETPOSITION, HRZMOTORMOVE, JPCONFIRMEDGE);
        else
          actions(LEFT, HrzConfirmSteps, NULL, SETPOSITION, HRZMOTORMOVE, JPCONFIRMEDGE);
        inSideWindow = LastInside = 0;
        Sleep(500);

      }
      else {
        LastInside = inSideWindow;
        actions(RIGHT, NULL, 1, SETPOSITION, HRZMOTORMOVE, JPCONFIRMEDGE);
        Sleep(300);
      }
    }

    if (inSideWindow >= 550 && inSideWindow <= 900) {
      if (LastInside <= 450) {
        HrzDetect = TRUE;

        if (inSideWindow + LastInside >= 1000)
          actions(LEFT, HrzConfirmSteps + 1, NULL, SETPOSITION, HRZMOTORMOVE, JPCONFIRMEDGE);
        else
          actions(LEFT, HrzConfirmSteps, NULL, SETPOSITION, HRZMOTORMOVE, JPCONFIRMEDGE);
        inSideWindow = LastInside = 0;
        Sleep(500);

      }
      else {
        LastInside = inSideWindow;
        actions(LEFT, 1, NULL, UNSETPOSITION, HRZMOTORMOVE, JPCONFIRMEDGE);
        Sleep(300);
      }

    }
  }

  //*****************************************************************
  //**********************	垂直确定边界		***********************
  //*****************************************************************
  if (VertEdgeConfirm == FALSE && HrzEdgeConfirm == TRUE) {
    if (inSideWindow < 100) {
      actions(DOWN, NULL, 7, UNSETPOSITION, VERTMOTORMOVE, JPCONFIRMEDGE);
      Sleep(300);
      VertConfirmSteps += 7;
    }
    if (inSideWindow > 900) {
      actions(UP, 5, NULL, UNSETPOSITION, VERTMOTORMOVE, JPCONFIRMEDGE);
      Sleep(300);
      VertConfirmSteps -= 5;
    }

    if (inSideWindow > 480 && inSideWindow < 520) {
      VertEdgeConfirm = TRUE;
      inSideWindow = LastInside = 0;

      //****将VertConfirmSteps写入配置文件

      actions(NULL, NULL, NULL, NULL, RESET, JPCONFIRMEDGE);
      Sleep(500);
      m_strRXData = "确定边框：完成";
      Edit_StandHigh = 0;
      walktext_deflection.Format("水平步数：%d\n垂直步数：%d", HrzConfirmSteps, VertConfirmSteps);
      UpdateData(FALSE);
      refleshFlag1 = 1; *refleshFlag1pointer = 1;//确定边框：完成
    }
    if (inSideWindow <= 480 && inSideWindow >= 100) {
      if (LastInside >= 520) {
        if (LastInside + inSideWindow <= 1000)
          VertConfirmSteps += 1;
        VertEdgeConfirm = TRUE;
        //****将VertConfirmSteps写入配置文件 
        inSideWindow = LastInside = 0;
        actions(NULL, NULL, NULL, NULL, RESET, JPCONFIRMEDGE);
        Sleep(500);
        m_strRXData = "确定边框：完成";
        Edit_StandHigh = 0;
        walktext_deflection.Format("水平步数：%d\n垂直步数：%d", HrzConfirmSteps, VertConfirmSteps);
        UpdateData(FALSE);
        refleshFlag1 = 1; *refleshFlag1pointer = 1;//确定边框：完成
      }
      else {
        LastInside = inSideWindow;
        actions(DOWN, NULL, 1, UNSETPOSITION, VERTMOTORMOVE, JPCONFIRMEDGE);
        Sleep(300);
        VertConfirmSteps += 1;
      }
    }

    if (inSideWindow >= 520 && inSideWindow <= 900) {
      if (LastInside <= 480) {
        if (inSideWindow + LastInside >= 1000)
          VertConfirmSteps -= 1;
        VertEdgeConfirm = TRUE;
        //****将VertConfirmSteps写入配置文件 
        inSideWindow = LastInside = 0;
        actions(NULL, NULL, NULL, NULL, RESET, JPCONFIRMEDGE);
        Sleep(500);

        m_strRXData = "确定边框：完成";
        Edit_StandHigh = 0;
        walktext_deflection.Format("水平步数：%d\n垂直步数：%d", HrzConfirmSteps, VertConfirmSteps);
        UpdateData(FALSE);
        refleshFlag1 = 1; *refleshFlag1pointer = 1;//确定边框：完成
      }
      else {
        LastInside = inSideWindow;
        actions(UP, 1, NULL, UNSETPOSITION, VERTMOTORMOVE, JPCONFIRMEDGE);
        Sleep(300);
        VertEdgeConfirm -= 1;
      }

    }

  }

  //*************************************************************
  //********************		水平确定边界		*******************
  //*************************************************************
  if (HrzEdgeConfirm == FALSE) {
    if (inSideWindow < 100) {
      actions(RIGHT, NULL, 5, UNSETPOSITION, HRZMOTORMOVE, JPCONFIRMEDGE);
      HrzConfirmSteps += 5;
      Sleep(300);
    }
    if (inSideWindow > 900) {
      actions(LEFT, 2, NULL, UNSETPOSITION, HRZMOTORMOVE, JPCONFIRMEDGE);
      HrzConfirmSteps -= 2;
      Sleep(300);
    }
    if (inSideWindow > 450 && inSideWindow < 550) {
      HrzEdgeConfirm = TRUE;
      //****将HrzConfirmSteps写入配置文件 
      inSideWindow = LastInside = 0;
      //m_ctrlComm.put_InputLen(0); //设置当前接收区数据长度为0 
      //m_ctrlComm.get_Input();
      Sleep(20);
      actions(NULL, NULL, NULL, NULL, RESET, JPCONFIRMEDGE);
      Sleep(300);
    }
    if (inSideWindow <= 450 && inSideWindow >= 100) {
      if (LastInside >= 500) {
        if (LastInside + inSideWindow <= 1000)
          HrzConfirmSteps += 1;
        HrzEdgeConfirm = TRUE;
        //****将HrzConfirmSteps写入配置文件 
        inSideWindow = LastInside = 0;
        //m_ctrlComm.put_InputLen(0); //设置当前接收区数据长度为0 
        //m_ctrlComm.get_Input();
        Sleep(20);
        actions(NULL, NULL, NULL, NULL, RESET, JPCONFIRMEDGE);
        Sleep(500);
      }
      else {
        LastInside = inSideWindow;
        actions(RIGHT, NULL, 1, UNSETPOSITION, HRZMOTORMOVE, JPCONFIRMEDGE);
        Sleep(300);
        HrzConfirmSteps += 1;
      }
    }

    if (inSideWindow >= 550 && inSideWindow <= 900) {
      if (LastInside <= 500) {
        if (inSideWindow + LastInside >= 1000)
          HrzConfirmSteps -= 1;
        HrzEdgeConfirm = TRUE;
        //****将HrzConfirmSteps写入配置文件 
        inSideWindow = LastInside = 0;
        //m_ctrlComm.put_InputLen(0); //设置当前接收区数据长度为0 
        //m_ctrlComm.get_Input();
        Sleep(20);
        actions(NULL, NULL, NULL, NULL, RESET, JPCONFIRMEDGE);
        Sleep(500);
      }
      else {
        LastInside = inSideWindow;
        actions(LEFT, 1, NULL, UNSETPOSITION, HRZMOTORMOVE, JPCONFIRMEDGE);
        Sleep(300);
        HrzConfirmSteps -= 1;
      }

    }

  }

}

void	CLaster::HrzProFunct() {
  CString setzero;
  CByteArray hexdata;
  int len;
  unsigned short length_data;
  length_data = (data_length - data_order) >> 1;

  //	ForHrzCalib  =	FALSE;
  //	HrzScnProc  =   FALSE;

  if (TwoDimenScan == TRUE) {
    refleshFlag3 = 1; *refleshFlag3pointer = 1;//画二维点
    //save_hrzCali_record("水平标定-HrzProFunct函数if");
    //画图，画二维点
    /*if(draw_status==1)
    {
      if(!bDrawInit) draw_OnLaserPaintInit();
      else draw_white();
      draw_getready_2Ddata();
      draw_2Ddata();
    }
    else if(draw_status==2)
    {
      draw_getready_xy();
      draw_xyCoor();
      draw_xy2Ddata();
    }*/

    FILE * file;
    SYSTEMTIME st;
    CString strData, strTime;
    GetLocalTime(&st);
    _mkdir("D:\\标定扫描");
    strTime.Format("D:\\标定扫描\\二维%2d-%2d-%2d.txt", st.wHour, st.wMinute, st.wSecond);

    file = fopen(strTime, "a+");
    for (int i = 0; i < length_data; i++) {
      fprintf(file, "%d\t", save_data[i]);

    }
    fclose(file);
    //save_hrzCali_record("水平标定-HrzProFunct函数if，2dscan置为0");
    TwoDimenScan = 0;
  }
  else {
    //save_hrzCali_record("水平标定-HrzProFunct函数else");
    distArr[0] = distArr[1] = distArr[2] = 0;

    for (int j = 0; j < 10; j++) {
      distArr[0] += save_data[j + 5];
      distArr[1] += save_data[(length_data / 2) - 7 + j];
      distArr[2] += save_data[length_data - 15 + j];
    }

    if (distArr[0] > distArr[1] && distArr[2] > distArr[1]) {
      isBalance = 1;
      save_hrzCali_record("水平标定2-HrzProFunct函数isBalance = 1;");
      //ForHrzCalib  =	FALSE;/////////////////

      setzero.Format("42 05 00 00 ca ");
      len = String2Hex(setzero, senddata);
      if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
      //len =String2Hex(setzero,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
      //m_ctrlComm.put_Output(COleVariant(hexdata));
      ////m_ctrlComm.put_RThreshold(0); //为0，不会跳转到OnComm()函数
      Sleep(500);
      //m_ctrlComm.put_InputLen(0); //设置当前接收区数据长度为0 
      //m_ctrlComm.get_Input();//清空缓冲数据
      //save_hrzCali_record("水平标定-HrzProFunct-actions(LEFT, 26, NULL, SETPOSITION, HRZMOTORMOVE, JPHRZCALIB);");
      actions(LEFT, 26, NULL, SETPOSITION, HRZMOTORMOVE, JPHRZCALIB);
      Sleep(500);
    }

    else if (distArr[0] > distArr[1] && distArr[1] > distArr[2]) {
      isBalance = 0;
      setzero.Format("42 05 00 00 ca ");
      len = String2Hex(setzero, senddata);
      if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
      //len =String2Hex(setzero,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
      //m_ctrlComm.put_Output(COleVariant(hexdata));
      Sleep(500);
      //m_ctrlComm.put_InputLen(0); //设置当前接收区数据长度为0 
      //m_ctrlComm.get_Input();//清空缓冲数据
      //save_hrzCali_record("水平标定-HrzProFunct-actions(RIGHT, NULL, 10, SETPOSITION, HRZMOTORMOVE, JPHRZCALIB);");
      actions(RIGHT, NULL, 10, SETPOSITION, HRZMOTORMOVE, JPHRZCALIB);
      Sleep(500);
    }

    else if (distArr[0] < distArr[1] && distArr[1] < distArr[2]) {
      isBalance = 0;
      setzero.Format("42 05 00 00 ca ");
      len = String2Hex(setzero, senddata);
      if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
      //len =String2Hex(setzero,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
      //m_ctrlComm.put_Output(COleVariant(hexdata));
      Sleep(250);
      //m_ctrlComm.put_InputLen(0); //设置当前接收区数据长度为0 
      //m_ctrlComm.get_Input();//清空缓冲数据
      //save_hrzCali_record("水平标定-HrzProFunct-actions(LEFT, 30, NULL, SETPOSITION, HRZMOTORMOVE, JPHRZCALIB);");
      actions(LEFT, 30, NULL, SETPOSITION, HRZMOTORMOVE, JPHRZCALIB);
      Sleep(500);
    }

    else if (distArr[0] < distArr[1] && distArr[1] > distArr[2]) {
      isBalance = 0;
      setzero.Format("42 05 00 00 ca ");
      int len = String2Hex(setzero, senddata);
      if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
      //len =String2Hex(setzero,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
      //m_ctrlComm.put_Output(COleVariant(hexdata));
      Sleep(250);
      //m_ctrlComm.put_InputLen(0); //设置当前接收区数据长度为0 
      //m_ctrlComm.get_Input();//清空缓冲数据
      //save_hrzCali_record("水平标定-HrzProFunct-actions(LEFT, 10, NULL, SETPOSITION, HRZMOTORMOVE, JPHRZCALIB);");
      actions(LEFT, 10, NULL, SETPOSITION, HRZMOTORMOVE, JPHRZCALIB);
      Sleep(500);
    }
  }
}

void	CLaster::ConfirmEdge() {
  if (HrzEdgeConfirm == FALSE) {
    actions(NULL, NULL, NULL, NULL, FIXPOINT, JPAVG);
    Sleep(20);
  }
  if (VertEdgeConfirm == FALSE && HrzEdgeConfirm == TRUE) {
    actions(NULL, NULL, NULL, NULL, FIXPOINT, JPAVG);
    Sleep(20);
  }
  if (HrzDetect == FALSE) {
    actions(NULL, NULL, NULL, NULL, FIXPOINT, JPAVG);
    Sleep(20);
  }
  if (VertDetect == FALSE && HrzDetect == TRUE) {
    actions(NULL, NULL, NULL, NULL, FIXPOINT, JPAVG);
    Sleep(20);
  }
}

void	CLaster::CalibHriz() {
  //save_hrzCali_record("水平标定-【进入CalibHriz()函数】");

  //hrizmeasure = FALSE;
  //ForHrzCalib =FALSE;

  unsigned short length_data;
  unsigned short i = 0, j = 0;

  CString setzero, sendData;
  CByteArray hexdata;
  int len;

  length_data = (data_length - data_order) / 2;

  if (isBalance == FALSE) {
    //save_hrzCali_record("水平标定-CalibHriz-actions(NULL, 26, 26, NULL, HRZSCANNING, JPHRZMOVE);");
    actions(NULL, 26, 26, NULL, HRZSCANNING, JPHRZMOVE);
    Sleep(500);
  }
  //每隔7步（1.6度）进行一次定点测距，范围是-7.65 ~ 7.65 度
  else if (NumberMove <= 10 && isBalance == TRUE) {
    if (DistMin > distancePrecese) {
      cntSteps += 7;
      DistMin = distancePrecese;
    }
    ResetOrNot = FALSE;

    NumberMove++;
    CString content; content.Format("水平标定2-测距，NumberMove=%d", NumberMove);
    save_hrzCali_record(content);
    //save_hrzCali_record("水平标定-CalibHriz()3410行if(DistMin > distancePrecese ) action()");
    actions(NULL, NULL, NULL, NULL, MEASPRICISE, JPAVGPRECISE);
    //Sleep(500);//SLY ADD
  }

  else if (NumberMove > 10 && ResetOrNot == FALSE) {
    ResetOrNot = TRUE;
    NumberMove = 11;
    //save_hrzCali_record("水平标定-CalibHriz-actions(NULL, NULL, NULL, TRUE, RESET, JPHRZCALIB);");
    actions(NULL, NULL, NULL, TRUE, RESET, JPHRZCALIB);
    Sleep(500);
  }

  else if (ResetOrNot == TRUE && jmpAbtCenter == FALSE) {
    jmpAbtCenter = TRUE;
    //save_hrzCali_record("水平标定-CalibHriz-actions(RIGHT, NULL, cntSteps, SETPOSITION, HRZMOTORMOVE, JPHRZCALIB );");
    save_hrzCali_record("水平标定2-确认+-10步");
    //actions(RIGHT, NULL, cntSteps-7, SETPOSITION, HRZMOTORMOVE, JPHRZCALIB );//原程序
    actions(RIGHT, NULL, cntSteps, SETPOSITION, HRZMOTORMOVE, JPHRZCALIB);
    Sleep(500);
  }

  else if (jmpAbtCenter == TRUE && microMove == FALSE) {
    microMove = TRUE;
    //save_hrzCali_record("水平标定-CalibHriz-actions(LEFT, 10, NULL, SETPOSITION, HRZMOTORMOVE, JPHRZCALIB);");
    save_hrzCali_record("水平标定2-定位到-10步");
    //actions(LEFT, 5, NULL, SETPOSITION, HRZMOTORMOVE, JPHRZCALIB);//原程序
    actions(LEFT, 10, NULL, SETPOSITION, HRZMOTORMOVE, JPHRZCALIB);
    Sleep(500);

    //measurenum =1;
    //microMove = TRUE;
    //// 		if(runnum)
    //// 		{
    //// 			//runnum--;
    //actions(LEFT, 40, NULL, UNSETPOSITION, HRZMOTORMOVE, JPHRZCALIB);
    ///*		}*/



    //Sleep(500);
  }
  else if (microMoveNum < 20 && microMove == TRUE) {
    //microMoveNum++;
    if (microMin >= distancePrecese) {
      microCntSteps++;

      if (microMin == distancePrecese)
        microEqual++;
      else
        microEqual = 0;
      microMin = distancePrecese;
    }
    microMoveNum++;
    CString content; content.Format("水平标定2-测距，microMoveNum=%d", microMoveNum);
    save_hrzCali_record(content);
    //if (microMoveNum < 20)
      //save_hrzCali_record("水平标定-CalibHriz()3451行if(microMoveNum < 20) action()");
    actions(NULL, NULL, NULL, NULL, MEASPRICISE, JPAVGPRECISE);
    Sleep(20);
    //microMoveNum=20;
    //runnum = 1;
    //actions(NULL,NULL, NULL, NULL, MEASPRICISE,HRIZDISTANCE);//测20000次，跳转到HrizDisAvg()函数，得到第一个边长
    //Sleep(500);
    //actions(RIGHT, 80, NULL, UNSETPOSITION, HRZMOTORMOVE, JPHRZCALIB);
  }

  else if (microMoveNum >= 20)//原程序为10
  {
    save_hrzCali_record("水平标定-【即将显示完成】\n");
    microCntSteps -= (microEqual / 2 + 1);
    sendData.Format("%04x", microCntSteps);
    setzero.Format("42 05 00 00 ca ");
    //len =String2Hex(setzero,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
    //m_ctrlComm.put_Output(COleVariant(hexdata));
    len = String2Hex(setzero, senddata);
    if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
    Sleep(500);
    setzero.Format("42 01 00 09 %c%c %c%c 07 d0 00 00 07 d0 01 ca",
      sendData[0], sendData[1], sendData[2], sendData[3]);
    int len = String2Hex(setzero, senddata);
    if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
    //len = String2Hex(setzero, hexdata);
    //m_ctrlComm.put_Output(COleVariant(hexdata));
    walktext = FALSE;
    scanyard = 0;
    scanyardToAdjust = 0;
    walksetup = FALSE;
    avg_order = FALSE;
    auto_catch = FALSE;
    Calib_fuction = FALSE;
    ForHrzCalib = FALSE;
    distPrcs = FALSE;
    HrzScnProc = FALSE;
    OrdDistance = FALSE;
    ConfirmFunct = FALSE;

    save_hrzCali_record("水平标定-【显示完成】\n");
    m_strRXData = "水平标定：完成";
    Edit_StandHigh = 0;
    UpdateData(FALSE);
    refleshFlag1 = 1; *refleshFlag1pointer = 1;//水平标定完成
  }
}

void CLaster::OnBnClickedButtonTwod() {
  refleshFlag3 = -1; *refleshFlag3pointer = -1;//二维扫描，等待画二维点
  refleshFlag1 = 0; *refleshFlag1pointer = 0;
  refleshFlag2 = 0; *refleshFlag2pointer = 0;
  refleshFlag4 = 0; *refleshFlag4pointer = 0;
  HrzScnProc = FALSE;
  for (int i = 0; i < 500; i++)
    save_data[i] = 4;
  TwoDimenScan = TRUE;
  //save_hrzCali_record("水平标定-OnBnClickedButtonTwod()-actions(NULL, 26, 26, NULL, HRZSCANNING, JPHRZMOVE);");
  actions(NULL, 26, 26, NULL, HRZSCANNING, JPHRZMOVE);

  Sleep(500);
  //OnBnClickedButtonReset();
}


void CLaster::OnBnClickedButtonLeftten() {
  // TODO: 在此添加控件通知处理程序代码
  CString up;
  up.Format("42 01 00 09 ff f6 07 d0 00 00 07 d0 00 ca");
  int len = String2Hex(up, senddata);
  if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
  //CByteArray hexdata; 
  //int len=String2Hex(up,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
  //m_ctrlComm.put_Output(COleVariant(hexdata)); 
  Sleep(100);
  //m_ctrlComm.put_InputLen(0); //设置当前接收区数据长度为0 
  //m_ctrlComm.get_Input();
}


void CLaster::OnBnClickedButtonUpten() {
  refleshFlag1 = -1; *refleshFlag1pointer = -1;
  refleshFlag2 = 0; *refleshFlag2pointer = 0;
  refleshFlag3 = 0; *refleshFlag3pointer = 0;
  refleshFlag4 = 0; *refleshFlag4pointer = 0;
  Edit_StandHigh += 20;
  UpdateData(FALSE);
  refleshFlag1 = 1; *refleshFlag1pointer = 1;//上移10步
  // TODO: 在此添加控件通知处理程序代码
  CString up;
  up.Format("42 01 00 09 00 00 07 d0 00 0a 07 d0 00 ca");
  int len = String2Hex(up, senddata);
  if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
  //CByteArray hexdata; 
  //int len=String2Hex(up,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
  //m_ctrlComm.put_Output(COleVariant(hexdata)); 
  Sleep(100);
  //m_ctrlComm.put_InputLen(0); //设置当前接收区数据长度为0 
  //m_ctrlComm.get_Input();
}


void CLaster::OnBnClickedButtonRightten() {
  // TODO: 在此添加控件通知处理程序代码
  CString up;
  up.Format("42 01 00 09 00 0a 07 d0 00 00 07 d0 00 ca");
  int len = String2Hex(up, senddata);
  if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
  //CByteArray hexdata; 
  //int len=String2Hex(up,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
  //m_ctrlComm.put_Output(COleVariant(hexdata)); 
  Sleep(100);
  //m_ctrlComm.put_InputLen(0); //设置当前接收区数据长度为0 
  //m_ctrlComm.get_Input();
}


void CLaster::OnBnClickedButtonDownten() {
  refleshFlag1 = -1; *refleshFlag1pointer = -1;
  refleshFlag2 = 0; *refleshFlag2pointer = 0;
  refleshFlag3 = 0; *refleshFlag3pointer = 0;
  refleshFlag4 = 0; *refleshFlag4pointer = 0;
  Edit_StandHigh -= 20;
  UpdateData(FALSE);
  refleshFlag1 = 1; *refleshFlag1pointer = 1;//下移10步
  // TODO: 在此添加控件通知处理程序代码
  CString up;
  up.Format("42 01 00 09 00 00 07 d0 ff f6 07 d0 00 ca");
  int len = String2Hex(up, senddata);
  if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
  //CByteArray hexdata; 
  //int len=String2Hex(up,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
  //m_ctrlComm.put_Output(COleVariant(hexdata)); 
  Sleep(100);
  //m_ctrlComm.put_InputLen(0); //设置当前接收区数据长度为0 
  //m_ctrlComm.get_Input();
}





void CLaster::OnBnClickedButtonReset() {
  refleshFlag1 = -1; *refleshFlag1pointer = -1;
  refleshFlag2 = 0; *refleshFlag2pointer = 0;
  refleshFlag3 = 0; *refleshFlag3pointer = 0;
  refleshFlag4 = 0; *refleshFlag4pointer = 0;
  Edit_StandHigh = 0;
  UpdateData(FALSE);
  refleshFlag1 = 1; *refleshFlag1pointer = 1;//回到零点
  // TODO: 在此添加控件通知处理程序代码
  avg_order = FALSE;
  //NUM = 1;
  CString setzero;
  setzero.Format("42 05 00 00 ca ");
  int len = String2Hex(setzero, senddata);
  if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
  //CByteArray hexdata; 
  //int len=String2Hex(setzero,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
  //m_ctrlComm.put_Output(COleVariant(hexdata)); 
  Sleep(100);
  //m_ctrlComm.put_InputLen(0); //设置当前接收区数据长度为0 
  //m_ctrlComm.get_Input();
}


void CLaster::OnBnClickedButtonPrepare() {
  // TODO: 在此添加控件通知处理程序代码
  data_order = 8;
  data_h = 0;
  line_num = 0;
  positive = 6;
  data_length = 1908;//03b6== 950；950*2+8=19008
  next_frame = 6;
  prepare_measure = TRUE;
  CString fix;
  int len;
  fix.Format("43 01 00 09 03 b6 03 01 02 03 e8 00 01 ca");//定点测距，1000个点，一帧包含1000个点
  //CByteArray hexdata; 
  //len = String2Hex(fix, hexdata);
  //m_ctrlComm.put_Output(COleVariant(hexdata));
  //m_ctrlComm.put_InputLen(0); //设置当前接收区数据长度为0 
  //m_ctrlComm.get_Input();
  //m_ctrlComm.put_RThreshold(data_length);  //参数1表示每当串口接收缓冲区中有多于或等于1个字符时将引发一个接收数据的OnComm事件 
  len = String2Hex(fix, senddata);
  if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);

}


void CLaster::OnBnClickedButton8() {
  // TODO: 在此添加控件通知处理程序代码
  //save_hrzCali_record("画图测试");
  //if(!bDrawInit);
  //k=(mY*mX-mXY*n)/(mX*mX-mXX*n);
  float temp9 = FLT_MIN;
  float temp1 = 1000 * (FLT_MIN);
  float temp2 = 1000 * (FLT_MIN)*(FLT_MIN);
  float temp4 = 100 * (FLT_MAX);
  float temp3 = 0.01*(FLT_MAX) / (FLT_MIN);
  if (abs(temp3) > FLT_MAX*0.01)
    temp3 = 0;
  float temp01 = (abs(FLT_MIN) < 1000 * (FLT_MIN)*FLT_MIN);
  float temp02 = (abs(FLT_MIN) < 1000 * (FLT_MIN)*FLT_MIN);



  draw_OnLaserPaintInit();
  //else draw_white();
  data_h = 109;
  for (int i = 5; i < 15; i++) {
    save_data[i] = 33330;
    save_data[i + 30] = 63330;
  }
  draw_getready_2Ddata();
  draw_2Ddata();
  MessageBox("2D");
  draw_getready_guideline();
  draw_guideline();
  {
    pDrawControlDC->SelectObject(&RedPen);
    int x = 0, y = 10000, pointSize = 3;
    pDrawControlDC->Ellipse(draw_x_real2pixel(x) - pointSize, draw_y_real2pixel(y) - pointSize, draw_x_real2pixel(x) + pointSize, draw_y_real2pixel(y) + pointSize);
    x = 100; y = 20000;
    pDrawControlDC->Ellipse(draw_x_real2pixel(x) - pointSize, draw_y_real2pixel(y) - pointSize, draw_x_real2pixel(x) + pointSize, draw_y_real2pixel(y) + pointSize);
  }
  MessageBox("guideline");

  draw_getready_xy();
  draw_xyCoor();
  MessageBox("xyCoor");
  draw_xy2Ddata();
  MessageBox("2Ddata1");
  for (int i = 5; i < 15; i++) {
    save_data[i] = 33330 / 2;
    save_data[i + 30] = 63330 / 2;
  }
  draw_xy2Ddata();
  MessageBox("2Ddata2");



  pDrawWnd->ReleaseDC(pDrawControlDC);
}


void CLaster::OnBnClickedSave() {
  // TODO: 在此添加控件通知处理程序代码
  {
    laser_parameters lasers;
    CString str;
    if (this->linenum.str_guidenum == "" || this->linenum.str_stopnum == "") {
      //str.Format("Laser11");
      MessageBox("未设置引导线！");
      return;
    }
    else
      str.Format("%s%s%s", "Laser", this->linenum.str_guidenum, this->linenum.str_stopnum);
    String Str = LPCTSTR(str);
    lasers.guideline_stopline_name = Str;
    //lasers.install_high=this->install_high;
    lasers.object_dis = this->object_dis;
    lasers.guide_line_k = this->guide_line_k;
    lasers.guide_line_b = this->guide_line_b;
    lasers.guide_line_x = this->guide_line_x;
    lasers.guide_line_y = this->guide_line_y;
    /*lasers.HrzConfirmSteps=this->HrzConfirmSteps;
    lasers.VertConfirmSteps=this->VertConfirmSteps;
    lasers.HrzOffsetAngle = this->Edit_x_Offset;
    lasers.HrzConfirmProbability = this->HrzConfirmProbability;
    lasers.VertConfirmProbability = this->VertConfirmProbability;*/

    Doc.writedata(routename, Str, lasers);
    MessageBox(TEXT("保存成功！"), TEXT("消息"));
    /*Doc.writedata(routename,"install_high",this->install_high);
    Doc.writedata(routename,"object_dis",this->object_dis);
    Doc.writedata(routename,"guide_line_k",this->guide_line_k);
    Doc.writedata(routename,"guide_line_b",this->guide_line_b);
    Doc.writedata(routename,"guide_line_x",this->guide_line_x);
    Doc.writedata(routename,"guide_line_y",this->guide_line_y);
    Doc.writedata(routename,"HrzConfirmSteps",this->HrzConfirmSteps);
    Doc.writedata(routename,"VertConfirmSteps",this->VertConfirmSteps);*/
    refleshFlag1 = 1; *refleshFlag1pointer = 1;//保存完成
    refleshFlag2 = 1; *refleshFlag2pointer = 1;//保存完成
  }
}


int CLaster::save_hrzCali_record(const char *content)//保存水平标定记录
{
  FILE *fp = fopen("D:\\plane\\HrzCali_Record.txt", "a");
  if (fp == NULL) return -1;
  char pblgtime[20];

  time_t suspectComplete_m_t;
  suspectComplete_m_t = time(&suspectComplete_m_t);
  strftime(pblgtime, 20, "%Y-%m-%d %X", localtime(&suspectComplete_m_t));
  fprintf(fp, "%s ", pblgtime);
  fprintf(fp, content);
  fprintf(fp, "\n");

  if (ferror(fp) != 0 || fclose(fp) != 0) return -1;
  else return 0;
}

void CLaster::OnBnClickedButtonReadedge() {
  refleshFlag1 = -1; *refleshFlag1pointer = -1;
  refleshFlag2 = 0; *refleshFlag2pointer = 0;
  refleshFlag3 = 0; *refleshFlag3pointer = 0;
  refleshFlag4 = 0; *refleshFlag4pointer = 0;
  HrzScnProc = FALSE;
  // TODO: 在此添加控件通知处理程序代码
  //Document readAgent;
  //HrzConfirmSteps = Doc.readalonexdata(".\\test.xml", "HrzConfirmSteps");//从配置文件读出
  //VertConfirmSteps = Doc.readalonexdata(".\\test.xml", "VertConfirmSteps");
  ForHrzCalib = FALSE;

  string str_guide, str_stop;
  //if(linenum.str_guidenum=="")
  str_guide = "guide1";
  //else 
  //str_guide="guide"+linenum.str_guidenum;
  //if(linenum.str_stopnum=="")
  str_stop = "stop1";
  //else str_stop="stop"+linenum.str_stopnum;

  laser_machine_parameters lp = Doc.readlaserparamters(".\\test.xml");
  HrzConfirmSteps = lp.HrzConfirmSteps;
  VertConfirmSteps = lp.VertConfirmSteps;
  install_high = lp.install_high;
  Auto_StandHigh = HrzConfirmSteps;
  if (guide_line_k != 0)
    guide_line_x = float(guide_line_y - guide_line_b) / guide_line_k;

  m_strRXData = "读取边框";
  walktext_deflection.Format("水平步数：%d\n垂直步数：%d", HrzConfirmSteps, VertConfirmSteps);
  UpdateData(FALSE);
  refleshFlag1 = 1; *refleshFlag1pointer = 1;//读取边框
}

void CLaster::OnBnClickedButtonReadguideline() {
  HrzScnProc = FALSE;
  // TODO: 在此添加控件通知处理程序代码
  string str_guide, str_stop;
  if (linenum.str_guidenum == "")str_guide = "guide1";
  else str_guide = "guide" + linenum.str_guidenum;
  if (linenum.str_stopnum == "")str_stop = "stop1";
  else str_stop = "stop" + linenum.str_stopnum;

  laser_parameters lp = Doc.readlaserdatas(".\\test.xml", str_guide, str_stop);
  guide_line_k = lp.guide_line_k;
  guide_line_b = lp.guide_line_b;
  guide_line_x = lp.guide_line_x;
  guide_line_y = lp.guide_line_y;
  object_dis = lp.object_dis;
  offset_angle = lp.offset_angle;

  laser_machine_parameters lmp = Doc.readlaserparamters(".\\test.xml");
  install_high = lmp.install_high;
  HrzConfirmSteps = lmp.HrzConfirmSteps;
  VertConfirmSteps = lmp.VertConfirmSteps;
  Edit_x_Offset = lmp.HrzOffsetAngle;
  HrzConfirmProbability = lmp.HrzConfirmProbability;
  VertConfirmProbability = lmp.VertConfirmProbability;

  if (guide_line_k != 0)
    guide_line_x = float(guide_line_y - guide_line_b) / guide_line_k;
  //GetDlgItem(IDC_WalkTest)->EnableWindow(TRUE);
  UpdateData(FALSE);
}
//void	CLaster::HrizDisAvg()
//{
//	int i,j = 0;
//	int ko,length_data,data_point;
//	int length_long;
//	int sum_n = 0;
//	int sum_a = 0;
//	int HrzCalibDistan = 0;
//	length_data = (data_length-data_order)/2;//一行的扫描数据点
//
//	ForHrzCalib = FALSE;
//	hrizmeasure = FALSE;
//
//	for (i=0;i<=data_h;i++)
//	{			
//		ko=i%length_data;//扫描点所在的扫描行
//		if (ko==0 && i!=0)
//		{
//			length_long=length_data;
//			data_point = i-length_data;//每一帧的数据点
//			for (data_point;data_point<i;data_point++)
//			{
//				if (save_data[data_point]>1000)
//				{
//					sum_n += save_data[data_point];
//				}
//				else
//				{
//					length_long--;
//				}
//
//			}
//			if (length_long==0)
//			{
//				length_long=1;
//			}
//			sum_a = sum_a + sum_n/length_long;
//			sum_n = 0;
//			j++;	//帧数
//		}
//
//	}	
//	if (j == 0)
//	{
//		HrzCalibDistan  = 100000;
//	}
//	else
//	{
//		HrzCalibDistan  = sum_a/j;
//	}
//	if(measurenum==1)
//	{
//		HrzCalibDistanFirst     = HrzCalibDistan;
//	}
//	else if(measurenum==2)
//	{
//		HrzCalibDistanSecond = HrzCalibDistan;
//	}
//	if(runnum==1)
//	{
//		runnum--;
//		actions(RIGHT, 80, NULL, UNSETPOSITION, HRZMOTORMOVE, JPHRZCALIB);
//		Sleep(500);
//	}
//	//return  HrzCalibDistan;
//}

void CLaster::OnBnClickedButton10() {
  // TODO: 在此添加控件通知处理程序代码
  CString text;
  CString redorder;

  if (redlight == 0) {
    redorder.Format("43 07 00 01 01 ca");
    //CByteArray hexdata; 
    //int len=String2Hex(redorder,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
    //m_ctrlComm.put_Output(COleVariant(hexdata)); //发送十六进制数据 
    int len = String2Hex(redorder, senddata);
    if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
    redlight = 1;
    save_hrzCali_record("发送命令开启红色激光");
  }
  else {
    redorder.Format("43 07 00 01 00 ca");
    //CByteArray hexdata; 
    //int len=String2Hex(redorder,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
    //m_ctrlComm.put_Output(COleVariant(hexdata)); //发送十六进制数据 
    int len = String2Hex(redorder, senddata);
    if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
    redlight = 0;
    save_hrzCali_record("发送命令关闭红色激光");
  }
}


void CLaster::OnBnClickedButtonClc() {
  // TODO: 在此添加控件通知处理程序代码
  bDrawGetReadyXY = 0;
  draw_OnLaserPaintInit();
  //draw_white();
}


void CLaster::OnBnClickedButton2dswitch3d() {
  // TODO: 在此添加控件通知处理程序代码
  if (draw_status == 1)
    draw_status = 2;
  else draw_status = 1;
}


void CLaster::OnBnClickedButtonComfirmtype() {
  refleshFlag1 = -1; *refleshFlag1pointer = -1;
  refleshFlag2 = 0; *refleshFlag2pointer = 0;
  refleshFlag3 = 0; *refleshFlag3pointer = 0;
  refleshFlag4 = 0; *refleshFlag4pointer = 0;
  // TODO: 在此添加控件通知处理程序代码
  HrzScnProc = FALSE;
  comfirm_type = 1;

  UpdateData(TRUE);

  if (guide_line_k != 0)
    guide_line_x = float(guide_line_y - guide_line_b) / guide_line_k;

  short int sx, ex, sy, ey;
  data_h = 0;
  data_order = 16;
  line_num = 0;
  positive = 6;
  next_frame = 5;
  walksetup = 0;
  walktext = 0;
  scanyard = 0;
  scanyardToAdjust = 0;
  stan_catch = 0;
  auto_catch = 0;
  stop_lp = FALSE;

  scan_mode = HORIZONTAL;
  loop = 0;
  float alpha_y_temp = ((float)install_high - (float)object_high) / (float)guide_line_y;
  float angle = atan(alpha_y_temp);
  int step = Edit_StandHigh;
  Edit_StandHigh = -angle * 180 / YSTEPANGLE / PI;
  if (Edit_StandHigh % 2 != 0) {
    Edit_StandHigh++;
  }
  step = (Edit_StandHigh - step) / 2;
  /*if(step>0)
  {
  actions(UP,0,step,UNSETPOSITION,VERTMOTORMOVE,JPAVGPRECISE);
  distPrcs=0;
  }
  else if(step<0)
  {
  actions(DOWN,0,-step,UNSETPOSITION,VERTMOTORMOVE,JPAVGPRECISE);
  distPrcs=0;
  }*/


  UpdateData(FALSE);
  refleshFlag1 = 2; *refleshFlag1pointer = 2;//验证停止点，改变了垂直角度，还没有扫描

  OnAutocatch();

}


void CLaster::OnBnClickedButtonReadtype() {
  refleshFlag1 = -1; *refleshFlag1pointer = -1;
  refleshFlag2 = -1; *refleshFlag2pointer = -1;
  refleshFlag3 = 0; *refleshFlag3pointer = 0;
  refleshFlag4 = 0; *refleshFlag4pointer = 0;
  // TODO: 在此添加控件通知处理程序代码
  HrzScnProc = FALSE;

  UpdateData(TRUE);
  //PlaneType4Confirm = "A320";//test
  string s(PlaneType4Confirm.GetBuffer());

  /*int lenth1,size1;
  char testStr[200];
  lenth1 = strlen(testStr);
  size1 = sizeof(testStr);
  strncpy(testStr,(LPCTSTR)PlaneType4Confirm,sizeof(testStr));
  strncpy(testStr,(LPCTSTR)PlaneType4Confirm,sizeof(PlaneType4Confirm));
  lenth1 = strlen(testStr);
*/
  Doc.read_gl_sl(".\\test.xml", s);

  PlaneType4Confirm;
  Doc.guaidlinename;
  Doc.stoplinename;
  /*string str_guide,str_stop;
  if(linenum.str_guidenum=="")str_guide= "guide1";
  else str_guide="guide"+linenum.str_guidenum;
  if(linenum.str_stopnum=="")str_stop= "stop1";
  else str_stop="stop"+linenum.str_stopnum;*/
  if (Doc.guaidlinename == "" || Doc.stoplinename == "") {
    m_strRXData = "读取机型";
    walktext_deflection.Format("%s\n未定义", PlaneType4Confirm);
    refleshFlag1 = 1; *refleshFlag1pointer = 1;//读取机型，未读到
    refleshFlag2 = 0; *refleshFlag2pointer = 0;//读取机型，未读到
  }
  else {
    laser_parameters lp = Doc.readlaserdatas(".\\test.xml", Doc.guaidlinename, Doc.stoplinename);
    //laser_parameters lp = Doc.readlaserdatas(".\\test.xml", str_guide,str_stop);
    guide_line_k = lp.guide_line_k;
    guide_line_b = lp.guide_line_b;
    guide_line_x = lp.guide_line_x;
    guide_line_y = lp.guide_line_y;
    object_dis = lp.object_dis;
	offset_angle = lp.offset_angle;
	Edit_x_Offset = offset_angle;
	Edit_Start_x = -6.2 + Edit_x_Offset;
    Edit_End_x = 6.2 + Edit_x_Offset;

    laser_machine_parameters lmp = Doc.readlaserparamters(".\\test.xml");
    install_high = lmp.install_high;
    HrzConfirmSteps = lmp.HrzConfirmSteps;
    VertConfirmSteps = lmp.VertConfirmSteps;
    Edit_x_Offset = lmp.HrzOffsetAngle;
    HrzConfirmProbability = lmp.HrzConfirmProbability;
    VertConfirmProbability = lmp.VertConfirmProbability;

    if ((lp.guide_line_y < 800000 && lp.guide_line_y >= 0) && abs(lp.guide_line_x) < 800000 && abs(lp.guide_line_b) < 800000) {
      if (guide_line_k != 0)
        guide_line_x = float(guide_line_y - guide_line_b) / guide_line_k;

      m_strRXData = "读取机型";
      walktext_deflection.Format("%s\n引导线%c 停止线%c\n停止点：\n%d, %d\nk=%.3f\nb=%.0f", PlaneType4Confirm, Doc.guaidlinename[5], Doc.stoplinename[4], guide_line_x, guide_line_y, guide_line_k, guide_line_b);
      //walktext_deflection.Format("引导线%c 停止线%c\n停止点：\n%d, %d\nk=%.3f\nb=%.0f",LineNum4Confirm[0],LineNum4Confirm[1],guide_line_x,guide_line_y,guide_line_k,guide_line_b);
      //UpdateData(FALSE);
      refleshFlag1 = 1; *refleshFlag1pointer = 1;//读取机型，读到
      refleshFlag2 = 1; *refleshFlag2pointer = 1;//读取机型，读到
      //画图，画引导线
      /*draw_getready_guideline();
      draw_white();
      draw_guideline();*/

      //GetDlgItem(IDC_WalkTest)->EnableWindow(TRUE);
      //GetDlgItem(IDC_StopLine)->EnableWindow(TRUE);
      //GetDlgItem(IDC_BUTTON_COMFIRMTYPE)->EnableWindow(TRUE);
    }
    else {
      m_strRXData = "读取机型";
      walktext_deflection.Format("%s\n引导线%c 停止线%c\n未定义引导线参数", PlaneType4Confirm, Doc.guaidlinename[5], Doc.stoplinename[4]);
      refleshFlag1 = 1; *refleshFlag1pointer = 1;//读取机型，读到但未定义引导线
      refleshFlag2 = 0; *refleshFlag2pointer = 0;//读取机型，读到但未定义引导线
    }
  }

  UpdateData(FALSE);
  //refleshFlag1=
}


void CLaster::OnBnClickedButtonReadlinenum() {
  refleshFlag1 = -1; *refleshFlag1pointer = -1;
  refleshFlag2 = -1; *refleshFlag2pointer = -1;
  refleshFlag3 = 0; *refleshFlag3pointer = 0;
  refleshFlag4 = 0; *refleshFlag4pointer = 0;
  // TODO: 在此添加控件通知处理程序代码
  HrzScnProc = FALSE;

  UpdateData(TRUE);
  CString tempCString, tempCString2;
  if (LineNum4Confirm.GetLength() == 2) {
    tempCString = "guide";
    tempCString2 = LineNum4Confirm[0];
    tempCString += tempCString2;
    Doc.guaidlinename = tempCString.GetBuffer();
    tempCString = "stop";
    if (LineNum4Confirm[1] >= 'a'&&LineNum4Confirm[1] <= 'z')
      LineNum4Confirm.SetAt(1, LineNum4Confirm[1] - ('a' - 'A'));
    tempCString2 = LineNum4Confirm[1];
    tempCString += tempCString2;
    Doc.stoplinename = tempCString.GetBuffer();
    laser_parameters lp = Doc.readlaserdatas(".\\test.xml", Doc.guaidlinename, Doc.stoplinename);
    laser_machine_parameters lmp = Doc.readlaserparamters(".\\test.xml");

    CString str;
    str.Format("%s%c%c", "PlaneType", LineNum4Confirm[0], LineNum4Confirm[1]);
    String Str = LPCSTR(str);
    vector<string>vplanetypes = Doc.read_Vplanetypes(routename, Str);
    int N = vplanetypes.size();
    CString CStype = "已关联机型：";
    if (N <= 0)CStype = "没有关联机型";
    for (int i = 0; i < N; i++) {
      CStype += vplanetypes.at(i).c_str();
      CStype += " ";
    }

    if ((lp.guide_line_y < 800000 && lp.guide_line_y >= 0) && abs(lp.guide_line_x) < 800000 && abs(lp.guide_line_b) < 800000) {
      guide_line_k = lp.guide_line_k;
      guide_line_b = lp.guide_line_b;
      guide_line_x = lp.guide_line_x;
      guide_line_y = lp.guide_line_y;
      install_high = lmp.install_high;
	  offset_angle = lp.offset_angle;
      object_dis = lp.object_dis;
      HrzConfirmSteps = lmp.HrzConfirmSteps;
      VertConfirmSteps = lmp.VertConfirmSteps;

	  Edit_x_Offset = offset_angle;
	  Edit_Start_x = -6.2 + Edit_x_Offset;
      Edit_End_x = 6.2 + Edit_x_Offset;
      if (guide_line_k != 0)
        guide_line_x = float(guide_line_y - guide_line_b) / guide_line_k;

      m_strRXData = "读取引导线";
      walktext_deflection.Format("引导线%c 停止线%c\n停止点：\n%d, %d\nk=%.3f\nb=%.0f\n%s", LineNum4Confirm[0], LineNum4Confirm[1], guide_line_x, guide_line_y, guide_line_k, guide_line_b, CStype);

      UpdateData(FALSE);
      refleshFlag1 = 1; *refleshFlag1pointer = 1;//读取引导线，读到
      refleshFlag2 = 1; *refleshFlag2pointer = 1;//读取引导线，读到
      //画图，画引导线
      /*draw_getready_guideline();
      draw_white();
      draw_guideline();
      //GetDlgItem(IDC_WalkTest)->EnableWindow(TRUE);
      //GetDlgItem(IDC_StopLine)->EnableWindow(TRUE);
      //GetDlgItem(IDC_BUTTON_COMFIRMTYPE)->EnableWindow(TRUE);*/
      return;
    }
    else {
      m_strRXData = "读取引导线";
      walktext_deflection.Format("引导线%c 停止线%c\n未定义参数\n%s", LineNum4Confirm[0], LineNum4Confirm[1], CStype);

      UpdateData(FALSE);
      refleshFlag1 = 1; *refleshFlag1pointer = 1;//读取引导线，未读到
      refleshFlag2 = 0; *refleshFlag2pointer = 0;//读取引导线，未读到
      //draw_white();
      return;
    }
  }
  m_strRXData = "读取引导线";
  walktext_deflection.Format("未定义\n请重新输入");//,LineNum4Confirm[0],LineNum4Confirm[1]);
  //walktext_deflection.Format("%s\n未定义",LineNum4Confirm);
  UpdateData(FALSE);
  refleshFlag1 = 1; *refleshFlag1pointer = 1;//读取引导线，输入有误
  refleshFlag2 = 0; *refleshFlag2pointer = 0;//读取引导线，输入有误

}


void CLaster::OnBnClickedButtonConfirmwalkset() {
  refleshFlag1 = -1; *refleshFlag1pointer = -1;
  refleshFlag2 = -1; *refleshFlag2pointer = -1;
  refleshFlag3 = 0; *refleshFlag3pointer = 0;
  refleshFlag4 = 0; *refleshFlag4pointer = 0;
  HrzScnProc = FALSE;
  // TODO: 在此添加控件通知处理程序代码
  if (walksetup_num == 0) {
    //MessageBox("未进行走步设置！");
    refleshFlag1 = 1; *refleshFlag1pointer = 1;
    refleshFlag2 = 1; *refleshFlag2pointer = 1;
    refleshFlag3 = 0; *refleshFlag3pointer = 0;
    refleshFlag4 = 0; *refleshFlag4pointer = 0;
    m_strRXData = "错误";
    walktext_deflection.Format("未进行走步设置！");//,LineNum4Confirm[0],LineNum4Confirm[1]);
    return;
  }

  UpdateData(TRUE);
  walksetup_data[abs(walksetup_num)][0] = END_LINE;
  float k, b;
  bool least_square_method = CalculateLineKB(k, b);

  if (least_square_method == TRUE) {
    walksetup_data[abs(walksetup_num)][2] = k * 10000;
    walksetup_data[abs(walksetup_num)][3] = b;
    CString strFileName1, str;
    str.Format("走步设置数据");
    ca.Format("%d", NUM);
    ca += str;
    strFileName1 = lo + ca + te;
    savefile2(strFileName1);
  }
  else {
    walksetup_data[abs(walksetup_num)][2] = 0;
    walksetup_data[abs(walksetup_num)][3] = 0;
    CString strFileName1, str;
    str.Format("走步设置数据");
    ca.Format("%d", NUM);
    ca += str;
    strFileName1 = lo + ca + te;
    savefile2(strFileName1);
  }



  guide_line_k = (float)walksetup_data[walksetup_num][2] / 10000;
  guide_line_b = walksetup_data[walksetup_num][3];
  guide_line_x = walksetup_data[walksetup_num - 1][2];
  guide_line_y = walksetup_data[walksetup_num - 1][3];

  if (guide_line_k != 0)
    guide_line_x = float(guide_line_y - guide_line_b) / guide_line_k;


  //walksetup_num = 0;


  m_strRXData = "引导线计算完成";
  walktext_deflection.Format("停止点：\n%d, %d\nk=%.3f\nb=%.0f\n请先编号\n再保存引导线", guide_line_x, guide_line_y, guide_line_k, guide_line_b);
  UpdateData(FALSE);
  refleshFlag1 = 1; *refleshFlag1pointer = 1;//引导线计算完成
  refleshFlag2 = 1; *refleshFlag2pointer = 1;//引导线计算完成

  /*draw_getready_guideline();
  draw_white();
  draw_guideline();

  //GetDlgItem(IDC_WalkTest)->EnableWindow(TRUE);
  //GetDlgItem(IDC_StopLine)->EnableWindow(TRUE);
  //GetDlgItem(IDC_BUTTON_COMFIRMTYPE)->EnableWindow(TRUE);*/
}


void CLaster::OnBnClickedButtonClearwalkset() {
  // TODO: 在此添加控件通知处理程序代码
  walksetup_num = 0;
  refleshFlag1 = 1; *refleshFlag1pointer = 1;//清空完成
  //GetDlgItem(IDC_BUTTON_ClearWalkSet)->EnableWindow(FALSE);
  //GetDlgItem(IDC_BUTTON_ConfirmWalkSet)->EnableWindow(FALSE);
}


void CLaster::OnBnClickedButtonRedlaseron() {
  //GetDlgItem(IDC_BUTTON_RedLaserOn)->EnableWindow(FALSE);
  //GetDlgItem(IDC_BUTTON_RedLaserOff)->EnableWindow(TRUE);
  // TODO: 在此添加控件通知处理程序代码
  CString text;
  CString redorder;

  //if (redlight == 0)
  {
    redorder.Format("43 07 00 01 01 ca");
    //CByteArray hexdata; 
    //int len=String2Hex(redorder,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
    //m_ctrlComm.put_Output(COleVariant(hexdata)); //发送十六进制数据 
    int len = String2Hex(redorder, senddata);
    if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
    redlight = 1;
    save_hrzCali_record("发送命令开启红色激光");
  }
  //else
  //{
  //	redorder.Format("43 07 00 01 00 ca");
  //	CByteArray hexdata; 
  //	int len=String2Hex(redorder,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
  //	m_ctrlComm.put_Output(COleVariant(hexdata)); //发送十六进制数据 
  //	redlight =0;
  //}
  //GetDlgItem(IDC_BUTTON_RedLaserOn)->EnableWindow(FALSE);
  //GetDlgItem(IDC_BUTTON_RedLaserOff)->EnableWindow(TRUE);
}


void CLaster::OnBnClickedButtonRedlaseroff() {
  //GetDlgItem(IDC_BUTTON_RedLaserOff)->EnableWindow(FALSE);
  //GetDlgItem(IDC_BUTTON_RedLaserOn)->EnableWindow(TRUE);
  // TODO: 在此添加控件通知处理程序代码
  CString text;
  CString redorder;

  //if (redlight == 0)
  //{
  //	redorder.Format("43 07 00 01 01 ca");
  //	CByteArray hexdata; 
  //	int len=String2Hex(redorder,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
  //	m_ctrlComm.put_Output(COleVariant(hexdata)); //发送十六进制数据 
  //	redlight =1;
  //}
  //else
  {
    redorder.Format("43 07 00 01 00 ca");
    //CByteArray hexdata; 
    //int len=String2Hex(redorder,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
    //m_ctrlComm.put_Output(COleVariant(hexdata)); //发送十六进制数据 
    int len = String2Hex(redorder, senddata);
    if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
    redlight = 0;
    save_hrzCali_record("发送命令关闭红色激光");
  }
  //GetDlgItem(IDC_BUTTON_RedLaserOff)->EnableWindow(FALSE);
  //GetDlgItem(IDC_BUTTON_RedLaserOn)->EnableWindow(TRUE);
}


void CLaster::OnBnClickedButtonScanall() {
  refleshFlag1 = -1; *refleshFlag1pointer = -1;
  refleshFlag2 = 0; *refleshFlag2pointer = 0;
  refleshFlag3 = 0; *refleshFlag3pointer = 0;
  refleshFlag4 = 0; *refleshFlag4pointer = 0;
  // TODO: 在此添加控件通知处理程序代码
  HrzScnProc = FALSE;
  scan_all = 1;
  countNearPoint[0] = 0;
  countNearPoint[1] = 0;

  UpdateData(TRUE);

  short int sx, ex, sy, ey;
  data_h = 0;
  data_order = 16;
  line_num = 0;
  positive = 6;
  next_frame = 5;
  walksetup = 0;
  stan_catch = 0;
  auto_catch = 0;
  stop_lp = FALSE;

  scan_mode = HORIZONTAL;
  loop = 0;
  /*float alpha_y_temp=((float)install_high-(float)object_high)/(float)guide_line_y;
  float angle=atan (alpha_y_temp);*/
  int Edit_StandHigh_org = Edit_StandHigh;
  Edit_StandHigh = 20;//起始扫描步数
  int step = (Edit_StandHigh - Edit_StandHigh_org) / 2;
  stepMoveNoSet0(step, UP, VERTICAL);
  Sleep(200);
  CString content;
  laser_machine_parameters lp = Doc.readlaserparamters(".\\test.xml");
  VertConfirmSteps = lp.VertConfirmSteps;
  /*int step=Edit_StandHigh;
  Edit_StandHigh = - angle*180/YSTEPANGLE/PI;
  if (Edit_StandHigh%2 != 0)
  {
    Edit_StandHigh++;
  }
  step=(Edit_StandHigh-step)/2;
  if(step>0)
  {
  actions(UP,0,step,UNSETPOSITION,VERTMOTORMOVE,JPAVGPRECISE);
  distPrcs=0;
  }
  else if(step<0)
  {
  actions(DOWN,0,-step,UNSETPOSITION,VERTMOTORMOVE,JPAVGPRECISE);
  distPrcs=0;
  }*/
  save_hrzCali_record("窗口扫描：开始");
  m_strRXData = "窗口扫描：开始";
  walktext_deflection = "执行中...";
  UpdateData(FALSE);
  refleshFlag1 = 2; *refleshFlag1pointer = 2;//窗口扫描：开始

  OnAutocatch();
  Sleep(100);
}
//void CLaster::OnBnClickedButtonScanall_test()
//{
//	HrzScnProc=FALSE;
//	// TODO: 在此添加控件通知处理程序代码
//	UpdateData(TRUE);
//	if(object_dis<=0)object_dis=40000;
//	if(guide_line_k!=0)
//		guide_line_x=float(guide_line_y-guide_line_b)/guide_line_k;
//	UpdateData(FALSE);
//	refleshFlag1=2;*refleshFlag1pointer=2;
//	//walktext = TRUE;
//	scan_all = 1;
//	CString change;
//
//	//画图，画引导线
//	draw_getready_guideline();
//	draw_white();
//	draw_guideline();
//
//	short int sx,ex,sy,ey;
//	data_h = 0;
//	data_order =16;
//	line_num = 0;
//	positive = 6;
//	next_frame = 5;
//	walksetup = 0;
//	stan_catch = 0;
//	auto_catch = 0;
//	stop_lp=FALSE;
//
//	scan_mode = HORIZONTAL;
//	loop = 0;
//	float alpha_y_temp=((float)install_high-(float)object_high-100)/(float)object_dis;//-100使扫描位置抬高一点
//	float angle=atan (alpha_y_temp);
//	Edit_StandHigh = - angle*180/YSTEPANGLE/PI;
//	if (Edit_StandHigh%2 != 0)
//	{
//		Edit_StandHigh++;
//	}
//
//	Edit_StandHigh = 0;
//
//	start_height = Edit_StandHigh;
//	// 	itoa(Edit_hundred,&hundred,10);
//	// 	itoa(Edit_ten,&ten,10);
//	// 	itoa(Edit_ge,&ge,10);
//
//	/*CString number;
//	if (format=="s0")
//	{
//	data_length=772;
//	}
//	number = "0000";*/
//	// TODO: 在此添加控件通知处理程序代码
//
//	/*m_strTXData=format+number;*/
//	save_start_x.Format("%d",Edit_Start_x);
//	save_end_x.Format("%d",Edit_End_x);
//	save_start_y.Format("%d",Edit_Start_y);
//	save_end_y.Format("%d",Edit_End_y);
//	sx = Edit_Start_x/XSTEPANGLE;
//	ex = Edit_End_x/XSTEPANGLE;
//	sy = Edit_Start_y/YSTEPANGLE;
//	ey = Edit_End_y/YSTEPANGLE;
//	int standhigh = Edit_StandHigh/2;
//	sx = sx/2;
//	ex = ex/2;
//	sy = sy/2;
//	ey = ey/2;
//	int standheight;
//	if (presacl !=0)
//	{ 
//		standheight =standhigh;
//		data_length=abs(ex - sx)*2*presacl+data_order;
//	}
//	else
//	{
//		standheight = Edit_StandHigh;
//		data_length=abs(ex - sx)*2+data_order;
//	}
//
//	frame_length = 7;
//	// format.Format("44 02 00 08 %c%c ",Edit_format[1],Edit_format[2]);
//	format.Format("53 01 00 0a 02 ");
//	change.Format("%08x",standheight);
//	Stand_High.Format("%c%c %c%c ",change[4],change[5],change[6],change[7]);
//
//
//
//	change.Format("%08x", sx);
//	start_x.Format("%c%c %c%c ",change[4],change[5],change[6],change[7]);
//
//	change.Format("%08x", ex);
//	end_x.Format("%c%c %c%c ",change[4],change[5],change[6],change[7]);
//
//	change.Format("%08x", sy);
//	start_y.Format("%c%c %c%c ",change[4],change[5],change[6],change[7]);
//
//	change.Format("%08x", ey);
//	end_y.Format("%c%c %c%c ",change[4],change[5],change[6],change[7]);
//
//	m_strTXData = format+Stand_High+start_x+end_x+"00 03 01 ca";
//	CByteArray hexdata; 
//	//int len=String2Hex(m_strTXData,hexdata); //此处返回的len可以用于计算发送了多少个十六进制数
//	//m_ctrlComm.put_Output(COleVariant(hexdata)); //发送十六进制数据 
//
//	//m_ctrlComm.put_RThreshold(data_length);  //参数1表示每当串口接收缓冲区中有多于或等于1个字符时将引发一个接收数据的OnComm事件 
//	//m_ctrlComm.put_InputLen(0); //设置当前接收区数据长度为0 
//	//m_ctrlComm.get_Input();//先预读缓冲区以清除残留数据
//	int len = String2Hex(m_strTXData, senddata);
//	if (senddata[0]==0x43)frame_length=2;else if (senddata[0]==0x53)frame_length=7;glb_sp_data.deliver2send(senddata,len);
//	m_strRXData="走步测试:启动";
//	UpdateData(FALSE);
//	//m_strRXData=m_strTXData;
//	//UpdateData(FALSE); //更新编辑框内容
//	//GetDlgItem(IDC_BTN_Clear)->EnableWindow(TRUE);
//}

void CLaster::draw_OnLaserPaintInit() {
  CPaintDC dc(this); // device context for painting
  pDrawWnd = GetDlgItem(IDC_STATIC_Draw);//(IDC_STATIC_Draw); IDC_STATIC_Draw
  pDrawControlDC = pDrawWnd->GetDC();

  pDrawWnd->Invalidate();
  pDrawWnd->UpdateWindow();

  //CRect rct; 
  pDrawWnd->GetClientRect(DrawRct);
  DrawRct_top = DrawRct.top; DrawRct_bottom = DrawRct.bottom; DrawRct_left = DrawRct.left; DrawRct_right = DrawRct.right;
  /*CPen tempPen(PS_SOLID,2,RGB(0,255,0));
  GreenPen.CreatePen(PS_SOLID,2,RGB(0,255,0));
  BluePen.CreatePen(PS_SOLID,2,RGB(0,0,255));
  OrangePen.CreatePen(PS_SOLID,2,RGB(255,150,0));
  RedPen.CreatePen(PS_SOLID,2,RGB(255,0,0));*/

  pDrawControlDC->SelectObject(&GreenPen);
  pDrawControlDC->Rectangle(DrawRct.left, DrawRct.top, DrawRct.right, DrawRct.bottom);

  draw_white_width_pixel = 10;
  bDrawInit = 1;
}

void CLaster::draw_getready_2Ddata() {
  //if(!bDrawInit)
  draw_OnLaserPaintInit();
  farY_real = DRAW_10M * 3;

  if (data_h == 0)return;
  for (int i = 0; i < data_h; i++) {
    if (save_data[i] > farY_real)
      farY_real = save_data[i];
  }
  int ten = farY_real / DRAW_10M;
  int yushu = farY_real % DRAW_10M;
  if (yushu != 0)ten++;
  farY_real = DRAW_10M * ten;

  draw_2Ddata_start_y_pixel = DrawRct_bottom;//-draw_white_width_pixel;//对应数据0
  draw_2Ddata_end_y_pixel = DrawRct_top;//+draw_white_width_pixel;//对应数据50000
  draw_y_ratio = float(farY_real) / (draw_2Ddata_start_y_pixel - draw_2Ddata_end_y_pixel);
}

void CLaster::draw_2Ddata() {//int save_data[]={4,4,788,788,4,4,15000,15000,16000,17000,14000,4,200,4};
  //if(!bDrawInit)
  draw_OnLaserPaintInit();
  /*else*/	draw_white();
  if (data_h == 0)return;
  int numData = data_h;//数据个数
  int gap = DrawRct_right / numData;//2;//两数据在画图中的x间隔
  if (gap < 1)gap = 1;
  int y_pixel, x_pixel = DrawRct_right / 2 - numData / 2 * gap, pointSize = 1;//画点的大小的一半
  //CString numstr;
  //numstr.Format("%d",numData);
  //numstr="点数"+numstr;
  //MessageBox(numstr);

  //CPen YellowPen;YellowPen.CreatePen(PS_SOLID,1,RGB(255,255,0));
  pDrawControlDC->SelectObject(&YellowPen);
  pDrawControlDC->MoveTo(DrawRct_right / 2, 0);
  pDrawControlDC->LineTo(DrawRct_right / 2, DrawRct_bottom);
  CString cor;
  for (int i = 1; i < farY_real / DRAW_10M; i++) {
    y_pixel = (draw_2Ddata_start_y_pixel - DRAW_10M * i / draw_y_ratio);
    cor.Format("%d0m", i);
    pDrawControlDC->TextOutA(0, y_pixel, cor);
    pDrawControlDC->MoveTo(0, y_pixel);
    pDrawControlDC->LineTo(DrawRct_right, y_pixel);
    cor.Format("%d0m", i);
  }

  for (int i = 0; i < numData; i++) {
    if (save_data[i] <= 4)
      pDrawControlDC->SelectObject(&BlackPen);
    else if (save_data[i] <= 2000)
      pDrawControlDC->SelectObject(&RedPen);
    else
      pDrawControlDC->SelectObject(&BluePen);
    y_pixel = (draw_2Ddata_start_y_pixel - float(save_data[i]) / draw_y_ratio);
    pDrawControlDC->Ellipse(x_pixel, y_pixel - pointSize, x_pixel + pointSize, y_pixel);
    x_pixel = x_pixel + gap;
  }
}

void CLaster::draw_getready_guideline()//从设置好的引导线来确定画图起始点、比例
{
  UpdateData(TRUE);
  if (guide_line_k != 0)
    guide_line_x = float(guide_line_y - guide_line_b) / guide_line_k;
  //if(!bDrawInit)
  draw_OnLaserPaintInit();
  //停止线上最远的xy
  nearY_real = guide_line_y;//10000<guide_line_y?10000:guide_line_y;【这里决定停止线是否画延长线】
  farY_real = 50000 > (guide_line_y + 30000) ? 50000 : (guide_line_y + 30000);//真实的较远的y，取50m和停止线+10m的较大值
  farX_real = guide_line_x;
  nearX_real = guide_line_x;
  if (guide_line_k != 0) {
    farX_real = float(farY_real - guide_line_b) / float(guide_line_k);
    nearX_real = float(nearY_real - guide_line_b) / float(guide_line_k);
  }

  draw_guideline_near_x_pixel = DrawRct_right / 2;//对应nearX_real
  draw_guideline_near_y_pixel = DrawRct_bottom - draw_white_width_pixel;//对应nearY_real

  //draw_guideline_start_x_pixel=DrawRct.right/2;//对应guide_line_x
  //draw_guideline_start_y_pixel=DrawRct.bottom-draw_white_width_pixel;//对应guide_line_y	
  draw_guideline_far_y_pixel = draw_white_width_pixel;//对应farY

  draw_y_ratio = float(farY_real - nearY_real) / float(draw_guideline_near_y_pixel - draw_guideline_far_y_pixel);
  draw_x_ratio = draw_y_ratio / 3;
  /*if(guide_line_k!=0)
  draw_guideline_end_x_pixel=(draw_guideline_end_y_pixel-draw_guideline_start_y_pixel)/guide_line_k;
  else
  draw_guideline_end_x_pixel=draw_guideline_start_x_pixel;*/

  /*draw_0_y_pixel=int( float(farY_real*draw_guideline_start_y_pixel-guide_line_y*draw_guideline_end_y_pixel)/float(farY_real-draw_guideline_end_y_pixel) );
  if(guide_line_k!=0)
  draw_0_x_pixel=draw_guideline_start_x_pixel;	*/
  if (guide_line_k != 0)
    guide_line_x = float(guide_line_y - guide_line_b) / guide_line_k;
  draw_guideline_far_x_pixel = draw_x_real2pixel(farX_real);
  draw_guideline_start_x_pixel = draw_x_real2pixel(guide_line_x);
  draw_guideline_start_y_pixel = draw_y_real2pixel(guide_line_y);
}

void CLaster::draw_guideline() {
  //draw_white();
  //if(!bDrawInit)
  draw_OnLaserPaintInit();

  int tenMin = nearY_real / DRAW_10M;
  int y_pixel, yushu = nearY_real % DRAW_10M;
  if (yushu != 0)tenMin++;
  if (tenMin < 1)tenMin = 1;
  int tenMax = farY_real / DRAW_10M;
  yushu = farY_real % DRAW_10M;
  //if(yushu!=0)tenMax--;

  pDrawControlDC->SelectObject(&YellowPen);
  CString cor;
  for (int i = tenMin; i <= tenMax; i++) {
    //y_pixel=(draw_2Ddata_start_y_pixel-DRAW_TEN*i/draw_y_ratio);
    y_pixel = draw_y_real2pixel(DRAW_10M*i);
    cor.Format("%d0m", i);
    pDrawControlDC->TextOutA(0, y_pixel, cor);
    pDrawControlDC->MoveTo(0, y_pixel);
    pDrawControlDC->LineTo(DrawRct_right, y_pixel);
    cor.Format("%d0m", i);
  }

  int stoplineLength = 200;//停止线长度共2*200mm
  int x1, y1, x2, y2, x1_pixel, y1_pixel, x2_pixel, y2_pixel;
  pDrawControlDC->SelectObject(&OrangePen);
  pDrawControlDC->MoveTo(draw_guideline_far_x_pixel, draw_guideline_far_y_pixel);
  pDrawControlDC->LineTo(draw_guideline_near_x_pixel, draw_guideline_near_y_pixel);
  if (guide_line_k != 0) {
    float alpha = atan(guide_line_k);
    x1 = guide_line_x - (float)stoplineLength*sin(alpha);
    x2 = guide_line_x + (float)stoplineLength*sin(alpha);
    y1 = guide_line_y + (float)stoplineLength*cos(alpha);
    y2 = guide_line_y - (float)stoplineLength*cos(alpha);
    /*float sqrtTemp=guide_line_k*guide_line_k+1;
    if (sqrtTemp>1)
    {
    x1=guide_line_x-stoplineLength/sqrt(sqrtTemp);
    x2=guide_line_x+stoplineLength/sqrt(sqrtTemp);
    }
    else
    {
    x1=guide_line_x-stoplineLength;
    x2=guide_line_x+stoplineLength;
    }
    y1=guide_line_y+stoplineLength/guide_line_k;
    y2=guide_line_y-stoplineLength/guide_line_k;*/
    x1_pixel = draw_x_real2pixel(x1);
    x2_pixel = draw_x_real2pixel(x2);
    y1_pixel = draw_y_real2pixel(y1);
    y2_pixel = draw_y_real2pixel(y2);
  }
  else {
    x1 = guide_line_x - stoplineLength;
    x2 = guide_line_x + stoplineLength;
    x1_pixel = draw_x_real2pixel(x1);
    x2_pixel = draw_x_real2pixel(x2);
    y1_pixel = draw_guideline_start_y_pixel;
    y2_pixel = draw_guideline_start_y_pixel;
  }
  pDrawControlDC->MoveTo(x1_pixel, y1_pixel);
  pDrawControlDC->LineTo(x2_pixel, y2_pixel);
}

void CLaster::draw_getready_xy()//
{
  draw_OnLaserPaintInit();
  //停止线上最远的xy
  int mini = 2000;//超过2000的数值才统计并画图
  int save_data_min = mini, save_data_max = save_data[0];
  for (int i = 0; i < data_h; i++) {
    if (save_data[i] > save_data_max)
      save_data_max = save_data[i];
    else if (save_data[i]<save_data_min && save_data[i]>mini)
      save_data_min = save_data[i];
  }
  nearY_real = 10000 < save_data_min ? 10000 : save_data_min;
  farY_real = 50000 > (save_data_max + 00000) ? 50000 : (save_data_max + 00000);//真实的较远的y，取50m和save_data_max+10m的较大值
  farX_real = 0;//
  nearX_real = 0;//
  //需使用 draw_guideline_near_x_pixel draw_guideline_near_y_pixel
  draw_guideline_near_x_pixel = DrawRct_right / 2;//对应 nearX_real farX_real
  draw_guideline_near_y_pixel = DrawRct_bottom - 0;//draw_white_width_pixel;//对应nearY_real
  draw_guideline_far_y_pixel = 0;//draw_white_width_pixel;//对应farY

  /*if(guide_line_k!=0)
  {
  farX_real=float(farY_real-guide_line_b)/float(guide_line_k);
  nearX_real=float(nearY_real-guide_line_b)/float(guide_line_k);
  }*/

  //draw_guideline_near_x_pixel=DrawRct_right/2;//对应nearX_real
  //draw_guideline_near_y_pixel=DrawRct_bottom-draw_white_width_pixel;//对应nearY_real

  ////draw_guideline_start_x_pixel=DrawRct.right/2;//对应guide_line_x
  ////draw_guideline_start_y_pixel=DrawRct.bottom-draw_white_width_pixel;//对应guide_line_y	
  //draw_guideline_far_y_pixel=draw_white_width_pixel;//对应farY

  draw_y_ratio = float(farY_real - nearY_real) / float(draw_guideline_near_y_pixel - draw_guideline_far_y_pixel);
  draw_x_ratio = draw_y_ratio;
  /*if(guide_line_k!=0)
  draw_guideline_end_x_pixel=(draw_guideline_end_y_pixel-draw_guideline_start_y_pixel)/guide_line_k;
  else
  draw_guideline_end_x_pixel=draw_guideline_start_x_pixel;*/

  /*draw_0_y_pixel=int( float(farY_real*draw_guideline_start_y_pixel-guide_line_y*draw_guideline_end_y_pixel)/float(farY_real-draw_guideline_end_y_pixel) );
  if(guide_line_k!=0)
  draw_0_x_pixel=draw_guideline_start_x_pixel;	*/
  //if(guide_line_k!=0)
  //	guide_line_x=float(guide_line_y-guide_line_b)/guide_line_k;
  draw_guideline_far_x_pixel = draw_x_real2pixel(farX_real);//【检查==0】
  //draw_guideline_start_x_pixel=draw_x_real2pixel(guide_line_x);
  //draw_guideline_start_y_pixel=draw_y_real2pixel(guide_line_y);
  bDrawGetReadyXY = 1;
}

void CLaster::draw_xyCoor()//画坐标
{
  draw_OnLaserPaintInit();

  int tenMin = nearY_real / DRAW_10M;
  int tempX_real, x_pixel, y_pixel, yushu = nearY_real % DRAW_10M;
  if (yushu != 0)tenMin++;
  if (tenMin < 1)tenMin = 1;
  int tenMax = farY_real / DRAW_10M;
  yushu = farY_real % DRAW_10M;
  if (yushu = 0)tenMax--;
  x_pixel = draw_guideline_near_x_pixel;

  pDrawControlDC->SelectObject(&GreenPen);
  pDrawControlDC->MoveTo(x_pixel, 0);
  pDrawControlDC->LineTo(x_pixel, DrawRct_bottom);
  //while(x_pixel)

  pDrawControlDC->SelectObject(&YellowPen);
  for (tempX_real = nearX_real - DRAW_10M; x_pixel > 0;) {
    x_pixel = draw_x_real2pixel(tempX_real);
    pDrawControlDC->MoveTo(x_pixel, 0);
    pDrawControlDC->LineTo(x_pixel, DrawRct_bottom);
    tempX_real -= DRAW_10M;
    x_pixel = draw_x_real2pixel(tempX_real);
  }
  for (tempX_real = nearX_real + DRAW_10M; x_pixel < DrawRct_right;) {
    x_pixel = draw_x_real2pixel(tempX_real);
    pDrawControlDC->MoveTo(x_pixel, 0);
    pDrawControlDC->LineTo(x_pixel, DrawRct_bottom);
    tempX_real += DRAW_10M;
    x_pixel = draw_x_real2pixel(tempX_real);
  }

  CString cor;
  for (int i = tenMin; i <= tenMax; i++) {
    //y_pixel=(draw_2Ddata_start_y_pixel-DRAW_TEN*i/draw_y_ratio);
    y_pixel = draw_y_real2pixel(DRAW_10M*i);
    cor.Format("%d0m", i);
    pDrawControlDC->TextOutA(0, y_pixel, cor);
    pDrawControlDC->MoveTo(0, y_pixel);
    pDrawControlDC->LineTo(DrawRct_right, y_pixel);
    cor.Format("%d0m", i);
  }
}

void CLaster::draw_xy2Ddata() {
  if (data_h == 0)return;
  //int numData=data_h;//数据个数
  //int gap=DrawRct_right/numData;//2;//两数据在画图中的x间隔
  //if (gap<1)gap=1;
  int y_pixel, x_pixel/*=DrawRct_right/2-numData/2*gap*/, pointSize = 1;//画点的大小的一半
  int rgbindx;//colormap的序号，取值0-1023
  float x, y, z;
  float alpha_y = YSTEPANGLE * (-(float)Edit_StandHigh)*PI / 180;
  float alpha_x/*=(Edit_Start_x/XSTEPANGLE+i-0)*XSTEPANGLE*PI/180*/;
  CPen HeightPen;

  //draw_OnLaserPaintInit();

  for (int i = 0; i < data_h; i++) {
    alpha_x = (Edit_Start_x / XSTEPANGLE + i - 0)*XSTEPANGLE*PI / 180;
    x = (float)save_data[i] * sin(alpha_x)*cos(alpha_y);
    y = (float)save_data[i] * cos(alpha_x)*cos(alpha_y);
    z = (float)install_high - (float)save_data[i] * sin(alpha_y);

    if (save_data[i] <= 4) {}
    //pDrawControlDC->SelectObject(&BlackPen); 
    else if (save_data[i] <= 2000) {}
    //pDrawControlDC->SelectObject(&RedPen); 
    else {
      rgbindx = (float)255 * z / 1000;
      if (rgbindx < 0)rgbindx = 0;
      else if (rgbindx > 1024)rgbindx = 1024;
      HeightPen.DeleteObject();
      HeightPen.CreatePen(PS_SOLID, 2, RGB(colormap1024[rgbindx][0], colormap1024[rgbindx][1], colormap1024[rgbindx][2]));//HeightPen.DeleteObject();
      pDrawControlDC->SelectObject(&HeightPen);
      x_pixel = draw_x_real2pixel(x);
      y_pixel = draw_y_real2pixel(y);
      //y_pixel=(draw_2Ddata_start_y_pixel-float(save_data[i])/draw_y_ratio);
      pDrawControlDC->Ellipse(x_pixel, y_pixel - pointSize, x_pixel + pointSize, y_pixel);
      TRACE("x=%03f, y=%03f, z=%03f\n", x, y, z);
    }
    //x_pixel=x_pixel+gap;
  }
  TRACE("\n", x, y, z);
}

int CLaster::draw_x_real2pixel(int x_real) {
  return ((int)(draw_guideline_near_x_pixel + float(x_real - nearX_real) / draw_x_ratio));
}

int CLaster::draw_y_real2pixel(int y_real) {
  return ((int)(draw_guideline_near_y_pixel - float(y_real - nearY_real) / draw_y_ratio));
}

void CLaster::draw_white()//画成全白
{
  //if(!bDrawInit)
  draw_OnLaserPaintInit();
  {
    pDrawControlDC->SelectObject(&GreenPen);
    pDrawControlDC->Rectangle(DrawRct.left, DrawRct.top, DrawRct.right, DrawRct.bottom);
  }
}

string CLaster::LaserInit() {
  try {
    HANDLE m_hMutex = ::CreateMutex(NULL, TRUE, "VIDGS_InfoCenter");//字符串里面的内容可以随便改.他只是一个名字
    while (GetLastError() == ERROR_ALREADY_EXISTS) {
      //AfxMessageBox("你已经打开了该程序!");
      //exit(0);
      //return;
      CloseHandle(m_hMutex);
      //m_hMutex =NULL;
      TRACE("ERROR_ALREADY_EXISTS\n");
      Sleep(1000);
      m_hMutex = ::CreateMutex(NULL, TRUE, "VIDGS_InfoCenter");//字符串里面的内容可以随便改.他只是一个名字
    }
    system_parameters para = Doc.readsystemdatas(".\\test.xml");
    string LASERCOM = para.LASERCOM;
    int LASERPAUD = para.LASERPAUD;
    laser_machine_parameters laser_para = Doc.readlaserparamters(".\\test.xml");
    forward_offset = laser_para.LaserForwardOffset;
    backward_offset = laser_para.LaserBackwardOffset;
    glb_sp_ptr = boost::make_shared<boost::asio::serial_port>(laser_iosev, LASERCOM);
    glb_sp_ptr->set_option(boost::asio::serial_port::baud_rate(LASERPAUD));
    glb_sp_ptr->set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::none));
    glb_sp_ptr->set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::odd));
    glb_sp_ptr->set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
    glb_sp_ptr->set_option(boost::asio::serial_port::character_size(8));
    laser_iosev_thread = glb_thread_group.create_thread(laser_iosev_run);
    if (glb_sp_ptr->is_open()) {
      OnCommandThread = glb_thread_group.create_thread(boost::bind(&CLaster::OnCommand, this));
      ComReadThread = glb_thread_group.create_thread(boost::bind(&CLaster::Com_Laser_Write, this));
      ComWriteThread = glb_thread_group.create_thread(boost::bind(&CLaster::Com_Laser_Read, this));
    }
    else {
      glb_sp_ptr = nullptr;
    }
    /*glb_thread_group.remove_thread(thread3);
    glb_thread_group.remove_thread(thread2);
    glb_thread_group.remove_thread(thread1);*/

    if (0) {//关闭红色激光
      Sleep(100);
      CString redorder;
      redorder.Format("43 07 00 01 00 ca");
      int len = String2Hex(redorder, senddata);
      if (senddata[0] == 0x43)frame_length = 2; else if (senddata[0] == 0x53)frame_length = 7; glb_sp_data.deliver2send(senddata, len);
      //LOG(INFO)<<"激光校准：发送关闭红色激光命令";
    }
    //MessageBox(NULL,TEXT("激光扫描初始化完成"),TEXT("激光扫描初始化完成"),MB_OK);
    //Doc.readalldata(".\\test.xml");
  }
  catch (std::exception& e) {
    //MessageBox(NULL,TEXT("激光扫描初始化失败！"));//,TEXT("激光扫描初始化失败！"),MB_OK);
    //return e.what();
    glb_sp_ptr = nullptr;
  }
  try {
    if (glb_led_obj_ptr == nullptr) {
      glb_led_obj_ptr = boost::make_shared<LED>();
      glb_thread_group.create_thread(boost::bind(&LED::LEDInit, glb_led_obj_ptr));
      int i = 0;
      do {
        this_thread::sleep(boost::posix_time::millisec(100));
        i++;
      } while (glb_led_ptr == nullptr && i < 10);
    }
    if (glb_led_obj_ptr != nullptr) glb_led_obj_ptr->sendmessage(0, 20, "", "NULL");
  }
  catch (std::exception& e) {
    glb_led_obj_ptr = nullptr;
    glb_led_ptr = nullptr;
    //MessageBox(NULL,TEXT("LED初始化失败！"));//,TEXT("激光扫描初始化失败！"),MB_OK);
    //return e.what();
  }

  try {
    if (glb_ptr_client == NULL) {
      bool TCPIPstarted = false;
      glb_thread_group.create_thread(boost::bind(&CLaster::InitTCPIPClient, this, &TCPIPstarted));
      int i = 0;
      while (!TCPIPstarted && i < 10) {
        this_thread::sleep(boost::posix_time::millisec(100));
        i++;
      }
    }
  }
  catch (std::exception& e) {
    //MessageBox(NULL,TEXT("TCPIP初始化失败！"));//,TEXT("激光扫描初始化失败！"),MB_OK);
    //return e.what();
  }
  return "SUCESS";
}

void CLaster::LaserDestory() {
  if (glb_led_ptr != NULL && glb_led_ptr->is_open()) glb_led_ptr->close();
  if (glb_sp_ptr != NULL && glb_sp_ptr->is_open()) glb_sp_ptr->close();
  laser_iosev.stop();
  tcpip_iosev.stop();
  led_iosev.stop();
  glb_thread_group.interrupt_all();
  glb_thread_group.join_all();
  glb_led_ptr = nullptr;
  glb_sp_ptr = nullptr;
  delete glb_ptr_client;
  //this->~CLaster();
  /*thread3->~thread();
  thread2->~thread();
  thread1->~thread();*/
}

int CLaster::unpackHead(unsigned char * laser_buf, int order_length) {
  //解析正确则返回下次要接收的长度
  int nextdata = (laser_buf[frame_length] * 256 + laser_buf[frame_length + 1]) * 2 + 1;
  if (laser_buf[0] == 0x43) {
    nextdata = (nextdata - 1) / 2 - 2;
  }
  data_length = data_length - order_length;

  if (data_length == nextdata) {
    return nextdata;
  }
  else
    return 0;//解析错误
}
int CLaster::unpackBody(unsigned char * laser_buf, int code_length) {
  //解析正确则返回下次要接收的长度
  CString charar1;
  int k;
  for (k = 0; k < code_length; k++) //将数组转换为Cstring型变量         
  {
    lacer_sum += laser_buf[k];
  }
  if (lacer_sum == 0) {
    return code_length;
  }
  else {
    lacer_sum = 0;
    return 0;//解析错误
  }

}

#define LASER_READDATA {readN = glb_sp_ptr->read_some(buffer(com_laser_buf+nTotalLen, commu_interface::max_onePack_length-nTotalLen), err);\
	nTotalLen += readN;leftByte+=readN;\
	boost::this_thread::interruption_point();\
	/*TRACE(" nTotalLen=%d, leftByte=%d, readN=%d", nTotalLen, leftByte, readN);TRACE("\n");//ws del trace 201410121610*/\
	int i;CString record,recorddata;record.Format("激光数据");\
	for (i=nTotalLen-readN;i<(nTotalLen<50?nTotalLen:50);i++)\
{recorddata.Format(" %02x",com_laser_buf[i]);record=record+recorddata;}\
	/*save_hrzCali_record(record);}*/\
	/*LOG(INFO)<<record;}*/\
/*TRACE(" %02x",com_laser_buf[i]);TRACE("\n");//ws del trace 201410121610 */}

void CLaster::Com_Laser_Read() //作为Laser的一个成员
{
  //测试启动线程用的代码
  int test_T2 = 3;
  while (TEST_THREAD) {
    test_T2 = test_T2 * 3; TRACE("(T2)=%d ", test_T2);
    Sleep(700);
  }
  //test_CRC16();
  int nNextLen = 1;
  size_t readN = 0;
  unsigned char com_laser_buf[commu_interface::max_onePack_length], com_laser_temp_trans_buf[commu_interface::max_onePack_length];
  int nTotalLen = 0;
  int Link_order = 0;
  int usedByte = 0;//统计已经用了多少字节
  int leftByte = 0;//还剩多少字节已经读入内存，但没有处理
  int tempUse;
  while (1) {
    boost::this_thread::interruption_point();
    //TRACE("\n      LASER   READ    WHILE\n");
    try {
      boost::system::error_code err;
      {
        //LOG(INFO)<<"LaserRead_offline0";
        while (leftByte == 0) LASER_READDATA

          if ((1 <= leftByte) && 0x53 == com_laser_buf[0]) {
            usedByte += 1;//已经判断了一个
            leftByte -= 1;

            nNextLen = 14;
            //num+=nNextLen;
            //TRACE("\nrecv head %x\n",com_laser_buf[0]);//nTotalLen]);
            //nTotalLen+=readN;
            data_order = usedByte/*nTotalLen*/ + nNextLen;


            while (nNextLen > nTotalLen - usedByte)	LASER_READDATA
              //readN = glb_sp_ptr->read_some(buffer(com_laser_buf+nTotalLen, nNextLen),err);
              //readN = boost::asio::read(*glb_sp_ptr, buffer(com_laser_buf+nTotalLen, nNextLen));
              //TRACE("\nrecv head %x\n",com_laser_buf[nTotalLen]);
              if (nNextLen <= leftByte && (nNextLen = unpackHead(com_laser_buf, usedByte/*nTotalLen*/ + nNextLen)))//readN)))//如果包头解析错误，则返回0
              {
                leftByte -= 14;
                usedByte += 14;
                //包头解析正确，
                //TRACE("\nrecv head unpack sucess %d\n",nNextLen);
                //nTotalLen+=readN;
                //readN = boost::asio::read(*glb_sp_ptr, buffer(com_laser_buf+nTotalLen, nNextLen));
                //readN =glb_sp_ptr->read_some(buffer(com_laser_buf+nTotalLen, nNextLen),err);
                while (nNextLen > nTotalLen - usedByte)	LASER_READDATA
                  data_length = usedByte + nNextLen;//原来data_length = nTotalLen+nNextLen;

                if (nNextLen <= leftByte)//==readN && (nNextLen=unpackBody(com_laser_buf,data_length))) //解析数据，如果出错，则返回0
                {
                  //TRACE("\nrecv body unpack sucess %d\n",nNextLen);
                  leftByte -= nNextLen;
                  usedByte += nNextLen;
                  if (tempUse = unpackBody(com_laser_buf, data_length)) {
                    glb_sp_data.deliver2Recv(com_laser_buf, data_length);
                  }
                  //nTotalLen+=readN;
                  //if(off_line_mode==0)
                  //glb_sp_data.deliver2Recv(com_laser_buf,nTotalLen);
                  //else 						
                }
              }

          }
          else if ((1 <= leftByte) && 0x5A == com_laser_buf[0])//【改】
          {
            usedByte += 1;//已经判断了一个
            leftByte -= 1;

            nNextLen = 14;

            data_order = usedByte/*nTotalLen*/ + nNextLen;


            while (nNextLen > nTotalLen - usedByte)	LASER_READDATA

              if (nNextLen <= leftByte && (nNextLen = unpackHead(com_laser_buf, usedByte/*nTotalLen*/ + nNextLen)))//readN)))//如果包头解析错误，则返回0
              {
                leftByte -= 14;
                usedByte += 14;

                while (nNextLen > nTotalLen - usedByte)	LASER_READDATA
                  data_length = usedByte + nNextLen;//原来data_length = nTotalLen+nNextLen;

                if (nNextLen <= leftByte)//==readN && (nNextLen=unpackBody(com_laser_buf,data_length))) //解析数据，如果出错，则返回0
                {
                  //TRACE("\nrecv body unpack sucess %d\n",nNextLen);
                  leftByte -= nNextLen;
                  usedByte += nNextLen;
                  if (tempUse = unpackBody(com_laser_buf, data_length)) {
                    glb_sp_data.deliver2Recv(com_laser_buf, data_length);
                  }

                }
              }

          }
          else if ((1 <= leftByte) && 0x43 == com_laser_buf[0]) {
            usedByte += 1;//已经判断了一个
            leftByte -= 1;

            nNextLen = 6;
            //nTotalLen+=readN;
            data_order = usedByte/*nTotalLen*/ + nNextLen;
            while (nNextLen > nTotalLen - usedByte)	LASER_READDATA
              //readN =glb_sp_ptr->read_some(buffer(com_laser_buf+nTotalLen, nNextLen),err);
              //readN = boost::asio::read(*glb_sp_ptr, buffer(com_laser_buf+nTotalLen, nNextLen));
              //TRACE("\nrecv head %d\n",readN);
              if (nNextLen <= leftByte && (nNextLen = unpackHead(com_laser_buf, usedByte + nNextLen)))//nTotalLen+readN)))//如果包头解析错误，则返回0
              {
                leftByte -= 6;
                usedByte += 6;//unpackHead要用6个
                //包头解析正确，
                //TRACE("\nrecv head unpack sucess %d\n",nNextLen);
                //nTotalLen+=readN;
                //readN = boost::asio::read(*glb_sp_ptr, buffer(com_laser_buf+nTotalLen, nNextLen));
                //readN =glb_sp_ptr->read_some(buffer(com_laser_buf+nTotalLen, nNextLen),err);
                while (nNextLen > nTotalLen - usedByte)	LASER_READDATA
                  data_length = usedByte + nNextLen;//原来data_length = nTotalLen+nNextLen;
                if (nNextLen <= leftByte)//==readN && (nNextLen=unpackBody(com_laser_buf,data_length))) //解析数据，如果出错，则返回0
                {
                  //TRACE("\nrecv body unpack sucess %d\n",nNextLen);
                  leftByte -= nNextLen;
                  usedByte += nNextLen;
                  //nTotalLen+=readN;
                  if (tempUse = unpackBody(com_laser_buf, data_length)) {
                    glb_sp_data.deliver2Recv(com_laser_buf, nTotalLen);
                  }
                }
              }
          }
          else if ((1 <= leftByte) && 0x42 == com_laser_buf[0]) {
            usedByte = 5;//已经判断了一个
            leftByte -= 5;
            glb_sp_data.deliver2Recv(com_laser_buf, 1);

          }
          else if ((1 <= leftByte) && 0x58 == com_laser_buf[0] && 0x11 == com_laser_buf[1] && 0x00 == com_laser_buf[2] && 0x00 == com_laser_buf[3]) {//ER01:校验和错误【改】
            usedByte = 5;//已经判断了一个这里最好写成红色激光的返回信息的长度，不知道是5个6个还是10个，不过只要能把第一位给略过去就行了
            leftByte -= 5;//必须与上一行的值相等
            //glb_sp_data.deliver2Recv(com_laser_buf,1);这一行应该是唤醒OnCommand线程的吧，不能要
            //MessageBox(NULL,TEXT("校验和错误"),TEXT("校验和错误"),MB_OK);
          }
          else if ((1 <= leftByte) && 0x58 == com_laser_buf[0] && 0x12 == com_laser_buf[1] && 0x00 == com_laser_buf[2] && 0x00 == com_laser_buf[3]) {//ER02:命令类型错误【改】
            usedByte = 5;//已经判断了一个这里最好写成红色激光的返回信息的长度，不知道是5个6个还是10个，不过只要能把第一位给略过去就行了
            leftByte -= 5;//必须与上一行的值相等
            //glb_sp_data.deliver2Recv(com_laser_buf,1);这一行应该是唤醒OnCommand线程的吧，不能要
            //MessageBox(NULL,TEXT("命令类型错误"),TEXT("命令类型错误"),MB_OK);
          }
          else if ((1 <= leftByte) && 0x58 == com_laser_buf[0] && 0x13 == com_laser_buf[1] && 0x00 == com_laser_buf[2] && 0x00 == com_laser_buf[3]) {//ER03:参数错误【改】
            usedByte = 5;//已经判断了一个这里最好写成红色激光的返回信息的长度，不知道是5个6个还是10个，不过只要能把第一位给略过去就行了
            leftByte -= 5;//必须与上一行的值相等
            //glb_sp_data.deliver2Recv(com_laser_buf,1);这一行应该是唤醒OnCommand线程的吧，不能要
            //MessageBox(NULL,TEXT("参数错误"),TEXT("参数错误"),MB_OK);
          }
          else if ((1 <= leftByte) && 0x58 == com_laser_buf[0] && 0x14 == com_laser_buf[1] && 0x00 == com_laser_buf[2] && 0x00 == com_laser_buf[3]) {//ER04:IPC通信中断【改】
            usedByte = 5;//已经判断了一个这里最好写成红色激光的返回信息的长度，不知道是5个6个还是10个，不过只要能把第一位给略过去就行了
            leftByte -= 5;//必须与上一行的值相等
            //glb_sp_data.deliver2Recv(com_laser_buf,1);这一行应该是唤醒OnCommand线程的吧，不能要
            //ICP_Error=true;
            //MessageBox(NULL,TEXT("IPC通信中断"),TEXT("IPC通信中断"),MB_OK);
          }
          else if ((1 <= leftByte) && 0x58 == com_laser_buf[0] && 0x15 == com_laser_buf[1] && 0x00 == com_laser_buf[2] && 0x00 == com_laser_buf[3]) {//ER05:LDM通信中断【改】
            usedByte = 5;//已经判断了一个这里最好写成红色激光的返回信息的长度，不知道是5个6个还是10个，不过只要能把第一位给略过去就行了
            leftByte -= 5;//必须与上一行的值相等
            //glb_sp_data.deliver2Recv(com_laser_buf,1);这一行应该是唤醒OnCommand线程的吧，不能要
            //MessageBox(NULL,TEXT("LDM通信中断"),TEXT("LDM通信中断"),MB_OK);
          }
          else if ((1 <= leftByte) && 0x58 == com_laser_buf[0] && 0x16 == com_laser_buf[1] && 0x00 == com_laser_buf[2] && 0x00 == com_laser_buf[3]) {//ER06:FPGA通信中断【改】
            usedByte = 5;//已经判断了一个这里最好写成红色激光的返回信息的长度，不知道是5个6个还是10个，不过只要能把第一位给略过去就行了
            leftByte -= 5;//必须与上一行的值相等
            //glb_sp_data.deliver2Recv(com_laser_buf,1);这一行应该是唤醒OnCommand线程的吧，不能要
            //MessageBox(NULL,TEXT("FPGA通信中断"),TEXT("FPGA通信中断"),MB_OK);
          }
      }

      //nNextLen=1;
      //nTotalLen=0;
      //TRACE("doen:l:%d,left:%d,used:%d :",nTotalLen,leftByte,usedByte);
      if (usedByte != 0)//这个包正常完毕
      {
        if (leftByte > 0) {
          memcpy(com_laser_temp_trans_buf, com_laser_buf + usedByte, leftByte);
          memcpy(com_laser_buf, com_laser_temp_trans_buf, leftByte);
        }
        nTotalLen = leftByte;
        usedByte = 0;
      }
      else if (usedByte == 0)//第一个字节错
      {
        if (leftByte - 1 > 0) {
          memcpy(com_laser_temp_trans_buf, com_laser_buf + 1, leftByte - 1);
          memcpy(com_laser_buf, com_laser_temp_trans_buf, leftByte - 1);
        }
        nTotalLen = leftByte - 1;
        leftByte -= 1;
        usedByte = 0;
      }
      else if (usedByte < 0)//程序逻辑严重错误
      {
        //LOG(INFO)<<"LaserRead:usedByte="<<usedByte<<endl;
        usedByte = 0;
        nTotalLen = 0;
        leftByte = 0;
      }
      //包头解析
      //收数据 readN = boost::asio::read(*glb_sp_ptr,buffer(com_LED_buf+packHead,nNextLen));
      //校验数据正确性 如果数据不正确则丢弃，
      //如果数据正确，则放到数据接收链表上并通知处理线程 glb_sp_data.deliver2recv(send_test_data,10);
      //如果正确则放在接收数据中

    }
    catch (std::exception& e) {
      //LOG(WARNING)<<"Laser_Read失败";
      TRACE("\nException: %s\n", e.what());
    }
  }
}

void CLaster::Com_Laser_Write() {
  int test_T3 = 5;
  while (TEST_THREAD) {
    test_T3 = test_T3 * 2; TRACE("(T3)=%d ", test_T3);
    Sleep(900);
  }
  ptrMsgData curSend = NULL;
  int sent_N = 0, curSent = 0;
  boost::system::error_code err;
  while (1) {
    boost::this_thread::interruption_point();
    {
      mutex::scoped_lock lock1(glb_sp_data.sendMu);
      glb_sp_data.con_send.wait(glb_sp_data.sendMu);
    }
    while (!stopped&&NULL != (curSend = glb_sp_data.getFromSendList())) {
      try {
        sent_N = 0;
        while (sent_N < curSend->lenData) {
          curSent = glb_sp_ptr->write_some(buffer(curSend->msgData + sent_N, curSend->lenData - sent_N), err);
          sent_N += curSent;
          boost::this_thread::interruption_point();
        }
        //LOG(INFO)<<"激光校准：send"<<curSend->msgData<<endl;
        //std::string t = to_iso_string(boost::posix_time::microsec_clock::local_time())+"--- "+"---serial1 send over\n\0";
        //TRACE("%s",t.c_str());
        //curSend=NULL;
      }
      catch (std::exception& e) {
        //LOG(WARNING)<<"Laser_Wright失败";
        TRACE("\nException: %s\n", e.what());
      }
    }

  }

}


void CLaster::Sum_Check(int len, unsigned char senddata[]) {
  int i;
  char checksum;
  checksum = 0;
  for (i = 0; i < len - 1; i++) {
    checksum += senddata[i];
  }
  checksum = checksum ^ 0xff;
  checksum++;
  senddata[len - 1] = checksum;
}

void CLaster::OnBnClickedButtonOffLineTest() {
  OfflineText();
  // TODO: 在此添加控件通知处理程序代码
}
void CLaster::OnBnClickedButtonGuideLineMove() {
  laser_parameters lasers;
  CString str;
  CString tempCString;
  for (int i = 49; i <= 53; i++) {
    for (int j = 49; j <= 70; j++) {

      tempCString = "guide";
      linenum.str_guidenum.Format(_T("%c"), i);
      tempCString += linenum.str_guidenum;
      Doc.guaidlinename = tempCString.GetBuffer();
      tempCString = "stop";
      linenum.str_stopnum.Format(_T("%c"), j);
      tempCString += linenum.str_stopnum;
      Doc.stoplinename = tempCString.GetBuffer();
      laser_parameters lp = Doc.readlaserdatas(".\\test.xml", Doc.guaidlinename, Doc.stoplinename);
      laser_machine_parameters lmp = Doc.readlaserparamters(".\\test.xml");
      if ((lp.guide_line_y < 800000 && lp.guide_line_y >= 0) && abs(lp.guide_line_x) < 800000) {
        guide_line_k = lp.guide_line_k;
        guide_line_b = lp.guide_line_b;
        guide_line_x = lp.guide_line_x;
        guide_line_y = lp.guide_line_y;
        install_high = lmp.install_high;
        object_dis = lp.object_dis;
        HrzConfirmSteps = lmp.HrzConfirmSteps;
        VertConfirmSteps = lmp.VertConfirmSteps;
        if (guide_line_k != 0)
          guide_line_x = float(guide_line_y - guide_line_b) / guide_line_k;

        guide_line_x += offsetforline;
        guide_line_b = guide_line_y - guide_line_x * guide_line_k;

        str.Format("%s%s%s", "Laser", this->linenum.str_guidenum, this->linenum.str_stopnum);
        laser_machine_parameters lmp = Doc.readlaserparamters(".\\test.xml");
        String Str = LPCTSTR(str);
        lasers.guideline_stopline_name = Str;
        lasers.object_dis = this->object_dis;
        lasers.guide_line_k = this->guide_line_k;
        lasers.guide_line_b = this->guide_line_b;
        lasers.guide_line_x = this->guide_line_x;
        lasers.guide_line_y = this->guide_line_y;
        Doc.writedata(routename, Str, lasers);
      }
      else {
      }
      if (j == 57) j = 64;
    }
  }
}
void CLaster::OnBnClickedButtonStopLineMoveSingle() {

}
void CLaster::OnBnClickedButtonStopLineMove() {
  laser_parameters lasers;
  CString str;
  CString tempCString;
  for (int i = 49; i <= 53; i++) {
    for (int j = 49; j <= 70; j++) {

      tempCString = "guide";
      linenum.str_guidenum.Format(_T("%c"), i);
      tempCString += linenum.str_guidenum;
      Doc.guaidlinename = tempCString.GetBuffer();
      tempCString = "stop";
      linenum.str_stopnum.Format(_T("%c"), j);
      tempCString += linenum.str_stopnum;
      Doc.stoplinename = tempCString.GetBuffer();
      laser_parameters lp = Doc.readlaserdatas(".\\test.xml", Doc.guaidlinename, Doc.stoplinename);
      laser_machine_parameters lmp = Doc.readlaserparamters(".\\test.xml");
      if ((lp.guide_line_y < 800000 && lp.guide_line_y >= 0) && abs(lp.guide_line_x) < 800000 && abs(lp.guide_line_b) < 800000) {
        guide_line_k = lp.guide_line_k;
        guide_line_b = lp.guide_line_b;
        guide_line_x = lp.guide_line_x;
        guide_line_y = lp.guide_line_y;
        install_high = lmp.install_high;
        object_dis = lp.object_dis;
        HrzConfirmSteps = lmp.HrzConfirmSteps;
        VertConfirmSteps = lmp.VertConfirmSteps;
        if (guide_line_k != 0)
          guide_line_x = float(guide_line_y - guide_line_b) / guide_line_k;

        guide_line_y += offsetforline * guide_line_k / sqrt(1 + pow(guide_line_k, 2));
        guide_line_x += offsetforline / sqrt(1 + pow(guide_line_k, 2));

        str.Format("%s%s%s", "Laser", this->linenum.str_guidenum, this->linenum.str_stopnum);
        String Str = LPCTSTR(str);
        lasers.guideline_stopline_name = Str;
        //lasers.install_high=this->install_high;
        lasers.object_dis = this->object_dis;
        lasers.guide_line_k = this->guide_line_k;
        lasers.guide_line_b = this->guide_line_b;
        lasers.guide_line_x = this->guide_line_x;
        lasers.guide_line_y = this->guide_line_y;
        //   lasers.HrzConfirmSteps=this->HrzConfirmSteps;
        //   lasers.VertConfirmSteps=this->VertConfirmSteps;
        //   lasers.HrzOffsetAngle = this->Edit_x_Offset;
         //lasers.HrzConfirmProbability = this->HrzConfirmProbability;
         //lasers.VertConfirmProbability = this->VertConfirmProbability;
        Doc.writedata(routename, Str, lasers);
      }
      else {
      }
      if (j == 57) j = 64;
    }
  }
}
void CLaster::offline_row() {
  CString Temp_txt;
  Temp_txt = "";
  int start_num = 0;
  int j = 0;
  if (offline_bWorking) {
    offline_bWorking = offline_finder.FindNextFile();
    if (!offline_finder.IsDirectory()) {
      offline_file_name = offline_finder.GetFilePath();
      // 此处 str 即楼主要的txt文件路径
    }

  }
  else {
    offline_finder.Close();
    offline_mode = 0;
    return;
  }
  CFile file;
  file.Open(offline_file_name, CFile::modeReadWrite);
  int file_length = file.GetLength();
  file.Read(offline_buff, file_length);

  data_h = 0;
  for (int i = 2; i < file_length; i++) {
    if (offline_buff[i - 2] == 13 && offline_buff[i - 1] == 10 && offline_buff[i] > 47 && offline_buff[i] < 58) {
      start_num++;
    }
    if (start_num == 1) {
      j = i;
      if (offline_buff[j] != 9) {
        Temp_txt += offline_buff[j];
      }
      else {
        save_data[data_h] = atoi(Temp_txt);
        data_h++;
        Temp_txt = "";
      }
    }
    if (start_num == 2) {
      j = i;
      if (offline_buff[j] != 9) {
        Temp_txt += offline_buff[j];
      }
      else {
        save_data[data_h] = atoi(Temp_txt);
        data_h++;
        Temp_txt = "";
      }
    }
  }
  Temp_txt = "";

  int  temp_num = 0;

  CString temp_c = "";
  for (int k = 0; k < offline_file_name.GetLength(); k++) {
    if (offline_file_name[k] > 47 && offline_file_name[k] < 58) {
      temp_c += offline_file_name[k];
      temp_num++;
      if (temp_num > 4) {
        temp_num = atoi(temp_c);
        itoa(temp_num, offline_chararr, 10);
        Temp_txt += offline_chararr;
        temp_c = "";
      }
    }
    else {
      if (offline_file_name[k] != '.') {
        temp_c = "";
        temp_num = 0;
      }
      else {
        temp_num = atoi(temp_c);
        Edit_StandHigh = -temp_num;
      }
      Temp_txt += offline_file_name[k];
    }
    /*else
    {
    if (temp_num>0&&temp_num<5)
    {
    temp_num=atoi(temp_c);

    if (offline_file_name[k]!='.')
    {
    temp_num++;
    itoa(temp_num,offline_chararr,10);
    Temp_txt += offline_chararr;
    }
    else
    {
    Edit_StandHigh=-temp_num;
    }
    }
    else
    {
    temp_c="";
    temp_num=0;
    }
    Temp_txt+=offline_file_name[k];
    }*/
  }
  Sleep(OFFLINE_SLEEP_TIME);
  /*if(detectBridgeMode==0)
  GuidePlane();
  else if(detectBridgeMode==1)
  DetectBridge();*/
  //file_name=Temp_txt;
}
void CLaster::OpenLaserCOM() {
  try {
    system_parameters para = Doc.readsystemdatas(".\\test.xml");
    string LASERCOM = para.LASERCOM;
    int LASERPAUD = para.LASERPAUD;
    laser_machine_parameters laser_para = Doc.readlaserparamters(".\\test.xml");
    forward_offset = laser_para.LaserForwardOffset;
    backward_offset = laser_para.LaserBackwardOffset;
    glb_sp_ptr = boost::make_shared<boost::asio::serial_port>(laser_iosev, LASERCOM);
    glb_sp_ptr->set_option(boost::asio::serial_port::baud_rate(LASERPAUD));
    glb_sp_ptr->set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::none));
    glb_sp_ptr->set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::odd));
    glb_sp_ptr->set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
    glb_sp_ptr->set_option(boost::asio::serial_port::character_size(8));
    bool boost_on = glb_sp_ptr->is_open();
    if (boost_on) {
      OnCommandThread = glb_thread_group.create_thread(boost::bind(&CLaster::OnCommand, this));
      ComReadThread = glb_thread_group.create_thread(boost::bind(&CLaster::Com_Laser_Write, this));
      ComWriteThread = glb_thread_group.create_thread(boost::bind(&CLaster::Com_Laser_Read, this));
    }
    else {
      glb_sp_ptr = nullptr;
    }
    glb_plane_type = "CONF";
    glb_plane_position = para.StationNumber;
  }
  catch (std::exception e) {

  }

  //if(glb_led_obj_ptr == nullptr)
  //{
  //	glb_led_obj_ptr = boost::make_shared<LED>();
  //	glb_led_obj_ptr->LEDInit();
  //	glb_led_obj_ptr->sendmessage(0, 20, "", "NULL");
  //}
  //if(glb_ptr_client == NULL)
  //{
  //	bool TCPIPstarted = false;
  //	glb_thread_group.create_thread(boost::bind(&CLaster::InitTCPIPClient,this,&TCPIPstarted));
  //	while (!TCPIPstarted)
  //	{
  //		this_thread::sleep(boost::posix_time::millisec(100));
  //	}
  //}
}
void CLaster::CloseLaserCOM() {
  try {
    if (glb_sp_ptr != nullptr && glb_sp_ptr->is_open()) glb_sp_ptr->close();

    if (ComWriteThread != nullptr) {
      ComWriteThread->interrupt();
      ComWriteThread->join();
      //glb_thread_group.remove_thread(ComWriteThread);
    }
    if (ComReadThread != nullptr) {
      ComReadThread->interrupt();
      ComReadThread->join();
      //glb_thread_group.remove_thread(ComReadThread);
    }
    if (OnCommandThread != nullptr) {
      OnCommandThread->interrupt();
      OnCommandThread->join();
      //glb_thread_group.remove_thread(OnCommandThread);
    }

    glb_sp_ptr = nullptr;
    //ComWriteThread->join();
    //ComReadThread->join();
  }
  catch (std::exception e) {

  }
}


void CLaster::OfflineText() {
  // TODO: 在此添加控件通知处理程序代码
  /*top_order[0]=0;
  top_order[1]=0;*/
  /*CallPropSheet* Window=NULL;
  Window=(CallPropSheet*)AfxGetApp()->m_pMainWnd;
  glb_VIDGS_system.write_laser_status_type(VIDGS_system::laser_capturing);
  if(Window&&(Window->m_VisuPage.GetSafeHwnd()))
  {
  Window->m_VisuPage.PostMessage(WM_SHOWMESSAGE,(WPARAM)6,0);
  }*/
  //CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"TXT Files (*.txt)|*.txt|All Files (*.*)|*.*||",NULL);
  //if(dlg.DoModal() == IDOK)	
  //{
  //	file_name = dlg.GetPathName();			//得到文件名
  //}
  //UpdateData(TRUE);

  //Laser str_file_name;//SCommTestDlg str_file_name;
  //offline_file_name = str_file_name.offlineGetDirectory();
  offline_file_name = /*glb_laser_ptr->*/offlineGetDirectory();
  offline_mode = TRUE;
  time_start = time_end = time_fin = 0;
  stand_dis_end = stand_dis_start = 0;
  object_speed = 0;
  direction_num = 0;
  smooth = 0;
  //avg_order = FALSE;
  scan_mode = HORIZONTAL;
  save_mode = HORIZONTAL;
  precise_y = precise_dis = 0;

  guide_plane = TRUE;
  loop = 0;
  //Guide_Mode = "";


  CString change;

  short int sx, ex, sy, ey;
  data_h = 0;
  data_order = 16;
  line_num = 0;
  positive = 6;
  next_frame = 5;
  walksetup = 0;
  //	stan_catch = 0;
  //	auto_catch = 0;
  walktext = 0;
  scanyard = 0;
  scanyardToAdjust = 0;

  save_start_x.Format("%d", Edit_Start_x);
  save_end_x.Format("%d", Edit_End_x);
  save_start_y.Format("%d", Edit_Start_y);
  save_end_y.Format("%d", Edit_End_y);
  sx = Edit_Start_x / XSTEPANGLE;
  ex = Edit_End_x / XSTEPANGLE;
  sy = Edit_Start_y / YSTEPANGLE;
  ey = Edit_End_y / YSTEPANGLE;
  int standhigh = Edit_StandHigh / 2;
  int standheight;
  sx = sx / 2 * 2 * presacl;
  ex = ex / 2 * 2 * presacl;
  sy = sy / 2 * 2 * presacl;
  ey = ey / 2 * 2 * presacl;
  if (presacl != 0) {
    standheight = standhigh * 2 * presacl;
    data_length = abs(ex - sx) + data_order + 2;
  }
  else {
    standheight = Edit_StandHigh;
    data_length = abs(ex - sx) * 2 + data_order + 2;
  }
  offline_bWorking = offline_finder.FindFile(offline_file_name);
  //offline_row();
  //if(offline_mode==1)
  {
    //LOG(INFO)<<"LaserRead_offline1";
    char* aaa = "aa";
    unsigned char *ccc = (unsigned char*)aaa;
    glb_sp_data.deliver2Recv(ccc, 2);
    //readN = boost::asio::read(*glb_sp_ptr, buffer(com_laser_buf+nTotalLen, nNextLen));
  }
}
CString CLaster::offlineGetDirectory() {
  BROWSEINFO bi;
  char name[MAX_PATH];
  ZeroMemory(&bi, sizeof(BROWSEINFO));
  bi.hwndOwner = AfxGetMainWnd()->GetSafeHwnd();
  bi.pszDisplayName = name;
  bi.lpszTitle = "选择文件夹目录";
  bi.ulFlags = BIF_RETURNFSANCESTORS;
  LPITEMIDLIST idl = SHBrowseForFolder(&bi);
  if (idl == NULL)
    return "";
  CString strDirectoryPath;
  SHGetPathFromIDList(idl, strDirectoryPath.GetBuffer(MAX_PATH));
  strDirectoryPath.ReleaseBuffer();
  if (strDirectoryPath.IsEmpty())
    return "";
  if (strDirectoryPath.Right(1) != "\\")
    strDirectoryPath += "\\*.txt";

  return strDirectoryPath;
}

void CLaster::stepMoveNoSet0(int stepNum, unsigned char direction, unsigned char type) {
  CString fix, sendSteps;
  unsigned short tempSteps;
  int len = 0;

  if (direction == LEFT || direction == DOWN)
    tempSteps = (unsigned short)(0 - stepNum);
  else
    tempSteps = stepNum;

  sendSteps.Format("%04x", tempSteps);

  if (type == HORIZONTAL)
    fix.Format("42 01 00 09 %c%c %c%c 07 d0 00 00 07 d0 00 ca",
      sendSteps[0], sendSteps[1], sendSteps[2], sendSteps[3]);
  else if (type == VERTICAL)
    fix.Format("42 01 00 09 00 00 07 d0 %c%c %c%c 07 d0 00 ca",
      sendSteps[0], sendSteps[1], sendSteps[2], sendSteps[3]);

  len = String2Hex(fix, senddata);
  Sum_Check(len, senddata);
  glb_sp_data.deliver2send(senddata, len);
}

void CLaster::InitTCPIPClient(bool* running) {
  system_parameters para = Doc.readsystemdatas(".\\test.xml");
  string server_ip = para.ServerAddress;//=glb_doc.readalonestring(PARAMETER_FILENAME, "ServerAdress");
  string server_port = para.ServerPort;//=glb_doc.readalonestring(PARAMETER_FILENAME, "ServerPort");
  glb_plane_position = para.StationNumber;
  //glb_ptr_client = NULL;
  //VIDGS_client client(glb_iosev, server_ip, server_port);
  glb_ptr_client = new VIDGS_client(tcpip_iosev, server_ip, server_port);
  *running = true;
  tcpip_iosev.run();
}