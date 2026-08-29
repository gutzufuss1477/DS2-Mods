using System;
using System.Globalization;

namespace DS2ModSuite
{
    internal enum UiLanguage
    {
        English,
        German
    }

    internal static class Localization
    {
        private static volatile int currentLanguage = (int)UiLanguage.English;

        public static UiLanguage CurrentLanguage
        {
            get { return (UiLanguage)currentLanguage; }
        }

        public static string CurrentLanguageCode
        {
            get { return ToCode(CurrentLanguage); }
        }

        public static void SetLanguage(UiLanguage language)
        {
            currentLanguage = (int)Normalize(language);
        }

        public static void SetLanguage(string languageCode)
        {
            SetLanguage(ParseOrDefault(languageCode));
        }

        public static UiLanguage ParseOrDefault(string languageCode)
        {
            UiLanguage language;
            return TryParse(languageCode, out language) ? language : UiLanguage.English;
        }

        public static bool TryParse(string languageCode, out UiLanguage language)
        {
            string normalized = (languageCode ?? string.Empty).Trim();
            if (string.Equals(normalized, "de", StringComparison.OrdinalIgnoreCase)
                || string.Equals(normalized, "de-DE", StringComparison.OrdinalIgnoreCase)
                || string.Equals(normalized, "German", StringComparison.OrdinalIgnoreCase)
                || string.Equals(normalized, "Deutsch", StringComparison.OrdinalIgnoreCase))
            {
                language = UiLanguage.German;
                return true;
            }

            if (string.Equals(normalized, "en", StringComparison.OrdinalIgnoreCase)
                || string.Equals(normalized, "en-US", StringComparison.OrdinalIgnoreCase)
                || string.Equals(normalized, "en-GB", StringComparison.OrdinalIgnoreCase)
                || string.Equals(normalized, "English", StringComparison.OrdinalIgnoreCase))
            {
                language = UiLanguage.English;
                return true;
            }

            language = UiLanguage.English;
            return false;
        }

        public static string ToCode(UiLanguage language)
        {
            return Normalize(language) == UiLanguage.German ? "de" : "en";
        }

        public static string T(string english, string german)
        {
            string fallback = english ?? string.Empty;
            if (CurrentLanguage != UiLanguage.German || string.IsNullOrWhiteSpace(german))
            {
                return fallback;
            }

            return german;
        }

        public static string Format(string english, string german, params object[] arguments)
        {
            string format = T(english, german);
            return string.Format(GetCulture(CurrentLanguage), format, arguments ?? new object[0]);
        }

        public static CultureInfo GetCulture(UiLanguage language)
        {
            return CultureInfo.GetCultureInfo(Normalize(language) == UiLanguage.German ? "de-DE" : "en-US");
        }

        private static UiLanguage Normalize(UiLanguage language)
        {
            return language == UiLanguage.German ? UiLanguage.German : UiLanguage.English;
        }
    }
}
