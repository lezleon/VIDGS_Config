   M S B A M L   `   `   `  ����   VIDGS配置软件'    !VIDGS配置软件.SystemParameter   \ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35^ ZWindowsFormsIntegration, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35F 9http://schemas.microsoft.com/winfx/2006/xaml/presentation     R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     X TXceed.Wpf.Toolkit, Version=2.5.0.0, Culture=neutral, PublicKeyToken=3e4669d2f30244f44xctk)http://schemas.xceed.com/wpf/xaml/toolkit  -     � Title$  系统属性��$��567.076��$��523.266�� � 
ResizeMode$ NoResize=� � 	Resources    TextDataTemplate �� Mode �� UpdateSourceTrigger ^� ToolTip  EnableTextBlockTemplate �� TextWrapping �� SelectionBrush��%  &	        &	 x     �� .R��� $
��Wrap=�$
��auto������ *Description+$ TwoWay=�$ PropertyChanged=� �� *Description+�� .R��� $
 Wrap=�$��0q�
 ��������� *Value+$ TwoWay=�$ PropertyChanged=�{��� *	Writeable+.���  � ColumnDefinitions �� $
��168* ��� $
��127* �.+��� $
��Left=�$	��488��$	��Top=�$	��517��$��2��$��
0,0,-0.2,0q�.@��� $��系统参数��.��� $��0,0,0,0q�.+�&   System.Windows.Controls.DataGrid  -   	  Name$	 SystemParaDataGrid��$
��Left=�$��	10,10,0,0q�$	��Top=�$	��419��$	��491��
   GridLinesVisibility$

 None=�   AutoGenerateColumns .     SelectionUnit$ CellOrRowHeader=�
�������   Columns 0  *System.Windows.Controls.DataGridTextColumn    parametername54,  &System.Windows.Controls.DataGridColumn   Header# �     Width5  /System.Windows.Controls.DataGridLengthConverter$
 1.8*    
IsReadOnly . 1  +System.Windows.Controls.DataGridBoundColumn   Binding �� *Name+$ TwoWay=�$ PropertyChanged=�4  .System.Windows.Controls.DataGridTemplateColumn    value56# �  $ 1*    CellTemplate# [     Description57# �  $ 2* # [  �� $��视觉参数��.��� .+�  -   $	 VisionParaDataGrid��$
��Left=�$��	10,10,0,0q�$	��Top=�$	��419��$	��491�� .     CanUserAddRows .    CanUserDeleteRows . $ CellOrRowHeader=�   MinRowHeight$ 10Q�   SelectionMode$ Single=�   AreRowDetailsFrozen . $
 
Horizontal=�   # �  $
 1.8*  .  �� *Name+$ TwoWay=�$ PropertyChanged=�  # �  $ 1* # [   # �  $ 2* # [  �� $��激光参数��.��� .+�  -   $	 LaserParaDataGrid��$
��Left=�$��	10,10,0,0q�$	��Top=�$	��419��$	��491�� .   .  . $ CellOrRowHeader=�$ 20Q�$ Single=�   RowDetailsVisibilityMode$ Visible=�   # �  $
 1.8*  .  �� *Name+$ TwoWay=�$ PropertyChanged=�  # �  $ 1* # [   # �  $ 2* # [  �� -    ��Name$ SubmitButton��  	confirm64#���  $
��Left=�$��294,488,0,0q�$	��Top=�$��94��$��40��$��2���� -   $ ApplyButton��  Apply63#���  $
��Left=�$��185,488,0,0q�$	��Top=�$��95��$��40���� -   $ CancelButton��  Cancel65#���  $��Right=�$��0,488,17.8,0q�$	��Top=�$��95��$��1��$��40��