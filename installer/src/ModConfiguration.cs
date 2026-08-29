using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;
using System.Text;

namespace DS2ModSuite
{
    [DataContract]
    internal sealed class SettingsSchema
    {
        [DataMember(Name = "schemaVersion")]
        public int SchemaVersion { get; set; }

        [DataMember(Name = "mods")]
        public List<SettingsSchemaMod> Mods { get; set; }
    }

    [DataContract]
    internal sealed class SettingsSchemaMod
    {
        [DataMember(Name = "modId")]
        public string ModId { get; set; }

        [DataMember(Name = "files")]
        public List<SettingsSchemaFile> Files { get; set; }
    }

    [DataContract]
    internal sealed class SettingsSchemaFile
    {
        [DataMember(Name = "target")]
        public string Target { get; set; }

        [DataMember(Name = "sections")]
        public List<SettingsSchemaSection> Sections { get; set; }
    }

    [DataContract]
    internal sealed class SettingsSchemaSection
    {
        [DataMember(Name = "name")]
        public string Name { get; set; }

        [DataMember(Name = "fields")]
        public List<SettingsSchemaField> Fields { get; set; }
    }

    [DataContract]
    internal sealed class SettingsSchemaField
    {
        [DataMember(Name = "key")]
        public string Key { get; set; }

        [DataMember(Name = "type")]
        public string Type { get; set; }

        [DataMember(Name = "default", EmitDefaultValue = false)]
        public string ExplicitDefault { get; set; }

        [DataMember(Name = "min", EmitDefaultValue = false)]
        public decimal? Min { get; set; }

        [DataMember(Name = "max", EmitDefaultValue = false)]
        public decimal? Max { get; set; }

        [DataMember(Name = "multipleOf", EmitDefaultValue = false)]
        public decimal? MultipleOf { get; set; }

        [DataMember(Name = "advanced", EmitDefaultValue = false)]
        public bool Advanced { get; set; }

        [DataMember(Name = "choices", EmitDefaultValue = false)]
        public List<string> Choices { get; set; }

        [DataMember(Name = "aliases", EmitDefaultValue = false)]
        public List<string> Aliases { get; set; }
    }

    internal sealed class ConfigFieldDefinition
    {
        public string Id { get; set; }
        public string ModId { get; set; }
        public string ModName { get; set; }
        public string Target { get; set; }
        public string PayloadHash { get; set; }
        public string Section { get; set; }
        public string Key { get; set; }
        public string Label { get; set; }
        public string Description { get; set; }
        public string DefaultValue { get; set; }
        public SettingsSchemaField Schema { get; set; }
    }

    [DataContract]
    internal sealed class ModConfigurationValue
    {
        [DataMember(Name = "id")]
        public string Id { get; set; }

        [DataMember(Name = "value")]
        public string Value { get; set; }
    }

    [DataContract]
    internal sealed class ModConfigurationProfile
    {
        [DataMember(Name = "schemaVersion")]
        public int SchemaVersion { get; set; }

        [DataMember(Name = "values")]
        public List<ModConfigurationValue> Values { get; set; }

        [DataMember(Name = "updatedUtc", EmitDefaultValue = false)]
        public string UpdatedUtc { get; set; }
    }

    internal sealed class IniDocument
    {
        private sealed class Line
        {
            public string Raw;
            public string Section;
            public string Key;
            public string Value;
            public bool IsKey;
        }

        private readonly List<Line> lines = new List<Line>();

        public static IniDocument Parse(string text)
        {
            IniDocument document = new IniDocument();
            string section = string.Empty;
            string normalized = (text ?? string.Empty).Replace("\r\n", "\n").Replace('\r', '\n');
            string[] sourceLines = normalized.Split('\n');
            int count = sourceLines.Length;
            if (count > 0 && sourceLines[count - 1].Length == 0) count--;
            for (int index = 0; index < count; index++)
            {
                string raw = sourceLines[index];
                string trimmed = raw.Trim();
                if (trimmed.Length >= 2 && trimmed[0] == '[' && trimmed[trimmed.Length - 1] == ']')
                {
                    section = trimmed.Substring(1, trimmed.Length - 2).Trim();
                    document.lines.Add(new Line { Raw = raw, Section = section });
                    continue;
                }

                int separator = trimmed.StartsWith(";", StringComparison.Ordinal) || trimmed.StartsWith("#", StringComparison.Ordinal)
                    ? -1
                    : raw.IndexOf('=');
                if (separator > 0)
                {
                    string key = raw.Substring(0, separator).Trim();
                    if (key.Length > 0)
                    {
                        document.lines.Add(new Line
                        {
                            Raw = raw,
                            Section = section,
                            Key = key,
                            Value = raw.Substring(separator + 1).Trim(),
                            IsKey = true
                        });
                        continue;
                    }
                }

                document.lines.Add(new Line { Raw = raw, Section = section });
            }
            return document;
        }

