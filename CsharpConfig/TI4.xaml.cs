using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Threading;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Globalization;

namespace CsharpConfig
{
    /// <summary>
    /// TI4.xaml 的交互逻辑
    /// </summary>
    public partial class TI4 : Window
    {
        public delegate void NoDataBetweenFormHandler(object sender, MoDataWinFormEventArgs e);
        public delegate void MoDataBetweenFormHandler(object sender, MoDataWinFormEventArgs e);
        public delegate void OoDataBetweenFormHandler(object sender, MoDataWinFormEventArgs e);
        public delegate void PoDataBetweenFormHandler(object sender, MoDataWinFormEventArgs e);
        public delegate void QoDataBetweenFormHandler(object sender, MoDataWinFormEventArgs e);
        public delegate void RoDataBetweenFormHandler(object sender, MoDataWinFormEventArgs e);
        public event NoDataBetweenFormHandler NoDataBetweenForm;
        public event MoDataBetweenFormHandler MoDataBetweenForm;
        public event OoDataBetweenFormHandler OoDataBetweenForm;
        public event PoDataBetweenFormHandler PoDataBetweenForm;
        public event QoDataBetweenFormHandler QoDataBetweenForm;
        public event RoDataBetweenFormHandler RoDataBetweenForm;
        public string GuideString;
        MainWindow maindependency;
        public TI4()
        {
            InitializeComponent();
            maindependency = (MainWindow)App.Current.MainWindow;
        }
        private void Window_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Pop_Guide.IsOpen = false;
            Pop_Rect.IsOpen = false;
            Pop_SStop.IsOpen = false;
            Pop_Stop.IsOpen = false;
            this.DragMove();
        }

