#pragma once

#include "CompatAliasMap.h"
#include <string>
#include <vector>
#include <span>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class CompatAliasMap final : public std::enable_shared_from_this<CompatAliasMap>
	{
	private:

		static constexpr unsigned long long CanonicalSchemaHash = 0x8B55'62D0'F19A'4C31ULL;
		static constexpr unsigned long long GeneratedArtifactHash = 0xB0A7'11A5'95E4'2227ULL;

		static inline const std::vector<std::shared_ptr<CompatAliasMapEntry>> EntryTable = {std::make_shared<CompatAliasMapEntry>(CompatAliasSourceKind::Opcode, L"VMXON", CompatAliasTargetKind::DomainRuntimeOperation, L"ActivateCompatibilityFrontend", true), std::make_shared<CompatAliasMapEntry>(CompatAliasSourceKind::Opcode, L"VMXOFF", CompatAliasTargetKind::DomainRuntimeOperation, L"DeactivateCompatibilityFrontend", true), std::make_shared<CompatAliasMapEntry>(CompatAliasSourceKind::Opcode, L"VMREAD", CompatAliasTargetKind::CompatibilityProjection, L"VmcsFieldAliasProjection.Read", true), std::make_shared<CompatAliasMapEntry>(CompatAliasSourceKind::Opcode, L"VMWRITE", CompatAliasTargetKind::CompatibilityProjection, L"VmcsFieldAliasProjection.Write", true), std::make_shared<CompatAliasMapEntry>(CompatAliasSourceKind::Opcode, L"VMCALL", CompatAliasTargetKind::CompatibilityProjection, L"VmxTrapProjectionMapper.Project", true), std::make_shared<CompatAliasMapEntry>(CompatAliasSourceKind::Csr, L"VmxCaps", CompatAliasTargetKind::CapabilityGrant, L"CapabilityDescriptorSet", true), std::make_shared<CompatAliasMapEntry>(CompatAliasSourceKind::Csr, L"VmxExitReason", CompatAliasTargetKind::CompletionRecord, L"CompletionRecord.ExitReason", true), std::make_shared<CompatAliasMapEntry>(CompatAliasSourceKind::Csr, L"VmxExitQual", CompatAliasTargetKind::CompletionRecord, L"CompletionRecord.ExitQualification", true), std::make_shared<CompatAliasMapEntry>(CompatAliasSourceKind::VmcsField, L"VmcsField", CompatAliasTargetKind::DescriptorField, L"GeneratedVmcsProjection", true), std::make_shared<CompatAliasMapEntry>(CompatAliasSourceKind::Completion, L"VmxCompletion", CompatAliasTargetKind::CompletionRecord, L"CompletionProjectionService", true), std::make_shared<CompatAliasMapEntry>(CompatAliasSourceKind::FunctionLeaf, L"VmxFunctionLeaf", CompatAliasTargetKind::CapabilityGrant, L"CapabilityNegotiationService", true)};

		static inline const std::shared_ptr<CompatAliasSchemaArtifact> CanonicalSchema = std::make_shared<CompatAliasSchemaArtifact>(L"compat-alias-schema", 1, CanonicalSchemaHash, GeneratedArtifactHash, static_cast<int>(EntryTable.size()), true);

	public:
		static std::span<const std::shared_ptr<CompatAliasMapEntry>> getEntries();

		static std::shared_ptr<CompatAliasSchemaArtifact> getCanonicalSchema();

		bool MatchesCanonicalSchema(std::shared_ptr<CompatAliasSchemaArtifact> artifact);

		bool ContainsSource(CompatAliasSourceKind sourceKind, const std::wstring &sourceName);

		bool TryGetEntry(CompatAliasSourceKind sourceKind, const std::wstring &sourceName, std::shared_ptr<CompatAliasMapEntry> &entry);

		bool IsFrozenAbiSource(CompatAliasSourceKind sourceKind, const std::wstring &sourceName);


	};
}
