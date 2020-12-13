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

//***	ˮƽ���ת�����á���direction����	*********
#define  LEFT			1
#define  RIGHT			2
#define  UP				1
#define  DOWN			2
//****	��ת�������趨����funciton����		*********
#define  JPCATCH		1		
#define	 JPVERTCALIB	2		//��׼����ֱ�궨������
#define  JPHRZCALIB		3		//��ת��ˮƽ�궨������
#define  JPAVGPRECISE	4		//��ת����ȷ��ദ����
#define  JPWALKSETUP	5		//��ת���߲����ô�����
#define  JPWALKTEST		6		//��ת���߲����Դ�����
#define  JPAVG			7		//��ת��AVG������
#define  JPHRZMOVE		8		//��ת����λ--ˮƽɨ�账��
#define  JPCONFIRMEDGE	9		//��ת��ȷ���߽紦����
#define  JPPREPAREMEASSURE 10	//ȷ��20m

//****	�Ƿ���λ�ò����趨����SETUP����		************
#define  SETPOSITION	1		//��  λ
#define  UNSETPOSITION	0		//����λ

//***	��ǰaction()����ִ�з�ʽ����type����	***********
#define  HRZSCANNING	1		//��ǰִ��ˮƽɨ�趯��
#define  VERTSCANNING	2		//��ǰִ�д���ɨ�趯��
#define	 FIXPOINT		3		//��ǰִ����ͨ��ද��������У׼����
#define	 HRZMOTORMOVE	4		//��ǰִ��ˮƽ�����λ����
#define  VERTMOTORMOVE	5		//��ǰִ�д�ֱ��λ����
#define  RESET			6		//��λ����
#define  SETZERO		7		//��λ����
#define  MEASPRICISE	8		//��ȷ��๦�ܣ���2W������ֵ���ֵ

#define NUM_NEED_REFRESH 4//����4��������Ҫˢ�½���
#define VERIFY_HORIZONTAL_MAX_NUM 20//ʹ�õ�ˮƽɨ���������ֵ
#define VERIFY_VERTICAL_MAX_NUM 10//ʹ�õĴ�ֱɨ���������ֵ

struct needTransform1_2//1���棬2�����ߣ�4�߲���������
{
public:
	int install_high;//��װ�߶�
	int object_high;//����߶�
	int object_dis;//�������
	float Edit_Start_x;//+-6��
	float Edit_End_x;
	int Edit_StandHigh;//��ֱ����
	int Auto_StandHigh;//ˮƽ����
	int mid_num;//������
	int guide_line_y;//������y
	//CString m_strRXData;//��ʾ״̬
	//CString walktext_deflection;//��ʾ��ϸ���
	char m_strRXData[50];//��ʾ״̬
	char walktext_deflection[200];//��ʾ��ϸ���
	//CString PlaneType4Confirm;
	//CString LineNum4Confirm;
	char PlaneType4Confirm[20];//���Ͷ�ȡ������
	char LineNum4Confirm[20];//��Ŷ�ȡ������

	float guide_line_k; //������ֱ�߷���y=kx+b;
	float guide_line_b;
	int guide_line_x;

	int x_walk;//�߲����Ե�λ�ã�ֻ����
	int y_walk;

	char str_guidenum;//�����߱�ţ���C#�еĵ����õ���ֻ���С���Ҫ��ʼ��Ϊ'\0'
	char str_stopnum;
	float offsetforline;//������ֹͣ���ƶ�ƫ����
};

struct needTransform3//��άɨ�����ݣ�ֻ����
{	
	int data_h;//����
	int save_data[250];
};

//struct needTransform4//�߲����Ե�λ�ã�ֻ����
//{
//	int x;
//	int y;
//};


// CLaster �Ի���

