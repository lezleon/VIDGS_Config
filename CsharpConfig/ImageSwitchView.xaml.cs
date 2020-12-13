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
using System.Windows.Threading;
using System.Windows.Media.Media3D;
using System.Windows.Media.Animation;

namespace VIDGS配置软件
{
    /// <summary>
    /// ImageSwitchView.xaml 的交互逻辑
    /// </summary>
    public partial class ImageSwitchView : UserControl
    {
        public delegate void TouchDownHander(UIElement view, int index);
        public event TouchDownHander OnTouchDownEvent;

        private static double OPACITY_DOWN_FACTOR = 0.35;    // 图像之间的透明度差

        private double _xCenter;
        private double _yCenter;

        private static double SPRINESS = 0.2;		    // 弹性运动参数
        private static double CRITICAL_POINT = 0.01;
        private static double MOVE_DISTANCE = 65; //移动距离后转换
        private double _touch_move_distance = 0;
        private double _target = 0;		// 目标位置
        private double _current = 0;	// 当前位置
        public double CurrentIndex
        {
            get { return _current; }
        }
        public double TargetIndex
        {
            get { return _target; }
            set { _target = value; }
        }
        private static double childViewWidth = 480;
        public double ChildViewWidth
        {
            get { return childViewWidth; }
            set { childViewWidth = value; }
        }

        private double childViewHeight = 360;

        public double ChildViewHeight
        {
            get { return childViewHeight; }
            set { childViewHeight = value; }
        }
        private double spaceWidth = 240;

        public double SpaceWidth
        {
            get { return spaceWidth; }
            set { spaceWidth = value; }
        }
        private bool IsPressed = false;

        private List<Viewport3DControl> _images = new List<Viewport3DControl>();
        private DispatcherTimer _timer = new DispatcherTimer();

        public ImageSwitchView()
        {
            InitializeComponent();
            this.Loaded += new RoutedEventHandler(ImageSwitchView_Loaded);

            Start();
        }

        void ImageSwitchView_Loaded(object sender, RoutedEventArgs e)
        {
            _xCenter = LayoutRoot.Width / 2 - SpaceWidth / 2-150;
            _yCenter = LayoutRoot.Height / 2 - ChildViewHeight / 2+50;
        }


        void _timer_Tick(object sender, EventArgs e)
        {
            //还原位置
            if (IsPressed == false && _touch_move_distance != 0)
            {
                //回弹
                _touch_move_distance += (-_touch_move_distance) * SPRINESS;
            }
            for (int i = 0; i < _images.Count; i++)
            {
                Viewport3DControl image = _images[i];
                posImage(image, i);
            }
            if (Math.Abs(_target - _current) < CRITICAL_POINT && IsPressed == false) return;
            _current += (_target - _current) * SPRINESS;
        }

        public void AddImages(string[] imagesUri)
        {
            for (int i = 0; i < imagesUri.Length; i++)
            {
                if (imagesUri[i] != null)
                {
                    string url = imagesUri[i];
                    Viewport3DControl image = new Viewport3DControl();
                    image.SetImageSource(url);
                    image.Index = i;
                    image.Width = ChildViewWidth;
                    image.Height = ChildViewHeight;
                    image.MouseDown += new MouseButtonEventHandler(image_MouseDown);
                    LayoutRoot.Children.Add(image);
                    posImage(image, i);
                    _images.Add(image);
                }
            }
        }
        public void AddImage(string imagesrc)
        {
            Viewport3DControl image = new Viewport3DControl();
            image.SetImageSource(imagesrc);
            image.Index = _images.Count;
            image.Width = ChildViewWidth;
            image.Height = ChildViewHeight;
            image.MouseDown += new MouseButtonEventHandler(image_MouseDown);
            LayoutRoot.Children.Add(image);
            posImage(image, _images.Count);
            _images.Add(image);
        }
        public void AddImage(ImageSource bitmapImage)
        {
            Viewport3DControl image = new Viewport3DControl();
            image.SetImageSource(ref bitmapImage);
            image.Index = _images.Count;
            image.Width = ChildViewWidth;
            image.Height = ChildViewHeight;
            image.MouseDown += new MouseButtonEventHandler(image_MouseDown);
            LayoutRoot.Children.Add(image);
            posImage(image, _images.Count);
            _images.Add(image);
        }

        void image_MouseDown(object sender, MouseButtonEventArgs e)
        {
            Viewport3DControl view = (Viewport3DControl)sender;
            OnTouchDownEvent(view, view.Index);
        }

        private void posImage(Viewport3DControl image, int index)
        {
            double diffFactor = index - _current;

            double left = _xCenter + diffFactor * SpaceWidth - _touch_move_distance; 
            double top = 1.0;

            double Zindex = -Math.Abs(diffFactor) * 100;
            image.SetValue(Canvas.ZIndexProperty, (int)Zindex);

            if (index == _target)
            {
                left += 30;
                image.AnimationRotateTo(1, 1, 1, 0);
            }
            else if (index > _target)
            {
                left += 55;
                image.AnimationRotateTo(0.9, 0.9, 1, -45);

            }
            else if (index < _target)
            {
                image.AnimationRotateTo(0.9, 0.9, 1, 45);
            }

            image.Opacity = 1 - Math.Abs(diffFactor) * OPACITY_DOWN_FACTOR;
            image.SetValue(Canvas.LeftProperty, left);
            image.SetValue(Canvas.TopProperty, top);
        }

        private void moveIndex(int value)
        {
            _target += value;
            _target = Math.Max(0, _target);
            _target = Math.Min(_images.Count - 1, _target);
        }

        public void MoveLeft()
        {
            moveIndex(-1);
        }

        public void MoveRight()
        {
            moveIndex(1);
        }
        private void Start()
        {
            _timer = new DispatcherTimer();
            _timer.Interval = new TimeSpan(0, 0, 0, 0, 10);
            _timer.Tick += new EventHandler(_timer_Tick);
            _timer.Start();
        }
        Double tempi = 0;
        Point tempp;

        //效果，滑动到一定距离后自动跳转到下一项
        private void LayoutRoot_MouseMove(object sender, MouseEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed)
            {
                IsPressed = true;
                tempi += 1;
                if (tempp.X > e.GetPosition(LayoutRoot).X)
                {
                    if (tempi > MOVE_DISTANCE)
                    {
                        MoveRight();
                        tempi = 0;
                    }
                     _touch_move_distance += 1;

                }
                else if (tempp.X < e.GetPosition(LayoutRoot).X)
                {
                    if (tempi > MOVE_DISTANCE)
                    {
                        MoveLeft();
                        tempi = 0;
                    }
                   _touch_move_distance -= 1;

                }
                tempp = e.GetPosition(LayoutRoot);

            }
            else if (e.LeftButton == MouseButtonState.Released)
            {
                IsPressed = false;
                tempi = 0;
                tempp = new Point(0, 0);
            }
            else
            {
                tempi = 0;
                tempp = new Point(0, 0);
            }
        }
    }
}
