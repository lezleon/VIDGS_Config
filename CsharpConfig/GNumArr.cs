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
    public class GNum
    {
        public char Guide { set; get; }
    }
    public class GNumArr : ObservableCollection<GNum>
    {
        public GNumArr()
        {
            this.Add(new GNum { Guide = '1' });
            this.Add(new GNum { Guide = '2' });
            this.Add(new GNum { Guide = '3' });
            this.Add(new GNum { Guide = '4' });
            this.Add(new GNum { Guide = '5' });
        }
    }
}
