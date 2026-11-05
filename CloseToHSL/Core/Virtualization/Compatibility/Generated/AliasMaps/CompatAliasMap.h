#pragma once
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class CompatAliasSourceKind : unsigned char
	{
		Opcode = 0,
		Csr = 1,
		VmcsField = 2,
		Completion = 3,
		FunctionLeaf = 4
	};

	enum class CompatAliasTargetKind : unsigned char
	{
		DomainRuntimeOperation = 0,
		DescriptorField = 1,
		CapabilityGrant = 2,
		CompletionRecord = 3,
		CompatibilityProjection = 4
	};

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