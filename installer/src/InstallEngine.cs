using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace DS2ModSuite
{
    internal sealed class InstallLogger : IDisposable
    {
        private readonly StreamWriter writer;
        public string Path { get; private set; }

        public InstallLogger(string logsDirectory = null)
        {
            string directory = string.IsNullOrWhiteSpace(logsDirectory) ? AppPaths.LogsDirectory : logsDirectory;
            Directory.CreateDirectory(directory);
            Path = System.IO.Path.Combine(
                directory,
                "install-" + DateTime.Now.ToString("yyyyMMdd-HHmmss-fff") + "-" + Guid.NewGuid().ToString("N").Substring(0, 6) + ".log");
            writer = new StreamWriter(new FileStream(Path, FileMode.CreateNew, FileAccess.Write, FileShare.Read));
            writer.AutoFlush = true;
            Write("DS2 Mod Suite installation log");
            Write("App image: " + Process.GetCurrentProcess().MainModule.FileName);
        }

        public void Write(string message)
        {
            try { writer.WriteLine("[{0:yyyy-MM-dd HH:mm:ss.fff}] {1}", DateTime.Now, message); }
            catch { }
        }

        public void Dispose()
        {
            try { writer.Dispose(); } catch { }
        }
    }

    internal sealed class FileChange
    {
        public string Destination { get; set; }
        public string Backup { get; set; }
        public bool Existed { get; set; }
        public byte[] OriginalBytes { get; set; }
        public string OriginalHash { get; set; }
        public string InstalledHash { get; set; }
    }

    internal sealed class FileTransaction : IDisposable
    {
        private readonly List<FileChange> changes = new List<FileChange>();
        private readonly InstallLogger logger;
        private readonly string backupRoot;
        private readonly StreamWriter manifest;
        private bool committed;
        private bool recoveryFailure;

        public string BackupRoot { get { return backupRoot; } }
        public bool HasChanges { get { return changes.Count > 0; } }
        public bool Committed { get { return committed; } }
        public bool RollbackAttempted { get; private set; }
        public bool RollbackSucceeded { get; private set; } = true;

        public FileTransaction(InstallLogger logger, string backupsDirectory = null)
        {
            this.logger = logger;
            string baseDirectory = string.IsNullOrWhiteSpace(backupsDirectory) ? AppPaths.BackupsDirectory : backupsDirectory;
            Directory.CreateDirectory(baseDirectory);
            backupRoot = System.IO.Path.Combine(
                baseDirectory,
                DateTime.Now.ToString("yyyyMMdd-HHmmss-fff") + "-" + Guid.NewGuid().ToString("N").Substring(0, 8));
            Directory.CreateDirectory(backupRoot);
            manifest = new StreamWriter(new FileStream(
                System.IO.Path.Combine(backupRoot, "backup-manifest.txt"),
                FileMode.CreateNew,
                FileAccess.Write,
                FileShare.Read));
            manifest.AutoFlush = true;
            manifest.WriteLine("DS2 Mod Suite backup");
            manifest.WriteLine("Created: " + DateTime.Now.ToString("O"));
            manifest.WriteLine();
        }

        public bool ReplaceFrom(string source, string destination, string expectedHash)
        {
            if (!File.Exists(source))
            {
                throw new FileNotFoundException(Localization.T("Installation file is missing.", "Installationsdatei fehlt."), source);
            }

            using (FileStream stream = new FileStream(source, FileMode.Open, FileAccess.Read, FileShare.Read))
            {
                return ReplaceFromStream(stream, destination, expectedHash, System.IO.Path.GetFileName(source));
            }
        }

        public bool ReplaceFromPayload(string destination, string expectedHash)
        {
            using (Stream stream = EmbeddedData.OpenPayload(expectedHash))
            {
                return ReplaceFromStream(stream, destination, expectedHash, "embedded payload");
            }
        }

        public bool ReplaceFromBytes(string destination, byte[] bytes, string sourceLabel)
        {
            if (bytes == null) throw new ArgumentNullException("bytes");
            string expectedHash = HashUtil.BytesSha256(bytes);
            using (MemoryStream stream = new MemoryStream(bytes, false))
            {
                return ReplaceFromStream(stream, destination, expectedHash, sourceLabel ?? "generated settings");
            }
        }

        private bool ReplaceFromStream(Stream source, string destination, string expectedHash, string sourceLabel)
        {
            RejectDestinationReparsePoint(destination);
            if (File.Exists(destination) && HashUtil.EqualsHash(HashUtil.FileSha256(destination), expectedHash))
            {
                logger.Write("Unchanged: " + destination);
                return false;
            }

            string directory = System.IO.Path.GetDirectoryName(destination);
            if (string.IsNullOrWhiteSpace(directory) || !Directory.Exists(directory))
            {
                throw new DirectoryNotFoundException(Localization.T("The target folder is missing: ", "Der Zielordner fehlt: ") + directory);
            }

            string temporary = System.IO.Path.Combine(directory, ".ds2aio-stage-" + Guid.NewGuid().ToString("N") + ".tmp");
            try
            {
                using (FileStream staged = new FileStream(temporary, FileMode.CreateNew, FileAccess.Write, FileShare.None))
                {
                    source.CopyTo(staged);
                    staged.Flush(true);
                }

                string stagedHash = HashUtil.FileSha256(temporary);
                if (!HashUtil.EqualsHash(stagedHash, expectedHash))
                {
                    throw new InvalidDataException("Payload-Hash stimmt nicht: " + sourceLabel);
                }

                FileChange change;
                if (File.Exists(destination))
                {
                    string atomicBackup = System.IO.Path.Combine(directory, ".ds2aio-replaced-" + Guid.NewGuid().ToString("N") + ".tmp");
                    File.Replace(temporary, destination, atomicBackup);
                    try
                    {
                        change = PrepareExistingChange(destination, atomicBackup, expectedHash);
                    }
                    catch
                    {
                        if (!changes.Any(item => string.Equals(item.Destination, destination, StringComparison.OrdinalIgnoreCase)))
                        {
                            try { File.Replace(atomicBackup, destination, null); }
                            catch
                            {
                                recoveryFailure = true;
                                RollbackAttempted = true;
                                RollbackSucceeded = false;
                            }
                        }
                        throw;
                    }
                    finally
                    {
                        if (changes.Any(item => string.Equals(item.Destination, destination, StringComparison.OrdinalIgnoreCase)))
                        {
                            try { if (File.Exists(atomicBackup)) File.Delete(atomicBackup); } catch { }
                        }
                    }
                }
                else
                {
                    change = PrepareNewChange(destination, expectedHash);
                    File.Move(temporary, destination);
                }

                if (!HashUtil.EqualsHash(HashUtil.FileSha256(destination), expectedHash))
                {
                    throw new IOException(Localization.T(
                        "The installed file has an unexpected hash after atomic replacement: ",
                        "Die installierte Datei hat nach dem atomaren Ersetzen einen unerwarteten Hash: ") + destination);
                }

                logger.Write("Installed: " + destination);
                return true;
            }
            finally
            {
                try { if (File.Exists(temporary)) File.Delete(temporary); } catch { }
            }
        }

        public bool DeleteIfExact(string destination, string expectedHash)
        {
            RejectDestinationReparsePoint(destination);
            if (!File.Exists(destination)) return false;

            string directory = System.IO.Path.GetDirectoryName(destination);
            string quarantine = System.IO.Path.Combine(directory, ".ds2aio-quarantine-" + Guid.NewGuid().ToString("N") + ".tmp");
            bool recorded = false;
            try
            {
                File.Move(destination, quarantine);
                RejectDestinationReparsePoint(quarantine);
                byte[] original = ReadStableBytes(quarantine);
                string actualHash = HashUtil.BytesSha256(original);
                if (!HashUtil.EqualsHash(actualHash, expectedHash))
                {
                    File.Move(quarantine, destination);
                    logger.Write("Retained because it is modified or foreign: " + destination);
                    return false;
                }

                PrepareExistingChange(destination, quarantine, null, original);
                recorded = true;
                File.Delete(quarantine);
                logger.Write("Removed: " + destination);
                return true;
            }
            catch
            {
                if (!recorded && File.Exists(quarantine) && !File.Exists(destination))
                {
                    try { File.Move(quarantine, destination); } catch { }
                }
                if (!recorded && File.Exists(quarantine))
                {
                    recoveryFailure = true;
                    RollbackAttempted = true;
                    RollbackSucceeded = false;
                    logger.Write("Quarantine could not be restored safely: " + quarantine);
                }
                throw;
            }
            finally
            {
                if (recorded)
                {
                    try { if (File.Exists(quarantine)) File.Delete(quarantine); } catch { }
                }
            }
        }

        private FileChange PrepareExistingChange(string destination, string sourceForBackup, string installedHash, byte[] knownOriginal = null)
        {
            byte[] original = knownOriginal ?? ReadStableBytes(sourceForBackup);
            string safeName = changes.Count.ToString("D4") + "-" + System.IO.Path.GetFileName(destination);
            string backup = System.IO.Path.Combine(backupRoot, safeName);
            using (FileStream stream = new FileStream(backup, FileMode.CreateNew, FileAccess.Write, FileShare.Read))
            {
                stream.Write(original, 0, original.Length);
                stream.Flush(true);
            }

            manifest.WriteLine(destination + " => " + backup);
            FileChange change = new FileChange
            {
                Destination = destination,
                Backup = backup,
                Existed = true,
                OriginalBytes = original,
                OriginalHash = HashUtil.BytesSha256(original),
                InstalledHash = installedHash
            };
            changes.Add(change);
            logger.Write("Backup: " + destination + " => " + backup);
            return change;
        }

        private FileChange PrepareNewChange(string destination, string installedHash)
        {
            manifest.WriteLine(destination + " => [new file]");
            FileChange change = new FileChange { Destination = destination, Existed = false, InstalledHash = installedHash };
            changes.Add(change);
            return change;
        }

        private static byte[] ReadStableBytes(string path)
        {
            const long MaximumManagedBackupSize = 256L * 1024L * 1024L;
            using (FileStream stream = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.Read))
            {
                long length = stream.Length;
                if (length < 0 || length > MaximumManagedBackupSize)
                {
                    throw new IOException(Localization.T(
                        "The file to be backed up is unexpectedly large and will not be replaced automatically: ",
                        "Die zu sichernde Datei ist unerwartet groß und wird nicht automatisch ersetzt: ") + path);
                }

                using (MemoryStream memory = new MemoryStream((int)length))
                {
                    stream.CopyTo(memory);
                    return memory.ToArray();
                }
            }
        }

        private static void RejectDestinationReparsePoint(string path)
        {
            if ((File.Exists(path) || Directory.Exists(path))
                && (File.GetAttributes(path) & FileAttributes.ReparsePoint) != 0)
            {
                throw new InvalidOperationException(Localization.T(
                    "A file link was rejected for safety: ",
                    "Eine Dateiverknüpfung wurde aus Sicherheitsgründen abgewiesen: ") + path);
            }
        }

        public void Commit()
        {
            manifest.WriteLine();
            manifest.WriteLine("Status: COMMITTED");
            manifest.Dispose();
            committed = true;
            if (changes.Count == 0)
            {
                try { Directory.Delete(backupRoot, true); } catch { }
            }
            else logger.Write("Backup completed: " + backupRoot);
        }

        private void Rollback()
        {
            RollbackAttempted = true;
            RollbackSucceeded = !recoveryFailure;
            logger.Write("Rollback started.");
            for (int index = changes.Count - 1; index >= 0; index--)
            {
                FileChange change = changes[index];
                try
                {
                    RejectDestinationReparsePoint(change.Destination);
                    if (change.Existed)
                    {
                        string directory = System.IO.Path.GetDirectoryName(change.Destination);
                        string temporary = System.IO.Path.Combine(directory, ".ds2aio-rollback-" + Guid.NewGuid().ToString("N") + ".tmp");
                        try
                        {
                            using (FileStream stream = new FileStream(temporary, FileMode.CreateNew, FileAccess.Write, FileShare.None))
                            {
                                stream.Write(change.OriginalBytes, 0, change.OriginalBytes.Length);
                                stream.Flush(true);
                            }
                            if (File.Exists(change.Destination))
                            {
                                if (string.IsNullOrWhiteSpace(change.InstalledHash)
                                    || !HashUtil.EqualsHash(HashUtil.FileSha256(change.Destination), change.InstalledHash))
                                {
                                    throw new IOException(Localization.T(
                                        "A target file modified by another process was not overwritten during rollback.",
                                        "Eine inzwischen fremd veränderte Zieldatei wurde beim Rollback nicht überschrieben."));
                                }
                                File.Replace(temporary, change.Destination, null);
                            }
                            else File.Move(temporary, change.Destination);
                        }
                        finally
                        {
                            try { if (File.Exists(temporary)) File.Delete(temporary); } catch { }
                        }

                        if (!HashUtil.EqualsHash(HashUtil.FileSha256(change.Destination), change.OriginalHash))
                        {
                            throw new IOException(Localization.T("The original hash could not be restored.", "Der ursprüngliche Hash konnte nicht wiederhergestellt werden."));
                        }
                    }
                    else if (File.Exists(change.Destination))
                    {
                        string currentHash = HashUtil.FileSha256(change.Destination);
                        if (!HashUtil.EqualsHash(currentHash, change.InstalledHash))
                        {
                            throw new IOException(Localization.T(
                                "A newly created file modified by another process was retained during rollback.",
                                "Eine inzwischen veränderte neue Datei wurde beim Rollback beibehalten."));
                        }
                        File.Delete(change.Destination);
                    }
                    logger.Write("Rollback verified: " + change.Destination);
                }
                catch (Exception exception)
                {
                    RollbackSucceeded = false;
                    logger.Write("Rollback error for " + change.Destination + ": " + exception.Message);
                }
            }
        }

        public void Dispose()
        {
            if (committed) return;
            try { manifest.WriteLine("Status: ROLLBACK"); } catch { }
            try { Rollback(); }
            finally
            {
                try { manifest.Dispose(); } catch { RollbackSucceeded = false; }
            }
        }
    }

    internal sealed class InstallEngine
    {
        private readonly Catalog catalog;
        private readonly bool allowUnsupportedBuild;

        public InstallEngine(Catalog catalog, bool allowUnsupportedBuild)
        {
            this.catalog = catalog;
            this.allowUnsupportedBuild = allowUnsupportedBuild;
        }

        public ApplyResult Apply(ApplyPlan plan, IProgress<ProgressInfo> progress)
        {
            ApplyResult result = new ApplyResult { RollbackComplete = true };
            InstallLogger logger = null;
            FileTransaction transaction = null;
            FileStream gameIdentityHandle = null;
            Mutex gameMutex = null;
            bool mutexAcquired = false;
            try
            {
                string gamePath = ValidatePlan(plan);
                PathGuard.RejectRootReparsePoint(gamePath);
                gameMutex = new Mutex(false, @"Local\DS2ModSuite-" + HashUtil.StringSha256(gamePath.ToUpperInvariant()));
                try { mutexAcquired = gameMutex.WaitOne(0); }
                catch (AbandonedMutexException) { mutexAcquired = true; }
                if (!mutexAcquired)
                {
                    throw new InvalidOperationException(Localization.T(
                        "Another DS2 mod operation is already running for this game folder.",
                        "Für diesen Spielordner läuft bereits eine andere DS2-Mod-Änderung."));
                }

                GameInspection game = GameInspector.Inspect(catalog, gamePath, false);
                if (!game.Found) throw new InvalidOperationException(game.Error ?? Localization.T("DS2.exe was not found.", "DS2.exe wurde nicht gefunden."));
                if (game.IsRunning && !allowUnsupportedBuild)
                {
                    throw new InvalidOperationException(Localization.T(
                        "Death Stranding 2 is still running. Close the game completely.",
                        "Death Stranding 2 läuft noch. Bitte schließe das Spiel vollständig."));
                }

                gameIdentityHandle = new FileStream(game.ExePath, FileMode.Open, FileAccess.Read, FileShare.Read);
                if (!HashUtil.EqualsHash(HashUtil.StreamSha256(gameIdentityHandle), game.Sha256))
                {
                    throw new IOException(Localization.T("DS2.exe was replaced during verification.", "DS2.exe wurde während der Prüfung ausgetauscht."));
                }

                HashSet<string> selected = new HashSet<string>(plan.SelectedModIds ?? new List<string>(), StringComparer.OrdinalIgnoreCase);
                bool needsPayloadChange = DesiredStateNeedsPayload(game, selected, plan.ConfigurationProfile);
                if (!allowUnsupportedBuild && !game.Supported && needsPayloadChange)
                {
                    throw new InvalidOperationException(Localization.T(
                        "This DS2.exe version is not supported for installations or updates. Safe removals remain available.",
                        "Diese DS2.exe-Version wird für Installationen oder Updates nicht unterstützt. Sichere Entfernungen bleiben möglich."));
                }
                if (needsPayloadChange && game.Loader.Status == LoaderStatus.InvalidPayload)
                {
                    throw new InvalidDataException(game.Loader.DetailText);
                }
                if (needsPayloadChange && game.Loader.Status == LoaderStatus.UnknownProxy)
                {
                    throw new InvalidOperationException(Localization.T(
                        "An unknown or duplicate DLL proxy blocks safe installation or updating. No proxy DLL was changed.",
                        "Ein unbekannter oder mehrfach vorhandener DLL-Proxy verhindert die sichere Installation bzw. Aktualisierung. Keine Proxy-DLL wurde verändert."));
                }
                if (needsPayloadChange) CatalogService.ValidatePayloads(catalog);

                string operationRoot = SecurityContext.IsAdministrator() && !allowUnsupportedBuild
                    ? PathGuard.ResolveUnderRoot(game.GamePath, ".ds2-mod-suite")
                    : AppPaths.UserDirectory;
                Directory.CreateDirectory(operationRoot);
                PathGuard.RejectRootReparsePoint(operationRoot);
                string logsDirectory = PathGuard.ResolveUnderRoot(operationRoot, "logs");
                string backupsDirectory = PathGuard.ResolveUnderRoot(operationRoot, "backups");
                Directory.CreateDirectory(logsDirectory);
                Directory.CreateDirectory(backupsDirectory);
                logsDirectory = PathGuard.ResolveUnderRoot(operationRoot, "logs");
                backupsDirectory = PathGuard.ResolveUnderRoot(operationRoot, "backups");
                logger = new InstallLogger(logsDirectory);
                result.LogPath = logger.Path;
                Report(progress, 3, Localization.T("Preparing installation …", "Installation wird vorbereitet …"));
                logger.Write("Game path: " + game.GamePath);
                logger.Write("Game version: " + game.FileVersion);
                logger.Write("Game SHA256: " + game.Sha256);
                logger.Write("Selected mods: " + string.Join(", ", selected.OrderBy(value => value)));

                VerifyGameIdentity(game);

                using (transaction = new FileTransaction(logger, backupsDirectory))
                {
                    int totalSteps = Math.Max(1, catalog.Mods.Count + 3);
                    int step = 1;
                    if (game.Loader.Status == LoaderStatus.Absent && selected.Count > 0)
                    {
                        Report(progress, 10, Localization.T("Installing ASI Loader …", "ASI Loader wird installiert …"));
                        string destination = PathGuard.ResolveUnderRoot(game.GamePath, catalog.Loader.FileName);
                        transaction.ReplaceFromPayload(destination, catalog.Loader.Sha256);
                        result.LoaderInstalled = true;
                    }
                    else if (game.Loader.Status == LoaderStatus.UnknownProxy)
                    {
                        logger.Write("Unknown proxy DLLs remain unchanged during mod-only removal.");
                    }
                    else logger.Write("ASI-Loader-Status: " + game.Loader.DisplayText);

                    foreach (ModSpec mod in catalog.Mods)
                    {
                        if (!allowUnsupportedBuild && Process.GetProcessesByName(Path.GetFileNameWithoutExtension(catalog.Game.Executable)).Length > 0)
                        {
                            throw new InvalidOperationException(Localization.T(
                                "Death Stranding 2 was started during the operation. Changes are being rolled back.",
                                "Death Stranding 2 wurde während der Änderung gestartet. Der Vorgang wird zurückgesetzt."));
                        }
                        step++;
                        int percent = 10 + (int)((step / (double)totalSteps) * 75.0);
                        bool shouldInstall = selected.Contains(mod.Id);
                        ModFileSpec primary = mod.Files.First(file => !file.IsConfig);
                        string primaryDestination = PathGuard.ResolveUnderRoot(game.GamePath, primary.Target);
                        List<ObsoleteFileSpec> obsoleteFiles = mod.ObsoleteFiles ?? new List<ObsoleteFileSpec>();
                        bool primaryTargetExisted = File.Exists(primaryDestination);
                        bool obsoleteExisted = obsoleteFiles.Any(file => File.Exists(PathGuard.ResolveUnderRoot(game.GamePath, file.Target)));
                        bool primaryExisted = primaryTargetExisted || obsoleteExisted;
                        bool primaryWasExact = IsExactFile(primaryDestination, primary.Sha256);

                        if (shouldInstall)
                        {
                            Report(progress, percent, mod.Name + Localization.T(" is being checked …", " wird geprüft …"));
                            bool payloadChanged = false;
                            foreach (ObsoleteFileSpec obsolete in obsoleteFiles)
                            {
                                string oldDestination = PathGuard.ResolveUnderRoot(game.GamePath, obsolete.Target);
                                if (!File.Exists(oldDestination)) continue;
                                if (!IsExactFile(oldDestination, obsolete.Sha256))
                                {
                                    throw new InvalidOperationException(Localization.T(
                                        "An unknown legacy file blocks the safe update: ",
                                        "Eine unbekannte ältere Datei verhindert das sichere Update: ") + obsolete.Target);
                                }
                                payloadChanged |= transaction.DeleteIfExact(oldDestination, obsolete.Sha256);
                            }

                            foreach (ModFileSpec file in mod.Files)
                            {
                                string destination = PathGuard.ResolveUnderRoot(game.GamePath, file.Target);
                                bool existedBefore = File.Exists(destination);
                                if (file.IsConfig && plan.ConfigurationProfile != null
                                    && ModConfigurationService.HasDefinitionForTarget(catalog, file.Target))
                                {
                                    byte[] configured = ModConfigurationService.BuildConfiguredIni(
                                        catalog, plan.ConfigurationProfile, file.Target, destination);
                                    bool configuredChanged = transaction.ReplaceFromBytes(destination, configured, "mod settings");
                                    if (configuredChanged)
                                    {
                                        result.ConfigurationsUpdated++;
                                        if (!existedBefore) payloadChanged = true;
                                    }
                                    continue;
                                }
                                if (file.IsConfig && File.Exists(destination))
                                {
                                    if (ModConfigurationService.RequiresExactSectionKeys(mod.Id))
                                    {
                                        if (!ModConfigurationService.StableExistingIniMatches(catalog, mod.Id, file.Target, destination))
                                        {
                                            byte[] stabilized = ModConfigurationService.BuildStableExistingIni(
                                                catalog, mod.Id, file.Target, destination);
                                            if (transaction.ReplaceFromBytes(destination, stabilized, "stable configuration migration"))
                                                result.ConfigurationsUpdated++;
                                        }
                                        else logger.Write("Existing stable configuration retained: " + destination);
                                        continue;
                                    }
                                    logger.Write("Existing configuration retained: " + destination);
                                    continue;
                                }
                                payloadChanged |= transaction.ReplaceFromPayload(destination, file.Sha256);
                            }

                            if (payloadChanged && !primaryExisted) result.Installed++;
                            else if (payloadChanged && (!primaryWasExact || obsoleteExisted)) result.Updated++;
                            else if (payloadChanged) result.Repaired++;
                        }
                        else
                        {
                            Report(progress, percent, mod.Name + Localization.T(" is being reconciled …", " wird abgeglichen …"));
                            bool removedPrimary = false;
                            foreach (ModFileSpec file in mod.Files.Where(file => !file.IsConfig))
                            {
                                string destination = PathGuard.ResolveUnderRoot(game.GamePath, file.Target);
                                bool existed = File.Exists(destination);
                                bool removed = transaction.DeleteIfExact(destination, file.Sha256);
                                removedPrimary |= removed;
                                if (existed && !removed) result.RetainedFiles++;
                            }
                            foreach (ObsoleteFileSpec obsolete in obsoleteFiles)
                            {
                                string oldDestination = PathGuard.ResolveUnderRoot(game.GamePath, obsolete.Target);
                                bool existed = File.Exists(oldDestination);
                                bool removed = transaction.DeleteIfExact(oldDestination, obsolete.Sha256);
                                removedPrimary |= removed;
                                if (existed && !removed) result.RetainedFiles++;
                            }
                            bool binaryRemains = mod.Files.Where(file => !file.IsConfig)
                                .Any(file => File.Exists(PathGuard.ResolveUnderRoot(game.GamePath, file.Target)))
                                || obsoleteFiles.Any(file => File.Exists(PathGuard.ResolveUnderRoot(game.GamePath, file.Target)));
                            if (removedPrimary && !binaryRemains)
                            {
                                foreach (ModFileSpec file in mod.Files.Where(file => file.IsConfig))
                                {
                                    string destination = PathGuard.ResolveUnderRoot(game.GamePath, file.Target);
                                    bool existed = File.Exists(destination);
                                    bool removed = transaction.DeleteIfExact(destination, file.Sha256);
                                    if (existed && !removed) result.RetainedFiles++;
                                }
                            }
                            else logger.Write("Companion files are retained while a current, legacy or foreign mod binary remains: " + mod.Name);
                            if (removedPrimary) result.Removed++;
                        }
                    }

                    Report(progress, 90, Localization.T("Verifying installed files …", "Installierte Dateien werden überprüft …"));
                    VerifyGameIdentity(game);
                    VerifyDesiredState(game, selected, plan.ConfigurationProfile);
                    transaction.Commit();
                }

                result.Success = true;
                result.RollbackComplete = true;
                result.Message = BuildSuccessMessage(result);
                logger.Write("SUCCESS: " + result.Message);
                Report(progress, 100, Localization.T("Changes were applied successfully.", "Änderungen wurden erfolgreich angewendet."));
                return result;
            }
            catch (Exception exception)
            {
                result.Success = false;
                result.RollbackComplete = transaction == null
                    || transaction.Committed
                    || (!transaction.RollbackAttempted && !transaction.HasChanges)
                    || (transaction.RollbackAttempted && transaction.RollbackSucceeded);
                result.Message = exception.Message;
                if (!result.RollbackComplete)
                {
                    result.Message += Localization.T(
                        " The automatic rollback was incomplete; check the log and backup.",
                        " Der automatische Rollback war nicht vollständig; bitte das Protokoll und das Backup prüfen.");
                }
                if (logger != null) logger.Write("ERROR: " + exception);
                return result;
            }
            finally
            {
                if (logger != null) logger.Dispose();
                if (gameIdentityHandle != null) gameIdentityHandle.Dispose();
                if (mutexAcquired && gameMutex != null)
                {
                    try { gameMutex.ReleaseMutex(); } catch { }
                }
                if (gameMutex != null) gameMutex.Dispose();
            }
        }

        private string ValidatePlan(ApplyPlan plan)
        {
            if (plan == null || string.IsNullOrWhiteSpace(plan.GamePath))
            {
                throw new InvalidDataException(Localization.T("The installation plan does not contain a game folder.", "Der Installationsplan enthält keinen Spielordner."));
            }
            if (!string.IsNullOrWhiteSpace(plan.Language))
            {
                UiLanguage language;
                if (!Localization.TryParse(plan.Language, out language))
                    throw new InvalidDataException("The installation plan contains an invalid language.");
                Localization.SetLanguage(language);
            }
            if (plan.ConfigurationProfile != null)
            {
                string configurationError;
                if (!ModConfigurationService.TryValidateProfile(catalog, plan.ConfigurationProfile, out configurationError))
                    throw new InvalidDataException(configurationError);
            }
            string gamePath = Path.GetFullPath(plan.GamePath);
            if (!File.Exists(Path.Combine(gamePath, catalog.Game.Executable)))
            {
                throw new FileNotFoundException(catalog.Game.Executable + Localization.T(" was not found in the target folder.", " wurde im Zielordner nicht gefunden."));
            }
            HashSet<string> validIds = new HashSet<string>(catalog.Mods.Select(mod => mod.Id), StringComparer.OrdinalIgnoreCase);
            foreach (string id in plan.SelectedModIds ?? new List<string>())
            {
                if (!validIds.Contains(id)) throw new InvalidDataException(Localization.T("Unknown mod ID in the installation plan: ", "Unbekannte Mod-ID im Installationsplan: ") + id);
            }
            return gamePath;
        }

        private void VerifyGameIdentity(GameInspection game)
        {
            PathGuard.RejectRootReparsePoint(game.GamePath);
            if (!HashUtil.EqualsHash(HashUtil.FileSha256(game.ExePath), game.Sha256))
            {
                throw new IOException(Localization.T(
                    "DS2.exe or the game folder changed during the operation.",
                    "DS2.exe oder der Spielordner wurden während des Vorgangs verändert."));
            }
            if (!allowUnsupportedBuild && Process.GetProcessesByName(Path.GetFileNameWithoutExtension(catalog.Game.Executable)).Length > 0)
            {
                throw new InvalidOperationException(Localization.T(
                    "Death Stranding 2 was started during the operation.",
                    "Death Stranding 2 wurde während des Vorgangs gestartet."));
            }
        }

        private bool DesiredStateNeedsPayload(GameInspection game, HashSet<string> selected, ModConfigurationProfile configuration)
        {
            if (selected.Count > 0 && game.Loader.Status == LoaderStatus.Absent) return true;
            foreach (ModSpec mod in catalog.Mods.Where(item => selected.Contains(item.Id)))
            {
                foreach (ObsoleteFileSpec obsolete in mod.ObsoleteFiles ?? new List<ObsoleteFileSpec>())
                {
                    if (File.Exists(PathGuard.ResolveUnderRoot(game.GamePath, obsolete.Target))) return true;
                }
                foreach (ModFileSpec file in mod.Files)
                {
                    string destination = PathGuard.ResolveUnderRoot(game.GamePath, file.Target);
                    if (!File.Exists(destination)) return true;
                    if (file.IsConfig && configuration != null && ModConfigurationService.HasDefinitionForTarget(catalog, file.Target))
                    {
                        if (!ModConfigurationService.ConfiguredIniMatches(catalog, configuration, file.Target, destination)) return true;
                    }
                    else if (file.IsConfig && ModConfigurationService.RequiresExactSectionKeys(mod.Id))
                    {
                        if (!ModConfigurationService.StableExistingIniMatches(catalog, mod.Id, file.Target, destination)) return true;
                    }
                    else if (!file.IsConfig && !IsExactFile(destination, file.Sha256)) return true;
                }
            }
            return false;
        }

        private static bool IsExactFile(string path, string expectedHash)
        {
            try { return File.Exists(path) && HashUtil.EqualsHash(HashUtil.FileSha256(path), expectedHash); }
            catch { return false; }
        }

        private void VerifyDesiredState(GameInspection game, HashSet<string> selected, ModConfigurationProfile configuration)
        {
            if (selected.Count > 0
                && (game.Loader.Status == LoaderStatus.KnownCompatible || game.Loader.Status == LoaderStatus.Absent))
            {
                LoaderInspection finalLoader = LoaderInspector.Inspect(game.GamePath, catalog.Loader);
                if (finalLoader.Status != LoaderStatus.KnownCompatible)
                {
                    throw new IOException(Localization.T(
                        "The verified ASI Loader is missing or conflicts with another proxy DLL after installation.",
                        "Der geprüfte ASI Loader fehlt oder steht nach der Installation in Konflikt mit einer weiteren Proxy-DLL."));
                }
            }

            foreach (ModSpec mod in catalog.Mods.Where(item => selected.Contains(item.Id)))
            {
                foreach (ObsoleteFileSpec obsolete in mod.ObsoleteFiles ?? new List<ObsoleteFileSpec>())
                {
                    string obsoletePath = PathGuard.ResolveUnderRoot(game.GamePath, obsolete.Target);
                    if (File.Exists(obsoletePath)) throw new IOException(Localization.T(
                        "A legacy conflicting file is still present after the update: ",
                        "Eine ältere Konfliktdatei ist nach dem Update noch vorhanden: ") + obsolete.Target);
                }
                foreach (ModFileSpec file in mod.Files)
                {
                    string destination = PathGuard.ResolveUnderRoot(game.GamePath, file.Target);
                    if (!File.Exists(destination)) throw new IOException(Localization.T(
                        "An installed file is missing during final verification: ",
                        "Installierte Datei fehlt bei der Abschlussprüfung: ") + file.Target);
                    if (file.IsConfig && configuration != null && ModConfigurationService.HasDefinitionForTarget(catalog, file.Target))
                    {
                        if (!ModConfigurationService.ConfiguredIniMatches(catalog, configuration, file.Target, destination))
                            throw new IOException(Localization.T("Configured values could not be verified: ", "Konfigurierte Werte konnten nicht bestätigt werden: ") + file.Target);
                    }
                    else if (file.IsConfig && ModConfigurationService.RequiresExactSectionKeys(mod.Id))
                    {
                        if (!ModConfigurationService.StableExistingIniMatches(catalog, mod.Id, file.Target, destination))
                            throw new IOException(Localization.T("The stable configuration migration could not be verified: ", "Die stabile Konfigurationsmigration konnte nicht bestätigt werden: ") + file.Target);
                    }
                    else if (!file.IsConfig && !IsExactFile(destination, file.Sha256))
                    {
                        throw new IOException(Localization.T(
                            "An installed file changed during final verification: ",
                            "Installierte Datei wurde bei der Abschlussprüfung verändert: ") + file.Target);
                    }
                }
            }
        }

        private static void Report(IProgress<ProgressInfo> progress, int percent, string message)
        {
            try { if (progress != null) progress.Report(new ProgressInfo(percent, message)); } catch { }
        }

        private static string BuildSuccessMessage(ApplyResult result)
        {
            List<string> parts = new List<string>();
            if (result.Installed > 0) parts.Add(result.Installed + (result.Installed == 1
                ? Localization.T(" mod installed", " Mod installiert")
                : Localization.T(" mods installed", " Mods installiert")));
            if (result.Updated > 0) parts.Add(result.Updated + (result.Updated == 1
                ? Localization.T(" mod updated", " Mod aktualisiert")
                : Localization.T(" mods updated", " Mods aktualisiert")));
            if (result.Repaired > 0) parts.Add(result.Repaired + (result.Repaired == 1
                ? Localization.T(" mod repaired", " Mod repariert")
                : Localization.T(" mods repaired", " Mods repariert")));
            if (result.Removed > 0) parts.Add(result.Removed + (result.Removed == 1
                ? Localization.T(" mod removed", " Mod entfernt")
                : Localization.T(" mods removed", " Mods entfernt")));
            if (result.LoaderInstalled) parts.Add(Localization.T("ASI Loader installed", "ASI Loader installiert"));
            if (result.ConfigurationsUpdated > 0) parts.Add(result.ConfigurationsUpdated == 1
                ? Localization.T("1 configuration updated", "1 Konfiguration aktualisiert")
                : Localization.Format("{0} configurations updated", "{0} Konfigurationen aktualisiert", result.ConfigurationsUpdated));
            if (result.RetainedFiles > 0) parts.Add(result.RetainedFiles + (result.RetainedFiles == 1
                ? Localization.T(" modified file retained for safety", " veränderte Datei sicherheitshalber beibehalten")
                : Localization.T(" modified files retained for safety", " veränderte Dateien sicherheitshalber beibehalten")));
            return parts.Count == 0
                ? Localization.T("The installation is already in the requested state.", "Die Installation ist bereits auf dem gewünschten Stand.")
                : string.Join(" · ", parts) + ".";
        }
    }

    internal static class ApplyCoordinator
    {
        public static async Task<ApplyResult> ApplyAsync(Catalog catalog, ApplyPlan plan, IProgress<ProgressInfo> progress)
        {
            ApplyResult result;
            if (SecurityContext.IsAdministrator() || CanWriteDirectory(plan.GamePath))
            {
                result = await Task.Run(() => new InstallEngine(catalog, false).Apply(plan, progress));
                PersistState(plan, result);
                return result;
            }

            try { if (progress != null) progress.Report(new ProgressInfo(2, Localization.T(
                "Requesting administrator approval …", "Administratorfreigabe wird angefordert …"))); } catch { }
            string token = Guid.NewGuid().ToString("N");
            string keyHex = AuthenticationUtil.CreateKeyHex();
            byte[] key = AuthenticationUtil.ParseKey(keyHex);
            string resultPath = GetElevatedResultPath(plan.GamePath, token);
            if (File.Exists(resultPath))
            {
                return new ApplyResult { Success = false, RollbackComplete = true, Message = Localization.T(
                    "The secure UAC result path is unexpectedly already occupied.",
                    "Der sichere UAC-Ergebnispfad ist unerwartet bereits belegt.") };
            }

            string selected = string.Join(",", (plan.SelectedModIds ?? new List<string>()).OrderBy(value => value));
            string language = string.IsNullOrWhiteSpace(plan.Language) ? Localization.CurrentLanguageCode : plan.Language;
            string encodedConfiguration = plan.ConfigurationProfile == null
                ? null
                : Convert.ToBase64String(JsonStore.ToBytes(plan.ConfigurationProfile));
            if (encodedConfiguration != null && encodedConfiguration.Length > 65536)
                throw new InvalidDataException(Localization.T("The mod settings payload is too large.", "Die Mod-Einstellungen sind zu groß."));
            string executable = Process.GetCurrentProcess().MainModule.FileName;
            ProcessStartInfo startInfo = new ProcessStartInfo
            {
                FileName = executable,
                Arguments = "--apply-token " + token
                    + " --auth-key " + keyHex
                    + " --game-path " + QuoteArgument(plan.GamePath)
                    + " --selected-mods " + QuoteArgument(selected)
                    + " --language " + QuoteArgument(language)
                    + (encodedConfiguration == null ? string.Empty : " --config-profile " + QuoteArgument(encodedConfiguration)),
                UseShellExecute = true,
                Verb = "runas",
                WorkingDirectory = AppPaths.BaseDirectory,
                WindowStyle = ProcessWindowStyle.Hidden
            };

            Process child;
            try { child = Process.Start(startInfo); }
            catch (Win32Exception exception)
            {
                if (exception.NativeErrorCode == 1223)
                {
                    return new ApplyResult { Success = false, RollbackComplete = true, Message = Localization.T(
                        "Administrator approval was cancelled.", "Die Administratorfreigabe wurde abgebrochen.") };
                }
                throw;
            }

            await Task.Run(() => child.WaitForExit());
            if (!File.Exists(resultPath))
            {
                return new ApplyResult { Success = false, Message = Localization.T(
                    "The elevated installation process did not return an authenticated result.",
                    "Der erhöhte Installationsprozess hat kein authentifiziertes Ergebnis zurückgegeben.") };
            }
            FileInfo resultInfo = new FileInfo(resultPath);
            if (resultInfo.Length <= 0 || resultInfo.Length > 262144)
            {
                return new ApplyResult { Success = false, Message = Localization.T(
                    "The UAC result has an invalid size.", "Das UAC-Ergebnis hat eine ungültige Größe.") };
            }

            AuthenticatedApplyResult envelope = JsonStore.Read<AuthenticatedApplyResult>(resultPath);
            if (envelope == null || envelope.Payload == null
                || !AuthenticationUtil.FixedTimeEqualsHex(AuthenticationUtil.HmacHex(key, envelope.Payload), envelope.HmacSha256))
            {
                return new ApplyResult { Success = false, Message = Localization.T(
                    "UAC result authentication failed.", "Die Authentifizierung des UAC-Ergebnisses ist fehlgeschlagen.") };
            }
            result = JsonStore.FromBytes<ApplyResult>(envelope.Payload);
            try { File.Delete(resultPath); } catch { }
            PersistState(plan, result);
            return result;
        }

        public static string GetElevatedResultPath(string gamePath, string token)
        {
            if (string.IsNullOrWhiteSpace(token) || token.Length != 32 || token.Any(character => !Uri.IsHexDigit(character)))
            {
                throw new InvalidDataException(Localization.T("Invalid UAC token.", "Ungültiges UAC-Token."));
            }
            string relative = System.IO.Path.Combine(".ds2-mod-suite", "results", "result-" + token.ToLowerInvariant() + ".json");
            return PathGuard.ResolveUnderRoot(Path.GetFullPath(gamePath), relative);
        }

        private static void PersistState(ApplyPlan plan, ApplyResult result)
        {
            if (result == null || !result.Success || SecurityContext.IsAdministrator()) return;
            try
            {
                string gamePath = Path.GetFullPath(plan.GamePath);
                InstalledState previous = JsonStore.ReadOrDefault<InstalledState>(AppPaths.StatePath);
                bool sameInstallation = previous != null && !string.IsNullOrWhiteSpace(previous.GamePath)
                    && string.Equals(Path.GetFullPath(previous.GamePath), gamePath, StringComparison.OrdinalIgnoreCase);
                InstalledState state = new InstalledState
                {
                    SchemaVersion = 1,
                    GamePath = gamePath,
                    ManagedLoader = result.LoaderInstalled || (sameInstallation && previous.ManagedLoader),
                    SelectedModIds = (plan.SelectedModIds ?? new List<string>()).OrderBy(value => value).ToList(),
                    UpdatedUtc = DateTime.UtcNow.ToString("O")
                };
                JsonStore.Write(AppPaths.StatePath, state);
            }
            catch { }
        }

        private static bool CanWriteDirectory(string directory)
        {
            string probe = null;
            try
            {
                probe = Path.Combine(directory, ".ds2aio-write-test-" + Guid.NewGuid().ToString("N") + ".tmp");
                using (FileStream stream = new FileStream(probe, FileMode.CreateNew, FileAccess.Write, FileShare.None))
                {
                    stream.WriteByte(0x44);
                    stream.Flush(true);
                }
                File.Delete(probe);
                return true;
            }
            catch { return false; }
            finally
            {
                try { if (probe != null && File.Exists(probe)) File.Delete(probe); } catch { }
            }
        }

        internal static string QuoteArgument(string value)
        {
            string input = value ?? string.Empty;
            System.Text.StringBuilder result = new System.Text.StringBuilder(input.Length + 2);
            result.Append('"');
            int backslashes = 0;
            foreach (char current in input)
            {
                if (current == '\\')
                {
                    backslashes++;
                    continue;
                }

                if (current == '"')
                {
                    result.Append('\\', backslashes * 2 + 1);
                    result.Append('"');
                }
                else
                {
                    result.Append('\\', backslashes);
                    result.Append(current);
                }
                backslashes = 0;
            }
            result.Append('\\', backslashes * 2);
            result.Append('"');
            return result.ToString();
        }
    }
}