        public string GetValue(string section, string key, IEnumerable<string> aliases)
        {
            string value = GetLastValue(section, key);
            if (value != null) return value;
            foreach (string alias in aliases ?? Enumerable.Empty<string>())
            {
                value = GetLastValue(section, alias);
                if (value != null) return value;
            }
            return null;
        }

        public string GetLeadingComment(string section, string key)
        {
            int keyIndex = -1;
            for (int index = 0; index < lines.Count; index++)
            {
                if (lines[index].IsKey
                    && EqualsName(lines[index].Section, section)
                    && EqualsName(lines[index].Key, key)) keyIndex = index;
            }
            if (keyIndex < 1) return string.Empty;

            List<string> comments = new List<string>();
            for (int index = keyIndex - 1; index >= 0; index--)
            {
                string trimmed = (lines[index].Raw ?? string.Empty).Trim();
                if (trimmed.Length == 0) break;
                if (!(trimmed.StartsWith(";", StringComparison.Ordinal) || trimmed.StartsWith("#", StringComparison.Ordinal))) break;
                comments.Insert(0, trimmed.Substring(1).Trim());
            }
            return string.Join(" ", comments.Where(value => value.Length > 0));
        }

        public void SetValue(string section, string key, string value)
        {
            List<int> matches = new List<int>();
            for (int index = 0; index < lines.Count; index++)
            {
                if (lines[index].IsKey && EqualsName(lines[index].Section, section) && EqualsName(lines[index].Key, key))
                    matches.Add(index);
            }

            if (matches.Count > 0)
            {
                int keep = matches[matches.Count - 1];
                lines[keep].Raw = key + "=" + value;
                lines[keep].Key = key;
                lines[keep].Value = value;
                for (int index = matches.Count - 2; index >= 0; index--) lines.RemoveAt(matches[index]);
                return;
            }

            int sectionHeader = -1;
            int insertAt = lines.Count;
            for (int index = 0; index < lines.Count; index++)
            {
                string trimmed = (lines[index].Raw ?? string.Empty).Trim();
                if (trimmed.Length >= 2 && trimmed[0] == '[' && trimmed[trimmed.Length - 1] == ']')
                {
                    string found = trimmed.Substring(1, trimmed.Length - 2).Trim();
                    if (EqualsName(found, section))
                    {
                        sectionHeader = index;
                        insertAt = lines.Count;
                        for (int next = index + 1; next < lines.Count; next++)
                        {
                            string nextTrimmed = (lines[next].Raw ?? string.Empty).Trim();
                            if (nextTrimmed.Length >= 2 && nextTrimmed[0] == '[' && nextTrimmed[nextTrimmed.Length - 1] == ']')
                            {
                                insertAt = next;
                                break;
                            }
                        }
                        break;
                    }
                }
            }

            if (sectionHeader < 0)
            {
                if (lines.Count > 0 && !string.IsNullOrWhiteSpace(lines[lines.Count - 1].Raw))
                    lines.Add(new Line { Raw = string.Empty, Section = string.Empty });
                lines.Add(new Line { Raw = "[" + section + "]", Section = section });
                insertAt = lines.Count;
            }

            lines.Insert(insertAt, new Line { Raw = key + "=" + value, Section = section, Key = key, Value = value, IsKey = true });
        }

        public void RemoveKeyEverywhere(string key)
        {
            for (int index = lines.Count - 1; index >= 0; index--)
                if (lines[index].IsKey && EqualsName(lines[index].Key, key)) lines.RemoveAt(index);
        }

