using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Threading;
using System.Configuration;
using System.IO;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Text.RegularExpressions;
using System.Threading;

namespace VIDGS配置软件
{
    /// <summary>
    /// Log.xaml 的交互逻辑
    /// </summary>
    public partial class Log : Window
    {
        double Xrenderi = 1;//X轴尺度系数
        double Yrenderi = 1;//Y轴尺度系数
        double Axis_x_mid = 0;//初始化X轴刻度中点
        double Axis_y_mid = 40000;//初始化Y轴刻度中点
        List<long> Time = new List<long>();//所有引导数据txt的datetime的logn格式
        long min = long.MaxValue;
        long max = long.MinValue;
        int target = 0;//当前引导数据的index
        int imageindex = 0;
        int imagenums = 0;
        int ForwardOffset = 0;
        int BackwardOffset = 0;
        int ImageWidth, ImageHeight;//图像宽度和长度
        int sensitiveDistance;//判定鼠标与Drawshape适合到了可操作距离
        int minImageBorder;
        public string filename = "";
        public string fileflag1;//时间匹配
        public string fileflag2;//机型匹配
        public string fileflag3;//站坪匹配
        string Imagefile="";//image文件夹名
        string Uploadfile="";//upload文件名
        long fileflagcreatetime;
        List<Point> HeightData = new List<Point>();//高度数据，用于heightcanvas
        List<Point> OffsetData = new List<Point>();//偏移数据，用于offsetcanvas
        private String[] IMAGES = new string[64];//图片路径数组
        Dictionary<int, ReguxList> funny = new Dictionary<int, ReguxList>();// ReguxList字典
        WriteableBitmap ImageBackupLog;
        byte[] newImageBufferLog;
        WriteableBitmap writeableSourceLog;
        private Point origin;
        private Point start;
        private Point origin_r;
        private Point start_r;

        MouseStatus nowDrawing;
        List<DrawShape> DrawingBackupLog;
        Dictionary<int, Tuple<int,int,int>[] > tempDictionary;
        Dictionary<byte, Tuple<byte, byte, byte>> colormap;
        DrawShape nowShapeLog;

        System.Drawing.Bitmap colorbar;

        public class ReguxList//读取数据的数据结构
        {
            private double[] height;
            public double[] Height
            {
                get { return height; }
                set { height = (double[])value.Clone(); }
            }
            private double[] distance;
            public double[] Distance
            {
                get { return distance; }
                set { distance = (double[])value.Clone(); }
            }
            private double[] offset;
            public double[] Offset
            {
                get { return offset; }
                set { offset = (double[])value.Clone(); }
            }
            private double[] hehe;//前向扫描数据
            public double[] HeHe
            {
                get { return hehe; }
                set { hehe = (double[])value.Clone();}
            }
            private double[] haha;//后向扫描数据
            public double[] HaHa
            {
                get { return haha; }
                set { haha = (double[])value.Clone(); }
            }
            ~ReguxList()
            {
                height = null;
                distance = null;
                offset = null;
                hehe = null;
                haha = null;
                Dispose(false);
                GC.Collect();
            }
            public void Dispose()
            {
                Dispose(true);
                GC.SuppressFinalize(this);
            }
            private bool disposed = false;
            protected void Dispose(bool disposing)
            {
                if (!disposed)
                {
                    if (disposing)
                    {
                        // Free other state (managed objects).
                    }
                    // Free your own state (unmanaged objects).
                    // Set large fields to null.
                    disposed = true;
                }
            }
        }
        public Log()
        {
            InitializeComponent();
            nowShapeLog = new DrawShape();
            DrawingBackupLog = new List<DrawShape>();
            TransformGroup groupLog = new TransformGroup();
            TransformGroup GroupRecord = new TransformGroup();
            ScaleTransform xformLog = new ScaleTransform();
            ScaleTransform xformrecord = new ScaleTransform();
            groupLog.Children.Add(xformLog);
            GroupRecord.Children.Add(xformrecord);
            TranslateTransform ttLog = new TranslateTransform();
            TranslateTransform ttrecord = new TranslateTransform();
            groupLog.Children.Add(ttLog);
            ImageLog.RenderTransform = groupLog;
            GroupRecord.Children.Add(ttrecord);
            ImageRecord.RenderTransform = GroupRecord;
            sensitiveDistance = Convert.ToInt32(ConfigurationManager.AppSettings["SensitiveDistance"]);//鼠标点击时响应距离
            minImageBorder = Convert.ToInt32(ConfigurationManager.AppSettings["MinImageBorder"]);
            colormap = new Dictionary<byte, Tuple<byte, byte, byte>>();
            for (int i = 0; i < 32; i++)
            {
                colormap.Add((byte)i, new Tuple<byte, byte, byte>(0, 0, (byte)(i * 4 + 128 + 3)));
            }
            for (int i = 0; i < 64; i++)
            {
                colormap.Add((byte)(i+32), new Tuple<byte, byte, byte>(0, (byte)(i * 4 + 3), 255));
            }
            for (int i = 0; i < 64; i++)
            {
                colormap.Add((byte)(i + 96), new Tuple<byte, byte, byte>((byte)(i * 4 + 3), 255, (byte)(255 - i * 4 - 3)));
            }
            for (int i = 0; i < 64; i++)
            {
                colormap.Add((byte)(i + 160), new Tuple<byte, byte, byte>(255, (byte)(255 - i * 4 - 3), 0));
            }
            for (int i = 0; i < 32; i++)
            {
                colormap.Add((byte)(i + 224), new Tuple<byte, byte, byte>((byte)(255 - i * 4 - 3), 0, 0));
            }
        }
        void imageSwitchView1_OnTouchDownEvent(UIElement view, int index)
        {

        }

        //读取日志目录图片路径下bmp后缀的图片，保存在private String[] IMAGES，最后调用RecordStatic
        void ReadImages()
        {
            if (Imagefile =="")
                return;
            string LocalLogDir = Parameter.glb_Parameter.system_parameter["LogSaveRoot"].InnerText;
            if (LocalLogDir.StartsWith("\"") && LocalLogDir.EndsWith("\"")) LocalLogDir = LocalLogDir.Substring(1, LocalLogDir.Length - 2);
            DirectoryInfo di = new DirectoryInfo(LocalLogDir + Imagefile);
            try
            {
                if (!di.Exists)
                    return;
                if (di == null)
                    return;
                int i = 0;
                foreach (var fi in di.GetFiles("*.bmp", SearchOption.AllDirectories))
                {
                    IMAGES[i] = fi.FullName;
                    i++;
                    if (i == 63)
                    {
                        imagenums = i;
                        break;
                    }
                }
                imagenums = i;
            }
            catch (Exception ex)
            {
                Xceed.Wpf.Toolkit.MessageBox.Show(ex.Message);
            }
            RecordStatic();
        }

