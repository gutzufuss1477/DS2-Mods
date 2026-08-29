using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;

namespace DS2ModSuite
{
    internal static class CatalogService
    {
        public static Catalog LoadAndValidate()
        {
            Catalog catalog;
            using (Stream stream = EmbeddedData.OpenCatalog())
            {
                catalog = JsonStore.Read<Catalog>(stream);
            }
            if (catalog == null || catalog.SchemaVersion != 1 || catalog.Game == null || catalog.Loader == null || catalog.Mods == null)
            {
                throw new InvalidDataException(Localization.T("The mod catalog is invalid or uses an unsupported version.", "Der Mod-Katalog ist ungültig oder verwendet eine nicht unterstützte Version."));
            }

            if (string.IsNullOrWhiteSpace(catalog.Game.Executable)
                || !string.Equals(Path.GetFileName(catalog.Game.Executable), catalog.Game.Executable, StringComparison.Ordinal)
                || string.IsNullOrWhiteSpace(catalog.Loader.FileName)
                || !string.Equals(Path.GetFileName(catalog.Loader.FileName), catalog.Loader.FileName, StringComparison.Ordinal)
                || string.IsNullOrWhiteSpace(catalog.Loader.Payload)
                || !IsSha256(catalog.Loader.Sha256))
            {
                throw new InvalidDataException(Localization.T("Game or loader information in the embedded catalog is invalid.", "Spiel- oder Loader-Angaben im eingebetteten Katalog sind ungültig."));
            }

            PathGuard.ResolveUnderRoot("C:\\DS2-CATALOG-PAYLOAD", catalog.Loader.Payload);
            foreach (string proxyName in catalog.Loader.ProxyFileNames ?? new List<string>())
            {
                if (!string.Equals(Path.GetFileName(proxyName), proxyName, StringComparison.Ordinal))
                {
                    throw new InvalidDataException(Localization.T("Invalid proxy filename in the catalog: ", "Ungültiger Proxy-Dateiname im Katalog: ") + proxyName);
                }
            }

            HashSet<string> identifiers = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
            HashSet<string> targets = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
            HashSet<string> obsoleteTargets = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
            foreach (ModSpec mod in catalog.Mods)
            {
                if (mod == null || string.IsNullOrWhiteSpace(mod.Id) || string.IsNullOrWhiteSpace(mod.Name) || mod.Files == null || mod.Files.Count == 0)
                {
                    throw new InvalidDataException(Localization.T("At least one mod entry in the catalog is incomplete.", "Mindestens ein Mod-Eintrag im Katalog ist unvollständig."));
                }

                if (!identifiers.Add(mod.Id))
                {
                    throw new InvalidDataException(Localization.T("Duplicate mod ID in the catalog: ", "Doppelte Mod-ID im Katalog: ") + mod.Id);
                }

                if (!mod.Files.Any(file => file != null && !file.IsConfig))
                {
                    throw new InvalidDataException(Localization.T("Mod without a binary file in the catalog: ", "Mod ohne Binärdatei im Katalog: ") + mod.Name);
                }

                foreach (ModFileSpec file in mod.Files)
                {
                    if (file == null || string.IsNullOrWhiteSpace(file.Payload) || string.IsNullOrWhiteSpace(file.Target) || !IsSha256(file.Sha256))
                    {
                        throw new InvalidDataException(Localization.T("Incomplete file entry for ", "Unvollständige Datei-Angabe bei ") + mod.Name + ".");
                    }

                    PathGuard.ResolveUnderRoot("C:\\DS2-CATALOG-PAYLOAD", file.Payload);
                    PathGuard.ResolveUnderRoot("C:\\DS2-CATALOG-VALIDATION", file.Target);
                    if (!targets.Add(file.Target))
                    {
                        throw new InvalidDataException(Localization.T("Multiple mods use the same target path: ", "Mehrere Mods verwenden denselben Zielpfad: ") + file.Target);
                    }
                }

                foreach (ObsoleteFileSpec obsolete in mod.ObsoleteFiles ?? new List<ObsoleteFileSpec>())
                {
                    if (obsolete == null || string.IsNullOrWhiteSpace(obsolete.Target) || string.IsNullOrWhiteSpace(obsolete.Sha256))
                    {
                        throw new InvalidDataException(Localization.T("Incomplete legacy-file entry for ", "Unvollständiger Altdatei-Eintrag bei ") + mod.Name + ".");
                    }

                    PathGuard.ResolveUnderRoot("C:\\DS2-CATALOG-VALIDATION", obsolete.Target);
                    if (!IsSha256(obsolete.Sha256) || !obsoleteTargets.Add(obsolete.Target))
                    {
                        throw new InvalidDataException(Localization.T("Duplicate or invalid legacy-file entry for ", "Doppelter oder ungültiger Altdatei-Eintrag bei ") + mod.Name + ": " + obsolete.Target);
                    }
                }
            }

            string overlap = obsoleteTargets.FirstOrDefault(target => targets.Contains(target));
            if (overlap != null)
            {
                throw new InvalidDataException(Localization.T("A legacy path conflicts with a current mod file: ", "Ein Altdateipfad kollidiert mit einer aktuellen Mod-Datei: ") + overlap);
            }

            return catalog;
        }