        public void NormalizeSection(string section, IEnumerable<string> allowedKeys)
        {
            HashSet<string> allowed = new HashSet<string>(allowedKeys ?? Enumerable.Empty<string>(), StringComparer.OrdinalIgnoreCase);
            HashSet<string> seen = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
            for (int index = lines.Count - 1; index >= 0; index--)
            {
                Line line = lines[index];
                if (!line.IsKey || !EqualsName(line.Section, section)) continue;
                if (!allowed.Contains(line.Key) || !seen.Add(line.Key)) lines.RemoveAt(index);
            }
        }

        public string ToText()
        {
            return string.Join("\r\n", lines.Select(line => line.Raw ?? string.Empty)) + "\r\n";
        }

        private string GetLastValue(string section, string key)
        {
            for (int index = lines.Count - 1; index >= 0; index--)
            {
                Line line = lines[index];
                if (line.IsKey && EqualsName(line.Section, section) && EqualsName(line.Key, key)) return line.Value;
            }
            return null;
        }

        private static bool EqualsName(string first, string second)
        {
            return string.Equals(first ?? string.Empty, second ?? string.Empty, StringComparison.OrdinalIgnoreCase);
        }
    }

    internal static class ModConfigurationService
    {
        private const string SchemaResource = "DS2ModSuite.SettingsSchema";
        private const int MaximumIniBytes = 1024 * 1024;
        private static readonly UTF8Encoding Utf8NoBom = new UTF8Encoding(false, true);

        public static string ProfilePath
        {
            get { return Path.Combine(AppPaths.UserDirectory, "mod-settings.json"); }
        }

        public static List<ConfigFieldDefinition> GetDefinitions(Catalog catalog)
        {
            if (catalog == null) throw new ArgumentNullException("catalog");
            SettingsSchema schema = LoadSchema();
            Dictionary<string, ModSpec> mods = catalog.Mods.ToDictionary(mod => mod.Id, StringComparer.OrdinalIgnoreCase);
            List<ConfigFieldDefinition> result = new List<ConfigFieldDefinition>();
            HashSet<string> ids = new HashSet<string>(StringComparer.OrdinalIgnoreCase);

            foreach (SettingsSchemaMod schemaMod in schema.Mods ?? new List<SettingsSchemaMod>())
            {
                ModSpec mod;
                if (string.IsNullOrWhiteSpace(schemaMod.ModId) || !mods.TryGetValue(schemaMod.ModId, out mod))
                    throw new InvalidDataException("Settings schema references an unknown mod: " + schemaMod.ModId);

                foreach (SettingsSchemaFile schemaFile in schemaMod.Files ?? new List<SettingsSchemaFile>())
                {
                    ModFileSpec file = mod.Files.FirstOrDefault(candidate => candidate.IsConfig
                        && string.Equals(candidate.Target, schemaFile.Target, StringComparison.Ordinal));
                    if (file == null) throw new InvalidDataException("Settings schema references an unknown configuration file: " + schemaFile.Target);
                    IniDocument defaults = IniDocument.Parse(ReadPayloadText(file.Sha256));

                    foreach (SettingsSchemaSection section in schemaFile.Sections ?? new List<SettingsSchemaSection>())
                    {
                        foreach (SettingsSchemaField field in section.Fields ?? new List<SettingsSchemaField>())
                        {
                            if (string.IsNullOrWhiteSpace(section.Name) || string.IsNullOrWhiteSpace(field.Key)
                                || string.IsNullOrWhiteSpace(field.Type) || field.Key.IndexOfAny(new[] { '[', ']', '=', '|', '\r', '\n' }) >= 0)
                                throw new InvalidDataException("Settings schema contains an invalid field.");

                            string id = BuildId(mod.Id, file.Target, section.Name, field.Key);
                            if (!ids.Add(id)) throw new InvalidDataException("Duplicate settings field: " + id);
                            string defaultValue = !string.IsNullOrWhiteSpace(field.ExplicitDefault)
                                ? field.ExplicitDefault
                                : defaults.GetValue(section.Name, field.Key, field.Aliases);
                            if (defaultValue == null) throw new InvalidDataException("A settings field has no default value: " + id);
                            string normalized;
                            string error;
                            if (!TryNormalize(field, defaultValue, out normalized, out error))
                                throw new InvalidDataException("Invalid default for " + id + ": " + error);

                            result.Add(new ConfigFieldDefinition
                            {
                                Id = id,
                                ModId = mod.Id,
                                ModName = mod.Name,
                                Target = file.Target,
                                PayloadHash = file.Sha256,
                                Section = section.Name,
                                Key = field.Key,
                                Label = Humanize(field.Key),
                                Description = defaults.GetLeadingComment(section.Name, field.Key),
                                DefaultValue = normalized,
                                Schema = field
                            });
                        }
                    }
                }
            }

            return result;
        }

