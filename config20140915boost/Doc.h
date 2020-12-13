#pragma once
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>   
#include <iostream>

using namespace cv;
using namespace std; 

//���Ӿ��������ٲ�������λ������Ϣ
typedef struct vision_machine_parameters { 
	
	//�Ӿ���⡢����
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
	int yard_scan_range;//վƺɨ�跶Χ��1��2��3��4��5��
	int yard_scan_obstacle_high;//վƺɨ���ϰ���ĸ߶�
	int yard_scan_obstacle_width;//վƺɨ���ϰ���Ŀ��
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
	int NeedShieldObscur;//�Ƿ��������ڵ�
	int X_Obscur_Left;//���������У����ݴ���Y_Obscurʱ�ж�x�����Ƿ���X_Obscur�ڣ���X_Obscur��ĵ�����
	int X_Obscur_Right;//���������У����ݴ���Y_Obscurʱ�ж�x�����Ƿ���X_Obscur�ڣ���X_Obscur��ĵ�����
	int PlaneLeaveHighMin;
	int PlaneLeaveHighMax;
	int PlaneLeaveYMin;
	int PlaneLeaveYMax;
	int PlaneLeaveZMin;
	int PlaneLeaveConfirmPoint;
	int PlaneLeaveConfirmLine;
	int Y_Obscur;//���������У����ݴ���Y_Obscurʱ�ж�x�����Ƿ���X_Obscur�ڣ���X_Obscur��ĵ�����
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
	string ServerAddress;//��λ��IP��ַ
	string ServerPort;//��λ���˿�
	string LogSaveRoot;//ͼ�񱣴�Ŀ¼
	double LogPreserveDays;
	string LASERCOM;
	int LASERPAUD;
	string LEDCOM;
	int LEDPAUD;
	string MODBUSCOM;
	int MODBUSPAUD;
	string DIPCOM;
	int DIPPAUD;
	//�ں�
	//double DistanceWeight;
	//double DriftWeight;
	//double SpeedBound;
	//double DriftBound;
	int NeedPlaneLeaveScan;//�Ƿ���֤�ɻ����
	double AccuracyGuideDistance; // ��ȷ��������
	double DriftDistance;  //ƫ����ʾ
	double DriftRemove;  //ƫ����ʾ�Ƴ�
	double OverSpeed24;  //24���ⳬ��
	double OverSpeed24Remove;  //24���ⳬ���Ƴ�
	double OverSpeed;  //24���ڳ���
	double OverSpeedRemove;  //24���ڳ����Ƴ�
	double ValidateDistance;  //���������֤����
	double DriftWeightStart;  //��ʼƫ��Ȩ��
	double DriftWeightEnd;  //0�״�ƫ��Ȩ��
	double DistanceWeightStart;  //��ʼ����Ȩ��
	double DistanceWeightEnd;  //0�״�����Ȩ��
	double SpeedWeightStart;  //��ʼ�ٶ�Ȩ��
	double SpeedWeightEnd;  //0�״��ٶ�Ȩ��
	double SlowDistance; //��ʼ��ʾSLOW�ľ���
	double DriftGain;  //ƫ������
	double DistanceGain;  //��������
	double MinDriftDistance;  //��С��ƫ�ƾ���
	double MaxStopDistance; //���ֹͣ����
	double TooFarDistance; //����ֹͣ����ֵ
	double SubsystemErrorTime;  //��ϵͳ����ʱ��
	double LEDDelayTime;  //LED�ӳ�ʱ��
	double ModbusDelayTime;  //MODBUS�ӳ�ʱ��
	int BoardInterlockPort;  //�ǻ��Ż����źŽӿ�
	int EmergencyStopPort;  //��ͣ�źŽӿ�
	double AutoSleepTime;   //�Զ�����ʱ��
	double StopDisplayTime;  //STOP������ʾʱ��
	double ChockOnDisplayTime;  //�ֵ��źų�����ʾʱ��
	double MaxSpeed;  //�������
	double MaxSpeedRemove;  //��������Ƴ�
	double StopToOKTime;  //STOP��OK��ʱ��
	double SpeedSmoothWindow;  //�ٶ�ƽ�������
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

//��ʵ�ʾ���͵�����
typedef struct m_OrientationPs { 
	string point_name;
	vector<pair<double,Point2d>>VLinePoints;
}m_OrientationPs;

//���
typedef struct m_point { 
	string point_name;
	Point2d m_p;
	m_point(string name,Point2d p) : point_name(name),m_p(p)
	{}
	m_point() : point_name(""),m_p(Point2d(0,0))
	{}
}m_point;

//�����
typedef struct m_rect { 
	string rect_name;
	Rect m_r;
	m_rect(string name,Rect r) : rect_name(name),m_r(r)
	{}
	m_rect() : rect_name(""),m_r(Rect(0,0,0,0))
	{}
}m_rect;


//�浥����ֵ
typedef struct m_alonex{ 
	string m_name;
	float m_x;
	m_alonex(string name,float x) : m_name(name),m_x(x)
	{}
	m_alonex() : m_name(""),m_x(0.0)
	{}
}m_alonex;

//�������
typedef struct m_Vpoints { 
	string m_Vpname;
	vector<Point2d> Vpoints;
}m_Vpoints;

//���ַ�������
typedef struct m_Vstrings{ 
	string m_Vsname;
	vector<string>Vstrings;
}m_Vstrings;

//���������
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

	//����Ӳ���
	vector<float>horizontalCurveFactor;
	//float horizontalCurveFactor[10];
	int numHorizontalCurveFactor;
	int horizontalXMaxFromHead;
	int minAverage;
	int maxAverage;

	PlaneInfo():horizontalCurveFactor(15){}//Ϊvector��ʼ����С
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
	vector<m_point>points;//������������ߡ�ֹͣ�����ˣ�
	vector<m_rect>rects;//�����������վƺ�򡢵ǻ��ſ�
	vector<m_alonex>xs;//�������ֲ�����������
	vector<m_Vpoints>VPs;//�����(ͼ�ϵľ���㣩
	vector<m_Vstrings>VSs;//�ַ���������������
	vector<m_OrientationPs>VOPs;//��λ������������
	vector<laser_parameters> VLPs;//������ϵͳ������������
	vision_machine_parameters V_M_datas;//�Ӿ���ϵͳ����
	laser_machine_parameters L_M_datas;//������ϵͳ����
	system_parameters V_S_datas;//ϵͳ����
}m_allnodedata;

