   M S B A M L   `   `   `  ���� L  HVIDGS配置软件, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null5clr-namespace:VIDGS配置软件VIDGS配置软件      VIDGS配置软件.DropBox   \ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35^ ZWindowsFormsIntegration, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35F 9http://schemas.microsoft.com/winfx/2006/xaml/presentation     R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     1localclr-namespace:VIDGS配置软件       � Title$  	停止线��$	��280��$	��317�� � AllowsTransparency . 
������  � WindowStyle$
 None=� � 
ResizeMode$ CanResizeWithGrip=� � 	Resources    VIDGS配置软件.GNumArr   GuideNum   VIDGS配置软件.SNumArr  StopNum��%   &	        &	          .���� -   $��windowFrame��$	��294��$
��Auto�����#395984$��1q� �� CornerRadius$ 30,30,30,30������ ��� �  � Color$ #E7EBF7�� � Offset$	 0.0Q�� $ #CEE3FF��$	 0.5Q�._�� $	��290��  �� 
TargetType	 �� Property
 �� Value��%G   '��       �� " ��.��� 	 � ��$
 16(��� 	 � ��$
 Center=� � RowDefinitions �� $��20 ��� $��40 ��� $��40 ��� $��* ��� $��70 � � ColumnDefinitions �� $��95 ��� $��* �.+��� -   $��1q� �� Padding$ 5q�$��0��$��2���� -    ��Name$ ForDependencyGL��$��18(�$��1��$��0��$��    引导线���� -   $ ForDependencySL��$��18(�$��    停止线��$��2��$��0���� -    ��Name$ drp1Name��$��18(� �� SelectedIndex$ 0��$��1��$��1��$��30��$��0,0,25,0q�#��[   �� DisplayMemberPath$ Guide�� �� SelectedValuePath$ Guide���� -   $ drpAge��$��18(�$ 0��$��2��$��1��$��30��$��0,0,25,0q�#��[ $
 Stop��$
 Stop��� $��3��$��Center=�$��2��$��Center=� �� $
��Auto ���  �� $
��Auto ��� $
��Auto �.+��� $��18(�$��引导线：���� -   $ tbName��$��1��$��18(�����  �� ElementName$ drp1Name�� �� Path$ SelectedValue� �� Mode$ TwoWay=��� $��1��$��18(�$��停止线：���� -   $ tbAge��$��1��$��1��$��18(����� $ drpAge��$ SelectedValue�$ TwoWay=�� $��3��$��Center=�$��2��$��Center=��� -	   $	��Top=�$��Center=�$��10q�$��46��$��	0,9,130,0q�$��61��$��21(�$��1��$��4��$��确定���� -
   $	��Top=�$��Center=�$��10q�$��46��$��	80,9,35,0q�$��61��$��21(�$��1��$��4��$��关闭��