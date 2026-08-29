using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Markup;
using System.Windows.Media;

namespace DS2ModSuite
{
    internal static class Theme
    {
        public static readonly SolidColorBrush Background = Brush("#0F1217");
        public static readonly SolidColorBrush Surface = Brush("#171C24");
        public static readonly SolidColorBrush SurfaceRaised = Brush("#1D2430");
        public static readonly SolidColorBrush SurfaceHover = Brush("#222A36");
        public static readonly SolidColorBrush Border = Brush("#303947");
        public static readonly SolidColorBrush TextPrimary = Brush("#F3F6F9");
        public static readonly SolidColorBrush TextSecondary = Brush("#AAB4C1");
        public static readonly SolidColorBrush Accent = Brush("#49A9DD");
        public static readonly SolidColorBrush AccentHover = Brush("#65B9E5");
        public static readonly SolidColorBrush Success = Brush("#42C98A");
        public static readonly SolidColorBrush Warning = Brush("#F1B84B");
        public static readonly SolidColorBrush Danger = Brush("#EE7070");
        public static readonly SolidColorBrush NeutralBadge = Brush("#2A3340");

        public static ResourceDictionary CreateResources()
        {
            const string xaml = @"
<ResourceDictionary xmlns=""http://schemas.microsoft.com/winfx/2006/xaml/presentation""
                    xmlns:x=""http://schemas.microsoft.com/winfx/2006/xaml"">
    <Style x:Key=""PrimaryButton"" TargetType=""Button"">
        <Setter Property=""Foreground"" Value=""#07131A""/>
        <Setter Property=""Background"" Value=""#49A9DD""/>
        <Setter Property=""BorderBrush"" Value=""#49A9DD""/>
        <Setter Property=""BorderThickness"" Value=""1""/>
        <Setter Property=""FontFamily"" Value=""Segoe UI""/>
        <Setter Property=""FontSize"" Value=""14""/>
        <Setter Property=""FontWeight"" Value=""SemiBold""/>
        <Setter Property=""Padding"" Value=""18,9""/>
        <Setter Property=""MinHeight"" Value=""42""/>
        <Setter Property=""Cursor"" Value=""Hand""/>
        <Setter Property=""Template"">
            <Setter.Value>
                <ControlTemplate TargetType=""Button"">
                    <Border x:Name=""Chrome"" Background=""{TemplateBinding Background}""
                            BorderBrush=""{TemplateBinding BorderBrush}""
                            BorderThickness=""{TemplateBinding BorderThickness}""
                            CornerRadius=""8"" SnapsToDevicePixels=""True"">
                        <ContentPresenter HorizontalAlignment=""Center"" VerticalAlignment=""Center""
                                          Margin=""{TemplateBinding Padding}""/>
                    </Border>
                    <ControlTemplate.Triggers>
                        <Trigger Property=""IsMouseOver"" Value=""True"">
                            <Setter TargetName=""Chrome"" Property=""Background"" Value=""#65B9E5""/>
                        </Trigger>
                        <Trigger Property=""IsPressed"" Value=""True"">
                            <Setter TargetName=""Chrome"" Property=""Background"" Value=""#328BB8""/>
                        </Trigger>
                        <Trigger Property=""IsKeyboardFocused"" Value=""True"">
                            <Setter TargetName=""Chrome"" Property=""BorderBrush"" Value=""#D6F0FC""/>
                            <Setter TargetName=""Chrome"" Property=""BorderThickness"" Value=""2""/>
                        </Trigger>
                        <Trigger Property=""IsEnabled"" Value=""False"">
                            <Setter TargetName=""Chrome"" Property=""Background"" Value=""#26303A""/>
                            <Setter TargetName=""Chrome"" Property=""BorderBrush"" Value=""#303947""/>
                            <Setter Property=""Foreground"" Value=""#71808F""/>
                            <Setter Property=""Cursor"" Value=""Arrow""/>
                        </Trigger>
                    </ControlTemplate.Triggers>
                </ControlTemplate>
            </Setter.Value>
        </Setter>
    </Style>

    <Style x:Key=""SecondaryButton"" TargetType=""Button"">
        <Setter Property=""Foreground"" Value=""#E7ECF1""/>
        <Setter Property=""Background"" Value=""#1D2430""/>
        <Setter Property=""BorderBrush"" Value=""#3A4554""/>
        <Setter Property=""BorderThickness"" Value=""1""/>
        <Setter Property=""FontFamily"" Value=""Segoe UI""/>
        <Setter Property=""FontSize"" Value=""13""/>
        <Setter Property=""FontWeight"" Value=""SemiBold""/>
        <Setter Property=""Padding"" Value=""14,8""/>
        <Setter Property=""MinHeight"" Value=""38""/>
        <Setter Property=""Cursor"" Value=""Hand""/>
        <Setter Property=""Template"">
            <Setter.Value>
                <ControlTemplate TargetType=""Button"">
                    <Border x:Name=""Chrome"" Background=""{TemplateBinding Background}""
                            BorderBrush=""{TemplateBinding BorderBrush}""
                            BorderThickness=""{TemplateBinding BorderThickness}""
                            CornerRadius=""7"" SnapsToDevicePixels=""True"">
                        <ContentPresenter HorizontalAlignment=""Center"" VerticalAlignment=""Center""
                                          Margin=""{TemplateBinding Padding}""/>
                    </Border>
                    <ControlTemplate.Triggers>
                        <Trigger Property=""IsMouseOver"" Value=""True"">
                            <Setter TargetName=""Chrome"" Property=""Background"" Value=""#27313E""/>
                            <Setter TargetName=""Chrome"" Property=""BorderBrush"" Value=""#4B596A""/>
                        </Trigger>
                        <Trigger Property=""IsPressed"" Value=""True"">
                            <Setter TargetName=""Chrome"" Property=""Background"" Value=""#121820""/>
                        </Trigger>
                        <Trigger Property=""IsKeyboardFocused"" Value=""True"">
                            <Setter TargetName=""Chrome"" Property=""BorderBrush"" Value=""#8ACCF0""/>
                            <Setter TargetName=""Chrome"" Property=""BorderThickness"" Value=""2""/>
                        </Trigger>
                        <Trigger Property=""IsEnabled"" Value=""False"">
                            <Setter Property=""Opacity"" Value=""0.48""/>
                            <Setter Property=""Cursor"" Value=""Arrow""/>
                        </Trigger>
                    </ControlTemplate.Triggers>
                </ControlTemplate>
            </Setter.Value>
        </Setter>
    </Style>

    <Style x:Key=""ModCheckBox"" TargetType=""CheckBox"">
        <Setter Property=""Width"" Value=""24""/>
        <Setter Property=""Height"" Value=""24""/>
        <Setter Property=""Cursor"" Value=""Hand""/>
        <Setter Property=""Template"">
            <Setter.Value>
                <ControlTemplate TargetType=""CheckBox"">
                    <Grid Width=""24"" Height=""24"" Background=""Transparent"">
                        <Border x:Name=""Box"" Width=""20"" Height=""20"" CornerRadius=""5""
                                HorizontalAlignment=""Center"" VerticalAlignment=""Center""
                                Background=""#10151B"" BorderBrush=""#526071"" BorderThickness=""1.5"">
                            <Path x:Name=""Tick"" Data=""M 3.5 8 L 7.2 11.6 L 13.8 4.8""
                                  Stroke=""#07131A"" StrokeThickness=""2.2"" StrokeStartLineCap=""Round""
                                  StrokeEndLineCap=""Round"" Visibility=""Collapsed""/>
                        </Border>
                    </Grid>
                    <ControlTemplate.Triggers>
                        <Trigger Property=""IsChecked"" Value=""True"">
                            <Setter TargetName=""Box"" Property=""Background"" Value=""#49A9DD""/>
                            <Setter TargetName=""Box"" Property=""BorderBrush"" Value=""#65B9E5""/>
                            <Setter TargetName=""Tick"" Property=""Visibility"" Value=""Visible""/>
                        </Trigger>
                        <Trigger Property=""IsMouseOver"" Value=""True"">
                            <Setter TargetName=""Box"" Property=""BorderBrush"" Value=""#8ACCF0""/>
                        </Trigger>
                        <Trigger Property=""IsKeyboardFocused"" Value=""True"">
                            <Setter TargetName=""Box"" Property=""BorderBrush"" Value=""#F3F6F9""/>
                            <Setter TargetName=""Box"" Property=""BorderThickness"" Value=""2""/>
                        </Trigger>
                        <Trigger Property=""IsEnabled"" Value=""False"">
                            <Setter Property=""Opacity"" Value=""0.45""/>
                        </Trigger>
                    </ControlTemplate.Triggers>
                </ControlTemplate>
            </Setter.Value>
        </Setter>
    </Style>

    <Style x:Key=""SettingsCheckBox"" TargetType=""CheckBox"">
        <Setter Property=""Foreground"" Value=""#E7ECF1""/>
        <Setter Property=""FontFamily"" Value=""Segoe UI""/>
        <Setter Property=""FontSize"" Value=""13""/>
        <Setter Property=""Cursor"" Value=""Hand""/>
        <Setter Property=""Template"">
            <Setter.Value>
                <ControlTemplate TargetType=""CheckBox"">
                    <StackPanel Orientation=""Horizontal"" Background=""Transparent"">
                        <Border x:Name=""Box"" Width=""18"" Height=""18"" CornerRadius=""4""
                                Background=""#10151B"" BorderBrush=""#526071"" BorderThickness=""1.5""
                                VerticalAlignment=""Center"">
                            <Path x:Name=""Tick"" Data=""M 3 7 L 6.2 10 L 12.7 3.8"" Stroke=""#07131A""
                                  StrokeThickness=""2"" StrokeStartLineCap=""Round"" StrokeEndLineCap=""Round""
                                  Visibility=""Collapsed""/>
                        </Border>
                        <ContentPresenter Margin=""8,0,0,0"" VerticalAlignment=""Center""/>
                    </StackPanel>
                    <ControlTemplate.Triggers>
                        <Trigger Property=""IsChecked"" Value=""True"">
                            <Setter TargetName=""Box"" Property=""Background"" Value=""#49A9DD""/>
                            <Setter TargetName=""Box"" Property=""BorderBrush"" Value=""#65B9E5""/>
                            <Setter TargetName=""Tick"" Property=""Visibility"" Value=""Visible""/>
                        </Trigger>
                        <Trigger Property=""IsMouseOver"" Value=""True"">
                            <Setter TargetName=""Box"" Property=""BorderBrush"" Value=""#8ACCF0""/>
                        </Trigger>
                        <Trigger Property=""IsKeyboardFocused"" Value=""True"">
                            <Setter TargetName=""Box"" Property=""BorderBrush"" Value=""#F3F6F9""/>
                        </Trigger>
                        <Trigger Property=""IsEnabled"" Value=""False"">
                            <Setter Property=""Opacity"" Value=""0.45""/>
                        </Trigger>
                    </ControlTemplate.Triggers>
                </ControlTemplate>
            </Setter.Value>
        </Setter>
    </Style>

    <Style x:Key=""DarkComboBoxItem"" TargetType=""ComboBoxItem"">
        <Setter Property=""Foreground"" Value=""#E7ECF1""/>
        <Setter Property=""Background"" Value=""#1D2430""/>
        <Setter Property=""Padding"" Value=""10,7""/>
        <Setter Property=""HorizontalContentAlignment"" Value=""Stretch""/>
        <Setter Property=""Template"">
            <Setter.Value>
                <ControlTemplate TargetType=""ComboBoxItem"">
                    <Border x:Name=""ItemChrome"" Background=""{TemplateBinding Background}"" Padding=""{TemplateBinding Padding}"">
                        <ContentPresenter/>
                    </Border>
                    <ControlTemplate.Triggers>
                        <Trigger Property=""IsHighlighted"" Value=""True"">
                            <Setter TargetName=""ItemChrome"" Property=""Background"" Value=""#27313E""/>
                        </Trigger>
                        <Trigger Property=""IsSelected"" Value=""True"">
                            <Setter TargetName=""ItemChrome"" Property=""Background"" Value=""#23475A""/>
                            <Setter Property=""Foreground"" Value=""#F3F6F9""/>
                        </Trigger>
                    </ControlTemplate.Triggers>
                </ControlTemplate>
            </Setter.Value>
        </Setter>
    </Style>

    <Style TargetType=""ComboBox"">
        <Setter Property=""Foreground"" Value=""#E7ECF1""/>
        <Setter Property=""Background"" Value=""#1D2430""/>
        <Setter Property=""BorderBrush"" Value=""#3A4554""/>
        <Setter Property=""BorderThickness"" Value=""1""/>
        <Setter Property=""Padding"" Value=""10,5""/>
        <Setter Property=""ItemContainerStyle"" Value=""{StaticResource DarkComboBoxItem}""/>
        <Setter Property=""Template"">
            <Setter.Value>
                <ControlTemplate TargetType=""ComboBox"">
                    <Grid>
                        <ToggleButton x:Name=""Toggle"" Focusable=""False"" ClickMode=""Press""
                                      Foreground=""{TemplateBinding Foreground}""
                                      Background=""{TemplateBinding Background}""
                                      BorderBrush=""{TemplateBinding BorderBrush}""
                                      BorderThickness=""{TemplateBinding BorderThickness}""
                                      Padding=""{TemplateBinding Padding}""
                                      IsChecked=""{Binding IsDropDownOpen, RelativeSource={RelativeSource TemplatedParent}, Mode=TwoWay}"">
                            <ToggleButton.Template>
                                <ControlTemplate TargetType=""ToggleButton"">
                                    <Border x:Name=""ComboChrome"" Background=""{TemplateBinding Background}""
                                            BorderBrush=""{TemplateBinding BorderBrush}"" BorderThickness=""{TemplateBinding BorderThickness}""
                                            CornerRadius=""7"" SnapsToDevicePixels=""True"">
                                        <Grid>
                                            <Grid.ColumnDefinitions>
                                                <ColumnDefinition Width=""*""/>
                                                <ColumnDefinition Width=""28""/>
                                            </Grid.ColumnDefinitions>
                                            <ContentPresenter Margin=""{TemplateBinding Padding}"" VerticalAlignment=""Center""
                                                              HorizontalAlignment=""Left"" Content=""{TemplateBinding Content}""
                                                              ContentTemplate=""{TemplateBinding ContentTemplate}""/>
                                            <Path Grid.Column=""1"" Data=""M 0 0 L 4 4 L 8 0"" Stroke=""#AAB4C1""
                                                  StrokeThickness=""1.5"" HorizontalAlignment=""Center"" VerticalAlignment=""Center""/>
                                        </Grid>
                                    </Border>
                                    <ControlTemplate.Triggers>
                                        <Trigger Property=""IsMouseOver"" Value=""True"">
                                            <Setter TargetName=""ComboChrome"" Property=""Background"" Value=""#27313E""/>
                                            <Setter TargetName=""ComboChrome"" Property=""BorderBrush"" Value=""#4B596A""/>
                                        </Trigger>
                                        <Trigger Property=""IsKeyboardFocused"" Value=""True"">
                                            <Setter TargetName=""ComboChrome"" Property=""BorderBrush"" Value=""#8ACCF0""/>
                                        </Trigger>
                                    </ControlTemplate.Triggers>
                                </ControlTemplate>
                            </ToggleButton.Template>
                            <ContentPresenter Content=""{TemplateBinding SelectionBoxItem}""
                                              ContentTemplate=""{TemplateBinding SelectionBoxItemTemplate}""
                                              VerticalAlignment=""Center"" HorizontalAlignment=""Left""/>
                        </ToggleButton>
                        <Popup x:Name=""PART_Popup"" Placement=""Bottom"" IsOpen=""{TemplateBinding IsDropDownOpen}""
                               AllowsTransparency=""True"" Focusable=""False"" PopupAnimation=""Fade"">
                            <Border Margin=""0,4,0,0"" MinWidth=""{TemplateBinding ActualWidth}"" MaxHeight=""320""
                                    Background=""#1D2430"" BorderBrush=""#3A4554"" BorderThickness=""1"" CornerRadius=""7"">
                                <ScrollViewer Margin=""1"" SnapsToDevicePixels=""True"">
                                    <ItemsPresenter/>
                                </ScrollViewer>
                            </Border>
                        </Popup>
                    </Grid>
                    <ControlTemplate.Triggers>
                        <Trigger Property=""IsEnabled"" Value=""False"">
                            <Setter Property=""Opacity"" Value=""0.48""/>
                        </Trigger>
                    </ControlTemplate.Triggers>
                </ControlTemplate>
            </Setter.Value>
        </Setter>
    </Style>

    <Style TargetType=""ScrollBar"">
        <Setter Property=""Background"" Value=""#12171E""/>
        <Setter Property=""Foreground"" Value=""#465363""/>
        <Setter Property=""Width"" Value=""10""/>
    </Style>
</ResourceDictionary>";
            return (ResourceDictionary)XamlReader.Parse(xaml);
        }

        public static SolidColorBrush Brush(string value)
        {
            SolidColorBrush brush = new SolidColorBrush((Color)ColorConverter.ConvertFromString(value));
            brush.Freeze();
            return brush;
        }

        public static TextBlock Text(string value, double size, Brush foreground, FontWeight weight)
        {
            return new TextBlock
            {
                Text = value,
                FontFamily = new FontFamily("Segoe UI"),
                FontSize = size,
                Foreground = foreground,
                FontWeight = weight,
                TextWrapping = TextWrapping.Wrap
            };
        }

        public static Button Button(string text, bool primary)
        {
            return new Button
            {
                Content = text,
                Style = (Style)Application.Current.Resources[primary ? "PrimaryButton" : "SecondaryButton"]
            };
        }
    }
}