class Document
{
public:
	Document();
	~Document(void);

//�������ݶ�д
public:
	vector<PlaneInfo> VPlaneInfos;//��ȫ���ɻ����ݵ�����
	vector<PlaneInfo> readallplaneinfodata(string routename);//��ȡ�����Ѿ�д�õĻ�������,���Գ�ʼ��ʱ�����������ݴ���VPlaneInfos
	//void readallplaneinfodata(string routename);//��ȡ�����Ѿ�д�õĻ�������
	void writedata(string routename,string planename,PlaneInfo planetypeinfo);//д��������
	void readplanedata(string routename,string planetype, PlaneInfo& planedata);//����������
	void readplanedata(string planetype, PlaneInfo& planedata);//��ȫ����������������ĳ���������ݣ�����Ҫ�����Ļ�����

public:
	////////////////д���ⳡ�������������///////////////////
	vector<laser_parameters> VLaserParameters;//��ȫ�����ⳡ������������ݵ�����
	void writedata(string routename,string gl_sl_name,laser_parameters laser_datas);//��������ֹͣ�߱��д���ⳡ�������������
	laser_parameters readlaserdatas(string routename, string glname, string slname);//ͨ��ֹͣ�������߱�Ŷ��������

	/////////////////���������д/////////////////////////
	laser_machine_parameters L_M_Datas;
	void writedata(string routename,laser_machine_parameters laser_datas);//д����
	laser_machine_parameters readlaserparamters(string routename);//������

    /////////////////�Ӿ�������д/////////////////////////
	vision_machine_parameters V_M_Datas;
	void writedata(string routename,vision_machine_parameters vison_datas);//д����
	vision_machine_parameters readvisiondatas(string routename);//������

	/////////////////system������д/////////////////////////
	system_parameters V_S_Datas;
	void writedata(string routename, system_parameters system_datas);//д����
	system_parameters readsystemdatas(string routename);//������

	/////////////////////�Ӿ������������ݶ�д/////////////////////////////////////////
	void writedata(string routename,string Vpsname,vector<pair<double,Point2d>>VLinePoints,int flag);//д�ṹ����������
	vector<pair<double,Point2d>> readVOrientationdata(string routename,string vname);//���ṹ����������

	void writedata(string routename,string Vstringsname,vector<string>Vstrings);//дstring��������
	vector<string> readVSdata(string routename,string vsname);//��string��������
	
	void writedata(string routename,string Vpointsname,vector<Point2d>Vpoints);//д����������
	vector<Point2d> readVPdata(string routename,string vpname);//������������

	void writedata(string routename,string name,float alonex);//д���ݣ�ĳЩ������
	float readalonexdata(string routename,string name);//������

	void writedata(string routename,string pointname,Point2d m_p);//д������
	Point2d readpointdata(string routename,string pointname);//��������

	void writedata(string routename,string rectname,Rect m_rect);//д���ο�����
	Rect readrectdata(string routename,string rectname);//�����ο�����

public:
	///////////////ȫ������///////////////////////////
	m_allnodedata readalldata(string routename);//��ȡ��������
	void writealldata(string routename);//д��������

    ///////////////�������///////////////////////////
	//�ɻ��Ͷ���������ֹͣ�߱��
	string guaidlinename ;
	string stoplinename ;
	void read_gl_sl(string routename,string planetype);
	//�������ߺ�ֹͣ�߶������ͺ�
	vector<string> read_Vplanetypes(string routename,string gl_slname);
	
private:
	vector<m_point>VscenePs;//������ݵ�����
	void Vpoints(string pointname,Point2d m_p);//��������

	vector<m_rect>VsceneRs;//����ο����ݵ�����
	void Vrects(string pointname,Rect m_r);//����������


	vector<m_alonex>VsceneXs;//�浥�����ݵ�����
	void ValoneXs(string name,float alonex);//�������

	vector<m_Vpoints>VsceneVPs;//�������
	void VVPs(string vpname,vector<Point2d> Vvpoints);//�������
	//void writevpoints(FileStorage fsw);

	vector<m_OrientationPs>VorientationVPs;//��ӳ�������
	void checkVorientationPs(string vpname,vector<pair<double,Point2d>>vPoints, int flag);//�������

	vector<m_Vstrings>VsceneVSs;//���ַ�������
	void VVstrings(string vsname,vector<string>Vvstrings);//�������
	
	void Vplaneinfos(string planetype,PlaneInfo planetypeinfo);//�����������������Ƿ��Ѿ����ظ���Ԫ��

	void VLaserdatas(string gl_sl_name,laser_parameters newlaserdatas);//��鼤�ⳡ�������������

};