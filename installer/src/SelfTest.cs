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
                Assert(catalog.SuiteVersion == "1.7.0" && catalog.Mods.Count == 22,
                    "suite version/mod count mismatch");
                report.AppendLine("PASS catalog and all payload hashes");

                List<ConfigFieldDefinition> definitions = ModConfigurationService.GetDefinitions(catalog);
                Assert(definitions.Count == 184 && definitions.Select(field => field.Target).Distinct(StringComparer.OrdinalIgnoreCase).Count() == 20,
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
                List<ConfigFieldDefinition> coffinBoardSettings = definitions
                    .Where(field => field.ModId == "coffin-board-all-terrain-speed")
                    .ToList();
                Assert(coffinBoardSettings.Count == 6
                    && coffinBoardSettings.Any(field => field.Key == "SpeedPercent"
                        && field.Schema.Min == 100 && field.Schema.Max == 1000 && field.DefaultValue == "500")
                    && coffinBoardSettings.Any(field => field.Key == "AccelerationPercent"
                        && field.Schema.Min == 100 && field.Schema.Max == 500 && field.DefaultValue == "400")
                    && coffinBoardSettings.Any(field => field.Key == "AllowFloatingCarrier"
                        && field.DefaultValue == "1"),
                    "Coffin Board settings schema mismatch");
                Assert(new[] { "Enabled", "EnableNetworkTraversal", "EnableOutsideNetworkMount" }
                    .All(key => coffinBoardSettings.Any(field => field.Key == key && field.DefaultValue == "1")),
                    "Coffin Board Reworked switches are missing or disabled by default");
                List<ModSpec> coffinBoardFilteredSettings = ModSettingsWindow.FilterInstalledConfigurableMods(
                    catalog,
                    definitions,
                    new[] { "coffin-board-all-terrain-speed" });
                Assert(coffinBoardFilteredSettings.Count == 1
                    && coffinBoardFilteredSettings[0].Id == "coffin-board-all-terrain-speed",
                    "Coffin Board installed-settings filter mismatch");
                ModConfigurationProfile configurationProfile = ModConfigurationService.LoadEffectiveProfile(catalog, null);
                string configurationError;
                Assert(ModConfigurationService.TryValidateProfile(catalog, configurationProfile, out configurationError),
                    "default settings profile failed validation: " + configurationError);
                ConfigFieldDefinition coffinBoardSpeed = coffinBoardSettings.First(field => field.Key == "SpeedPercent");
                ModConfigurationService.SetValue(configurationProfile, coffinBoardSpeed.Id, "1001");
                Assert(!ModConfigurationService.TryValidateProfile(catalog, configurationProfile, out configurationError),
                    "out-of-range Coffin Board speed was accepted");
                ModConfigurationService.SetValue(configurationProfile, coffinBoardSpeed.Id, "500");
                Assert(ModConfigurationService.TryValidateProfile(catalog, configurationProfile, out configurationError),
                    "valid Coffin Board settings were rejected: " + configurationError);
                string legacyCoffinRoot = Path.Combine(testRoot, "legacy-coffin");
                Directory.CreateDirectory(legacyCoffinRoot);
                string legacyCoffinPath = Path.Combine(legacyCoffinRoot, "ds2_coffin_board_all_terrain_speed.ini");
                File.WriteAllText(legacyCoffinPath,
                    "[CoffinBoardAllTerrainSpeed]\r\nSpeedPercent=500\r\nAccelerationPercent=400\r\n"
                    + "SteeringPercent=250\r\nWetGripPercent=300\r\nSpeedTelemetry=1\r\n\r\n[Unrelated]\r\nKeep=1\r\n");
                Assert(ModConfigurationService.HasDifferences(catalog, configurationProfile,
                    new[] { "coffin-board-all-terrain-speed" }, legacyCoffinRoot),
                    "legacy Coffin Board test keys were not detected");
                byte[] normalizedCoffinIni = ModConfigurationService.BuildConfiguredIni(
                    catalog, configurationProfile, "ds2_coffin_board_all_terrain_speed.ini", legacyCoffinPath);
                string normalizedCoffinText = Encoding.UTF8.GetString(normalizedCoffinIni);
                Assert(!normalizedCoffinText.Contains("SteeringPercent=")
                    && !normalizedCoffinText.Contains("WetGripPercent=")
                    && !normalizedCoffinText.Contains("SpeedTelemetry=")
                    && normalizedCoffinText.Contains("SpeedPercent=500")
                    && normalizedCoffinText.Contains("AccelerationPercent=400")
                    && normalizedCoffinText.Contains("[Unrelated]")
                    && normalizedCoffinText.Contains("Keep=1"),
                    "legacy Coffin Board INI was not safely normalized");
                File.WriteAllBytes(legacyCoffinPath, normalizedCoffinIni);
                Assert(ModConfigurationService.ConfiguredIniMatches(
                    catalog, configurationProfile, "ds2_coffin_board_all_terrain_speed.ini", legacyCoffinPath),
                    "normalized Coffin Board INI did not match the stable schema");
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
                report.AppendLine("PASS English/German localization, persistence and 184-field settings schema validation");

                string runningExecutable = System.Diagnostics.Process.GetCurrentProcess().MainModule.FileName;
                File.Copy(runningExecutable, Path.Combine(testRoot, catalog.Game.Executable), true);
                TestBackpackUpgrade(catalog, testRoot, runningExecutable);
                report.AppendLine("PASS Backpack legacy filename migration, conflict guard, idempotence, removal and charm-state preservation");
                TestOdradekSettings(catalog, testRoot, runningExecutable);
                report.AppendLine("PASS Odradek defaults, bounds/modes, profile upgrade, install/adoption, settings, idempotence and removal");
                TestApasOptIn(catalog, testRoot, runningExecutable);
                report.AppendLine("PASS APAS default-off fresh install, legacy migration with/without profile, explicit opt-in/out, preservation and idempotence");

                string noProfileUpgradeRoot = Path.Combine(testRoot, "coffin-no-profile-upgrade");
                Directory.CreateDirectory(noProfileUpgradeRoot);
                File.Copy(runningExecutable, Path.Combine(noProfileUpgradeRoot, catalog.Game.Executable), true);
                File.WriteAllText(Path.Combine(noProfileUpgradeRoot, "ds2_coffin_board_all_terrain_speed.asi"), "legacy test binary");
                string noProfileLegacyIni = Path.Combine(noProfileUpgradeRoot, "ds2_coffin_board_all_terrain_speed.ini");
                File.WriteAllText(noProfileLegacyIni,
                    "[CoffinBoardAllTerrainSpeed]\r\nSpeedPercent=650\r\nAccelerationPercent=999\r\n"
                    + "SteeringPercent=250\r\nWetGripPercent=300\r\nTelemetry=1\r\n\r\n[Unrelated]\r\nKeep=1\r\n");
                ApplyResult noProfileUpgrade = new InstallEngine(catalog, true).Apply(
                    new ApplyPlan
                    {
                        GamePath = noProfileUpgradeRoot,
                        SelectedModIds = new List<string> { "coffin-board-all-terrain-speed" },
                        ConfigurationProfile = null,
                        Language = "en"
                    },
                    new DirectProgress());
                string noProfileMigratedText = File.ReadAllText(noProfileLegacyIni);
                Assert(noProfileUpgrade.Success && noProfileUpgrade.Updated == 1 && noProfileUpgrade.ConfigurationsUpdated == 1,
                    "Coffin Board no-profile TEST upgrade failed: " + noProfileUpgrade.Message);
                Assert(noProfileMigratedText.Contains("SpeedPercent=650")
                    && noProfileMigratedText.Contains("AccelerationPercent=400")
                    && noProfileMigratedText.Contains("Enabled=1")
                    && noProfileMigratedText.Contains("EnableNetworkTraversal=1")
                    && noProfileMigratedText.Contains("EnableOutsideNetworkMount=1")
                    && !noProfileMigratedText.Contains("SteeringPercent=")
                    && !noProfileMigratedText.Contains("WetGripPercent=")
                    && !noProfileMigratedText.Contains("Telemetry=")
                    && noProfileMigratedText.Contains("[Unrelated]")
                    && ModConfigurationService.StableExistingIniMatches(catalog, "coffin-board-all-terrain-speed",
                        "ds2_coffin_board_all_terrain_speed.ini", noProfileLegacyIni),
                    "Coffin Board no-profile TEST INI was not safely migrated");
                byte[] stableNoProfileBytes = File.ReadAllBytes(noProfileLegacyIni);
                ApplyResult noProfileIdempotent = new InstallEngine(catalog, true).Apply(
                    new ApplyPlan
                    {
                        GamePath = noProfileUpgradeRoot,
                        SelectedModIds = new List<string> { "coffin-board-all-terrain-speed" },
                        ConfigurationProfile = null,
                        Language = "en"
                    },
                    new DirectProgress());
                Assert(noProfileIdempotent.Success && noProfileIdempotent.Installed == 0
                    && noProfileIdempotent.Updated == 0 && noProfileIdempotent.Repaired == 0
                    && noProfileIdempotent.ConfigurationsUpdated == 0 && !noProfileIdempotent.LoaderInstalled,
                    "Coffin Board no-profile migration was not idempotent");
                Assert(stableNoProfileBytes.SequenceEqual(File.ReadAllBytes(noProfileLegacyIni)),
                    "stable Coffin Board configuration was rewritten during idempotent apply");
                report.AppendLine("PASS Coffin Board TEST upgrade without central profile, strict INI migration and idempotent re-apply");

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

        private static void TestBackpackUpgrade(Catalog catalog, string testRoot, string runningExecutable)
        {
            ModSpec mod = catalog.Mods.Single(item => item.Id == "high-density-backpack-modules");
            ObsoleteFileSpec old = mod.ObsoleteFiles.Single();
            ModFileSpec current = mod.Files.Single();
            Assert(mod.Version == "1.1.0" && old.Target == "DS2_HighDensityBackpackModules_v1.0.0.asi"
                && old.Sha256 == "C4C5CC2B525BEB88AEF264521B56C00E6616ABF6DFB0C2F6970C92379D719C1C"
                && current.Target == "DS2_HighDensityBackpackModules_v1.1.0.asi",
                "Backpack release/migration metadata mismatch");
            string gameRoot = Path.Combine(testRoot, "backpack-upgrade");
            Directory.CreateDirectory(gameRoot);
            File.Copy(runningExecutable, Path.Combine(gameRoot, catalog.Game.Executable));
            string oldPath = Path.Combine(gameRoot, old.Target);
            File.WriteAllText(oldPath, "synthetic known legacy backpack");
            string charms = Path.Combine(gameRoot, "DS2_HighDensityBackpackModules.charms.ini");
            File.WriteAllText(charms, "; player's extra charm selections\r\n[Charms]\r\nSlot0=42\r\n");
            byte[] charmBytes = File.ReadAllBytes(charms);
            string releasedHash = old.Sha256;
            try
            {
                old.Sha256 = HashUtil.FileSha256(oldPath);
                ApplyPlan plan = new ApplyPlan { GamePath = gameRoot, SelectedModIds = new List<string> { mod.Id } };
                ModRuntimeState state = GameInspector.InspectMods(GameInspector.Inspect(catalog, gameRoot, false), catalog)
                    .Single(item => item.Spec.Id == mod.Id);
                Assert(state.HasObsoleteBinary && !state.HasUnknownObsoleteBinary, "old Backpack was not recognized");
                ApplyResult upgraded = new InstallEngine(catalog, true).Apply(plan, new DirectProgress());
                Assert(upgraded.Success && upgraded.Updated == 1 && !File.Exists(oldPath)
                    && Directory.GetFiles(gameRoot, "*.asi").Length == 1
                    && HashUtil.EqualsHash(HashUtil.FileSha256(Path.Combine(gameRoot, current.Target)), current.Sha256)
                    && charmBytes.SequenceEqual(File.ReadAllBytes(charms)), "Backpack upgrade lost state or left two ASIs");
                ApplyResult repeated = new InstallEngine(catalog, true).Apply(plan, new DirectProgress());
                Assert(repeated.Success && repeated.Updated == 0 && repeated.Installed == 0 && repeated.Repaired == 0,
                    "Backpack upgrade was not idempotent");
                File.WriteAllText(oldPath, "unknown changed backpack");
                ApplyResult conflict = new InstallEngine(catalog, true).Apply(plan, new DirectProgress());
                Assert(!conflict.Success && File.ReadAllText(oldPath) == "unknown changed backpack"
                    && charmBytes.SequenceEqual(File.ReadAllBytes(charms)), "unknown legacy Backpack was overwritten");
                File.Delete(oldPath);
                plan.SelectedModIds.Clear();
                ApplyResult removed = new InstallEngine(catalog, true).Apply(plan, new DirectProgress());
                Assert(removed.Success && !File.Exists(Path.Combine(gameRoot, current.Target))
                    && charmBytes.SequenceEqual(File.ReadAllBytes(charms)), "Backpack removal lost charm preferences");
            }
            finally { old.Sha256 = releasedHash; }
        }

        private static void TestOdradekSettings(Catalog catalog, string testRoot, string runningExecutable)
        {
            const string modId = "improved-odradek-scan";
            List<ConfigFieldDefinition> fields = ModConfigurationService.GetDefinitions(catalog)
                .Where(field => field.ModId == modId).ToList();
            ConfigFieldDefinition range = fields.Single(field => field.Key == "RangeMeters");
            ConfigFieldDefinition scale = fields.Single(field => field.Key == "VisualWaveScale");
            ConfigFieldDefinition mode = fields.Single(field => field.Key == "FullCircleMode");
            Assert(fields.Count == 7 && range.DefaultValue == "500" && scale.DefaultValue == "1"
                && mode.DefaultValue == "Sphere" && mode.Schema.Advanced && scale.Schema.Advanced
                && fields.Single(field => field.Key == "DebugLog").DefaultValue == "0",
                "Odradek release defaults mismatch");
            ModConfigurationProfile profile = ModConfigurationService.LoadEffectiveProfile(catalog, null);
            string error;
            foreach (string invalid in new[] { "49", "1001", "NaN" })
            {
                ModConfigurationService.SetValue(profile, range.Id, invalid);
                Assert(!ModConfigurationService.TryValidateProfile(catalog, profile, out error), "invalid Odradek range accepted");
            }
            ModConfigurationService.SetValue(profile, range.Id, "725.5");
            foreach (string invalid in new[] { "0.24", "1.01" })
            {
                ModConfigurationService.SetValue(profile, scale.Id, invalid);
                Assert(!ModConfigurationService.TryValidateProfile(catalog, profile, out error), "invalid visual scale accepted");
            }
            ModConfigurationService.SetValue(profile, scale.Id, "0.5");
            ModConfigurationService.SetValue(profile, mode.Id, "unsupported");
            Assert(!ModConfigurationService.TryValidateProfile(catalog, profile, out error), "invalid scan mode accepted");
            foreach (string valid in new[] { "Sphere", "Fan360", "Spherical" })
            {
                ModConfigurationService.SetValue(profile, mode.Id, valid);
                Assert(ModConfigurationService.TryValidateProfile(catalog, profile, out error), "valid Odradek mode rejected");
            }

            // Loading a v1.6 profile must add the seven new defaults and preserve APAS opt-in.
            ModConfigurationProfile legacy = ModConfigurationService.CloneProfile(profile);
            legacy.Values.RemoveAll(value => value.Id.StartsWith(modId + "|", StringComparison.Ordinal));
            ConfigFieldDefinition unlock = ModConfigurationService.GetDefinitions(catalog)
                .Single(field => field.ModId == "apas-memory-costs" && field.Key == "UnlockAll");
            ModConfigurationService.SetValue(legacy, unlock.Id, "1");
            JsonStore.Write(ModConfigurationService.ProfilePath, legacy);
            try
            {
                ModConfigurationProfile migrated = ModConfigurationService.LoadStoredProfile(catalog);
                Assert(ModConfigurationService.TryValidateProfile(catalog, migrated, out error)
                    && ModConfigurationService.GetValue(migrated, range.Id) == "500"
                    && ModConfigurationService.GetValue(migrated, unlock.Id) == "1", "v1.6 profile migration lost settings");
            }
            finally { File.Delete(ModConfigurationService.ProfilePath); }

            foreach (bool adopting in new[] { false, true })
            {
                string gameRoot = Path.Combine(testRoot, "odradek-" + adopting);
                Directory.CreateDirectory(gameRoot);
                File.Copy(runningExecutable, Path.Combine(gameRoot, catalog.Game.Executable));
                string ini = Path.Combine(gameRoot, "ds2_odradek_scan.ini");
                if (adopting) File.WriteAllText(ini, "; custom scan\r\n[OdradekScan]\r\nEnabled=1\r\nRangeMeters=650\r\n[Unrelated]\r\nKeep=1\r\n");
                ApplyPlan plan = new ApplyPlan { GamePath = gameRoot, SelectedModIds = new List<string> { modId } };
                ApplyResult installed = new InstallEngine(catalog, true).Apply(plan, new DirectProgress());
                Assert(installed.Success && ModConfigurationService.StableExistingIniMatches(catalog, modId, "ds2_odradek_scan.ini", ini)
                    && File.ReadAllText(ini).Contains(adopting ? "RangeMeters=650" : "RangeMeters=500"),
                    "Odradek install/adoption failed");
                plan.ConfigurationProfile = profile;
                ApplyResult configured = new InstallEngine(catalog, true).Apply(plan, new DirectProgress());
                Assert(configured.Success && File.ReadAllText(ini).Contains("RangeMeters=725.5")
                    && File.ReadAllText(ini).Contains("FullCircleMode=Spherical")
                    && (!adopting || File.ReadAllText(ini).Contains("Keep=1")), "Odradek settings write failed");
                ApplyResult repeated = new InstallEngine(catalog, true).Apply(plan, new DirectProgress());
                Assert(repeated.Success && repeated.ConfigurationsUpdated == 0 && repeated.Updated == 0, "Odradek reapply changed files");
                byte[] configuredBytes = File.ReadAllBytes(ini);
                plan.SelectedModIds.Clear();
                ApplyResult removed = new InstallEngine(catalog, true).Apply(plan, new DirectProgress());
                Assert(removed.Success && !File.Exists(Path.Combine(gameRoot, "ds2_odradek_scan.asi"))
                    && configuredBytes.SequenceEqual(File.ReadAllBytes(ini)), "Odradek removal lost custom INI");
            }
        }

        private static void TestApasOptIn(Catalog catalog, string testRoot, string runningExecutable)
        {
            const string modId = "apas-memory-costs";
            const string target = "ds2_apas_memory_costs.ini";
            ModSpec apas = catalog.Mods.First(mod => mod.Id == modId);
            ModFileSpec binary = apas.Files.Single(file => !file.IsConfig);
            ConfigFieldDefinition unlock = ModConfigurationService.GetDefinitions(catalog)
                .Single(field => field.ModId == modId && field.Key == "UnlockAll");
            Assert(apas.Version == "3.0.0-rc.1" && unlock.DefaultValue == "0" && !unlock.Schema.Advanced,
                "APAS optional unlock must be visible and off by default");

            foreach (bool withProfile in new[] { false, true })
            foreach (string previous in new[] { "fresh", "legacy", "0", "1" })
            {
                string gameRoot = Path.Combine(testRoot, "apas-" + withProfile + "-" + previous);
                Directory.CreateDirectory(gameRoot);
                File.Copy(runningExecutable, Path.Combine(gameRoot, catalog.Game.Executable));
                string iniPath = Path.Combine(gameRoot, target);
                if (previous != "fresh")
                {
                    File.WriteAllText(Path.Combine(gameRoot, binary.Target), "old APAS binary");
                    File.WriteAllText(iniPath, "; custom cost\r\n[APASMemoryCosts]\r\nEnabled=0\r\nGlobalCost=7\r\nCustomKey=42\r\n"
                        + (previous == "legacy" ? "" : "[APASUnlocks]\r\nUnlockAll=" + previous + "\r\n")
                        + "[Unrelated]\r\nKeep=1\r\n");
                }
                ModConfigurationProfile profile = withProfile
                    ? ModConfigurationService.LoadEffectiveProfile(catalog, gameRoot) : null;
                if (withProfile && previous == "legacy")
                {
                    Assert(ModConfigurationService.HasDifferences(catalog, profile, new[] { modId }, gameRoot),
                        "missing APAS opt-in must be detected as a configuration change");
                    Assert(!ModConfigurationService.ConfiguredIniMatches(catalog, profile, target, iniPath),
                        "missing APAS opt-in must not be treated as the safe default on disk");
                }
                ApplyPlan plan = new ApplyPlan
                {
                    GamePath = gameRoot, SelectedModIds = new List<string> { modId },
                    ConfigurationProfile = profile, Language = "en"
                };
                ApplyResult applied = new InstallEngine(catalog, true).Apply(plan, new DirectProgress());
                Assert(applied.Success, "APAS install/migration failed: " + applied.Message);
                string installedText = File.ReadAllText(iniPath);
                IniDocument installed = IniDocument.Parse(installedText);
                Assert(installed.GetValue("APASUnlocks", "UnlockAll", null) == (previous == "1" ? "1" : "0"),
                    "APAS install changed or implicitly enabled the unlock choice");
                Assert(HashUtil.EqualsHash(HashUtil.FileSha256(Path.Combine(gameRoot, binary.Target)), binary.Sha256)
                    && Directory.GetFiles(gameRoot, "*.asi").Length == 1,
                    "APAS must use exactly one replacement ASI");
                if (previous != "fresh")
                    Assert(installed.GetValue("APASMemoryCosts", "Enabled", null) == "0"
                        && installed.GetValue("APASMemoryCosts", "GlobalCost", null) == "7"
                        && installedText.Contains("CustomKey=42") && installedText.Contains("Keep=1")
                        && installedText.Contains("; custom cost"), "APAS migration lost custom values/comments");
                byte[] before = File.ReadAllBytes(iniPath);
                ApplyResult repeated = new InstallEngine(catalog, true).Apply(plan, new DirectProgress());
                Assert(repeated.Success && repeated.ConfigurationsUpdated == 0 && repeated.Updated == 0
                    && repeated.Installed == 0 && repeated.Repaired == 0 && before.SequenceEqual(File.ReadAllBytes(iniPath)),
                    "APAS migration was not idempotent");

                if (withProfile)
                {
                    foreach (string choice in new[] { "1", "0" })
                    {
                        ModConfigurationService.SetValue(profile, unlock.Id, choice);
                        ApplyResult toggled = new InstallEngine(catalog, true).Apply(plan, new DirectProgress());
                        Assert(toggled.Success && IniDocument.Parse(File.ReadAllText(iniPath))
                            .GetValue("APASUnlocks", "UnlockAll", null) == choice,
                            "explicit APAS opt-in/out failed");
                    }
                }
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
