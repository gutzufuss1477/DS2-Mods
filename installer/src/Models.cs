using System;
using System.Collections.Generic;
using System.Runtime.Serialization;

namespace DS2ModSuite
{
    [DataContract]
    internal sealed class Catalog
    {
        [DataMember(Name = "schemaVersion")]
        public int SchemaVersion { get; set; }

        [DataMember(Name = "suiteVersion")]
        public string SuiteVersion { get; set; }

        [DataMember(Name = "game")]
        public GameSpec Game { get; set; }

        [DataMember(Name = "loader")]
        public LoaderSpec Loader { get; set; }

        [DataMember(Name = "mods")]
        public List<ModSpec> Mods { get; set; }
    }

    [DataContract]
    internal sealed class GameSpec
    {
        [DataMember(Name = "name")]
        public string Name { get; set; }

        [DataMember(Name = "executable")]
        public string Executable { get; set; }

        [DataMember(Name = "steamFolder")]
        public string SteamFolder { get; set; }

        [DataMember(Name = "supportedVersions")]
        public List<GameBuildSpec> SupportedVersions { get; set; }
    }

    [DataContract]
    internal sealed class GameBuildSpec
    {
        [DataMember(Name = "fileVersion")]
        public string FileVersion { get; set; }

        [DataMember(Name = "sha256")]
        public string Sha256 { get; set; }
    }

    [DataContract]
    internal sealed class LoaderSpec
    {
        [DataMember(Name = "name")]
        public string Name { get; set; }

        [DataMember(Name = "version")]
        public string Version { get; set; }

        [DataMember(Name = "fileName")]
        public string FileName { get; set; }

        [DataMember(Name = "payload")]
        public string Payload { get; set; }

        [DataMember(Name = "sha256")]
        public string Sha256 { get; set; }

        [DataMember(Name = "knownCompatibleHashes")]
        public List<string> KnownCompatibleHashes { get; set; }

        [DataMember(Name = "proxyFileNames")]
        public List<string> ProxyFileNames { get; set; }
    }

    [DataContract]
    internal sealed class ModSpec
    {
        [DataMember(Name = "id")]
        public string Id { get; set; }

        [DataMember(Name = "name")]
        public string Name { get; set; }

        [DataMember(Name = "version")]
        public string Version { get; set; }

        [DataMember(Name = "category")]
        public string Category { get; set; }

        [DataMember(Name = "categoryDe", EmitDefaultValue = false)]
        public string CategoryDe { get; set; }

        [DataMember(Name = "description")]
        public string Description { get; set; }

        [DataMember(Name = "descriptionDe", EmitDefaultValue = false)]
        public string DescriptionDe { get; set; }

        public string LocalizedCategory
        {
            get { return Localization.T(Category, CategoryDe); }
        }

        public string LocalizedDescription
        {
            get { return Localization.T(Description, DescriptionDe); }
        }

        [DataMember(Name = "files")]
        public List<ModFileSpec> Files { get; set; }

        [DataMember(Name = "obsoleteFiles", EmitDefaultValue = false)]
        public List<ObsoleteFileSpec> ObsoleteFiles { get; set; }
    }

    [DataContract]
    internal sealed class ObsoleteFileSpec
    {
        [DataMember(Name = "target")]
        public string Target { get; set; }

        [DataMember(Name = "sha256")]
        public string Sha256 { get; set; }
    }

    [DataContract]
    internal sealed class ModFileSpec
    {
        [DataMember(Name = "payload")]
        public string Payload { get; set; }

        [DataMember(Name = "target")]
        public string Target { get; set; }

        [DataMember(Name = "sha256")]
        public string Sha256 { get; set; }

        [DataMember(Name = "kind")]
        public string Kind { get; set; }

        public bool IsConfig
        {
            get { return string.Equals(Kind, "config", StringComparison.OrdinalIgnoreCase); }
        }
    }

