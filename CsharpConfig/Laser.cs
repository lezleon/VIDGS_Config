using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using System.IO.Ports;      //SerialPort
using System.Threading;
using System.IO;            //TXT

namespace Config
{
    public partial class Laser : Form
    {

        #region 全局变量
        SerialPort serialPort1;     //定义 SerialPort对象
        CLaserFunc LaserFunc;       //定义 激光功能实现函数
        CShareData ShareData;       //定义 共享数据

        Thread ThreadRefresh;       //定义 刷新界面的线程     
        //Thread ThreadProcess;       //定义 后台运行的线程  
        #endregion

        #region Windows 窗体设计
        public Laser()
        {
            InitializeComponent();
            InitializeForm();
        }
        private void InitializeForm()
        {
            this.StartPosition = FormStartPosition.WindowsDefaultLocation;
            this.Text = "激光配置";
            this.BackColor = Color.LightSteelBlue;
            //this.Size = new Size(800, 600);

            //string IconFileName = Application.StartupPath + @".\..\..\config.ico";
            //this.Icon = new Icon(IconFileName);      
        }

        private void FormReady()    //窗体打开前，最后的初始化函数
        {
            LaserFunc = new CLaserFunc();   //类 激光功能实现函数类
            ShareData = new CShareData();   //类 激光功能实现函数类

            ThreadRefresh = new Thread(new ThreadStart(ThreadRefreshMain));  //初始化 Thread 类的新实例
            ThreadRefresh.Start();

            //ThreadParameter parameter = new ThreadParameter();    //线程传入参数
            //parameter.serialPort1 = serialPort1;
            //parameter.Status = Status;
            //parameter.pBStatus = pBStatus;
            //ThreadProcess = new Thread(new ParameterizedThreadStart(ThreadProcessMain));  //初始化 Thread 类的新实例
            //ThreadProcess.Start(parameter);  //启动线程，传递参数



            LaserFunc.OpenCloseCOMFunc(serialPort1, Status, pBStatus, OpenCloseCOM);    //打开串口

            ReadConfigData();   //加载配置文件
            laInstallHeight.Value = CShareData.InstallHeight;

            CShareData.OperateRecordStr += "初始化完毕\r\n";

        }
        private void Laser_Load(object sender, EventArgs e)
        {                
            // 
            // SerialPort
            // 
            serialPort1 = new SerialPort("COM1");
            serialPort1.BaudRate = 115200;//波特率
            serialPort1.Parity = Parity.None;//无奇偶校验位
            serialPort1.StopBits = StopBits.One;//1个停止位
            serialPort1.ReceivedBytesThreshold = 1;//设置 DataReceived 事件发生前内部输入缓冲区中的字节数
            serialPort1.ReadBufferSize = 4096;
            serialPort1.WriteBufferSize = 2048;
            serialPort1.DataReceived += new SerialDataReceivedEventHandler(serialPort1_DataReceived);//DataReceived事件委托
            // 
            // Status
            // 
            //this.Status = new Label();
            this.Status.Location = new System.Drawing.Point(12, this.Height - 55);
            this.Status.Name = "Status";
            this.Status.Size = new System.Drawing.Size(55, 12);
            this.Status.Text = "欢迎使用！";
            // 
            // pBStatus
            // 
            //this.pBStatus = new ProgressBar();
            this.pBStatus.Location = new System.Drawing.Point(this.Width - 420, this.Height - 55);
            this.pBStatus.Name = "pBStatus";
            this.pBStatus.Size = new System.Drawing.Size(400, 12);

            // 
            // OpenCloseCOM
            // 
            this.OpenCloseCOM.Text = "打开串口";
            this.OpenCloseCOM.Size = new Size(80, 35);
            this.OpenCloseCOM.Location = new Point(12,12);
            this.OpenCloseCOM.Font = new Font("宋体",12);            
            // 
            // Move
            // 
            this.MoveUpStep.Minimum = 1;
            this.MoveDownStep.Minimum = 1;
            this.MoveLeftStep.Minimum = 1;
            this.MoveRightStep.Minimum = 1;
            this.MoveUpStep.Maximum = 4095;
            this.MoveDownStep.Maximum = 4095;
            this.MoveLeftStep.Maximum = 4095;
            this.MoveRightStep.Maximum = 4095;
            

            FormReady();    //窗体打开前，最后的初始化函数
        }

        private void Laser_FormClosing(object sender, FormClosingEventArgs e)
        {
            try
            {
                WriteConfigData();  //保存配置文件
                LaserFunc.SaveOperateRecord();  //保存操作记录

                if (ThreadRefresh.IsAlive)      //stop the thread
                    ThreadRefresh.Abort();
            }
            catch
            {
                MessageBox.Show("确定要关闭窗口吗？", "提示", MessageBoxButtons.OKCancel, MessageBoxIcon.Warning);
            }
        }
        #endregion

        #region ThreadRefreshMain
        int cnt = 0;
        private void ThreadRefreshMain()
        {
            while (true)
            {
                cnt++;
                Thread.Sleep(100);
                this.Invoke(new Action<Label, string>(LaserFunc.LabelUpdata), laRunning, cnt.ToString());
                Application.DoEvents();
            }

        }
        #endregion

