using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace VIDGS配置软件
{
    /// <summary>
    /// Interaction logic for OfficeWindow.xaml
    /// </summary>

    public partial class MWindow : Window
    {
        System.Threading.Mutex mutex;
        public delegate void PassDataBetweenFormHandler(object sender, PassDataWinFormEventArgs e);
        //添加一个PassDataBetweenFormHandler类型的事件
        public event PassDataBetweenFormHandler PassDataBetweenForm;
        public MWindow()
        {
            InitializeComponent();
            Guide_Start();
        }
        bool isWiden = false;
        private void window_initiateWiden(object sender, System.Windows.Input.MouseEventArgs e)
        {
            isWiden = true;
        }
        private void window_endWiden(object sender, System.Windows.Input.MouseEventArgs e)
        {
            isWiden = false;

            // Make sure capture is released.
            Rectangle rect = (Rectangle)sender;
            rect.ReleaseMouseCapture();
        }

        private void window_Widen(object sender, System.Windows.Input.MouseEventArgs e)
        {
            Rectangle rect = (Rectangle)sender;
            if (isWiden)
            {
                rect.CaptureMouse();
                double newWidth = e.GetPosition(this).X + 5;
                if (newWidth > 0) this.Width = newWidth;
            }
        }
        private void txtNum_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            string mystring;
            mystring = ForGuidelineDependency.Text + txtNum.Text;
            PassDataWinFormEventArgs args = new PassDataWinFormEventArgs(mystring);
            PassDataBetweenForm(this, args);
        }
        private void txtNum2_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            string mystring;
            mystring = ForGuidelineDependency.Text + txtNum2.Text;
            PassDataWinFormEventArgs args = new PassDataWinFormEventArgs(mystring);
            PassDataBetweenForm(this, args);
        }
        private void txtNum3_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            string mystring;
            mystring = ForGuidelineDependency.Text + txtNum3.Text;
            PassDataWinFormEventArgs args = new PassDataWinFormEventArgs(mystring);
            PassDataBetweenForm(this, args);
        }
        private void txtNum4_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            string mystring;
            mystring = ForGuidelineDependency .Text+ txtNum4.Text;
            PassDataWinFormEventArgs args = new PassDataWinFormEventArgs(mystring);
            PassDataBetweenForm(this, args);
        }
        private void txtNum5_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            string mystring;
            mystring = ForGuidelineDependency.Text + txtNum5.Text;
            PassDataWinFormEventArgs args = new PassDataWinFormEventArgs(mystring);
            PassDataBetweenForm(this, args);
        }

        private void titleBar_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this.DragMove();
        }
        private void cmdClose_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
        void Guide_Start()
        {
            bool ret;
            mutex = new System.Threading.Mutex(true, "WpfMuerterttex", out ret);
            if (!ret)
            {
                MessageBoxResult result = Xceed.Wpf.Toolkit.MessageBox.Show("GuideLine setting is opening!");
                this.Close();
            }
        }
    }
}