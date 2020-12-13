#pragma once
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>   
#include <iostream>

using namespace cv;
using namespace std; 

//存视觉检测与跟踪参数及上位机等信息
typedef struct vision_machine_parameters { 
	
	//视觉检测、跟踪
	int CheckVisionEngineNum;
	int CheckVisionEngineGap;
	float MaxReportDistance;
	float GuideLineWidth;
	float BlackestJudgeStart;
	float BlackestJudgeStep;
	float CircleJudgeStart;
	float CircleJudgeStep;
	float maxIterTimes;
	float MinEngineRadius;
	float MinEnginePerimeter;
	float MinTemplateScale;
	float MaxTemplateScale;
	float MinMatchRate;
	float WheelDetectParamX;
	float WheelDetectParamY;
	float WheelRectHeight;
	float CutDownThresh;
	float OriginalScale;
	float MaxOffset;
	float VisualDistanceGain;
	float LaserDistanceGain;
	//float StationNum;
	//float 
}vision_machine_parameters;

typedef struct laser_machine_parameters{
	float install_high; 
	int HrzConfirmSteps;
	int VertConfirmSteps;	
	int Q12Steps;
	float HrzOffsetAngle;
	float HrzConfirmProbability;
	float VertConfirmProbability;
	int WalkTestDistance;
	int LaserForwardOffset;
	int LaserBackwardOffset;
	int VerifyEngine;
	int VerifyPlaneType;
	int RightLeftEngine;
	int NeedYardScan;
	int NeedBridgeScan;
	int RegularVertical;
	int RegularVerticalTime;
	int AccurateGuideDis;
	int yard_scan_range;//站坪扫描范围（1，2，3，4，5）
	int yard_scan_obstacle_high;//站坪扫描障碍物的高度
	int yard_scan_obstacle_width;//站坪扫描障碍物的宽度
	int VerifyHeadHighDis;
	int VerifyHeadHighMin;
	int VerifyHeadHighMax;
	int VerifyEngineDis;
	int EngineHeadDisMin;
	int ScanEngineTimes;
	int TowContinu;
	int VerifyEngineWidthMin;
	int VerifyEngineWidthMax;
	int VerifyEngineCabinDisMin;
	int VerifyEngineCabinDisMax;
	int VerifyEngineHeadDisMin;
	int VerifyEngineHeadDisMax;
	int NeedShieldObscur;//是否开启屏蔽遮挡
	int X_Obscur_Left;//引导过程中，数据大于Y_Obscur时判断x方向是否在X_Obscur内，在X_Obscur外的点舍弃
	int X_Obscur_Right;//引导过程中，数据大于Y_Obscur时判断x方向是否在X_Obscur内，在X_Obscur外的点舍弃
	int PlaneLeaveHighMin;
	int PlaneLeaveHighMax;
	int PlaneLeaveYMin;
	int PlaneLeaveYMax;
	int PlaneLeaveZMin;
	int PlaneLeaveConfirmPoint;
	int PlaneLeaveConfirmLine;
	int Y_Obscur;//引导过程中，数据大于Y_Obscur时判断x方向是否在X_Obscur内，在X_Obscur外的点舍弃
	int SuspectMode;
	int SuspectHotMax;
	int SuspectRowMin;
	int SuspectSMNumberMin;
	int MaxDistance;
	int CatchDisMin;
	int CatchDisMax;
	int ObjectHighMin;
	int ObjectHighMax;
	int CarHighMax;
	int ObjectDisMin;
	int ObjectDisMax;
	float WidthMaxRatio;
	float WidthMeasureRatio;
	int NeighborDifferMax;
	int PlanePointNumberMin;
	int BirdgePointNumberMin;
	int MaxGap;
	int AlwayMoveMax;
	int OfflineSimulaMode;
	int SaveDataWhenOffline;
	int OfflineSleepTime;
}laser_machine_parameters;

