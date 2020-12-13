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
using Xceed.Wpf.Toolkit.PropertyGrid;
using Xceed.Wpf.Toolkit.PropertyGrid.Editors;

namespace CsharpConfig
{
    /// <summary>
    /// NewSystemParameter.xaml 的交互逻辑
    /// </summary>
    public partial class NewSystemParameter : Window
    {
        CultureInfo ci;
        System.Threading.Mutex mutex;
        
        Dictionary<string, string> cnNames;
        Dictionary<string, string> cnDescription;

        public static readonly DependencyProperty StringProperty = DependencyProperty.Register("Value", typeof(string), typeof(TextBlockEditor));

        private PropertyDefinitionCollection getSystemPara()
        {
            PropertyDefinitionCollection result = new PropertyDefinitionCollection();
            foreach (XmlNode p in Parameter.glb_Parameter.system_parameter)
            {
                try
                {
                    PropertyDefinition para = new PropertyDefinition();
                    para.Category = "系统参数";
                    para.TargetProperties.Add(p.Name);
                    para.Description = "";
                    para.SetValue(StringProperty, p.InnerText);
                    para.DisplayName = cnNames[p.Name];
                    string des;
                    cnDescription.TryGetValue(p.Name, out des);
                    para.Description = des;
                    result.Add(para);
                    
                }
                catch (Exception e)
                {
                }
            }
            return result;
        }

        private void setSystemPara(PropertyDefinitionCollection paras)
        {
            Parameter.glb_Parameter.DoNotSave = true;
            foreach (var p in paras)
            {
                Parameter.glb_Parameter.WriteSystemPara(p.TargetProperties[0] as string, p.GetValue(StringProperty));
            }
            Parameter.glb_Parameter.DoNotSave = false;
            Parameter.glb_Parameter.Save();
        }

        private PropertyDefinitionCollection getVisonPara()
        {
            PropertyDefinitionCollection result = new PropertyDefinitionCollection();
            foreach (XmlNode p in Parameter.glb_Parameter.vision_machine_datas_list)
            {
                try
                {
                    PropertyDefinition para = new PropertyDefinition();
                    para.Category = "系统参数";
                    para.TargetProperties.Add(p.Name);
                    para.SetValue(StringProperty, p.InnerText);
                    para.DisplayName = cnNames[p.Name];
                    string des;
                    cnDescription.TryGetValue(p.Name, out des);
                    para.Description = des;
                    result.Add(para);

                }
                catch (Exception e)
                {
                }
            }
            return result;
        }

        private void setVisionPara(PropertyDefinitionCollection paras)
        {
            Parameter.glb_Parameter.DoNotSave = true;
            foreach (var p in paras)
            {
                Parameter.glb_Parameter.WriteSystemPara(p.TargetProperties[0] as string, p.GetValue(StringProperty));
            }
            Parameter.glb_Parameter.DoNotSave = false;
            Parameter.glb_Parameter.Save();
        }

        private PropertyDefinitionCollection getLaserPara()
        {
            PropertyDefinitionCollection result = new PropertyDefinitionCollection();
            foreach (XmlNode p in Parameter.glb_Parameter.laser_machine_datas_list)
            {
                try
                {
                    PropertyDefinition para = new PropertyDefinition();
                    para.Category = "系统参数";
                    para.TargetProperties.Add(p.Name);
                    para.SetValue(StringProperty, p.InnerText);
                    para.DisplayName = cnNames[p.Name];
                    string des;
                    cnDescription.TryGetValue(p.Name, out des);
                    para.Description = des;
                    result.Add(para);

                }
                catch (Exception e)
                {
                }
            }
            return result;
        }

        private void setLaserPara(PropertyDefinitionCollection paras)
        {
            Parameter.glb_Parameter.DoNotSave = true;
            foreach (var p in paras)
            {
                Parameter.glb_Parameter.WriteSystemPara(p.TargetProperties[0] as string, p.GetValue(StringProperty));
            }
            Parameter.glb_Parameter.DoNotSave = false;
            Parameter.glb_Parameter.Save();
        }

        public NewSystemParameter()
        {
            InitializeComponent();
            NewSystemParameter_Start();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            cnNames = new Dictionary<string, string>();
            cnDescription = new Dictionary<string, string>();
            string[] dict = File.ReadAllLines("ParaDict.txt",Encoding.Default);
            foreach (var d in dict)
            {
                string[] pair = d.Split('\t');
                cnNames.Add(pair[0], pair[1]);
                if (pair.Length == 3)
                {
                    cnDescription.Add(pair[0], pair[2]);
                }
            }
            SystemPropertyGrid.PropertyDefinitions = getSystemPara();
            SystemPropertyGrid.Update();
            VisionPropertyGrid.PropertyDefinitions = getVisonPara();
            LaserPropertyGrid.PropertyDefinitions = getLaserPara() ;
        }

        private void ApplyButton_Click(object sender, RoutedEventArgs e)
        {
            setSystemPara(SystemPropertyGrid.PropertyDefinitions);
            setVisionPara(VisionPropertyGrid.PropertyDefinitions);
            setLaserPara(LaserPropertyGrid.PropertyDefinitions);
        }

        private void SubmitButton_Click(object sender, RoutedEventArgs e)
        {
            setSystemPara(SystemPropertyGrid.PropertyDefinitions);
            setVisionPara(VisionPropertyGrid.PropertyDefinitions);
            setLaserPara(LaserPropertyGrid.PropertyDefinitions);
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
        void NewSystemParameter_Start()
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
    }
}
