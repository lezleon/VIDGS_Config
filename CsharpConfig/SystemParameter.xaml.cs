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
using System.Windows.Shapes;
using System.Xml;
using System.IO;
using System.Globalization;
using System.Threading;
using Xceed.Wpf.Toolkit;
using System.Data;

namespace VIDGS配置软件
{
    /// <summary>
    /// SystemParameter.xaml 的交互逻辑
    /// </summary>
    public partial class SystemParameter : Window
    {
        CultureInfo ci;
        System.Threading.Mutex mutex;
        public class SystemPara
        {
            public string Name { get; set; }
            public string Value { get; set; }
            public string Description { get; set; }
            public string EnglishName;
            public bool Writeable { get; set; }
            public SystemPara(string _name, string _value, string _description)
            {
                EnglishName = _name;
                Name = _name;
                Value = _value;
                Description = _description;
                Writeable = true;
            }
        }
        Dictionary<string, string> cnNames;
        Dictionary<string, string> cnDescription;
        Dictionary<string, bool> WriteableDict;
        Dictionary<string, bool> needQuote;

        private List<SystemPara> getSystemPara()
        {
            List<SystemPara> result = new List<SystemPara>();
            //try
            {
                foreach (XmlNode p in Parameter.glb_Parameter.system_parameter)
                {
                    try
                    {
                        SystemPara para = new SystemPara(p.Name, p.InnerText, "");
                        para.Name = cnNames[p.Name];
                        string des = "";
                        if (cnDescription.ContainsKey(p.Name)) cnDescription.TryGetValue(p.Name, out des);
                        para.Description = des;
                        bool writeable = true;
                        if (WriteableDict.ContainsKey(p.Name)) WriteableDict.TryGetValue(p.Name, out writeable);
                        para.Writeable = writeable;
                        if (p.InnerText.StartsWith("\"") && p.InnerText.EndsWith("\""))
                        {
                            needQuote[p.Name] = true;
                            para.Value = para.Value.Substring(1, para.Value.Length - 2);
                        }
                        if (des != "") result.Add(para);
                    }
                    catch (Exception e)
                    {
                        //SystemPara para = new SystemPara(p.Name, p.InnerText, "");
                        //para.Name = p.Name;
                        //result.Add(para);
                    }
                }
            }
            //catch (Exception e)
            //{
            //    Xceed.Wpf.Toolkit.MessageBox.Show("打开文档失败！\r\n" + e.Message);
            //}
            return result;
        }

        private void setSystemPara(List<SystemPara> paras)
        {
            Parameter.glb_Parameter.DoNotSave = true;
            foreach (var p in paras)
            {
                Parameter.glb_Parameter.WriteSystemPara(p.EnglishName, needQuote.ContainsKey(p.EnglishName)? "\"" + p.Value + "\"" : p.Value);
            }
            Parameter.glb_Parameter.DoNotSave = false;
            Parameter.glb_Parameter.Save();
        }

        private List<SystemPara> getVisonPara()
        {
            List<SystemPara> result = new List<SystemPara>();
            //try
            {
                foreach (XmlNode p in Parameter.glb_Parameter.vision_machine_datas_list)
                {
                    try
                    {
                        SystemPara para = new SystemPara(p.Name, p.InnerText, "");
                        para.Name = cnNames[p.Name];
                        string des = "";

                        if (cnDescription.ContainsKey(p.Name)) cnDescription.TryGetValue(p.Name, out des);
                        para.Description = des;

                        bool writeable = true;
                        if (WriteableDict.ContainsKey(p.Name)) WriteableDict.TryGetValue(p.Name, out writeable);
                        para.Writeable = writeable;
                        if (p.InnerText.StartsWith("\"") && p.InnerText.EndsWith("\""))
                        {
                            needQuote[p.Name] = true;
                            para.Value = para.Value.Substring(1, para.Value.Length - 2);
                        }
                        if (des != "") result.Add(para);
                    }
                    catch(Exception e)
                    {
                        //SystemPara para = new SystemPara(p.Name, p.InnerText, "");
                        //para.Name = p.Name;
                        //result.Add(para);
                    }

                }
            }
            //catch (Exception e)
            //{
            //    Xceed.Wpf.Toolkit.MessageBox.Show("打开文档失败！\r\n" + e.Message);
            //}
            return result;
        }

        private void setVisionPara(List<SystemPara> paras)
        {
            Parameter.glb_Parameter.DoNotSave = true;
            foreach (var p in paras)
            {
                Parameter.glb_Parameter.WriteVisionPara(p.EnglishName, needQuote.ContainsKey(p.EnglishName) ? "\"" + p.Value + "\"" : p.Value);
            }
            Parameter.glb_Parameter.DoNotSave = false;
            Parameter.glb_Parameter.Save();
        }

