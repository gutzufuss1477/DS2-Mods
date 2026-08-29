using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.Serialization.Json;
using System.Security.Cryptography;
using System.Security.Principal;
using System.Text;
using System.Text.RegularExpressions;

namespace DS2ModSuite
{
    internal static class AppPaths
    {
        public const string SelfTestDataSlot = "DS2ModSuite.SelfTestUserRoot";
        public static readonly string BaseDirectory = AppDomain.CurrentDomain.BaseDirectory;
        private static readonly string SelfTestUserRoot = AppDomain.CurrentDomain.GetData(SelfTestDataSlot) as string;
        public static readonly bool IsSelfTest = !string.IsNullOrWhiteSpace(SelfTestUserRoot);
        public static readonly string UserDirectory = ResolveUserDirectory();
        public static readonly string SettingsPath = Path.Combine(UserDirectory, "settings.json");
        public static readonly string StatePath = Path.Combine(UserDirectory, "state.json");
        public static readonly string LogsDirectory = Path.Combine(UserDirectory, "logs");
        public static readonly string BackupsDirectory = Path.Combine(UserDirectory, "backups");

        private static string ResolveUserDirectory()
        {
            if (IsSelfTest)
            {
                return Path.GetFullPath(SelfTestUserRoot);
            }

            return Path.Combine(
                Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
                "SimonMods",
                "DS2ModSuite");
        }
    }

    internal static class EmbeddedData
    {
        private const string CatalogResource = "DS2ModSuite.Catalog";
        private const string PayloadPrefix = "DS2ModSuite.Payload.";

        public static Stream OpenCatalog()
        {
            Stream stream = Assembly.GetExecutingAssembly().GetManifestResourceStream(CatalogResource);
            if (stream == null)
            {
                throw new FileNotFoundException(Localization.T("The embedded mod catalog is missing.", "Der eingebettete Mod-Katalog fehlt."), CatalogResource);
            }

            return stream;
        }

        public static Stream OpenPayload(string expectedHash)
        {
            string normalized = NormalizeSha256(expectedHash);
            Stream stream = Assembly.GetExecutingAssembly().GetManifestResourceStream(PayloadPrefix + normalized);
            if (stream == null)
            {
                throw new FileNotFoundException(Localization.T("An embedded installation file is missing.", "Eine eingebettete Installationsdatei fehlt."), normalized);
            }

            return stream;
        }

        public static void CopyPayloadToFile(string expectedHash, string destination)
        {
            using (Stream source = OpenPayload(expectedHash))
            using (FileStream target = new FileStream(destination, FileMode.CreateNew, FileAccess.Write, FileShare.None))
            {
                source.CopyTo(target);
                target.Flush(true);
            }
        }

        private static string NormalizeSha256(string value)
        {
            string normalized = (value ?? string.Empty).Trim().ToUpperInvariant();
            if (!Regex.IsMatch(normalized, "^[0-9A-F]{64}$"))
            {
                throw new InvalidDataException(Localization.T("Invalid SHA-256 value in the catalog.", "Ungültiger SHA-256-Wert im Katalog."));
            }

            return normalized;
        }
    }

    internal static class JsonStore
    {
        private const long MaximumJsonBytes = 1024L * 1024L;

        public static T Read<T>(string path)
        {
            using (FileStream stream = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.Read))
            {
                if (stream.Length < 0 || stream.Length > MaximumJsonBytes)
                    throw new InvalidDataException("JSON data exceeds the supported size limit.");
                return Read<T>(stream);
            }
        }

        public static T Read<T>(Stream stream)
        {
            DataContractJsonSerializer serializer = new DataContractJsonSerializer(typeof(T));
            return (T)serializer.ReadObject(stream);
        }

        public static T FromBytes<T>(byte[] value)
        {
            using (MemoryStream stream = new MemoryStream(value, false))
            {
                return Read<T>(stream);
            }
        }

        public static byte[] ToBytes<T>(T value)
        {
            using (MemoryStream stream = new MemoryStream())
            {
                DataContractJsonSerializer serializer = new DataContractJsonSerializer(typeof(T));
                serializer.WriteObject(stream, value);
                return stream.ToArray();
            }
        }

        public static T ReadOrDefault<T>(string path) where T : class
        {
            try
            {
                return File.Exists(path) ? Read<T>(path) : null;
            }
            catch
            {
                return null;
            }
        }

