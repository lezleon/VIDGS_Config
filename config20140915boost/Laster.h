#pragma once
//#include "Paramini.h"
//#include "mscomm1.h"
#include "cxcore.h"
#include "resource.h"
#include"Doc.h"
#include"LineNum.h"
#include "TCPIPclient.h"
using namespace cv;  
using namespace std; 

//#define  RECOG_EXPORTS
//#ifdef RECOG_EXPORTS
//#define RECOG_API extern "C" __declspec(dllexport)
//#else
//#define RECOG_API extern "C" __declspec(dllimport)
//#endif

//***	水平电机转动设置——direction参数	*********
#define  LEFT			1
#define  RIGHT			2
#define  UP				1
#define  DOWN			2
//****	跳转函数表设定——funciton参数		*********
#define  JPCATCH		1		
#define	 JPVERTCALIB	2		//挑准到垂直标定处理函数
#define  JPHRZCALIB		3		//跳转到水平标定处理函数
#define  JPAVGPRECISE	4		//跳转到精确测距处理函数
#define  JPWALKSETUP	5		//跳转到走步设置处理函数
#define  JPWALKTEST		6		//跳转到走步测试处理函数
#define  JPAVG			7		//跳转到AVG处理函数
#define  JPHRZMOVE		8		//跳转到移位--水平扫描处理
#define  JPCONFIRMEDGE	9		//跳转到确定边界处理函数
#define  JPPREPAREMEASSURE 10	//确定20m

//****	是否置位置参数设定——SETUP参数		************
#define  SETPOSITION	1		//置  位
#define  UNSETPOSITION	0		//不置位

//***	当前action()函数执行方式——type参数	***********
#define  HRZSCANNING	1		//当前执行水平扫描动作
#define  VERTSCANNING	2		//当前执行处置扫描动作
#define	 FIXPOINT		3		//当前执行普通测距动作，用于校准功能
#define	 HRZMOTORMOVE	4		//当前执行水平电机定位功能
#define  VERTMOTORMOVE	5		//当前执行垂直定位功能
#define  RESET			6		//复位功能
#define  SETZERO		7		//置位功能
#define  MEASPRICISE	8		//精确测距功能，测2W个距离值求均值

#define NUM_NEED_REFRESH 4//共有4组数据需要刷新界面
#define VERIFY_HORIZONTAL_MAX_NUM 20//使用的水平扫描行数最大值
#define VERIFY_VERTICAL_MAX_NUM 10//使用的垂直扫描列数最大值

struct needTransform1_2//1常规，2引导线，4走步测试坐标
{
public:
	int install_high;//安装高度
	int object_high;//对象高度
	int object_dis;//对象距离
	float Edit_Start_x;//+-6度
	float Edit_End_x;
	int Edit_StandHigh;//垂直步数
	int Auto_StandHigh;//水平步数
	int mid_num;//单点测距
	int guide_line_y;//引导线y
	//CString m_strRXData;//显示状态
	//CString walktext_deflection;//显示详细情况
	char m_strRXData[50];//显示状态
	char walktext_deflection[200];//显示详细情况
	//CString PlaneType4Confirm;
	//CString LineNum4Confirm;
	char PlaneType4Confirm[20];//机型读取引导线
	char LineNum4Confirm[20];//编号读取引导线

	float guide_line_k; //引导线直线方程y=kx+b;
	float guide_line_b;
	int guide_line_x;

	int x_walk;//走步测试的位置，只上行
	int y_walk;

	char str_guidenum;//引导线编号，由C#中的弹窗得到，只下行。需要初始化为'\0'
	char str_stopnum;
	float offsetforline;//引导线停止线移动偏移量
};

struct needTransform3//二维扫描数据，只上行
{	
	int data_h;//点数
	int save_data[250];
};

//struct needTransform4//走步测试的位置，只上行
//{
//	int x;
//	int y;
//};


// CLaster 对话框

