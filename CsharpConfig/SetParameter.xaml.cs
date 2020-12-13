using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Threading;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Interop;
using System.IO;
using System.Runtime.InteropServices;
using System.Configuration;
using Microsoft.Win32;
using System.Diagnostics;
using System.ComponentModel;
using System.Threading;

namespace CsharpConfig
{
    /// <summary>
    /// Parameter.xaml 的交互逻辑
    /// </summary>
    /// 
    /*
    public class Needtransform :DependencyObject  
    {
        //注册依赖项属性 
        public static readonly DependencyProperty TraProperty =
            DependencyProperty.Register("Tra",
            typeof(string),
            typeof(Needtransform),
            new PropertyMetadata(string.Empty));
        public string Tra
        {
            get
            {
                return (string)this.GetValue(TraProperty);
            }
            set
            {
                this.SetValue(TraProperty, value);
            }
        }
        
    }
    */
    /*
    public class ClassneedTransform1_2 : INotifyPropertyChanged
    {
        private int ed;
        public event PropertyChangedEventHandler PropertyChanged;
        public int Edit_StandHigh
        {
            get
            { return ed; }
            set
            {
                ed = value;
                if (this.PropertyChanged != null)
                {
                    this.PropertyChanged.Invoke(this, new PropertyChangedEventArgs("Edit_StandHigh"));
                }
            }
        }
    }
    */
    public partial class SetParameter : Window
    {
        public delegate void TimerEventArgsHandler(object sender, TimerEventArgs e);
        //添加一个PassDataBetweenFormHandler类型的事件
        public event TimerEventArgsHandler TimerCon;
        public static bool MastermodKB = false;//专家模式标志位
        public int AxisMod = 0;//坐标轴模式
        public double renderi = 1;//X轴尺度系数
        public double Yrenderi = 1;//Y轴尺度系数
        public int Axis_x_mid = 0;//初始化X轴刻度中点
        public int Axis_y_mid = 0;//初始化Y轴刻度中点
        //public static int x_walk_T = 0;
        WriteableBitmap ImageBackup2d;
        byte[] newImageBuffer2d;
        WriteableBitmap writeableSource2d;
        List<DrawShape> DrawingBackup2d;//记录当前所画的所有形状
        DrawShape nowShape2d;//当前所画形状
        DrawShape nowShape2dtemp;//NeedFreshK中所画形状
        DrawingVisual drawingVisual;
        MouseStatus nowDrawing2d;//用于ZoomIn and ZoomOut
        //必要变量
        int refleshFlag1Cs = 0, refleshFlag2Cs = 0, refleshFlag3Cs = 0, refleshFlag4Cs = 0;//刷新标志位
        //ClassneedTransform1_2 ClassneedTransform1_2DataCs = new ClassneedTransform1_2();
        needTransform1_2 needTransform1_2DataCs = new needTransform1_2();
        needTransform3 needTransform3DataCs = new needTransform3();
        //ClassneedTransform1_2 classneedTransform1_2DataCs = new ClassneedTransform1_2();
        IntPtr laser_class;
        static IntPtr laser_class_all;
        static int bLaserInited;
        //【修改】添加上边2个变量

