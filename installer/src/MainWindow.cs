using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Automation;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

namespace DS2ModSuite
{
    internal sealed class ModRowControls
    {
        public Border Card { get; set; }
        public CheckBox CheckBox { get; set; }
        public Border Badge { get; set; }
        public TextBlock BadgeText { get; set; }
    }

    internal sealed class MainWindow : Window
    {
        private readonly Catalog catalog;
        private readonly Dictionary<ModRuntimeState, ModRowControls> modRows = new Dictionary<ModRuntimeState, ModRowControls>();
        private readonly List<Control> interactiveControls = new List<Control>();

        private GameInspection inspection;
        private List<ModRuntimeState> modStates = new List<ModRuntimeState>();
        private string selectedGamePath;
        private string lastLogPath;
        private bool isBusy;
        private bool suppressSelectionEvents;
        private bool suppressLanguageEvents;
        private ModConfigurationProfile configurationProfile;

        private Border statusCard;
        private Border statusIcon;
        private TextBlock statusIconText;
        private TextBlock readinessText;
        private TextBlock readinessDetailText;
        private TextBlock pathText;
        private WrapPanel statusBadges;
        private TextBlock modCountText;
        private StackPanel modListPanel;
        private Border noticeBar;
        private TextBlock noticeText;
        private TextBlock summaryText;
        private TextBlock progressText;
        private ProgressBar progressBar;
        private Button applyButton;
        private Button logButton;
        private Button settingsButton;
        private ComboBox languageSelector;

        public MainWindow(Catalog catalog)
        {
            this.catalog = catalog;
            configurationProfile = ModConfigurationService.LoadStoredProfile(catalog);
            Title = "DS2 Mod Suite";
            Width = 1060;
            Height = 790;
            MinWidth = 860;
            MinHeight = 640;
            WindowStartupLocation = WindowStartupLocation.CenterScreen;
            Background = Theme.Background;
            Foreground = Theme.TextPrimary;
            FontFamily = new FontFamily("Segoe UI");
            UseLayoutRounding = true;
            SnapsToDevicePixels = true;
            Content = BuildLayout();
            Loaded += async delegate { await RefreshAsync(); };
            Closing += MainWindow_Closing;
        }

        private UIElement BuildLayout()
        {
            Grid root = new Grid { Background = Theme.Background };
            root.RowDefinitions.Add(new RowDefinition { Height = GridLength.Auto });
            root.RowDefinitions.Add(new RowDefinition { Height = GridLength.Auto });
            root.RowDefinitions.Add(new RowDefinition { Height = new GridLength(1, GridUnitType.Star) });
            root.RowDefinitions.Add(new RowDefinition { Height = GridLength.Auto });

            root.Children.Add(BuildHeader());
            statusCard = BuildStatusCard();
            Grid.SetRow(statusCard, 1);
            root.Children.Add(statusCard);

            Grid content = BuildModContent();
            Grid.SetRow(content, 2);
            root.Children.Add(content);

            Border footer = BuildFooter();
            Grid.SetRow(footer, 3);
            root.Children.Add(footer);
            return root;
        }

        private UIElement BuildHeader()
        {
            Grid header = new Grid { Margin = new Thickness(26, 20, 26, 16) };
            header.ColumnDefinitions.Add(new ColumnDefinition { Width = GridLength.Auto });
            header.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(1, GridUnitType.Star) });
            header.ColumnDefinitions.Add(new ColumnDefinition { Width = GridLength.Auto });

            Border mark = new Border
            {
                Width = 48,
                Height = 48,
                CornerRadius = new CornerRadius(12),
                Background = Theme.Accent,
                VerticalAlignment = VerticalAlignment.Center,
                Child = Theme.Text("DS2", 16, Theme.Brush("#07131A"), FontWeights.Bold)
            };
            ((TextBlock)mark.Child).HorizontalAlignment = HorizontalAlignment.Center;
            ((TextBlock)mark.Child).VerticalAlignment = VerticalAlignment.Center;
            header.Children.Add(mark);

            StackPanel title = new StackPanel { Margin = new Thickness(14, 0, 0, 0), VerticalAlignment = VerticalAlignment.Center };
            title.Children.Add(Theme.Text("DS2 Mod Suite", 24, Theme.TextPrimary, FontWeights.SemiBold));
            TextBlock subtitle = Theme.Text(Localization.T(
                "Choose your mods. Backup, installation and verification run automatically.",
                "Mods auswählen. Sicherung, Installation und Prüfung laufen automatisch."), 13.5, Theme.TextSecondary, FontWeights.Normal);
            subtitle.Margin = new Thickness(0, 3, 0, 0);
            title.Children.Add(subtitle);
            Grid.SetColumn(title, 1);
            header.Children.Add(title);

            StackPanel tools = new StackPanel { Orientation = Orientation.Horizontal, VerticalAlignment = VerticalAlignment.Center, Margin = new Thickness(18, 0, 0, 0) };
            settingsButton = Theme.Button(Localization.T("Mod Settings", "Mod-Einstellungen"), false);
            settingsButton.MinWidth = 116;
            settingsButton.Click += Settings_Click;
            tools.Children.Add(settingsButton);
            interactiveControls.Add(settingsButton);

