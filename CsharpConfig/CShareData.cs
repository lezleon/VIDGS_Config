using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Threading; //Semaphore
//using System.IO.Ports;      //SerialPort
//using System.Windows.Forms;

namespace VIDGS配置软件
{
    class CShareData
    {
        //红色激光
        public enum RedLaserValue
        {
            OFF = 0,
            ON = 1
        };

        //移动方向（上下左右）
        public enum MoveDirection
        {
            Up,
            Down,
            Left,
            Right,
            None
        };

        //电机位置置位
        public enum SetZeroValue
        {
            UnSet = 0,
            Set = 1
        };

        //Z型扫描移动方向
        public enum ZScanDirection
        {
            Up,
            Down
        };

        //是否使用调试模式
        public static bool DEBUG = true;
        public static int StatuspBValue = 0;    //进度条数值0-100
        public static string StatusLableStr = "";  //状态显示,返回错误信息


        //数据保存路径(常量)
        public const String StartPath = "D:\\标定扫描\\";
        //定义激光落在箱子内的最大距离(常量)
        public const Int16 BOXMAXDISTANCE = 1500;
        //定义激光器与水平镜的距离(常量)
        public const Int16 DISTANCE_C = 66;
        //定义水平镜与垂直镜的距离(常量)
        public const Int16 DISTANCE_E = 105;
        //定义激光器与垂直镜的距离(常量)
        public const Int16 DISTANCE_CE = DISTANCE_C + DISTANCE_E;
        //水平、垂直电机步距角
        public const double XSTEPANGLE = 0.1125;
        public const double YSTEPANGLE = 0.05625;

        //定义走一圈的步数
        public const Int16 XCIRCLESTEP = 3200;  //360/0.1125 
        public const Int16 YCIRCLESTEP = 6400;  //360/0.5625 

        //边框确认参数1000次测距次数
        public const Int16 NTIMEEDGEX = 1;
        public const Int16 NTIMEEDGEY = 3;

        //确认边框和校准扫描判断的概率误差
        public const double XEDGEPROBLIMIT = 0.05;
        public const double YEDGEPROBLIMIT = 0.01;

        //扫描点数
        public const Int16 XSCANPOINTNUM = 109;
        public const Int16 YSCANPOINTNUM = 109;
        //电机位置
        public const Int16 XMOTORRANGE = (XSCANPOINTNUM - 1) / 2;
        public const Int16 YMOTORRANGE = (YSCANPOINTNUM - 1) / 2;


        //调试用变量
        public static int TempInt1 = -1;
        public static int TempInt2 = -1;
        public static string TempStr = "";
        public static string TempStr1 = "";

        //串口serialPort
        public static byte[] spReceivedData = new byte[4096];   //the Received data of serialPort    
        public static int spReceivedDataLen = 0;                //the real len of Received Data
        public static int ReadDataLen = 0;                      //命令中读出的长度
        public static bool ReadContinueFalg = false;            //是否完成此次接收
        public static int spReceivedDataLenIdeal = 0;           //the ideal len of Received Data
        public static Semaphore spSem = new Semaphore(0, 1, "serialPortSem");    //serialPort wait semaphore
        public static Mutex spMutex = new Mutex();
        public static Int32 spSemTimeout = 2000;          //the max wait time for WaitOne()
        public static bool spErrorFlag = false;                 //the error flag of serialPort
        // public static int ReadBytes = 0;                //the num of data in buffer


        //按钮事件，查看单点测距的距离值
        public static int[] ViewOnePointDistance = new int[1000];   //the value of measure distance
        public static int ViewOnePointDistanceLen = 0;              //the length of measure distance        
        public static bool ViewOnePointDistanceFlag = false;        //the error flag of OnePointMeasureFunc
        public static double ViewOnePointDistanceValidProb = 0;     //the probability of short + long
        public static double ViewOnePointDistance4Prob = 0;         //the probability of error distance(4)
        public static double ViewOnePointDistanceShortProb = 0;     //the probability of short distance(300-500)
        public static double ViewOnePointDistanceLongProb = 0;     //the probability of long distance（>2000)

        //单点测距的距离值
        public static int[] OnePointDistance = new int[1000];   //the value of measure distance
        public static int OnePointDistanceLen = 0;              //the length of measure distance        
        public static bool OnePointDistanceFlag = false;        //the error flag of OnePointMeasureFunc
        public static double OnePointDistanceValidProb = 0;     //the probability of valid distance
        //单点测距的距离值,返回原始数据
        public static int[] OrgOnePointDistance = new int[1000];   //the value of measure distance
        public static int OrgOnePointDistanceLen = 0;              //the length of measure distance        
        public static bool OrgOnePointDistanceFlag = false;        //the error flag of OnePointMeasureFunc
        //单点测距的距离值,只统计落在框外的数据
        public static int[] OutOnePointDistance = new int[1000];   //the value of measure distance
        public static int OutOnePointDistanceLen = 0;              //the length of measure distance        
        public static bool OutOnePointDistanceFlag = false;        //the error flag of OnePointMeasureFunc
        public static double OutOnePointDistanceValidProb = 0;     //the probability of valid distance

