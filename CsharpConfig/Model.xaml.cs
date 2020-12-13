﻿using System;
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
using System.Windows.Shapes;
using System.Collections;
using System.Threading;
using System.Xml;

namespace VIDGS配置软件
{
    
    /// <summary>
    /// Interaction logic for DataGridEditing.xaml
    /// </summary>
    public partial class Model : Window
    {
        System.Threading.Mutex mutex;
        public XmlDocument Doc;
        public XmlNode xml_plane;
        public class PlaneType
        {
            public string type{get; set;}
            public int guideLine {
                get
                {
                    return _guideLine;
                }
                set
                {
                    _guideLine = value;
                    var temp = stopLine.ToCharArray();
                    if (temp[0] < 65)
                        sortId = value * 100 + Convert.ToInt32(temp[0] - 48);
                    else
                        sortId = value * 100 + Convert.ToInt32(temp[0] - 55); ;
                }
            }
            public int _guideLine;
            public string stopLine
            {
                get
                {
                    return _stopLine;
                }
                set
                {
                    _stopLine = value;
                    var temp = value.ToCharArray();
                    if (temp[0] < 65)
                        sortId = guideLine * 100 + Convert.ToInt32(temp[0] - 48);
                    else
                        sortId = guideLine * 100 + Convert.ToInt32(temp[0] - 55); ;
                }
            }
            public string _stopLine ="1";
            public int sortId;
            public PlaneType(string _type, int _guideLine, string _stopLine)
            {
                //sortId = _guideLine * 100 + _stopLine;
                this.type = _type;
                this.guideLine = _guideLine;
                this.stopLine = _stopLine;
            }
            public static List<PlaneType> getAllPlaneType()
            {
                List<Parameter.StopLine> allStopLines = Parameter.glb_Parameter.ReadStopLines();
                List<PlaneType> allPlaneTypes = new List<PlaneType>();
                foreach (Parameter.StopLine s in allStopLines)
                {
                    int guideline = Convert.ToInt32(s.Name.Substring(9, 1));
                    int tempi;
                    string stopline;
                    if (s.Name.Length == 11)
                        stopline = s.Name.Substring(10, 1);
                    else
                    {
                        tempi = Convert.ToInt32(s.Name.Substring(10, 2));
                        stopline = ((char)(tempi + 55)).ToString();
                    }
                    foreach (string t in s.planeTypes)
                    {
                        allPlaneTypes.Add(new PlaneType(t, guideline, stopline));
                    }
                }
                Comparison<PlaneType> dpCompare = delegate(PlaneType p1, PlaneType p2)
                {
                    return (int)(p1.sortId - p2.sortId);
                };
                allPlaneTypes.Sort(dpCompare);
                return allPlaneTypes;
            }
            
        }
        public static List<PlaneType> AllPlaneTypes;
        public static List<string> SelectPlaneTypes;
        public Model()
        {

            InitializeComponent();
            Model_Start();
            SelectPlaneTypes = new List<string>();
            Planedata();
            //categoryColumn.ItemsSource = App.StoreDb.GetCategories();
            //categoryColumn1.ItemsSource = App.StoreDb.GetCategories();
            AllPlaneTypes = PlaneType.getAllPlaneType();
            PlaneTypeDataGrid.ItemsSource = AllPlaneTypes;
            //foreach (PlaneType p in AllPlaneTypes)
            //{
            //    PlaneTypeDataGrid.Items.Add(p);
            //}
            PlaneTypeDataGrid.LoadingRow += PlaneTypeDataGrid_LoadingRow;
            PlaneTypeDataGrid.AutoGeneratedColumns += PlaneTypeDataGrid_AutoGeneratedColumns;
            PlaneTypeDataGrid.InitializingNewItem += PlaneTypeDataGrid_InitializingNewItem;
            PlaneTypeDataGrid.CellEditEnding += PlaneTypeDataGrid_CellEditEnding;
            MouseOnCancel = false;
        }

