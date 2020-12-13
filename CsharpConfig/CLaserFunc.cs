using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Windows.Forms; //控件相关
using System.IO.Ports;      //SerialPort
using System.IO;            //TXT
using System.Threading;
using Susi4.APIs;

namespace VIDGS配置软件
{
    class LabelUpadteEventArgs : EventArgs
    {
        public string content;
        public LabelUpadteEventArgs(string label)
        {
            content = label;
        }
    }
    class CLaserFunc
    {
        #region 串口打开按钮功能函数
        bool open_flag = false;
        public bool OpenCloseCOMFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus, Button OpenCloseCOM)
        {
            ProgressBarUpdate(pBStatus, 0);
            if (open_flag)      //Close the serialPort
            {
                OpenCloseCOM.Text = "打开串口";
                open_flag = false;
                if (serialPort1.IsOpen)
                {
                    serialPort1.Close();
                    LabelUpdate(Status, "成功关闭串口！");
                }
            }
            else                //Open the serialPort
            {
                if (serialPort1.IsOpen == false)
                {
                    try
                    {
                        serialPort1.Open();
                        OpenCloseCOM.Text = "关闭串口";
                        open_flag = true;

                        LabelUpdate(Status, "成功打开串口:" + serialPort1.PortName + "," + serialPort1.BaudRate + "," + "None");
                    }
                    catch
                    {
                        OpenCloseCOM.Text = "打开串口";
                        open_flag = false;
                        LabelUpdate(Status, "错误：打开串口失败！");
                        ProgressBarUpdate(pBStatus, 0);
                        return false;
                    }
                }
            }
            ProgressBarUpdate(pBStatus, 100);
            return true;
        }
        #endregion

        #region 串口发送函数 DataWrite
        private bool DataWrite(SerialPort serialPort1, byte[] DateWriteBuffer)
        {
            lock (CShareData.spMutex)
            {
                try
                {
                    DateWriteBuffer[DateWriteBuffer.Length - 1] = GetCheckSum(DateWriteBuffer, DateWriteBuffer.Length);  //校验和
                    Array.Clear(CShareData.spReceivedData, 0, CShareData.spReceivedData.Length);

                    if (serialPort1.IsOpen)
                    {
                        CShareData.ReadContinueFalg = false;
                        CShareData.spReceivedDataLen = 0;   //Clean the ReceivedDataLen(忽略上次影响,覆盖数据)
                        CShareData.ReadDataLen = 0;
                        CShareData.spSem.WaitOne(0);        //Clean the Semaphore
                        serialPort1.Write(DateWriteBuffer, 0, DateWriteBuffer.Length);

                        return true;
                    }
                    else
                    {

                        return false;
                    }

                }
                catch
                {
                    return false;
                }
            }
        }
        #endregion

        #region CheckSum
        private byte GetCheckSum(byte[] Data, int DataLen)
        {
            int sum = 0;
            for (int i = 0; i < (DataLen - 1); i++)
            {
                sum += Data[i];
            }

            sum = sum ^ 0xff;
            sum++;

            return Convert.ToByte(sum & 0x00FF);
        }
        private bool JudegCheckSum(byte[] Data, int DataLen)
        {
            int CheckSum = GetCheckSum(Data, DataLen);
            if (CheckSum == Data[DataLen - 1])      //No Error
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        #endregion

        #region ErrorCheck
        int ErrorCheckTime = 0;
        int ErrorTimeX12 = 0;
        public byte ErrorCheck(Label Status)
        {
            ErrorCheckTime++;
            byte ErrorClass = 0;
            if ('X' == CShareData.spReceivedData[0])
            {
                ErrorClass = CShareData.spReceivedData[1];
                switch (ErrorClass)
                {
                    case 0x11:
                        LabelUpdate(Status, "ER11:Cmd CheckSum!");
                        break;
                    case 0x12:
                        LabelUpdate(Status, "ER12:Cmd not Exist!");
                        ErrorTimeX12++;
                        break;
                    case 0x13:
                        LabelUpdate(Status, "ER13:Param Unreasonable!");
                        break;
                    case 0x14:
                        LabelUpdate(Status, "ER14:IPC Outage!");
                        break;
                    case 0x15:
                        LabelUpdate(Status, "ER15:LDM Outage!");
                        break;
                    case 0x16:
                        LabelUpdate(Status, "ER16:FPGA Outage!");
                        break;
                    default:
                        LabelUpdate(Status, "Unknown Error!");
                        break;
                }
            }
            CShareData.TempInt1 = ErrorCheckTime;
            CShareData.TempInt2 = ErrorTimeX12;
            return ErrorClass;
        }
        #endregion

        #region ByteArrayToString
        public string ByteArrayToString(byte[] DataArray, int DataLen)
        {
            string str = "";
            for (int i = 0; i < DataLen; i++)
            {
                str += Convert.ToString(DataArray[i], 16) + " ";
            }
            return str;
        }
        #endregion

        #region IntArrayToString
        public string IntArrayToString(int[] DataArray, int DataLen)
        {
            string str = "";
            for (int i = 0; i < DataLen; i++)
            {
                str += Convert.ToString(DataArray[i], 10) + "\t";     //use TAB to adjust
            }
            return str;
        }
        #endregion

        #region WriteStrToTXT
        //write Str to txt
        public bool WriteStrToTXT(string path, string str)
        {
            try
            {
                FileStream fs = new FileStream(path, FileMode.Create);
                StreamWriter sw = new StreamWriter(fs, Encoding.Default);  //Encoding.UTF8解决乱码问题
                //StreamWriter sw = new StreamWriter(fs);
                //开始写入
                sw.Write(str);
                //清空缓冲区
                sw.Flush();
                //关闭流
                sw.Close();
                fs.Close();

                return true;
            }
            catch
            {
                return false;
            }
        }
        #endregion

        #region SaveOnePointDistance
        public bool SaveOnePointDistance(string path, int[] DataArray, int DataLen)
        {
            string str = IntArrayToString(DataArray, DataLen);
            str += "\r\n\r\n";
            str += "所有测量点均为激光测距仪读取的距离值！\r\n";
            str += "此次保存数据的个数：" + DataLen.ToString() + "\r\n";
            str += "保存成功！";
            if (WriteStrToTXT(path, str))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        #endregion

        #region SaveScanDistance
        public bool SaveScanDistance(string path, int[,] DataArray, int DataLen, int DataDim)
        {
            string str = "";
            for (int i = 0; i < DataLen; i++)
            {
                str += DataArray[0, i].ToString() + "\t";
            }
            for (int m = 1; m < DataDim; m++)
            {
                str += "\r\n\r\n";
                for (int i = 0; i < DataLen; i++)
                {
                    str += DataArray[m, i].ToString() + "\t";
                }
            }

            str += "\r\n\r\n";
            str += "\r\n\r\n";
            str += "所有测量点均为激光测距仪读取的距离值！\r\n";
            str += "单次扫描数据长度：" + DataLen.ToString() + "\r\n";
            int DataNum = DataDim * DataLen;
            str += "此次保存数据的个数：" + DataNum.ToString() + "\r\n";
            str += "保存成功！";

            if (WriteStrToTXT(path, str))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        #endregion

        #region SaveAdjustStep
        public bool SaveAdjustStep(string path, double[,] DataArray)
        {
            int DataLen = DataArray.Length / 2;
            string str = "";
            str += path + "\r\n";
            for (int i = 0; i < DataLen; i++)
            {
                str += DataArray[0, i].ToString() + "\t";
            }
            for (int m = 1; m < 2; m++)
            {
                str += "\r\n\r\n";
                for (int i = 0; i < DataLen; i++)
                {
                    str += DataArray[m, i].ToString("0.000") + "\t";
                }
            }
            str += "\r\n\r\n";
            str += "保存成功！";

            if (WriteStrToTXT(path, str))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        #endregion


        #region ReplaceInvalidData 替换扫描数据中的无效数据4
        public Int32[,] ReplaceInvalidData(Int32[,] DataArry, Int16 DataLen)
        {
            const Int32 InvalidData = 4;      //待替换的数据

            Int32 DataValid = 260000;   //如果全是4，则改为260m
            for (int i = 0; i < 2; i++)
            {
                for (int j = 0; j < DataLen; j++)
                {
                    if (InvalidData == DataArry[i, j])
                    {
                        int DataValidIndex = 0;
                        for (DataValidIndex = j + 1; DataValidIndex < DataLen; DataValidIndex++)
                        {
                            if (InvalidData != DataArry[i, DataValidIndex])
                            {
                                DataValid = DataArry[i, DataValidIndex];
                                break;
                            }
                        }
                        if (DataValidIndex < DataLen)
                        {
                            for (int m = j; m < DataValidIndex; m++)
                            {
                                DataArry[i, m] = DataValid; //替换
                            }
                            j = DataValidIndex; //直接少判断几个
                        }
                        else //最后一个也是4，则倒回来找
                        {
                            int BackIndex = 0;
                            for (BackIndex = DataLen - 1; BackIndex > 0; BackIndex--)
                            {
                                if (InvalidData != DataArry[i, BackIndex])
                                {
                                    DataValid = DataArry[i, BackIndex];
                                    break;
                                }
                            }
                            if (BackIndex > 0)
                            {
                                for (int m = BackIndex + 1; m < DataLen; m++)   //将最后几个4全改为有效值
                                {
                                    DataArry[i, m] = DataValid;
                                }
                            }
                            else//全4
                            {
                                for (int m = 0; m < DataLen; m++)
                                {
                                    DataArry[i, m] = DataValid; //全为260000
                                }
                            }
                        }

                    }
                }
            }
            return DataArry;
        }
        #endregion

        #region LabelUpdate
        public event EventHandler<LabelUpadteEventArgs> OnLabelUpdate;
        public void LabelUpdate(Label la, string str)
        {
            if (la != null)
            {
                la.Text = str;
                la.Update();

                if (la.Name == "Status")    //将状态变化保存在txt中
                {
                    CShareData.StatusLableStr = str;
                }

                if (CShareData.DEBUG)
                {
                    if (la.Name == "Status")    //将状态变化保存在txt中
                    {
                        CShareData.OperateRecordStr += str + "\r\n";
                        
                    }
                }
            }
            else
            {
                if (OnLabelUpdate != null) OnLabelUpdate(this, new LabelUpadteEventArgs(str));
            }
        }
        #endregion

        #region ProgressBarUpdate
        public void ProgressBarUpdate(ProgressBar pb, int value)
        {
            if (pb != null)
            {
                if ((value <= 100) && (value >= 0))
                {
                    pb.Value = value;
                    pb.Update();
                }
                else
                {
                    return;
                }

                if (pb.Name == "pBStatus")
                {
                    CShareData.StatuspBValue = value;
                }

            }
        }
        #endregion

        #region 保存操作记录
        public void SaveOperateRecord()
        {
            if (CShareData.DEBUG)
            {
                string path = "操作记录" + ".txt";
                WriteStrToTXT(path, CShareData.OperateRecordStr);
            }
        }
        #endregion

        #region 红色激光
        public bool RedLaser(CShareData.RedLaserValue RedLaser, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            //--------------------------------------C---子类--Len_H-Len_L--Data-Check 
            byte[] RedLaserCommand = new byte[] { 0x43, 0x07, 0x00, 0x01, 0x00, 0xca };   //Open or Close RedLaser                    
            CShareData.spReceivedDataLenIdeal = 5;   //you should receive 5 bytes -- 0x43 0x07 0x00 0x00 0xb6
            RedLaserCommand[4] = Convert.ToByte(RedLaser);

            DataWrite(serialPort1, RedLaserCommand);     //send command
            if (CShareData.spSem.WaitOne(CShareData.spSemTimeout) == false)    //wait sem
            {
                LabelUpdate(Status, "串口接收等待超时！");
                ProgressBarUpdate(pBStatus, 0);
                return false;
            }
            lock (CShareData.spMutex)
            {
                if (CShareData.spErrorFlag)
                {
                    LabelUpdate(Status, "串口错误！");
                    ProgressBarUpdate(pBStatus, 0);
                    return false;
                }

                if (CShareData.spReceivedDataLen == CShareData.spReceivedDataLenIdeal)  //长度
                {
                    if (JudegCheckSum(CShareData.spReceivedData, CShareData.spReceivedDataLen)) //校验和
                    {
                        if ((CShareData.spReceivedData[0] == 0x43) && (CShareData.spReceivedData[1] == 0x07))
                        {
                            ProgressBarUpdate(pBStatus, 100);
                            return true;
                        }
                        else
                        {
                            ErrorCheck(Status);  //检查错误
                            ProgressBarUpdate(pBStatus, 0);
                            return false;
                        }
                    }
                    else
                    {
                        LabelUpdate(Status, "校验证和错误！");
                        ProgressBarUpdate(pBStatus, 0);
                        return false;
                    }

                }
                else
                {
                    ErrorCheck(Status);  //检查错误
                    LabelUpdate(Status, "仅接收到" + CShareData.spReceivedDataLen.ToString() + "个数据！");
                    ProgressBarUpdate(pBStatus, 0);
                    return false;
                }
            }
        }
        #endregion

        #region 按钮事件，单点测距
        public double ViewOnePointMeasureFunc(int MeasureNum, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            CShareData.ViewOnePointDistanceFlag = false;
            //-------------------------------------C---子类--Len_H-Len_L--N_H--N_L----MF----SA----DT------1000------TDx---TDy--Check 
            byte[] MeasureCommand = new byte[] { 0x43, 0x01, 0x00, 0x09, 0x00, 0xC8, 0x02, 0x03, 0x02, 0x03, 0xe8, 0x00, 0x00, 0xCA };
            //zzm SZ 0729:change 03 e8 to 07 D0
            int N = MeasureNum;      //the point num for measure, max-2000
            MeasureCommand[4] = Convert.ToByte((N & 0xFF00) >> 8);
            MeasureCommand[5] = Convert.ToByte(N & 0x00FF);

            CShareData.spReceivedDataLenIdeal = 2 * N + 8;  //you should receive 2*N + 8 bytes -- 0x43 0x01 Len_H Len_L 0x00 0x00 0x00 2*N-data CheckSum

            ProgressBarUpdate(pBStatus, 10);
            DataWrite(serialPort1, MeasureCommand);   //send command
            if (CShareData.spSem.WaitOne(CShareData.spSemTimeout) == false)    //wait sem
            {
                string str = "--" + CShareData.spReceivedDataLen.ToString() + "--" + CShareData.ReadDataLen.ToString() + "--" + CShareData.spReceivedDataLenIdeal.ToString();
                LabelUpdate(Status, "串口接收等待超时！" + str);
                ProgressBarUpdate(pBStatus, 0);
                return 0;
            }
            lock (CShareData.spMutex)
            {

                if (CShareData.spErrorFlag)
                {
                    LabelUpdate(Status, "串口错误！");
                    ProgressBarUpdate(pBStatus, 0);
                    return 1;
                }

                Array.Clear(CShareData.ViewOnePointDistance, 0, CShareData.ViewOnePointDistance.Length);
                if (CShareData.spReceivedDataLen == CShareData.spReceivedDataLenIdeal)  //长度
                {
                    if (JudegCheckSum(CShareData.spReceivedData, CShareData.spReceivedDataLen)) //校验和
                    {
                        if ((CShareData.spReceivedData[0] == 0x43) && (CShareData.spReceivedData[1] == 0x01))
                        {
                            for (int i = 0; i < N; i++)
                            {
                                //find the value of every measure,and change to real distance(mm)
                                CShareData.ViewOnePointDistance[i] = 4 * (CShareData.spReceivedData[2 * i + 7] * 256 + CShareData.spReceivedData[2 * i + 8]);
                            }
                            CShareData.ViewOnePointDistanceLen = N;    //距离值的个数

                            //距离中的“4”
                            Int32[] ViewDistanceFilter4 = new Int32[N];
                            int ViewDistanceFilter4Len = 0;
                            for (int i = 0; i < N; i++)
                            {
                                if (CShareData.ViewOnePointDistance[i] < 10)
                                    ViewDistanceFilter4[ViewDistanceFilter4Len++] = CShareData.ViewOnePointDistance[i];
                            }
                            CShareData.ViewOnePointDistance4Prob = 1.0 * ViewDistanceFilter4Len / N;

                            //距离中的机柜内的数据
                            Int32[] ViewDistanceFilterShort = new Int32[N];
                            int ViewDistanceFilterShortLen = 0;
                            for (int i = 0; i < N; i++)
                            {
                                if ((CShareData.ViewOnePointDistance[i] > 10) && (CShareData.ViewOnePointDistance[i] < CShareData.BOXMAXDISTANCE))
                                    ViewDistanceFilterShort[ViewDistanceFilterShortLen++] = CShareData.ViewOnePointDistance[i];
                            }
                            CShareData.ViewOnePointDistanceShortProb = 1.0 * ViewDistanceFilterShortLen / N;

                            //距离中机柜外的数据
                            Int32[] ViewDistanceFilterLong = new Int32[N];
                            int ViewDistanceFilterLongLen = 0;
                            for (int i = 0; i < N; i++)
                            {
                                if (CShareData.ViewOnePointDistance[i] > CShareData.BOXMAXDISTANCE)
                                    ViewDistanceFilterLong[ViewDistanceFilterLongLen++] = CShareData.ViewOnePointDistance[i];
                            }
                            CShareData.ViewOnePointDistanceLongProb = 1.0 * ViewDistanceFilterLongLen / N;

                            double DistanceFilterAverage = 0;
                            if (((CShareData.ViewOnePointDistanceLongProb) > (CShareData.ViewOnePointDistanceShortProb)) && (CShareData.ViewOnePointDistanceLongProb) > (CShareData.ViewOnePointDistance4Prob))
                            {
                                DistanceFilterAverage = ViewDistanceFilterLong.Sum() / ViewDistanceFilterLongLen;
                            }
                            else if ((CShareData.ViewOnePointDistanceShortProb) > (CShareData.ViewOnePointDistance4Prob))
                            {
                                DistanceFilterAverage = ViewDistanceFilterShort.Sum() / ViewDistanceFilterShortLen;
                            }
                            else
                            {
                                DistanceFilterAverage = 4;
                            }

                            CShareData.ViewOnePointDistanceValidProb = CShareData.ViewOnePointDistanceLongProb + CShareData.ViewOnePointDistanceShortProb;

                            LabelUpdate(Status, "有效数据：" + (CShareData.ViewOnePointDistanceValidProb * 100).ToString() + "%" + "--" + (CShareData.ViewOnePointDistanceLongProb * 100).ToString() + "%" + "--" + (CShareData.ViewOnePointDistanceShortProb * 100).ToString() + "%" + "--" + (CShareData.ViewOnePointDistance4Prob * 100).ToString() + "%");
                            ProgressBarUpdate(pBStatus, 100);
                            CShareData.ViewOnePointDistanceFlag = true;

                            return DistanceFilterAverage;
                        }
                        else
                        {
                            ErrorCheck(Status);  //检查错误
                            ProgressBarUpdate(pBStatus, 0);
                            return 4;
                        }
                    }
                    else
                    {
                        LabelUpdate(Status, "校验证和错误！");
                        ProgressBarUpdate(pBStatus, 0);
                        return 3;
                    }

                }
                else
                {
                    ErrorCheck(Status);  //检查错误
                    ProgressBarUpdate(pBStatus, 0);
                    return 2;
                }
            }
        }

        #endregion

        #region 单点测距
        public double OnePointMeasureFunc(int MeasureNum, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            CShareData.OnePointDistanceFlag = false;
            //-------------------------------------C---子类--Len_H-Len_L--N_H--N_L----MF----SA----DT------1000------TDx---TDy--Check 
            byte[] MeasureCommand = new byte[] { 0x43, 0x01, 0x00, 0x09, 0x00, 0xC8, 0x02, 0x03, 0x02, 0x03, 0xe8, 0x00, 0x00, 0xCA };
            //zzm SZ 0729:change 03 e8 to 07 D0
            int N = MeasureNum;      //the point num for measure, max-2000
            MeasureCommand[4] = Convert.ToByte((N & 0xFF00) >> 8);
            MeasureCommand[5] = Convert.ToByte(N & 0x00FF);

            CShareData.spReceivedDataLenIdeal = 2 * N + 8;  //you should receive 2*N + 8 bytes -- 0x43 0x01 Len_H Len_L 0x00 0x00 0x00 2*N-data CheckSum

            ProgressBarUpdate(pBStatus, 10);
            DataWrite(serialPort1, MeasureCommand);   //send command
            if (CShareData.spSem.WaitOne(CShareData.spSemTimeout) == false)    //wait sem
            {
                string str = "--" + CShareData.spReceivedDataLen.ToString() + "--" + CShareData.ReadDataLen.ToString() + "--" + CShareData.spReceivedDataLenIdeal.ToString();
                LabelUpdate(Status, "串口接收等待超时！" + str);
                ProgressBarUpdate(pBStatus, 0);
                return 0;
            }
            lock (CShareData.spMutex)
            {

                if (CShareData.spErrorFlag)
                {
                    LabelUpdate(Status, "串口错误！");
                    ProgressBarUpdate(pBStatus, 0);
                    return 1;
                }

                Array.Clear(CShareData.OnePointDistance, 0, CShareData.OnePointDistance.Length);
                if (CShareData.spReceivedDataLen == CShareData.spReceivedDataLenIdeal)  //长度
                {
                    if (JudegCheckSum(CShareData.spReceivedData, CShareData.spReceivedDataLen)) //校验和
                    {
                        if ((CShareData.spReceivedData[0] == 0x43) && (CShareData.spReceivedData[1] == 0x01))
                        {
                            for (int i = 0; i < N; i++)
                            {
                                //find the value of every measure,and change to real distance(mm)
                                CShareData.OnePointDistance[i] = 4 * (CShareData.spReceivedData[2 * i + 7] * 256 + CShareData.spReceivedData[2 * i + 8]);
                            }
                            CShareData.OnePointDistanceLen = N;    //距离值的个数

                            //距离中的“4”
                            Int32[] DistanceFilter = new Int32[N];
                            int DistanceFilterLen = 0;
                            for (int i = 0; i < N; i++)
                            {
                                if (CShareData.OnePointDistance[i] < 10)
                                    DistanceFilter[DistanceFilterLen++] = CShareData.OnePointDistance[i];
                            }

                            //OnePointMeasureValue.Text = Math.Round(Distance.Average()).ToString();  //calculate the average
                            CShareData.OnePointDistanceValidProb = 1.0 * DistanceFilterLen / N;
                            double DistanceFilterAverage;
                            if (DistanceFilterLen > (0.8 * N)) //80%是有效数据
                                DistanceFilterAverage = DistanceFilter.Sum() / DistanceFilterLen;
                            else
                                DistanceFilterAverage = CShareData.OnePointDistance.Sum() / N;

                            LabelUpdate(Status, "有效数据：" + (CShareData.OnePointDistanceValidProb * 100).ToString() + "%");
                            ProgressBarUpdate(pBStatus, 100);
                            CShareData.OnePointDistanceFlag = true;

                            return DistanceFilterAverage;
                        }
                        else
                        {
                            ErrorCheck(Status);  //检查错误
                            ProgressBarUpdate(pBStatus, 0);
                            return 4;
                        }
                    }
                    else
                    {
                        LabelUpdate(Status, "校验证和错误！");
                        ProgressBarUpdate(pBStatus, 0);
                        return 3;
                    }

                }
                else
                {
                    ErrorCheck(Status);  //检查错误
                    ProgressBarUpdate(pBStatus, 0);
                    return 2;
                }
            }
        }

        #endregion

        #region 单点测距，不进行筛选，仅返回原始数据
        public double OrgOnePointMeasureFunc(int MeasureNum, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            CShareData.OrgOnePointDistanceFlag = false;
            //-------------------------------------C---子类--Len_H-Len_L--N_H--N_L----MF----SA----DT------1000------TDx---TDy--Check 
            byte[] MeasureCommand = new byte[] { 0x43, 0x01, 0x00, 0x09, 0x00, 0xC8, 0x02, 0x03, 0x02, 0x03, 0xe8, 0x00, 0x00, 0xCA };
            //zzm SZ 0729:change 03 e8 to 07 D0
            int N = MeasureNum;      //the point num for measure, max-2000
            MeasureCommand[4] = Convert.ToByte((N & 0xFF00) >> 8);
            MeasureCommand[5] = Convert.ToByte(N & 0x00FF);

            CShareData.spReceivedDataLenIdeal = 2 * N + 8;  //you should receive 2*N + 8 bytes -- 0x43 0x01 Len_H Len_L 0x00 0x00 0x00 2*N-data CheckSum

            ProgressBarUpdate(pBStatus, 10);
            DataWrite(serialPort1, MeasureCommand);   //send command
            if (CShareData.spSem.WaitOne(CShareData.spSemTimeout) == false)    //wait sem
            {
                string str = "--" + CShareData.spReceivedDataLen.ToString() + "--" + CShareData.ReadDataLen.ToString() + "--" + CShareData.spReceivedDataLenIdeal.ToString();
                LabelUpdate(Status, "串口接收等待超时！" + str);
                ProgressBarUpdate(pBStatus, 0);
                return -1;
            }
            lock (CShareData.spMutex)
            {

                if (CShareData.spErrorFlag)
                {
                    LabelUpdate(Status, "串口错误！");
                    ProgressBarUpdate(pBStatus, 0);
                    return 1;
                }

                Array.Clear(CShareData.OrgOnePointDistance, 0, CShareData.OrgOnePointDistance.Length);
                if (CShareData.spReceivedDataLen == CShareData.spReceivedDataLenIdeal)  //长度
                {
                    if (JudegCheckSum(CShareData.spReceivedData, CShareData.spReceivedDataLen)) //校验和
                    {
                        if ((CShareData.spReceivedData[0] == 0x43) && (CShareData.spReceivedData[1] == 0x01))
                        {
                            for (int i = 0; i < N; i++)
                            {
                                //find the value of every measure,and change to real distance(mm)
                                CShareData.OrgOnePointDistance[i] = 4 * (CShareData.spReceivedData[2 * i + 7] * 256 + CShareData.spReceivedData[2 * i + 8]);
                            }
                            CShareData.OrgOnePointDistanceLen = N;    //距离值的个数
                            CShareData.OrgOnePointDistanceFlag = true;

                            return 0;   //测距成功
                        }
                        else
                        {
                            ErrorCheck(Status);  //检查错误
                            ProgressBarUpdate(pBStatus, 0);
                            return 4;
                        }
                    }
                    else
                    {
                        LabelUpdate(Status, "校验证和错误！");
                        ProgressBarUpdate(pBStatus, 0);
                        return 3;
                    }

                }
                else
                {
                    ErrorCheck(Status);  //检查错误
                    ProgressBarUpdate(pBStatus, 0);
                    return 2;
                }
            }
        }

        #endregion

        #region 单点测距,仅统计框外有效数据
        public double OutOnePointMeasureFunc(int MeasureNum, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            CShareData.OutOnePointDistanceFlag = false;
            //-------------------------------------C---子类--Len_H-Len_L--N_H--N_L----MF----SA----DT------1000------TDx---TDy--Check 
            byte[] MeasureCommand = new byte[] { 0x43, 0x01, 0x00, 0x09, 0x00, 0xC8, 0x02, 0x03, 0x02, 0x03, 0xe8, 0x00, 0x00, 0xCA };
            //zzm SZ 0729:change 03 e8 to 07 D0
            int N = MeasureNum;      //the point num for measure, max-2000
            MeasureCommand[4] = Convert.ToByte((N & 0xFF00) >> 8);
            MeasureCommand[5] = Convert.ToByte(N & 0x00FF);

            CShareData.spReceivedDataLenIdeal = 2 * N + 8;  //you should receive 2*N + 8 bytes -- 0x43 0x01 Len_H Len_L 0x00 0x00 0x00 2*N-data CheckSum

            ProgressBarUpdate(pBStatus, 10);
            DataWrite(serialPort1, MeasureCommand);   //send command
            if (CShareData.spSem.WaitOne(CShareData.spSemTimeout) == false)    //wait sem
            {
                string str = "--" + CShareData.spReceivedDataLen.ToString() + "--" + CShareData.ReadDataLen.ToString() + "--" + CShareData.spReceivedDataLenIdeal.ToString();
                LabelUpdate(Status, "串口接收等待超时！" + str);
                ProgressBarUpdate(pBStatus, 0);
                return 0;
            }
            lock (CShareData.spMutex)
            {

                if (CShareData.spErrorFlag)
                {
                    LabelUpdate(Status, "串口错误！");
                    ProgressBarUpdate(pBStatus, 0);
                    return 1;
                }

                Array.Clear(CShareData.OutOnePointDistance, 0, CShareData.OutOnePointDistance.Length);
                if (CShareData.spReceivedDataLen == CShareData.spReceivedDataLenIdeal)  //长度
                {
                    if (JudegCheckSum(CShareData.spReceivedData, CShareData.spReceivedDataLen)) //校验和
                    {
                        if ((CShareData.spReceivedData[0] == 0x43) && (CShareData.spReceivedData[1] == 0x01))
                        {
                            for (int i = 0; i < N; i++)
                            {
                                //find the value of every measure,and change to real distance(mm)
                                CShareData.OutOnePointDistance[i] = 4 * (CShareData.spReceivedData[2 * i + 7] * 256 + CShareData.spReceivedData[2 * i + 8]);
                            }
                            CShareData.OutOnePointDistanceLen = N;    //距离值的个数

                            //去掉距离中的“4”
                            Int32[] DistanceFilter = new Int32[N];
                            int DistanceFilterLen = 0;
                            for (int i = 0; i < N; i++)
                            {
                                if (CShareData.OutOnePointDistance[i] > CShareData.BOXMAXDISTANCE)
                                    DistanceFilter[DistanceFilterLen++] = CShareData.OutOnePointDistance[i];
                            }

                            //OnePointMeasureValue.Text = Math.Round(Distance.Average()).ToString();  //calculate the average
                            CShareData.OutOnePointDistanceValidProb = 1.0 * DistanceFilterLen / N;
                            double DistanceFilterAverage;
                            if (DistanceFilterLen > (0.1 * N)) //80%是有效数据
                                DistanceFilterAverage = DistanceFilter.Sum() / DistanceFilterLen;
                            else
                                DistanceFilterAverage = CShareData.OutOnePointDistance.Sum() / N;

                            LabelUpdate(Status, "有效数据：" + (CShareData.OutOnePointDistanceValidProb * 100).ToString() + "%");
                            ProgressBarUpdate(pBStatus, 100);
                            CShareData.OutOnePointDistanceFlag = true;

                            return DistanceFilterAverage;
                        }
                        else
                        {
                            ErrorCheck(Status);  //检查错误
                            ProgressBarUpdate(pBStatus, 0);
                            return 4;
                        }
                    }
                    else
                    {
                        LabelUpdate(Status, "校验证和错误！");
                        ProgressBarUpdate(pBStatus, 0);
                        return 3;
                    }

                }
                else
                {
                    ErrorCheck(Status);  //检查错误
                    ProgressBarUpdate(pBStatus, 0);
                    return 2;
                }
            }
        }

        #endregion

        #region 多次单点测距（基数1000）
        public double NTimeOnePointMeasureFunc(int NTime, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            CShareData.NTimeOnePointDistanceFlag = false;

            //-------------------------------------C---子类--Len_H-Len_L--N_H--N_L----MF----SA----DT------1000------TDx---TDy--Check 
            byte[] MeasureCommand = new byte[] { 0x43, 0x01, 0x00, 0x09, 0x00, 0xC8, 0x02, 0x03, 0x02, 0x03, 0xE8, 0x00, 0x00, 0xCA };
            int N = 1000;      //the point num for measure, max-2000
            MeasureCommand[4] = Convert.ToByte((N & 0xFF00) >> 8);
            MeasureCommand[5] = Convert.ToByte(N & 0x00FF);

            CShareData.spReceivedDataLenIdeal = 2 * N + 8;  //you should receive 2*N + 8 bytes -- 0x43 0x01 Len_H Len_L 0x00 0x00 0x00 2*N-data CheckSum

            Array.Clear(CShareData.NTimeOnePointDistance, 0, CShareData.NTimeOnePointDistance.Length);
            for (int time = 0; time < NTime; time++)
            {
                ProgressBarUpdate(pBStatus, Convert.ToInt32(1.0 * time / NTime * 100));

                DataWrite(serialPort1, MeasureCommand);   //send command
                if (CShareData.spSem.WaitOne(CShareData.spSemTimeout) == false)    //wait sem
                {
                    string str = "--" + CShareData.spReceivedDataLen.ToString() + "--" + CShareData.ReadDataLen.ToString() + "--" + CShareData.spReceivedDataLenIdeal.ToString();
                    LabelUpdate(Status, "串口接收等待超时！" + str);
                    ProgressBarUpdate(pBStatus, 0);
                    return 0;
                }
                lock (CShareData.spMutex)
                {
                    if (CShareData.spErrorFlag)
                    {
                        LabelUpdate(Status, "串口错误！");
                        ProgressBarUpdate(pBStatus, 0);
                        return 1;
                    }
                    Int32 DistanceIndex = N * time;
                    CShareData.NTimeOnePointDistanceLen = N * NTime;    //距离值的个数
                    if (CShareData.spReceivedDataLen == CShareData.spReceivedDataLenIdeal)  //长度
                    {
                        if (JudegCheckSum(CShareData.spReceivedData, CShareData.spReceivedDataLen)) //校验和
                        {
                            if ((CShareData.spReceivedData[0] == 0x43) && (CShareData.spReceivedData[1] == 0x01))
                            {
                                for (int i = 0; i < N; i++)
                                {
                                    //find the value of every measure,and change to real distance(mm)
                                    CShareData.NTimeOnePointDistance[DistanceIndex + i] = 4 * (CShareData.spReceivedData[2 * i + 7] * 256 + CShareData.spReceivedData[2 * i + 8]);
                                }

                            }
                            else
                            {
                                ErrorCheck(Status);  //检查错误
                                ProgressBarUpdate(pBStatus, 0);
                                return 4;
                            }
                        }
                        else
                        {
                            LabelUpdate(Status, "校验证和错误！");
                            ProgressBarUpdate(pBStatus, 0);
                            return 3;
                        }

                    }
                    else
                    {
                        ErrorCheck(Status);  //检查错误
                        ProgressBarUpdate(pBStatus, 0);
                        return 2;
                    }
                }
            }

            double DistanceFilterAverage;
            DistanceFilterAverage = CShareData.NTimeOnePointDistance.Sum() / CShareData.NTimeOnePointDistanceLen;

            CShareData.NTimeOnePointDistanceFlag = true;

            return DistanceFilterAverage;
        }

        #endregion

        #region 统计边界概率
        public double GetEdgeProbFunc(int NTime, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            int InSideWindow = 0;
            double InSideWindowProb = 0;

            NTimeOnePointMeasureFunc(NTime, serialPort1, Status, null);    //定点测距:NTime*1000次
            for (int j = 0; j < CShareData.NTimeOnePointDistanceLen; j++)
            {
                if ((CShareData.NTimeOnePointDistance[j] > 4) && (CShareData.NTimeOnePointDistance[j] < CShareData.BOXMAXDISTANCE))
                {
                    InSideWindow++;
                }
            }
            InSideWindowProb = Convert.ToDouble(InSideWindow) / CShareData.NTimeOnePointDistanceLen;   //落在箱子内的概率

            return InSideWindowProb;

        }
        #endregion

        #region 水平扫描     //正常使用，需要将SA=1改为SA=3（激光测距仪本身问题）
        public Int32[,] HrzScanFunc(Int16 YPos, Int16 XStartPos, Int16 XEndPos, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            CShareData.HrzScanFlag = false;
            //-----------------------------------S----子类--Len_H-Len_L-Scan-----YPos------XStartPos----XEndPos---Delay---MF----SA---Check 
            byte[] HrzScanCommand = new byte[] { 0x53, 0x01, 0x00, 0x0A, 0x02, 0x00, 0x00, 0xFF, 0xE6, 0x00, 0x1A, 0x00, 0x03, 0x03, 0xCA };
            Int16 ScanTime = 2;      //the Scan time (default:2)
            Int16 PointCnt = Convert.ToInt16(((XEndPos - XStartPos) << 1) + 1);  //the number of distance,default:109 
            HrzScanCommand[4] = Convert.ToByte(ScanTime & 0x00FF);
            HrzScanCommand[5] = Convert.ToByte((YPos >> 8) & 0x00FF);
            HrzScanCommand[6] = Convert.ToByte((YPos >> 0) & 0x00FF);
            HrzScanCommand[7] = Convert.ToByte((XStartPos >> 8) & 0x00FF);
            HrzScanCommand[8] = Convert.ToByte((XStartPos >> 0) & 0x00FF);
            HrzScanCommand[9] = Convert.ToByte((XEndPos >> 8) & 0x00FF);
            HrzScanCommand[10] = Convert.ToByte((XEndPos >> 0) & 0x00FF);

            Int32[,] HrzScanDistance = new Int32[ScanTime, PointCnt];  //读取的距离值
            CShareData.spReceivedDataLenIdeal = (PointCnt << 1) + 16;     //every scan you should receive 226 bytes -- 0x53 0x01 Len_H Len_L 0x00 0x00 0x00 ---- 2*N-data CheckSum
            DataWrite(serialPort1, HrzScanCommand);   //send command

            for (int i = 0; i < ScanTime; i++)
            //int i = 0;
            {
                Int32 WaitTime = CShareData.spSemTimeout + Math.Abs(CShareData.XShift) + Math.Abs(CShareData.YShift);
                if (CShareData.spSem.WaitOne(WaitTime) == false)    //wait sem
                {
                    string str = "--" + CShareData.spReceivedDataLen.ToString() + "--" + CShareData.ReadDataLen.ToString() + "--" + CShareData.spReceivedDataLenIdeal.ToString();
                    LabelUpdate(Status, "串口接收等待超时！" + str);
                    goto Error;
                }

                lock (CShareData.spMutex)
                {
                    if (CShareData.spErrorFlag)
                    {
                        LabelUpdate(Status, "串口错误！");
                        goto Error;
                    }

                    if (CShareData.spReceivedDataLen == CShareData.spReceivedDataLenIdeal)  //长度
                    {
                        if (JudegCheckSum(CShareData.spReceivedData, CShareData.spReceivedDataLen)) //校验和
                        {
                            if ((CShareData.spReceivedData[0] == 0x53) && (CShareData.spReceivedData[1] == 0x01))
                            {
                                for (int j = 0; j < PointCnt; j++)
                                {
                                    //find the value of every measure,and change to real distance(mm)
                                    if (CShareData.spReceivedData[6] == 0x01)    //若是反向扫描，纠正数据排列顺序
                                        HrzScanDistance[i, PointCnt - 1 - j] = 4 * (CShareData.spReceivedData[2 * j + 15] * 256 + CShareData.spReceivedData[2 * j + 16]);
                                    else
                                        HrzScanDistance[i, j] = 4 * (CShareData.spReceivedData[2 * j + 15] * 256 + CShareData.spReceivedData[2 * j + 16]);
                                }

                                byte FrameNum = CShareData.spReceivedData[4];
                                if (FrameNum != i)
                                {
                                    LabelUpdate(Status, "扫描帧号乱序!");
                                    goto Error;
                                }

                                if ((CShareData.spReceivedData[5] == 0x00) && (i != ScanTime - 1))    //如果读取一部分就没有后续帧，直接停止读数据
                                {
                                    LabelUpdate(Status, "扫描数据未读完!");
                                    goto Error;
                                }
                            }
                            else
                            {
                                ErrorCheck(Status);  //检查错误
                                goto Error;
                            }
                        }
                        else
                        {
                            LabelUpdate(Status, "校验证和错误！");
                            goto Error;
                        }

                    }
                    else
                    {
                        ErrorCheck(Status);  //检查错误                    
                        goto Error;
                    }
                }
            }

            //LabelUpdate(Status,"扫描成功!");
            CShareData.HrzScanFlag = true; //无错误goto,则扫描成功
            CShareData.XShift = XStartPos;
            ProgressBarUpdate(pBStatus, 100);
            return HrzScanDistance;

        Error:
            CShareData.HrzScanFlag = false;
            ProgressBarUpdate(pBStatus, 0);
            return HrzScanDistance;
        }
        #endregion

        #region Z型扫描
        public Int32[,] Z01ScanFunc(CShareData.ZScanDirection Dir, Int16 YAdjust, Int16 YPos, Int16 XStartPos, Int16 XEndPos, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            CShareData.Z01ScanFlag = false;
            //-----------------------------------Z----子类--Len_H-Len_L--Y_H---Y_L---YStartPos--XStartPos----XEndPos---Delay---MF----SA---Check 
            byte[] Z01ScanCommand = new byte[] { 0x5A, 0x01, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xE6, 0x00, 0x1A, 0x00, 0x03, 0x03, 0xCA };
            Int16 ScanTime = 2;      //the Scan time (default:2)
            Int16 PointCnt = Convert.ToInt16(((XEndPos - XStartPos) << 1) + 1);  //the number of distance,default:109 
            if (CShareData.ZScanDirection.Up == Dir)
            {
                Z01ScanCommand[4] = Convert.ToByte((YAdjust >> 8) & 0x00FF);
                Z01ScanCommand[5] = Convert.ToByte((YAdjust >> 0) & 0x00FF);
            }
            else
            {
                Z01ScanCommand[4] = Convert.ToByte(((0 - YAdjust) >> 8) & 0x00FF);
                Z01ScanCommand[5] = Convert.ToByte(((0 - YAdjust) >> 0) & 0x00FF);
            }
            Z01ScanCommand[6] = Convert.ToByte((YPos >> 8) & 0x00FF);
            Z01ScanCommand[7] = Convert.ToByte((YPos >> 0) & 0x00FF);
            Z01ScanCommand[8] = Convert.ToByte((XStartPos >> 8) & 0x00FF);
            Z01ScanCommand[9] = Convert.ToByte((XStartPos >> 0) & 0x00FF);
            Z01ScanCommand[10] = Convert.ToByte((XEndPos >> 8) & 0x00FF);
            Z01ScanCommand[11] = Convert.ToByte((XEndPos >> 0) & 0x00FF);

            Int32[,] Z01ScanDistance = new Int32[ScanTime, PointCnt];  //读取的距离值
            CShareData.spReceivedDataLenIdeal = (PointCnt << 1) + 16;     //every scan you should receive 226 bytes -- 0x5A 0x01 Len_H Len_L 0x00 0x00 0x00 ---- 2*N-data CheckSum
            DataWrite(serialPort1, Z01ScanCommand);   //send command

            for (int i = 0; i < ScanTime; i++)
            {
                Int32 WaitTime = CShareData.spSemTimeout + Math.Abs(CShareData.XShift) + Math.Abs(CShareData.YShift);
                if (CShareData.spSem.WaitOne(WaitTime) == false)    //wait sem
                {
                    string str = "--" + CShareData.spReceivedDataLen.ToString() + "--" + CShareData.ReadDataLen.ToString() + "--" + CShareData.spReceivedDataLenIdeal.ToString();
                    LabelUpdate(Status, "串口接收等待超时！" + str);
                    goto Error;
                }

                lock (CShareData.spMutex)
                {
                    if (CShareData.spErrorFlag)
                    {
                        LabelUpdate(Status, "串口错误！");
                        goto Error;
                    }

                    if (CShareData.spReceivedDataLen == CShareData.spReceivedDataLenIdeal)  //长度
                    {
                        if (JudegCheckSum(CShareData.spReceivedData, CShareData.spReceivedDataLen)) //校验和
                        {
                            if ((CShareData.spReceivedData[0] == 0x5A) && (CShareData.spReceivedData[1] == 0x01))
                            {
                                for (int j = 0; j < PointCnt; j++)
                                {
                                    //find the value of every measure,and change to real distance(mm)
                                    if (CShareData.spReceivedData[6] == 0x01)    //若是反向扫描，纠正数据排列顺序
                                        Z01ScanDistance[i, PointCnt - 1 - j] = 4 * (CShareData.spReceivedData[2 * j + 15] * 256 + CShareData.spReceivedData[2 * j + 16]);
                                    else
                                        Z01ScanDistance[i, j] = 4 * (CShareData.spReceivedData[2 * j + 15] * 256 + CShareData.spReceivedData[2 * j + 16]);
                                }

                                byte FrameNum = CShareData.spReceivedData[4];
                                if (FrameNum != i)
                                {
                                    LabelUpdate(Status, "扫描帧号乱序!");
                                    goto Error;
                                }

                                if ((CShareData.spReceivedData[5] == 0x00) && (i != ScanTime - 1))    //如果读取一部分就没有后续帧，直接停止读数据
                                {
                                    LabelUpdate(Status, "扫描数据未读完!");
                                    goto Error;
                                }
                            }
                            else
                            {
                                ErrorCheck(Status);  //检查错误
                                goto Error;
                            }
                        }
                        else
                        {
                            LabelUpdate(Status, "校验证和错误！");
                            goto Error;
                        }

                    }
                    else
                    {
                        ErrorCheck(Status);  //检查错误                    
                        goto Error;
                    }
                }
            }

            //LabelUpdate(Status,"扫描成功!");
            CShareData.Z01ScanFlag = true; //无错误goto,则扫描成功
            CShareData.XShift = XStartPos;
            ProgressBarUpdate(pBStatus, 100);
            return Z01ScanDistance;

        Error:
            CShareData.Z01ScanFlag = false;
            ProgressBarUpdate(pBStatus, 0);
            return Z01ScanDistance;
        }
        #endregion

        #region 垂直扫描
        public Int32[,] VerScanFunc(Int16 XPos, Int16 YStartPos, Int16 YEndPos, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            CShareData.VerScanFlag = false;
            //-----------------------------------S----子类--Len_H-Len_L-Scan-----XPos------YStartPos----YEndPos---Delay---MF----SA---Check 
            byte[] VerScanCommand = new byte[] { 0x53, 0x02, 0x00, 0x0A, 0x02, 0x00, 0x00, 0x00, 0x1A, 0xFF, 0xE6, 0x00, 0x03, 0x03, 0xCA };
            Int16 ScanTime = 2;      //the Scan time (default:2)
            Int16 PointCnt = Convert.ToInt16(((YStartPos - YEndPos) << 1) + 1);  //the number of distance,default:109 
            VerScanCommand[4] = Convert.ToByte(ScanTime & 0x00FF);
            VerScanCommand[5] = Convert.ToByte((XPos >> 8) & 0x00FF);
            VerScanCommand[6] = Convert.ToByte((XPos >> 0) & 0x00FF);
            VerScanCommand[7] = Convert.ToByte((YStartPos >> 8) & 0x00FF);
            VerScanCommand[8] = Convert.ToByte((YStartPos >> 0) & 0x00FF);
            VerScanCommand[9] = Convert.ToByte((YEndPos >> 8) & 0x00FF);
            VerScanCommand[10] = Convert.ToByte((YEndPos >> 0) & 0x00FF);

            Int32[,] VerScanDistance = new Int32[ScanTime, PointCnt];  //读取的距离值
            CShareData.spReceivedDataLenIdeal = (PointCnt << 1) + 16;     //every scan you should receive 226 bytes -- 0x53 0x01 Len_H Len_L 0x00 0x00 0x00 ---- 2*N-data CheckSum
            DataWrite(serialPort1, VerScanCommand);   //send command

            for (int i = 0; i < ScanTime; i++)
            //int i = 0;
            {
                Int32 WaitTime = CShareData.spSemTimeout + Math.Abs(CShareData.XShift) + Math.Abs(CShareData.YShift);
                if (CShareData.spSem.WaitOne(WaitTime) == false)    //wait sem
                {
                    LabelUpdate(Status, "串口接收等待超时！");
                    goto Error;
                }

                lock (CShareData.spMutex)
                {
                    if (CShareData.spErrorFlag)
                    {
                        LabelUpdate(Status, "串口错误！");
                        goto Error;
                    }

                    if (CShareData.spReceivedDataLen == CShareData.spReceivedDataLenIdeal)  //长度
                    {
                        if (JudegCheckSum(CShareData.spReceivedData, CShareData.spReceivedDataLen)) //校验和
                        {
                            if ((CShareData.spReceivedData[0] == 0x53) && (CShareData.spReceivedData[1] == 0x02))
                            {
                                for (int j = 0; j < PointCnt; j++)
                                {
                                    //find the value of every measure,and change to real distance(mm)
                                    if (CShareData.spReceivedData[6] == 0x01)    //若是反向扫描，纠正数据排列顺序
                                        VerScanDistance[i, PointCnt - 1 - j] = 4 * (CShareData.spReceivedData[2 * j + 15] * 256 + CShareData.spReceivedData[2 * j + 16]);
                                    else
                                        VerScanDistance[i, j] = 4 * (CShareData.spReceivedData[2 * j + 15] * 256 + CShareData.spReceivedData[2 * j + 16]);
                                }

                                byte FrameNum = CShareData.spReceivedData[4];
                                if (FrameNum != i)
                                {
                                    LabelUpdate(Status, "扫描帧号乱序!");
                                    goto Error;
                                }

                                if ((CShareData.spReceivedData[5] == 0x00) && (i != ScanTime - 1))    //如果读取一部分就没有后续帧，直接停止读数据
                                {
                                    LabelUpdate(Status, "扫描数据未读完!");
                                    goto Error;
                                }
                            }
                            else
                            {
                                ErrorCheck(Status);  //检查错误
                                goto Error;
                            }
                        }
                        else
                        {
                            LabelUpdate(Status, "校验证和错误！");
                            goto Error;
                        }

                    }
                    else
                    {
                        ErrorCheck(Status);  //检查错误                    
                        goto Error;
                    }
                }
            }
            //LabelUpdate(Status,"扫描成功!");
            CShareData.VerScanFlag = true; //无错误goto,则扫描成功
            CShareData.YShift = YStartPos;
            ProgressBarUpdate(pBStatus, 100);
            return VerScanDistance;

        Error:
            CShareData.VerScanFlag = false;
            ProgressBarUpdate(pBStatus, 0);
            return VerScanDistance;
        }
        #endregion

        #region 三维扫描
        public bool ThreeDScanFunc(Int16 YStartPos, Int16 YEndPos, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            LabelUpdate(Status, "扫描范围--垂直步数：" + YStartPos.ToString() + ":" + YEndPos.ToString());

            Int16 ScanPointNum = 109;
            Int32[,] ReceivedDistance = new Int32[2, ScanPointNum];     //读取水平扫描距离值
            Int16 XStartPos = Convert.ToInt16(0 - ((ScanPointNum - 1) >> 2));
            Int16 XEndPos = Convert.ToInt16((ScanPointNum - 1) >> 2);

            for (Int16 YPos = YStartPos; YPos >= YEndPos; YPos--)
            {
                ProgressBarUpdate(pBStatus, Convert.ToInt16(100.0 * (YStartPos - YPos) / (YStartPos - YEndPos)));

                ReceivedDistance = HrzScanFunc(YPos, XStartPos, XEndPos, serialPort1, Status, null);   //调用水平扫描函数
                if (CShareData.HrzScanFlag)
                {
                    string path = CShareData.StartPath + "三维扫描" + YPos.ToString() + ".txt";
                    SaveScanDistance(path, ReceivedDistance, ScanPointNum, 2);
                }
                else
                {
                    LabelUpdate(Status, "水平扫描失败--垂直步数" + YPos.ToString());
                    return false;
                }
            }
            ProgressBarUpdate(pBStatus, 100);
            return true;
        }
        #endregion

        #region  上下左右移动
        //上正下负  左负右正
        public bool MoveFunc(CShareData.MoveDirection MoveDir, Int16 MoveStep, CShareData.SetZeroValue SetZero, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            //-----------------------------------B---子类--Len_H-Len_L---X_H--X_L----F_H--F_L----Y_H--Y_L----F_H--F_L---Zero--Check 
            byte[] MoveCommand = new byte[] { 0x42, 0x01, 0x00, 0x09, 0x00, 0x00, 0x07, 0xD0, 0x00, 0x00, 0x07, 0xD0, 0x00, 0xCA };
            CShareData.spReceivedDataLenIdeal = 5;       //you should receive 5 bytes -- 0x42 0x01 0x00 0x00 0xbd
            MoveCommand[12] = Convert.ToByte(SetZero);  //是否设为零点
            switch (MoveDir)
            {
                case CShareData.MoveDirection.Up:
                    MoveCommand[8] = Convert.ToByte((MoveStep & 0xFF00) >> 8);
                    MoveCommand[9] = Convert.ToByte(MoveStep & 0x00FF);
                    break;
                case CShareData.MoveDirection.Down:
                    MoveCommand[8] = Convert.ToByte(((0 - MoveStep) & 0x0000FF00) >> 8);
                    MoveCommand[9] = Convert.ToByte((0 - MoveStep) & 0x000000FF);
                    break;
                case CShareData.MoveDirection.Left:
                    MoveCommand[4] = Convert.ToByte(((0 - MoveStep) & 0x0000FF00) >> 8);
                    MoveCommand[5] = Convert.ToByte((0 - MoveStep) & 0x000000FF);
                    break;
                case CShareData.MoveDirection.Right:
                    MoveCommand[4] = Convert.ToByte((MoveStep & 0xFF00) >> 8);
                    MoveCommand[5] = Convert.ToByte(MoveStep & 0x00FF);
                    break;
                default:
                    break;
            }
            DataWrite(serialPort1, MoveCommand);   //send command
            Int32 WaitTme = CShareData.spSemTimeout + MoveStep;
            if (CShareData.spSem.WaitOne(WaitTme) == false)    //wait sem
            {
                string str = "--" + CShareData.spReceivedDataLen.ToString() + "--" + CShareData.ReadDataLen.ToString() + "--" + CShareData.spReceivedDataLenIdeal.ToString();
                LabelUpdate(Status, "串口接收等待超时！" + str);
                ProgressBarUpdate(pBStatus, 0);
                return false;
            }
            lock (CShareData.spMutex)
            {
                if (CShareData.spErrorFlag)
                {
                    LabelUpdate(Status, "串口错误！");
                    ProgressBarUpdate(pBStatus, 0);
                    return false;
                }

                if (CShareData.spReceivedDataLen == CShareData.spReceivedDataLenIdeal)  //长度
                {
                    if (JudegCheckSum(CShareData.spReceivedData, CShareData.spReceivedDataLen)) //校验和
                    {
                        if ((CShareData.spReceivedData[0] == 0x42) && (CShareData.spReceivedData[1] == 0x01))
                        {
                            if (SetZero == CShareData.SetZeroValue.Set)
                            {
                                CShareData.XShift = 0;
                                CShareData.YShift = 0;
                            }
                            else
                            {
                                switch (MoveDir)
                                {
                                    case CShareData.MoveDirection.Up:
                                        CShareData.YShift += MoveStep;
                                        break;
                                    case CShareData.MoveDirection.Down:
                                        CShareData.YShift -= MoveStep;
                                        break;
                                    case CShareData.MoveDirection.Left:
                                        CShareData.XShift -= MoveStep;
                                        break;
                                    case CShareData.MoveDirection.Right:
                                        CShareData.XShift += MoveStep;
                                        break;
                                    default:
                                        break;
                                }
                            }

                            ProgressBarUpdate(pBStatus, 100);
                            return true;
                        }
                        else
                        {
                            ErrorCheck(Status);  //检查错误
                            ProgressBarUpdate(pBStatus, 0);
                            return false;
                        }
                    }
                    else
                    {
                        LabelUpdate(Status, "校验证和错误！");
                        ProgressBarUpdate(pBStatus, 0);
                        return false;
                    }

                }
                else
                {
                    ErrorCheck(Status);  //检查错误
                    LabelUpdate(Status, "仅接收到" + CShareData.spReceivedDataLen.ToString() + "个数据！");
                    ProgressBarUpdate(pBStatus, 0);
                    return false;
                }
            }
        }
        #endregion

                                                                #region 回零，置零
        public bool ReturnZeroFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            //------------------------------------------B---子类--Len_H-Len_L--Check
            byte[] ReturnZeroCommand = new byte[] { 0x42, 0x05, 0x00, 0x00, 0xca };
            CShareData.spReceivedDataLenIdeal = 5;   //you should receive 5 bytes -- 0x42 0x05 0x00 0x00 0xb9

            DataWrite(serialPort1, ReturnZeroCommand);   //send command

            //ShiftRefreshFunc(serialPort1, Status, pBStatus);
            Int32 WaitTime = CShareData.spSemTimeout + Math.Abs(CShareData.XShift) + Math.Abs(CShareData.YShift);
            if (CShareData.spSem.WaitOne(WaitTime) == false)    //wait sem
            {
                LabelUpdate(Status, "串口接收等待超时！");
                ProgressBarUpdate(pBStatus, 0);
                return false;
            }
            lock (CShareData.spMutex)
            {
                if (CShareData.spErrorFlag)
                {
                    LabelUpdate(Status, "串口错误！");
                    ProgressBarUpdate(pBStatus, 0);
                    return false;
                }

                if (CShareData.spReceivedDataLen == CShareData.spReceivedDataLenIdeal)  //长度
                {
                    if (JudegCheckSum(CShareData.spReceivedData, CShareData.spReceivedDataLen)) //校验和
                    {
                        if ((CShareData.spReceivedData[0] == 0x42) && (CShareData.spReceivedData[1] == 0x05))
                        {
                            CShareData.XShift = 0;
                            CShareData.YShift = 0;

                            ProgressBarUpdate(pBStatus, 100);
                            return true;
                        }
                        else
                        {
                            ErrorCheck(Status);  //检查错误
                            ProgressBarUpdate(pBStatus, 0);
                            return false;
                        }
                    }
                    else
                    {
                        LabelUpdate(Status, "校验证和错误！");
                        ProgressBarUpdate(pBStatus, 0);
                        return false;
                    }

                }
                else
                {
                    ErrorCheck(Status);  //检查错误
                    LabelUpdate(Status, "仅接收到" + CShareData.spReceivedDataLen.ToString() + "个数据！");
                    ProgressBarUpdate(pBStatus, 0);
                    return false;
                }
            }
        }

        public bool SetZeroFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);     
            if (MoveFunc(CShareData.MoveDirection.None, 0, CShareData.SetZeroValue.Set, serialPort1, Status, null))
            {
                ProgressBarUpdate(pBStatus, 100);
                return true;
            }
            else
            {
                ProgressBarUpdate(pBStatus, 0);
                return false;
            }
        }
        #endregion

        #region 刷新电机位置
        public bool ShiftRefreshFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            //-----------------------------------------B---子类---Len_H-Len_L--Check 
            byte[] ShiftRefreshCommand = new byte[] { 0x5A, 0x05, 0x00, 0x00, 0xCA };
            CShareData.spReceivedDataLenIdeal = 9;       //you should receive 9 bytes --

            DataWrite(serialPort1, ShiftRefreshCommand);   //send command
            if (CShareData.spSem.WaitOne(CShareData.spSemTimeout) == false)    //wait sem
            {
                LabelUpdate(Status, "串口接收等待超时！");
                ProgressBarUpdate(pBStatus, 0);
                return false;
            }
            lock (CShareData.spMutex)
            {
                if (CShareData.spErrorFlag)
                {
                    LabelUpdate(Status, "串口错误！");
                    ProgressBarUpdate(pBStatus, 0);
                    return false;
                }

                if (CShareData.spReceivedDataLen == CShareData.spReceivedDataLenIdeal)  //长度
                {
                    if (JudegCheckSum(CShareData.spReceivedData, CShareData.spReceivedDataLen)) //校验和
                    {
                        if ((CShareData.spReceivedData[0] == 0x5A) && (CShareData.spReceivedData[1] == 0x05))
                        {
                            int ShiftValue = (CShareData.spReceivedData[4] << 8) + CShareData.spReceivedData[5];
                            if (CShareData.spReceivedData[4] >= 0x80) //负数   
                            {
                                CShareData.XShift = Convert.ToInt16(0 - (65536 - ShiftValue));
                            }
                            else
                            {
                                CShareData.XShift = Convert.ToInt16(ShiftValue);
                            }
                            ShiftValue = (CShareData.spReceivedData[6] << 8) + CShareData.spReceivedData[7];
                            if (CShareData.spReceivedData[6] >= 0x80) //负数   
                            {
                                CShareData.YShift = Convert.ToInt16(0 - (65536 - ShiftValue));
                            }
                            else
                            {
                                CShareData.YShift = Convert.ToInt16(ShiftValue);
                            }

                            ProgressBarUpdate(pBStatus, 100);
                            return true;
                        }
                        else
                        {
                            ErrorCheck(Status);  //检查错误
                            ProgressBarUpdate(pBStatus, 0);
                            return false;
                        }
                    }
                    else
                    {
                        LabelUpdate(Status, "校验证和错误！");
                        ProgressBarUpdate(pBStatus, 0);
                        return false;
                    }

                }
                else
                {
                    ErrorCheck(Status);  //检查错误
                    LabelUpdate(Status, "仅接收到" + CShareData.spReceivedDataLen.ToString() + "个数据！");
                    ProgressBarUpdate(pBStatus, 0);
                    return false;
                }
            }
        }
        #endregion