        //多次单点测距的距离值
        public static int[] NTimeOnePointDistance = new int[20000];   //the value of measure distance
        public static int NTimeOnePointDistanceLen = 0;              //the length of measure distance        
        public static bool NTimeOnePointDistanceFlag = false;        //the error flag of OnePointMeasureFunc   

        //水平扫描
        public static bool HrzScanFlag = false;                 //the error flag of HrzScanFunc
        //Z型扫描
        public static bool Z01ScanFlag = false;                 //the error flag of Z01ScanFunc
        //垂直扫描
        public static bool VerScanFlag = false;                 //the error flag of VerScanFunc

        //Z06扫描
        public static bool Z06ScanFlag = false;                 //the error flag of Z06ScanFunc

        //电机位置
        public static int XShift = 0;                           //the shift step of X motor
        public static int YShift = 0;                           //the shift step of Y motor    

        //边框确认参数
        public static Int16 XEdgeStepSave = 80;                  //X:the steps for save after confirm edge
        public static double XProbSave = 0.5;                     //X:the probability for save after confirm edge
        public static Int16 YEdgeStepSave = 285;                  //Y:the steps for save after confirm edge
        public static double YProbSave = 0.5;                     //Y:the probability for save after confirm edge

        public static double ViewProb = 0;                      //the probability after click btnViweProb

        //Q12位置
        public static Int16 Q12Steps;

        //校准参数
        public static Int16 XAdjustStepError = 100;               //X:the Error steps after Adjust
        public static double XAdjustProb = 0.5;                   //X:the probability after Adjust
        public static Int16 YAdjustStepError = 100;               //Y:the Error steps after Adjust
        public static double YAdjustProb = 0.5;                   //Y:the probability after Adjust


        //水平、垂直标定
        public static Int32 InstallHeight = 5000;                //the install height of VIDGS
        public static double CalibMeasureDistance = 0;                //the measure distance of laser 
        public static double CalibHrzDistance = 0;                //the measure distance of laser 

        public static Int16 CalibHrzZeroMove = 0;                //水平标定后，零点偏移 

        //粗略回零
        public static Int16 RoughReturnZeroJudgeTime = 0;       //the Scan Times for RoughReturnZero


        //保存操作记录
        public static string OperateRecordStr = "加载配置文件！\r\n";



        //确认边框返回值
        public enum ReturnValueEdge
        {
            SUCCESS,
            SUCCESS_HRZ,    //仅水平确认边框成功
            SUCCESS_VER,    //仅垂直确认边框成功
            FAILURE,        //一般不会出现，主要指扫描，测距等子函数出错

            ERROR_HRZ_ZERO,     //零点不合理，导致找不到边框
            ERROR_HRZ_STEP,     //单步移动超时
            ERROR_HRZ_SCAN,     //未扫描到边框
            ERROR_HRZ_PROB,     //概率不在0.2-0.8之间

            ERROR_VER_ZERO,     //零点不合理，导致找不到边框
            ERROR_VER_STEP,     //单步移动超时
            ERROR_VER_SCAN,     //未扫描到边框
            ERROR_VER_PROB,     //概率不在0.2-0.8之间
        };

        //校准返回值
        public enum ReturnValueAdjust
        {
            SUCCESS,
            SUCCESS_HRZ,    //仅水平确认边框成功
            SUCCESS_VER,    //仅垂直确认边框成功
            FAILURE,        //一般不会出现，主要指扫描，测距等子函数出错

            ERROR_HRZ_SCAN,     //未扫描到边框
            ERROR_HRZ_PROB,     //与确认边框概率差别较大

            ERROR_VER_SCAN,     //未扫描到边框
            ERROR_VER_PROB,     //与确认边框概率差别较大
        };


        //水平标定返回值
        public enum ReturnValueCalibHrz
        {
            SUCCESS,
            FAILURE,            //一般不会出现，主要指扫描，测距等子函数出错
            ERROR_MEASURE_TIMEOUT,      //定点测距超时
            ERROR_YSTEP,        //垂直步数不合里 
            ERROR_POSITION,     //人的位置不合理
            ERROR_LR_EDGE   //定点测距时未找到左右边界
        };

        //垂直标定返回值
        public enum ReturnValueCalibVer
        {
            SUCCESS,
            FAILURE,            //一般不会出现，主要指扫描，测距等子函数出错
            ERROR_MEASURE,      //定点测距错误
            ERROR_ANGLE,        //角度值不合法    
            ERROR_VALID_PROB,   //概率未满足，有效值太小(该位置不行)
            ERROR_STEP,         //计算出步数不合理
        };






    }
}