        public static ModConfigurationProfile LoadEffectiveProfile(Catalog catalog, string gamePath)
        {
            List<ConfigFieldDefinition> definitions = GetDefinitions(catalog);
            ModConfigurationProfile stored = JsonStore.ReadOrDefault<ModConfigurationProfile>(ProfilePath);
            ModConfigurationProfile profile = NormalizeProfile(definitions, stored);
            if (stored == null && !string.IsNullOrWhiteSpace(gamePath)) ImportFromGame(definitions, profile, gamePath);
            return profile;
        }

        public static ModConfigurationProfile LoadStoredProfile(Catalog catalog)
        {
            ModConfigurationProfile stored = JsonStore.ReadOrDefault<ModConfigurationProfile>(ProfilePath);
            return stored == null ? null : NormalizeProfile(GetDefinitions(catalog), stored);
        }

        public static ModConfigurationProfile CloneProfile(ModConfigurationProfile profile)
        {
            return profile == null ? null : JsonStore.FromBytes<ModConfigurationProfile>(JsonStore.ToBytes(profile));
        }

        public static bool SaveProfile(Catalog catalog, ModConfigurationProfile profile, out string error)
        {
            error = null;
            if (!TryValidateProfile(catalog, profile, out error)) return false;
            if (SecurityContext.IsAdministrator())
            {
                error = Localization.T(
                    "The elevated application keeps these settings for this session only. Start the manager normally to save them permanently.",
                    "Die erhöhte Anwendung behält diese Einstellungen nur für diese Sitzung. Starte den Manager normal, um sie dauerhaft zu speichern.");
                return false;
            }
            profile.UpdatedUtc = DateTime.UtcNow.ToString("O", CultureInfo.InvariantCulture);
            JsonStore.Write(ProfilePath, profile);
            return true;
        }

        public static bool TryValidateProfile(Catalog catalog, ModConfigurationProfile profile, out string error)
        {
            List<ConfigFieldDefinition> definitions;
            try { definitions = GetDefinitions(catalog); }
            catch (Exception exception) { error = exception.Message; return false; }
            return TryValidateProfile(definitions, profile, true, out error);
        }

        public static bool HasDifferences(Catalog catalog, ModConfigurationProfile profile, IEnumerable<string> selectedModIds, string gamePath)
        {
            if (profile == null || string.IsNullOrWhiteSpace(gamePath) || !Directory.Exists(gamePath)) return false;
            List<ConfigFieldDefinition> definitions = GetDefinitions(catalog);
            string error;
            if (!TryValidateProfile(definitions, profile, true, out error)) return true;
            HashSet<string> selected = new HashSet<string>(selectedModIds ?? Enumerable.Empty<string>(), StringComparer.OrdinalIgnoreCase);
            Dictionary<string, string> values = ProfileDictionary(profile);
            foreach (IGrouping<string, ConfigFieldDefinition> fileGroup in definitions.Where(field => selected.Contains(field.ModId)).GroupBy(field => field.Target, StringComparer.OrdinalIgnoreCase))
            {
                string path = PathGuard.ResolveUnderRoot(gamePath, fileGroup.Key);
                if (!File.Exists(path)) return true;
                IniDocument document;
                try { document = ReadIni(path); }
                catch { return true; }
                foreach (ConfigFieldDefinition field in fileGroup)
                {
                    string current = document.GetValue(field.Section, field.Key, field.Schema.Aliases) ?? field.DefaultValue;
                    string normalized;
                    string validationError;
                    if (!TryNormalize(field.Schema, current, out normalized, out validationError)
                        || !string.Equals(normalized, values[field.Id], StringComparison.OrdinalIgnoreCase)) return true;
                }
            }
            return false;
        }