        #region 水平、垂直确认边框  查看概率

        #region 水平确认边框
        public CShareData.ReturnValueEdge XConfirmEdgeFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            ReturnZeroFunc(serialPort1, Status, null);   //先回零

            Int16 XEdgeStep = 0;    //距离边框的步数
            Int16 XEdgeIndexStep = 0;   //边框在一帧扫描数据中的位置
            Int16 ScanZeroMove = 50;   //每次扫描零点移动的步数
            Int16 ScanStepRange = CShareData.XMOTORRANGE;   //每次扫描电机转动步数
            Int16 OnePointMeasureNum = 1000;
            double ProbMax = 0.5 + CShareData.XEDGEPROBLIMIT;
            double ProbMin = 0.5 - CShareData.XEDGEPROBLIMIT;
            double ProbMid = (ProbMax + ProbMin) / 2;

            Int16 XStartPos = 0;
            Int16 XEndPos = ScanStepRange;
            Int16 PointCnt = Convert.ToInt16(((XEndPos - XStartPos) << 1) + 1);  //the number of distance,default:109


            Int32[,] ReceivedDistance = new Int32[2, PointCnt];     //接收到的水平扫描距离值


            int EdgeIndexPos = 0;     //正向数据边界下标
            int EdgeIndexNeg = 0;     //反向数据边界下标
            bool ScanFindEdgeFlag = false;  //找到边框
            bool EdgeInFrame = false;   //此次扫描是否包含边界
            for (int i = 0; i < 64; i++)    //能保证转1周时，应取3200/ZeroMove
            {
                if (i > 4)
                {
                    LabelUpdate(Status, "镜子已经跑飞！");
                    ProgressBarUpdate(pBStatus, 0);
                    return CShareData.ReturnValueEdge.ERROR_HRZ_ZERO;
                }

                ScanFindEdgeFlag = false;
                EdgeInFrame = false;

                ReceivedDistance = HrzScanFunc(0, XStartPos, XEndPos, serialPort1, Status, null);   //调用水平扫描函数

                if (CShareData.DEBUG)
                {
                    string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                    string path = CShareData.StartPath + "水平确认边框" + TimeNow + ".txt";
                    SaveScanDistance(path, ReceivedDistance, PointCnt, 2);      //保存扫描数据
                }




                if (CShareData.HrzScanFlag)
                {
                    Int16 ValidPointBegin = 0;
                    Int16 ValidPointEnd = 0;
                    for (int m = 0; m < 10; m++)//反向数据中前10个在箱子外
                    {
                        if ((ReceivedDistance[1, m] <= 4) || (ReceivedDistance[1, m] >= CShareData.BOXMAXDISTANCE))  //第1个点在箱子外
                        {
                            ValidPointBegin++;
                        }

                        if ((ReceivedDistance[1, PointCnt - 1 - m] > 4) && (ReceivedDistance[1, PointCnt - 1 - m] < CShareData.BOXMAXDISTANCE))//最后一个在箱子内
                        {
                            ValidPointEnd++;
                        }
                    }

                    if ((ValidPointBegin >= 2) && (ValidPointEnd >= 2))
                    {
                        EdgeInFrame = true;
                    }
                    //if ((ReceivedDistance[0, 0] <= 4) || (ReceivedDistance[0, 0] >= CShareData.BOXMAXDISTANCE))//第1个点在箱子外
                    //{
                    //    if ((ReceivedDistance[0, PointCnt - 1] > 4) && (ReceivedDistance[0, PointCnt - 1] < CShareData.BOXMAXDISTANCE))//最后一个在箱子内
                    //    {
                    //        EdgeInFrame = true;
                    //    }
                    //}
                    if (EdgeInFrame)    //此次扫描数据中包含边界,再确定边界对应一帧数据中哪个点
                    {
                        break;
                    }
                    else //此次扫描数据中没有包含边界
                    {
                        XEdgeStep += ScanZeroMove;
                        XStartPos += ScanZeroMove;
                        XEndPos = Convert.ToInt16(XStartPos + ScanStepRange);
                    }
                }
            }

            ProgressBarUpdate(pBStatus, 10);
            if (EdgeInFrame)    //此次扫描数据中包含边界,再确定边界对应一帧数据中哪个点
            {
                for (int j = 1; j < PointCnt; j++)   //正向数据
                {
                    if ((ReceivedDistance[0, j] > 4) && (ReceivedDistance[0, j] < CShareData.BOXMAXDISTANCE))   //落在箱子内
                    {
                        if ((ReceivedDistance[0, j - 1] <= 4) || (ReceivedDistance[0, j - 1] >= CShareData.BOXMAXDISTANCE))      //前一个点未落在箱子内（右边界）
                        {
                            EdgeIndexPos = j;
                            ScanFindEdgeFlag = true;
                            break;
                        }
                    }
                }
                for (int j = 1; j < PointCnt; j++)   //反向数据
                {
                    if ((ReceivedDistance[1, j] > 4) && (ReceivedDistance[1, j] < CShareData.BOXMAXDISTANCE))   //落在箱子内
                    {
                        if ((ReceivedDistance[1, j - 1] <= 4) || (ReceivedDistance[1, j - 1] >= CShareData.BOXMAXDISTANCE))      //前一个点未落在箱子内（右边界）
                        {
                            EdgeIndexNeg = j;
                            ScanFindEdgeFlag = true;
                            break;
                        }
                    }
                }
            }


