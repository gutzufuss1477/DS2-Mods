using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace DS2ModSuite
{
    internal static partial class SelfTest
    {
        private static bool IsRelease1120Field(ConfigFieldDefinition f)
        {
            return (f.ModId == "sam-overhaul" && new[] { "Footprints", "GeneratorRange", "TimefallShelterRange" }.Contains(f.Section))
                || (f.ModId == "pickup-cargo-capacity" && f.Key == "RearIndicatorScaling")
                || (f.ModId == "infrastructure-one-unit" && f.Key == "MineProductionCrystalCost")
                || (f.ModId == "crafting-unlocks" && f.Section == "PCCStructures");
        }

        private static void TestRelease1120(Catalog catalog, string testRoot, string executable, StringBuilder report)
        {
            var fields = ModConfigurationService.GetDefinitions(catalog);
            var added = fields.Where(IsRelease1120Field).ToList();
            Assert(added.Count == 11, "v1.12 field delta mismatch");
            var expected = new Dictionary<string, string> {
                { "Footprints|HideFootprints", "0" }, { "GeneratorRange|Enabled", "0" },
                { "GeneratorRange|RangePercent", "200" }, { "TimefallShelterRange|Enabled", "0" },
                { "TimefallShelterRange|RangePercent", "200" }, { "TimefallShelterRange|RepairRadiusPercent", "215" },
                { "TimefallShelterRange|FixRestPrompt", "1" }, { "TimefallShelterRange|SpatialDiagnostics", "0" },
                { "PickupCargoCapacity|RearIndicatorScaling", "1" },
                { "InfrastructureOneUnit|MineProductionCrystalCost", "0" }, { "PCCStructures|UnlockAll", "1" }
            };
            Assert(added.All(f => f.DefaultValue == expected[f.Section + "|" + f.Key]), "standalone defaults changed");
            Assert(added.Single(f => f.Key == "SpatialDiagnostics").Schema.Advanced, "developer diagnostics must be advanced");
            // Same-name releases must remain selected for upgrade; unknown files stay untouched on removal.
            foreach (var mod in catalog.Mods.Where(m => m.Files.Any(f => f.PreviousHashes != null)))
            {
                string root = Path.Combine(testRoot, "same-name-" + mod.Id);
                Directory.CreateDirectory(root);
                File.Copy(executable, Path.Combine(root, catalog.Game.Executable));
                var binary = mod.Files.First(f => !f.IsConfig);
                var hashes = binary.PreviousHashes;
                string path = Path.Combine(root, binary.Target);
                try
                {
                    File.WriteAllText(path, "trusted previous " + mod.Id);
                    binary.PreviousHashes = new List<string> { HashUtil.FileSha256(path) };
                    var game = GameInspector.Inspect(catalog, root, false);
                    var state = GameInspector.InspectMods(game, catalog).Single(m => m.Spec.Id == mod.Id);
                    Assert(state.DesiredEnabled && state.HasObsoleteBinary && !state.HasUnknownObsoleteBinary, "same-name upgrade not detected");
                    state.DesiredEnabled = false;
                    GameInspector.UpdateStatus(state);
                    Assert(state.StatusText == Localization.T("Will be removed", "Wird entfernt"), "known previous removal status misleading");
                    var engine = new InstallEngine(catalog, true);
                    var plan = new ApplyPlan { GamePath = root, SelectedModIds = new List<string> { mod.Id } };
                    var result = engine.Apply(plan, new DirectProgress());
                    Assert(result.Success && result.Updated == 1 && HashUtil.EqualsHash(HashUtil.FileSha256(path), binary.Sha256), "same-name upgrade failed");
                    File.WriteAllText(path, "trusted previous " + mod.Id);
                    plan.SelectedModIds.Clear();
                    Assert(engine.Apply(plan, new DirectProgress()).Success && !File.Exists(path), "known previous binary not removed");
                    File.WriteAllText(path, "unknown custom binary");
                    state = GameInspector.InspectMods(game, catalog).Single(m => m.Spec.Id == mod.Id);
                    Assert(!state.DesiredEnabled && !state.HasObsoleteBinary, "unknown version selected automatically");
                    Assert(engine.Apply(plan, new DirectProgress()).Success && File.ReadAllText(path) == "unknown custom binary", "unknown binary removed");
                }
                finally { binary.PreviousHashes = hashes; }
            }
            var defaults = ModConfigurationService.LoadEffectiveProfile(catalog, null);
            string error;
            foreach (var f in added.Where(f => f.Schema.Min.HasValue))
            {
                foreach (decimal value in new[] { f.Schema.Min.Value, f.Schema.Max.Value })
                {
                    ModConfigurationService.SetValue(defaults, f.Id, value.ToString(System.Globalization.CultureInfo.InvariantCulture));
                    Assert(ModConfigurationService.TryValidateProfile(catalog, defaults, out error), "valid new bound rejected");
                }
                foreach (decimal value in new[] { f.Schema.Min.Value - 1, f.Schema.Max.Value + 1 })
                {
                    ModConfigurationService.SetValue(defaults, f.Id, value.ToString(System.Globalization.CultureInfo.InvariantCulture));
                    Assert(!ModConfigurationService.TryValidateProfile(catalog, defaults, out error), "invalid new bound accepted");
                }
                ModConfigurationService.SetValue(defaults, f.Id, f.DefaultValue);
            }
            foreach (bool storedProfile in new[] { false, true })
            foreach (bool explicitOptions in new[] { false, true })
            {
                string root = Path.Combine(testRoot, "release1120-" + storedProfile + "-" + explicitOptions);
                Directory.CreateDirectory(root);
                File.Copy(executable, Path.Combine(root, catalog.Game.Executable));
                File.WriteAllText(Path.Combine(root, "ds2_sam_overhaul.ini"), "; personal Sam note\r\n[AutoDrive]\r\nActivationSeconds=3.5\r\n[Custom]\r\nKeep=1\r\n"
                    + (explicitOptions ? "[Footprints]\r\nHideFootprints=1\r\n[GeneratorRange]\r\nEnabled=1\r\nRangePercent=300\r\n[TimefallShelterRange]\r\nEnabled=1\r\nRepairRadiusPercent=310\r\n" : ""));
                File.WriteAllText(Path.Combine(root, "PickupCargoCapacity.ini"), "; personal pickup note\r\n[PickupCargoCapacity]\r\nEnabled=1\r\nCapacityUnits=400\r\n"
                    + (explicitOptions ? "RearIndicatorScaling=0\r\n" : ""));
                File.WriteAllText(Path.Combine(root, "ds2_infrastructure_one_unit.ini"), "; personal mine note\r\n[InfrastructureOneUnit]\r\nEnabled=1\r\nRemainingUnits=5\r\n"
                    + (explicitOptions ? "MineProductionCrystalCost=7\r\n" : ""));
                File.WriteAllText(Path.Combine(root, "ds2_crafting_unlocks.ini"), "; personal crafting note\r\n[CraftingUnlocks]\r\nEnabled=1\r\nFreeCrafting=0\r\n[Custom]\r\nKeep=1\r\n"
                    + (explicitOptions ? "[PCCStructures]\r\nUnlockAll=0\r\n" : ""));
                var capacity = fields.Single(f => f.ModId == "pickup-cargo-capacity" && f.Key == "CapacityUnits");
                if (storedProfile)
                {
                    var legacy = ModConfigurationService.LoadEffectiveProfile(catalog, null);
                    var ids = new HashSet<string>(added.Select(f => f.Id));
                    legacy.Values.RemoveAll(v => ids.Contains(v.Id));
                    Assert(legacy.Values.Count == 377 - ExcludedApasSettings, "v1.11 fixture field count");
                    ModConfigurationService.SetValue(legacy, capacity.Id, "352");
                    JsonStore.Write(ModConfigurationService.ProfilePath, legacy);
                }
                try
                {
                    var profile = storedProfile ? ModConfigurationService.LoadStoredProfile(catalog, root)
                        : ModConfigurationService.LoadEffectiveProfile(catalog, root);
                    Assert(profile.Values.Count == ExpectedSettingsCount, "profile upgrade coverage");
                    Assert(ModConfigurationService.GetValue(profile, capacity.Id) == (storedProfile ? "352" : "400"), "existing value precedence changed");
                    foreach (var f in added)
                    {
                        string value = expected[f.Section + "|" + f.Key];
                        if (explicitOptions)
                        {
                            if (f.Key == "HideFootprints" || (f.Key == "Enabled" && f.ModId == "sam-overhaul")) value = "1";
                            if (f.Section == "GeneratorRange" && f.Key == "RangePercent") value = "300";
                            if (f.Key == "RepairRadiusPercent") value = "310";
                            if (f.Key == "RearIndicatorScaling" || f.Section == "PCCStructures") value = "0";
                            if (f.Key == "MineProductionCrystalCost") value = "7";
                        }
                        Assert(ModConfigurationService.GetValue(profile, f.Id) == value, "new option import/default mismatch: " + f.Id);
                    }
                    var engine = new InstallEngine(catalog, true);
                    var plan = new ApplyPlan { GamePath = root, SelectedModIds = new List<string> {
                        "sam-overhaul", "pickup-cargo-capacity", "infrastructure-one-unit", "crafting-unlocks" }, ConfigurationProfile = profile };
                    var result = engine.Apply(plan, new DirectProgress());
                    Assert(result.Success, "v1.12 installation: " + result.Message);
                    var inis = Directory.GetFiles(root, "*.ini").ToDictionary(x => x, File.ReadAllBytes);
                    Assert(inis.Count == 4 && inis.Keys.All(x => File.ReadAllText(x).Contains("; personal")), "custom comments lost");
                    result = engine.Apply(plan, new DirectProgress());
                    Assert(result.Success && result.ConfigurationsUpdated == 0 && inis.All(x => x.Value.SequenceEqual(File.ReadAllBytes(x.Key))), "v1.12 apply not idempotent");
                    // No central profile: the installer must merge all newly added keys too.
                    if (!storedProfile && !explicitOptions)
                    {
                        foreach (var f in added)
                        {
                            var path = Path.Combine(root, f.Target);
                            var lines = File.ReadAllLines(path).Where(line => !line.TrimStart().StartsWith(f.Key + "=", StringComparison.OrdinalIgnoreCase));
                            File.WriteAllLines(path, lines);
                        }
                        plan.ConfigurationProfile = null;
                        Assert(engine.Apply(plan, new DirectProgress()).Success, "profile-free INI migration failed");
                        var migrated = ModConfigurationService.LoadEffectiveProfile(catalog, root);
                        Assert(added.All(f => ModConfigurationService.GetValue(migrated, f.Id) == f.DefaultValue), "profile-free defaults mismatch");
                        inis = Directory.GetFiles(root, "*.ini").ToDictionary(x => x, File.ReadAllBytes);
                    }
                    plan.SelectedModIds.Clear();
                    Assert(engine.Apply(plan, new DirectProgress()).Success && Directory.GetFiles(root, "*.asi").Length == 0
                        && inis.All(x => x.Value.SequenceEqual(File.ReadAllBytes(x.Key))), "v1.12 removal changed customized INIs");
                }
                finally { if (File.Exists(ModConfigurationService.ProfilePath)) File.Delete(ModConfigurationService.ProfilePath); }
            }
            report.AppendLine("PASS v1.12 defaults, range bounds, v1.11 profile migration, standalone opt-outs, missing keys, idempotence and custom INI retention");
        }
    }
}
