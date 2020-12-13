using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
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
    /// about.xaml 的交互逻辑
    /// </summary>
    public partial class about : UserControl
    {
        public static readonly RoutedEvent CloseButtonClickEvent = EventManager.RegisterRoutedEvent(
    "CloseButtonClick", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(about));

        // Expose the PersonInfo Close Button click event
        public event RoutedEventHandler CloseButtonClick
        {
            add { AddHandler(CloseButtonClickEvent, value); }
            remove { RemoveHandler(CloseButtonClickEvent, value); }
        }
        public about()
        {
            InitializeComponent();
        }
        private void CloseButton_Click(object sender, RoutedEventArgs e)
        {
            RaiseEvent(new RoutedEventArgs(CloseButtonClickEvent));
        }
    }
}
