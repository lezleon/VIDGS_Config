using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace VIDGS配置软件
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class DropBox : Window
    {
        System.Threading.Mutex mutex;
        public delegate void PassBetweenFormHandler(object sender, PassWinFormEventArgs e);
        //添加一个PassDataBetweenFormHandler类型的事件
        public event PassBetweenFormHandler PassBetweenForm;
        public DropBox()
        {
            InitializeComponent();
            Stop_Start();
            // 示例数据
        }

        private void DropBoxOK_Click(object sender, EventArgs e)
        {
            string GuideNum,StopNum;
            if (drp1Name.SelectedIndex == -1 || drpAge.SelectedIndex == -1)
                return;
            GuideNum = ForDependencyGL.Text + drp1Name.SelectedValue.ToString();
            StopNum = ForDependencySL.Text + drpAge.SelectedValue.ToString();
            PassWinFormEventArgs args = new PassWinFormEventArgs(GuideNum,StopNum);
            PassBetweenForm(this, args);
            this.Close();
        }
        private void DropBoxOKClose_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
        private void titleBar_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this.DragMove();
        }
        void Stop_Start()
        {
            bool ret;
            mutex = new System.Threading.Mutex(true, "WpfMuex", out ret);
            if (!ret)
            {
                MessageBoxResult result = Xceed.Wpf.Toolkit.MessageBox.Show("设定停止线界面已经打开");
                this.Close();
            }
        }
    }
}