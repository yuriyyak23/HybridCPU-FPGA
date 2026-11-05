#pragma once

#include "DomainCheckpointImage.h"
#include "../Format/MigrationDescriptor.h"
#include "../../Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h"
#include "../Validation/YAKSys_Hybrid_CPU.Core.MigrationValidationPolicy.h"
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class DomainCheckpointImage final : public std::enable_shared_from_this<DomainCheckpointImage>
	{
	private:
		static inline const std::shared_ptr<DomainCheckpointImage> Empty = std::make_shared<DomainCheckpointImage>();
		const DomainCheckpointAuthority Authority;
		const unsigned long long CheckpointEpoch;
		const unsigned long long PayloadMask;
		const unsigned long long EvidenceMask;
		const bool ContainsCompatibilityProjectionMetadata;
		static inline const std::vector<MigrationPayloadClass> PayloadClasses = {MigrationPayloadClass::GuestArchitecturalState, MigrationPayloadClass::DomainDescriptorState, MigrationPayloadClass::CompatibilityProjectionMetadata, MigrationPayloadClass::HostOwnedRuntimeEvidence, MigrationPayloadClass::SchedulerEvidence, MigrationPayloadClass::BackendBindingEvidence, MigrationPayloadClass::NativeTokenEvidence};

	public:
		DomainCheckpointImage();

		DomainCheckpointImage(DomainCheckpointAuthority authority, unsigned long long checkpointEpoch, unsigned long long payloadMask, unsigned long long evidenceMask, bool containsCompatibilityProjectionMetadata);

		static std::shared_ptr<DomainCheckpointImage> getEmpty();

		DomainCheckpointAuthority getAuthority() const;

		unsigned long long getCheckpointEpoch() const;

		unsigned long long getPayloadMask() const;

		unsigned long long getEvidenceMask() const;

		bool getContainsCompatibilityProjectionMetadata() const;

		bool isDomainAuthoritative() const;

		bool isEmpty() const;

		bool getContainsHostOwnedEvidence() const;

		bool IncludesPayload(MigrationPayloadClass payloadClass) const;

		bool IncludesEvidence(EvidenceVisibilityClass evidenceClass) const;

		std::shared_ptr<MigrationValidationResult> ValidateRestore(std::shared_ptr<MigrationValidationPolicy> policy, EvidenceRestorePolicy restorePolicy);

		std::shared_ptr<DomainCheckpointImage> WithPayload(MigrationPayloadClass payloadClass);

	private:
		static unsigned long long ToMask(MigrationPayloadClass payloadClass);

		static unsigned long long ToMask(EvidenceVisibilityClass evidenceClass);

		static EvidenceVisibilityClass ToEvidenceClass(MigrationPayloadClass payloadClass);

		static std::vector<MigrationPayloadClass> getPayloadClasses();
	};
}