        private static bool IsSha256(string value)
        {
            return !string.IsNullOrWhiteSpace(value)
                && value.Trim().Length == 64
                && value.Trim().All(character => Uri.IsHexDigit(character));
        }

        public static void ValidatePayloads(Catalog catalog)
        {
            ValidatePayload(catalog.Loader.Payload, catalog.Loader.Sha256, "ASI Loader");
            foreach (ModSpec mod in catalog.Mods)
            {
                foreach (ModFileSpec file in mod.Files)
                {
                    ValidatePayload(file.Payload, file.Sha256, mod.Name);
                }
            }
        }

        private static void ValidatePayload(string relativePath, string expectedHash, string owner)
        {
            try
            {
                using (Stream stream = EmbeddedData.OpenPayload(expectedHash))
                {
                    string actualHash = HashUtil.StreamSha256(stream);
                    if (!HashUtil.EqualsHash(actualHash, expectedHash))
                    {
                        throw new InvalidDataException(Localization.T("Hash verification failed for ", "Hash-Prüfung fehlgeschlagen für ") + owner + ": " + Path.GetFileName(relativePath));
                    }
                }
            }
            catch (FileNotFoundException exception)
            {
                throw new FileNotFoundException(Localization.T("Payload is missing for ", "Payload fehlt für ") + owner + ": " + Path.GetFileName(relativePath), exception);
            }
        }
    }