        public static byte[] BuildConfiguredIni(Catalog catalog, ModConfigurationProfile profile, string target, string existingPath)
        {
            List<ConfigFieldDefinition> definitions = GetDefinitions(catalog)
                .Where(field => string.Equals(field.Target, target, StringComparison.OrdinalIgnoreCase)).ToList();
            if (definitions.Count == 0) throw new InvalidDataException("No settings schema exists for " + target);
            string error;
            if (!TryValidateProfile(GetDefinitions(catalog), profile, true, out error)) throw new InvalidDataException(error);
            Dictionary<string, string> values = ProfileDictionary(profile);
            string text = File.Exists(existingPath) ? ReadText(existingPath) : ReadPayloadText(definitions[0].PayloadHash);
            IniDocument document = IniDocument.Parse(text);

            string modId = definitions[0].ModId;
            if (string.Equals(modId, "sam-stats-booster", StringComparison.OrdinalIgnoreCase)
                || string.Equals(modId, "porter-grade-booster", StringComparison.OrdinalIgnoreCase))
            {
                foreach (ConfigFieldDefinition field in definitions) document.RemoveKeyEverywhere(field.Key);
            }
            if (string.Equals(modId, "chiral-bandwidth-costs", StringComparison.OrdinalIgnoreCase))
            {
                foreach (IGrouping<string, ConfigFieldDefinition> section in definitions.GroupBy(field => field.Section, StringComparer.OrdinalIgnoreCase))
                    document.NormalizeSection(section.Key, section.Select(field => field.Key));
            }

            foreach (ConfigFieldDefinition field in definitions)
            {
                foreach (string alias in field.Schema.Aliases ?? new List<string>()) document.RemoveKeyEverywhere(alias);
                document.SetValue(field.Section, field.Key, values[field.Id]);
            }
            return Utf8NoBom.GetBytes(document.ToText());
        }

        public static bool ConfiguredIniMatches(Catalog catalog, ModConfigurationProfile profile, string target, string path)
        {
            if (!File.Exists(path)) return false;
            List<ConfigFieldDefinition> definitions = GetDefinitions(catalog)
                .Where(field => string.Equals(field.Target, target, StringComparison.OrdinalIgnoreCase)).ToList();
            Dictionary<string, string> values = ProfileDictionary(profile);
            IniDocument document = ReadIni(path);
            foreach (ConfigFieldDefinition field in definitions)
            {
                string current = document.GetValue(field.Section, field.Key, field.Schema.Aliases) ?? field.DefaultValue;
                string normalized;
                string error;
                if (!TryNormalize(field.Schema, current, out normalized, out error)
                    || !string.Equals(normalized, values[field.Id], StringComparison.OrdinalIgnoreCase)) return false;
            }
            return true;
        }

        public static bool HasDefinitionForTarget(Catalog catalog, string target)
        {
            return GetDefinitions(catalog).Any(field => string.Equals(field.Target, target, StringComparison.OrdinalIgnoreCase));
        }

        public static void ImportFromGame(
            Catalog catalog,
            ModConfigurationProfile profile,
            string gamePath,
            IEnumerable<string> includedModIds = null)
        {
            List<ConfigFieldDefinition> definitions = GetDefinitions(catalog);
            if (includedModIds != null)
            {
                HashSet<string> included = new HashSet<string>(includedModIds, StringComparer.OrdinalIgnoreCase);
                definitions = definitions.Where(field => included.Contains(field.ModId)).ToList();
            }
            ImportFromGame(definitions, profile, gamePath);
        }

        public static string GetValue(ModConfigurationProfile profile, string id)
        {
            ModConfigurationValue value = (profile == null ? null : profile.Values == null ? null : profile.Values.FirstOrDefault(item => string.Equals(item.Id, id, StringComparison.OrdinalIgnoreCase)));
            return value == null ? null : value.Value;
        }

        public static void SetValue(ModConfigurationProfile profile, string id, string value)
        {
            if (profile.Values == null) profile.Values = new List<ModConfigurationValue>();
            ModConfigurationValue current = profile.Values.FirstOrDefault(item => string.Equals(item.Id, id, StringComparison.OrdinalIgnoreCase));
            if (current == null)
            {
                profile.Values.Add(new ModConfigurationValue { Id = id, Value = value });
            }
            else current.Value = value;
        }

