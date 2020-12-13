using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Navigation;
using System.Threading;
using System.Runtime.InteropServices;
using System.Drawing;
using System.Drawing.Imaging;
using PvNET;

namespace VIDGS配置软件
{
    public class Camera
    {
        public tCamera thisCamera;
        tLinkCallback lLinkCB;
        public static bool Started = false;
        public static double illumiance = 0;
        public static UInt32 exposuretime = 1000;
        public static UInt32 gain = 0;
        public const int lowIlluminance = 70;
        public const int highIlluminance = 130;
        public static Byte[] NightLUT;
        public static Byte[] NoonLUT;
        public static bool isNight = false;
        public static bool isNoon = false;
        public bool Inited = false;
        public static string err_string = "";
        public static bool Connected = false;
        public static UInt32 Camera_UniqueId;

        // Place holder for all the data being used.
        public struct tCamera
        {
            public UInt32 UID;
            public UInt32 Handle;
            public tFrame Frame;
            public GCHandle GC;
            public byte[] Buffer;
            public string Serial;
        };

        public Bitmap getFrame()
        {
            if (!Started) return null;
            //try
            //{
            //System.Windows.MessageBox.Show(string.Format("图像宽：{0}，高:{1}", (int)GCamera.Frame.Width, (int)GCamera.Frame.Height));
            int width = (int)GCamera.Frame.Width;
            int height = (int)GCamera.Frame.Height;
            if (width == 0 || height == 0) return null;
            Bitmap lBitmap = new Bitmap(width, height, PixelFormat.Format24bppRgb);
            Rectangle lRect = new Rectangle(new Point(0, 0), new Size((int)GCamera.Frame.Width, (int)GCamera.Frame.Height));
            BitmapData lData = lBitmap.LockBits(lRect, ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb);

            if (Frame2Data(ref GCamera, ref lData))
            {
                lBitmap.UnlockBits(lData);
                return (Bitmap)lBitmap.Clone();
            }
            else
            {
                return null;
                //throw new Exception("Get frame fail!");
            }
            //}
            //catch (Exception e)
            //{
            //    System.Windows.MessageBox.Show("图像获取失败。");
            //    return null;
            //}
        }

        static void YUV2RGB(int y, int u, int v, ref int r, ref int g, ref int b)
        {
            // u and v are +-0.5.
            u -= 128;
            v -= 128;

            // Conversion (clamped to 0.255).
            r = Math.Min(Math.Max(0, (int)(y + 1.370705 * (float)v)), 255);
            g = Math.Min(Math.Max(0, (int)(y - 0.698001 * (float)v - 0.337633 * (float)u)), 255);
            b = Math.Min(Math.Max(0, (int)(y + 1.732446 * (float)u)), 255);
        }


        // Frame callback dummy. 
        static void FrameDummyCB(IntPtr pFrame)
        {
        }


        static private tFrameCallback FrameCB = FrameDoneCB;
        static private bool Close = false;
        // Global camera.
        static tCamera GCamera = new tCamera();


        // Allows for time to be kept.
        [DllImport("kernel32.dll", CharSet = CharSet.Auto, ExactSpelling = true)]
        public static extern int GetTickCount();