        Point offset;//图像偏移，暂时用不到
        int Width2d, Height2d;//图像宽度和长度
        private Point origin2d;
        private DrawShape originShape2d;
        private Point start2d;
        int minImageBorder2d;
        private BackgroundWorker backgroundWorker;
        Set set;
        public SetParameter()
        {
            //bLaserInited = false;
            InitializeComponent();
            nowShape2d = new DrawShape();
            DrawingBackup2d = new List<DrawShape>();
            offset = new Point(0, 0);
            minImageBorder2d = Convert.ToInt32(ConfigurationManager.AppSettings["minImageBorder2d"]);
            //this.MainGrid.DataContext = ClassneedTransform1_2DataCs;
            //初始化的一部分
            backgroundWorker = ((BackgroundWorker)this.FindResource("backgroundWorker"));

            TransformGroup group2d = new TransformGroup();

            ScaleTransform xform2d = new ScaleTransform();
            group2d.Children.Add(xform2d);

            TranslateTransform tt2d = new TranslateTransform();
            group2d.Children.Add(tt2d);

            Image2dBox.RenderTransform = group2d;
        }
        #region UI刷新，界面交互
        public void WPF2Cpp()
        {
            WPF2struct();
            transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
        }
        private void CoolDown()
        {

            set.button_VerticalCalibration.IsEnabled = false;
            button_Back000.IsEnabled = false;
            button_2dScan000.IsEnabled = false;
            button_MasterMod.IsEnabled = false;
            button_Proof.IsEnabled = false;
            button_ReadRect.IsEnabled = false;
            button_PointMeasure.IsEnabled = false;
            button_OpenRed.IsEnabled = false;
            button_CloseRed.IsEnabled = false;
            button_WindowScan.IsEnabled = false; ;
            button_Clc.IsEnabled = false;
            button_Save.IsEnabled = false;
            button_Esc.IsEnabled = false;
        }
        private void CoolDownOut()
        {
            button_Back000.IsEnabled = true;
            button_2dScan000.IsEnabled = true;
            button_MasterMod.IsEnabled = true;
            button_Proof.IsEnabled = true;
            button_ReadRect.IsEnabled = true;
            button_PointMeasure.IsEnabled = true;
            button_OpenRed.IsEnabled = true;
            button_CloseRed.IsEnabled = true;
            button_WindowScan.IsEnabled = true;
            button_Clc.IsEnabled = true;
            button_Save.IsEnabled = true;
            button_Esc.IsEnabled = true;
        }
        public void Csharp2WPF()
        {
            transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, false);//C++ -> C#
            Install_height.Text = needTransform1_2DataCs.install_high.ToString();
            Object_height.Text = needTransform1_2DataCs.object_high.ToString();
            Object_distance.Text = needTransform1_2DataCs.object_dis.ToString();
            edit_StandHigh.Text = needTransform1_2DataCs.Edit_StandHigh.ToString();
            mid_num.Content = needTransform1_2DataCs.mid_num.ToString();
            //set.Model_GuideLine.Text = needTransform1_2DataCs.PlaneType4Confirm;
            //set.Number_GuideLine.Text = needTransform1_2DataCs.LineNum4Confirm;
            //set.Stop_Y.Text = needTransform1_2DataCs.guide_line_y.ToString();
            Block1.Text = needTransform1_2DataCs.m_strRXData;
            Block2.Text = needTransform1_2DataCs.walktext_deflection;
        }
        public  void WPF2struct()
        {
            needTransform1_2DataCs.install_high = Convert.ToInt32(Install_height.Text);
            needTransform1_2DataCs.object_high = Convert.ToInt32(Object_height.Text);
            needTransform1_2DataCs.object_dis = Convert.ToInt32(Object_distance.Text);
            needTransform1_2DataCs.Edit_StandHigh = Convert.ToInt32(edit_StandHigh.Text);
            needTransform1_2DataCs.mid_num = Convert.ToInt32(mid_num.Content);
            //needTransform1_2DataCs.PlaneType4Confirm = set.Model_GuideLine.Text;
            //needTransform1_2DataCs.LineNum4Confirm = set.Number_GuideLine.Text;
            //needTransform1_2DataCs.guide_line_y = Convert.ToInt32(set.Stop_Y.Text);
        }
        public void DrawWalkTest()
        {
            int judge_direction;
            float temp_numerator, temp_denominator, temp_sign;
            /*
            if (needTransform1_2DataCs.x_walk == x_walk_T)
            {
                return;
            }
            */
            //用直线方程计算偏转
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
                    nowShape2d.point1.X = (needTransform1_2DataCs.x_walk - needTransform1_2DataCs.guide_line_x) * 145 / 10000 + 177;
                    nowShape2d.point1.Y = 437 - needTransform1_2DataCs.y_walk * 35 / 8000;
                    DrawingBackup2d.Add(nowShape2d);
                    nowShape2d = new DrawShape();
                    NeedFreshK();
                }
                else if (judge_direction > 200)
                {
                    //pDrawControlDC->SelectObject(&OrangePen); 
                    nowShape2d.type = MouseStatus.DrawingPoint;
                    nowShape2d.color = Colors.Purple;
                    nowShape2d.point1.X = (needTransform1_2DataCs.x_walk - needTransform1_2DataCs.guide_line_x) * 145 / 10000 + 177;
                    nowShape2d.point1.Y = 437 - needTransform1_2DataCs.y_walk * 35 / 8000;
                    DrawingBackup2d.Add(nowShape2d);
                    nowShape2d = new DrawShape();
                    NeedFreshK();
                }
                else //if (judge_direction>100)
                {
                    //pDrawControlDC->SelectObject(&GreenPen);
                    nowShape2d.type = MouseStatus.DrawingPoint;
                    nowShape2d.color = Colors.Blue;
                    nowShape2d.point1.X = (needTransform1_2DataCs.x_walk - needTransform1_2DataCs.guide_line_x) * 145 / 10000 + 177;
                    nowShape2d.point1.Y = 437 - needTransform1_2DataCs.y_walk * 35 / 8000;
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
                    nowShape2d.point1.X = (needTransform1_2DataCs.x_walk - needTransform1_2DataCs.guide_line_x) * 145 / 10000 + 177;
                    nowShape2d.point1.Y = 437 - needTransform1_2DataCs.y_walk * 35 / 8000;
                    DrawingBackup2d.Add(nowShape2d);
                    nowShape2d = new DrawShape();
                    NeedFreshK();
                }
                else if (judge_direction < -200)
                {
                    //pDrawControlDC->SelectObject(&OrangePen);
                    nowShape2d.type = MouseStatus.DrawingPoint;
                    nowShape2d.color = Colors.Purple;
                    nowShape2d.point1.X = (needTransform1_2DataCs.x_walk - needTransform1_2DataCs.guide_line_x) * 145 / 10000 + 177;
                    nowShape2d.point1.Y = 437 - needTransform1_2DataCs.y_walk * 35 / 8000;
                    DrawingBackup2d.Add(nowShape2d);
                    nowShape2d = new DrawShape();
                    NeedFreshK();
                }
                else //if (judge_direction>100)
                {
                    //pDrawControlDC->SelectObject(&GreenPen);
                    nowShape2d.type = MouseStatus.DrawingPoint;
                    nowShape2d.color = Colors.Blue;
                    nowShape2d.point1.X = (needTransform1_2DataCs.x_walk - needTransform1_2DataCs.guide_line_x) * 145 / 10000 + 177;
                    nowShape2d.point1.Y = 437 - needTransform1_2DataCs.y_walk * 35 / 8000;
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
                nowShape2d.point1.X = (needTransform1_2DataCs.x_walk - needTransform1_2DataCs.guide_line_x) * 145 / 10000 + 177;
                nowShape2d.point1.Y = 437 - needTransform1_2DataCs.y_walk * 35 / 8000;
                DrawingBackup2d.Add(nowShape2d);
                nowShape2d = new DrawShape();
                NeedFreshK();
            }
            //x_walk_T = needTransform1_2DataCs.x_walk;
        }
        public void FreshUI()//int bThreading = 0)
        {
            //timer = new DispatcherTimer();
            //timer.Tick += new EventHandler(timer_Tick_static2);
            //timer.Interval = new TimeSpan(0,0,0,3,0);
            //timer.Start();
            //this.Dispatcher.BeginInvoke(DispatcherPriority.Normal, (ThreadStart)delegate
            //{
            while (true)
            {
                refleshFlag1Cs = trans1Flag_std(laser_class, 1);
                refleshFlag2Cs = trans1Flag_std(laser_class, 2);
                refleshFlag3Cs = trans1Flag_std(laser_class, 3);
                refleshFlag4Cs = trans1Flag_std(laser_class, 4);
                if (refleshFlag1Cs == 1 || refleshFlag1Cs == 2)//刷新界面
                {
                    if (refleshFlag1Cs == 1)
                    {
                        refleshFlag1Cs = 0; setFlagTo_std(laser_class, 0, 1);
                    }//刷新并跳出
                    else if (refleshFlag1Cs == 2)
                    {
                        refleshFlag1Cs = -1; setFlagTo_std(laser_class, -1, 1);
                    }//刷新并继续刷新
                    //刷新界面
                    transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, false);//C++ -> C#
                    this.Dispatcher.BeginInvoke(new Action(() =>
                        {
                            Install_height.Text = needTransform1_2DataCs.install_high.ToString();
                            Object_height.Text = needTransform1_2DataCs.object_high.ToString();
                            Object_distance.Text = needTransform1_2DataCs.object_dis.ToString();
                            edit_StandHigh.Text = needTransform1_2DataCs.Edit_StandHigh.ToString();
                            mid_num.Content = needTransform1_2DataCs.mid_num.ToString();
                            //set.Model_GuideLine.Text = needTransform1_2DataCs.PlaneType4Confirm;
                            //set.Number_GuideLine.Text = needTransform1_2DataCs.LineNum4Confirm;
                            //set.Stop_Y.Text = needTransform1_2DataCs.guide_line_y.ToString();
                            Block1.Text = needTransform1_2DataCs.m_strRXData;
                            Block2.Text = needTransform1_2DataCs.walktext_deflection;
                        }), null);
                    
                }
                if (refleshFlag2Cs == 1 || refleshFlag2Cs == 2)//画引导线
                {
                    if (refleshFlag2Cs == 1) { refleshFlag2Cs = 0; setFlagTo_std(laser_class, 0, 2); }//刷新并跳出
                    else if (refleshFlag2Cs == 2) { refleshFlag2Cs = -1; setFlagTo_std(laser_class, -1, 2); }//刷新并继续刷新
                    transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, false);
                    //画引导线
                    AxisMod = 2;
                    if (needTransform1_2DataCs.guide_line_k != 0)
                    {
                        nowShape2d.type = MouseStatus.DrawingLine;
                        nowShape2d.color = Colors.Yellow;
                        nowShape2d.point1.X = 177;
                        nowShape2d.point1.Y = 437 - needTransform1_2DataCs.guide_line_y * 35 / 8000;
                        nowShape2d.point2.X = (80000 - needTransform1_2DataCs.guide_line_y) * 145 / (10000 * needTransform1_2DataCs.guide_line_k) + 177;
                        nowShape2d.point2.Y = 87;
                        Axis_x_mid = needTransform1_2DataCs.guide_line_x;
                        Axis_y_mid = 40000;
                        renderi = 1;
                        DrawingBackup2d.Add(nowShape2d);
                        nowShape2d = new DrawShape();
                        nowShape2d.type = MouseStatus.DrawingLine;
                        nowShape2d.color = Colors.Black;
                        nowShape2d.point1.X = 157;
                        nowShape2d.point1.Y = 437 - needTransform1_2DataCs.guide_line_y * 35 / 8000;
                        nowShape2d.point2.X = 197;
                        nowShape2d.point2.Y = 437 - needTransform1_2DataCs.guide_line_y * 35 / 8000;
                        DrawingBackup2d.Add(nowShape2d);
                        nowShape2d = new DrawShape();
                        NeedFreshK();
                    }
                }
                if (refleshFlag3Cs == 1 || refleshFlag3Cs == 2)//画二维点
                {
                    if (refleshFlag3Cs == 1) { refleshFlag3Cs = 0; setFlagTo_std(laser_class, 0, 3); }//刷新并跳出
                    else if (refleshFlag3Cs == 2) { refleshFlag3Cs = -1; setFlagTo_std(laser_class, -1, 3); }//刷新并继续刷新
                    transform3Struct_std(laser_class, ref needTransform3DataCs, false);
                    //画二维点
                    AxisMod = 1;
                    for (int temp3 = 0; temp3 < 106; temp3++)
                    {
                        nowShape2d.type = MouseStatus.DrawingPoint;
                        nowShape2d.color = Colors.Brown;
                        nowShape2d.point1.X = 64 + temp3 * 193 / 105;
                        nowShape2d.point1.Y = 437 - needTransform3DataCs.save_data[temp3] * 35 / 8000;
                        DrawingBackup2d.Add(nowShape2d);
                        nowShape2d = new DrawShape();
                        NeedFreshK();
                    }

                }
                if (refleshFlag4Cs == 1 || refleshFlag4Cs == 2)//画走步测试
                {
                    if (refleshFlag4Cs == 1) { refleshFlag4Cs = 0; setFlagTo_std(laser_class, 0, 4); }//刷新并跳出
                    else if (refleshFlag4Cs == 2) { refleshFlag4Cs = -1; setFlagTo_std(laser_class, -1, 4); }//刷新并继续刷新
                    transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, false);
                    //画走步测试
                    //DrawWalkTest();
                }
                if (refleshFlag1Cs == 0 && refleshFlag2Cs == 0 && refleshFlag3Cs == 0 && refleshFlag4Cs == 0)
                    break;
                //System.Threading.Thread.Sleep(50);
            }
            //    });
        }
        private void backgroundWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            int back = 1;
            bool bNeedFreshUI = false;

            while (true)
            {
                bNeedFreshUI = false;
                refleshFlag1Cs = trans1Flag_std(laser_class, 1);
                refleshFlag2Cs = trans1Flag_std(laser_class, 2);
                refleshFlag3Cs = trans1Flag_std(laser_class, 3);
                refleshFlag4Cs = trans1Flag_std(laser_class, 4);
                if (refleshFlag1Cs == 1 || refleshFlag1Cs == 2)//刷新界面
                {
                    bNeedFreshUI = true;
                }
                if (refleshFlag2Cs == 1 || refleshFlag2Cs == 2)//画引导线
                {
                    bNeedFreshUI = true;
                }
                if (refleshFlag3Cs == 1 || refleshFlag3Cs == 2)//画二维点
                {
                    bNeedFreshUI = true;
                }
                if (refleshFlag4Cs == 1 || refleshFlag4Cs == 2)//画走步测试
                {
                    bNeedFreshUI = true;
                }
                if (bNeedFreshUI)
                {
                    back = -back;
                    backgroundWorker.ReportProgress(back);
                }

                if (refleshFlag1Cs == 0 && refleshFlag2Cs == 0 && refleshFlag3Cs == 0 && refleshFlag4Cs == 0)
                    break;//from while()
                System.Threading.Thread.Sleep(50);
            }
            ////org
            //for (back=0; back <= 100;back++ )
            //{
            //    refleshFlag1Cs = trans1Flag_std(laser_class, 1);
            //    if (refleshFlag1Cs == 1)
            //    {
            //        backgroundWorker.ReportProgress(100);
            //        break;
            //    }
            //    System.Threading.Thread.Sleep(200);
            //    backgroundWorker.ReportProgress(back);
            //}
        }
        private void backgroundWorker_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            ProgressBar1.IsIndeterminate = true;
            if (refleshFlag1Cs == 1 || refleshFlag1Cs == 2)//刷新界面
            {
                if (refleshFlag1Cs == 1)
                {
                    refleshFlag1Cs = 0; setFlagTo_std(laser_class, 0, 1);                    
                }//刷新并跳出
                else if (refleshFlag1Cs == 2)
                {
                    refleshFlag1Cs = -1; setFlagTo_std(laser_class, -1, 1);                    
                }//刷新并继续刷新
                //刷新界面
                transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, false);//C++ -> C#
                Install_height.Text = needTransform1_2DataCs.install_high.ToString();
                Object_height.Text = needTransform1_2DataCs.object_high.ToString();
                Object_distance.Text = needTransform1_2DataCs.object_dis.ToString();
                edit_StandHigh.Text = needTransform1_2DataCs.Edit_StandHigh.ToString();
                mid_num.Content = needTransform1_2DataCs.mid_num.ToString();
                //set.Model_GuideLine.Text = needTransform1_2DataCs.PlaneType4Confirm;
                //set.Number_GuideLine.Text = needTransform1_2DataCs.LineNum4Confirm;
                //set.Stop_Y.Text = needTransform1_2DataCs.guide_line_y.ToString();
                /*Stop_Y.Text = "123";*/
                Block1.Text = needTransform1_2DataCs.m_strRXData;
                Block2.Text = needTransform1_2DataCs.walktext_deflection;
            }
            if (refleshFlag2Cs == 1 || refleshFlag2Cs == 2)//画引导线
            {
                if (refleshFlag2Cs == 1) { refleshFlag2Cs = 0; setFlagTo_std(laser_class, 0, 2); }//刷新并跳出
                else if (refleshFlag2Cs == 2) { refleshFlag2Cs = -1; setFlagTo_std(laser_class, -1, 2); }//刷新并继续刷新
                transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, false);

                //画引导线
                AxisMod = 2;
                if (needTransform1_2DataCs.guide_line_k != 0)
                {
                    nowShape2d.type = MouseStatus.DrawingLine;
                    nowShape2d.color = Colors.Yellow;
                    nowShape2d.point1.X = 177;
                    nowShape2d.point1.Y = 437 - needTransform1_2DataCs.guide_line_y * 35 / 8000;
                    nowShape2d.point2.X = (80000 - needTransform1_2DataCs.guide_line_y) * 145 / (10000 * needTransform1_2DataCs.guide_line_k) + 177;
                    nowShape2d.point2.Y = 87;
                    Axis_x_mid = needTransform1_2DataCs.guide_line_x;
                    Axis_y_mid = 40000;
                    renderi = 1;
                    DrawingBackup2d.Add(nowShape2d);
                    nowShape2d = new DrawShape();
                    nowShape2d.type = MouseStatus.DrawingLine;
                    nowShape2d.color = Colors.Black;
                    nowShape2d.point1.X = 157;
                    nowShape2d.point1.Y = 437 - needTransform1_2DataCs.guide_line_y * 35 / 8000;
                    nowShape2d.point2.X = 197;
                    nowShape2d.point2.Y = 437 - needTransform1_2DataCs.guide_line_y * 35 / 8000;
                    DrawingBackup2d.Add(nowShape2d);
                    nowShape2d = new DrawShape();
                    NeedFreshK();
                }
            }
            if (refleshFlag3Cs == 1 || refleshFlag3Cs == 2)//画二维点
            {
                if (refleshFlag3Cs == 1) { refleshFlag3Cs = 0; setFlagTo_std(laser_class, 0, 3); }//刷新并跳出
                else if (refleshFlag3Cs == 2) { refleshFlag3Cs = -1; setFlagTo_std(laser_class, -1, 3); }//刷新并继续刷新
                transform3Struct_std(laser_class, ref needTransform3DataCs, false);
                Install_height.Text = needTransform1_2DataCs.install_high.ToString();
                Object_height.Text = needTransform1_2DataCs.object_high.ToString();
                Object_distance.Text = needTransform1_2DataCs.object_dis.ToString();
                edit_StandHigh.Text = needTransform1_2DataCs.Edit_StandHigh.ToString();
                mid_num.Content = needTransform1_2DataCs.mid_num.ToString();
                //set.Model_GuideLine.Text = needTransform1_2DataCs.PlaneType4Confirm;
                //set.Number_GuideLine.Text = needTransform1_2DataCs.LineNum4Confirm;
                //set.Stop_Y.Text = needTransform1_2DataCs.guide_line_y.ToString();
                /*Stop_Y.Text = "123";*/
                Block1.Text = needTransform1_2DataCs.m_strRXData;
                Block2.Text = needTransform1_2DataCs.walktext_deflection;
                //画二维点
                AxisMod = 1;
                for (int temp3 = 0; temp3 < 106; temp3++)
                {
                    nowShape2d.type = MouseStatus.DrawingPoint;
                    nowShape2d.color = Colors.Brown;
                    nowShape2d.point1.X = 64 + temp3 * 193 / 105;
                    nowShape2d.point1.Y = 437 - needTransform3DataCs.save_data[temp3] * 35 / 8000;
                    DrawingBackup2d.Add(nowShape2d);
                    nowShape2d = new DrawShape();
                    NeedFreshK();
                }

            }
            if (refleshFlag4Cs == 1 || refleshFlag4Cs == 2)//画走步测试
            {
                if (refleshFlag4Cs == 1) { refleshFlag4Cs = 0; setFlagTo_std(laser_class, 0, 4); }//刷新并跳出
                else if (refleshFlag4Cs == 2) { refleshFlag4Cs = -1; setFlagTo_std(laser_class, -1, 4); }//刷新并继续刷新
                transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, false);
                AxisMod = 2;
                Axis_x_mid = needTransform1_2DataCs.guide_line_x;
                Axis_y_mid = 40000;
                renderi = 1;
                //画走步测试
                DrawWalkTest();
            }
            //if (refleshFlag1Cs == 0 && refleshFlag2Cs == 0 && refleshFlag3Cs == 0 && refleshFlag4Cs == 0)break;

            ////org
            //transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, false);//C++ -> C#
            //edit_StandHigh.Text = needTransform1_2DataCs.Edit_StandHigh.ToString();

        }
        private void backgroundWorker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            backgroundWorker.CancelAsync();
            //CoolDownOut();
            ProgressBar1.IsIndeterminate = false;
        }
        #endregion
        private void SetParameter_Loaded(object sender, RoutedEventArgs e)
        {
            //Image2dBox.Source = new BitmapImage(new Uri(@"E:\虹桥\configWin32App\CsharpConfig\CsharpConfig\可视化3.bmp"));
            System.Drawing.Bitmap image = new System.Drawing.Bitmap(@"./可视化3.bmp");
            Image2dBox.Source = MainWindow.ToBitmapSource(image);
            Width2d = (int)Image2dBox.Source.Width;
            Height2d = (int)Image2dBox.Source.Height;
            ImageBackup2d = new WriteableBitmap(Image2dBox.Source as BitmapSource);
            ImageBackup2d = BitmapFactory.ConvertToPbgra32Format(ImageBackup2d);
            writeableSource2d = ImageBackup2d.Clone();
            newImageBuffer2d = new byte[2000 * 2000 * 4];
            //this.Grid1.DataContext = classneedTransform1_2DataCs;
            //【修改】开始
            if (0 == bLaserInited)
            {
                laser_class = classLaserInit();//C++创建类（C#只能使用laser_class）
                laser_class_all = laser_class;
                bLaserInited = 1;
            //trans1pointerInt_std(laser_class, ref refleshFlag1Cs, 1);//把4个标志位的地址赋给C++，实现共用内存
            //trans1pointerInt_std(laser_class, ref refleshFlag2Cs, 2);
            //trans1pointerInt_std(laser_class, ref refleshFlag3Cs, 3);
            //trans1pointerInt_std(laser_class, ref refleshFlag4Cs, 4);
            OnBnClickedConnect_std(laser_class);
            }
            else
            {
                laser_class = laser_class_all;
            }
            //【修改】结束
            transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, false);//C++ -> C#
            Install_height.Text = needTransform1_2DataCs.install_high.ToString();
            Object_height.Text = needTransform1_2DataCs.object_high.ToString();
            Object_distance.Text = needTransform1_2DataCs.object_dis.ToString();
            edit_StandHigh.Text = needTransform1_2DataCs.Edit_StandHigh.ToString();
            mid_num.Content = needTransform1_2DataCs.mid_num.ToString();
            Block1.Text = needTransform1_2DataCs.m_strRXData;
            Block2.Text = needTransform1_2DataCs.walktext_deflection;
            try
            {
                VistaGlassHelper.ExtendGlass(this, -1, -1, -1, -1);

            }
            // If not Vista, paint background white.
            catch //(DllNotFoundException)
            {
                this.Background = Brushes.White;
            }
        }
        void NeedFreshK()
        {
            if (Image2dBox.Source != null)
            {
                //CameraImageBox.Source = null;
                //LastImage = null;

                //if (CameraImageBox.IsMouseCaptured)
                //{
                this.Dispatcher.BeginInvoke(new Action(() =>
                {
                byte[] ImageBackupBuffer2d = ImageBackup2d.ToByteArray();

                ImageBackupBuffer2d.CopyTo(newImageBuffer2d, 0);
                writeableSource2d.Clear();
                writeableSource2d.FromByteArray(newImageBuffer2d, ImageBackupBuffer2d.Length);
                //}
                //else
                //{
                //    writeableSource = ImageBackup;
                //}
                //Image<Gray, Byte> grayFrame = currentFrame.Convert<Gray, Byte>();
                foreach (var shape in DrawingBackup2d)
                {
                    if (shape.point1.X == 0 && shape.point1.Y == 0) continue;
                    switch (shape.type)
                    {
                        case MouseStatus.DrawingLine:
                            if (shape.point2.X == 0 && shape.point2.Y == 0) continue;
                            int temp_1_x = (int)((shape.point1.X - 177) * 10000 / 145 + needTransform1_2DataCs.guide_line_x);
                            int temp_1_y = (int)((437 - shape.point1.Y) * 8000 / 35);
                            int temp_2_x = (int)((shape.point2.X - 177) * 10000 / 145 + needTransform1_2DataCs.guide_line_x);
                            int temp_2_y = (int)((437 - shape.point2.Y) * 8000 / 35);
                            nowShape2dtemp = new DrawShape();
                            nowShape2dtemp.point1.X = (int)((temp_1_x - Axis_x_mid) * 145 * renderi / 10000 + 177);
                            nowShape2dtemp.point1.Y = (int)(262 - (temp_1_y - Axis_y_mid) * 35 * renderi / 8000);
                            nowShape2dtemp.point2.X = (int)((temp_2_x - Axis_x_mid) * 145 * renderi / 10000 + 177);
                            nowShape2dtemp.point2.Y = (int)(262 - (temp_2_y - Axis_y_mid) * 35 * renderi / 8000);
                            DrawThickLine(ref writeableSource2d, nowShape2dtemp.point1, nowShape2dtemp.point2, 1, shape.color);
                            //WriteableBitmapExtensions.DrawLineAa(writeableSource, (int)shape.point1.X, 
                            //(int)shape.point1.Y, (int)shape.point2.X, (int)shape.point2.Y, shape.color);
                            break;
                        case MouseStatus.DrawingRect:
                            break;
                        case MouseStatus.DrawingPoint:
                            int temp_x = (int)((shape.point1.X - 177) * 10000 / 145 + needTransform1_2DataCs.guide_line_x);
                            int temp_y = (int)((437-shape.point1.Y)*8000/35);
                            if (temp_x <= (Axis_x_mid + 10000 / renderi) && temp_x >= (Axis_x_mid - 10000 / renderi) &&
                                temp_y <= (Axis_y_mid + 40000 / renderi) && temp_y >= (Axis_y_mid - 40000 / renderi))
                            {
                                WriteableBitmapExtensions.FillEllipseCentered(writeableSource2d, (int)((temp_x-Axis_x_mid)*145*renderi/10000+177), (int)(262-(temp_y-Axis_y_mid)*35*renderi/8000), 1, 1, shape.color);
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
        }

        //------DLL接口声明
        //class __declspec(dllexport) Recog;
        //[DllImport("config.dll")]
        //public class CLaster { }//无法使用？

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
        public static extern void OnBnClickedButtonScanall_std(IntPtr laser_class);//全屏扫描
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


        [DllImport("gdi32")]
        private static extern int DeleteObject(IntPtr o);
        #endregion

        public static void DrawThickLine(ref WriteableBitmap writeableBmp, Point point1, Point point2, double thickness, Color color)
        {
            //Original points for line 
            int x1 = (int)point1.X;
            int y1 = (int)point1.Y;
            int x2 = (int)point2.X;
            int y2 = (int)point2.Y;

            //Parallel line code from http://stackoverflow.com/questions/2825412/draw-a-parallel-line 
            var L = Math.Sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)) * 2;
            // This is the second line will be parallel to the first 
            int x1p = (int)(x1 + thickness * (y2 - y1) / L);
            int x2p = (int)(x2 + thickness * (y2 - y1) / L);
            int y1p = (int)(y1 + thickness * (x1 - x2) / L);
            int y2p = (int)(y2 + thickness * (x1 - x2) / L);

            int x1q = (int)(x1 - thickness * (y2 - y1) / L);
            int x2q = (int)(x2 - thickness * (y2 - y1) / L);
            int y1q = (int)(y1 - thickness * (x1 - x2) / L);
            int y2q = (int)(y2 - thickness * (x1 - x2) / L);

            //writeableBmp.DrawLine(x1, y1, x2, y2, Colors.Red); 
            //writeableBmp.DrawLine(x1p, y1p, x2p, y2p, Colors.Blue); 
            //Create closed filled polygon for "thick line" 
            writeableBmp.FillPolygon(new int[] { x1q, y1q, x2q, y2q, x2p, y2p, x1p, y1p, x1q, y1q }, color);
        }
        private void Image2dBox_MouseWheel(object sender, MouseWheelEventArgs e)
        {
            TransformGroup transformGroup2d = (TransformGroup)Image2dBox.RenderTransform;
            ScaleTransform transform2d = (ScaleTransform)transformGroup2d.Children[0];

            double zoom = e.Delta > 0 ? .2 : -.2;

            renderi += zoom;
            if (renderi < 0.5) renderi = 0.5;
            else if (renderi > 16) renderi = 16;
            NeedFreshK();
        }
        private void Image2dBox_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Image2dBox.CaptureMouse();
            Point thisPoint = e.GetPosition(e.Source as FrameworkElement);
            thisPoint.X = offset.X+ thisPoint.X * Width2d / Image2dBox.Width;
            thisPoint.Y = offset.Y + thisPoint.Y * Height2d / Image2dBox.Height;
            if (thisPoint.X < 32 || thisPoint.Y > 438) return;
            switch (nowDrawing2d)
            {
                case MouseStatus.ZoomIn:
                    if (renderi >= 16) renderi = 16;
                    else
                    {
                        renderi *= 2;
                    }
                    if (thisPoint.X < 105 && thisPoint.X >= 32)
                    {
                        Axis_x_mid -= (int)(10000 / renderi);
                    }
                    else if (thisPoint.X >= 105 && thisPoint.X < 250)
                    {
                        Axis_x_mid += (int)((thisPoint.X - 177) * 10000 / (145 * renderi));
                    }
                    else if (thisPoint.X >= 250 && thisPoint.X <= 322)
                    {
                        Axis_x_mid += (int)(10000 / renderi);
                    }
                    if (thisPoint.Y <= 438 && thisPoint.Y > 350)
                    {
                        Axis_y_mid -= (int)(40000 / renderi);
                    }
                    else if (thisPoint.Y > 174 && thisPoint.Y <= 350)
                    {
                        Axis_y_mid -= (int)((thisPoint.Y - 262) * 40000 / (175.5 * renderi));
                    }
                    else if (thisPoint.Y >= 0 && thisPoint.Y <= 174)
                    {
                        Axis_y_mid += (int)(10000 / renderi);
                    }
                    NeedFreshK();
                    break;
                case MouseStatus.ZoomOut:
                    if (thisPoint.X < 105 && thisPoint.X >= 32)
                    {
                        Axis_x_mid += (int)(10000 / renderi);
                        if ((Axis_x_mid + 10000 / renderi) > (needTransform1_2DataCs.guide_line_x + 10000)) Axis_x_mid +=
 (needTransform1_2DataCs.guide_line_x + 10000) - (Axis_x_mid + (int)(10000 / renderi));
                    }
                    else if (thisPoint.X >= 105 && thisPoint.X < 250)
                    {
                        Axis_x_mid += (int)((thisPoint.X - 177) * 10000 / (145 * renderi));
                        if ((Axis_x_mid - 10000 / renderi) < (needTransform1_2DataCs.guide_line_x - 10000)) Axis_x_mid +=
       (needTransform1_2DataCs.guide_line_x - 10000) - (Axis_x_mid - (int)(10000 / renderi));
                        else if ((Axis_x_mid + 10000 / renderi) > (needTransform1_2DataCs.guide_line_x + 10000)) Axis_x_mid +=
 (needTransform1_2DataCs.guide_line_x + 10000) - (Axis_x_mid + (int)(10000 / renderi));
                    }
                    else if (thisPoint.X >= 250 && thisPoint.X <= 322)
                    {
                        Axis_x_mid -= (int)(10000 / renderi);
                        if ((Axis_x_mid - 10000 / renderi) < (needTransform1_2DataCs.guide_line_x - 10000)) Axis_x_mid +=
       (needTransform1_2DataCs.guide_line_x - 10000) - (Axis_x_mid - (int)(10000 / renderi));
                    }
                    if (thisPoint.Y <= 438 && thisPoint.Y > 350)
                    {
                        Axis_y_mid += (int)(40000 / renderi);
                        if ((Axis_y_mid + 40000 / renderi) > 80000) Axis_y_mid += 80000 - (Axis_y_mid + (int)(40000 / renderi));
                    }
                    else if (thisPoint.Y > 174 && thisPoint.Y <= 350)
                    {
                        Axis_y_mid -= (int)((thisPoint.Y - 262) * 40000 / (175.5 * renderi));
                        if ((Axis_y_mid + 40000 / renderi) > 80000) Axis_y_mid += 80000 - (Axis_y_mid + (int)(40000 / renderi));
                        else if ((Axis_y_mid - 40000 / renderi) < 0) Axis_y_mid -= (Axis_y_mid - (int)(40000 / renderi));
                    }
                    else if (thisPoint.Y >= 0 && thisPoint.Y <= 174)
                    {
                        Axis_y_mid -= (int)(10000 / renderi);
                        if ((Axis_y_mid - 40000 / renderi) < 0) Axis_y_mid -= (Axis_y_mid - (int)(40000 / renderi));
                    }
                    if (renderi <= 2)
                    {
                        renderi = 1;
                        Axis_x_mid = needTransform1_2DataCs.guide_line_x;
                        Axis_y_mid = 40000;
                    }
                    else renderi /= 2;
                    NeedFreshK();
                    break;
                default:
                    break;
            }
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
            start2d = e.GetPosition(border);
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
                labelPosition.Content = ((437 - thisPoint.Y) * 8000 / 35).ToString();
            }
            else labelPosition.Content = string.Format("x:{0:f} y:{1:f}", (thisPoint.X - 177) * 10000 / (145 * renderi) +
                Axis_x_mid, (262 - thisPoint.Y) * 8000 / (35 * renderi) + Axis_y_mid);
            if (!Image2dBox.IsMouseCaptured) return;
            if (e.RightButton == MouseButtonState.Pressed)
            {
                //var trans = (TranslateTransform)((TransformGroup)Image2dBox.RenderTransform).Children.First(tr => tr is TranslateTransform);
                Vector v = start2d - e.GetPosition(border);
                v.Y = -v.Y;
                start2d = e.GetPosition(border);

                Axis_x_mid += (int)(v.X * 10000 / (145 * renderi));
                Axis_y_mid += (int)(v.Y * 40000 / (175.5 * renderi));
                if (Axis_x_mid < -10000) Axis_x_mid = -10000;
                else if (Axis_x_mid > 10000) Axis_x_mid = 10000;
                if (Axis_y_mid < 0) Axis_y_mid = 0;
                else if (Axis_y_mid > 80000) Axis_y_mid = 80000;
                NeedFreshK();
                //trans.X = origin2d.X - v.X;
                //trans.Y = origin2d.Y - v.Y;
                //if (posGroup.X < 0) end.X -= posGroup.X;
                //if (posGroup.Y < 0) end.Y -= posGroup.Y;
                //if (posGroup.X > Group2.ActualWidth) end.X -= posGroup.X - Group2.ActualWidth;
                //if (posGroup.Y > Group2.ActualHeight) end.Y -= posGroup.Y - Group2.ActualHeight;
                //TransformGroup transformGroup = (TransformGroup)Image2dBox.RenderTransform;
                //ScaleTransform scale = (ScaleTransform)transformGroup.Children[0];
                //label_Position.Content = string.Format("x:{0},y:{1}", (int)(CameraImageBox.ActualWidth * transform.ScaleX), (int)(CameraImageBox.ActualHeight * transform.ScaleY));
                /*
                if (trans.X < minImageBorder2d - Group2.ActualWidth / 2 - Image2dBox.ActualWidth * scale.ScaleX / 2)
                    trans.X = minImageBorder2d - Group2.ActualWidth / 2 - Image2dBox.ActualWidth * scale.ScaleX / 2;
                if (trans.Y < minImageBorder2d - Group2.ActualHeight / 2 - Image2dBox.ActualHeight * scale.ScaleY / 2)
                    trans.Y = minImageBorder2d - Group2.ActualHeight / 2 - Image2dBox.ActualHeight * scale.ScaleY / 2;
                if (trans.X > Group2.ActualWidth / 2 + Image2dBox.ActualWidth * scale.ScaleX / 2 - minImageBorder2d)
                    trans.X = Group2.ActualWidth / 2 + Image2dBox.ActualWidth * scale.ScaleX / 2 - minImageBorder2d;
                if (trans.Y > Group2.ActualHeight / 2 + Image2dBox.ActualHeight * scale.ScaleY / 2 - minImageBorder2d)
                    trans.Y = Group2.ActualHeight / 2 + Image2dBox.ActualHeight * scale.ScaleY / 2 - minImageBorder2d;
                */
            }
        }
        #region 激光界面按钮函数
        private void button_Clc_Click(object sender, RoutedEventArgs e)
        {
            DrawingBackup2d.Clear();
            AxisMod = 0;
            NeedFreshK();
        }
        private void button_Start_Click(object sender, RoutedEventArgs e)
        {
        }
        private void button_Proof_Click(object sender, RoutedEventArgs e)//校准
        {
            OnBnClickedButtonDetect_std(laser_class);
            FreshUI();
        }
        private void button_WalkSet_Click(object sender, EventArgs1 e)//走不设置
        {
            //CoolDown();
            WPF2struct();
            needTransform1_2DataCs.LineNum4Confirm = e.SetNumber;
            needTransform1_2DataCs.PlaneType4Confirm = e.SetModel;
            needTransform1_2DataCs.guide_line_y = e.SetGuideLiney;
            transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
            OnBnClickedWalkset_std(laser_class);
            backgroundWorker.RunWorkerAsync();
        }

        private void button_WalkTest_Click(object sender, EventArgs1 e)//走不测试
        {
            //CoolDown();
            WPF2struct();
            needTransform1_2DataCs.LineNum4Confirm = e.SetNumber;
            needTransform1_2DataCs.PlaneType4Confirm = e.SetModel;
            needTransform1_2DataCs.guide_line_y = e.SetGuideLiney;
            transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
            OnBnClickedWalktest_std(laser_class);
            backgroundWorker.RunWorkerAsync();
        }

        private void button_WalkStop_Click(object sender, EventArgs1 e)//停止
        {
            //backgroundWorker.CancelAsync();
            OnBnClickedBtnClear_std(laser_class);
            //CoolDownOut();
            //FreshUI();

        }
        private void button_WalkConfirm_Click(object sender, EventArgs1 e)//确认（计算引导线）
        {
            OnBnClickedButtonConfirmwalkset_std(laser_class);
            FreshUI();
        }
        private void button_WalkDelete_Click(object sender, EventArgs1 e)//清空
        {
            OnBnClickedButtonClearwalkset_std(laser_class);
        }
        private void button_WalkNum_Click(object sender, EventArgs1 e)//编号【需要弹窗】
        {
            //WPF2Cpp();
            //弹窗
            Numb num = new Numb();
            num.NumcharForm += new Numb.NumCharEventHandler(Numb_NumChar);
            if (needTransform1_2DataCs.str_guidenum != null)
            {
                num.drpGuide.SelectedValue = needTransform1_2DataCs.str_guidenum;
            }
            if (needTransform1_2DataCs.str_stopnum != null)
            {
                num.drpStop.SelectedValue = needTransform1_2DataCs.str_stopnum;
            }
            num.Owner = this;
            num.WindowStartupLocation = WindowStartupLocation.CenterScreen;
            num.Show();
            num.Activate();
            transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
        }
        private void button_ModelGuide_Click(object sender, EventArgs1 e)//由机型读取引导线
        {
            WPF2struct();
            needTransform1_2DataCs.LineNum4Confirm = e.SetNumber;
            needTransform1_2DataCs.PlaneType4Confirm = e.SetModel;
            needTransform1_2DataCs.guide_line_y = e.SetGuideLiney;
            transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
            DrawingBackup2d.Clear();
            OnBnClickedButtonReadtype_std(laser_class);
            FreshUI();
        }
        private void button_NumberGuide_Click(object sender, EventArgs1 e)//由编号读取引导线
        {
            WPF2struct();
            needTransform1_2DataCs.LineNum4Confirm = e.SetNumber;
            needTransform1_2DataCs.PlaneType4Confirm = e.SetModel;
            needTransform1_2DataCs.guide_line_y = e.SetGuideLiney;
            transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
            DrawingBackup2d.Clear();
            OnBnClickedButtonReadlinenum_std(laser_class);

            FreshUI();
        }
        private void button_TestConfirmPoint_Click(object sender, EventArgs1 e)//验证停止点
        {
            WPF2struct();
            needTransform1_2DataCs.LineNum4Confirm = e.SetNumber;
            needTransform1_2DataCs.PlaneType4Confirm = e.SetModel;
            needTransform1_2DataCs.guide_line_y = e.SetGuideLiney;
            transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
            OnBnClickedButtonComfirmtype_std(laser_class);
            FreshUI();
        }
        private void button_ReadRect_Click(object sender, RoutedEventArgs e)//读取边框
        {
            OnBnClickedButtonReadedge_std(laser_class);
            FreshUI();
        }
        private void button_LeftMove_Click(object sender, EventArgs1 e)
        {
            OnBnClickedButtonLeft_std(laser_class);
        }
        private void button_RightMove_Click(object sender, EventArgs1 e)
        {
            OnBnClickedButtonRight_std(laser_class);
        }
        private void button_DownMove_Click(object sender, EventArgs1 e)
        {
            OnBnClickedButtonDown_std(laser_class);
            FreshUI();
        }
        private void button_UpMove_Click(object sender, EventArgs1 e)
        {
            OnBnClickedButtonUp_std(laser_class);
            FreshUI();
        }
        private void button_PointMeasure_Click(object sender, RoutedEventArgs e)//单点测量
        {
            mid_num.Content = "0";
            Thread PointMeasureThread = new Thread(new ThreadStart(() =>
                {
            OnBnClickedButtonLocate_std(laser_class);
           FreshUI();
                }));
            PointMeasureThread.Start();
        }
        private void button_OpenRed_Click(object sender, RoutedEventArgs e)
        {
            OnBnClickedButtonRedlaseron_std(laser_class);
        }//打开红色激光
        private void button_CloseRed_Click(object sender, RoutedEventArgs e)
        {
            OnBnClickedButtonRedlaseroff_std(laser_class);
        }//关闭红色激光
        private void button_WindowScan_Click(object sender, RoutedEventArgs e)//全屏扫描
        {
            //button_WalkTest.IsEnabled = false;
            //CoolDown();
            OnBnClickedButtonScanall_std(laser_class);
            backgroundWorker.RunWorkerAsync();
            //FreshUI();
            //Thread t = new Thread(FreshUI);
            //t.Start();
        }
        private void button_ConfirmRect_Click(object sender, EventArgs1 e)//确定边框
        {
            Thread confirmRectThread = new Thread(new ThreadStart(() =>
                {
            OnBnClickedButtonEdge_std(laser_class);
            FreshUI();
                }));
            confirmRectThread.Start();
        }
        private void button_Set0_Click(object sender, EventArgs1 e)
        {
            OnBnClickedButtonSetzero_std(laser_class);
            FreshUI();
        }
        private void button_Back0_Click(object sender, EventArgs1 e)
        {
            OnBnClickedButtonReset_std(laser_class);
            FreshUI();
        }
        private void button_Back000_Click(object sender, RoutedEventArgs e)
        {
            OnBnClickedButtonReset_std(laser_class);
            FreshUI();
        }
        private void button_2dScan_Click(object sender, EventArgs1 e)//二维扫描
        {
            WPF2struct();
            needTransform1_2DataCs.LineNum4Confirm = e.SetNumber;
            needTransform1_2DataCs.PlaneType4Confirm = e.SetModel;
            needTransform1_2DataCs.guide_line_y = e.SetGuideLiney;
            transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
            DrawingBackup2d.Clear();
            OnBnClickedButtonTwod_std(laser_class);
            FreshUI();
        }
        private void button_2dScan000_Click(object sender, RoutedEventArgs e)//二维扫描
        {
            WPF2Cpp();
            DrawingBackup2d.Clear();
            OnBnClickedButtonTwod_std(laser_class);
            FreshUI();
        }
        private void button_UpMove10_Click(object sender, EventArgs1 e)
        {
            OnBnClickedButtonUpten_std(laser_class);
            FreshUI();
        }
        private void button_LeftMove10_Click(object sender, EventArgs1 e)
        {
            OnBnClickedButtonLeftten_std(laser_class);
        }
        private void button_DownMove10_Click(object sender, EventArgs1 e)
        {
            OnBnClickedButtonDownten_std(laser_class);
            FreshUI();
        }
        private void button_RightMove10_Click(object sender, EventArgs1 e)
        {
            OnBnClickedButtonRightten_std(laser_class);
        }
        private void button_AutoCalibration_Click(object sender, EventArgs1 e)//水平标定
        {
            //CoolDown();
            OnBnClickedButtonHrzcalibration_std(laser_class);
            backgroundWorker.RunWorkerAsync();
            //FreshUI();
        }
        private void button_VerticalCalibration_Click(object sender, EventArgs1 e)//垂直标定
        {
            //Thread calibrationThread = new Thread(new ThreadStart(() =>
            //    {
            //        OnBnClickedButtonCalibration_std(laser_class);
            //        FreshUI();
            //    }));
            //calibrationThread.Start();
            //CoolDown();
            OnBnClickedButtonCalibration_std(laser_class);
            backgroundWorker.RunWorkerAsync();

        }
        private void Stop_Y_Changed(object sender, EventArgs1 e)
        {
            needTransform1_2DataCs.guide_line_y = Convert.ToInt32(e.SetGuideLiney);
            transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
        }
        private void button_NextStep1_Click(object sender, RoutedEventArgs e)
        {
            Set set = new Set();
            set.Event1 += new Set.PassBetween1Handler(button_2dScan_Click);
            set.Event2 += new Set.PassBetween2Handler(button_Back0_Click);
            set.Event3 += new Set.PassBetween3Handler(button_Set0_Click);
            set.Event4 += new Set.PassBetween4Handler(button_LeftMove_Click);
            set.Event5 += new Set.PassBetween5Handler(button_RightMove_Click);
            set.Event6 += new Set.PassBetween6Handler(button_UpMove_Click);
            set.Event7 += new Set.PassBetween7Handler(button_DownMove_Click);
            set.Event8 += new Set.PassBetween8Handler(button_LeftMove10_Click);
            set.Event9 += new Set.PassBetween9Handler(button_RightMove10_Click);
            set.Event10 += new Set.PassBetween10Handler(button_UpMove10_Click);
            set.Event11 += new Set.PassBetween11Handler(button_DownMove10_Click);
            set.Event12 += new Set.PassBetween12Handler(button_AutoCalibration_Click);
            set.Event13 += new Set.PassBetween13Handler(button_VerticalCalibration_Click);
            set.Event14 += new Set.PassBetween14Handler(button_ConfirmRect_Click);
            set.Event15 += new Set.PassBetween15Handler(button_WalkSet_Click);
            set.Event16 += new Set.PassBetween16Handler(button_WalkTest_Click);
            set.Event17 += new Set.PassBetween17Handler(button_WalkStop_Click);
            set.Event18 += new Set.PassBetween18Handler(button_WalkConfirm_Click);
            set.Event19 += new Set.PassBetween19Handler(button_WalkDelete_Click);
            set.Event20 += new Set.PassBetween20Handler(button_WalkNum_Click);
            set.Event21 += new Set.PassBetween21Handler(Stop_Y_Changed);
            set.Event22 += new Set.PassBetween22Handler(button_ModelGuide_Click);
            set.Event23 += new Set.PassBetween23Handler(button_NumberGuide_Click);
            set.Event24 += new Set.PassBetween24Handler(button_TestConfirmPoint_Click);
            
            //num.NumcharForm += new Numb.NumCharEventHandler(Numb_NumChar);
            set.Model_GuideLine.Text = needTransform1_2DataCs.PlaneType4Confirm;
            set.Number_GuideLine.Text = needTransform1_2DataCs.LineNum4Confirm;
            set.Stop_Y.Text = needTransform1_2DataCs.guide_line_y.ToString();
            set.Owner = this;
            set.WindowStartupLocation = WindowStartupLocation.CenterScreen;
            set.Show();
            set.Activate();
            transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
            /*
            if (Grid2.Visibility == Visibility.Collapsed && Grid3.Visibility == Visibility.Collapsed
                && Grid4.Visibility == Visibility.Collapsed && MastermodKB == false)
            {
                Grid2.Visibility = Visibility.Visible;
            }
            */
        }
        private void Numb_NumChar(object sender, NumCharEventArgs e)
        {
            needTransform1_2DataCs.str_guidenum = e.Guide;
            needTransform1_2DataCs.str_stopnum = e.Stop;
        }
        private void Image2dBox_ImageFailed(object sender, ExceptionRoutedEventArgs e)
        {

        }
        private void button_MasterMod_Click(object sender, RoutedEventArgs e)
        {
            MastermodKB = !MastermodKB;
            
            if (MastermodKB == true)
            {
                Set set = new Set();
                set.Event1 += new Set.PassBetween1Handler(button_2dScan_Click);
                set.Event2 += new Set.PassBetween2Handler(button_Back0_Click);
                set.Event3 += new Set.PassBetween3Handler(button_Set0_Click);
                set.Event4 += new Set.PassBetween4Handler(button_LeftMove_Click);
                set.Event5 += new Set.PassBetween5Handler(button_RightMove_Click);
                set.Event6 += new Set.PassBetween6Handler(button_UpMove_Click);
                set.Event7 += new Set.PassBetween7Handler(button_DownMove_Click);
                set.Event8 += new Set.PassBetween8Handler(button_LeftMove10_Click);
                set.Event9 += new Set.PassBetween9Handler(button_RightMove10_Click);
                set.Event10 += new Set.PassBetween10Handler(button_UpMove10_Click);
                set.Event11 += new Set.PassBetween11Handler(button_DownMove10_Click);
                set.Event12 += new Set.PassBetween12Handler(button_AutoCalibration_Click);
                set.Event13 += new Set.PassBetween13Handler(button_VerticalCalibration_Click);
                set.Event14 += new Set.PassBetween14Handler(button_ConfirmRect_Click);
                set.Event15 += new Set.PassBetween15Handler(button_WalkSet_Click);
                set.Event16 += new Set.PassBetween16Handler(button_WalkTest_Click);
                set.Event17 += new Set.PassBetween17Handler(button_WalkStop_Click);
                set.Event18 += new Set.PassBetween18Handler(button_WalkConfirm_Click);
                set.Event19 += new Set.PassBetween19Handler(button_WalkDelete_Click);
                set.Event20 += new Set.PassBetween20Handler(button_WalkNum_Click);
                set.Event21 += new Set.PassBetween21Handler(Stop_Y_Changed);
                set.Event22 += new Set.PassBetween22Handler(button_ModelGuide_Click);
                set.Event23 += new Set.PassBetween23Handler(button_NumberGuide_Click);
                set.Event24 += new Set.PassBetween24Handler(button_TestConfirmPoint_Click);

                //num.NumcharForm += new Numb.NumCharEventHandler(Numb_NumChar);
                set.Model_GuideLine.Text = needTransform1_2DataCs.PlaneType4Confirm;
                set.Number_GuideLine.Text = needTransform1_2DataCs.LineNum4Confirm;
                set.Stop_Y.Text = needTransform1_2DataCs.guide_line_y.ToString();
                set.Owner = this;
                set.WindowStartupLocation = WindowStartupLocation.CenterScreen;
                set.Show();
                set.Activate();
                transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
                Grid6.Visibility = Visibility.Visible;
            }
            else if (MastermodKB == false)
            {
                Set set = new Set();
                foreach (Window win in App.Current.Windows)
                {
                    if (win.GetType() == set.GetType())
                    {
                        win.Close();
                    }
                }
                Grid6.Visibility = Visibility.Collapsed;
            }
            
        }
        private void button_Save_Click(object sender, RoutedEventArgs e)//保存
        {
            transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
            OnBnClickedSave_std(laser_class);
        }
        private void button_Esc_Click(object sender, RoutedEventArgs e)//退出【修改】不用DestoryLaser了
        {
            //timer.start();
            OnBnClickedButtonRedlaseroff_std(laser_class);
            if (TimerCon!=null) TimerCon(this, new TimerEventArgs());
            this.Close();
        }
        private void button_OffLineTest_Click(object sender, RoutedEventArgs e)
        {
            OnBnClickedButtonOffLineTest_std(laser_class);
        }
        private void button_ZoomIn_Click(object sender, RoutedEventArgs e)//放大按钮
        {
            nowDrawing2d = MouseStatus.ZoomIn;
            System.Windows.Resources.StreamResourceInfo info = Application.GetResourceStream(new Uri("/CsharpConfig;component/ZoomIn.ani", UriKind.Relative));
            Image2dBox.Cursor = new System.Windows.Input.Cursor(info.Stream);
        }
        private void button_ZoomOut_Click(object sender, RoutedEventArgs e)//缩回按钮
        {
            nowDrawing2d = MouseStatus.ZoomOut;
            System.Windows.Resources.StreamResourceInfo info = Application.GetResourceStream(new Uri("/CsharpConfig;component/ZoomOut.ani", UriKind.Relative));
            Image2dBox.Cursor = new System.Windows.Input.Cursor(info.Stream);
        }
        private void button_Back0_Copy_Click(object sender, RoutedEventArgs e)
        {
            /*
            int step = 56;
            int need10 = step / 10, need1 = step % 10;
            for (int i = 0; i < need10; i++)
            {
                button_RightMove10_Click(this, new RoutedEventArgs());
                Thread.Sleep(20);
            }
            for (int i = 0; i < need1; i++)
            {
                button_RightMove_Click(this, new RoutedEventArgs());
                Thread.Sleep(20);
            }
            */
        }
        private void button_GuideLineMove_Click(object sender, RoutedEventArgs e)
        {
            if (TextBox_GuideLineMove.Text != "")
            {
                needTransform1_2DataCs.offsetforline = Convert.ToInt32(TextBox_GuideLineMove.Text);
                TextBox_GuideLineMove.Text = "0";
                transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
                OnBnClickedButtonGuideLineMove_std(laser_class);
                needTransform1_2DataCs.guide_line_x += Convert.ToInt32(needTransform1_2DataCs.offsetforline);
                needTransform1_2DataCs.guide_line_b = needTransform1_2DataCs.guide_line_y - needTransform1_2DataCs.guide_line_x * needTransform1_2DataCs.guide_line_k;
                renderi = 1;
                Axis_x_mid = needTransform1_2DataCs.guide_line_x;
                Axis_y_mid = 40000;
                NeedFreshK();
            }
        }
        private void button_StopLineMove_Click(object sender, RoutedEventArgs e)
        {
            if (TextBox_StopLineMove.Text != "")
            {
                needTransform1_2DataCs.offsetforline = Convert.ToInt32(TextBox_StopLineMove.Text);
                TextBox_StopLineMove.Text = "0";
                transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
                OnBnClickedButtonStopLineMove_std(laser_class);
                needTransform1_2DataCs.guide_line_y += Convert.ToInt32(needTransform1_2DataCs.offsetforline);
                Axis_x_mid = needTransform1_2DataCs.guide_line_x;
                foreach (var shape in DrawingBackup2d)
                {
                    if (shape.point1.X == 0 && shape.point1.Y == 0) continue;
                    switch (shape.type)
                    {
                        case MouseStatus.DrawingLine:
                            if (shape.point1.X == 177)
                            {
                                shape.point1.Y = 437 - needTransform1_2DataCs.guide_line_y * 35 / 8000;
                            }
                            else
                            {
                                shape.point1.Y = 437 - needTransform1_2DataCs.guide_line_y * 35 / 8000;
                                shape.point2.Y = 437 - needTransform1_2DataCs.guide_line_y * 35 / 8000;
                            }
                            break;
                        default:
                            break;
                    }
                }
                NeedFreshK();
            }
        }
        private void button_StopLineMoveSingle_Click(object sender, RoutedEventArgs e)
        {
            //needTransform1_2DataCs.guide_line_x = Convert.ToInt32(TextBox_GuideLineMove.Text);
            //transform1_2Struct_std(laser_class, ref needTransform1_2DataCs, true);
            //OnBnClickedButtonStopLineMoveSingle_std(laser_class);
        }
        #endregion
        #region 刷新机制
        void Axis2dScan(int origin_y)//2维扫描模式
        {
            DrawingVisual drawingVisual = new DrawingVisual();
            var rtbitmap = new RenderTargetBitmap(writeableSource2d.PixelWidth,
    writeableSource2d.PixelHeight, writeableSource2d.DpiX, writeableSource2d.DpiY,
    PixelFormats.Default);
            int temp5 = (int)(40000 / renderi) + origin_y;
            FormattedText text1 = new FormattedText(temp5.ToString(),
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 10, Brushes.Black);
            temp5 = (int)(30000 / renderi) + origin_y;
            FormattedText text2 = new FormattedText(temp5.ToString(),
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 10, Brushes.Black);
            temp5 = (int)(20000 / renderi) + origin_y;
            FormattedText text3 = new FormattedText(temp5.ToString(),
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 10, Brushes.Black);
            temp5 = (int)(10000 / renderi) + origin_y;
            FormattedText text4 = new FormattedText(temp5.ToString(),
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 10, Brushes.Black);
            temp5 = origin_y;
            FormattedText text5 = new FormattedText(temp5.ToString(),
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 10, Brushes.Black);
            temp5 = (int)(-10000 / renderi) + origin_y;
            FormattedText text6 = new FormattedText(temp5.ToString(),
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 10, Brushes.Black);
            temp5 = (int)(-20000 / renderi) + origin_y;
            FormattedText text7 = new FormattedText(temp5.ToString(),
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 10, Brushes.Black);
            temp5 = (int)(-30000 / renderi) + origin_y;
            FormattedText text8 = new FormattedText(temp5.ToString(),
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 10, Brushes.Black);
            temp5 = (int)(-40000 / renderi) + origin_y;
            FormattedText text9 = new FormattedText(temp5.ToString(),
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 10, Brushes.Black);

            FormattedText text11 = new FormattedText("-8",
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 20, Brushes.Black);
            FormattedText text12 = new FormattedText("-6",
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 20, Brushes.Black);
            FormattedText text13 = new FormattedText("-4",
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 20, Brushes.Black);
            FormattedText text14 = new FormattedText("-2",
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 20, Brushes.Black);
            FormattedText text15 = new FormattedText("0",
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 20, Brushes.Black);
            FormattedText text16 = new FormattedText("2",
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 20, Brushes.Black);
            FormattedText text17 = new FormattedText("4",
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 20, Brushes.Black);
            using (var dc = drawingVisual.RenderOpen())
            {
                dc.DrawImage(writeableSource2d, new Rect(0, 0, writeableSource2d.Width, writeableSource2d.Height));
                //dc.DrawRectangle(Brushes.Transparent, new Pen(Brushes.Black, 7), new Rect(10, 10, 50 + text.Width, 50 + text.Height));
                dc.DrawText(text1, new Point(2, 84));
                dc.DrawText(text2, new Point(2, 128));
                dc.DrawText(text3, new Point(2, 172));
                dc.DrawText(text4, new Point(2, 216));
                dc.DrawText(text5, new Point(2, 260));
                dc.DrawText(text6, new Point(2, 304));
                dc.DrawText(text7, new Point(2, 348));
                dc.DrawText(text8, new Point(2, 392));
                dc.DrawText(text9, new Point(2, 435));
                if (renderi == 1)
                {
                    dc.DrawText(text11, new Point(49, 440));
                    dc.DrawText(text12, new Point(80, 440));
                    dc.DrawText(text13, new Point(112, 440));
                    dc.DrawText(text14, new Point(148, 440));
                    dc.DrawText(text15, new Point(181, 440));
                    dc.DrawText(text16, new Point(214, 440));
                    dc.DrawText(text17, new Point(248, 440));
                }
                else { }
            }
            rtbitmap.Render(drawingVisual);
            Image2dBox.Source = rtbitmap;
        }
        void AxisNormal(int origin_x,int origin_y)//画两个坐标轴的刻度
        {
            DrawingVisual drawingVisual = new DrawingVisual();
            var rtbitmap = new RenderTargetBitmap(writeableSource2d.PixelWidth,
    writeableSource2d.PixelHeight, writeableSource2d.DpiX, writeableSource2d.DpiY,
    PixelFormats.Default);
            int temp5 = (int)(40000 / renderi) + origin_y;
            FormattedText text1 = new FormattedText(temp5.ToString(),
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 10, Brushes.Black);

            temp5 = (int)(30000 / renderi) + origin_y;
            FormattedText text2 = new FormattedText(temp5.ToString(),
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 10, Brushes.Black);

            temp5 = (int)(20000 / renderi) + origin_y;
            FormattedText text3 = new FormattedText(temp5.ToString(),
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 10, Brushes.Black);

            temp5 = (int)(10000 / renderi) + origin_y;
            FormattedText text4 = new FormattedText(temp5.ToString(),
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 10, Brushes.Black);

            temp5 = origin_y;
            FormattedText text5 = new FormattedText(temp5.ToString(),
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 10, Brushes.Black);

            temp5 = (int)(-10000 / renderi) + origin_y;
            FormattedText text6 = new FormattedText(temp5.ToString(),
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 10, Brushes.Black);

            temp5 = (int)(-20000 / renderi) + origin_y;
            FormattedText text7 = new FormattedText(temp5.ToString(),
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 10, Brushes.Black);

            temp5 = (int)(-30000 / renderi) + origin_y;
            FormattedText text8 = new FormattedText(temp5.ToString(),
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 10, Brushes.Black);

            temp5 = (int)(-40000 / renderi) + origin_y;
            FormattedText text9 = new FormattedText(temp5.ToString(),
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 10, Brushes.Black);
            int temp4 = (int)((64 - 177) * 10000 / (145 * renderi)) + origin_x;
            FormattedText text11 = new FormattedText(temp4.ToString(),
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 10, Brushes.Black);

            temp4 = (int)((96 - 177) * 10000 / (145 * renderi)) + origin_x;
            FormattedText text12 = new FormattedText(temp4.ToString(),
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 10, Brushes.Black);

            temp4 = (int)((128 - 177) * 10000 / (145 * renderi)) + origin_x;
            FormattedText text13 = new FormattedText(temp4.ToString(),
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 10, Brushes.Black);

            temp4 = (int)((160 - 177) * 10000 / (145 * renderi)) + origin_x;
            FormattedText text14 = new FormattedText(temp4.ToString(),
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 10, Brushes.Black);

            temp4 = (int)((193 - 177) * 10000 / (145 * renderi)) + origin_x;
            FormattedText text15 = new FormattedText(temp4.ToString(),
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 10, Brushes.Black);

            temp4 = (int)((225 - 177) * 10000 / (145 * renderi)) + origin_x;
            FormattedText text16 = new FormattedText(temp4.ToString(),
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 10, Brushes.Black);

            temp4 = (int)((257 - 177) * 10000 / (145 * renderi)) + origin_x;
            FormattedText text17 = new FormattedText(temp4.ToString(),
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 10, Brushes.Black);

            temp4 = (int)((290 - 177) * 10000 / (145 * renderi)) + origin_x;
            FormattedText text18 = new FormattedText(temp4.ToString(),
                System.Globalization.CultureInfo.CurrentCulture,
                System.Windows.FlowDirection.LeftToRight,
                new Typeface(SystemFonts.MessageFontFamily,
                    FontStyles.Normal, FontWeights.Thin,
                    FontStretches.Normal), 10, Brushes.Black);
            FormattedText text21 = new FormattedText("零点",
    System.Globalization.CultureInfo.CurrentCulture,
    System.Windows.FlowDirection.LeftToRight,
    new Typeface(SystemFonts.MessageFontFamily,
        FontStyles.Normal, FontWeights.Thin,
        FontStretches.Normal), 10, Brushes.Black);
            int Xtemp = 177 - (int)(Axis_x_mid * renderi * 145 / 10000);
            int Ytemp = 262 + (int)(Axis_y_mid * renderi * 176 / 40000);
            using (var dc = drawingVisual.RenderOpen())
            {
                dc.DrawImage(writeableSource2d, new Rect(0, 0, writeableSource2d.Width, writeableSource2d.Height));
                //dc.DrawRectangle(Brushes.Transparent, new Pen(Brushes.Black, 7), new Rect(10, 10, 50 + text.Width, 50 + text.Height));
                dc.DrawText(text1, new Point(2, 84));
                dc.DrawText(text2, new Point(2, 128));
                dc.DrawText(text3, new Point(2, 172));
                dc.DrawText(text4, new Point(2, 216));
                dc.DrawText(text5, new Point(2, 260));
                dc.DrawText(text6, new Point(2, 304));
                dc.DrawText(text7, new Point(2, 348));
                dc.DrawText(text8, new Point(2, 392));
                dc.DrawText(text9, new Point(2, 435));
                dc.DrawText(text11, new Point(49, 440));
                dc.DrawText(text12, new Point(80, 440));
                dc.DrawText(text13, new Point(112, 440));
                dc.DrawText(text14, new Point(148, 440));
                dc.DrawText(text15, new Point(181, 440));
                dc.DrawText(text16, new Point(214, 440));
                dc.DrawText(text17, new Point(248, 440));
                dc.DrawText(text18, new Point(279, 440));
                dc.DrawText(text21, new Point(Xtemp - 7, Ytemp+12));
                dc.DrawLine(new Pen(Brushes.DarkOrange, 2), new Point(Xtemp - 4, Ytemp - 4), new Point(Xtemp + 4, Ytemp+4));
                dc.DrawLine(new Pen(Brushes.DarkOrange, 2), new Point(Xtemp - 4, Ytemp + 4), new Point(Xtemp + 4, Ytemp-4));
            }
            rtbitmap.Render(drawingVisual);
            Image2dBox.Source = rtbitmap;
        }
        void AxisHrzcalibration()//水平标定模式
        {

        }
        #endregion
        private void Install_height_KeyDown(object sender, KeyEventArgs e)
        {
            TextBox txt = sender as TextBox;

            //屏蔽非法按键
            if ((e.Key >= Key.NumPad0 && e.Key <= Key.NumPad9) || e.Key == Key.Decimal)
            {
                if (txt.Text.Contains(".") && e.Key == Key.Decimal)
                {
                    e.Handled = true;
                    return;
                }
                e.Handled = false;
            }
            else if (((e.Key >= Key.D0 && e.Key <= Key.D9) || e.Key == Key.OemPeriod) && e.KeyboardDevice.Modifiers != ModifierKeys.Shift)
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
    }
}