        public static void ResetMod(Catalog catalog, ModConfigurationProfile profile, string modId)
        {
            foreach (ConfigFieldDefinition field in GetDefinitions(catalog).Where(item => string.Equals(item.ModId, modId, StringComparison.OrdinalIgnoreCase)))
                SetValue(profile, field.Id, field.DefaultValue);
        }

        private static SettingsSchema LoadSchema()
        {
            using (Stream stream = Assembly.GetExecutingAssembly().GetManifestResourceStream(SchemaResource))
            {
                if (stream == null) throw new FileNotFoundException("The embedded settings schema is missing.", SchemaResource);
                DataContractJsonSerializer serializer = new DataContractJsonSerializer(typeof(SettingsSchema));
                SettingsSchema schema = (SettingsSchema)serializer.ReadObject(stream);
                if (schema == null || schema.SchemaVersion != 1 || schema.Mods == null) throw new InvalidDataException("The settings schema is invalid.");
                return schema;
            }
        }

        private static ModConfigurationProfile NormalizeProfile(List<ConfigFieldDefinition> definitions, ModConfigurationProfile profile)
        {
            Dictionary<string, string> existing = ProfileDictionary(profile);
            ModConfigurationProfile normalized = new ModConfigurationProfile { SchemaVersion = 1, Values = new List<ModConfigurationValue>() };
            foreach (ConfigFieldDefinition field in definitions)
            {
                string value;
                string candidate;
                string error;
                if (!existing.TryGetValue(field.Id, out candidate) || !TryNormalize(field.Schema, candidate, out value, out error))
                    value = field.DefaultValue;
                normalized.Values.Add(new ModConfigurationValue { Id = field.Id, Value = value });
            }
            return normalized;
        }

        private static bool TryValidateProfile(List<ConfigFieldDefinition> definitions, ModConfigurationProfile profile, bool normalize, out string error)
        {
            error = null;
            if (profile == null || profile.SchemaVersion != 1 || profile.Values == null)
            {
                error = Localization.T("The mod settings profile is invalid.", "Das Mod-Einstellungsprofil ist ungültig.");
                return false;
            }
            Dictionary<string, ConfigFieldDefinition> byId = definitions.ToDictionary(field => field.Id, StringComparer.OrdinalIgnoreCase);
            HashSet<string> seen = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
            foreach (ModConfigurationValue item in profile.Values)
            {
                ConfigFieldDefinition field;
                if (item == null || string.IsNullOrWhiteSpace(item.Id) || !seen.Add(item.Id) || !byId.TryGetValue(item.Id, out field))
                {
                    error = Localization.T("The mod settings profile contains an unknown or duplicate field.", "Das Mod-Einstellungsprofil enthält ein unbekanntes oder doppeltes Feld.");
                    return false;
                }
                string value;
                if (!TryNormalize(field.Schema, item.Value, out value, out error))
                {
                    error = field.Label + ": " + error;
                    return false;
                }
                if (normalize) item.Value = value;
            }
            if (seen.Count != definitions.Count)
            {
                error = Localization.T("The mod settings profile is incomplete.", "Das Mod-Einstellungsprofil ist unvollständig.");
                return false;
            }
            return ValidateRelationships(definitions, profile, out error);
        }

