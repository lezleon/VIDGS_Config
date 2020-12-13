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
    public partial class Area : Window
    {
        public delegate void PassDataBetweenFormHandler(object sender, PassDataWinFormEventArgs e);
        //添加一个PassDataBetweenFormHandler类型的事件
        public event PassDataBetweenFormHandler PassDataBetweenForm;
        public Area()
        {
            InitializeComponent();
            // 示例数据
            drp1Name.DropItems = new string[] 
            {
                " 捕获区域 ", " 跟踪定位区", " 登机桥位置"," 站坪位置"
            };
        }

        private void drp1Name_TextChanged(object sender, RoutedEventArgs e)
        {
            if (tbName != null)
            {
                tbName.Text = drp1Name.Text;
            }
        }

        private void PWindow_Click(object sender, EventArgs e)
        {
            string GuideNum;
            GuideNum = drp1Name.Text;
            PassDataWinFormEventArgs args = new PassDataWinFormEventArgs(GuideNum);
            PassDataBetweenForm(this, args);
        }

        private void PWindowClose_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
    }
}
