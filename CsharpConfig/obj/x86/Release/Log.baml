   M S B A M L   `   `   `  ���� T  PMahApps.Metro, Version=1.1.2.0, Culture=neutral, PublicKeyToken=f4fb5a3c4d1e5b4fV;clr-namespace:MahApps.Metro.Controls;assembly=MahApps.MetroMahApps.Metro.Controls  L HVIDGS配置软件, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null5clr-namespace:VIDGS配置软件VIDGS配置软件    VIDGS配置软件.Log   \ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35^ ZWindowsFormsIntegration, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35F 9http://schemas.microsoft.com/winfx/2006/xaml/presentation     R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     PControls;clr-namespace:MahApps.Metro.Controls;assembly=MahApps.Metro     1localclr-namespace:VIDGS配置软件     -     � WindowStartupLocation$  CenterScreen=� � 
ResizeMode$ CanMinimize=� � Title$	 Log��$	��650��$	��900�� � 	Resources    formatTextStyle �� 
TargetType �� Property �� Value �� ElementName  formatImageStyle  ShowimageSwitchView1 g� 	BeginTime	 �� 
TargetName
 �� TargetProperty O� KeyTime O� Value R� KeyTime  System.Windows.Visibility   Visible R� Value  HideimageSwitchView1   Hidden  OfficeTabItem �� 
TargetType �� CornerRadius �� Triggers P� Property P� Value �� 
TargetName  TitleParagraph  StatisticsControlStyle��%  &	        &	 �     &	 �     &	 �    &	     &	 N    'J� �    &	 T    �� " ��.���  � ��$ Palatino Linotype)���  � ��$ 30����  � ��$ 14(���  � �� �� $ mainRTB���� " ��.���  � ��$ 30����  � �� �� $ StoryRichTextBox���� .��U� $ 00:00:00X�$	 imageSwitchView1��$
 (UIElement.Opacity)�.\��� $ 00:00:00��$ 0Q��� $ 00:00:00.6000000��$ 1Q�T� $ 00:00:00X�$	 imageSwitchView1��$
 (UIElement.Visibility)�..�i� $ 00:00:00��# Z i� $ 00:00:00.6000000��# Z �� .��U� $ 00:00:00X�$
 (UIElement.Opacity)�$	 imageSwitchView1��.\��� $ 00:00:00��$ 1Q��� $ 00:00:00.3000000��$ 0Q�T� $ 00:00:00X�$	 imageSwitchView1��$
 (UIElement.Visibility)�..�i� $ 00:00:00��# Z i� $ 00:00:00.3000000��# Z �� " ��.���  � �� �� " ��.R�� $	��120��.+��� $��bd��#��z��
�������$��1q�$ 0,0,0,0��$��0,3,0,0q�._��� $��Header��$
��10,3q� P�  �@��
IsSelected . . ��� $ bd�� � ��
 ������� $ bd�� � ��$ 0q�P�  �@J�IsMouseOver . . ��� $ bd�� � ��
 ������� " J�.���  � ��$ Calibri)���  � ��$
 14pt(���  � ��
 �**����  � ��
 �������  � ��$
 Bold"���  �@��TextAlignment$
 Left=��� " J�.���  � ��$ Calibri)���  � ��$
 10pt(���  � ��
 �   ��� " ��.���  � ��$ 	10,0,10,0q���  � �� �	#33FFFFFF��  �@��CornerRadius$ 3,3,3,3��.���� .@��� #��[ $��泊位记录��.���  � RowDefinitions �� $��* ��� $	��450 � � ColumnDefinitions �� $	��630 ��� $��* �.+��� -    ��Name$ HeightCanvas�����#D9DADB$��5q�}�. $��0��.+�� ���#857852$��3�� �� StrokeLineJoin$ Bevel=� J� SnapsToDevicePixels .  � Points �   @x}�w� F�#�w�$	��600��� ���#857852$��3��$ Bevel=� .  �   @��@Ys@����� � -    �Name$ LaserImageControl��$��5q�$��1�����#F7F0F0 �� $��72 ��� $	��300 ��� $��* ��� $	��150 �.+�)   #MahApps.Metro.Controls.ToggleSwitch  -     Name$ Log_Station_Switch��   	IsChecked$
 TrueY�$��80��$	��100��    Header$  显示站坪扫描 ��$��0,20,0,339.6q�!   OffLabel$	! 关��"   OnLabel$	" 开��$��Microsoft YaHei)�$��2���� -     MetroCircleButtonStyle#���  $��70��$��70��$��Center=�$��0,150,0,219.6q�$��2��.���� $��30��$��30����!� # j� Stretch$
# Fill=�  appbar_arrow_up$ !� Visual#$ [ �� -   #���  $��70��$��70��$��Center=�$��0,253,0,116.6q�$��2��.���� $��30��$��30����!� $
# Fill=� 	 appbar_arrow_down#$ [	 �� $��1��.+��� -   % ��Name$% groupLog��$��0q�$	��294��$	��388��}�. $��0,0,0,0q���Z� .���� -   $��	borderLog��$	��294��$	��388��._��� -	   $��ImageLog��$	��288��$	��382��& J� RenderTransformOrigin$& 0.5,0.5&�$
��Auto=�$��Visible=��� -
   ' ��Name$' 
TimeSlider��( � Minimum$( 0Q�) � Maximum$) 10Q�* � SmallChange$* 0.001Q�+ �� TickPlacement$+ TopLeft=�$��	10,0,10,0q��� , �� Orientation$, 
Horizontal=�.+��� -   - ��Name$- minDate��$��10,0,0,0q�$��80��$��36��$
��Left=��� -   $- currentDate��$��	30,0,30,0q�$��80��$��36��$��Center=��� -   $- maxDate��$��0,0,10,0q�$��80��$��36��$��Right=��� -   $��LogListView�����	#FFEAE1E1$��3��$��5q�. �� View. �� P��� $
��List���� -   $��Colormap_Image��$
��Left=�$	��400��$��2,0,2,0q�$��Center=�$��68��$��0���� -   $ OffsetCanvas�����#D9DADB$��	5,10,5,10q�}�. $��1��$��1��.+�� ���#857852$��3��$ Bevel=� .  �   @���e�@������ ���#857852$��3��$ Bevel=� .  �   ��� @u��*d@u�� $��5q�$��1��$��0�����#DFE6E6 �� $��30 ��� $��* ��� $��* ��� $��* � �� $
��1.8* ��� $��* ��� $��* �.+��� $��高度��$��1��$��0��$��Center=�$��Center=�$��16(��� $��到停止线距离��$��WrapWithOverflow=�$��2��$��0��$��Center=�$��Center=�$��16(��� $��偏移��$��3��$��0��$��Center=�$��Center=�$��16(��� $��正向��$��0��$��1��$��Center=�$��Center=�$��16(��� $��反向��$��0��$��2��$��Center=�$��Center=�$��16(��� -   / ��Name$/ 	TB_Height��$��1��$��1��$��Center=�$��Center=�$��16(��� -   $/ 	TF_Height��$��1��$��2��$��Center=�$��Center=�$��16(��� -   $/ TB_Dis��$��2��$��1��$��Center=�$��Center=�$��16(��� -   $/ TF_Dis��$��2��$��2��$��Center=�$��Center=�$��16(��� -   $/ 	TB_Offset��$��3��$��1��$��Center=�$��Center=�$��16(��� -   $/ 	TF_Offset��$��3��$��2��$��Center=�$��Center=�$��16(��� #��[ $��关键记录��.���� $��
10,0,10,10q�._�]� -   0 ]�Name$0 	MainPanel��.+��� -   1 ��Name$1 
BodyBorder��$ 	0,0,10,10��$��0,1,0,0q�._�� -   $ BodyGrid��$	��Top=�.+��� -   $1 StoryViewBorder��$��
5,10,10,10q�._��  �� $��40 ��� $��* �.+�]� $��	10,10,0,0q�.+��� -   2 ��Name$2 ButtonPrint��$��_打印��$��Right=�$��0,0,10,0q�$��Right=�$	��Top=�-� -   3 -�Name$3 StoryViewer��$��Stretch=�$��Stretch=�$��5,0,10,0q�4 -� MinZoom$4 50Q�5 -� IsFindEnabled5 .  6 -� IsPrintEnabled6 . 7 -� IsTwoPageViewEnabled7 .  8 -� MaxZoom$	8 150Q����	#00FFFFFF$��1��.��/� �� #��[ $��图片记录��.���  �� $	��110 ��� $��* ��� $	��110 �.+��� -   $��0��#���  $��70��$��70��$��Center=�$��40,0,0,0q�.���� $��30��$��30����!� $
# Fill=� 
 appbar_arrow_left#$ [
 �� -   $��2��#���  $��70��$��70��$��Center=�$��0,0,40,0q�.���� $��30��$��30����!� $
# Fill=�  appbar_arrow_right#$ [ �� $��1��.+��� -   $% GroupRecord��}�. 9 J� Opacity$9 1Q�$y�Visible=�$��Center=�$��Center=�$��微软雅黑)���Z� ��Z� .���� -    $��borderImageRecord��$9 1Q�$y�Visible=�$��Center=�$��Center=�._��� -!   $��ImageRecord��$& 0.5,0.5&�$
��Auto=�$��Visible=�]� $��2��$��	10,10,0,0q�.+��� -"   $2 
ImagePrint��$��_打印��$��Right=�$��0,0,10,0q�$��Right=�$	��Top=