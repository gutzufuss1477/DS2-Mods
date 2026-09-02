using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Windows;

[assembly: AssemblyTitle("DS2 Mod Suite")]
[assembly: AssemblyDescription("All-in-one mod manager for DEATH STRANDING 2: ON THE BEACH")]
[assembly: AssemblyCompany("SimonMods")]
[assembly: AssemblyProduct("DS2 Mod Suite")]
[assembly: AssemblyCopyright("Unofficial community mod manager")]
[assembly: AssemblyVersion("1.3.0.0")]
[assembly: AssemblyFileVersion("1.3.0.0")]

namespace DS2ModSuite
{
    internal sealed class DirectProgress : IProgress<ProgressInfo>
    {
        public void Report(ProgressInfo value)
        {
            // Headless operations record detailed progress in the installation log.
        }
    }

    internal static class Program
    {
        [STAThread]
        public static int Main(string[] args)
        {
            Dictionary<string, string> arguments = ParseArguments(args);
            string requestedLanguage;
            if (arguments.TryGetValue("--language", out requestedLanguage))
            {
                Localization.SetLanguage(requestedLanguage);
            }
            else if (!arguments.ContainsKey("--self-test")
                && !arguments.ContainsKey("--apply-token")
                && !SecurityContext.IsAdministrator())
            {
                Localization.SetLanguage(SettingsStore.ReadLanguage());
            }
            try
            {
                if (arguments.ContainsKey("--self-test"))
                {
                    string testRoot = Path.Combine(Path.GetTempPath(), "DS2ModSuite-TestUser-" + Guid.NewGuid().ToString("N"));
                    AppDomain.CurrentDomain.SetData(AppPaths.SelfTestDataSlot, testRoot);
                    string testResult = SelfTest.Run();
                    WriteTextResult(arguments, testResult);
                    return 0;
                }

                if (arguments.ContainsKey("--apply-token"))
                {
                    return RunHeadlessApply(arguments);
                }

                Catalog catalog = CatalogService.LoadAndValidate();
                CatalogService.ValidatePayloads(catalog);
                Application application = new Application
                {
                    ShutdownMode = ShutdownMode.OnMainWindowClose
                };
                application.Resources.MergedDictionaries.Add(Theme.CreateResources());
                application.DispatcherUnhandledException += delegate(object sender, System.Windows.Threading.DispatcherUnhandledExceptionEventArgs eventArgs)
                {
                    MessageBox.Show(
                        Localization.T(
                            "An unexpected error occurred. No further changes were made.\n\n",
                            "Ein unerwarteter Fehler ist aufgetreten. Es wurden keine weiteren Änderungen ausgeführt.\n\n") + eventArgs.Exception.Message,
                        "DS2 Mod Suite",
                        MessageBoxButton.OK,
                        MessageBoxImage.Error);
                    eventArgs.Handled = true;
                };

                return application.Run(new MainWindow(catalog));
            }
            catch (Exception exception)
            {
                if (arguments.ContainsKey("--self-test"))
                {
                    try { WriteTextResult(arguments, "FAIL: " + exception); } catch { }
                }
                else if (arguments.ContainsKey("--apply-token"))
                {
                    // The parent process reports a missing authenticated result; never show a hidden elevated dialog.
                }
                else
                {
                    MessageBox.Show(exception.Message, "DS2 Mod Suite", MessageBoxButton.OK, MessageBoxImage.Error);
                }

                return 1;
            }
        }

