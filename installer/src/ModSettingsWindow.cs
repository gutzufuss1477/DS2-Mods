using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Windows;
using System.Windows.Automation;
using System.Windows.Controls;
using System.Windows.Media;

namespace DS2ModSuite
{
    internal sealed class ModChoice
    {
        public ModSpec Mod { get; set; }

        public override string ToString()
        {
            return Mod == null ? string.Empty : Mod.Name;
        }
    }

    internal sealed class ModSettingsWindow : Window
    {
        private readonly Catalog catalog;
        private readonly string gamePath;
        private readonly List<ConfigFieldDefinition> definitions;
        private readonly HashSet<string> installedModIds;
        private readonly Dictionary<string, Control> editors = new Dictionary<string, Control>(StringComparer.OrdinalIgnoreCase);
        private ModConfigurationProfile profile;
        private ComboBox modSelector;
        private CheckBox showAdvanced;
        private StackPanel settingsPanel;
        private TextBlock noticeText;
        private Button importButton;

        public ModConfigurationProfile ResultProfile { get; private set; }

        public ModSettingsWindow(Catalog catalog, ModConfigurationProfile initialProfile, string gamePath, IEnumerable<string> installedModIds)
        {
            this.catalog = catalog ?? throw new ArgumentNullException("catalog");
            this.gamePath = gamePath;
            definitions = ModConfigurationService.GetDefinitions(catalog);
            profile = ModConfigurationService.CloneProfile(initialProfile) ?? ModConfigurationService.LoadEffectiveProfile(catalog, gamePath);
            this.installedModIds = new HashSet<string>(installedModIds ?? Enumerable.Empty<string>(), StringComparer.OrdinalIgnoreCase);

            Title = Localization.T("DS2 Mod Suite — Mod Settings", "DS2 Mod Suite — Mod-Einstellungen");
            Width = 920;
            Height = 740;
            MinWidth = 760;
            MinHeight = 600;
            WindowStartupLocation = WindowStartupLocation.CenterOwner;
            Background = Theme.Background;
            Foreground = Theme.TextPrimary;
            FontFamily = new FontFamily("Segoe UI");
            UseLayoutRounding = true;
            SnapsToDevicePixels = true;
            Content = BuildLayout();
        }

        internal static List<ModSpec> FilterInstalledConfigurableMods(
            Catalog catalog,
            IEnumerable<ConfigFieldDefinition> definitions,
            IEnumerable<string> installedModIds)
        {
            HashSet<string> installed = new HashSet<string>(installedModIds ?? Enumerable.Empty<string>(), StringComparer.OrdinalIgnoreCase);
            HashSet<string> configurable = new HashSet<string>(
                (definitions ?? Enumerable.Empty<ConfigFieldDefinition>()).Select(field => field.ModId),
                StringComparer.OrdinalIgnoreCase);
            return (catalog == null ? Enumerable.Empty<ModSpec>() : catalog.Mods ?? new List<ModSpec>())
                .Where(mod => mod != null && installed.Contains(mod.Id) && configurable.Contains(mod.Id))
                .ToList();
        }