    internal enum LoaderStatus
    {
        Absent,
        KnownCompatible,
        UnknownProxy,
        InvalidPayload
    }

    internal sealed class LoaderInspection
    {
        public LoaderStatus Status { get; set; }
        public string DisplayText { get; set; }
        public string DetailText { get; set; }
        public string FilePath { get; set; }
        public string FileVersion { get; set; }
        public string Hash { get; set; }
        public int OtherProxyCount { get; set; }
        public List<string> CandidateFileNames { get; set; }
    }

    internal sealed class GameInspection
    {
        public string GamePath { get; set; }
        public string ExePath { get; set; }
        public string FileVersion { get; set; }
        public string Sha256 { get; set; }
        public bool Found { get; set; }
        public bool Supported { get; set; }
        public bool IsRunning { get; set; }
        public string Error { get; set; }
        public LoaderInspection Loader { get; set; }
        public int ForeignAsiCount { get; set; }
    }

    internal sealed class ModRuntimeState
    {
        public ModSpec Spec { get; set; }
        public bool IsInstalled { get; set; }
        public bool HasTargetBinary { get; set; }
        public bool HasModifiedBinary { get; set; }
        public bool HasObsoleteBinary { get; set; }
        public bool HasUnknownObsoleteBinary { get; set; }
        public string ConflictFileNames { get; set; }
        public bool NeedsRepair { get; set; }
        public bool DesiredEnabled { get; set; }
        public string StatusText { get; set; }
        public string StatusTone { get; set; }
    }

    [DataContract]
    internal sealed class ApplyPlan
    {
        [DataMember(Name = "gamePath")]
        public string GamePath { get; set; }

        [DataMember(Name = "selectedModIds")]
        public List<string> SelectedModIds { get; set; }

        [DataMember(Name = "configurationProfile", EmitDefaultValue = false)]
        public ModConfigurationProfile ConfigurationProfile { get; set; }

        [DataMember(Name = "language", EmitDefaultValue = false)]
        public string Language { get; set; }
    }

    [DataContract]
    internal sealed class ApplyResult
    {
        [DataMember(Name = "success")]
        public bool Success { get; set; }

        [DataMember(Name = "message")]
        public string Message { get; set; }

        [DataMember(Name = "logPath")]
        public string LogPath { get; set; }

        [DataMember(Name = "installed")]
        public int Installed { get; set; }

        [DataMember(Name = "updated")]
        public int Updated { get; set; }

        [DataMember(Name = "removed")]
        public int Removed { get; set; }

        [DataMember(Name = "repaired")]
        public int Repaired { get; set; }

        [DataMember(Name = "loaderInstalled")]
        public bool LoaderInstalled { get; set; }

        [DataMember(Name = "retainedFiles")]
        public int RetainedFiles { get; set; }

        [DataMember(Name = "configurationsUpdated")]
        public int ConfigurationsUpdated { get; set; }

        [DataMember(Name = "rollbackComplete")]
        public bool RollbackComplete { get; set; }
    }

    [DataContract]
    internal sealed class AuthenticatedApplyResult
    {
        [DataMember(Name = "payload")]
        public byte[] Payload { get; set; }

        [DataMember(Name = "hmacSha256")]
        public string HmacSha256 { get; set; }
    }

    [DataContract]
    internal sealed class InstalledState
    {
        [DataMember(Name = "schemaVersion")]
        public int SchemaVersion { get; set; }

        [DataMember(Name = "gamePath")]
        public string GamePath { get; set; }

        [DataMember(Name = "managedLoader")]
        public bool ManagedLoader { get; set; }

        [DataMember(Name = "selectedModIds")]
        public List<string> SelectedModIds { get; set; }

        [DataMember(Name = "updatedUtc")]
        public string UpdatedUtc { get; set; }
    }

    internal sealed class ProgressInfo
    {
        public int Percent { get; set; }
        public string Message { get; set; }

        public ProgressInfo(int percent, string message)
        {
            Percent = percent;
            Message = message;
        }
    }
}