        // Keeps track of data being streamed from the camera.
        static void ThreadFunc()
        {
            tErr Err;
            UInt32 Dropped, Done, Completed;
            UInt32 Total, Missed, Errs;
            int Before, Now, Elapsed;
            double Fps;
            float Rate;

            Dropped = 0;
            Missed = 0;
            Completed = 0;
            Errs = 0;
            Rate = 0;
            Fps = 0;
            Elapsed = 0;
            Total = 0;
            Done = 0;

            // Start timing.
            Before = GetTickCount();

            // Get some infomation about the streaming process. 
            while (Close == false && ((Err = Pv.AttrUint32Get(GCamera.Handle, "StatFramesCompleted", ref Completed)) == 0)
                && ((Err = Pv.AttrUint32Get(GCamera.Handle, "StatFramesDropped", ref Dropped)) == 0)
                && ((Err = Pv.AttrUint32Get(GCamera.Handle, "StatPacketsMissed", ref Missed)) == 0)
                && ((Err = Pv.AttrUint32Get(GCamera.Handle, "StatPacketsErroneous", ref Errs)) == 0)
                && ((Err = Pv.AttrFloat32Get(GCamera.Handle, "StatFrameRate", ref Rate)) == 0))
            {


                // Store the elapsed time. 
                Now = GetTickCount();

                // Keep track of the total amount of frames completed.
                Total += (Completed - Done);

                // Keep track of the total time that has elapsed.
                Elapsed += (Now - Before);

                // Updates the Fps rate every 500 milliseconds.
                if (Elapsed >= 500)
                {
                    Fps = (double)(Total * 1000.0) / (double)Elapsed;
                    Elapsed = 0;
                    Total = 0;
                }

                // Display the current infomation.
                //Console.WriteLine("Completed : {0} Dropped : {1} Missed : {2} Err {3} Rate: {4:.00} <{5:.00}>", Completed, Dropped, Missed, Errs, Rate, Fps);

                Before = GetTickCount();
                Done = Completed;
                Thread.Sleep(20);
            }
            Console.WriteLine();
        }


        // Spawn a new thread.
        static void SpawnThread()
        {
            Thread Thread = new Thread(new ThreadStart(ThreadFunc));
            // Start the thread.
            Thread.Start();
        }


        // Callbacks called when the camera is plugged/unplugged.
        static void LinkCB(IntPtr Context, tInterface Interface, tLinkEvent Event, UInt32 UniqueId)
        {
            switch (Event)
            {
                case tLinkEvent.eLinkAdd:
                    {
                        System.Console.WriteLine("Camera {0} has been plugged in.", UniqueId);
                        Camera_UniqueId = UniqueId;
                        //HandleCameraPlugged(UniqueId);
                        Connected = true;
                        break;
                    }
                case tLinkEvent.eLinkRemove:
                    {
                        System.Console.WriteLine("Camera {0} has been unplugged.", UniqueId);
                        HandleCameraUnplugged(UniqueId);
                        Connected = false;
                        err_string = "相机已断开！";
                        break;
                    }
            }
        }


        // Action taken if the camera is unplugged.
        public static void HandleCameraUnplugged(UInt32 UniqueId)
        {
            if (GCamera.UID == UniqueId)
            {
                CameraStop();
                CameraClose();


                GCamera.Handle = 0;
                GCamera.UID = 0;
                Close = true;
            }
        }


        // Action taken if the camera is plugged in. 
        public static void HandleCameraPlugged(UInt32 UniqueId)
        {
            Started = false;
            if (GCamera.UID == 0)
            {
                GCamera.UID = UniqueId;

                if (CameraGrab())
                {
                    // Open the camera.
                    if (CameraSetup())
                    {
                        //System.Windows.MessageBox.Show(string.Format("Camera {0} is now open.", UniqueId));

                        // Start streaming from the camera.
                        if (CameraStart())
                        {
                            //System.Windows.MessageBox.Show("Camera is now streaming.Press 'X' to terminate, or unplug the camera.");
                            Started = true;
                            // Spawn the thread while the camera is streaming.
                            SpawnThread();
                        }
                        else
                        {
                            System.Windows.MessageBox.Show("The camera failed to start streaming.");
                            err_string = "The camera failed to start streaming.";
                        }
                    }
                    else
                    {
                        System.Windows.MessageBox.Show("The camera failed to be setup.");
                        err_string = "The camera failed to start streaming.";
                    }
                }
                else
                {
                    System.Windows.MessageBox.Show("The camera was not grabbed.");
                    err_string = "The camera was not grabbed.";
                }
            }
        }


