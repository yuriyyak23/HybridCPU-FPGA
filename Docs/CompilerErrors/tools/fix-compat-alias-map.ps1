$base='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Virtualization\Compatibility\Generated\AliasMaps'
$p=Join-Path $base 'CompatAliasMap.h'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('#pragma once', "#pragma once`r`n#include <string>`r`n#include <utility>")
$i=$s.IndexOf("// C# TO C++ CONVERTER TASK: C# 'records' are not converted"); if($i -lt 0){throw 'record anchor missing'}
$s=$s.Substring(0,$i)+@'
struct CompatAliasMapEntry {
    CompatAliasSourceKind SourceKind{}; std::wstring SourceName; CompatAliasTargetKind TargetKind{}; std::wstring TargetName; bool IsFrozenAbi{};
    CompatAliasMapEntry() = default;
    CompatAliasMapEntry(CompatAliasSourceKind s, std::wstring n, CompatAliasTargetKind t, std::wstring d, bool f) : SourceKind(s), SourceName(std::move(n)), TargetKind(t), TargetName(std::move(d)), IsFrozenAbi(f) {}
};
struct CompatAliasSchemaArtifact {
    std::wstring SchemaName; int SchemaVersion{}; unsigned long long SourceHash{}, ArtifactHash{}; int EntryCount{}; bool IsCanonical{};
    CompatAliasSchemaArtifact(std::wstring n, int v, unsigned long long s, unsigned long long a, int c, bool i) : SchemaName(std::move(n)), SchemaVersion(v), SourceHash(s), ArtifactHash(a), EntryCount(c), IsCanonical(i) {}
};
}
'@
[IO.File]::WriteAllText($p,$s)
$p=Join-Path $base 'YAKSys_Hybrid_CPU.Core.CompatAliasMap.h'; $s=[IO.File]::ReadAllText($p)
$s=[regex]::Replace($s,'(?m)^#include "\.\./.*CompatAliasMap\.expected\.h"\r?\n','')
$s=$s.Replace('#include <vector>',"#include <vector>`r`n#include <span>")
$s=[regex]::Replace($s,'(?m)^\s*static inline const std::shared_ptr<CompatAliasSchemaArtifact> CanonicalSchema = .*?;\r?\n','')
$second=$s.IndexOf("`tprivate:",$s.IndexOf("`tpublic:")); if($second -lt 0){throw 'duplicate header block missing'}
$s=$s.Substring(0,$second)+"`t};`r`n}`r`n"
$s=$s.Replace('static ReadOnlySpan<std::shared_ptr<CompatAliasMapEntry>> getEntries();','static std::span<const std::shared_ptr<CompatAliasMapEntry>> getEntries();')
$line='static inline const std::shared_ptr<CompatAliasSchemaArtifact> CanonicalSchema = std::make_shared<CompatAliasSchemaArtifact>(L"compat-alias-schema", 1, CanonicalSchemaHash, GeneratedArtifactHash, static_cast<int>(EntryTable.size()), true);'
$s=$s.Replace("`tpublic:`r`n", "`t`t$line`r`n`r`n`tpublic:`r`n")
[IO.File]::WriteAllText($p,$s)
$p=Join-Path $base 'YAKSys_Hybrid_CPU.Core.CompatAliasMap.cpp'; $s=[IO.File]::ReadAllText($p)
$needle='ReadOnlySpan<std::shared_ptr<CompatAliasMapEntry>> CompatAliasMap::getEntries()'; $first=$s.IndexOf($needle); $second=$s.IndexOf($needle,$first+1); if($second -lt 0){throw 'duplicate cpp block missing'}
$s=$s.Substring(0,$second).TrimEnd()+"`r`n}`r`n"
$s=$s.Replace($needle,'std::span<const std::shared_ptr<CompatAliasMapEntry>> CompatAliasMap::getEntries()')
$s=$s.Replace('return TryGetEntry(sourceKind, sourceName, _);',"std::shared_ptr<CompatAliasMapEntry> ignored;`r`n`t`treturn TryGetEntry(sourceKind, sourceName, ignored);")
$s=$s.Replace('std::wstring::Equals(candidate->SourceName, sourceName, StringComparison::Ordinal)','candidate->SourceName == sourceName')
[IO.File]::WriteAllText($p,$s)