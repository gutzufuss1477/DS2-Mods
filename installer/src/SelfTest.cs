using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace DS2ModSuite
{
    internal static class SelfTest
    {
        public static string Run()
        {
            string testRoot = Path.Combine(Path.GetTempPath(), "DS2ModSuite-SelfTest-" + Guid.NewGuid().ToString("N"));
            Directory.CreateDirectory(testRoot);
            StringBuilder report = new StringBuilder();
            try
            {
                Catalog catalog = CatalogService.LoadAndValidate();
                CatalogService.ValidatePayloads(catalog);
                report.AppendLine("PASS catalog and all payload hashes");

                List<ConfigFieldDefinition> definitions = ModConfigurationService.GetDefinitions(catalog);
                Assert(definitions.Count == 83 && definitions.Select(field => field.Target).Distinct(StringComparer.OrdinalIgnoreCase).Count() == 14,
                    "settings schema field/file coverage mismatch");
                List<ModSpec> filteredSettingsMods = ModSettingsWindow.FilterInstalledConfigurableMods(
                    catalog,
                    definitions,
                    new[] { catalog.Mods[0].Id, catalog.Mods[2].Id });
                Assert(filteredSettingsMods.Count == 1 && filteredSettingsMods[0].Id == catalog.Mods[0].Id,
                    "installed/configurable settings dropdown filter mismatch");
                List<ConfigFieldDefinition> maxLevelSettings = definitions
                    .Where(field => field.ModId == "construction-max-level-on-build")
                    .ToList();
                Assert(maxLevelSettings.Count == 2
                    && maxLevelSettings.Any(field => field.Key == "Enabled" && !field.Schema.Advanced)
                    && maxLevelSettings.Any(field => field.Key == "DebugLog" && field.Schema.Advanced),
                    "Construction Max Level on Build settings schema mismatch");
                List<ModSpec> maxLevelFilteredSettings = ModSettingsWindow.FilterInstalledConfigurableMods(
                    catalog,
                    definitions,
                    new[] { "construction-max-level-on-build" });
                Assert(maxLevelFilteredSettings.Count == 1
                    && maxLevelFilteredSettings[0].Id == "construction-max-level-on-build",
                    "Construction Max Level on Build installed-settings filter mismatch");
                ModConfigurationProfile configurationProfile = ModConfigurationService.LoadEffectiveProfile(catalog, null);
                string configurationError;
                Assert(ModConfigurationService.TryValidateProfile(catalog, configurationProfile, out configurationError),
                    "default settings profile failed validation: " + configurationError);
                ConfigFieldDefinition pickupCapacity = definitions.First(field => field.ModId == "pickup-cargo-capacity" && field.Key == "CapacityUnits");
                ModConfigurationService.SetValue(configurationProfile, pickupCapacity.Id, "333");
                Assert(!ModConfigurationService.TryValidateProfile(catalog, configurationProfile, out configurationError),
                    "invalid cargo-capacity multiple was accepted");
                ModConfigurationService.SetValue(configurationProfile, pickupCapacity.Id, "336");
                Assert(ModConfigurationService.TryValidateProfile(catalog, configurationProfile, out configurationError),
                    "valid settings profile was rejected: " + configurationError);
                SettingsStore.WriteGamePath("C:\\synthetic-game-a");
                SettingsStore.WriteLanguage(UiLanguage.German);
                SettingsStore.WriteGamePath("C:\\synthetic-game-b");
                Assert(SettingsStore.ReadLanguage() == UiLanguage.German
                    && SettingsStore.ReadGamePath() == "C:\\synthetic-game-b", "language/game-path read-modify-write persistence failed");
                Localization.SetLanguage(UiLanguage.German);
                Assert(catalog.Mods[0].LocalizedDescription == catalog.Mods[0].DescriptionDe, "German catalog localization failed");
                LoaderInspection localizedLoader = new LoaderInspection
                {
                    Status = LoaderStatus.KnownCompatible,
                    FilePath = "winmm.dll",
                    FileVersion = "9.7.2"
                };
                LoaderInspector.Relocalize(localizedLoader);
                Assert(localizedLoader.DisplayText == "ASI Loader 9.7.2 ist installiert", "German loader relocalization failed");
                Localization.SetLanguage(UiLanguage.English);
                Assert(catalog.Mods[0].LocalizedDescription == catalog.Mods[0].Description, "English catalog localization failed");
                LoaderInspector.Relocalize(localizedLoader);
                Assert(localizedLoader.DisplayText == "ASI Loader 9.7.2 is installed", "English loader relocalization failed");
                report.AppendLine("PASS English/German localization, persistence and 83-field settings schema validation");

                string runningExecutable = System.Diagnostics.Process.GetCurrentProcess().MainModule.FileName;
                File.Copy(runningExecutable, Path.Combine(testRoot, catalog.Game.Executable), true);

                ModSpec migrationMod = catalog.Mods[0];
                ModFileSpec migrationBinary = migrationMod.Files.First(file => !file.IsConfig);
                string syntheticOldName = "synthetic-old-build.asi";
                migrationMod.ObsoleteFiles = new List<ObsoleteFileSpec>
                {
                    new ObsoleteFileSpec { Target = syntheticOldName, Sha256 = migrationBinary.Sha256 }
                };
                EmbeddedData.CopyPayloadToFile(migrationBinary.Sha256, Path.Combine(testRoot, syntheticOldName));

                List<string> selected = catalog.Mods.Take(2).Select(mod => mod.Id).ToList();
                ApplyPlan installPlan = new ApplyPlan
                {
                    GamePath = testRoot,
                    SelectedModIds = selected,
                    ConfigurationProfile = configurationProfile,
                    Language = "en"
                };
                ApplyResult installResult = new InstallEngine(catalog, true).Apply(installPlan, new DirectProgress());
                Assert(installResult.Success, "temporary install failed: " + installResult.Message);
                Assert(File.Exists(Path.Combine(testRoot, catalog.Loader.FileName)), "loader was not installed");
                Assert(HashUtil.EqualsHash(HashUtil.FileSha256(Path.Combine(testRoot, catalog.Loader.FileName)), catalog.Loader.Sha256), "loader hash mismatch");
                Assert(!File.Exists(Path.Combine(testRoot, syntheticOldName)), "known obsolete ASI was not migrated");
                foreach (ModSpec mod in catalog.Mods.Take(2))
                {
                    foreach (ModFileSpec file in mod.Files)
                    {
                        Assert(File.Exists(Path.Combine(testRoot, file.Target)), "missing installed file: " + file.Target);
                    }
                }
                string pickupConfigurationPath = Path.Combine(testRoot, "PickupCargoCapacity.ini");
                Assert(File.ReadAllText(pickupConfigurationPath).Contains("CapacityUnits=336"),
                    "central settings profile was not written to the pickup INI");
                Assert(installResult.ConfigurationsUpdated == 3, "initial configured INI count was not reported");
                report.AppendLine("PASS install, loader dependency, obsolete-build migration and post-install verification");

                ApplyResult idempotentResult = new InstallEngine(catalog, true).Apply(installPlan, new DirectProgress());
                Assert(idempotentResult.Success, "idempotent apply failed: " + idempotentResult.Message);
                Assert(idempotentResult.Installed == 0 && idempotentResult.Updated == 0 && idempotentResult.Removed == 0
                    && idempotentResult.ConfigurationsUpdated == 0 && !idempotentResult.LoaderInstalled,
                    "idempotent apply reported unexpected changes");
                report.AppendLine("PASS idempotent re-apply");

                File.AppendAllText(pickupConfigurationPath, "; retained custom comment\r\n");
                ModConfigurationService.SetValue(configurationProfile, pickupCapacity.Id, "352");
                ApplyResult settingsOnlyResult = new InstallEngine(catalog, true).Apply(installPlan, new DirectProgress());
                string configuredText = File.ReadAllText(pickupConfigurationPath);
                Assert(settingsOnlyResult.Success && settingsOnlyResult.ConfigurationsUpdated == 1
                    && settingsOnlyResult.Installed == 0 && settingsOnlyResult.Updated == 0 && settingsOnlyResult.Repaired == 0,
                    "settings-only apply reported incorrect changes");
                Assert(configuredText.Contains("CapacityUnits=352") && configuredText.Contains("; retained custom comment"),
                    "settings-only apply did not preserve comments or write the new value");
                Assert(!ModConfigurationService.HasDifferences(catalog, configurationProfile, selected, testRoot),
                    "settings remained pending after a verified apply");
                report.AppendLine("PASS central settings UI profile, individual INI write, validation and comment preservation");

                File.WriteAllText(Path.Combine(testRoot, syntheticOldName), "unknown obsolete build");
                GameInspection conflictInspection = GameInspector.Inspect(catalog, testRoot, false);
                ModRuntimeState conflictState = GameInspector.InspectMods(conflictInspection, catalog).First(state => state.Spec.Id == migrationMod.Id);
                Assert(conflictState.HasUnknownObsoleteBinary && conflictState.StatusText == Localization.T("Conflicting file", "Konfliktdatei"),
                    "unknown obsolete build was not surfaced as a blocking conflict");
                ApplyResult conflictRemoval = new InstallEngine(catalog, true).Apply(
                    new ApplyPlan { GamePath = testRoot, SelectedModIds = new List<string> { catalog.Mods[1].Id } },
                    new DirectProgress());
                Assert(conflictRemoval.Success && conflictRemoval.RetainedFiles > 0,
                    "safe removal beside an unknown obsolete build was not reported correctly");
                foreach (ModFileSpec config in migrationMod.Files.Where(file => file.IsConfig))
                {
                    Assert(File.Exists(Path.Combine(testRoot, config.Target)),
                        "configuration was removed while an unknown obsolete binary remained: " + config.Target);
                }
                File.Delete(Path.Combine(testRoot, syntheticOldName));
                ApplyResult restoreAfterConflict = new InstallEngine(catalog, true).Apply(installPlan, new DirectProgress());
                Assert(restoreAfterConflict.Success, "reinstall after resolving obsolete conflict failed");
                report.AppendLine("PASS unknown obsolete-build conflict detection and companion-config preservation");

                ModFileSpec changedConfig = catalog.Mods.Take(2).SelectMany(mod => mod.Files).First(file => file.IsConfig);
                string changedConfigPath = Path.Combine(testRoot, changedConfig.Target);
                File.AppendAllText(changedConfigPath, Environment.NewLine + "; self-test user setting");
                ModFileSpec changedBinary = catalog.Mods[1].Files.First(file => !file.IsConfig);
                string changedBinaryPath = Path.Combine(testRoot, changedBinary.Target);
                File.AppendAllText(changedBinaryPath, "modified");
                ModFileSpec companionConfig = catalog.Mods[1].Files.First(file => file.IsConfig);
                string companionConfigPath = Path.Combine(testRoot, companionConfig.Target);
                GameInspection modifiedInspection = GameInspector.Inspect(catalog, testRoot, false);
                ModRuntimeState modifiedState = GameInspector.InspectMods(modifiedInspection, catalog).First(state => state.Spec.Id == catalog.Mods[1].Id);
                Assert(!modifiedState.DesiredEnabled && modifiedState.StatusText == Localization.T("Different version", "Andere Version"),
                    "modified ASI was selected automatically");
                ApplyPlan removePlan = new ApplyPlan { GamePath = testRoot, SelectedModIds = new List<string>() };
                ApplyResult removeResult = new InstallEngine(catalog, true).Apply(removePlan, new DirectProgress());
                Assert(removeResult.Success, "temporary removal failed: " + removeResult.Message);
                Assert(File.Exists(changedConfigPath), "modified configuration was removed");
                ModFileSpec exactBinary = catalog.Mods[0].Files.First(file => !file.IsConfig);
                Assert(!File.Exists(Path.Combine(testRoot, exactBinary.Target)), "unchanged managed ASI was not removed: " + exactBinary.Target);
                Assert(File.Exists(changedBinaryPath), "modified ASI was removed without an exact hash match");
                Assert(File.Exists(companionConfigPath), "companion configuration of a modified ASI was removed");
                Assert(removeResult.RetainedFiles > 0, "retained modified files were not reported");
                report.AppendLine("PASS safe removal, retained-file reporting and preservation of modified ASI companion/config files");

                string unknownLoaderRoot = Path.Combine(testRoot, "unknown-loader");
                Directory.CreateDirectory(unknownLoaderRoot);
                File.Copy(runningExecutable, Path.Combine(unknownLoaderRoot, catalog.Game.Executable), true);
                string unknownProxy = Path.Combine(unknownLoaderRoot, catalog.Loader.FileName);
                File.WriteAllText(unknownProxy, "not a loader");
                LoaderInspection unknownInspection = LoaderInspector.Inspect(unknownLoaderRoot, catalog.Loader);
                Assert(unknownInspection.Status == LoaderStatus.UnknownProxy, "unknown proxy DLL was not detected safely");
                Assert(File.ReadAllText(unknownProxy) == "not a loader", "unknown proxy DLL was modified during inspection");
                ApplyPlan blockedPlan = new ApplyPlan
                {
                    GamePath = unknownLoaderRoot,
                    SelectedModIds = new List<string> { catalog.Mods[0].Id }
                };
                ApplyResult blockedResult = new InstallEngine(catalog, true).Apply(blockedPlan, new DirectProgress());
                Assert(!blockedResult.Success, "unknown proxy DLL did not block a mod installation");
                Assert(File.ReadAllText(unknownProxy) == "not a loader", "unknown proxy DLL was modified by a blocked installation");
                string secondProxy = Path.Combine(unknownLoaderRoot, "dinput8.dll");
                EmbeddedData.CopyPayloadToFile(catalog.Loader.Sha256, secondProxy);
                LoaderInspection multipleInspection = LoaderInspector.Inspect(unknownLoaderRoot, catalog.Loader);
                Assert(multipleInspection.Status == LoaderStatus.UnknownProxy && multipleInspection.OtherProxyCount == 1,
                    "multiple proxy DLL conflict was not detected");
                report.AppendLine("PASS strict loader allowlist, unknown/multiple proxy block without overwrite");

                string original = Path.Combine(testRoot, "rollback-target.bin");
                string replacement = Path.Combine(testRoot, "rollback-source.bin");
                File.WriteAllText(original, "ORIGINAL");
                File.WriteAllText(replacement, "REPLACEMENT");
                using (InstallLogger logger = new InstallLogger())
                {
                    try
                    {
                        using (FileTransaction transaction = new FileTransaction(logger))
                        {
                            transaction.ReplaceFrom(replacement, original, HashUtil.FileSha256(replacement));
                            throw new InvalidOperationException("synthetic failure");
                        }
                    }
                    catch (InvalidOperationException)
                    {
                        // Expected: disposing the uncommitted transaction restores the original file.
                    }
                }
                Assert(File.ReadAllText(original) == "ORIGINAL", "rollback did not restore the original file");
                string retainedTarget = Path.Combine(testRoot, "retained-target.bin");
                File.WriteAllText(retainedTarget, "FOREIGN");
                using (InstallLogger logger = new InstallLogger())
                using (FileTransaction transaction = new FileTransaction(logger))
                {
                    bool removed = transaction.DeleteIfExact(retainedTarget, HashUtil.FileSha256(replacement));
                    Assert(!removed, "mismatched file was removed");
                    transaction.Commit();
                }
                Assert(File.ReadAllText(retainedTarget) == "FOREIGN", "mismatched file changed during quarantine check");
                report.AppendLine("PASS transactional rollback and atomic exact-hash removal guard");

                byte[] authenticatedPayload = JsonStore.ToBytes(new ApplyResult { Success = true, Message = "test" });
                byte[] authenticationKey = AuthenticationUtil.ParseKey(AuthenticationUtil.CreateKeyHex());
                string authenticatedMac = AuthenticationUtil.HmacHex(authenticationKey, authenticatedPayload);
                Assert(AuthenticationUtil.FixedTimeEqualsHex(authenticatedMac, AuthenticationUtil.HmacHex(authenticationKey, authenticatedPayload)),
                    "valid UAC result authentication failed");
                authenticatedPayload[0] ^= 1;
                Assert(!AuthenticationUtil.FixedTimeEqualsHex(authenticatedMac, AuthenticationUtil.HmacHex(authenticationKey, authenticatedPayload)),
                    "tampered UAC result authentication was accepted");
                Assert(ApplyCoordinator.QuoteArgument("C:\\") == "\"C:\\\\\"",
                    "Windows argument quoting did not protect a trailing backslash");
                report.AppendLine("PASS authenticated UAC result integrity check");

                string escaped = PathGuard.ResolveUnderRoot(testRoot, "safe\\file.txt");
                Assert(escaped.StartsWith(Path.GetFullPath(testRoot), StringComparison.OrdinalIgnoreCase), "safe path rejected");
                bool traversalRejected = false;
                try { PathGuard.ResolveUnderRoot(testRoot, "..\\escape.txt"); }
                catch (InvalidOperationException) { traversalRejected = true; }
                Assert(traversalRejected, "path traversal was not rejected");
                bool alternateStreamRejected = false;
                try { PathGuard.ResolveUnderRoot(testRoot, "safe.txt:stream"); }
                catch (InvalidOperationException) { alternateStreamRejected = true; }
                Assert(alternateStreamRejected, "alternate data stream was not rejected");
                report.AppendLine("PASS path traversal and alternate-data-stream guards");
                report.AppendLine("SELF-TEST PASSED");
                return report.ToString();
            }
            finally
            {
                try { Directory.Delete(testRoot, true); } catch { }
                try { Directory.Delete(AppPaths.UserDirectory, true); } catch { }
            }
        }

        private static void Assert(bool condition, string message)
        {
            if (!condition)
            {
                throw new InvalidOperationException("SELF-TEST FAILED: " + message);
            }
        }
    }
}