        private UIElement BuildLayout()
        {
            Grid root = new Grid { Background = Theme.Background };
            root.RowDefinitions.Add(new RowDefinition { Height = GridLength.Auto });
            root.RowDefinitions.Add(new RowDefinition { Height = GridLength.Auto });
            root.RowDefinitions.Add(new RowDefinition { Height = new GridLength(1, GridUnitType.Star) });
            root.RowDefinitions.Add(new RowDefinition { Height = GridLength.Auto });

            Border header = new Border
            {
                Background = Theme.Surface,
                BorderBrush = Theme.Border,
                BorderThickness = new Thickness(0, 0, 0, 1),
                Padding = new Thickness(24, 20, 24, 18)
            };
            StackPanel heading = new StackPanel();
            heading.Children.Add(Theme.Text(Localization.T("Mod Settings", "Mod-Einstellungen"), 23, Theme.TextPrimary, FontWeights.SemiBold));
            TextBlock explanation = Theme.Text(Localization.T(
                "Manage settings for installed mods here. Apply writes the values to each mod's individual INI with backup and rollback protection.",
                "Verwalte hier die Einstellungen installierter Mods. Anwenden schreibt die Werte mit Backup- und Rollback-Schutz in die einzelnen INIs."),
                13, Theme.TextSecondary, FontWeights.Normal);
            explanation.Margin = new Thickness(0, 5, 0, 0);
            explanation.TextWrapping = TextWrapping.Wrap;
            heading.Children.Add(explanation);
            header.Child = heading;
            root.Children.Add(header);

            Grid toolbar = new Grid { Margin = new Thickness(24, 16, 24, 14) };
            toolbar.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(1, GridUnitType.Star) });
            toolbar.ColumnDefinitions.Add(new ColumnDefinition { Width = GridLength.Auto });
            StackPanel selectorStack = new StackPanel();
            selectorStack.Children.Add(Theme.Text(Localization.T("Installed mod", "Installierter Mod"), 11.5, Theme.TextSecondary, FontWeights.SemiBold));
            modSelector = CreateComboBox(360);
            modSelector.Margin = new Thickness(0, 5, 0, 0);
            foreach (ModSpec mod in FilterInstalledConfigurableMods(catalog, definitions, installedModIds))
                modSelector.Items.Add(new ModChoice { Mod = mod });
            bool hasAvailableMods = modSelector.Items.Count > 0;
            modSelector.IsEnabled = hasAvailableMods;
            modSelector.SelectionChanged += delegate { RenderSelectedMod(); };
            selectorStack.Children.Add(modSelector);
            toolbar.Children.Add(selectorStack);

            showAdvanced = new CheckBox
            {
                Content = Localization.T("Show advanced settings", "Erweiterte Einstellungen anzeigen"),
                Style = (Style)Application.Current.Resources["SettingsCheckBox"],
                VerticalAlignment = VerticalAlignment.Bottom,
                Margin = new Thickness(24, 0, 0, 7)
            };
            showAdvanced.IsEnabled = hasAvailableMods;
            showAdvanced.Checked += delegate { RenderSelectedMod(); };
            showAdvanced.Unchecked += delegate { RenderSelectedMod(); };
            Grid.SetColumn(showAdvanced, 1);
            toolbar.Children.Add(showAdvanced);
            toolbar.Visibility = hasAvailableMods ? Visibility.Visible : Visibility.Collapsed;
            Grid.SetRow(toolbar, 1);
            root.Children.Add(toolbar);

            ScrollViewer scroll = new ScrollViewer
            {
                VerticalScrollBarVisibility = ScrollBarVisibility.Auto,
                HorizontalScrollBarVisibility = ScrollBarVisibility.Disabled,
                Margin = new Thickness(24, hasAvailableMods ? 0 : 18, 17, 0),
                Focusable = false
            };
            settingsPanel = new StackPanel { Margin = new Thickness(0, 0, 7, 18) };
            scroll.Content = settingsPanel;
            Grid.SetRow(scroll, 2);
            root.Children.Add(scroll);