typedef struct system_parameters {
	string RunningMode;
	string StationNumber;
	string ServerAddress;//上位机IP地址
	string ServerPort;//上位机端口
	string LogSaveRoot;//图像保存目录
	double LogPreserveDays;
	string LASERCOM;
	int LASERPAUD;
	string LEDCOM;
	int LEDPAUD;
	string MODBUSCOM;
	int MODBUSPAUD;
	string DIPCOM;
	int DIPPAUD;
	//融合
	//double DistanceWeight;
	//double DriftWeight;
	//double SpeedBound;
	//double DriftBound;
	int NeedPlaneLeaveScan;//是否验证飞机离港
	double AccuracyGuideDistance; // 精确引导距离
	double DriftDistance;  //偏移提示
	double DriftRemove;  //偏移提示移除
	double OverSpeed24;  //24米外超速
	double OverSpeed24Remove;  //24米外超速移除
	double OverSpeed;  //24米内超速
	double OverSpeedRemove;  //24米内超速移除
	double ValidateDistance;  //最近机型验证距离
	double DriftWeightStart;  //初始偏移权重
	double DriftWeightEnd;  //0米处偏移权重
	double DistanceWeightStart;  //初始距离权重
	double DistanceWeightEnd;  //0米处距离权重
	double SpeedWeightStart;  //初始速度权重
	double SpeedWeightEnd;  //0米处速度权重
	double SlowDistance; //开始显示SLOW的距离
	double DriftGain;  //偏移增益
	double DistanceGain;  //距离增益
	double MinDriftDistance;  //最小报偏移距离
	double MaxStopDistance; //最大停止距离
	double TooFarDistance; //超出停止线阈值
	double SubsystemErrorTime;  //子系统故障时间
	double LEDDelayTime;  //LED延迟时间
	double ModbusDelayTime;  //MODBUS延迟时间
	int BoardInterlockPort;  //登机桥互锁信号接口
	int EmergencyStopPort;  //急停信号接口
	double AutoSleepTime;   //自动休眠时间
	double StopDisplayTime;  //STOP持续显示时间
	double ChockOnDisplayTime;  //轮挡信号持续显示时间
	double MaxSpeed;  //最高限速
	double MaxSpeedRemove;  //最高限速移除
	double StopToOKTime;  //STOP到OK的时间
	double SpeedSmoothWindow;  //速度平滑窗宽度
	int PBBAllow;
	int PCAAllow;
	int FGPAllow;
	int PCAPort;
	int FGPPort;
	int Q12Port;
	int UPSPort;
	int LEDERRPort;
	int LEDCOMERRPort;
	double DiskSpaceAlarm;
	int TaskMode;
	int SaveKeyImage;
	int CheckLeaveMode;
	int RestartInterval;
	int RestartTime;
	double CheckLeaveTime;
	double MaxAngleBiasX;
	double MaxAngleBiasY;
	double MaxAngleShakeX;
	double MaxAngleShakeY;
};

//存实际距离和点坐标
typedef struct m_OrientationPs { 
	string point_name;
	vector<pair<double,Point2d>>VLinePoints;
}m_OrientationPs;

//存点
typedef struct m_point { 
	string point_name;
	Point2d m_p;
	m_point(string name,Point2d p) : point_name(name),m_p(p)
	{}
	m_point() : point_name(""),m_p(Point2d(0,0))
	{}
}m_point;

//存矩形
typedef struct m_rect { 
	string rect_name;
	Rect m_r;
	m_rect(string name,Rect r) : rect_name(name),m_r(r)
	{}
	m_rect() : rect_name(""),m_r(Rect(0,0,0,0))
	{}
}m_rect;


//存单个的值
typedef struct m_alonex{ 
	string m_name;
	float m_x;
	m_alonex(string name,float x) : m_name(name),m_x(x)
	{}
	m_alonex() : m_name(""),m_x(0.0)
	{}
}m_alonex;

//存点向量
typedef struct m_Vpoints { 
	string m_Vpname;
	vector<Point2d> Vpoints;
}m_Vpoints;

//存字符串向量
typedef struct m_Vstrings{ 
	string m_Vsname;
	vector<string>Vstrings;
}m_Vstrings;

//存机型数据
typedef struct PlaneInfo { 
	string plane_Type;
	float cabin_Width;
	float wing_Width;
	float head_Height;
	float head_wheel_HoriDis;
	float engine_Centerdis;	
	float engine_Radius;
	float center_floor_Dis;//1 name 7
	float wheel_width;
	float wheel_height;
	float engine_head_HoriDis;

	//周添加部分
	vector<float>horizontalCurveFactor;
	//float horizontalCurveFactor[10];
	int numHorizontalCurveFactor;
	int horizontalXMaxFromHead;
	int minAverage;
	int maxAverage;

	PlaneInfo():horizontalCurveFactor(15){}//为vector初始化大小
	/*PlaneInfo(string name, float cw ,float ww,  float hh, float hwh, float ec, float er, float cfd):
	plane_Type(name),cabin_Width(cw),wing_Width(ww), head_Height(hh), head_wheel_HoriDis(hwh),
	engine_Centerdis(ec),engine_Radius(er),center_floor_Dis(cfd)
	{}
	PlaneInfo() : plane_Type(""),cabin_Width(0),wing_Width(0), head_Height(0), head_wheel_HoriDis(0),
	engine_Centerdis(0),engine_Radius(0),center_floor_Dis(0)
	{}*/
}PlaneInfo;

//typedef struct allPlaneInfodata{ 
//	vector<PlaneInfo>Vplaneinfodatas;
//}allPlaneInfodata;

typedef struct laser_parameters{ 
	string guideline_stopline_name;
	float object_dis;
	float guide_line_k;
	float guide_line_b;
	float guide_line_x;
	float guide_line_y;
	float offset_angle;
}laser_parameters;

