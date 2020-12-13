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

namespace CsharpConfig
{
    /// <summary>
    /// Interaction logic for OfficeWindow.xaml
    /// </summary>

    public partial class Obj_dis : Window
    {
        System.Threading.Mutex mutex;
        public delegate void PassDataBetweenFormHandler(object sender, PassDataWinFormEventArgs e);
        //添加一个PassDataBetweenFormHandler类型的事件
        public event PassDataBetweenFormHandler PassDataBetweenForm;
        public Obj_dis()
        {
            InitializeComponent();
            Obj_dis_Start();
        }
        private void MWindow_Loaded(object sender, RoutedEventArgs e)
        {
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
        private void titleBar_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this.DragMove();
        }
        private void Obj_dis_Cancel(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
        private void Obj_dis_Confirm(object sender, RoutedEventArgs e)
        {
            string mystring;
            mystring = Obj_dis_Textbox.Text;
            PassDataWinFormEventArgs args = new PassDataWinFormEventArgs(mystring);
            PassDataBetweenForm(this, args);
        }
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
        void Obj_dis_Start()
        {
            bool ret;
            mutex = new System.Threading.Mutex(true, "WpfMuerttex", out ret);
            if (!ret)
            {
                MessageBoxResult result = Xceed.Wpf.Toolkit.MessageBox.Show("Objective Distance Setting is opening!");
                this.Close();
            }
        }
    }
}