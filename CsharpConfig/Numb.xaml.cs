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

namespace CsharpConfig
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    /// 
    public partial class Numb : Window
    {
        public delegate void NumCharEventHandler(object sender, NumCharEventArgs e);
        //添加一个PassDataBetweenFormHandler类型的事件
        public event NumCharEventHandler NumcharForm;
        public Numb()
        {
            InitializeComponent();
            // 示例数据
        }
        private void drpGuide_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {

        }
        private void drpStop_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {

        }
        private void Numb_Click(object sender, EventArgs e)
        {
            if (this.drpGuide.SelectedIndex != -1 && this.drpStop.SelectedIndex != -1)
            {
                char GuideNum, StopNum;
                GuideNum = (char)drpGuide.SelectedValue;
                StopNum = (char)drpStop.SelectedValue;
                NumCharEventArgs args = new NumCharEventArgs(GuideNum, StopNum);
                NumcharForm(this, args);
                this.Close();
            }
        }

        private void NumbClose_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
    }
}