        // Callback for when the frame is completed.
        static void FrameDoneCB(IntPtr pFrame)
        {
            if (!Started) return;
            // Marshal the pointer into a frame structure.
            tFrame Frame = (tFrame)Marshal.PtrToStructure(pFrame, typeof(tFrame));
            GCamera.Frame = Frame;

            // If the frame was completed (or if data were missing/lost), re-enqueue it.
            if (Frame.Status == tErr.eErrSuccess ||
                Frame.Status == tErr.eErrDataLost ||
                Frame.Status == tErr.eErrDataMissing)

                Pv.CaptureQueueFrame(GCamera.Handle, pFrame, FrameCB);
        }


        // Get the UID  and Serial number of the first camera listed.
        static bool CameraGrab()
        {
            UInt32 count, connected = 0;
            tCameraInfo[] list = new tCameraInfo[1];

            if ((count = Pv.CameraList(list, 1, ref connected)) != 0)
            {
                GCamera.UID = list[0].UniqueId;
                GCamera.Serial = list[0].SerialString;
                Console.WriteLine("Camera {0} grabbed. ", GCamera.Serial);
                return true;
            }
            else
                return false;
        }


        // Close the camera.
        static void CameraClose()
        {
            // Clear the queue.
            Console.WriteLine("Clearing the queue...");
            Pv.CaptureQueueClear(GCamera.Handle);
            // close the camera
            Console.WriteLine("The camera is now closed.");
            Pv.CameraClose(GCamera.Handle);
        }


        // Setup the camera up for streaming.
        static bool CameraStart()
        {

            UInt32 FrameSize = 0;

            // Adjust packet size for optimal performance. 
            Pv.CaptureAdjustPacketSize(GCamera.Handle, 8228);

            // Determines how big the frame buffers should be.
            if (Pv.AttrUint32Get(GCamera.Handle, "TotalBytesPerFrame", ref FrameSize) == 0)
            {

                tFrame Frame = new tFrame();
                byte[] Buffer = new byte[FrameSize];
                GCHandle pBuffer = GCHandle.Alloc(Buffer, GCHandleType.Pinned);

                // Set the frame's fields.
                // Handle to the Camera.
                Frame.Context.Field0 = new IntPtr(GCamera.Handle);
                // Address of the pinned object.		
                Frame.ImageBuffer = pBuffer.AddrOfPinnedObject();
                // Buffer size.
                Frame.ImageBufferSize = FrameSize;

                // Start the capture mode.
                if (Pv.CaptureStart(GCamera.Handle) == 0)
                {
                    // Set the camera in continuous acquisition mode,and in "Freerun".
                    if (Pv.AttrEnumSet(GCamera.Handle, "FrameStartTriggerMode", "Freerun") == 0)
                    {
                        // Set the acquisition mode into continuous.
                        if (Pv.CommandRun(GCamera.Handle, "AcquisitionStart") != 0)
                        {
                            // If that fails, reset the camera to non-capture mode.
                            Pv.CaptureEnd(GCamera.Handle);

                            Console.WriteLine("Failed to start.");
                            return false;
                        }
                        else
                        {
                            // Pin down a copy of the frame structure.
                            GCHandle pFrame = GCHandle.Alloc(Frame, GCHandleType.Pinned);

                            // Enqueue the frame.
                            Pv.CaptureQueueFrame(GCamera.Handle, pFrame.AddrOfPinnedObject(), FrameCB);

                            return true;
                        }
                    }
                    else
                        return false;
                }
                else
                    return false;
            }
            else
                return false;
        }


        // Stop the camera from streaming.
        static void CameraStop()
        {
            Console.WriteLine("The streaming is now being stopped.");
            Pv.CommandRun(GCamera.Handle, "AcquisitionStop");
            Pv.CaptureEnd(GCamera.Handle);
            // Clear the queue.
            Pv.CaptureQueueClear(GCamera.Handle);
        }


        // Open the camera.
        static bool CameraSetup()
        {
            return Pv.CameraOpen(GCamera.UID, tAccessFlags.eAccessMaster, out GCamera.Handle) == 0;
        }

