$d='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Virtualization\Compatibility\Generated\VmcsProjection'
$p=Join-Path $d 'VmcsFieldProjectionSchema.h'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('#include <vector>',"#include <vector>`r`n#include <span>`r`n#include <string>`r`n#include <utility>")
$i=$s.IndexOf("// C# TO C++ CONVERTER TASK: C# 'records'"); $end=$s.IndexOf('class VmcsFieldProjectionSchema final',$i); if($i -lt 0 -or $end -lt 0){throw 'anchor missing'}
$def=@'
struct VmcsFieldProjectionSchemaEntry {
    const VmcsField Field; const VmcsFieldProjectionOwner Owner; const EvidenceVisibilityClass EvidenceClass; const VmcsFieldProjectionAccessPolicy AccessPolicy; const VmcsFieldProjectionMigrationPolicy MigrationPolicy; const bool IsGeneratedAlias;
    VmcsFieldProjectionSchemaEntry(VmcsField field, VmcsFieldProjectionOwner owner, EvidenceVisibilityClass evidence, VmcsFieldProjectionAccessPolicy access, VmcsFieldProjectionMigrationPolicy migration, bool generated) : Field(field), Owner(owner), EvidenceClass(evidence), AccessPolicy(access), MigrationPolicy(migration), IsGeneratedAlias(generated) {}
};
struct VmcsFieldProjectionSchemaArtifact {
    const std::wstring SchemaName; const int SchemaVersion; const unsigned long long SourceHash, ArtifactHash; const int EntryCount; const bool IsCanonical;
    VmcsFieldProjectionSchemaArtifact(std::wstring name, int version, unsigned long long source, unsigned long long artifact, int count, bool canonical) : SchemaName(std::move(name)), SchemaVersion(version), SourceHash(source), ArtifactHash(artifact), EntryCount(count), IsCanonical(canonical) {}
};
'@
$s=$s.Substring(0,$i)+$def+"`r`n`t"+$s.Substring($end)
$s=[regex]::Replace($s,'(?m)^\s*static inline const std::shared_ptr<VmcsFieldProjectionSchemaArtifact> CanonicalArtifact = .*?;\r?\n','')
$s=$s.Replace('static ReadOnlySpan<std::shared_ptr<VmcsFieldProjectionSchemaEntry>> getEntries();','static std::span<const std::shared_ptr<VmcsFieldProjectionSchemaEntry>> getEntries();')
$anchor="`tpublic:`r`n`t`tstatic std::span"; if(-not $s.Contains($anchor)){throw 'artifact anchor missing'}
$artifact='static inline const std::shared_ptr<VmcsFieldProjectionSchemaArtifact> CanonicalArtifact = std::make_shared<VmcsFieldProjectionSchemaArtifact>(L"vmcs-field-projection-schema", CurrentVersion, 0xC71D''41B2''0A6F''9E23ULL, 0xA99E''512C''4F80''73D1ULL, static_cast<int>(EntryTable.size()), true);'
$s=$s.Replace($anchor,"`t`t$artifact`r`n`r`n"+$anchor); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $d 'VmcsFieldProjectionSchema.cpp'; $s=[IO.File]::ReadAllText($p).Replace('ReadOnlySpan<std::shared_ptr<VmcsFieldProjectionSchemaEntry>>','std::span<const std::shared_ptr<VmcsFieldProjectionSchemaEntry>>'); [IO.File]::WriteAllText($p,$s)