typedef struct m_allnodedata{ 
	vector<m_point>points;//点参数（引导线、停止线两端）
	vector<m_rect>rects;//框参数（检测框、站坪框、登机桥框）
	vector<m_alonex>xs;//单独数字参数（废弃）
	vector<m_Vpoints>VPs;//点参数(图上的距离点）
	vector<m_Vstrings>VSs;//字符串参数（废弃）
	vector<m_OrientationPs>VOPs;//定位参数（废弃）
	vector<laser_parameters> VLPs;//激光子系统参数（废弃）
	vision_machine_parameters V_M_datas;//视觉子系统参数
	laser_machine_parameters L_M_datas;//激光子系统参数
	system_parameters V_S_datas;//系统参数
}m_allnodedata;

class Document
{
public:
	Document();
	~Document(void);

//机型数据读写
public:
	vector<PlaneInfo> VPlaneInfos;//存全部飞机数据的向量
	vector<PlaneInfo> readallplaneinfodata(string routename);//读取所有已经写好的机型数据,可以初始化时读出所有数据存入VPlaneInfos
	//void readallplaneinfodata(string routename);//读取所有已经写好的机型数据
	void writedata(string routename,string planename,PlaneInfo planetypeinfo);//写机型数据
	void readplanedata(string routename,string planetype, PlaneInfo& planedata);//读机型数据
	void readplanedata(string planetype, PlaneInfo& planedata);//由全部机型数据向量读某个机型数据，输入要读出的机型名

public:
	////////////////写激光场景定义参数数据///////////////////
	vector<laser_parameters> VLaserParameters;//存全部激光场景定义参数数据的向量
	void writedata(string routename,string gl_sl_name,laser_parameters laser_datas);//按引导线停止线编号写激光场景定义参数数据
	laser_parameters readlaserdatas(string routename, string glname, string slname);//通过停止线引导线编号读激光参数

	/////////////////激光参数读写/////////////////////////
	laser_machine_parameters L_M_Datas;
	void writedata(string routename,laser_machine_parameters laser_datas);//写参数
	laser_machine_parameters readlaserparamters(string routename);//读参数

    /////////////////视觉参数读写/////////////////////////
	vision_machine_parameters V_M_Datas;
	void writedata(string routename,vision_machine_parameters vison_datas);//写参数
	vision_machine_parameters readvisiondatas(string routename);//读参数

	/////////////////system参数读写/////////////////////////
	system_parameters V_S_Datas;
	void writedata(string routename, system_parameters system_datas);//写参数
	system_parameters readsystemdatas(string routename);//读参数

	/////////////////////视觉场景定义数据读写/////////////////////////////////////////
	void writedata(string routename,string Vpsname,vector<pair<double,Point2d>>VLinePoints,int flag);//写结构体向量数据
	vector<pair<double,Point2d>> readVOrientationdata(string routename,string vname);//读结构体向量数据

	void writedata(string routename,string Vstringsname,vector<string>Vstrings);//写string向量数据
	vector<string> readVSdata(string routename,string vsname);//读string向量数据
	
	void writedata(string routename,string Vpointsname,vector<Point2d>Vpoints);//写点向量数据
	vector<Point2d> readVPdata(string routename,string vpname);//读点向量数据

	void writedata(string routename,string name,float alonex);//写数据，某些单数据
	float readalonexdata(string routename,string name);//读数据

	void writedata(string routename,string pointname,Point2d m_p);//写点数据
	Point2d readpointdata(string routename,string pointname);//读点数据

	void writedata(string routename,string rectname,Rect m_rect);//写矩形框数据
	Rect readrectdata(string routename,string rectname);//读矩形框数据

public:
	///////////////全部数据///////////////////////////
	m_allnodedata readalldata(string routename);//读取所有数据
	void writealldata(string routename);//写所有数据

    ///////////////机型相关///////////////////////////
	//由机型读出引导线停止线编号
	string guaidlinename ;
	string stoplinename ;
	void read_gl_sl(string routename,string planetype);
	//由引导线和停止线读出机型号
	vector<string> read_Vplanetypes(string routename,string gl_slname);
	
private:
	vector<m_point>VscenePs;//存点数据的向量
	void Vpoints(string pointname,Point2d m_p);//检查点向量

	vector<m_rect>VsceneRs;//存矩形框数据的向量
	void Vrects(string pointname,Rect m_r);//检查矩形向量


	vector<m_alonex>VsceneXs;//存单独数据的向量
	void ValoneXs(string name,float alonex);//检查向量

	vector<m_Vpoints>VsceneVPs;//存点向量
	void VVPs(string vpname,vector<Point2d> Vvpoints);//检查向量
	//void writevpoints(FileStorage fsw);

	vector<m_OrientationPs>VorientationVPs;//存映射点向量
	void checkVorientationPs(string vpname,vector<pair<double,Point2d>>vPoints, int flag);//检查向量

	vector<m_Vstrings>VsceneVSs;//存字符串向量
	void VVstrings(string vsname,vector<string>Vvstrings);//检查向量
	
	void Vplaneinfos(string planetype,PlaneInfo planetypeinfo);//检查机型数据向量中是否已经有重复的元素

	void VLaserdatas(string gl_sl_name,laser_parameters newlaserdatas);//检查激光场景定义参数向量

};