        #region 安装高度
        private void laInstallHeight_ValueChanged(object sender, EventArgs e)
        {
            Int16 Height = Convert.ToInt16(laInstallHeight.Value);
            if ((Height > 100) && (Height < 10000))
            {
                CShareData.InstallHeight = Height;
            }
        }
        #endregion

        #region SerialPort
        private void OpenCloseCOM_Click(object sender, EventArgs e)
        {
            LaserFunc.OpenCloseCOMFunc(serialPort1,Status,pBStatus,OpenCloseCOM);
        }

        
        //DataReceived事件委托方法
        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
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

                            CShareData.ReadDataLen = 0;
                            CShareData.spSem.WaitOne(0);        //Clean the Semaphore
                            //Console.WriteLine("package start.");
                        }

                        //CShareData.ReadBytes = serialPort1.BytesToRead;
                        //serialPort1.Read(CShareData.spReceivedData, CShareData.spReceivedDataLen, CShareData.ReadBytes);
                        //CShareData.spReceivedDataLen += CShareData.ReadBytes;

                        CShareData.spReceivedDataLen += serialPort1.Read(CShareData.spReceivedData, CShareData.spReceivedDataLen, serialPort1.BytesToRead);
                        if (CShareData.spReceivedDataLen > 4) CShareData.ReadDataLen = 5 + (CShareData.spReceivedData[2] << 8) + CShareData.spReceivedData[3]; //计算数据长度，其实没必要一没次都计算
                        else return;
                        if (CShareData.ReadDataLen == 0 || CShareData.spReceivedDataLen == 0) return;

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

       
        #endregion

        #region XYShiftUpdata
        public void XYShiftUpdata()
        {
            XShiftShow.Text = CShareData.XShift.ToString();
            YShiftShow.Text = CShareData.YShift.ToString();
            XShiftShow.Update();
            YShiftShow.Update();
        }
        #endregion

        #region ConfigData配置文件
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
            MoveUpStep.Value = Convert.ToInt16(strs[1]);
            MoveDownStep.Value = Convert.ToInt16(strs[2]);
            MoveLeftStep.Value = Convert.ToInt16(strs[3]);
            MoveRightStep.Value = Convert.ToInt16(strs[4]);

            strs = ReadTXTStr[2].Split('\t');     //边框(水平、垂直)
            CShareData.XEdgeStepSave = Convert.ToInt16(strs[1]);
            CShareData.XProbSave = Convert.ToDouble(strs[2]);
            CShareData.YEdgeStepSave = Convert.ToInt16(strs[3]);
            CShareData.YProbSave = Convert.ToDouble(strs[4]);

            laXEdgeStep.Text = CShareData.XEdgeStepSave.ToString();
            laXProb.Text = CShareData.XProbSave.ToString();
            laYEdgeStep.Text = CShareData.YEdgeStepSave.ToString();
            laYProb.Text = CShareData.YProbSave.ToString();

