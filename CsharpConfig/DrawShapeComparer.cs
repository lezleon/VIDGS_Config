using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace VIDGS配置软件
{
    class DrawShapeComparer : IEqualityComparer<DrawShape>
    {
         public bool Equals(DrawShape x, DrawShape y)
        {
            return x.prompt == y.prompt;
        }

         public int GetHashCode(DrawShape obj)
         {
             return obj.prompt.GetHashCode();
         }
    }
}