        public static void Write<T>(string path, T value)
        {
            string directory = Path.GetDirectoryName(path);
            if (!string.IsNullOrEmpty(directory))
            {
                Directory.CreateDirectory(directory);
            }

            string temporary = path + ".tmp-" + Guid.NewGuid().ToString("N");
            try
            {
                using (FileStream stream = new FileStream(temporary, FileMode.CreateNew, FileAccess.Write, FileShare.None))
                {
                    DataContractJsonSerializer serializer = new DataContractJsonSerializer(typeof(T));
                    serializer.WriteObject(stream, value);
                    stream.Flush(true);
                }

                if (File.Exists(path))
                {
                    File.Replace(temporary, path, null);
                }
                else
                {
                    File.Move(temporary, path);
                }
            }
            finally
            {
                if (File.Exists(temporary))
                {
                    File.Delete(temporary);
                }
            }
        }
    }

    internal static class HashUtil
    {
        public static string FileSha256(string path)
        {
            using (FileStream stream = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.Read))
            {
                return StreamSha256(stream);
            }
        }

        public static string StringSha256(string value)
        {
            using (SHA256 algorithm = SHA256.Create())
            {
                return BytesToHex(algorithm.ComputeHash(Encoding.UTF8.GetBytes(value ?? string.Empty)));
            }
        }

        public static string BytesSha256(byte[] value)
        {
            using (SHA256 algorithm = SHA256.Create())
            {
                return BytesToHex(algorithm.ComputeHash(value ?? new byte[0]));
            }
        }

        public static string StreamSha256(Stream stream)
        {
            using (SHA256 algorithm = SHA256.Create())
            {
                byte[] hash = algorithm.ComputeHash(stream);
                return BytesToHex(hash);
            }
        }

        public static string BytesToHex(byte[] value)
        {
            StringBuilder text = new StringBuilder(value.Length * 2);
            for (int index = 0; index < value.Length; index++)
            {
                text.Append(value[index].ToString("X2", CultureInfo.InvariantCulture));
            }

            return text.ToString();
        }

        public static bool EqualsHash(string first, string second)
        {
            return !string.IsNullOrWhiteSpace(first)
                && !string.IsNullOrWhiteSpace(second)
                && string.Equals(first.Trim(), second.Trim(), StringComparison.OrdinalIgnoreCase);
        }
    }

    internal static class PathGuard
    {
        public static string ResolveUnderRoot(string root, string relativePath)
        {
            if (string.IsNullOrWhiteSpace(root) || string.IsNullOrWhiteSpace(relativePath))
            {
                throw new InvalidOperationException(Localization.T("A required path is missing.", "Ein erforderlicher Pfad fehlt."));
            }

            if (Path.IsPathRooted(relativePath))
            {
                throw new InvalidOperationException(Localization.T("Absolute target paths are not allowed in the catalog.", "Absolute Zielpfade sind im Katalog nicht erlaubt."));
            }

            if (relativePath.IndexOf(':') >= 0)
            {
                throw new InvalidOperationException(Localization.T(
                    "Alternate data streams are not allowed in installation paths: ",
                    "Alternative Datenströme sind in Installationspfaden nicht erlaubt: ") + relativePath);
            }

            string fullRoot = Path.GetFullPath(root).TrimEnd(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar)
                + Path.DirectorySeparatorChar;
            string resolved = Path.GetFullPath(Path.Combine(fullRoot, relativePath));
            if (!resolved.StartsWith(fullRoot, StringComparison.OrdinalIgnoreCase))
            {
                throw new InvalidOperationException(Localization.T("The target path leaves the allowed folder: ", "Der Zielpfad verlässt den erlaubten Ordner: ") + relativePath);
            }

            RejectExistingReparsePoints(fullRoot, resolved);

            return resolved;
        }

        public static void RejectRootReparsePoint(string root)
        {
            string fullRoot = Path.GetFullPath(root).TrimEnd(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar);
            if (!Directory.Exists(fullRoot))
            {
                throw new DirectoryNotFoundException(Localization.T("The allowed target folder is missing: ", "Der erlaubte Zielordner fehlt: ") + fullRoot);
            }

            FileAttributes attributes = File.GetAttributes(fullRoot);
            if ((attributes & FileAttributes.ReparsePoint) != 0)
            {
                throw new InvalidOperationException(Localization.T(
                    "A linked target folder was rejected for safety: ",
                    "Ein verknüpfter Zielordner wurde aus Sicherheitsgründen abgewiesen: ") + fullRoot);
            }
        }

        private static void RejectExistingReparsePoints(string fullRoot, string resolved)
        {
            string relative = resolved.Substring(fullRoot.Length);
            string current = fullRoot.TrimEnd(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar);
            foreach (string segment in relative.Split(new[] { Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar }, StringSplitOptions.RemoveEmptyEntries))
            {
                current = Path.Combine(current, segment);
                if (!File.Exists(current) && !Directory.Exists(current))
                {
                    continue;
                }

                FileAttributes attributes = File.GetAttributes(current);
                if ((attributes & FileAttributes.ReparsePoint) != 0)
                {
                    throw new InvalidOperationException(Localization.T(
                        "A file link in the installation target was rejected for safety: ",
                        "Eine Dateiverknüpfung im Installationsziel wurde aus Sicherheitsgründen abgewiesen: ") + current);
                }
            }
        }
    }

    internal static class PeInspector
    {
        private const ushort MachineAmd64 = 0x8664;

        public static bool Is64Bit(string path)
        {
            try
            {
                using (FileStream stream = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.Read))
                {
                    return Is64Bit(stream);
                }
            }
            catch
            {
                return false;
            }
        }

        public static bool Is64Bit(Stream stream)
        {
            try
            {
                using (BinaryReader reader = new BinaryReader(stream, Encoding.UTF8, true))
                {
                    if (reader.ReadUInt16() != 0x5A4D)
                    {
                        return false;
                    }

                    stream.Position = 0x3C;
                    int peOffset = reader.ReadInt32();
                    if (peOffset < 0 || peOffset > stream.Length - 6)
                    {
                        return false;
                    }

                    stream.Position = peOffset;
                    if (reader.ReadUInt32() != 0x00004550)
                    {
                        return false;
                    }

                    return reader.ReadUInt16() == MachineAmd64;
                }
            }
            catch
            {
                return false;
            }
        }
    }

    internal static class AuthenticationUtil
    {
        public static string CreateKeyHex()
        {
            byte[] value = new byte[32];
            using (RandomNumberGenerator generator = RandomNumberGenerator.Create())
            {
                generator.GetBytes(value);
            }

            return HashUtil.BytesToHex(value);
        }

        public static byte[] ParseKey(string keyHex)
        {
            if (string.IsNullOrWhiteSpace(keyHex) || !Regex.IsMatch(keyHex, "^[0-9A-Fa-f]{64}$"))
            {
                throw new InvalidDataException(Localization.T("The UAC authentication key is invalid.", "Der UAC-Authentifizierungsschlüssel ist ungültig."));
            }

            byte[] result = new byte[32];
            for (int index = 0; index < result.Length; index++)
            {
                result[index] = byte.Parse(keyHex.Substring(index * 2, 2), NumberStyles.HexNumber, CultureInfo.InvariantCulture);
            }

            return result;
        }

        public static string HmacHex(byte[] key, byte[] payload)
        {
            using (HMACSHA256 algorithm = new HMACSHA256(key))
            {
                return HashUtil.BytesToHex(algorithm.ComputeHash(payload));
            }
        }

        public static bool FixedTimeEqualsHex(string first, string second)
        {
            if (string.IsNullOrEmpty(first) || string.IsNullOrEmpty(second) || first.Length != second.Length)
            {
                return false;
            }

            int difference = 0;
            for (int index = 0; index < first.Length; index++)
            {
                difference |= first[index] ^ second[index];
            }

            return difference == 0;
        }
    }

    [System.Runtime.Serialization.DataContract]
    internal sealed class UserSettings
    {
        [System.Runtime.Serialization.DataMember(Name = "gamePath")]
        public string GamePath { get; set; }

        [System.Runtime.Serialization.DataMember(Name = "language", EmitDefaultValue = false)]
        public string Language { get; set; }
    }

    internal static class SettingsStore
    {
        private static readonly object SyncRoot = new object();

        public static string ReadGamePath()
        {
            lock (SyncRoot)
            {
                UserSettings settings = ReadSettings();
                return settings.GamePath;
            }
        }

        public static UiLanguage ReadLanguage()
        {
            lock (SyncRoot)
            {
                return Localization.ParseOrDefault(ReadSettings().Language);
            }
        }

        public static void WriteGamePath(string gamePath)
        {
            Update(delegate(UserSettings settings) { settings.GamePath = gamePath; });
        }

        public static void WriteLanguage(UiLanguage language)
        {
            Update(delegate(UserSettings settings) { settings.Language = Localization.ToCode(language); });
        }

        private static UserSettings ReadSettings()
        {
            return JsonStore.ReadOrDefault<UserSettings>(AppPaths.SettingsPath) ?? new UserSettings();
        }

        private static void Update(Action<UserSettings> update)
        {
            if (update == null || (SecurityContext.IsAdministrator() && !AppPaths.IsSelfTest))
            {
                return;
            }

            lock (SyncRoot)
            {
                UserSettings settings = ReadSettings();
                update(settings);
                JsonStore.Write(AppPaths.SettingsPath, settings);
            }
        }
    }

    internal static class GameLocator
    {
        public static string Find(GameSpec game)
        {
            List<string> candidates = new List<string>();
            AddCandidate(candidates, SettingsStore.ReadGamePath());
            AddCandidate(candidates, AppPaths.BaseDirectory);

            foreach (string root in FindSteamRoots())
            {
                AddCandidate(candidates, Path.Combine(root, "steamapps", "common", game.SteamFolder));
                string libraryFile = Path.Combine(root, "steamapps", "libraryfolders.vdf");
                if (!File.Exists(libraryFile))
                {
                    continue;
                }

                try
                {
                    string content = File.ReadAllText(libraryFile);
                    foreach (Match match in Regex.Matches(content, "\\\"path\\\"\\s+\\\"([^\\\"]+)\\\"", RegexOptions.IgnoreCase))
                    {
                        string library = match.Groups[1].Value.Replace("\\\\", "\\");
                        AddCandidate(candidates, Path.Combine(library, "steamapps", "common", game.SteamFolder));
                    }
                }
                catch
                {
                    // An unreadable Steam library is ignored; the user can still select DS2.exe manually.
                }
            }

            foreach (string candidate in candidates)
            {
                if (File.Exists(Path.Combine(candidate, game.Executable)))
                {
                    return candidate;
                }
            }

            return null;
        }

        private static IEnumerable<string> FindSteamRoots()
        {
            List<string> roots = new List<string>();
            TryReadSteamRegistry(roots, RegistryHive.CurrentUser, RegistryView.Default, @"Software\Valve\Steam", "SteamPath");
            TryReadSteamRegistry(roots, RegistryHive.LocalMachine, RegistryView.Registry32, @"SOFTWARE\Valve\Steam", "InstallPath");
            TryReadSteamRegistry(roots, RegistryHive.LocalMachine, RegistryView.Registry64, @"SOFTWARE\WOW6432Node\Valve\Steam", "InstallPath");
            AddCandidate(roots, @"C:\Program Files (x86)\Steam");
            AddCandidate(roots, @"C:\Program Files\Steam");
            return roots;
        }

        private static void TryReadSteamRegistry(List<string> roots, RegistryHive hive, RegistryView view, string keyPath, string valueName)
        {
            try
            {
                using (RegistryKey baseKey = RegistryKey.OpenBaseKey(hive, view))
                using (RegistryKey key = baseKey.OpenSubKey(keyPath))
                {
                    if (key != null)
                    {
                        AddCandidate(roots, key.GetValue(valueName) as string);
                    }
                }
            }
            catch
            {
                // Registry lookup is best-effort.
            }
        }

        private static void AddCandidate(List<string> values, string path)
        {
            if (string.IsNullOrWhiteSpace(path))
            {
                return;
            }

            try
            {
                string full = Path.GetFullPath(path);
                if (!values.Any(value => string.Equals(value, full, StringComparison.OrdinalIgnoreCase)))
                {
                    values.Add(full);
                }
            }
            catch
            {
                // Ignore malformed paths from external configuration.
            }
        }
    }

    internal static class SecurityContext
    {
        public static bool IsAdministrator()
        {
            try
            {
                WindowsIdentity identity = WindowsIdentity.GetCurrent();
                WindowsPrincipal principal = new WindowsPrincipal(identity);
                return principal.IsInRole(WindowsBuiltInRole.Administrator);
            }
            catch
            {
                return false;
            }
        }
    }
}