            Int16 InSideWindow = 0;
            double InSideWindowProb = 0;     //落在箱子内的概率
            double LastInSideWindowProb = 0;
            bool FindEdgeFlag = false;     //确定边框成功标志
            if (ScanFindEdgeFlag)   //扫描的方式找到边框
            {
                XEdgeIndexStep = Convert.ToInt16((EdgeIndexPos + EdgeIndexNeg) >> 2);
                XEdgeStep += XEdgeIndexStep;

                //粗略定位到右移到边框的边界
                MoveFunc(CShareData.MoveDirection.Right, XEdgeIndexStep, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);

                InSideWindow = 0;
                InSideWindowProb = 0;
                LastInSideWindowProb = 0;
                for (int i = 0; i < ScanStepRange; i++)        //最多定位52次，防止死循环
                {
                    string str = "X定点测量次数：" + i.ToString();
                    LabelUpdate(Status, str);

                    //InSideWindow = 0;
                    //OnePointMeasureFunc(OnePointMeasureNum,serialPort1,Status,null);    //定点测距
                    //for (int j = 0; j < OnePointMeasureNum; j++)
                    //{
                    //    if ((CShareData.OnePointDistance[j] > 4) && (CShareData.OnePointDistance[j] < CShareData.BOXMAXDISTANCE))
                    //    {
                    //        InSideWindow++;
                    //    }
                    //}
                    LastInSideWindowProb = InSideWindowProb;
                    //InSideWindowProb = Convert.ToDouble(InSideWindow) / OnePointMeasureNum;   //落在箱子内的概率
                    InSideWindowProb = GetEdgeProbFunc(CShareData.NTIMEEDGEX, serialPort1, Status, null);

                    if (i == 0)     //如果没有这句，第一次判断有可能满足条件，InSideWindowProb=1，LastInSideWindowProb=0
                    {
                        LastInSideWindowProb = InSideWindowProb;
                    }

                    //判断落在边框内的概率，最稳定的情况为45%~55%
                    if ((InSideWindowProb >= ProbMin) && (InSideWindowProb <= ProbMax))
                    {
                        FindEdgeFlag = true;
                        break;

                    }
                    else if (InSideWindowProb < ProbMax)
                    {
                        if (LastInSideWindowProb > ProbMin)    //上次概率太大，这次概率太小
                        {
                            //比较哪个更接近ProbMid
                            if (Math.Abs(InSideWindowProb - ProbMid) > Math.Abs(LastInSideWindowProb - ProbMid))    //上次更接近ProbMid
                            {

                                InSideWindowProb = LastInSideWindowProb;  //保存上次概率
                                //调整到上次的位置
                                MoveFunc(CShareData.MoveDirection.Right, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                                XEdgeStep += 1;
                            }

                            FindEdgeFlag = true;
                            break;
                        }
                        else
                        {
                            MoveFunc(CShareData.MoveDirection.Right, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                            XEdgeStep += 1;
                        }
                    }
                    else if (InSideWindowProb > ProbMin)
                    {
                        if (LastInSideWindowProb < ProbMax) //上次概率太小，这次概率太大
                        {
                            //比较哪个更接近ProbMid
                            if (Math.Abs(InSideWindowProb - ProbMid) > Math.Abs(LastInSideWindowProb - ProbMid))    //上次更接近ProbMid
                            {

                                InSideWindowProb = LastInSideWindowProb;  //保存上次概率
                                //调整到上次的位置
                                MoveFunc(CShareData.MoveDirection.Left, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                                XEdgeStep -= 1;
                            }

                            FindEdgeFlag = true;
                            break;
                        }
                        else
                        {
                            MoveFunc(CShareData.MoveDirection.Left, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                            XEdgeStep -= 1;
                        }
                    }
                    if (i == ScanStepRange)
                    {
                        LabelUpdate(Status, "单步移动超时！");
                        ProgressBarUpdate(pBStatus, 80);
                        return CShareData.ReturnValueEdge.ERROR_HRZ_STEP;
                    }

                }

            }
            else
            {
                LabelUpdate(Status, "未扫描到边框！");
                ProgressBarUpdate(pBStatus, 80);
                return CShareData.ReturnValueEdge.ERROR_HRZ_SCAN;
            }

            ProgressBarUpdate(pBStatus, 80);

            //让电机停稳，二次统计概率
            if (FindEdgeFlag)
            {
                Thread.Sleep(20); //防止因镜子振动，导致电机位置未停稳，概率误差较大
                OnePointMeasureFunc(OnePointMeasureNum, serialPort1, Status, null);    //定点测距
                InSideWindow = 0;
                for (int j = 0; j < OnePointMeasureNum; j++)
                {
                    if ((CShareData.OnePointDistance[j] > 4) && (CShareData.OnePointDistance[j] < CShareData.BOXMAXDISTANCE))
                    {
                        InSideWindow++;
                    }
                }
                InSideWindowProb = Convert.ToDouble(InSideWindow) / OnePointMeasureNum;   //落在箱子内的概率
            }

            ProgressBarUpdate(pBStatus, 95);
            if (FindEdgeFlag)   //概率确认
            {
                if ((InSideWindowProb < 0.2) || (InSideWindowProb > 0.8))
                {
                    LabelUpdate(Status, "概率统计错误！");
                    return CShareData.ReturnValueEdge.ERROR_HRZ_PROB;
                }
            }

            if (FindEdgeFlag)   //完成边框确认
            {
                //MoveLeftFunc(XEdgeStep, 1);  //移动到之前的零点，并置位
                CShareData.XEdgeStepSave = XEdgeStep;
                CShareData.XProbSave = Math.Round(InSideWindowProb, 3);
            }

            if (FindEdgeFlag)
            {
                ProgressBarUpdate(pBStatus, 100);
                return CShareData.ReturnValueEdge.SUCCESS;
            }
            else
            {
                ProgressBarUpdate(pBStatus, 0);
                return CShareData.ReturnValueEdge.FAILURE;
            }
        }

        #endregion

        #region 垂直边框确认
        public CShareData.ReturnValueEdge YConfirmEdgeFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            ReturnZeroFunc(serialPort1, Status, null);   //先回零

            Int16 YEdgeStep = 0;    //距离边框的步数
            Int16 YEdgeIndexStep = 0;   //边框在一帧扫描数据中的位置
            Int16 ScanZeroMove = 50;   //每次扫描零点移动的步数
            Int16 ScanStepRange = CShareData.YMOTORRANGE;   //每次扫描电机转动步数
            double ProbMax = 0.5 + CShareData.YEDGEPROBLIMIT;
            double ProbMin = 0.5 - CShareData.YEDGEPROBLIMIT;
            double ProbMid = (ProbMax + ProbMin) / 2;

            Int16 YStartPos = 0;
            Int16 YEndPos = Convert.ToInt16(YStartPos - ScanStepRange);
            Int16 PointCnt = Convert.ToInt16(((YStartPos - YEndPos) << 1) + 1);  //the number of distance,default:109

            Int32[,] ReceivedDistance = new Int32[2, PointCnt];     //接收到的水平扫描距离值

            int EdgeIndexPos = 0;     //正向数据边界下标
            int EdgeIndexNeg = 0;     //反向数据边界下标
            bool ScanFindEdgeFlag = false;  //找到边框
            bool EdgeInFrame = false;   //此次扫描是否包含边界


            for (int i = 0; i < (CShareData.YCIRCLESTEP / ScanZeroMove); i++)    //能保证转1周时，应取3200/ZeroMove
            {
                if (i > 12)
                {
                    LabelUpdate(Status, "镜子已经跑飞！");
                    ProgressBarUpdate(pBStatus, 80);
                    return CShareData.ReturnValueEdge.ERROR_VER_ZERO;
                    //return false;
                }

                ScanFindEdgeFlag = false;
                EdgeInFrame = false;

                ReceivedDistance = VerScanFunc(0, YStartPos, YEndPos, serialPort1, Status, null);   //调用水平扫描函数

                if (CShareData.DEBUG)
                {
                    string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                    string path = CShareData.StartPath + "垂直确认边框" + TimeNow + ".txt";
                    SaveScanDistance(path, ReceivedDistance, PointCnt, 2);      //保存扫描数据
                }


                if (CShareData.VerScanFlag)    //正向数据有可能会有个别异常点，因此垂直扫描判断反向数据
                {
                    Int16 ValidPointBegin = 0;
                    Int16 ValidPointEnd = 0;
                    for (int m = 0; m < 10; m++)//反向数据中前10个在箱子外
                    {
                        if ((ReceivedDistance[1, m] <= 4) || (ReceivedDistance[1, m] >= CShareData.BOXMAXDISTANCE))  //第1个点在箱子外
                        {
                            ValidPointBegin++;
                        }

                        if ((ReceivedDistance[1, PointCnt - 1 - m] > 4) && (ReceivedDistance[1, PointCnt - 1 - m] < CShareData.BOXMAXDISTANCE))//最后一个在箱子内
                        {
                            ValidPointEnd++;
                        }
                    }

                    if ((ValidPointBegin >= 2) && (ValidPointEnd >= 2))
                    {
                        EdgeInFrame = true;
                    }
                    //for (int m = 0; m < 10; m++)//反向数据中前10个在箱子外
                    //{
                    //    if ((ReceivedDistance[1, 0] <= 4) || (ReceivedDistance[1, 0] >= CShareData.BOXMAXDISTANCE))  //第1个点在箱子外
                    //    {
                    //        if (((ReceivedDistance[1, PointCnt - 1] > 4) && (ReceivedDistance[1, PointCnt - 1] < CShareData.BOXMAXDISTANCE)) && ((ReceivedDistance[1, PointCnt - 2] > 4) && (ReceivedDistance[1, PointCnt - 2] < CShareData.BOXMAXDISTANCE)))//最后一个在箱子内
                    //        {
                    //            EdgeInFrame = true;                                
                    //        }
                    //        break;
                    //    }
                    //}
                    if (EdgeInFrame)    //此次扫描数据中包含边界,再确定边界对应一帧数据中哪个点
                    {
                        break;
                    }
                    else //此次扫描数据中没有包含边界
                    {
                        YEdgeStep += ScanZeroMove;
                        YStartPos -= ScanZeroMove;
                        YEndPos = Convert.ToInt16(YStartPos - ScanStepRange);
                    }
                }
            }

            ProgressBarUpdate(pBStatus, 10);
            if (EdgeInFrame)    //此次扫描数据中包含边界,再确定边界对应一帧数据中哪个点
            {
                for (int j = 1; j < PointCnt; j++)   //正向数据
                {
                    if ((ReceivedDistance[0, j] > 4) && (ReceivedDistance[0, j] < CShareData.BOXMAXDISTANCE))   //落在箱子内
                    {
                        if ((ReceivedDistance[0, j - 1] <= 4) || (ReceivedDistance[0, j - 1] >= CShareData.BOXMAXDISTANCE))      //前一个点未落在箱子内（右边界）
                        {
                            EdgeIndexPos = j;
                            ScanFindEdgeFlag = true;
                            break;
                        }
                    }
                }
                for (int j = 1; j < PointCnt; j++)   //反向数据
                {
                    if ((ReceivedDistance[1, j] > 4) && (ReceivedDistance[1, j] < CShareData.BOXMAXDISTANCE))   //落在箱子内
                    {
                        if ((ReceivedDistance[1, j - 1] <= 4) || (ReceivedDistance[1, j - 1] >= CShareData.BOXMAXDISTANCE))      //前一个点未落在箱子内（右边界）
                        {
                            EdgeIndexNeg = j;
                            ScanFindEdgeFlag = true;
                            break;
                        }
                    }
                }
            }



            //Int16 InSideWindow = 0;
            double InSideWindowProb = 0;     //落在箱子内的概率
            double LastInSideWindowProb = 0;
            bool FindEdgeFlag = false;     //确定边框成功标志
            if (ScanFindEdgeFlag)   //扫描的方式找到边框
            {
                YEdgeIndexStep = Convert.ToInt16((EdgeIndexPos + EdgeIndexNeg) >> 2);
                YEdgeStep += YEdgeIndexStep;

                //粗略定位到下边框的边界
                MoveFunc(CShareData.MoveDirection.Down, YEdgeIndexStep, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);

                //InSideWindow = 0;
                InSideWindowProb = 0;
                LastInSideWindowProb = 0;
                for (int i = 0; i < ScanStepRange; i++)        //最多定位52次，防止死循环
                {
                    string str = "Y定点测量次数：" + i.ToString();
                    LabelUpdate(Status, str);
                    //InSideWindow = 0;
                    //NTimeOnePointMeasureFunc(CShareData.NTIMEEDGEY, serialPort1, Status, null);    //定点测距
                    //for (int j = 0; j < CShareData.NTimeOnePointDistanceLen; j++)
                    //{
                    //    if ((CShareData.NTimeOnePointDistance[j] > 4) && (CShareData.NTimeOnePointDistance[j] < CShareData.BOXMAXDISTANCE))
                    //    {
                    //        InSideWindow++;
                    //    }
                    //}
                    LastInSideWindowProb = InSideWindowProb;
                    //InSideWindowProb = Convert.ToDouble(InSideWindow) / CShareData.NTimeOnePointDistanceLen;   //落在箱子内的概率
                    InSideWindowProb = GetEdgeProbFunc(CShareData.NTIMEEDGEY, serialPort1, Status, null);
                    if (i == 0)     //如果没有这句，第一次判断有可能满足条件，InSideWindowProb=1，LastInSideWindowProb=0
                    {
                        LastInSideWindowProb = InSideWindowProb;
                    }

                    //判断落在边框内的概率，最稳定的情况为45%~55%
                    if ((InSideWindowProb >= ProbMin) && (InSideWindowProb <= ProbMax))
                    {
                        FindEdgeFlag = true;
                        break;

                    }
                    else if (InSideWindowProb < ProbMax)    //上次概率太大，这次概率太小
                    {
                        if (LastInSideWindowProb > ProbMin)
                        {
                            //比较哪个更接近ProbMid
                            if (Math.Abs(InSideWindowProb - ProbMid) > Math.Abs(LastInSideWindowProb - ProbMid))    //上次更接近ProbMid
                            {

                                InSideWindowProb = LastInSideWindowProb;  //保存上次概率
                                //调整到上次的位置
                                MoveFunc(CShareData.MoveDirection.Down, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                                YEdgeStep += 1;
                            }

                            FindEdgeFlag = true;
                            break;
                        }
                        else
                        {
                            if (Math.Abs(InSideWindowProb - ProbMid) < 0.3)
                            {
                                MoveFunc(CShareData.MoveDirection.Down, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                                YEdgeStep += 1;
                            }
                            else
                            {
                                MoveFunc(CShareData.MoveDirection.Down, 3, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                                YEdgeStep += 3;
                            }
                        }
                    }
                    else if (InSideWindowProb > ProbMin)
                    {
                        if (LastInSideWindowProb < ProbMax) //上次概率太小，这次概率太大
                        {
                            //比较哪个更接近ProbMid
                            if (Math.Abs(InSideWindowProb - ProbMid) > Math.Abs(LastInSideWindowProb - ProbMid))    //上次更接近ProbMid
                            {

                                InSideWindowProb = LastInSideWindowProb;  //保存上次概率
                                //调整到上次的位置
                                MoveFunc(CShareData.MoveDirection.Up, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                                YEdgeStep -= 1;
                            }
                            FindEdgeFlag = true;
                            break;
                        }
                        else
                        {
                            if (Math.Abs(InSideWindowProb - ProbMid) < 0.3)
                            {
                                MoveFunc(CShareData.MoveDirection.Up, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                                YEdgeStep -= 1;
                            }
                            else
                            {
                                MoveFunc(CShareData.MoveDirection.Up, 3, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                                YEdgeStep -= 3;
                            }
                        }
                    }



                    if (i == ScanStepRange)
                    {
                        LabelUpdate(Status, "单步移动超时！");
                        ProgressBarUpdate(pBStatus, 0);
                        return CShareData.ReturnValueEdge.ERROR_VER_STEP;
                        //return false;
                    }

                }

            }
            else
            {
                LabelUpdate(Status, "未扫描到边框！");
                ProgressBarUpdate(pBStatus, 0);
                return CShareData.ReturnValueEdge.ERROR_VER_SCAN;
                //return false;
            }

            ProgressBarUpdate(pBStatus, 80);
            //让电机停稳，二次统计概率
            if (FindEdgeFlag)
            {
                Thread.Sleep(20); //防止因镜子振动，导致电机位置未停稳，概率误差较大
                //NTimeOnePointMeasureFunc(CShareData.NTIMEEDGEY, serialPort1, Status, null);    //定点测距

                //InSideWindow = 0;
                //for (int j = 0; j < CShareData.NTimeOnePointDistanceLen; j++)
                //{
                //    if ((CShareData.NTimeOnePointDistance[j] > 4) && (CShareData.NTimeOnePointDistance[j] < CShareData.BOXMAXDISTANCE))
                //    {
                //        InSideWindow++;
                //    }
                //}
                //InSideWindowProb = Convert.ToDouble(InSideWindow) / CShareData.NTimeOnePointDistanceLen;   //落在箱子内的概率
                InSideWindowProb = GetEdgeProbFunc(CShareData.NTIMEEDGEY, serialPort1, Status, null);
            }




            ProgressBarUpdate(pBStatus, 90);
            if (FindEdgeFlag)   //概率确认
            {
                if ((InSideWindowProb < 0.2) || (InSideWindowProb > 0.8))
                {
                    LabelUpdate(Status, "概率统计错误！");
                    return CShareData.ReturnValueEdge.ERROR_VER_PROB;
                    //FindEdgeFlag = false;
                }
            }

            if (FindEdgeFlag)   //完成边框确认
            {
                CShareData.YEdgeStepSave = YEdgeStep;
                CShareData.YProbSave = Math.Round(InSideWindowProb, 3);
            }
            if (FindEdgeFlag)
            {
                ProgressBarUpdate(pBStatus, 100);
                return CShareData.ReturnValueEdge.SUCCESS;
            }
            else
            {
                ProgressBarUpdate(pBStatus, 0);
                return CShareData.ReturnValueEdge.FAILURE;
            }

            //return FindEdgeFlag;
        }
        #endregion

        #region 水平、垂直确认边框
        public CShareData.ReturnValueEdge XYConfirmEdgeFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            CShareData.ReturnValueEdge Result = XYConfirmEdgeBasicFunc(serialPort1, Status, null);
            if (CShareData.ReturnValueEdge.SUCCESS != Result)
            {
                LabelUpdate(Status, "----------------------------第1次确认边框失败，再次确认！");
                Result = XYConfirmEdgeBasicFunc(serialPort1, Status, null);
            }
            return Result;
        }
        public CShareData.ReturnValueEdge XYConfirmEdgeBasicFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            bool XConfirmEdgeFlag = false;
            bool YConfirmEdgeFlag = false;
            //int XYConfirmEdgeResult = 0;            

            LabelUpdate(Status, "水平确认边框……");
            if (CShareData.ReturnValueEdge.SUCCESS == XConfirmEdgeFunc(serialPort1, Status, pBStatus))
            {
                XConfirmEdgeFlag = true;
            }
            else if (CShareData.ReturnValueEdge.SUCCESS == XConfirmEdgeFunc(serialPort1, Status, pBStatus))
            {
                XConfirmEdgeFlag = true;
            }

            LabelUpdate(Status, "垂直确认边框……");
            //if (YConfirmEdgeFunc(serialPort1, Status, pBStatus))
            //{
            //    YConfirmEdgeFlag = true;
            //}

            int YConfirmEdgeErrorTime = 0;

            Int16 YEdgeStepSave1 = 285;
            double YProbSave1 = 0.5;
            Int16 YEdgeStepSave2 = 285;
            double YProbSave2 = 0.5;
            Int16 YEdgeStepSave3 = 285;
            double YProbSave3 = 0.5;
            if (CShareData.ReturnValueEdge.SUCCESS == YConfirmEdgeFunc(serialPort1, Status, pBStatus))
            {
                LabelUpdate(Status, "获取边框--1……" + CShareData.YEdgeStepSave.ToString() + "--" + CShareData.YProbSave.ToString());
                YEdgeStepSave1 = CShareData.YEdgeStepSave;
                YProbSave1 = CShareData.YProbSave;
            }
            else if (CShareData.ReturnValueEdge.SUCCESS == YConfirmEdgeFunc(serialPort1, Status, pBStatus))
            {
                LabelUpdate(Status, "获取边框--1……" + CShareData.YEdgeStepSave.ToString() + "--" + CShareData.YProbSave.ToString());
                YEdgeStepSave1 = CShareData.YEdgeStepSave;
                YProbSave1 = CShareData.YProbSave;
            }
            else
                YConfirmEdgeErrorTime++;

            if (CShareData.ReturnValueEdge.SUCCESS == YConfirmEdgeFunc(serialPort1, Status, pBStatus))
            {
                LabelUpdate(Status, "获取边框--2……" + CShareData.YEdgeStepSave.ToString() + "--" + CShareData.YProbSave.ToString());
                YEdgeStepSave2 = CShareData.YEdgeStepSave;
                YProbSave2 = CShareData.YProbSave;
            }
            else if (CShareData.ReturnValueEdge.SUCCESS == YConfirmEdgeFunc(serialPort1, Status, pBStatus))
            {
                LabelUpdate(Status, "获取边框--2……" + CShareData.YEdgeStepSave.ToString() + "--" + CShareData.YProbSave.ToString());
                YEdgeStepSave2 = CShareData.YEdgeStepSave;
                YProbSave2 = CShareData.YProbSave;
            }
            else
                YConfirmEdgeErrorTime++;

            if (CShareData.ReturnValueEdge.SUCCESS == YConfirmEdgeFunc(serialPort1, Status, pBStatus))
            {
                LabelUpdate(Status, "获取边框--3……" + CShareData.YEdgeStepSave.ToString() + "--" + CShareData.YProbSave.ToString());
                YEdgeStepSave3 = CShareData.YEdgeStepSave;
                YProbSave3 = CShareData.YProbSave;
            }
            else if (CShareData.ReturnValueEdge.SUCCESS == YConfirmEdgeFunc(serialPort1, Status, pBStatus))
            {
                LabelUpdate(Status, "获取边框--3……" + CShareData.YEdgeStepSave.ToString() + "--" + CShareData.YProbSave.ToString());
                YEdgeStepSave3 = CShareData.YEdgeStepSave;
                YProbSave3 = CShareData.YProbSave;
            }
            else
                YConfirmEdgeErrorTime++;

            YConfirmEdgeFlag = true;
            if (YEdgeStepSave1 == YEdgeStepSave2)
            {
                CShareData.YEdgeStepSave = YEdgeStepSave1;
                CShareData.YProbSave = (YProbSave1 + YProbSave2) / 2;
            }
            else if (YEdgeStepSave1 == YEdgeStepSave3)
            {
                CShareData.YEdgeStepSave = YEdgeStepSave1;
                CShareData.YProbSave = (YProbSave1 + YProbSave3) / 2;
            }
            else if (YEdgeStepSave2 == YEdgeStepSave3)
            {
                CShareData.YEdgeStepSave = YEdgeStepSave2;
                CShareData.YProbSave = (YProbSave2 + YProbSave3) / 2;
            }
            else
            {
                LabelUpdate(Status, "边框步数两两不相等！");
                YConfirmEdgeFlag = false;
                //return 10;
            }

            if (YConfirmEdgeErrorTime > 1)
            {
                LabelUpdate(Status, "3次确认边框中，有多次失败！");
                YConfirmEdgeFlag = false;
            }




            ReturnZeroFunc(serialPort1, Status, null);   //回零

            if (XConfirmEdgeFlag && YConfirmEdgeFlag)
            {
                LabelUpdate(Status, "水平、垂直确认边框均成功！");
                ProgressBarUpdate(pBStatus, 100);
                return CShareData.ReturnValueEdge.SUCCESS;
            }
            else if (XConfirmEdgeFlag)
            {
                LabelUpdate(Status, "仅水平确认边框成功！");
                ProgressBarUpdate(pBStatus, 50);
                return CShareData.ReturnValueEdge.SUCCESS_HRZ;
            }
            else if (YConfirmEdgeFlag)
            {
                LabelUpdate(Status, "仅垂直确认边框成功！");
                ProgressBarUpdate(pBStatus, 50);
                return CShareData.ReturnValueEdge.SUCCESS_VER;
            }
            else
            {
                LabelUpdate(Status, "水平、垂直确认边框均失败！");
                ProgressBarUpdate(pBStatus, 0);
                return CShareData.ReturnValueEdge.FAILURE;
            }

        }

        #endregion

        #region 查看概率
        public bool ViewProbFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);

            Int16 OnePointMeasureNum = 1000;
            Int16 InSideWindow = 0;
            double InSideWindowProb = 0;     //落在箱子内的概率
            OnePointMeasureFunc(OnePointMeasureNum, serialPort1, Status, null);    //定点测距
            if (CShareData.OnePointDistanceFlag)
            {
                InSideWindow = 0;
                for (int j = 0; j < OnePointMeasureNum; j++)
                {
                    if ((CShareData.OnePointDistance[j] > 4) && (CShareData.OnePointDistance[j] < CShareData.BOXMAXDISTANCE))
                    {
                        InSideWindow++;
                    }
                }
                InSideWindowProb = Convert.ToDouble(InSideWindow) / OnePointMeasureNum;   //落在箱子内的概率
                CShareData.ViewProb = InSideWindowProb;
                ProgressBarUpdate(pBStatus, 100);
                return true;
            }
            else
            {
                ProgressBarUpdate(pBStatus, 0);
                return false;
            }
        }
        #endregion

        #region 查看概率(N*1000次)
        public bool NTimeViewProbFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);


            Int16 InSideWindow = 0;
            double InSideWindowProb = 0;     //落在箱子内的概率
            NTimeOnePointMeasureFunc(CShareData.NTIMEEDGEY, serialPort1, Status, pBStatus);    //定点测距
            if (CShareData.NTimeOnePointDistanceFlag)
            {
                InSideWindow = 0;
                for (int j = 0; j < CShareData.NTimeOnePointDistanceLen; j++)
                {
                    if ((CShareData.NTimeOnePointDistance[j] > 4) && (CShareData.NTimeOnePointDistance[j] < CShareData.BOXMAXDISTANCE))
                    {
                        InSideWindow++;
                    }
                }
                InSideWindowProb = Convert.ToDouble(InSideWindow) / CShareData.NTimeOnePointDistanceLen;   //落在箱子内的概率
                CShareData.ViewProb = InSideWindowProb;
                ProgressBarUpdate(pBStatus, 100);
                return true;
            }
            else
            {
                ProgressBarUpdate(pBStatus, 0);
                return false;
            }
        }
        #endregion

        #endregion

        #region 水平、垂直校准
        #region 水平校准
        public CShareData.ReturnValueAdjust XAdjustFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            ReturnZeroFunc(serialPort1, Status, null);   //先回零

            Int16 XEdgeStep = CShareData.XEdgeStepSave;
            double XEdgeProb = CShareData.XProbSave;
            double ProbMax = XEdgeProb + CShareData.XEDGEPROBLIMIT;
            double ProbMin = XEdgeProb - CShareData.XEDGEPROBLIMIT;
            double ProbMid = (ProbMax + ProbMin) / 2;

            //回到边框附近
            MoveFunc(CShareData.MoveDirection.Right, CShareData.XEdgeStepSave, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);

            //Int16 OnePointMeasureNum = 1000;
            //Int16 InSideWindow = 0;
            double InSideWindowProb = 0.6;     //落在箱子内的概率
            double LastInSideWindowProb = 0.6;
            bool AdjustFlag = false;     //校准成功标志
            Int16 XEdgeStepError = 0;


            Int16 MaxMoveStep = 6; //防止无限制移动
            double[,] AdjustStepProb = new double[2, MaxMoveStep];     //接收到的水平扫描距离值

            int MoveTime = 0;
            for (MoveTime = 0; MoveTime < MaxMoveStep; MoveTime++)        //最多定位52次，防止死循环
            {

                //InSideWindow = 0;
                //OnePointMeasureFunc(OnePointMeasureNum, serialPort1, Status, null);    //定点测距

                //for (int j = 0; j < OnePointMeasureNum; j++)
                //{
                //    if ((CShareData.OnePointDistance[j] > 4) && (CShareData.OnePointDistance[j] < CShareData.BOXMAXDISTANCE))
                //    {
                //        InSideWindow++;
                //    }
                //}

                LastInSideWindowProb = InSideWindowProb;
                //InSideWindowProb = Convert.ToDouble(InSideWindow) / OnePointMeasureNum;   //落在箱子内的概率
                InSideWindowProb = GetEdgeProbFunc(CShareData.NTIMEEDGEY, serialPort1, Status, null);


                if (MoveTime == 0)     //如果没有这句，第一次判断有可能满足条件，InSideWindowProb=1，LastInSideWindowProb=0
                {
                    LastInSideWindowProb = InSideWindowProb;
                }



                AdjustStepProb[0, MoveTime] = XEdgeStep;
                AdjustStepProb[1, MoveTime] = InSideWindowProb;


                //判断落在边框内的概率，最稳定的情况为45%~55%
                if ((InSideWindowProb >= ProbMin) && (InSideWindowProb <= ProbMax))
                {
                    AdjustFlag = true;
                    break;
                }
                else if (InSideWindowProb < ProbMax)
                {
                    if (LastInSideWindowProb > ProbMin)    //上次概率太大，这次概率太小
                    {
                        //比较哪个更接近ProbMid
                        if (Math.Abs(InSideWindowProb - ProbMid) > Math.Abs(LastInSideWindowProb - ProbMid))    //上次更接近ProbMid
                        {

                            InSideWindowProb = LastInSideWindowProb;  //保存上次概率
                            //调整到上次的位置
                            MoveFunc(CShareData.MoveDirection.Right, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                            XEdgeStep += 1;
                        }

                        AdjustFlag = true;
                        break;
                    }
                    else
                    {
                        MoveFunc(CShareData.MoveDirection.Right, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                        XEdgeStep += 1;
                    }
                }
                else if (InSideWindowProb > ProbMin)
                {
                    if (LastInSideWindowProb < ProbMax) //上次概率太小，这次概率太大
                    {
                        //比较哪个更接近ProbMid
                        if (Math.Abs(InSideWindowProb - ProbMid) > Math.Abs(LastInSideWindowProb - ProbMid))    //上次更接近ProbMid
                        {

                            InSideWindowProb = LastInSideWindowProb;  //保存上次概率
                            //调整到上次的位置
                            MoveFunc(CShareData.MoveDirection.Left, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                            //XEdgeStep += 1;
                            XEdgeStep -= 1;
                        }

                        AdjustFlag = true;
                        break;
                    }
                    else
                    {
                        MoveFunc(CShareData.MoveDirection.Left, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                        //XEdgeStep += 1;
                        XEdgeStep -= 1;
                    }
                }
            }

            if (CShareData.DEBUG)
            {
                string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                string path = CShareData.StartPath + "水平步数首次定位调整" + TimeNow + ".txt";
                SaveAdjustStep(path, AdjustStepProb);
            }

            if (MoveTime == MaxMoveStep)    //镜子跑飞后，类似边框确认找边界
            {
                LabelUpdate(Status, "镜子跑飞，纠正中……");
                ProgressBarUpdate(pBStatus, 10);

                ReturnZeroFunc(serialPort1, Status, null);    //先回到零点后再校准
                if (XAdjustCorrectFunc(serialPort1, Status, null))
                {
                    XEdgeStep = XAdjustCorrectStep;
                    InSideWindowProb = XAdjustCorrectProb;

                    XEdgeStepError = Convert.ToInt16(XEdgeStep - CShareData.XEdgeStepSave);
                    AdjustFlag = true;
                }
                else
                {
                    LabelUpdate(Status, "水平方向未扫描到边框，请检查零点位置！");
                    ProgressBarUpdate(pBStatus, 0);
                    return CShareData.ReturnValueAdjust.ERROR_HRZ_SCAN;
                    //return false;
                }
            }
            ProgressBarUpdate(pBStatus, 80);


            //让电机停稳，二次统计概率
            if (AdjustFlag)
            {
                Thread.Sleep(20); //防止因镜子振动，导致电机位置未停稳，概率误差较大

                //OnePointMeasureFunc(OnePointMeasureNum, serialPort1, Status, null);    //定点测距
                //InSideWindow = 0;
                //for (int j = 0; j < OnePointMeasureNum; j++)
                //{
                //    if ((CShareData.OnePointDistance[j] > 4) && (CShareData.OnePointDistance[j] < CShareData.BOXMAXDISTANCE))
                //    {
                //        InSideWindow++;
                //    }
                //}
                //InSideWindowProb = Convert.ToDouble(InSideWindow) / OnePointMeasureNum;   //落在箱子内的概率
                InSideWindowProb = GetEdgeProbFunc(CShareData.NTIMEEDGEY, serialPort1, Status, null);
            }

            if (AdjustFlag)   //概率确认
            {
                if (Math.Abs(InSideWindowProb - XEdgeProb) > 0.4)
                {
                    LabelUpdate(Status, "该位置概率误差太大！");
                    ProgressBarUpdate(pBStatus, 0);
                    //return false;
                    return CShareData.ReturnValueAdjust.ERROR_HRZ_PROB;
                }
            }



            if (AdjustFlag)   //完成边框确认
            {
                XEdgeStepError = Convert.ToInt16(CShareData.XEdgeStepSave - XEdgeStep);
                CShareData.XAdjustStepError = XEdgeStepError;
                CShareData.XAdjustProb = Math.Round(InSideWindowProb, 3);
                LabelUpdate(Status, "水平校准成功：误差" + XEdgeStepError.ToString() + "步");

                //回到零点并置位
                //MoveFunc(CShareData.MoveDirection.Left, CShareData.XEdgeStepSave, CShareData.SetZeroValue.Set, serialPort1, Status, null);

                ProgressBarUpdate(pBStatus, 100);
                return CShareData.ReturnValueAdjust.SUCCESS;
            }
            else
            {
                ReturnZeroFunc(serialPort1, Status, null);   //回到零点
                ProgressBarUpdate(pBStatus, 0);
                return CShareData.ReturnValueAdjust.FAILURE;
            }
        }





        Int16 XAdjustCorrectStep = 0;   //纠正后的步数
        double XAdjustCorrectProb = 0;  //纠正后的概率
        private bool XAdjustCorrectFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)   //类似于边框确认
        {
            Int16 XEdgeStep = 0;    //距离边框的步数
            Int16 XEdgeIndexStep = 0;   //边框在一帧扫描数据中的位置
            Int16 ScanZeroMove = 50;   //每次扫描零点移动的步数
            Int16 ScanStepRange = CShareData.XMOTORRANGE;   //每次扫描电机转动步数

            double XEdgeProb = CShareData.XProbSave;
            double ProbMax = XEdgeProb + CShareData.XEDGEPROBLIMIT;
            double ProbMin = XEdgeProb - CShareData.XEDGEPROBLIMIT;
            double ProbMid = (ProbMax + ProbMin) / 2;


            Int16 XStartPos = 0;
            Int16 XEndPos = ScanStepRange;
            Int16 PointCnt = Convert.ToInt16(((XEndPos - XStartPos) << 1) + 1);  //the number of distance,default:109


            Int32[,] ReceivedDistance = new Int32[2, PointCnt];     //接收到的水平扫描距离值


            int EdgeIndexPos = 0;     //正向数据边界下标
            int EdgeIndexNeg = 0;     //反向数据边界下标
            bool ScanFindEdgeFlag = false;  //找到边框
            bool EdgeInFrame = false;   //此次扫描是否包含边界
            for (int i = 0; i < ScanStepRange; i++)    //能保证转1周时，应取3200/ZeroMove
            {
                //此处与边框确认不同
                if (i > 5)
                {
                    LabelUpdate(Status, "镜子已经跑飞！");
                    ProgressBarUpdate(pBStatus, 0);
                    return false;
                }

                ScanFindEdgeFlag = false;
                EdgeInFrame = false;

                ReceivedDistance = HrzScanFunc(0, XStartPos, XEndPos, serialPort1, Status, null);    //调用水平扫描函数
                if (CShareData.DEBUG)
                {
                    string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                    string path = CShareData.StartPath + "水平校准扫描" + TimeNow + ".txt";
                    SaveScanDistance(path, ReceivedDistance, PointCnt, 2);      //保存扫描数据
                }


                if (CShareData.HrzScanFlag)
                {
                    Int16 ValidPointBegin = 0;
                    Int16 ValidPointEnd = 0;
                    for (int m = 0; m < 10; m++)//反向数据中前10个在箱子外
                    {
                        if ((ReceivedDistance[1, m] <= 4) || (ReceivedDistance[1, m] >= CShareData.BOXMAXDISTANCE))  //第1个点在箱子外
                        {
                            ValidPointBegin++;
                        }

                        if ((ReceivedDistance[1, PointCnt - 1 - m] > 4) && (ReceivedDistance[1, PointCnt - 1 - m] < CShareData.BOXMAXDISTANCE))//最后一个在箱子内
                        {
                            ValidPointEnd++;
                        }
                    }

                    if ((ValidPointBegin >= 2) && (ValidPointEnd >= 2))
                    {
                        EdgeInFrame = true;
                    }

                    if (EdgeInFrame)    //此次扫描数据中包含边界,再确定边界对应一帧数据中哪个点
                    {
                        break;
                    }
                    else //此次扫描数据中没有包含边界
                    {
                        XEdgeStep += ScanZeroMove;
                        XStartPos += ScanZeroMove;
                        XEndPos = Convert.ToInt16(XStartPos + ScanStepRange);
                    }
                }
            }

            if (EdgeInFrame)    //此次扫描数据中包含边界,再确定边界对应一帧数据中哪个点
            {
                for (int j = 1; j < PointCnt; j++)   //正向数据
                {
                    if ((ReceivedDistance[0, j] > 4) && (ReceivedDistance[0, j] < CShareData.BOXMAXDISTANCE))   //落在箱子内
                    {
                        if ((ReceivedDistance[0, j - 1] <= 4) || (ReceivedDistance[0, j - 1] >= CShareData.BOXMAXDISTANCE))      //前一个点未落在箱子内（右边界）
                        {
                            EdgeIndexPos = j;
                            ScanFindEdgeFlag = true;
                            break;
                        }
                    }
                }
                for (int j = 1; j < PointCnt; j++)   //反向数据
                {
                    if ((ReceivedDistance[1, j] > 4) && (ReceivedDistance[1, j] < CShareData.BOXMAXDISTANCE))   //落在箱子内
                    {
                        if ((ReceivedDistance[1, j - 1] <= 4) || (ReceivedDistance[1, j - 1] >= CShareData.BOXMAXDISTANCE))      //前一个点未落在箱子内（右边界）
                        {
                            EdgeIndexNeg = j;
                            ScanFindEdgeFlag = true;
                            break;
                        }
                    }
                }
            }

            ProgressBarUpdate(pBStatus, 50);

            double[,] AdjustStepProb = new double[2, ScanStepRange];     //接收到的水平扫描距离值

            //Int16 OnePointMeasureNum = 1000;
            //Int16 InSideWindow = 0;
            double InSideWindowProb = 0;     //落在箱子内的概率
            double LastInSideWindowProb = 0;
            bool FindEdgeFlag = false;     //确定边框成功标志
            if (ScanFindEdgeFlag)   //扫描的方式找到边框
            {
                XEdgeIndexStep = Convert.ToInt16((EdgeIndexPos + EdgeIndexNeg) >> 2);
                XEdgeStep += XEdgeIndexStep;

                //粗略定位到右移到边框的边界
                MoveFunc(CShareData.MoveDirection.Right, XEdgeIndexStep, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);

                InSideWindowProb = 0;
                LastInSideWindowProb = 0;
                for (int i = 0; i < ScanStepRange; i++)        //最多定位52次，防止死循环
                {
                    string str = "X定点测量次数：" + i.ToString();
                    LabelUpdate(Status, str);

                    //InSideWindow = 0;

                    //OnePointMeasureFunc(OnePointMeasureNum, serialPort1, Status, null);    //定点测距

                    //for (int j = 0; j < OnePointMeasureNum; j++)
                    //{
                    //    if ((CShareData.OnePointDistance[j] > 4) && (CShareData.OnePointDistance[j] < CShareData.BOXMAXDISTANCE))
                    //    {
                    //        InSideWindow++;
                    //    }
                    //}
                    LastInSideWindowProb = InSideWindowProb;
                    //InSideWindowProb = Convert.ToDouble(InSideWindow) / OnePointMeasureNum;   //落在箱子内的概率
                    InSideWindowProb = GetEdgeProbFunc(CShareData.NTIMEEDGEX, serialPort1, Status, null);

                    AdjustStepProb[0, i] = XEdgeStep;
                    AdjustStepProb[1, i] = InSideWindowProb;

                    if (i == 0)     //如果没有这句，第一次判断有可能满足条件，InSideWindowProb=1，LastInSideWindowProb=0
                    {
                        LastInSideWindowProb = InSideWindowProb;
                    }

                    //判断落在边框内的概率，最稳定的情况为45%~55%
                    if ((InSideWindowProb >= ProbMin) && (InSideWindowProb <= ProbMax))
                    {
                        FindEdgeFlag = true;
                        break;
                    }

                    else if (InSideWindowProb < ProbMax)
                    {
                        if (LastInSideWindowProb > ProbMin)    //上次概率太大，这次概率太小
                        {
                            //比较哪个更接近ProbMid
                            if (Math.Abs(InSideWindowProb - ProbMid) > Math.Abs(LastInSideWindowProb - ProbMid))    //上次更接近ProbMid
                            {
                                //调整到上次的位置
                                MoveFunc(CShareData.MoveDirection.Right, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                                XEdgeStep += 1;
                            }

                            FindEdgeFlag = true;
                            break;
                        }
                        else
                        {
                            MoveFunc(CShareData.MoveDirection.Right, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                            XEdgeStep += 1;
                        }
                    }
                    else if (InSideWindowProb > ProbMin)
                    {
                        if (LastInSideWindowProb < ProbMax) //上次概率太小，这次概率太大
                        {
                            //比较哪个更接近ProbMid
                            if (Math.Abs(InSideWindowProb - ProbMid) > Math.Abs(LastInSideWindowProb - ProbMid))    //上次更接近ProbMid
                            {
                                InSideWindowProb = LastInSideWindowProb;  //保存上次概率
                                //调整到上次的位置
                                MoveFunc(CShareData.MoveDirection.Left, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                                XEdgeStep -= 1;
                            }

                            FindEdgeFlag = true;
                            break;
                        }
                        else
                        {
                            MoveFunc(CShareData.MoveDirection.Left, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                            XEdgeStep -= 1;
                        }
                    }
                    if (i == ScanStepRange)
                    {
                        LabelUpdate(Status, "单步移动超时！");
                        ProgressBarUpdate(pBStatus, 0);
                        return false;
                    }
                }

            }
            else
            {
                LabelUpdate(Status, "未扫描到边框！");
                ProgressBarUpdate(pBStatus, 0);
                return false;
            }


            if (CShareData.DEBUG)
            {
                string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                string path = CShareData.StartPath + "水平扫描判断后定位步数调整" + TimeNow + ".txt";
                SaveAdjustStep(path, AdjustStepProb);
            }

            //此处与边框确认不同
            if (FindEdgeFlag)   //完成边框确认
            {

                XAdjustCorrectStep = XEdgeStep;
                XAdjustCorrectProb = InSideWindowProb;
                //EdgeStepShowX.Text = XEdgeStep.ToString();
                //ProbX.Text = InSideWindowProb.ToString();

                FindEdgeFlag = true;
            }

            return FindEdgeFlag;
        }


        #endregion

        #region 垂直校准
        public CShareData.ReturnValueAdjust YAdjustFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            ReturnZeroFunc(serialPort1, Status, null);   //先回零

            Int16 YEdgeStep = CShareData.YEdgeStepSave;
            double YEdgeProb = CShareData.YProbSave;
            double ProbMax = YEdgeProb + CShareData.YEDGEPROBLIMIT;
            double ProbMin = YEdgeProb - CShareData.YEDGEPROBLIMIT;
            double ProbMid = (ProbMax + ProbMin) / 2;

            //回到边框附近
            MoveFunc(CShareData.MoveDirection.Down, CShareData.YEdgeStepSave, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);

            //Int16 OnePointMeasureNum = 1000;
            //Int16 InSideWindow = 0;
            double InSideWindowProb = 0.6;     //落在箱子内的概率
            double LastInSideWindowProb = 0.6;
            bool AdjustFlag = false;     //校准成功标志
            Int16 YEdgeStepError = 0;


            Int16 MaxMoveStep = 6; //防止无限制移动
            double[,] AdjustStepProb = new double[2, MaxMoveStep];     //接收到的水平扫描距离值

            int MoveTime = 0;
            for (MoveTime = 0; MoveTime < MaxMoveStep; MoveTime++)        //最多定位6次，防止死循环
            {
                //InSideWindow = 0;
                //OnePointMeasureFunc(OnePointMeasureNum, serialPort1, Status, null);    //定点测距
                //for (int j = 0; j < OnePointMeasureNum; j++)
                //{
                //    if ((CShareData.OnePointDistance[j] > 4) && (CShareData.OnePointDistance[j] < CShareData.BOXMAXDISTANCE))
                //    {
                //        InSideWindow++;
                //    }
                //}
                LastInSideWindowProb = InSideWindowProb;
                //InSideWindowProb = Convert.ToDouble(InSideWindow) / OnePointMeasureNum;   //落在箱子内的概率
                InSideWindowProb = GetEdgeProbFunc(CShareData.NTIMEEDGEY, serialPort1, Status, null);


                if (MoveTime == 0)     //如果没有这句，第一次判断有可能满足条件，InSideWindowProb=1，LastInSideWindowProb=0
                {
                    LastInSideWindowProb = InSideWindowProb;
                }

                AdjustStepProb[0, MoveTime] = YEdgeStep;
                AdjustStepProb[1, MoveTime] = InSideWindowProb;

                //判断落在边框内的概率，最稳定的情况为45%~55%
                if ((InSideWindowProb >= ProbMin) && (InSideWindowProb <= ProbMax))
                {
                    AdjustFlag = true;
                    break;

                }
                else if (InSideWindowProb < ProbMax)    //上次概率太大，这次概率太小
                {
                    if (LastInSideWindowProb > ProbMin)
                    {
                        //比较哪个更接近ProbMid
                        if (Math.Abs(InSideWindowProb - ProbMid) > Math.Abs(LastInSideWindowProb - ProbMid))    //上次更接近ProbMid
                        {
                            InSideWindowProb = LastInSideWindowProb;  //保存上次概率
                            //调整到上次的位置
                            MoveFunc(CShareData.MoveDirection.Down, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                            YEdgeStep += 1;
                        }

                        AdjustFlag = true;
                        break;
                    }
                    else
                    {
                        MoveFunc(CShareData.MoveDirection.Down, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                        YEdgeStep += 1;
                    }
                }
                else if (InSideWindowProb > ProbMin)
                {
                    if (LastInSideWindowProb < ProbMax) //上次概率太小，这次概率太大
                    {
                        //比较哪个更接近ProbMid
                        if (Math.Abs(InSideWindowProb - ProbMid) > Math.Abs(LastInSideWindowProb - ProbMid))    //上次更接近ProbMid
                        {

                            InSideWindowProb = LastInSideWindowProb;  //保存上次概率
                            //调整到上次的位置
                            MoveFunc(CShareData.MoveDirection.Up, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                            YEdgeStep -= 1;
                        }
                        AdjustFlag = true;
                        break;
                    }
                    else
                    {
                        MoveFunc(CShareData.MoveDirection.Up, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                        YEdgeStep -= 1;
                    }
                }
            }


            if (CShareData.DEBUG)
            {
                string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                string path = CShareData.StartPath + "垂直首次定位步数调整" + TimeNow + ".txt";
                SaveAdjustStep(path, AdjustStepProb);
            }


            if (MoveTime == MaxMoveStep)    //镜子跑飞后，类似边框确认找边界
            {
                LabelUpdate(Status, "镜子跑飞，纠正中……");
                ProgressBarUpdate(pBStatus, 10);

                //先回到零点后再校准
                ReturnZeroFunc(serialPort1, Status, null);
                if (YAdjustCorrectFunc(serialPort1, Status, pBStatus))
                {
                    YEdgeStep = YAdjustCorrectStep;
                    InSideWindowProb = YAdjustCorrectProb;

                    YEdgeStepError = Convert.ToInt16(YEdgeStep - CShareData.YEdgeStepSave);

                    AdjustFlag = true;
                }
                else
                {
                    LabelUpdate(Status, "垂直方向未扫描到边框，请检查零点位置！");
                    ProgressBarUpdate(pBStatus, 0);
                    return CShareData.ReturnValueAdjust.ERROR_VER_SCAN;
                }
            }

            ProgressBarUpdate(pBStatus, 80);
            //让电机停稳，二次统计概率
            if (AdjustFlag)
            {
                Thread.Sleep(20); //防止因镜子振动，导致电机位置未停稳，概率误差较大

                //OnePointMeasureFunc(OnePointMeasureNum, serialPort1, Status, null);    //定点测距

                //InSideWindow = 0;
                //for (int j = 0; j < OnePointMeasureNum; j++)
                //{
                //    if ((CShareData.OnePointDistance[j] > 4) && (CShareData.OnePointDistance[j] < CShareData.BOXMAXDISTANCE))
                //    {
                //        InSideWindow++;
                //    }
                //}
                //InSideWindowProb = Convert.ToDouble(InSideWindow) / OnePointMeasureNum;   //落在箱子内的概率
                InSideWindowProb = GetEdgeProbFunc(CShareData.NTIMEEDGEY, serialPort1, Status, null);
            }

            if (AdjustFlag)   //概率确认
            {
                if (Math.Abs(InSideWindowProb - YEdgeProb) > 0.2)
                {
                    LabelUpdate(Status, "该位置概率误差太大！");
                    ProgressBarUpdate(pBStatus, 0);
                    return CShareData.ReturnValueAdjust.ERROR_VER_PROB;
                }
            }

            if (AdjustFlag)   //完成校准
            {
                YEdgeStepError = Convert.ToInt16(YEdgeStep - CShareData.YEdgeStepSave);
                CShareData.YAdjustStepError = YEdgeStepError;
                CShareData.YAdjustProb = Math.Round(InSideWindowProb, 3);

                LabelUpdate(Status, "垂直校准成功：误差" + YEdgeStepError.ToString() + "步");

                //回到零点并置位
                //MoveFunc(CShareData.MoveDirection.Up,CShareData.YEdgeStepSave , CShareData.SetZeroValue.Set, serialPort1, Status, null);

                ProgressBarUpdate(pBStatus, 100);
                return CShareData.ReturnValueAdjust.SUCCESS;
            }
            else
            {
                ReturnZeroFunc(serialPort1, Status, null);   //回到零点
                ProgressBarUpdate(pBStatus, 0);
                return CShareData.ReturnValueAdjust.FAILURE;
            }
        }





        Int16 YAdjustCorrectStep = 0;   //纠正后的步数
        double YAdjustCorrectProb = 0;  //纠正后的概率
        private bool YAdjustCorrectFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)   //类似于边框确认
        {

            Int16 YEdgeStep = 0;    //距离边框的步数
            Int16 YEdgeIndexStep = 0;   //边框在一帧扫描数据中的位置
            Int16 ScanZeroMove = 50;   //每次扫描零点移动的步数
            Int16 ScanStepRange = CShareData.YMOTORRANGE;   //每次扫描电机转动步数

            double YEdgeProb = CShareData.YProbSave;
            double ProbMax = YEdgeProb + CShareData.YEDGEPROBLIMIT;
            double ProbMin = YEdgeProb - CShareData.YEDGEPROBLIMIT;
            double ProbMid = (ProbMax + ProbMin) / 2;


            Int16 YStartPos = 0;
            Int16 YEndPos = Convert.ToInt16(0 - ScanStepRange);
            Int16 PointCnt = Convert.ToInt16(((YStartPos - YEndPos) << 1) + 1);  //the number of distance,default:109


            Int32[,] ReceivedDistance = new Int32[2, PointCnt];     //接收到的水平扫描距离值


            int EdgeIndexPos = 0;     //正向数据边界下标
            int EdgeIndexNeg = 0;     //反向数据边界下标
            bool ScanFindEdgeFlag = false;  //找到边框
            bool EdgeInFrame = false;   //此次扫描是否包含边界
            for (int i = 0; i < CShareData.YCIRCLESTEP / ScanZeroMove; i++)    //能保证转1周时，应取3200/ZeroMove
            {
                //此处与边框确认不同
                if (i > 12)
                {
                    LabelUpdate(Status, "镜子已经跑飞！");
                    ProgressBarUpdate(pBStatus, 0);
                    return false;
                }

                ScanFindEdgeFlag = false;
                EdgeInFrame = false;

                ReceivedDistance = VerScanFunc(0, YStartPos, YEndPos, serialPort1, Status, null);   //调用垂直扫描函数

                if (CShareData.DEBUG)
                {
                    string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                    string path = CShareData.StartPath + "垂直校准扫描" + TimeNow + ".txt";
                    SaveScanDistance(path, ReceivedDistance, PointCnt, 2);      //保存扫描数据
                }


                if (CShareData.VerScanFlag)    //正向数据有可能会有个别异常点，因此垂直扫描判断反向数据
                {
                    Int16 ValidPointBegin = 0;
                    Int16 ValidPointEnd = 0;
                    for (int m = 0; m < 10; m++)//反向数据中前10个在箱子外
                    {
                        if ((ReceivedDistance[1, m] <= 4) || (ReceivedDistance[1, m] >= CShareData.BOXMAXDISTANCE))  //第1个点在箱子外
                        {
                            ValidPointBegin++;
                        }

                        if ((ReceivedDistance[1, PointCnt - 1 - m] > 4) && (ReceivedDistance[1, PointCnt - 1 - m] < CShareData.BOXMAXDISTANCE))//最后一个在箱子内
                        {
                            ValidPointEnd++;
                        }
                    }

                    if ((ValidPointBegin >= 2) && (ValidPointEnd >= 2))
                    {
                        EdgeInFrame = true;
                    }
                    if (EdgeInFrame)    //此次扫描数据中包含边界,再确定边界对应一帧数据中哪个点
                    {
                        break;
                    }
                    else //此次扫描数据中没有包含边界
                    {
                        YEdgeStep += ScanZeroMove;
                        YStartPos -= ScanZeroMove;
                        YEndPos = Convert.ToInt16(YStartPos - ScanStepRange);
                    }
                }
            }

            if (EdgeInFrame)    //此次扫描数据中包含边界,再确定边界对应一帧数据中哪个点
            {
                for (int j = 1; j < PointCnt; j++)   //正向数据
                {
                    if ((ReceivedDistance[0, j] > 4) && (ReceivedDistance[0, j] < CShareData.BOXMAXDISTANCE))   //落在箱子内
                    {
                        if ((ReceivedDistance[0, j - 1] <= 4) || (ReceivedDistance[0, j - 1] >= CShareData.BOXMAXDISTANCE))      //前一个点未落在箱子内（右边界）
                        {
                            EdgeIndexPos = j;
                            ScanFindEdgeFlag = true;
                            break;
                        }
                    }
                }
                for (int j = 1; j < PointCnt; j++)   //反向数据
                {
                    if ((ReceivedDistance[1, j] > 4) && (ReceivedDistance[1, j] < CShareData.BOXMAXDISTANCE))   //落在箱子内
                    {
                        if ((ReceivedDistance[1, j - 1] <= 4) || (ReceivedDistance[1, j - 1] >= CShareData.BOXMAXDISTANCE))      //前一个点未落在箱子内（右边界）
                        {
                            EdgeIndexNeg = j;
                            ScanFindEdgeFlag = true;
                            break;
                        }
                    }
                }
            }

            ProgressBarUpdate(pBStatus, 50);
            double[,] AdjustStepProb = new double[2, ScanStepRange];     //接收到的水平扫描距离值

            //Int16 OnePointMeasureNum = 1000;
            //Int16 InSideWindow = 0;
            double InSideWindowProb = 0;     //落在箱子内的概率
            double LastInSideWindowProb = 0;
            bool FindEdgeFlag = false;     //确定边框成功标志
            if (ScanFindEdgeFlag)   //扫描的方式找到边框
            {
                YEdgeIndexStep = Convert.ToInt16((EdgeIndexPos + EdgeIndexNeg) >> 2);
                YEdgeStep += YEdgeIndexStep;

                //粗略定位到右移到边框的边界
                MoveFunc(CShareData.MoveDirection.Down, YEdgeIndexStep, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);


                //InSideWindow = 0;
                InSideWindowProb = 0;
                LastInSideWindowProb = 0;
                for (int i = 0; i < ScanStepRange; i++)        //最多定位52次，防止死循环
                {
                    string str = "Y定点测量次数：" + i.ToString();
                    LabelUpdate(Status, str);

                    //InSideWindow = 0;

                    //OnePointMeasureFunc(OnePointMeasureNum,serialPort1,Status,null);    //定点测距


                    //for (int j = 0; j < OnePointMeasureNum; j++)
                    //{
                    //    if ((CShareData.OnePointDistance[j] > 4) && (CShareData.OnePointDistance[j] < CShareData.BOXMAXDISTANCE))
                    //    {
                    //        InSideWindow++;
                    //    }
                    //}
                    LastInSideWindowProb = InSideWindowProb;
                    //InSideWindowProb = Convert.ToDouble(InSideWindow) / OnePointMeasureNum;   //落在箱子内的概率
                    InSideWindowProb = GetEdgeProbFunc(CShareData.NTIMEEDGEY, serialPort1, Status, null);
                    if (i == 0)     //如果没有这句，第一次判断有可能满足条件，InSideWindowProb=1，LastInSideWindowProb=0
                    {
                        LastInSideWindowProb = InSideWindowProb;
                    }

                    AdjustStepProb[0, i] = YEdgeStep;
                    AdjustStepProb[1, i] = InSideWindowProb;


                    //判断落在边框内的概率，最稳定的情况为45%~55%
                    if ((InSideWindowProb >= ProbMin) && (InSideWindowProb <= ProbMax))
                    {
                        FindEdgeFlag = true;
                        break;

                    }
                    else if (InSideWindowProb < ProbMax)    //上次概率太大，这次概率太小
                    {
                        if (LastInSideWindowProb > ProbMin)
                        {
                            //比较哪个更接近ProbMid
                            if (Math.Abs(InSideWindowProb - ProbMid) > Math.Abs(LastInSideWindowProb - ProbMid))    //上次更接近ProbMid
                            {
                                InSideWindowProb = LastInSideWindowProb;  //保存上次概率
                                //调整到上次的位置
                                MoveFunc(CShareData.MoveDirection.Down, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                                YEdgeStep += 1;
                            }

                            FindEdgeFlag = true;
                            break;
                        }
                        else
                        {
                            if (Math.Abs(InSideWindowProb - ProbMid) < 0.3)
                            {
                                MoveFunc(CShareData.MoveDirection.Down, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                                YEdgeStep += 1;
                            }
                            else
                            {
                                MoveFunc(CShareData.MoveDirection.Down, 3, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                                YEdgeStep += 3;
                            }
                        }
                    }
                    else if (InSideWindowProb > ProbMin)
                    {
                        if (LastInSideWindowProb < ProbMax) //上次概率太小，这次概率太大
                        {
                            //比较哪个更接近ProbMid
                            if (Math.Abs(InSideWindowProb - ProbMid) > Math.Abs(LastInSideWindowProb - ProbMid))    //上次更接近ProbMid
                            {

                                InSideWindowProb = LastInSideWindowProb;  //保存上次概率
                                //调整到上次的位置
                                MoveFunc(CShareData.MoveDirection.Up, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                                YEdgeStep -= 1;
                            }
                            FindEdgeFlag = true;
                            break;
                        }
                        else
                        {
                            if (Math.Abs(InSideWindowProb - ProbMid) < 0.3)
                            {
                                MoveFunc(CShareData.MoveDirection.Up, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                                YEdgeStep -= 1;
                            }
                            else
                            {
                                MoveFunc(CShareData.MoveDirection.Up, 3, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                                YEdgeStep -= 3;
                            }

                        }
                    }


                    if (i == ScanStepRange)
                    {
                        LabelUpdate(Status, "单步移动超时！");
                        ProgressBarUpdate(pBStatus, 0);
                        return false;
                    }
                }
            }
            else
            {
                LabelUpdate(Status, "未扫描到边框！");
                ProgressBarUpdate(pBStatus, 0);
                return false;
            }


            if (CShareData.DEBUG)
            {
                string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                string path = CShareData.StartPath + "垂直扫描判断后定位步数调整" + TimeNow + ".txt";
                SaveAdjustStep(path, AdjustStepProb);
            }

            //此处与边框确认不同
            if (FindEdgeFlag)   //完成边框确认
            {
                YAdjustCorrectStep = YEdgeStep;
                YAdjustCorrectProb = InSideWindowProb;

                FindEdgeFlag = true;
            }
            return FindEdgeFlag;
        }


        #endregion

        #region 水平、垂直校准
        public CShareData.ReturnValueAdjust XYAdjustFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            bool XAdjustFlag1 = false;
            bool YAdjustFlag1 = false;
            //int XYAdjustResult = 0;


            //记录误差
            Int16 XAdjustStepError1 = 1000;
            Int16 YAdjustStepError1 = 1000;
            Int16 XAdjustStepError2 = 2000;
            Int16 YAdjustStepError2 = 2000;
            Int16 XAdjustStepError3 = 3000;
            Int16 YAdjustStepError3 = 3000;

            bool XAdjustSetZero = false;    //校准时，重新置零
            bool YAdjustSetZero = false;    //校准时，重新置零

            bool XAdjustDoneFlag = false; //最终校准是否成功
            bool YAdjustDoneFlag = false; //最终校准是否成功

            ProgressBarUpdate(pBStatus, 0);
            LabelUpdate(Status, "水平校准……");
            if (CShareData.ReturnValueAdjust.SUCCESS == XAdjustFunc(serialPort1, Status, pBStatus))
            {
                XAdjustStepError1 = CShareData.XAdjustStepError;
                XAdjustFlag1 = true;
            }
            else
            {
                if (CShareData.ReturnValueAdjust.SUCCESS == XAdjustFunc(serialPort1, Status, pBStatus))
                {
                    XAdjustStepError1 = CShareData.XAdjustStepError;
                    XAdjustFlag1 = true;
                }
            }

            LabelUpdate(Status, "二次水平校准……");
            if (CShareData.ReturnValueAdjust.SUCCESS == XAdjustFunc(serialPort1, Status, pBStatus))
            {
                XAdjustStepError2 = CShareData.XAdjustStepError;
            }
            else
            {
                XAdjustFunc(serialPort1, Status, pBStatus);
                XAdjustStepError2 = CShareData.XAdjustStepError;
            }
            if ((XAdjustStepError1 > 800) && (XAdjustStepError2 > 800))
            {
                LabelUpdate(Status, "水平方向未找到边界");
                return CShareData.ReturnValueAdjust.FAILURE;//零点偏离太多
            }


            if (XAdjustFlag1 && (Math.Abs(XAdjustStepError1) <= 1) && (Math.Abs(XAdjustStepError2) <= 1))
            {
                XAdjustDoneFlag = true;
                LabelUpdate(Status, "水平误差小于1步，校准成功！");
            }
            else
            {
                if (Math.Abs(XAdjustStepError1 - XAdjustStepError2) <= 5) //两次误差相同，重新置零
                {
                    //回到零点并置位
                    MoveFunc(CShareData.MoveDirection.Left, CShareData.XEdgeStepSave, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                    XAdjustSetZero = true;
                    XAdjustDoneFlag = true;
                    LabelUpdate(Status, "水平校准成功，重新置零！");
                }
                else
                {
                    LabelUpdate(Status, "-------------------------------两次水平校准步数误差相差太大！");
                    if (CShareData.ReturnValueAdjust.SUCCESS == XAdjustFunc(serialPort1, Status, pBStatus))
                    {
                        XAdjustStepError3 = CShareData.XAdjustStepError;
                        if ((Math.Abs(XAdjustStepError1 - XAdjustStepError3) <= 5) || (Math.Abs(XAdjustStepError2 - XAdjustStepError3) <= 5))
                        {
                            MoveFunc(CShareData.MoveDirection.Left, CShareData.XEdgeStepSave, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                            XAdjustSetZero = true;
                            XAdjustDoneFlag = true;
                            LabelUpdate(Status, "水平校准成功，重新置零！");
                        }
                        else
                        {
                            LabelUpdate(Status, "-------------------------------水平步数错误！");
                            return CShareData.ReturnValueAdjust.FAILURE;//概率明显错误
                        }
                    }
                    else
                    {
                        XAdjustFunc(serialPort1, Status, pBStatus);
                        XAdjustStepError3 = CShareData.XAdjustStepError;
                        if ((Math.Abs(XAdjustStepError1 - XAdjustStepError3) <= 5) || (Math.Abs(XAdjustStepError2 - XAdjustStepError3) <= 5))
                        {
                            MoveFunc(CShareData.MoveDirection.Left, CShareData.XEdgeStepSave, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                            XAdjustSetZero = true;
                            XAdjustDoneFlag = true;
                            LabelUpdate(Status, "水平校准成功，重新置零！");
                        }
                        else
                        {
                            LabelUpdate(Status, "-------------------------------水平步数错误！");
                            return CShareData.ReturnValueAdjust.FAILURE;//概率明显错误
                        }
                    }
                }
            }



            LabelUpdate(Status, "垂直校准……");
            if (CShareData.ReturnValueAdjust.SUCCESS == YAdjustFunc(serialPort1, Status, pBStatus))
            {
                YAdjustStepError1 = CShareData.YAdjustStepError;
                YAdjustFlag1 = true;
            }
            else
            {
                if (CShareData.ReturnValueAdjust.SUCCESS == YAdjustFunc(serialPort1, Status, pBStatus))
                {
                    YAdjustStepError1 = CShareData.YAdjustStepError;
                    YAdjustFlag1 = true;
                }
            }

            LabelUpdate(Status, "二次垂直校准……");
            if (CShareData.ReturnValueAdjust.SUCCESS == YAdjustFunc(serialPort1, Status, pBStatus))
            {
                YAdjustStepError2 = CShareData.YAdjustStepError;
            }
            else
            {
                YAdjustFunc(serialPort1, Status, pBStatus);
                YAdjustStepError2 = CShareData.YAdjustStepError;
            }
            if ((YAdjustStepError1 > 500) && (YAdjustStepError2 > 500))
            {
                LabelUpdate(Status, "垂直方向未找到边界");
                return CShareData.ReturnValueAdjust.FAILURE;//零点偏离太多
            }

            if (YAdjustFlag1 && (Math.Abs(YAdjustStepError1) <= 1) && (Math.Abs(YAdjustStepError2) <= 1))
            {
                YAdjustDoneFlag = true;
                LabelUpdate(Status, "垂直误差小于1步，校准成功！");
            }
            else
            {
                if (Math.Abs(YAdjustStepError1 - YAdjustStepError2) <= 15)
                {
                    //回到零点并置位
                    MoveFunc(CShareData.MoveDirection.Up, CShareData.YEdgeStepSave, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                    YAdjustSetZero = true;
                    YAdjustDoneFlag = true;
                    LabelUpdate(Status, "垂直校准成功，重新置零！");
                }
                else
                {
                    LabelUpdate(Status, "-------------------------------两次垂直校准步数误差相差太大！");

                    if (CShareData.ReturnValueAdjust.SUCCESS == YAdjustFunc(serialPort1, Status, pBStatus))
                    {
                        YAdjustStepError3 = CShareData.YAdjustStepError;
                        if ((Math.Abs(YAdjustStepError1 - YAdjustStepError3) <= 5) || (Math.Abs(YAdjustStepError2 - YAdjustStepError3) <= 5))
                        {
                            MoveFunc(CShareData.MoveDirection.Up, CShareData.YEdgeStepSave, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                            YAdjustSetZero = true;
                            YAdjustDoneFlag = true;
                            LabelUpdate(Status, "垂直校准成功，重新置零！");
                        }
                        else
                        {
                            LabelUpdate(Status, "-------------------------------垂直步数错误！");
                            return CShareData.ReturnValueAdjust.FAILURE;//概率明显错误
                        }
                    }
                    else
                    {
                        YAdjustFunc(serialPort1, Status, pBStatus);
                        YAdjustStepError3 = CShareData.YAdjustStepError;
                        if ((Math.Abs(YAdjustStepError1 - YAdjustStepError3) <= 5) || (Math.Abs(YAdjustStepError2 - YAdjustStepError3) <= 5))
                        {
                            MoveFunc(CShareData.MoveDirection.Up, CShareData.YEdgeStepSave, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                            YAdjustSetZero = true;
                            YAdjustDoneFlag = true;
                            LabelUpdate(Status, "垂直校准成功，重新置零！");
                        }
                        else
                        {
                            LabelUpdate(Status, "-------------------------------垂直步数错误！");
                            return CShareData.ReturnValueAdjust.FAILURE;//概率明显错误
                        }
                    }
                }

            }
            string str = "--第1次误差：水平" + XAdjustStepError1.ToString() + "步，垂直" + YAdjustStepError1.ToString() + "步";
            str += "--第2次误差：水平" + XAdjustStepError2.ToString() + "步，垂直" + YAdjustStepError2.ToString() + "步";
            LabelUpdate(Status, str);


            if (XAdjustSetZero && YAdjustSetZero)   //当水平零点和垂直零点均变时，二次校准
            {
                LabelUpdate(Status, "水平、垂直均重新置零，需再次校准确认！");

                if (CShareData.ReturnValueAdjust.SUCCESS == XAdjustFunc(serialPort1, Status, pBStatus))
                {
                    if (Math.Abs(CShareData.XAdjustStepError) < 1)      //由于上次步数有所调整，因此有1步误差需要校准回来
                    {
                        XAdjustDoneFlag = true;
                        LabelUpdate(Status, "水平误差小于1步！");
                    }
                    else if (Math.Abs(CShareData.XAdjustStepError) <= 5)
                    {
                        //回到零点并置位
                        MoveFunc(CShareData.MoveDirection.Left, CShareData.XEdgeStepSave, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                        XAdjustDoneFlag = true;
                        LabelUpdate(Status, "水平，再次校准确认！--重新置零");
                    }
                    else
                    {
                        XAdjustDoneFlag = false;
                    }
                }

                if (CShareData.ReturnValueAdjust.SUCCESS == YAdjustFunc(serialPort1, Status, pBStatus))
                {
                    if (Math.Abs(CShareData.YAdjustStepError) <= 1)
                    {
                        YAdjustDoneFlag = true;
                        LabelUpdate(Status, "垂直误差小于1步！");
                    }
                    else if (Math.Abs(CShareData.YAdjustStepError) <= 10)
                    {
                        //回到零点并置位
                        MoveFunc(CShareData.MoveDirection.Up, CShareData.YEdgeStepSave, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                        YAdjustDoneFlag = true;
                        LabelUpdate(Status, "垂直，再次校准确认！--重新置零");
                    }
                    else
                    {
                        YAdjustDoneFlag = false;
                    }
                }
            }

            bool XStepErrorFlag = false;
            bool YStepErrorFlag = false;
            //if ((false == XAdjustSetZero)  || (false == YAdjustSetZero))
            {
                if (Math.Abs(CShareData.XAdjustStepError) > 1)
                {
                    XStepErrorFlag = true;
                }
                if (Math.Abs(CShareData.YAdjustStepError) > 1)
                {
                    YStepErrorFlag = true;
                }
            }
            if (XStepErrorFlag)
            {
                LabelUpdate(Status, "水平步数存在误差，校准确认！");

                if (CShareData.ReturnValueAdjust.SUCCESS == XAdjustFunc(serialPort1, Status, pBStatus))
                {
                    if (Math.Abs(CShareData.XAdjustStepError) <= 1)
                    {
                        XAdjustDoneFlag = true;
                        LabelUpdate(Status, "水平误差小于1步！");
                    }
                    else if (Math.Abs(CShareData.XAdjustStepError) <= 5)
                    {
                        //回到零点并置位
                        MoveFunc(CShareData.MoveDirection.Left, CShareData.XEdgeStepSave, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                        LabelUpdate(Status, "水平校准成功，重新置零！");
                        XAdjustDoneFlag = true;
                    }
                    else
                    {
                        XAdjustDoneFlag = false;
                    }
                }
            }
            if (YStepErrorFlag)
            {
                LabelUpdate(Status, "垂直步数存在误差，校准确认！");
                if (CShareData.ReturnValueAdjust.SUCCESS == YAdjustFunc(serialPort1, Status, pBStatus))
                {
                    if (Math.Abs(CShareData.YAdjustStepError) <= 1)
                    {
                        YAdjustDoneFlag = true;
                        LabelUpdate(Status, "垂直误差小于1步！");
                    }
                    else if (Math.Abs(CShareData.YAdjustStepError) <= 10)
                    {
                        //回到零点并置位
                        MoveFunc(CShareData.MoveDirection.Up, CShareData.YEdgeStepSave, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                        LabelUpdate(Status, "垂直校准成功，重新置零！");
                        YAdjustDoneFlag = true;
                    }
                    else
                    {
                        YAdjustDoneFlag = false;
                    }
                }
            }





            bool XProbErrorFlag = false;
            bool YProbErrorFlag = false;
            if (Math.Abs(CShareData.XAdjustProb - CShareData.XProbSave) > 0.2)
            {
                XProbErrorFlag = true;
            }
            if (Math.Abs(CShareData.YAdjustProb - CShareData.YProbSave) > 0.05)
            {
                YProbErrorFlag = true;
            }
            if (XProbErrorFlag)
            {
                LabelUpdate(Status, "水平概率误差较大，再次校准确认！");

                if (CShareData.ReturnValueAdjust.SUCCESS == XAdjustFunc(serialPort1, Status, pBStatus))
                {
                    if (Math.Abs(CShareData.XAdjustStepError) <= 1)
                    {
                        XAdjustDoneFlag = true;
                        LabelUpdate(Status, "水平误差小于1步！");
                    }
                    else if (Math.Abs(CShareData.XAdjustStepError) <= 5)
                    {
                        //回到零点并置位
                        MoveFunc(CShareData.MoveDirection.Left, CShareData.XEdgeStepSave, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                        LabelUpdate(Status, "水平校准成功，重新置零！");
                        XAdjustDoneFlag = true;
                    }
                    else
                    {
                        XAdjustDoneFlag = false;
                    }
                }
            }
            if (YProbErrorFlag)
            {
                LabelUpdate(Status, "垂直概率误差较大，再次校准确认！");
                if (CShareData.ReturnValueAdjust.SUCCESS == YAdjustFunc(serialPort1, Status, pBStatus))
                {
                    if (Math.Abs(CShareData.YAdjustStepError) <= 1)
                    {
                        YAdjustDoneFlag = true;
                        LabelUpdate(Status, "垂直误差小于1步！");
                    }
                    else if (Math.Abs(CShareData.YAdjustStepError) <= 10)
                    {
                        //回到零点并置位
                        MoveFunc(CShareData.MoveDirection.Up, CShareData.YEdgeStepSave, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                        LabelUpdate(Status, "垂直校准成功，重新置零！");
                        YAdjustDoneFlag = true;
                    }
                    else
                    {
                        YAdjustDoneFlag = false;
                    }
                }
            }



            if ((Math.Abs(CShareData.XAdjustProb - CShareData.XProbSave) > 0.4) || (Math.Abs(CShareData.YAdjustProb - CShareData.YProbSave) > 0.2))
            {
                LabelUpdate(Status, "概率明显错误！" + str);
                return CShareData.ReturnValueAdjust.FAILURE;//概率明显错误
            }


            ReturnZeroFunc(serialPort1, Status, null);

            if (XAdjustDoneFlag && YAdjustDoneFlag)
            {
                LabelUpdate(Status, "校准成功！" + str);
                ProgressBarUpdate(pBStatus, 100);
                return CShareData.ReturnValueAdjust.SUCCESS;
            }
            else if (XAdjustDoneFlag)
            {
                LabelUpdate(Status, "仅水平校准成功！" + str);
                ProgressBarUpdate(pBStatus, 50);
                return CShareData.ReturnValueAdjust.SUCCESS_HRZ;
            }
            else if (YAdjustDoneFlag)
            {
                LabelUpdate(Status, "仅垂直校准成功！" + str);
                ProgressBarUpdate(pBStatus, 50);
                return CShareData.ReturnValueAdjust.SUCCESS_VER;
            }
            else
            {
                LabelUpdate(Status, "水平、垂直校准均失败！" + str);
                ProgressBarUpdate(pBStatus, 0);
                return CShareData.ReturnValueAdjust.FAILURE;
            }
            //return XYAdjustResult;

        }
        #endregion
        #endregion

        #region X、Y粗略回零
        #region X粗略回零
        public bool XRoughReturnZeroFunc(int YPos, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            //CShareData.RoughReturnZeroJudgeTime = 0;    //扫描判断次数

            Int16 XMoveMaxStep = CShareData.XCIRCLESTEP;    //走一圈需要3200步
            Int16 PointCnt = 129; //每次扫描得到的点数
            Int16 XMoveStepSample = Convert.ToInt16(XMoveMaxStep / (PointCnt - 1));  //转1周，测距128个点

            Int16 MotorFreq = Convert.ToInt16(1250 * 0.1125 / CShareData.XSTEPANGLE);   //电机转动频率
            Byte SA = 3;    //如果SA=1，出现4的概率比较大，SA=2，一般不会出现4，但SA不能太大
            int MF = MotorFreq / XMoveStepSample * SA;


            Int16 YMoveMaxStep = Convert.ToInt16(180.0 / CShareData.YSTEPANGLE);    //走一圈需要6400步
            //Int16 YPointCnt = 129; //每次扫描得到的点数 zzm 0725 change 65 to 129
            Int16 YMoveStepSample = Convert.ToInt16(YMoveMaxStep / (PointCnt - 1));  //转1周，测距128个点


            Int16 YMotorFreq = Convert.ToInt16(1250 * 0.1125 / CShareData.YSTEPANGLE);   //电机转动频率
            //Byte YSA = 3;    //如果SA=1，出现4的概率比较大，SA=2，一般不会出现4，但SA不能太大
            int YMF = MotorFreq / YMoveStepSample * SA;

            //----------------------------------Z----子类--Len_H-Len_L-Scan------YPos----XStartPos----XEndPos---Delay------MF------SA---Point--Check 
            byte[] ZScanCommand = new byte[] { 0x5A, 0x03, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x80, 0x00, 0x1D, 0x4C, 0x01, 0x81, 0xCA };
            Int16 ScanTime = 1;      //the Scan time (default:1)
            ZScanCommand[4] = Convert.ToByte(ScanTime & 0x00FF);
            ZScanCommand[5] = Convert.ToByte((YPos >> 8) & 0x00FF);
            ZScanCommand[6] = Convert.ToByte((YPos >> 0) & 0x00FF);
            ZScanCommand[7] = 0;
            ZScanCommand[8] = 0;
            ZScanCommand[9] = Convert.ToByte(((0 - XMoveMaxStep) >> 8) & 0x00FF);
            ZScanCommand[10] = Convert.ToByte((XMoveMaxStep >> 0) & 0x00FF);
            ZScanCommand[11] = 0;
            ZScanCommand[12] = Convert.ToByte((MF >> 8) & 0x00FF);
            ZScanCommand[13] = Convert.ToByte((MF >> 0) & 0x00FF);
            ZScanCommand[14] = SA;
            ZScanCommand[15] = Convert.ToByte(PointCnt);

            Int32[] ZReturnZeroDistance = new Int32[PointCnt];  //读取的距离值
            CShareData.spReceivedDataLenIdeal = 2 * PointCnt + 16;     //every scan you should receive 224 bytes -- 0x53 0x01 Len_H Len_L 0x00 0x00 0x00 ---- 2*N-data CheckSum
            DataWrite(serialPort1, ZScanCommand);   //send command
            CShareData.XShift = 0 - XMoveMaxStep;   //命令发出后，电机一般就要转动
            CShareData.YShift = 0;

            //for (int i = 0; i < ScanTime; i++)
            {
                Int32 WaitTime = 5 * CShareData.spSemTimeout + Math.Abs(CShareData.XShift) + Math.Abs(CShareData.YShift);
                if (CShareData.spSem.WaitOne(WaitTime) == false)    //wait sem
                {
                    LabelUpdate(Status, "串口接收等待超时！");
                    ProgressBarUpdate(pBStatus, 0);
                    return false;
                }
                if (CShareData.spErrorFlag)
                {
                    LabelUpdate(Status, "串口错误！");
                    ProgressBarUpdate(pBStatus, 0);
                    return false;
                }
                ProgressBarUpdate(pBStatus, 50);

                if (CShareData.spReceivedDataLen == CShareData.spReceivedDataLenIdeal)  //长度
                {
                    if (JudegCheckSum(CShareData.spReceivedData, CShareData.spReceivedDataLen)) //校验和
                    {
                        if ((CShareData.spReceivedData[0] == 0x5A) && (CShareData.spReceivedData[1] == 0x03))
                        {
                            for (int j = 0; j < PointCnt; j++)
                            {
                                //find the value of every measure,and change to real distance(mm)
                                //if (CShareData.spReceivedData[6] == 0x01)    //若是反向扫描，纠正数据排列顺序
                                //    ZReturnZeroDistance[i, PointCnt - 1 - j] = 4 * (CShareData.spReceivedData[2 * j + 15] * 256 + CShareData.spReceivedData[2 * j + 16]);
                                //else
                                ZReturnZeroDistance[j] = 4 * (CShareData.spReceivedData[2 * j + 15] * 256 + CShareData.spReceivedData[2 * j + 16]);
                                //CShareData.XShift = 0 - XMoveMaxStep;
                            }
                        }
                        else
                        {
                            ErrorCheck(Status);  //检查错误
                            ProgressBarUpdate(pBStatus, 0);
                            return false;
                        }
                    }
                    else
                    {
                        LabelUpdate(Status, "校验证和错误！");
                        ProgressBarUpdate(pBStatus, 0);
                        return false;
                    }

                }
                else
                {
                    ErrorCheck(Status);  //检查错误
                    LabelUpdate(Status, "仅接收到" + CShareData.spReceivedDataLen.ToString() + "个数据！");
                    ProgressBarUpdate(pBStatus, 0);
                    return false;
                }


                if (CShareData.DEBUG)
                {
                    string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                    string path = CShareData.StartPath + "X粗略回零" + TimeNow + ".txt";
                    string str = IntArrayToString(ZReturnZeroDistance, PointCnt);
                    str += "\r\n\r\n";
                    str += "所有测量点均为激光测距仪读取的距离值！\r\n";
                    str += "此次保存数据的个数：" + PointCnt.ToString() + "\r\n";
                    str += "保存成功！";
                    WriteStrToTXT(path, str);
                }

                /**********************************找箱子外的点*************************************/
                int XMoveBack = 0;          //移动的多了，需要返回的步数
                int OutBoxCnt = 0;
                for (OutBoxCnt = 0; OutBoxCnt < PointCnt; OutBoxCnt++)      //先找机箱子外的有效距离值
                {
                    if (ZReturnZeroDistance[OutBoxCnt] > CShareData.BOXMAXDISTANCE)
                    {
                        if (OutBoxCnt == 0)  //此时就在原始位置
                        {
                            XMoveBack = 0;
                        }
                        else
                        {
                            XMoveBack = PointCnt - 1 - OutBoxCnt;
                        }
                        goto FindOutBox;
                    }
                }
            /* SZ 0729
            for (OutBoxCnt = 0; OutBoxCnt < PointCnt; OutBoxCnt++)  //如果没有效距离值，‘4’也被认为机箱外，这样做一定程度避免误判机箱内的黑色物体
            {
                if (ZReturnZeroDistance[OutBoxCnt] == 4)
                {
                    if (OutBoxCnt == 0)  //此时就在原始位置
                    {
                        XMoveBack = 0;
                    }
                    else
                    {
                        XMoveBack = PointCnt - 1 - OutBoxCnt;
                    }
                    goto FindOutBox;
                }
            }
             */

            FindOutBox:
                /**********************************返回已经找到的零点**************************************/
                if (OutBoxCnt < PointCnt)   //找到零点
                {
                    Int16 XMoveBackStep = Convert.ToInt16(XMoveBack * XMoveStepSample);//计算步数
                    //MoveLeftFunc(XMoveBackStep, 1);    //把多移动的再倒回来,然后将此位置作为0位置
                    //把多移动的再倒回来,然后将此位置作为0位置
                    MoveFunc(CShareData.MoveDirection.Right, XMoveBackStep, CShareData.SetZeroValue.Set, serialPort1, Status, null);

                    if (ScanJudgeZero(serialPort1, Status, pBStatus))  //二维扫描，至少10个连续有效点才为零点，防止误判
                    {
                        ProgressBarUpdate(pBStatus, 100);
                        return true;
                    }
                    else
                    {
                        LabelUpdate(Status, "扫描判断未通过！");
                        ProgressBarUpdate(pBStatus, 0);
                        return false;
                    }
                }
                else
                {
                    LabelUpdate(Status, "暂无满足条件的点……");
                    //LabelUpdate(Status, "当前位置--水平：" + CShareData.XShift.ToString() + "--" + "垂直：" + CShareData.YShift.ToString());
                    ProgressBarUpdate(pBStatus, 0);
                    return false;
                }
            }
        }
        #endregion

        #region Y粗略回零
        public bool YRoughReturnZeroFunc(int XPos, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            if (Z04ScanFunc(0, serialPort1, Status, pBStatus)) //分两次旋转完成转动一周，（无法一次转动6400步，最大4095）
            {
                return true;
            }
            else
            {
                SetZeroFunc(serialPort1, Status, pBStatus); //先设置0，再转180度
                if (Z04ScanFunc(0, serialPort1, Status, pBStatus))
                {
                    return true;
                }
                else
                {
                    SetZeroFunc(serialPort1, Status, pBStatus); //两次转180度，相当于回到原点，20150726
                    return false;
                }
            }
        }
        public bool Z04ScanFunc(int XPos, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            CShareData.RoughReturnZeroJudgeTime = 0;    //扫描判断次数

            Int16 YMoveMaxStep = Convert.ToInt16(180.0 / CShareData.YSTEPANGLE);    //走一圈需要6400步
            Int16 PointCnt = 129; //每次扫描得到的点数 zzm 0725 change 65 to 129
            Int16 YMoveStepSample = Convert.ToInt16(YMoveMaxStep / (PointCnt - 1));  //转1周，测距128个点


            Int16 MotorFreq = Convert.ToInt16(1250 * 0.1125 / CShareData.YSTEPANGLE);   //电机转动频率
            Byte SA = 3;    //如果SA=1，出现4的概率比较大，SA=2，一般不会出现4，但SA不能太大
            int MF = MotorFreq / YMoveStepSample * SA;

            //----------------------------------Z----子类--Len_H-Len_L-Scan------XPos----YStartPos----YEndPos---Delay--MF_H-MF_L---SA---Point--Check 
            byte[] ZScanCommand = new byte[] { 0x5A, 0x04, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x80, 0x00, 0x1D, 0x4C, 0x01, 0x81, 0xCA };
            Int16 ScanTime = 1;      //the Scan time (default:1)
            ZScanCommand[4] = Convert.ToByte(ScanTime & 0x00FF);
            ZScanCommand[5] = Convert.ToByte((XPos >> 8) & 0x00FF);
            ZScanCommand[6] = Convert.ToByte((XPos >> 0) & 0x00FF);
            ZScanCommand[7] = 0;
            ZScanCommand[8] = 0;
            ZScanCommand[9] = Convert.ToByte((YMoveMaxStep >> 8) & 0x00FF);
            ZScanCommand[10] = Convert.ToByte((YMoveMaxStep >> 0) & 0x00FF);
            ZScanCommand[11] = 0;
            ZScanCommand[12] = Convert.ToByte((MF >> 8) & 0x00FF);
            ZScanCommand[13] = Convert.ToByte((MF >> 0) & 0x00FF);
            ZScanCommand[14] = SA;
            ZScanCommand[15] = Convert.ToByte(PointCnt);

            Int32[] ZReturnZeroDistance = new Int32[PointCnt];  //读取的距离值
            CShareData.spReceivedDataLenIdeal = 2 * PointCnt + 16;     //every scan you should receive 224 bytes -- 0x53 0x01 Len_H Len_L 0x00 0x00 0x00 ---- 2*N-data CheckSum
            DataWrite(serialPort1, ZScanCommand);   //send command
            CShareData.XShift = 0;
            CShareData.YShift = YMoveMaxStep;  //命令发出后，电机一般就要转动

            //for (int i = 0; i < ScanTime; i++)
            {
                Int32 WaitTime = 5 * CShareData.spSemTimeout + Math.Abs(CShareData.XShift) + Math.Abs(CShareData.YShift);
                if (CShareData.spSem.WaitOne(WaitTime) == false)    //wait sem
                {
                    LabelUpdate(Status, "串口接收等待超时！");
                    ProgressBarUpdate(pBStatus, 0);
                    return false;
                }
                if (CShareData.spErrorFlag)
                {
                    LabelUpdate(Status, "串口错误！");
                    ProgressBarUpdate(pBStatus, 0);
                    return false;
                }

                ProgressBarUpdate(pBStatus, 50);

                if (CShareData.spReceivedDataLen == CShareData.spReceivedDataLenIdeal)  //长度
                {
                    if (JudegCheckSum(CShareData.spReceivedData, CShareData.spReceivedDataLen)) //校验和
                    {
                        if ((CShareData.spReceivedData[0] == 0x5A) && (CShareData.spReceivedData[1] == 0x04))
                        {
                            for (int j = 0; j < PointCnt; j++)
                            {
                                //find the value of every measure,and change to real distance(mm)
                                //if (CShareData.spReceivedData[6] == 0x01)    //若是反向扫描，纠正数据排列顺序
                                //    ZReturnZeroDistance[i, PointCnt - 1 - j] = 4 * (CShareData.spReceivedData[2 * j + 15] * 256 + CShareData.spReceivedData[2 * j + 16]);
                                //else
                                ZReturnZeroDistance[j] = 4 * (CShareData.spReceivedData[2 * j + 15] * 256 + CShareData.spReceivedData[2 * j + 16]);
                            }
                        }
                        else
                        {
                            ErrorCheck(Status);  //检查错误
                            ProgressBarUpdate(pBStatus, 0);
                            return false;
                        }
                    }
                    else
                    {
                        LabelUpdate(Status, "校验证和错误！");
                        ProgressBarUpdate(pBStatus, 0);
                        return false;
                    }

                }
                else
                {
                    ErrorCheck(Status);  //检查错误
                    LabelUpdate(Status, "仅接收到" + CShareData.spReceivedDataLen.ToString() + "个数据！");
                    ProgressBarUpdate(pBStatus, 0);
                    return false;
                }


                if (CShareData.DEBUG)
                {
                    string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                    string path = CShareData.StartPath + "Y粗略回零" + TimeNow + ".txt";
                    string str = IntArrayToString(ZReturnZeroDistance, PointCnt);
                    str += "\r\n\r\n";
                    str += "所有测量点均为激光测距仪读取的距离值！\r\n";
                    str += "此次保存数据的个数：" + PointCnt.ToString() + "\r\n";
                    str += "保存成功！";
                    WriteStrToTXT(path, str);
                }

                /**********************************找箱子外的点*************************************/
                int YMoveBack = 0;          //移动的多了，需要返回的步数
                int OutBoxCnt = 0;
                for (OutBoxCnt = 0; OutBoxCnt < PointCnt; OutBoxCnt++)      //先找机箱子外的有效距离值
                {
                    if (ZReturnZeroDistance[OutBoxCnt] > CShareData.BOXMAXDISTANCE)
                    {
                        //if (OutBoxCnt == 0)  //此时就在原始位置
                        //{
                        //    YMoveBack = 0;
                        //}
                        //else
                        {
                            YMoveBack = PointCnt - 1 - OutBoxCnt;
                        }
                        goto FindOutBox;
                    }
                }
            /*SZ 0729
            for (OutBoxCnt = 0; OutBoxCnt < PointCnt; OutBoxCnt++)  //如果没有效距离值，‘4’也被认为机箱外，这样做一定程度避免误判机箱内的黑色物体
            {
                if (ZReturnZeroDistance[OutBoxCnt] == 4)
                {
                    if (OutBoxCnt == 0)  //此时就在原始位置
                    {
                        YMoveBack = 0;
                    }
                    else
                    {
                        YMoveBack = PointCnt - 1 - OutBoxCnt;
                    }
                    goto FindOutBox;
                }
            }
             */

            FindOutBox:
                /**********************************返回已经找到的零点**************************************/
                if (OutBoxCnt < PointCnt)   //找到零点
                {
                    Int16 YMoveBackStep = Convert.ToInt16(YMoveBack * YMoveStepSample);//计算步数
                    //MoveDownFunc(YMoveBackStep, 1);    //把多移动的再倒回来,然后将此位置作为0位置
                    MoveFunc(CShareData.MoveDirection.Down, YMoveBackStep, CShareData.SetZeroValue.Set, serialPort1, Status, null);

                    if (ScanJudgeZero(serialPort1, Status, pBStatus))  //二维扫描，至少10个连续有效点才为零点，防止误判
                    {
                        ProgressBarUpdate(pBStatus, 100);
                        return true;
                    }
                    else
                    {
                        LabelUpdate(Status, "扫描判断未通过！");
                        ProgressBarUpdate(pBStatus, 0);
                        return false;
                    }
                }
                else
                {
                    //LabelUpdate(Status, "暂无满足条件的点……");
                    LabelUpdate(Status, "垂直粗略回零---当前位置--水平：" + CShareData.XShift.ToString() + "--" + "垂直：" + CShareData.YShift.ToString());
                    ProgressBarUpdate(pBStatus, 0);
                    return false;
                }
            }
        }
        #endregion
        public static int Q12Port = -1;
        public bool YQ12DetectionFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            Int16 YMoveMaxStep = Convert.ToInt16(360.0 / CShareData.YSTEPANGLE);    //走一圈需要6400步            
            Int16 YStepSample = 10;  //对Q12采样的步数
            int YMoveTime = YMoveMaxStep / YStepSample; //每次扫描得到的点数 zzm 0725 change 65 to 129
            bool Q12Value = false;
            if (!Susi4.APIs.SusiGPIO.Available) return false;
            if (Q12Port == -1)
            {
                Q12Port = Convert.ToInt32(Parameter.glb_Parameter.system_parameter["Q12Port"].InnerText);
            }

            for (int i = 0; i < YMoveTime; i++)
            {
                MoveFunc(CShareData.MoveDirection.Down, YStepSample, CShareData.SetZeroValue.UnSet, serialPort1, null, null);
                UInt32 Value;
                UInt32 stat;
                UInt32 id = SusiGPIO.DevList[0].ID;
                stat = SusiGPIO.SusiGPIOGetLevel(id, 0xFFFFu, out Value);
                if (stat == SusiStatus.SUSI_STATUS_SUCCESS)
                {
                    string binary_code = Convert.ToString(Value, 2);
                    binary_code = binary_code.PadLeft(16, '0');
                    if (binary_code[15 - Q12Port] == '0')
                    {
                        if (CShareData.Q12Steps > 0)
                        {
                            MoveFunc(CShareData.MoveDirection.Up, CShareData.Q12Steps, CShareData.SetZeroValue.UnSet, serialPort1, null, null);
                        }
                        else
                        {
                            MoveFunc(CShareData.MoveDirection.Down, Math.Abs(CShareData.Q12Steps), CShareData.SetZeroValue.UnSet, serialPort1, null, null);
                        }
                        return true;
                    }
                }
            }
            return Q12Value;
        }

        public bool YQ12FineDetectionFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            Int16 YMoveMaxStep = Convert.ToInt16(360.0 / CShareData.YSTEPANGLE);    //走一圈需要6400步            
            Int16 YStepSample = 10;  //对Q12采样的步数
            int YMoveTime = YMoveMaxStep / YStepSample; //每次扫描得到的点数 zzm 0725 change 65 to 129
            bool Q12Value = false;
           if (!Susi4.APIs.SusiGPIO.Available) return false;
            if (Q12Port == -1)
            {
                Q12Port = Convert.ToInt32(Parameter.glb_Parameter.system_parameter["Q12Port"].InnerText);
            }

            for (int i = 0; i < YMoveTime; i++)
            {
                MoveFunc(CShareData.MoveDirection.Down, YStepSample, CShareData.SetZeroValue.UnSet, serialPort1, null, null);
                UInt32 Value;
                UInt32 stat;
                UInt32 id = SusiGPIO.DevList[0].ID;
                stat = SusiGPIO.SusiGPIOGetLevel(id, 0xFFFFu, out Value);
                if (stat == SusiStatus.SUSI_STATUS_SUCCESS)
                {
                    string binary_code = Convert.ToString(Value, 2);
                    binary_code = binary_code.PadLeft(16, '0');
                    if (binary_code[15 - Q12Port] == '0')
                    {
                        int j = 0;
                        for (j = 0; i < YMoveTime - i; i++)
                        {
                            MoveFunc(CShareData.MoveDirection.Down, YStepSample, CShareData.SetZeroValue.UnSet, serialPort1, null, null);
                            stat = SusiGPIO.SusiGPIOGetLevel(id, 0xFFFFu, out Value);
                            if (stat == SusiStatus.SUSI_STATUS_SUCCESS)
                            {
                                binary_code = Convert.ToString(Value, 2);
                                binary_code = binary_code.PadLeft(16, '0');
                                if (binary_code[15 - Q12Port] != '0')
                                {
                                    break;
                                }
                            }
                        }
                        MoveFunc(CShareData.MoveDirection.Up, Convert.ToInt16(j * YStepSample / 2), CShareData.SetZeroValue.UnSet, serialPort1, null, null);
                        LabelUpdate(Status, "Q12检测到垂直镜！步数："+i.ToString());
                        if (CShareData.Q12Steps > 0)
                        {
                            MoveFunc(CShareData.MoveDirection.Up, CShareData.Q12Steps, CShareData.SetZeroValue.UnSet, serialPort1, null, null);
                        }
                        else
                        {
                            MoveFunc(CShareData.MoveDirection.Down, Math.Abs(CShareData.Q12Steps), CShareData.SetZeroValue.UnSet, serialPort1, null, null);
                        }
                        return true;
                    }
                }
            }
            return Q12Value;
        }

        #region XY粗略回零
        public bool XYRoughReturnZeroFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            CShareData.RoughReturnZeroJudgeTime = 0;

            if (YQ12FineDetectionFunc(serialPort1, Status, null))
            {
                //if (Math.Abs(CShareData.YShift) > 4095)
                {
                    SetZeroFunc(serialPort1, Status, null);
                }
                
                if (XRoughReturnZeroFunc(CShareData.YShift, serialPort1, Status, null))    //找到零点
                {
                    ProgressBarUpdate(pBStatus, 100);
                    MoveFunc(CShareData.MoveDirection.Left, 20, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                    //MoveFunc(CShareData.MoveDirection.Up, 50, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                    LabelUpdate(Status, "成功找到零点！--粗略回零成功");
                    return true;
                }
                else
                {
                    LabelUpdate(Status, "检测垂直镜的背面！");
                    MoveFunc(CShareData.MoveDirection.Up, 3200, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);//垂直镜转到背面
                    SetZeroFunc(serialPort1, Status, null);
                    if (XRoughReturnZeroFunc(CShareData.YShift, serialPort1, Status, null))    //找到零点
                    {
                        ProgressBarUpdate(pBStatus, 100);
                        MoveFunc(CShareData.MoveDirection.Left, 20, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                        //MoveFunc(CShareData.MoveDirection.Up, 50, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                        LabelUpdate(Status, "成功找到零点！--粗略回零成功");
                        return true;
                    }
                    else
                    {
                        MoveFunc(CShareData.MoveDirection.Down, 3200, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                        return false;
                    }
                }
            }
            else
            {
                LabelUpdate(Status, "ERROR:Q12未检测到垂直镜！");
                return false;
            }
        }

        public bool XYRoughReturnZeroCircleFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            CShareData.RoughReturnZeroJudgeTime = 0;

            LabelUpdate(Status, "当前位置--水平：" + CShareData.XShift.ToString() + "--" + "垂直：" + CShareData.YShift.ToString());
            if (YRoughReturnZeroFunc(0, serialPort1, Status, null))   //Y不在零点，X在零点时，有效地缩短时间
            {
                MoveFunc(CShareData.MoveDirection.Up, 50, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                return true;
            }
            LabelUpdate(Status, "仅垂直粗略回零失败，需同时调整水平镜子！");

            Int16 YMoveMaxStep = CShareData.YCIRCLESTEP;    //走一圈需要3200步
            Int16 YMoveStep = 50;  //每次调整32步，理论步数是59步
            int YMoveTime = YMoveMaxStep / YMoveStep; //每次扫描得到的点数

            int YMovePosCnt = 0;
            for (YMovePosCnt = 0; YMovePosCnt < YMoveTime; YMovePosCnt++)
            {
                ProgressBarUpdate(pBStatus, Convert.ToInt32(1.0 * YMovePosCnt / YMoveTime * 100));
                LabelUpdate(Status, "垂直镜调整次数--" + YMovePosCnt.ToString());

                if (XRoughReturnZeroFunc(0, serialPort1, Status, null))    //找到零点
                {
                    break;
                }
                else
                {
                    //MoveUpFunc(YMoveStep, 1);   //置位主要针对水平镜，使其不需要回到起始位置
                    //SetZeroFunc();
                    MoveFunc(CShareData.MoveDirection.Up, YMoveStep, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                }
            }

            if (YMovePosCnt < YMoveTime)
            {
                ProgressBarUpdate(pBStatus, 100);
                MoveFunc(CShareData.MoveDirection.Left, 20, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                MoveFunc(CShareData.MoveDirection.Up, 50, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                LabelUpdate(Status, "成功找到零点！--粗略回零成功");
                return true;
            }
            else
            {
                ProgressBarUpdate(pBStatus, 0);
                LabelUpdate(Status, "请检查玻璃窗是否被遮挡！");
                return false;
                
            }
        }



        public bool Find30Meter(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            Int16 YMoveMaxStep = 500;  //最多找500步        
            Int16 YStepSample = 2;  //采样的步数
            int YMoveTime = YMoveMaxStep / YStepSample; //每次扫描得到的点数 zzm 0725 change 65 to 129
            MoveFunc(CShareData.MoveDirection.Up, 100, CShareData.SetZeroValue.UnSet, serialPort1, null, null);

            for (int i = 0; i < YMoveTime; i++)
            {
                MoveFunc(CShareData.MoveDirection.Down, YStepSample, CShareData.SetZeroValue.UnSet, serialPort1, null, null);
                double distance = OnePointMeasureFunc(100, serialPort1, Status, pBStatus);
                if (distance > 20000 && distance < 30000)
                {
                    LabelUpdate(Status, "成功找到地面！距离:" + (distance / 1000).ToString() + "米！");
                    return true;
                }
            }
            MoveFunc(CShareData.MoveDirection.Up, YMoveMaxStep, CShareData.SetZeroValue.UnSet, serialPort1, null, null);
            return false;
        }

        public int RoughReturnZeroSteps(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            CShareData.Q12Steps = 0;
            int Step30, StepQ12;
            if (Find30Meter(serialPort1, Status, pBStatus))
            {
                Step30 = CShareData.YShift;
            }
            else
            {
                LabelUpdate(Status, "ERROR:未找到30米处地面！");
                return -99999;
            }
            if (YQ12FineDetectionFunc(serialPort1, Status, null))
            {
                LabelUpdate(Status, "检测到垂直镜！");
                StepQ12 = CShareData.YShift;
            }
            else
            {
                LabelUpdate(Status, "ERROR:Q12未检测到垂直镜！");
                return -99999;
            }
            int result = Step30 - StepQ12;
            if (result < -1600) result += 3200;
            if (result > 1600) result -= 3200;
            CShareData.Q12Steps = Convert.ToInt16(result);
            ReturnZeroFunc(serialPort1, Status, pBStatus);
            return result;
        }

        #endregion

        #region 扫描数据中连续的有效点
        //找扫描数据中连续的有效点
        private bool ScanJudgeZero(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            CShareData.RoughReturnZeroJudgeTime++;    //扫描判断次数

            Int16 ScanPointNum = 109;
            Int32[,] ReceivedDistance = new Int32[2, ScanPointNum];     //读取水平扫描距离值
            Int16 XStartPos = Convert.ToInt16(0 - ((ScanPointNum - 1) >> 2));
            Int16 XEndPos = Convert.ToInt16((ScanPointNum - 1) >> 2);
            ReceivedDistance = HrzScanFunc(0, XStartPos, XEndPos, serialPort1, Status, null);   //调用水平扫描函数


            string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
            string path = CShareData.StartPath + "二维" + TimeNow + ".txt";
            SaveScanDistance(path, ReceivedDistance, ScanPointNum, 2);

            ReturnZeroFunc(serialPort1, Status, null);  //回到零点
            Int16 ValidPointCnt = 0;
            if (CShareData.HrzScanFlag)
            {
                for (int i = 0; i < ScanPointNum; i++)
                {
                    //if ((ReceivedDistance[0,i]>=CShareData.BOXMAXDISTANCE) || (ReceivedDistance[0,i] == 4))
                    if (ReceivedDistance[0, i] >= CShareData.BOXMAXDISTANCE)
                    {
                        for (int j = i; j < ScanPointNum; j++)
                        {
                            //if ((ReceivedDistance[0, j] >= CShareData.BOXMAXDISTANCE) || (ReceivedDistance[0, j] == 4))
                            if (ReceivedDistance[0, j] >= CShareData.BOXMAXDISTANCE)
                            {
                                ValidPointCnt++;
                                if (ValidPointCnt >= 10)
                                {
                                    LabelUpdate(Status, "连续有效值点的个数：" + ValidPointCnt.ToString());
                                    return true;
                                }
                            }
                            else
                            {
                                ValidPointCnt = 0;  //只统计连续的，而不是统计所有值
                                break;
                            }
                        }

                    }
                    LabelUpdate(Status, "连续有效值点的个数：" + ValidPointCnt.ToString());
                }
            }
            ProgressBarUpdate(pBStatus, 0);
            return false;
        }
        #endregion
        #endregion

        #region 水平、垂直标定 自动检测

        #region 水平标定 使用概率
        public CShareData.ReturnValueCalibHrz HrzCalibrationProbFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);

            Int16 DistanceSubMin = 3000;    //两点的距离差大于这个值时，视为障碍物            
            Int16 ScanPointNum = CShareData.YSCANPOINTNUM;       //扫描点数        

            Int16 MeasureNum = 200;          //定点测量的次数
            Int16 MeasureTimeMax = 60;

            //Int32 MeasureIdealPos = 40000;     //理想情况下的测量点位置
            //计算首次移动的步数
            //Int16 MoveDownStep = Convert.ToInt16(Math.Atan(1.0 * CShareData.InstallHeight / MeasureIdealPos) * 180 / Math.PI / AngleStep);
            Int16 MoveDownStep = (Int16)CShareData.YShift;        //扫描前向下调整的步数（安装高度5m，距离40m）


            //水平扫描
            Int32[,] ReceivedDistance = new Int32[2, ScanPointNum];     //读取水平扫描距离值
            Int16 XStartPos = Convert.ToInt16(0 - ((ScanPointNum - 1) >> 2));
            Int16 XEndPos = Convert.ToInt16((ScanPointNum - 1) >> 2);
            bool ZeroInFrameFlag = false;   //零点是否在该扫描帧中
            Int16 ZeroPositionPos = 0;  //正向数据中的零点位置
            Int16 ZeroPositionNeg = 0;  //反向数据中的零点位置
            Int16 ZeroPosition = 0;     //找出的零点平均位置

            ReceivedDistance = HrzScanFunc(MoveDownStep, XStartPos, XEndPos, serialPort1, Status, null);  //调用水平扫描函数                
            //滤掉无效数据4
            ReceivedDistance = ReplaceInvalidData(ReceivedDistance, ScanPointNum);

            for (int m = 1; m < ScanPointNum - 1; m++)  //不包含边界
            {
                //找到一个有价值的点，一般而言，人身上的点肯定是有效值，地面上的点有可能是“4”，调用ReplaceInvalidData后，用附近的值替换“4”
                if ((ReceivedDistance[0, m] - ReceivedDistance[0, m + 1]) > DistanceSubMin)    //找到一个有价值的点，人身上的点距离近，地面上的距离远
                {
                    if (Math.Abs(ReceivedDistance[0, m] - ReceivedDistance[0, m - 1]) < DistanceSubMin)    //保证不是畸变点
                    {
                        ZeroInFrameFlag = true;
                        ZeroPositionPos = Convert.ToInt16(XStartPos + Math.Round(0.5 * m));


                        for (int n = 1; n < ScanPointNum - 1; n++)  //判断反向数据
                        {
                            if ((ReceivedDistance[1, n] - ReceivedDistance[1, n + 1]) > DistanceSubMin)    //找到一个有价值的点
                            {
                                if (Math.Abs(ReceivedDistance[1, n] - ReceivedDistance[1, n - 1]) < DistanceSubMin)    //保证不是畸变点
                                {
                                    ZeroInFrameFlag = true;
                                    ZeroPositionNeg = Convert.ToInt16(XStartPos + Math.Round(0.5 * n));
                                    break;
                                }
                            }
                        }
                        if (ZeroPositionNeg == 0)   //防止平均后，数值很离谱
                        {
                            ZeroPositionNeg = ZeroPositionPos;
                        }

                        ZeroPosition = Convert.ToInt16(0.5 * (ZeroPositionPos + ZeroPositionNeg));  //此位置偏向于标记物最左边
                    }
                }
            }

            double DistanceBackground = 0;
            double DistancePerson = 0;
            bool DistanceDiffFlag = false;
            if (ZeroInFrameFlag)
            {
                if ((ZeroPositionPos > XStartPos + 3) && (ZeroPositionPos < XStartPos + ScanPointNum - 3))
                {
                    int ZeroPositionArrayIndex = 2 * (ZeroPositionPos - XStartPos);   //下标从0开始
                    DistanceBackground = (ReceivedDistance[0, ZeroPositionArrayIndex - 2] + ReceivedDistance[0, ZeroPositionArrayIndex - 3] + ReceivedDistance[0, ZeroPositionArrayIndex - 4]) / 3;
                    DistancePerson = (ReceivedDistance[0, ZeroPositionArrayIndex + 1] + ReceivedDistance[0, ZeroPositionArrayIndex + 2]) / 2;
                    if (Math.Abs(DistanceBackground - DistancePerson) > DistanceSubMin)
                    {
                        DistanceDiffFlag = true;
                    }
                    else
                    {
                        LabelUpdate(Status, "背景提取错误！");
                        return CShareData.ReturnValueCalibHrz.FAILURE;
                    }

                }
                else
                {
                    LabelUpdate(Status, "人的位置离边界太近！");
                    ProgressBarUpdate(pBStatus, 0);
                    return CShareData.ReturnValueCalibHrz.ERROR_POSITION;
                }
            }
            else
            {
                LabelUpdate(Status, "垂直步数不合适！");
                ProgressBarUpdate(pBStatus, 0);
                return CShareData.ReturnValueCalibHrz.ERROR_YSTEP;
            }


            ProgressBarUpdate(pBStatus, 50);

            Int16 ZeroPositionLeft = 0;    //标记物的左边位置
            Int16 ZeroPositionRight = 0;      //标记物的右边位置
            bool ZeroPositionLeftFlag = false;
            bool ZeroPositionRightFlag = false;
            bool ZeroPositionLRFlag = false;
            double DistanceLast = 0;
            double DistanceNow = 0;
            double Result = -1;

            double[] OnePointMeasureDistance = new double[MeasureTimeMax];     //读取水平扫描距离值
            double[] DistancePersonProb = new double[MeasureTimeMax];     //读取水平扫描距离值
            double PersonProbTemp = 0;

            int LeftIndex = 0;
            int RightIndex = 0;
            double ProbToStep = 0;
            if (DistanceDiffFlag)    //扫描到标记物，定点测距准确定位
            {
                ZeroPositionLeft = ZeroPosition;
                ZeroPositionRight = ZeroPosition;

                ZeroPosition -= 5;//保证能全部包含障碍物
                ReturnZeroFunc(serialPort1, Status, null);  //回零
                MoveFunc(CShareData.MoveDirection.Down, MoveDownStep, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                if (ZeroPosition < 0)  //移动到障碍物最左端
                {
                    MoveFunc(CShareData.MoveDirection.Left, Convert.ToInt16(0 - ZeroPosition), CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                }
                else
                {
                    MoveFunc(CShareData.MoveDirection.Right, ZeroPosition, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                }
                ZeroPositionLeftFlag = false;
                ZeroPositionRightFlag = false;
                ZeroPositionLRFlag = false;

                Result = OrgOnePointMeasureFunc(MeasureNum, serialPort1, Status, null);  //获取初始值
                if (Result == 0)//定点测距成功
                {
                    Int32[] DistanceFilter = new Int32[CShareData.OrgOnePointDistanceLen];
                    int DistanceFilterLen = 0;
                    for (int m = 0; m < CShareData.OrgOnePointDistanceLen; m++)
                    {
                        if ((CShareData.OrgOnePointDistance[m] < (DistancePerson + DistanceSubMin)) && (CShareData.OrgOnePointDistance[m] > 50))
                            DistanceFilter[DistanceFilterLen++] = CShareData.OrgOnePointDistance[m];
                    }

                    //OnePointMeasureValue.Text = Math.Round(Distance.Average()).ToString();  //calculate the average
                    PersonProbTemp = 1.0 * DistanceFilterLen / CShareData.OrgOnePointDistanceLen;
                    DistancePersonProb[0] = PersonProbTemp;
                    if (DistanceFilterLen > 100)
                    {
                        OnePointMeasureDistance[0] = DistanceFilter.Sum() / DistanceFilterLen;
                    }
                    else
                    {
                        OnePointMeasureDistance[0] = DistanceBackground;
                    }
                }
                else
                {
                    LabelUpdate(Status, "定点测距失败！");
                    return CShareData.ReturnValueCalibHrz.FAILURE;
                }

                for (int i = 1; i < MeasureTimeMax; i++)
                {

                    if (i == (MeasureTimeMax - 1))
                    {
                        LabelUpdate(Status, "移动到最大位置，仍然未找到边界！");
                        ProgressBarUpdate(pBStatus, 0);
                        return CShareData.ReturnValueCalibHrz.ERROR_MEASURE_TIMEOUT;
                    }

                    Result = OrgOnePointMeasureFunc(MeasureNum, serialPort1, Status, null);  //获取初始值
                    if (Result == 0)//定点测距成功
                    {
                        Int32[] DistanceFilter = new Int32[CShareData.OrgOnePointDistanceLen];
                        int DistanceFilterLen = 0;
                        for (int m = 0; m < CShareData.OrgOnePointDistanceLen; m++)
                        {
                            if ((CShareData.OrgOnePointDistance[m] < (DistancePerson + DistanceSubMin)) && (CShareData.OrgOnePointDistance[m] > 50))
                                DistanceFilter[DistanceFilterLen++] = CShareData.OrgOnePointDistance[m];
                        }

                        //OnePointMeasureValue.Text = Math.Round(Distance.Average()).ToString();  //calculate the average
                        PersonProbTemp = 1.0 * DistanceFilterLen / CShareData.OrgOnePointDistanceLen;
                        DistancePersonProb[i] = PersonProbTemp;
                        if (DistanceFilterLen > 100)
                        {
                            OnePointMeasureDistance[i] = DistanceFilter.Sum() / DistanceFilterLen;
                        }
                        else
                        {
                            OnePointMeasureDistance[i] = DistanceBackground;
                        }
                    }
                    else
                    {
                        LabelUpdate(Status, "定点测距失败！");
                        return CShareData.ReturnValueCalibHrz.FAILURE;
                    }

                    if (ZeroPositionLRFlag)
                    {
                        CShareData.TempStr = DistancePersonProb[LeftIndex].ToString() + "--" + DistancePersonProb[RightIndex].ToString() + "--" + ProbToStep.ToString();
                        break;
                    }

                    DistanceLast = OnePointMeasureDistance[i - 1];
                    DistanceNow = OnePointMeasureDistance[i];
                    //if (CShareData.OutOnePointDistanceValidProb > 0.1)
                    {
                        if ((DistanceLast - DistanceNow) > DistanceSubMin)  //左边界判断
                        {
                            if (ZeroPositionLeftFlag == false)
                            {
                                ZeroPositionLeftFlag = true;
                                ZeroPositionLeft = ZeroPosition;    //得到左侧的边界位置
                                LeftIndex = i;
                            }
                        }
                        if ((DistanceNow - DistanceLast) > DistanceSubMin)  //右边界判断
                        {
                            if (ZeroPositionRightFlag == false)
                            {
                                ZeroPositionRightFlag = true;
                                ZeroPositionRight = ZeroPosition;    //得到右侧的边界位置
                                RightIndex = i - 1;
                            }
                        }
                        if (ZeroPositionLeftFlag && ZeroPositionRightFlag)  //两个边界均找到
                        {
                            ProbToStep = DistancePersonProb[LeftIndex] + DistancePersonProb[RightIndex];
                            ZeroPositionLRFlag = true;

                            CShareData.TempStr = DistancePersonProb[LeftIndex].ToString() + "--" + DistancePersonProb[RightIndex].ToString() + "--" + ProbToStep.ToString();
                            //goto MoveZero;  //移动到零点位置
                            //break;
                        }
                        else
                        {

                            if (MoveFunc(CShareData.MoveDirection.Right, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, pBStatus))
                            {
                                ZeroPosition += 1;  //右移1步
                            }
                            else
                            {
                                LabelUpdate(Status, "向右移动失败！");
                                ProgressBarUpdate(pBStatus, 0);
                                return CShareData.ReturnValueCalibHrz.FAILURE;
                            }
                        }
                    }
                    //else
                    //{
                    //    LabelUpdate(Status, "无效距离值太多！");
                    //    ProgressBarUpdate(pBStatus, 0);
                    //    return false;
                    //}
                }
            }
            else
            {
                LabelUpdate(Status, "背景提取错误！");
                ProgressBarUpdate(pBStatus, 0);
                return CShareData.ReturnValueCalibHrz.FAILURE;
            }

            ProgressBarUpdate(pBStatus, 80);

            if (CShareData.DEBUG)
            {
                string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                string path = CShareData.StartPath + "水平标定" + TimeNow + ".txt";

                string str = "";
                for (int i = 0; i < MeasureTimeMax; i++)
                {
                    str += OnePointMeasureDistance[i].ToString() + "\t";
                }
                str += "\r\n\r\n";
                for (int i = 0; i < MeasureTimeMax; i++)
                {
                    str += DistancePersonProb[i].ToString() + "\t";
                }
                WriteStrToTXT(path, str);
            }



            //MoveZero:
            if (ZeroPositionLRFlag)
            {
                Int16 MoveStep = Convert.ToInt16(0.5 * (ZeroPositionLeft + ZeroPositionRight + ProbToStep) - 1);
                CShareData.CalibHrzZeroMove = MoveStep;

                ReturnZeroFunc(serialPort1, Status, null);  //回零
                if (MoveStep < 0)
                {
                    MoveFunc(CShareData.MoveDirection.Left, Convert.ToInt16(0 - MoveStep), CShareData.SetZeroValue.Set, serialPort1, Status, null);
                }
                else
                {
                    MoveFunc(CShareData.MoveDirection.Right, MoveStep, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                }
                ProgressBarUpdate(pBStatus, 100);
                return CShareData.ReturnValueCalibHrz.SUCCESS;
            }
            else
            {
                LabelUpdate(Status, "未找到左右边界");
                ProgressBarUpdate(pBStatus, 0);
                return CShareData.ReturnValueCalibHrz.ERROR_LR_EDGE;
            }





















            //ProgressBarUpdate(pBStatus, 0);
            //if (MoveFunc(CShareData.MoveDirection.Right, 27, CShareData.SetZeroValue.Set, serialPort1, Status, null))
            //{
            //    ProgressBarUpdate(pBStatus,100);
            //    return true;
            //}
            //else
            //{
            //    ProgressBarUpdate(pBStatus, 0);
            //    return false;
            //}


        }
        #endregion


        #region 水平标定
        public bool HrzCalibrationAutoFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);

            Int16 DistanceSubMin = 500;    //两点的距离差大于这个值时，视为障碍物            
            Int16 ScanTimeMax = 10;         //扫描次数
            Int16 ScanPointNum = 109;       //扫描点数
            Int16 ScanZeroMove = 50;        //每次扫描零点移动的步数            
            Int16 ZeroPosition = 0;         //找出的零点位置
            Int16 MeasureNum = 200;          //定点测量的次数
            Int16 MeasureTimeMax = 60;

            //Int32 MeasureIdealPos = 40000;     //理想情况下的测量点位置
            //计算首次移动的步数
            //Int16 MoveDownStep = Convert.ToInt16(Math.Atan(1.0 * CShareData.InstallHeight / MeasureIdealPos) * 180 / Math.PI / AngleStep);
            Int16 MoveDownStep = 0;        //扫描前向下调整的步数（安装高度5m，距离40m）

            ReturnZeroFunc(serialPort1, Status, null);  //后面要扫描，因此最好先回零，防止因为电机转动时间长，串口等待时间超时

            //水平扫描
            Int32[,] ReceivedDistance = new Int32[2, ScanPointNum];     //读取水平扫描距离值
            Int16 XStartPos = Convert.ToInt16(0 - ((ScanPointNum - 1) >> 2));
            Int16 XEndPos = Convert.ToInt16((ScanPointNum - 1) >> 2);
            Int16 ZeroShift = 0;
            bool MoveLeftFlag = true;       //默认向左移动后扫描
            bool MoveRightFlag = false;     //向右移动后扫描
            bool ZeroInFrameFlag = false;   //零点是否在该扫描帧中
            Int16 ZeroPositionPos = 0;  //正向数据中的零点位置
            Int16 ZeroPositionNeg = 0;  //反向数据中的零点位置
            for (int i = 0; i < ScanTimeMax; i++)
            {
                if (i > 1)    //第1次不执行
                {
                    if (MoveLeftFlag)   //向左移动的概率较大
                    {
                        ZeroShift = Convert.ToInt16(0 - ScanZeroMove);
                    }
                    else if (MoveRightFlag)
                    {
                        ZeroShift = ScanZeroMove;
                    }
                    else
                    {
                        ZeroShift = Convert.ToInt16(0 - ScanZeroMove);
                    }
                    XStartPos += ZeroShift;
                    XEndPos += ZeroShift;
                }
                ReceivedDistance = HrzScanFunc(Convert.ToInt16(0 - MoveDownStep), XStartPos, XEndPos, serialPort1, Status, null);  //调用水平扫描函数                
                //滤掉无效数据4
                ReceivedDistance = ReplaceInvalidData(ReceivedDistance, ScanPointNum);

                if ((ReceivedDistance[0, 0] < CShareData.BOXMAXDISTANCE) && (ReceivedDistance[0, 1] < CShareData.BOXMAXDISTANCE) && (ReceivedDistance[0, ScanPointNum - 1] > CShareData.BOXMAXDISTANCE))   //左边界
                {
                    MoveLeftFlag = false;
                    MoveRightFlag = true;
                    continue;
                }
                if ((ReceivedDistance[0, ScanPointNum - 1] < CShareData.BOXMAXDISTANCE) && (ReceivedDistance[0, ScanPointNum - 2] < CShareData.BOXMAXDISTANCE) && (ReceivedDistance[0, 0] > CShareData.BOXMAXDISTANCE))   //右边界
                {
                    MoveLeftFlag = true;
                    MoveRightFlag = false;
                    continue;
                }

                for (int m = 1; m < ScanPointNum - 1; m++)  //不包含边界
                {
                    //找到一个有价值的点，一般而言，人身上的点肯定是有效值，地面上的点有可能是“4”，调用ReplaceInvalidData后，用附近的值替换“4”
                    if ((ReceivedDistance[0, m] - ReceivedDistance[0, m + 1]) > DistanceSubMin)    //找到一个有价值的点，人身上的点距离近，地面上的距离远
                    {
                        if (Math.Abs(ReceivedDistance[0, m] - ReceivedDistance[0, m - 1]) < DistanceSubMin)    //保证不是畸变点
                        {
                            ZeroInFrameFlag = true;
                            ZeroPositionPos = Convert.ToInt16(XStartPos + Math.Round(0.5 * m));


                            for (int n = 1; n < ScanPointNum - 1; n++)  //判断反向数据
                            {
                                if ((ReceivedDistance[1, n] - ReceivedDistance[1, n + 1]) > DistanceSubMin)    //找到一个有价值的点
                                {
                                    if (Math.Abs(ReceivedDistance[1, n] - ReceivedDistance[1, n - 1]) < DistanceSubMin)    //保证不是畸变点
                                    {
                                        ZeroInFrameFlag = true;
                                        ZeroPositionNeg = Convert.ToInt16(XStartPos + Math.Round(0.5 * n));
                                        break;
                                    }
                                }
                            }
                            if (ZeroPositionNeg == 0)   //防止平均后，数值很离谱
                            {
                                ZeroPositionNeg = ZeroPositionPos;
                            }

                            ZeroPosition = Convert.ToInt16(0.5 * (ZeroPositionPos + ZeroPositionNeg));  //此位置偏向于标记物最左边
                            goto FindZeroInFrame;   //此外用break无法完全跳出
                        }
                    }
                }
            }
            ProgressBarUpdate(pBStatus, 50);

            Int16 ZeroPositionLeft = 0;    //标记物的左边位置
            Int16 ZeroPositionRight = 0;      //标记物的右边位置
            bool ZeroPositionLeftFlag = false;
            bool ZeroPositionRightFlag = false;
            bool ZeroPositionLRFlag = false;
            double DistanceLast = 0;
            double DistanceNow = 0;
        FindZeroInFrame:
            if (ZeroInFrameFlag)    //扫描到标记物，定点测距准确定位
            {
                ZeroPositionLeft = ZeroPosition;
                ZeroPositionRight = ZeroPosition;

                ZeroPosition -= 5;//保证能全部包含障碍物
                ReturnZeroFunc(serialPort1, Status, null);  //回零
                MoveFunc(CShareData.MoveDirection.Down, MoveDownStep, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                if (ZeroPosition < 0)  //移动到障碍物最左端
                {
                    MoveFunc(CShareData.MoveDirection.Left, Convert.ToInt16(0 - ZeroPosition), CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                }
                else
                {
                    MoveFunc(CShareData.MoveDirection.Right, ZeroPosition, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                }
                ZeroPositionLeftFlag = false;
                ZeroPositionRightFlag = false;
                ZeroPositionLRFlag = false;
                DistanceNow = OnePointMeasureFunc(MeasureNum, serialPort1, Status, null);  //获取初始值
                for (int i = 0; i < MeasureTimeMax; i++)
                {
                    DistanceLast = DistanceNow;
                    DistanceNow = OnePointMeasureFunc(MeasureNum, serialPort1, Status, null);
                    if (CShareData.OnePointDistanceValidProb > 0.9)
                    {
                        if ((DistanceLast - DistanceNow) > DistanceSubMin)  //左边界判断
                        {
                            if (ZeroPositionLeftFlag == false)
                            {
                                ZeroPositionLeftFlag = true;
                                ZeroPositionLeft = ZeroPosition;    //得到左侧的边界位置
                            }
                        }
                        if ((DistanceNow - DistanceLast) > DistanceSubMin)  //右边界判断
                        {
                            if (ZeroPositionRightFlag == false)
                            {
                                ZeroPositionRightFlag = true;
                                ZeroPositionRight = ZeroPosition;    //得到右侧的边界位置
                            }
                        }
                        if (ZeroPositionLeftFlag && ZeroPositionRightFlag)  //两个边界均找到
                        {
                            ZeroPositionLRFlag = true;
                            goto MoveZero;  //移动到零点位置
                            //break;
                        }
                        else
                        {

                            if (MoveFunc(CShareData.MoveDirection.Right, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, pBStatus))
                            {
                                ZeroPosition += 1;  //右移1步
                            }
                            else
                            {
                                LabelUpdate(Status, "向右移动失败！");
                                ProgressBarUpdate(pBStatus, 0);
                                return false;
                            }
                            if (i == (MeasureTimeMax - 1))
                            {
                                LabelUpdate(Status, "移动到最大位置，仍然未找到边界！");
                                ProgressBarUpdate(pBStatus, 0);
                                return false;
                            }
                        }
                    }
                    else
                    {
                        LabelUpdate(Status, "无效距离值太多！");
                        ProgressBarUpdate(pBStatus, 0);
                        return false;
                    }
                }
            }
            else
            {
                LabelUpdate(Status, "未扫描到零点标志物！");
                ProgressBarUpdate(pBStatus, 0);
                return false;
            }

            ProgressBarUpdate(pBStatus, 80);
        MoveZero:
            if (ZeroPositionLRFlag)
            {
                Int16 MoveStep = Convert.ToInt16(0.5 * (ZeroPositionLeft + ZeroPositionRight));

                ReturnZeroFunc(serialPort1, Status, null);  //回零
                if (MoveStep < 0)
                {
                    MoveFunc(CShareData.MoveDirection.Left, Convert.ToInt16(0 - MoveStep), CShareData.SetZeroValue.Set, serialPort1, Status, null);
                }
                else
                {
                    MoveFunc(CShareData.MoveDirection.Right, MoveStep, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                }
                ProgressBarUpdate(pBStatus, 100);
                return true;
            }
            else
            {
                LabelUpdate(Status, "未找到左右边界");
                ProgressBarUpdate(pBStatus, 0);
                return false;
            }

        }
        #endregion

        #region 垂直标定
        public bool VerCalibrationAutoFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);

            int MeasureNum = 1000;
            double AngleStep = 2 * CShareData.XSTEPANGLE;
            //Int32 MeasureIdealPos = 40000;     //理想情况下的测量点位置
            //计算首次移动的步数
            //Int16 MoveDownStep = Convert.ToInt16(Math.Atan(1.0 * CShareData.InstallHeight / MeasureIdealPos) * 180 / Math.PI / AngleStep);
            Int16 MoveDownStep = 22;    //安装高度5m，距离40m
            Int16 MoveTimeMax = 15;     //安装高度5m，距离75m

            ReturnZeroFunc(serialPort1, Status, null);  //回零
            MoveFunc(CShareData.MoveDirection.Down, MoveDownStep, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);

            double Distance = 0;        //测量距离
            double AngleSinValue = 0;   //正弦值
            double AngleMeasure = 0;    //角度值
            bool AngleValidFlag = false;
            for (int i = 0; i < MoveTimeMax; i++)
            {
                Distance = OnePointMeasureFunc(MeasureNum, serialPort1, Status, null);
                ProgressBarUpdate(pBStatus, Convert.ToInt32(1.0 * i / MoveTimeMax * 100));
                if (CShareData.OnePointDistanceValidProb >= 0.95)  //全为有效值
                {
                    AngleSinValue = CShareData.InstallHeight / (Distance - CShareData.DISTANCE_CE);
                    if ((AngleSinValue > 0) && (AngleSinValue < 1))
                    {
                        AngleMeasure = Math.Asin(AngleSinValue) * 180 / Math.PI;
                        AngleValidFlag = true;
                        break;
                    }
                    else  //AngleSinValue>1时，说明测量距离太近，测量点则落在箱子内
                    {
                        LabelUpdate(Status, "角度值不合法!");
                        AngleValidFlag = false;
                        MoveFunc(CShareData.MoveDirection.Up, 5, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                        //ProgressBarUpdate(pBStatus, 0);
                        //return false;
                    }
                }
                else
                {
                    MoveFunc(CShareData.MoveDirection.Down, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                }
            }

            ProgressBarUpdate(pBStatus, 80);

            Int16 MoveUpStep = 0;
            if (AngleValidFlag)
            {
                MoveUpStep = Convert.ToInt16(Math.Round(AngleMeasure / AngleStep));
                if ((MoveUpStep >= 10) && (MoveUpStep <= 100))
                {
                    if (ShiftRefreshFunc(serialPort1, Status, pBStatus))   //读电机位置，保证水平方向不变
                    {
                        Int16 MoveLRStep = Convert.ToInt16(Math.Abs(CShareData.XShift));
                        if (CShareData.XShift > 0)
                        {
                            MoveFunc(CShareData.MoveDirection.Left, MoveLRStep, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                        }
                        else
                        {
                            MoveFunc(CShareData.MoveDirection.Right, MoveLRStep, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                        }
                    }

                    MoveFunc(CShareData.MoveDirection.Up, MoveUpStep, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                    ProgressBarUpdate(pBStatus, 100);
                    return true;
                }
                else
                {
                    LabelUpdate(Status, "计算出的步数不合理！");
                    ProgressBarUpdate(pBStatus, 0);
                    return false;
                }
            }
            else
            {
                LabelUpdate(Status, "未找到合适的测量位置！");
                ProgressBarUpdate(pBStatus, 0);
                return false;
            }
        }

        #endregion

        #region 水平、垂直标定
        public int HrzVerCalibrationAutoFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            bool HrzCalibFlag = false;
            bool VerCalibFlag = false;
            int CalibrationResult = 0;
            if (VerCalibrationFunc(serialPort1, Status, pBStatus) == CShareData.ReturnValueCalibVer.SUCCESS)  //先垂直，粗略回零后再右下角，并且水平要扫描，需有一定的高度
            {
                VerCalibFlag = true;
            }
            if (CShareData.ReturnValueCalibHrz.SUCCESS == HrzCalibrationFunc(serialPort1, Status, pBStatus))
            {
                HrzCalibFlag = true;
            }

            if (VerCalibFlag && HrzCalibFlag)
            {
                CalibrationResult = 1;
                ProgressBarUpdate(pBStatus, 100);
            }
            else if (VerCalibFlag)
            {
                CalibrationResult = 2;
                ProgressBarUpdate(pBStatus, 50);
            }
            else if (HrzCalibFlag)
            {
                CalibrationResult = 3;
                ProgressBarUpdate(pBStatus, 50);
            }
            else
            {
                CalibrationResult = 4;
                ProgressBarUpdate(pBStatus, 0);
            }
            return CalibrationResult;
        }
        #endregion
        #endregion

        #region 故障检测
        #region IPC
        public bool IPCCheckFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            //--------------------------------X----子类--Len_H-Len_L-------Object------Check 
            byte[] IPCCommand = new byte[] { 0x58, 0x02, 0x00, 0x03, 0x00, 0x00, 0x00, 0xCA };
            IPCCommand[4] = Convert.ToByte('I');
            IPCCommand[5] = Convert.ToByte('P');
            IPCCommand[6] = Convert.ToByte('C');
            CShareData.spReceivedDataLenIdeal = 7;     //receive 7 bytes -- 58 02 00 02 45 52 0D


            DataWrite(serialPort1, IPCCommand);     //send command
            if (CShareData.spSem.WaitOne(CShareData.spSemTimeout) == false)    //wait sem
            {
                LabelUpdate(Status, "串口接收等待超时！");
                ProgressBarUpdate(pBStatus, 0);
                return false;
            }
            if (CShareData.spErrorFlag)
            {
                LabelUpdate(Status, "串口错误！");
                ProgressBarUpdate(pBStatus, 0);
                return false;
            }

            if (CShareData.spReceivedDataLen == CShareData.spReceivedDataLenIdeal)  //长度
            {
                if (JudegCheckSum(CShareData.spReceivedData, CShareData.spReceivedDataLen)) //校验和
                {
                    if ((CShareData.spReceivedData[0] == 0x58) && (CShareData.spReceivedData[1] == 0x02))
                    {
                        if ((CShareData.spReceivedData[4] == 0x4F) && (CShareData.spReceivedData[5] == 0x4B))
                        {
                            Status.Text = "IPC通信正常！";
                            ProgressBarUpdate(pBStatus, 100);
                            return true;
                        }
                        else
                        {
                            Status.Text = "IPC通信错误！";
                            ProgressBarUpdate(pBStatus, 0);
                            return false;
                        }
                    }
                    else
                    {
                        ErrorCheck(Status);  //检查错误
                        ProgressBarUpdate(pBStatus, 0);
                        return false;
                    }
                }
                else
                {
                    LabelUpdate(Status, "校验证和错误！");
                    ProgressBarUpdate(pBStatus, 0);
                    return false;
                }

            }
            else
            {
                ErrorCheck(Status);  //检查错误
                LabelUpdate(Status, "仅接收到" + CShareData.spReceivedDataLen.ToString() + "个数据！");
                ProgressBarUpdate(pBStatus, 0);
                return false;
            }
        }
        #endregion

        #region LDM
        public bool LDMCheckFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            //--------------------------------X----子类--Len_H-Len_L-------Object------Check 
            byte[] LDMCommand = new byte[] { 0x58, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0xCA };
            LDMCommand[4] = Convert.ToByte('L');
            LDMCommand[5] = Convert.ToByte('D');
            LDMCommand[6] = Convert.ToByte('M');
            CShareData.spReceivedDataLenIdeal = 7;     //receive 7 bytes -- 58 03 00 02 45 52 0D


            DataWrite(serialPort1, LDMCommand);     //send command
            if (CShareData.spSem.WaitOne(CShareData.spSemTimeout) == false)    //wait sem
            {
                LabelUpdate(Status, "串口接收等待超时！");
                ProgressBarUpdate(pBStatus, 0);
                return false;
            }
            if (CShareData.spErrorFlag)
            {
                LabelUpdate(Status, "串口错误！");
                ProgressBarUpdate(pBStatus, 0);
                return false;
            }

            if (CShareData.spReceivedDataLen == CShareData.spReceivedDataLenIdeal)  //长度
            {
                if (JudegCheckSum(CShareData.spReceivedData, CShareData.spReceivedDataLen)) //校验和
                {
                    if ((CShareData.spReceivedData[0] == 0x58) && (CShareData.spReceivedData[1] == 0x03))
                    {
                        if ((CShareData.spReceivedData[4] == 0x4F) && (CShareData.spReceivedData[5] == 0x4B))
                        {
                            Status.Text = "LDM通信正常！";
                            ProgressBarUpdate(pBStatus, 100);
                            return true;
                        }
                        else
                        {
                            Status.Text = "LDM通信错误！";
                            ProgressBarUpdate(pBStatus, 0);
                            return false;
                        }
                    }
                    else
                    {
                        ErrorCheck(Status);  //检查错误
                        ProgressBarUpdate(pBStatus, 0);
                        return false;
                    }
                }
                else
                {
                    LabelUpdate(Status, "校验证和错误！");
                    ProgressBarUpdate(pBStatus, 0);
                    return false;
                }

            }
            else
            {
                ErrorCheck(Status);  //检查错误
                LabelUpdate(Status, "仅接收到" + CShareData.spReceivedDataLen.ToString() + "个数据！");
                ProgressBarUpdate(pBStatus, 0);
                return false;
            }
        }
        #endregion

        #region FPGA
        public bool FPGACheckFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            //--------------------------------X----子类--Len_H-Len_L----------Object---------Check 
            byte[] FPGACommand = new byte[] { 0x58, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xCA };
            FPGACommand[4] = Convert.ToByte('F');
            FPGACommand[5] = Convert.ToByte('P');
            FPGACommand[6] = Convert.ToByte('G');
            FPGACommand[7] = Convert.ToByte('A');
            CShareData.spReceivedDataLenIdeal = 7;     //receive 7 bytes -- 58 03 00 02 45 52 0D

            DataWrite(serialPort1, FPGACommand);     //send command
            if (CShareData.spSem.WaitOne(CShareData.spSemTimeout) == false)    //wait sem
            {
                LabelUpdate(Status, "串口接收等待超时！");
                ProgressBarUpdate(pBStatus, 0);
                return false;
            }
            if (CShareData.spErrorFlag)
            {
                LabelUpdate(Status, "串口错误！");
                ProgressBarUpdate(pBStatus, 0);
                return false;
            }

            if (CShareData.spReceivedDataLen == CShareData.spReceivedDataLenIdeal)  //长度
            {
                if (JudegCheckSum(CShareData.spReceivedData, CShareData.spReceivedDataLen)) //校验和
                {
                    if ((CShareData.spReceivedData[0] == 0x58) && (CShareData.spReceivedData[1] == 0x04))
                    {
                        if ((CShareData.spReceivedData[4] == 0x4F) && (CShareData.spReceivedData[5] == 0x4B))
                        {
                            Status.Text = "FPGA通信正常！";
                            ProgressBarUpdate(pBStatus, 100);
                            return true;
                        }
                        else
                        {
                            Status.Text = "FPGA通信错误！";
                            ProgressBarUpdate(pBStatus, 0);
                            return false;
                        }
                    }
                    else
                    {
                        ErrorCheck(Status);  //检查错误
                        ProgressBarUpdate(pBStatus, 0);
                        return false;
                    }
                }
                else
                {
                    LabelUpdate(Status, "校验证和错误！");
                    ProgressBarUpdate(pBStatus, 0);
                    return false;
                }

            }
            else
            {
                ErrorCheck(Status);  //检查错误
                LabelUpdate(Status, "仅接收到" + CShareData.spReceivedDataLen.ToString() + "个数据！");
                ProgressBarUpdate(pBStatus, 0);
                return false;
            }
        }
        #endregion

        #endregion

        #region 水平、垂直标定
        #region 水平标定
        public CShareData.ReturnValueCalibHrz HrzCalibrationFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {

            ProgressBarUpdate(pBStatus, 0);

            Int16 DistanceSubMin = 2000;    //两点的距离差大于这个值时，视为障碍物            
            //Int16 ScanTimeMax = 10;         //扫描次数
            Int16 ScanPointNum = CShareData.YSCANPOINTNUM;       //扫描点数
            //Int16 ScanZeroMove = 50;        //每次扫描零点移动的步数           

            Int16 MeasureNum = 200;          //定点测量的次数
            Int16 MeasureTimeMax = 60;

            //Int32 MeasureIdealPos = 40000;     //理想情况下的测量点位置
            //计算首次移动的步数
            //Int16 MoveDownStep = Convert.ToInt16(Math.Atan(1.0 * CShareData.InstallHeight / MeasureIdealPos) * 180 / Math.PI / AngleStep);
            Int16 MoveDownStep = (Int16)CShareData.YShift;        //扫描前向下调整的步数（安装高度5m，距离40m）


            //水平扫描
            Int32[,] ReceivedDistance = new Int32[2, ScanPointNum];     //读取水平扫描距离值
            Int16 XStartPos = Convert.ToInt16(0 - ((ScanPointNum - 1) >> 2));
            Int16 XEndPos = Convert.ToInt16((ScanPointNum - 1) >> 2);
            //Int16 ZeroShift = 0;
            //bool MoveLeftFlag = true;       //默认向左移动后扫描
            //bool MoveRightFlag = false;     //向右移动后扫描
            bool ZeroInFrameFlag = false;   //零点是否在该扫描帧中
            Int16 ZeroPositionPos = 0;  //正向数据中的零点位置
            Int16 ZeroPositionNeg = 0;  //反向数据中的零点位置
            Int16 ZeroPosition = 0;     //找出的零点平均位置

            ReceivedDistance = HrzScanFunc(MoveDownStep, XStartPos, XEndPos, serialPort1, Status, null);  //调用水平扫描函数                
            //滤掉无效数据4
            ReceivedDistance = ReplaceInvalidData(ReceivedDistance, ScanPointNum);

            for (int m = 1; m < ScanPointNum - 1; m++)  //不包含边界
            {
                //找到一个有价值的点，一般而言，人身上的点肯定是有效值，地面上的点有可能是“4”，调用ReplaceInvalidData后，用附近的值替换“4”
                if ((ReceivedDistance[0, m] - ReceivedDistance[0, m + 1]) > DistanceSubMin)    //找到一个有价值的点，人身上的点距离近，地面上的距离远
                {
                    if (Math.Abs(ReceivedDistance[0, m] - ReceivedDistance[0, m - 1]) < DistanceSubMin)    //保证不是畸变点
                    {
                        ZeroInFrameFlag = true;
                        ZeroPositionPos = Convert.ToInt16(XStartPos + Math.Round(0.5 * m));

                        for (int n = 1; n < ScanPointNum - 1; n++)  //判断反向数据
                        {
                            if ((ReceivedDistance[1, n] - ReceivedDistance[1, n + 1]) > DistanceSubMin)    //找到一个有价值的点
                            {
                                if (Math.Abs(ReceivedDistance[1, n] - ReceivedDistance[1, n - 1]) < DistanceSubMin)    //保证不是畸变点
                                {
                                    ZeroInFrameFlag = true;
                                    ZeroPositionNeg = Convert.ToInt16(XStartPos + Math.Round(0.5 * n));
                                    break;
                                }
                            }
                        }
                        if (ZeroPositionNeg == 0)   //防止平均后，数值很离谱
                        {
                            ZeroPositionNeg = ZeroPositionPos;
                        }

                        ZeroPosition = Convert.ToInt16(0.5 * (ZeroPositionPos + ZeroPositionNeg));  //此位置偏向于标记物最左边
                    }
                }
            }

            double DistanceBackground = 0;
            double DistancePerson = 0;
            //bool DistanceDiffFlag = false;
            if (ZeroInFrameFlag)
            {
                if ((ZeroPositionPos > XStartPos + 3) && (ZeroPositionPos < XStartPos + ScanPointNum - 3))
                {
                    int ZeroPositionArrayIndex = 2 * (ZeroPositionPos - XStartPos);   //下标从0开始
                    //int ZeroPositionArrayIndex = ZeroPositionPos - XStartPos;   //下标从0开始
                    DistanceBackground = (ReceivedDistance[0, ZeroPositionArrayIndex - 2] + ReceivedDistance[0, ZeroPositionArrayIndex - 3] + ReceivedDistance[0, ZeroPositionArrayIndex - 4]) / 3;
                    DistancePerson = (ReceivedDistance[0, ZeroPositionArrayIndex + 1] + ReceivedDistance[0, ZeroPositionArrayIndex + 2]) / 2;
                }
                else
                {
                    LabelUpdate(Status, "人的位置离边界太近！");
                    ProgressBarUpdate(pBStatus, 0);
                    return CShareData.ReturnValueCalibHrz.ERROR_POSITION;
                }
            }


            double[] OnePointMeasureDistance = new double[MeasureTimeMax];     //读取定点测距的距离值

            ProgressBarUpdate(pBStatus, 50);

            Int16 ZeroPositionLeft = 0;    //标记物的左边位置
            Int16 ZeroPositionRight = 0;      //标记物的右边位置
            bool ZeroPositionLeftFlag = false;
            bool ZeroPositionRightFlag = false;
            bool ZeroPositionLRFlag = false;
            double DistanceLast = 0;
            double DistanceNow = 0;
            if (ZeroInFrameFlag)    //扫描到标记物，定点测距准确定位
            {
                ZeroPositionLeft = ZeroPosition;
                ZeroPositionRight = ZeroPosition;

                ZeroPosition -= 6;//保证能全部包含障碍物
                ReturnZeroFunc(serialPort1, Status, null);  //回零
                MoveFunc(CShareData.MoveDirection.Down, MoveDownStep, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                if (ZeroPosition < 0)  //移动到障碍物最左端
                {
                    MoveFunc(CShareData.MoveDirection.Left, Convert.ToInt16(0 - ZeroPosition), CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                }
                else
                {
                    MoveFunc(CShareData.MoveDirection.Right, ZeroPosition, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                }
                ZeroPositionLeftFlag = false;
                ZeroPositionRightFlag = false;
                ZeroPositionLRFlag = false;

                DistanceNow = OutOnePointMeasureFunc(MeasureNum, serialPort1, Status, null);  //获取初始值
                if (CShareData.OutOnePointDistanceValidProb < 0.1) //说明一定是落在在面上，指定一个较远距离
                {
                    DistanceNow = DistanceBackground;   //落在地上时，4太多时，指定为背景
                }
                OnePointMeasureDistance[0] = DistanceNow;
                for (int i = 1; i < MeasureTimeMax; i++)
                {

                    if (i == (MeasureTimeMax - 1))
                    {
                        LabelUpdate(Status, "移动到最大位置，仍然未找到边界！");
                        ProgressBarUpdate(pBStatus, 0);
                        return CShareData.ReturnValueCalibHrz.ERROR_MEASURE_TIMEOUT;
                    }

                    DistanceLast = DistanceNow;
                    DistanceNow = OutOnePointMeasureFunc(MeasureNum, serialPort1, Status, null);
                    if (CShareData.OutOnePointDistanceValidProb < 0.1) //说明一定是落在在面上，只保留上次数据就好
                    {
                        DistanceNow = DistanceBackground;   //为防止数据有效率低时，永远找不到右边界
                        //if (ZeroPositionLeftFlag == false)
                        //{
                        //    DistanceNow = DistanceLast;
                        //}
                        //else
                        //{
                        //    DistanceNow = DistanceBackground;   //为防止数据有效率低时，永远找不到右边界
                        //}
                    }
                    OnePointMeasureDistance[i] = DistanceNow;
                    //if (CShareData.OutOnePointDistanceValidProb > 0.1)
                    {
                        if ((DistanceLast - DistanceNow) > DistanceSubMin)  //左边界判断
                        {
                            if (ZeroPositionLeftFlag == false)
                            {
                                ZeroPositionLeftFlag = true;
                                ZeroPositionLeft = ZeroPosition;    //得到左侧的边界位置
                            }
                        }
                        if ((DistanceNow - DistanceLast) > DistanceSubMin)  //右边界判断
                        {
                            if (ZeroPositionRightFlag == false)
                            {
                                ZeroPositionRightFlag = true;
                                ZeroPositionRight = ZeroPosition;    //得到右侧的边界位置
                            }
                        }
                        if (ZeroPositionLeftFlag && ZeroPositionRightFlag)  //两个边界均找到
                        {
                            ZeroPositionLRFlag = true;
                            //goto MoveZero;  //移动到零点位置
                            break;
                        }
                        else
                        {

                            if (MoveFunc(CShareData.MoveDirection.Right, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, pBStatus))
                            {
                                ZeroPosition += 1;  //右移1步
                            }
                            else
                            {
                                LabelUpdate(Status, "向右移动失败！");
                                ProgressBarUpdate(pBStatus, 0);
                                return CShareData.ReturnValueCalibHrz.FAILURE;
                            }
                        }
                    }
                    //else
                    //{
                    //    LabelUpdate(Status, "无效距离值太多！");
                    //    ProgressBarUpdate(pBStatus, 0);
                    //    return false;
                    //}
                }
            }
            else
            {
                LabelUpdate(Status, "垂直步数不合适！");
                ProgressBarUpdate(pBStatus, 0);
                return CShareData.ReturnValueCalibHrz.ERROR_YSTEP;
            }

            ProgressBarUpdate(pBStatus, 80);


            if (CShareData.DEBUG)
            {
                string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                string path = CShareData.StartPath + "水平标定测距" + TimeNow + ".txt";

                string str = "水平标定定点测距：\r\n";
                str += "背景距离：" + DistanceBackground.ToString() + "\r\n";
                for (int i = 0; i < MeasureTimeMax; i++)
                {
                    str += OnePointMeasureDistance[i].ToString() + "\t";
                }
                WriteStrToTXT(path, str);
            }


            //MoveZero:
            if (ZeroPositionLRFlag)
            {
                Int16 MoveStep = Convert.ToInt16(0.5 * (ZeroPositionLeft + ZeroPositionRight));
                CShareData.CalibHrzZeroMove = MoveStep;
                ReturnZeroFunc(serialPort1, Status, null);  //回零
                if (MoveStep < 0)
                {
                    MoveFunc(CShareData.MoveDirection.Left, Convert.ToInt16(0 - MoveStep), CShareData.SetZeroValue.Set, serialPort1, Status, null);
                }
                else
                {
                    MoveFunc(CShareData.MoveDirection.Right, MoveStep, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                }
                ProgressBarUpdate(pBStatus, 100);
                return CShareData.ReturnValueCalibHrz.SUCCESS;
            }
            else
            {
                LabelUpdate(Status, "未找到左右边界");
                ProgressBarUpdate(pBStatus, 0);
                return CShareData.ReturnValueCalibHrz.ERROR_LR_EDGE;
            }

            //ProgressBarUpdate(pBStatus, 0);
            //if (MoveFunc(CShareData.MoveDirection.Right, 27, CShareData.SetZeroValue.Set, serialPort1, Status, null))
            //{
            //    ProgressBarUpdate(pBStatus,100);
            //    return true;
            //}
            //else
            //{
            //    ProgressBarUpdate(pBStatus, 0);
            //    return false;
            //}
        }
        #endregion

        #region 垂直标定
        public CShareData.ReturnValueCalibVer VerCalibrationFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);

            int MeasureNum = 1000;
            double AngleStep = 2 * CShareData.YSTEPANGLE;

            double Distance = 0;        //测量距离
            double AngleSinValue = 0;   //正弦值
            double AngleMeasure = 0;    //角度值
            bool AngleValidFlag = false;

            //double Result = OrgOnePointMeasureFunc(MeasureNum, serialPort1, Status, null);
            //if (Result != 0)
            //{
            //    LabelUpdate(Status, "测距错误!");
            //    return CShareData.ReturnValueCalibVer.ERROR_MEASURE;
            //}
            ////去掉距离中，距离较小的值
            //Int32[] DistanceFilter = new Int32[CShareData.OrgOnePointDistanceLen];
            //int DistanceFilterLen = 0;
            //for (int i = 0; i < CShareData.OrgOnePointDistanceLen; i++)
            //{
            //    if (CShareData.OrgOnePointDistance[i] > CShareData.BOXMAXDISTANCE)
            //        DistanceFilter[DistanceFilterLen++] = CShareData.OrgOnePointDistance[i];
            //}
            //double DistanceValidProb = 1.0 * DistanceFilterLen / CShareData.OrgOnePointDistanceLen;
            //Distance = DistanceFilter.Sum() / DistanceFilterLen; 
            Distance = OutOnePointMeasureFunc(MeasureNum, serialPort1, Status, null);

            if (CShareData.OutOnePointDistanceValidProb >= 0.95)  //全为有效值
            {
                CShareData.CalibMeasureDistance = Distance - CShareData.DISTANCE_CE;
                CShareData.CalibHrzDistance = Math.Sqrt(Math.Pow(CShareData.CalibMeasureDistance, 2) - Math.Pow(CShareData.InstallHeight, 2));
                AngleSinValue = CShareData.InstallHeight / CShareData.CalibMeasureDistance;
                if ((AngleSinValue > 0) && (AngleSinValue < 1))
                {
                    AngleMeasure = Math.Asin(AngleSinValue) * 180 / Math.PI;
                    AngleValidFlag = true;
                }
                else  //AngleSinValue>1时，说明测量距离太近，测量点则落在箱子内
                {
                    LabelUpdate(Status, "角度值不合法!");
                    return CShareData.ReturnValueCalibVer.ERROR_ANGLE;
                }
            }
            else
            {
                LabelUpdate(Status, "该测量点不合适--有效值：" + CShareData.OnePointDistanceValidProb.ToString());
                return CShareData.ReturnValueCalibVer.ERROR_VALID_PROB;
            }


            Int16 MoveUpStep = 0;
            if (AngleValidFlag)
            {
                MoveUpStep = Convert.ToInt16(Math.Round(AngleMeasure / AngleStep));
                if ((MoveUpStep >= 10) && (MoveUpStep <= 400))
                {
                    if (ShiftRefreshFunc(serialPort1, Status, pBStatus))   //读电机位置，保证水平方向不变
                    {
                        Int16 MoveLRStep = Convert.ToInt16(Math.Abs(CShareData.XShift));
                        if (CShareData.XShift > 0)
                        {
                            MoveFunc(CShareData.MoveDirection.Left, MoveLRStep, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                        }
                        else
                        {
                            MoveFunc(CShareData.MoveDirection.Right, MoveLRStep, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                        }
                    }

                    MoveFunc(CShareData.MoveDirection.Up, MoveUpStep, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                    ProgressBarUpdate(pBStatus, 100);

                    return CShareData.ReturnValueCalibVer.SUCCESS;
                }
                else
                {
                    LabelUpdate(Status, "计算出的步数不合理！");
                    ProgressBarUpdate(pBStatus, 0);
                    return CShareData.ReturnValueCalibVer.ERROR_STEP;
                }
            }
            else
            {
                LabelUpdate(Status, "未找到合适的测量位置！");
                ProgressBarUpdate(pBStatus, 0);
                return CShareData.ReturnValueCalibVer.FAILURE;
            }
        }

        #endregion

        #region 水平、垂直标定
        public int HrzVerCalibrationFunc(SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            bool HrzCalibFlag = false;
            bool VerCalibFlag = false;
            int CalibrationResult = 0;
            if (VerCalibrationFunc(serialPort1, Status, pBStatus) == CShareData.ReturnValueCalibVer.SUCCESS)  //先垂直，粗略回零后再右下角，并且水平要扫描，需有一定的高度
            {
                VerCalibFlag = true;
            }
            if (CShareData.ReturnValueCalibHrz.SUCCESS == HrzCalibrationFunc(serialPort1, Status, pBStatus))
            {
                HrzCalibFlag = true;
            }

            if (VerCalibFlag && HrzCalibFlag)
            {
                CalibrationResult = 1;
                ProgressBarUpdate(pBStatus, 100);
            }
            else if (VerCalibFlag)
            {
                CalibrationResult = 2;
                ProgressBarUpdate(pBStatus, 50);
            }
            else if (HrzCalibFlag)
            {
                CalibrationResult = 3;
                ProgressBarUpdate(pBStatus, 50);
            }
            else
            {
                CalibrationResult = 4;
                ProgressBarUpdate(pBStatus, 0);
            }
            return CalibrationResult;
        }
        #endregion
        #endregion

        #region Z06型扫描、增加信号强度
        public Int32[,] Z06ScanFunc(CShareData.ZScanDirection Dir, Int16 YAdjust, Int16 YPos, Int16 XStartPos, Int16 XEndPos, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ProgressBarUpdate(pBStatus, 0);
            CShareData.Z06ScanFlag = false;
            //-----------------------------------Z----子类--Len_H-Len_L--Y_H---Y_L---YStartPos--XStartPos----XEndPos---Delay---MF----SA---Check 
            byte[] Z06ScanCommand = new byte[] { 0x5A, 0x06, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xE6, 0x00, 0x1A, 0x00, 0x03, 0x03, 0xCA };
            Int16 ScanTime = 2;      //the Scan time (default:2)
            Int16 PointCnt = Convert.ToInt16(((XEndPos - XStartPos) << 1) + 1);  //the number of distance,default:109 
            if (CShareData.ZScanDirection.Up == Dir)
            {
                Z06ScanCommand[4] = Convert.ToByte((YAdjust >> 8) & 0x00FF);
                Z06ScanCommand[5] = Convert.ToByte((YAdjust >> 0) & 0x00FF);
            }
            else
            {
                Z06ScanCommand[4] = Convert.ToByte(((0 - YAdjust) >> 8) & 0x00FF);
                Z06ScanCommand[5] = Convert.ToByte(((0 - YAdjust) >> 0) & 0x00FF);
            }
            Z06ScanCommand[6] = Convert.ToByte((YPos >> 8) & 0x00FF);
            Z06ScanCommand[7] = Convert.ToByte((YPos >> 0) & 0x00FF);
            Z06ScanCommand[8] = Convert.ToByte((XStartPos >> 8) & 0x00FF);
            Z06ScanCommand[9] = Convert.ToByte((XStartPos >> 0) & 0x00FF);
            Z06ScanCommand[10] = Convert.ToByte((XEndPos >> 8) & 0x00FF);
            Z06ScanCommand[11] = Convert.ToByte((XEndPos >> 0) & 0x00FF);

            Int32[,] Z06ScanDistance = new Int32[2 * ScanTime, PointCnt];  //读取的距离值
            CShareData.spReceivedDataLenIdeal = (PointCnt * 3) + 16;     //every scan you should receive 226 bytes -- 0x5A 0x01 Len_H Len_L 0x00 0x00 0x00 ---- 2*N-data CheckSum
            DataWrite(serialPort1, Z06ScanCommand);   //send command

            for (int i = 0; i < ScanTime; i++)
            {
                Int32 WaitTime = CShareData.spSemTimeout + Math.Abs(CShareData.XShift) + Math.Abs(CShareData.YShift);
                if (CShareData.spSem.WaitOne(WaitTime) == false)    //wait sem
                {
                    string str = "--" + CShareData.spReceivedDataLen.ToString() + "--" + CShareData.ReadDataLen.ToString() + "--" + CShareData.spReceivedDataLenIdeal.ToString();
                    LabelUpdate(Status, "串口接收等待超时！" + str);
                    goto Error;
                }

                if (CShareData.spErrorFlag)
                {
                    LabelUpdate(Status, "串口错误！");
                    goto Error;
                }

                if (CShareData.spReceivedDataLen == CShareData.spReceivedDataLenIdeal)  //长度
                {
                    if (JudegCheckSum(CShareData.spReceivedData, CShareData.spReceivedDataLen)) //校验和
                    {
                        if ((CShareData.spReceivedData[0] == 0x5A) && (CShareData.spReceivedData[1] == 0x06))
                        {
                            for (int j = 0; j < PointCnt; j++)
                            {
                                //find the value of every measure,and change to real distance(mm)
                                if (CShareData.spReceivedData[6] == 0x01)    //若是反向扫描，纠正数据排列顺序
                                {
                                    Z06ScanDistance[i, PointCnt - 1 - j] = 4 * (CShareData.spReceivedData[3 * j + 15] * 256 + CShareData.spReceivedData[3 * j + 16]);
                                    Z06ScanDistance[i + ScanTime, PointCnt - 1 - j] = CShareData.spReceivedData[3 * j + 17];
                                }
                                else
                                {
                                    Z06ScanDistance[i, j] = 4 * (CShareData.spReceivedData[3 * j + 15] * 256 + CShareData.spReceivedData[3 * j + 16]);
                                    Z06ScanDistance[i + ScanTime, j] = CShareData.spReceivedData[3 * j + 17];
                                }
                            }

                            byte FrameNum = CShareData.spReceivedData[4];
                            if (FrameNum != i)
                            {
                                LabelUpdate(Status, "扫描帧号乱序!");
                                goto Error;
                            }

                            if ((CShareData.spReceivedData[5] == 0x00) && (i != ScanTime - 1))  //如果读取一部分就没有后续帧，直接停止读数据
                            {
                                LabelUpdate(Status, "扫描数据未读完!");
                                goto Error;
                            }
                        }
                        else
                        {
                            ErrorCheck(Status);  //检查错误
                            goto Error;
                        }
                    }
                    else
                    {
                        LabelUpdate(Status, "校验证和错误！");
                        goto Error;
                    }

                }
                else
                {
                    ErrorCheck(Status);  //检查错误                    
                    goto Error;
                }
            }

            //LabelUpdate(Status,"扫描成功!");
            CShareData.Z06ScanFlag = true; //无错误goto,则扫描成功
            CShareData.XShift = XStartPos;
            ProgressBarUpdate(pBStatus, 100);
            return Z06ScanDistance;

        Error:
            CShareData.Z06ScanFlag = false;
            ProgressBarUpdate(pBStatus, 0);
            return Z06ScanDistance;
        }
        #endregion

        #region 整体功能测试
        public bool FunctionTestFunc(Int32 TestNum, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            Int16 ScanPointNum = 109;
            Int16 StartPos = Convert.ToInt16(0 - ((ScanPointNum - 1) >> 2));
            Int16 EndPos = Convert.ToInt16((ScanPointNum - 1) >> 2);
            for (int i = 0; i < TestNum; i++)
            {
                ProgressBarUpdate(pBStatus, Convert.ToInt32(100.0 / TestNum * i));

                CShareData.OperateRecordStr += "\r\n";
                LabelUpdate(Status, "定点测距……");
                OnePointMeasureFunc(1000, serialPort1, Status, null);

                CShareData.OperateRecordStr += "\r\n";
                LabelUpdate(Status, "垂直扫描……");
                VerScanFunc(0, EndPos, StartPos, serialPort1, Status, null);

                CShareData.OperateRecordStr += "\r\n";
                LabelUpdate(Status, "水平扫描……");
                HrzScanFunc(0, StartPos, EndPos, serialPort1, Status, null);

                CShareData.OperateRecordStr += "\r\n";
                LabelUpdate(Status, "Z型扫描……");
                Z01ScanFunc(CShareData.ZScanDirection.Down, 5, 0, StartPos, EndPos, serialPort1, Status, null);

                CShareData.OperateRecordStr += "\r\n";
                LabelUpdate(Status, "Z06扫描……");
                Z06ScanFunc(CShareData.ZScanDirection.Down, 0, 0, StartPos, EndPos, serialPort1, Status, null);



                CShareData.OperateRecordStr += "\r\n";
                LabelUpdate(Status, "移动……");
                MoveFunc(CShareData.MoveDirection.Up, 200, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                MoveFunc(CShareData.MoveDirection.Down, 200, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                MoveFunc(CShareData.MoveDirection.Left, 200, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                MoveFunc(CShareData.MoveDirection.Right, 200, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);

                CShareData.OperateRecordStr += "\r\n";
                LabelUpdate(Status, "回零……");
                ReturnZeroFunc(serialPort1, Status, null);

                CShareData.OperateRecordStr += "\r\n";
                LabelUpdate(Status, "置零……");
                SetZeroFunc(serialPort1, Status, null);

                CShareData.OperateRecordStr += "\r\n";
                LabelUpdate(Status, "刷新……");
                ShiftRefreshFunc(serialPort1, Status, null);

                CShareData.OperateRecordStr += "\r\n";
                LabelUpdate(Status, "红点激光……");
                RedLaser(CShareData.RedLaserValue.OFF, serialPort1, Status, null);
                RedLaser(CShareData.RedLaserValue.ON, serialPort1, Status, null);

                //CShareData.OperateRecordStr += "\r\n";
                //LabelUpdate(Status, "X粗略回零……");
                //XRoughReturnZeroFunc(0, serialPort1, Status, null);

                //CShareData.OperateRecordStr += "\r\n";
                //LabelUpdate(Status, "Y粗略回零……");
                //YRoughReturnZeroFunc(0, serialPort1, Status, null);

                SaveOperateRecord();    //保存操作记录
            }

            SaveOperateRecord();    //保存操作记录
            return true;
        }
        #endregion

        #region 水平扫描功能测试
        public bool TestHrzScanFunc(Int32 TestNum, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            string TestStr = "水平扫描功能测试:" + "\r\n" + "\r\n";
            Int32 ErrorTime = 0;

            Int16 ScanPointNum = 109;
            Int32[,] ReceivedDistance = new Int32[2, ScanPointNum];     //读取水平扫描距离值
            Int16 XStartPos = Convert.ToInt16(0 - ((ScanPointNum - 1) >> 2));
            Int16 XEndPos = Convert.ToInt16((ScanPointNum - 1) >> 2);

            for (int i = 0; i < TestNum; i++)
            {
                //MoveFunc(CShareData.MoveDirection.Down, 1, CShareData.SetZeroValue.UnSet, serialPort1, Status, null);
                //ReturnZeroFunc(serialPort1, Status, null);
                //SetZeroFunc(serialPort1, Status, null);
                //Thread.Sleep(100);

                ProgressBarUpdate(pBStatus, Convert.ToInt32(i * 100.0 / TestNum));
                ReceivedDistance = HrzScanFunc((Int16)CShareData.YShift, XStartPos, XEndPos, serialPort1, Status, null);   //调用水平扫描函数
                if (CShareData.HrzScanFlag)
                {
                    LabelUpdate(Status, "扫描成功!");

                    string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                    string path = CShareData.StartPath + "二维扫描" + TimeNow + ".txt";
                    //if (SaveScanDistance(path, ReceivedDistance, ScanPointNum, 2))
                    //{
                    //    TestStr += "第" + i.ToString() + "次测试：" + "\t" + "扫描成功!";
                    //    TestStr += "\r\n";
                    //}
                    //else
                    //{
                    //    TestStr += "第" + i.ToString() + "次测试：" + "\t" + "错误：保存扫描数据失败!";
                    //    TestStr += "\r\n";
                    //    ErrorTime++;
                    //    LabelUpdate(Status, "此次测试失败！");
                    //    TestStr += "此次测试失败！\r\n";
                    //}
                }
                else
                {
                    TestStr += "第" + i.ToString() + "次测试：" + "\t" + "错误：此次扫描失败!";
                    TestStr += "\r\n";
                    ErrorTime++;
                    LabelUpdate(Status, "此次测试失败！");
                    TestStr += "此次测试失败！\r\n";
                }
            }

            TestStr += "\r\n";
            TestStr += "\r\n";
            TestStr += "共测试" + TestNum.ToString() + "次";
            TestStr += "\r\n";
            TestStr += "失败" + ErrorTime.ToString() + "次" + "\t" + "失败率：" + (100.0 * ErrorTime / TestNum).ToString() + "%";

            string path1 = "水平扫描功能测试" + ".txt";
            WriteStrToTXT(path1, TestStr);
            ProgressBarUpdate(pBStatus, 100);
            LabelUpdate(Status, "失败" + ErrorTime.ToString() + "次" + "\t" + "失败率：" + (100.0 * ErrorTime / TestNum).ToString() + "%");

            SaveOperateRecord();    //保存操作记录
            return true;
        }
        #endregion

        #region Z扫描功能测试
        public bool TestZ01ScanFunc(Int32 TestNum, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            string TestStr = "Z扫描功能测试:" + "\r\n" + "\r\n";
            Int32 ErrorTime = 0;

            Int16 ScanPointNum = 109;
            Int32[,] ReceivedDistance = new Int32[2, ScanPointNum];     //读取水平扫描距离值
            Int16 XStartPos = Convert.ToInt16(0 - ((ScanPointNum - 1) >> 2));
            Int16 XEndPos = Convert.ToInt16((ScanPointNum - 1) >> 2);

            for (int i = 0; i < TestNum; i++)
            {

                ProgressBarUpdate(pBStatus, Convert.ToInt32(i * 100.0 / TestNum));
                ReceivedDistance = Z01ScanFunc(CShareData.ZScanDirection.Down, 10, 100, XStartPos, XEndPos, serialPort1, Status, null);   //调用水平扫描函数
                if (CShareData.Z01ScanFlag)
                {
                    LabelUpdate(Status, "扫描成功!");

                    string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                    string path = CShareData.StartPath + "Z01扫描" + TimeNow + ".txt";
                    if (SaveScanDistance(path, ReceivedDistance, ScanPointNum, 2))
                    {
                        TestStr += "第" + i.ToString() + "次测试：" + "\t" + "扫描成功!";
                        TestStr += "\r\n";
                    }
                    else
                    {
                        TestStr += "第" + i.ToString() + "次测试：" + "\t" + "错误：保存扫描数据失败!";
                        TestStr += "\r\n";
                        ErrorTime++;
                        LabelUpdate(Status, "此次测试失败！");
                        TestStr += "此次测试失败！\r\n";
                    }
                }
                else
                {
                    TestStr += "第" + i.ToString() + "次测试：" + "\t" + "错误：此次扫描失败!";
                    TestStr += "\r\n";
                    ErrorTime++;
                    LabelUpdate(Status, "此次测试失败！");
                    TestStr += "此次测试失败！\r\n";
                }
            }

            TestStr += "\r\n";
            TestStr += "\r\n";
            TestStr += "共测试" + TestNum.ToString() + "次";
            TestStr += "\r\n";
            TestStr += "失败" + ErrorTime.ToString() + "次" + "\t" + "失败率：" + (100.0 * ErrorTime / TestNum).ToString() + "%";

            string path1 = "Z扫描功能测试" + ".txt";
            WriteStrToTXT(path1, TestStr);
            ProgressBarUpdate(pBStatus, 100);
            LabelUpdate(Status, "失败" + ErrorTime.ToString() + "次" + "\t" + "失败率：" + (100.0 * ErrorTime / TestNum).ToString() + "%");

            SaveOperateRecord();    //保存操作记录
            return true;
        }
        #endregion

        #region 确认边框功能测试
        public bool TestConfirmEdgeFunc(Int32 TestNum, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            string path = "确认边框功能测试" + ".txt";
            string TestStr = "确认边框功能测试:" + "\r\n" + "\r\n";
            Int32 ErrorTime = 0;
            CShareData.ReturnValueEdge Result;
            for (int i = 0; i < TestNum; i++)
            {
                LabelUpdate(Status, "\r\n------------------第" + i.ToString() + "测试：");

                Result = XYConfirmEdgeBasicFunc(serialPort1, Status, null);
                switch (Result)
                {
                    case CShareData.ReturnValueEdge.SUCCESS:
                        TestStr += "第" + i.ToString() + "次测试：" + "\t" + "水平方向：" + CShareData.XEdgeStepSave.ToString() + "\t" + CShareData.XProbSave.ToString() + "\t" + "垂直方向：" + CShareData.YEdgeStepSave.ToString() + "\t" + CShareData.YProbSave.ToString();
                        TestStr += "\r\n";
                        break;
                    case CShareData.ReturnValueEdge.SUCCESS_HRZ:
                        TestStr += "第" + i.ToString() + "次测试：" + "\t" + "水平方向：" + CShareData.XEdgeStepSave.ToString() + "\t" + CShareData.XProbSave.ToString() + "\t" + "垂直方向：" + "ER" + "\t" + "ER";
                        TestStr += "\r\n";
                        ErrorTime++;
                        LabelUpdate(Status, "此次测试失败！");
                        TestStr += "此次测试失败！\r\n";
                        break;
                    case CShareData.ReturnValueEdge.SUCCESS_VER:
                        TestStr += "第" + i.ToString() + "次测试：" + "\t" + "水平方向：" + "ER" + "\t" + "ER" + "\t" + "垂直方向：" + CShareData.YEdgeStepSave.ToString() + "\t" + CShareData.YProbSave.ToString();
                        TestStr += "\r\n";
                        ErrorTime++;
                        LabelUpdate(Status, "此次测试失败！");
                        TestStr += "此次测试失败！\r\n";
                        break;
                    default:
                        TestStr += "第" + i.ToString() + "次测试：" + "\t" + "水平方向：" + "ER" + "\t" + "ER" + "\t" + "垂直方向：" + "ER" + "\t" + "ER";
                        TestStr += "\r\n";
                        ErrorTime++;
                        LabelUpdate(Status, "此次测试失败！");
                        TestStr += "此次测试失败！\r\n";
                        break;
                }
                ProgressBarUpdate(pBStatus, Convert.ToInt32(100.0 / TestNum * i));

                string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                LabelUpdate(Status, "第" + i.ToString() + "次测试：时间：" + TimeNow.ToString() + "\t" + "水平方向：" + CShareData.XEdgeStepSave.ToString() + "\t" + CShareData.XProbSave.ToString() + "\t" + "垂直方向：" + CShareData.YEdgeStepSave.ToString() + "\t" + CShareData.YProbSave.ToString());
                SaveOperateRecord();    //保存操作记录
                WriteStrToTXT(path, TestStr);
            }

            TestStr += "\r\n";
            TestStr += "\r\n";
            TestStr += "共测试" + TestNum.ToString() + "次";
            TestStr += "\r\n";
            TestStr += "失败" + ErrorTime.ToString() + "次" + "\t" + "失败率：" + (100.0 * ErrorTime / TestNum).ToString() + "%";


            WriteStrToTXT(path, TestStr);
            ProgressBarUpdate(pBStatus, 100);
            LabelUpdate(Status, "失败" + ErrorTime.ToString() + "次" + "\t" + "失败率：" + (100.0 * ErrorTime / TestNum).ToString() + "%");

            SaveOperateRecord();    //保存操作记录
            return true;
        }


        #region X确认边框功能测试
        public bool TestConfirmXEdgeFunc(Int32 TestNum, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            string TestStr = "X确认边框功能测试:" + "\r\n" + "\r\n";
            Int32 ErrorTime = 0;
            for (int i = 0; i < TestNum; i++)
            {
                if (CShareData.ReturnValueEdge.SUCCESS == XConfirmEdgeFunc(serialPort1, Status, null))
                {
                    TestStr += "第" + i.ToString() + "次测试：" + "\t" + "水平方向：" + CShareData.XEdgeStepSave.ToString() + "\t" + CShareData.XProbSave.ToString();
                    TestStr += "\r\n";
                }
                else
                {
                    TestStr += "第" + i.ToString() + "次测试：" + "\t" + "水平方向：" + "ER" + "\t" + "ER";
                    TestStr += "\r\n";
                    ErrorTime++;
                    LabelUpdate(Status, "此次测试失败！");
                    TestStr += "此次测试失败！\r\n";
                }
                ProgressBarUpdate(pBStatus, Convert.ToInt32(100.0 / TestNum * i));
            }

            TestStr += "\r\n";
            TestStr += "\r\n";
            TestStr += "共测试" + TestNum.ToString() + "次";
            TestStr += "\r\n";
            TestStr += "失败" + ErrorTime.ToString() + "次" + "\t" + "失败率：" + (100.0 * ErrorTime / TestNum).ToString() + "%";

            string path = "X确认边框功能测试" + ".txt";
            WriteStrToTXT(path, TestStr);
            ProgressBarUpdate(pBStatus, 100);
            LabelUpdate(Status, "失败" + ErrorTime.ToString() + "次" + "\t" + "失败率：" + (100.0 * ErrorTime / TestNum).ToString() + "%");

            SaveOperateRecord();    //保存操作记录
            return true;
        }
        #endregion

        #region Y确认边框功能测试
        public bool TestConfirmYEdgeFunc(Int32 TestNum, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            string TestStr = "Y确认边框功能测试:" + "\r\n" + "\r\n";
            Int32 ErrorTime = 0;
            for (int i = 0; i < TestNum; i++)
            {
                if (CShareData.ReturnValueEdge.SUCCESS == YConfirmEdgeFunc(serialPort1, Status, null))
                {
                    TestStr += "第" + i.ToString() + "次测试：" + "\t" + "垂直方向：" + CShareData.YEdgeStepSave.ToString() + "\t" + CShareData.YProbSave.ToString();
                    TestStr += "\r\n";
                }
                else
                {
                    TestStr += "第" + i.ToString() + "次测试：" + "\t" + "垂直方向：" + "ER" + "\t" + "ER";
                    TestStr += "\r\n";
                    ErrorTime++;
                    LabelUpdate(Status, "此次测试失败！");
                    TestStr += "此次测试失败！\r\n";
                }
                ProgressBarUpdate(pBStatus, Convert.ToInt32(100.0 / TestNum * i));
            }

            TestStr += "\r\n";
            TestStr += "\r\n";
            TestStr += "共测试" + TestNum.ToString() + "次";
            TestStr += "\r\n";
            TestStr += "失败" + ErrorTime.ToString() + "次" + "\t" + "失败率：" + (100.0 * ErrorTime / TestNum).ToString() + "%";

            string path = "Y确认边框功能测试" + ".txt";
            WriteStrToTXT(path, TestStr);
            ProgressBarUpdate(pBStatus, 100);
            LabelUpdate(Status, "失败" + ErrorTime.ToString() + "次" + "\t" + "失败率：" + (100.0 * ErrorTime / TestNum).ToString() + "%");

            SaveOperateRecord();    //保存操作记录
            return true;
        }
        #endregion

        #endregion

        #region 校准功能测试
        public bool TestAdjustFunc(Int32 TestNum, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            ReturnZeroFunc(serialPort1, Status, null);

            string path = "校准功能测试" + ".txt";
            string TestStr = "校准功能测试:" + "\r\n" + "\r\n";
            Int16 StepRange = Convert.ToInt16(Math.Round(Math.Sqrt(TestNum)));
            Int16 Step = 1;
            Int32 ErrorTime = 0;
            Int32 TestTime = 0;

            Int16 StepStart = Convert.ToInt16(-1 * StepRange / 2);
            Int16 StepEnd = Convert.ToInt16(StepRange / 2);
            Int16 XStep = 0;
            Int16 YStep = 0;
            for (XStep = StepStart; XStep < StepEnd; XStep += Step)
            {
                for (YStep = StepStart; YStep < StepEnd; YStep += Step)
                {
                    TestTime++;
                    MoveFunc(CShareData.MoveDirection.Right, XStep, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                    MoveFunc(CShareData.MoveDirection.Up, YStep, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                    LabelUpdate(Status, "\r\n-----第" + TestTime.ToString() + "次测量：重新设置零点-------" + "\t" + "水平位置：" + XStep.ToString() + "\t" + "垂直位置：" + YStep.ToString());

                    CShareData.ReturnValueAdjust Result = XYAdjustFunc(serialPort1, Status, null);

                    LabelUpdate(Status, "为得最后一次误差和概率，校准得到步数误差及概率，但不置位！");

                    if (Math.Abs(CShareData.XAdjustStepError) >= 1)
                    {
                        if (CShareData.ReturnValueAdjust.SUCCESS == XAdjustFunc(serialPort1, Status, null))
                        {
                            if (Math.Abs(CShareData.XAdjustStepError) > 1)
                            {
                                XAdjustFunc(serialPort1, Status, null);
                            }
                        }
                    }
                    else
                    {
                        LabelUpdate(Status, "水平误差为零，无需校准确认！");
                    }

                    if (Math.Abs(CShareData.YAdjustStepError) > 1)
                    {
                        if (CShareData.ReturnValueAdjust.SUCCESS == YAdjustFunc(serialPort1, Status, null))
                        {
                            if (Math.Abs(CShareData.YAdjustStepError) > 1)
                            {
                                YAdjustFunc(serialPort1, Status, null);
                            }
                        }
                    }
                    else
                    {
                        LabelUpdate(Status, "垂直小于1步，无需校准确认！");
                    }

                    ReturnZeroFunc(serialPort1, Status, null);  //必须要先回零，要不然，下次直接移动了

                    double Distance = OnePointMeasureFunc(1000, serialPort1, null, null);
                    switch (Result)
                    {
                        case CShareData.ReturnValueAdjust.SUCCESS:
                            TestStr += "第" + TestTime.ToString() + "次测试：" + "\t" + "初始零点误差：" + "\t" + "水平：" + XStep.ToString() + "\t" + "垂直：" + YStep.ToString() + "\t" + "水平误差：" + CShareData.XAdjustStepError.ToString() + "\t" + CShareData.XAdjustProb.ToString() + "\t" + "垂直误差：" + CShareData.YAdjustStepError.ToString() + "\t" + CShareData.YAdjustProb.ToString() + "\t" + "距离：" + Distance.ToString();
                            TestStr += "\r\n";
                            break;
                        case CShareData.ReturnValueAdjust.SUCCESS_HRZ:
                            TestStr += "第" + TestTime.ToString() + "次测试：" + "\t" + "初始零点误差：" + "\t" + "水平：" + XStep.ToString() + "\t" + "垂直：" + "水平位置：" + XStep.ToString() + "\t" + "垂直位置：" + YStep.ToString() + "\t" + "水平误差：" + CShareData.XAdjustStepError.ToString() + "\t" + CShareData.XAdjustProb.ToString() + "\t" + "垂直误差：" + "ER" + "\t" + "ER" + "\t" + "距离：" + Distance.ToString();
                            TestStr += "\r\n";
                            break;
                        case CShareData.ReturnValueAdjust.SUCCESS_VER:
                            TestStr += "第" + TestTime.ToString() + "次测试：" + "\t" + "初始零点误差：" + "\t" + "水平：" + XStep.ToString() + "\t" + "垂直：" + "水平位置：" + XStep.ToString() + "\t" + "垂直位置：" + YStep.ToString() + "\t" + "水平误差：" + "ER" + "\t" + "ER" + "\t" + "垂直误差：" + CShareData.YAdjustStepError.ToString() + "\t" + CShareData.YAdjustProb.ToString() + "\t" + "距离：" + Distance.ToString();
                            TestStr += "\r\n";
                            break;
                        default:
                            TestStr += "第" + TestTime.ToString() + "次测试：" + "\t" + "初始零点误差：" + "\t" + "水平：" + XStep.ToString() + "\t" + "垂直：" + "水平位置：" + XStep.ToString() + "\t" + "垂直位置：" + YStep.ToString() + "\t" + "水平误差：" + "ER" + "\t" + "ER" + "\t" + "垂直误差：" + "ER" + "\t" + "ER" + "\t" + "距离：" + Distance.ToString();
                            TestStr += "\r\n";
                            break;
                    }
                    //if ((CShareData.XAdjustStepError != 0) || (CShareData.YAdjustStepError != 0))   //有步数误差就认为失败
                    if ((Math.Abs(CShareData.XAdjustStepError) > 1) || (Math.Abs(CShareData.YAdjustStepError) > 1))   //有步数误差就认为失败
                    {
                        ErrorTime++;
                        LabelUpdate(Status, "此次测试失败！");
                        TestStr += "此次测试失败！\r\n";
                    }
                    if (Distance < CShareData.BOXMAXDISTANCE)
                    {
                        ErrorTime++;
                        LabelUpdate(Status, "此次测试失败！");
                        TestStr += "此次测试失败！\r\n";
                    }

                    ProgressBarUpdate(pBStatus, Convert.ToInt32(100.0 / TestNum * TestTime));
                    string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                    LabelUpdate(Status, "第" + TestTime.ToString() + "次测试：时间：" + TimeNow.ToString() + "\t" + "水平位置：" + XStep.ToString() + "\t" + "垂直位置：" + YStep.ToString() + "\t" + "水平误差：" + CShareData.XAdjustStepError.ToString() + "\t" + CShareData.XAdjustProb.ToString() + "\t" + "垂直误差：" + CShareData.YAdjustStepError.ToString() + "\t" + CShareData.YAdjustProb.ToString() + "\t" + "距离：" + Distance.ToString());
                    SaveOperateRecord();    //保存操作记录
                    WriteStrToTXT(path, TestStr);
                }
            }


            TestStr += "\r\n";
            TestStr += "\r\n";
            TestStr += "共测试" + TestTime.ToString() + "次";
            TestStr += "\r\n";
            TestStr += "失败" + ErrorTime.ToString() + "次" + "\t" + "失败率：" + (100.0 * ErrorTime / TestTime).ToString() + "%";


            WriteStrToTXT(path, TestStr);
            ProgressBarUpdate(pBStatus, 100);
            LabelUpdate(Status, "失败" + ErrorTime.ToString() + "次" + "\t" + "失败率：" + (100.0 * ErrorTime / TestTime).ToString() + "%");


            SaveOperateRecord();    //保存操作记录
            return true;
        }

        public bool TestAdjustXFunc(Int32 TestNum, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            string TestStr = "水平校准功能测试:" + "\r\n" + "\r\n";
            Int32 ErrorTime = 0;

            Int16 StepStart = Convert.ToInt16(-1 * TestNum / 2);
            Int16 StepEnd = Convert.ToInt16(TestNum / 2);
            for (Int16 step = StepStart; step < StepEnd; step += 1)
            {
                MoveFunc(CShareData.MoveDirection.Left, step, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                CShareData.ReturnValueAdjust Result = XAdjustFunc(serialPort1, Status, null);

                if (CShareData.ReturnValueAdjust.SUCCESS == Result)
                {
                    //回到零点并置位
                    MoveFunc(CShareData.MoveDirection.Left, CShareData.XEdgeStepSave, CShareData.SetZeroValue.Set, serialPort1, Status, null);

                    double Distance = OnePointMeasureFunc(1000, serialPort1, Status, null);
                    ReturnZeroFunc(serialPort1, Status, null);

                    TestStr += "第" + step.ToString() + "次测试：" + "\t" + "位置：" + step.ToString() + "\t" + "误差：" + CShareData.XAdjustStepError.ToString() + "\t" + CShareData.XAdjustProb.ToString() + "\t" + "距离：" + Distance.ToString();
                    TestStr += "\r\n";

                    if (Math.Abs(CShareData.XAdjustStepError - step) > 1)
                    {
                        ErrorTime++;
                        LabelUpdate(Status, "此次测试失败！");
                        TestStr += "此次测试失败！\r\n";
                    }
                }
                else
                {
                    TestStr += "第" + step.ToString() + "次测试：" + "\t" + "----------------失败!";
                    ErrorTime++;
                    LabelUpdate(Status, "此次测试失败！");
                    TestStr += "此次测试失败！\r\n";
                }

                ProgressBarUpdate(pBStatus, Convert.ToInt32(100.0 * step / TestNum));
                SaveOperateRecord();    //保存操作记录
            }


            TestStr += "\r\n";
            TestStr += "\r\n";
            TestStr += "共测试" + TestNum.ToString() + "次";
            TestStr += "\r\n";
            TestStr += "失败" + ErrorTime.ToString() + "次" + "\t" + "失败率：" + (100.0 * ErrorTime / TestNum).ToString() + "%";

            string path = "X校准功能测试" + ".txt";
            WriteStrToTXT(path, TestStr);
            ProgressBarUpdate(pBStatus, 100);
            LabelUpdate(Status, "失败" + ErrorTime.ToString() + "次" + "\t" + "失败率：" + (100.0 * ErrorTime / TestNum).ToString() + "%");

            SaveOperateRecord();    //保存操作记录
            return true;
        }


        public bool TestAdjustYFunc(Int32 TestNum, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            string TestStr = "垂直校准功能测试:" + "\r\n" + "\r\n";
            Int32 ErrorTime = 0;
            Int16 StepStart = Convert.ToInt16(-1 * TestNum / 2);
            Int16 StepEnd = Convert.ToInt16(TestNum / 2);
            for (Int16 step = StepStart; step < StepEnd; step += 1)
            {
                MoveFunc(CShareData.MoveDirection.Up, step, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                CShareData.ReturnValueAdjust Result = YAdjustFunc(serialPort1, Status, null);

                if (CShareData.ReturnValueAdjust.SUCCESS == Result)
                {
                    //回到零点并置位
                    MoveFunc(CShareData.MoveDirection.Up, CShareData.YEdgeStepSave, CShareData.SetZeroValue.Set, serialPort1, Status, null);

                    double Distance = OnePointMeasureFunc(1000, serialPort1, Status, null);
                    ReturnZeroFunc(serialPort1, Status, null);

                    TestStr += "第" + step.ToString() + "次测试：" + "\t" + "位置：" + step.ToString() + "\t" + "误差：" + CShareData.YAdjustStepError.ToString() + "\t" + CShareData.YAdjustProb.ToString() + "\t" + "距离：" + Distance.ToString();
                    TestStr += "\r\n";

                    if (Math.Abs(CShareData.YAdjustStepError - step) > 1)
                    {
                        ErrorTime++;
                        LabelUpdate(Status, "此次测试失败！");
                        TestStr += "此次测试失败！\r\n";
                    }
                }
                else
                {
                    TestStr += "第" + step.ToString() + "次测试：" + "\t" + "----------------失败!";
                    ErrorTime++;
                    LabelUpdate(Status, "此次测试失败！");
                    TestStr += "此次测试失败！\r\n";
                }
                ProgressBarUpdate(pBStatus, Convert.ToInt32(100.0 * step / TestNum));

                SaveOperateRecord();    //保存操作记录
            }

            TestStr += "\r\n";
            TestStr += "\r\n";
            TestStr += "共测试" + TestNum.ToString() + "次";
            TestStr += "\r\n";
            TestStr += "失败" + ErrorTime.ToString() + "次" + "\t" + "失败率：" + (100.0 * ErrorTime / TestNum).ToString() + "%";

            string path = "Y校准功能测试" + ".txt";
            WriteStrToTXT(path, TestStr);
            ProgressBarUpdate(pBStatus, 100);
            LabelUpdate(Status, "失败" + ErrorTime.ToString() + "次" + "\t" + "失败率：" + (100.0 * ErrorTime / TestNum).ToString() + "%");

            SaveOperateRecord();    //保存操作记录
            return true;
        }

        #endregion

        #region 粗略回零功能测试
        public bool TestRoughReturnZeroFunc(Int32 TestNum, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            string path = "粗略回零功能测试" + ".txt";
            string TestStr = "粗略回零功能测试:" + "\r\n" + "\r\n";
            Int16 StepRange = 3200;
            Int16 Step = Convert.ToInt16(StepRange / TestNum);
            Int32 ErrorTime = 0;
            Int32 TestTime = 0;

            Int16 XStep = 0;
            Int16 YStep = 0;
            for (XStep = 0; XStep < StepRange; XStep += Step)
            {
                YStep = XStep;
                //for (YStep = 0; YStep < StepRange; YStep += Step)
                {
                    MoveFunc(CShareData.MoveDirection.Left, XStep, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                    MoveFunc(CShareData.MoveDirection.Down, YStep, CShareData.SetZeroValue.Set, serialPort1, Status, null);
                    LabelUpdate(Status, "\r\n-------------------第" + TestTime.ToString() + "次测试:");
                    LabelUpdate(Status, "重新置零---水平向左：" + XStep.ToString() + "\t" + "垂直向下：" + YStep.ToString());

                    bool Result = XYRoughReturnZeroFunc(serialPort1, Status, null);

                    double Distance = OnePointMeasureFunc(1000, serialPort1, Status, null);
                    string strValid = (CShareData.OnePointDistanceValidProb * 100).ToString() + "%";
                    TestStr += "第" + TestTime.ToString() + "次测试：" + "\t" + "当前位置--" + "水平向左：" + XStep.ToString() + "\t" + "垂直向下：" + YStep.ToString() + "\t" + "定点测量距离：" + Distance.ToString() + "\t有效数据概率：" + strValid;
                    TestStr += "\r\n";

                    string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
                    LabelUpdate(Status, "第" + TestTime.ToString() + "次测试：" + "--时间：" + TimeNow.ToString() + "--" + "定点测量距离：" + Distance.ToString() + "\t有效数据概率：" + strValid);

                    if ((Result == false) || (Distance < CShareData.BOXMAXDISTANCE))
                    {
                        ErrorTime++;
                        LabelUpdate(Status, "此次测试失败！");
                        TestStr += "此次测试失败！\r\n";
                    }

                    TestTime++;
                    ProgressBarUpdate(pBStatus, Convert.ToInt32(100.0 / TestNum * TestTime));
                    SaveOperateRecord();    //保存操作记录
                    WriteStrToTXT(path, TestStr);
                }
            }


            TestStr += "\r\n";
            TestStr += "\r\n";
            TestStr += "共测试" + TestNum.ToString() + "次";
            TestStr += "\r\n";
            TestStr += "失败" + ErrorTime.ToString() + "次" + "\t" + "失败率：" + (100.0 * ErrorTime / TestNum).ToString() + "%";


            WriteStrToTXT(path, TestStr);
            ProgressBarUpdate(pBStatus, 100);
            LabelUpdate(Status, "失败" + ErrorTime.ToString() + "次" + "\t" + "失败率：" + (100.0 * ErrorTime / TestNum).ToString() + "%");

            SaveOperateRecord();    //保存操作记录
            return true;
        }
        #endregion


        #region 保存多组边框概率
        public bool SaveNProb(Int32 SaveNum, SerialPort serialPort1, Label Status, ProgressBar pBStatus)
        {
            double[] ProbArray = new double[SaveNum];
            for (int i = 0; i < SaveNum; i++)
            {
                //if (NTimeViewProbFunc(serialPort1, Status, null))
                if (ViewProbFunc(serialPort1, Status, pBStatus))
                {
                    ProbArray[i] = CShareData.ViewProb;
                    LabelUpdate(Status, "保存概率：" + i.ToString());
                    ProgressBarUpdate(pBStatus, Convert.ToInt32(100.0 / SaveNum * i));
                }
            }

            string str = "";
            for (int i = 0; i < SaveNum; i++)
            {
                str += ProbArray[i].ToString() + "\t";
            }
            //string TimeNow = DateTime.Now.ToString("HH-mm-ss") + "-" + DateTime.Now.Millisecond.ToString();
            string path = CShareData.StartPath + "保存概率--垂直步数" + CShareData.YShift.ToString() + ".txt";
            WriteStrToTXT(path, str);
            ProgressBarUpdate(pBStatus, 100);
            LabelUpdate(Status, "保存成功！");
            SaveOperateRecord();    //保存操作记录



            return true;
        }
        #endregion

    }
}