        // Convert the raw data in the frame's buffer into the bitmap's data, this method doesn't support 
        // the following Pixel formats: eFmtRgb48, eFmtYuv411 and eFmtYuv444.
        static unsafe bool Frame2Data(ref tCamera Camera, ref BitmapData Data)
        {
            double PixelMean = 0;
            if (NightLUT == null || NightLUT.Length == 0)
            {
                NightLUT = new Byte[256];
                for (int i = 0; i < 256; i++)
                {
                    double t = i + 0.01 * i * (255 - i);
                    if (t > 255) t = 255;
                    if (t < 0) t = 0;
                    NightLUT[i] = (byte)t;
                }
            }
            if (NoonLUT == null || NoonLUT.Length == 0)
            {
                NoonLUT = new Byte[256];
                double gamma_r = 1.5;
                double gamma_b = 2;
                double gamma_c = 0.9;
                for (int i = 0; i < 256; i++)
                {
                    double t = (gamma_c * Math.Pow(i / 255.0, gamma_r) * 255.0 - gamma_b);
                    if (t > 255) t = 255;
                    if (t < 0) t = 0;
                    NoonLUT[i] = (byte)t;
                }
            }
            switch (Camera.Frame.Format)
            {
                case tImageFormat.eFmtMono8:
                    {
                        UInt32 lOffset = 0;
                        UInt32 lPos = 0;
                        byte* lDst = (byte*)Data.Scan0;
                        byte* lSrc = (byte*)Camera.Frame.ImageBuffer;


                        while (lOffset < Camera.Frame.ImageBufferSize)
                        {
                            try
                            {
                                byte thisPixel = lSrc[lOffset];
                                if (isNight)
                                    thisPixel = NightLUT[thisPixel];
                                else if (isNoon) thisPixel = NoonLUT[thisPixel];
                                lDst[lPos] = thisPixel;
                                lDst[lPos + 1] = thisPixel;
                                lDst[lPos + 2] = thisPixel;
                                PixelMean += lSrc[lOffset];
                            }
                            catch (Exception e)
                            {
                                //System.Windows.MessageBox.Show(e.Message);
                                return false;
                            }
                            lOffset++;
                            lPos += 3;
                            try
                            {
                                // Take care of the padding in the destination bitmap.
                                if ((lOffset % Camera.Frame.Width) == 0)
                                    lPos += (UInt32)Data.Stride - (Camera.Frame.Width * 3);
                            }
                            catch (Exception e)
                            {
                                //System.Windows.MessageBox.Show(e.Message);
                                return false;
                            }
                        }
                        PixelMean /= (double)Camera.Frame.ImageBufferSize;
                        illumiance = PixelMean;
                        //System.Windows.MessageBox.Show(PixelMean.ToString());
                        LuminanceAdjust(ref Camera, PixelMean);
                        return true;
                    }
                case tImageFormat.eFmtMono16:
                    {
                        UInt32 lOffset = 0;
                        UInt32 lPos = 0;
                        byte* lDst = (byte*)Data.Scan0;
                        byte bitshift = (byte)((int)Camera.Frame.BitDepth - 8);
                        UInt16* lSrc = (UInt16*)Camera.Frame.ImageBuffer;

                        while (lOffset < Camera.Frame.Width * Camera.Frame.Height)
                        {
                            lDst[lPos] = (byte)(lSrc[lOffset] >> bitshift);
                            lDst[lPos + 1] = lDst[lPos];
                            lDst[lPos + 2] = lDst[lPos];

                            lOffset++;
                            lPos += 3;

                            // Take care of the padding in the destination bitmap.
                            if ((lOffset % Camera.Frame.Width) == 0)
                                lPos += (UInt32)Data.Stride - (Camera.Frame.Width * 3);
                        }

                        return true;
                    }
                case tImageFormat.eFmtBayer8:
                    {
                        UInt32 WidthSize = Camera.Frame.Width * 3;
                        GCHandle pFrame = GCHandle.Alloc(Camera.Frame, GCHandleType.Pinned);
                        UInt32 remainder = (((WidthSize + 3U) & ~3U) - WidthSize);

                        // Interpolate the colors.
                        IntPtr pRed = (IntPtr)((byte*)Data.Scan0 + 2);
                        IntPtr pGreen = (IntPtr)((byte*)Data.Scan0 + 1);
                        IntPtr pBlue = (IntPtr)((byte*)Data.Scan0);
                        Pv.ColorInterpolate(pFrame.AddrOfPinnedObject(), pRed, pGreen, pBlue, 2, remainder);
                        UInt32 lOffset = 0;
                        byte* lSrc = (byte*)Camera.Frame.ImageBuffer;
                        //double pixelMean2 = 0;
                        while (lOffset < Camera.Frame.ImageBufferSize)
                        {
                            PixelMean += ((double)lSrc[lOffset + 2] + (double)lSrc[lOffset + 1] + (double)lSrc[lOffset]) / 3;

                            if (isNight)
                            {
                                lSrc[lOffset + 2] = NightLUT[lSrc[lOffset + 2]];
                                lSrc[lOffset + 1] = NightLUT[lSrc[lOffset + 1]];
                                lSrc[lOffset] = NightLUT[lSrc[lOffset]];
                            }
                            if (isNoon)
                            {
                                lSrc[lOffset + 2] = NoonLUT[lSrc[lOffset + 2]];
                                lSrc[lOffset + 1] = NoonLUT[lSrc[lOffset + 1]];
                                lSrc[lOffset] = NoonLUT[lSrc[lOffset]];
                            }
                            //pixelMean2 += ((double)lSrc[lOffset + 2] + (double)lSrc[lOffset + 1] + (double)lSrc[lOffset]) / 3;
                            lOffset += 3;
                        }
                        PixelMean /= (double)Camera.Frame.ImageBufferSize / 3;
                        illumiance = PixelMean;
                        //System.Windows.MessageBox.Show(PixelMean.ToString());
                        LuminanceAdjust(ref Camera, PixelMean);
                        pFrame.Free();

                        return true;
                    }
                case tImageFormat.eFmtBayer16:
                    {
                        UInt32 WidthSize = Camera.Frame.Width * 3;
                        UInt32 lOffset = 0;
                        byte bitshift = (byte)((int)Camera.Frame.BitDepth - 8);
                        UInt16* lSrc = (UInt16*)Camera.Frame.ImageBuffer;
                        byte* lDst = (byte*)Camera.Frame.ImageBuffer;
                        UInt32 remainder = (((WidthSize + 3U) & ~3U) - WidthSize);
                        GCHandle pFrame;

                        Camera.Frame.Format = tImageFormat.eFmtBayer8;

                        pFrame = GCHandle.Alloc(Camera.Frame, GCHandleType.Pinned);

                        // Shift the pixel.
                        while (lOffset < Camera.Frame.Width * Camera.Frame.Height)
                            lDst[lOffset] = (byte)(lSrc[lOffset++] >> bitshift);

                        // Interpolate the colors.
                        IntPtr pRed = (IntPtr)((byte*)Data.Scan0 + 2);
                        IntPtr pGreen = (IntPtr)((byte*)Data.Scan0 + 1);
                        IntPtr pBlue = (IntPtr)((byte*)Data.Scan0);
                        Pv.ColorInterpolate(pFrame.AddrOfPinnedObject(), pRed, pGreen, pBlue, 2, remainder);

                        pFrame.Free();

                        return true;
                    }
                case tImageFormat.eFmtRgb24:
                    {
                        UInt32 lOffset = 0;
                        UInt32 lPos = 0;
                        byte* lDst = (byte*)Data.Scan0;

                        while (lOffset < Camera.Frame.ImageBufferSize)
                        {
                            // Copy the data.
                            lDst[lPos] = Camera.Buffer[lOffset + 2];
                            lDst[lPos + 1] = Camera.Buffer[lOffset + 1];
                            lDst[lPos + 2] = Camera.Buffer[lOffset];
                            PixelMean += ((double)Camera.Buffer[lOffset + 2] + (double)Camera.Buffer[lOffset + 1] + (double)Camera.Buffer[lOffset]) / 3;

                            lOffset += 3;
                            lPos += 3;

                            // Take care of the padding in the destination bitmap.
                            if ((lOffset % (Camera.Frame.Width * 3)) == 0)
                                lPos += (UInt32)Data.Stride - (Camera.Frame.Width * 3);
                        }
                        PixelMean /= (double)Camera.Frame.ImageBufferSize / 3;
                        illumiance = PixelMean;
                        //System.Windows.MessageBox.Show(PixelMean.ToString());
                        LuminanceAdjust(ref Camera, PixelMean);
                        return true;
                    }
                case tImageFormat.eFmtRgb48:
                    {
                        UInt32 lOffset = 0;
                        UInt32 lPos = 0;
                        UInt32 lLength = Camera.Frame.ImageBufferSize / sizeof(UInt16);
                        UInt16* lSrc = (UInt16*)Camera.Frame.ImageBuffer;
                        byte* lDst = (byte*)Data.Scan0;
                        byte bitshift = (byte)((int)Camera.Frame.BitDepth - 8);

                        while (lOffset < lLength)
                        {
                            // Copy the data.
                            lDst[lPos] = (byte)(lSrc[lOffset + 2] >> bitshift);
                            lDst[lPos + 1] = (byte)(lSrc[lOffset + 1] >> bitshift);
                            lDst[lPos + 2] = (byte)(lSrc[lOffset] >> bitshift);

                            lOffset += 3;
                            lPos += 3;

                            // Take care of the padding in the destination bitmap.
                            if ((lOffset % (Camera.Frame.Width * 3)) == 0)
                                lPos += (UInt32)Data.Stride - (Camera.Frame.Width * 3);
                        }

                        return true;
                    }
                case tImageFormat.eFmtYuv411:
                    {
                        UInt32 lOffset = 0;
                        UInt32 lPos = 0;
                        byte* lDst = (byte*)Data.Scan0;
                        int y1, y2, y3, y4, u, v;
                        int r, g, b;

                        r = g = b = 0;

                        while (lOffset < Camera.Frame.ImageBufferSize)
                        {
                            u = Camera.Buffer[lOffset++];
                            y1 = Camera.Buffer[lOffset++];
                            y2 = Camera.Buffer[lOffset++];
                            v = Camera.Buffer[lOffset++];
                            y3 = Camera.Buffer[lOffset++];
                            y4 = Camera.Buffer[lOffset++];

                            YUV2RGB(y1, u, v, ref r, ref g, ref b);
                            lDst[lPos++] = (byte)b;
                            lDst[lPos++] = (byte)g;
                            lDst[lPos++] = (byte)r;
                            YUV2RGB(y2, u, v, ref r, ref g, ref b);
                            lDst[lPos++] = (byte)b;
                            lDst[lPos++] = (byte)g;
                            lDst[lPos++] = (byte)r;
                            YUV2RGB(y3, u, v, ref r, ref g, ref b);
                            lDst[lPos++] = (byte)b;
                            lDst[lPos++] = (byte)g;
                            lDst[lPos++] = (byte)r;
                            YUV2RGB(y4, u, v, ref r, ref g, ref b);
                            lDst[lPos++] = (byte)b;
                            lDst[lPos++] = (byte)g;
                            lDst[lPos++] = (byte)r;
                        }

                        return true;
                    }
                case tImageFormat.eFmtYuv422:
                    {
                        UInt32 lOffset = 0;
                        UInt32 lPos = 0;
                        byte* lDst = (byte*)Data.Scan0;
                        int y1, y2, u, v;
                        int r, g, b;

                        r = g = b = 0;

                        while (lOffset < Camera.Frame.ImageBufferSize)
                        {
                            u = Camera.Buffer[lOffset++];
                            y1 = Camera.Buffer[lOffset++];
                            v = Camera.Buffer[lOffset++];
                            y2 = Camera.Buffer[lOffset++];

                            YUV2RGB(y1, u, v, ref r, ref g, ref b);
                            lDst[lPos++] = (byte)b;
                            lDst[lPos++] = (byte)g;
                            lDst[lPos++] = (byte)r;
                            YUV2RGB(y2, u, v, ref r, ref g, ref b);
                            lDst[lPos++] = (byte)b;
                            lDst[lPos++] = (byte)g;
                            lDst[lPos++] = (byte)r;
                        }

                        return true;
                    }
                case tImageFormat.eFmtYuv444:
                    {
                        UInt32 lOffset = 0;
                        UInt32 lPos = 0;
                        byte* lDst = (byte*)Data.Scan0;
                        int y1, y2, u, v;
                        int r, g, b;

                        r = g = b = 0;

                        while (lOffset < Camera.Frame.ImageBufferSize)
                        {
                            u = Camera.Buffer[lOffset++];
                            y1 = Camera.Buffer[lOffset++];
                            v = Camera.Buffer[lOffset++];
                            lOffset++;
                            y2 = Camera.Buffer[lOffset++];
                            lOffset++;

                            YUV2RGB(y1, u, v, ref r, ref g, ref b);
                            lDst[lPos++] = (byte)b;
                            lDst[lPos++] = (byte)g;
                            lDst[lPos++] = (byte)r;
                            YUV2RGB(y2, u, v, ref r, ref g, ref b);
                            lDst[lPos++] = (byte)b;
                            lDst[lPos++] = (byte)g;
                            lDst[lPos++] = (byte)r;
                        }

                        return true;
                    }
                default:
                    return false;
            }
        }


