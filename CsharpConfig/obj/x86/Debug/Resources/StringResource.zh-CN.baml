   M S B A M L   `   `   `  ����O  Kmscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e0891&clr-namespace:System;assembly=mscorlibSystem  5      R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35\ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35^ ZWindowsFormsIntegration, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089 
  Config  Camera1 
 plane2  system3  
visualset4  TI5  
Guideline6  	Stopline7  
Markpoint8 	 Capture region9 
 Tracking area10  The boarding bridge11  Draw GuideLine12  Draw StopLine13  Delete14  Draw Rect15  Insert Markpoint16  Save VideoSet17  SaveAndEsc18  GuideLineNum19  StopLineNum20  DeleteLine21  install_high22  object_high23  object_dis24  Edit_StandHigh25  2dscan_dis26  Back0_dis27  
Set0_dis28  AutoCalibration29  VerticalCalibration30  ConfirmRect31   	WalkSet32 ! 
WalkTest33 " 
WalkStop34 # WalkConfirm35 $ WalkDelete36 % 	WalkNum37 & ModelGuide38 ' NumberGuid39 ( TestConfirmPoint40 ) Proof41 * 
ReadRect42 + 	mid_num43 , 	OpenRed44 - 
CloseRed45 . WindowScan46 / Xaxiszoom47 0 Yaxiszoom48 	1 Clc49 
2 Save50 	3 Esc51 4 
distance52 5 CloseCamera53 6 parametername54 7 planetype55 8 value56 9 Description57 : 	Chinese58 ; 	English59 < Visual interface60 = Laser settings interface61 > Adding planetype62 ? Apply63 @ Delete63 A 	confirm64 B Cancel65 C line width66 D line color67 E guideline68 F 
stopline69 	G dis70 H Guidelinemove71 I stoplinemove72 J Master mod73 K Calibration74 L Laser equipment operation75 M Plane type test76 N M77 O M78 P M79 Q M80 R M81 S M82 T M83 ,U (Shift laser scanning area to the right84 #V Move down laser scanning area85 +W 'Shift laser scanning area to the left86 !X Move up laser scanning area87 Y Start Walk settings88 Z Confirm walk settings89 6[ 2Number the Guide line generated by walk settings90 \ Input Plane Type91 ] Input Line Number92 #^ Input num to shift Guide line93 "_ Input num to shift Stop line94 ` RoughlyBack095 a F96 b F97 c F98 d F99 e F100 f F101 g F102 h F103 i F104 j F105 k F106 l F107 m F108 n F109 o F110 p F111 q C112 r C113 s C114 t C115 u C116 v C117 w C118 x C119 y C120 z C123 { C124 | C125 } C126 ~ C127  C128 � C129 � C130 � C131 � C132 � C133�� 5      F 9http://schemas.microsoft.com/winfx/2006/xaml/presentation     :x,http://schemas.microsoft.com/winfx/2006/xaml     5      .sys&clr-namespace:System;assembly=mscorlib  5      %*  &	        &	 )     &	 M     &	 q     &	 �     &	 �     &	 �     &	     &	 8    &		 Y    &	
 }    &	 �    &	 �    &	 �    &	     &	 =    &	 a    &	 �    &	 �    &	 �    &	      &	 '    &	 K    &	 s    &	 �    &	 �    &	 �    &	     &	 5    &	 Y    &	 }    &	 �    &	  �    &	! �    &	"     &	# +    &	$ I    &	% g    &	& �    &	' �    &	( �    &	)     &	* *    &	+ N    &	, v    &	- �    &	. �    &	/ �    &	0     &	1 2    &	2 P    &	3 n    &	4 �    &	5 �    &	6 �    &	7 �    &	8     &	9 (    &	: F    &	; d    &	< �    &	= �    &	> �    &	? �    &	@ 	    &	A 6	    &	B T	    &	C r	    &	D �	    &	E �	    &	F �	    &	G �	    &	H 
    &	I E
    &	J l
    &	K �
    &	L �
    &	M �
    &	N �
    &	O &    &	P V    &	Q �    &	R     &	S <    &	T l    &	U �    &	V �    &	W     &	X >    &	Y t    &	Z �    &	[ �    &	\ �    &	] (    &	^ ^    &	_ �    &	` �    &	a �    &	b F    &	c y    &	d �    &	e �    &	f     &	g ;    &	h w    &	i �    &	j �    &	k     &	l :    &	m g    &	n �    &	o �    &	p �    &	q     &	r 9    &	s c    &	t �    &	u �    &	v �    &	w     &	x ,    &	y J    &	z t    &	{ �    &	| �    &	} �    &	~     &	 8    &	� \    &	� �    &	� �    &	� �    &	� �    ��5      VIDGS配置软件��6+   ��5      开启相机��6'   ��5      机型设置��6&   ��5      #系统基础属性 查询配置��6/   ��5      保存视觉配置��6,   ��5	      标定工具��6#   ��5
      	引导线��6)   ��5      	停止线��6(   ��5      	标志点��6)   ��5      捕获区域��6/   ��5      跟踪定位区��60   ��5      登机桥位置��66   ��5      绘制引导线��61   ��5      绘制停止线��60   ��5      单个删除��6(   ��5      绘制选框��6+   ��5      插入标志点��63   ��5      保存视频设置��61   ��5      保存并退出��6-   ��5      引导线编号��6/   ��5      停止线编号��6.   ��5      删除选线��6,   ��5      安装高度(mm)��62   ��5      对象高度(mm)��61   ��5      对象距离(mm)��60   ��5      垂直步数(1/32)��66   ��5      二维扫描��6,   ��5      回到零点��6+   ��5       置为零点��6*   ��5!      水平标定��61   ��5"      垂直标定��65   ��5#      确认边框��6-   ��5$      走步设置��6)   ��5%      走步测试��6*   ��5&      
停止��6(   ��5'      
确认��6+   ��5(      
清空��6*   ��5)      
编号��6'   ��5*      由机型读取引导线��60   ��5+      由编号读取引导线��60   ��5,      验证停止点��63   ��5-      
校准��6%   ��5.      读取边框��6*   ��5/      单点测量(mm)��6-   ��50      打开红色激光��6+   ��51      关闭红色激光��6,   ��52      窗口扫描��6,   ��53      
X轴缩放��6,   ��54      
Y轴缩放��6,   ��55      
清屏��6#   ��56      
保存��6$   ��57      
退出��6#   ��58      
距离��6(   ��59      关闭相机��6-   ��5:      	参数名��6.   ��5;      
机型��6)   ��5<      值��6$   ��5=      
描述��6+   ��5>      
中文��6'   ��5?      
英语��6'   ��5@      视觉设置界面��64   ��5A      激光设置界面��6<   ��5B      添加机型��62   ��5C      
应用��6%   ��5D      
删除��6&   ��5E      
确定��6'   ��5F      
取消��6&   ��5G      线条宽度��6,   ��5H      选线颜色��6,   ��5I      
引导线:��6+   ��5J      
停止线:��6*   ��5K      距离 :��6%   ��5L      引导线平移��60   ��5M      停止线平移��6/   ��5N      
更多��6*   ��5O      
标定��6-   ��5P      激光设备操作��6=   ��5Q      机型测试��61   ��5R      确认是否要退出？��6'   ��5S      正在进行水平标定��6'   ��5T      UQ等待左上方显示水平标定完成，可点击下方确定按钮关闭提示��6:   ��5U      /+已经存在,如需绘制，请先删除！��6.   ��5V      &"不存在,请先绘制引导线！��6+   ��5W      确认移动引导线？��6'   ��5X      确认移动停止线？��6'   ��5Y      "向右移动激光扫面区域��6N   ��5Z      "向下移动激光扫面区域��6E   ��5[      "向左移动激光扫面区域��6M   ��5\      "向上移动激光扫面区域��6C   ��5]      开始走步设置��67   ��5^      确认走步设置��69   ��5_      "对走步设置引导线编号��6X   ��5`      输入飞机型号��64   ��5a      "输入引导线停止线编号��69   ��5b      输入数字平移引导线��6D   ��5c      输入数字平移停止线��6C   ��5d      粗略回零��6.   ��5e      JF无法打开串口,请检测此串口是否有效或被其他占用！��67   ��5f      视觉参数保存成功！��6(   ��5g      !视觉参数保存失败！\n��6*   ��5h      "选择是，设置新的零点��6)   ��5i      选择否，取消操作��6(   ��5j      串口已丢失��6%   ��5k      ($无法接收数据，原因未知！��6,   ��5l      ($无法关闭串口，原因未知！��6,   ��5m      已经存在��6$   ��5n      %!请勿修改选线的基本属性��6+   ��5o      相机开启失败��6&   ��5p      相机初始化失败��6'   ��5q      执行成功��6$   ��5r      执行失败��6$   ��5s      引导信息保存成功！��6)   ��5t      引导信息保存失败！��6)   ��5u      串口连接��6$   ��5v      系统参数配置��6&   ��5w      设置安装高度��6&   ��5x      关联机型��6$   ��5y      捕获区域设置��6&   ��5z      跟踪区域设置��6&   ��5{      引导线设置��6%   ��5|      
问题��6"   ��5}      保存配置文件��6&   ��5~      捕获距离��6$   ��5      设置日期范围��6&   ��5�      关键字查找��6%   ��5�      泊位记录文件列表��6(   ��5�      边框:��6#   ��5�      线段颜色��6$   ��5�      线段宽度��6$   ��5�      
确定��6"   ��5�      
关闭��6"   ��5�      
绘制��6"   ��5�      
恢复��6"   5�      