        private void Border_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
            double opacity = button_ConfirmGuideLine.Opacity;
            button_ConfirmGuideLine.Opacity = 1 - opacity;
            button_ConfirmStopLine.Opacity = 1 - opacity;
            button_Delete.Opacity = 1 - opacity;
            button_DrawRect.Opacity = 1 - opacity;
            button_ConfirmArea.Opacity = 1 - opacity;
            button_DrawPoint.Opacity = 1 - opacity;
            button_SaveCameraSet.Opacity = 1 - opacity;
            button_Clear.Opacity = 1 - opacity;
        }
        private void button_ConfirmGuideLine_Click(object sender, RoutedEventArgs e)
        {
            Pop_Guide.IsOpen = !Pop_Guide.IsOpen;
            Pop_Stop.IsOpen = false;
            Pop_SStop.IsOpen = false;
            Pop_Rect.IsOpen = false;
            //NoDataWinFormEventArgs args = new NoDataWinFormEventArgs();
            //NoDataBetweenForm(this, args);
        }
        private void Width_1_Click(object sender, RoutedEventArgs e)
        {
            string mystring = maindependency.L1.string1 + "1";
            MoDataWinFormEventArgs args = new MoDataWinFormEventArgs(mystring);
            NoDataBetweenForm(this, args);
            Pop_Guide.IsOpen = false;
        }
        private void Width_2_Click(object sender, RoutedEventArgs e)
        {
            string mystring = maindependency.L1.string1 + "2";
            MoDataWinFormEventArgs args = new MoDataWinFormEventArgs(mystring);
            NoDataBetweenForm(this, args);
            Pop_Guide.IsOpen = false;
        }
        private void Width_3_Click(object sender, RoutedEventArgs e)
        {
            string mystring = maindependency.L1.string1 + "3";
            MoDataWinFormEventArgs args = new MoDataWinFormEventArgs(mystring);
            NoDataBetweenForm(this, args);
            Pop_Guide.IsOpen = false;
        }
        private void button_ConfirmStopLine_Click(object sender, RoutedEventArgs e)
        {
            Pop_Stop.IsOpen = !Pop_Stop.IsOpen;
            //MoDataWinFormEventArgs args = new MoDataWinFormEventArgs();
            //MoDataBetweenForm(this, args);
            Pop_Guide.IsOpen = false;
        }
        private void WidthS_1_Click(object sender, RoutedEventArgs e)
        {
            GuideString = "(" + maindependency.L1.string1 + "1)";
            Pop_SStop.IsOpen = !Pop_SStop.IsOpen;
        }
        private void WidthS_2_Click(object sender, RoutedEventArgs e)
        {
            GuideString = "(" + maindependency.L1.string1 + "2)";
            Pop_SStop.IsOpen = !Pop_SStop.IsOpen;
        }
        private void WidthS_3_Click(object sender, RoutedEventArgs e)
        {
            GuideString = "(" + maindependency.L1.string1 + "3)";
            Pop_SStop.IsOpen = !Pop_SStop.IsOpen;
        }
        private void WidthSS_1_Click(object sender, RoutedEventArgs e)
        {
            string mystring = maindependency.L2.string1 + "1" + GuideString;
            MoDataWinFormEventArgs args = new MoDataWinFormEventArgs(mystring);
            MoDataBetweenForm(this, args);
            Pop_Stop.IsOpen = false;
            Pop_SStop.IsOpen = false;
        }
        private void WidthSS_2_Click(object sender, RoutedEventArgs e)
        {
            string mystring = maindependency.L2.string1 + "2" + GuideString;
            MoDataWinFormEventArgs args = new MoDataWinFormEventArgs(mystring);
            MoDataBetweenForm(this, args);
            Pop_Stop.IsOpen = false;
            Pop_SStop.IsOpen = false;
        }
        private void WidthSS_3_Click(object sender, RoutedEventArgs e)
        {
            string mystring = maindependency.L2.string1 + "3" + GuideString;
            MoDataWinFormEventArgs args = new MoDataWinFormEventArgs(mystring);
            MoDataBetweenForm(this, args);
            Pop_Stop.IsOpen = false;
            Pop_SStop.IsOpen = false;
        }
        private void WidthSS_4_Click(object sender, RoutedEventArgs e)
        {
            string mystring = maindependency.L2.string1 + "4" + GuideString;
            MoDataWinFormEventArgs args = new MoDataWinFormEventArgs(mystring);
            MoDataBetweenForm(this, args);
            Pop_Stop.IsOpen = false;
            Pop_SStop.IsOpen = false;
        }
        private void WidthSS_5_Click(object sender, RoutedEventArgs e)
        {
            string mystring = maindependency.L2.string1 + "5" + GuideString;
            MoDataWinFormEventArgs args = new MoDataWinFormEventArgs(mystring);
            MoDataBetweenForm(this, args);
            Pop_Stop.IsOpen = false;
            Pop_SStop.IsOpen = false;
        }
        private void WidthSS_6_Click(object sender, RoutedEventArgs e)
        {
            string mystring = maindependency.L2.string1 + "6" + GuideString;
            MoDataWinFormEventArgs args = new MoDataWinFormEventArgs(mystring);
            MoDataBetweenForm(this, args);
            Pop_Stop.IsOpen = false;
            Pop_SStop.IsOpen = false;
        }
        private void WidthSS_7_Click(object sender, RoutedEventArgs e)
        {
            string mystring = maindependency.L2.string1 + "7" + GuideString;
            MoDataWinFormEventArgs args = new MoDataWinFormEventArgs(mystring);
            MoDataBetweenForm(this, args);
            Pop_Stop.IsOpen = false;
            Pop_SStop.IsOpen = false;
        }
        private void WidthSS_8_Click(object sender, RoutedEventArgs e)
        {
            string mystring = maindependency.L2.string1 + "8" + GuideString;
            MoDataWinFormEventArgs args = new MoDataWinFormEventArgs(mystring);
            MoDataBetweenForm(this, args);
            Pop_Stop.IsOpen = false;
            Pop_SStop.IsOpen = false;
        }
        private void WidthSS_9_Click(object sender, RoutedEventArgs e)
        {
            string mystring = maindependency.L2.string1 + "9" + GuideString;
            MoDataWinFormEventArgs args = new MoDataWinFormEventArgs(mystring);
            MoDataBetweenForm(this, args);
            Pop_Stop.IsOpen = false;
            Pop_SStop.IsOpen = false;
        }
        private void WidthSS_A_Click(object sender, RoutedEventArgs e)
        {
            string mystring = maindependency.L2.string1 + "A" + GuideString;
            MoDataWinFormEventArgs args = new MoDataWinFormEventArgs(mystring);
            MoDataBetweenForm(this, args);
            Pop_Stop.IsOpen = false;
            Pop_SStop.IsOpen = false;
        }
        private void WidthSS_B_Click(object sender, RoutedEventArgs e)
        {
            string mystring = maindependency.L2.string1 + "B" + GuideString;
            MoDataWinFormEventArgs args = new MoDataWinFormEventArgs(mystring);
            MoDataBetweenForm(this, args);
            Pop_Stop.IsOpen = false;
            Pop_SStop.IsOpen = false;
        }
        private void WidthSS_C_Click(object sender, RoutedEventArgs e)
        {
            string mystring = maindependency.L2.string1 + "C" + GuideString;
            MoDataWinFormEventArgs args = new MoDataWinFormEventArgs(mystring);
            MoDataBetweenForm(this, args);
            Pop_Stop.IsOpen = false;
            Pop_SStop.IsOpen = false;
        }
        private void WidthSS_D_Click(object sender, RoutedEventArgs e)
        {
            string mystring = maindependency.L2.string1 + "D" + GuideString;
            MoDataWinFormEventArgs args = new MoDataWinFormEventArgs(mystring);
            MoDataBetweenForm(this, args);
            Pop_Stop.IsOpen = false;
            Pop_SStop.IsOpen = false;
        }
        private void WidthSS_E_Click(object sender, RoutedEventArgs e)
        {
            string mystring = maindependency.L2.string1 + "E" + GuideString;
            MoDataWinFormEventArgs args = new MoDataWinFormEventArgs(mystring);
            MoDataBetweenForm(this, args);
            Pop_Stop.IsOpen = false;
            Pop_SStop.IsOpen = false;
        }
        private void WidthSS_F_Click(object sender, RoutedEventArgs e)
        {
            string mystring = maindependency.L2.string1 + "F" + GuideString;
            MoDataWinFormEventArgs args = new MoDataWinFormEventArgs(mystring);
            MoDataBetweenForm(this, args);
            Pop_Stop.IsOpen = false;
            Pop_SStop.IsOpen = false;
        }
        private void Rect_1_Click(object sender, RoutedEventArgs e)
        {
            string mystring = maindependency.A1.string1;
            MoDataWinFormEventArgs args = new MoDataWinFormEventArgs(mystring);
            PoDataBetweenForm(this, args);
            Pop_Rect.IsOpen = false;
        }
        private void Rect_2_Click(object sender, RoutedEventArgs e)
        {
            string mystring = maindependency.A2.string1;
            MoDataWinFormEventArgs args = new MoDataWinFormEventArgs(mystring);
            PoDataBetweenForm(this, args);
            Pop_Rect.IsOpen = false;
        }
        private void Rect_3_Click(object sender, RoutedEventArgs e)
        {
            string mystring = maindependency.A3.string1;
            MoDataWinFormEventArgs args = new MoDataWinFormEventArgs(mystring);
            PoDataBetweenForm(this, args);
            Pop_Rect.IsOpen = false;
        }
        private void Rect_4_Click(object sender, RoutedEventArgs e)
        {
            string mystring = "站坪位置";
            MoDataWinFormEventArgs args = new MoDataWinFormEventArgs(mystring);
            PoDataBetweenForm(this, args);
            Pop_Rect.IsOpen = false;
        }
        private void button_Delete_Click(object sender, RoutedEventArgs e)
        {
            MoDataWinFormEventArgs args = new MoDataWinFormEventArgs();
            OoDataBetweenForm(this, args);
            Pop_Guide.IsOpen = false;
            Pop_Stop.IsOpen = false;
            Pop_SStop.IsOpen = false;
            Pop_Rect.IsOpen = false;
        }
        private void button_DrawRect_Click(object sender, RoutedEventArgs e)
        {
            Pop_Rect.IsOpen = !Pop_Rect.IsOpen;
            Pop_Guide.IsOpen = false;
            Pop_Stop.IsOpen = false;
            Pop_SStop.IsOpen = false;
        }
        private void button_ConfirmArea_Click(object sender, RoutedEventArgs e)
        {
            Pop_Guide.IsOpen = false;
            Pop_Stop.IsOpen = false;
            Pop_SStop.IsOpen = false;
            Pop_Rect.IsOpen = false;
        }
        private void button_DrawPoint_Click(object sender, RoutedEventArgs e)
        {
            MoDataWinFormEventArgs args = new MoDataWinFormEventArgs();
            QoDataBetweenForm(this, args);
            Pop_Guide.IsOpen = false;
            Pop_Stop.IsOpen = false;
            Pop_SStop.IsOpen = false;
            Pop_Rect.IsOpen = false;
        }
        private void button_SaveCameraSet_Click(object sender, RoutedEventArgs e)
        {
            Pop_Guide.IsOpen = false;
            Pop_Stop.IsOpen = false;
            Pop_SStop.IsOpen = false;
            Pop_Rect.IsOpen = false;
        }
        private void button_Clear_Click(object sender, RoutedEventArgs e)
        {
            MoDataWinFormEventArgs args = new MoDataWinFormEventArgs();
            RoDataBetweenForm(this, args);
            Rect_1.IsEnabled = true;
            Rect_2.IsEnabled = true;
            Rect_3.IsEnabled = true;
            Rect_4.IsEnabled = true;
            Pop_Guide.IsOpen = false;
            Pop_Stop.IsOpen = false;
            Pop_SStop.IsOpen = false;
            Pop_Rect.IsOpen = false;
        }
        private void button_SaveAndEsc_Click(object sender, RoutedEventArgs e)
        {

        }
    }
}
