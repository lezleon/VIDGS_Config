using System;
using System.Text;
using System.ComponentModel;
using System.Collections.ObjectModel;
using System.IO;
using System.Collections.Generic;
using System.Xml.Serialization;
using System.Globalization;

namespace PD
{
    public class ContentChangedEventArgs : EventArgs
    {
        private singleplane newPlane;

        public singleplane NewPlane
        {
            get { return newPlane; }
        }

        public ContentChangedEventArgs(singleplane np)
        {
            this.newPlane = np;
        }
    }

    public class singleplane : INotifyPropertyChanged, IEquatable<singleplane>
    {
        public singleplane(string ttype, string ccabinwidth, string wwingspan,
            string hheadheight, string hheadwheelhoridis, string eenginecenterdis, string eengineradius,
            string eengineheadhordis, string ccenterfloordis, string wwheelwidth, string wwheelheight)
        {
            this.Type = ttype;
            this.CabinWidth = ccabinwidth;
            this.WingSpan = wwingspan;
            this.HeadHeight = hheadheight;
            this.HeadWheelHoridis = hheadwheelhoridis;
            this.EngineCenterdis = eenginecenterdis;
            this.EngineRadius = eengineradius;
            this.EngineHeadhordis = eengineheadhordis;
            this.CenterFloordis = ccenterfloordis;
            this.WheelWidth = wwheelwidth;
            this.WheelHeight = wwheelheight;
            this.isdirty = false;
        }
        private string type;
        private string cabinwidth;
        private string wingspan;
        private string headheight;
        private string headwheelhoridis;
        private string enginecenterdis;
        private string engineradius;
        private string engineheadhordis;
        private string centerfloordis;
        private string wheelwidth;
        private string wheelheight;
        private bool dirty;
        public bool isdirty
        {
            get { return dirty; }
            set
            {
                if (dirty != value)
                {
                    dirty = value;
                }
            }
        }
        public string Type
        {
            get { return type; }
            set
            {
                if (type != value)
                {
                    type = value;
                    isdirty = true;
                }
            }
        }
        public string CabinWidth
        {
            get { return cabinwidth; }
            set
            {
                if (cabinwidth != value)
                {
                    cabinwidth = value;
                    isdirty = true;
                }
            }
        }
        public string WingSpan
        {
            get { return wingspan; }
            set
            {
                if (wingspan != value)
                {
                    wingspan = value;
                    isdirty = true;
                }
            }
        }
        public string HeadHeight
        {
            get { return headheight; }
            set
            {
                if (headheight != value)
                {
                    headheight = value;
                    isdirty = true;
                }
            }
        }
        public string HeadWheelHoridis
        {
            get { return headwheelhoridis; }
            set
            {
                if (headwheelhoridis != value)
                {
                    headwheelhoridis = value;
                    isdirty = true;
                }
            }
        }
        public string EngineCenterdis
        {
            get { return enginecenterdis; }
            set
            {
                if (enginecenterdis != value)
                {
                    enginecenterdis = value;
                    isdirty = true;
                }
            }
        }
        public string EngineRadius
        {
            get { return engineradius; }
            set
            {
                if (engineradius != value)
                {
                    engineradius = value;
                    isdirty = true;
                }
            }
        }
        public string EngineHeadhordis
        {
            get { return engineheadhordis; }
            set
            {
                if (engineheadhordis != value)
                {
                    engineheadhordis = value;
                    isdirty = true;
                }
            }
        }
        public string CenterFloordis
        {
            get { return centerfloordis; }
            set
            {
                if (centerfloordis != value)
                {
                    centerfloordis = value;
                    isdirty = true;
                }
            }
        }
        public string WheelWidth
        {
            get { return wheelwidth; }
            set
            {
                if (wheelwidth != value)
                {
                    wheelwidth = value;
                    isdirty = true;
                }
            }
        }
        public string WheelHeight
        {
            get { return wheelheight; }
            set
            {
                if (wheelheight != value)
                {
                    wheelheight = value;
                    isdirty = true;
                }
            }
        }
        public event PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        }
        public bool Equals(singleplane other)
        {
            return (this.Type == other.Type);
        }
    }

    public class planeCollection : ObservableCollection<singleplane>, INotifyPropertyChanged
    {
        public planeCollection() { }
        private singleplane current;
        private bool dirty;
        public bool IsDirty
        {
            get { return dirty; }
            set { dirty = value; }
        }
        public singleplane Current
        {
            get { return current; }
            set
            {
                if (current != value)
                {
                    current = value;
                    OnPropertyChanged("Current");
                    OnCurrentChanged();
                }
            }
        }
        public event EventHandler CurrentChanged;
        protected void OnCurrentChanged()
        {
            if (CurrentChanged != null)
                CurrentChanged(this, EventArgs.Empty);
        }
        protected override event PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        }
        public event EventHandler<ContentChangedEventArgs> ContentChanged;
        public void OnContentChanged()
        {
            dirty = true;
            if (ContentChanged != null)
                ContentChanged(this, new ContentChangedEventArgs(null));
        }
    }

    public class plane
    {
        private planeCollection plane_Collection;
        public planeCollection PlaneCollection
        {
            get { return plane_Collection; }
        }
        public plane()
        {
            plane_Collection = new planeCollection();
        }
        private string fullyQualifiedFilename;
        public string FullyQualifiedFilename
        {
            get { return fullyQualifiedFilename; }

            set { fullyQualifiedFilename = value; }
        }
        public void Load()
        {
            this.plane_Collection.IsDirty = false;
        }
    }
}
