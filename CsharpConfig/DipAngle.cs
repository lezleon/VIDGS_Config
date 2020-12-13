using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;
using System.Threading;

namespace VIDGS配置软件
{
    public class GetAngleArgs : EventArgs
    {
        public float Xangle;
        public float Yangle;
        public GetAngleArgs(float xAngle, float yAngle)
        {
            Xangle = xAngle;
            Yangle = yAngle;
        }
    }
    public class DipAngle
    {
        public SerialPort DipAngleSerialPort;
        public float Xangle
        {
            get;
            private set;
        }
        public float Yangle
        {
            get;
            private set;
        }
        public bool isOpen;
        bool set_zero_flag = false;
        public event EventHandler<GetAngleArgs> gotAngle;
        public DipAngle()
        {
            try
            {
                DipAngleSerialPort = new SerialPort(Parameter.glb_Parameter.system_parameter["DIPCOM"].InnerText);
                DipAngleSerialPort.BaudRate = Convert.ToInt32(Parameter.glb_Parameter.system_parameter["DIPPAUD"].InnerText);//波特率
                DipAngleSerialPort.Parity = Parity.None;//无奇偶校验位
                DipAngleSerialPort.StopBits = StopBits.One;//1个停止位
                DipAngleSerialPort.ReceivedBytesThreshold = 1;//设置 DataReceived 事件发生前内部输入缓冲区中的字节数
                DipAngleSerialPort.ReadBufferSize = 4096;
                DipAngleSerialPort.WriteBufferSize = 2048;
                DipAngleSerialPort.DataReceived += new SerialDataReceivedEventHandler(DipAngleSerialPort_DataReceived);
                DipAngleSerialPort.Open();//打开串口
                isOpen = true;
            }
            catch (System.Exception ex)
            {
                isOpen = false;
            }
        }

        void DipAngleSerialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            try
            {
                string message = DipAngleSerialPort.ReadLine();

                if (set_zero_flag)
                {
                    if (message == "set relative zero\r")
                    {
                        set_zero_flag = false;
                        //Xceed.Wpf.Toolkit.MessageBox.Show("角度检测器设置零点成功！");
                    }
                }
                else
                {
                    try
                    {
                        Xangle = Convert.ToSingle(message.Substring(1, 6));
                        Yangle = Convert.ToSingle(message.Substring(10, 6));
                        if (gotAngle != null) gotAngle(this, new GetAngleArgs(Xangle, Yangle));
                    }
                    catch (System.Exception ex)
                    {

                    }
                }
                DipAngleSerialPort.ReadExisting();
            }
            catch (Exception ex)
            {

            }
        }

        public void SetZero()
        {
            DipAngleSerialPort.WriteLine("&Z");
            set_zero_flag = true;
        }
        
    }
}
