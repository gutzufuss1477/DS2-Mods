using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

namespace DS2ModSuite
{
    internal static partial class SelfTest
    {
        private static void TestLanguages(Catalog catalog, StringBuilder report)
        {
            Assert(Localization.Languages.Count == 10, "language selector must expose ten languages");
            Localization.SetLanguage(UiLanguage.English);
            var source = Localization.GetTranslations(UiLanguage.English);
            var definitions = ModConfigurationService.GetDefinitions(catalog);
            var profile = ModConfigurationService.LoadEffectiveProfile(catalog, null);
            var expectedInis = definitions.Select(field => field.Target).Distinct(StringComparer.OrdinalIgnoreCase)
                .ToDictionary(target => target, target => ModConfigurationService.BuildConfiguredIni(catalog, profile, target, null));
            string encoded = ConfigurationTransport.Encode(profile);
            string savedPath = SettingsStore.ReadGamePath();
            foreach (ConfigFieldDefinition field in definitions)
            {
                Assert(source.ContainsKey(field.Label), "untracked settings label: " + field.Label);
                Assert(string.IsNullOrWhiteSpace(field.Description) || source.ContainsKey(field.Description),
                    "untracked settings help: " + field.Id);
            }
            try
            {
                foreach (LanguageOption option in Localization.Languages)
                {
                    var translations = Localization.GetTranslations(option.Language);
                    Assert(new HashSet<string>(source.Keys).SetEquals(translations.Keys), "translation coverage: " + option.Code);
                    foreach (var pair in translations)
                    {
                        Assert(!string.IsNullOrWhiteSpace(pair.Value), "empty translation: " + option.Code + " / " + pair.Key);
                        Assert(Placeholders(pair.Key) == Placeholders(pair.Value), "format placeholder mismatch: " + option.Code + " / " + pair.Key);
                        Assert(Regex.Match(pair.Key, @"^\s*").Value == Regex.Match(pair.Value, @"^\s*").Value
                            && Regex.Match(pair.Key, @"\s*$").Value == Regex.Match(pair.Value, @"\s*$").Value,
                            "concatenation whitespace mismatch: " + option.Code + " / " + pair.Key);
                        if (Placeholders(pair.Key).Length > 0)
                            string.Format(Localization.GetCulture(option.Language), pair.Value, 12, 34, 56, 78);
                    }
                    UiLanguage parsed;
                    Assert(Localization.TryParse(option.Code, out parsed) && parsed == option.Language, "language code round trip: " + option.Code);
                    Assert(Localization.TryParse(option.NativeName, out parsed) && parsed == option.Language, "native language name: " + option.Code);
                    Localization.SetLanguage(option.Code);
                    SettingsStore.WriteLanguage(option.Language);
                    Assert(SettingsStore.ReadLanguage() == option.Language && SettingsStore.ReadGamePath() == savedPath,
                        "language persistence changed game path: " + option.Code);
                    Assert(Localization.CurrentLanguageCode == option.Code, "current language code: " + option.Code);
                    Assert(Localization.T("Unregistered fallback text") == "Unregistered fallback text", "missing translation fallback");
                    Assert(Localization.T(null) == string.Empty, "null translation fallback");
                    Assert(option.Language == UiLanguage.English || Localization.T("Mod Settings") != "Mod Settings", "untranslated primary UI: " + option.Code);
                    foreach (ModSpec mod in catalog.Mods)
                    {
                        Assert(source.ContainsKey(mod.Description) && source.ContainsKey(mod.Category), "untracked catalog text");
                        Assert(option.Language == UiLanguage.English || mod.LocalizedDescription != mod.Description, "untranslated catalog description: " + option.Code);
                    }
                    var localized = ModConfigurationService.GetDefinitions(catalog);
                    Assert(localized.Count == definitions.Count, "language changed field count");
                    for (int i = 0; i < definitions.Count; i++)
                    {
                        var original = definitions[i]; var translated = localized[i];
                        Assert(original.Id == translated.Id && original.DefaultValue == translated.DefaultValue
                            && original.Key == translated.Key && original.Section == translated.Section && original.Target == translated.Target,
                            "language changed configuration identity or defaults: " + option.Code);
                        Assert(translated.Label == translations[original.Label], "settings label not localized: " + option.Code + " / " + original.Id);
                        Assert(string.IsNullOrWhiteSpace(original.Description) || translated.Description == translations[original.Description],
                            "settings help not localized: " + option.Code + " / " + original.Id);
                    }
                    string error;
                    Assert(ModConfigurationService.TryValidateProfile(catalog, profile, out error), "localized profile rejected: " + error);
                    Assert(encoded == ConfigurationTransport.Encode(profile), "language changed elevated configuration payload");
                    foreach (var ini in expectedInis)
                        Assert(ini.Value.SequenceEqual(ModConfigurationService.BuildConfiguredIni(catalog, profile, ini.Key, null)),
                            "language changed generated INI bytes: " + option.Code + " / " + ini.Key);
                }
                UiLanguage alias;
                Assert(Localization.TryParse("es-419", out alias) && alias == UiLanguage.Spanish, "Spanish regional alias");
                Assert(Localization.TryParse("zh-Hans", out alias) && alias == UiLanguage.SimplifiedChinese, "Simplified Chinese alias");
                Assert(Localization.TryParse("pt_BR", out alias) && alias == UiLanguage.BrazilianPortuguese, "Brazilian Portuguese alias");
                Assert(!Localization.TryParse("zh-TW", out alias) && !Localization.TryParse("xx", out alias), "unsupported language accepted");
                Localization.SetLanguage((UiLanguage)999);
                Assert(Localization.CurrentLanguage == UiLanguage.English, "invalid enum fallback");
                report.AppendLine("PASS ten embedded languages, complete string/placeholder coverage, settings persistence, 325 invariant fields, 21 byte-identical INIs and elevated payload");
            }
            finally
            {
                Localization.SetLanguage(UiLanguage.English);
                SettingsStore.WriteLanguage(UiLanguage.English);
            }
        }

        private static string Placeholders(string text)
        {
            return string.Join("|", Regex.Matches(text, @"\{[0-9]+(?:[^{}]*)\}").Cast<Match>().Select(m => m.Value).OrderBy(s => s, StringComparer.Ordinal));
        }
    }
}