class CLaster : public CDialog
{
	DECLARE_DYNAMIC(CLaster)

public:
	CLaster(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLaster();

	

// 对话框数据
	enum { IDD = IDD_Laster };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int save_data[100000];
	//CMscomm1 m_ctrlComm;
	CString m_strRXData;
	CString m_strTXData;
	CString format,start_x,end_x,start_y,end_y,Stand_High,up_y;
	CString Edit_format;
	float Edit_Start_x,Edit_End_x,Edit_Start_y,Edit_End_y; //读入水平和垂直的起始与结束角度
	float Edit_x_Offset;
	int forward_offset;
	int backward_offset;
	short int Edit_StandHigh; //水平扫描垂直步数
	short int Auto_StandHigh; //水平扫描垂直步数
	int object_high;//对象高度
	CString catch_plane; //显示捕获状态
	CString End_order;   //输入命令的后5个指令
	CString walktext_deflection; //显示走步测试偏转情况
	int install_high;   //输入安装高度
	int walksetup;      //判断是否进行走步设置
	int walktext;       //判断是否进行走步测试
	int scanyard;   //判断是否进行站坪扫描
	int scanyardToAdjust;   //判断是否进行站坪扫描然后校准
	int loop;
	unsigned char sum;
	CString lo,ca,te;   //存储目录有
	CString D_Plan_Time,time_str;//存储站坪扫描
	//CString format;       //输入命令总类
	CString reset_order;
	CString save_start_x,save_start_y;
	CString save_end_x,save_end_y;
	int data_length;      //接收数据长度
	int data_order;       //返回指令长度
	int data_h;           //扫描数据点数
	int NUM;

	int Calib_fuction;   
	int calib_num;   
	int redlight;    //判断红色激光是否工作
	int line;        //扫描行数
	int positive;    //判断正向
	CString Edit_line;
	int line_num;
	int page;        //扫描幅数
	int ci;          //校准点步数
	int next_frame;   //是否有后续数据帧
	int stan_catch;   //判断是否水平捕获
	int auto_catch;   //判断是否进行自动捕获
	int auto_num;     //自动捕获次数
	int mid_num;  //中点距离
	int avg_order;
	int start_height;     //起始扫描垂直步数
	int object_dis;//对象到激光器的距离，机场中为120m左右
	int object_width;
	float guide_line_k; //***********************引导线直线方程y=kx+b;************
	float guide_line_b;
	float offsetforline;
	int guide_line_y; //停止线到激光器零点坐标的垂直距离
	int guide_line_x; //停止线到激光器零点坐标的水平距离
	int stop_lp;        //停止循环
	int walksetup_num; //走步设置数据点数
	int stip_h; //垂直镜子单次移动步数
	BOOL laster_flag;
	int scan_mode;  //扫描方式，分为水平扫描和垂直扫描
	int judge_direct;   //存储当前垂直镜子步进位置
	float first_long;   //单点测距第一次的平均数据
	int temp;             //校准用的选项
	//***********站坪扫描初始化****************	
	int DetectThing_SaveModle;
	int Detect_Thing_Init;
	int length_data;//一行数据点的长度
	vector<vector<int> >Y_All;
	vector<vector<int> >Z_All;
	vector<int>obstacle_dis;
	int DtcTEdit_StandHigh_min;
	int NUM_NEW;
	int Scan_Cate;//3为S扫描，0为己字扫描
	int Up_Y_Step;
	int yard_scan_range;//站坪扫描范围（1，2，3，4，5）
	int yard_scan_obstacle_high;//站坪扫描障碍物的高度
	int yard_scan_obstacle_width;//站坪扫描障碍物的宽度
	int Edit_StandHigh_last;//窗口扫描和检测登机桥用
	int Edit_StandHigh_Org;//Org为全局变量，org为局部变量	
	float Edit_Start_x_Org;
	float Edit_End_x_Org;
	float adjustAlpha_ySum;//用于站坪扫描后的校准
	int adjustTimes;//用于站坪扫描后的校准
	int adjustEdit_StandHigh;//用于站坪扫描后的校准，正数为往上抬的步数
	int z_temp;//用于计算站坪平均高度
	//**********  初始化设置 ***********
	int divid_x; //x细分
	int divid_y; //y细分
	int presacl; //分频数
	int insert_num;//内插值
	int expan;  //额外点数
//)
	//
	double time_start,time_end,time_fin; //记录起始时间，结束时间和过程时间
    float stand_dis_start,stand_dis_end; //用于计算对象速度的起始距离和结束距离
	int smooth;
	int save_mode;   //存储模式
	float precise_y;//精确定位机鼻y
    float precise_dis;//精确定位垂直镜子移动一步的距离
	float object_speed;     //对象速度
	int guide_plane;   //判断是否进行飞机引导
	int direction_num;  //平滑数据

	Document Doc;
	CLineNum linenum;
	string routename;

//*******************************************************************************
//******************** for calibration ******
	unsigned char	ForHrzCalib;	//水平标定处理函数的参数
	unsigned char	OrdDistance;	//普通测距参数，主要用于校准功能内
	unsigned char	distPrcs;		//精确测距参数
	unsigned char	HrzScnProc;		//水平扫描处理函数

	unsigned char	isBalance;		//验证水平零点是否居中
	unsigned int	distArr[3];		//距离值，distArr[0]--最左边十个距离值和  distArr[1]--中间十个距离值和 disArr[2]最右十个距离值和
	unsigned int	distancePrecese;//精确测距得到的距离
	unsigned int	DistMin;		//最小值，这个值用来和distanccePreces比较
	unsigned char	NumberMove;		//从左向右比较十次，每次NumberMove自加	
	unsigned short  cntSteps ;		//记录中心的大概区域距离边缘有多少步
	
	unsigned char	ResetOrNot;		//step2 复位 然后偏移cntsteps步到大概的中心位置
	unsigned char	jmpAbtCenter;	//是否偏移，TRUE=偏移后 False=便宜前

	unsigned char	microMove;		//微步测距，没1步测量一次
	unsigned char	microMoveNum;	//同NumberMove
	unsigned int	microMin;		//微步测距最小值，和distancePrecese比较
	unsigned short	microEqual;		//若MicroMin = distancePrecese则microEqual自加
	unsigned short	microCntSteps;	//同CntSteps，记录精确的零点偏移有多少步
		
	unsigned short	inSideWindow;	//落在校准窗边缘内的点数
	unsigned char	HrzEdgeConfirm;	//判断是否确定好水平边界
	unsigned char	VertEdgeConfirm;//判断是否确定好垂直边界	
	unsigned short	LastInside;		//上一次落入校准窗内的点数
	unsigned char	ConfirmFunct;
	short	HrzConfirmSteps;
	short	VertConfirmSteps;

	unsigned char	HrzDetect;
	unsigned char	VertDetect;
	unsigned short TwoDimenScan;
	
//*****************	 校准	***************************/
	void	actions(int direction, int StepsSrce, int stepsDst, int SetZero, int type, int jumpFunction); 
	void    stepMove(int stepNum, unsigned char direction, unsigned char type);//移动后至为零点
	void	hrzScanningForCalib(); 
	void	AVGprecise();
	void	normalMeasure();
	void	HrzProFunct();
	void	ConfirmEdge();
	void	CalibHriz();
//*****************	 自动寻找地面点	***************************/
	bool prepare_measure;//当为TRUE时，进行标定前的20m基准数据的确定
	bool  prepare_complete;//20m的距离的寻找完成
	int save_hrzCali_record(const char *content="D:\\plane\\D\\LR.txt");//保存水平标定记录
	//*****************	 验证停止点	***************************/
	int comfirm_type;
	//string planeType4Confirm;
	//string planeType;
	CString PlaneType4Confirm;
	CString LineNum4Confirm;
	int scan_all;
	int countNearPoint[2];
	//*****************	 dll	***************************/
	int refleshFlag[NUM_NEED_REFRESH];//可能不用了
	//赋值：0不需刷新，1刷新并跳出，2刷新并继续刷新，-1等待。标号：1常规界面，2引导线，3二维点，4走步测试坐标******需初始化为0。
	int refleshFlag1,refleshFlag2,refleshFlag3,refleshFlag4;
	int *refleshFlag1pointer,*refleshFlag2pointer,*refleshFlag3pointer,*refleshFlag4pointer;//与C#共用内存，安全起见，类构造的时候将这些指针赋地址
	int x_walk,y_walk;//用以传递走步测试坐标
	int countWalkSetCount;//记录走不设置中得到的点数，用于自动停止
	////*****************	 boost	***************************/
	//boost::thread_group glb_thread_group;
	//boost::thread *thread1, *thread2, *thread3;
	boost::posix_time::ptime Laser_clock;
	unsigned char senddata[100];//用于发送16进制指令
	unsigned char lacer_sum;  //校验和
	bool stopped;
	int frame_length;
	int String2Hex(CString str,unsigned char senddata[]); //将字符串指令转换成16进制指令【重载，来自VIDGS】
	string LaserInit();
	void LaserDestory();
	void OnCommand();
	int unpackHead(unsigned char * laser_buf,int order_length);//解析包头
	int unpackBody(unsigned char * laser_buf,int code_length);//解析主体
	void Com_Laser_Read(); //串口读
	void Com_Laser_Write(); //串口写
	void Sum_Check(int len,unsigned char senddata[]);
//*****************	 画图	***************************/
	CWnd* pDrawWnd; //表示控件窗口的变量 
	CDC* pDrawControlDC; //表示控件窗口的设备描述表.DC 
	CRect DrawRct; 
	//int draw;//用于绘图
	//int draw_max;  //用于归一化
	//int draw_stop_x;//停止线x
	//int draw_stop_y;//停止线y
	//int draw_start_x;//起始引导线x
	//int draw_start_y;//起始引导线y
	//int draw_y;     //目标坐标y
	//int draw_x;     //目标坐标x
	//int draw_extral_y;//补偿y
	//int draw_extral_x;//补偿x

	CPen GreenPen;//(PS_SOLID,2,RGB(0,255,0));
	CPen BluePen;//(PS_SOLID,2,RGB(0,0,255));
	CPen OrangePen;//(PS_SOLID,2,RGB(255,150,0));
	CPen RedPen;//(PS_SOLID,2,RGB(255,0,0));
	CPen BlackPen;//(PS_SOLID,2,RGB(0,0,0));
	CPen YellowPen;

	bool bDrawInit;//是否进行过初始化
	bool bDrawGetReadyXY;//
	//bool bDrawWhite;//是否画白
	//int draw_standard_near_x_pixel;
	//int draw_standard_near_y_pixel;
	//int draw_standard_far_x_pixel;
	//int draw_standard_far_y_pixel;
	int draw_guideline_near_x_pixel;
	int draw_guideline_near_y_pixel;
	int draw_guideline_start_x_pixel;
	int draw_guideline_start_y_pixel;
	int draw_guideline_far_x_pixel;
	int draw_guideline_far_y_pixel;
	//int draw_0_x_pixel;
	//int draw_0_y_pixel;//真实0点在画图中的坐标
	int draw_2Ddata_start_y_pixel;
	int draw_2Ddata_end_y_pixel;
	float draw_x_ratio;//real:pixel
	float draw_y_ratio;
	int draw_white_width_pixel;//上下留白的宽度=10
	int farY_real;
	int farX_real;//停止线上最远的xy
	int nearX_real;
	int nearY_real;
	int draw_status;//1原始数据；2xy坐标
	int DrawRct_top;
	int DrawRct_bottom;
	int DrawRct_left;
	int DrawRct_right;
	int colormap1024[1024][3];
		
	void draw_OnLaserPaintInit();//画图初始化、连接
	int draw_x_real2pixel(int x_real);
	int draw_y_real2pixel(int y_real);//从真实坐标计算画图坐标
	void draw_getready_guideline();//从设置好的引导线来确定画图起始点、比例
	void draw_guideline();//画引导线，自动没有画成全白
	void draw_getready_2Ddata();//从二维扫描来确定画图起始点、比例
	void draw_2Ddata();//画二维数据，自动画成全白
	void draw_getready_xy();//
	void draw_xyCoor();//画坐标
	void draw_xy2Ddata();//画
	void draw_white();//画成全白	
	//*******************************************************************************

	//*****************	 新水平标定方法的变量	***************************/
	//unsigned int HrzCalibDistanFirst;//水平标定第一次定点测量20000次的距离平均值
	//unsigned int HrzCalibDistanSecond;//水平标定第二次定点测量20000次的距离平均值

	//#define  HRIZDISTANCE   10//action()函数里，跳转到HrizDisAvg()的标志位
	bool  hrizmeasure;//是否跳转到HrizDisAvg()函数
	//int     measurenum;
	//void  HrizDisAvg(void);
	//int   runnum;
	//****************************************************
//****************************************************
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	DECLARE_EVENTSINK_MAP()
	void OnComm();
	void Manualsend();
	char ConvertHexChar(char ch);
	int String2Hex(CString str, CByteArray &senddata);//【重载，config原本函数】
	void WalkSetUp();
	void Jundgeheight(int Sm,int St,double mid,int length_data);
	void savefile(CString file_name);
	void savefile2(CString file_name);
	int save_laserSuspect_file(CString str_content, CString filename="疑似捕获记录.txt" ,bool savetime=1);//保存激光疑似捕获
	void OnAutocatch();
	void stepMoveNoSet0(int stepNum, unsigned char direction, unsigned char type);



	
	int ConfirmCentre(int Sm_data  /*平均回波数据*/);
	int FindSm(int* point_num /*扫描点总数*/);
	int Jundgemid(int Sm,int St,double mid,int length_data,int SmMinForward);
	int JundgeComfirmType(int Sm,int St,int mid,int length_data,int SmMinForward);
	void WalkText();
	void ScanYard();
	void ScanYardToAdjust();
	void CLaster::WalkTextToAdjust();
	void ComfirmType();
	void ScanAll();
	void ScanAll_org();
	void MedFilter(int length_data,int num);//中值滤波
	bool JundgeCatch(int Sm,int St,int SmStart,double SmMin,int length_data);//判断捕获是否成功
	BOOL CalculateLineKB(float &k,float &b);
	void OnCalibration(int num);
	void Calibration();
	void Avg();
	//**********  离线测试 ***********
    void offline_row();
	int offline_mode;
	CString offlineGetDirectory();
	CFileFind offline_finder;
	BOOL offline_bWorking;//下一个文件存在
	char  offline_chararr[4];
	char offline_buff[2000];
	CString offline_file_name;
	void OfflineText();
	boost::thread* OnCommandThread;
	boost::thread* ComReadThread;
	boost::thread* ComWriteThread;
	//*********** TCPIP连接
	void InitTCPIPClient(bool* running);
	//****************************
	//**********  机型验证使用 ***********
  double verifyAverageSum;//各行平均数的和，过程中每次累加
  double verifyAverageSum2;//各行平均数的和，最后计算，最多20行
  double verifyAverage[VERIFY_HORIZONTAL_MAX_NUM];//各行平均数，最多记录20行
  int verifyCountRow;//累计行数
  bool verifyComplete;//机型验证是否完成
  bool verifyFinishNow;//立刻要求验证结果
  int verifyHorizonalResult;//-1验证正常完成，0没有有效行，正数是有效行不够时的行数
  float verifyHorizonalConfidence;//水平轮廓置信度
  int verifyCountVirtical;//垂直扫描次数
  float verifyHeightSum;//高度累加，过程中每次累加
  float verifyHeightSum2;//高度累加，最后计算，最多10列
  float verifyHeight[VERIFY_VERTICAL_MAX_NUM];//各列高度值，最多记录10个
  int verifyVirticalResult;
  float verifyHeightConfidence;//高度验证置信度
	//void OnBnClickedConnect_agent()
  float HrzConfirmProbability;
  float VertConfirmProbability;

	public:
	afx_msg void OnBnClickedConnect();

	afx_msg void OnBnClickedScanyard();
	afx_msg void OnBnClickedScanyardToAdjust();
	afx_msg void OnBnClickedWalkset();
	afx_msg void OnBnClickedWalktest();
	afx_msg void OnBnClickedBtnClear();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedStopline();
	afx_msg void OnBnClickedButtonSetzero();
	afx_msg void OnBnClickedButtonCalibration();
	afx_msg void OnBnClickedButtonLocate();
	afx_msg void OnBnClickedButtonLeft();
	afx_msg void OnBnClickedButtonRight();
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	afx_msg void OnBnClickedButtonDetect();
	afx_msg void OnBnClickedButtonHrzcalibration();
	afx_msg void OnBnClickedButtonEdge();
	afx_msg void OnBnClickedButtonTwod();
	afx_msg void OnBnClickedButtonLeftten();
	afx_msg void OnBnClickedButtonUpten();
	afx_msg void OnBnClickedButtonRightten();
	afx_msg void OnBnClickedButtonDownten();
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnBnClickedButtonPrepare();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedSave();
	//afx_msg void OnBnClickedSety();
	afx_msg void OnBnClickedButtonReadedge();//读取边框步数
	afx_msg void OnBnClickedButtonReadguideline();//读取引导线（已作废）
	afx_msg void OnBnClickedButton10();//画图测试
	afx_msg void OnBnClickedButtonClc();//画图清屏
	afx_msg void OnBnClickedButton2dswitch3d();//画图切换2D3D模式
	afx_msg void OnBnClickedButtonComfirmtype();//由机型读取引导线
	afx_msg void OnBnClickedButtonReadtype();//由机型读取引导线
	
	afx_msg void OnBnClickedButtonReadlinenum();//由编号读取引导线
	afx_msg void OnBnClickedButtonConfirmwalkset();//确认走步设置（计算）
	afx_msg void OnBnClickedButtonClearwalkset();//清空走不设置
	afx_msg void OnBnClickedButtonRedlaseron();//打开红点激光
	afx_msg void OnBnClickedButtonRedlaseroff();//关闭红点激光
	afx_msg void OnBnClickedButtonScanall();//窗口扫描

	afx_msg void OnBnClickedButtonWalkTestToAdjust();//进行走步测试调整
	afx_msg void OnBnClickedButtonOffLineTest();
	afx_msg void OnBnClickedButtonGuideLineMove();
	afx_msg void OnBnClickedButtonStopLineMoveSingle();
	afx_msg void OnBnClickedButtonStopLineMove();
	afx_msg void OpenLaserCOM();
	afx_msg void CloseLaserCOM();
};
//extern boost::thread_group glb_thread_group;

//RECOG_API CLaster* _stdcall classLaserInit();//