        private static int RunHeadlessApply(Dictionary<string, string> arguments)
        {
            string token;
            string keyHex;
            string gamePath;
            string selectedText;
            string languageText;
            if (!SecurityContext.IsAdministrator()
                || !arguments.TryGetValue("--apply-token", out token)
                || !arguments.TryGetValue("--auth-key", out keyHex)
                || !arguments.TryGetValue("--game-path", out gamePath)
                || !arguments.TryGetValue("--selected-mods", out selectedText)
                || !arguments.TryGetValue("--language", out languageText))
            {
                return 2;
            }

            UiLanguage requestedLanguage;
            if (!Localization.TryParse(languageText, out requestedLanguage)) return 2;
            Localization.SetLanguage(requestedLanguage);

            byte[] key = AuthenticationUtil.ParseKey(keyHex);
            gamePath = Path.GetFullPath(gamePath);
            PathGuard.RejectRootReparsePoint(gamePath);
            Catalog catalog = CatalogService.LoadAndValidate();
            if (!File.Exists(Path.Combine(gamePath, catalog.Game.Executable)))
            {
                return 4;
            }
            string resultPath = ApplyCoordinator.GetElevatedResultPath(gamePath, token);
            string resultDirectory = Path.GetDirectoryName(resultPath);
            Directory.CreateDirectory(resultDirectory);
            resultPath = ApplyCoordinator.GetElevatedResultPath(gamePath, token);
            if (File.Exists(resultPath))
            {
                return 3;
            }

            foreach (string staleResult in Directory.GetFiles(resultDirectory, "result-*.json", SearchOption.TopDirectoryOnly))
            {
                if (!string.Equals(staleResult, resultPath, StringComparison.OrdinalIgnoreCase)
                    && File.GetLastWriteTimeUtc(staleResult) < DateTime.UtcNow.AddDays(-7))
                {
                    try { File.Delete(staleResult); } catch { }
                }
            }

            ApplyResult result;
            try
            {
                List<string> selectedIds = string.IsNullOrWhiteSpace(selectedText)
                    ? new List<string>()
                    : new List<string>(selectedText.Split(new[] { ',' }, StringSplitOptions.RemoveEmptyEntries));
                ModConfigurationProfile configuration = null;
                string encodedConfiguration;
                if (arguments.TryGetValue("--config-profile", out encodedConfiguration)
                    && !string.IsNullOrWhiteSpace(encodedConfiguration))
                {
                    if (encodedConfiguration.Length > 65536) throw new InvalidDataException("The mod settings payload is too large.");
                    byte[] configurationBytes = Convert.FromBase64String(encodedConfiguration);
                    if (configurationBytes.Length > 49152) throw new InvalidDataException("The mod settings payload is too large.");
                    configuration = JsonStore.FromBytes<ModConfigurationProfile>(configurationBytes);
                }
                ApplyPlan plan = new ApplyPlan
                {
                    GamePath = gamePath,
                    SelectedModIds = selectedIds,
                    ConfigurationProfile = configuration,
                    Language = Localization.CurrentLanguageCode
                };
                result = new InstallEngine(catalog, false).Apply(plan, new DirectProgress());
            }
            catch (Exception exception)
            {
                result = new ApplyResult
                {
                    Success = false,
                    RollbackComplete = true,
                    Message = exception.Message
                };
            }

            byte[] payload = JsonStore.ToBytes(result);
            AuthenticatedApplyResult envelope = new AuthenticatedApplyResult
            {
                Payload = payload,
                HmacSha256 = AuthenticationUtil.HmacHex(key, payload)
            };
            JsonStore.Write(resultPath, envelope);
            return result.Success ? 0 : 1;
        }

        private static Dictionary<string, string> ParseArguments(string[] args)
        {
            Dictionary<string, string> values = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
            for (int index = 0; index < args.Length; index++)
            {
                string key = args[index];
                if (!key.StartsWith("--", StringComparison.Ordinal))
                {
                    continue;
                }

                if (index + 1 < args.Length && !args[index + 1].StartsWith("--", StringComparison.Ordinal))
                {
                    values[key] = args[++index];
                }
                else
                {
                    values[key] = string.Empty;
                }
            }

            return values;
        }

        private static void WriteTextResult(Dictionary<string, string> arguments, string text)
        {
            string path;
            if (arguments.TryGetValue("--result", out path) && !string.IsNullOrWhiteSpace(path))
            {
                string requested = Path.GetFullPath(path);
                string allowed = Path.GetFullPath(Path.Combine(AppPaths.BaseDirectory, "selftest-result.txt"));
                if (!string.Equals(requested, allowed, StringComparison.OrdinalIgnoreCase))
                {
                    throw new InvalidOperationException(Localization.T(
                        "The self-test result may only be created in the application directory.",
                        "Das Selftest-Ergebnis darf nur im Programmordner angelegt werden."));
                }

                byte[] bytes = System.Text.Encoding.UTF8.GetBytes(text ?? string.Empty);
                using (FileStream stream = new FileStream(allowed, FileMode.CreateNew, FileAccess.Write, FileShare.Read))
                {
                    stream.Write(bytes, 0, bytes.Length);
                    stream.Flush(true);
                }
            }
        }
    }
}
