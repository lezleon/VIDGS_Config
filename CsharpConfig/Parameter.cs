using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml;
using System.Windows;
//using System.Drawing;

namespace VIDGS配置软件
{
    
    public class Parameter
    {
        public string allText;
        public XmlDocument Doc;
        public string FileName;
        public string SavingFileName;
        public XmlNode vision_machine_datas_list;
        public XmlNode laser_machine_datas_list;
        public XmlNode system_parameter;
        public static Parameter glb_Parameter;
        public bool DoNotSave;

        public struct StopLine
        {
            Point Start;
            Point End;
            public string Name;
            public string GuideLineName;
            public List<string> planeTypes;
            public StopLine(Point start, Point end, string name, string guidelineName)
            {
                Start = start;
                End = end;
                Name = name;
                GuideLineName = guidelineName;
                planeTypes = new List<string>();
            }

        }

        public struct GuideLine
        {
            string Name;
            //Key = distance, Value = Point
            List<KeyValuePair<double, Point>> DistanceAndPoints;
            List<StopLine> StopLines;
            List<string> PlaneTypes;
        }

        public Parameter(string filename)
        {
            try
            {
                allText = File.ReadAllText(filename);
                Doc = new XmlDocument();
                Doc.Load(filename);
                FileName = filename;
                vision_machine_datas_list = Doc["opencv_storage"]["vision_machine_datas_list"];
                laser_machine_datas_list = Doc["opencv_storage"]["laser_machine_datas_list"];
                system_parameter = Doc["opencv_storage"]["system_parameter"];
                Doc.Save(FileName + ".bak");
                DoNotSave = false;
            }
            catch (Exception e)
            {
                MessageBox.Show("加载xml文档失败!错误原因：" + e.Message);
            }
        }

        public void Save()
        {
            if(!DoNotSave) Doc.Save(FileName);
        }