        private List<SystemPara> getLaserPara()
        {
            List<SystemPara> result = new List<SystemPara>();
            foreach (XmlNode p in Parameter.glb_Parameter.laser_machine_datas_list)
            {
                try
                {
                    SystemPara para = new SystemPara(p.Name, p.InnerText, "");
                    para.Name = cnNames[p.Name];
                    string des = "";
                    if (cnDescription.ContainsKey(p.Name)) cnDescription.TryGetValue(p.Name, out des);
                    para.Description = des;
                    bool writeable = true;
                    if (WriteableDict.ContainsKey(p.Name)) WriteableDict.TryGetValue(p.Name, out writeable);
                    para.Writeable = writeable;
                    if (p.InnerText.StartsWith("\"") && p.InnerText.EndsWith("\""))
                    {
                        needQuote[p.Name] = true;
                        para.Value = para.Value.Substring(1, para.Value.Length - 2);
                    }
                    if (des != "") result.Add(para);
                }
                catch (Exception e)
                {
                    //SystemPara para = new SystemPara(p.Name, p.InnerText, "");
                    //para.Name = p.Name;
                    //result.Add(para);
                }
            }
            return result;
        }

        private void setLaserPara(List<SystemPara> paras)
        {
            Parameter.glb_Parameter.DoNotSave = true;
            foreach (var p in paras)
            {
                Parameter.glb_Parameter.WriteLaserPara(p.EnglishName, needQuote.ContainsKey(p.EnglishName) ? "\"" + p.Value + "\"" : p.Value);
            }
            Parameter.glb_Parameter.DoNotSave = false;
            Parameter.glb_Parameter.Save();
        }

        public SystemParameter()
        {
            InitializeComponent();
            SystemParameter_Start();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            cnNames = new Dictionary<string, string>();
            cnDescription = new Dictionary<string, string>();
            WriteableDict = new Dictionary<string, bool>();
            needQuote = new Dictionary<string, bool>();
            string[] dict = File.ReadAllLines("ParaDict.txt",Encoding.Default);
            foreach (var d in dict)
            {
                string[] pair = d.Split('\t');
                cnNames.Add(pair[0], pair[1]);
                if (pair.Length >= 3)
                {
                    cnDescription.Add(pair[0], pair[2]);
                }
                if (pair.Length == 4)
                {
                    WriteableDict.Add(pair[0], pair[3]=="true");
                }
            }
            VisionParaDataGrid.ItemsSource = getVisonPara();
            SystemParaDataGrid.ItemsSource = getSystemPara();
            LaserParaDataGrid.ItemsSource = getLaserPara();
        }

        private void ApplyButton_Click(object sender, RoutedEventArgs e)
        {
            setSystemPara(SystemParaDataGrid.ItemsSource as List<SystemPara>);
            setVisionPara(VisionParaDataGrid.ItemsSource as List<SystemPara>);
            setLaserPara(LaserParaDataGrid.ItemsSource as List<SystemPara>);
        }

        private void SubmitButton_Click(object sender, RoutedEventArgs e)
        {
            setSystemPara(SystemParaDataGrid.ItemsSource as List<SystemPara>);
            setVisionPara(VisionParaDataGrid.ItemsSource as List<SystemPara>);
            setLaserPara(LaserParaDataGrid.ItemsSource as List<SystemPara>);
            this.Close();
        }

        private void CancelButton_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void VisionParaDataGrid_RowEditEnding(object sender, DataGridRowEditEndingEventArgs e)
        {
            ApplyButton.IsEnabled = true;
        }
        private void SlanguageChange(CultureInfo Culture)
        {
            List<ResourceDictionary> dictionaryList = new List<ResourceDictionary>();
            foreach (ResourceDictionary dictionary in Application.Current.Resources.MergedDictionaries)
            {
                dictionaryList.Add(dictionary);
            }
            string requestedCulture = string.Format(@"Resources\StringResource.{0}.xaml", Culture);
            ResourceDictionary resourceDictionary = dictionaryList.FirstOrDefault(d => d.Source.OriginalString.Equals(requestedCulture));
            if (resourceDictionary == null)
            {
                requestedCulture = @"Resources\StringResource.xaml";
                resourceDictionary = dictionaryList.FirstOrDefault(d => d.Source.OriginalString.Equals(requestedCulture));
            }
            if (resourceDictionary != null)
            {
                Application.Current.Resources.MergedDictionaries.Remove(resourceDictionary);
                Application.Current.Resources.MergedDictionaries.Add(resourceDictionary);
            }
        }
        private void Slanguage(CultureInfo Culture)
        {
            ci = new CultureInfo("zh-CN");
            Thread.CurrentThread.CurrentCulture = ci;
            Thread.CurrentThread.CurrentUICulture = ci;
            SlanguageChange(ci);
        }
        void SystemParameter_Start()
        {
            bool ret;
            mutex = new System.Threading.Mutex(true, "WpfMuerterterterttex", out ret);
            if (!ret)
            {
                MessageBoxResult result = Xceed.Wpf.Toolkit.MessageBox.Show("SystemParameter window is opening!");
                this.Close();
            }
        }

        private void LaserParaDataGrid_RowEditEnding(object sender, DataGridRowEditEndingEventArgs e)
        {

        }

        private void SystemParaDataGrid_LoadingRow(object sender, DataGridRowEventArgs e)
        {
            
        }
    }
}
