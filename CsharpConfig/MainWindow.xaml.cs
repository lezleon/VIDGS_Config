using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Threading;
using Xceed.Wpf.AvalonDock.Layout;
using Xceed.Wpf.AvalonDock.Layout.Serialization;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Interop;
using System.IO;
using System.IO.Ports;
using System.Runtime.InteropServices;
using System.Configuration;
using Microsoft.Win32;
using System.Diagnostics;
using System.Xml.Serialization;
using System.Xml;
using System.ComponentModel;
using MahApps.Metro.Controls;
using System.Globalization;
using MahApps.Metro.Controls.Dialogs;
using System.Resources;
using System.Collections;
using Xceed.Wpf.AvalonDock;
using Xceed.Wpf.Toolkit;
using System.Text.RegularExpressions;
using PvNET;
using Susi4.APIs;

namespace VIDGS配置软件
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    #region 数据与委托
    public class KUAN : DependencyObject
    {
        //注册依赖项属性
        public static readonly DependencyProperty WidthProperty =
            DependencyProperty.Register("Width",
            typeof(double),
            typeof(KUAN),
            new PropertyMetadata((double)0));
        public double Width
        {
            get
            {
                return (double)this.GetValue(WidthProperty);
            }
            set
            {
                this.SetValue(WidthProperty, value);
            }
        }
    }
    public struct needTransform1_2//1常规，2引导线，4走步测试坐标
    {
        public int install_high;//安装高度
        public int object_high;//对象高度
        public int object_dis;//对象距离
        public int Edit_Start_x;//+-6度
        public int Edit_End_x;
        public int Edit_StandHigh;//垂直步数
        public int Auto_StandHigh;//水平步数
        public int mid_num;//单点测距
        public int guide_line_y;//引导线y
        //CString m_strRXData;//显示状态
        //CString walktext_deflection;//显示详细情况
        //public char m_strRXData[50];//显示状态
        //public char walktext_deflection[200];//显示详细情况
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 50)]
        public string m_strRXData;//显示状态
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 200)]
        public string walktext_deflection;//显示详细情况
        //CString PlaneType4Confirm;
        //CString LineNum4Confirm;
        //public char PlaneType4Confirm[20];//机型读取引导线
        //public char LineNum4Confirm[20];//编号读取引导线
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 20)]
        public string PlaneType4Confirm;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 20)]
        public string LineNum4Confirm;

        public float guide_line_k; //引导线直线方程y=kx+b;
        public float guide_line_b;
        public int guide_line_x;

        public int x_walk;//走步测试的位置，只上行
        public int y_walk;

        public char str_guidenum;//引导线编号，由C#中的弹窗得到，只下行。需要初始化为'\0'
        public char str_stopnum;

        public float offsetforline;
    };
    public struct needTransform3//二维扫描数据，只上行
    {
        public int data_h;//点数
        //public int save_data[250];
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 250)]
        public int[] save_data;
    };
    public class ComboPlane
    {
        public string comboPlane { get; set; }
    }
    public class TimerEventArgs : EventArgs
    {
        public TimerEventArgs()
        {

        }
    }
    public enum task_working
    {
        empty_0,
        walk_set_1,
        walk_test_2,
        walk_confirm_3,
        walk_clear_4,
        model_guideline_5,
        num_guideline_6,
        confirm_spoint_7,
        window_scan_8,
        station_scan_9
    }
    public enum MouseStatus
    {
        ZoomAndPlug,//缩放和拖动
        DrawingRect,//画框
        DrawingLine,//画线
        DrawingPoint,//画点
        DrawingCircle,
        DragRect,
        DragLine,
        DragPoint,
        Delete,
        DragMidLine
    }
    public enum DragLineStatus
    {
        DragPoint1,
        DragPoint2,
        DragLine
    }
    public enum DragRectStatus
    {
        DragNWPoint,
        DragNEPoint,
        DragSWPoint,
        DragSEPoint,
        DragNLine,
        DragELine,
        DragWLine,
        DragSLine
    }
    public class MoDataWinFormEventArgs : EventArgs
    {
        public MoDataWinFormEventArgs()
        {
            //
        }
        public MoDataWinFormEventArgs(string refNum3)
        {
            this.number = refNum3;
        }
        private string number;
        public string MYNONO
        {
            get { return number; }
            set { number = value; }
        }
    }
    public class PassDataWinFormEventArgs : EventArgs
    {
        public PassDataWinFormEventArgs(string refNum)
        {
            this.number = refNum;
        }
        private string number;
        public string MY
        {
            get { return number; }
            set { number = value; }
        }
    }
    public class PassWinFormEventArgs : EventArgs
    {
        public PassWinFormEventArgs(string refG, string refS)
        {
            this._G = refG;
            this._S = refS;
        }

        private string _G;
        private string _S;

        public string Guide
        {
            get { return _G; }
            set { _G = value; }
        }
        public string Stop
        {
            get { return _S; }
            set { _S = value; }
        }
    }
    public class DrawShape
    {
        /// <summary>
        /// type为DrawingPoint时，仅使用point1；
        /// type为DrawingLine时，point1代表起始点，point2代表终止点；
        /// type为DrawingRect时，point1代表左上角，point2代表右下角；
        /// </summary>
        public MouseStatus type;
        public Point point1;
        public Point point2;
        public Color color;
        public int radius;
        public string prompt;
        public string Sforstopline;
        public object flag;
        public event EventHandler OnChange;
        public event EventHandler ChangeDone;

        public void DoChange()
        {
            if (OnChange != null) OnChange(this, EventArgs.Empty);
        }
        public void DoChangeDone()
        {
            if (ChangeDone != null) ChangeDone(this, EventArgs.Empty);
        }
        public DrawShape clone()
        {
            DrawShape newShape = new DrawShape();
            newShape.type = type;
            newShape.point1 = point1;
            newShape.point2 = point2;
            newShape.color = color;
            newShape.radius = radius;
            newShape.OnChange = OnChange;
            newShape.ChangeDone = ChangeDone;
            newShape.prompt = prompt;
            return newShape;
        }

    }
    public class Emp
    {
        public string TTime { get; set; }

        public string PPort { get; set; }

        public string TType { get; set; }

        public string FFileName { get; set; }
    }
    #endregion
    public partial class MainWindow : MetroWindow
    {
        #region 变量委托等声明
        DipAngle AngleDetector;
        CLaserFunc LaserFunc;       //zzm 定义 激光功能实现函数
        CShareData ShareData;       //zzm 定义 共享数据
        SerialPort serialPort1;//声明一个串口   
        System.Threading.Mutex mutex;//互斥
        public delegate void TimerEventArgsHandler(object sender, TimerEventArgs e);
        public event TimerEventArgsHandler TimerCon;
        Camera PvCamera;
        DispatcherTimer timer;//计时器，控制刷新率
        DispatcherTimer statusTimer;//状态检测计时器
        public static int now = 666;
        public int AxisMod = 0;//坐标轴模式
        double Xrenderi = 1;//X轴尺度系数
        double Yrenderi = 1;//Y轴尺度系数
        double Axis_x_mid = 0;//初始化X轴刻度中点
        double Axis_y_mid = 0;//初始化Y轴刻度中点
        int GuideLineNum = 5;//引导线条数
        int StopLineNum = 15;//停止线条数
        public static bool ContextMenuKB = false;//右键菜单
        public static bool TI4_OpenPara = false;//工具窗口
        private int ReadDataLen = 0;
        private bool ReadContinueFalg = false;  //是否完成此次接收
        public bool SerialComCanwork = false;//初始化时判断S能否工作
        public bool BoostIsOpen = true;//   Boost串口
        bool DeleteOrClear = false;// false:Delete , true: Clear;
        bool needDrawMidline = false;//是否画图像的垂直中心线和最佳水平位置；
        #region 依赖项
        KUAN kuan;//线条宽度绑定
        #endregion
        /// <summary> 测试用，结束后换位置
        public static bool LanguageBool = false;
        /// </summary>
        /// 带2D的用于激光设置界面
        WriteableBitmap ImageBackup;
        WriteableBitmap ImageBackup2d;
        byte[] newImageBuffer;
        byte[] newImageBuffer2d;
        WriteableBitmap writeableSource;//发现内存泄露，试试这个方法。
        WriteableBitmap writeableSource2d;

        MouseStatus nowDrawing;//当前鼠标状态
        MouseStatus nowDrawing2d;//用于ZoomIn and ZoomOut
        task_working now_task_working;//记录backgroundwordk当前操作命令
        DragLineStatus nowDragLine;
        DragRectStatus nowDragRect;
        List<DrawShape> DrawingBackup;//记录主界面当前所画的所有形状
        List<DrawShape> DeleteDrawingBackup;//记录主界面删除的形状
        List<DrawShape> DrawingBackup2d;//记录激光设置界面当前所画的所有形状
        public static List<string> stoplineFromLaser;//激光设置中的停止线集合
        public static List<string> stoplineFormVisual;//视觉绘制的停止线集合
        public static Dictionary<string, HashSet<string>> stoplineFromBoth;//两个集合的并集
        DrawShape nowShape;//主界面当前所画形状
        DrawShape lastShape;
        DrawShape nowShape2d;//激光界面当前所画形状
        DrawShape nowShape2dtemp;//NeedFreshK中所画形状
        DrawShape midline;//基准线
        DrawingVisual drawingVisual;

        Point offset;//图像偏移，暂时用不到
        int ImageWidth, ImageHeight;//图像宽度和长度
        private Point origin;//
        private DrawShape originShape;
        private Point start;
        int sensitiveDistance;//判定鼠标与Drawshape适合到了可操作距离
        int minImageBorder;
        Comparison<DrawShape> shapeComparison;
        int Width2d, Height2d;//图像宽度和长度
        private Point origin2d;
        private Point start2d;

        private BackgroundWorker backgroundWorker;//多线程
        //必要变量
        int refreshFlag1Cs = 0, refreshFlag2Cs = 0, refreshFlag3Cs = 0, refreshFlag4Cs = 0;//刷新标志位
        //ClassneedTransform1_2 ClassneedTransform1_2DataCs = new ClassneedTransform1_2();
        needTransform1_2 needTransform1_2DataCs;//与C++交互结构体
        needTransform3 needTransform3DataCs;//
        //ClassneedTransform1_2 classneedTransform1_2DataCs = new ClassneedTransform1_2();
        IntPtr laser_class;
        static IntPtr laser_class_all;
        static int bLaserInited;
        static bool laser_using = false;
        #endregion
        public MainWindow()
        {
            InitializeComponent();
            
        }

        void LaserFunc_OnLabelUpdate(object sender, LabelUpadteEventArgs e)
        {
            this.Dispatcher.BeginInvoke(new Action(() =>
            {
                Block2.Text = e.content;
                edit_StandHigh.Text = (CShareData.YShift * 2).ToString();
            }));
            //throw new NotImplementedException();
        }
        #region UI刷新，界面交互
        public void WPF2Cpp()
        {
            WPF2struct();
            if (laser_class != IntPtr.Zero)
                transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
        }
        private void CoolDown()
        {
            //button_VerticalCalibration.IsEnabled = false;
            button_Proof.IsEnabled = false;
            button_ReadRect.IsEnabled = false;
            button_PointMeasure.IsEnabled = false;
            button_OpenRed.IsEnabled = false;
            button_CloseRed.IsEnabled = false;
            button_WindowScan.IsEnabled = false; ;
            button_Clc.IsEnabled = false;
            button_Save.IsEnabled = false;
            button_Esc.IsEnabled = false;
        }//禁止按钮
        private void CoolDownOut()
        {
            button_Proof.IsEnabled = true;
            button_ReadRect.IsEnabled = true;
            button_PointMeasure.IsEnabled = true;
            button_OpenRed.IsEnabled = true;
            button_CloseRed.IsEnabled = true;
            button_WindowScan.IsEnabled = true;
            button_Clc.IsEnabled = true;
            button_Save.IsEnabled = true;
            button_Esc.IsEnabled = true;
        }//接触按钮禁止
        public void WPF2struct()
        {
            if (Install_height.Text != "")
                needTransform1_2DataCs.install_high = Convert.ToInt32(Install_height.Text);
            if (Object_height.Text != "")
                needTransform1_2DataCs.object_high = Convert.ToInt32(Object_height.Text);
            if (Object_distance.Text != "")
                needTransform1_2DataCs.object_dis = Convert.ToInt32(Object_distance.Text);
            if (edit_StandHigh.Text != "")
                needTransform1_2DataCs.Edit_StandHigh = Convert.ToInt32(edit_StandHigh.Text);
            //if (mid_num.Content != "")
            //  needTransform1_2DataCs.mid_num = Convert.ToInt32(mid_num.Content);
            if (drpModel.SelectedIndex != -1)
                needTransform1_2DataCs.PlaneType4Confirm = drpModel.SelectedValue.ToString();
            if (fuckModel.SelectedIndex != -1)
                needTransform1_2DataCs.LineNum4Confirm = fuckModel.SelectedValue.ToString();
            if (Stop_Y.Text != "")
                needTransform1_2DataCs.guide_line_y = Convert.ToInt32(Stop_Y.Text);
        }//UI控件属性更新到结构体
        public void DrawWalkTest()
        {
            int judge_direction;
            float temp_numerator, temp_denominator, temp_sign;
            if (needTransform1_2DataCs.guide_line_k != 0)
            {
                temp_numerator = needTransform1_2DataCs.guide_line_k * needTransform1_2DataCs.x_walk + needTransform1_2DataCs.guide_line_b - needTransform1_2DataCs.y_walk;
                temp_denominator = needTransform1_2DataCs.guide_line_k * needTransform1_2DataCs.guide_line_k + 1;
                temp_sign = needTransform1_2DataCs.x_walk - (needTransform1_2DataCs.y_walk - needTransform1_2DataCs.guide_line_b) / needTransform1_2DataCs.guide_line_k;
                temp_sign = temp_sign / Math.Abs(temp_sign);
                judge_direction = Convert.ToInt32(temp_sign * Math.Abs(temp_numerator) / Math.Sqrt(temp_denominator));
            }
            else
            {
                judge_direction = needTransform1_2DataCs.x_walk - needTransform1_2DataCs.guide_line_x;
            };
            if (judge_direction > 100)
            {
                //walktext_deflection.Format("偏右 %d",judge_direction);
                if (judge_direction > 300)
                {
                    //pDrawControlDC->SelectObject(&RedPen); 
                    nowShape2d.type = MouseStatus.DrawingPoint;
                    nowShape2d.color = Colors.Red;
                    nowShape2d.radius = 1;
                    nowShape2d.point1.X = (double)(needTransform1_2DataCs.x_walk - needTransform1_2DataCs.guide_line_x) * 145 / 10000 + 177;
                    nowShape2d.point1.Y = 437 - (double)needTransform1_2DataCs.y_walk * 35 / 8000;
                    DrawingBackup2d.Add(nowShape2d);
                    nowShape2d = new DrawShape();
                    NeedFreshK();
                }
                else if (judge_direction > 200)
                {
                    //pDrawControlDC->SelectObject(&OrangePen); 
                    nowShape2d.type = MouseStatus.DrawingPoint;
                    nowShape2d.color = Colors.Purple;
                    nowShape2d.radius = 1;
                    nowShape2d.point1.X = (double)(needTransform1_2DataCs.x_walk - needTransform1_2DataCs.guide_line_x) * 145 / 10000 + 177;
                    nowShape2d.point1.Y = 437 - (double)needTransform1_2DataCs.y_walk * 35 / 8000;
                    DrawingBackup2d.Add(nowShape2d);
                    nowShape2d = new DrawShape();
                    NeedFreshK();
                }
                else //if (judge_direction>100)
                {
                    //pDrawControlDC->SelectObject(&GreenPen);
                    nowShape2d.type = MouseStatus.DrawingPoint;
                    nowShape2d.color = Colors.Blue;
                    nowShape2d.radius = 1;
                    nowShape2d.point1.X = (double)(needTransform1_2DataCs.x_walk - needTransform1_2DataCs.guide_line_x) * 145 / 10000 + 177;
                    nowShape2d.point1.Y = 437 - (double)needTransform1_2DataCs.y_walk * 35 / 8000;
                    DrawingBackup2d.Add(nowShape2d);
                    nowShape2d = new DrawShape();
                    NeedFreshK();
                }
            }
            else if (judge_direction < -100)
            {
                //walktext_deflection.Format("偏左 %d",judge_direction);
                if (judge_direction < -300)
                {
                    //pDrawControlDC->SelectObject(&RedPen); 
                    nowShape2d.type = MouseStatus.DrawingPoint;
                    nowShape2d.color = Colors.Red;
                    nowShape2d.radius = 1;
                    nowShape2d.point1.X = (double)(needTransform1_2DataCs.x_walk - needTransform1_2DataCs.guide_line_x) * 145 / 10000 + 177;
                    nowShape2d.point1.Y = 437 - (double)needTransform1_2DataCs.y_walk * 35 / 8000;
                    DrawingBackup2d.Add(nowShape2d);
                    nowShape2d = new DrawShape();
                    NeedFreshK();
                }
                else if (judge_direction < -200)
                {
                    //pDrawControlDC->SelectObject(&OrangePen);
                    nowShape2d.type = MouseStatus.DrawingPoint;
                    nowShape2d.color = Colors.Purple;
                    nowShape2d.radius = 1;
                    nowShape2d.point1.X = (double)(needTransform1_2DataCs.x_walk - needTransform1_2DataCs.guide_line_x) * 145 / 10000 + 177;
                    nowShape2d.point1.Y = 437 - (double)needTransform1_2DataCs.y_walk * 35 / 8000;
                    DrawingBackup2d.Add(nowShape2d);
                    nowShape2d = new DrawShape();
                    NeedFreshK();
                }
                else //if (judge_direction>100)
                {
                    //pDrawControlDC->SelectObject(&GreenPen);
                    nowShape2d.type = MouseStatus.DrawingPoint;
                    nowShape2d.color = Colors.Blue;
                    nowShape2d.radius = 1;
                    nowShape2d.point1.X = (double)(needTransform1_2DataCs.x_walk - needTransform1_2DataCs.guide_line_x) * 145 / 10000 + 177;
                    nowShape2d.point1.Y = 437 - (double)needTransform1_2DataCs.y_walk * 35 / 8000;
                    DrawingBackup2d.Add(nowShape2d);
                    nowShape2d = new DrawShape();
                    NeedFreshK();
                }
            }
            else
            {
                //walktext_deflection.Format("居中 %d", judge_direction);
                //pDrawControlDC->SelectObject(&BluePen);
                nowShape2d.type = MouseStatus.DrawingPoint;
                nowShape2d.color = Colors.Green;
                nowShape2d.radius = 1;
                nowShape2d.point1.X = (double)(needTransform1_2DataCs.x_walk - needTransform1_2DataCs.guide_line_x) * 145 / 10000 + 177;
                nowShape2d.point1.Y = 437 - (double)needTransform1_2DataCs.y_walk * 35 / 8000;
                DrawingBackup2d.Add(nowShape2d);
                nowShape2d = new DrawShape();
                NeedFreshK();
            }
            //x_walk_T = needTransform1_2DataCs.x_walk;
        }//走步测试画点
        public void FreshUI()//int bThreading = 0)
        {
            if (laser_class == IntPtr.Zero) return;
            while (true)
            {
                refreshFlag1Cs = trans1Flag_std(laser_class, 1);
                refreshFlag2Cs = trans1Flag_std(laser_class, 2);
                refreshFlag3Cs = trans1Flag_std(laser_class, 3);
                refreshFlag4Cs = trans1Flag_std(laser_class, 4);
                if (refreshFlag1Cs == 1 || refreshFlag1Cs == 2)//刷新界面
                {
                    if (refreshFlag1Cs == 1)
                    {
                        refreshFlag1Cs = 0; setFlagTo_std(laser_class, 0, 1);
                    }//刷新并跳出
                    else if (refreshFlag1Cs == 2)
                    {
                        refreshFlag1Cs = -1; setFlagTo_std(laser_class, -1, 1);
                    }//刷新并继续刷新
                    //刷新界面
                    transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, false);//C++ -> C#
                    this.Dispatcher.BeginInvoke(new Action(() =>
                    {
                        Install_height.Text = needTransform1_2DataCs.install_high.ToString();
                        Object_height.Text = needTransform1_2DataCs.object_high.ToString();
                        Object_distance.Text = needTransform1_2DataCs.object_dis.ToString();
                        edit_StandHigh.Text = needTransform1_2DataCs.Edit_StandHigh.ToString();
                        //mid_num.Content = needTransform1_2DataCs.mid_num.ToString();
                        Stop_Y.Text = needTransform1_2DataCs.guide_line_y.ToString();
                        Block1.Text = needTransform1_2DataCs.m_strRXData;
                        Block2.Text = needTransform1_2DataCs.walktext_deflection;
                    }), null);

                }
                if (refreshFlag2Cs == 1 || refreshFlag2Cs == 2)//画引导线
                {
                    if (refreshFlag2Cs == 1) { refreshFlag2Cs = 0; setFlagTo_std(laser_class, 0, 2); }//刷新并跳出
                    else if (refreshFlag2Cs == 2) { refreshFlag2Cs = -1; setFlagTo_std(laser_class, -1, 2); }//刷新并继续刷新
                    transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, false);
                    //画引导线
                    AxisMod = 2;
                    if (needTransform1_2DataCs.guide_line_k != 0)
                    {
                        nowShape2d.type = MouseStatus.DrawingLine;
                        nowShape2d.color = Colors.Yellow;
                        nowShape2d.point1.X = 177;
                        nowShape2d.point1.Y = 437 - (double)needTransform1_2DataCs.guide_line_y * 35 / 8000;
                        nowShape2d.point2.X = (80000 - (double)needTransform1_2DataCs.guide_line_y) * 145 / (10000 * (double)needTransform1_2DataCs.guide_line_k) + 177;
                        nowShape2d.point2.Y = 87;
                        Axis_x_mid = needTransform1_2DataCs.guide_line_x;
                        Axis_y_mid = 40000;
                        Xrenderi = 1;
                        Yrenderi = 1;
                        DrawingBackup2d.Add(nowShape2d);
                        nowShape2d = new DrawShape();
                        nowShape2d.type = MouseStatus.DrawingLine;
                        nowShape2d.color = Colors.Black;
                        nowShape2d.point1.X = 157;
                        nowShape2d.point1.Y = 437 - (double)needTransform1_2DataCs.guide_line_y * 35 / 8000;
                        nowShape2d.point2.X = 197;
                        nowShape2d.point2.Y = 437 - (double)needTransform1_2DataCs.guide_line_y * 35 / 8000;
                        DrawingBackup2d.Add(nowShape2d);
                        nowShape2d = new DrawShape();
                        NeedFreshK();
                    }
                }
                if (refreshFlag3Cs == 1 || refreshFlag3Cs == 2)//画二维点
                {
                    if (refreshFlag3Cs == 1) { refreshFlag3Cs = 0; setFlagTo_std(laser_class, 0, 3); }//刷新并跳出
                    else if (refreshFlag3Cs == 2) { refreshFlag3Cs = -1; setFlagTo_std(laser_class, -1, 3); }//刷新并继续刷新
                    transform3Struct_std(laser_class, ref needTransform3DataCs, false);
                    //画二维点
                    AxisMod = 1;
                    for (int temp3 = 0; temp3 < 109; temp3++)
                    {
                        nowShape2d.type = MouseStatus.DrawingPoint;
                        nowShape2d.color = Colors.Brown;
                        nowShape2d.point1.X = 64 + (double)temp3 * 193 / 109;
                        nowShape2d.point1.Y = 437 - (double)needTransform3DataCs.save_data[temp3] * 35 / 8000;
                        DrawingBackup2d.Add(nowShape2d);
                        nowShape2d = new DrawShape();
                    }
                    NeedFreshK();
                }
                if (refreshFlag4Cs == 1 || refreshFlag4Cs == 2)//画走步测试
                {
                    if (refreshFlag4Cs == 1) { refreshFlag4Cs = 0; setFlagTo_std(laser_class, 0, 4); }//刷新并跳出
                    else if (refreshFlag4Cs == 2) { refreshFlag4Cs = -1; setFlagTo_std(laser_class, -1, 4); }//刷新并继续刷新
                    transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, false);
                    //画走步测试
                    //DrawWalkTest();
                }
                if (refreshFlag1Cs == 0 && refreshFlag2Cs == 0 && refreshFlag3Cs == 0 && refreshFlag4Cs == 0)
                    break;
                //System.Threading.Thread.Sleep(50);
            }
            //    });
        }
        private void backgroundWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            if (laser_class == IntPtr.Zero) return;
            int back = 1;
            bool bNeedFreshUI = false;
            this.Dispatcher.BeginInvoke(new Action(() =>
            {
                _startshine();
            }), null);
            while (true)
            {
                bNeedFreshUI = false;
                refreshFlag1Cs = trans1Flag_std(laser_class, 1);
                refreshFlag2Cs = trans1Flag_std(laser_class, 2);
                refreshFlag3Cs = trans1Flag_std(laser_class, 3);
                refreshFlag4Cs = trans1Flag_std(laser_class, 4);
                if (refreshFlag1Cs == -1 || refreshFlag1Cs == 2)//刷新界面
                {
                    bNeedFreshUI = true;
                }
                if (refreshFlag2Cs == -1 || refreshFlag2Cs == 2)//画引导线
                {
                    bNeedFreshUI = true;
                }
                if (refreshFlag3Cs == -1 || refreshFlag3Cs == 2)//画二维点
                {
                    bNeedFreshUI = true;
                }
                if (refreshFlag4Cs == -1 || refreshFlag4Cs == 2)//画走步测试
                {
                    bNeedFreshUI = true;
                }
                if (bNeedFreshUI)
                {
                    back = -back;
                    backgroundWorker.ReportProgress(back);
                }

                if (refreshFlag1Cs == 1 || refreshFlag2Cs == 1 || refreshFlag3Cs == 1 || refreshFlag4Cs == 1)
                {
                    backgroundWorker.ReportProgress(100);
                    break;//from while()
                }
                System.Threading.Thread.Sleep(50);
            }
        }
        private void backgroundWorker_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            if (laser_class == IntPtr.Zero) return;
            if (refreshFlag1Cs == 1 || refreshFlag1Cs == 2)//刷新界面
            {
                if (refreshFlag1Cs == 1)
                {
                    refreshFlag1Cs = 0; setFlagTo_std(laser_class, 0, 1);
                }//刷新并跳出
                else if (refreshFlag1Cs == 2)
                {
                    refreshFlag1Cs = -1; setFlagTo_std(laser_class, -1, 1);
                }//刷新并继续刷新
                //刷新界面
                transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, false);//C++ -> C#
                this.Dispatcher.BeginInvoke(new Action(() =>
                {
                    Install_height.Text = needTransform1_2DataCs.install_high.ToString();
                    Object_height.Text = needTransform1_2DataCs.object_high.ToString();
                    Object_distance.Text = needTransform1_2DataCs.object_dis.ToString();
                    edit_StandHigh.Text = needTransform1_2DataCs.Edit_StandHigh.ToString();
                    //mid_num.Content = needTransform1_2DataCs.mid_num.ToString();
                    Stop_Y.Text = needTransform1_2DataCs.guide_line_y.ToString();
                    /*Stop_Y.Text = "123";*/
                    Block1.Text = needTransform1_2DataCs.m_strRXData;
                    Block2.Text = needTransform1_2DataCs.walktext_deflection;
                }), null);
            }
            if (refreshFlag2Cs == 1 || refreshFlag2Cs == 2)//画引导线
            {
                if (refreshFlag2Cs == 1) { refreshFlag2Cs = 0; setFlagTo_std(laser_class, 0, 2); }//刷新并跳出
                else if (refreshFlag2Cs == 2) { refreshFlag2Cs = -1; setFlagTo_std(laser_class, -1, 2); }//刷新并继续刷新
                transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, false);

                //画引导线
                AxisMod = 2;
                if (needTransform1_2DataCs.guide_line_k != 0)
                {
                    nowShape2d.type = MouseStatus.DrawingLine;
                    nowShape2d.color = Colors.Yellow;
                    nowShape2d.point1.X = 177;
                    nowShape2d.point1.Y = 437 - (double)needTransform1_2DataCs.guide_line_y * 35 / 8000;
                    nowShape2d.point2.X = (80000 - (double)needTransform1_2DataCs.guide_line_y) * 145 / (10000 * (double)needTransform1_2DataCs.guide_line_k) + 177;
                    nowShape2d.point2.Y = 87;
                    Axis_x_mid = needTransform1_2DataCs.guide_line_x;
                    Axis_y_mid = 40000;
                    Xrenderi = 1;
                    Yrenderi = 1;
                    DrawingBackup2d.Add(nowShape2d);
                    nowShape2d = new DrawShape();
                    nowShape2d.type = MouseStatus.DrawingLine;
                    nowShape2d.color = Colors.Black;
                    nowShape2d.point1.X = 157;
                    nowShape2d.point1.Y = 437 - (double)needTransform1_2DataCs.guide_line_y * 35 / 8000;
                    nowShape2d.point2.X = 197;
                    nowShape2d.point2.Y = 437 - (double)needTransform1_2DataCs.guide_line_y * 35 / 8000;
                    DrawingBackup2d.Add(nowShape2d);
                    nowShape2d = new DrawShape();
                    NeedFreshK();
                }
            }
            if (refreshFlag3Cs == 1 || refreshFlag3Cs == 2)//画二维点
            {
                if (refreshFlag3Cs == 1) { refreshFlag3Cs = 0; setFlagTo_std(laser_class, 0, 3); }//刷新并跳出
                else if (refreshFlag3Cs == 2) { refreshFlag3Cs = -1; setFlagTo_std(laser_class, -1, 3); }//刷新并继续刷新
                transform3Struct_std(laser_class, ref needTransform3DataCs, false);
                Install_height.Text = needTransform1_2DataCs.install_high.ToString();
                Object_height.Text = needTransform1_2DataCs.object_high.ToString();
                Object_distance.Text = needTransform1_2DataCs.object_dis.ToString();
                edit_StandHigh.Text = needTransform1_2DataCs.Edit_StandHigh.ToString();
                //mid_num.Content = needTransform1_2DataCs.mid_num.ToString();
                Stop_Y.Text = needTransform1_2DataCs.guide_line_y.ToString();
                /*Stop_Y.Text = "123";*/
                Block1.Text = needTransform1_2DataCs.m_strRXData;
                Block2.Text = needTransform1_2DataCs.walktext_deflection;
                //画二维点
                AxisMod = 1;
                for (int temp3 = 0; temp3 < 109; temp3++)
                {
                    nowShape2d.type = MouseStatus.DrawingPoint;
                    nowShape2d.color = Colors.Brown;
                    nowShape2d.point1.X = 64 + (double)temp3 * 193 / 109;
                    nowShape2d.point1.Y = 437 - (double)needTransform3DataCs.save_data[temp3] * 35 / 8000;
                    DrawingBackup2d.Add(nowShape2d);
                    nowShape2d = new DrawShape();
                }
                NeedFreshK();

            }
            if (refreshFlag4Cs == 1 || refreshFlag4Cs == 2)//画走步测试
            {
                if (refreshFlag4Cs == 1) { refreshFlag4Cs = 0; setFlagTo_std(laser_class, 0, 4); }//刷新并跳出
                else if (refreshFlag4Cs == 2) { refreshFlag4Cs = -1; setFlagTo_std(laser_class, -1, 4); }//刷新并继续刷新
                transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, false);
                AxisMod = 2;
                Axis_x_mid = needTransform1_2DataCs.guide_line_x;
                Axis_y_mid = 40000;
                Xrenderi = 1;
                Yrenderi = 1;
                //画走步测试
                DrawWalkTest();
            }

        }
        private void backgroundWorker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            backgroundWorker.CancelAsync();
            this.Dispatcher.BeginInvoke(new Action(() =>
            {
                _stopshine();
                switch (now_task_working)
                {
                    case task_working.walk_set_1:
                        Block1.Text = "走步设置执行完毕";
                        break;
                    case task_working.walk_test_2:
                        Block1.Text = "走步测试执行完毕";
                        break;
                    case task_working.walk_confirm_3:
                        Block1.Text = "走步确认执行完毕";
                        break;
                    case task_working.walk_clear_4:
                        Block1.Text = "走步清空执行完毕";
                        break;
                    case task_working.model_guideline_5:
                        Block1.Text = "机型读取引导线执行完毕";
                        break;
                    case task_working.num_guideline_6:
                        Block1.Text = "编号读取引导线执行完毕";
                        break;
                    case task_working.confirm_spoint_7:
                        Block1.Text = "验证停止点执行完毕";
                        break;
                    case task_working.window_scan_8:
                        Block1.Text = "窗口扫描执行完毕";
                        break;
                    case task_working.station_scan_9:
                        Block1.Text = "站坪扫描执行完毕";
                        string LocalLogDir = Parameter.glb_Parameter.system_parameter["LogSaveRoot"].InnerText;
                        if (LocalLogDir.StartsWith("\"") && LocalLogDir.EndsWith("\"")) LocalLogDir = LocalLogDir.Substring(1, LocalLogDir.Length - 2);
                        DirectoryInfo di = new DirectoryInfo(LocalLogDir);
                        DirectoryInfo newestFile = di.GetDirectories().OrderBy(n => n.LastWriteTime).Last();
                        string s = newestFile.Name;
                        Log log = new Log();
                        log.Owner = this;
                        log.Title = "站坪扫描记录" + "-" + s;
                        log.filename = s;
                        log.fileflag1 = s.Substring(s.IndexOf("-") + 1, 19);
                        log.fileflag2 = s.Substring(23, s.Length - 23);
                        log.AddHeightLine();
                        log.AddOffsetLine();
                        log.Show();
                        log.Activate();
                        LinearGradientBrush brush = new LinearGradientBrush();
                        brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                        brush.GradientStops.Add(new GradientStop(color: Colors.Green, offset: 1));
                        Checkbox_Problem.Fill = brush;
                        break;
                    case task_working.empty_0:
                        break;
                }
                now_task_working = task_working.empty_0;   
                OpenCOM(serialPort1);
            }), null);
            //CoolDownOut();
        }
        #endregion
        #region 声明接口
        [DllImport("config.dll")]
        public static extern IntPtr classLaserInit();//类对象
        [DllImport("config.dll")]
        public static extern void classLaserDestroy(IntPtr laser_class);//退出按钮
        [DllImport("config.dll")]
        public static extern void OnBnClickedConnect_std(IntPtr laser_class);//启动按钮
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonReadtype_std(IntPtr laser_class);//由机型读取停止线
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonReadlinenum_std(IntPtr laser_class);//由编号读取停止线
        [DllImport("config.dll")]
        public static extern void OnBnClickedStopline_std(IntPtr laser_class);//编号

        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonDetect_std(IntPtr laser_class);//校准
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonReadedge_std(IntPtr laser_class);//读取边框
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonLeft_std(IntPtr laser_class);//左移1步
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonRight_std(IntPtr laser_class);//右移1步
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonDown_std(IntPtr laser_class);//下移1步
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonUp_std(IntPtr laser_class);//上移1步
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonLocate_std(IntPtr laser_class);//单点测量
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonRedlaseron_std(IntPtr laser_class);//打开红色激光
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonRedlaseroff_std(IntPtr laser_class);//关闭红色激光
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonScanall_std(IntPtr laser_class);//窗口扫描
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonEdge_std(IntPtr laser_class);//确定边框
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonSetzero_std(IntPtr laser_class);//置为零点
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonReset_std(IntPtr laser_class);//回到零点
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonTwod_std(IntPtr laser_class);//二维扫描
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonUpten_std(IntPtr laser_class);//上移10步
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonLeftten_std(IntPtr laser_class);//向左10步
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonDownten_std(IntPtr laser_class);//下移10步
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonRightten_std(IntPtr laser_class);//向右10步
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonHrzcalibration_std(IntPtr laser_class);//水平标定
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonCalibration_std(IntPtr laser_class);//垂直标定
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonScanYard_std(IntPtr laser_class);//站坪扫描
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonScanYardToAdjust_std(IntPtr laser_class);//站坪扫描后校准
        [DllImport("config.dll")]
        public static extern void OnBnClickedWalkset_std(IntPtr laser_class);//走步设置
        [DllImport("config.dll")]
        public static extern void OnBnClickedWalktest_std(IntPtr laser_class);//走步测试
        [DllImport("config.dll")]
        public static extern void OnBnClickedBtnClear_std(IntPtr laser_class);//停止
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonConfirmwalkset_std(IntPtr laser_class);//确认
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonClearwalkset_std(IntPtr laser_class);//清空
        //[DllImport("config.dll")]
        //public static extern void OnBnClickedStopline_std(IntPtr laser_class);//编号

        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonComfirmtype_std(IntPtr laser_class);//验证停止点
        [DllImport("config.dll")]
        public static extern void OnBnClickedSave_std(IntPtr laser_class);//保存
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonOffLineTest_std(IntPtr laser_class);//离线测试
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonGuideLineMove_std(IntPtr laser_class);//引导线整体移动
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonStopLineMoveSingle_std(IntPtr laser_class);//停止线单独移动
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonStopLineMove_std(IntPtr laser_class);//停止线整体移动
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonBoostOn_std(IntPtr laser_class);//Boost串口打开
        [DllImport("config.dll")]
        public static extern void OnBnClickedButtonBoostClose_std(IntPtr laser_class);//Boost串口关闭

        [DllImport("config.dll")]
        public static extern void transform1_2Struct_std(IntPtr laser_class, ref needTransform1_2 needTransform1_2DataCs, bool mode);
        [DllImport("config.dll")]
        public static extern void transform3Struct_std(IntPtr laser_class, ref needTransform3 needTransform3DataCs, bool mode);

        [DllImport("config.dll")]
        public static extern void testNoSense(IntPtr laser_class);//无用，保留

        [DllImport("config.dll")]
        public static extern void trans1pointerInt_std(IntPtr laser_class, [MarshalAs(UnmanagedType.I4)]ref int a, [MarshalAs(UnmanagedType.I4)]int idx);
        [DllImport("config.dll")]
        public static extern int trans1Flag_std(IntPtr laser_class, [MarshalAs(UnmanagedType.I4)]int idx);
        [DllImport("config.dll")]
        public static extern int setFlagTo_std(IntPtr laser_class, [MarshalAs(UnmanagedType.I4)]int flagValue, [MarshalAs(UnmanagedType.I4)]int idx);
        [DllImport("config.dll")]
        public static extern void WriteLedMessage(int flagValue, float idx, string led_time, string title);

        [DllImport("config.dll")]
        public static extern byte GetLEDStatus();//LED连通状态 
        [DllImport("config.dll")]
        public static extern byte GetTCPIPStatus();//TCPIP连通状态

        [DllImport("gdi32")]
        private static extern int DeleteObject(IntPtr o);
        #endregion
        private void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            {//MainWindow()
                stoplineFromLaser = new List<string>();//激光设置中的停止线集合
                stoplineFormVisual = new List<string>();//视觉绘制的停止线集合
                stoplineFromBoth = new Dictionary<string, HashSet<string>>();//两个集合的并集
                needTransform1_2DataCs = new needTransform1_2();//与C++交互结构体
                needTransform3DataCs = new needTransform3();//
                Parameter.glb_Parameter = new Parameter(@"test.xml");
                datePicker1.SelectedDate = System.DateTime.Now.AddDays(-6);
                datePicker2.SelectedDate = System.DateTime.Now.AddDays(1);

                CsharpConfig_Start();//Mutex控制程序只打开一次

                LaserFunc = new CLaserFunc();   //zzm 类 激光功能实现函数类
                LaserFunc.OnLabelUpdate += LaserFunc_OnLabelUpdate;
                ShareData = new CShareData();   //zzm 类 激光功能实现函数类
                //ReadConfigData();   //zzm 加载配置文件
                Susi4.APIs.SusiGPIO.InitializeLib();
                Susi4.APIs.SusiGPIO.InitializeGPIO();
                if (Susi4.APIs.SusiGPIO.Available)
                {
                    Susi4.APIs.SusiGPIO.InitializePins();
                    Susi4.APIs.SusiGPIO.SetDirection();
                }

                nowShape = new DrawShape();
                nowShape2d = new DrawShape();
                midline = new DrawShape();
                lastShape = nowShape;
                DrawingBackup = new List<DrawShape>();
                DeleteDrawingBackup = new List<DrawShape>();
                DrawingBackup2d = new List<DrawShape>();
                offset = new Point(2, 0);

                TransformGroup group = new TransformGroup();
                TransformGroup group2d = new TransformGroup();

                ScaleTransform xform = new ScaleTransform();
                ScaleTransform xform2d = new ScaleTransform();
                group.Children.Add(xform);
                group2d.Children.Add(xform2d);

                TranslateTransform tt = new TranslateTransform();
                TranslateTransform tt2d = new TranslateTransform();
                group.Children.Add(tt);
                group2d.Children.Add(tt2d);

                CameraImageBox.RenderTransform = group;
                Image2dBox.RenderTransform = group2d;
                sensitiveDistance = Convert.ToInt32(ConfigurationManager.AppSettings["SensitiveDistance"]);//鼠标点击时响应距离
                minImageBorder = Convert.ToInt32(ConfigurationManager.AppSettings["MinImageBorder"]);
                backgroundWorker = ((BackgroundWorker)this.FindResource("backgroundWorker"));//多线程backgroundWorker

                ImageToolTip.Width = double.NaN;
                ImageToolTip.Height = double.NaN;
                shapeComparison = delegate(DrawShape shape1, DrawShape shape2)
                {
                    if (shape1.type == shape2.type)
                    {
                        if (shape1.type == MouseStatus.DrawingPoint)
                        {
                            if (shape1.flag != null && shape2.flag != null)
                                return ((int)(shape1.flag) - (int)(shape2.flag)) * 10000 + (int)(shape2.point1.Y - shape1.point1.Y + 0.5);
                            else
                                return (int)(shape2.point1.Y - shape1.point1.Y + 0.5);
                        }
                        else if (shape1.type == MouseStatus.DrawingRect)
                            return (int)(shape1.point1.Y - shape2.point1.Y + 0.5);
                        else if (shape1.type == MouseStatus.DrawingLine)
                            return (int)(shape2.point1.Y - shape1.point1.Y + 0.5);
                        else return (int)(shape1.point1.Y - shape2.point1.Y + 0.5);
                    }
                    else
                        return shape1.type - shape2.type;
                };
                PvCamera = new Camera();
                try
                {
                    List<ComboPlane> CPList = new List<ComboPlane>();
                    List<ComboPlane> CNList = new List<ComboPlane>();
                    List<Parameter.StopLine> allComboplanetype = Parameter.glb_Parameter.ReadStopLines();
                    foreach (Parameter.StopLine s in allComboplanetype)
                    {
                        string temps = s.Name.Substring(9, s.Name.Length - 9);
                        if (temps.Length == 3)
                        {
                            switch (temps.Substring(1, 2))
                            {
                                case "10":
                                    temps = temps.Replace("10", "A");
                                    break;
                                case "11":
                                    temps = temps.Replace("11", "B");
                                    break;
                                case "12":
                                    temps = temps.Replace("12", "C");
                                    break;
                                case "13":
                                    temps = temps.Replace("13", "D");
                                    break;
                                case "14":
                                    temps = temps.Replace("14", "E");
                                    break;
                                case "15":
                                    temps = temps.Replace("15", "F");
                                    break;
                                default:
                                    break;
                            }
                        }
                        stoplineFromLaser.Add(temps);
                        CNList.Add(new ComboPlane() { comboPlane = temps });
                        foreach (string t in s.planeTypes)
                        {
                            CPList.Add(new ComboPlane() { comboPlane = t });
                        }
                    }
                    drpModel.ItemsSource = CPList;
                    drpModel.DisplayMemberPath = "comboPlane";
                    drpModel.SelectedValuePath = "comboPlane";
                    fuckModel.ItemsSource = CNList;
                    fuckModel.DisplayMemberPath = "comboPlane";
                    fuckModel.SelectedValuePath = "comboPlane";
                }
                catch (Exception ex)
                {
                    Xceed.Wpf.Toolkit.MessageBox.Show(string.Format("Failed to open the configuration file！\n{0}", ex.Message));
                }
                Rect rc = SystemParameters.WorkArea;
                this.Width = rc.Width;
                this.Height = rc.Height;
                //依赖项属性实例
                #region   依赖项实例化
                kuan = new KUAN();
                #endregion
            }
            System.Drawing.Bitmap image = new System.Drawing.Bitmap(@"./welcome.png");
            System.Drawing.Bitmap image2d = new System.Drawing.Bitmap(@"./可视化3.bmp");
            CameraImageBox.Source = ToBitmapSource(image);
            Image2dBox.Source = MainWindow.ToBitmapSource(image2d);
            ImageWidth = (int)CameraImageBox.Source.Width;
            Width2d = (int)Image2dBox.Source.Width;
            ImageHeight = (int)CameraImageBox.Source.Height;
            Height2d = (int)Image2dBox.Source.Height;
            ImageBackup = new WriteableBitmap(CameraImageBox.Source as BitmapSource);
            ImageBackup2d = new WriteableBitmap(Image2dBox.Source as BitmapSource);
            ImageBackup = BitmapFactory.ConvertToPbgra32Format(ImageBackup);
            ImageBackup2d = BitmapFactory.ConvertToPbgra32Format(ImageBackup2d);
            writeableSource = ImageBackup.Clone();
            writeableSource2d = ImageBackup2d.Clone();
            newImageBuffer = new byte[2000 * 2000 * 4];
            newImageBuffer2d = new byte[2000 * 2000 * 4];
            midline.point1.X = ImageWidth * 3 / 10;//20180328
            midline.point1.Y = ImageHeight * 3 / 4;
            midline.point2.X = ImageWidth * 7 / 10;
            midline.point2.Y = ImageHeight * 3 / 4;
            midline.color = Colors.Red;
            #region 依赖项属性绑定
            BindingOperations.SetBinding(kuan, KUAN.WidthProperty,
                new Binding
                {
                    Source = XSlider,
                    Path = new PropertyPath("Value")
                });
            #endregion
            //【修改】开始
            if (0 == bLaserInited)
            {
                try
                {
                    laser_class = classLaserInit();//C++创建类（C#只能使用laser_class）
                    laser_class_all = laser_class;
                    bLaserInited = 1;
                    OnBnClickedConnect_std(laser_class);
                }
                catch (Exception ex)
                {
                    Xceed.Wpf.Toolkit.MessageBox.Show("config.dll加载失败！错误：", ex.Message);
                    //App.Current.Shutdown();
                }
            }
            else
            {
                laser_class = laser_class_all;
            }
            if (laser_class != IntPtr.Zero)
            {
                //【修改】结束
                transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, false);//C++ -> C#
                Install_height.Text = needTransform1_2DataCs.install_high.ToString();
                Object_height.Text = needTransform1_2DataCs.object_high.ToString();
                Object_distance.Text = needTransform1_2DataCs.object_dis.ToString();
                edit_StandHigh.Text = needTransform1_2DataCs.Edit_StandHigh.ToString();
                //mid_num.Content = needTransform1_2DataCs.mid_num.ToString();
                Block1.Text = needTransform1_2DataCs.m_strRXData;
                Block2.Text = needTransform1_2DataCs.walktext_deflection;
                OnBnClickedButtonBoostClose_std(laser_class);
            }
            BoostIsOpen = false;
            Thread.Sleep(50);
            try//尝试打开串口
            {
                serialPort1 = new SerialPort(Parameter.glb_Parameter.system_parameter["LASERCOM"].InnerText);
                serialPort1.BaudRate = Convert.ToInt32(Parameter.glb_Parameter.system_parameter["LASERPAUD"].InnerText);//波特率
                serialPort1.Parity = Parity.Odd;//无奇偶校验位
                serialPort1.StopBits = StopBits.One;//1个停止位
                serialPort1.ReceivedBytesThreshold = 1;//设置 DataReceived 事件发生前内部输入缓冲区中的字节数
                serialPort1.ReadBufferSize = 4096;
                serialPort1.WriteBufferSize = 2048;
                serialPort1.DataReceived += new SerialDataReceivedEventHandler(serialPort1_DataReceived);//DataReceived事件委托
                serialPort1.Open();//打开串口
                SerialComCanwork = true;
                AngleDetector = new DipAngle();
                AngleDetector.gotAngle += OnGetNewAngle;
                //LinearGradientBrush brush = new LinearGradientBrush();
                //brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                //brush.GradientStops.Add(new GradientStop(color: Colors.Green, offset: 1));
                //Checkbox_Com.Fill = brush;
            }
            catch//如果串口被其他占用，则无法打开
            {
                Xceed.Wpf.Toolkit.MessageBox.Show("无法打开串口, 请检测此串口是否有效或被其他占用！");
                SerialComCanwork = false;
                serialPort1 = null;
                //LinearGradientBrush brush = new LinearGradientBrush();
                //brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                //brush.GradientStops.Add(new GradientStop(color: Colors.Red, offset: 1));
                //Checkbox_Com.Fill = brush;
            }
            try
            {
                VistaGlassHelper.ExtendGlass(this, -1, -1, -1, -1);

            }
            // If not Vista, paint background white.
            catch //(DllNotFoundException)
            {
                this.Background = Brushes.White;
            }
            ReadLaserPatameter();
            ReadVisionParameter();

            timer = new DispatcherTimer();
            timer.Tick += new EventHandler(timer_Tick_static);
            timer.Interval = new TimeSpan(0, 0, 0, 0, 30);//30ms刷一次
            timer.Start();

            LinearGradientBrush greenBrush = new LinearGradientBrush();
            greenBrush.GradientStops.Add(new GradientStop(color: Colors.YellowGreen, offset: 0));
            greenBrush.GradientStops.Add(new GradientStop(color: Colors.Green, offset: 1));
            LinearGradientBrush redBrush = new LinearGradientBrush();
            redBrush.GradientStops.Add(new GradientStop(color: Colors.YellowGreen, offset: 0));
            redBrush.GradientStops.Add(new GradientStop(color: Colors.Red, offset: 1));
            if (PvCamera == null)
            {
                PvCamera = new Camera();
            }
            if (!PvCamera.Inited)
            {
                try
                {
                    tErr err = PvCamera.InitCamera();
                    if (err != tErr.eErrSuccess)
                    {
                        System.Windows.MessageBox.Show("相机初始化失败！" + "错误：" + err.ToString());
                        Checkbox_Camera.Fill = redBrush;
                        Checkbox_Camera.ToolTip = "相机初始化失败！" + "错误：" + err.ToString();
                    }
                    else
                    {
                        //Task CheckOpenTask = Task.Factory.StartNew(CheckCameraOpen);
                    }
                }
                catch
                {
                    System.Windows.MessageBox.Show("相机初始化失败！");
                    Checkbox_Camera.Fill = redBrush;
                    Checkbox_Camera.ToolTip = "相机初始化失败！";
                }
            }
            statusTimer = new DispatcherTimer();
            statusTimer.Tick += new EventHandler(CheckStatus);
            statusTimer.Interval = new TimeSpan(0, 0, 0, 2, 0);//2s刷一次
            statusTimer.Start();
        }

        static int BoardInterlockPort = -1, EmergencyStopPort = -1, PCAPort = -1, FGP400HzPort = -1;

        void CheckStatus(object sender, EventArgs e)
        {
            LinearGradientBrush greenBrush = new LinearGradientBrush();
            greenBrush.GradientStops.Add(new GradientStop(color: Colors.YellowGreen, offset: 0));
            greenBrush.GradientStops.Add(new GradientStop(color: Colors.Green, offset: 1));
            LinearGradientBrush redBrush = new LinearGradientBrush();
            redBrush.GradientStops.Add(new GradientStop(color: Colors.YellowGreen, offset: 0));
            redBrush.GradientStops.Add(new GradientStop(color: Colors.Red, offset: 1));
            if (BoardInterlockPort == -1)
            {
                try
                {
                    BoardInterlockPort = Convert.ToInt32(Parameter.glb_Parameter.system_parameter["BoardInterlockPort"].InnerText);
                    EmergencyStopPort = Convert.ToInt32(Parameter.glb_Parameter.system_parameter["EmergencyStopPort"].InnerText);
                    PCAPort = Convert.ToInt32(Parameter.glb_Parameter.system_parameter["PCAPort"].InnerText);
                    FGP400HzPort = Convert.ToInt32(Parameter.glb_Parameter.system_parameter["FGPPort"].InnerText);
                }
                catch
                {

                }
            }

            if (laser_using) return;
            Thread checkStatusThread = new Thread(new ThreadStart(() =>
            {
                laser_using = true;
                if (PvCamera.Inited && Camera.Connected)
                {
                    this.Dispatcher.BeginInvoke(new Action(() =>
                    {
                        Checkbox_Camera.Fill = greenBrush;
                        Checkbox_Camera.ToolTip = null;
                    }));
                }
                else
                {
                    this.Dispatcher.BeginInvoke(new Action(() =>
                    {
                        Checkbox_Camera.Fill = redBrush;
                        Checkbox_Camera.ToolTip = Camera.err_string;
                    }));
                }
                if (laser_class != IntPtr.Zero && GetLEDStatus() == 1)
                {
                    this.Dispatcher.BeginInvoke(new Action(() =>
                    {
                        Checkbox_LED.Fill = greenBrush;
                        Checkbox_LED.ToolTip = null;
                    }));
                }
                else
                {
                    this.Dispatcher.BeginInvoke(new Action(() =>
                    {
                        Checkbox_LED.Fill = redBrush;
                        Checkbox_LED.ToolTip = "LED串口通讯丢失！";
                    }));
                }
                if (laser_class != IntPtr.Zero && GetTCPIPStatus() == 1)
                {
                    this.Dispatcher.BeginInvoke(new Action(() =>
                    {
                        Checkbox_Server.Fill = greenBrush;
                        Checkbox_Server.ToolTip = null;
                    }));
                }
                else
                {
                    this.Dispatcher.BeginInvoke(new Action(() =>
                    {
                        Checkbox_Server.Fill = redBrush;
                        Checkbox_Server.ToolTip = "TCPIP通讯丢失！";
                    }));
                }
                System.Windows.Forms.Label statusLabel = new System.Windows.Forms.Label();
                statusLabel.Name = "Status";
                if (!backgroundWorker.IsBusy)
                {
                    if (LaserFunc.IPCCheckFunc(serialPort1, statusLabel, null))
                    {
                        this.Dispatcher.BeginInvoke(new Action(() =>
                        {
                            Checkbox_LaserK.Fill = greenBrush;
                            Checkbox_LaserK.ToolTip = null;
                        }));
                    }
                    else
                    {
                        this.Dispatcher.BeginInvoke(new Action(() =>
                        {
                            Checkbox_LaserK.Fill = redBrush;
                            Checkbox_LaserK.ToolTip = CShareData.StatusLableStr;
                            Checkbox_LRF.Fill = redBrush;
                            Checkbox_LRF.ToolTip = CShareData.StatusLableStr;
                            Checkbox_FPGA.Fill = redBrush;
                            Checkbox_FPGA.ToolTip = CShareData.StatusLableStr;
                        }));
                    }
                }
                if (!backgroundWorker.IsBusy && SerialComCanwork)
                {
                    if (LaserFunc.LDMCheckFunc(serialPort1, statusLabel, null))
                    {
                        this.Dispatcher.BeginInvoke(new Action(() =>
                        {
                            Checkbox_LRF.Fill = greenBrush;
                            Checkbox_LRF.ToolTip = null;
                        }));
                    }
                    else
                    {
                        this.Dispatcher.BeginInvoke(new Action(() =>
                        {
                            Checkbox_LRF.Fill = redBrush;
                            Checkbox_LRF.ToolTip = CShareData.StatusLableStr;
                        }));
                    }
                }
                if (!backgroundWorker.IsBusy && SerialComCanwork)
                {
                    if (LaserFunc.FPGACheckFunc(serialPort1, statusLabel, null))
                    {
                        this.Dispatcher.BeginInvoke(new Action(() =>
                        {
                            Checkbox_FPGA.Fill = greenBrush;
                            Checkbox_FPGA.ToolTip = null;
                        }));
                    }
                    else
                    {
                        this.Dispatcher.BeginInvoke(new Action(() =>
                        {
                            Checkbox_FPGA.Fill = redBrush;
                            Checkbox_FPGA.ToolTip = CShareData.StatusLableStr;
                        }));
                    }
                }
                laser_using = false;
                UInt32 Value;
                UInt32 stat;
                if (Susi4.APIs.SusiGPIO.Available)
                {
                    UInt32 id = SusiGPIO.DevList[0].ID;
                    stat = SusiGPIO.SusiGPIOGetLevel(id, 0xFFFFu, out Value);
                    if (stat == SusiStatus.SUSI_STATUS_SUCCESS)
                    {
                        string binary_code = Convert.ToString(Value, 2);
                        binary_code = binary_code.PadLeft(16, '0');
                        if (BoardInterlockPort > 0 && binary_code[15 - BoardInterlockPort] == '0')
                        {
                            this.Dispatcher.BeginInvoke(new Action(() =>
                            {
                                BoardInterlock.Fill = greenBrush;
                            }));
                        }
                        else
                        {
                            this.Dispatcher.BeginInvoke(new Action(() =>
                            {
                                BoardInterlock.Fill = redBrush;
                            }));
                        }
                        if (EmergencyStopPort > 0 && binary_code[15 - EmergencyStopPort] == '0')
                        {
                            this.Dispatcher.BeginInvoke(new Action(() =>
                            {
                                Emergency_Stop.Fill = greenBrush;
                            }));
                        }
                        else
                        {
                            this.Dispatcher.BeginInvoke(new Action(() =>
                            {
                                Emergency_Stop.Fill = redBrush;
                            }));
                        }
                        if (PCAPort > 0 && binary_code[15 - PCAPort] == '1')
                        {
                            this.Dispatcher.BeginInvoke(new Action(() =>
                            {
                                PCA_Interlock.Fill = greenBrush;
                            }));
                        }
                        else
                        {
                            this.Dispatcher.BeginInvoke(new Action(() =>
                            {
                                PCA_Interlock.Fill = redBrush;
                            }));
                        }
                        if (FGP400HzPort > 0 && binary_code[15 - FGP400HzPort] == '1')
                        {
                            this.Dispatcher.BeginInvoke(new Action(() =>
                            {
                                FGP400hz.Fill = greenBrush;
                            }));
                        }
                        else
                        {
                            this.Dispatcher.BeginInvoke(new Action(() =>
                            {
                                FGP400hz.Fill = redBrush;
                            }));
                        }
                    }
                }
            }));
            checkStatusThread.Start();
        }
        void NeedFreshK()
        {
            if (Image2dBox.Source != null)
            {
                this.Dispatcher.BeginInvoke(new Action(() =>
                {
                    byte[] ImageBackupBuffer2d = ImageBackup2d.ToByteArray();

                    ImageBackupBuffer2d.CopyTo(newImageBuffer2d, 0);
                    writeableSource2d.Clear();
                    writeableSource2d.FromByteArray(newImageBuffer2d, ImageBackupBuffer2d.Length);
                    foreach (var shape in DrawingBackup2d)
                    {
                        if (shape.point1.X == 0 && shape.point1.Y == 0) continue;
                        switch (shape.type)
                        {
                            case MouseStatus.DrawingLine:
                                if (shape.point2.X == 0 && shape.point2.Y == 0) continue;
                                double temp_1_x = ((shape.point1.X - 177) * 10000 / 145 + needTransform1_2DataCs.guide_line_x);
                                double temp_1_y = ((437 - shape.point1.Y) * 8000 / 35);
                                double temp_2_x = ((shape.point2.X - 177) * 10000 / 145 + needTransform1_2DataCs.guide_line_x);
                                double temp_2_y = ((437 - shape.point2.Y) * 8000 / 35);
                                nowShape2dtemp = new DrawShape();
                                nowShape2dtemp.point1.X = ((temp_1_x - Axis_x_mid) * 145 * Xrenderi / 10000 + 177);
                                nowShape2dtemp.point1.Y = (262 - (temp_1_y - Axis_y_mid) * 35 * Yrenderi / 8000);
                                nowShape2dtemp.point2.X = ((temp_2_x - Axis_x_mid) * 145 * Xrenderi / 10000 + 177);
                                nowShape2dtemp.point2.Y = (262 - (temp_2_y - Axis_y_mid) * 35 * Yrenderi / 8000);
                                DrawThickLine(ref writeableSource2d, nowShape2dtemp.point1, nowShape2dtemp.point2, 1.2, shape.color);
                                break;
                            case MouseStatus.DrawingRect:
                                break;
                            case MouseStatus.DrawingPoint:
                                double temp_x = (double)((shape.point1.X - 177) * 10000 / 145 + needTransform1_2DataCs.guide_line_x);
                                double temp_y = (double)((437 - shape.point1.Y) * 8000 / 35);
                                if (temp_x <= (Axis_x_mid + 10000 / Xrenderi) && temp_x >= (Axis_x_mid - 10000 / Xrenderi) &&
                                    temp_y <= (Axis_y_mid + 40000 / Yrenderi) && temp_y >= (Axis_y_mid - 40000 / Yrenderi))
                                {
                                    if(shape.radius <= 1)
                                        WriteableBitmapExtensions.FillEllipseCentered(writeableSource2d, (int)((temp_x - Axis_x_mid) * 145 * Xrenderi / 10000 + 177), (int)(262 - (temp_y - Axis_y_mid) * 35 * Yrenderi / 8000), shape.radius, shape.radius, shape.color);
                                    else
                                        WriteableBitmapExtensions.DrawEllipseCentered(writeableSource2d, (int)((temp_x - Axis_x_mid) * 145 * Xrenderi / 10000 + 177), (int)(262 - (temp_y - Axis_y_mid) * 35 * Yrenderi / 8000), shape.radius, shape.radius, shape.color);
                                }
                                break;
                            default:
                                break;
                        }
                    }
                    Image2dBox.Source = writeableSource2d;
                    switch (AxisMod)
                    {
                        case 0:
                            break;
                        case 1:
                            Axis2dScan(Axis_y_mid);
                            break;
                        case 2:
                            AxisNormal(Axis_x_mid, Axis_y_mid);
                            break;
                        case 3:
                            AxisHrzcalibration();
                            break;
                        default:
                            break;
                    }
                }), null);
                //GC.Collect();
            }
        }//激光设置界面刷新可视化
        public static void DrawThickLine(ref WriteableBitmap writeableBmp, Point point1, Point point2, double thickness, Color color)
        {
            //Original points for line 
            double x1 = point1.X;
            double y1 = point1.Y;
            double x2 = point2.X;
            double y2 = point2.Y;

            //Parallel line code from http://stackoverflow.com/questions/2825412/draw-a-parallel-line 
            var L = Math.Sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)) * 2;
            // This is the second line will be parallel to the first 
            int x1p = (int)(x1 + thickness * (y2 - y1) / L + 0.5);
            int x2p = (int)(x2 + thickness * (y2 - y1) / L + 0.5);
            int y1p = (int)(y1 + thickness * (x1 - x2) / L + 0.5);
            int y2p = (int)(y2 + thickness * (x1 - x2) / L + 0.5);

            int x1q = (int)(x1 - thickness * (y2 - y1) / L + 0.5);
            int x2q = (int)(x2 - thickness * (y2 - y1) / L + 0.5);
            int y1q = (int)(y1 - thickness * (x1 - x2) / L + 0.5);
            int y2q = (int)(y2 - thickness * (x1 - x2) / L + 0.5);

            //writeableBmp.DrawLine(x1, y1, x2, y2, Colors.Red); 
            //writeableBmp.DrawLine(x1p, y1p, x2p, y2p, Colors.Blue); 
            //Create closed filled polygon for "thick line" 
            writeableBmp.FillPolygon(new int[] { x1q, y1q, x2q, y2q, x2p, y2p, x1p, y1p, x1q, y1q }, color);
            //writeableBmp.FillPolygon(new int[] { (int)(x1 + 0.5), (int)(y1 + 0.5), (int)(x2 + 0.5), (int)(y2 + 0.5), x2p, y2p, x1p, y1p, (int)(x1 + 0.5), (int)(y1 + 0.5) }, color);
        }
        void timer_Tick(object sender, EventArgs e)
        {
            System.Drawing.Bitmap BitmapImg = PvCamera.getFrame();
            
            if (BitmapImg != null)
            {
                ImageWidth = BitmapImg.Width;
                ImageHeight = BitmapImg.Height;
                if (midline.color == Colors.Red)
                {
                    midline.point1.X = ImageWidth * 3 / 10;//20160622
                    midline.point1.Y = ImageHeight * 3 / 4;
                    midline.point2.X = ImageWidth * 7 / 10;
                    midline.point2.Y = ImageHeight * 3 / 4;
                    midline.color = Colors.Gold;
                }
                label_Position_Copy.Content = string.Format("平均亮度：{0}", Camera.illumiance);
                BitmapSource source = ToBitmapSource(BitmapImg);

                CameraImageBox.Source = source;

                WriteableBitmap tmp = new WriteableBitmap(source);
                writeableSource = BitmapFactory.ConvertToPbgra32Format(tmp);
                if (TI4_OpenPara && Camera.Started)
                {
                    if (needDrawMidline == true)//20160622
                    {
                        DrawThickLine(ref writeableSource, new Point(midline.point1.X + ImageWidth / 5, 0),
                            new Point(midline.point1.X + ImageWidth / 5, ImageHeight), 2, midline.color);
                        DrawThickLine(ref writeableSource, midline.point1, midline.point2, 2, midline.color);
                    }
                    foreach (var shape in DrawingBackup)
                    {
                        if (shape.point1.X == 0 && shape.point1.Y == 0) continue;
                        switch (shape.type)
                        {
                            case MouseStatus.DrawingLine:
                                if (shape.point2.X == 0 && shape.point2.Y == 0) continue;
                                DrawThickLine(ref writeableSource, shape.point1, shape.point2, kuan.Width, shape.color);
                                //WriteableBitmapExtensions.DrawLineAa(writeableSource, (int)shape.point1.X, (int)shape.point1.Y, (int)shape.point2.X, (int)shape.point2.Y, shape.color);
                                break;
                            case MouseStatus.DrawingRect:
                                if (shape.point2.X == 0 && shape.point2.Y == 0) continue;
                                int x1 = (int)shape.point1.X;
                                int y1 = (int)shape.point1.Y;
                                int x2 = (int)shape.point2.X;
                                int y2 = (int)shape.point2.Y;
                                int t;
                                if (x1 > x2)
                                {
                                    t = x1; x1 = x2; x2 = t;
                                }
                                if (y1 > y2)
                                {
                                    t = y1; y1 = y2; y2 = t;
                                }
                                WriteableBitmapExtensions.DrawRectangle(writeableSource, x1, y1, x2, y2, shape.color);
                                WriteableBitmapExtensions.DrawRectangle(writeableSource, x1 - 1, y1 - 1, x2 + 1, y2 + 1, shape.color);
                                WriteableBitmapExtensions.DrawRectangle(writeableSource, x1 + 1, y1 + 1, x2 - 1, y2 - 1, shape.color);
                                break;
                            case MouseStatus.DrawingPoint:
                                WriteableBitmapExtensions.FillEllipseCentered(writeableSource, (int)shape.point1.X, (int)shape.point1.Y, 2, 2, shape.color);
                                break;
                            default:
                                break;
                        }
                    }
                }
                CameraImageBox.Source = writeableSource;
                GC.Collect();
            }
        }
        void timer_Tick_static(object sender, EventArgs e)
        {
            label_Position_Copy.Content = string.Format("平均亮度：{0}", Camera.illumiance);
            if (CameraImageBox.Source != null)
            {
                byte[] ImageBackupBuffer = ImageBackup.ToByteArray();

                ImageBackupBuffer.CopyTo(newImageBuffer, 0);
                writeableSource.Clear();
                writeableSource.FromByteArray(newImageBuffer, ImageBackupBuffer.Length);
                if (TI4_OpenPara)
                {
                    if (needDrawMidline == true)//20160622
                    {
                        DrawThickLine(ref writeableSource, new Point(midline.point1.X + ImageWidth / 5, 0),
                            new Point(midline.point1.X + ImageWidth / 5, ImageHeight), 2, midline.color);
                        DrawThickLine(ref writeableSource, midline.point1, midline.point2, 2, midline.color);
                    }
                    foreach (var shape in DrawingBackup)
                    {
                        if (shape.point1.X == 0 && shape.point1.Y == 0) continue;
                        int x1, x2, y1, y2;
                        switch (shape.type)
                        {
                            case MouseStatus.DrawingLine:
                                if (shape.point2.X == 0 && shape.point2.Y == 0) continue;
                                DrawThickLine(ref writeableSource, shape.point1, shape.point2, kuan.Width, shape.color);
                                break;
                            case MouseStatus.DrawingRect:
                                if (shape.point2.X == 0 && shape.point2.Y == 0) continue;
                                x1 = (int)shape.point1.X;
                                y1 = (int)shape.point1.Y;
                                x2 = (int)shape.point2.X;
                                y2 = (int)shape.point2.Y;
                                int t;
                                if (x1 > x2)
                                {
                                    t = x1; x1 = x2; x2 = t;
                                }
                                if (y1 > y2)
                                {
                                    t = y1; y1 = y2; y2 = t;
                                }
                                WriteableBitmapExtensions.DrawRectangle(writeableSource, x1, y1, x2, y2, shape.color);
                                WriteableBitmapExtensions.DrawRectangle(writeableSource, x1 - 1, y1 - 1, x2 + 1, y2 + 1, shape.color);
                                WriteableBitmapExtensions.DrawRectangle(writeableSource, x1 + 1, y1 + 1, x2 - 1, y2 - 1, shape.color);
                                break;
                            case MouseStatus.DrawingPoint:
                                WriteableBitmapExtensions.FillEllipseCentered(writeableSource, (int)shape.point1.X, (int)shape.point1.Y, 1, 1, shape.color);
                                break;
                            default:
                                break;
                        }
                    }
                }
                CameraImageBox.Source = writeableSource;
                GC.Collect();
            }
        }
        public static BitmapSource ToBitmapSource(System.Drawing.Bitmap source)
        {
            IntPtr ptr = source.GetHbitmap(); //obtain the Hbitmap

            BitmapSource bs = System.Windows.Interop.Imaging.CreateBitmapSourceFromHBitmap(
                ptr,
                IntPtr.Zero,
                Int32Rect.Empty,
                System.Windows.Media.Imaging.BitmapSizeOptions.FromEmptyOptions());

            DeleteObject(ptr); //release the HBitmap
            return bs;
        }
        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            MessageBoxResult result = Xceed.Wpf.Toolkit.MessageBox.Show("确认是否要退出？", "退出", MessageBoxButton.YesNo);//显示确认窗口
            if (result == MessageBoxResult.Yes)
            {
                try
                {
                    if (laser_class != IntPtr.Zero) classLaserDestroy(laser_class);
                    PvCamera.dispose();
                    SusiLib.SusiLibInitialize();
                    if (serialPort1 != null && serialPort1.IsOpen) serialPort1.Close();
                    if (laser_class != null && BoostIsOpen == true)
                    {
                        OnBnClickedButtonBoostClose_std(laser_class);
                    }
                    AngleDetector.gotAngle -= OnGetNewAngle;
                    if (AngleDetector.DipAngleSerialPort != null && AngleDetector.DipAngleSerialPort.IsOpen) AngleDetector.DipAngleSerialPort.Close();
                }
                catch
                {

                }
                foreach (Window win in System.Windows.Application.Current.Windows)
                {
                    if (win != this)
                    {
                        win.Close();
                    }
                }
            }
            else
            {
                e.Cancel = true;//取消操作
            }
        }
        #region 委托函数+标定工具
        private void button_midline_Click(object sender, RoutedEventArgs e)
        {
            if (needDrawMidline == false)
            {
                midlinetext.Text = "关闭基准线";
                needDrawMidline = true;
            }
            else
            {
                midlinetext.Text = "打开基准线";
                needDrawMidline = false;
            }
        }//显示/关闭基准线按钮
        private void button_ConfirmGuideLine_Click(object sender, RoutedEventArgs e)
        {
            List<ComboPlane> Guidenum_List = new List<ComboPlane>();
            for (int i = 1; i <= GuideLineNum; i++)
            {
                Guidenum_List.Add(new ComboPlane() { comboPlane = i.ToString() });
                Guidenum_List.Add(new ComboPlane() { comboPlane = i.ToString()+"_left" });
                Guidenum_List.Add(new ComboPlane() { comboPlane = i.ToString() + "_right" });
            }
            newcombobox.ItemsSource = Guidenum_List;
            newcombobox.DisplayMemberPath = "comboPlane";
            newcombobox.SelectedValuePath = "comboPlane";
            if (WrapPanel2.Visibility == Visibility.Visible)
                WrapPanel2.Visibility = Visibility.Collapsed;
            else
                WrapPanel2.Visibility = Visibility.Visible;
        }//标定工具引导线编号弹出
        private void button_ConfirmStopLine_Click(object sender, RoutedEventArgs e)
        {
            List<ComboPlane> Guidenum_List = new List<ComboPlane>();
            List<ComboPlane> Stopnum_List = new List<ComboPlane>();
            for (int i = 1; i <= GuideLineNum; i++)
            {
                Guidenum_List.Add(new ComboPlane() { comboPlane = i.ToString() });
            }
            newcombobox2A.ItemsSource = Guidenum_List;
            newcombobox2A.DisplayMemberPath = "comboPlane";
            newcombobox2A.SelectedValuePath = "comboPlane";
            for (int i = 1; i <= StopLineNum; i++)
            {
                if (i <= 9)
                    Stopnum_List.Add(new ComboPlane() { comboPlane = i.ToString() });
                else
                    Stopnum_List.Add(new ComboPlane() { comboPlane = ((char)(i + 55)).ToString() });
            }
            newcombobox2B.ItemsSource = Stopnum_List;
            newcombobox2B.DisplayMemberPath = "comboPlane";
            newcombobox2B.SelectedValuePath = "comboPlane";
            if (WrapPanel3.Visibility == Visibility.Visible)
                WrapPanel3.Visibility = Visibility.Collapsed;
            else
                WrapPanel3.Visibility = Visibility.Visible;
        }//标定工具停止线编号弹出
        private void button_AddRect_Click(object sender, RoutedEventArgs e)
        {
            List<ComboPlane> Rect_List = new List<ComboPlane>();
            Rect_List.Add(new ComboPlane() { comboPlane = "捕获区域" });
            Rect_List.Add(new ComboPlane() { comboPlane = "跟踪定位区" });
            Rect_List.Add(new ComboPlane() { comboPlane = "登机桥位置" });
            newcombobox3.ItemsSource = Rect_List;
            newcombobox3.DisplayMemberPath = "comboPlane";
            newcombobox3.SelectedValuePath = "comboPlane";
            if (WrapPanel6.Visibility == Visibility.Visible)
                WrapPanel6.Visibility = Visibility.Collapsed;
            else
                WrapPanel6.Visibility = Visibility.Visible;
        }
        private void DrawGuideLine_Close(object sender, RoutedEventArgs e)
        {
            WrapPanel2.Visibility = Visibility.Collapsed;
        }
        private void DrawStopLine_Close(object sender, RoutedEventArgs e)
        {
            WrapPanel3.Visibility = Visibility.Collapsed;
        }
        private void DrawRect_Close(object sender, RoutedEventArgs e)
        {
            WrapPanel6.Visibility = Visibility.Collapsed;
        }
        private void TI4_open(object sender, RoutedEventArgs e)//打开悬浮标定工具操作窗
        {
            //if (!Camera.Started)
            //{
            //    Xceed.Wpf.Toolkit.MessageBox.Show("请先打开相机", "标定工具", MessageBoxButton.OK);
            //    return;
            //}
            if (!XBottomSide.IsFloating)
                XBottomSide.Float();
            if (TI4_OpenPara == true)
            {
                TI5.Visibility = Visibility.Collapsed;
                TI4_OpenPara = !TI4_OpenPara;
                Group2.Margin = new Thickness(0, 0, 0, 4);
            }
            else if (TI4_OpenPara == false)
            {
                TI4_OpenPara = !TI4_OpenPara;
                TI5.Visibility = Visibility.Visible;
                Group2.Margin = new Thickness(-120, 0, 0, 4);
            }
        }
        private void Button_DrawGuideLine_Click(object sender, RoutedEventArgs e)//绘制引导线的委托
        {
            now = 666;
            ClearGhostGraphic();
            bool TempChange = true;
            if (newcombobox.SelectedIndex == -1)
                return;
            string tempstring = "引导线" + newcombobox.SelectedValue.ToString();
            foreach (var shape in DrawingBackup)
            {
                if (shape.prompt == tempstring)
                    TempChange = false;
            }
            if (TempChange)
            {
                nowDrawing = MouseStatus.DrawingLine;
                CameraImageBox.Cursor = Cursors.Pen;
                nowShape.type = MouseStatus.DrawingLine;
                nowShape.color = Colors.Yellow;
                nowShape.prompt = tempstring;
                DrawingBackup.Add(nowShape);
                LinearGradientBrush brush = new LinearGradientBrush();
                brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                brush.GradientStops.Add(new GradientStop(color: Colors.Green, offset: 1));
                Checkbox_GuidelineSet.Fill = brush;
            }
            else
                Xceed.Wpf.Toolkit.MessageBox.Show(tempstring + "已经存在,如需绘制，请先删除！", "引导线", MessageBoxButton.OK);
        }
        private void button_Delete_Click(object sender, RoutedEventArgs e)//删除的委托
        {
            nowDrawing = MouseStatus.Delete;
            now = 666;
            CameraImageBox.Cursor = Cursors.Cross;
        }
        private void button_Recover_Click(object sender, RoutedEventArgs e)
        {
            int n = DeleteDrawingBackup.Count;
            if (n > 0)
            {
                if (DeleteOrClear)
                {
                    DrawingBackup.AddRange(DeleteDrawingBackup);
                    DeleteDrawingBackup.Clear();
                }
                else
                {
                    DrawingBackup.Add(DeleteDrawingBackup[n - 1]);
                    DeleteDrawingBackup.RemoveAt(n - 1);
                }
            }
        }
        private void Button_DrawStopLine2B(object sender, RoutedEventArgs e)//绘制停止线的委托
        {
            now = 666;
            ClearGhostGraphic();
            bool TempChange = true;
            bool TempExist = false;
            if (newcombobox2A.SelectedIndex == -1 || newcombobox2B.SelectedIndex == -1)
                return;
            string tempstring = "引导线" + newcombobox2A.SelectedValue.ToString() + "停止线" + newcombobox2B.SelectedValue.ToString();
            string tempstring2 = "引导线" + newcombobox2A.SelectedValue.ToString();
            foreach (var shape in DrawingBackup)
            {
                if (shape.prompt == tempstring)
                    TempChange = false;
                if (shape.prompt == tempstring2)
                    TempExist = true;
            }
            if (TempExist)
            {
                if (TempChange)
                {
                    nowDrawing = MouseStatus.DrawingLine;
                    CameraImageBox.Cursor = Cursors.Pen;
                    nowShape.type = MouseStatus.DrawingLine;
                    nowShape.color = Colors.Yellow;
                    nowShape.prompt = tempstring;
                    String TempSS = "";
                    for (int i = 0; i < tempstring.Length; i++)
                    {
                        if (Convert.ToInt32(tempstring[i]) >= 48 && Convert.ToInt32(tempstring[i]) <= 57)
                        {
                            TempSS += tempstring[i].ToString();
                        }
                    }
                    nowShape.Sforstopline = TempSS;
                    DrawingBackup.Add(nowShape);
                }
                else
                    Xceed.Wpf.Toolkit.MessageBox.Show(tempstring + "已经存在,如需绘制，请先删除！", "停止线", MessageBoxButton.OK);
            }
            else
                Xceed.Wpf.Toolkit.MessageBox.Show(tempstring2 + "不存在,请先绘制引导线！", "停止线", MessageBoxButton.OK);
        }
        private void button_DrawRect_Click(object sender, RoutedEventArgs e)//绘制选框的委托
        {
            now = 666;
            ClearGhostGraphic();
            bool TempChange = true;
            if (newcombobox3.SelectedIndex == -1)
                return;
            foreach (var shape in DrawingBackup)
            {
                if (shape.prompt == newcombobox3.SelectedValue.ToString())
                    TempChange = false;
            }
            if (TempChange)
            {
                nowDrawing = MouseStatus.DrawingRect;
                CameraImageBox.Cursor = Cursors.Pen;
                nowShape.type = MouseStatus.DrawingRect;
                switch (newcombobox3.SelectedIndex)
                {
                    case 0:
                        nowShape.color = Colors.Blue;
                        break;
                    case 1:
                        nowShape.color = Colors.DarkOrange;
                        break;
                    case 2:
                        nowShape.color = Colors.DeepPink;
                        break;
                    default:
                        break;
                }
                nowShape.prompt = newcombobox3.SelectedValue.ToString();
                DrawingBackup.Add(nowShape);
            }
            else
                Xceed.Wpf.Toolkit.MessageBox.Show(newcombobox3.SelectedValue.ToString() + "已经存在,如需绘制，请先删除！", label_DrawRect.Content.ToString(), MessageBoxButton.OK);
        }
        private void button_InsertMarkpoint_Click(object sender, RoutedEventArgs e)//绘制标记点的委托
        {
            now = 666;
            ClearGhostGraphic();
            nowDrawing = MouseStatus.DrawingPoint;
            CameraImageBox.Cursor = Cursors.Pen;
            nowShape.type = MouseStatus.DrawingPoint;
            nowShape.color = Colors.Red;
            nowShape.prompt = "标志点";
            DrawingBackup.Add(nowShape);
        }
        private void button_Clear_Click(object sender, RoutedEventArgs e)//清屏的委托
        {
            now = 666;
            if (DrawingBackup.Count > 0)
            {
                DeleteDrawingBackup.Clear();
                DeleteDrawingBackup.AddRange(DrawingBackup);
                DrawingBackup.Clear();
                DeleteOrClear = true;
            }
        }
        private void MWindow_PassDataBetweenForm(object sender, PassDataWinFormEventArgs e)//右键窗口对引导线编号的委托
        {
            bool TempChange = true;
            if ((now < 666) && (DrawingBackup[now].type == MouseStatus.DrawingLine))
            {
                if (DrawingBackup[now].prompt.Contains("引导线") && !DrawingBackup[now].prompt.Contains("停止线"))
                {
                    for (int tempi = 0; tempi < DrawingBackup.Count(); tempi++)
                    {
                        if (DrawingBackup[tempi].prompt == e.MY)
                            TempChange = false;
                    }
                    if (TempChange)
                        DrawingBackup[now].prompt = e.MY;
                    else
                        Xceed.Wpf.Toolkit.MessageBox.Show(e.MY + "已经存在！", "引导线", MessageBoxButton.OK);
                }
                else
                    Xceed.Wpf.Toolkit.MessageBox.Show("请勿修改选线的基本属性！", "引导线", MessageBoxButton.OK);
            }
            now = 666;
        }
        private void DropBox_PassBetweenForm(object sender, PassWinFormEventArgs e)//右键窗口对停止线编号的委托
        {
            bool TempChange = true;
            bool TempExist = false;
            if ((now < 666) && (DrawingBackup[now].type == MouseStatus.DrawingLine))
            {
                if (DrawingBackup[now].prompt.Contains("停止线"))
                {
                    foreach (var shape in DrawingBackup)
                    {
                        if (shape.prompt == e.Guide + e.Stop)
                            TempChange = false;
                        if (shape.prompt == e.Guide)
                            TempExist = true;
                    }
                    if (TempExist)
                    {
                        if (TempChange)
                            DrawingBackup[now].prompt = e.Guide + e.Stop;
                        else
                            Xceed.Wpf.Toolkit.MessageBox.Show(e.Guide + e.Stop + "已经存在！", "停止线", MessageBoxButton.OK);
                    }
                    else
                        Xceed.Wpf.Toolkit.MessageBox.Show(e.Guide + "不存在,请先绘制引导线！", "停止线", MessageBoxButton.OK);
                }
                else
                    Xceed.Wpf.Toolkit.MessageBox.Show("请勿修改选线的基本属性！", "停止线", MessageBoxButton.OK);
            }
            now = 666;
        }
        public void PWindow_PassBetweenForm(object sender, PassWinFormEventArgs e)//对标记点设置的委托
        {
            //lastShape.prompt = "("+ L1.string1 + e.Guide+")"+e.Stop;
            if (e.Stop == "233" && e.Guide == "233")
                DrawingBackup.Remove(lastShape);
            else
            {
                if (e.Guide == "1")
                    lastShape.color = Color.FromRgb(255, 0, 0);
                else if (e.Guide == "2")
                    lastShape.color = Color.FromRgb(0, 255, 0);
                else if (e.Guide == "3")
                    lastShape.color = Color.FromRgb(255, 0, 255);
                else
                    lastShape.color = Color.FromRgb(0, 255, 255);
                lastShape.prompt = e.Stop;
                lastShape.flag = Convert.ToInt32(e.Guide);
            }
            now = 666;
        }
        private void refresh_up_Click(object sender, RoutedEventArgs e)
        {
            var trans = (TranslateTransform)((TransformGroup)CameraImageBox.RenderTransform).Children.First(tr => tr is TranslateTransform);
            var transX = 0;
            var transY = 0;
            trans.X = transX;
            trans.Y = transY;
            TransformGroup transformGroup = (TransformGroup)CameraImageBox.RenderTransform;
            ScaleTransform scale = (ScaleTransform)transformGroup.Children[0];
            scale.ScaleX = 1;
            scale.ScaleY = 1;
            sensitiveDistance = (int)(Convert.ToInt32(ConfigurationManager.AppSettings["SensitiveDistance"]) / scale.ScaleX);
            sensitiveDistance = sensitiveDistance < 3 ? 3 : sensitiveDistance;
        }
        private void ClearGhostGraphic()
        {
            int num = DrawingBackup.Count;
            double x1, y1, x2, y2;
            for (int i = 0; i < num; i++)
            {
                if (DrawingBackup[i].point2.X > ImageWidth)
                    DrawingBackup[i].point2.X = ImageWidth - 30;
                if (DrawingBackup[i].point2.X < 0)
                    DrawingBackup[i].point2.X = 30;
                if (DrawingBackup[i].point2.Y > ImageHeight)
                    DrawingBackup[i].point2.Y = ImageHeight - 30;
                if (DrawingBackup[i].point2.Y < 0)
                    DrawingBackup[i].point2.Y = 30;
                if (DrawingBackup[i].point1.X > ImageWidth)
                    DrawingBackup[i].point1.X = ImageWidth - 30;
                if (DrawingBackup[i].point1.X < 0)
                    DrawingBackup[i].point1.X = 30;
                if (DrawingBackup[i].point1.Y > ImageHeight)
                    DrawingBackup[i].point1.Y = ImageHeight - 30;
                if (DrawingBackup[i].point1.Y < 0)
                    DrawingBackup[i].point1.Y = 30;
                x1 = DrawingBackup[i].point1.X;
                y1 = DrawingBackup[i].point1.Y;
                x2 = DrawingBackup[i].point2.X;
                y2 = DrawingBackup[i].point2.Y;
                switch (DrawingBackup[i].type)
                {
                    case MouseStatus.DrawingLine:
                        if (x1 == 0 && y1 == 0 && x2 == 0 && y2 == 0)
                        {
                            DrawingBackup.RemoveAt(i);
                            i--;
                            num--;
                        }
                        else if ((Math.Pow((x1 - x2), 2) + Math.Pow((y1 - y2), 2)) < 100)
                        {
                            DrawingBackup.RemoveAt(i);
                            i--;
                            num--;
                        }
                        break;
                    case MouseStatus.DrawingRect:
                        if (x1 == 0 && y1 == 0 && x2 == 0 && y2 == 0)
                        {
                            DrawingBackup.RemoveAt(i);
                            i--;
                            num--;
                        }
                        else if ((Math.Pow((x1 - x2), 2) + Math.Pow((y1 - y2), 2)) < 100)
                        {
                            DrawingBackup.RemoveAt(i);
                            i--;
                            num--;
                        }
                        break;
                    case MouseStatus.DrawingPoint:
                        if (x1 == 0 && y1 == 0 && x2 == 0 && y2 == 0)
                        {
                            DrawingBackup.RemoveAt(i);
                            i--;
                            num--;
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        #endregion
        #region CameraImageBox操作
        private void CameraImageBox_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            CameraImageBox.CaptureMouse();
            Point thisPoint = e.GetPosition(e.Source as FrameworkElement);
            thisPoint.X = offset.X + thisPoint.X * ImageWidth / CameraImageBox.Width;
            thisPoint.Y = offset.Y + thisPoint.Y * ImageHeight / CameraImageBox.Height;
            bool DeleteOnce = false;
            switch (nowDrawing)
            {
                case MouseStatus.DrawingLine:
                    nowShape.point1 = thisPoint;
                    break;
                case MouseStatus.DrawingRect:
                    nowShape.point1 = thisPoint;
                    break;
                case MouseStatus.DrawingPoint:
                    nowShape.point1 = thisPoint;
                    break;
                case MouseStatus.ZoomAndPlug:
                    CameraImageBox.Cursor = Cursors.Hand;
                    //for (int i = DrawingBackup.Count - 1; i >= 0; i--)
                    for (int i = 0; i < DrawingBackup.Count; i++)
                    {
                        if (DrawingBackup[i].point1.X == 0 && DrawingBackup[i].point1.Y == 0) continue;
                        switch (DrawingBackup[i].type)
                        {
                            case MouseStatus.DrawingLine:
                                if (DrawingBackup[i].point2.X == 0 && DrawingBackup[i].point2.Y == 0) continue;
                                double k = (DrawingBackup[i].point2.Y - DrawingBackup[i].point1.Y) / (DrawingBackup[i].point2.X - DrawingBackup[i].point1.X);
                                double angle = Math.Atan(k);
                                Cursor doubleArrowCursor = CameraImageBox.Cursor;
                                if (angle < -Math.PI * 5 / 12)
                                {
                                    doubleArrowCursor = Cursors.SizeNS;
                                }
                                else if (angle < -Math.PI / 12)
                                {
                                    doubleArrowCursor = Cursors.SizeNESW;
                                }
                                else if (angle < Math.PI / 12)
                                {
                                    doubleArrowCursor = Cursors.SizeWE;
                                }
                                else if (angle < Math.PI * 5 / 12)
                                {
                                    doubleArrowCursor = Cursors.SizeNWSE;
                                }
                                else
                                {
                                    doubleArrowCursor = Cursors.SizeNS;
                                }
                                double distanceToPoint1 = Math.Sqrt((thisPoint.X - DrawingBackup[i].point1.X) * (thisPoint.X - DrawingBackup[i].point1.X) +
                                    (thisPoint.Y - DrawingBackup[i].point1.Y) * (thisPoint.Y - DrawingBackup[i].point1.Y));
                                double distanceToPoint2 = Math.Sqrt((thisPoint.X - DrawingBackup[i].point2.X) * (thisPoint.X - DrawingBackup[i].point2.X) +
                                    (thisPoint.Y - DrawingBackup[i].point2.Y) * (thisPoint.Y - DrawingBackup[i].point2.Y));
                                double distanceToLine = Math.Abs(k * thisPoint.X - thisPoint.Y - k * DrawingBackup[i].point1.X + DrawingBackup[i].point1.Y) / Math.Sqrt(k * k + 1);
                                //label_Position_Copy.Content = string.Format("x:{0:f} y:{1:f}", distanceToPoint1, distanceToPoint2);
                                //端点
                                if (distanceToPoint1 < sensitiveDistance)
                                {
                                    CameraImageBox.Cursor = doubleArrowCursor;
                                    nowDrawing = MouseStatus.DragLine;
                                    nowDragLine = DragLineStatus.DragPoint1;
                                    nowShape = DrawingBackup[i];
                                    now = i;
                                }
                                else if (distanceToPoint2 < sensitiveDistance)
                                {
                                    CameraImageBox.Cursor = doubleArrowCursor;
                                    nowDrawing = MouseStatus.DragLine;
                                    nowDragLine = DragLineStatus.DragPoint2;
                                    nowShape = DrawingBackup[i];
                                    now = i;
                                }
                                else if (distanceToLine < sensitiveDistance
                                    && thisPoint.X > Math.Min(DrawingBackup[i].point1.X, DrawingBackup[i].point2.X) && thisPoint.X < Math.Max(DrawingBackup[i].point1.X, DrawingBackup[i].point2.X)
                                    && thisPoint.X > Math.Min(DrawingBackup[i].point1.X, DrawingBackup[i].point2.X) && thisPoint.X < Math.Max(DrawingBackup[i].point1.X, DrawingBackup[i].point2.X))
                                {
                                    CameraImageBox.Cursor = Cursors.SizeAll;
                                    nowDrawing = MouseStatus.DragLine;
                                    nowDragLine = DragLineStatus.DragLine;
                                    nowShape = DrawingBackup[i];
                                    start = thisPoint;
                                    originShape = DrawingBackup[i].clone();
                                    now = i;
                                }
                                break;
                            case MouseStatus.DrawingRect:
                                if (DrawingBackup[i].point2.X == 0 && DrawingBackup[i].point2.Y == 0) continue;
                                double x1 = (double)DrawingBackup[i].point1.X;
                                double y1 = (double)DrawingBackup[i].point1.Y;
                                double x2 = (double)DrawingBackup[i].point2.X;
                                double y2 = (double)DrawingBackup[i].point2.Y;
                                double t;
                                if (x1 > x2)
                                {
                                    t = x1; x1 = x2; x2 = t;
                                }
                                if (y1 > y2)
                                {
                                    t = y1; y1 = y2; y2 = t;
                                }
                                double distanceToNWPoint = Math.Sqrt((thisPoint.X - x1) * (thisPoint.X - x1) + (thisPoint.Y - y1) * (thisPoint.Y - y1));
                                double distanceToNEPoint = Math.Sqrt((thisPoint.X - x2) * (thisPoint.X - x2) + (thisPoint.Y - y1) * (thisPoint.Y - y1));
                                double distanceToSWPoint = Math.Sqrt((thisPoint.X - x1) * (thisPoint.X - x1) + (thisPoint.Y - y2) * (thisPoint.Y - y2));
                                double distanceToSEPoint = Math.Sqrt((thisPoint.X - x2) * (thisPoint.X - x2) + (thisPoint.Y - y2) * (thisPoint.Y - y2));
                                if (distanceToNWPoint < sensitiveDistance)
                                {
                                    CameraImageBox.Cursor = Cursors.SizeNWSE;
                                    nowDrawing = MouseStatus.DragRect;
                                    nowDragRect = DragRectStatus.DragNWPoint;
                                    nowShape = DrawingBackup[i];
                                    now = i;
                                }
                                else if (distanceToNEPoint < sensitiveDistance)
                                {
                                    CameraImageBox.Cursor = Cursors.SizeNESW;
                                    nowDrawing = MouseStatus.DragRect;
                                    nowDragRect = DragRectStatus.DragNEPoint;
                                    nowShape = DrawingBackup[i];
                                    now = i;
                                }
                                else if (distanceToSWPoint < sensitiveDistance)
                                {
                                    CameraImageBox.Cursor = Cursors.SizeNESW;
                                    nowDrawing = MouseStatus.DragRect;
                                    nowDragRect = DragRectStatus.DragSWPoint;
                                    nowShape = DrawingBackup[i];
                                    now = i;
                                }
                                else if (distanceToSEPoint < sensitiveDistance)
                                {
                                    CameraImageBox.Cursor = Cursors.SizeNWSE;
                                    nowDrawing = MouseStatus.DragRect;
                                    nowDragRect = DragRectStatus.DragSEPoint;
                                    nowShape = DrawingBackup[i];
                                    now = i;
                                }
                                else if (Math.Abs(thisPoint.X - x1) < sensitiveDistance && thisPoint.Y > y1 && thisPoint.Y < y2)
                                {
                                    CameraImageBox.Cursor = Cursors.SizeWE;
                                    nowDrawing = MouseStatus.DragRect;
                                    nowDragRect = DragRectStatus.DragWLine;
                                    nowShape = DrawingBackup[i];
                                    now = i;
                                }
                                else if (Math.Abs(thisPoint.X - x2) < sensitiveDistance && thisPoint.Y > y1 && thisPoint.Y < y2)
                                {
                                    CameraImageBox.Cursor = Cursors.SizeWE;
                                    nowDrawing = MouseStatus.DragRect;
                                    nowDragRect = DragRectStatus.DragELine;
                                    nowShape = DrawingBackup[i];
                                    now = i;
                                }
                                else if (Math.Abs(thisPoint.Y - y1) < sensitiveDistance && thisPoint.X > x1 && thisPoint.X < x2)
                                {
                                    CameraImageBox.Cursor = Cursors.SizeNS;
                                    nowDrawing = MouseStatus.DragRect;
                                    nowDragRect = DragRectStatus.DragNLine;
                                    nowShape = DrawingBackup[i];
                                    now = i;
                                }
                                else if (Math.Abs(thisPoint.Y - y2) < sensitiveDistance && thisPoint.X > x1 && thisPoint.X < x2)
                                {
                                    CameraImageBox.Cursor = Cursors.SizeNS;
                                    nowDrawing = MouseStatus.DragRect;
                                    nowDragRect = DragRectStatus.DragSLine;
                                    nowShape = DrawingBackup[i];
                                    now = i;
                                }
                                break;
                            case MouseStatus.DrawingPoint:
                                double distanceToPoint = Math.Sqrt((thisPoint.X - DrawingBackup[i].point1.X) * (thisPoint.X - DrawingBackup[i].point1.X) +
                                    (thisPoint.Y - DrawingBackup[i].point1.Y) * (thisPoint.Y - DrawingBackup[i].point1.Y));
                                if (distanceToPoint < sensitiveDistance)
                                {
                                    CameraImageBox.Cursor = Cursors.SizeAll;
                                    nowDrawing = MouseStatus.DragPoint;
                                    nowShape = DrawingBackup[i];
                                    now = i;
                                }
                                break;
                            default:
                                break;
                        }
                    }
                    double midlinek = (midline.point2.Y - midline.point1.Y) / (midline.point2.X - midline.point1.X);//20160622
                    double distanceTomidlinePoint1 = Math.Sqrt((thisPoint.X - midline.point1.X) * (thisPoint.X - midline.point1.X) +
                                    (thisPoint.Y - midline.point1.Y) * (thisPoint.Y - midline.point1.Y));
                    double distanceTomidlinePoint2 = Math.Sqrt((thisPoint.X - midline.point2.X) * (thisPoint.X - midline.point2.X) +
                                    (thisPoint.Y - midline.point2.Y) * (thisPoint.Y - midline.point2.Y));
                    double distanceTomidline = Math.Abs(midlinek * thisPoint.X - thisPoint.Y - midlinek * midline.point1.X +
                                    midline.point1.Y) / Math.Sqrt(midlinek * midlinek + 1);
                    double distanceToVline = Math.Abs(thisPoint.X - midline.point1.X/2-midline.point2.X/2) ;
                    if (distanceTomidlinePoint1 < sensitiveDistance)
                    {
                        nowDrawing = MouseStatus.DragMidLine;
                        nowDragLine = DragLineStatus.DragPoint1;
                        nowShape = midline;
                        start = thisPoint;
                        now = 666;
                    }
                    else if (distanceTomidlinePoint2 < sensitiveDistance)
                    {
                        nowDrawing = MouseStatus.DragMidLine;
                        nowDragLine = DragLineStatus.DragPoint2;
                        nowShape = midline;
                        start = thisPoint;
                        now = 666;
                    }
                    else if (distanceTomidline < sensitiveDistance
                        && thisPoint.X > Math.Min(midline.point1.X, midline.point2.X) && thisPoint.X < Math.Max(midline.point1.X, midline.point2.X)
                        && thisPoint.X > Math.Min(midline.point1.X, midline.point2.X) && thisPoint.X < Math.Max(midline.point1.X, midline.point2.X))
                    {
                        nowDrawing = MouseStatus.DragMidLine;
                        nowDragLine = DragLineStatus.DragLine;
                        nowShape = midline;
                        start = thisPoint;
                        originShape = midline.clone();
                        now = 666;
                    }
                    else if (distanceToVline < sensitiveDistance)
                    {
                        nowDrawing = MouseStatus.DragMidLine;
                        nowDragLine = DragLineStatus.DragPoint2;
                        nowShape = midline;
                        start = thisPoint;
                        now = 666;
                    }
                    break;
                case MouseStatus.Delete:
                    for (int i = DrawingBackup.Count - 1; i >= 0; i--)
                    {
                        if (DrawingBackup[i].point1.X == 0 && DrawingBackup[i].point1.Y == 0) continue;
                        switch (DrawingBackup[i].type)
                        {
                            case MouseStatus.DrawingLine:
                                if (DrawingBackup[i].point2.X == 0 && DrawingBackup[i].point2.Y == 0) continue;
                                double k = (DrawingBackup[i].point2.Y - DrawingBackup[i].point1.Y) / (DrawingBackup[i].point2.X - DrawingBackup[i].point1.X);
                                double distanceToPoint1 = Math.Sqrt((thisPoint.X - DrawingBackup[i].point1.X) * (thisPoint.X - DrawingBackup[i].point1.X) +
                                    (thisPoint.Y - DrawingBackup[i].point1.Y) * (thisPoint.Y - DrawingBackup[i].point1.Y));
                                double distanceToPoint2 = Math.Sqrt((thisPoint.X - DrawingBackup[i].point2.X) * (thisPoint.X - DrawingBackup[i].point2.X) +
                                    (thisPoint.Y - DrawingBackup[i].point2.Y) * (thisPoint.Y - DrawingBackup[i].point2.Y));
                                double distanceToLine = Math.Abs(k * thisPoint.X - thisPoint.Y - k * DrawingBackup[i].point1.X + DrawingBackup[i].point1.Y) / Math.Sqrt(k * k + 1);
                                //label_Position_Copy.Content = string.Format("x:{0:f} y:{1:f}", distanceToPoint1, distanceToPoint2);
                                //端点
                                if (distanceToPoint1 < 2 * sensitiveDistance || distanceToPoint2 < 2 * sensitiveDistance || (distanceToLine < 2 * sensitiveDistance
                                    && thisPoint.X > Math.Min(DrawingBackup[i].point1.X, DrawingBackup[i].point2.X) &&
                                    thisPoint.X < Math.Max(DrawingBackup[i].point1.X, DrawingBackup[i].point2.X)
                                    && thisPoint.X > Math.Min(DrawingBackup[i].point1.X, DrawingBackup[i].point2.X) &&
                                    thisPoint.X < Math.Max(DrawingBackup[i].point1.X, DrawingBackup[i].point2.X)))
                                {
                                    DeleteDrawingBackup.Add(DrawingBackup[i]);
                                    DrawingBackup.RemoveAt(i);
                                    DeleteOrClear = false;
                                    DeleteOnce = true;
                                }
                                break;
                            case MouseStatus.DrawingRect:
                                if (DrawingBackup[i].point2.X == 0 && DrawingBackup[i].point2.Y == 0) continue;
                                double x1 = (double)DrawingBackup[i].point1.X;
                                double y1 = (double)DrawingBackup[i].point1.Y;
                                double x2 = (double)DrawingBackup[i].point2.X;
                                double y2 = (double)DrawingBackup[i].point2.Y;
                                double t;
                                if (x1 > x2)
                                {
                                    t = x1; x1 = x2; x2 = t;
                                }
                                if (y1 > y2)
                                {
                                    t = y1; y1 = y2; y2 = t;
                                }
                                double distanceToNWPoint = Math.Sqrt((thisPoint.X - x1) * (thisPoint.X - x1) + (thisPoint.Y - y1) * (thisPoint.Y - y1));
                                double distanceToNEPoint = Math.Sqrt((thisPoint.X - x2) * (thisPoint.X - x2) + (thisPoint.Y - y1) * (thisPoint.Y - y1));
                                double distanceToSWPoint = Math.Sqrt((thisPoint.X - x1) * (thisPoint.X - x1) + (thisPoint.Y - y2) * (thisPoint.Y - y2));
                                double distanceToSEPoint = Math.Sqrt((thisPoint.X - x2) * (thisPoint.X - x2) + (thisPoint.Y - y2) * (thisPoint.Y - y2));
                                if (distanceToNWPoint < 2 * sensitiveDistance || distanceToNEPoint < 2 * sensitiveDistance || distanceToSWPoint < 2 * sensitiveDistance
                                     || distanceToSEPoint < 2 * sensitiveDistance || (Math.Abs(thisPoint.X - x1) < 2 * sensitiveDistance && thisPoint.Y > y1 && thisPoint.Y < y2)
                                     || (Math.Abs(thisPoint.X - x2) < 2 * sensitiveDistance && thisPoint.Y > y1 && thisPoint.Y < y2)
                                     || (Math.Abs(thisPoint.Y - y1) < 2 * sensitiveDistance && thisPoint.X > x1 && thisPoint.X < x2)
                                     || (Math.Abs(thisPoint.Y - y2) < 2 * sensitiveDistance && thisPoint.X > x1 && thisPoint.X < x2))
                                {
                                    DeleteDrawingBackup.Add(DrawingBackup[i]);
                                    DrawingBackup.RemoveAt(i);
                                    DeleteOrClear = false;
                                    DeleteOnce = true;
                                }
                                break;
                            case MouseStatus.DrawingPoint:
                                double distanceToPoint = Math.Sqrt((thisPoint.X - DrawingBackup[i].point1.X) * (thisPoint.X - DrawingBackup[i].point1.X) +
                                    (thisPoint.Y - DrawingBackup[i].point1.Y) * (thisPoint.Y - DrawingBackup[i].point1.Y));
                                if (distanceToPoint < 2 * sensitiveDistance)
                                {
                                    DeleteDrawingBackup.Add(DrawingBackup[i]);
                                    DrawingBackup.RemoveAt(i);
                                    DeleteOrClear = false;
                                    DeleteOnce = true;
                                }
                                break;
                            default:
                                break;
                        }
                        if (DeleteOnce == true)
                            break;
                    }
                    break;
                default:
                    break;
            }
        }
        string last_distance = "";
        private void LEDShowPoint(string distance)
        {
            if (distance.Length == 1) distance = " 0" + distance + " ";
            else if (distance.Length == 2) distance = " " + distance + " ";
            else if (distance.Length == 3) distance = distance + " ";
            else if (distance.Length > 4) distance = distance.Substring(0, 4);
            if (distance != last_distance)
            {
                WriteLedMessage(6, 0, "", distance);
                last_distance = distance;
            }
        }
        private void CameraImageBox_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (!CameraImageBox.IsMouseCaptured) return;
            CameraImageBox.ReleaseMouseCapture();
            Point thisPoint = e.GetPosition(e.Source as FrameworkElement);
            thisPoint.X = offset.X + thisPoint.X * ImageWidth / CameraImageBox.Width;
            thisPoint.Y = offset.Y + thisPoint.Y * ImageHeight / CameraImageBox.Height;
            switch (nowDrawing)
            {
                case MouseStatus.DrawingLine:
                    if (nowShape.point1.X != 0 && nowShape.point1.Y != 0)
                    {
                        nowShape.point2 = thisPoint;
                        if ((timer == null || !timer.IsEnabled) && nowShape.point2.X != 0 && nowShape.point2.Y != 0)
                        {
                            WriteableBitmapExtensions.DrawLineAa(ImageBackup, (int)nowShape.point1.X, (int)nowShape.point1.Y, (int)nowShape.point2.X,
                                (int)nowShape.point2.Y, nowShape.color);
                            CameraImageBox.Source = ImageBackup;
                        }
                    }

                    nowDrawing = MouseStatus.ZoomAndPlug;
                    lastShape = nowShape;
                    nowShape = new DrawShape();
                    CameraImageBox.Cursor = Cursors.Hand;
                    break;
                case MouseStatus.DrawingRect:
                    if (nowShape.point1.X != 0 && nowShape.point1.Y != 0)
                    {
                        nowShape.point2 = thisPoint;
                        if ((timer == null || !timer.IsEnabled) && nowShape.point2.X != 0 && nowShape.point2.Y != 0)
                        {
                            WriteableBitmapExtensions.DrawRectangle(ImageBackup, (int)nowShape.point1.X, (int)nowShape.point1.Y,
                                (int)nowShape.point2.X, (int)nowShape.point2.Y, nowShape.color);
                            CameraImageBox.Source = ImageBackup;
                        }
                    }
                    nowDrawing = MouseStatus.ZoomAndPlug;
                    lastShape = nowShape;
                    nowShape = new DrawShape();
                    CameraImageBox.Cursor = Cursors.Hand;
                    break;
                case MouseStatus.DrawingPoint:
                    if (nowShape.point1.X != 0 && nowShape.point1.Y != 0)
                    {
                        nowShape.point1 = thisPoint;
                        if (timer == null || !timer.IsEnabled)
                        {
                            WriteableBitmapExtensions.FillEllipseCentered(ImageBackup, (int)nowShape.point1.X, (int)nowShape.point1.Y, 2, 2, nowShape.color);
                            CameraImageBox.Source = ImageBackup;
                        }
                    }
                    nowDrawing = MouseStatus.ZoomAndPlug;
                    lastShape = nowShape;
                    PWindow Ptest = new PWindow();
                    try
                    {
                        Ptest.PassBetweenForm += new PWindow.PassBetweenFormHandler(PWindow_PassBetweenForm);
                        Ptest.Owner = this;
                        Ptest.WindowStartupLocation = System.Windows.WindowStartupLocation.Manual;
                        Ptest.Show();
                        Ptest.Activate();
                    }
                    catch (Exception ex)
                    {
                        Xceed.Wpf.Toolkit.MessageBox.Show(ex.Message);
                    }
                    nowShape = new DrawShape();
                    CameraImageBox.Cursor = Cursors.Hand;
                    break;
                case MouseStatus.Delete:
                    break;
                default:
                    nowDrawing = MouseStatus.ZoomAndPlug;
                    lastShape = nowShape;
                    nowShape = new DrawShape();
                    CameraImageBox.Cursor = Cursors.Hand;
                    break;
            }
            ClearGhostGraphic();
            //DrawingBackup.Sort(shapeComparison);
        }
        private void CameraImageBox_MouseMove(object sender, MouseEventArgs e)
        {
            Point thisPoint = e.GetPosition(e.Source as FrameworkElement);
            thisPoint.X = offset.X + thisPoint.X * ImageWidth / CameraImageBox.Width;
            thisPoint.Y = offset.Y + thisPoint.Y * ImageHeight / CameraImageBox.Height;
            label_Position.Content = string.Format("x:{0:f} y:{1:f}", thisPoint.X, thisPoint.Y);
            if (TI4_OpenPara && ((nowDrawing == MouseStatus.ZoomAndPlug && e.LeftButton == MouseButtonState.Released && e.RightButton == MouseButtonState.Released)
                || nowDrawing == MouseStatus.DragLine || nowDrawing == MouseStatus.DragRect || nowDrawing == MouseStatus.DragPoint))
            {
                ImageToolTip.Visibility = Visibility.Hidden;
                if (!CameraImageBox.IsMouseCaptured) CameraImageBox.Cursor = Cursors.Hand;
                foreach (var shape in DrawingBackup)
                {
                    if (shape.point1.X == 0 && shape.point1.Y == 0) continue;
                    switch (shape.type)
                    {
                        case MouseStatus.DrawingLine:
                            if (shape.point2.X == 0 && shape.point2.Y == 0) continue;
                            double k = (shape.point2.Y - shape.point1.Y) / (shape.point2.X - shape.point1.X);
                            double angle = Math.Atan(k);
                            Cursor doubleArrowCursor = CameraImageBox.Cursor;
                            //label_Position_Copy.Content = string.Format("x:{0:f} y:{1:f}", angle, thisPoint.Y);
                            if (angle < -Math.PI * 5 / 12)
                            {
                                doubleArrowCursor = Cursors.SizeNS;
                            }
                            else if (angle < -Math.PI / 12)
                            {
                                doubleArrowCursor = Cursors.SizeNESW;
                            }
                            else if (angle < Math.PI / 12)
                            {
                                doubleArrowCursor = Cursors.SizeWE;
                            }
                            else if (angle < Math.PI * 5 / 12)
                            {
                                doubleArrowCursor = Cursors.SizeNWSE;
                            }
                            else
                            {
                                doubleArrowCursor = Cursors.SizeNS;
                            }
                            double distanceToPoint1 = Math.Sqrt((thisPoint.X - shape.point1.X) * (thisPoint.X - shape.point1.X) +
                                (thisPoint.Y - shape.point1.Y) * (thisPoint.Y - shape.point1.Y));
                            double distanceToPoint2 = Math.Sqrt((thisPoint.X - shape.point2.X) * (thisPoint.X - shape.point2.X) +
                                (thisPoint.Y - shape.point2.Y) * (thisPoint.Y - shape.point2.Y));
                            double distanceToLine = Math.Abs(k * thisPoint.X - thisPoint.Y - k * shape.point1.X + shape.point1.Y) / Math.Sqrt(k * k + 1);
                            //label_Position_Copy.Content = string.Format("x:{0:f} y:{1:f}", distanceToPoint1, distanceToPoint2);
                            //端点
                            if (distanceToPoint1 < sensitiveDistance)
                            {
                                CameraImageBox.Cursor = doubleArrowCursor;
                            }
                            else if (distanceToPoint2 < sensitiveDistance)
                            {
                                CameraImageBox.Cursor = doubleArrowCursor;
                            }
                            else if (distanceToLine < sensitiveDistance
                                && thisPoint.X > Math.Min(shape.point1.X, shape.point2.X) && thisPoint.X < Math.Max(shape.point1.X, shape.point2.X)
                                && thisPoint.X > Math.Min(shape.point1.X, shape.point2.X) && thisPoint.X < Math.Max(shape.point1.X, shape.point2.X))
                            {
                                CameraImageBox.Cursor = Cursors.SizeAll;
                                if (!CameraImageBox.IsMouseCaptured)
                                {
                                    ImageToolTip.Content = shape.prompt;
                                    ImageToolTip.Visibility = Visibility.Visible;
                                    ImageToolTip.SetValue(Canvas.LeftProperty, e.GetPosition(ToolTipCanvas).X + 5);
                                    ImageToolTip.SetValue(Canvas.TopProperty, e.GetPosition(ToolTipCanvas).Y + 5);
                                }
                            }
                            break;
                        case MouseStatus.DrawingRect:
                            if (shape.point2.X == 0 && shape.point2.Y == 0) continue;
                            double x1 = (double)shape.point1.X;
                            double y1 = (double)shape.point1.Y;
                            double x2 = (double)shape.point2.X;
                            double y2 = (double)shape.point2.Y;
                            double t;
                            if (x1 > x2)
                            {
                                t = x1; x1 = x2; x2 = t;
                            }
                            if (y1 > y2)
                            {
                                t = y1; y1 = y2; y2 = t;
                            }
                            double distanceToNWPoint = Math.Sqrt((thisPoint.X - x1) * (thisPoint.X - x1) + (thisPoint.Y - y1) * (thisPoint.Y - y1));
                            double distanceToNEPoint = Math.Sqrt((thisPoint.X - x2) * (thisPoint.X - x2) + (thisPoint.Y - y1) * (thisPoint.Y - y1));
                            double distanceToSWPoint = Math.Sqrt((thisPoint.X - x1) * (thisPoint.X - x1) + (thisPoint.Y - y2) * (thisPoint.Y - y2));
                            double distanceToSEPoint = Math.Sqrt((thisPoint.X - x2) * (thisPoint.X - x2) + (thisPoint.Y - y2) * (thisPoint.Y - y2));
                            bool needTooltip = true;
                            if (distanceToNWPoint < sensitiveDistance)
                            {
                                CameraImageBox.Cursor = Cursors.SizeNWSE;
                            }
                            else if (distanceToNEPoint < sensitiveDistance)
                            {
                                CameraImageBox.Cursor = Cursors.SizeNESW;
                            }
                            else if (distanceToSWPoint < sensitiveDistance)
                            {
                                CameraImageBox.Cursor = Cursors.SizeNESW;
                            }
                            else if (distanceToSEPoint < sensitiveDistance)
                            {
                                CameraImageBox.Cursor = Cursors.SizeNWSE;
                            }
                            else if (Math.Abs(thisPoint.X - x1) < sensitiveDistance && thisPoint.Y > y1 && thisPoint.Y < y2)
                            {
                                CameraImageBox.Cursor = Cursors.SizeWE;
                                if (!CameraImageBox.IsMouseCaptured)
                                {
                                    ImageToolTip.Visibility = Visibility.Visible;
                                    ImageToolTip.Content = shape.prompt;
                                }
                            }
                            else if (Math.Abs(thisPoint.X - x2) < sensitiveDistance && thisPoint.Y > y1 && thisPoint.Y < y2)
                            {
                                CameraImageBox.Cursor = Cursors.SizeWE;
                                if (!CameraImageBox.IsMouseCaptured)
                                {
                                    ImageToolTip.Content = shape.prompt;
                                    ImageToolTip.Visibility = Visibility.Visible;
                                }
                            }
                            else if (Math.Abs(thisPoint.Y - y1) < sensitiveDistance && thisPoint.X > x1 && thisPoint.X < x2)
                            {
                                CameraImageBox.Cursor = Cursors.SizeNS;
                                if (!CameraImageBox.IsMouseCaptured)
                                {
                                    ImageToolTip.Content = shape.prompt;
                                    ImageToolTip.Visibility = Visibility.Visible;
                                }
                            }
                            else if (Math.Abs(thisPoint.Y - y2) < sensitiveDistance && thisPoint.X > x1 && thisPoint.X < x2)
                            {
                                CameraImageBox.Cursor = Cursors.SizeNS;
                            }
                            else
                            {
                                needTooltip = false;
                            }
                            if (needTooltip && !CameraImageBox.IsMouseCaptured)
                            {
                                ImageToolTip.Content = shape.prompt;
                                ImageToolTip.Visibility = Visibility.Visible;
                                ImageToolTip.SetValue(Canvas.LeftProperty, e.GetPosition(ToolTipCanvas).X + 5);
                                ImageToolTip.SetValue(Canvas.TopProperty, e.GetPosition(ToolTipCanvas).Y + 5);
                            }
                            break;
                        case MouseStatus.DrawingPoint:
                            double distanceToPoint = Math.Sqrt((thisPoint.X - shape.point1.X) * (thisPoint.X - shape.point1.X) +
                                (thisPoint.Y - shape.point1.Y) * (thisPoint.Y - shape.point1.Y));
                            if (distanceToPoint < sensitiveDistance)
                            {
                                CameraImageBox.Cursor = Cursors.SizeAll;
                                LEDShowPoint(shape.prompt);
                                if (!CameraImageBox.IsMouseCaptured)
                                {
                                    ImageToolTip.Visibility = Visibility.Visible;
                                    ImageToolTip.Content = shape.prompt;
                                    ImageToolTip.SetValue(Canvas.LeftProperty, e.GetPosition(ToolTipCanvas).X + 5);
                                    ImageToolTip.SetValue(Canvas.TopProperty, e.GetPosition(ToolTipCanvas).Y + 5);
                                }
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
            if (!CameraImageBox.IsMouseCaptured) return;
            if (thisPoint.X < 0 || thisPoint.X > ImageWidth || thisPoint.Y < 0 || thisPoint.Y > ImageHeight)
                return;
            switch (nowDrawing)
            {
                case MouseStatus.DrawingLine:
                    if (nowShape.point1.X != 0 && nowShape.point1.Y != 0)
                    {
                        nowShape.point2 = thisPoint;
                    }
                    break;
                case MouseStatus.DrawingRect:
                    if (nowShape.point1.X != 0 && nowShape.point1.Y != 0)
                    {
                        nowShape.point2 = thisPoint;
                    }
                    break;
                case MouseStatus.DrawingPoint:
                    if (nowShape.point1.X != 0 && nowShape.point1.Y != 0)
                    {
                        nowShape.point1 = thisPoint;
                    }
                    break;
                case MouseStatus.ZoomAndPlug:
                    if (e.RightButton == MouseButtonState.Pressed)
                    {
                        var trans = (TranslateTransform)((TransformGroup)CameraImageBox.RenderTransform).Children.First(tr => tr is TranslateTransform);
                        Point end = e.GetPosition(border);
                        Point posGroup = e.GetPosition(Group2);
                        Vector v = start - end;
                        var transX = origin.X - v.X;
                        var transY = origin.Y - v.Y;
                        TransformGroup transformGroup = (TransformGroup)CameraImageBox.RenderTransform;
                        ScaleTransform scale = (ScaleTransform)transformGroup.Children[0];

                        //label_Position_Copy.Content = string.Format("x:{0},y:{1}", (int)(minImageBorder - Group2.ActualWidth / 2 - CameraImageBox.ActualWidth * scale.ScaleX / 2), (int)(minImageBorder - Group2.ActualHeight / 2 - CameraImageBox.ActualHeight * scale.ScaleY / 2));
                        if (transX < minImageBorder - Group2.ActualWidth / 2 - CameraImageBox.ActualWidth * scale.ScaleX / 2)
                            transX = minImageBorder - Group2.ActualWidth / 2 - CameraImageBox.ActualWidth * scale.ScaleX / 2;
                        if (transY < minImageBorder - Group2.ActualHeight / 2 - CameraImageBox.ActualHeight * scale.ScaleY / 2)
                            transY = minImageBorder - Group2.ActualHeight / 2 - CameraImageBox.ActualHeight * scale.ScaleY / 2;
                        if (transX > Group2.ActualWidth / 2 + CameraImageBox.ActualWidth * scale.ScaleX / 2 - minImageBorder)
                            transX = Group2.ActualWidth / 2 + CameraImageBox.ActualWidth * scale.ScaleX / 2 - minImageBorder;
                        if (transY > Group2.ActualHeight / 2 + CameraImageBox.ActualHeight * scale.ScaleY / 2 - minImageBorder)
                            transY = Group2.ActualHeight / 2 + CameraImageBox.ActualHeight * scale.ScaleY / 2 - minImageBorder;
                        trans.X = transX;
                        trans.Y = transY;
                    }
                    break;
                case MouseStatus.DragMidLine://20160622
                    switch (nowDragLine)
                    {
                        case DragLineStatus.DragPoint1:
                        case DragLineStatus.DragPoint2:
                        case DragLineStatus.DragLine:
                            midline.point1 = midline.point1 + (thisPoint - start);
                            midline.point2 = midline.point2 + (thisPoint - start);
                            start = thisPoint;
                            break;
                    }
                    break;
                case MouseStatus.DragLine:
                    switch (nowDragLine)
                    {
                        case DragLineStatus.DragPoint1:
                            foreach (var shape in DrawingBackup)
                            {
                                if (shape.type == MouseStatus.DrawingPoint && shape.prompt.Contains(nowShape.prompt) == true)
                                {
                                    double line_k = 0;
                                    double line_k2 = 0;
                                    double line_b2 = 0;
                                    if (nowShape.point1.X == nowShape.point2.X)
                                        line_k = 9999;
                                    else line_k = (nowShape.point1.Y - nowShape.point2.Y) / (nowShape.point1.X - nowShape.point2.X);
                                    double line_b = nowShape.point1.Y - line_k * nowShape.point1.X;
                                    if (line_k == 0)
                                    {
                                        line_k2 = 9999;
                                        line_b2 = shape.point1.Y - line_k2 * shape.point1.X;
                                    }
                                    else
                                    {
                                        line_k2 = -1 / line_k;
                                        line_b2 = shape.point1.Y + shape.point1.X / line_k;
                                    }
                                    double cross_X = (line_b - line_b2) / (line_k2 - line_k);
                                    double cross_Y = cross_X * line_k + line_b;
                                    double offset_X = shape.point1.X - cross_X;
                                    double offset_Y = shape.point1.Y - cross_Y;
                                    double offset_XY = Math.Sqrt(Math.Pow(offset_X, 2)
                                        + Math.Pow(offset_Y, 2));
                                    if (nowShape.point1.Y == nowShape.point2.Y)
                                    {
                                        cross_Y = thisPoint.Y + (nowShape.point2.Y - thisPoint.Y) * (nowShape.point1.X - cross_X) / (nowShape.point1.X - nowShape.point2.X);
                                        cross_X = thisPoint.X + (nowShape.point2.X - thisPoint.X) * (nowShape.point1.X - cross_X) / (nowShape.point1.X - nowShape.point2.X);
                                    }
                                    else
                                    {
                                        cross_X = thisPoint.X + (nowShape.point2.X - thisPoint.X) * (nowShape.point1.Y - cross_Y) / (nowShape.point1.Y - nowShape.point2.Y);
                                        cross_Y = thisPoint.Y + (nowShape.point2.Y - thisPoint.Y) * (nowShape.point1.Y - cross_Y) / (nowShape.point1.Y - nowShape.point2.Y);
                                    }
                                    line_k = (thisPoint.Y - nowShape.point2.Y) / (thisPoint.X - nowShape.point2.X);
                                    if (line_k == 0)
                                    {
                                        shape.point1.X = cross_X;
                                        if (offset_Y < 0)
                                            shape.point1.Y = cross_Y - offset_XY;
                                        else
                                            shape.point1.Y = cross_Y + offset_XY;
                                    }
                                    else
                                    {
                                        if (offset_X > 0)
                                            shape.point1.X = cross_X + offset_XY / Math.Sqrt(1 + 1 / Math.Pow(line_k, 2));
                                        else
                                            shape.point1.X = cross_X - offset_XY / Math.Sqrt(1 + 1 / Math.Pow(line_k, 2));
                                        if (offset_Y > 0)
                                            shape.point1.Y = cross_Y + offset_XY / Math.Sqrt(1 + Math.Pow(line_k, 2));
                                        else
                                            shape.point1.Y = cross_Y - offset_XY / Math.Sqrt(1 + Math.Pow(line_k, 2));
                                    }
                                }
                                else continue;
                            }
                            nowShape.point1 = thisPoint;
                            break;
                        case DragLineStatus.DragPoint2:
                            foreach (var shape in DrawingBackup)
                            {
                                if (shape.type == MouseStatus.DrawingPoint && shape.prompt.Contains(nowShape.prompt) == true)
                                {
                                    double line_k = 0;
                                    double line_k2 = 0;
                                    double line_b2 = 0;
                                    if (nowShape.point1.X == nowShape.point2.X)
                                        line_k = 9999;
                                    else line_k = (nowShape.point1.Y - nowShape.point2.Y) / (nowShape.point1.X - nowShape.point2.X);
                                    double line_b = nowShape.point1.Y - line_k * nowShape.point1.X;
                                    if (line_k == 0)
                                    {
                                        line_k2 = 9999;
                                        line_b2 = shape.point1.Y - line_k2 * shape.point1.X;
                                    }
                                    else
                                    {
                                        line_k2 = -1 / line_k;
                                        line_b2 = shape.point1.Y + shape.point1.X / line_k;
                                    }
                                    double cross_X = (line_b - line_b2) / (line_k2 - line_k);
                                    double cross_Y = cross_X * line_k + line_b;
                                    double offset_X = shape.point1.X - cross_X;
                                    double offset_Y = shape.point1.Y - cross_Y;
                                    double offset_XY = Math.Sqrt(Math.Pow(offset_X, 2)
                                        + Math.Pow(offset_Y, 2));
                                    if (nowShape.point1.Y == nowShape.point2.Y)
                                    {
                                        cross_Y = thisPoint.Y + (nowShape.point1.Y - thisPoint.Y) * (nowShape.point2.X - cross_X) / (nowShape.point2.X - nowShape.point1.X);
                                        cross_X = thisPoint.X + (nowShape.point1.X - thisPoint.X) * (nowShape.point2.X - cross_X) / (nowShape.point2.X - nowShape.point1.X);
                                    }
                                    else
                                    {
                                        cross_X = thisPoint.X + (nowShape.point1.X - thisPoint.X) * (nowShape.point2.Y - cross_Y) / (nowShape.point2.Y - nowShape.point1.Y);
                                        cross_Y = thisPoint.Y + (nowShape.point1.Y - thisPoint.Y) * (nowShape.point2.Y - cross_Y) / (nowShape.point2.Y - nowShape.point1.Y);
                                    }
                                    line_k = (thisPoint.Y - nowShape.point1.Y) / (thisPoint.X - nowShape.point1.X);
                                    if (line_k == 0)
                                    {
                                        shape.point1.X = cross_X;
                                        shape.point1.Y = cross_Y - offset_XY;
                                    }
                                    else
                                    {
                                        shape.point1.X = cross_X - offset_XY / Math.Sqrt(1 + 1 / Math.Pow(line_k, 2));
                                        shape.point1.Y = cross_Y - offset_XY / Math.Sqrt(1 + Math.Pow(line_k, 2));
                                    }
                                    continue;
                                }
                                else continue;
                            }
                            nowShape.point2 = thisPoint;
                            break;
                        case DragLineStatus.DragLine:
                            foreach (var shape in DrawingBackup)
                            {
                                if (shape.type == MouseStatus.DrawingPoint && shape.prompt.Contains(nowShape.prompt) == true)
                                    shape.point1 = shape.point1 + (thisPoint - start);
                            }
                            nowShape.point1 = nowShape.point1 + (thisPoint - start);
                            nowShape.point2 = nowShape.point2 + (thisPoint - start);
                            start = thisPoint;
                            break;
                    }

                    break;
                case MouseStatus.DragRect:
                    switch (nowDragRect)
                    {
                        case DragRectStatus.DragNWPoint:
                            nowShape.point1 = thisPoint;
                            break;
                        case DragRectStatus.DragNEPoint:
                            nowShape.point1.Y = thisPoint.Y;
                            nowShape.point2.X = thisPoint.X;
                            break;
                        case DragRectStatus.DragSWPoint:
                            nowShape.point1.X = thisPoint.X;
                            nowShape.point2.Y = thisPoint.Y;
                            break;
                        case DragRectStatus.DragSEPoint:
                            nowShape.point2 = thisPoint;
                            break;
                        case DragRectStatus.DragNLine:
                            nowShape.point1.Y = thisPoint.Y;
                            break;
                        case DragRectStatus.DragSLine:
                            nowShape.point2.Y = thisPoint.Y;
                            break;
                        case DragRectStatus.DragWLine:
                            nowShape.point1.X = thisPoint.X;
                            break;
                        case DragRectStatus.DragELine:
                            nowShape.point2.X = thisPoint.X;
                            break;
                    }
                    break;
                case MouseStatus.DragPoint:
                    nowShape.point1 = thisPoint;
                    break;
                default:
                    break;
            }
            nowShape.DoChange();
        }
        private void CameraImageBox_MouseWheel(object sender, MouseWheelEventArgs e)
        {
            TransformGroup transformGroup = (TransformGroup)CameraImageBox.RenderTransform;
            ScaleTransform transform = (ScaleTransform)transformGroup.Children[0];
            double zoom = e.Delta > 0 ? .2 : -.2;
            transform.ScaleX += zoom;
            transform.ScaleY += zoom;
            if (transform.ScaleX < 0.5) transform.ScaleX = 0.5;
            if (transform.ScaleY < 0.5) transform.ScaleY = 0.5;
            sensitiveDistance = (int)(Convert.ToInt32(ConfigurationManager.AppSettings["SensitiveDistance"]) / transform.ScaleX);
            sensitiveDistance = sensitiveDistance < 3 ? 3 : sensitiveDistance;
        }
        private void CameraImageBox_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
            CameraImageBox.CaptureMouse();
            Point thisPoint = e.GetPosition(e.Source as FrameworkElement);
            thisPoint.X = offset.X + thisPoint.X * ImageWidth / CameraImageBox.Width;
            thisPoint.Y = offset.Y + thisPoint.Y * ImageHeight / CameraImageBox.Height;
            for (int i = DrawingBackup.Count - 1; i >= 0; i--)
            {
                if (DrawingBackup[i].point1.X == 0 && DrawingBackup[i].point1.Y == 0) continue;
                switch (DrawingBackup[i].type)
                {
                    case MouseStatus.DrawingLine:
                        if (DrawingBackup[i].point2.X == 0 && DrawingBackup[i].point2.Y == 0) continue;
                        double k = (DrawingBackup[i].point2.Y - DrawingBackup[i].point1.Y) / (DrawingBackup[i].point2.X - DrawingBackup[i].point1.X);
                        double distanceToPoint1 = Math.Sqrt((thisPoint.X - DrawingBackup[i].point1.X) * (thisPoint.X - DrawingBackup[i].point1.X) +
                                         (thisPoint.Y - DrawingBackup[i].point1.Y) * (thisPoint.Y - DrawingBackup[i].point1.Y));
                        double distanceToPoint2 = Math.Sqrt((thisPoint.X - DrawingBackup[i].point2.X) * (thisPoint.X - DrawingBackup[i].point2.X) +
                                         (thisPoint.Y - DrawingBackup[i].point2.Y) * (thisPoint.Y - DrawingBackup[i].point2.Y));
                        double distanceToLine = Math.Abs(k * thisPoint.X - thisPoint.Y - k * DrawingBackup[i].point1.X + DrawingBackup[i].point1.Y) / Math.Sqrt(k * k + 1);
                        if (distanceToPoint1 < sensitiveDistance)
                        {
                            ContextMenuKB = true;
                            now = i;
                            XContextMenu.Visibility = Visibility.Visible;
                        }
                        else if (distanceToPoint2 < sensitiveDistance)
                        {
                            ContextMenuKB = true;
                            now = i;
                            XContextMenu.Visibility = Visibility.Visible;
                        }
                        else if (distanceToLine < sensitiveDistance
                                        && thisPoint.X > Math.Min(DrawingBackup[i].point1.X, DrawingBackup[i].point2.X) && thisPoint.X < Math.Max(DrawingBackup[i].point1.X, DrawingBackup[i].point2.X)
                                        && thisPoint.X > Math.Min(DrawingBackup[i].point1.X, DrawingBackup[i].point2.X) && thisPoint.X < Math.Max(DrawingBackup[i].point1.X, DrawingBackup[i].point2.X))
                        {
                            ContextMenuKB = true;
                            now = i;
                            XContextMenu.Visibility = Visibility.Visible;
                        }
                        break;
                    default:
                        break;
                }
            }
            switch (nowDrawing)
            {
                case MouseStatus.DrawingLine:
                case MouseStatus.DrawingPoint:
                case MouseStatus.DrawingRect:
                    if (e.LeftButton == MouseButtonState.Pressed)
                    {
                        DrawingBackup.Remove(nowShape);
                        nowShape = new DrawShape();
                    }
                    else
                    {
                        nowDrawing = MouseStatus.ZoomAndPlug;
                        //nowShape = new DrawShape();
                        CameraImageBox.Cursor = Cursors.Hand;
                    }
                    e.Handled = true;
                    break;
                case MouseStatus.Delete:
                    nowDrawing = MouseStatus.ZoomAndPlug;
                    CameraImageBox.Cursor = Cursors.Hand;
                    e.Handled = true;
                    break;
                case MouseStatus.ZoomAndPlug:
                    var tt = (TranslateTransform)((TransformGroup)CameraImageBox.RenderTransform).Children.First(tr => tr is TranslateTransform);
                    start = e.GetPosition(border);
                    origin = new Point(tt.X, tt.Y);
                    break;
                default:
                    break;
            }
        }
        private void CameraImageBox_MouseRightButtonUp(object sender, MouseButtonEventArgs e)
        {
            CameraImageBox.ReleaseMouseCapture();
            if (!ContextMenuKB) CameraImageBox.ContextMenu.IsOpen = false;
        }
        #endregion
        #region 按钮函数
        private void button_Open_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog myDialog = new OpenFileDialog();

            myDialog.Filter = "Image Files(*.BMP;*.JPG;*.GIF)|*.BMP;*.JPG;*.GIF" +
              "|All files (*.*)|*.*";
            myDialog.CheckFileExists = true;
            myDialog.Multiselect = true;
        }
        private void CheckCameraOpen()
        {
            Thread.Sleep(1500);
            if (Camera.Started == false)
            {
                System.Windows.MessageBox.Show("相机开启失败！");
            }
        }
        private void button_Play_Click(object sender, RoutedEventArgs e)
        {
            if (PvCamera == null)
            {
                PvCamera = new Camera();
            }
            if (!PvCamera.Inited)
            {
                try
                {
                    tErr err = PvCamera.InitCamera();
                    if (err != tErr.eErrSuccess)
                    {
                        System.Windows.MessageBox.Show("相机初始化失败！" + "错误：" + err.ToString());
                        return;
                    }
                    else
                    {
                        Task CheckOpenTask = Task.Factory.StartNew(CheckCameraOpen);
                    }
                }
                catch
                {
                    System.Windows.MessageBox.Show("相机初始化失败！");
                    return;
                }
            }
            if (Camera.Started == false)
            {
                Camera.HandleCameraPlugged(Camera.Camera_UniqueId);
            }
            for (int wait_time = 0; wait_time < 10; wait_time++)
            {
                if (Camera.Started == true) break;
                Thread.Sleep(50);
            }
            if (Camera.Started == false)
            {
                Xceed.Wpf.Toolkit.MessageBox.Show("请先打开相机", "相机", MessageBoxButton.OK);
                return;
            }
            if ((string)Tile_Play.Title == "开启相机")
            {   // 开启视频
                //timer = new DispatcherTimer();
                try
                {
                    timer.Tick -= new EventHandler(timer_Tick_static);
                    timer.Tick += new EventHandler(timer_Tick);
                    System.Threading.Thread.Sleep(100);
                    //timer.Interval = new TimeSpan(0, 0, 0, 0, 30);//30ms刷一次
                    //timer.Start();
                    Tile_Play.Title = "关闭相机";
                    Tile_Play.IsEnabled = false;

                    Thread enableThread = new Thread(new ThreadStart(delegate()
                    {
                        Thread.Sleep(3000);
                        Dispatcher.Invoke(new Action(() =>
                        {
                            Tile_Play.IsEnabled = true;
                        }));
                    }));
                    enableThread.Start();
                    if (!XBottomSide.IsFloating)
                        XBottomSide.Float();
                }
                catch
                {
                    Xceed.Wpf.Toolkit.MessageBox.Show("相机开启失败！");
                    return;
                }
            }
            else
            {
                timer.Tick -= new EventHandler(timer_Tick);
                System.Threading.Thread.Sleep(100);
                //ImageBackup = writeableSource.Clone();
                System.Drawing.Bitmap BitmapImg = PvCamera.getFrame();
                if (BitmapImg != null)
                {
                    ImageWidth = BitmapImg.Width;
                    ImageHeight = BitmapImg.Height;
                    //label_Position_Copy.Content = string.Format("x:{0:f} y:{1:f}", ImageWidth, ImageHeight);

                    //System.Drawing.Bitmap BitmapImg = currentFrame.Bitmap;
                    BitmapSource source = ToBitmapSource(BitmapImg);

                    CameraImageBox.Source = source;

                    WriteableBitmap tmp = new WriteableBitmap(source);
                    ImageBackup = BitmapFactory.ConvertToPbgra32Format(tmp);
                }
                timer.Tick += new EventHandler(timer_Tick_static);
                Tile_Play.Title = "开启相机";
                Tile_Play.IsEnabled = false;
                Thread enableThread = new Thread(new ThreadStart(delegate()
                {
                    Thread.Sleep(3000);
                    Dispatcher.Invoke(new Action(() =>
                    {
                        Tile_Play.IsEnabled = true;
                    }));
                }));
                enableThread.Start();
                if (XBottomSide.IsFloating)
                    XBottomSide.Dock();
            }
        }//开启相机相关
        private void button_SetModel_Click(object sender, RoutedEventArgs e)
        {
            now = 666;
            UpdatestoplineFormBoth();
            Model min = new Model();
            try
            {
                min.Owner = this;
                min.WindowStartupLocation = System.Windows.WindowStartupLocation.CenterOwner;
                min.Show();
                min.Activate();
                LinearGradientBrush brush = new LinearGradientBrush();
                brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                brush.GradientStops.Add(new GradientStop(color: Colors.Green, offset: 1));
                Checkbox_connect_plane.Fill = brush;
            }
            catch (Exception ex)
            {
                Xceed.Wpf.Toolkit.MessageBox.Show(ex.Message);
            }
        }//机型市值
        private void MenuGuide_Click(object sender, RoutedEventArgs e)
        {
            MWindow win = new MWindow();
            try
            {
                win.PassDataBetweenForm += new MWindow.PassDataBetweenFormHandler(MWindow_PassDataBetweenForm);
                win.Owner = this;
                win.WindowStartupLocation = System.Windows.WindowStartupLocation.CenterOwner;
                win.Show();
                win.Activate();
            }
            catch (Exception ex)
            {
                Xceed.Wpf.Toolkit.MessageBox.Show(ex.Message);
            }

        }//右键菜单
        private void MenuStop_Click(object sender, RoutedEventArgs e)
        {
            DropBox Bbox = new DropBox();
            try
            {
                Bbox.PassBetweenForm += new DropBox.PassBetweenFormHandler(DropBox_PassBetweenForm);
                Bbox.Owner = this;
                Bbox.WindowStartupLocation = System.Windows.WindowStartupLocation.CenterOwner;
                Bbox.Show();
                Bbox.Activate();
            }
            catch (Exception ex)
            {
                Xceed.Wpf.Toolkit.MessageBox.Show(ex.Message);
            }
        }
        private void MenuDelete_Click(object sender, RoutedEventArgs e)
        {
            if (now < 666)
            {
                DrawingBackup.RemoveAt(now);
            }
        }
        private void MenuWidth_Enter(object sender, RoutedEventArgs e)
        {
            WrapPanel1.Visibility = Visibility.Collapsed;
            WrapPanel4.Visibility = Visibility.Collapsed;
            WrapPanel5.Visibility = Visibility.Visible;
        }
        private void MenuColor_Enter(object sender, RoutedEventArgs e)
        {
            WrapPanel1.Visibility = Visibility.Collapsed;
            WrapPanel5.Visibility = Visibility.Collapsed;
            WrapPanel4.Visibility = Visibility.Visible;
        }
        private void WrapPanel4_Close(object sender, RoutedEventArgs e)
        {
            now = 666;
            WrapPanel4.Visibility = Visibility.Collapsed;
            WrapPanel1.Visibility = Visibility.Visible;
        }
        private void WrapPanel5_Close(object sender, RoutedEventArgs e)
        {
            now = 666;
            WrapPanel5.Visibility = Visibility.Collapsed;
            WrapPanel1.Visibility = Visibility.Visible;
        }
        private void button_SaveVideoSet_Click(object sender, RoutedEventArgs e)
        {
            MessageBoxResult result = Xceed.Wpf.Toolkit.MessageBox.Show("是否保存视觉配置", "保存", MessageBoxButton.YesNo);//显示确认窗口
            if (result == MessageBoxResult.Yes)
            {
                try
                {
                    SaveVisionParameter();
                    DeleteDrawingBackup.Clear();
                    Xceed.Wpf.Toolkit.MessageBox.Show("视觉参数保存成功！");
                }
                catch (Exception ex)
                {
                    Xceed.Wpf.Toolkit.MessageBox.Show("视觉参数保存失败！" + ex.Message);
                }
            }
            else
            {
                //无操作
            }
        }
        private void button_SaveAndEsc_Clcik(object sender, RoutedEventArgs e)
        {
            MessageBoxResult result = Xceed.Wpf.Toolkit.MessageBox.Show("是否保存视觉配置", "保存", MessageBoxButton.YesNo);//显示确认窗口
            if (result == MessageBoxResult.Yes)
            {
                try
                {
                    SaveVisionParameter();
                    DeleteDrawingBackup.Clear();
                    if (TI4_OpenPara == true)
                    {
                        TI5.Visibility = Visibility.Collapsed;
                        TI4_OpenPara = !TI4_OpenPara;
                    }
                    Xceed.Wpf.Toolkit.MessageBox.Show("视觉参数保存成功！");
                }
                catch (Exception ex)
                {
                    Xceed.Wpf.Toolkit.MessageBox.Show("视觉参数保存失败！" + ex.Message);
                }
            }
            else
            {
                //无操作
            }
        }
        private void button_nowSaveAndEsc_Clcik(object sender, RoutedEventArgs e)
        {
            TI5.Visibility = Visibility.Collapsed;
            TI4_OpenPara = !TI4_OpenPara;
        }
        private void ColorPickerSChange(object sender, RoutedPropertyChangedEventArgs<Color?> e)
        {
            if ((now < 666) && (DrawingBackup[now].type == MouseStatus.DrawingLine))
            {
                DrawingBackup[now].color = (System.Windows.Media.Color)XColorPicker.SelectedColor;
            }
        }
        void ContextMenu_Opened(object sender, RoutedEventArgs e)
        {
            if (now < 666)
            {
                if (DrawingBackup[now].prompt.Contains("停止线"))
                    MenuGuide.IsEnabled = false;
                else if (DrawingBackup[now].prompt.Contains("引导线"))
                    MenuStop.IsEnabled = false;
            }
            CameraImageBox.ContextMenu.IsOpen = ContextMenuKB;
            ContextMenuKB = false;
        }
        void ContextMenu_Closed(object sender, RoutedEventArgs e)
        {
            MenuGuide.IsEnabled = true;
            MenuStop.IsEnabled = true;
            XContextMenu.Visibility = Visibility.Collapsed;
        }
        private void ReadLaserPatameter()
        {
            Parameter.glb_Parameter = new Parameter(".\\test.xml");
            System.Xml.XmlNode LaserData = Parameter.glb_Parameter.laser_machine_datas_list;
            if (LaserData == null)
                return;
            try
            {
                CShareData.XEdgeStepSave = Convert.ToInt16(LaserData["HrzConfirmSteps"].InnerText);
                CShareData.YEdgeStepSave = Convert.ToInt16(LaserData["VertConfirmSteps"].InnerText);
                CShareData.Q12Steps = Convert.ToInt16(LaserData["Q12Steps"].InnerText);
                CShareData.XProbSave = Convert.ToDouble(LaserData["HrzConfirmProbability"].InnerText);
                CShareData.YProbSave = Convert.ToDouble(LaserData["VertConfirmProbability"].InnerText);
                Install_height.Text = Convert.ToInt32(Convert.ToSingle(LaserData["install_high"].InnerText)).ToString();
            }
            catch (Exception ex)
            {
                Xceed.Wpf.Toolkit.MessageBox.Show(ex.Message);
            }
        }
        private void ReadVisionParameter()
        {
            int[] trackRect = Parameter.glb_Parameter.ReadRectData("TrackRect");
            DrawShape trackRectShape = new DrawShape();
            trackRectShape.point1 = new Point(trackRect[0], trackRect[1]);
            trackRectShape.point2 = new Point(trackRect[0] + trackRect[3], trackRect[1] + trackRect[2]);//马莹这里写反了。。
            trackRectShape.prompt = "跟踪定位区";
            trackRectShape.type = MouseStatus.DrawingRect;
            trackRectShape.color = Color.FromRgb(0, 0, 255);
            DrawingBackup.Add(trackRectShape);

            int[] SearchRect = Parameter.glb_Parameter.ReadRectData("SearchRect");
            DrawShape SearchRectShape = new DrawShape();
            SearchRectShape.point1 = new Point(SearchRect[0], SearchRect[1]);
            SearchRectShape.point2 = new Point(SearchRect[0] + SearchRect[3], SearchRect[1] + SearchRect[2]);
            SearchRectShape.prompt = "捕获区域";
            SearchRectShape.type = MouseStatus.DrawingRect;
            SearchRectShape.color = Color.FromRgb(255, 0, 255);
            DrawingBackup.Add(SearchRectShape);

            int[] BridgeRect = Parameter.glb_Parameter.ReadRectData("BridgeRect");
            DrawShape BridgeRectShape = new DrawShape();
            BridgeRectShape.point1 = new Point(BridgeRect[0], BridgeRect[1]);
            BridgeRectShape.point2 = new Point(BridgeRect[0] + BridgeRect[3], BridgeRect[1] + BridgeRect[2]);
            BridgeRectShape.prompt = "登机桥位置";
            BridgeRectShape.type = MouseStatus.DrawingRect;
            BridgeRectShape.color = Color.FromRgb(255, 255, 255);
            DrawingBackup.Add(BridgeRectShape);

            List<KeyValuePair<String, Point>> Lines = Parameter.glb_Parameter.ReadPointDataList();
            foreach (var line in Lines)
            {
                //if (line.Key.IndexOf("stop") >= 0)
                //{
                if (line.Key.IndexOf("start") >= 0)
                {
                    DrawShape lineShape = new DrawShape();
                    lineShape.point1 = line.Value;
                    lineShape.prompt = line.Key.Substring(0, line.Key.IndexOf("_start"));
                    lineShape.type = MouseStatus.DrawingLine;
                    lineShape.color = Color.FromRgb(255, 255, 0);
                    DrawingBackup.Add(lineShape);
                }
                else if (line.Key.IndexOf("end") >= 0)
                {
                    int i;
                    for (i = DrawingBackup.Count - 1; i >= 0; i--)
                    {
                        if (DrawingBackup[i].prompt == line.Key.Substring(0, line.Key.IndexOf("_end")))
                        {
                            DrawingBackup[i].point2 = line.Value;
                            DrawingBackup[i].prompt = DrawingBackup[i].prompt.Replace("guide", "引导线");
                            DrawingBackup[i].prompt = DrawingBackup[i].prompt.Replace("_stop", "停止线");
                            //BUG潜在地点，标记一发
                            break;
                        }
                    }
                }
            }

            for (int i = 1; i < 10; i++)
            {
                List<KeyValuePair<double, Point>> DistancePoints = Parameter.glb_Parameter.readAllPoints("guide" + i.ToString());
                foreach (var p in DistancePoints)
                {
                    DrawShape pointShape = new DrawShape();
                    pointShape.type = MouseStatus.DrawingPoint;
                    pointShape.point1 = p.Value;
                    pointShape.prompt = p.Key.ToString();
                    pointShape.flag = i;
                    if(i == 1)
                        pointShape.color = Color.FromRgb(255, 0, 0);
                    else if(i==2)
                        pointShape.color = Color.FromRgb(0, 255, 0);
                    else if (i==3)
                        pointShape.color = Color.FromRgb(255, 0, 255);
                    else
                        pointShape.color = Color.FromRgb(0, 255, 255);
                    DrawingBackup.Add(pointShape);
                }
            }
            DrawingBackup.Sort(shapeComparison);
            UpdatestoplineFormVisual();
        }
        private void SaveVisionParameter()
        {
            ClearGhostGraphic();
            Parameter.glb_Parameter.DoNotSave = true;
            Parameter.glb_Parameter.ClearRectData();
            Parameter.glb_Parameter.ClearPoints();
            Parameter.glb_Parameter.ClearPointData();
            foreach (var shape in DrawingBackup)
            {
                switch (shape.type)
                {
                    case MouseStatus.DrawingLine:
                        string prompt = shape.prompt;
                        prompt = prompt.Replace("引导线", "guide");
                        prompt = prompt.Replace("停止线", "_stop");
                        Parameter.glb_Parameter.WritePointData(prompt + "_start", shape.point1);
                        Parameter.glb_Parameter.WritePointData(prompt + "_end", shape.point2);
                        break;
                    case MouseStatus.DrawingPoint:
                        Parameter.glb_Parameter.SavePoints("guide" + ((int)shape.flag).ToString(),
                            new KeyValuePair<double, Point>(Convert.ToDouble(shape.prompt), shape.point1));
                        break;
                    case MouseStatus.DrawingRect:
                        double x1, x2, y1, y2;
                        x1 = shape.point1.X;
                        y1 = shape.point1.Y;
                        x2 = shape.point2.X;
                        y2 = shape.point2.Y;
                        if (x1 > x2)
                        {
                            double temp = x1;
                            x1 = x2;
                            x2 = temp;
                        }
                        if (y1 > y2)
                        {
                            double temp = y1;
                            y1 = y2;
                            y2 = temp;
                        }
                        shape.point1.X = x1;
                        shape.point1.Y = y1;
                        shape.point2.X = x2;
                        shape.point2.Y = y2;
                        string RectName = "";
                        if (shape.prompt == "跟踪定位区") RectName = "TrackRect";
                        else if (shape.prompt == "捕获区域") RectName = "SearchRect";
                        else if (shape.prompt == "登机桥位置") RectName = "BridgeRect";
                        Parameter.glb_Parameter.WriteRectData(RectName,
                            new int[] { (int)shape.point1.X, (int)shape.point1.Y, 
                                (int)(shape.point2.Y - shape.point1.Y + 1), (int)(shape.point2.X - shape.point1.X + 1) });
                        break;
                }
            }
            Parameter.glb_Parameter.DoNotSave = false;
            Parameter.glb_Parameter.Save();
            UpdatestoplineFormVisual();
        }
        private void button_Vision_Para_Click(object sender, RoutedEventArgs e)
        {
            MessageBoxResult result = Xceed.Wpf.Toolkit.MessageBox.Show("是否保存视觉配置", "保存", MessageBoxButton.YesNo);//显示确认窗口
            if (result == MessageBoxResult.Yes)
            {
                try
                {
                    SaveVisionParameter();
                    DeleteDrawingBackup.Clear();
                    Xceed.Wpf.Toolkit.MessageBox.Show("视觉参数保存成功！");
                }
                catch (Exception ex)
                {
                    Xceed.Wpf.Toolkit.MessageBox.Show("视觉参数保存失败！" + ex.Message);
                }
            }
            else
            {
                //无操作
            }
        }
        private void button_SystemPara_Click(object sender, RoutedEventArgs e)
        {
            now = 666;
            SystemParameter min = new SystemParameter();
            try
            {
                min.Owner = this;
                min.WindowStartupLocation = System.Windows.WindowStartupLocation.CenterOwner;
                min.Show();
                min.Activate();
            }
            catch (Exception ex)
            {
                Xceed.Wpf.Toolkit.MessageBox.Show(ex.Message);
            }
        }
        private void AllMove(Vector offset)
        {
            foreach (var shape in DrawingBackup)
            {
                shape.point1 += offset;
                if (shape.point2.X != 0 && shape.point2.Y != 0) shape.point2 += offset;
            }
        }
        private void Progress_Fluout1_Click(object sender, RoutedEventArgs e)
        {
            //Progress_Fluout1.IsOpen = !Progress_Fluout1.IsOpen;//关闭侧边栏2018.4.10
        }
private void Image_Save_Click(object sender, RoutedEventArgs e)
        {
            if (CameraImageBox.Source == null)
                return;
            try
            {
                string LocalLogDir = Parameter.glb_Parameter.system_parameter["LogSaveRoot"].InnerText;
                if (LocalLogDir.StartsWith("\"") && LocalLogDir.EndsWith("\"")) LocalLogDir = LocalLogDir.Substring(1, LocalLogDir.Length - 2);
                LocalLogDir = LocalLogDir + "images\\";
                if (!Directory.Exists(LocalLogDir)) Directory.CreateDirectory(LocalLogDir);
                using (FileStream stream = new FileStream(LocalLogDir + System.DateTime.Now.Year.ToString() + "-"
                    + System.DateTime.Now.Month.ToString() + "-" + System.DateTime.Now.Day.ToString() +"-"
                    + System.DateTime.Now.Hour.ToString() + "-" + System.DateTime.Now.Minute.ToString() + "-"
                    + System.DateTime.Now.Second.ToString() + ".bmp", FileMode.Create))
                {
                    BmpBitmapEncoder encoder = new BmpBitmapEncoder();
                    encoder.Frames.Add(BitmapFrame.Create(CameraImageBox.Source as BitmapSource));
                    encoder.Save(stream);
                    stream.Close();
                    System.Windows.MessageBox.Show("相机图片保存成功！", "提示");
                }
            }
            catch (System.Exception ee) { System.Windows.MessageBox.Show(ee.ToString(), "错误"); }
            //WriteableBitmap tempimage = new WriteableBitmap(CameraImageBox.Source as BitmapSource);
        }
        #endregion
        #region image2dBOX 鼠标操作
        private void Image2dBox_MouseWheel(object sender, MouseWheelEventArgs e)
        {
            TransformGroup transformGroup2d = (TransformGroup)Image2dBox.RenderTransform;
            ScaleTransform transform2d = (ScaleTransform)transformGroup2d.Children[0];

            double zoom = e.Delta > 0 ? .2 : -.2;

            if (Xaxiszoom.IsChecked == true)
            {
                Xrenderi += zoom;
            }
            if (Yaxiszoom.IsChecked == true)
            {
                Yrenderi += zoom;
            }
            if (Xrenderi < 0.5) Xrenderi = 0.5;
            else if (Xrenderi > 16) Xrenderi = 16;
            if (Yrenderi < 0.5) Yrenderi = 0.5;
            else if (Yrenderi > 16) Yrenderi = 16;
            NeedFreshK();
        }
        private void Image2dBox_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Image2dBox.CaptureMouse();
            Point thisPoint = e.GetPosition(e.Source as FrameworkElement);
            thisPoint.X = offset.X + thisPoint.X * Width2d / Image2dBox.Width;
            thisPoint.Y = offset.Y + thisPoint.Y * Height2d / Image2dBox.Height;
            if (thisPoint.X < 32 || thisPoint.Y > 438) return;
        }
        private void Image2dBox_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (!Image2dBox.IsMouseCaptured) return;
            Image2dBox.ReleaseMouseCapture();
            nowDrawing2d = MouseStatus.ZoomAndPlug;
            Image2dBox.Cursor = Cursors.Hand;
        }
        private void Image2dBox_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
            Image2dBox.CaptureMouse();
            Point thisPoint = e.GetPosition(e.Source as FrameworkElement);
            thisPoint.X = offset.X + thisPoint.X * Width2d / Image2dBox.Width;
            thisPoint.Y = offset.Y + thisPoint.Y * Height2d / Image2dBox.Height;

            var tt = (TranslateTransform)((TransformGroup)Image2dBox.RenderTransform).Children.First(tr => tr is TranslateTransform);
            start2d = e.GetPosition(border2d);
            origin2d = new Point(tt.X, tt.Y);

        }
        private void Image2dBox_MouseRightButtonUp(object sender, MouseButtonEventArgs e)
        {
            Image2dBox.ReleaseMouseCapture();
        }
        private void Image2dBox_MouseMove(object sender, MouseEventArgs e)
        {
            Point thisPoint = e.GetPosition(e.Source as FrameworkElement);
            thisPoint.X = offset.X + thisPoint.X * Width2d / Image2dBox.Width;
            thisPoint.Y = offset.Y + thisPoint.Y * Height2d / Image2dBox.Height;
            if (needTransform1_2DataCs.guide_line_x == 0)
            {
                labelPosition.Content = (int)((262 - thisPoint.Y) * 8000 / (35 * Yrenderi) + Axis_y_mid);
            }
            else labelPosition.Content = string.Format("x:{0:f} y:{1:f}", (int)((thisPoint.X - 177) * 10000 / (145 * Xrenderi) +
                Axis_x_mid), (int)((262 - thisPoint.Y) * 8000 / (35 * Yrenderi) + Axis_y_mid));
            if (!Image2dBox.IsMouseCaptured) return;
            if (e.RightButton == MouseButtonState.Pressed)
            {
                Vector v = start2d - e.GetPosition(border2d);
                v.Y = -v.Y;
                start2d = e.GetPosition(border2d);

                Axis_x_mid += (v.X * 10000 / (145 * Xrenderi));
                Axis_y_mid += (v.Y * 40000 / (175.5 * Yrenderi));
                if (Axis_x_mid < -10000) Axis_x_mid = -10000;
                else if (Axis_x_mid > 10000) Axis_x_mid = 10000;
                if (Axis_y_mid < 0) Axis_y_mid = 0;
                else if (Axis_y_mid > 80000) Axis_y_mid = 80000;
                NeedFreshK();
            }
        }
        #endregion
        #region 激光界面按钮函数
        private void button_Clc_Click(object sender, RoutedEventArgs e)
        {
            DrawingBackup2d.Clear();
            AxisMod = 0;
            NeedFreshK();
        }//清屏
        private void button_Proof_Click(object sender, RoutedEventArgs e)//校准
        {
            if (OpenCOM(serialPort1))
            {
                Thread ProofThread = new Thread(new ThreadStart(() =>
                {
                    this.Dispatcher.BeginInvoke(new Action(() =>
                    {
                        _startshine();
                        Block1.Text = button_Proof.Content.ToString();
                        Block2.Text = "校准进行中......";
                    }));
                    CShareData.ReturnValueAdjust Result = LaserFunc.XYAdjustFunc(serialPort1, null, null);

                    this.Dispatcher.BeginInvoke(new Action(() =>
                    {
                        switch (Result)
                        {
                            case CShareData.ReturnValueAdjust.SUCCESS:
                                Block2.Text = "水平、垂直校准均成功" + Environment.NewLine + "  水平： " + CShareData.XAdjustStepError.ToString()
            + "  垂直： " + CShareData.YAdjustStepError.ToString();
                                break;
                            case CShareData.ReturnValueAdjust.SUCCESS_HRZ:
                                Block2.Text = "仅水平校准成功" + Environment.NewLine + "  水平： " + CShareData.XAdjustStepError.ToString();
                                break;
                            case CShareData.ReturnValueAdjust.SUCCESS_VER:
                                Block2.Text = "仅垂直校准成功" + Environment.NewLine + "  垂直： " + CShareData.YAdjustStepError.ToString();
                                break;
                            case CShareData.ReturnValueAdjust.ERROR_HRZ_PROB:
                                Block2.Text = "3次水平校准均失败" + Environment.NewLine + "与确认边框概率差别较大";
                                break;
                            case CShareData.ReturnValueAdjust.ERROR_HRZ_SCAN:
                                Block2.Text = "3次水平校准均失败" + Environment.NewLine + "未扫描到边框";
                                break;
                            case CShareData.ReturnValueAdjust.ERROR_VER_PROB:
                                Block2.Text = "3次垂直校准均失败" + Environment.NewLine + "与确认边框概率差别较大";
                                break;
                            case CShareData.ReturnValueAdjust.ERROR_VER_SCAN:
                                Block2.Text = "3次垂直校准均失败" + Environment.NewLine + "未扫描到边框";
                                break;
                            default:
                                Block2.Text = "水平、垂直校准均失败";
                                break;
                        }
                        _stopshine();
                    }));
                }));
                ProofThread.Start();
            }
        }
        private void button_WalkSet_Click(object sender, RoutedEventArgs e)//走步设置
        {
            if (CloseCOM(serialPort1))
            {
                try
                {
                    WPF2struct();
                    if (fuckModel.SelectedIndex != -1)
                        needTransform1_2DataCs.LineNum4Confirm = fuckModel.SelectedValue.ToString();
                    if (drpModel.SelectedIndex != -1)
                        needTransform1_2DataCs.PlaneType4Confirm = drpModel.SelectedValue.ToString();
                    if (Stop_Y.Text != "")
                    {
                        needTransform1_2DataCs.guide_line_y = Convert.ToInt32(Stop_Y.Text);
                    }
                    transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
                    Thread.Sleep(50);
                    if (!backgroundWorker.IsBusy)
                    {
                        OnBnClickedWalkset_std(laser_class);
                        Block1.Text = "走步设置";
                        Block2.Text = "";
                        now_task_working = task_working.walk_set_1;
                        backgroundWorker.RunWorkerAsync();
                    }
                    else
                        Xceed.Wpf.Toolkit.MessageBox.Show("仍在执行另一项任务", "任务", MessageBoxButton.OK);
                }
                catch { }
            }
        }
        private void button_WalkTest_Click(object sender, RoutedEventArgs e)//走步测试
        {
            if (Stop_Y.Text == "0" || Stop_Y.Text == "")
            {
                Xceed.Wpf.Toolkit.MessageBox.Show("尚未标定或读取引导线信息！", "错误", MessageBoxButton.OK);
                return;
            }
            if (CloseCOM(serialPort1))
            {
                try
                {
                    WPF2struct();
                    if (fuckModel.SelectedIndex != -1)
                        needTransform1_2DataCs.LineNum4Confirm = fuckModel.SelectedValue.ToString();
                    if (drpModel.SelectedIndex != -1)
                        needTransform1_2DataCs.PlaneType4Confirm = drpModel.SelectedValue.ToString(); ;
                    if (Stop_Y.Text != "")
                    {
                        needTransform1_2DataCs.guide_line_y = Convert.ToInt32(Stop_Y.Text);
                    }
                    transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
                    Thread.Sleep(50);
                    if (!backgroundWorker.IsBusy)
                    {
                        OnBnClickedWalktest_std(laser_class);
                        Block1.Text = "走步测试";
                        Block2.Text = "";
                        now_task_working = task_working.walk_test_2;
                        backgroundWorker.RunWorkerAsync();
                    }
                    else
                        Xceed.Wpf.Toolkit.MessageBox.Show("仍在执行另一项任务", "任务", MessageBoxButton.OK);
                }
                catch { }
            }
        }
        private void button_WalkStop_Click(object sender, RoutedEventArgs e)//停止
        {
            //if (CloseCOM(serialPort1))
            {
                OnBnClickedBtnClear_std(laser_class);
            }

        }
        private void button_WalkConfirm_Click(object sender, RoutedEventArgs e)//确认（计算引导线）
        {
            if (CloseCOM(serialPort1))
            {
                if (!backgroundWorker.IsBusy)
                {
                    OnBnClickedButtonConfirmwalkset_std(laser_class);
                    now_task_working = task_working.walk_confirm_3;
                    backgroundWorker.RunWorkerAsync();
                }
                else
                    Xceed.Wpf.Toolkit.MessageBox.Show("仍在执行另一项任务", "任务", MessageBoxButton.OK);
            }
        }
        private void button_WalkDelete_Click(object sender, RoutedEventArgs e)//清空
        {
            if (CloseCOM(serialPort1))
            {
                if (!backgroundWorker.IsBusy)
                {
                    OnBnClickedButtonClearwalkset_std(laser_class);
                    now_task_working = task_working.walk_clear_4;
                    backgroundWorker.RunWorkerAsync();
                }
                else
                    Xceed.Wpf.Toolkit.MessageBox.Show("仍在执行另一项任务", "任务", MessageBoxButton.OK);
            }
        }
        private void button_WalkNum_Click(object sender, RoutedEventArgs e)//编号【需要弹窗】
        {
            //if (CloseCOM(serialPort1))
            {
                SetTabControl.SelectedIndex = 3;
                Tab4.Visibility = Visibility.Visible;
                Tab1.Visibility = Visibility.Collapsed;
                Tab2.Visibility = Visibility.Collapsed;
                Tab3.Visibility = Visibility.Collapsed;
                if (needTransform1_2DataCs.str_guidenum != null)
                {
                    drpGuide.SelectedValue = needTransform1_2DataCs.str_guidenum;
                }
                if (needTransform1_2DataCs.str_stopnum != null)
                {
                    drpStop.SelectedValue = needTransform1_2DataCs.str_stopnum;
                }
                transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
            }
        }
        private void button_ModelGuide_Click(object sender, RoutedEventArgs e)//由机型读取引导线
        {
            //if (CloseCOM(serialPort1))
            {
                
                WPF2struct();
                if (fuckModel.SelectedIndex != -1)
                    needTransform1_2DataCs.LineNum4Confirm = fuckModel.SelectedValue.ToString();
                if (drpModel.SelectedIndex != -1)
                    needTransform1_2DataCs.PlaneType4Confirm = drpModel.SelectedValue.ToString();
                else
                {
                    Xceed.Wpf.Toolkit.MessageBox.Show("请填写飞机型号!", "错误", MessageBoxButton.OK);
                    return;
                }
                if (Stop_Y.Text != "")
                {
                    needTransform1_2DataCs.guide_line_y = Convert.ToInt32(Stop_Y.Text);
                }
                transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
                Thread.Sleep(50);
                DrawingBackup2d.Clear();
                if (!backgroundWorker.IsBusy)
                {
                    OnBnClickedButtonReadtype_std(laser_class);
                    now_task_working = task_working.model_guideline_5;
                    backgroundWorker.RunWorkerAsync();
                }
                else
                    Xceed.Wpf.Toolkit.MessageBox.Show("仍在执行另一项任务", "任务", MessageBoxButton.OK);
                
                //FreshUI();


                Parameter.glb_Parameter = new Parameter(@"test.xml");
                List<Parameter.StopLine> stoplines = Parameter.glb_Parameter.ReadStopLines();
                string stopLineNumber = "";
                foreach (var stopline in stoplines)
                {
                    foreach (var planetype in stopline.planeTypes)
                    {
                        if (planetype == drpModel.SelectedValue.ToString())
                            stopLineNumber = stopline.Name;
                    }
                }
                //if (stopLineNumber != "")
                //{
                //    System.Xml.XmlNode LaserData = Parameter.glb_Parameter.ReadLaserData(
                //    Number_GuideLine.Text[0], Number_GuideLine.Text[1]);
                //    CShareData.XProbSave = Convert.ToDouble(LaserData["HrzConfirmProbability"].InnerText);
                //    CShareData.YProbSave = Convert.ToDouble(LaserData["VertConfirmProbability"].InnerText);
                //}
            }
        }
        private void button_NumberGuide_Click(object sender, RoutedEventArgs e)//由编号读取引导线
        {
            //if (CloseCOM(serialPort1))
            {
                WPF2struct();
                if (fuckModel.SelectedIndex != -1)
                    needTransform1_2DataCs.LineNum4Confirm = fuckModel.SelectedValue.ToString();
                else
                {
                    Xceed.Wpf.Toolkit.MessageBox.Show("请填写引导线停止线编号!", "错误", MessageBoxButton.OK);
                    return;
                }
                if (drpModel.SelectedIndex != -1)
                    needTransform1_2DataCs.PlaneType4Confirm = drpModel.SelectedValue.ToString();
                if (Stop_Y.Text != "")
                    needTransform1_2DataCs.guide_line_y = Convert.ToInt32(Stop_Y.Text);
                transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
                Thread.Sleep(50);
                DrawingBackup2d.Clear();
                if (!backgroundWorker.IsBusy)
                {
                    OnBnClickedButtonReadlinenum_std(laser_class);
                    now_task_working = task_working.num_guideline_6;
                    backgroundWorker.RunWorkerAsync();
                }
                else
                    Xceed.Wpf.Toolkit.MessageBox.Show("仍在执行另一项任务", "任务", MessageBoxButton.OK);
                

                Parameter.glb_Parameter = new Parameter(@"test.xml");
                //System.Xml.XmlNode LaserData = Parameter.glb_Parameter.ReadLaserData(
                //    Number_GuideLine.Text[0], Number_GuideLine.Text[1]);
                //CShareData.XProbSave = Convert.ToDouble(LaserData["HrzConfirmProbability"].InnerText);
                //CShareData.YProbSave = Convert.ToDouble(LaserData["VertConfirmProbability"].InnerText);

                FreshUI();
            }
        }
        private void button_TestConfirmPoint_Click(object sender, RoutedEventArgs e)//验证停止点
        {
            if (CloseCOM(serialPort1))
            {
                try
                {
                    WPF2struct();
                    if (fuckModel.SelectedIndex != -1)
                        needTransform1_2DataCs.LineNum4Confirm = fuckModel.SelectedValue.ToString();
                    if (drpModel.SelectedIndex != -1)
                        needTransform1_2DataCs.PlaneType4Confirm = drpModel.SelectedValue.ToString(); ;
                    if (Stop_Y.Text != "")
                        needTransform1_2DataCs.guide_line_y = Convert.ToInt32(Stop_Y.Text);
                    transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
                    Thread.Sleep(50);
                    if (!backgroundWorker.IsBusy)
                    {
                        OnBnClickedButtonComfirmtype_std(laser_class);
                        now_task_working = task_working.confirm_spoint_7;
                        backgroundWorker.RunWorkerAsync();
                    }
                    else
                        Xceed.Wpf.Toolkit.MessageBox.Show("仍在执行另一项任务", "任务", MessageBoxButton.OK);
                    FreshUI();
                }
                catch { }
            }
        }
        private void button_ReadRect_Click(object sender, RoutedEventArgs e)//读取边框
        {
            ReadLaserPatameter();
            Block1.Text = "读取边框成功";
            Block2.Text = "水平步数：" + CShareData.XEdgeStepSave + Environment.NewLine + "垂直步数：" + CShareData.YEdgeStepSave
                 + Environment.NewLine + "Q12步数：" + CShareData.Q12Steps;
        }
        private void button_PointMeasure_Click(object sender, RoutedEventArgs e)//单点测量
        {
            if (OpenCOM(serialPort1))
            {
                //mid_num.Content = "0";
                double DistanceAVG = LaserFunc.ViewOnePointMeasureFunc(200, serialPort1, null, null);
                Thread.Sleep(10);
                if (DistanceAVG > 4)
                {
                    Block1.Text = "执行成功！";
                    this.Dispatcher.BeginInvoke(new Action(() =>
                    {
                        Block2.Text = "距离：" + Math.Round(DistanceAVG).ToString() + "mm"
                            + Environment.NewLine + "窗内有效数据：" + (CShareData.ViewOnePointDistanceShortProb * 100).ToString() + "%"
                            + Environment.NewLine + "窗外有效数据：" + (CShareData.ViewOnePointDistanceLongProb * 100).ToString() + "%";
                    }));
                    //mid_num.Content = Math.Round(DistanceAVG).ToString();
                    button_PointMeasure.Content = "单点测量" + Math.Round(DistanceAVG).ToString() + "mm";
                    button_PointMeasure.FontSize = 18;
                }
                else if (DistanceAVG == 4)
                {
                    Block1.Text = "执行成功！";
                    this.Dispatcher.BeginInvoke(new Action(() =>
                    {
                        Block2.Text = "窗内有效数据：" + (CShareData.ViewOnePointDistanceShortProb * 100).ToString() + "%"
                            + Environment.NewLine + "窗外有效数据：" + (CShareData.ViewOnePointDistanceLongProb * 100).ToString() + "%"
                            + Environment.NewLine + "有效数据不足!";
                    }));
                    button_PointMeasure.Content = "单点测量";
                }
                else
                {
                    Block1.Text = "单点测量执行出错！";
                    this.Dispatcher.BeginInvoke(new Action(() =>
                    {
                        Block2.Text = "错误信息：" + CShareData.StatusLableStr;
                    }));
                    button_PointMeasure.Content = "单点测量";
                }

                if (CShareData.ViewOnePointDistanceFlag)    //保存数据到Txt中
                {
                    string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                    string path = CShareData.StartPath + "单点" + TimeNow + ".txt";
                    LaserFunc.SaveOnePointDistance(path, CShareData.ViewOnePointDistance, CShareData.ViewOnePointDistanceLen);
                }
            }
        }
        private void button_OpenRed_Click(object sender, RoutedEventArgs e)
        {
            if (OpenCOM(serialPort1))
            {
                if (LaserFunc.RedLaser(CShareData.RedLaserValue.ON, serialPort1, null, null))
                {
                    ;
                }
            }
        }//打开红色激光
        private void button_CloseRed_Click(object sender, RoutedEventArgs e)
        {
            if (OpenCOM(serialPort1))
            {
                if (LaserFunc.RedLaser(CShareData.RedLaserValue.OFF, serialPort1, null, null))
                {
                    ;
                }
            }
        }//关闭红色激光
        private void button_WindowScan_Click(object sender, RoutedEventArgs e)//窗口扫描
        {
            if (CloseCOM(serialPort1))
            {
                WPF2struct();
                if (fuckModel.SelectedIndex != -1)
                    needTransform1_2DataCs.LineNum4Confirm = fuckModel.SelectedValue.ToString();
                if (drpModel.SelectedIndex != -1)
                    needTransform1_2DataCs.PlaneType4Confirm = drpModel.SelectedValue.ToString(); ;
                if (Stop_Y.Text != "")
                {
                    needTransform1_2DataCs.guide_line_y = Convert.ToInt32(Stop_Y.Text);
                }
                transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
                Thread.Sleep(50);
                if (!backgroundWorker.IsBusy)
                {
                    OnBnClickedButtonScanall_std(laser_class);
                    Block1.Text = "窗口扫描";
                    now_task_working = task_working.window_scan_8;
                    backgroundWorker.RunWorkerAsync();
                }
                else
                    Xceed.Wpf.Toolkit.MessageBox.Show("仍在执行另一项任务", "Task", MessageBoxButton.OK);
            }
        }
        private void button_ConfirmRect_Click(object sender, RoutedEventArgs e)//确定边框
        {
            if (OpenCOM(serialPort1))
            {
                Block1.Text = ForConfirmRect31.Text;
                Block2.Text = "";
                _startshine();
                Thread confirmRectThread = new Thread(new ThreadStart(() =>
                {
                    CShareData.ReturnValueEdge Result = LaserFunc.XYConfirmEdgeFunc(serialPort1, null, null);
                    if (Result == CShareData.ReturnValueEdge.SUCCESS)
                    {
                        Thread.Sleep(500);
                        LaserFunc.RoughReturnZeroSteps(serialPort1, null, null);
                    }
                    this.Dispatcher.BeginInvoke(new Action(() =>
                    {
                        edit_StandHigh.Text = "0";
                        LinearGradientBrush brush = new LinearGradientBrush();
                        switch (Result)
                        {
                            case CShareData.ReturnValueEdge.SUCCESS:
                                Block2.Text = "水平、垂直确认边框均成功" + Environment.NewLine
                                    + "  水平： " + CShareData.XEdgeStepSave.ToString()
                                    + "  垂直： " + CShareData.YEdgeStepSave.ToString()
                                    + "  Q12：" + CShareData.Q12Steps.ToString() + Environment.NewLine
                                    + "请点击保存按钮将数据保存至配置文件。";

                                brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                                brush.GradientStops.Add(new GradientStop(color: Colors.Green, offset: 1));
                                Checkbox_ConfirmRect.Fill = brush;
                                break;
                            case CShareData.ReturnValueEdge.SUCCESS_HRZ:
                                Block2.Text = "仅水平确认边框成功" + "  水平： " + CShareData.XEdgeStepSave.ToString();
                                brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                                brush.GradientStops.Add(new GradientStop(color: Colors.Red, offset: 1));
                                Checkbox_ConfirmRect.Fill = brush;
                                break;
                            case CShareData.ReturnValueEdge.SUCCESS_VER:
                                Block2.Text = "仅垂直确认边框成功" + "  垂直： " + CShareData.YEdgeStepSave.ToString();
                                brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                                brush.GradientStops.Add(new GradientStop(color: Colors.Red, offset: 1));
                                Checkbox_ConfirmRect.Fill = brush;
                                break;
                            case CShareData.ReturnValueEdge.ERROR_HRZ_PROB:
                                Block2.Text = "3次水平确认边框均失败" + Environment.NewLine + "概率不在0.2-0.8之间";
                                brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                                brush.GradientStops.Add(new GradientStop(color: Colors.Red, offset: 1));
                                Checkbox_ConfirmRect.Fill = brush;
                                break;
                            case CShareData.ReturnValueEdge.ERROR_HRZ_SCAN:
                                Block2.Text = "3次水平确认边框均失败" + Environment.NewLine + "未扫描到边框";
                                brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                                brush.GradientStops.Add(new GradientStop(color: Colors.Red, offset: 1));
                                Checkbox_ConfirmRect.Fill = brush;
                                break;
                            case CShareData.ReturnValueEdge.ERROR_HRZ_STEP:
                                Block2.Text = "3次水平确认边框均失败" + Environment.NewLine + "单步移动超时";
                                brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                                brush.GradientStops.Add(new GradientStop(color: Colors.Red, offset: 1));
                                Checkbox_ConfirmRect.Fill = brush;
                                break;
                            case CShareData.ReturnValueEdge.ERROR_HRZ_ZERO:
                                Block2.Text = "3次水平确认边框均失败" + Environment.NewLine + "零点不合理，导致找不到边框";
                                brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                                brush.GradientStops.Add(new GradientStop(color: Colors.Red, offset: 1));
                                Checkbox_ConfirmRect.Fill = brush;
                                break;
                            case CShareData.ReturnValueEdge.ERROR_VER_PROB:
                                Block2.Text = "3次垂直确认边框均失败" + Environment.NewLine + "概率不在0.2-0.8之间";
                                brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                                brush.GradientStops.Add(new GradientStop(color: Colors.Red, offset: 1));
                                Checkbox_ConfirmRect.Fill = brush;
                                break;
                            case CShareData.ReturnValueEdge.ERROR_VER_SCAN:
                                Block2.Text = "3次垂直确认边框均失败" + Environment.NewLine + "未扫描到边框";
                                brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                                brush.GradientStops.Add(new GradientStop(color: Colors.Red, offset: 1));
                                Checkbox_ConfirmRect.Fill = brush;
                                break;
                            case CShareData.ReturnValueEdge.ERROR_VER_STEP:
                                Block2.Text = "3次垂直确认边框均失败" + Environment.NewLine + "单步移动超时";
                                brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                                brush.GradientStops.Add(new GradientStop(color: Colors.Red, offset: 1));
                                Checkbox_ConfirmRect.Fill = brush;
                                break;
                            case CShareData.ReturnValueEdge.ERROR_VER_ZERO:
                                Block2.Text = "3次垂直确认边框均失败" + Environment.NewLine + "零点不合理，导致找不到边框";
                                brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                                brush.GradientStops.Add(new GradientStop(color: Colors.Red, offset: 1));
                                Checkbox_ConfirmRect.Fill = brush;
                                break;
                            default:
                                Block2.Text = "水平、垂直确认边框均失败";
                                brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                                brush.GradientStops.Add(new GradientStop(color: Colors.Red, offset: 1));
                                Checkbox_ConfirmRect.Fill = brush;
                                break;
                        }
                        _stopshine();
                    }), null);
                }));
                confirmRectThread.Start();
            }
            try
            {
                WriteConfigData();  //保存配置文件
            }
            catch
            {
            }
        }
        private void button_Set0_Click(object sender, RoutedEventArgs e)
        {
            MessageBoxResult result = Xceed.Wpf.Toolkit.MessageBox.Show("选择是，设置新的零点" + Environment.NewLine
                + "选择否，取消操作", button_Set0.Content.ToString(), MessageBoxButton.YesNo);//显示确认窗口
            if (result == MessageBoxResult.Yes)
            {
                if (OpenCOM(serialPort1))
                {
                    Block1.Text = button_Set0.Content.ToString();
                    Block2.Text = "";
                    if (LaserFunc.SetZeroFunc(serialPort1, null, null))
                    {
                        Block2.Text = "执行成功！";
                        edit_StandHigh.Text = (0).ToString();
                        WPF2struct();
                    }
                    else
                    {
                        Block2.Text = "执行失败！";
                    }
                }
            }
            else
            {
                ;//取消操作
            }
        }//置为零点
        private void button_Back0_Click(object sender, RoutedEventArgs e)
        {
            if (OpenCOM(serialPort1))
            {
                Block1.Text = ForBack0_dis27.Text;
                Block2.Text = "";
                if (LaserFunc.ReturnZeroFunc(serialPort1, null, null))
                {
                    Block2.Text = "执行成功！";
                    edit_StandHigh.Text = "0";
                    WPF2struct();
                }
                else
                {
                    Block2.Text = "执行失败！";
                }
            }
        }//回到零点
        private void button_2dScan_Click(object sender, RoutedEventArgs e)//二维扫描
        {
            DrawingBackup2d.Clear();
            if (edit_StandHigh.Text != "")
                CShareData.YShift = Convert.ToInt32(edit_StandHigh.Text) / 2;
            
            int ForwardOffset = Convert.ToInt32(Parameter.glb_Parameter.laser_machine_datas_list["LaserForwardOffset"].InnerText);
            int BackwardOffset = Convert.ToInt32(Parameter.glb_Parameter.laser_machine_datas_list["LaserBackwardOffset"].InnerText);
            if (OpenCOM(serialPort1))
            {
                Block1.Text = "二维扫描";
                Block2.Text = "";
                _startshine();
                Int16 ScanPointNum = 109;
                Int32[,] ReceivedDistance = new Int32[2, ScanPointNum];     //读取水平扫描距离值
                Int16 XStartPos = Convert.ToInt16(0 - ((ScanPointNum - 1) >> 2));
                Int16 XEndPos = Convert.ToInt16((ScanPointNum - 1) >> 2);
                System.Windows.Forms.Label statusLabel = new System.Windows.Forms.Label();
                statusLabel.Name = "Status";
                ReceivedDistance = LaserFunc.HrzScanFunc(Convert.ToInt16(CShareData.YShift), XStartPos, XEndPos, serialPort1, statusLabel, null);   //调用水平扫描函数

                if (CShareData.HrzScanFlag)//扫描成功
                {
                    Block2.Text = "执行成功！";
                    AxisMod = 1;
                    needTransform1_2DataCs.guide_line_x = 0;
                    Axis_x_mid = 0;
                    Axis_y_mid = 40000;
                    for (int temp3 = ForwardOffset; temp3 < ScanPointNum; temp3++)
                    {
                        nowShape2d.type = MouseStatus.DrawingPoint;
                        nowShape2d.color = Colors.Brown;
                        nowShape2d.radius = 2;
                        nowShape2d.point1.X = 32 + (temp3 - ForwardOffset) * 290 / ScanPointNum;
                        nowShape2d.point1.Y = 437 - (double)ReceivedDistance[0, temp3] * 35 / 8000;
                        DrawingBackup2d.Add(nowShape2d);
                        nowShape2d = new DrawShape();
                    }
                    NeedFreshK();
                    for (int temp3 = 0; temp3 < ScanPointNum - BackwardOffset; temp3++)
                    {
                        nowShape2d.type = MouseStatus.DrawingPoint;
                        nowShape2d.color = Colors.Blue;
                        nowShape2d.radius = 1;
                        nowShape2d.point1.X = 32 + (temp3 + BackwardOffset) * 290 / ScanPointNum;
                        nowShape2d.point1.Y = 437 - (double)ReceivedDistance[1, temp3] * 35 / 8000;
                        DrawingBackup2d.Add(nowShape2d);
                        nowShape2d = new DrawShape();
                    }
                    NeedFreshK();
                }
                else
                    Block2.Text = "执行失败！" + Environment.NewLine + statusLabel.Text;
                _stopshine();
                string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                string path = CShareData.StartPath + "二维" + TimeNow + ".txt";
                LaserFunc.SaveScanDistance(path, ReceivedDistance, ScanPointNum, 2);
            }
        }
        private void button_AutoCalibration_Click(object sender, RoutedEventArgs e)//水平标定
        {
            if (OpenCOM(serialPort1))
            {
                Block1.Text = ForAutoCalibration29.Text;
                Block2.Text = "执行中.......";
                _startshine();
                Thread confirmRectThread = new Thread(new ThreadStart(() =>
                {
                    CShareData.ReturnValueCalibHrz Result = LaserFunc.HrzCalibrationFunc(serialPort1, null, null);
                    this.Dispatcher.BeginInvoke(new Action(() =>
                    {
                        LinearGradientBrush brush = new LinearGradientBrush();
                        switch (Result)
                        {
                            case CShareData.ReturnValueCalibHrz.SUCCESS:
                                this.Dispatcher.BeginInvoke(new Action(() =>
                                {
                                    Block2.Text = "执行成功！" + Environment.NewLine +
                                        "XShift ：  " + CShareData.XShift.ToString() + Environment.NewLine +
                                            "YShift ：  " + CShareData.YShift.ToString();
                                }));
                                brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                                brush.GradientStops.Add(new GradientStop(color: Colors.Green, offset: 1));
                                Checkbox_AutoCalibration.Fill = brush;
                                break;
                            case CShareData.ReturnValueCalibHrz.ERROR_LR_EDGE:
                                this.Dispatcher.BeginInvoke(new Action(() =>
                    {
                        Block2.Text = "水平标定失败" + Environment.NewLine +
                            "未找到左右边界";
                    }));
                                brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                                brush.GradientStops.Add(new GradientStop(color: Colors.Red, offset: 1));
                                Checkbox_AutoCalibration.Fill = brush;
                                break;
                            case CShareData.ReturnValueCalibHrz.ERROR_POSITION:
                                this.Dispatcher.BeginInvoke(new Action(() =>
                    {
                        Block2.Text = "水平标定失败" + Environment.NewLine +
                            "人的位置不合理";
                    }));
                                brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                                brush.GradientStops.Add(new GradientStop(color: Colors.Red, offset: 1));
                                Checkbox_AutoCalibration.Fill = brush;
                                break;
                            case CShareData.ReturnValueCalibHrz.ERROR_YSTEP:
                                this.Dispatcher.BeginInvoke(new Action(() =>
                    {
                        Block2.Text = "水平标定失败" + Environment.NewLine +
                            "垂直步数不合理";
                    }));
                                brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                                brush.GradientStops.Add(new GradientStop(color: Colors.Red, offset: 1));
                                Checkbox_AutoCalibration.Fill = brush;
                                break;
                            case CShareData.ReturnValueCalibHrz.ERROR_MEASURE_TIMEOUT:
                                this.Dispatcher.BeginInvoke(new Action(() =>
                    {
                        Block2.Text = "水平标定失败" + Environment.NewLine +
                            "定点测距超时";
                    }));
                                brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                                brush.GradientStops.Add(new GradientStop(color: Colors.Red, offset: 1));
                                Checkbox_AutoCalibration.Fill = brush;
                                break;
                            case CShareData.ReturnValueCalibHrz.FAILURE:
                                Block2.Text = "执行失败！";
                                brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                                brush.GradientStops.Add(new GradientStop(color: Colors.Red, offset: 1));
                                Checkbox_AutoCalibration.Fill = brush;
                                break;
                        }
                        _stopshine();
                    }), null);
                }));
                confirmRectThread.Start();
            }
        }
        private void button_VerticalCalibration_Click(object sender, RoutedEventArgs e)//垂直标定
        {
            Int16 ReadInstallHeight = Convert.ToInt16(Install_height.Text); //zzm 读安装高度
            if ((ReadInstallHeight > 666) && (ReadInstallHeight < 10000))   //zzm 有效值0.1m-10m
            {
                CShareData.InstallHeight = ReadInstallHeight;
            }
            if (OpenCOM(serialPort1))
            {
                Block1.Text = ForVerticalCalibration30.Text;
                Block2.Text = "执行中.......";
                _startshine();
                CShareData.ReturnValueCalibVer Result = LaserFunc.VerCalibrationFunc(serialPort1, null, null);
                LinearGradientBrush brush = new LinearGradientBrush();
                switch (Result)
                {
                    case CShareData.ReturnValueCalibVer.SUCCESS:
                        this.Dispatcher.BeginInvoke(new Action(() =>
                    {
                        Block2.Text = "执行成功！" + Environment.NewLine +
                            "XShift ：  " + CShareData.XShift.ToString() + Environment.NewLine +
                               "YShift ：  " + CShareData.YShift.ToString();
                    }));
                        brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                        brush.GradientStops.Add(new GradientStop(color: Colors.Green, offset: 1));
                        Checkbox_VerticalCalibration.Fill = brush;
                        break;
                    case CShareData.ReturnValueCalibVer.ERROR_STEP:
                        this.Dispatcher.BeginInvoke(new Action(() =>
                    {
                        Block2.Text = "垂直标定失败" + Environment.NewLine +
                            "计算出步数不合理";
                    }));
                        brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                        brush.GradientStops.Add(new GradientStop(color: Colors.Red, offset: 1));
                        Checkbox_VerticalCalibration.Fill = brush;
                        break;
                    case CShareData.ReturnValueCalibVer.ERROR_VALID_PROB:
                        this.Dispatcher.BeginInvoke(new Action(() =>
                    {
                        Block2.Text = "垂直标定失败" + Environment.NewLine +
                            "测量概率未满足有效值太小，请从其他位置再次执行垂直标定。";
                    }));
                        brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                        brush.GradientStops.Add(new GradientStop(color: Colors.Red, offset: 1));
                        Checkbox_VerticalCalibration.Fill = brush;
                        break;
                    case CShareData.ReturnValueCalibVer.ERROR_ANGLE:
                        this.Dispatcher.BeginInvoke(new Action(() =>
                    {
                        Block2.Text = "垂直标定失败" + Environment.NewLine +
                            "角度值不合法";
                    }));
                        brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                        brush.GradientStops.Add(new GradientStop(color: Colors.Red, offset: 1));
                        Checkbox_VerticalCalibration.Fill = brush;
                        break;
                    case CShareData.ReturnValueCalibVer.ERROR_MEASURE:
                        this.Dispatcher.BeginInvoke(new Action(() =>
                    {
                        Block2.Text = "垂直标定失败" + Environment.NewLine +
                            "定点测距错误";
                    }));
                        brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                        brush.GradientStops.Add(new GradientStop(color: Colors.Red, offset: 1));
                        Checkbox_VerticalCalibration.Fill = brush;
                        break;
                    case CShareData.ReturnValueCalibVer.FAILURE:
                        Block2.Text = "执行失败！";
                        brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                        brush.GradientStops.Add(new GradientStop(color: Colors.Red, offset: 1));
                        Checkbox_VerticalCalibration.Fill = brush;
                        break;
                }
                _stopshine();
            }
        }
        private void button_stationScan_Click(object sender, RoutedEventArgs e)//站坪扫描
        {
            if (CloseCOM(serialPort1))
            {
                WPF2struct();
                if (fuckModel.SelectedIndex != -1)
                    needTransform1_2DataCs.LineNum4Confirm = fuckModel.SelectedValue.ToString();
                if (drpModel.SelectedIndex != -1)
                    needTransform1_2DataCs.PlaneType4Confirm = drpModel.SelectedValue.ToString(); ;
                if (Stop_Y.Text != "")
                {
                    needTransform1_2DataCs.guide_line_y = Convert.ToInt32(Stop_Y.Text);
                }
                transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
                Thread.Sleep(50);
                if (!backgroundWorker.IsBusy)
                {
                    OnBnClickedButtonScanYard_std(laser_class);
                    Block1.Text = "站坪扫描";
                    Block2.Text = "执行中.......";
                    now_task_working = task_working.station_scan_9;
                    backgroundWorker.RunWorkerAsync();
                }
                else
                    Xceed.Wpf.Toolkit.MessageBox.Show("仍在执行另一项任务", "任务", MessageBoxButton.OK);
            }
        }
        private void button_stationScan_RightClick(object sender, RoutedEventArgs e)//站坪扫描
        {
            if (CloseCOM(serialPort1))
            {
                WPF2struct();
                if (fuckModel.SelectedIndex != -1)
                    needTransform1_2DataCs.LineNum4Confirm = fuckModel.SelectedValue.ToString();
                if (drpModel.SelectedIndex != -1)
                    needTransform1_2DataCs.PlaneType4Confirm = drpModel.SelectedValue.ToString(); ;
                if (Stop_Y.Text != "")
                {
                    needTransform1_2DataCs.guide_line_y = Convert.ToInt32(Stop_Y.Text);
                }
                transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
                Thread.Sleep(50);
                if (!backgroundWorker.IsBusy)
                {
                    OnBnClickedButtonScanYardToAdjust_std(laser_class);
                    Block1.Text = "站坪扫描后校准";
                    Block2.Text = "执行中.......";
                    now_task_working = task_working.station_scan_9;
                    backgroundWorker.RunWorkerAsync();
                }
                else
                    Xceed.Wpf.Toolkit.MessageBox.Show("仍在执行另一项任务", "任务", MessageBoxButton.OK);
            }
        }
        private void Image2dBox_ImageFailed(object sender, ExceptionRoutedEventArgs e)
        {

        }
        private void button_Save_Click(object sender, RoutedEventArgs e)//保存
        {
            Parameter.glb_Parameter = new Parameter(@"test.xml");
            System.Xml.XmlNode LaserDataOld = Parameter.glb_Parameter.laser_machine_datas_list;
            if (drpdistance.Text != "")
                Object_distance.Text = drpdistance.Text;
            //if (CloseCOM(serialPort1))
            {
                if (needTransform1_2DataCs.str_guidenum != 0 && needTransform1_2DataCs.str_stopnum != 0)
                {
                    WPF2struct();
                    transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
                    OnBnClickedSave_std(laser_class);
                }
            }
            Thread.Sleep(50);

            try
            {
                Parameter new_laser_doc = new Parameter(@"test.xml");
                Parameter.glb_Parameter.Doc["opencv_storage"]["laser_datas_list"].InnerXml = new_laser_doc.Doc["opencv_storage"]["laser_datas_list"].InnerXml;
                System.Xml.XmlNode LaserMachineData = Parameter.glb_Parameter.laser_machine_datas_list;
                if (Convert.ToInt32(Convert.ToSingle(LaserMachineData["HrzConfirmSteps"].InnerText)) == 0 || Convert.ToInt32(Convert.ToSingle(LaserMachineData["VertConfirmSteps"].InnerText)) == 0)
                {
                    LaserMachineData["HrzConfirmSteps"].InnerText = LaserDataOld["HrzConfirmSteps"].InnerText;
                    LaserMachineData["VertConfirmSteps"].InnerText = LaserDataOld["VertConfirmSteps"].InnerText;
                    LaserMachineData["Q12Steps"].InnerText = LaserDataOld["Q12Steps"].InnerText;
                }
                else
                {
                    LaserMachineData["HrzConfirmSteps"].InnerText = CShareData.XEdgeStepSave.ToString();
                    LaserMachineData["VertConfirmSteps"].InnerText = CShareData.YEdgeStepSave.ToString();
                    LaserMachineData["Q12Steps"].InnerText = CShareData.Q12Steps.ToString();
                }
                LaserMachineData["install_high"].InnerText = Install_height.Text;
                LaserMachineData["HrzConfirmProbability"].InnerText = CShareData.XProbSave.ToString();
                LaserMachineData["VertConfirmProbability"].InnerText = CShareData.YProbSave.ToString();
                Parameter.glb_Parameter.Save();
                System.Windows.MessageBox.Show("引导信息保存成功！", "提示");
            }
            catch (System.Exception ex)
            {
                System.Windows.MessageBox.Show("引导信息保存失败！", "错误");
            }
            UpdatestoplineFromLaser();
        }
        private void button_Esc_Click(object sender, RoutedEventArgs e)//退出【修改】不用DestoryLaser了
        {
            if (CloseCOM(serialPort1))
            {
                OnBnClickedButtonRedlaseroff_std(laser_class);
                if (TimerCon != null) TimerCon(this, new TimerEventArgs());
                this.Close();
            }
        }
        private void button_RoughlyBack0_Click(object sender, RoutedEventArgs e)
        {
            if (OpenCOM(serialPort1))
            {
                Block1.Text = "粗略回零";
                Block2.Text = "";
                _startshine();
                Thread confirmRectThread = new Thread(new ThreadStart(() =>
                {
                    //bool bbq = LaserFunc.YQ12DetectionFunc(serialPort1, null, null);
                    bool bbq = LaserFunc.XYRoughReturnZeroFunc(serialPort1, null, null);
                    this.Dispatcher.BeginInvoke(new Action(() =>
                    {
                        if (bbq)
                        {
                            Block2.Text = "执行成功！";
                        }
                        else
                        {
                            Block2.Text = "执行失败！";
                            
                        }
                        _stopshine();
                    }), null);
                }));
                confirmRectThread.Start();
            }
        }//粗略回0
        //private void button_GuideLineMove_Click(object sender, RoutedEventArgs e)
        //{
        //    MessageBoxResult result = Xceed.Wpf.Toolkit.MessageBox.Show("确认移动引导线？", "取消", MessageBoxButton.YesNo);//显示确认窗口
        //    if (result == MessageBoxResult.Yes)
        //    {
        //        if (CloseCOM(serialPort1))
        //        {
        //            needTransform1_2DataCs.offsetforline = Convert.ToInt32(TextBox_GuideLineMove.Text);
        //            TextBox_GuideLineMove.Text = "0";
        //            transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
        //            Thread.Sleep(50);
        //            OnBnClickedButtonGuideLineMove_std(laser_class);
        //            needTransform1_2DataCs.guide_line_x += Convert.ToInt32(needTransform1_2DataCs.offsetforline);
        //            needTransform1_2DataCs.guide_line_b = needTransform1_2DataCs.guide_line_y - needTransform1_2DataCs.guide_line_x * needTransform1_2DataCs.guide_line_k;
        //            Xrenderi = 1;
        //            Yrenderi = 1;
        //            Axis_x_mid = needTransform1_2DataCs.guide_line_x;
        //            Axis_y_mid = 40000;
        //            NeedFreshK();
        //        }
        //    }
        //    else if (TextBox_GuideLineMove.Text != "")
        //    {
        //        //无操作
        //    }
        //}//引导线移动
        //private void button_StopLineMove_Click(object sender, RoutedEventArgs e)
        //{
        //    MessageBoxResult result = Xceed.Wpf.Toolkit.MessageBox.Show("确认移动停止线？", "取消", MessageBoxButton.YesNo);//显示确认窗口
        //    if (result == MessageBoxResult.Yes)
        //    {
        //        if (CloseCOM(serialPort1))
        //        {
        //            needTransform1_2DataCs.offsetforline = Convert.ToInt32(TextBox_StopLineMove.Text);
        //            TextBox_StopLineMove.Text = "0";
        //            transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
        //            Thread.Sleep(50);
        //            OnBnClickedButtonStopLineMove_std(laser_class);
        //            needTransform1_2DataCs.guide_line_y += Convert.ToInt32(Convert.ToInt32(needTransform1_2DataCs.offsetforline) *
        //                needTransform1_2DataCs.guide_line_k / Math.Sqrt(1 + Math.Pow(needTransform1_2DataCs.guide_line_k, 2)));
        //            needTransform1_2DataCs.guide_line_x += Convert.ToInt32(Convert.ToInt32(needTransform1_2DataCs.offsetforline) /
        //                Math.Sqrt(1 + Math.Pow(needTransform1_2DataCs.guide_line_k, 2)));
        //            Xrenderi = 1;
        //            Yrenderi = 1;
        //            Axis_x_mid = needTransform1_2DataCs.guide_line_x;
        //            Axis_y_mid = 40000;
        //            foreach (var shape in DrawingBackup2d)
        //            {
        //                if (shape.point1.X == 0 && shape.point1.Y == 0) continue;
        //                switch (shape.type)
        //                {
        //                    case MouseStatus.DrawingLine:
        //                        if (shape.point1.Y != shape.point2.Y)
        //                        {
        //                            shape.point1.Y = 437 - (double)needTransform1_2DataCs.guide_line_y * 35 / 8000;
        //                        }
        //                        else
        //                        {
        //                            shape.point1.Y = 437 - (double)needTransform1_2DataCs.guide_line_y * 35 / 8000;
        //                            shape.point2.Y = 437 - (double)needTransform1_2DataCs.guide_line_y * 35 / 8000;
        //                        }
        //                        break;
        //                    default:
        //                        break;
        //                }
        //            }
        //            NeedFreshK();
        //        }
        //    }
        //    else if (TextBox_StopLineMove.Text != "")
        //    {
        //        //无操作
        //    }
        //} //停止线移动
        private void button_OK_Click(object sender, RoutedEventArgs e)
        {
            if (this.drpGuide.SelectedIndex != -1 && this.drpStop.SelectedIndex != -1)
            {
                needTransform1_2DataCs.str_guidenum = (char)drpGuide.SelectedValue;
                needTransform1_2DataCs.str_stopnum = (char)drpStop.SelectedValue;
            }
            SetTabControl.SelectedIndex = 1;
            Tab4.Visibility = Visibility.Collapsed;
            Tab1.Visibility = Visibility.Visible;
            Tab2.Visibility = Visibility.Visible;
            Tab3.Visibility = Visibility.Visible;
        }//编号确认
        private void button_Close_Click(object sender, RoutedEventArgs e)
        {
            SetTabControl.SelectedIndex = 1;
            Tab4.Visibility = Visibility.Collapsed;
            Tab1.Visibility = Visibility.Visible;
            Tab2.Visibility = Visibility.Visible;
            Tab3.Visibility = Visibility.Visible;
        }//取消编号
        private void Click_appbar_arrow_down(object sender, RoutedEventArgs e)
        {
            Int16 MoveStep = Convert.ToInt16(XNumericUpDown.Value);
            if (OpenCOM(serialPort1))
            {
                _startshine();
                if (LaserFunc.MoveFunc(CShareData.MoveDirection.Down, MoveStep, CShareData.SetZeroValue.UnSet, serialPort1, null, null))
                {
                    edit_StandHigh.Text = (2 * CShareData.YShift).ToString();
                }
                _stopshine();
            }
        }//下移
        private void Click_appbar_arrow_Up(object sender, RoutedEventArgs e)
        {
            Int16 MoveStep = Convert.ToInt16(XNumericUpDown.Value);
            if (OpenCOM(serialPort1))
            {
                _startshine();
                if (LaserFunc.MoveFunc(CShareData.MoveDirection.Up, MoveStep, CShareData.SetZeroValue.UnSet, serialPort1, null, null))
                {
                    edit_StandHigh.Text = (2 * CShareData.YShift).ToString();
                }
                _stopshine();
            }
        }//上移
        private void Click_appbar_arrow_left(object sender, RoutedEventArgs e)
        {
            Int16 MoveStep = Convert.ToInt16(XNumericUpDown.Value);
            if (OpenCOM(serialPort1))
            {
                _startshine();
                if (LaserFunc.MoveFunc(CShareData.MoveDirection.Left, MoveStep, CShareData.SetZeroValue.UnSet, serialPort1, null, null))
                {
                    ;
                }
                _stopshine();
            }
        }//左移
        private void Click_appbar_arrow_right(object sender, RoutedEventArgs e)
        {
            Int16 MoveStep = Convert.ToInt16(XNumericUpDown.Value);
            if (OpenCOM(serialPort1))
            {
                _startshine();
                if (LaserFunc.MoveFunc(CShareData.MoveDirection.Right, MoveStep, CShareData.SetZeroValue.UnSet, serialPort1, null, null))
                {
                    ;
                }
                _stopshine();
            }
        }//右移
        #endregion
        #region 刷新机制
        void Axis2dScan(double origin_y)//2维扫描模式
        {
            DrawingVisual drawingVisual = new DrawingVisual();
            var rtbitmap = new RenderTargetBitmap(writeableSource2d.PixelWidth,
    writeableSource2d.PixelHeight, writeableSource2d.DpiX, writeableSource2d.DpiY,
    PixelFormats.Default);
            int temp5;
            FormattedText text1;

            double Xtemp = 177 - (Axis_x_mid * Xrenderi * 145 / 10000);
            using (var dc = drawingVisual.RenderOpen())
            {
                dc.DrawImage(writeableSource2d, new Rect(0, 0, writeableSource2d.Width, writeableSource2d.Height));
                //dc.DrawRectangle(Brushes.Transparent, new Pen(Brushes.Black, 7), new Rect(10, 10, 50 + text.Width, 50 + text.Height));
                for (int i = 0; i < 9; i++)
                {
                    temp5 = (int)(((40000 - 10000 * i) / Yrenderi) + origin_y);
                    text1 = new FormattedText(temp5.ToString(),
                    System.Globalization.CultureInfo.CurrentCulture,
                    System.Windows.FlowDirection.LeftToRight,
                    new Typeface(SystemFonts.MessageFontFamily,
                        FontStyles.Normal, FontWeights.Thin,
                        FontStretches.Normal), 10, Brushes.Black);
                    dc.DrawText(text1, new Point(0, 80 + 44 * i));
                }
                dc.DrawLine(new Pen(Brushes.DarkOrange, 2), new Point(Xtemp, 435), new Point(Xtemp, 84));
            }
            rtbitmap.Render(drawingVisual);
            Image2dBox.Source = rtbitmap;
        }
        void AxisNormal(double origin_x, double origin_y)//画两个坐标轴的刻度
        {
            DrawingVisual drawingVisual = new DrawingVisual();
            var rtbitmap = new RenderTargetBitmap(writeableSource2d.PixelWidth,
    writeableSource2d.PixelHeight, writeableSource2d.DpiX, writeableSource2d.DpiY,
    PixelFormats.Default);
            int temp5;
            FormattedText text1;
            int temp4;
            FormattedText text11;

            FormattedText text21 = new FormattedText("Zero point",
    System.Globalization.CultureInfo.CurrentCulture,
    System.Windows.FlowDirection.LeftToRight,
    new Typeface(SystemFonts.MessageFontFamily,
        FontStyles.Normal, FontWeights.Thin,
        FontStretches.Normal), 10, Brushes.Black);
            double Xtemp = 177 - (Axis_x_mid * Xrenderi * 145 / 10000);
            int Ytemp = 262 + (int)(Axis_y_mid * Yrenderi * 176 / 40000);
            using (var dc = drawingVisual.RenderOpen())
            {
                dc.DrawImage(writeableSource2d, new Rect(0, 0, writeableSource2d.Width, writeableSource2d.Height));
                //dc.DrawRectangle(Brushes.Transparent, new Pen(Brushes.Black, 7), new Rect(10, 10, 50 + text.Width, 50 + text.Height));
                for (int i = 0; i < 8; i++)
                {
                    temp5 = (int)(((40000 - 10000 * i) / Yrenderi) + origin_y);
                    temp4 = (int)(((64 + 32 * i - 177) * 10000 / (145 * Xrenderi)) + origin_x);
                    text1 = new FormattedText(temp5.ToString(),
                        System.Globalization.CultureInfo.CurrentCulture,
                        System.Windows.FlowDirection.LeftToRight,
                        new Typeface(SystemFonts.MessageFontFamily,
                            FontStyles.Normal, FontWeights.Thin,
                            FontStretches.Normal), 10, Brushes.Black);
                    text11 = new FormattedText(temp4.ToString(),
                        System.Globalization.CultureInfo.CurrentCulture,
                        System.Windows.FlowDirection.LeftToRight,
                        new Typeface(SystemFonts.MessageFontFamily,
                            FontStyles.Normal, FontWeights.Thin,
                            FontStretches.Normal), 10, Brushes.Black);
                    dc.DrawText(text1, new Point(0, 80 + 44 * i));
                    dc.DrawText(text11, new Point(47 + 33.2 * i, 440));
                }
                temp5 = (int)((-40000 / Yrenderi) + origin_y);
                text1 = new FormattedText(temp5.ToString(),
                    System.Globalization.CultureInfo.CurrentCulture,
                    System.Windows.FlowDirection.LeftToRight,
                    new Typeface(SystemFonts.MessageFontFamily,
                        FontStyles.Normal, FontWeights.Thin,
                        FontStretches.Normal), 10, Brushes.Black);
                dc.DrawText(text1, new Point(2, 435));

                dc.DrawText(text21, new Point(Xtemp - 7, Ytemp + 12));
                dc.DrawLine(new Pen(Brushes.DarkOrange, 2), new Point(Xtemp - 4, Ytemp - 4), new Point(Xtemp + 4, Ytemp + 4));
                dc.DrawLine(new Pen(Brushes.DarkOrange, 2), new Point(Xtemp - 4, Ytemp + 4), new Point(Xtemp + 4, Ytemp - 4));
            }
            rtbitmap.Render(drawingVisual);
            Image2dBox.Source = rtbitmap;
        }
        void AxisHrzcalibration()//水平标定模式
        {

        }
        #endregion
        #region 按键输入控制
        private void Install_height_KeyDown(object sender, KeyEventArgs e)
        {
            TextBox txt = sender as TextBox;

            //屏蔽非法按键
            if (e.Key >= Key.NumPad0 && e.Key <= Key.NumPad9)
            {
                if (txt.Text.Contains(".") && e.Key == Key.Decimal)
                {
                    e.Handled = true;
                    return;
                }
                e.Handled = false;
            }
            else if ((e.Key >= Key.D0 && e.Key <= Key.D9) && e.KeyboardDevice.Modifiers != ModifierKeys.Shift)
            {
                if (txt.Text.Contains(".") && e.Key == Key.OemPeriod)
                {
                    e.Handled = true;
                    return;
                }
                e.Handled = false;
            }
            else
            {
                e.Handled = true;
            }
        }
        private void Install_height_TextChanged(object sender, TextChangedEventArgs e)
        {
            //屏蔽中文输入和非法字符粘贴输入
            TextBox textBox = sender as TextBox;
            TextChange[] change = new TextChange[e.Changes.Count];
            e.Changes.CopyTo(change, 0);

            int offset = change[0].Offset;
            if (change[0].AddedLength > 0)
            {
                double num = 0;
                if (!Double.TryParse(textBox.Text, out num))
                {
                    textBox.Text = textBox.Text.Remove(offset, change[0].AddedLength);
                    textBox.Select(offset, 0);
                }
            }
        }
        private void Move_KeyDown(object sender, KeyEventArgs e)
        {
            TextBox txt = sender as TextBox;

            //屏蔽非法按键
            if ((e.Key >= Key.NumPad0 && e.Key <= Key.NumPad9) || e.Key == Key.ImeProcessed || e.Key == Key.OemMinus || e.Key == Key.Subtract)
            {
                if (txt.Text.Contains(".") && e.Key == Key.Decimal)
                {
                    e.Handled = true;
                    return;
                }
                e.Handled = false;
            }
            else if ((e.Key >= Key.D0 && e.Key <= Key.D9) && e.KeyboardDevice.Modifiers != ModifierKeys.Shift)
            {
                if (txt.Text.Contains(".") && e.Key == Key.OemPeriod)
                {
                    e.Handled = true;
                    return;
                }
                e.Handled = false;
            }
            else
            {
                e.Handled = true;
            }
        }
        private void Move_TextChanged(object sender, TextChangedEventArgs e)
        {
            //屏蔽中文输入和非法字符粘贴输入
            TextBox textBox = sender as TextBox;
            TextChange[] change = new TextChange[e.Changes.Count];
            e.Changes.CopyTo(change, 0);

            int offset = change[0].Offset;
            if (change[0].AddedLength > 0)
            {
                double num = 0;
                if (!Double.TryParse(textBox.Text, out num) && textBox.Text != "-")
                {
                    textBox.Text = textBox.Text.Remove(offset, change[0].AddedLength);
                    textBox.Select(offset, 0);
                }
                else if (textBox.Text.Contains("-") && textBox.Text.Length > 7)
                {
                    textBox.Text = textBox.Text.Remove(offset, change[0].AddedLength);
                    textBox.Select(offset, 0);
                }
                else if (!textBox.Text.Contains("-") && textBox.Text.Length > 6)
                {
                    textBox.Text = textBox.Text.Remove(offset, change[0].AddedLength);
                    textBox.Select(offset, 0);
                }
            }
        }
        private void Numguide_KeyDown(object sender, KeyEventArgs e)
        {
            TextBox txt = sender as TextBox;

            //屏蔽非法按键
            if (e.Key >= Key.NumPad0 && e.Key <= Key.NumPad9)
            {
                if (txt.Text.Contains(".") && e.Key == Key.Decimal)
                {
                    e.Handled = true;
                    return;
                }
                e.Handled = false;
            }
            else if ((e.Key >= Key.D0 && e.Key <= Key.D9) && e.KeyboardDevice.Modifiers != ModifierKeys.Shift)
            {
                if (txt.Text.Contains(".") && e.Key == Key.OemPeriod)
                {
                    e.Handled = true;
                    return;
                }
                e.Handled = false;
            }
            else if ((e.Key >= Key.A && e.Key <= Key.Z || e.Key == Key.ImeProcessed) && e.KeyboardDevice.Modifiers != ModifierKeys.Shift)
            {
                if (txt.Text.Contains(".") && e.Key == Key.OemPeriod)
                {
                    e.Handled = true;
                    return;
                }
                e.Handled = false;
            }
            else
            {
                e.Handled = true;
            }
        }
        #endregion
        void CsharpConfig_Start()
        {
            bool ret;
            mutex = new System.Threading.Mutex(true, "WpfMuerterrrterterttex", out ret);
            if (!ret)
            {
                MessageBoxResult result = Xceed.Wpf.Toolkit.MessageBox.Show("VIDGS配置软件正在工作中");
                Environment.Exit(0);
            }
        }//避免重复打开软件
        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            //CShareData.spErrorFlag = false; //串口接收错误   
            //Thread.Sleep(10);
            //try
            //{
            //    if (ReadContinueFalg == false)  //发送命令后的第一次接收
            //    {
            //        CShareData.spReceivedDataLen = 0;   //为解决二维扫描，只发送一次信号量的问题，并且不能与下文合并
            //    }
            //    if (serialPort1.IsOpen) //read serialPort1
            //    {
            //        CShareData.spReceivedDataLen += serialPort1.Read(CShareData.spReceivedData, CShareData.spReceivedDataLen, serialPort1.BytesToRead);
            //    }

            //    if (ReadContinueFalg == false)  //发送命令后的第一次接收
            //    {
            //        ReadContinueFalg = true;
            //        ReadDataLen = 5 + (CShareData.spReceivedData[2] << 8) + CShareData.spReceivedData[3]; //计算数据长度
            //    }
            //    if (ReadDataLen == CShareData.spReceivedDataLen)   //此命令数据是否读取完毕
            //    {
            //        ReadContinueFalg = false;
            //        CShareData.spSem.Release(1);    //释放信号量
            //    }
            //    else
            //    {
            //        if (ReadDataLen >= 25969) //处理复位后，接收到的"Hello"，此命令合法，并不算错误
            //        {
            //            ReadContinueFalg = false;
            //        }
            //        else
            //        {
            //            CShareData.spErrorFlag = true;
            //        }
            //    }
            //}
            //catch
            //{
            //    UIAction(() =>
            //    {
            //        if (serialPort1.IsOpen == false)//如果ComPort.IsOpen == false，说明串口已丢失
            //        {
            //            MessageBox.Show(F6.string1);
            //        }
            //        else
            //        {
            //            MessageBox.Show(F7.string1);
            //        }
            //    });
            //}

            //
            lock (CShareData.spMutex)
            {
                CShareData.spErrorFlag = false; //串口接收错误
                //Thread.Sleep(5);
                try
                {
                    if (serialPort1.IsOpen) //read serialPort1
                    {
                        if (CShareData.ReadContinueFalg == false)  //发送命令后的第一次接收时执行
                        {
                            CShareData.spReceivedDataLen = 0;   //为解决二维扫描，只发送一次信号量的问题，并且不能与下文合并
                            CShareData.ReadContinueFalg = true;

                            CShareData.ReadDataLen = 5;
                            CShareData.spSem.WaitOne(0);        //Clean the Semaphore
                            //Console.WriteLine("package start.");
                        }

                        //CShareData.ReadBytes = serialPort1.BytesToRead;
                        //serialPort1.Read(CShareData.spReceivedData, CShareData.spReceivedDataLen, CShareData.ReadBytes);
                        //CShareData.spReceivedDataLen += CShareData.ReadBytes;

                        CShareData.spReceivedDataLen += serialPort1.Read(CShareData.spReceivedData, CShareData.spReceivedDataLen, serialPort1.BytesToRead);
                        if (CShareData.spReceivedDataLen > 4)
                            CShareData.ReadDataLen = 5 + (CShareData.spReceivedData[2] << 8) + CShareData.spReceivedData[3]; //计算数据长度，其实没必要一没次都计算
                        else
                            CShareData.spSem.WaitOne(0);        //Clean the Semaphore

                        /////////////////////if (CShareData.ReadDataLen == 0 || CShareData.spReceivedDataLen == 0) return;

                        if (CShareData.spReceivedDataLen >= CShareData.ReadDataLen)   //此命令数据是否读取完毕
                        {
                            CShareData.ReadContinueFalg = false;
                            CShareData.spErrorFlag = false;
                            CShareData.spSem.WaitOne(0);    //先清空信号量，防止多次释放而达到最大值
                            CShareData.spSem.Release(1);    //释放信号量
                            //Console.WriteLine("package end.");
                        }
                        else
                        {
                            if (CShareData.ReadDataLen >= 25969) //处理复位后，接收到的"Hello"，此命令合法，并不算错误
                            {
                                CShareData.ReadContinueFalg = false;
                                CShareData.spErrorFlag = false;
                            }
                            //else
                            //{
                            //    CShareData.spErrorFlag = true;
                            //}
                        }
                    }//if (serialPort1.IsOpen)
                    else
                    {
                        CShareData.spErrorFlag = true;
                        return;
                    }
                }
                catch
                {
                    return;
                }
            }
            if (!CShareData.ReadContinueFalg) Thread.Sleep(5);
        }
        void UIAction(Action action)//在主线程外激活线程方法
        {
            System.Threading.SynchronizationContext.SetSynchronizationContext(new System.Windows.Threading.DispatcherSynchronizationContext(App.Current.Dispatcher));
            System.Threading.SynchronizationContext.Current.Post(_ => action(), null);
        }
        private bool OpenCOM(SerialPort serialPort1)
        {
            if (serialPort1 == null) return false;
            if (SerialComCanwork == true && serialPort1.IsOpen == true)
            {
                if (laser_using)
                {
                    Thread.Sleep(100);
                    if (laser_using)
                    {
                        Xceed.Wpf.Toolkit.MessageBox.Show("请等待其他指令执行完毕");
                        return false;
                    }
                }
                return true;
            }
            else if (SerialComCanwork == true && serialPort1.IsOpen == false)
            {
                if (laser_using)
                {
                    Thread.Sleep(100);
                    if (laser_using)
                    {
                        Xceed.Wpf.Toolkit.MessageBox.Show("请等待其他指令执行完毕");
                        return false;
                    }
                }
                try
                {
                    if (laser_class != null && BoostIsOpen == true)
                    {
                        OnBnClickedButtonBoostClose_std(laser_class);
                        BoostIsOpen = false;
                    }
                    Thread.Sleep(50);
                    serialPort1.Open();
                    return true;
                }
                catch
                {
                    Xceed.Wpf.Toolkit.MessageBox.Show("无法打开串口, 请检测此串口是否有效或被其他占用");
                    //LinearGradientBrush brush = new LinearGradientBrush();
                    //brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                    //brush.GradientStops.Add(new GradientStop(color: Colors.Red, offset: 1));
                    //Checkbox_Com.Fill = brush;
                    return false;
                }
            }
            else
            {
                return false;
            }
        }// 打开C#串口，关闭C++调用的串口
        private bool CloseCOM(SerialPort serialPort1)
        {
            if (serialPort1 == null) return false;
            if (laser_class == null) return false;
            if (SerialComCanwork == true && serialPort1.IsOpen == true)
            {
                if (laser_using)
                {
                    Thread.Sleep(50);
                    if (laser_using)
                    {
                        Xceed.Wpf.Toolkit.MessageBox.Show("请等待其他指令执行完毕");
                        return false;
                    }
                }
                try
                {
                    serialPort1.Close();
                    while (serialPort1.IsOpen)
                        Thread.Sleep(50);
                    if (BoostIsOpen == false)
                    {
                        OnBnClickedButtonBoostOn_std(laser_class);
                        BoostIsOpen = true;
                        Thread.Sleep(50);
                    }
                    return true;
                }
                catch
                {
                    //LinearGradientBrush brush = new LinearGradientBrush();
                    //brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                    //brush.GradientStops.Add(new GradientStop(color: Colors.Red, offset: 1));
                    //Checkbox_Com.Fill = brush;
                    if (serialPort1.IsOpen == false)//判断当前串口状态，如果ComPort.IsOpen==false，说明串口已丢失
                    {
                        Xceed.Wpf.Toolkit.MessageBox.Show("串口已丢失");
                    }
                    else//未知原因，无法关闭串口
                    {
                        Xceed.Wpf.Toolkit.MessageBox.Show("无法关闭串口，原因未知！");//无法关闭串口，提示后直接返回
                    }
                    return false;
                }
            }
            else
            {
                return true;
            }
        }//
        private void UpdatestoplineFromLaser()
        {
            stoplineFromLaser.Clear();
            Parameter.glb_Parameter = new Parameter(@"test.xml");
            List<ComboPlane> CPList = new List<ComboPlane>();
            List<ComboPlane> CNList = new List<ComboPlane>();
            List<Parameter.StopLine> allComboplanetype = Parameter.glb_Parameter.ReadStopLines();
            foreach (Parameter.StopLine s in allComboplanetype)
            {
                string temps = s.Name.Substring(9, s.Name.Length - 9);
                if (temps.Length == 3)
                {
                    switch (temps.Substring(1, 2))
                    {
                        case "10":
                            temps = temps.Replace("10", "A");
                            break;
                        case "11":
                            temps = temps.Replace("11", "B");
                            break;
                        case "12":
                            temps = temps.Replace("12", "C");
                            break;
                        case "13":
                            temps = temps.Replace("13", "D");
                            break;
                        case "14":
                            temps = temps.Replace("14", "E");
                            break;
                        case "15":
                            temps = temps.Replace("15", "F");
                            break;
                        default:
                            break;
                    }
                }
                stoplineFromLaser.Add(temps);//备注下，走步测试编号保存后更新
                CNList.Add(new ComboPlane() { comboPlane = temps });
                foreach (string t in s.planeTypes)
                {
                    CPList.Add(new ComboPlane() { comboPlane = t });
                }
            }
            drpModel.ItemsSource = CPList;
            drpModel.DisplayMemberPath = "comboPlane";
            drpModel.SelectedValuePath = "comboPlane";
            fuckModel.ItemsSource = CNList;
            fuckModel.DisplayMemberPath = "comboPlane";
            fuckModel.SelectedValuePath = "comboPlane";
        }//更新激光停止线集合，同时更新机型激动引导线的combobox
        private void UpdatestoplineFormVisual()//更新视觉停止线集合
        {
            stoplineFormVisual.Clear();
            foreach (var shape in DrawingBackup)
            {
                if (shape.prompt.Contains("停止线"))
                    stoplineFormVisual.Add(shape.prompt.Substring(3, 1) + shape.prompt.Substring(7, 1));
            }
        }
        private void UpdatestoplineFormBoth()//更新上面两个的并集
        {
            stoplineFromBoth.Clear();
            //List<string> templist = stoplineFromLaser.Union(stoplineFormVisual).ToList();
            List<string> templist = stoplineFormVisual.ToList();
            foreach (var sfuck in templist)
            {
                if (stoplineFromBoth.ContainsKey(sfuck.Substring(0, 1)))
                {
                    stoplineFromBoth[sfuck.Substring(0, 1)].Add(sfuck.Substring(1, 1));
                }
                else
                {
                    stoplineFromBoth.Add(sfuck.Substring(0, 1), new HashSet<string>() { sfuck.Substring(1, 1) });
                }
            }
        }
        private void _startshine()
        {
            laser_using = true;
            ((System.Windows.Media.Animation.Storyboard)this.Resources["startshine"]).Begin(this);
        }
        private void _stopshine()
        {
            laser_using = false;
            ((System.Windows.Media.Animation.Storyboard)this.Resources["shine_toGreen"]).Begin(this);
        }
        private void button_about_Click(object sender, RoutedEventArgs e)
        {
            ((System.Windows.Media.Animation.Storyboard)this.Resources["Showabout"]).Begin(this);
        }
        private void about_CloseButtonClick(object sender, RoutedEventArgs e)
        {
            ((System.Windows.Media.Animation.Storyboard)this.Resources["Hideabout"]).Begin(this);
        }
        #region  zzm  ConfigData配置文件
        //string ConfigDataPath = ".\\ConfigData.txt";
        string ConfigDataPath = ".\\ConfigData.dat";
        //Read txt
        private string[] ReadTXT(string path)
        {
            StreamReader fs = new StreamReader(path, Encoding.UTF8);
            String[] str = new string[100];
            Int16 i = 0;
            while ((str[i] = fs.ReadLine()) != null)
            {
                i++;
            }

            //关闭流
            fs.Close();
            return str;
        }

        private void ReadConfigData()
        {
            string[] ReadTXTStr = ReadTXT(ConfigDataPath);       //数字间采用TAB键分隔
            string[] strs = new string[100];
            strs = ReadTXTStr[1].Split('\t');     //移动步数(上下左右)
            //MoveUpStep.Value = Convert.ToInt16(strs[1]);
            //MoveDownStep.Value = Convert.ToInt16(strs[2]);
            //MoveLeftStep.Value = Convert.ToInt16(strs[3]);
            //MoveRightStep.Value = Convert.ToInt16(strs[4]);

            strs = ReadTXTStr[2].Split('\t');     //边框(水平、垂直)
            CShareData.XEdgeStepSave = Convert.ToInt16(strs[1]);
            CShareData.XProbSave = Convert.ToDouble(strs[2]);
            CShareData.YEdgeStepSave = Convert.ToInt16(strs[3]);
            CShareData.YProbSave = Convert.ToDouble(strs[4]);

            //laXEdgeStep.Text = CShareData.XEdgeStepSave.ToString();
            //laXProb.Text = CShareData.XProbSave.ToString();
            //laYEdgeStep.Text = CShareData.YEdgeStepSave.ToString();
            //laYProb.Text = CShareData.YProbSave.ToString();
        }

        private void WriteConfigData()
        {
            string str = "";
            str += "Config参数：";
            str += "\r\n";
            str += "移动步数(上下左右)：" + "\t" + "26" + "\t" + "26" + "\t" + "26" + "\t" + "26";
            str += "\r\n";
            str += "确认边框(水平、垂直)：" + "\t" + CShareData.XEdgeStepSave.ToString() + "\t" + CShareData.XProbSave.ToString() + "\t" + CShareData.YEdgeStepSave.ToString() + "\t" + CShareData.YProbSave.ToString();
            str += "\r\n";

            LaserFunc.WriteStrToTXT(ConfigDataPath, str);
        }
        #endregion
        #region 记录页面相关
        private void allitem()
        {
            lstProducts.Items.Clear();
            string LocalLogDir = Parameter.glb_Parameter.system_parameter["LogSaveRoot"].InnerText;
            if (LocalLogDir.StartsWith("\"") && LocalLogDir.EndsWith("\"")) LocalLogDir = LocalLogDir.Substring(1, LocalLogDir.Length - 2);
            DirectoryInfo di = new DirectoryInfo(LocalLogDir);
            try
            {
                if (!di.Exists)
                    return;
                if (di == null)
                    return;
                TimeSpan TS1;
                TimeSpan TS2;
                //di.GetDirectories()
                if ((datePicker1.SelectedDate == null) || (datePicker2.SelectedDate == null))
                {
                    return;
                }
                foreach (var fi in di.GetDirectories("*-20*", SearchOption.TopDirectoryOnly))
                {
                    string regex1 = @"20\d{2}-\d{2}-\d{2}T\d{2}.\d{2}.\d{2}";
                    string fitime = Regex.Match(fi.Name, regex1).ToString();
                    fitime = fitime.Replace('T', ' ');
                    string format = "yyyy-MM-dd HH.mm.ss";
                    DateTime convert = DateTime.ParseExact(fitime, format, CultureInfo.CurrentCulture);
                    DateTime datelowbound = datePicker1.SelectedDate.Value;
                    DateTime datehighbound = datePicker2.SelectedDate.Value.AddDays(1);
                    TS1 = datelowbound.Subtract(convert);
                    TS2 = datehighbound.Subtract(convert);
                    if ((TS1.TotalDays < 0) && (TS2.TotalDays > 0))
                    {
                        string s = fi.Name;
                        int star = s.IndexOf("-");
                        if (star == -1 || star > 3) continue;
                        lstProducts.Items.Add(new Emp
                        {
                            TTime = s.Substring(s.IndexOf("-") + 1, 19),
                            PPort = s.Substring(0, s.IndexOf("-")),
                            TType = s.Substring(23, s.Length - 23),
                            FFileName = s
                        });
                    }
                }
            }
            catch (Exception ex)
            {
                Xceed.Wpf.Toolkit.MessageBox.Show(ex.Message);
            }
        }
        private void lstProducts_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            Emp o = lstProducts.SelectedItem as Emp;
            if (o == null || !(o is Emp))
                return;
            if (o.TType == "yard")
            {
                try
                {
                    Log log = new Log();
                    log.Owner = this;
                    log.Title = "站坪扫描记录" + "-" + o.FFileName;
                    log.filename = o.FFileName;
                    log.fileflag1 = o.TTime;
                    log.fileflag2 = o.TType;
                    log.AddHeightLine();
                    log.AddOffsetLine();
                    log.Show();
                    log.Activate();
                    LinearGradientBrush brush = new LinearGradientBrush();
                    brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                    brush.GradientStops.Add(new GradientStop(color: Colors.Green, offset: 1));
                    Checkbox_Problem.Fill = brush;
                }
                catch (Exception ex)
                {
                    Xceed.Wpf.Toolkit.MessageBox.Show(ex.Message);
                }
            }
            else
            {
                try
                {
                    Log log = new Log();
                    log.Owner = this;
                    log.fileflag1 = o.TTime;
                    log.fileflag2 = o.TType;
                    log.fileflag3 = o.PPort;
                    log.LineData(o.FFileName);
                    log.filename = o.FFileName;
                    log.AddHeightLine();
                    log.AddOffsetLine();
                    log.Show();
                    log.Activate();
                    LinearGradientBrush brush = new LinearGradientBrush();
                    brush.GradientStops.Add(new GradientStop(color: Colors.White, offset: 0));
                    brush.GradientStops.Add(new GradientStop(color: Colors.Green, offset: 1));
                    Checkbox_Problem.Fill = brush;
                }
                catch (Exception ex)
                {
                    Xceed.Wpf.Toolkit.MessageBox.Show(ex.Message);
                }
            }
        }
        private void TextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            TextBox textBox = sender as TextBox;
            textBox.Text = string.IsNullOrEmpty(textBox.Text) ? "" : textBox.Text.ToLower(CultureInfo.CurrentCulture).Trim();
            allitem();
            if (textBox.Text == "")
            {
                return;
            }
            for (int it = 0; it < lstProducts.Items.Count; it++)
            {
                Emp o = lstProducts.Items[it] as Emp;
                if (!o.FFileName.ToLower(CultureInfo.CurrentCulture).Contains(textBox.Text))
                {
                    lstProducts.Items.RemoveAt(it);
                    it--;
                }
            }
        }//filter文字改变
        private void datePicker_SelectedDateChanged(object sender, SelectionChangedEventArgs e)
        {
            if(Parameter.glb_Parameter != null) allitem();
        }
        private void Today(object sender, RoutedEventArgs e)//修改10001
        {
            datePicker1.SelectedDate = System.DateTime.Now.AddSeconds(-DateTime.Now.Hour * 3600 -
                DateTime.Now.Minute * 60 - DateTime.Now.Second);
            datePicker2.SelectedDate = System.DateTime.Now.AddDays(0);
        }//当日记录
        private void Sevendays(object sender, RoutedEventArgs e)//修改10001
        {
            datePicker1.SelectedDate = System.DateTime.Now.AddDays(-6).AddSeconds(-DateTime.Now.Hour * 3600 -
                DateTime.Now.Minute * 60 - DateTime.Now.Second);
            datePicker2.SelectedDate = System.DateTime.Now.AddDays(0);
        }//7日记录
        private void Thirtydays(object sender, RoutedEventArgs e)//修改10001
        {
            datePicker1.SelectedDate = System.DateTime.Now.AddMonths(-1).AddSeconds(-DateTime.Now.Hour * 3600 -
                DateTime.Now.Minute * 60 - DateTime.Now.Second);
            datePicker2.SelectedDate = System.DateTime.Now.AddDays(0);
        }//30日记录

        private void AngleSetZeroButton_Click(object sender, RoutedEventArgs e)
        {
            AngleDetector.SetZero();
        }

        public void OnGetNewAngle(object sender, GetAngleArgs e)
        {
            if (AngleDetector.isOpen)
            {
                this.Dispatcher.BeginInvoke(new Action(() =>
                {
                    AngleLabel.Content = string.Format("x:{0:+00.00;-00.00} y:{1:+00.00;-00.00}", e.Xangle, e.Yangle);
                    AngleLabelLaserPanel.Content = string.Format("x:{0:+00.00;-00.00} y:{1:+00.00;-00.00}", e.Xangle, e.Yangle);
                }));
            }
            else
            {
                this.Dispatcher.BeginInvoke(new Action(() =>
                {
                    AngleLabel.Content = "x:NaN y:NaN";
                    AngleLabelLaserPanel.Content = "x:NaN y:NaN";
                }));
            }
        }

        #endregion

        private void button_2dScan0_MouseRightButtonUp(object sender, MouseButtonEventArgs e)
        {
            DrawingBackup2d.Clear();
            if (edit_StandHigh.Text != "")
                CShareData.YShift = Convert.ToInt32(edit_StandHigh.Text) / 2;

            int ForwardOffset = Convert.ToInt32(Parameter.glb_Parameter.laser_machine_datas_list["LaserForwardOffset"].InnerText);
            int BackwardOffset = Convert.ToInt32(Parameter.glb_Parameter.laser_machine_datas_list["LaserBackwardOffset"].InnerText);
            if (OpenCOM(serialPort1))
            {
                Block1.Text = "Z01扫描";
                Block2.Text = "";
                _startshine();
                Int16 ScanPointNum = 109;
                Int32[,] ReceivedDistance = new Int32[2, ScanPointNum];     //读取水平扫描距离值
                Int16 XStartPos = Convert.ToInt16(0 - ((ScanPointNum - 1) >> 2));
                Int16 XEndPos = Convert.ToInt16((ScanPointNum - 1) >> 2);
                System.Windows.Forms.Label statusLabel = new System.Windows.Forms.Label();
                statusLabel.Name = "Status";
                ReceivedDistance = LaserFunc.Z01ScanFunc(CShareData.ZScanDirection.Down, 1, Convert.ToInt16(CShareData.YShift), XStartPos, XEndPos, serialPort1, statusLabel, null);   //调用水平扫描函数

                if (CShareData.Z01ScanFlag)//扫描成功
                {
                    Block2.Text = "执行成功！";
                    AxisMod = 1;
                    needTransform1_2DataCs.guide_line_x = 0;
                    Axis_x_mid = 0;
                    Axis_y_mid = 40000;
                    for (int temp3 = ForwardOffset; temp3 < ScanPointNum; temp3++)
                    {
                        nowShape2d.type = MouseStatus.DrawingPoint;
                        nowShape2d.color = Colors.Brown;
                        nowShape2d.radius = 2;
                        nowShape2d.point1.X = 32 + (temp3 - ForwardOffset) * 290 / ScanPointNum;
                        nowShape2d.point1.Y = 437 - (double)ReceivedDistance[0, temp3] * 35 / 8000;
                        DrawingBackup2d.Add(nowShape2d);
                        nowShape2d = new DrawShape();
                    }
                    NeedFreshK();
                    for (int temp3 = 0; temp3 < ScanPointNum - BackwardOffset; temp3++)
                    {
                        nowShape2d.type = MouseStatus.DrawingPoint;
                        nowShape2d.color = Colors.Blue;
                        nowShape2d.radius = 1;
                        nowShape2d.point1.X = 32 + (temp3 + BackwardOffset) * 290 / ScanPointNum;
                        nowShape2d.point1.Y = 437 - (double)ReceivedDistance[1, temp3] * 35 / 8000;
                        DrawingBackup2d.Add(nowShape2d);
                        nowShape2d = new DrawShape();
                    }
                    NeedFreshK();
                }
                else
                    Block2.Text = "执行失败！" + Environment.NewLine + statusLabel.Text;
                _stopshine();
                string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                string path = CShareData.StartPath + "Z01-" + TimeNow + ".txt";
                LaserFunc.SaveScanDistance(path, ReceivedDistance, ScanPointNum, 2);
            }
        }

        private void button_RoughlyBack0_MouseRightButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (OpenCOM(serialPort1))
            {
                Block1.Text = "粗略回零步数设定";
                Block2.Text = "";
                _startshine();
                Thread confirmRectThread = new Thread(new ThreadStart(() =>
                {
                    //bool bbq = LaserFunc.YQ12DetectionFunc(serialPort1, null, null);
                    int bbq = LaserFunc.RoughReturnZeroSteps(serialPort1, null, null);
                    this.Dispatcher.BeginInvoke(new Action(() =>
                    {
                        if (bbq != -99999)
                        {
                            Block2.Text = "执行成功！" + Environment.NewLine + "Q12步数至地面步数：" + bbq.ToString() + Environment.NewLine + "请点击保存按钮将数据保存至配置文件。";
                        }
                        else
                        {
                            Block2.Text = "执行失败！";
                        }
                        _stopshine();
                    }), null);
                }));
                confirmRectThread.Start();
            }
        }
    }
}