            languageSelector = new ComboBox
            {
                Width = 112,
                Height = 34,
                Margin = new Thickness(8, 0, 0, 0),
                Padding = new Thickness(7, 3, 7, 3),
                Foreground = Theme.TextPrimary,
                Background = Theme.SurfaceRaised,
                BorderBrush = Theme.Border,
                BorderThickness = new Thickness(1),
                VerticalContentAlignment = VerticalAlignment.Center
            };
            ComboBoxItem english = new ComboBoxItem { Content = "English", Tag = "en" };
            ComboBoxItem german = new ComboBoxItem { Content = "Deutsch", Tag = "de" };
            languageSelector.Items.Add(english);
            languageSelector.Items.Add(german);
            suppressLanguageEvents = true;
            languageSelector.SelectedItem = Localization.CurrentLanguage == UiLanguage.German ? german : english;
            suppressLanguageEvents = false;
            languageSelector.SelectionChanged += LanguageSelector_SelectionChanged;
            AutomationProperties.SetName(languageSelector, Localization.T("Language", "Sprache"));
            tools.Children.Add(languageSelector);
            interactiveControls.Add(languageSelector);

            TextBlock version = Theme.Text("v" + catalog.SuiteVersion, 12, Theme.TextSecondary, FontWeights.Normal);
            version.VerticalAlignment = VerticalAlignment.Center;
            version.Margin = new Thickness(10, 0, 0, 0);
            tools.Children.Add(version);
            Grid.SetColumn(tools, 2);
            header.Children.Add(tools);
            return header;
        }

        private Border BuildStatusCard()
        {
            Border card = new Border
            {
                Margin = new Thickness(26, 0, 26, 18),
                Padding = new Thickness(18, 15, 18, 15),
                Background = Theme.Surface,
                BorderBrush = Theme.Border,
                BorderThickness = new Thickness(1),
                CornerRadius = new CornerRadius(12)
            };

            Grid grid = new Grid();
            grid.ColumnDefinitions.Add(new ColumnDefinition { Width = GridLength.Auto });
            grid.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(1, GridUnitType.Star) });
            grid.ColumnDefinitions.Add(new ColumnDefinition { Width = GridLength.Auto });

            statusIcon = new Border
            {
                Width = 38,
                Height = 38,
                CornerRadius = new CornerRadius(19),
                Background = Theme.NeutralBadge,
                VerticalAlignment = VerticalAlignment.Top
            };
            statusIconText = Theme.Text("…", 18, Theme.TextPrimary, FontWeights.Bold);
            statusIconText.HorizontalAlignment = HorizontalAlignment.Center;
            statusIconText.VerticalAlignment = VerticalAlignment.Center;
            statusIcon.Child = statusIconText;
            grid.Children.Add(statusIcon);

            StackPanel information = new StackPanel { Margin = new Thickness(14, 0, 18, 0) };
            readinessText = Theme.Text(Localization.T("Checking game installation …", "Spielinstallation wird geprüft …"), 17, Theme.TextPrimary, FontWeights.SemiBold);
            information.Children.Add(readinessText);
            readinessDetailText = Theme.Text(Localization.T(
                "Steam libraries and existing loaders are detected automatically.",
                "Steam-Bibliotheken und vorhandene Loader werden erkannt."), 13, Theme.TextSecondary, FontWeights.Normal);
            readinessDetailText.Margin = new Thickness(0, 3, 0, 0);
            information.Children.Add(readinessDetailText);

            pathText = Theme.Text(Localization.T("No game folder selected yet", "Noch kein Spielordner ausgewählt"), 12.5, Theme.TextSecondary, FontWeights.Normal);
            pathText.Margin = new Thickness(0, 10, 0, 0);
            pathText.TextTrimming = TextTrimming.CharacterEllipsis;
            pathText.MaxWidth = 650;
            information.Children.Add(pathText);

            statusBadges = new WrapPanel { Margin = new Thickness(0, 11, 0, -4) };
            information.Children.Add(statusBadges);
            Grid.SetColumn(information, 1);
            grid.Children.Add(information);

            StackPanel actions = new StackPanel { VerticalAlignment = VerticalAlignment.Top };
            Button browse = Theme.Button(Localization.T("Select DS2.exe", "DS2.exe auswählen"), false);
            browse.Click += BrowseGame_Click;
            browse.MinWidth = 146;
            actions.Children.Add(browse);
            interactiveControls.Add(browse);
            Button rescan = Theme.Button(Localization.T("Rescan", "Neu prüfen"), false);
            rescan.Margin = new Thickness(0, 8, 0, 0);
            rescan.Click += async delegate { await RefreshAsync(); };
            actions.Children.Add(rescan);
            interactiveControls.Add(rescan);
            Grid.SetColumn(actions, 2);
            grid.Children.Add(actions);

            card.Child = grid;
            return card;
        }

        private Grid BuildModContent()
        {
            Grid content = new Grid { Margin = new Thickness(26, 0, 26, 0) };
            content.RowDefinitions.Add(new RowDefinition { Height = GridLength.Auto });
            content.RowDefinitions.Add(new RowDefinition { Height = GridLength.Auto });
            content.RowDefinitions.Add(new RowDefinition { Height = new GridLength(1, GridUnitType.Star) });

            Grid heading = new Grid { Margin = new Thickness(0, 0, 0, 12) };
            heading.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(1, GridUnitType.Star) });
            heading.ColumnDefinitions.Add(new ColumnDefinition { Width = GridLength.Auto });

            StackPanel headingText = new StackPanel();
            headingText.Children.Add(Theme.Text("Mods", 18, Theme.TextPrimary, FontWeights.SemiBold));
            modCountText = Theme.Text(Localization.Format("{0} available", "{0} verfügbar", catalog.Mods.Count), 12.5, Theme.TextSecondary, FontWeights.Normal);
            modCountText.Margin = new Thickness(0, 2, 0, 0);
            headingText.Children.Add(modCountText);
            heading.Children.Add(headingText);

            StackPanel quickActions = new StackPanel { Orientation = Orientation.Horizontal, VerticalAlignment = VerticalAlignment.Center };
            Button all = Theme.Button(Localization.T("Select all", "Alle auswählen"), false);
            all.Click += delegate { SetAllSelections(true); };
            quickActions.Children.Add(all);
            interactiveControls.Add(all);
            Button none = Theme.Button(Localization.T("Select none", "Keine auswählen"), false);
            none.Margin = new Thickness(8, 0, 0, 0);
            none.Click += delegate { SetAllSelections(false); };
            quickActions.Children.Add(none);
            interactiveControls.Add(none);
            Grid.SetColumn(quickActions, 1);
            heading.Children.Add(quickActions);
            content.Children.Add(heading);

            noticeBar = new Border
            {
                Visibility = Visibility.Collapsed,
                Margin = new Thickness(0, 0, 0, 12),
                Padding = new Thickness(13, 10, 13, 10),
                Background = Theme.SurfaceRaised,
                BorderBrush = Theme.Border,
                BorderThickness = new Thickness(1),
                CornerRadius = new CornerRadius(8)
            };
            noticeText = Theme.Text(string.Empty, 13, Theme.TextPrimary, FontWeights.Normal);
            noticeBar.Child = noticeText;
            AutomationProperties.SetLiveSetting(noticeText, AutomationLiveSetting.Polite);
            Grid.SetRow(noticeBar, 1);
            content.Children.Add(noticeBar);

            ScrollViewer scroll = new ScrollViewer
            {
                VerticalScrollBarVisibility = ScrollBarVisibility.Auto,
                HorizontalScrollBarVisibility = ScrollBarVisibility.Disabled,
                PanningMode = PanningMode.VerticalOnly,
                Focusable = false
            };
            modListPanel = new StackPanel { Margin = new Thickness(0, 0, 7, 16) };
            scroll.Content = modListPanel;
            Grid.SetRow(scroll, 2);
            content.Children.Add(scroll);
            return content;
        }

        private Border BuildFooter()
        {
            Border footer = new Border
            {
                Background = Theme.Surface,
                BorderBrush = Theme.Border,
                BorderThickness = new Thickness(0, 1, 0, 0),
                Padding = new Thickness(26, 14, 26, 16)
            };
            Grid grid = new Grid();
            grid.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(1, GridUnitType.Star) });
            grid.ColumnDefinitions.Add(new ColumnDefinition { Width = GridLength.Auto });

            StackPanel state = new StackPanel { VerticalAlignment = VerticalAlignment.Center };
            summaryText = Theme.Text(Localization.T("No pending changes.", "Keine ausstehenden Änderungen."), 13.5, Theme.TextPrimary, FontWeights.SemiBold);
            AutomationProperties.SetLiveSetting(summaryText, AutomationLiveSetting.Polite);
            state.Children.Add(summaryText);
            progressText = Theme.Text(string.Empty, 12, Theme.TextSecondary, FontWeights.Normal);
            progressText.Margin = new Thickness(0, 3, 0, 0);
            progressText.Visibility = Visibility.Collapsed;
            state.Children.Add(progressText);
            progressBar = new ProgressBar
            {
                Height = 4,
                Width = 420,
                Minimum = 0,
                Maximum = 100,
                Foreground = Theme.Accent,
                Background = Theme.NeutralBadge,
                BorderThickness = new Thickness(0),
                Margin = new Thickness(0, 9, 0, 0),
                HorizontalAlignment = HorizontalAlignment.Left,
                Visibility = Visibility.Collapsed
            };
            state.Children.Add(progressBar);
            grid.Children.Add(state);

            StackPanel actions = new StackPanel { Orientation = Orientation.Horizontal, VerticalAlignment = VerticalAlignment.Center };
            logButton = Theme.Button(Localization.T("Open log", "Protokoll öffnen"), false);
            logButton.Visibility = Visibility.Collapsed;
            logButton.Margin = new Thickness(0, 0, 8, 0);
            logButton.Click += OpenLog_Click;
            actions.Children.Add(logButton);
            Button reset = Theme.Button(Localization.T("Reset selection", "Auswahl zurücksetzen"), false);
            reset.Click += delegate { ResetSelection(); };
            actions.Children.Add(reset);
            interactiveControls.Add(reset);
            applyButton = Theme.Button(Localization.T("No changes", "Keine Änderungen"), true);
            applyButton.Margin = new Thickness(9, 0, 0, 0);
            applyButton.MinWidth = 176;
            applyButton.IsEnabled = false;
            applyButton.Click += Apply_Click;
            actions.Children.Add(applyButton);
            interactiveControls.Add(applyButton);
            Grid.SetColumn(actions, 1);
            grid.Children.Add(actions);
            footer.Child = grid;
            return footer;
        }

        private async void LanguageSelector_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (suppressLanguageEvents || isBusy) return;
            ComboBoxItem item = languageSelector.SelectedItem as ComboBoxItem;
            string code = item == null ? "en" : item.Tag as string;
            UiLanguage language;
            if (!Localization.TryParse(code, out language)) language = UiLanguage.English;
            if (language == Localization.CurrentLanguage) return;

            Localization.SetLanguage(language);
            SettingsStore.WriteLanguage(language);
            bool refreshLocalizedError = inspection != null && !inspection.Found;
            if (inspection != null) LoaderInspector.Relocalize(inspection.Loader);
            foreach (ModRuntimeState state in modStates) GameInspector.UpdateStatus(state);
            RebuildLocalizedUi();
            if (refreshLocalizedError) await RefreshAsync();
        }

        private void RebuildLocalizedUi()
        {
            interactiveControls.Clear();
            modRows.Clear();
            Content = BuildLayout();
            if (inspection != null)
            {
                RenderInspection();
                BuildModList();
            }
            UpdateChangeSummary();
            if (!string.IsNullOrWhiteSpace(lastLogPath) && File.Exists(lastLogPath)) logButton.Visibility = Visibility.Visible;
        }

        private void Settings_Click(object sender, RoutedEventArgs e)
        {
            if (isBusy) return;
            try
            {
                ModConfigurationProfile initial = configurationProfile
                    ?? ModConfigurationService.LoadEffectiveProfile(catalog, inspection != null && inspection.Found ? inspection.GamePath : selectedGamePath);
                List<string> installedModIds = modStates
                    .Where(state => state.IsInstalled || state.HasObsoleteBinary)
                    .Select(state => state.Spec.Id)
                    .ToList();
                ModSettingsWindow window = new ModSettingsWindow(
                    catalog,
                    initial,
                    inspection != null && inspection.Found ? inspection.GamePath : selectedGamePath,
                    installedModIds)
                {
                    Owner = this
                };
                if (window.ShowDialog() != true || window.ResultProfile == null) return;
                configurationProfile = window.ResultProfile;
                string saveError;
                bool saved = ModConfigurationService.SaveProfile(catalog, configurationProfile, out saveError);
                UpdateChangeSummary();
                ShowNotice(saved
                    ? Localization.T(
                        "Mod settings saved. Choose Apply to write changed values to the individual INI files.",
                        "Mod-Einstellungen gespeichert. Wähle Anwenden, um geänderte Werte in die einzelnen INI-Dateien zu schreiben.")
                    : saveError,
                    saved ? "accent" : "warning");
            }
            catch (Exception exception)
            {
                ShowNotice(Localization.T("Mod settings could not be opened: ", "Mod-Einstellungen konnten nicht geöffnet werden: ") + exception.Message, "danger");
            }
        }

        private async Task RefreshAsync()
        {
            if (isBusy)
            {
                return;
            }

            SetBusy(true, Localization.T("Checking game installation …", "Spielinstallation wird geprüft …"));
            readinessText.Text = Localization.T("Checking game installation …", "Spielinstallation wird geprüft …");
            readinessDetailText.Text = Localization.T(
                "Checking Steam libraries, game version and ASI Loader.",
                "Steam-Bibliotheken, Spielversion und ASI Loader werden geprüft.");
            statusIcon.Background = Theme.NeutralBadge;
            statusIconText.Text = "…";
            try
            {
                inspection = await Task.Run(() => GameInspector.Inspect(
                    catalog,
                    selectedGamePath,
                    !SecurityContext.IsAdministrator()));
                if (inspection.Found)
                {
                    selectedGamePath = inspection.GamePath;
                }

                modStates = GameInspector.InspectMods(inspection, catalog);
                RenderInspection();
                BuildModList();
            }
            catch (Exception exception)
            {
                inspection = new GameInspection { Error = exception.Message, Loader = new LoaderInspection { Status = LoaderStatus.InvalidPayload } };
                modStates = GameInspector.InspectMods(inspection, catalog);
                RenderInspection();
                BuildModList();
            }
            finally
            {
                SetBusy(false, null);
                UpdateChangeSummary();
            }
        }

        private void RenderInspection()
        {
            statusBadges.Children.Clear();
            if (inspection == null || !inspection.Found)
            {
                readinessText.Text = Localization.T("Select game installation", "Spielinstallation auswählen");
                readinessDetailText.Text = inspection == null
                    ? Localization.T("DS2.exe has not been checked yet.", "DS2.exe wurde noch nicht geprüft.")
                    : inspection.Error;
                pathText.Text = Localization.T("No valid DS2.exe selected", "Keine gültige DS2.exe ausgewählt");
                pathText.ToolTip = null;
                SetStatusVisual("!", Theme.Danger);
                AddBadge(Localization.T("Game missing", "Spiel fehlt"), "danger");
                ShowNotice(readinessDetailText.Text, "danger");
                return;
            }

            pathText.Text = inspection.ExePath;
            pathText.ToolTip = inspection.ExePath;
            if (inspection.IsRunning)
            {
                readinessText.Text = Localization.T("Death Stranding 2 is still running", "Death Stranding 2 läuft noch");
                readinessDetailText.Text = Localization.T(
                    "Close the game completely before applying changes.",
                    "Bitte schließe das Spiel vollständig, bevor Änderungen angewendet werden.");
                SetStatusVisual("!", Theme.Warning);
            }
            else if (!inspection.Supported)
            {
                readinessText.Text = Localization.T("Game version is not supported", "Spielversion wird nicht unterstützt");
                readinessDetailText.Text = Localization.T(
                    "Installations and updates are blocked; exactly known suite files can still be removed safely.",
                    "Installationen und Updates sind gesperrt; exakt bekannte Suite-Dateien können sicher entfernt werden.");
                SetStatusVisual("×", Theme.Danger);
            }
            else if (inspection.Loader.Status == LoaderStatus.InvalidPayload)
            {
                readinessText.Text = Localization.T("Installer package is incomplete", "Installer-Paket ist unvollständig");
                readinessDetailText.Text = inspection.Loader.DetailText;
                SetStatusVisual("×", Theme.Danger);
            }
            else if (inspection.Loader.Status == LoaderStatus.UnknownProxy)
            {
                readinessText.Text = Localization.T("Existing loader needs attention", "Vorhandener Loader muss geprüft werden");
                readinessDetailText.Text = inspection.Loader.DetailText;
                SetStatusVisual("!", Theme.Warning);
            }
            else
            {
                readinessText.Text = Localization.T("Ready", "Bereit");
                readinessDetailText.Text = Localization.T(
                    "Game version and installation target were verified successfully.",
                    "Spielversion und Installationsziel wurden erfolgreich geprüft.");
                SetStatusVisual("✓", Theme.Success);
            }

            AddBadge("DS2.exe " + (inspection.FileVersion ?? Localization.T("unknown", "unbekannt")), inspection.Supported ? "success" : "danger");
            AddBadge(inspection.Loader.DisplayText, inspection.Loader.Status == LoaderStatus.KnownCompatible ? "success" : inspection.Loader.Status == LoaderStatus.Absent ? "accent" : "warning");
            AddBadge(Localization.T("Backups enabled", "Backups aktiv"), "neutral");

            if (inspection.IsRunning)
            {
                ShowNotice(Localization.T(
                    "Death Stranding 2 is still running. Close the game, then click Rescan.",
                    "Death Stranding 2 läuft noch. Bitte schließe das Spiel und klicke auf „Neu prüfen“."), "warning");
            }
            else if (!inspection.Supported)
            {
                ShowNotice(Localization.T("Found: DS2.exe ", "Gefunden: DS2.exe ")
                    + (inspection.FileVersion ?? Localization.T("unknown", "unbekannt"))
                    + Localization.T(
                        ". Currently only 1.10.89.0 with the verified hash is supported. Safe removals remain available.",
                        ". Unterstützt wird derzeit ausschließlich 1.10.89.0 mit geprüftem Hash. Sichere Entfernungen bleiben möglich."), "danger");
            }
            else if (inspection.Loader.Status == LoaderStatus.UnknownProxy)
            {
                ShowNotice(inspection.Loader.DetailText + Localization.T(
                    " Removal of exactly known suite files remains available.",
                    " Reine Entfernungen exakt bekannter Suite-Dateien bleiben möglich."), "warning");
            }
            else if (inspection.Loader.Status == LoaderStatus.InvalidPayload)
            {
                ShowNotice(inspection.Loader.DetailText, "danger");
            }
            else if (modStates.Any(state => state.HasUnknownObsoleteBinary))
            {
                ModRuntimeState conflict = modStates.First(state => state.HasUnknownObsoleteBinary);
                ShowNotice(Localization.T("Conflict for “", "Konflikt bei „") + conflict.Spec.Name + "“: " + conflict.ConflictFileNames
                    + Localization.T(
                        ". Check, rename or remove this file manually.",
                        ". Diese Datei bitte manuell prüfen, umbenennen oder entfernen."), "warning");
            }
            else if (inspection.ForeignAsiCount > 0)
            {
                ShowNotice(inspection.ForeignAsiCount + (inspection.ForeignAsiCount == 1
                    ? Localization.T(" unmanaged ASI file was detected. It will remain unchanged.", " nicht verwaltete ASI-Datei wurde erkannt. Sie bleibt unverändert.")
                    : Localization.T(" unmanaged ASI files were detected. They will remain unchanged.", " nicht verwaltete ASI-Dateien wurden erkannt. Sie bleiben unverändert.")), "warning");
            }
            else if (inspection.Loader.Status == LoaderStatus.Absent)
            {
                ShowNotice(Localization.T(
                    "The tested Ultimate ASI Loader x64 will be installed automatically when at least one mod is selected.",
                    "Der getestete Ultimate ASI Loader x64 wird automatisch installiert, sobald mindestens ein Mod ausgewählt ist."), "accent");
            }
            else
            {
                HideNotice();
            }
        }

        private void BuildModList()
        {
            foreach (ModRowControls row in modRows.Values)
            {
                interactiveControls.Remove(row.CheckBox);
            }
            modRows.Clear();
            modListPanel.Children.Clear();
            foreach (IGrouping<string, ModRuntimeState> category in modStates.GroupBy(state => state.Spec.LocalizedCategory))
            {
                Grid categoryHeader = new Grid { Margin = new Thickness(2, modListPanel.Children.Count == 0 ? 2 : 14, 2, 8) };
                categoryHeader.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(1, GridUnitType.Star) });
                categoryHeader.ColumnDefinitions.Add(new ColumnDefinition { Width = GridLength.Auto });
                categoryHeader.Children.Add(Theme.Text(category.Key, 13, Theme.TextSecondary, FontWeights.SemiBold));
                TextBlock count = Theme.Text(category.Count().ToString(), 12, Theme.TextSecondary, FontWeights.Normal);
                Grid.SetColumn(count, 1);
                categoryHeader.Children.Add(count);
                modListPanel.Children.Add(categoryHeader);

                foreach (ModRuntimeState state in category)
                {
                    modListPanel.Children.Add(BuildModCard(state));
                }
            }

            UpdateModCount();
        }

        private Border BuildModCard(ModRuntimeState state)
        {
            Border card = new Border
            {
                Background = Theme.Surface,
                BorderBrush = Theme.Border,
                BorderThickness = new Thickness(1),
                CornerRadius = new CornerRadius(10),
                Padding = new Thickness(14, 12, 14, 12),
                Margin = new Thickness(0, 0, 0, 9)
            };
            card.MouseEnter += delegate { if (!isBusy) card.Background = Theme.SurfaceRaised; };
            card.MouseLeave += delegate { card.Background = Theme.Surface; };

            Grid grid = new Grid();
            grid.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(42) });
            grid.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(1, GridUnitType.Star) });
            grid.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(150) });

            CheckBox check = new CheckBox
            {
                IsChecked = state.DesiredEnabled,
                Style = (Style)Application.Current.Resources["ModCheckBox"],
                VerticalAlignment = VerticalAlignment.Center,
                HorizontalAlignment = HorizontalAlignment.Left
            };
            AutomationProperties.SetName(check, state.Spec.Name + ", Version " + state.Spec.Version + ", " + state.StatusText);
            check.Checked += delegate { SelectionChanged(state, true); };
            check.Unchecked += delegate { SelectionChanged(state, false); };
            grid.Children.Add(check);
            interactiveControls.Add(check);

            StackPanel details = new StackPanel { VerticalAlignment = VerticalAlignment.Center };
            TextBlock title = Theme.Text(state.Spec.Name, 15, Theme.TextPrimary, FontWeights.SemiBold);
            details.Children.Add(title);
            TextBlock description = Theme.Text(state.Spec.LocalizedDescription, 12.8, Theme.TextSecondary, FontWeights.Normal);
            description.Margin = new Thickness(0, 3, 12, 0);
            details.Children.Add(description);
            if (state.HasUnknownObsoleteBinary)
            {
                TextBlock conflict = Theme.Text(Localization.T("Conflict: ", "Konflikt: ") + state.ConflictFileNames, 11.8, Theme.Warning, FontWeights.SemiBold);
                conflict.Margin = new Thickness(0, 4, 12, 0);
                details.Children.Add(conflict);
            }
            Grid.SetColumn(details, 1);
            grid.Children.Add(details);

            StackPanel status = new StackPanel { HorizontalAlignment = HorizontalAlignment.Right, VerticalAlignment = VerticalAlignment.Center };
            TextBlock badgeText = Theme.Text(state.StatusText, 11.5, Theme.TextPrimary, FontWeights.SemiBold);
            badgeText.HorizontalAlignment = HorizontalAlignment.Center;
            Border badge = new Border
            {
                Padding = new Thickness(9, 4, 9, 4),
                CornerRadius = new CornerRadius(10),
                Background = BadgeBrush(state.StatusTone),
                Child = badgeText,
                HorizontalAlignment = HorizontalAlignment.Right
            };
            status.Children.Add(badge);
            TextBlock version = Theme.Text("v" + state.Spec.Version, 11.5, Theme.TextSecondary, FontWeights.Normal);
            version.Margin = new Thickness(0, 5, 2, 0);
            version.HorizontalAlignment = HorizontalAlignment.Right;
            status.Children.Add(version);
            Grid.SetColumn(status, 2);
            grid.Children.Add(status);

            card.Child = grid;
            modRows[state] = new ModRowControls { Card = card, CheckBox = check, Badge = badge, BadgeText = badgeText };
            return card;
        }

        private void SelectionChanged(ModRuntimeState state, bool enabled)
        {
            if (suppressSelectionEvents)
            {
                return;
            }

            state.DesiredEnabled = enabled;
            GameInspector.UpdateStatus(state);
            UpdateModRow(state);
            UpdateChangeSummary();
        }

        private void UpdateModRow(ModRuntimeState state)
        {
            ModRowControls row;
            if (!modRows.TryGetValue(state, out row))
            {
                return;
            }

            row.BadgeText.Text = state.StatusText;
            row.Badge.Background = BadgeBrush(state.StatusTone);
            AutomationProperties.SetName(row.CheckBox, state.Spec.Name + ", Version " + state.Spec.Version + ", " + state.StatusText);
        }

        private void SetAllSelections(bool enabled)
        {
            suppressSelectionEvents = true;
            foreach (ModRuntimeState state in modStates)
            {
                state.DesiredEnabled = enabled;
                GameInspector.UpdateStatus(state);
                ModRowControls row;
                if (modRows.TryGetValue(state, out row))
                {
                    row.CheckBox.IsChecked = enabled;
                    UpdateModRow(state);
                }
            }
            suppressSelectionEvents = false;
            UpdateChangeSummary();
        }

        private void ResetSelection()
        {
            suppressSelectionEvents = true;
            foreach (ModRuntimeState state in modStates)
            {
                state.DesiredEnabled = state.IsInstalled || state.HasObsoleteBinary;
                GameInspector.UpdateStatus(state);
                ModRowControls row;
                if (modRows.TryGetValue(state, out row))
                {
                    row.CheckBox.IsChecked = state.DesiredEnabled;
                    UpdateModRow(state);
                }
            }
            suppressSelectionEvents = false;
            UpdateChangeSummary();
        }

        private void UpdateChangeSummary()
        {
            int install = modStates.Count(state => state.DesiredEnabled && !state.HasTargetBinary && !state.HasObsoleteBinary);
            int update = modStates.Count(state => state.DesiredEnabled && (state.HasModifiedBinary || state.HasObsoleteBinary));
            int repair = modStates.Count(state => state.DesiredEnabled && state.NeedsRepair && !state.HasModifiedBinary && !state.HasObsoleteBinary);
            int remove = modStates.Count(state => !state.DesiredEnabled && (state.IsInstalled || state.HasObsoleteBinary));
            bool wantsMods = modStates.Any(state => state.DesiredEnabled);
            bool hasBlockingConflict = modStates.Any(state => state.DesiredEnabled && state.HasUnknownObsoleteBinary);
            bool loaderInstall = inspection != null && inspection.Found && wantsMods && inspection.Loader.Status == LoaderStatus.Absent;
            List<string> selectedIds = modStates.Where(state => state.DesiredEnabled).Select(state => state.Spec.Id).ToList();
            bool configurationChange = false;
            if (configurationProfile != null && inspection != null && inspection.Found)
            {
                try { configurationChange = ModConfigurationService.HasDifferences(catalog, configurationProfile, selectedIds, inspection.GamePath); }
                catch { configurationChange = true; }
            }
            int changes = install + update + repair + remove + (loaderInstall ? 1 : 0) + (configurationChange ? 1 : 0);

            List<string> parts = new List<string>();
            if (install > 0) parts.Add(Localization.Format("{0} to install", "{0} installieren", install));
            if (update > 0) parts.Add(Localization.Format("{0} to update", "{0} aktualisieren", update));
            if (repair > 0) parts.Add(Localization.Format("{0} to repair", "{0} reparieren", repair));
            if (remove > 0) parts.Add(Localization.Format("{0} to remove", "{0} entfernen", remove));
            if (loaderInstall) parts.Add(Localization.T("install ASI Loader", "ASI Loader installieren"));
            if (configurationChange) parts.Add(Localization.T("apply mod settings", "Mod-Einstellungen anwenden"));
            if (hasBlockingConflict) parts.Add(Localization.T("resolve conflicting file manually", "Konfliktdatei manuell prüfen"));
            summaryText.Text = parts.Count == 0 ? Localization.T("No pending changes.", "Keine ausstehenden Änderungen.") : string.Join(" · ", parts);

            bool needsTrustedPayload = install > 0 || update > 0 || repair > 0 || loaderInstall || configurationChange;
            bool environmentReady = inspection != null
                && inspection.Found
                && !inspection.IsRunning
                && (!needsTrustedPayload || (inspection.Supported
                    && inspection.Loader.Status != LoaderStatus.InvalidPayload
                    && inspection.Loader.Status != LoaderStatus.UnknownProxy))
                && !hasBlockingConflict;
            applyButton.IsEnabled = !isBusy && environmentReady && changes > 0;
            if (hasBlockingConflict)
            {
                applyButton.Content = Localization.T("Installation blocked", "Installation blockiert");
            }
            else if (changes == 0)
            {
                applyButton.Content = Localization.T("No changes", "Keine Änderungen");
            }
            else if (install > 0 && update == 0 && repair == 0 && remove == 0)
            {
                applyButton.Content = install == 1
                    ? Localization.T("Install 1 mod", "1 Mod installieren")
                    : Localization.Format("Install {0} mods", "{0} Mods installieren", install);
            }
            else if (remove > 0 && install == 0 && update == 0 && repair == 0)
            {
                applyButton.Content = remove == 1
                    ? Localization.T("Remove 1 mod", "1 Mod entfernen")
                    : Localization.Format("Remove {0} mods", "{0} Mods entfernen", remove);
            }
            else if (configurationChange && install == 0 && update == 0 && repair == 0 && remove == 0 && !loaderInstall)
            {
                applyButton.Content = Localization.T("Apply mod settings", "Mod-Einstellungen anwenden");
            }
            else
            {
                applyButton.Content = Localization.T("Apply changes", "Änderungen anwenden");
            }

            UpdateModCount();
        }

        private void UpdateModCount()
        {
            if (modCountText == null)
            {
                return;
            }

            int installed = modStates.Count(state => state.IsInstalled || state.HasObsoleteBinary || state.HasModifiedBinary);
            int desired = modStates.Count(state => state.DesiredEnabled);
            modCountText.Text = Localization.Format(
                "{0} available · {1} installed · {2} selected",
                "{0} verfügbar · {1} installiert · {2} ausgewählt",
                catalog.Mods.Count, installed, desired);
        }

        private async void Apply_Click(object sender, RoutedEventArgs e)
        {
            if (inspection == null || !inspection.Found || isBusy)
            {
                return;
            }

            SetBusy(true, Localization.T("Preparing installation …", "Installation wird vorbereitet …"));
            HideNotice();
            progressBar.Value = 0;
            progressBar.Visibility = Visibility.Visible;
            progressText.Visibility = Visibility.Visible;
            Progress<ProgressInfo> progress = new Progress<ProgressInfo>(info =>
            {
                progressBar.Value = info.Percent;
                progressText.Text = info.Message;
            });

            ApplyResult result = null;
            bool applyStarted = false;
            try
            {
                List<string> selectedIds = modStates.Where(state => state.DesiredEnabled).Select(state => state.Spec.Id).ToList();
                ModConfigurationProfile configurationForPlan = null;
                if (configurationProfile != null
                    && ModConfigurationService.HasDifferences(catalog, configurationProfile, selectedIds, inspection.GamePath))
                {
                    configurationForPlan = ModConfigurationService.CloneProfile(configurationProfile);
                }
                ApplyPlan plan = new ApplyPlan
                {
                    GamePath = inspection.GamePath,
                    SelectedModIds = selectedIds,
                    ConfigurationProfile = configurationForPlan,
                    Language = Localization.CurrentLanguageCode
                };
                applyStarted = true;
                result = await ApplyCoordinator.ApplyAsync(catalog, plan, progress);
                if (result == null) throw new InvalidOperationException(Localization.T(
                    "The installation process returned no result.",
                    "Der Installationsprozess hat kein Ergebnis zurückgegeben."));
            }
            catch (Exception exception)
            {
                result = new ApplyResult { Success = false, RollbackComplete = !applyStarted, Message = exception.Message };
            }
            finally
            {
                lastLogPath = result == null ? null : result.LogPath;
                SetBusy(false, null);
                progressBar.Visibility = Visibility.Collapsed;
                progressText.Visibility = Visibility.Collapsed;
                logButton.Visibility = !string.IsNullOrWhiteSpace(lastLogPath) && File.Exists(lastLogPath)
                    ? Visibility.Visible
                    : Visibility.Collapsed;
            }

            await RefreshAsync();
            ShowNotice(result.Success
                ? Localization.T("Changes applied successfully. ", "Änderungen erfolgreich angewendet. ") + result.Message
                : Localization.T("Installation failed. ", "Installation fehlgeschlagen. ") + (result.RollbackComplete
                    ? Localization.T("No unverified file changes were left behind. ", "Es wurden keine unbestätigten Dateiänderungen hinterlassen. ")
                    : Localization.T("The automatic rollback was incomplete. ", "Der automatische Rollback war nicht vollständig. ")) + result.Message,
                result.Success ? "success" : "danger");
        }

        private async void BrowseGame_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog dialog = new OpenFileDialog
            {
                Title = Localization.T("Select DS2.exe", "DS2.exe auswählen"),
                Filter = Localization.T(
                    "Death Stranding 2 (DS2.exe)|DS2.exe|Applications (*.exe)|*.exe",
                    "Death Stranding 2 (DS2.exe)|DS2.exe|Programme (*.exe)|*.exe"),
                CheckFileExists = true,
                Multiselect = false,
                FileName = "DS2.exe"
            };
            if (inspection != null && inspection.Found)
            {
                dialog.InitialDirectory = inspection.GamePath;
            }

            if (dialog.ShowDialog(this) == true)
            {
                if (!string.Equals(Path.GetFileName(dialog.FileName), catalog.Game.Executable, StringComparison.OrdinalIgnoreCase))
                {
                    ShowNotice(Localization.T(
                        "Select DS2.exe from the Death Stranding 2 installation folder.",
                        "Bitte wähle die DS2.exe im Installationsordner von Death Stranding 2 aus."), "danger");
                    return;
                }

                selectedGamePath = Path.GetDirectoryName(dialog.FileName);
                await RefreshAsync();
            }
        }

        private void OpenLog_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (IsAllowedLogPath(lastLogPath) && File.Exists(lastLogPath))
                {
                    Process.Start(new ProcessStartInfo { FileName = lastLogPath, UseShellExecute = true });
                }
                else
                {
                    ShowNotice(Localization.T(
                        "The log path is invalid or outside the allowed folders.",
                        "Der Protokollpfad ist ungültig oder liegt außerhalb der erlaubten Ordner."), "danger");
                }
            }
            catch (Exception exception)
            {
                ShowNotice(Localization.T("The log could not be opened: ", "Das Protokoll konnte nicht geöffnet werden: ") + exception.Message, "danger");
            }
        }

        private void SetBusy(bool busy, string message)
        {
            isBusy = busy;
            foreach (Control control in interactiveControls)
            {
                control.IsEnabled = !busy;
            }

            if (busy && !string.IsNullOrWhiteSpace(message) && progressText != null)
            {
                progressText.Text = message;
            }

            if (!busy)
            {
                UpdateChangeSummary();
            }
        }

        private void MainWindow_Closing(object sender, CancelEventArgs e)
        {
            if (!isBusy)
            {
                return;
            }

            e.Cancel = true;
            ShowNotice(Localization.T(
                "A check or file operation is still running. The window can be closed safely when it finishes.",
                "Eine Prüfung oder Dateiänderung läuft noch. Das Fenster kann danach sicher geschlossen werden."), "warning");
        }

        private bool IsAllowedLogPath(string path)
        {
            if (string.IsNullOrWhiteSpace(path) || !string.Equals(System.IO.Path.GetExtension(path), ".log", StringComparison.OrdinalIgnoreCase))
            {
                return false;
            }

            try
            {
                string full = System.IO.Path.GetFullPath(path);
                List<string> roots = new List<string> { AppPaths.LogsDirectory };
                if (inspection != null && inspection.Found)
                {
                    roots.Add(System.IO.Path.Combine(inspection.GamePath, ".ds2-mod-suite", "logs"));
                }

                foreach (string root in roots)
                {
                    string fullRoot = System.IO.Path.GetFullPath(root).TrimEnd(System.IO.Path.DirectorySeparatorChar, System.IO.Path.AltDirectorySeparatorChar)
                        + System.IO.Path.DirectorySeparatorChar;
                    if (full.StartsWith(fullRoot, StringComparison.OrdinalIgnoreCase)
                        && string.Equals(System.IO.Path.GetFileName(full), System.IO.Path.GetFileName(path), StringComparison.Ordinal))
                    {
                        return (File.GetAttributes(full) & FileAttributes.ReparsePoint) == 0;
                    }
                }
            }
            catch
            {
                return false;
            }

            return false;
        }

        private void SetStatusVisual(string symbol, Brush color)
        {
            statusIcon.Background = new SolidColorBrush(Color.FromArgb(36, ((SolidColorBrush)color).Color.R, ((SolidColorBrush)color).Color.G, ((SolidColorBrush)color).Color.B));
            statusIconText.Text = symbol;
            statusIconText.Foreground = color;
        }

        private void AddBadge(string text, string tone)
        {
            TextBlock label = Theme.Text(text, 11.5, BadgeForeground(tone), FontWeights.SemiBold);
            Border badge = new Border
            {
                Background = BadgeBrush(tone),
                CornerRadius = new CornerRadius(10),
                Padding = new Thickness(9, 4, 9, 4),
                Margin = new Thickness(0, 0, 7, 4),
                Child = label
            };
            statusBadges.Children.Add(badge);
        }

        private void ShowNotice(string message, string tone)
        {
            noticeText.Text = message ?? string.Empty;
            noticeText.Foreground = tone == "danger" ? Theme.Danger : tone == "warning" ? Theme.Warning : tone == "success" ? Theme.Success : Theme.TextPrimary;
            noticeBar.BorderBrush = noticeText.Foreground;
            noticeBar.Background = Theme.SurfaceRaised;
            noticeBar.Visibility = Visibility.Visible;
        }

        private void HideNotice()
        {
            noticeBar.Visibility = Visibility.Collapsed;
            noticeText.Text = string.Empty;
        }

        private static Brush BadgeBrush(string tone)
        {
            if (tone == "success") return Theme.Brush("#17372D");
            if (tone == "warning") return Theme.Brush("#3A3020");
            if (tone == "danger") return Theme.Brush("#3A2428");
            if (tone == "accent") return Theme.Brush("#163242");
            return Theme.NeutralBadge;
        }

        private static Brush BadgeForeground(string tone)
        {
            if (tone == "success") return Theme.Success;
            if (tone == "warning") return Theme.Warning;
            if (tone == "danger") return Theme.Danger;
            if (tone == "accent") return Theme.Accent;
            return Theme.TextSecondary;
        }
    }
}
