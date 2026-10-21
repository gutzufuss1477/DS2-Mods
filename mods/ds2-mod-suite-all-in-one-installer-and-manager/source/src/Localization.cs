using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Reflection;
using System.Runtime.Serialization.Json;

namespace DS2ModSuite
{
    internal enum UiLanguage
    {
        English, German, French, Spanish, Italian, SimplifiedChinese,
        Japanese, Korean, BrazilianPortuguese, Russian
    }

    internal sealed class LanguageOption
    {
        public UiLanguage Language { get; private set; }
        public string Code { get; private set; }
        public string NativeName { get; private set; }
        public string Culture { get; private set; }
        public string FontFamily { get; private set; }

        public LanguageOption(UiLanguage language, string code, string name, string culture, string font = "Segoe UI")
        {
            Language = language; Code = code; NativeName = name; Culture = culture; FontFamily = font;
        }
    }

    internal static class Localization
    {
        private static volatile int currentLanguage = (int)UiLanguage.English;
        public static readonly System.Collections.ObjectModel.ReadOnlyCollection<LanguageOption> Languages =
            Array.AsReadOnly(new[]
            {
                new LanguageOption(UiLanguage.English, "en", "English", "en-US"),
                new LanguageOption(UiLanguage.German, "de", "Deutsch", "de-DE"),
                new LanguageOption(UiLanguage.French, "fr", "Français", "fr-FR"),
                new LanguageOption(UiLanguage.Spanish, "es", "Español", "es-ES"),
                new LanguageOption(UiLanguage.Italian, "it", "Italiano", "it-IT"),
                new LanguageOption(UiLanguage.BrazilianPortuguese, "pt-BR", "Português (Brasil)", "pt-BR"),
                new LanguageOption(UiLanguage.Russian, "ru", "Русский", "ru-RU"),
                new LanguageOption(UiLanguage.SimplifiedChinese, "zh-CN", "简体中文", "zh-CN", "Microsoft YaHei UI"),
                new LanguageOption(UiLanguage.Japanese, "ja", "日本語", "ja-JP", "Yu Gothic UI"),
                new LanguageOption(UiLanguage.Korean, "ko", "한국어", "ko-KR", "Malgun Gothic")
            });
        private static readonly Lazy<Dictionary<string, Dictionary<string, string>>> resources =
            new Lazy<Dictionary<string, Dictionary<string, string>>>(LoadResources);

        public static UiLanguage CurrentLanguage { get { return (UiLanguage)currentLanguage; } }
        public static string CurrentLanguageCode { get { return ToCode(CurrentLanguage); } }
        public static string FontFamilyName { get { return Option(CurrentLanguage).FontFamily; } }
        public static void SetLanguage(UiLanguage language) { currentLanguage = (int)Option(language).Language; }
        public static void SetLanguage(string code) { SetLanguage(ParseOrDefault(code)); }
        public static UiLanguage ParseOrDefault(string code)
        {
            UiLanguage language;
            return TryParse(code, out language) ? language : UiLanguage.English;
        }

        public static bool TryParse(string code, out UiLanguage language)
        {
            string normalized = (code ?? string.Empty).Trim().Replace('_', '-');
            foreach (LanguageOption option in Languages)
            {
                bool matches = string.Equals(normalized, option.Code, StringComparison.OrdinalIgnoreCase)
                    || string.Equals(normalized, option.NativeName, StringComparison.OrdinalIgnoreCase)
                    || string.Equals(normalized, option.Language.ToString(), StringComparison.OrdinalIgnoreCase);
                // Regional aliases are accepted only for supported writing systems.
                if (option.Code != "zh-CN" && option.Code != "pt-BR")
                    matches |= normalized.StartsWith(option.Code + "-", StringComparison.OrdinalIgnoreCase);
                if (option.Code == "zh-CN")
                    matches |= string.Equals(normalized, "zh-Hans", StringComparison.OrdinalIgnoreCase)
                        || string.Equals(normalized, "zh-SG", StringComparison.OrdinalIgnoreCase);
                if (option.Code == "pt-BR") matches |= string.Equals(normalized, "pt", StringComparison.OrdinalIgnoreCase);
                if (matches) { language = option.Language; return true; }
            }
            language = UiLanguage.English;
            return false;
        }

        public static string ToCode(UiLanguage language) { return Option(language).Code; }
        public static CultureInfo GetCulture(UiLanguage language) { return CultureInfo.GetCultureInfo(Option(language).Culture); }

        public static string T(string english, string german = null)
        {
            string key = english ?? string.Empty;
            if (CurrentLanguage == UiLanguage.English || key.Length == 0) return key;
            string value;
            if (resources.Value[CurrentLanguageCode].TryGetValue(key, out value) && !string.IsNullOrWhiteSpace(value)) return value;
            return CurrentLanguage == UiLanguage.German && !string.IsNullOrWhiteSpace(german) ? german : key;
        }

        public static string Format(string english, string german, params object[] arguments)
        {
            return string.Format(GetCulture(CurrentLanguage), T(english, german), arguments ?? new object[0]);
        }

        internal static IDictionary<string, string> GetTranslations(UiLanguage language)
        {
            return new System.Collections.ObjectModel.ReadOnlyDictionary<string, string>(resources.Value[ToCode(language)]);
        }

        private static LanguageOption Option(UiLanguage language)
        {
            foreach (LanguageOption option in Languages) if (option.Language == language) return option;
            return Languages[0];
        }

        private static Dictionary<string, Dictionary<string, string>> LoadResources()
        {
            var result = new Dictionary<string, Dictionary<string, string>>(StringComparer.Ordinal);
            foreach (LanguageOption option in Languages)
            {
                string name = "DS2ModSuite.Locale." + option.Code;
                using (Stream stream = Assembly.GetExecutingAssembly().GetManifestResourceStream(name))
                {
                    if (stream == null) throw new InvalidDataException("Missing embedded language resource: " + name);
                    var serializer = new DataContractJsonSerializer(typeof(Dictionary<string, string>),
                        new DataContractJsonSerializerSettings { UseSimpleDictionaryFormat = true });
                    result.Add(option.Code, (Dictionary<string, string>)serializer.ReadObject(stream));
                }
            }
            return result;
        }
    }
}