        // Save a frame to a file.
        static bool FrameSave(ref tCamera Camera, string file)
        {
            Bitmap lBitmap = new Bitmap((int)Camera.Frame.Width, (int)Camera.Frame.Height, PixelFormat.Format24bppRgb);
            Rectangle lRect = new Rectangle(new Point(0, 0), new Size((int)Camera.Frame.Width, (int)Camera.Frame.Height));
            BitmapData lData = lBitmap.LockBits(lRect, ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb);

            if (Frame2Data(ref Camera, ref lData))
            {
                lBitmap.UnlockBits(lData);
                lBitmap.Save(file);
                return true;
            }
            else
            {
                lBitmap.UnlockBits(lData);
                return false;
            }
        }

        public tErr InitCamera()
        {
            tErr err = 0;
            if (Inited) return 0;
            // Initialize the API.
            if ((err = Pv.Initialize()) == 0)
            {
                lLinkCB = new tLinkCallback(LinkCB);

                // Register callbacks.
                Pv.LinkCallbackRegister(lLinkCB, tLinkEvent.eLinkAdd, IntPtr.Zero);
                Pv.LinkCallbackRegister(lLinkCB, tLinkEvent.eLinkRemove, IntPtr.Zero);
                Inited = true;
            }
            else
            {
                Console.Write("无法加载相机API，错误代码： ");
                Console.WriteLine(err);
                err_string = "无法加载相机API，错误代码： " + err.ToString();
            }
            return err;
        }