        void PlaneTypeDataGrid_InitializingNewItem(object sender, InitializingNewItemEventArgs e)
        {
            
            //throw new NotImplementedException();
        }
        void PlaneTypeDataGrid_AutoGeneratedColumns(object sender, EventArgs e)
        {
            PlaneTypeDataGrid.CanUserAddRows = true;
            PlaneTypeDataGrid.CanUserDeleteRows = true;
            PlaneTypeDataGrid.CanUserSortColumns = false;
            //PlaneTypeDataGrid.CurrentCell = new DataGridCellInfo(Splash.WPF.DataGridPlus.GetCell(PlaneTypeDataGrid, PlaneTypeDataGrid.Items.Count - 1, 0));
            //throw new NotImplementedException();
        }
        public delegate void SetEditStatus(object item);
        void SetEditStatusThread(object focus_item)
        {
            Thread.Sleep(300);
            SetEditStatus st = new SetEditStatus(delegate(object item)
            {
                PlaneTypeDataGrid.Focus();
                PlaneTypeDataGrid.SelectedCells.Clear();
                PlaneTypeDataGrid.ScrollIntoView(item);
                PlaneTypeDataGrid.CurrentCell = new DataGridCellInfo(item, PlaneTypeDataGrid.Columns[0]);
                PlaneTypeDataGrid.SelectedItem = item;
                PlaneTypeDataGrid.BeginEdit();
                PlaneTypeDataGrid.CellEditEnding += PlaneTypeDataGrid_CellEditEnding;
            });
            Dispatcher.Invoke(st, focus_item);
        }
        void PlaneTypeDataGrid_CellEditEnding(object sender, DataGridCellEditEndingEventArgs e)
        {
            PlaneType p;
            if (MouseOnCancel) return;
            if (!e.Cancel)
            {
                p = e.Row.Item as PlaneType;
                bool findSame = false;
                bool findExist = false;
                foreach (var plane_type in AllPlaneTypes)
                {
                    if (p != plane_type && plane_type.type == p.type)
                    {
                        findSame = true;
                    }
                }
                foreach (var plane_type in SelectPlaneTypes)
                {
                    if (p.type == plane_type)
                    {
                        findExist = true;
                    }
                }
                if (findExist)
                {
                    if (findSame)
                    {
                        PlaneTypeDataGrid.CellEditEnding -= PlaneTypeDataGrid_CellEditEnding;
                        MessageBox.Show("机型不可重复！", "错误提示");
                        Thread setEditThread = new Thread(new ParameterizedThreadStart(SetEditStatusThread));
                        setEditThread.Start(e.Row.Item);
                    }
                }
                else
                {
                    PlaneTypeDataGrid.CellEditEnding -= PlaneTypeDataGrid_CellEditEnding;
                    MessageBox.Show("机型不存在！", "错误提示");
                    Thread setEditThread = new Thread(new ParameterizedThreadStart(SetEditStatusThread));
                    setEditThread.Start(e.Row.Item);
                }
            }
        }
        void PlaneTypeDataGrid_LoadingRow(object sender, DataGridRowEventArgs e)
        {
            e.Row.Header = e.Row.GetIndex() + 1;
            //throw new NotImplementedException();
        }
        public void SaveAllPlaneTypes()
        {
            Comparison<PlaneType> dpCompare = delegate(PlaneType p1, PlaneType p2)
            {
                return (int)(p1.sortId - p2.sortId);
            };
            AllPlaneTypes.Sort(dpCompare);
            Parameter.glb_Parameter.ClearPlaneType();
            int lastId = 0;
            List<string> stopLinePlaneTypes = new List<string>();
            foreach (PlaneType p in AllPlaneTypes)
            {
                if (lastId != p.sortId && stopLinePlaneTypes.Count>0)
                {
                    Parameter.glb_Parameter.SetPlaneType(lastId / 100, lastId % 100, stopLinePlaneTypes);
                    stopLinePlaneTypes = new List<string>();
                }
                stopLinePlaneTypes.Add(p.type);
                lastId = p.sortId;
            }
            if (stopLinePlaneTypes.Count > 0)
            {
                Parameter.glb_Parameter.SetPlaneType(lastId / 100, lastId % 100, stopLinePlaneTypes);
                stopLinePlaneTypes = new List<string>();
            }
        }
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            SaveAllPlaneTypes();
        }
        private void appbar_arrow_left_Click(object sender, RoutedEventArgs e)
        {
            if (PlaneTypeDataGrid.SelectedCells.Count != 0)
            {
                if (PlaneTypeDataGrid.SelectedCells[0] != null)
                    AllPlaneTypes.Remove((PlaneType)PlaneTypeDataGrid.SelectedCells[0].Item);
            }
            else
            {
                MessageBox.Show("请选择一个机型!", "错误信息");
            }
            PlaneTypeDataGrid.ItemsSource = null;
            PlaneTypeDataGrid.ItemsSource = AllPlaneTypes;
            //SaveAllPlaneTypes();
        }
        private void Button_Click_2(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
        private bool MouseOnCancel;
        private void Button_MouseEnter_1(object sender, MouseEventArgs e)
        {
            MouseOnCancel = true;
        }
        private void Button_MouseLeave_1(object sender, MouseEventArgs e)
        {
            MouseOnCancel = false;
        }
        void Model_Start()
        {
            bool ret;
            mutex = new System.Threading.Mutex(true, "WpfMuerterterttex", out ret);
            if (!ret)
            {
                MessageBoxResult result = Xceed.Wpf.Toolkit.MessageBox.Show("机型设置界面已经打开");
                this.Close();
            }
        }
        private void Planedata()
        {
            Doc = new XmlDocument();
            Doc.Load(@"./PlaneData.xml");
            xml_plane = Doc["opencv_storage"]["planelist"];
            foreach (XmlNode glNode in xml_plane)
            {
                if (glNode["planetype"].InnerText != "")
                {
                    ForSelect.Items.Add(glNode["planetype"].InnerText);
                    SelectPlaneTypes.Add(glNode["planetype"].InnerText);
                }

            }
        }
        private void appbar_arrow_right_Click(object sender, RoutedEventArgs e)
        {
            if (ForSelect.SelectedItems.Count != 0)
            {
                string p = ForSelect.SelectedItem.ToString();
                bool findSame = false;
                foreach (var plane_type in AllPlaneTypes)
                {
                    if (plane_type.type == p)
                    {
                        findSame = true;
                    }
                }
                if (findSame)
                {
                    MessageBox.Show("机型不可重复！", "错误提示");
                }
                else if (ForSelect1.SelectedIndex == -1 || ForSelect2.SelectedIndex == -1)
                {
                    MessageBox.Show("请设置引导线和停止线", "错误提示");
                }
                else
                {
                    char[] tempc = ForSelect2.SelectedValue.ToString().ToCharArray();
                    PlaneTypeDataGrid.ItemsSource = null;
                    AllPlaneTypes.Add(new PlaneType(p, Convert.ToInt32(ForSelect1.SelectedValue), ForSelect2.SelectedValue.ToString()));
                    PlaneTypeDataGrid.ItemsSource = AllPlaneTypes;
                    PlaneTypeDataGrid.Focus();
                    PlaneTypeDataGrid.ScrollIntoView(PlaneTypeDataGrid.Items[PlaneTypeDataGrid.Items.Count - 1]);
                    PlaneTypeDataGrid.CurrentCell = new DataGridCellInfo(PlaneTypeDataGrid.Items[PlaneTypeDataGrid.Items.Count - 1], PlaneTypeDataGrid.Columns[0]);
                    PlaneTypeDataGrid.SelectedItem = PlaneTypeDataGrid.Items[PlaneTypeDataGrid.Items.Count - 1];
                    PlaneTypeDataGrid.BeginEdit();
                }
            }
        }
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            List<ComboPlane> Guidenum_List = new List<ComboPlane>();
            foreach(var key in MainWindow.stoplineFromBoth.Keys)
            {
                Guidenum_List.Add(new ComboPlane() { comboPlane = key.ToString() });
            }
            ForSelect1.ItemsSource = Guidenum_List;
            ForSelect1.DisplayMemberPath = "comboPlane";
            ForSelect1.SelectedValuePath = "comboPlane";
        }

        private void ForSelect1_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            List<ComboPlane> Stopnum_List = new List<ComboPlane>();
            if (ForSelect1.SelectedIndex == -1)
                return;
            var tempHashSet = MainWindow.stoplineFromBoth[ForSelect1.SelectedValue.ToString()];
            foreach(var s in tempHashSet)
            {
                Stopnum_List.Add(new ComboPlane() { comboPlane = s });
            }
            ForSelect2.ItemsSource = Stopnum_List;
            ForSelect2.DisplayMemberPath = "comboPlane";
            ForSelect2.SelectedValuePath = "comboPlane";
        }
    }
}
