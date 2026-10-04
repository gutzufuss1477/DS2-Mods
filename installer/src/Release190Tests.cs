using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace DS2ModSuite
{
    internal static partial class SelfTest
    {
        private static void TestRelease190Settings(Catalog catalog, string testRoot, string executable)
        {
            const string modId = "climbing-power-gloves-range";
            const string target = "ds2_climbing_gloves_range.ini";
            Assert(catalog.Mods.Single(m => m.Id == modId).CategoryDe == "Ausrüstung & Fortschritt",
                "German catalog UTF-8 text was corrupted");
            List<ConfigFieldDefinition> definitions = ModConfigurationService.GetDefinitions(catalog);
            List<ConfigFieldDefinition> gloves = definitions.Where(f => f.ModId == modId).ToList();
            ConfigFieldDefinition level1 = gloves.Single(f => f.Key == "Level1RangeMeters");
            ConfigFieldDefinition level2 = gloves.Single(f => f.Key == "Level2RangeMeters");
            ConfigFieldDefinition combat = gloves.Single(f => f.Key == "EnableCargoPickup");
            Assert(gloves.Count == 10 && level1.DefaultValue == "30" && level2.DefaultValue == "50"
                && combat.DefaultValue == "1" && !combat.Schema.Advanced, "glove release defaults mismatch");
            List<ConfigFieldDefinition> items = definitions.Where(f => f.ModId == "crafting-unlocks" && f.Section == "Items").ToList();
            List<ConfigFieldDefinition> added = items.Where(f => f.Label == "Chiral Boots" || f.Schema.Group == "Enemy-Drop Weapons").ToList();
            Assert(items.Count == 132 && added.Count == 12 && added.Any(f => f.Label == "Ghost Blade")
                && items.All(f => f.DefaultValue == "1" && f.Schema.Advanced && !string.IsNullOrWhiteSpace(f.Schema.Group))
                && items.Select(f => f.Schema.Group).Distinct().Count() == 13, "Crafting 1.5 item/group coverage mismatch");
            ConfigFieldDefinition chiral = added.Single(f => f.Label == "Chiral Boots");
            ConfigFieldDefinition ghost = added.Single(f => f.Label == "Ghost Blade");

            ModConfigurationProfile defaults = ModConfigurationService.LoadEffectiveProfile(catalog, null);
            string error;
            foreach (string invalid in new[] { "7.9", "100.1" })
            {
                ModConfigurationService.SetValue(defaults, level1.Id, invalid);
                Assert(!ModConfigurationService.TryValidateProfile(catalog, defaults, out error), "invalid glove Level 1 range accepted");
            }
            ModConfigurationService.SetValue(defaults, level1.Id, "70.5");
            ModConfigurationService.SetValue(defaults, level2.Id, "50");
            Assert(!ModConfigurationService.TryValidateProfile(catalog, defaults, out error), "inverted glove ranges accepted");
            ModConfigurationService.SetValue(defaults, level2.Id, "70.5");
            Assert(ModConfigurationService.TryValidateProfile(catalog, defaults, out error), "valid equal decimal glove ranges rejected");
            ModConfigurationService.SetValue(defaults, level1.Id, "30");
            foreach (string invalid in new[] { "9.9", "100.1" })
            {
                ModConfigurationService.SetValue(defaults, level2.Id, invalid);
                Assert(!ModConfigurationService.TryValidateProfile(catalog, defaults, out error), "invalid glove Level 2 range accepted");
            }
            ModConfigurationService.SetValue(defaults, level2.Id, "50");
            ModConfigurationService.SetValue(defaults, ghost.Id, "inherit");
            Assert(ModConfigurationService.TryValidateProfile(catalog, defaults, out error), "Crafting inherit choice rejected");
            ModConfigurationService.SetValue(defaults, ghost.Id, "invalid");
            Assert(!ModConfigurationService.TryValidateProfile(catalog, defaults, out error), "invalid Crafting choice accepted");

            foreach (bool configured in new[] { false, true })
            foreach (bool explicitOptOut in new[] { false, true })
            {
                string gameRoot = Path.Combine(testRoot, "glove-upgrade-" + configured + "-" + explicitOptOut);
                Directory.CreateDirectory(gameRoot);
                File.Copy(executable, Path.Combine(gameRoot, catalog.Game.Executable));
                string ini = Path.Combine(gameRoot, target);
                File.WriteAllText(ini, "; personal glove note\r\n[ClimbingGlovesRange]\r\nEnabled=0\r\n"
                    + "Level1RangeMeters=35.5\r\nLevel2RangeMeters=65.5\r\nDebugLog=0\r\n"
                    + (explicitOptOut ? "[CombatGloves]\r\nEnableCargoPickup=0\r\n" : "") + "[Personal]\r\nKeep=1\r\n");
                ApplyPlan plan = new ApplyPlan { GamePath = gameRoot, SelectedModIds = new List<string> { modId },
                    ConfigurationProfile = configured ? ModConfigurationService.LoadEffectiveProfile(catalog, gameRoot) : null };
                InstallEngine engine = new InstallEngine(catalog, true);
                ApplyResult result = engine.Apply(plan, new DirectProgress());
                string written = File.ReadAllText(ini);
                Assert(result.Success && written.Contains("Enabled=0") && written.Contains("Level1RangeMeters=35.5")
                    && written.Contains("Level2RangeMeters=65.5") && written.Contains("EnableCargoPickup=" + (explicitOptOut ? "0" : "1"))
                    && written.Contains("; personal glove note") && written.Contains("Keep=1"), "glove settings lost in migration: " + result.Message);
                byte[] bytes = File.ReadAllBytes(ini);
                result = engine.Apply(plan, new DirectProgress());
                Assert(result.Success && result.ConfigurationsUpdated == 0 && bytes.SequenceEqual(File.ReadAllBytes(ini)), "glove migration not idempotent");
                if (configured)
                {
                    ModConfigurationService.SetValue(plan.ConfigurationProfile, combat.Id, explicitOptOut ? "1" : "0");
                    Assert(engine.Apply(plan, new DirectProgress()).Success
                        && File.ReadAllText(ini).Contains("EnableCargoPickup=" + (explicitOptOut ? "1" : "0")), "Combat switch failed");
                    bytes = File.ReadAllBytes(ini);
                }
                plan.SelectedModIds.Clear();
                Assert(engine.Apply(plan, new DirectProgress()).Success
                    && !File.Exists(Path.Combine(gameRoot, "ds2_climbing_gloves_range.asi"))
                    && bytes.SequenceEqual(File.ReadAllBytes(ini)), "glove removal lost user INI");
            }

            string migrationRoot = Path.Combine(testRoot, "profile-180-to-190");
            Directory.CreateDirectory(migrationRoot);
            File.Copy(executable, Path.Combine(migrationRoot, catalog.Game.Executable));
            File.WriteAllText(Path.Combine(migrationRoot, target), "[ClimbingGlovesRange]\r\nLevel1RangeMeters=80\r\nLevel2RangeMeters=90\r\n[CombatGloves]\r\nEnableCargoPickup=0\r\n");
            string craftingIni = Path.Combine(migrationRoot, "ds2_crafting_unlocks.ini");
            File.WriteAllText(craftingIni, "; preserve me\r\n[Items]\r\n" + chiral.Key + "=0 ; my boots\r\n" + ghost.Key + "=inherit ; follow default\r\n[Custom]\r\nKeep=1\r\n");
            ModConfigurationProfile legacy = ModConfigurationService.LoadEffectiveProfile(catalog, null);
            HashSet<string> addedIds = new HashSet<string>(added.Select(f => f.Id)) { combat.Id };
            addedIds.UnionWith(definitions.Where(IsRelease1110Field).Select(f => f.Id));
            legacy.Values.RemoveAll(v => addedIds.Contains(v.Id));
            Assert(legacy.Values.Count == 312, "v1.8 profile fixture must have 312 fields");
            ModConfigurationService.SetValue(legacy, level1.Id, "40");
            ModConfigurationService.SetValue(legacy, level2.Id, "75");
            JsonStore.Write(ModConfigurationService.ProfilePath, legacy);
            try
            {
                foreach (ModConfigurationProfile upgraded in new[] {
                    ModConfigurationService.LoadStoredProfile(catalog, migrationRoot),
                    ModConfigurationService.LoadEffectiveProfile(catalog, migrationRoot) })
                {
                    Assert(upgraded.Values.Count == 377 && ModConfigurationService.TryValidateProfile(catalog, upgraded, out error)
                        && ModConfigurationService.GetValue(upgraded, level1.Id) == "40"
                        && ModConfigurationService.GetValue(upgraded, level2.Id) == "75"
                        && ModConfigurationService.GetValue(upgraded, combat.Id) == "0"
                        && ModConfigurationService.GetValue(upgraded, chiral.Id) == "0"
                        && ModConfigurationService.GetValue(upgraded, ghost.Id) == "inherit", "profile migration overwrote saved/new standalone settings");
                    string generated = Encoding.UTF8.GetString(ModConfigurationService.BuildConfiguredIni(catalog, upgraded, "ds2_crafting_unlocks.ini", craftingIni));
                    Assert(generated.Contains(chiral.Key + "=0  ; my boots") && generated.Contains(ghost.Key + "=inherit  ; follow default")
                        && generated.Contains("Keep=1") && generated.Contains("; preserve me"), "new crafting choices/comments lost during apply");
                }
            }
            finally { File.Delete(ModConfigurationService.ProfilePath); }
            string stable = Encoding.UTF8.GetString(ModConfigurationService.BuildStableExistingIni(catalog, "crafting-unlocks", "ds2_crafting_unlocks.ini", craftingIni));
            Assert(stable.Contains(ghost.Key + "=inherit") && stable.Contains(chiral.Key + "=0"), "no-profile crafting migration lost explicit choices");

            string invalidRoot = Path.Combine(testRoot, "invalid-glove-rollback");
            Directory.CreateDirectory(invalidRoot);
            File.Copy(executable, Path.Combine(invalidRoot, catalog.Game.Executable));
            string invalidIni = Path.Combine(invalidRoot, target);
            File.WriteAllText(invalidIni, "[ClimbingGlovesRange]\r\nEnabled=1\r\nLevel1RangeMeters=90\r\nLevel2RangeMeters=50\r\nDebugLog=0\r\n[CombatGloves]\r\nEnableCargoPickup=0\r\n");
            byte[] invalidBytes = File.ReadAllBytes(invalidIni);
            ApplyResult refused = new InstallEngine(catalog, true).Apply(new ApplyPlan {
                GamePath = invalidRoot, SelectedModIds = new List<string> { modId } }, new DirectProgress());
            Assert(!refused.Success && invalidBytes.SequenceEqual(File.ReadAllBytes(invalidIni))
                && !File.Exists(Path.Combine(invalidRoot, "ds2_climbing_gloves_range.asi"))
                && !File.Exists(Path.Combine(invalidRoot, catalog.Loader.FileName)), "invalid glove ranges were applied or rollback failed");
            Localization.SetLanguage(UiLanguage.German);
            try
            {
                ConfigFieldDefinition translated = ModConfigurationService.GetDefinitions(catalog).Single(f => f.Id == combat.Id);
                Assert(translated.Label.Contains("Kampfhandschuh") && translated.Description.Contains("Hauptschalter"), "German glove guidance missing");
            }
            finally { Localization.SetLanguage(UiLanguage.English); }
        }

    }
}
