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
using System.ComponentModel;
using System.Collections.ObjectModel;

namespace VIDGS配置软件
{
    public class SNum
    {
        public char Stop { set; get; }
    }
    public class SNumArr : ObservableCollection<SNum>
    {
        public SNumArr()
        {
            this.Add(new SNum { Stop = '1' });
            this.Add(new SNum { Stop = '2' });
            this.Add(new SNum { Stop = '3' });
            this.Add(new SNum { Stop = '4' });
            this.Add(new SNum { Stop = '5' });
            this.Add(new SNum { Stop = '6' });
            this.Add(new SNum { Stop = '7' });
            this.Add(new SNum { Stop = '8' });
            this.Add(new SNum { Stop = '9' });
            this.Add(new SNum { Stop = 'A' });
            this.Add(new SNum { Stop = 'B' });
            this.Add(new SNum { Stop = 'C' });
            this.Add(new SNum { Stop = 'D' });
            this.Add(new SNum { Stop = 'E' });
            this.Add(new SNum { Stop = 'F' });
        }
    }
}