        //在主窗口打开log窗口的方法中，显示调用的读取数据的方法，打开一个新的线程读取数据。
        //这里对几个路径变量赋值，读取引导数据到Dictionary<int, ReguxList> funny = new Dictionary<int, ReguxList>();
        //该方法最后调用的LoadStoryText和ReadImages
        public void LineData(string s)
        {
            Thread LinaDataThread = new Thread(new ThreadStart(delegate()
            {
                Dispatcher.Invoke(new Action(() =>
                {
            this.Title = "飞机引导记录" + "-" + s;
            string LocalLogDir = Parameter.glb_Parameter.system_parameter["LogSaveRoot"].InnerText;
            if (LocalLogDir.StartsWith("\"") && LocalLogDir.EndsWith("\"")) LocalLogDir = LocalLogDir.Substring(1, LocalLogDir.Length - 2);
            string log_s = LocalLogDir + s;
            string log_folder_name = s;
            Imagefile = s + "\\images\\";
            s = LocalLogDir + s + "/laser";
            var tempforFileInfo = new FileInfo(s);
            fileflagcreatetime = tempforFileInfo.CreationTime.ToBinary();
            #region 匹配记录文件 图片文件 站坪扫描文件

            DirectoryInfo di = new DirectoryInfo(LocalLogDir);
            //try
            //{
            //    if (di.Exists && di != null)
            //    {
            //        string[] testsss = Directory.GetFileSystemEntries(LocalLogDir);
            //        List<string> SortSSS = testsss.OrderBy(ss => new FileInfo(ss).LastWriteTime).ToList();
            //        FileInfo fiii;
            //        long aiiiii = long.MaxValue;
            //        foreach (var fss in SortSSS)
            //        {
            //            fiii = new FileInfo(fss);
            //            if (fiii.Name.Contains(fileflag1) && fiii.Name.Contains(fileflag2))
            //                if (Math.Abs(fiii.CreationTime.ToBinary() - fileflagcreatetime) < aiiiii)
            //                {
            //                    aiiiii = Math.Abs(fiii.CreationTime.ToBinary() - fileflagcreatetime);
            //                    Imagefile = fiii.Name + "\\images\\";
            //                }
            //        }
            //    }
            //}
            //catch (Exception ex)
            //{
            //    Xceed.Wpf.Toolkit.MessageBox.Show(ex.Message);
            //}
            di = new DirectoryInfo(log_s);
            try
            {
                if (di.Exists && di != null)
                {
                    string[] testsss = Directory.GetFiles(log_s);
                    List<string> SortSSS = testsss.OrderBy(ss => new FileInfo(ss).LastWriteTime).ToList();
                    FileInfo fiii;
                    long aiiiii = long.MaxValue;
                    foreach (var fss in SortSSS)
                    {
                        fiii = new FileInfo(fss);
                        if (fiii.Name.Contains("UPLOAD")  && fiii.Name.Contains(fileflag2))
                            if (Math.Abs(fiii.CreationTime.ToBinary() - fileflagcreatetime) < aiiiii)
                            {
                                aiiiii = Math.Abs(fiii.CreationTime.ToBinary() - fileflagcreatetime);
                                Uploadfile = log_folder_name + "\\" + fiii.Name;
                            }
                    }
                }
            }
            catch (Exception ex)
            {
                Xceed.Wpf.Toolkit.MessageBox.Show(ex.Message);
            }
            //di = new DirectoryInfo(log_s);
            //try
            //{
            //    if (!di.Exists)
            //        return;
            //    if (di == null)
            //        return;
            //    string[] testss = Directory.GetFileSystemEntries(log_s);
            //    List<string>  SortSS = testss.OrderBy(ss => new FileInfo(ss).LastWriteTime).ToList();
            //    FileInfo fii;
            //    long aiiiii = long.MaxValue;
            //    foreach (var fss in SortSS)
            //    {
            //        fii = new FileInfo(fss);
            //        if (fii.Name.Contains("yard") && (Math.Abs(fii.CreationTime.ToBinary() - fileflagcreatetime) < aiiiii))
            //        {
            //            aiiiii = Math.Abs(fii.CreationTime.ToBinary() - fileflagcreatetime);
            //            filename = log_folder_name + "/" + fii.Name;
            //        }
            //    }
            //}
            //catch (Exception ex)
            //{
            //    Xceed.Wpf.Toolkit.MessageBox.Show(ex.Message);
            //}
            #endregion
            #region 读取引导数据文件
            di = new DirectoryInfo(s);
            try
            {
                if (!di.Exists)
                    return;
                if (di == null)
                    return;
                string regex1 = @"(?<=目标高度)\D?\d+";
                string regex2 = @"(?<=距离停止线)\D?\d+";
                string regex3 = @"(?<=左右偏转)\D?\d+";
                int tempi = 0;
                string regex4 = @"(?<=左右偏转\D?\d+\r\n)\d+(\t\d+)+";
                Point sp = new Point();
                string[] tests = Directory.GetFiles(s);
                List<string> SortS = tests.OrderBy(ss => new FileInfo(ss).LastWriteTime).ToList();
                //di.GetDirectories()
                FileInfo fi;
                foreach (var fs in SortS)
                {
                    fi = new FileInfo(fs);
                    string txtcontent = "";
                    if (fi.LastWriteTime.ToBinary() < min)
                        min = fi.LastWriteTime.ToBinary();
                    if (fi.LastWriteTime.ToBinary() > max)
                        max = fi.LastWriteTime.ToBinary();
                    using (StreamReader sr = new StreamReader(fi.FullName, Encoding.Default))
                    {
                        txtcontent = sr.ReadToEnd();
                    }
                    ReguxList tempR = new ReguxList();
                    double[] arraymc = new double[2];
                    int js = 0;
                    foreach (Match i in Regex.Matches(txtcontent, regex1))
                    {
                        arraymc[js] = Convert.ToDouble(i.Value);
                        if (js == 1)
                            js = 0;
                        else
                            js = 1;
                    }
                    tempR.Height = arraymc;
                    foreach (Match i in Regex.Matches(txtcontent, regex2))
                    {
                        arraymc[js] = Convert.ToDouble(i.Value);
                        if (js == 1)
                            js = 0;
                        else
                            js = 1;
                    }
                    tempR.Distance = arraymc;
                    foreach (Match i in Regex.Matches(txtcontent, regex3))
                    {
                        arraymc[js] = Convert.ToDouble(i.Value);
                        if (js == 1)
                            js = 0;
                        else
                            js = 1;
                    }
                    tempR.Offset = arraymc;
                    if (tempR.Distance[0] == 0 && tempR.Height[0] == 0 && tempR.Offset[0] == 0)
                        continue;
                    Match m;
                    m = Regex.Match(txtcontent, regex4);
                    txtcontent = m.Groups[0].ToString();
                    if (txtcontent == "")
                        continue;
                    string[] arrayS = Regex.Split(txtcontent, @"\t", RegexOptions.IgnoreCase);
                    double[] arrayI = new double[arrayS.Length];
                    m = m.NextMatch();
                    txtcontent = m.Groups[0].ToString();
                    if (txtcontent == "")
                        continue;
                    string[] arraySBack = Regex.Split(txtcontent, @"\t", RegexOptions.IgnoreCase);
                    double[] arrayIBack = new double[arrayS.Length];
                    for (int i = 0; i < arrayS.Length; i++)
                    {
                        arrayI[i] = Convert.ToDouble(arrayS[i]);
                        arrayIBack[i] = Convert.ToDouble(arraySBack[i]);
                    }
                    sp.X = 65 + tempR.Distance[0] / 200;
                    sp.Y = 110 - tempR.Height[0] / 50;
                    HeightData.Add(sp);
                    sp.X = 145 + tempR.Offset[0] / 30;
                    sp.Y = 365 - tempR.Distance[0] / 300;
                    OffsetData.Add(sp);

                    tempR.HeHe = arrayI;
                    tempR.HaHa = arrayIBack;
                    funny.Add(tempi, tempR);
                    Time.Add(fi.LastWriteTime.ToBinary());
                    LogListView.Items.Add(fi.LastWriteTime.ToString());
                    tempi++;

                }
                if (min != long.MaxValue)
                {
                    minDate.Content = DateTime.FromBinary(min).ToLongTimeString().ToString();
                    currentDate.Content = DateTime.FromBinary(min).ToLongTimeString().ToString();
                    maxDate.Content = DateTime.FromBinary(max).ToLongTimeString().ToString();
                }
                
            }
            catch (Exception ex)
            {
                Xceed.Wpf.Toolkit.MessageBox.Show(ex.Message);
            }
            #endregion      
            LoadStoryText(StoryViewer.Document);
            ReadImages();
                }));
            }));
            LinaDataThread.Start();
        }
        public void AddHeightLine()
        {
           Thread AddHeightLineThread = new Thread(new ThreadStart(delegate()
            {
                Dispatcher.Invoke(new Action(() =>
                {
            Polyline line = new Polyline();
            PointCollection collection = new PointCollection();
            int i = 0;
            foreach (Point p in HeightData)
            {
                collection.Add(p);
                AddVline(p,i);
                i++;
            }
            line.Points = collection;
            line.Stroke = new SolidColorBrush(Colors.Black);
            line.StrokeThickness = 2;
            HeightCanvas.Children.Add(line);
                }));
            }));
           AddHeightLineThread.Start();
        }
        private void AddVline(Point pp,int i)
        {
            Polyline line = new Polyline();
            PointCollection collection = new PointCollection();
            collection.Add(pp);
            collection.Add(new Point(pp.X, 110));
            line.Points = collection;
            line.Stroke = new SolidColorBrush(Colors.DarkGreen);
            line.StrokeThickness = 2;
            line.MouseEnter += new MouseEventHandler(line_MouseEnter);
            line.MouseLeave += new MouseEventHandler(line_MouseLeave);
            line.MouseLeftButtonDown += new MouseButtonEventHandler(line_MouseLeftButtonDown);
            line.Tag = i;
            HeightCanvas.Children.Add(line);
        }
        public void AddOffsetLine()
        {
               Thread AddOffsetLineThread = new Thread(new ThreadStart(delegate()
            {
                Dispatcher.Invoke(new Action(() =>
                {
            Polygon gon = new Polygon();            
            PointCollection collection = new PointCollection();
            for(int i=0;i< OffsetData.Count();i++)
            {
                collection.Add(OffsetData[i]);
                collection.Add(OffsetData[i]);
                gon.Points = collection;
                gon.Stroke = new SolidColorBrush(Colors.DarkGreen);
                gon.StrokeThickness = 4;
                gon.MouseEnter += new MouseEventHandler(Gon_MouseEnter);
                gon.MouseLeave += new MouseEventHandler(Gon_MouseLeave);
                gon.MouseLeftButtonDown += new MouseButtonEventHandler(Gon_MouseLeftButtonDown);
                gon.Tag = i;
                OffsetCanvas.Children.Add(gon);
                gon = new Polygon();
                collection = new PointCollection();
            }
                }));
            }));
               AddOffsetLineThread.Start();
        }
        private void Gon_MouseEnter(object sender, MouseEventArgs e)
        {
            Polygon g = e.OriginalSource as Polygon;
            g.StrokeThickness = 6;
            g.Stroke = new SolidColorBrush(Colors.Red);
        }
        private void line_MouseEnter(object sender, MouseEventArgs e)
        {
            Polyline g = e.OriginalSource as Polyline;
            if (g.Tag != null)
            {
                g.StrokeThickness = 4;
                g.Stroke = new SolidColorBrush(Colors.Red);
            }
        }
        private void Gon_MouseLeave(object sender, MouseEventArgs e)
        {
            OffsetCanvasWow();
        }
        private void line_MouseLeave(object sender, MouseEventArgs e)
        {
            HeightCanvasWow();
        }
        private void Gon_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Polygon g = e.OriginalSource as Polygon;
            ReguxList se = new ReguxList();
            if (funny.TryGetValue(Convert.ToInt32(g.Tag), out se))
            {
                target = Convert.ToInt32(g.Tag);
                LogListView.SelectedIndex = target;
                LogListView.UpdateLayout();
                LogListView.ScrollIntoView(LogListView.SelectedItem);
                currentDate.Content = DateTime.FromBinary(Time[target]).ToLongTimeString().ToString();
                TimeSlider.Value = (double)(10*(Time[target] - min)) / (max - min);
                target_Changed();
            }
        }
        private void line_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Polyline g = e.OriginalSource as Polyline;
            ReguxList se = new ReguxList();
            if (funny.TryGetValue(Convert.ToInt32(g.Tag), out se))
            {
                target = Convert.ToInt32(g.Tag);
                LogListView.SelectedIndex = target;
                LogListView.UpdateLayout();
                LogListView.ScrollIntoView(LogListView.SelectedItem);
                currentDate.Content = DateTime.FromBinary(Time[target]).ToLongTimeString().ToString();
                TimeSlider.Value = (double)(10 * (Time[target] - min)) / (max - min);
                target_Changed();
            }
        }
        private void ShowimageSwitchView1()
        {
                ((Storyboard)this.Resources["ShowimageSwitchView1"]).Begin(this);
        }
        private void HideimageSwitchView1()
        {
            // Uses an animation to hide the Family Data Control
            //if (imageSwitchView1.IsVisible)
                //((Storyboard)this.Resources["HideimageSwitchView1"]).Begin(this);
        }
        private void Button_KeyImage_Click(object sender, RoutedEventArgs e)
        {
            ShowimageSwitchView1();
        }
        private void LaserImage_CloseButtonClick(object sender, RoutedEventArgs e)
        {
            HideimageSwitchView1();
        }
        private void Log_Loaded(object sender, RoutedEventArgs e)
        {
            System.Drawing.Bitmap imagelog = new System.Drawing.Bitmap(@"./可视化3.bmp");
            ImageLog.Source = MainWindow.ToBitmapSource(imagelog);
            colorbar = new System.Drawing.Bitmap(@"./colorbar.bmp");
            Colormap_Image.Source = MainWindow.ToBitmapSource(colorbar);
            ImageWidth = (int)ImageLog.Source.Width;
            ImageHeight = (int)ImageLog.Source.Height;
            Read_StationScan_txt();
            //ImageBackupLog = new WriteableBitmap(ImageLog.Source as BitmapSource);
            //ImageBackupLog = BitmapFactory.ConvertToPbgra32Format(ImageBackupLog);
            //writeableSourceLog = ImageBackupLog.Clone();
            //newImageBufferLog = new byte[2000 * 2000 * 4];
        }

        //Log_Loaded中调用，读取站坪扫描记录到tempDictionary，并画图。最后调用Axis2dScan画出坐标轴
        void Read_StationScan_txt()
        {
            if (filename == "")
                return;
            string LocalLogDir = Parameter.glb_Parameter.system_parameter["LogSaveRoot"].InnerText;
            if (LocalLogDir.StartsWith("\"") && LocalLogDir.EndsWith("\"")) LocalLogDir = LocalLogDir.Substring(1, LocalLogDir.Length - 2);
            string s = LocalLogDir + filename + "\\yard";
            DirectoryInfo di = new DirectoryInfo(s);
            try
            {
                if (!di.Exists)
                    return;
                if (di == null)
                    return;
                tempDictionary = new Dictionary<int, Tuple<int, int, int>[]>();
                string[] tests = Directory.GetFiles(s);
                List<string> SortS = tests.OrderBy(ss => new FileInfo(ss).LastWriteTime).ToList();
                FileInfo fi;
                int tempi;
                foreach (var fs in SortS)
                {
                    fi = new FileInfo(fs);
                    string txtcontent = "";
                    txtcontent = fi.FullName;
                    Match m;
                    m = Regex.Match(txtcontent, @"(\d+)全扫描记录");
                    if (m.Length == 0)
                        continue;
                    tempi = Convert.ToInt32(m.Groups[1].ToString());
                    //if (tempi % 2 == 1) continue;
                    if (tempi > 95000)
                        continue;
                    using (StreamReader sr = new StreamReader(fi.FullName, Encoding.Default))
                    {
                        txtcontent = sr.ReadToEnd();
                    }
                    if (txtcontent == "")
                        continue;
                    string[] arrayS = Regex.Split(txtcontent, @"\t", RegexOptions.IgnoreCase);
                    Tuple<int, int, int>[] arrayI = new Tuple<int, int, int>[arrayS.Length - 1];
                    for (int i = 0; i < arrayS.Length-1; i++)
                    {
                        string tuple = arrayS[i].Substring(1, arrayS[i].Length - 2);
                        string[] xyz = tuple.Split(',');
                        if (xyz.Length != 3) continue;
                        arrayI[i] = new Tuple<int, int, int>(Convert.ToInt32(xyz[0]),Convert.ToInt32(xyz[1]),Convert.ToInt32(xyz[2]));
                    }
                    tempDictionary.Add(tempi, arrayI);
                }
                ImageBackupLog = new WriteableBitmap(ImageLog.Source as BitmapSource);
                ImageBackupLog = BitmapFactory.ConvertToPbgra32Format(ImageBackupLog);
                writeableSourceLog = ImageBackupLog.Clone();
                newImageBufferLog = new byte[2000 * 2000 * 4];
                if (ImageLog.Source != null)
                {
                    byte[] ImageBackupBufferLog = ImageBackupLog.ToByteArray();

                    ImageBackupBufferLog.CopyTo(newImageBufferLog, 0);
                    writeableSourceLog.Clear();
                    writeableSourceLog.FromByteArray(newImageBufferLog, ImageBackupBufferLog.Length);
                    byte a = 255, c = 0;
                    foreach (int key in tempDictionary.Keys)
                    {
                        Tuple<int, int, int>[] xyz = (Tuple<int, int, int>[])tempDictionary[key].Clone();
                        double z_color = 0;
                        for (int temp3 = 0; temp3 < xyz.Length; temp3++)
                        {
                            a = 255;
                            if (xyz[temp3].Item1 == 0)
                                continue;
                            z_color = (double)(xyz[temp3].Item3 + 500) / 1000.0 * 255.0;
                            if (z_color < 0)
                            {
                                c = 0;
                            }
                            else if (z_color > 255)
                            {
                                c = 255;
                            }
                            else
                            {
                                c = (byte)z_color;
                            }
                            Tuple<byte, byte, byte> color = colormap[c];
                            int temp_x = (int)((xyz[temp3].Item1 - Axis_x_mid) * 145 * Xrenderi / 10000 + 177);
                            if (temp_x < 2 || temp_x > writeableSourceLog.Width - 2) continue;
                            int temp_y = (int)(262 - (xyz[temp3].Item2 - Axis_y_mid) * 35 * Yrenderi / 8000);
                            if (temp_y < 2 || temp_y > writeableSourceLog.Height - 2) continue;
                            WriteableBitmapExtensions.FillEllipseCentered(writeableSourceLog, temp_x,
                                temp_y, 1, 1, Color.FromArgb(a, color.Item1, color.Item2, color.Item3));
                        }
                    }
                    ImageLog.Source = writeableSourceLog;
                    Axis2dScan(Axis_y_mid, Axis_x_mid);
                }
            }
            catch (Exception ex)
            {
                Xceed.Wpf.Toolkit.MessageBox.Show(ex.Message);
            }
        }
        //读取图片并显示
        public void RecordStatic(string s = "")//20160812
        {
            System.Drawing.Bitmap imagerecord;
            if (s == "")
            {
                if (IMAGES[0] != null)
                    imagerecord = new System.Drawing.Bitmap(IMAGES[0]);
                else
                    return;
            }
            else
                imagerecord = new System.Drawing.Bitmap(s);
            ImageRecord.Source = MainWindow.ToBitmapSource(imagerecord);
            refresh_up_Click();
        }
        //ImageLog的画点方法，主要设计画点颜色的转换
        void LogStatic()
        {
            if (ImageLog.Source != null && ImageBackupLog != null)
            {
                this.Dispatcher.BeginInvoke(new Action(() =>
                {
                    byte[] ImageBackupBufferLog = ImageBackupLog.ToByteArray();

                    ImageBackupBufferLog.CopyTo(newImageBufferLog, 0);
                    writeableSourceLog.Clear();
                    writeableSourceLog.FromByteArray(newImageBufferLog, ImageBackupBufferLog.Length);
                    if (Log_Station_Switch.IsChecked == true)
                    {
                        Colormap_Image.Source = MainWindow.ToBitmapSource(colorbar);
                        byte a = 255, c = 0;
                        double z_color = 0;
                        foreach (int key in tempDictionary.Keys)
                        {
                            Tuple<int, int, int>[] xyz = (Tuple<int, int, int>[])tempDictionary[key].Clone();
                            for (int temp3 = 0; temp3 < xyz.Length; temp3++)
                            {
                                a = 255;
                                if (xyz[temp3].Item2 == 0)
                                    continue;
                                z_color = (double)(xyz[temp3].Item3 + 500) / 1000.0 * 255.0;
                                if (z_color < 0)
                                {
                                    c = 0;
                                }
                                else if (z_color > 255)
                                {
                                    c = 255;
                                }
                                else
                                {
                                    c = (byte)z_color;
                                }
                                Tuple<byte, byte, byte> color = colormap[c];
                                int temp_x = (int)((xyz[temp3].Item1 - Axis_x_mid) * 145 * Xrenderi / 10000 + 177);
                                if (temp_x < 2 || temp_x > writeableSourceLog.Width - 2) continue;
                                int temp_y = (int)(262 - (xyz[temp3].Item2 - Axis_y_mid) * 35 * Yrenderi / 8000);
                                if (temp_y < 2 || temp_y > writeableSourceLog.Height - 2) continue;
                                WriteableBitmapExtensions.FillEllipseCentered(writeableSourceLog, temp_x,
                                    temp_y, 1, 1, Color.FromArgb(a, color.Item1, color.Item2, color.Item3));
                            }
                        }
                    }
                    else
                    {
                        Colormap_Image.Source = null;
                        foreach (var shape in DrawingBackupLog)
                        {
                            if (shape.point1.X == 0 && shape.point1.Y == 0) continue;
                            switch (shape.type)
                            {
                                case MouseStatus.DrawingLine:
                                    if (shape.point2.X == 0 && shape.point2.Y == 0) continue;
                                    MainWindow.DrawThickLine(ref writeableSourceLog, shape.point1, shape.point2, 1.57, shape.color);
                                    break;
                                case MouseStatus.DrawingPoint:
                                    double temp_x = (double)((shape.point1.X - 177) * 10000 / 145);
                                    double temp_y = (double)((437 - shape.point1.Y) * 8000 / 35);
                                    if (temp_x <= (Axis_x_mid + 10000 / Xrenderi) && temp_x >= (Axis_x_mid - 10000 / Xrenderi) &&
                                        temp_y <= (Axis_y_mid + 70000 / Yrenderi) && temp_y >= (Axis_y_mid - 42000 / Yrenderi))
                                    {
                                        WriteableBitmapExtensions.FillEllipseCentered(writeableSourceLog, (int)((temp_x - Axis_x_mid) * 145 * Xrenderi / 10000 + 177), (int)(262 - (temp_y - Axis_y_mid) * 35 * Yrenderi / 8000), 1, 1, shape.color);
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                    }
                    ImageLog.Source = writeableSourceLog;
                    Axis2dScan(Axis_y_mid, Axis_x_mid);
                }), null);
            }
        }
        //和点击线段点类似，点击日期滚动条中的日期，下方的时间轴更新，日期滚动条更新，调用target_Changed更新右上的记录。
        private void LogListView_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            int o = LogListView.SelectedIndex;
            if (o == -1)
                return;
            target = o;
            Log_Station_Switch.IsChecked = false;
            currentDate.Content = DateTime.FromBinary(Time[target]).ToLongTimeString().ToString();
            TimeSlider.Value = (double)(10*(Time[target] - min)) / (max - min);
            target_Changed();
        }
        private void TimeSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            long templong = (long)(min + (max - min) * TimeSlider.Value / 10);
            if (min != long.MaxValue)
                currentDate.Content = DateTime.FromBinary(templong).ToLongTimeString().ToString();
            long sub = long.MaxValue;
            int temp = 0;
            for (int i = 0; i < Time.Count(); i++)
            {
                if (Math.Abs(Time[i] - templong) < sub)
                {
                    sub = Math.Abs(Time[i] - templong);
                    temp = i;
                }
            }
            if (temp != target)
            {
                target = temp;
                LogListView.SelectedIndex = target;
                LogListView.UpdateLayout();
                LogListView.ScrollIntoView(LogListView.SelectedItems);
                target_Changed();
            }
        }

        private void ImageLog_MouseWheel(object sender, MouseWheelEventArgs e)
        {
            TransformGroup transformGroup = (TransformGroup)ImageLog.RenderTransform;
            ScaleTransform transform = (ScaleTransform)transformGroup.Children[0];

            double zoom = e.Delta > 0 ? .2 : -.2;
            Xrenderi += zoom;
            Yrenderi += zoom;
            if (Xrenderi < 0.5) Xrenderi = 0.5;
            else if (Xrenderi > 16) Xrenderi = 16;
            if (Yrenderi < 0.5) Yrenderi = 0.5;
            else if (Yrenderi > 16) Yrenderi = 16;
            LogStatic();
        }
        private void ImageRecord_MouseWheel(object sender, MouseWheelEventArgs e)
        {
            TransformGroup transformGroup = (TransformGroup)ImageRecord.RenderTransform;
            ScaleTransform transform = (ScaleTransform)transformGroup.Children[0];
            double zoom = e.Delta > 0 ? .2 : -.2;
            transform.ScaleX += zoom;
            transform.ScaleY += zoom;
            if (transform.ScaleX < 0.5) transform.ScaleX = 0.5;
            if (transform.ScaleY < 0.5) transform.ScaleY = 0.5;
            if (transform.ScaleX > 16) transform.ScaleX = 16;
            if (transform.ScaleY > 16) transform.ScaleY = 16;
        }
        private void ImageLog_MouseMove(object sender, MouseEventArgs e)
        {
            Point thisPoint = e.GetPosition(e.Source as FrameworkElement);
            thisPoint.X = thisPoint.X * ImageWidth / ImageLog.Width;
            thisPoint.Y = thisPoint.Y * ImageHeight / ImageLog.Height;

            if (!ImageLog.IsMouseCaptured) return;
            if (e.RightButton == MouseButtonState.Pressed)
            {
                Vector v = start - e.GetPosition(borderLog);
                v.Y = -v.Y;
                start = e.GetPosition(borderLog);

                Axis_x_mid += (v.X * 10000 / (145 * Xrenderi));
                Axis_y_mid += (v.Y * 8000 / (35 * Yrenderi));
                if (Axis_x_mid < -10000) Axis_x_mid = -10000;
                else if (Axis_x_mid > 10000) Axis_x_mid = 10000;
                if (Axis_y_mid < 0) Axis_y_mid = 0;
                else if (Axis_y_mid > 80000) Axis_y_mid = 80000;
                LogStatic();
            }
        }
        private void ImageRecord_MouseMove(object sender, MouseEventArgs e)
        {
            Point thisPoint = e.GetPosition(e.Source as FrameworkElement);
            thisPoint.X = thisPoint.X * ImageWidth / ImageRecord.Width;
            thisPoint.Y = thisPoint.Y * ImageHeight / ImageRecord.Height;
            if (e.RightButton == MouseButtonState.Pressed)
            {
                var trans = (TranslateTransform)((TransformGroup)ImageRecord.RenderTransform).Children.First(tr => tr is TranslateTransform);
                Point end = e.GetPosition(borderImageRecord);
                Point posGroup = e.GetPosition(GroupRecord);
                Vector v = start_r - end;
                var transX = origin_r.X - v.X;
                var transY = origin_r.Y - v.Y;
                TransformGroup transformGroup = (TransformGroup)ImageRecord.RenderTransform;
                ScaleTransform scale = (ScaleTransform)transformGroup.Children[0];

                //label_Position_Copy.Content = string.Format("x:{0},y:{1}", (int)(minImageBorder - Group2.ActualWidth / 2 - CameraImageBox.ActualWidth * scale.ScaleX / 2), (int)(minImageBorder - Group2.ActualHeight / 2 - CameraImageBox.ActualHeight * scale.ScaleY / 2));
                if (transX < minImageBorder - GroupRecord.ActualWidth / 2 - ImageRecord.ActualWidth * scale.ScaleX / 2)
                    transX = minImageBorder - GroupRecord.ActualWidth / 2 - ImageRecord.ActualWidth * scale.ScaleX / 2;
                if (transY < minImageBorder - GroupRecord.ActualHeight / 2 - ImageRecord.ActualHeight * scale.ScaleY / 2)
                    transY = minImageBorder - GroupRecord.ActualHeight / 2 - ImageRecord.ActualHeight * scale.ScaleY / 2;
                if (transX > GroupRecord.ActualWidth / 2 + ImageRecord.ActualWidth * scale.ScaleX / 2 - minImageBorder)
                    transX = GroupRecord.ActualWidth / 2 + ImageRecord.ActualWidth * scale.ScaleX / 2 - minImageBorder;
                if (transY > GroupRecord.ActualHeight / 2 + ImageRecord.ActualHeight * scale.ScaleY / 2 - minImageBorder)
                    transY = GroupRecord.ActualHeight / 2 + ImageRecord.ActualHeight * scale.ScaleY / 2 - minImageBorder;
                trans.X = transX;
                trans.Y = transY;
            }
        }
        private void ImageLog_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
            ImageLog.CaptureMouse();
            Point thisPoint = e.GetPosition(e.Source as FrameworkElement);
            thisPoint.X = thisPoint.X * ImageWidth / ImageLog.Width;
            thisPoint.Y = thisPoint.Y * ImageHeight / ImageLog.Height;
            var tt = (TranslateTransform)((TransformGroup)ImageLog.RenderTransform).Children.First(tr => tr is TranslateTransform);
            start = e.GetPosition(borderLog);
            origin = new Point(tt.X, tt.Y);

        }
        private void ImageLog_MouseRightButtonUp(object sender, MouseButtonEventArgs e)
        {
            ImageLog.ReleaseMouseCapture();
        }
        private void ImageRecord_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
            ImageRecord.CaptureMouse();
            Point thisPoint = e.GetPosition(e.Source as FrameworkElement);
            thisPoint.X = thisPoint.X * ImageWidth / ImageRecord.Width;
            thisPoint.Y = thisPoint.Y * ImageHeight / ImageRecord.Height;
            var tt = (TranslateTransform)((TransformGroup)ImageRecord.RenderTransform).Children.First(tr => tr is TranslateTransform);
            start_r = e.GetPosition(borderImageRecord);
            origin_r = new Point(tt.X, tt.Y);
        }
        private void ImageRecord_MouseRightButtonUp(object sender, MouseButtonEventArgs e)
        {
            ImageRecord.ReleaseMouseCapture();
        }
        private void refresh_up_Click()
        {
            var trans = (TranslateTransform)((TransformGroup)ImageRecord.RenderTransform).Children.First(tr => tr is TranslateTransform);
            var transX = 0;
            var transY = 0;
            trans.X = transX;
            trans.Y = transY;
            TransformGroup transformGroup = (TransformGroup)ImageRecord.RenderTransform;
            ScaleTransform scale = (ScaleTransform)transformGroup.Children[0];
            scale.ScaleX = 1;
            scale.ScaleY = 1;
            sensitiveDistance = (int)(Convert.ToInt32(ConfigurationManager.AppSettings["SensitiveDistance"]) / scale.ScaleX);
            sensitiveDistance = sensitiveDistance < 3 ? 3 : sensitiveDistance;
        }

        private void target_Changed()
        {
            OffsetCanvasWow();
            HeightCanvasWow();
            ReguxList se = new ReguxList();
            if (funny.TryGetValue(target, out se))
            {
                TB_Height.Text = se.Height[0].ToString();
                TB_Dis.Text = se.Distance[0].ToString();
                TB_Offset.Text = se.Offset[0].ToString();
                TF_Height.Text = se.Height[1].ToString();
                TF_Dis.Text = se.Distance[1].ToString();
                TF_Offset.Text = se.Offset[1].ToString();
            }
            ImageFresh();
        }
        private void ImageFresh()
        {
            DrawingBackupLog.Clear();
            Xrenderi = 1;
            Yrenderi = 1;
            Axis_x_mid = 0;
            Axis_y_mid = 40000;
            ReguxList se = new ReguxList();
            if (funny.TryGetValue(target, out se))
            {
                for (int temp3 = 0; temp3 < se.HeHe.Length; temp3++)
                {
                    nowShapeLog.type = MouseStatus.DrawingPoint;
                    nowShapeLog.color = Colors.OrangeRed;
                    nowShapeLog.point1.X = 29 + (double)(temp3 - ForwardOffset) * 228 / 109;
                    nowShapeLog.point1.Y = 437 - se.HeHe[temp3] * 35 / 8000;
                    DrawingBackupLog.Add(nowShapeLog);
                    nowShapeLog = new DrawShape();
                }
                for (int temp4 = 0; temp4 < se.HaHa.Length; temp4++)
                {
                    nowShapeLog.type = MouseStatus.DrawingPoint;
                    nowShapeLog.color = Colors.Blue;
                    nowShapeLog.point1.X = 29 + (double)(temp4 + BackwardOffset) * 228 / 109;
                    nowShapeLog.point1.Y = 437 - se.HaHa[temp4] * 35 / 8000;
                    DrawingBackupLog.Add(nowShapeLog);
                    nowShapeLog = new DrawShape();
                }
            }
            LogStatic();
        }
        private void OffsetCanvasWow()
        {
            Polygon g = new Polygon();
            for (int i = 0; i < OffsetCanvas.Children.Count; i++)
            {
                if (OffsetCanvas.Children[i].GetType() == g.GetType())
                {
                    Polygon hehe = OffsetCanvas.Children[i] as Polygon;
                    hehe.StrokeThickness = 4;
                    hehe.Stroke = new SolidColorBrush(Colors.DarkGreen);
                    if (hehe.Tag != null && Convert.ToInt32(hehe.Tag) == target)
                    {
                        hehe.StrokeThickness = 6;
                        hehe.Stroke = new SolidColorBrush(Colors.Red);
                    }
                }
            }
        }
        private void HeightCanvasWow()
        {
            Polyline g = new Polyline();
            for (int i = 0; i < HeightCanvas.Children.Count; i++)
            {
                if (HeightCanvas.Children[i].GetType() == g.GetType())
                {
                    Polyline hehe = HeightCanvas.Children[i] as Polyline;
                    if (hehe.Tag != null)
                    {                   
                        hehe.StrokeThickness = 2;
                        hehe.Stroke = new SolidColorBrush(Colors.DarkGreen);
                        if (Convert.ToInt32(hehe.Tag) == target)
                        {
                            hehe.StrokeThickness = 4;
                            hehe.Stroke = new SolidColorBrush(Colors.Red);
                        }
                    }
                }
            }
        }

        private void IndexUp(object sender, RoutedEventArgs e)
        {
            if (target > 0)
            {
                target--;
                currentDate.Content = DateTime.FromBinary(Time[target]).ToLongTimeString().ToString();
                TimeSlider.Value = (double)(10 * (Time[target] - min)) / (max - min);
                target_Changed();
                LogListView.SelectedIndex = target;
            }
        }
        private void IndexDown(object sender, RoutedEventArgs e)
        {
            if (target < Time.Count - 1)
            {
                target++;
                currentDate.Content = DateTime.FromBinary(Time[target]).ToLongTimeString().ToString();
                TimeSlider.Value = (double)(10 * (Time[target] - min)) / (max - min);
                target_Changed();
                LogListView.SelectedIndex = target;
            }
        }
        private void LoadStoryText(FlowDocument flowDocument)
        {
            flowDocument.Blocks.Clear();
            TextRange textRange = new TextRange(flowDocument.ContentStart, flowDocument.ContentEnd);
            if (Uploadfile == "")
                return;
            FileInfo fi;
            string LocalLogDir = Parameter.glb_Parameter.system_parameter["LogSaveRoot"].InnerText;
            if (LocalLogDir.StartsWith("\"") && LocalLogDir.EndsWith("\"")) LocalLogDir = LocalLogDir.Substring(1, LocalLogDir.Length - 2);
            fi = new FileInfo(LocalLogDir + Uploadfile);
            string txtcontent = "";
            using (StreamReader sr = new StreamReader(fi.FullName, Encoding.Default))
            {
                txtcontent = sr.ReadToEnd();
            }
            transpose(ref txtcontent);
            textRange.Text = this.Title+"\n"+ txtcontent;
            textRange.Select(StoryViewer.Document.ContentStart, StoryViewer.Document.ContentEnd);
            StoryViewer.ViewingMode = FlowDocumentReaderViewingMode.Scroll;
            StoryViewer.ViewingMode = FlowDocumentReaderViewingMode.Page;

        }
        private void transpose(ref string txtcontent)
        {
            string[] tempS = Regex.Split(txtcontent, @"\r\n", RegexOptions.IgnoreCase);
            txtcontent="";
            for (int i = 3; i < tempS.Length; i++)
            {
                txtcontent += tempS[i] + "\r\n";
            }
            string regex1 = @"W\d+";
            
            Match m;
            m = Regex.Match(txtcontent, regex1);
            string sss = m.Groups[0].ToString();
            #region 替换
            if (txtcontent == "")
                return;
            txtcontent = txtcontent.Replace(sss, "");
            txtcontent = txtcontent.Replace("]", "");
            txtcontent = txtcontent.Replace("VIDGS start.", "\t启动任务，选择机型" + fileflag2);
            txtcontent = txtcontent.Replace("System Status: Ready.", "\t系统准备：就绪");
            txtcontent = txtcontent.Replace("System Status: Self Calibration.", "\t系统自检通过");
            txtcontent = txtcontent.Replace("Guide Success.", "\t引导成功");
            txtcontent = txtcontent.Replace("System Pause.", "\t系统停止");
            txtcontent = txtcontent.Replace("System Status: OK.", "\t系统状态：正常");
            txtcontent = txtcontent.Replace("Camera Error!", "\t相机故障");
            txtcontent = txtcontent.Replace("Too dark!", "\t图像过暗！");
            txtcontent = txtcontent.Replace("Too bright!", "\t图像过亮！");
            txtcontent = txtcontent.Replace("Apron Block!", "\t站坪上检测到障碍物！");
            txtcontent = txtcontent.Replace("Apron Block Clear!", "\t站坪上障碍物已清除！");
            txtcontent = txtcontent.Replace("Laser View Block!", "\t激光系统发现视界阻挡！");
            txtcontent = txtcontent.Replace("Vision View Block!", "\t视觉系统发现视界阻挡！");
            txtcontent = txtcontent.Replace("Laser Gate Block!", "\t激光系统发现站坪阻挡！");
            txtcontent = txtcontent.Replace("Vision Gate Block!", "\t视觉系统发现站坪阻挡！");
            txtcontent = txtcontent.Replace("Laser View Block Recover!", "\t激光系统视界阻挡恢复！");
            txtcontent = txtcontent.Replace("Vision View Block Recover!", "\t视觉系统视界阻挡恢复！");
            txtcontent = txtcontent.Replace("Vision View Block Recover!", "\t视觉系统视界阻挡恢复！");
            txtcontent = txtcontent.Replace("Laser Gate Block Recover!", "\t激光系统站坪阻挡恢复！");
            txtcontent = txtcontent.Replace("Vision Gate Block Recover!", "\t视觉系统站坪阻挡恢复！");
            txtcontent = txtcontent.Replace("Procedure Exit.", "\t退出引导。");
            txtcontent = txtcontent.Replace("Re-validating...", "\t机型重新识别");
            txtcontent = txtcontent.Replace("Insufficient Disk Memory:", "\t硬盘空间不充足");
            txtcontent = txtcontent.Replace("Self Calibration done.", "\t系统自检结束");
            txtcontent = txtcontent.Replace("plane type is null!", "\t机型缺失");
            txtcontent = txtcontent.Replace("System Status: Tracking.", "\t系统状态：跟踪");
            txtcontent = txtcontent.Replace("Unknown Error Canceled!", "\t未知错误解除");
            txtcontent = txtcontent.Replace("Boarding Bridge Status:", "\t登机桥连锁信号状态");
            txtcontent = txtcontent.Replace("true", "\t正常");
            txtcontent = txtcontent.Replace("false", "\t异常");
            txtcontent = txtcontent.Replace("Boarding Bridge Position Error!", "\t登机桥位置错误");
            txtcontent = txtcontent.Replace("System Status: Self Calibration.", "\t系统状态：自检");
            txtcontent = txtcontent.Replace("System Status: Capturing.", "\t系统状态：捕获");
            txtcontent = txtcontent.Replace("System Status: Packed.", "\t系统状态：飞机停稳");
            txtcontent = txtcontent.Replace("Laser CheckSum Error!", "\t激光校验和出错");
            txtcontent = txtcontent.Replace("Laser Command Error!", "\t激光命令错误");
            txtcontent = txtcontent.Replace("Laser Parameter Error!", "\t激光参数错误");
            txtcontent = txtcontent.Replace("Laser IPC Communication Error!", "\t激光IPC通信错误");
            txtcontent = txtcontent.Replace("Laser LDM Communication Error!", "\t激光LDM通信错误");
            txtcontent = txtcontent.Replace("Laser FPGA Communication Error!", "\t激光FPGA通信错误");
            txtcontent = txtcontent.Replace("Laser No Response!", "\t激光系统无响应");
            txtcontent = txtcontent.Replace("Vison No Response!", "\t视觉系统无响应");
            txtcontent = txtcontent.Replace("Unknown Error!", "\t未知错误");
            txtcontent = txtcontent.Replace("Laser CheckSum Recover!", "\t激光校验和错误解除");
            txtcontent = txtcontent.Replace("Laser Command Recover!", "\t激光命令错误解除");
            txtcontent = txtcontent.Replace("Laser Parameter Recover!", "\t激光参数错误解除");
            txtcontent = txtcontent.Replace("Laser IPC Communication Recover!", "\t激光IPC通信错误解除");
            txtcontent = txtcontent.Replace("Laser LDM Communication Recover!", "\t激光LDM通信错误解除");
            txtcontent = txtcontent.Replace("Laser FPGA Communication Recover!", "\t激光FPGA通信错误解除");
            txtcontent = txtcontent.Replace("Laser Recover!", "\t激光系统恢复响应");
            txtcontent = txtcontent.Replace("Vision Recover!", "\t视觉系统恢复响应");
            txtcontent = txtcontent.Replace("PCA Status:", "\tPCA状态");
            txtcontent = txtcontent.Replace("PCA status Error!", "\tPCA状态错误");
            txtcontent = txtcontent.Replace("FGP 400Hz Status", "\t登机桥400Hz电源状态：");
            txtcontent = txtcontent.Replace("BFGP 400Hz Error!", "\t登机桥400Hz电源状态错误");
            txtcontent = txtcontent.Replace("Emergence Stop State:", "\t紧急停止按钮状态：");
            txtcontent = txtcontent.Replace("Laser Tracking Failed", "\t激光跟踪失败");
            txtcontent = txtcontent.Replace("Laser Tracking recovered", "\t激光跟踪失败");
            txtcontent = txtcontent.Replace("Plane too fast.", "\t飞机速度过快");
            txtcontent = txtcontent.Replace("Plane position too far!", "\t飞机超出停止线过远");
            txtcontent = txtcontent.Replace("System Error!", "\t系统错误");
            txtcontent = txtcontent.Replace("Camera calibration error!", "\t相机标定错误");
            txtcontent = txtcontent.Replace("Plane Exited confirmed by HMI!", "\t地面操作人员确认飞机已离开");
            txtcontent = txtcontent.Replace("Vision Type Validation Fail!", "\t视觉模块机型验证失败");
            txtcontent = txtcontent.Replace("Light off signal.", "\t关闭大灯提示");
            txtcontent = txtcontent.Replace("Light closed.", "\t大灯已关闭");
            txtcontent = txtcontent.Replace("Plane Verify: head height error!", "\t机型验证：机头高度错误！");
            txtcontent = txtcontent.Replace("Plane Verify: Engine-Cabin distance error!", "\t机型验证：引擎机舱距离出错！");
            txtcontent = txtcontent.Replace("Plane Verify: Engine-Head distance error!", "\t机型验证：引擎机头距离出错！");
            txtcontent = txtcontent.Replace("Plane Verify: Engine position error!", "\t机型验证：引擎验证出错！");
            txtcontent = txtcontent.Replace("Can't detect bridge.", "\t未测到登机桥。");
            txtcontent = txtcontent.Replace("Found bridge.", "\t检测到登机桥。");
            txtcontent = txtcontent.Replace("Plane leave.", "\t检测飞机离港:飞机已离港。");
            txtcontent = txtcontent.Replace("Plane doesn't leave.", "\t检测飞机离港:飞机未离港。");
            txtcontent = txtcontent.Replace("Chock on.", "\t轮挡已放置");
            txtcontent = txtcontent.Replace("Chock off.", "\t轮挡已移除");
            txtcontent = txtcontent.Replace("Park on type:", "\t手动确认飞机已停好,机型：");
            txtcontent = txtcontent.Replace("Park off type:", "\t手动确认飞机已离开，机型：");
            txtcontent = txtcontent.Replace("LED Error!", "\tLED系统故障！");
            txtcontent = txtcontent.Replace("LED Communication Error!", "\tLED通信故障！");
            txtcontent = txtcontent.Replace("Manually confirm plane type.", "\t手动确认机型验证通过。");

            txtcontent = txtcontent.Replace("Plane Type", "\t机型");
            txtcontent = txtcontent.Replace("Error!", "验证错误");
            txtcontent = txtcontent.Replace("Laser points fall out of window:", "激光点跑飞，箱内点数：");
            txtcontent = txtcontent.Replace("Barrier found at distance:", "在以下距离发现障碍物：");
            txtcontent = txtcontent.Replace("Laser zero point error!", "激光零点参数异常");

            txtcontent = txtcontent.Replace("Validated!", "验证通过");
            txtcontent = txtcontent.Replace(" setted by server", "由上位机设置");
            string regex2 = @"(?<=RIGHT at distance )-?\d+.\d+";
            string regex3 = @"(?<=MIDDLE at distance )-?\d+.\d+";
            string regex4 = @"(?<=LEFT at distance )-?\d+.\d+";
            MatchCollection mc = Regex.Matches(txtcontent, regex2);
            foreach (Match match in mc)
            {
                string sss2 = match.Groups[0].ToString();
                txtcontent = txtcontent.Replace("RIGHT at distance " + sss2, "\t在" + sss2 + "米处偏右");
            }
            mc = Regex.Matches(txtcontent, regex3);
            foreach (Match match in mc)
            {
                string sss3 = match.Groups[0].ToString();
                txtcontent = txtcontent.Replace("MIDDLE at distance " + sss3, "\t在" + sss3 + "米处居中");
            }
            mc = Regex.Matches(txtcontent, regex4);
            foreach (Match match in mc)
            {
                string sss4 = match.Groups[0].ToString();
                txtcontent = txtcontent.Replace("LEFT at distance " + sss4, "\t在" + sss4 + "米处偏左");
            }
            txtcontent = txtcontent.Replace("Captured at distance:", "捕获到飞机，距离：");
            txtcontent = txtcontent.Replace("Reach stop line at distance:", "到达停止线，距离：");
            txtcontent = txtcontent.Replace("PARKING distance:", "停止距离：");
            txtcontent = txtcontent.Replace(" LEFT:", "，偏左：");
            txtcontent = txtcontent.Replace(" RIGHT:", "，偏右：");
            txtcontent = txtcontent.Replace(" ALIGN.", "，居中");
            txtcontent = txtcontent.Replace("distance:", "距离：");
            txtcontent = txtcontent.Replace("TOOFAR", "滑行过远");
            #endregion
        }
        private void ButtonPrint_Click(object sender, RoutedEventArgs e)
        {
            PrintDialog printDialog = new PrintDialog();
            if (printDialog.ShowDialog() == true)
            {
                // Save all the existing settings.                                
                double pageHeight = StoryViewer.Document.PageHeight;
                double pageWidth = StoryViewer.Document.PageWidth;
                Thickness pagePadding = StoryViewer.Document.PagePadding;
                double columnGap = StoryViewer.Document.ColumnGap;
                double columnWidth = StoryViewer.Document.ColumnWidth;

                // Make the FlowDocument page match the printed page.
                StoryViewer.Document.PageHeight = printDialog.PrintableAreaHeight;
                StoryViewer.Document.PageWidth = printDialog.PrintableAreaWidth;
                StoryViewer.Document.PagePadding = new Thickness(50);

                // Use two columns.
                StoryViewer.Document.ColumnGap = 25;
                StoryViewer.Document.ColumnWidth = (StoryViewer.Document.PageWidth - StoryViewer.Document.ColumnGap
                    - StoryViewer.Document.PagePadding.Left - StoryViewer.Document.PagePadding.Right) / 2;

                FlowDocument document = StoryViewer.Document;
                StoryViewer.Document = null;

                HeaderedFlowDocumentPaginator paginator = new HeaderedFlowDocumentPaginator(document);
                printDialog.PrintDocument(paginator, this.Title);

                StoryViewer.Document = document;

                // Reapply the old settings.
                StoryViewer.Document.PageHeight = pageHeight;
                StoryViewer.Document.PageWidth = pageWidth;
                StoryViewer.Document.PagePadding = pagePadding;
                StoryViewer.Document.ColumnGap = columnGap;
                StoryViewer.Document.ColumnWidth = columnWidth;
            }
        }

        //坐标轴画图方法
        void Axis2dScan(double origin_y, double origin_x)
        {
            DrawingVisual drawingVisual = new DrawingVisual();
            var rtbitmap = new RenderTargetBitmap(writeableSourceLog.PixelWidth,
    writeableSourceLog.PixelHeight, writeableSourceLog.DpiX, writeableSourceLog.DpiY,
    PixelFormats.Default);
            int temp5;
            FormattedText text1;

            double Xtemp = 177 - (origin_x * Xrenderi * 145 / 10000);
            using (var dc = drawingVisual.RenderOpen())
            {
                dc.DrawImage(writeableSourceLog, new Rect(0, 0, writeableSourceLog.Width, writeableSourceLog.Height));
                //dc.DrawRectangle(Brushes.Transparent, new Pen(Brushes.Black, 7), new Rect(10, 10, 50 + text.Width, 50 + text.Height));
                for (int i = 0; i < 9; i++)
                {
                    temp5 = (int)(((40000 - 10000 * i) / Yrenderi) + origin_y);
                    text1 = new FormattedText(temp5.ToString(),
                    System.Globalization.CultureInfo.CurrentCulture,
                    System.Windows.FlowDirection.LeftToRight,
                    new Typeface(SystemFonts.MessageFontFamily,
                        FontStyles.Normal, FontWeights.Thin,
                        FontStretches.Normal), 10, Brushes.Black);
                    dc.DrawText(text1, new Point(0, 80 + 44 * i));
                }
                if (Log_Station_Switch.IsChecked == true)
                {
                    for (int i = 0; i < 8; i++)
                    {
                        temp5 = (int)((290 * (double)i / 9 - 145.0) * 10000.0 / (double)(Xrenderi * 145.0) + origin_x);
                        text1 = new FormattedText(temp5.ToString(),
                        System.Globalization.CultureInfo.CurrentCulture,
                        System.Windows.FlowDirection.LeftToRight,
                        new Typeface(SystemFonts.MessageFontFamily,
                            FontStyles.Normal, FontWeights.Thin,
                            FontStretches.Normal), 10, Brushes.Black);
                        dc.DrawText(text1, new Point(13 + 33 * i, rtbitmap.Height - 28));
                    }
                }
            }
            rtbitmap.Render(drawingVisual);
            ImageLog.Source = rtbitmap;
        }

        private void Log_Station_Switch_IsCheckedChanged(object sender, EventArgs e)
        {
            Xrenderi = 1;
            Yrenderi = 1;
            Axis_x_mid = 0;
            Axis_y_mid = 40000;
            LogStatic();
        }
        private void IndexRight(object sender, RoutedEventArgs e)
        {
            if (imagenums != 0)
            {
                imageindex += 1;
                if (imageindex == imagenums)
                    imageindex = 0;
                RecordStatic(IMAGES[imageindex]);
            }
            //imageSwitchView1.MoveRight();
        }
        private void IndexLeft(object sender, RoutedEventArgs e)
        {
            if (imagenums != 0)
            {
                imageindex -= 1;
                if (imageindex == -1)
                    imageindex = imagenums - 1;
                RecordStatic(IMAGES[imageindex]);
            }
            //imageSwitchView1.MoveLeft();
        }
        private void LogListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {

        }

        private void Log_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            //20160812
            //imageSwitchView1.OnTouchDownEvent -= new ImageSwitchView.TouchDownHander(imageSwitchView1_OnTouchDownEvent);
            foreach (var x in HeightCanvas.Children)
            {
                Polyline line = x as Polyline;
                line.MouseEnter -= new MouseEventHandler(line_MouseEnter);
                line.MouseLeave -= new MouseEventHandler(line_MouseLeave);
                line.MouseLeftButtonDown -= new MouseButtonEventHandler(line_MouseLeftButtonDown);
            }
            GC.Collect();
        }
        private void ImagePrint_Click(object sender, RoutedEventArgs e)
        {
            PrintDialog dialog = new PrintDialog();
            if (dialog.ShowDialog() == true)
            {
                dialog.PrintVisual(ImageRecord, "打印图片");
            }
        }
    }
}
