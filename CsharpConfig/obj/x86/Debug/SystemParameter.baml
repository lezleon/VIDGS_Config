   M S B A M L   `   `   `  ����  VIDGS配置软件'    !VIDGS配置软件.SystemParameter   5   	   R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35\ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35^ ZWindowsFormsIntegration, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35F 9http://schemas.microsoft.com/winfx/2006/xaml/presentation     5   	   R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     5   	   :xctk)http://schemas.xceed.com/wpf/xaml/toolkit     5   	   -   5          � Title$  系统属性��5   	   $��567.076��6   $��523.266��6'    � 
ResizeMode$ NoResize=�6N    � 	Resources 5         TextDataTemplate �� Mode �� UpdateSourceTrigger ^� ToolTip  EnableTextBlockTemplate �� TextWrapping �� SelectionBrush��%g  &	        &	 �     �� 5   
   .R�5      �� $
��Wrap=�6   $
��auto��6,   ��6:   �� *Description+$ TwoWay=�$ PropertyChanged=� 6�   �� *Description+5	      �� 5
   
   .R�5      �� $
 Wrap=�6   $��0q�6�   
 �������6*   �� *Value+$ TwoWay=�$ PropertyChanged=�{�6s   �� *	Writeable+6�   5      5      .��5      �  � ColumnDefinitions 5   
   �� 5      $
��168* �6   �� 5      $
��127* �6   .+�5   
   �� $
��Left=�6   $	��488��60   $	��Top=�6=   $	��517��6U   $��2��6a   $��
0,0,-0.2,0q�6u   .@�5      �� $��系统参数��6   .��5      � $��0,0,0,0q�6   .+�5      &   System.Windows.Controls.DataGrid  -   5        	  Name$	 SystemParaDataGrid��5      $
��Left=�6;   $��	10,10,0,0q�6V   $	��Top=�6i   $	��419��6�   $	��491��6�   
   GridLinesVisibility$

 None=�6�      AutoGenerateColumns .     SelectionUnit$ CellOrRowHeader=�6�   
�������   Columns 5      0  *System.Windows.Controls.DataGridTextColumn  5        parametername54,  &System.Windows.Controls.DataGridColumn   Header# �  61      Width5  /System.Windows.Controls.DataGridLengthConverter$
 1.8* 6\      
IsReadOnly . 1  +System.Windows.Controls.DataGridBoundColumn   Binding 6i   �� *Name+$ TwoWay=�$ PropertyChanged=�6�   4  .System.Windows.Controls.DataGridTemplateColumn  5        value56# �  65   $ 1* 6X      CellTemplate# [ 6c     5        Description57# �  66   $ 2* 6_   # [  6j   5      5      5      �� 5      $��视觉参数��6   .��5       � .+�5!        -   5        $	 VisionParaDataGrid��5!      $
��Left=�6;   $��	10,10,0,0q�6V   $	��Top=�6i   $	��419��6�   $	��491��6�    .     CanUserAddRows .    CanUserDeleteRows . $ CellOrRowHeader=�6�      MinRowHeight$ 10Q�6     SelectionMode$ Single=�6     AreRowDetailsFrozen . $
 
Horizontal=�6z   5"        5#      # �  61   $
 1.8* 6\    .  6i   �� *Name+$ TwoWay=�$ PropertyChanged=�6�     5$      # �  65   $ 1* 6X   # [ 6c     5%      # �  66   $ 2* 6_   # [  6j   5'      5(      5)      �� 5*      $��激光参数��6   .��5+      � .+�5,        -   5        $	 LaserParaDataGrid��5,      $
��Left=�6:   $��	10,10,0,0q�6U   $	��Top=�6h   $	��419��6�   $	��491��6�    .   .  . $ CellOrRowHeader=�6�   $ 20Q�6  $ Single=�6     RowDetailsVisibilityMode$ Visible=�6]   5-        5.      # �  61   $
 1.8* 6\    .  6i   �� *Name+$ TwoWay=�$ PropertyChanged=�6�     5/      # �  65   $ 1* 6X   # [ 6c     50      # �  66   $ 2* 6_   # [  6j   52      53      54      55      �� 59   
   -   5         ��Name$ SubmitButton��59        	confirm64#���  6'   $
��Left=�6M   $��294,488,0,0q�6h   $	��Top=�6}   $��94��6�   $��40��6�   $��2��6�   �� 5:   
   -   5        $ ApplyButton��5:        Apply63#���  6&   $
��Left=�6J   $��185,488,0,0q�6e   $	��Top=�6z   $��95��6�   $��40��6�   �� 5;   
   -   5        $ CancelButton��5;        Cancel65#���  6'   $��Right=�6L   $��0,488,17.8,0q�6h   $	��Top=�6~   $��95��6�   $��1��6�   $��40��6�   6�   5>      5?      