    internal static class LoaderInspector
    {
        public static LoaderInspection Inspect(string gamePath, LoaderSpec spec)
        {
            LoaderInspection result = new LoaderInspection
            {
                Status = LoaderStatus.Absent,
                DisplayText = Localization.T("ASI Loader will be installed automatically", "ASI Loader wird automatisch installiert"),
                DetailText = Localization.T("The tested x64 loader is added when you apply the changes.", "Der getestete x64-Loader wird beim Anwenden ergänzt."),
                CandidateFileNames = new List<string>()
            };

            bool payloadValid = false;
            try
            {
                using (Stream stream = EmbeddedData.OpenPayload(spec.Sha256))
                {
                    payloadValid = PeInspector.Is64Bit(stream);
                }
                using (Stream stream = EmbeddedData.OpenPayload(spec.Sha256))
                {
                    payloadValid = payloadValid && HashUtil.EqualsHash(HashUtil.StreamSha256(stream), spec.Sha256);
                }
            }
            catch
            {
                payloadValid = false;
            }

            if (!payloadValid)
            {
                result.Status = LoaderStatus.InvalidPayload;
                result.DisplayText = Localization.T("Loader payload is invalid", "Loader-Payload ist ungültig");
                result.DetailText = Localization.T("The bundled loader file is missing or has an unexpected hash.", "Die mitgelieferte Loader-Datei fehlt oder hat einen unerwarteten Hash.");
                return result;
            }

            List<string> candidates = new List<string>();
            foreach (string fileName in spec.ProxyFileNames ?? new List<string>())
            {
                string candidate = Path.Combine(gamePath, fileName);
                if (File.Exists(candidate))
                {
                    candidates.Add(candidate);
                }
            }
            result.CandidateFileNames = candidates.Select(Path.GetFileName).ToList();

            List<string> knownCandidates = new List<string>();
            List<string> unknownCandidates = new List<string>();
            Dictionary<string, string> candidateHashes = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
            foreach (string candidate in candidates)
            {
                string hash;
                try
                {
                    hash = HashUtil.FileSha256(candidate);
                }
                catch
                {
                    continue;
                }

                bool knownHash = HashUtil.EqualsHash(hash, spec.Sha256)
                    || (spec.KnownCompatibleHashes ?? new List<string>()).Any(value => HashUtil.EqualsHash(value, hash));
                candidateHashes[candidate] = hash;
                if (knownHash && PeInspector.Is64Bit(candidate))
                {
                    knownCandidates.Add(candidate);
                }
                else
                {
                    unknownCandidates.Add(candidate);
                }
            }

            if (knownCandidates.Count == 1 && unknownCandidates.Count == 0 && candidates.Count == 1)
            {
                string candidate = knownCandidates[0];
                FileVersionInfo versionInfo = FileVersionInfo.GetVersionInfo(candidate);
                result.Status = LoaderStatus.KnownCompatible;
                result.DisplayText = "ASI Loader " + (string.IsNullOrWhiteSpace(versionInfo.FileVersion)
                    ? Localization.T("detected", "erkannt")
                    : versionInfo.FileVersion + Localization.T(" is installed", " ist installiert"));
                result.DetailText = Path.GetFileName(candidate) + Localization.T(" · x64 · Hash verified", " · x64 · Hash bestätigt");
                result.FilePath = candidate;
                result.FileVersion = versionInfo.FileVersion;
                result.Hash = candidateHashes[candidate];
                return result;
            }

            if (candidates.Count > 0)
            {
                string candidate = unknownCandidates.FirstOrDefault() ?? candidates[0];
                result.Status = LoaderStatus.UnknownProxy;
                result.DisplayText = candidates.Count > 1
                    ? Localization.T("Multiple proxy DLLs found", "Mehrere Proxy-DLLs gefunden")
                    : Localization.T("Unknown proxy DLL found", "Unbekannte Proxy-DLL gefunden");
                result.DetailText = string.Join(", ", candidates.Select(Path.GetFileName)) + Localization.T(
                    " will not be changed. New installations and updates are blocked until you resolve this manually.",
                    " werden nicht verändert. Neue Installationen und Updates sind bis zur manuellen Klärung gesperrt.");
                result.FilePath = candidate;
                result.Hash = candidateHashes.ContainsKey(candidate) ? candidateHashes[candidate] : null;
                result.OtherProxyCount = Math.Max(0, candidates.Count - 1);
            }

            return result;
        }

        public static void Relocalize(LoaderInspection result)
        {
            if (result == null) return;

            if (result.Status == LoaderStatus.KnownCompatible)
            {
                result.DisplayText = "ASI Loader " + (string.IsNullOrWhiteSpace(result.FileVersion)
                    ? Localization.T("detected", "erkannt")
                    : result.FileVersion + Localization.T(" is installed", " ist installiert"));
                result.DetailText = Path.GetFileName(result.FilePath) + Localization.T(" · x64 · Hash verified", " · x64 · Hash bestätigt");
                return;
            }

            if (result.Status == LoaderStatus.InvalidPayload)
            {
                result.DisplayText = Localization.T("Loader payload is invalid", "Loader-Payload ist ungültig");
                result.DetailText = Localization.T(
                    "The bundled loader file is missing or has an unexpected hash.",
                    "Die mitgelieferte Loader-Datei fehlt oder hat einen unerwarteten Hash.");
                return;
            }

            if (result.Status == LoaderStatus.UnknownProxy)
            {
                List<string> names = (result.CandidateFileNames ?? new List<string>())
                    .Where(value => !string.IsNullOrWhiteSpace(value))
                    .ToList();
                if (names.Count == 0 && !string.IsNullOrWhiteSpace(result.FilePath)) names.Add(Path.GetFileName(result.FilePath));
                result.DisplayText = result.OtherProxyCount > 0
                    ? Localization.T("Multiple proxy DLLs found", "Mehrere Proxy-DLLs gefunden")
                    : Localization.T("Unknown proxy DLL found", "Unbekannte Proxy-DLL gefunden");
                bool multiple = names.Count > 1 || result.OtherProxyCount > 0;
                result.DetailText = (names.Count > 0 ? string.Join(", ", names) : Localization.T("The detected proxy DLL", "Die erkannte Proxy-DLL"))
                    + Localization.T(
                        " will not be changed. New installations and updates are blocked until you resolve this manually.",
                        multiple
                            ? " werden nicht verändert. Neue Installationen und Updates sind bis zur manuellen Klärung gesperrt."
                            : " wird nicht verändert. Neue Installationen und Updates sind bis zur manuellen Klärung gesperrt.");
                return;
            }

            result.DisplayText = Localization.T("ASI Loader will be installed automatically", "ASI Loader wird automatisch installiert");
            result.DetailText = Localization.T(
                "The tested x64 loader is added when you apply the changes.",
                "Der getestete x64-Loader wird beim Anwenden ergänzt.");
        }
    }