        public List<KeyValuePair<double, Point>> readAllPoints(string guideLineName)
        {
            List<KeyValuePair<double, Point>> result = new List<KeyValuePair<double, Point>>();
            XmlNode guideLineListNode = Doc["opencv_storage"]["VDisandPointslist"];
            XmlNode DistancePointListNode = null;
            foreach (XmlNode glNode in guideLineListNode)
            {
                if (glNode["Vectorname"].InnerText == guideLineName) DistancePointListNode = glNode["points"];
            }
            if (DistancePointListNode == null) return result;
            foreach (XmlNode dpNode in DistancePointListNode)
            {
                Point p = new Point(Convert.ToDouble(dpNode["x"].InnerText), Convert.ToDouble(dpNode["y"].InnerText));
                KeyValuePair<double, Point> dp = new KeyValuePair<double, Point>(Convert.ToDouble(dpNode["distance"].InnerText), p);
                result.Add(dp);
            }
            Comparison<KeyValuePair<double, Point>> dpCompare = delegate(KeyValuePair<double, Point> p1, KeyValuePair<double, Point> p2)
            {
                return (int)(p1.Key * 10 - p2.Key * 10);
            };
            result.Sort(dpCompare);
            return result;
        }
        public void ClearPoints()
        {
            XmlNode guideLineListNode = Doc["opencv_storage"]["VDisandPointslist"];
            guideLineListNode.InnerXml = "";
        }
        public void SavePoints(string guideLineName, KeyValuePair<double, Point> distanceAndPoint)
        {
            XmlNode guideLineListNode = Doc["opencv_storage"]["VDisandPointslist"];
            XmlNode DistancePointListNode = null;
            foreach (XmlNode glNode in guideLineListNode)
            {
                if (guideLineListNode.InnerText!=""&&glNode["Vectorname"].InnerText == guideLineName) DistancePointListNode = glNode["points"];
            }
            if (DistancePointListNode == null)
            {
                XmlElement dp = Doc.CreateElement("_");
                XmlElement vectorName = Doc.CreateElement("Vectorname");
                vectorName.InnerText = guideLineName;
                dp.AppendChild(vectorName);
                XmlElement points = Doc.CreateElement("points");
                dp.AppendChild(points);
                guideLineListNode.AppendChild(dp);
                DistancePointListNode = points;
            }
            bool exist = false;
            foreach (XmlNode dpNode in DistancePointListNode)
            {
                if (distanceAndPoint.Key == Convert.ToDouble(dpNode["distance"].InnerText))
                {
                    dpNode["x"].InnerText = distanceAndPoint.Value.X.ToString();
                    dpNode["y"].InnerText = distanceAndPoint.Value.Y.ToString();
                    exist = true;
                }
            }
            if (!exist)
            {
                XmlElement dp = Doc.CreateElement("_");
                XmlElement distance = Doc.CreateElement("distance");
                distance.InnerText = distanceAndPoint.Key.ToString();
                XmlElement x = Doc.CreateElement("x");
                x.InnerText = distanceAndPoint.Value.X.ToString();
                XmlElement y = Doc.CreateElement("y");
                y.InnerText = distanceAndPoint.Value.Y.ToString();
                dp.AppendChild(distance);
                dp.AppendChild(x);
                dp.AppendChild(y);
                DistancePointListNode.AppendChild(dp);
            }
            Save();
        }
        public List<StopLine> ReadStopLines()
        {
            List<StopLine> result = new List<StopLine>();
            XmlNode guideLineListNode = Doc["opencv_storage"]["Vstringslist"];
            XmlNode PlaneTypeListNode = null;
            foreach (XmlNode glNode in guideLineListNode)
            {
                string stopText = glNode["Vectorname"].InnerText;
                if (stopText.Substring(0, 9) == "PlaneType")
                {
                    PlaneTypeListNode = glNode["strings"];
                    StopLine s = new StopLine(new Point(0, 0), new Point(0, 0), stopText,
                        stopText.Substring(0, 10).Replace("PlaneType", "GuideLine"));
                    foreach (XmlNode ptNode in PlaneTypeListNode)
                    {
                        string t = ptNode["stringname"].InnerText;
                        if (t.StartsWith("stop") || t.StartsWith("guide")) continue;
                        s.planeTypes.Add(t);
                    }
                    result.Add(s);
                }
            }
            return result;
        }
        public List<string> ReadPlaneType(int guideLineIndex, int stopLineIndex)
        {
            string stopLineName = "PlaneType" + guideLineIndex.ToString() + stopLineIndex.ToString();
            List<string> result = new List<string>();
            XmlNode guideLineListNode = Doc["opencv_storage"]["Vstringslist"];
            XmlNode PlaneTypeListNode = null;
            foreach (XmlNode glNode in guideLineListNode)
            {
                if (glNode["Vectorname"].InnerText == stopLineName) PlaneTypeListNode = glNode["strings"];
            }
            if (PlaneTypeListNode == null) return result;
            foreach (XmlNode ptNode in PlaneTypeListNode)
            {
                result.Add(ptNode["stringname"].InnerText);
            }
            result.RemoveAt(0);
            result.RemoveAt(0);
            return result;
        }
        public void ClearPlaneType()
        {
            XmlNode guideLineListNode = Doc["opencv_storage"]["Vstringslist"];
            guideLineListNode.InnerXml = "";
        }
        public void SetPlaneType(int guideLineIndex, int stopLineIndex, List<string> PlaneTypes)
        {
            string stopLineName = "PlaneType" + guideLineIndex.ToString() + stopLineIndex.ToString();
            XmlNode guideLineListNode = Doc["opencv_storage"]["Vstringslist"];
            XmlNode PlaneTypeListNode = null;
            foreach (XmlNode glNode in guideLineListNode)
            {
                if (glNode.Value != ""&&glNode["Vectorname"].InnerText == stopLineName) PlaneTypeListNode = glNode;
            }
            XmlElement _Node = Doc.CreateElement("_");
            XmlElement Vectorname = Doc.CreateElement("Vectorname");
            Vectorname.InnerText = stopLineName;
            XmlElement strings = Doc.CreateElement("strings");
            XmlElement _subNode1 = Doc.CreateElement("_");
            XmlElement _subNode2 = Doc.CreateElement("_");
            XmlElement stringname1 = Doc.CreateElement("stringname");
            stringname1.InnerText = "guide" + guideLineIndex.ToString();
            _subNode1.AppendChild(stringname1);
            strings.AppendChild(_subNode1);
            XmlElement stringname2 = Doc.CreateElement("stringname");
            stringname2.InnerText = "stop" + stopLineIndex.ToString();
            _subNode2.AppendChild(stringname2);
            strings.AppendChild(_subNode2);
            foreach (string pt in PlaneTypes)
            {
                XmlElement _newSubNode = Doc.CreateElement("_");
                XmlElement newstringname = Doc.CreateElement("stringname");
                newstringname.InnerText = pt;
                _newSubNode.AppendChild(newstringname);
                strings.AppendChild(_newSubNode);
            }
            _Node.AppendChild(Vectorname);
            _Node.AppendChild(strings);
            if (PlaneTypeListNode == null)
            {
                guideLineListNode.AppendChild(_Node);
            }
            else
            {
                PlaneTypeListNode.InnerXml = _Node.InnerXml;
            }
            Save();
        }
        public string ReadXData(string dataName)
        {
            string result = null;
            XmlNode CategoryNode = Doc["opencv_storage"]["alonexlist"];
            foreach (XmlNode glNode in CategoryNode)
            {
                if (glNode["name"].InnerText == dataName) result = glNode["x"].InnerXml;
            }
            return result;
        }
        public void WriteXData(string dataName, string value)
        {
            string result = null;
            XmlNode CategoryNode = Doc["opencv_storage"]["alonexlist"];
            XmlNode DataNode = null;
            foreach (XmlNode glNode in CategoryNode)
            {
                if (glNode["name"].InnerText == dataName) DataNode = glNode["x"];
            }
            if (DataNode == null)
            {
                XmlElement _Node = Doc.CreateElement("_");
                XmlElement nameNode = Doc.CreateElement("name");
                nameNode.InnerText = dataName;
                XmlElement xNode = Doc.CreateElement("x");
                xNode.InnerText = value;
                _Node.AppendChild(nameNode);
                _Node.AppendChild(xNode);
                CategoryNode.AppendChild(_Node);
            }
            else
            {
                DataNode.InnerXml = value;
            }
            Save();
        }
        public void ClearRectData()
        {
            XmlNode CategoryNode = Doc["opencv_storage"]["rectlist"];
            CategoryNode.InnerXml = "";
        }
        public int[] ReadRectData(string rectName)
        {
            int[] result = new int[4];
            XmlNode CategoryNode = Doc["opencv_storage"]["rectlist"];
            XmlNode DataNode = null;
            foreach (XmlNode glNode in CategoryNode)
            {
                if (CategoryNode.InnerText != "" && glNode["name"].InnerText == rectName)
                {
                    result[0] = Convert.ToInt32(glNode["x"].InnerText);
                    result[1] = Convert.ToInt32(glNode["y"].InnerText);
                    result[2] = Convert.ToInt32(glNode["height"].InnerText);
                    result[3] = Convert.ToInt32(glNode["width"].InnerText);
                }
            }
            return result;
        }
        public void WriteRectData(string rectName, int[] rect)
        {
            XmlNode CategoryNode = Doc["opencv_storage"]["rectlist"];
            bool exist = false;
            foreach (XmlNode glNode in CategoryNode)
            {
                if (CategoryNode.InnerText!=""&&glNode["name"].InnerText == rectName)
                {
                    exist = true;
                    glNode["x"].InnerText = rect[0].ToString();
                    glNode["y"].InnerText = rect[1].ToString();
                    glNode["height"].InnerText = rect[2].ToString();
                    glNode["width"].InnerText = rect[3].ToString();
                }
            }
            if (!exist)
            {
                XmlElement _Node = Doc.CreateElement("_");
                XmlElement nameNode = Doc.CreateElement("name");
                nameNode.InnerText = rectName;
                XmlElement xNode = Doc.CreateElement("x");
                xNode.InnerText = rect[0].ToString();
                XmlElement yNode = Doc.CreateElement("y");
                yNode.InnerText = rect[1].ToString();
                XmlElement heightNode = Doc.CreateElement("height");
                heightNode.InnerText = rect[2].ToString();
                XmlElement widthNode = Doc.CreateElement("width");
                widthNode.InnerText = rect[3].ToString();
                _Node.AppendChild(nameNode);
                _Node.AppendChild(xNode);
                _Node.AppendChild(yNode);
                _Node.AppendChild(heightNode);
                _Node.AppendChild(widthNode);
                CategoryNode.AppendChild(_Node);
            }
            Save();
        }
        public void ClearPointData()
        {
            XmlNode CategoryNode = Doc["opencv_storage"]["pointlist"];
            CategoryNode.InnerXml = "";
        }
        public Point ReadPointData(string pointName)
        {
            Point result = new Point();
            XmlNode CategoryNode = Doc["opencv_storage"]["pointlist"];
            XmlNode DataNode = null;
            foreach (XmlNode glNode in CategoryNode)
            {
                if (CategoryNode.InnerText!=""&&glNode["name"].InnerText == pointName)
                {
                    result.X = Convert.ToInt32(glNode["x"].InnerText);
                    result.Y = Convert.ToInt32(glNode["y"].InnerText);
                }
            }
            return result;
        }
        public List<KeyValuePair<String, Point>> ReadPointDataList()
        {
            List<KeyValuePair<String, Point>> result = new List<KeyValuePair<string,Point>>();
            XmlNode CategoryNode = Doc["opencv_storage"]["pointlist"];
            if (CategoryNode.InnerText != "")
            {
                foreach (XmlNode glNode in CategoryNode)
                {
                    result.Add(new KeyValuePair<string, Point>(glNode["name"].InnerText,
                        new Point(Convert.ToDouble(glNode["x"].InnerText), Convert.ToDouble(glNode["y"].InnerText))));
                }
            }
            return result;
        }
        public void WritePointData(string pointName, Point point)
        {
            XmlNode CategoryNode = Doc["opencv_storage"]["pointlist"];
            bool exist = false;
            foreach (XmlNode glNode in CategoryNode)
            {
                if (CategoryNode.InnerText != "" && glNode["name"].InnerText == pointName)
                {
                    exist = true;
                    glNode["x"].InnerText = point.X.ToString();
                    glNode["y"].InnerText = point.Y.ToString();
                }
            }
            if (!exist)
            {
                XmlElement _Node = Doc.CreateElement("_");
                XmlElement nameNode = Doc.CreateElement("name");
                nameNode.InnerText = pointName;
                XmlElement xNode = Doc.CreateElement("x");
                xNode.InnerText = point.X.ToString();
                XmlElement yNode = Doc.CreateElement("y");
                yNode.InnerText = point.Y.ToString();
                _Node.AppendChild(nameNode);
                _Node.AppendChild(xNode);
                _Node.AppendChild(yNode);
                CategoryNode.AppendChild(_Node);
            }
            Save();
        }
        public string ReadVisionPara(string paraName)
        {
            string result = vision_machine_datas_list[paraName].InnerText;
            if (result[0] == '\"') result = result.Substring(1, result.Length - 2);
            return result;
        }
        public void ClearLaserData()
        {
            XmlNode guideLineListNode = Doc["opencv_storage"]["laser_datas_list"];
            guideLineListNode.InnerXml = "";
        }
        //  <install_high>5181.</install_high>
        //<object_dis>40000.</object_dis>
        //<guide_line_k>1.6323629760742187e+002</guide_line_k>
        //<guide_line_b>256012.</guide_line_b>
        //<guide_line_x>-1318.</guide_line_x>
        //<guide_line_y>40734.</guide_line_y>
        //<HrzConfirmSteps>70.</HrzConfirmSteps>
        //<VertConfirmSteps>91.</VertConfirmSteps>
        public XmlNode ReadLaserData(int guideLineIndex, int stopLineIndex)
        {
            string stopLineName = "Laser" + guideLineIndex.ToString() + stopLineIndex.ToString();
            XmlNode guideLineListNode = Doc["opencv_storage"]["laser_datas_list"];
            XmlNode LaserGuideLineNode = null;
            foreach (XmlNode glNode in guideLineListNode)
            {
                if (guideLineListNode.InnerText!=""&&glNode["guideline_stopline_name"].InnerText == stopLineName) LaserGuideLineNode = glNode;
            }
            return LaserGuideLineNode;
        }
        public XmlNode ReadLaserData(char guideLineIndex, char stopLineIndex)
        {
            string stopLineName = "Laser" + guideLineIndex + stopLineIndex;
            XmlNode guideLineListNode = Doc["opencv_storage"]["laser_datas_list"];
            XmlNode LaserGuideLineNode = null;
            foreach (XmlNode glNode in guideLineListNode)
            {
                if (guideLineListNode.InnerText != "" && glNode["guideline_stopline_name"].InnerText == stopLineName) LaserGuideLineNode = glNode;
            }
            return LaserGuideLineNode;
        }
        public void SaveObjData(string GS, int Distance)
        {
            string stopLineName = "Laser" + GS;
            XmlNode guideLineListNode = Doc["opencv_storage"]["laser_datas_list"];
            foreach (XmlNode glNode in guideLineListNode)
            {
                if (guideLineListNode.InnerText != "" && glNode["guideline_stopline_name"].InnerText == stopLineName)
                    glNode["object_dis"].InnerText = Distance.ToString() + ".";
            }
            Save();
        }
        public void WriteLaserData(int guideLineIndex, int stopLineIndex, XmlNode newData)
        {
            string stopLineName = "Laser" + guideLineIndex.ToString() + stopLineIndex.ToString();
            if (newData["guideline_stopline_name"].InnerText != stopLineName) newData["guideline_stopline_name"].InnerText = stopLineName;
            XmlNode guideLineListNode = Doc["opencv_storage"]["laser_datas_list"];
            XmlNode LaserGuideLineNode = null;
            foreach (XmlNode glNode in guideLineListNode)
            {
                if (guideLineListNode.InnerText != "" && glNode["guideline_stopline_name"].InnerText == stopLineName) LaserGuideLineNode = glNode;
            }
            if (LaserGuideLineNode == null)
            {
                guideLineListNode.AppendChild(newData);
            }
            else
            {
                LaserGuideLineNode.InnerXml = newData.InnerXml;
            }
            Save();
        }

        public string ReadSystemPara(string paraName)
        {
            string result = system_parameter[paraName].InnerText;
            if (result[0] == '\"') result = result.Substring(1, result.Length - 2);
            return result;
        }

        public void WriteVisionPara(string paraName, object value)
        {
            vision_machine_datas_list[paraName].InnerText = value.ToString();
            Save();
        }

        public void WriteLaserPara(string paraName, object value)
        {
            laser_machine_datas_list[paraName].InnerText = value.ToString();
            Save();
        }

        public void WriteSystemPara(string paraName, object value)
        {
            system_parameter[paraName].InnerText = value.ToString();
            Save();
        }

    }
}