        private static bool ValidateRelationships(List<ConfigFieldDefinition> definitions, ModConfigurationProfile profile, out string error)
        {
            error = null;
            Dictionary<string, string> values = ProfileDictionary(profile);
            Func<string, string, string, decimal> number = delegate(string modId, string section, string key)
            {
                ConfigFieldDefinition field = definitions.First(item => string.Equals(item.ModId, modId, StringComparison.OrdinalIgnoreCase)
                    && string.Equals(item.Section, section, StringComparison.OrdinalIgnoreCase)
                    && string.Equals(item.Key, key, StringComparison.OrdinalIgnoreCase));
                return decimal.Parse(values[field.Id], NumberStyles.Number, CultureInfo.InvariantCulture);
            };

            if (number("hill-assist-and-speed-boost", "HillAssist", "FullAssistSlopeDegrees")
                <= number("hill-assist-and-speed-boost", "HillAssist", "AssistStartSlopeDegrees"))
            {
                error = Localization.T("Full assist slope must be greater than the assist start slope.", "Die Volle-Hilfe-Steigung muss größer als die Start-Steigung sein.");
                return false;
            }
            foreach (string section in new[] { "HillAssist", "VehicleSpeedBoost" })
            {
                if (number("hill-assist-and-speed-boost", section, "ScanEndGroup")
                    <= number("hill-assist-and-speed-boost", section, "ScanStartGroup"))
                {
                    error = Localization.T("Each scan end group must be greater than its start group.", "Jede Scan-Endgruppe muss größer als ihre Startgruppe sein.");
                    return false;
                }
            }
            if (number("zipline-range", "ZiplineRange", "Level2RangeMeters")
                < number("zipline-range", "ZiplineRange", "Level1RangeMeters"))
            {
                error = Localization.T("Level 2 zipline range must not be lower than Level 1 range.", "Die Level-2-Zipline-Reichweite darf nicht kleiner als Level 1 sein.");
                return false;
            }
            return true;
        }

        private static bool TryNormalize(SettingsSchemaField field, string input, out string normalized, out string error)
        {
            normalized = null;
            error = null;
            string value = (input ?? string.Empty).Trim();
            if (value.Length == 0 || value.Length > 128 || value.IndexOfAny(new[] { '\r', '\n', '[', ']', '=', ';' }) >= 0)
            {
                error = Localization.T("Enter a valid value.", "Gib einen gültigen Wert ein.");
                return false;
            }

            decimal numeric;
            if (string.Equals(field.Type, "bool", StringComparison.OrdinalIgnoreCase))
            {
                if (value == "1" || string.Equals(value, "true", StringComparison.OrdinalIgnoreCase)) normalized = "1";
                else if (value == "0" || string.Equals(value, "false", StringComparison.OrdinalIgnoreCase)) normalized = "0";
                else { error = Localization.T("Use enabled or disabled.", "Verwende aktiviert oder deaktiviert."); return false; }
                return true;
            }
            if (string.Equals(field.Type, "choice", StringComparison.OrdinalIgnoreCase))
            {
                string choice = (field.Choices ?? new List<string>()).FirstOrDefault(item => string.Equals(item, value, StringComparison.OrdinalIgnoreCase));
                if (choice == null) { error = Localization.T("Select one of the available values.", "Wähle einen der verfügbaren Werte."); return false; }
                normalized = choice;
                return true;
            }
            if (string.Equals(field.Type, "nativeOrInt", StringComparison.OrdinalIgnoreCase)
                && string.Equals(value, "Native", StringComparison.OrdinalIgnoreCase))
            {
                normalized = "Native";
                return true;
            }
            if (string.Equals(field.Type, "uint64", StringComparison.OrdinalIgnoreCase))
            {
                ulong unsigned;
                if (!ulong.TryParse(value, NumberStyles.None, CultureInfo.InvariantCulture, out unsigned))
                { error = Localization.T("Enter a whole non-negative number.", "Gib eine nichtnegative ganze Zahl ein."); return false; }
                numeric = unsigned;
                normalized = unsigned.ToString(CultureInfo.InvariantCulture);
            }
            else if (string.Equals(field.Type, "int", StringComparison.OrdinalIgnoreCase)
                || string.Equals(field.Type, "nativeOrInt", StringComparison.OrdinalIgnoreCase))
            {
                long integer;
                if (!long.TryParse(value, NumberStyles.Integer, CultureInfo.InvariantCulture, out integer))
                { error = Localization.T("Enter a whole number.", "Gib eine ganze Zahl ein."); return false; }
                numeric = integer;
                normalized = integer.ToString(CultureInfo.InvariantCulture);
            }
            else if (string.Equals(field.Type, "decimal", StringComparison.OrdinalIgnoreCase))
            {
                if (!decimal.TryParse(value.Replace(',', '.'), NumberStyles.AllowDecimalPoint | NumberStyles.AllowLeadingSign, CultureInfo.InvariantCulture, out numeric))
                { error = Localization.T("Enter a number.", "Gib eine Zahl ein."); return false; }
                normalized = numeric.ToString("0.############################", CultureInfo.InvariantCulture);
            }
            else
            {
                error = Localization.T("The setting type is not supported.", "Der Einstellungstyp wird nicht unterstützt.");
                return false;
            }

            if ((field.Min.HasValue && numeric < field.Min.Value) || (field.Max.HasValue && numeric > field.Max.Value))
            {
                error = Localization.Format("Allowed range: {0} to {1}.", "Erlaubter Bereich: {0} bis {1}.",
                    field.Min.HasValue ? field.Min.Value.ToString(CultureInfo.InvariantCulture) : "-∞",
                    field.Max.HasValue ? field.Max.Value.ToString(CultureInfo.InvariantCulture) : "∞");
                return false;
            }
            if (field.MultipleOf.HasValue && field.MultipleOf.Value != 0 && numeric % field.MultipleOf.Value != 0)
            {
                error = Localization.Format("The value must be a multiple of {0}.", "Der Wert muss ein Vielfaches von {0} sein.", field.MultipleOf.Value);
                return false;
            }
            return true;
        }