        public void dispose()
        {
            Started = false;
            // Unregister the callbacks.
            Pv.LinkCallbackUnregister(lLinkCB, tLinkEvent.eLinkAdd);
            Pv.LinkCallbackUnregister(lLinkCB, tLinkEvent.eLinkRemove);
            CameraClose();
            // Uninitialize the API.
            Pv.UnInitialize();
            Inited = false;
        }
        
        public static int lastStatus = 0;
        public static double scale = 1;
        public static int LuminanceAdjust(ref tCamera Camera, double mean)
        {
            int result = 0;
            if (mean < lowIlluminance)
            {
                result = -1;
            }
            if (mean > highIlluminance)
            {
                result = 1;
            }
            if (result * lastStatus == -1)//如果上一级过暗，此级过亮（或反之）
            {
                if (gain == 0)
                {
                    //exposuretime = (exposuretime + lastExposuretime) / 2;
                    if ((int)(exposuretime * scale * 0.5) > 0)
                        scale *= 0.5;
                }
                else
                {
                    result = 0;//增益为整数，无法调整，直接不进行调整
                }
            }
            if (result==-1)
            {
                //低照度情况
                isNoon = false;
                //if (frame<=200)
                {
                    //自动增加曝光时间或增益来调整图像亮度
                    if (exposuretime >= 40000)
                    {
                        exposuretime = 40000;
                        if (gain < 10)
                        {
                            gain++;
                        }
                        else
                            gain = 10;
                    }
                    else if (exposuretime <= 10)
                    {
                        exposuretime = 10;
                    }
                    else if (10000 <= exposuretime && exposuretime < 40000)
                    {
                        exposuretime = exposuretime + (uint)(10000 * scale);
                        gain = 0;
                    }
                    else if (1000 <= exposuretime && exposuretime < 10000)
                    {
                        exposuretime = exposuretime + (uint)(1000 * scale);
                        gain = 0;
                    }
                    else if (exposuretime >= 100 && exposuretime < 1000)
                    {
                        exposuretime = exposuretime + (uint)(100 * scale);
                        gain = 0;
                    }
                    else if (exposuretime >= 10 && exposuretime < 100)
                    {
                        exposuretime = exposuretime + (uint)(10 * scale);
                        gain = 0;
                    }
                }
                if (exposuretime == 40000 && gain == 10)
                {
                    //当曝光时间和增益调整到最大值后，对低照度图像进行非线性变换调整图像质量
                    if (isNight == false) isNight = true;
                    else result = 0;
                }

            }
            else if (result==1)
            {
                //强光照情况
                isNight = false;
                //if (frame<=200)
                {
                    //自动降低曝光时间和增益来降低图像亮度
                    if (gain > 0)
                    {
                        gain = 0;
                    }
                    else
                    {
                        if (exposuretime > 40000)
                        {
                            exposuretime = 40000;
                        }
                        else if (exposuretime <= 10)
                        {
                            exposuretime = 10;
                        }
                        else if (10000 < exposuretime && exposuretime <= 40000)
                        {
                            exposuretime = exposuretime - (uint)(10000 * scale);
                        }
                        else if (1000 < exposuretime && exposuretime <= 10000)
                        {
                            exposuretime = exposuretime - (uint)(1000 * scale);
                        }
                        else if (exposuretime > 100 && exposuretime <= 1000)
                        {
                            exposuretime = exposuretime - (uint)(100 * scale);
                        }
                        else if (exposuretime > 10 && exposuretime <= 100)
                        {
                            exposuretime = exposuretime - (uint)(10 * scale);
                        }

                    }

                }
                if (exposuretime == 10 && gain == 0)
                {
                    //当无法对曝光时间和增益进行调整时对图像进行gamma变换来调整图像亮度
                    if (isNoon == false) isNoon = true;
                    else result = 0;

                }

            }
            else if (mean < highIlluminance && mean >= lowIlluminance)
            {
                //正常光照情况，不对图像进行任何处理

            }
            if (result != 0)
            {
                Pv.AttrUint32Set(Camera.Handle, "ExposureValue", exposuretime);
                Pv.AttrUint32Set(Camera.Handle, "GainValue", gain);
            }
            lastStatus = result;
            Console.WriteLine("Brightness: {0}, ExposureValue: {1}, GainValue: {2}", mean, exposuretime, gain);
            return result;
        }
    }
}


//while (WaitForUserToQuitOrSnap())
//                            {

//                                if (CameraSnap(ref thisCamera))
//                                {
//                                    filename = "Snap";
//                                    filename += Count++;
//                                    filename += ".bmp";

//                                    if (FrameSave(ref thisCamera, filename) == false)
//                                        Console.WriteLine("Failed to save the image.");
//                                    else
//                                        Console.WriteLine("Frame saved.");
//                                    Console.WriteLine("The camera is waiting for a trigger. Press Q to quit or S to take a picture.");
//                                }
//                                else
//                                    Console.WriteLine("Failed to snap an image.");
//                            }
