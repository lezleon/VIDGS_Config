using System;
using System.Collections.Generic;
using System.Text;

namespace Susi4.Libraries
{
    class Common
    {
        public static bool IsHex(char c)
        {
            if (Char.IsDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
            {
                return true;
            }

            return false;
        }

        public static byte[] StringToByteArray(string str)
        {
            int index = 0;
            byte[] baByte;

            try
            {
                str = str.Trim();

                while (str.IndexOf("  ", index) >= 0)
                {
                    str = str.Replace("  ", " ");
                }

                string[] saTemp = str.Split(new char[] { ' ' });
                baByte = new byte[saTemp.Length];

                for (int i = 0; i < saTemp.Length; i++)
                {
                    UInt32 dwTemp = Convert.ToUInt32(saTemp[i], 16);

                    if (dwTemp > 255)
                    {
                        return new byte[0];
                    }
                    baByte[i] = (byte)dwTemp;
                }
            }
            catch
            {
                return new byte[0];
            }

            return baByte;
        }

        public static string ByteArrayToHexString(byte[] bytes)
        {
            StringBuilder sb = new StringBuilder();

            for (int i = 0; i < bytes.Length; i++)
            {
                sb.Append(String.Format("{0:X2} ", bytes[i]));
            }

            return sb.ToString().TrimEnd();
        }
    }
}
