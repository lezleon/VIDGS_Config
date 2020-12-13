using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
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
using System.Windows.Media.Animation;
using System.Windows.Input;
using System.IO;
using System.IO.Packaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using MahApps.Metro.Controls;
using System.Xml; 

namespace PD
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : MetroWindow
    {
        plane all = App.All;
        planeCollection allCollection = App.ALLCollection;
        int target = -1;
        XmlDocument xmldoc;
        XmlNodeList NodeList_total;
        public MainWindow()
        {
            InitializeComponent();
            s1.IsEnabled = false;
            s2.IsEnabled = false;
        }
        private void Import(object sender, RoutedEventArgs e)
        {
            PromptToSave();
            CommonDialog dialog = new CommonDialog();
            dialog.InitialDirectory = System.IO.Path.Combine(
                  Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments),
                  "PD");
            dialog.Filter.Add(new FilterEntry("xml File(.xml)", "*.xml"));
            dialog.Filter.Add(new FilterEntry("All Files (*.*)", "*.*"));
            dialog.Title = "打开";
            dialog.ShowOpen();

            if (!string.IsNullOrEmpty(dialog.FileName))
            {
                all.FullyQualifiedFilename = dialog.FileName;
                xmldoc = new XmlDocument();
                xmldoc.Load(dialog.FileName);
                NodeList_total = xmldoc.SelectNodes("opencv_storage/planelist/_");
                if (NodeList_total.Count != 0)
                {
                    foreach (XmlNode Node in NodeList_total)
                    {
                        XmlNodeList item = Node.ChildNodes;
                        allCollection.Add(new singleplane(item.Item(0).InnerText,
                            item.Item(1).InnerText,
                            item.Item(2).InnerText,
                            item.Item(3).InnerText,
                            item.Item(4).InnerText,
                            item.Item(5).InnerText,
                            item.Item(6).InnerText,
                            item.Item(7).InnerText,
                            item.Item(8).InnerText,
                            item.Item(9).InnerText,
                            item.Item(10).InnerText));
                    }
                }
                updatelist();
                s1.IsEnabled = true;
                s2.IsEnabled = true;
            }
        }
        private void updatelist()
        {
            moshoushijie.Items.Clear();
            foreach (var x in allCollection)
            {
                moshoushijie.Items.Add(x.Type);
            }
        }
        private void PromptToSave()
        {
            savethetemp(target);
            if (!allCollection.IsDirty)
                return;

            MessageBoxResult result = MessageBox.Show("机型信息尚未保存，是否保存",
                "保存", MessageBoxButton.YesNo, MessageBoxImage.Warning);

            if (result == MessageBoxResult.Yes)
            {
                if (string.IsNullOrEmpty(all.FullyQualifiedFilename))
                {
                    CommonDialog dialog = new CommonDialog();
                    dialog.InitialDirectory = System.IO.Path.Combine(
                      Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments),
                      "PD");
                    dialog.Filter.Add(new FilterEntry("xml File(.xml)", "*.xml"));
                    dialog.Filter.Add(new FilterEntry("All Files (*.*)", "*.*"));
                    dialog.Title = "另存为";
                    dialog.DefaultExtension = ".xml";
                    dialog.ShowSave();
                    if (!string.IsNullOrEmpty(dialog.FileName))
                    {
                        Save(dialog.FileName);
                    }
                }
                else
                    Save();
            }
        }
        private void Savexml(object sender, RoutedEventArgs e)
        {
            if (string.IsNullOrEmpty(all.FullyQualifiedFilename))
            {
                CommonDialog dialog = new CommonDialog();
                dialog.InitialDirectory = System.IO.Path.Combine(
                  Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments),
                  "PD");
                dialog.Filter.Add(new FilterEntry("xml File(.xml)", "*.xml"));
                dialog.Filter.Add(new FilterEntry("All Files (*.*)", "*.*"));
                dialog.Title = "保存";
                dialog.DefaultExtension = ".xml";
                dialog.ShowSave();

                if (!string.IsNullOrEmpty(dialog.FileName))
                {
                    Save(dialog.FileName);
                }
            }
            else
            {
                Save();
            }
        }
        public void Save()
        {
            savethetemp(target);
            if (this.allCollection == null || this.allCollection.Count == 0 || this.allCollection.IsDirty == false)
                return;
            savetoxml();
            xmldoc.Save(all.FullyQualifiedFilename);
            this.allCollection.IsDirty = false;
        }
        public void Save(string FQFilename)
        {
            all.FullyQualifiedFilename = FQFilename;
            Save();
        }
        private void Saveasxml(object sender, RoutedEventArgs e)
        {
            CommonDialog dialog = new CommonDialog();
            dialog.InitialDirectory = System.IO.Path.Combine(
              Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments),
              "PD");
            dialog.Filter.Add(new FilterEntry("xml File(.xml)", "*.xml"));
            dialog.Filter.Add(new FilterEntry("All Files (*.*)", "*.*"));
            dialog.Title = "保存为";
            dialog.DefaultExtension = ".xml";
            dialog.ShowSave();

            if (!string.IsNullOrEmpty(dialog.FileName))
            {
                Save(dialog.FileName);
            }
        }
        private void dddddd(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            IMright.Visibility = Visibility.Visible;
            int o = moshoushijie.SelectedIndex;
            if (o == -1)
                return;
            if(target > -1)
                savethetemp(target);
            target = o;
            changetheright(o);
        }
        private void savethetemp(int num)
        {
            if (num > -1)
            {
                var a = allCollection[num];
                if (a.Type == im0.Text && a.CabinWidth == im1.Text &&
                a.WingSpan == im2.Text && a.HeadHeight == im3.Text &&
                a.HeadWheelHoridis == im4.Text && a.EngineCenterdis == im5.Text &&
                a.EngineRadius == im6.Text && a.EngineHeadhordis == im7.Text &&
                a.CenterFloordis == im8.Text && a.WheelWidth == im9.Text &&
                a.WheelHeight == im10.Text)
                    ;
                else
                {
                    a.Type = im0.Text;
                    a.CabinWidth = im1.Text;
                    a.WingSpan = im2.Text;
                    a.HeadHeight = im3.Text;
                    a.HeadWheelHoridis = im4.Text;
                    a.EngineCenterdis = im5.Text;
                    a.EngineRadius = im6.Text;
                    a.EngineHeadhordis = im7.Text;
                    a.CenterFloordis = im8.Text;
                    a.WheelWidth = im9.Text;
                    a.WheelHeight = im10.Text;
                    a.isdirty = true;
                    this.allCollection.IsDirty = true;
                    updatelist();
                }
            }
        }
        private void savetoxml()
        {
            int i = 0;
            foreach (XmlNode Node in NodeList_total)
            {
                if (allCollection[i].isdirty == true)
                {
                    XmlNodeList item = Node.ChildNodes;
                    item.Item(0).InnerText = allCollection[i].Type;
                    item.Item(1).InnerText = allCollection[i].CabinWidth;
                    item.Item(2).InnerText = allCollection[i].WingSpan;
                    item.Item(3).InnerText = allCollection[i].HeadHeight;
                    item.Item(4).InnerText = allCollection[i].HeadWheelHoridis;
                    item.Item(5).InnerText = allCollection[i].EngineCenterdis;
                    item.Item(6).InnerText = allCollection[i].EngineRadius;
                    item.Item(7).InnerText = allCollection[i].EngineHeadhordis;
                    item.Item(8).InnerText = allCollection[i].CenterFloordis;
                    item.Item(9).InnerText = allCollection[i].WheelWidth;
                    item.Item(10).InnerText = allCollection[i].WheelHeight;
                    allCollection[i].isdirty = false;
                }
                i++;
            }
        }
        private void changetheright(int o)
        {
            var a = allCollection[o];
            im0.Text = a.Type;
            im1.Text = a.CabinWidth;
            im2.Text = a.WingSpan;
            im3.Text = a.HeadHeight;
            im4.Text = a.HeadWheelHoridis;
            im5.Text = a.EngineCenterdis;
            im6.Text = a.EngineRadius;
            im7.Text = a.EngineHeadhordis;
            im8.Text = a.CenterFloordis;
            im9.Text = a.WheelWidth;
            im10.Text = a.WheelHeight;
        }
        private void delete(object sender, RoutedEventArgs e)
        {
            if (target < 0)
                return;
            MessageBoxResult result = MessageBox.Show("确定要删除该机型信息",
                "删除", MessageBoxButton.YesNo, MessageBoxImage.Warning);

            if (result == MessageBoxResult.Yes)
            {
                            allCollection.RemoveAt(target);
                            moshoushijie.Items.RemoveAt(target);
                            allCollection.OnContentChanged();
                            NodeList_total[target].ParentNode.RemoveChild(NodeList_total[target]);
                            target = -1;
                            NodeList_total = xmldoc.SelectNodes("opencv_storage/planelist/_");
                            this.allCollection.IsDirty = true;
                            IMright.Visibility = Visibility.Hidden;
                            updatelist();
           }
        }
        private void newplane(object sender, RoutedEventArgs e)
        {
            if (this.allCollection == null || NodeList_total == null)
                return;
            if (NodeList_total.Count == 0)
                return;
            savethetemp(target);
            allCollection.Add(new singleplane("未命名","","","","","","","","","",""));
            moshoushijie.Items.Add("未命名");
            target = allCollection.Count - 1;
            moshoushijie.SelectedIndex = target;
            moshoushijie.UpdateLayout();
            moshoushijie.ScrollIntoView(moshoushijie.SelectedItems);
            changetheright(target);
            allCollection.OnContentChanged();
            NodeList_total[0].ParentNode.AppendChild(NodeList_total[0].ParentNode.LastChild.Clone());
            this.allCollection.IsDirty = true;
            allCollection[target].isdirty = true;
            IMright.Visibility = Visibility.Visible;
            updatelist();
        }

        private void MetroWindow_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            PromptToSave();
        }
    }
}