class CLaster : public CDialog
{
	DECLARE_DYNAMIC(CLaster)

public:
	CLaster(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLaster();

	

// �Ի�������
	enum { IDD = IDD_Laster };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int save_data[100000];
	//CMscomm1 m_ctrlComm;
	CString m_strRXData;
	CString m_strTXData;
	CString format,start_x,end_x,start_y,end_y,Stand_High,up_y;
	CString Edit_format;
	float Edit_Start_x,Edit_End_x,Edit_Start_y,Edit_End_y; //����ˮƽ�ʹ�ֱ����ʼ������Ƕ�
	float Edit_x_Offset;
	int forward_offset;
	int backward_offset;
	short int Edit_StandHigh; //ˮƽɨ�费ֱ����
	short int Auto_StandHigh; //ˮƽɨ�费ֱ����
	int object_high;//����߶�
	CString catch_plane; //��ʾ����״̬
	CString End_order;   //��������ĺ�5��ָ��
	CString walktext_deflection; //��ʾ�߲�����ƫת���
	int install_high;   //���밲װ�߶�
	int walksetup;      //�ж��Ƿ�����߲�����
	int walktext;       //�ж��Ƿ�����߲�����
	int scanyard;   //�ж��Ƿ����վƺɨ��
	int scanyardToAdjust;   //�ж��Ƿ����վƺɨ��Ȼ��У׼
	int loop;
	unsigned char sum;
	CString lo,ca,te;   //�洢Ŀ¼��
	CString D_Plan_Time,time_str;//�洢վƺɨ��
	//CString format;       //������������
	CString reset_order;
	CString save_start_x,save_start_y;
	CString save_end_x,save_end_y;
	int data_length;      //�������ݳ���
	int data_order;       //����ָ���
	int data_h;           //ɨ�����ݵ���
	int NUM;

	int Calib_fuction;   
	int calib_num;   
	int redlight;    //�жϺ�ɫ�����Ƿ���
	int line;        //ɨ������
	int positive;    //�ж�����
	CString Edit_line;
	int line_num;
	int page;        //ɨ�����
	int ci;          //У׼�㲽��
	int next_frame;   //�Ƿ��к�������֡
	int stan_catch;   //�ж��Ƿ�ˮƽ����
	int auto_catch;   //�ж��Ƿ�����Զ�����
	int auto_num;     //�Զ��������
	int mid_num;  //�е����
	int avg_order;
	int start_height;     //��ʼɨ�费ֱ����
	int object_dis;//���󵽼������ľ��룬������Ϊ120m����
	int object_width;
	float guide_line_k; //***********************������ֱ�߷���y=kx+b;************
	float guide_line_b;
	float offsetforline;
	int guide_line_y; //ֹͣ�ߵ��������������Ĵ�ֱ����
	int guide_line_x; //ֹͣ�ߵ���������������ˮƽ����
	int stop_lp;        //ֹͣѭ��
	int walksetup_num; //�߲��������ݵ���
	int stip_h; //��ֱ���ӵ����ƶ�����
	BOOL laster_flag;
	int scan_mode;  //ɨ�跽ʽ����Ϊˮƽɨ��ʹ�ֱɨ��
	int judge_direct;   //�洢��ǰ��ֱ���Ӳ���λ��
	float first_long;   //�������һ�ε�ƽ������
	int temp;             //У׼�õ�ѡ��
	//***********վƺɨ���ʼ��****************	
	int DetectThing_SaveModle;
	int Detect_Thing_Init;
	int length_data;//һ�����ݵ�ĳ���
	vector<vector<int> >Y_All;
	vector<vector<int> >Z_All;
	vector<int>obstacle_dis;
	int DtcTEdit_StandHigh_min;
	int NUM_NEW;
	int Scan_Cate;//3ΪSɨ�裬0Ϊ����ɨ��
	int Up_Y_Step;
	int yard_scan_range;//վƺɨ�跶Χ��1��2��3��4��5��
	int yard_scan_obstacle_high;//վƺɨ���ϰ���ĸ߶�
	int yard_scan_obstacle_width;//վƺɨ���ϰ���Ŀ��
	int Edit_StandHigh_last;//����ɨ��ͼ��ǻ�����
	int Edit_StandHigh_Org;//OrgΪȫ�ֱ�����orgΪ�ֲ�����	
	float Edit_Start_x_Org;
	float Edit_End_x_Org;
	float adjustAlpha_ySum;//����վƺɨ����У׼
	int adjustTimes;//����վƺɨ����У׼
	int adjustEdit_StandHigh;//����վƺɨ����У׼������Ϊ����̧�Ĳ���
	int z_temp;//���ڼ���վƺƽ���߶�
	//**********  ��ʼ������ ***********
	int divid_x; //xϸ��
	int divid_y; //yϸ��
	int presacl; //��Ƶ��
	int insert_num;//�ڲ�ֵ
	int expan;  //�������
//)
	//
	double time_start,time_end,time_fin; //��¼��ʼʱ�䣬����ʱ��͹���ʱ��
    float stand_dis_start,stand_dis_end; //���ڼ�������ٶȵ���ʼ����ͽ�������
	int smooth;
	int save_mode;   //�洢ģʽ
	float precise_y;//��ȷ��λ����y
    float precise_dis;//��ȷ��λ��ֱ�����ƶ�һ���ľ���
	float object_speed;     //�����ٶ�
	int guide_plane;   //�ж��Ƿ���зɻ�����
	int direction_num;  //ƽ������

	Document Doc;
	CLineNum linenum;
	string routename;

//*******************************************************************************
//******************** for calibration ******
	unsigned char	ForHrzCalib;	//ˮƽ�궨�������Ĳ���
	unsigned char	OrdDistance;	//��ͨ����������Ҫ����У׼������
	unsigned char	distPrcs;		//��ȷ������
	unsigned char	HrzScnProc;		//ˮƽɨ�账����

	unsigned char	isBalance;		//��֤ˮƽ����Ƿ����
	unsigned int	distArr[3];		//����ֵ��distArr[0]--�����ʮ������ֵ��  distArr[1]--�м�ʮ������ֵ�� disArr[2]����ʮ������ֵ��
	unsigned int	distancePrecese;//��ȷ���õ��ľ���
	unsigned int	DistMin;		//��Сֵ�����ֵ������distanccePreces�Ƚ�
	unsigned char	NumberMove;		//�������ұȽ�ʮ�Σ�ÿ��NumberMove�Լ�	
	unsigned short  cntSteps ;		//��¼���ĵĴ����������Ե�ж��ٲ�
	
	unsigned char	ResetOrNot;		//step2 ��λ Ȼ��ƫ��cntsteps������ŵ�����λ��
	unsigned char	jmpAbtCenter;	//�Ƿ�ƫ�ƣ�TRUE=ƫ�ƺ� False=����ǰ

	unsigned char	microMove;		//΢����࣬û1������һ��
	unsigned char	microMoveNum;	//ͬNumberMove
	unsigned int	microMin;		//΢�������Сֵ����distancePrecese�Ƚ�
	unsigned short	microEqual;		//��MicroMin = distancePrecese��microEqual�Լ�
	unsigned short	microCntSteps;	//ͬCntSteps����¼��ȷ�����ƫ���ж��ٲ�
		
	unsigned short	inSideWindow;	//����У׼����Ե�ڵĵ���
	unsigned char	HrzEdgeConfirm;	//�ж��Ƿ�ȷ����ˮƽ�߽�
	unsigned char	VertEdgeConfirm;//�ж��Ƿ�ȷ���ô�ֱ�߽�	
	unsigned short	LastInside;		//��һ������У׼���ڵĵ���
	unsigned char	ConfirmFunct;
	short	HrzConfirmSteps;
	short	VertConfirmSteps;

	unsigned char	HrzDetect;
	unsigned char	VertDetect;
	unsigned short TwoDimenScan;
	
//*****************	 У׼	***************************/
	void	actions(int direction, int StepsSrce, int stepsDst, int SetZero, int type, int jumpFunction); 
	void    stepMove(int stepNum, unsigned char direction, unsigned char type);//�ƶ�����Ϊ���
	void	hrzScanningForCalib(); 
	void	AVGprecise();
	void	normalMeasure();
	void	HrzProFunct();
	void	ConfirmEdge();
	void	CalibHriz();
//*****************	 �Զ�Ѱ�ҵ����	***************************/
	bool prepare_measure;//��ΪTRUEʱ�����б궨ǰ��20m��׼���ݵ�ȷ��
	bool  prepare_complete;//20m�ľ����Ѱ�����
	int save_hrzCali_record(const char *content="D:\\plane\\D\\LR.txt");//����ˮƽ�궨��¼
	//*****************	 ��ֹ֤ͣ��	***************************/
	int comfirm_type;
	//string planeType4Confirm;
	//string planeType;
	CString PlaneType4Confirm;
	CString LineNum4Confirm;
	int scan_all;
	int countNearPoint[2];
	//*****************	 dll	***************************/
	int refleshFlag[NUM_NEED_REFRESH];//���ܲ�����
	//��ֵ��0����ˢ�£�1ˢ�²�������2ˢ�²�����ˢ�£�-1�ȴ�����ţ�1������棬2�����ߣ�3��ά�㣬4�߲���������******���ʼ��Ϊ0��
	int refleshFlag1,refleshFlag2,refleshFlag3,refleshFlag4;
	int *refleshFlag1pointer,*refleshFlag2pointer,*refleshFlag3pointer,*refleshFlag4pointer;//��C#�����ڴ棬��ȫ������๹���ʱ����Щָ�븳��ַ
	int x_walk,y_walk;//���Դ����߲���������
	int countWalkSetCount;//��¼�߲������еõ��ĵ����������Զ�ֹͣ
	////*****************	 boost	***************************/
	//boost::thread_group glb_thread_group;
	//boost::thread *thread1, *thread2, *thread3;
	boost::posix_time::ptime Laser_clock;
	unsigned char senddata[100];//���ڷ���16����ָ��
	unsigned char lacer_sum;  //У���
	bool stopped;
	int frame_length;
	int String2Hex(CString str,unsigned char senddata[]); //���ַ���ָ��ת����16����ָ����أ�����VIDGS��
	string LaserInit();
	void LaserDestory();
	void OnCommand();
	int unpackHead(unsigned char * laser_buf,int order_length);//������ͷ
	int unpackBody(unsigned char * laser_buf,int code_length);//��������
	void Com_Laser_Read(); //���ڶ�
	void Com_Laser_Write(); //����д
	void Sum_Check(int len,unsigned char senddata[]);
//*****************	 ��ͼ	***************************/
	CWnd* pDrawWnd; //��ʾ�ؼ����ڵı��� 
	CDC* pDrawControlDC; //��ʾ�ؼ����ڵ��豸������.DC 
	CRect DrawRct; 
	//int draw;//���ڻ�ͼ
	//int draw_max;  //���ڹ�һ��
	//int draw_stop_x;//ֹͣ��x
	//int draw_stop_y;//ֹͣ��y
	//int draw_start_x;//��ʼ������x
	//int draw_start_y;//��ʼ������y
	//int draw_y;     //Ŀ������y
	//int draw_x;     //Ŀ������x
	//int draw_extral_y;//����y
	//int draw_extral_x;//����x

	CPen GreenPen;//(PS_SOLID,2,RGB(0,255,0));
	CPen BluePen;//(PS_SOLID,2,RGB(0,0,255));
	CPen OrangePen;//(PS_SOLID,2,RGB(255,150,0));
	CPen RedPen;//(PS_SOLID,2,RGB(255,0,0));
	CPen BlackPen;//(PS_SOLID,2,RGB(0,0,0));
	CPen YellowPen;

	bool bDrawInit;//�Ƿ���й���ʼ��
	bool bDrawGetReadyXY;//
	//bool bDrawWhite;//�Ƿ񻭰�
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
	//int draw_0_y_pixel;//��ʵ0���ڻ�ͼ�е�����
	int draw_2Ddata_start_y_pixel;
	int draw_2Ddata_end_y_pixel;
	float draw_x_ratio;//real:pixel
	float draw_y_ratio;
	int draw_white_width_pixel;//�������׵Ŀ��=10
	int farY_real;
	int farX_real;//ֹͣ������Զ��xy
	int nearX_real;
	int nearY_real;
	int draw_status;//1ԭʼ���ݣ�2xy����
	int DrawRct_top;
	int DrawRct_bottom;
	int DrawRct_left;
	int DrawRct_right;
	int colormap1024[1024][3];
		
	void draw_OnLaserPaintInit();//��ͼ��ʼ��������
	int draw_x_real2pixel(int x_real);
	int draw_y_real2pixel(int y_real);//����ʵ������㻭ͼ����
	void draw_getready_guideline();//�����úõ���������ȷ����ͼ��ʼ�㡢����
	void draw_guideline();//�������ߣ��Զ�û�л���ȫ��
	void draw_getready_2Ddata();//�Ӷ�άɨ����ȷ����ͼ��ʼ�㡢����
	void draw_2Ddata();//����ά���ݣ��Զ�����ȫ��
	void draw_getready_xy();//
	void draw_xyCoor();//������
	void draw_xy2Ddata();//��
	void draw_white();//����ȫ��	
	//*******************************************************************************

	//*****************	 ��ˮƽ�궨�����ı���	***************************/
	//unsigned int HrzCalibDistanFirst;//ˮƽ�궨��һ�ζ������20000�εľ���ƽ��ֵ
	//unsigned int HrzCalibDistanSecond;//ˮƽ�궨�ڶ��ζ������20000�εľ���ƽ��ֵ

	//#define  HRIZDISTANCE   10//action()�������ת��HrizDisAvg()�ı�־λ
	bool  hrizmeasure;//�Ƿ���ת��HrizDisAvg()����
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
	int String2Hex(CString str, CByteArray &senddata);//�����أ�configԭ��������
	void WalkSetUp();
	void Jundgeheight(int Sm,int St,double mid,int length_data);
	void savefile(CString file_name);
	void savefile2(CString file_name);
	int save_laserSuspect_file(CString str_content, CString filename="���Ʋ����¼.txt" ,bool savetime=1);//���漤�����Ʋ���
	void OnAutocatch();
	void stepMoveNoSet0(int stepNum, unsigned char direction, unsigned char type);



	
	int ConfirmCentre(int Sm_data  /*ƽ���ز�����*/);
	int FindSm(int* point_num /*ɨ�������*/);
	int Jundgemid(int Sm,int St,double mid,int length_data,int SmMinForward);
	int JundgeComfirmType(int Sm,int St,int mid,int length_data,int SmMinForward);
	void WalkText();
	void ScanYard();
	void ScanYardToAdjust();
	void CLaster::WalkTextToAdjust();
	void ComfirmType();
	void ScanAll();
	void ScanAll_org();
	void MedFilter(int length_data,int num);//��ֵ�˲�
	bool JundgeCatch(int Sm,int St,int SmStart,double SmMin,int length_data);//�жϲ����Ƿ�ɹ�
	BOOL CalculateLineKB(float &k,float &b);
	void OnCalibration(int num);
	void Calibration();
	void Avg();
	//**********  ���߲��� ***********
    void offline_row();
	int offline_mode;
	CString offlineGetDirectory();
	CFileFind offline_finder;
	BOOL offline_bWorking;//��һ���ļ�����
	char  offline_chararr[4];
	char offline_buff[2000];
	CString offline_file_name;
	void OfflineText();
	boost::thread* OnCommandThread;
	boost::thread* ComReadThread;
	boost::thread* ComWriteThread;
	//*********** TCPIP����
	void InitTCPIPClient(bool* running);
	//****************************
	//**********  ������֤ʹ�� ***********
  double verifyAverageSum;//����ƽ�����ĺͣ�������ÿ���ۼ�
  double verifyAverageSum2;//����ƽ�����ĺͣ������㣬���20��
  double verifyAverage[VERIFY_HORIZONTAL_MAX_NUM];//����ƽ����������¼20��
  int verifyCountRow;//�ۼ�����
  bool verifyComplete;//������֤�Ƿ����
  bool verifyFinishNow;//����Ҫ����֤���
  int verifyHorizonalResult;//-1��֤������ɣ�0û����Ч�У���������Ч�в���ʱ������
  float verifyHorizonalConfidence;//ˮƽ�������Ŷ�
  int verifyCountVirtical;//��ֱɨ�����
  float verifyHeightSum;//�߶��ۼӣ�������ÿ���ۼ�
  float verifyHeightSum2;//�߶��ۼӣ������㣬���10��
  float verifyHeight[VERIFY_VERTICAL_MAX_NUM];//���и߶�ֵ������¼10��
  int verifyVirticalResult;
  float verifyHeightConfidence;//�߶���֤���Ŷ�
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
	afx_msg void OnBnClickedButtonReadedge();//��ȡ�߿���
	afx_msg void OnBnClickedButtonReadguideline();//��ȡ�����ߣ������ϣ�
	afx_msg void OnBnClickedButton10();//��ͼ����
	afx_msg void OnBnClickedButtonClc();//��ͼ����
	afx_msg void OnBnClickedButton2dswitch3d();//��ͼ�л�2D3Dģʽ
	afx_msg void OnBnClickedButtonComfirmtype();//�ɻ��Ͷ�ȡ������
	afx_msg void OnBnClickedButtonReadtype();//�ɻ��Ͷ�ȡ������
	
	afx_msg void OnBnClickedButtonReadlinenum();//�ɱ�Ŷ�ȡ������
	afx_msg void OnBnClickedButtonConfirmwalkset();//ȷ���߲����ã����㣩
	afx_msg void OnBnClickedButtonClearwalkset();//����߲�����
	afx_msg void OnBnClickedButtonRedlaseron();//�򿪺�㼤��
	afx_msg void OnBnClickedButtonRedlaseroff();//�رպ�㼤��
	afx_msg void OnBnClickedButtonScanall();//����ɨ��

	afx_msg void OnBnClickedButtonWalkTestToAdjust();//�����߲����Ե���
	afx_msg void OnBnClickedButtonOffLineTest();
	afx_msg void OnBnClickedButtonGuideLineMove();
	afx_msg void OnBnClickedButtonStopLineMoveSingle();
	afx_msg void OnBnClickedButtonStopLineMove();
	afx_msg void OpenLaserCOM();
	afx_msg void CloseLaserCOM();
};
//extern boost::thread_group glb_thread_group;

//RECOG_API CLaster* _stdcall classLaserInit();//