        private static void ImportFromGame(List<ConfigFieldDefinition> definitions, ModConfigurationProfile profile, string gamePath)
        {
            if (profile == null || string.IsNullOrWhiteSpace(gamePath) || !Directory.Exists(gamePath)) return;
            foreach (IGrouping<string, ConfigFieldDefinition> fileGroup in definitions.GroupBy(field => field.Target, StringComparer.OrdinalIgnoreCase))
            {
                string path;
                try { path = PathGuard.ResolveUnderRoot(gamePath, fileGroup.Key); }
                catch { continue; }
                if (!File.Exists(path)) continue;
                IniDocument document;
                try { document = ReadIni(path); }
                catch { continue; }
                foreach (ConfigFieldDefinition field in fileGroup)
                {
                    string disk = document.GetValue(field.Section, field.Key, field.Schema.Aliases);
                    string normalized;
                    string error;
                    if (disk != null && TryNormalize(field.Schema, disk, out normalized, out error)) SetValue(profile, field.Id, normalized);
                }
            }
        }

        private static IniDocument ReadIni(string path)
        {
            return IniDocument.Parse(ReadText(path));
        }

        private static string ReadText(string path)
        {
            FileInfo info = new FileInfo(path);
            if (info.Length < 0 || info.Length > MaximumIniBytes) throw new IOException("Configuration file is unexpectedly large: " + path);
            using (FileStream stream = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.Read))
            {
                if (stream.Length < 0 || stream.Length > MaximumIniBytes) throw new IOException("Configuration file is unexpectedly large: " + path);
                using (StreamReader reader = new StreamReader(stream, Encoding.UTF8, true, 4096, false)) return reader.ReadToEnd();
            }
        }

        private static string ReadPayloadText(string hash)
        {
            using (Stream stream = EmbeddedData.OpenPayload(hash))
            {
                if (stream.Length < 0 || stream.Length > MaximumIniBytes) throw new IOException("Embedded configuration is unexpectedly large.");
                using (StreamReader reader = new StreamReader(stream, Encoding.UTF8, true, 4096, false)) return reader.ReadToEnd();
            }
        }

        private static Dictionary<string, string> ProfileDictionary(ModConfigurationProfile profile)
        {
            Dictionary<string, string> result = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
            if (profile == null || profile.Values == null) return result;
            foreach (ModConfigurationValue item in profile.Values)
                if (item != null && !string.IsNullOrWhiteSpace(item.Id) && !result.ContainsKey(item.Id)) result[item.Id] = item.Value;
            return result;
        }

        private static string BuildId(string modId, string target, string section, string key)
        {
            return modId + "|" + target + "|" + section + "|" + key;
        }

        private static string Humanize(string value)
        {
            if (string.IsNullOrWhiteSpace(value)) return string.Empty;
            StringBuilder result = new StringBuilder();
            for (int index = 0; index < value.Length; index++)
            {
                char current = value[index];
                if (index > 0 && char.IsUpper(current)
                    && (char.IsLower(value[index - 1]) || (index + 1 < value.Length && char.IsLower(value[index + 1])))) result.Append(' ');
                result.Append(current);
            }
            return result.ToString();
        }
    }
}