            Border footer = new Border
            {
                Background = Theme.Surface,
                BorderBrush = Theme.Border,
                BorderThickness = new Thickness(0, 1, 0, 0),
                Padding = new Thickness(24, 13, 24, 15)
            };
            Grid footerGrid = new Grid();
            footerGrid.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(1, GridUnitType.Star) });
            footerGrid.ColumnDefinitions.Add(new ColumnDefinition { Width = GridLength.Auto });

            StackPanel footerLeft = new StackPanel { VerticalAlignment = VerticalAlignment.Center };
            noticeText = Theme.Text(hasAvailableMods
                ? Localization.T(
                    "Changes take effect after Apply and require a game restart.",
                    "Änderungen werden nach Anwenden aktiv und benötigen einen Spielneustart.")
                : Localization.T(
                    "Install a configurable mod before editing its INI settings.",
                    "Installiere zuerst einen konfigurierbaren Mod, um dessen INI-Einstellungen zu bearbeiten."),
                12, Theme.TextSecondary, FontWeights.Normal);
            noticeText.TextWrapping = TextWrapping.Wrap;
            AutomationProperties.SetLiveSetting(noticeText, AutomationLiveSetting.Polite);
            footerLeft.Children.Add(noticeText);
            footerGrid.Children.Add(footerLeft);

            StackPanel buttons = new StackPanel { Orientation = Orientation.Horizontal, VerticalAlignment = VerticalAlignment.Center };
            importButton = Theme.Button(Localization.T("Import installed INIs", "Installierte INIs importieren"), false);
            importButton.Click += Import_Click;
            importButton.IsEnabled = hasAvailableMods && !string.IsNullOrWhiteSpace(gamePath) && Directory.Exists(gamePath);
            buttons.Children.Add(importButton);
            Button reset = Theme.Button(Localization.T("Reset this mod", "Diesen Mod zurücksetzen"), false);
            reset.Margin = new Thickness(8, 0, 0, 0);
            reset.Click += Reset_Click;
            reset.IsEnabled = hasAvailableMods;
            buttons.Children.Add(reset);
            Button cancel = Theme.Button(Localization.T("Cancel", "Abbrechen"), false);
            cancel.Margin = new Thickness(8, 0, 0, 0);
            cancel.Click += delegate { DialogResult = false; };
            buttons.Children.Add(cancel);
            Button save = Theme.Button(Localization.T("Save settings", "Einstellungen speichern"), true);
            save.Margin = new Thickness(8, 0, 0, 0);
            save.Click += Save_Click;
            save.IsEnabled = hasAvailableMods;
            buttons.Children.Add(save);
            Grid.SetColumn(buttons, 1);
            footerGrid.Children.Add(buttons);
            footer.Child = footerGrid;
            Grid.SetRow(footer, 3);
            root.Children.Add(footer);

            if (hasAvailableMods) modSelector.SelectedIndex = 0;
            else RenderSelectedMod();
            return root;
        }

        private void RenderSelectedMod()
        {
            CaptureEditors();
            editors.Clear();
            if (settingsPanel == null) return;
            settingsPanel.Children.Clear();
            ModChoice choice = modSelector == null ? null : modSelector.SelectedItem as ModChoice;
            if (choice == null || choice.Mod == null)
            {
                Border empty = CreateCard();
                empty.Padding = new Thickness(20, 18, 20, 18);
                StackPanel content = new StackPanel();
                content.Children.Add(Theme.Text(Localization.T(
                    "No installed mods with configurable settings",
                    "Keine installierten Mods mit konfigurierbaren Einstellungen"),
                    17, Theme.TextPrimary, FontWeights.SemiBold));
                TextBlock detail = Theme.Text(Localization.T(
                    "Install at least one mod that provides an INI file, apply the installation, and then reopen Mod Settings.",
                    "Installiere mindestens einen Mod mit eigener INI-Datei, wende die Installation an und öffne danach die Mod-Einstellungen erneut."),
                    12.8, Theme.TextSecondary, FontWeights.Normal);
                detail.Margin = new Thickness(0, 5, 0, 0);
                content.Children.Add(detail);
                empty.Child = content;
                settingsPanel.Children.Add(empty);
                return;
            }

            List<ConfigFieldDefinition> modFields = definitions.Where(field => string.Equals(field.ModId, choice.Mod.Id, StringComparison.OrdinalIgnoreCase)).ToList();
            TextBlock title = Theme.Text(choice.Mod.Name, 19, Theme.TextPrimary, FontWeights.SemiBold);
            title.Margin = new Thickness(1, 2, 0, 2);
            settingsPanel.Children.Add(title);
            TextBlock description = Theme.Text(choice.Mod.LocalizedDescription, 12.8, Theme.TextSecondary, FontWeights.Normal);
            description.TextWrapping = TextWrapping.Wrap;
            description.Margin = new Thickness(1, 0, 0, 14);
            settingsPanel.Children.Add(description);

            if (modFields.Count == 0)
            {
                Border empty = CreateCard();
                empty.Padding = new Thickness(18);
                empty.Child = Theme.Text(Localization.T(
                    "This mod has no configurable runtime settings.",
                    "Dieser Mod besitzt keine konfigurierbaren Laufzeiteinstellungen."), 13.5, Theme.TextSecondary, FontWeights.Normal);
                settingsPanel.Children.Add(empty);
                return;
            }

            bool includeAdvanced = showAdvanced != null && showAdvanced.IsChecked == true;
            foreach (IGrouping<string, ConfigFieldDefinition> file in modFields.GroupBy(field => field.Target, StringComparer.OrdinalIgnoreCase))
            {
                Border card = CreateCard();
                StackPanel cardPanel = new StackPanel();
                TextBlock fileName = Theme.Text(file.Key, 13.5, Theme.Accent, FontWeights.SemiBold);
                fileName.Margin = new Thickness(0, 0, 0, 8);
                cardPanel.Children.Add(fileName);

                foreach (IGrouping<string, ConfigFieldDefinition> section in file.GroupBy(field => field.Section, StringComparer.OrdinalIgnoreCase))
                {
                    TextBlock sectionName = Theme.Text("[" + section.Key + "]", 12, Theme.TextSecondary, FontWeights.SemiBold);
                    sectionName.Margin = new Thickness(0, 5, 0, 4);
                    cardPanel.Children.Add(sectionName);
                    foreach (ConfigFieldDefinition field in section)
                    {
                        if (field.Schema.Advanced && !includeAdvanced) continue;
                        cardPanel.Children.Add(BuildFieldRow(field));
                    }
                }
                card.Child = cardPanel;
                settingsPanel.Children.Add(card);
            }
        }

        private UIElement BuildFieldRow(ConfigFieldDefinition field)
        {
            Grid row = new Grid { Margin = new Thickness(0, 4, 0, 4) };
            row.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(1, GridUnitType.Star) });
            row.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(240) });
            StackPanel labelPanel = new StackPanel { Margin = new Thickness(0, 1, 20, 1), VerticalAlignment = VerticalAlignment.Center };
            TextBlock label = Theme.Text(field.Label + (field.Schema.Advanced ? "  ·  " + Localization.T("Advanced", "Erweitert") : string.Empty),
                13, Theme.TextPrimary, FontWeights.SemiBold);
            labelPanel.Children.Add(label);
            string detail = string.IsNullOrWhiteSpace(field.Description) ? RangeHint(field) : field.Description;
            if (!string.IsNullOrWhiteSpace(detail))
            {
                TextBlock help = Theme.Text(detail, 11.5, Theme.TextSecondary, FontWeights.Normal);
                help.TextWrapping = TextWrapping.Wrap;
                help.MaxWidth = 520;
                help.Margin = new Thickness(0, 2, 0, 0);
                labelPanel.Children.Add(help);
            }
            row.Children.Add(labelPanel);

            Control editor;
            string value = ModConfigurationService.GetValue(profile, field.Id) ?? field.DefaultValue;
            if (string.Equals(field.Schema.Type, "bool", StringComparison.OrdinalIgnoreCase))
            {
                CheckBox toggle = new CheckBox
                {
                    Content = Localization.T("Enabled", "Aktiviert"),
                    IsChecked = value == "1",
                    Style = (Style)Application.Current.Resources["SettingsCheckBox"],
                    VerticalAlignment = VerticalAlignment.Center,
                    HorizontalAlignment = HorizontalAlignment.Left
                };
                editor = toggle;
            }
            else if (string.Equals(field.Schema.Type, "choice", StringComparison.OrdinalIgnoreCase))
            {
                ComboBox combo = CreateComboBox(230);
                foreach (string option in field.Schema.Choices ?? new List<string>()) combo.Items.Add(option);
                combo.SelectedItem = combo.Items.Cast<object>().FirstOrDefault(item => string.Equals(item.ToString(), value, StringComparison.OrdinalIgnoreCase));
                if (combo.SelectedIndex < 0 && combo.Items.Count > 0) combo.SelectedIndex = 0;
                editor = combo;
            }
            else
            {
                TextBox input = new TextBox
                {
                    Text = value,
                    Width = 230,
                    Height = 32,
                    Padding = new Thickness(9, 5, 9, 5),
                    Foreground = Theme.TextPrimary,
                    Background = Theme.SurfaceRaised,
                    BorderBrush = Theme.Border,
                    BorderThickness = new Thickness(1),
                    CaretBrush = Theme.TextPrimary,
                    VerticalContentAlignment = VerticalAlignment.Center
                };
                editor = input;
            }
            editor.ToolTip = RangeHint(field);
            editor.VerticalAlignment = VerticalAlignment.Center;
            editor.HorizontalAlignment = HorizontalAlignment.Right;
            AutomationProperties.SetName(editor, field.Label);
            editors[field.Id] = editor;
            Grid.SetColumn(editor, 1);
            row.Children.Add(editor);
            return row;
        }

        private void CaptureEditors()
        {
            foreach (KeyValuePair<string, Control> entry in editors)
            {
                CheckBox toggle = entry.Value as CheckBox;
                if (toggle != null)
                {
                    ModConfigurationService.SetValue(profile, entry.Key, toggle.IsChecked == true ? "1" : "0");
                    continue;
                }
                ComboBox combo = entry.Value as ComboBox;
                if (combo != null)
                {
                    ModConfigurationService.SetValue(profile, entry.Key, combo.SelectedItem == null ? string.Empty : combo.SelectedItem.ToString());
                    continue;
                }
                TextBox input = entry.Value as TextBox;
                if (input != null) ModConfigurationService.SetValue(profile, entry.Key, input.Text);
            }
        }

        private void Import_Click(object sender, RoutedEventArgs e)
        {
            CaptureEditors();
            try
            {
                ModConfigurationService.ImportFromGame(catalog, profile, gamePath, installedModIds);
                editors.Clear();
                RenderSelectedMod();
                SetNotice(Localization.T(
                    "Installed INI values were imported. Review them, then save and Apply.",
                    "Installierte INI-Werte wurden importiert. Prüfe sie und wähle danach Speichern und Anwenden."), false);
            }
            catch (Exception exception)
            {
                SetNotice(Localization.T("Import failed: ", "Import fehlgeschlagen: ") + exception.Message, true);
            }
        }

        private void Reset_Click(object sender, RoutedEventArgs e)
        {
            CaptureEditors();
            ModChoice choice = modSelector.SelectedItem as ModChoice;
            if (choice == null || choice.Mod == null) return;
            ModConfigurationService.ResetMod(catalog, profile, choice.Mod.Id);
            editors.Clear();
            RenderSelectedMod();
            SetNotice(Localization.T("This mod was reset to its safe defaults.", "Dieser Mod wurde auf sichere Standardwerte zurückgesetzt."), false);
        }

        private void Save_Click(object sender, RoutedEventArgs e)
        {
            CaptureEditors();
            string error;
            if (!ModConfigurationService.TryValidateProfile(catalog, profile, out error))
            {
                SetNotice(error, true);
                return;
            }
            ResultProfile = ModConfigurationService.CloneProfile(profile);
            DialogResult = true;
        }

        private void SetNotice(string text, bool danger)
        {
            noticeText.Text = text ?? string.Empty;
            noticeText.Foreground = danger ? Theme.Danger : Theme.Success;
        }

        private static Border CreateCard()
        {
            return new Border
            {
                Background = Theme.Surface,
                BorderBrush = Theme.Border,
                BorderThickness = new Thickness(1),
                CornerRadius = new CornerRadius(10),
                Padding = new Thickness(15, 12, 15, 12),
                Margin = new Thickness(0, 0, 0, 10)
            };
        }

        private static ComboBox CreateComboBox(double width)
        {
            return new ComboBox
            {
                Width = width,
                Height = 34,
                Padding = new Thickness(8, 3, 8, 3),
                Foreground = Theme.TextPrimary,
                Background = Theme.SurfaceRaised,
                BorderBrush = Theme.Border,
                BorderThickness = new Thickness(1),
                VerticalContentAlignment = VerticalAlignment.Center
            };
        }

        private static string RangeHint(ConfigFieldDefinition field)
        {
            List<string> parts = new List<string>();
            if (field.Schema.Min.HasValue || field.Schema.Max.HasValue)
            {
                parts.Add(Localization.Format("Range: {0} to {1}", "Bereich: {0} bis {1}",
                    field.Schema.Min.HasValue ? field.Schema.Min.Value.ToString(System.Globalization.CultureInfo.InvariantCulture) : "-∞",
                    field.Schema.Max.HasValue ? field.Schema.Max.Value.ToString(System.Globalization.CultureInfo.InvariantCulture) : "∞"));
            }
            if (field.Schema.MultipleOf.HasValue)
                parts.Add(Localization.Format("Multiple of {0}", "Vielfaches von {0}", field.Schema.MultipleOf.Value));
            if (parts.Count == 0 && field.Schema.Choices != null && field.Schema.Choices.Count > 0)
                parts.Add(Localization.T("Options: ", "Optionen: ") + string.Join(", ", field.Schema.Choices));
            return parts.Count == 0 ? string.Empty : string.Join(" · ", parts);
        }
    }
}