    internal static class GameInspector
    {
        public static GameInspection Inspect(Catalog catalog, string selectedPath, bool persistSettings = true)
        {
            GameInspection result = new GameInspection();
            string gamePath = string.IsNullOrWhiteSpace(selectedPath) ? GameLocator.Find(catalog.Game) : selectedPath;
            if (string.IsNullOrWhiteSpace(gamePath))
            {
                result.Error = Localization.T("The game folder could not be found automatically.", "Der Spielordner konnte nicht automatisch gefunden werden.");
                result.Loader = new LoaderInspection { Status = LoaderStatus.Absent, DisplayText = Localization.T("Not checked yet", "Noch nicht geprüft") };
                return result;
            }

            try
            {
                gamePath = Path.GetFullPath(gamePath);
                string exePath = Path.Combine(gamePath, catalog.Game.Executable);
                result.GamePath = gamePath;
                result.ExePath = exePath;
                if (!File.Exists(exePath))
                {
                    result.Error = Localization.T("No ", "Im ausgewählten Ordner wurde keine ") + catalog.Game.Executable
                        + Localization.T(" was found in the selected folder.", " gefunden.");
                    result.Loader = new LoaderInspection { Status = LoaderStatus.Absent, DisplayText = Localization.T("Not checked yet", "Noch nicht geprüft") };
                    return result;
                }

                result.Found = true;
                FileVersionInfo info = FileVersionInfo.GetVersionInfo(exePath);
                result.FileVersion = info.FileVersion;
                result.Sha256 = HashUtil.FileSha256(exePath);
                result.Supported = (catalog.Game.SupportedVersions ?? new List<GameBuildSpec>()).Any(build =>
                    string.Equals(build.FileVersion, result.FileVersion, StringComparison.OrdinalIgnoreCase)
                    && HashUtil.EqualsHash(build.Sha256, result.Sha256));
                result.IsRunning = Process.GetProcessesByName(Path.GetFileNameWithoutExtension(catalog.Game.Executable)).Length > 0;
                result.Loader = LoaderInspector.Inspect(gamePath, catalog.Loader);
                result.ForeignAsiCount = CountForeignAsis(gamePath, catalog);
                if (persistSettings)
                {
                    SettingsStore.WriteGamePath(gamePath);
                }
            }
            catch (Exception exception)
            {
                result.Error = Localization.T("The game installation could not be verified completely: ", "Die Spielinstallation konnte nicht vollständig geprüft werden: ") + exception.Message;
                if (result.Loader == null)
                {
                    result.Loader = new LoaderInspection { Status = LoaderStatus.Absent, DisplayText = Localization.T("Check failed", "Prüfung fehlgeschlagen") };
                }
            }

            return result;
        }

        private static int CountForeignAsis(string gamePath, Catalog catalog)
        {
            HashSet<string> knownNames = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
            foreach (ModSpec mod in catalog.Mods)
            {
                foreach (ModFileSpec file in mod.Files.Where(item => !item.IsConfig))
                {
                    knownNames.Add(Path.GetFileName(file.Target));
                }
                foreach (ObsoleteFileSpec file in mod.ObsoleteFiles ?? new List<ObsoleteFileSpec>())
                {
                    knownNames.Add(Path.GetFileName(file.Target));
                }
            }

            try
            {
                return Directory.GetFiles(gamePath, "*.asi", SearchOption.TopDirectoryOnly)
                    .Count(path => !knownNames.Contains(Path.GetFileName(path)));
            }
            catch
            {
                return 0;
            }
        }

