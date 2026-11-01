using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;

namespace DS2ModSuite
{
    internal static partial class SelfTest
    {
        private static bool IsRelease1130Field(ConfigFieldDefinition f)
        {
            return (f.ModId == "no-magellan-evaluation" && f.Key.StartsWith("Remove"))
                || (f.ModId == "crafting-unlocks" && (new[] { "WarriorWeapons", "RedGuitar", "Guitars", "Cosmetics" }.Contains(f.Section)
                || new[] { "0x22B02657", "0x22424DD4", "0x5029CED7" }.Contains(f.Key)));
        }

        private static void TestRelease1130(Catalog catalog, string testRoot, string executable, StringBuilder report)
        {
            var fields = ModConfigurationService.GetDefinitions(catalog);
            var added = fields.Where(IsRelease1130Field).ToList();
            Assert(added.Count == 15 && !fields.Any(f => f.Section == "CombatDebug"), "v1.13 schema delta");
            var profile = ModConfigurationService.LoadEffectiveProfile(catalog, null);
            string error;
            foreach (var field in added.Where(f => f.Schema.Min.HasValue))
            {
                foreach (decimal n in new[] { field.Schema.Min.Value, field.Schema.Max.Value })
                {
                    ModConfigurationService.SetValue(profile, field.Id, n.ToString(CultureInfo.InvariantCulture));
                    Assert(ModConfigurationService.TryValidateProfile(catalog, profile, out error), "v1.13 valid bound rejected");
                }
                foreach (decimal n in new[] { field.Schema.Min.Value - 1, field.Schema.Max.Value + 1 })
                {
                    ModConfigurationService.SetValue(profile, field.Id, n.ToString(CultureInfo.InvariantCulture));
                    Assert(!ModConfigurationService.TryValidateProfile(catalog, profile, out error), "v1.13 invalid bound accepted");
                }
                ModConfigurationService.SetValue(profile, field.Id, field.DefaultValue);
            }
            var reserve = added.Single(f => f.Key == "ReserveMode");
            foreach (string value in new[] { "0", "1", "2", "3", "inherit" })
            {
                ModConfigurationService.SetValue(profile, reserve.Id, value);
                Assert(ModConfigurationService.TryValidateProfile(catalog, profile, out error) == new[] { "0", "1", "2" }.Contains(value),
                    "Warrior reserve mode validation");
            }

            foreach (bool stored in new[] { false, true })
            {
                var root = Path.Combine(testRoot, "release1130-" + stored);
                Directory.CreateDirectory(root);
                File.Copy(executable, Path.Combine(root, catalog.Game.Executable));
                string crafting = Path.Combine(root, "ds2_crafting_unlocks.ini");
                string magellan = Path.Combine(root, "ds2_no_magellan_evaluation.ini");
                string metadata = Path.Combine(root, "ds2_story_coffin_variants.bin");
                File.WriteAllText(metadata, "personal coffin save metadata");
                File.WriteAllText(crafting, "; keep crafting note\r\n[CraftingUnlocks]\r\nEnabled=0\r\nDefaultUnlock=0\r\nFreeCrafting=0\r\n[WarriorWeapons]\r\nReserveMode=2\r\nRifleMagazine=42\r\n");
                File.WriteAllText(magellan, "; keep delivery choices\r\n[NoMagellanEvaluation]\r\nEnabled=1\r\nRemoveLikePenalty=0\r\n");
                try
                {
                    if (stored)
                    {
                        var legacy = ModConfigurationService.LoadEffectiveProfile(catalog, root);
                        var ids = new HashSet<string>(added.Select(f => f.Id));
                        legacy.Values.RemoveAll(v => ids.Contains(v.Id));
                        // Real v1.12 profiles include five retired glove diagnostics.
                        foreach (string key in new[] { "LiveDebug", "ScanGeneration", "FallbackAction", "RootSubcategoryFallback", "MapRightWriterToLeft" })
                            legacy.Values.Add(new ModConfigurationValue { Id = "climbing-power-gloves-range|ds2_climbing_gloves_range.ini|CombatDebug|" + key, Value = "1" });
                        Assert(legacy.Values.Count == 388 - ExcludedApasSettings, "v1.12 fixture size");
                        JsonStore.Write(ModConfigurationService.ProfilePath, legacy);
                    }
                    var upgraded = stored ? ModConfigurationService.LoadStoredProfile(catalog, root) : null;
                    if (stored)
                        Assert(upgraded.Values.Count == ExpectedSettingsCount && ModConfigurationService.TryValidateProfile(catalog, upgraded, out error),
                            "retired/new profile fields not reconciled");
                    var plan = new ApplyPlan { GamePath = root, SelectedModIds = new List<string> { "crafting-unlocks", "no-magellan-evaluation" }, ConfigurationProfile = upgraded };
                    var engine = new InstallEngine(catalog, true);
                    var result = engine.Apply(plan, new DirectProgress());
                    string ini = File.ReadAllText(crafting);
                    Assert(result.Success && ini.Contains("Enabled=0") && ini.Contains("DefaultUnlock=0")
                        && ini.Contains("RifleMagazine=42") && ini.Contains("ReserveMode=2") && ini.Contains("; keep crafting note"),
                        "v1.13 migration lost settings: " + result.Message);
                    foreach (var f in added.Where(f => f.Section == "Items"))
                        Assert(ini.Contains(f.Key + "=inherit"), "new weapon bypasses DefaultUnlock opt-out");
                    Assert(File.ReadAllText(magellan).Contains("RemoveLikePenalty=0") && File.ReadAllText(magellan).Contains("RemoveMagellanRank=1"),
                        "Magellan migration lost independent switches");
                    var before = Directory.GetFiles(root, "*.ini").ToDictionary(p => p, File.ReadAllBytes);
                    result = engine.Apply(plan, new DirectProgress());
                    Assert(result.Success && result.ConfigurationsUpdated == 0 && before.All(p => p.Value.SequenceEqual(File.ReadAllBytes(p.Key))),
                        "v1.13 INI migration not idempotent");
                    plan.SelectedModIds.Clear();
                    Assert(engine.Apply(plan, new DirectProgress()).Success && File.ReadAllText(metadata) == "personal coffin save metadata"
                        && before.All(p => p.Value.SequenceEqual(File.ReadAllBytes(p.Key))), "removal lost settings or coffin metadata");
                }
                finally { File.Delete(ModConfigurationService.ProfilePath); }
            }

            var states = catalog.Mods.Select(m => new ModRuntimeState { Spec = m }).ToList();
            var main = states.Single(s => s.Spec.Id == "crafting-unlocks");
            var addons = states.Where(s => s.Spec.Requires != null && s.Spec.Requires.Contains(main.Spec.Id)).ToList();
            Assert(addons.Count == 2, "two ATLAS dependencies required");
            foreach (var addon in addons)
            {
                ModDependencies.SetSelection(states, addon, true);
                Assert(main.DesiredEnabled && addon.DesiredEnabled, "addon must select Crafting");
            }
            ModDependencies.SetSelection(states, main, false);
            Assert(addons.All(s => !s.DesiredEnabled), "removing Crafting must deselect both addons");
            addons[0].DesiredEnabled = true;
            ModDependencies.IncludeRequired(states);
            Assert(main.DesiredEnabled, "installed addon recovery did not select missing base");
            var previous = main.Spec.Requires;
            try
            {
                main.Spec.Requires = new List<string> { addons[0].Spec.Id };
                bool rejected = false;
                try { ModDependencies.ValidateCatalog(catalog); } catch (InvalidDataException) { rejected = true; }
                Assert(rejected, "dependency cycle accepted");
                main.Spec.Requires = new List<string> { "missing-base" };
                rejected = false;
                try { ModDependencies.ValidateCatalog(catalog); } catch (InvalidDataException) { rejected = true; }
                Assert(rejected, "unknown dependency accepted");
            }
            finally { main.Spec.Requires = previous; }
            var addonRoot = Path.Combine(testRoot, "atlas-dependencies");
            Directory.CreateDirectory(addonRoot);
            File.Copy(executable, Path.Combine(addonRoot, catalog.Game.Executable));
            var addonEngine = new InstallEngine(catalog, true);
            foreach (var addon in addons)
            {
                var plan = new ApplyPlan { GamePath = addonRoot, SelectedModIds = new List<string> { addon.Spec.Id } };
                var unchanged = Directory.GetFiles(addonRoot).ToDictionary(p => p, File.ReadAllBytes);
                var result = addonEngine.Apply(plan, new DirectProgress());
                Assert(!result.Success && Directory.GetFiles(addonRoot, "*.asi").Length == 0
                    && Directory.GetFiles(addonRoot).Length == unchanged.Count
                    && unchanged.All(p => p.Value.SequenceEqual(File.ReadAllBytes(p.Key))), "invalid dependency plan wrote payloads");
                plan.SelectedModIds.Add(main.Spec.Id);
                Assert(addonEngine.Apply(plan, new DirectProgress()).Success, "addon plus base install failed");
                plan.SelectedModIds.Remove(main.Spec.Id);
                Assert(!addonEngine.Apply(plan, new DirectProgress()).Success
                    && File.Exists(Path.Combine(addonRoot, main.Spec.Files[0].Target)), "base removed while addon remained selected");
                plan.SelectedModIds.Clear();
                Assert(addonEngine.Apply(plan, new DirectProgress()).Success && Directory.GetFiles(addonRoot, "*.asi").Length == 0,
                    "addon/base removal failed");
            }
            report.AppendLine("PASS v1.13 bounds, legacy/no-profile INI migration, retired diagnostics, opt-outs, metadata preservation, dependency selection and invalid-plan guards");
        }
    }
}

