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
    public partial class PWindow : Window
    {
        System.Threading.Mutex mutex;
        public delegate void PassBetweenFormHandler(object sender, PassWinFormEventArgs e);
        //添加一个PassDataBetweenFormHandler类型的事件
        public event PassBetweenFormHandler PassBetweenForm;
        public PWindow()
        {
            InitializeComponent();
            Point_Start();
            // 示例数据
        }
        private void drpAge_TextChanged(object sender, TextChangedEventArgs e)
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
                else if (textBox.Text.Contains("-") && textBox.Text.Length > 6)
                {
                    textBox.Text = textBox.Text.Remove(offset, change[0].AddedLength);
                    textBox.Select(offset, 0);
                }
                else if (!textBox.Text.Contains("-") && textBox.Text.Length > 5)
                {
                    textBox.Text = textBox.Text.Remove(offset, change[0].AddedLength);
                    textBox.Select(offset, 0);
                }
            }
        }

        private void PWindow_Click(object sender, EventArgs e)
        {
            string GuideNum, StopNum;
            if (drp1Name.SelectedIndex == -1)
                return;
            GuideNum = drp1Name.SelectedValue.ToString();
            StopNum = drpAge.Text;
            PassWinFormEventArgs args = new PassWinFormEventArgs(GuideNum, StopNum);
            PassBetweenForm(this, args);
            this.Close();
        }

        private void PWindowClose_Click(object sender, RoutedEventArgs e)
        {
            PassWinFormEventArgs args = new PassWinFormEventArgs("233", "233");
            PassBetweenForm(this, args);
            this.Close();
        }
        private void drpAge_KeyDown(object sender, KeyEventArgs e)
        {
            TextBox txt = sender as TextBox;

            //屏蔽非法按键
            if ((e.Key >= Key.NumPad0 && e.Key <= Key.NumPad9) || e.Key == Key.ImeProcessed ||e.Key == Key.OemMinus || e.Key == Key.Subtract)
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
        private void titleBar_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this.DragMove();
        }
        void Point_Start()
        {
            bool ret;
            mutex = new System.Threading.Mutex(true, "WpfMuertex", out ret);
            if (!ret)
            {
                MessageBoxResult result = Xceed.Wpf.Toolkit.MessageBox.Show("设定标志点界面已经打开");
                this.Close();
            }
        }
    }
}