        public static List<ModRuntimeState> InspectMods(GameInspection game, Catalog catalog)
        {
            List<ModRuntimeState> states = new List<ModRuntimeState>();
            foreach (ModSpec mod in catalog.Mods)
            {
                ModFileSpec primary = mod.Files.FirstOrDefault(file => !file.IsConfig);
                string target = game != null && game.Found && primary != null
                    ? PathGuard.ResolveUnderRoot(game.GamePath, primary.Target)
                    : null;
                bool exists = target != null && File.Exists(target);
                bool exact = false;
                bool missingRequiredFile = false;
                bool hasObsolete = false;
                bool hasUnknownObsolete = false;
                List<string> unknownObsoleteNames = new List<string>();
                if (exists)
                {
                    try
                    {
                        exact = HashUtil.EqualsHash(HashUtil.FileSha256(target), primary.Sha256);
                    }
                    catch
                    {
                        exact = false;
                    }
                }

                if (game != null && game.Found && exact)
                {
                    missingRequiredFile = mod.Files.Any(file =>
                        !File.Exists(PathGuard.ResolveUnderRoot(game.GamePath, file.Target)));
                }

                if (game != null && game.Found)
                {
                    foreach (ObsoleteFileSpec file in mod.ObsoleteFiles ?? new List<ObsoleteFileSpec>())
                    {
                        string obsoletePath = PathGuard.ResolveUnderRoot(game.GamePath, file.Target);
                        if (!File.Exists(obsoletePath))
                        {
                            continue;
                        }

                        try
                        {
                            if (HashUtil.EqualsHash(HashUtil.FileSha256(obsoletePath), file.Sha256)) hasObsolete = true;
                            else
                            {
                                hasUnknownObsolete = true;
                                unknownObsoleteNames.Add(Path.GetFileName(file.Target));
                            }
                        }
                        catch
                        {
                            hasUnknownObsolete = true;
                            unknownObsoleteNames.Add(Path.GetFileName(file.Target));
                        }
                    }
                }

                ModRuntimeState state = new ModRuntimeState
                {
                    Spec = mod,
                    IsInstalled = exact,
                    HasTargetBinary = exists,
                    HasModifiedBinary = exists && !exact,
                    HasObsoleteBinary = hasObsolete,
                    HasUnknownObsoleteBinary = hasUnknownObsolete,
                    ConflictFileNames = string.Join(", ", unknownObsoleteNames.Distinct(StringComparer.OrdinalIgnoreCase)),
                    NeedsRepair = missingRequiredFile,
                    DesiredEnabled = exact || hasObsolete
                };
                UpdateStatus(state);
                states.Add(state);
            }

            return states;
        }

        public static void UpdateStatus(ModRuntimeState state)
        {
            if (state.DesiredEnabled && state.HasUnknownObsoleteBinary)
            {
                state.StatusText = Localization.T("Conflicting file", "Konfliktdatei");
                state.StatusTone = "warning";
            }
            else if (state.DesiredEnabled && state.HasObsoleteBinary)
            {
                state.StatusText = Localization.T("Will be updated", "Wird aktualisiert");
                state.StatusTone = "warning";
            }
            else if (state.DesiredEnabled && !state.HasTargetBinary)
            {
                state.StatusText = Localization.T("Will be installed", "Wird installiert");
                state.StatusTone = "accent";
            }
            else if (state.DesiredEnabled && state.HasModifiedBinary)
            {
                state.StatusText = Localization.T("Will be updated", "Wird aktualisiert");
                state.StatusTone = "warning";
            }
            else if (state.DesiredEnabled && state.NeedsRepair)
            {
                state.StatusText = Localization.T("Will be repaired", "Wird repariert");
                state.StatusTone = "warning";
            }
            else if (state.DesiredEnabled && state.IsInstalled)
            {
                state.StatusText = Localization.T("Installed", "Installiert");
                state.StatusTone = "success";
            }
            else if (!state.DesiredEnabled && state.IsInstalled)
            {
                state.StatusText = Localization.T("Will be removed", "Wird entfernt");
                state.StatusTone = "warning";
            }
            else if (!state.DesiredEnabled && (state.HasModifiedBinary || state.HasUnknownObsoleteBinary))
            {
                state.StatusText = Localization.T("Different version", "Andere Version");
                state.StatusTone = "neutral";
            }
            else
            {
                state.StatusText = Localization.T("Not installed", "Nicht installiert");
                state.StatusTone = "neutral";
            }
        }
    }
}
