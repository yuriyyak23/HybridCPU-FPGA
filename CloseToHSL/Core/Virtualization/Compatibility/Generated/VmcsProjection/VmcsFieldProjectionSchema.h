#pragma once

#include "../../../../Runtime/Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h"
#include "../../FrozenAbi/VmcsFieldAliases/VmcsField.h"
#include <vector>
#include <span>
#include <string>
#include <utility>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VmcsFieldProjectionOwner : unsigned char
	{
		ExecutionDomainDescriptor = 0,
		MemoryDomainDescriptor = 1,
		CompletionRecord = 2,
		CompatibilityControlDescriptor = 3
	};

	enum class VmcsFieldProjectionAccessPolicy : unsigned char
	{
		Denied = 0,
		ReadOnly = 1,
		ReadWrite = 2
	};

	enum class VmcsFieldProjectionMigrationPolicy : unsigned char
	{
		None = 0,
		DescriptorOwned = 1,
		RecomputedCompletion = 2,
		ProjectionOnly = 3
	};

struct VmcsFieldProjectionSchemaEntry {
    const VmcsField Field; const VmcsFieldProjectionOwner Owner; const EvidenceVisibilityClass EvidenceClass; const VmcsFieldProjectionAccessPolicy AccessPolicy; const VmcsFieldProjectionMigrationPolicy MigrationPolicy; const bool IsGeneratedAlias;
    VmcsFieldProjectionSchemaEntry(VmcsField field, VmcsFieldProjectionOwner owner, EvidenceVisibilityClass evidence, VmcsFieldProjectionAccessPolicy access, VmcsFieldProjectionMigrationPolicy migration, bool generated) : Field(field), Owner(owner), EvidenceClass(evidence), AccessPolicy(access), MigrationPolicy(migration), IsGeneratedAlias(generated) {}
};
struct VmcsFieldProjectionSchemaArtifact {
    const std::wstring SchemaName; const int SchemaVersion; const unsigned long long SourceHash, ArtifactHash; const int EntryCount; const bool IsCanonical;
    VmcsFieldProjectionSchemaArtifact(std::wstring name, int version, unsigned long long source, unsigned long long artifact, int count, bool canonical) : SchemaName(std::move(name)), SchemaVersion(version), SourceHash(source), ArtifactHash(artifact), EntryCount(count), IsCanonical(canonical) {}
};
	class VmcsFieldProjectionSchema final
	{
	private:

	public:
		static constexpr int CurrentVersion = 1;

	private:
		static inline const std::vector<std::shared_ptr<VmcsFieldProjectionSchemaEntry>> EntryTable = {std::make_shared<VmcsFieldProjectionSchemaEntry>(VmcsField::GuestPc, VmcsFieldProjectionOwner::ExecutionDomainDescriptor, EvidenceVisibilityClass::GuestArchitecturalState, VmcsFieldProjectionAccessPolicy::ReadOnly, VmcsFieldProjectionMigrationPolicy::DescriptorOwned, true), std::make_shared<VmcsFieldProjectionSchemaEntry>(VmcsField::GuestSp, VmcsFieldProjectionOwner::ExecutionDomainDescriptor, EvidenceVisibilityClass::GuestArchitecturalState, VmcsFieldProjectionAccessPolicy::ReadOnly, VmcsFieldProjectionMigrationPolicy::DescriptorOwned, true), std::make_shared<VmcsFieldProjectionSchemaEntry>(VmcsField::GuestFlags, VmcsFieldProjectionOwner::ExecutionDomainDescriptor, EvidenceVisibilityClass::GuestArchitecturalState, VmcsFieldProjectionAccessPolicy::ReadOnly, VmcsFieldProjectionMigrationPolicy::DescriptorOwned, true), std::make_shared<VmcsFieldProjectionSchemaEntry>(VmcsField::GuestCr0, VmcsFieldProjectionOwner::ExecutionDomainDescriptor, EvidenceVisibilityClass::GuestArchitecturalState, VmcsFieldProjectionAccessPolicy::ReadOnly, VmcsFieldProjectionMigrationPolicy::DescriptorOwned, true), std::make_shared<VmcsFieldProjectionSchemaEntry>(VmcsField::GuestCr3, VmcsFieldProjectionOwner::MemoryDomainDescriptor, EvidenceVisibilityClass::GuestArchitecturalState, VmcsFieldProjectionAccessPolicy::ReadOnly, VmcsFieldProjectionMigrationPolicy::DescriptorOwned, true), std::make_shared<VmcsFieldProjectionSchemaEntry>(VmcsField::GuestCr4, VmcsFieldProjectionOwner::ExecutionDomainDescriptor, EvidenceVisibilityClass::GuestArchitecturalState, VmcsFieldProjectionAccessPolicy::ReadOnly, VmcsFieldProjectionMigrationPolicy::DescriptorOwned, true), std::make_shared<VmcsFieldProjectionSchemaEntry>(VmcsField::HostPc, VmcsFieldProjectionOwner::ExecutionDomainDescriptor, EvidenceVisibilityClass::CompatibilityAlias, VmcsFieldProjectionAccessPolicy::ReadOnly, VmcsFieldProjectionMigrationPolicy::ProjectionOnly, true), std::make_shared<VmcsFieldProjectionSchemaEntry>(VmcsField::HostSp, VmcsFieldProjectionOwner::ExecutionDomainDescriptor, EvidenceVisibilityClass::CompatibilityAlias, VmcsFieldProjectionAccessPolicy::ReadOnly, VmcsFieldProjectionMigrationPolicy::ProjectionOnly, true), std::make_shared<VmcsFieldProjectionSchemaEntry>(VmcsField::HostFlags, VmcsFieldProjectionOwner::ExecutionDomainDescriptor, EvidenceVisibilityClass::CompatibilityAlias, VmcsFieldProjectionAccessPolicy::ReadOnly, VmcsFieldProjectionMigrationPolicy::ProjectionOnly, true), std::make_shared<VmcsFieldProjectionSchemaEntry>(VmcsField::HostCr0, VmcsFieldProjectionOwner::ExecutionDomainDescriptor, EvidenceVisibilityClass::CompatibilityAlias, VmcsFieldProjectionAccessPolicy::ReadOnly, VmcsFieldProjectionMigrationPolicy::ProjectionOnly, true), std::make_shared<VmcsFieldProjectionSchemaEntry>(VmcsField::HostCr3, VmcsFieldProjectionOwner::MemoryDomainDescriptor, EvidenceVisibilityClass::CompatibilityAlias, VmcsFieldProjectionAccessPolicy::ReadOnly, VmcsFieldProjectionMigrationPolicy::ProjectionOnly, true), std::make_shared<VmcsFieldProjectionSchemaEntry>(VmcsField::PinBasedControls, VmcsFieldProjectionOwner::CompatibilityControlDescriptor, EvidenceVisibilityClass::CompatibilityAlias, VmcsFieldProjectionAccessPolicy::ReadOnly, VmcsFieldProjectionMigrationPolicy::ProjectionOnly, true), std::make_shared<VmcsFieldProjectionSchemaEntry>(VmcsField::ProcBasedControls, VmcsFieldProjectionOwner::CompatibilityControlDescriptor, EvidenceVisibilityClass::CompatibilityAlias, VmcsFieldProjectionAccessPolicy::ReadOnly, VmcsFieldProjectionMigrationPolicy::ProjectionOnly, true), std::make_shared<VmcsFieldProjectionSchemaEntry>(VmcsField::ExitControls, VmcsFieldProjectionOwner::CompatibilityControlDescriptor, EvidenceVisibilityClass::CompatibilityAlias, VmcsFieldProjectionAccessPolicy::ReadOnly, VmcsFieldProjectionMigrationPolicy::ProjectionOnly, true), std::make_shared<VmcsFieldProjectionSchemaEntry>(VmcsField::EntryControls, VmcsFieldProjectionOwner::CompatibilityControlDescriptor, EvidenceVisibilityClass::CompatibilityAlias, VmcsFieldProjectionAccessPolicy::ReadOnly, VmcsFieldProjectionMigrationPolicy::ProjectionOnly, true), std::make_shared<VmcsFieldProjectionSchemaEntry>(VmcsField::EptPointer, VmcsFieldProjectionOwner::MemoryDomainDescriptor, EvidenceVisibilityClass::CompatibilityAlias, VmcsFieldProjectionAccessPolicy::ReadOnly, VmcsFieldProjectionMigrationPolicy::DescriptorOwned, true), std::make_shared<VmcsFieldProjectionSchemaEntry>(VmcsField::Vpid, VmcsFieldProjectionOwner::MemoryDomainDescriptor, EvidenceVisibilityClass::CompatibilityAlias, VmcsFieldProjectionAccessPolicy::ReadOnly, VmcsFieldProjectionMigrationPolicy::DescriptorOwned, true), std::make_shared<VmcsFieldProjectionSchemaEntry>(VmcsField::SecondaryProcControls, VmcsFieldProjectionOwner::CompatibilityControlDescriptor, EvidenceVisibilityClass::CompatibilityAlias, VmcsFieldProjectionAccessPolicy::ReadOnly, VmcsFieldProjectionMigrationPolicy::ProjectionOnly, true), std::make_shared<VmcsFieldProjectionSchemaEntry>(VmcsField::Cr3TargetCount, VmcsFieldProjectionOwner::MemoryDomainDescriptor, EvidenceVisibilityClass::CompatibilityAlias, VmcsFieldProjectionAccessPolicy::ReadOnly, VmcsFieldProjectionMigrationPolicy::DescriptorOwned, true), std::make_shared<VmcsFieldProjectionSchemaEntry>(VmcsField::ExitReason, VmcsFieldProjectionOwner::CompletionRecord, EvidenceVisibilityClass::CompatibilityAlias, VmcsFieldProjectionAccessPolicy::ReadOnly, VmcsFieldProjectionMigrationPolicy::RecomputedCompletion, true), std::make_shared<VmcsFieldProjectionSchemaEntry>(VmcsField::ExitQualification, VmcsFieldProjectionOwner::CompletionRecord, EvidenceVisibilityClass::CompatibilityAlias, VmcsFieldProjectionAccessPolicy::ReadOnly, VmcsFieldProjectionMigrationPolicy::RecomputedCompletion, true), std::make_shared<VmcsFieldProjectionSchemaEntry>(VmcsField::GuestPhysicalAddress, VmcsFieldProjectionOwner::CompletionRecord, EvidenceVisibilityClass::CompatibilityAlias, VmcsFieldProjectionAccessPolicy::ReadOnly, VmcsFieldProjectionMigrationPolicy::RecomputedCompletion, true), std::make_shared<VmcsFieldProjectionSchemaEntry>(VmcsField::EptViolationQualification, VmcsFieldProjectionOwner::CompletionRecord, EvidenceVisibilityClass::CompatibilityAlias, VmcsFieldProjectionAccessPolicy::ReadOnly, VmcsFieldProjectionMigrationPolicy::RecomputedCompletion, true)};

		static inline const std::shared_ptr<VmcsFieldProjectionSchemaArtifact> CanonicalArtifact = std::make_shared<VmcsFieldProjectionSchemaArtifact>(L"vmcs-field-projection-schema", CurrentVersion, 0xC71D'41B2'0A6F'9E23ULL, 0xA99E'512C'4F80'73D1ULL, static_cast<int>(EntryTable.size()), true);

	public:
		static std::span<const std::shared_ptr<VmcsFieldProjectionSchemaEntry>> getEntries();

		static std::shared_ptr<VmcsFieldProjectionSchemaArtifact> getCanonicalArtifact();

		static bool TryGet(VmcsField field, std::shared_ptr<VmcsFieldProjectionSchemaEntry> &entry);

		static bool CanRead(std::shared_ptr<VmcsFieldProjectionSchemaEntry> entry);

		static bool CanWrite(std::shared_ptr<VmcsFieldProjectionSchemaEntry> entry);
	};

}
