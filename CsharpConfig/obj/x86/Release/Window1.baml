   M S B A M L   `   `   `  ���� L  HVIDGS配置软件, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null5clr-namespace:VIDGS配置软件VIDGS配置软件  M ISystem, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089M3clr-namespace:System.ComponentModel;assembly=SystemSystem.ComponentModel T PMahApps.Metro, Version=1.1.2.0, Culture=neutral, PublicKeyToken=f4fb5a3c4d1e5b4fV;clr-namespace:MahApps.Metro.Controls;assembly=MahApps.MetroMahApps.Metro.Controls 5clr-namespace:VIDGS配置软件VIDGS配置软件  [ WXceed.Wpf.AvalonDock, Version=2.0.0.0, Culture=neutral, PublicKeyToken=3e4669d2f30244f4_Cclr-namespace:Microsoft.Windows.Shell;assembly=Xceed.Wpf.AvalonDockMicrosoft.Windows.Shell $    VIDGS配置软件.SetParameter   \ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35^ ZWindowsFormsIntegration, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35F 9http://schemas.microsoft.com/winfx/2006/xaml/presentation     R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     1localclr-namespace:VIDGS配置软件     Bcm3clr-namespace:System.ComponentModel;assembly=System     PControls;clr-namespace:MahApps.Metro.Controls;assembly=MahApps.Metro     d	 `Microsoft.Expression.Controls, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35>ec5http://schemas.microsoft.com/expression/2010/controls 	 1localThemesclr-namespace:VIDGS配置软件 	 OshellCclr-namespace:Microsoft.Windows.Shell;assembly=Xceed.Wpf.AvalonDock 	 h
 dXceed.Wpf.AvalonDock.Themes.Metro, Version=2.0.0.0, Culture=neutral, PublicKeyToken=3e4669d2f30244f4i eXceed.Wpf.AvalonDock.Themes.VS2010, Version=2.0.0.0, Culture=neutral, PublicKeyToken=3e4669d2f30244f4m iXceed.Wpf.AvalonDock.Themes.Expression, Version=2.0.0.0, Culture=neutral, PublicKeyToken=3e4669d2f30244f4g cXceed.Wpf.AvalonDock.Themes.Aero, Version=2.0.0.0, Culture=neutral, PublicKeyToken=3e4669d2f30244f4E
avalonDock,http://schemas.xceed.com/wpf/xaml/avalondock 
     X TXceed.Wpf.Toolkit, Version=2.5.0.0, Culture=neutral, PublicKeyToken=3e4669d2f30244f44xctk)http://schemas.xceed.com/wpf/xaml/toolkit  -     � 
