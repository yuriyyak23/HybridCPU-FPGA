#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class CompatSpecArtifactKind : unsigned char
	{
		FrozenOpcodeAliases = 0,
		FrozenCsrAliases = 1,
		VmcsFieldAliases = 2,
		CapabilityProjection = 3,
		CompletionProjection = 4,
		CompatAliasMap = 5
	};

	enum class CompatSpecArtifactLineageKind : unsigned char
	{
		FrozenAbiVocabulary = 0,
		ExecutableGeneratedSource = 1,
		ProjectionContractOnly = 2
	};

	struct CompatSpecArtifact final
	{
		const CompatSpecArtifactKind Kind;
		const std::wstring Name;
		const std::wstring LineageSource;
		const bool IsGenerated;
		const bool IsFrozenAbi;
		const CompatSpecArtifactLineageKind LineageKind;

		CompatSpecArtifact(CompatSpecArtifactKind kind, std::wstring name, std::wstring lineageSource,
			bool isGenerated, bool isFrozenAbi, CompatSpecArtifactLineageKind lineageKind)
			: Kind(kind), Name(std::move(name)), LineageSource(std::move(lineageSource)),
			  IsGenerated(isGenerated), IsFrozenAbi(isFrozenAbi), LineageKind(lineageKind) {}
	};

	class CompatSpecArtifactSet final : public std::enable_shared_from_this<CompatSpecArtifactSet>
	{
	private:
		static inline const std::vector<std::shared_ptr<CompatSpecArtifact>> ArtifactTable = {
			std::make_shared<CompatSpecArtifact>(CompatSpecArtifactKind::FrozenOpcodeAliases, L"VmxOpcodeAliasSet", L"FrozenOpcodeAbiVocabulary", false, true, CompatSpecArtifactLineageKind::FrozenAbiVocabulary),
			std::make_shared<CompatSpecArtifact>(CompatSpecArtifactKind::FrozenCsrAliases, L"VmxCsrAliasSet", L"FrozenCsrAbiVocabulary", false, true, CompatSpecArtifactLineageKind::FrozenAbiVocabulary),
			std::make_shared<CompatSpecArtifact>(CompatSpecArtifactKind::VmcsFieldAliases, L"VmcsFieldAliasSet", L"VmcsFieldProjectionSchema", true, true, CompatSpecArtifactLineageKind::ExecutableGeneratedSource),
			std::make_shared<CompatSpecArtifact>(CompatSpecArtifactKind::CapabilityProjection, L"VmxCapsProjection", L"CapabilityDescriptorSetSchema", true, true, CompatSpecArtifactLineageKind::ExecutableGeneratedSource),
			std::make_shared<CompatSpecArtifact>(CompatSpecArtifactKind::CompletionProjection, L"CompletionProjectionService", L"ProjectionContractOnly", false, true, CompatSpecArtifactLineageKind::ProjectionContractOnly),
			std::make_shared<CompatSpecArtifact>(CompatSpecArtifactKind::CompatAliasMap, L"CompatAliasMap", L"CompatAliasMap", true, true, CompatSpecArtifactLineageKind::ExecutableGeneratedSource)
		};

	public:
		static const std::vector<std::shared_ptr<CompatSpecArtifact>>& getArtifacts();
		bool Contains(CompatSpecArtifactKind kind);
		bool TryGetArtifact(CompatSpecArtifactKind kind, std::shared_ptr<CompatSpecArtifact>& artifact);
		bool HasExecutableLineage(CompatSpecArtifactKind kind);
		bool RequiresGeneratedParity(CompatSpecArtifactKind kind);
		bool RequiresAbiFreeze(CompatSpecArtifactKind kind);
	};
}