            strs = ReadTXTStr[3].Split('\t');     //边框(水平、垂直)
            CShareData.InstallHeight = Convert.ToInt16(strs[1]);
        }

        private void WriteConfigData()
        {
            string str = "";
            str += "Config参数：";
            str += "\r\n";
            str += "移动步数(上下左右)：" + "\t" + MoveUpStep.Value.ToString() + "\t" + MoveDownStep.Value.ToString() + "\t" + MoveLeftStep.Value.ToString() + "\t" + MoveRightStep.Value.ToString();
            str += "\r\n";
            str += "确认边框(水平、垂直)：" + "\t" + CShareData.XEdgeStepSave.ToString() + "\t" + CShareData.XProbSave.ToString() + "\t" + CShareData.YEdgeStepSave.ToString() +"\t" + CShareData.YProbSave.ToString();
            str += "\r\n";
            str += "安装高度：" + "\t" + CShareData.InstallHeight.ToString();
            str += "\r\n";

            LaserFunc.WriteStrToTXT(ConfigDataPath, str);

            LaserFunc.LabelUpdata(Status, "\r\n成功保存配置文件！");
        }
        #endregion

        #region Z型、垂直扫描、三维扫描     按钮事件
        #region Z型扫描
        private void btnZScan2D_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status,"Z型扫描……");
            Int16 ScanPointNum = 109;
            Int32[,] ReceivedDistance = new Int32[2, ScanPointNum];     //读取水平扫描距离值
            Int16 XStartPos = Convert.ToInt16(0 - ((ScanPointNum - 1) >> 2));
            Int16 XEndPos = Convert.ToInt16((ScanPointNum - 1) >> 2);
            ReceivedDistance = LaserFunc.Z01ScanFunc(CShareData.ZScanDirection.Down, 5, -20, XStartPos, XEndPos, serialPort1, Status, pBStatus);

            if (CShareData.Z01ScanFlag)
            {
                XYShiftUpdata();
                LaserFunc.LabelUpdata(Status,"Z型扫描成功!");


                richTextBox1.Text = LaserFunc.ByteArrayToString(CShareData.spReceivedData, CShareData.spReceivedDataLen);
                DataLen.Text = CShareData.spReceivedDataLen.ToString();

                string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                string path = CShareData.StartPath + "Z型扫描" + TimeNow + ".txt";
                LaserFunc.SaveScanDistance(path, ReceivedDistance, ScanPointNum,2);
            }
        }
        #endregion

        #region 垂直扫描
        private void btnYScan2D_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status,"垂直扫描……");
            Int16 ScanPointNum = 109;
            Int32[,] ReceivedDistance = new Int32[2, ScanPointNum];     //读取水平扫描距离值
            Int16 YStartPos = Convert.ToInt16((ScanPointNum - 1) >> 2);
            Int16 YEndPos = Convert.ToInt16(0 - ((ScanPointNum - 1) >> 2));
            ReceivedDistance = LaserFunc.VerScanFunc(0, YStartPos, YEndPos, serialPort1, Status, null);   //调用水平扫描函数

            if (CShareData.VerScanFlag)
            {
                XYShiftUpdata();
                LaserFunc.LabelUpdata(Status,"垂直扫描成功!");

                richTextBox1.Text = LaserFunc.ByteArrayToString(CShareData.spReceivedData, CShareData.spReceivedDataLen);
                DataLen.Text = CShareData.spReceivedDataLen.ToString();

                string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                string path = CShareData.StartPath + "垂直扫描" + TimeNow + ".txt";
                LaserFunc.SaveScanDistance(path, ReceivedDistance, ScanPointNum,2);
            }
        }
        #endregion

        #region 三维扫描
        private void btnScan3D_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status, "三维扫描……");
            if (LaserFunc.ThreeDScanFunc(10, -35, serialPort1, Status, pBStatus))
            {
                LaserFunc.LabelUpdata(Status, "三维扫描成功");
            }
        }
        #endregion
        #endregion

        #region 红色激光    按钮事件
        private void OpenRedLaser_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status,"打开红色激光……");
            if (LaserFunc.RedLaser(CShareData.RedLaserValue.ON, serialPort1, null, null))
            {
                LaserFunc.LabelUpdata(Status,"成功打开红色激光！");
            }
            richTextBox1.Text = LaserFunc.ByteArrayToString(CShareData.spReceivedData, CShareData.spReceivedDataLen);
            DataLen.Text= CShareData.spReceivedDataLen.ToString();
        }

        private void CloseRedLaser_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status,"关闭红色激光……");
            if (LaserFunc.RedLaser(CShareData.RedLaserValue.OFF, serialPort1, Status, pBStatus))
            {
                LaserFunc.LabelUpdata(Status,"成功关闭红色激光！");
            }
            richTextBox1.Text = LaserFunc.ByteArrayToString(CShareData.spReceivedData, CShareData.spReceivedDataLen);
            DataLen.Text= CShareData.spReceivedDataLen.ToString();
        }
        #endregion

        #region 单点测距    按钮事件
        private void OnePointMeasure_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status,"单点测距……");
            double DistanceAVG = LaserFunc.OnePointMeasureFunc(1000, serialPort1, Status, null);
            if (DistanceAVG > 0)
            {
                OnePointMeasureValue.Text = Math.Round(DistanceAVG).ToString();
            }
            if (CShareData.OnePointDistanceFlag)    //保存数据到Txt中
            {
                string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                string path = CShareData.StartPath + "单点" + TimeNow + ".txt";
                LaserFunc.SaveOnePointDistance(path, CShareData.OnePointDistance, CShareData.OnePointDistanceLen);
            }
            richTextBox1.Text = LaserFunc.ByteArrayToString(CShareData.spReceivedData, CShareData.spReceivedDataLen);
            DataLen.Text= CShareData.spReceivedDataLen.ToString();
        }
        #endregion

        #region 二维扫描    按钮事件
        private void Scan2D_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status,"二维扫描……");
            Int16 ScanPointNum = 109;
            Int32[,] ReceivedDistance = new Int32[2, ScanPointNum];     //读取水平扫描距离值
            Int16 XStartPos = Convert.ToInt16(0 - ((ScanPointNum - 1) >> 2));
            Int16 XEndPos = Convert.ToInt16((ScanPointNum - 1) >> 2);            
            ReceivedDistance = LaserFunc.HrzScanFunc(Convert.ToInt16(CShareData.YShift), XStartPos, XEndPos, serialPort1, Status, null);   //调用水平扫描函数
            
            if (CShareData.HrzScanFlag)
            {
                XYShiftUpdata();
                LaserFunc.LabelUpdata(Status,"扫描成功!");

                richTextBox1.Text = LaserFunc.ByteArrayToString(CShareData.spReceivedData, CShareData.spReceivedDataLen);
                DataLen.Text = CShareData.spReceivedDataLen.ToString();

                string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                string path = CShareData.StartPath + "二维扫描" + TimeNow + ".txt";
                LaserFunc.SaveScanDistance(path, ReceivedDistance, ScanPointNum,2);
            }
        }
        #endregion

        #region 上下左右移动  按钮事件
        private void btnMoveUp_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status,"向上……");
            Int16 MoveStep = Convert.ToInt16(MoveUpStep.Value);
            if (LaserFunc.MoveFunc(CShareData.MoveDirection.Up, MoveStep, CShareData.SetZeroValue.UnSet, serialPort1, Status, pBStatus))
            {
                XYShiftUpdata();
                LaserFunc.LabelUpdata(Status,"定位成功！");
            }
            richTextBox1.Text = LaserFunc.ByteArrayToString(CShareData.spReceivedData, CShareData.spReceivedDataLen);
            DataLen.Text= CShareData.spReceivedDataLen.ToString();
        }

        private void btnMoveDown_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status,"向下……");
            Int16 MoveStep = Convert.ToInt16(MoveDownStep.Value);
            if (LaserFunc.MoveFunc(CShareData.MoveDirection.Down, MoveStep, CShareData.SetZeroValue.UnSet, serialPort1, Status, pBStatus))
            {
                XYShiftUpdata();
                LaserFunc.LabelUpdata(Status,"定位成功！");
            }
            richTextBox1.Text = LaserFunc.ByteArrayToString(CShareData.spReceivedData, CShareData.spReceivedDataLen);
            DataLen.Text= CShareData.spReceivedDataLen.ToString();
        }

        private void btnMoveLeft_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status,"向左……");
            Int16 MoveStep = Convert.ToInt16(MoveLeftStep.Value);
            if (LaserFunc.MoveFunc(CShareData.MoveDirection.Left, MoveStep, CShareData.SetZeroValue.UnSet, serialPort1, Status, pBStatus))
            {
                XYShiftUpdata();
                LaserFunc.LabelUpdata(Status,"定位成功！");
            }
            richTextBox1.Text = LaserFunc.ByteArrayToString(CShareData.spReceivedData, CShareData.spReceivedDataLen);
            DataLen.Text= CShareData.spReceivedDataLen.ToString();
        }

        private void btnMoveRight_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status,"向右……");
            Int16 MoveStep = Convert.ToInt16(MoveRightStep.Value);
            if (LaserFunc.MoveFunc(CShareData.MoveDirection.Right, MoveStep, CShareData.SetZeroValue.UnSet, serialPort1, Status, pBStatus))
            {
                XYShiftUpdata();
                LaserFunc.LabelUpdata(Status,"定位成功！");
            }
            richTextBox1.Text = LaserFunc.ByteArrayToString(CShareData.spReceivedData, CShareData.spReceivedDataLen);
            DataLen.Text= CShareData.spReceivedDataLen.ToString();
        }
        #endregion

        #region 回零、置零   按钮事件
        private void ReturnZero_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status,"回零……");
            if (LaserFunc.ReturnZeroFunc(serialPort1, Status, pBStatus))
            {
                XYShiftUpdata();
                LaserFunc.LabelUpdata(Status,"回零成功！");
            }
            richTextBox1.Text = LaserFunc.ByteArrayToString(CShareData.spReceivedData, CShareData.spReceivedDataLen);
            DataLen.Text= CShareData.spReceivedDataLen.ToString();
        }

        private void SetZero_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status,"置零……");
            if (LaserFunc.SetZeroFunc(serialPort1, Status, pBStatus))
            {
                XYShiftUpdata();
                LaserFunc.LabelUpdata(Status,"置零成功！");
            }
            richTextBox1.Text = LaserFunc.ByteArrayToString(CShareData.spReceivedData, CShareData.spReceivedDataLen);
            DataLen.Text= CShareData.spReceivedDataLen.ToString();
        }
        #endregion
        
        #region 刷新电机位置  按钮事件
        private void ShiftRefresh_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status,"刷新……");
            if (LaserFunc.ShiftRefreshFunc(serialPort1, Status, pBStatus))
            {
                XYShiftUpdata();
                LaserFunc.LabelUpdata(Status,"刷新成功！");
            }
            richTextBox1.Text = LaserFunc.ByteArrayToString(CShareData.spReceivedData, CShareData.spReceivedDataLen);
            DataLen.Text= CShareData.spReceivedDataLen.ToString();
        }
        #endregion
        
        #region 边框确认    按钮事件
        private void ConfirmEdgeX_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status, "确认边框……");
            if (LaserFunc.XConfirmEdgeFunc(serialPort1, Status, pBStatus))
            {
                laXEdgeStep.Text = CShareData.XEdgeStepSave.ToString();
                laXProb.Text = CShareData.XProbSave.ToString();
                XYShiftUpdata();
                LaserFunc.LabelUpdata(Status, "确认边框成功！");
            }
            else
            {
                laXEdgeStep.Text = "ER";
                laXProb.Text = "ER";
            }
            richTextBox1.Text = LaserFunc.ByteArrayToString(CShareData.spReceivedData, CShareData.spReceivedDataLen);
            DataLen.Text = CShareData.spReceivedDataLen.ToString();
        }

        private void ConfirmEdgeY_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status, "确认边框……");
            if (LaserFunc.YConfirmEdgeFunc(serialPort1, Status, pBStatus))
            {
                laYEdgeStep.Text = CShareData.YEdgeStepSave.ToString();
                laYProb.Text = CShareData.YProbSave.ToString();
                XYShiftUpdata();
                LaserFunc.LabelUpdata(Status, "确认边框成功！");
            }
            else
            {
                laYEdgeStep.Text = "ER";
                laYProb.Text = "ER";
            }
            richTextBox1.Text = LaserFunc.ByteArrayToString(CShareData.spReceivedData, CShareData.spReceivedDataLen);
            DataLen.Text = CShareData.spReceivedDataLen.ToString();
        }

        private void ConfirmEdgeXY_Click(object sender, EventArgs e)
        {
            int Result = LaserFunc.XYConfirmEdgeFunc(serialPort1, Status, null);
            switch (Result)
            {
                case 1:
                    laXEdgeStep.Text = CShareData.XEdgeStepSave.ToString();
                    laXProb.Text = CShareData.XProbSave.ToString();
                    laYEdgeStep.Text = CShareData.YEdgeStepSave.ToString();
                    laYProb.Text = CShareData.YProbSave.ToString();
                    break;
                case 2:
                    laXEdgeStep.Text = CShareData.XEdgeStepSave.ToString();
                    laXProb.Text = CShareData.XProbSave.ToString();
                    laYEdgeStep.Text = "ER";
                    laYProb.Text = "ER";
                    break;
                case 3:
                    laXEdgeStep.Text = "ER";
                    laXProb.Text = "ER";
                    laYEdgeStep.Text = CShareData.YEdgeStepSave.ToString();
                    laYProb.Text = CShareData.YProbSave.ToString();
                    break;
                default:
                    laXEdgeStep.Text = "ER";
                    laXProb.Text = "ER";
                    laYEdgeStep.Text = "ER";
                    laYProb.Text = "ER";
                    break;
            }
            XYShiftUpdata();
        }


        private void btnViewProb_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status, "查看概率……");
            if (LaserFunc.NTimeViewProbFunc(serialPort1, Status, pBStatus))
            //if (LaserFunc.ViewProbFunc(serialPort1, Status, pBStatus))
            {
                laViewProb.Text = CShareData.ViewProb.ToString();
                LaserFunc.LabelUpdata(Status, "查看概率成功！");
            }
            richTextBox1.Text = LaserFunc.ByteArrayToString(CShareData.spReceivedData, CShareData.spReceivedDataLen);
            DataLen.Text = CShareData.spReceivedDataLen.ToString();
        }
        #endregion

        #region 校准  按钮事件  
        private void btnAjustX_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status, "水平校准……");
            if (LaserFunc.XAdjustFunc(serialPort1, Status, pBStatus))
            {
                //回到零点并置位
                LaserFunc.MoveFunc(CShareData.MoveDirection.Left, CShareData.XEdgeStepSave, CShareData.SetZeroValue.Set, serialPort1, Status, null);
 

                laXAdjustStepError.Text = CShareData.XAdjustStepError.ToString();
                laXAdjustProb.Text = CShareData.XAdjustProb.ToString();
                XYShiftUpdata();
                LaserFunc.LabelUpdata(Status, "水平校准成功！");
            }
            else
            {
                laXAdjustStepError.Text = "ER";
                laXAdjustProb.Text = "ER";
            }
            LaserFunc.ReturnZeroFunc(serialPort1, Status, null);

            richTextBox1.Text = LaserFunc.ByteArrayToString(CShareData.spReceivedData, CShareData.spReceivedDataLen);
            DataLen.Text = CShareData.spReceivedDataLen.ToString();
        }

        private void btnAdjustY_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status, "垂直校准……");
            if (LaserFunc.YAdjustFunc(serialPort1, Status, pBStatus))
            {
                //回到零点并置位
                LaserFunc.MoveFunc(CShareData.MoveDirection.Up, CShareData.YEdgeStepSave, CShareData.SetZeroValue.Set, serialPort1, Status, null);

                laYAdjustStepError.Text = CShareData.YAdjustStepError.ToString();
                laYAdjustProb.Text = CShareData.YAdjustProb.ToString();
                XYShiftUpdata();
                LaserFunc.LabelUpdata(Status, "垂直校准成功！");
            }
            else
            {
                laYAdjustStepError.Text = "ER";
                laYAdjustProb.Text = "ER";
            }
            LaserFunc.ReturnZeroFunc(serialPort1, Status, null);

            richTextBox1.Text = LaserFunc.ByteArrayToString(CShareData.spReceivedData, CShareData.spReceivedDataLen);
            DataLen.Text = CShareData.spReceivedDataLen.ToString();
        }

        private void btnAdjustXY_Click(object sender, EventArgs e)
        {
            int Result = LaserFunc.XYAdjustFunc(serialPort1, Status, null);
            switch (Result)
            {
                case 1:
                    laXAdjustStepError.Text = CShareData.XAdjustStepError.ToString();
                    laXAdjustProb.Text = CShareData.XAdjustProb.ToString();
                    laYAdjustStepError.Text = CShareData.YAdjustStepError.ToString();
                    laYAdjustProb.Text = CShareData.YAdjustProb.ToString();
                    break;
                case 2:
                    laXAdjustStepError.Text = CShareData.XAdjustStepError.ToString();
                    laXAdjustProb.Text = CShareData.XAdjustProb.ToString();
                    laYAdjustStepError.Text = "ER";
                    laYAdjustProb.Text = "ER";
                    break;
                case 3:
                    laXAdjustStepError.Text = "ER";
                    laXAdjustProb.Text = "ER";
                    laYAdjustStepError.Text = CShareData.YAdjustStepError.ToString();
                    laYAdjustProb.Text = CShareData.YAdjustProb.ToString();
                    break;
                case 4:
                    laXAdjustStepError.Text = "ER";
                    laXAdjustProb.Text = "ER";
                    laYAdjustStepError.Text = "ER";
                    laYAdjustProb.Text = "ER";
                    break;

                default:
                    laXAdjustStepError.Text = "ERR";
                    laXAdjustProb.Text = "ERR";
                    laYAdjustStepError.Text = "ERR";
                    laYAdjustProb.Text = "ERR";
                    break;
            }
            XYShiftUpdata();
        }


        #endregion

        #region X、Y粗略回零 按钮事件
        private void RoughReturnZeroX_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status, "X粗略回零……");
            CShareData.RoughReturnZeroJudgeTime = 0;    //扫描判断次数
            if (LaserFunc.XRoughReturnZeroFunc(0,serialPort1,Status,pBStatus))
            {               
                LaserFunc.LabelUpdata(Status, "X粗略回零成功--扫描判断"+CShareData.RoughReturnZeroJudgeTime.ToString()+"次");
            }
            XYShiftUpdata();
        }

        private void RoughReturnZeroY_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status, "Y粗略回零……");
            CShareData.RoughReturnZeroJudgeTime = 0;    //扫描判断次数
            if (LaserFunc.YRoughReturnZeroFunc(0, serialPort1, Status, pBStatus))
            {
                LaserFunc.LabelUpdata(Status, "Y粗略回零成功--扫描判断"+CShareData.RoughReturnZeroJudgeTime.ToString()+"次");
            }
            XYShiftUpdata();
        }

        private void RoughReturnZeroXY_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status, "XY粗略回零……");
            if (LaserFunc.XYRoughReturnZeroFunc(serialPort1, Status, pBStatus))
            {
                LaserFunc.LabelUpdata(Status, "XY粗略回零成功!--扫描判断"+CShareData.RoughReturnZeroJudgeTime.ToString()+"次");
            }
            XYShiftUpdata();
        }
        #endregion

        #region 水平、垂直标定 按钮事件
        private void HrzCalibration_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status,"水平标定……");
            if (LaserFunc.HrzCalibrationFunc(serialPort1, Status, pBStatus))
            {           
                LaserFunc.LabelUpdata(Status,"水平标定成功！");
            }
            XYShiftUpdata();
        }

        private void VerCalibration_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status,"垂直标定……");
            if (LaserFunc.VerCalibrationFunc(serialPort1, Status, pBStatus))
            { 
                LaserFunc.LabelUpdata(Status,"垂直标定成功--测量距离："+ CShareData.CalibMeasureDistance + "水平距离：" + CShareData.CalibHrzDistance);
            }
            XYShiftUpdata();
        }

        private void HrzVerCalibration_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status,"垂直、水平标定……");
            int Result = LaserFunc.HrzVerCalibrationFunc(serialPort1, Status, null);
            switch(Result)
            {
                case 1:
                    LaserFunc.LabelUpdata(Status,"垂直、水平标定均成功！");
                    break;
                case 2:
                    LaserFunc.LabelUpdata(Status,"垂直标定成功！");
                    break;
                case 3:
                    LaserFunc.LabelUpdata(Status,"水平标定成功！");
                    break;
                default:
                    LaserFunc.LabelUpdata(Status,"垂直、水平标定均失败！");
                    break;
            }
            XYShiftUpdata();
        }
        #endregion

        #region 故障检测
        private void btnIPCCheck_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status, "IPC通信检测……");
            if (LaserFunc.IPCCheckFunc(serialPort1, Status, pBStatus))
            {
                LaserFunc.LabelUpdata(Status, "IPC通信正常！");
            }
        }

        private void btnLDMCheck_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status, "LDM通信检测……");
            if (LaserFunc.LDMCheckFunc(serialPort1, Status, pBStatus))
            {
                LaserFunc.LabelUpdata(Status, "LDM通信正常！");
            }
        }

        private void btnFPGACheck_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status, "FPGA通信检测……");
            if (LaserFunc.FPGACheckFunc(serialPort1, Status, pBStatus))
            {
                LaserFunc.LabelUpdata(Status, "FPGA通信正常！");
            }
        }
        private void btnStabilityCheck_Click(object sender, EventArgs e)
        {
            Int32 CheckTime = 5;
            if (LaserFunc.FunctionTestFunc(CheckTime, serialPort1, Status, pBStatus))
            {
                LaserFunc.LabelUpdata(Status,"功能测试" + CheckTime.ToString() + "次，全部通过！");
                pBStatus.Value = 100;
            }

        }
        #endregion

        #region 水平扫描测试
        private void XScan_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status,"水平扫描……");
            Int16 ScanPointNum = Convert.ToInt16(ScanPoint.Text);
            Int32[,] ReceivedDistance = new Int32[2, ScanPointNum];     //读取水平扫描距离值
            Int16 XStartPos = Convert.ToInt16(0 - ((ScanPointNum - 1) >> 2));
            Int16 XEndPos = Convert.ToInt16((ScanPointNum - 1) >> 2);
            ReceivedDistance = LaserFunc.HrzScanFunc(0, XStartPos, XEndPos, serialPort1, Status, null);   //调用水平扫描函数

            if (CShareData.HrzScanFlag)
            {
                XYShiftUpdata();
                LaserFunc.LabelUpdata(Status,"扫描成功!");
            }

            richTextBox1.Text = LaserFunc.ByteArrayToString(CShareData.spReceivedData, CShareData.spReceivedDataLen);
            DataLen.Text= CShareData.spReceivedDataLen.ToString();

            string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
            string path = CShareData.StartPath + "水平" + TimeNow + ".txt";
            LaserFunc.SaveScanDistance(path, ReceivedDistance, ScanPointNum,2);
        }

        private void LeftRightEdge_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status,"左右边界测量……");
            LaserFunc.ProgressBarUpdata(pBStatus, 0);

            Int16 MeasurePos = Convert.ToInt16((Convert.ToInt16(ScanPoint.Text) - 1)>>2);
            bool MeasurePosLeftFlag = false;
            bool MeasurePosRightFlag = false;

            LaserFunc.ReturnZeroFunc(serialPort1,Status,pBStatus);
            LaserFunc.MoveFunc(CShareData.MoveDirection.Left,MeasurePos,CShareData.SetZeroValue.UnSet,serialPort1,Status,pBStatus);
            Thread.Sleep(500);  //最左侧点
            Int32 LeftDistance = Convert.ToInt32(LaserFunc.OnePointMeasureFunc(1000, serialPort1, Status, pBStatus));
            if (CShareData.OnePointDistanceFlag)
            {
                MeasurePosLeftFlag = true;
            }
            LaserFunc.ProgressBarUpdata(pBStatus, 50);

            LaserFunc.ReturnZeroFunc(serialPort1, Status, pBStatus);
            LaserFunc.MoveFunc(CShareData.MoveDirection.Right, MeasurePos, CShareData.SetZeroValue.UnSet, serialPort1, Status, pBStatus);
            Thread.Sleep(500);  //最右侧点
            Int32 RightDistance = Convert.ToInt32(LaserFunc.OnePointMeasureFunc(1000, serialPort1, Status, pBStatus));
            if (CShareData.OnePointDistanceFlag)
            {
                MeasurePosRightFlag = true;
            }

            if (MeasurePosLeftFlag && MeasurePosRightFlag)
            {
                string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                string path = CShareData.StartPath + "左右边界" + TimeNow + ".txt";
                string str = LeftDistance + "\t" + RightDistance;
                LaserFunc.WriteStrToTXT(path, str);
                LaserFunc.LabelUpdata(Status,"保存成功！");
                LaserFunc.ProgressBarUpdata(pBStatus, 100);
            }
            else
            {
                LaserFunc.LabelUpdata(Status,"测量值无效，重新测量!");
                LaserFunc.ProgressBarUpdata(pBStatus, 0);
            }

        }

        private void MoveMeasure_Click(object sender, EventArgs e)
        {
            LaserFunc.ProgressBarUpdata(pBStatus, 0);
            Int16 MoveTime = Convert.ToInt16((Convert.ToInt16(ScanPoint.Text) + 1) >> 1);
            Int16 MeasureLeftPos = Convert.ToInt16((Convert.ToInt16(ScanPoint.Text) - 1) >> 2);

            LaserFunc.ReturnZeroFunc(serialPort1, Status, pBStatus);
            LaserFunc.MoveFunc(CShareData.MoveDirection.Left, MeasureLeftPos, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
            Thread.Sleep(500);

            Int32[] Distance = new Int32[MoveTime];
            bool MoveMeasureFlag = false;
            for (int i = 0; i < MoveTime;i++ )
            {
                LaserFunc.ProgressBarUpdata(pBStatus, Convert.ToInt16(1.0*i/MoveTime));

                Distance[i] = Convert.ToInt32(LaserFunc.OnePointMeasureFunc(200, serialPort1, Status, null));
                if (CShareData.OnePointDistanceFlag)
                {
                    MoveMeasureFlag = true;
                }
                else //二次测量
                {
                    Distance[i] = Convert.ToInt32(LaserFunc.OnePointMeasureFunc(200, serialPort1, Status, null));
                    if (CShareData.OnePointDistanceFlag)
                    {
                        MoveMeasureFlag = true;
                    }
                    else
                    {
                        MoveMeasureFlag = false;
                        break;
                    }
                }
                LaserFunc.MoveFunc(CShareData.MoveDirection.Right, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                Thread.Sleep(200);
            }

            if (MoveMeasureFlag)
            {
                string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                string path = CShareData.StartPath + "移动测量" + TimeNow + ".txt";
                string str = LaserFunc.IntArrayToString(Distance, Distance.Length);
                LaserFunc.WriteStrToTXT(path, str);
                LaserFunc.LabelUpdata(Status,"保存成功！");
                LaserFunc.ProgressBarUpdata(pBStatus, 100);
            }
            else
            {
                LaserFunc.LabelUpdata(Status,"测量失败，重新测量!");
                LaserFunc.ProgressBarUpdata(pBStatus, 0);
            }
        }
        #endregion

        #region Z06扫描(信号强度测试)
        private void btnZScanSignal_Click(object sender, EventArgs e)
        {
            CShareData.OperateRecordStr += "\r\n";
            LaserFunc.LabelUpdata(Status, "Z06扫描……");
            Int16 ScanPointNum = 109;
            Int32[,] ReceivedDistance = new Int32[4, ScanPointNum];     //读取水平扫描距离值
            Int16 XStartPos = Convert.ToInt16(0 - ((ScanPointNum - 1) >> 2));
            Int16 XEndPos = Convert.ToInt16((ScanPointNum - 1) >> 2);
            ReceivedDistance = LaserFunc.Z06ScanFunc(CShareData.ZScanDirection.Down, 0, 0, XStartPos, XEndPos, serialPort1, Status, pBStatus);

            if (CShareData.Z06ScanFlag)
            {
                XYShiftUpdata();
                LaserFunc.LabelUpdata(Status, "Z06扫描成功!");


                richTextBox1.Text = LaserFunc.ByteArrayToString(CShareData.spReceivedData, CShareData.spReceivedDataLen);
                DataLen.Text = CShareData.spReceivedDataLen.ToString();

                string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                string path = CShareData.StartPath + "Z06扫描" + TimeNow + ".txt";
                LaserFunc.SaveScanDistance(path, ReceivedDistance, ScanPointNum,4);
            }
        }
        #endregion



        #region 功能测试

        private void btnTestHrzScan_Click(object sender, EventArgs e)
        {
            LaserFunc.TestHrzScanFunc(100, serialPort1, Status, pBStatus);
        }
        private void btnTestZ01Scan_Click(object sender, EventArgs e)
        {
            LaserFunc.TestZ01ScanFunc(100, serialPort1, Status, pBStatus);
        }
        private void btnTestConfirmEdge_Click(object sender, EventArgs e)
        {
            LaserFunc.TestConfirmEdgeFunc(10, serialPort1, Status, pBStatus);
        }

        private void btnTestAdjust_Click(object sender, EventArgs e)
        {
            LaserFunc.TestAdjustFunc(400, serialPort1, Status, pBStatus);
        }

        private void btnTestRoughReturnZero_Click(object sender, EventArgs e)
        {
            LaserFunc.TestRoughReturnZeroFunc(5, serialPort1, Status, pBStatus);
        }

        private void btnTestConfirmEdgeY_Click(object sender, EventArgs e)
        {
            LaserFunc.TestConfirmYEdgeFunc(20, serialPort1, Status, pBStatus);
        }
        private void btnTestConfirmEdgeX_Click(object sender, EventArgs e)
        {
            LaserFunc.TestConfirmXEdgeFunc(20, serialPort1, Status, pBStatus);
        }

        private void btnTestAdjustY_Click(object sender, EventArgs e)
        {
            LaserFunc.TestAdjustYFunc(64, serialPort1, Status, pBStatus);
        }

        private void btnTestAdjustX_Click(object sender, EventArgs e)
        {
            LaserFunc.TestAdjustXFunc(64, serialPort1, Status, pBStatus);
        }
        #endregion



        #region 测试按钮
        private void btnTest1_Click(object sender, EventArgs e)
        {
            //LaserFunc.TestHrzScanFunc(1000, serialPort1, Status, pBStatus);
            //LaserFunc.TestZ01ScanFunc(1000, serialPort1, Status, pBStatus);
            //LaserFunc.TestConfirmXEdgeFunc(1000, serialPort1, Status, pBStatus);
            //LaserFunc.TestConfirmYEdgeFunc(100, serialPort1, Status, pBStatus);
            //LaserFunc.TestConfirmEdgeFunc(1000, serialPort1, Status, pBStatus);


            //LaserFunc.TestAdjustXFunc(128, serialPort1, Status, pBStatus);
            //LaserFunc.TestAdjustYFunc(128, serialPort1, Status, pBStatus);
            //LaserFunc.TestAdjustFunc(1000, serialPort1, Status, pBStatus);
            //LaserFunc.TestRoughReturnZeroFunc(100, serialPort1, Status, pBStatus);     





            LaserFunc.TestConfirmXEdgeFunc(6, serialPort1, Status, pBStatus);
            LaserFunc.TestConfirmYEdgeFunc(10, serialPort1, Status, pBStatus);
            LaserFunc.TestConfirmEdgeFunc(10, serialPort1, Status, pBStatus);
            LaserFunc.TestAdjustXFunc(10, serialPort1, Status, pBStatus);
            LaserFunc.TestAdjustYFunc(10, serialPort1, Status, pBStatus);
            LaserFunc.TestAdjustFunc(36, serialPort1, Status, pBStatus);


            //LaserFunc.TestRoughReturnZeroFunc(6, serialPort1, Status, pBStatus);  
        }

        private void btnTest2_Click(object sender, EventArgs e)
        {
            
            CShareData.spSem.WaitOne(1);
            laTest2.Text = "test2";
            //CShareData.OperateRecordStr += "\r\n";
            //LaserFunc.LabelUpdata(Status, "查看概率……");
            //double prob = 0;
            //string str = "";
            //for (int i = 0; i < 5; i++)
            //{
            //    if (LaserFunc.ViewProbFunc(serialPort1, Status, pBStatus))
            //    {
            //        str += CShareData.ViewProb.ToString() + '\t';
            //        prob += CShareData.ViewProb;
            //    }
            //}
            //prob = prob / 5;
            //laTest1.Text = prob.ToString();
            //laTest2.Text = str;
        }

        #endregion


































    }
}
