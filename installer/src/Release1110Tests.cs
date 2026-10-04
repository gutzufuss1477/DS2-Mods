using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace DS2ModSuite
{
    internal static partial class SelfTest
    {
        private static bool IsRelease1110Field(ConfigFieldDefinition f)
        {
            return f.ModId == "sam-overhaul" || f.ModId == "jump-ramp-unlimited" || f.ModId == "beach-jump-with-cargo"
                || (f.ModId == "crafting-unlocks" && (f.Section == "AtlasEquipment" || f.Section == "SuppressedWeapons"))
                || (f.ModId == "climbing-power-gloves-range" && f.Section == "CombatDebug");
        }

        private static void TestRelease1110(Catalog catalog, string testRoot, string executable)
        {
            var definitions = ModConfigurationService.GetDefinitions(catalog);
            Assert(definitions.Count(IsRelease1110Field) == 52, "v1.11 field delta mismatch");
            Assert(!catalog.Mods.Any(m => m.Id == "sneaky-sam"), "Sneaky Sam must be replaced");
            var sam = catalog.Mods.Single(m => m.Id == "sam-overhaul");
            Assert(sam.ObsoleteFiles.Count == 2 && sam.ObsoleteFiles.All(f => f.Target.StartsWith("DS2_SneakySam_")), "Sneaky Sam migration paths missing");
            var root = Path.Combine(testRoot, "release1110");
            Directory.CreateDirectory(root);
            File.Copy(executable, Path.Combine(root, catalog.Game.Executable));
            var oldProfile = ModConfigurationService.LoadEffectiveProfile(catalog, null);
            var newIds = new HashSet<string>(definitions.Where(IsRelease1110Field).Select(f => f.Id));
            oldProfile.Values.RemoveAll(v => newIds.Contains(v.Id));
            Assert(oldProfile.Values.Count == 325, "v1.10 profile fixture mismatch");
            var global = definitions.Single(f => f.ModId == "crafting-unlocks" && f.Section == "CraftingUnlocks" && f.Key == "Enabled");
            ModConfigurationService.SetValue(oldProfile, global.Id, "0");
            string craftingPath = Path.Combine(root, "ds2_crafting_unlocks.ini");
            File.WriteAllText(craftingPath, "; user note\r\n[CraftingUnlocks]\r\nEnabled=1\r\n[AtlasEquipment]\r\nEnabled=0\r\nGoldSkeletonSkin=0\r\n[SuppressedWeapons]\r\nEnabled=0\r\nLanguage=2\r\n[Custom]\r\nKeep=1\r\n");
            JsonStore.Write(ModConfigurationService.ProfilePath, oldProfile);
            ModConfigurationProfile upgraded;
            try
            {
                upgraded = ModConfigurationService.LoadStoredProfile(catalog, root);
                Assert(upgraded.Values.Count == 377 && ModConfigurationService.GetValue(upgraded, global.Id) == "0", "saved central choice lost");
                foreach (string key in new[] { "Enabled", "GoldSkeletonSkin" })
                {
                    var field = definitions.Single(f => f.ModId == "crafting-unlocks" && f.Section == "AtlasEquipment" && f.Key == key);
                    Assert(ModConfigurationService.GetValue(upgraded, field.Id) == "0", "ATLAS opt-out overwritten");
                }
                var language = definitions.Single(f => f.Section == "SuppressedWeapons" && f.Key == "Language");
                Assert(ModConfigurationService.GetValue(upgraded, language.Id) == "2", "standalone weapon language lost");
                string error;
                foreach (string value in new[] { "0", "1", "2" })
                {
                    ModConfigurationService.SetValue(upgraded, language.Id, value);
                    Assert(ModConfigurationService.TryValidateProfile(catalog, upgraded, out error), "valid weapon language rejected");
                }
                ModConfigurationService.SetValue(upgraded, language.Id, "3");
                Assert(!ModConfigurationService.TryValidateProfile(catalog, upgraded, out error), "invalid weapon language accepted");
                ModConfigurationService.SetValue(upgraded, language.Id, "2");
                foreach (var field in definitions.Where(f => f.ModId == "sam-overhaul" && f.Schema.Min.HasValue))
                {
                    string original = ModConfigurationService.GetValue(upgraded, field.Id);
                    ModConfigurationService.SetValue(upgraded, field.Id, (field.Schema.Min.Value - 1).ToString(System.Globalization.CultureInfo.InvariantCulture));
                    Assert(!ModConfigurationService.TryValidateProfile(catalog, upgraded, out error), "Sam lower bound ignored: " + field.Key);
                    ModConfigurationService.SetValue(upgraded, field.Id, (field.Schema.Max.Value + 1).ToString(System.Globalization.CultureInfo.InvariantCulture));
                    Assert(!ModConfigurationService.TryValidateProfile(catalog, upgraded, out error), "Sam upper bound ignored: " + field.Key);
                    ModConfigurationService.SetValue(upgraded, field.Id, original);
                }
            }
            finally { File.Delete(ModConfigurationService.ProfilePath); }

            var ids = new List<string> { "sam-overhaul", "crafting-unlocks", "climbing-power-gloves-range", "jump-ramp-unlimited", "beach-jump-with-cargo" };
            var plan = new ApplyPlan { GamePath = root, SelectedModIds = ids, ConfigurationProfile = upgraded };
            var engine = new InstallEngine(catalog, true);
            foreach (var mod in catalog.Mods.Where(m => m.ConflictingFiles != null))
            foreach (string conflict in mod.ConflictingFiles)
            {
                string path = Path.Combine(root, conflict);
                File.WriteAllText(path, "user standalone binary");
                var state = GameInspector.InspectMods(GameInspector.Inspect(catalog, root, false), catalog).Single(s => s.Spec.Id == mod.Id);
                Assert(state.HasUnknownObsoleteBinary && state.ConflictFileNames.Contains(conflict), "standalone conflict not displayed");
                Assert(!engine.Apply(plan, new DirectProgress()).Success && File.ReadAllText(path) == "user standalone binary"
                    && !File.Exists(Path.Combine(root, catalog.Loader.FileName)), "standalone conflict not blocked before writes");
                File.Delete(path);
            }
            foreach (var old in sam.ObsoleteFiles)
            {
                string path = Path.Combine(root, old.Target);
                string hash = old.Sha256;
                try
                {
                    File.WriteAllText(path, "synthetic trusted Sneaky Sam");
                    old.Sha256 = HashUtil.FileSha256(path);
                    var state = GameInspector.InspectMods(GameInspector.Inspect(catalog, root, false), catalog).Single(s => s.Spec.Id == sam.Id);
                    Assert(state.HasObsoleteBinary && state.DesiredEnabled, "Sneaky Sam not selected for upgrade");
                    var result = engine.Apply(plan, new DirectProgress());
                    Assert(result.Success && !File.Exists(path) && File.Exists(Path.Combine(root, sam.Files[0].Target)), "Sam replacement failed: " + result.Message);
                    Assert(Directory.GetFiles(root, "*.asi").Length == 5, "duplicate ASI after Sam migration");
                    File.WriteAllText(path, "unknown Sneaky Sam");
                    byte[] before = File.ReadAllBytes(craftingPath);
                    Assert(!engine.Apply(plan, new DirectProgress()).Success && File.ReadAllText(path) == "unknown Sneaky Sam"
                        && before.SequenceEqual(File.ReadAllBytes(craftingPath)), "unknown legacy file changed during refused upgrade");
                    File.Delete(path);
                }
                finally { old.Sha256 = hash; }
            }
            var iniFiles = Directory.GetFiles(root, "*.ini").ToDictionary(p => p, File.ReadAllBytes);
            Assert(File.ReadAllText(craftingPath).Contains("; user note") && File.ReadAllText(craftingPath).Contains("Keep=1"), "comments/custom settings lost");
            var repeated = engine.Apply(plan, new DirectProgress());
            Assert(repeated.Success && repeated.ConfigurationsUpdated == 0 && iniFiles.All(f => f.Value.SequenceEqual(File.ReadAllBytes(f.Key))), "new settings apply not idempotent");
            foreach (string path in iniFiles.Keys.ToList()) File.AppendAllText(path, "\r\n; retained personal note\r\n");
            iniFiles = iniFiles.Keys.ToDictionary(p => p, File.ReadAllBytes);
            plan.SelectedModIds = new List<string>();
            Assert(engine.Apply(plan, new DirectProgress()).Success && Directory.GetFiles(root, "*.asi").Length == 0
                && iniFiles.All(f => f.Value.SequenceEqual(File.ReadAllBytes(f.Key))), "new mods removal lost user INIs");

            // Adopt incomplete standalone INIs without any central profile, preserving explicit opt-outs.
            File.WriteAllText(Path.Combine(root, "ds2_sam_overhaul.ini"), "; custom\r\n[CargoVisibility]\r\nHideBackpackCargo=0\r\n[AutoDrive]\r\nActivationSeconds=4.5\r\n");
            File.WriteAllText(Path.Combine(root, "ds2_jump_ramp_unlimited.ini"), "[JumpRampUnlimited]\r\nEnabled=0\r\n");
            File.WriteAllText(Path.Combine(root, "ds2_beach_jump_with_cargo.ini"), "[BeachJumpWithCargo]\r\nEnabled=0\r\n");
            plan.SelectedModIds = ids;
            plan.ConfigurationProfile = null;
            Assert(engine.Apply(plan, new DirectProgress()).Success, "no-profile adoption failed");
            string samIni = File.ReadAllText(Path.Combine(root, "ds2_sam_overhaul.ini"));
            Assert(samIni.Contains("HideBackpackCargo=0") && samIni.Contains("ActivationSeconds=4.5") && samIni.Contains("ChargeSeconds=0.75")
                && File.ReadAllText(Path.Combine(root, "ds2_jump_ramp_unlimited.ini")).Contains("Enabled=0")
                && File.ReadAllText(Path.Combine(root, "ds2_beach_jump_with_cargo.ini")).Contains("Enabled=0"), "standalone values/defaults lost");
        }
    }
}
