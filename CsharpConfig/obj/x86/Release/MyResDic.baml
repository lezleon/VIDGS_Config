   M S B A M L   `   `   `  ���� L  HVIDGS配置软件, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null5clr-namespace:VIDGS配置软件VIDGS配置软件  \ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35^ ZWindowsFormsIntegration, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089 !  MyScrollViewerControlTemplate  �� 
TargetType �Name � ColumnDefinitions � RowDefinitions ��Name ��Name �� CanContentScroll �� CanHorizontallyScroll �� CanVerticallyScroll	 ��Name4   .System.Windows.Automation.AutomationProperties
    AutomationId � Cursor �� ScrollableHeight � Maximum � Minimum* �� #ComputedVerticalScrollBarVisibility �� ViewportHeight �� ViewportSize  MyScrollBarStyle � Value �� Mode �� RelativeSource �� ScrollableWidth �� Orientation, �� %ComputedHorizontalScrollBarVisibility �� ViewportWidth  ScrollBarDisabledBackground �� Color  VerticalScrollBarPageButton �� 
TargetType �� Property �� Value  ScrollBarThumb �� RadiusX �� RadiusY �� Triggers  P� Property! P� Value" �� 
TargetName ! HorizontalScrollBarPageButton# J� SnapsToDevicePixels$ ]�Name% J� IsMouseOver& ]� DecreaseRepeatButton' �� PageUpCommand( ]� IncreaseRepeatButton) �� PageDownCommand* ]� Thumb+ �� Triggers, �� PageLeftCommand- �� PageRightCommand  SelfMenuItemTemplate. �� Orientation  PackageTemplate/ �� ImageSource  package_stb0 �� 	Converter 	 ScenceTemplate 
 
scence_stb�� F 9http://schemas.microsoft.com/winfx/2006/xaml/presentation     )localclr-namespace:VIDGS配置软件  :x,http://schemas.microsoft.com/winfx/2006/xaml     %g  &	        &	 �    &	 �    &	 P    &	 8    &	 �    7  7  7  7  7  7  7  7  &	 �    &	 �    7  &		 `
    7 
 �� "  ��.R�� $
 Grid��#��z�� �� $��* ��� $
��Auto � �� $��* ��� $
��Auto �.+��� $ Corner��$��1��
�������$��1���� $! PART_ScrollContentPresenter��# z�� .   .  #��z��#��z��$��0��#��z��$��0���� $	 PART_VerticalScrollBar��$
 VerticalScrollBar��$ Arrow��$��1��# z $ 0Q�$��0��#y�z # z #���   �� *VerticalOffset+$ OneWay=� �� *TemplatedParent+�� $	 PART_HorizontalScrollBar��$
 HorizontalScrollBar��$ Arrow��$��0��# z $ 0Q�$ 
Horizontal=�$��1��#y�z # z #���   �� *HorizontalOffset+$ OneWay=� �� *TemplatedParent+�� $ #F4F4F4���� " ��.���  �@�OverridesDefaultStyle . ��  � ��
 ���� ��  � |� .  ��  � �� .  ��  � �� �� "  ��.R��� #��z��#��z��#��z���� " n�.���  �@�OverridesDefaultStyle . ��  � �� .  ��  � �� �� "  n�.R��� $��	thumbRect��
����d�$ 3��$ 3�� P�   �@J�IsMouseOver! . . ��� $" 	thumbRect�� � ��
 ��  ��� " ��.���  �@�OverridesDefaultStyle . ��  � ��
 ���� ��  � |� .  ��  � �� .  ��  � �� �� "  ��.R��� #��z��#��z��#��z���� " ��.���  � ��
 �������  �@��IsPressAndHoldEnabled .  ��  �@��IsFlicksEnabled .  ��  � ��$ 6����  � ��$ 6����  � �� �� "  ��.R�� $ Bg��
�������# . $��6�� �� .+�]� $$ 
PART_Track����. #{�z% & �� #��Z' 8��  ( �� #��Z) 8�� * n� 8��  P�   � {�! .  . ��� $" Bg��8   � ��+ P�   �@��Orientation$! 
Horizontal=�. ���  � ��$
 Auto����  � ��$ 0����  � ��$ 6����  � ��$ 6����  � ��
 �������  � �� �� "  ��.R�� $ Bg��
�������# .  �� .+�]� $$ 
PART_Track��#{�z% & �� #��Z, 8�� ( �� #��Z- 8�� * n� 8��  P�   � {�! .  . ��� $" Bg��8   � ���� "  m�.R��� 
������ $��25��._��� $. 
Horizontal=�.+��� $��	mousemark��$��5��$��25��
��������� $	��5,0q�$��Center=����� *Header+ �� *TemplatedParent+ P�   �@J�IsMouseOver! . . ��� $" 	mousemark�� � ��
 ��  ��� .R��  �� $	��182 ��� $��20 ��� $��20 �.+��� $	��180��$	��182��$��0������ / �� *
IsSelected+80   ._��� $	��150��$	��150��$
��Fill=�$��13,13q�$
��Left=�$	��Top=����� *	ImagePath+�� $��1��$��Center=�$��Center=�$
��Bold"�$��14(����� *PackageName+�� $��2��$��Center=�$��Center=����� *NumStr+�� .R��� $	��168��$	��170������ / �� *
IsSelected+80   ._��� $	��150��$	��150��$
��Fill=����� *	ImagePath+