ResizeMode$  CanMinimize=� J� 	AllowDrop .  � SizeToContent$ WidthAndHeight=�(  "MahApps.Metro.Controls.MetroWindow   WindowTransitionsEnabled .   � Title$ 激光设置��$	��748��$
��1024�� � 	Resources    VIDGS配置软件.GNumArr   GuideNum   VIDGS配置软件.SNumArr  StopNum,  &System.ComponentModel.BackgroundWorker  backgroundWorker   WorkerReportsProgress!   WorkerSupportsCancellation��%@   &	        &	      &	 
           -    .  . .���� $��0,0,0,0q����#395984 �� CornerRadius$ 20,20,20,20��$
��1018��$	��715������ ��� � 	 � Color$	 #A7A9B0��
 � Offset$	
 0.0Q�� $	 #323A52��$	
 1.0Q�._��  � ColumnDefinitions �� $��* ��� $	��356 ��� $	��284 � � RowDefinitions �� $
��1.4* ��� $��* ��� $
��2.5* �.+�� $��	5,-2,10,0q�$��0��$��2�����	#FFB8A5A5 �� $��5 ��� $��70 ��� $��15 ��� $��50 ��� $��5 ��� $��50 ��� $��5 ��� $	��100 ��� $��* � �� $��* ��� $
��0.8* ��� $
��0.1* �.+��� -    ��Name$ Block1��$
��Wrap=�$��21(�$��1��$��0��$��2��$y�Visible=�$��-10,10,10,5q�$��2���� -   $ Block2��$
��Wrap=�$��18(�$��3��$��5��$��0��$��2��$y�Visible=�� $��1��$��3��$��0,0,7,10q����	#FF898484 �� $��* ��� $��39 ��� $��58 ��� $��48 ��� $��30 ��� $��10 � �� $��* ��� $��* �.+��� -    ��Name$ Group2d��$��0��$��2��$��6,0,6,0q�$��微软雅黑)���Z� ��Z� .���� -    ��Name$ border2d��$	��322��$	��466��$��	0,-14,0,0q�}�. ._��� -    ��Name$ 
Image2dBox��$	��322��$	��466��$��	0,-14,0,0q� J� RenderTransformOrigin$ 0.5,0.5&� � Cursor$
 Hand��$
��Auto=�$��Visible=��� $��2��$��0��$��18(�$��40,0,0,0q�.��X轴缩放开/  )MahApps.Metro.Controls.ToggleSwitchButton  -     Name$ 	Xaxiszoom��$	��100��$��2��$��0�� d� 	IsChecked$
 TrueY�$��
40,25,30,0q�)  #MahApps.Metro.Controls.ToggleSwitch  -	     Name$ 	Yaxiszoom��  Yaxiszoom48   Header# �  $	��120��$��2��$��1��   	IsChecked$
 TrueY�   ContentDirection$ RightToLeft=�   OffLabel$	 关��   OnLabel$	 开���� -
    ��Name$ labelPosition��$��1��$��2��$��x:0 y:0��$��Center=�$��14(�$��Center=�$��-10,-20,1,1q�$	��169��$��37���� -    ��Name$ 
button_Clc��$��1��$��1��$��21(�$
��auto��$
��auto��$��84,-20,7,10q� 	 Clc49#���  �� -   $ button_Save��$��3��$��90��$��21(�$��22,0,14,-20q� 
 Save50#���  $��45��$��7,3,10,3q��� -   $ 
button_Esc��$��3��$��1��$��90��$��21(�$��21,25,29,-20q� 	 Esc51#���  $��45��$��3,7,10,3q�� $��2��$��0��$��	2,-20,8,7q�����  �� EndPoint$ 0.5,1&� � MappingMode$ RelativeToBoundingBox=� �� 
StartPoint$ 0.5,0&�.��� $	 	#FFAEAEAE��$
 1Q�� $	 	#FF6A7675��$
 0.384Q� �� $��30 ��� $��30 ��� $��30 ��� $��* �.+�*  $MahApps.Metro.Controls.NumericUpDown  -      Name$  XNumericUpDown��!   Minimum$! 1Q�"   Maximum$" 50Q�#   Value$# 1X�$   Interval$$ 1Q�$��3��$��48,82,48,61q�$��34��$��72���� -   $ button_2dScan0��$��1��$��-45,-20,102,25q�$��2��$��45��$	��110��$��21(�.���� $
��auto��$
��auto��.+���   2dscan_dis26#���  $��WrapWithOverflow=��� -   $ button_Back0��$��3��$��21(�$��130,-58,-24,180q�$��45��$	��110��.���� $
��auto��$
��auto��.+��� -   $ ForBack0_dis27��  Back0_dis27#���  $��WrapWithOverflow=��� -   $ button_RoughlyBack0��$��3��$��21(�$��-45,-58,102,180q�$��45��$	��110��.���� $
��auto��$
��auto��.+���  	 RoughlyBack095#��� 	 $��WrapWithOverflow=�$ 0.5,0.5&�z�V� .~��� �� �� % �� Angle$
% 1.11Q�T� �� -   $ button_Set0��$��1��$��130,-20,-24,25q�$��21(� 
 
Set0_dis28#��� 
 $��2��$��45��$	��110��$
��10,5q�/ 	 )Microsoft.Expression.Controls.PathListBox  $��39,80.25,31,74q�$��3��&   LayoutPaths& .	 	 (Microsoft.Expression.Controls.LayoutPath	  ' 	  Distribution$
' Even=�( 	  SourceElement( �� ) �� ElementName$) ellipse��.@��� -   $ button_RightMove��  MetroCircleButtonStyle#���  $��58��$��58�� , (Shift laser scanning area to the right84* � ToolTip#* �  $	��Top=�.���� $��20��$��20����!� + j� Stretch$
+ Fill=�  appbar_arrow_right, !� Visual#, [ �� -   $ button_DownMove��#���  $��58��$��58�� # Move down laser scanning area85#* �  $	��Top=�.���� $��20��$��20����!� $
+ Fill=�  appbar_arrow_down#, [ �� -   $ button_LeftMove��#���  $��58��$��58�� + 'Shift laser scanning area to the left86#* �  $	��Top=���Z� .���� $��20��$��20����!� $
+ Fill=�  appbar_arrow_left#, [ �� -   $ button_UpMove��#���  $��58��$��58�� ! Move up laser scanning area87#* �  $	��Top=�.���� $��20��$��20����!� $
+ Fill=�  appbar_arrow_up#, [ A� -   - A�Name$- ellipse��$��Stretch=�$	��130��$��0,90,0,0q�$��3��$	��Top=�$	��130��,
  &MahApps.Metro.Controls.MetroTabControl
  -   . 
 Name$. SetTabControl��$��2��$��1��$��2�����	#FF898484.@�)  #MahApps.Metro.Controls.MetroTabItem  -   /  Name$
/ Tab1��  Calibration74#���  $y�Visible=�$��15,0,0,0q�$��84��.��� $��5,0,0,0q� �� $��10 ��� $��* ��� $��* ��� $��* ��� $��* ��� $��* ��� $��98 �.+�  $��80��$	��100��$��3��$
 TrueY��� -   $ button_ConfirmRect��$��5��$��30,10q�$��2��.���� $
��auto��$
��auto��$��Center=�.+��� -   $ ForConfirmRect31��$��24(�  ConfirmRect31#���  $��WrapWithOverflow=��� -   $ button_AutoCalibration��$��1��$��30,10q�.���� $
��auto��$
��auto��$��Center=�.+��� -   $ ForAutoCalibration29��$��24(�  AutoCalibration29#���  $��WrapWithOverflow=��� -   $  button_VerticalCalibration��$��2��$��30,10q�.���� $
��auto��$
��auto��$��Center=�.+��� -    $ ForVerticalCalibration30��$��24(�  VerticalCalibration30#���  $��WrapWithOverflow=�  -!   $
/ Tab2��  
Guideline6#���  $y�Visible=�$��13,0,0,0q�$	��100��.��� $��5,0,0,0q� �� $��65 ��� $��65 ��� $��65 ��� $��45 ��� $��65 ��� $��60 ��� $��60 ��� $��50 � �� $
��0.6* ��� $
��0.6* ��� $��* ��� $��* �.+��� -"   $ button_WalkSet��$��0��$��0��$��3��$��15,10,25,10q�  	WalkSet32#���  $��21(��� -#   $ button_WalkTest��$��1��$��0��$��3��  Start Walk settings88#* �  $��15,10,25,10q�  
WalkTest33#���  $��21(��� -$   $ button_WalkStop��$��0��$��3��$��-13,10,16,10q�  
WalkStop34#���  $��21(��� -%   $ button_WalkConfirm��$��1��  Confirm walk settings89#* �  $��3��$��-13,10,16,10q�  WalkConfirm35#���  $��21(��� -&   $ button_WalkDelete��$��2��$��21(�$��3��$��-13,10,16,10q�  WalkDelete36#���  $ 
0.5,-0.108&��� -'   $ button_WalkNum��$��2�� 6  2Number the Guide line generated by walk settings90#* �   $��0��$��3��$��15,10,25,10q� ! 	WalkNum37#��� ! $��21(��� -(   $ Fordependcystopline��$��3��$��2�� " 	Stopline7#��� " $��50��$��90��$��21(�$��17,0,0,0q��� -)   0 ��Name$0 Stop_Y��$��3��$��2��$��2��$��18(�$��	30,0,16,0q�$��38���� -*   1 ��Name$1 drpModel��$��5��$��0��2 �� SelectedIndex$2 0��$��2��$��38��$��10,0,5,0q��� -+   $0 Number_GuideLine��$��6��$��0��$��2��$��18(�$��38��$��10,0,5,0q��� -,   $ button_ModelGuide��$��5��$��2��$��2�� # Input Plane Type91#* � # $
��auto��$
��auto��$
��5,10q�$��18(�.���� $
��auto��$
��auto��.+���  $ ModelGuide38#��� $ $��WrapWithOverflow=��� --   $ button_NumberGuide��$��6��$��2��$��2�� % Input Line Number92#* � % $
��auto��$
��auto��$
��5,10q�$��18(�.���� $
��auto��$
��auto��.+���  & NumberGuid39#��� & $��WrapWithOverflow=��� -.   $ button_TestConfirmPoint��$
��auto��$��4��$��0��$��4��$��38.561,10,38,10q�$��21(�.���� $
��auto��$
��auto��.+���  ' TestConfirmPoint40#��� ' $��WrapWithOverflow=�  -/   $
/ Tab3�� ( Master mod73#��� ( $y�Visible=�$��16,10,-16,-10q�$��84��.��� $��1��$��2��$��	10,6,17,6q� �� $��10 ��� $��55 ��� $��55 ��� $��55 ��� $��55 ��� $��55 ��� $��55 ��� $��55 � �� $	��34* ��� $	��94* ��� $
��129* �.+��� -0   $ button_ReadRect��$��1��$��21(�$��0��$��3��$
��30,5q� ) 
ReadRect42#��� ) �� -1   $ button_Proof��$��2��$��0��$��3��$
��30,5q�$��21(� * Proof41#��� * �� -2   $ button_PointMeasure��$��4��$��0��$��14,5,3,5q�$��14(�$��2��.���� 3 �� Orientation$3 
Horizontal=�$��Center=�.+���  + 	mid_num43#��� + $��WrapWithOverflow=��� -3   $ mid_num��$��4��$��2��$��0��$��	20,5,10,5q�$��18(��� -4   $ button_OpenRed��$��5��$��0��$��14,5,3,5q�$��2��.���� $
��auto��$
��auto��.+���  , 	OpenRed44#��� , $��WrapWithOverflow=�$��15(��� -5   $ button_CloseRed��$��5��$��2��$��18,5,0,4q�.���� $
��auto��$
��auto��.+���  - 
CloseRed45#��� - $��WrapWithOverflow=�$��15(��� -6   $ button_WindowScan��$��3��$��21(�$��0��$��3��$
��30,5q� . WindowScan46#��� . �� -7   $ button_GuideLineMove��$��6��$��0��$��
14,5,118,5q� #/ Input num to shift Guide line93#* � / $��3��.���� $
��auto��$
��auto��.+���  0 Guidelinemove71#��� 0 $��WrapWithOverflow=�$��21(��� -8   $0 TextBox_GuideLineMove��$��6��$��2��$��99��$��38��$��30,9,0,9q��� -9   $ button_StopLineMove��$��7��$��0��$��
14,5,118,5q� "1 Input num to shift Stop line94#* � 1 $��3��.���� $
��auto��$
��auto��.+���  2 stoplinemove72#��� 2 $��WrapWithOverflow=�$��21(��� -:   $0 TextBox_StopLineMove��$��7��$��2��$	��100��$��38��$��	30,9,-1,9q�  -;   $
/ Tab4��#��� ! $y�	Collapsed=�.���� $	��243��$
��Auto��$��1q�$ 20,20,20,20��._�� $	��241�� 4 �� 
TargetType5 �� Property6 �� Value��%G   '��       �� "4 ��.��� 5 � ��$6 16(��� 5 � ��$6 Center=� �� $��50 ��� $��30 ��� $��30 ��� $��30 ��� $
��1.5* ��� $��80 ��� $��* � �� $��75 ��� $��* �.+��� $��1��$��0�� 3 guideline68#��� 3 ��  4 
stopline69#��� 4 $��2��$��0���� $��3�� 5 
distance52#��� 5 �� -<   $0 drpdistance��$��3��$��1��$��0,0,25,0q�*  $MahApps.Metro.Controls.TextBoxHelper7   ClearTextButton7 . �� -=   $1 drpGuide��$��1��$��1��$��30��$��0,0,25,0q�#��[  8 �� DisplayMemberPath$8 Guide��9 �� SelectedValuePath$9 Guide��$2 0���� ->   $1 drpStop��$��2��$��1��$��30��$��0,0,25,0q�#��[ $
8 Stop��$
9 Stop��$2 0��� $��4��$��Center=�$��2��$��Center=� �� $
��Auto ��� ��  �� $
��Auto ��� $
��Auto ��� $
��Auto �.+��� #��� 3 $
��Left=�$��64���� $��1��$��60������ $) drpGuide��: �� Path$: SelectedValue��� $��1��#��� 4 �� -?   $ NumStop��$��1��$��1��$��60������ $) drpStop��$: SelectedValue���  6 C123#��� 6 $��2��$
��Left=�$��64���� $��2��$��1��$��60������ $) drpdistance��$
: Text��� $��2��$��2��$��mm��� $��4��$��Center=�$��2��$��Center=��� $	��Top=�$��Center=�$��10q�$��46��$��-40,0,121,0q�$��65��$��14(�$��1��$��5�� 7 C130#��� 7 �� $	��Top=�$��Center=�$��10q�$��46��$��	68,0,35,0q�$��65��$��14(�$��1��$��5�� 8 C131#��� 8 � $��0��$��2�����	#FF898484 �� $��5 ��� $��40 ��� $��5 ��� $��40 ��� $��5 ��� $��40 ��� $��5 ��� $��40 � �� $
��0.4* ��� $
��0.5* �.+��� $��1��$��0��$��18(�$��0,6,0,0q� 9 install_high22#��� 9 $
��Wrap=��� -@   $0 Install_height��$��1��$��1��7 . $��18(��� $��3��$��0��$��18(�$��0,6,0,0q� : object_high23#��� : $
��Wrap=��� -A   $0 Object_height��$��3��$��1��$��18(�7 . �� $��5��$��0��$��19(�$��0,6,0,0q� ; object_dis24#��� ; $
��Wrap=��� -B   $0 Object_distance��$��5��$��1��$��18(�7 . �� $��7��$��0��$��18(�$��0,6,0,0q� < Edit_StandHigh25#��� < $
��Wrap=��� -C   $0 edit_StandHigh��$��7��$��1��$��18(�7 . F� $��1�������5#5 `�	���	#FFF4F4F5$
��Left=�$��1��$��439.326,4.833,-92.5,0q�$��2��; �� Stretch$
; Fill=�
���   �$	��Top=�$��1��<   Flyouts< +  %MahApps.Metro.Controls.FlyoutsControl  