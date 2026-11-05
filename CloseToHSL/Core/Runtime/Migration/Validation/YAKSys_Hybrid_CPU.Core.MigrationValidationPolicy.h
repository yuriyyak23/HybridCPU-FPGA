#pragma once

#include "../Format/YAKSys_Hybrid_CPU.Core.MigrationDescriptor.h"
#include "../../Domains/Descriptors/EvidencePolicy/YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"
#include "../../Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h"
#include "../Format/MigrationDescriptor.h"
#include "MigrationValidationPolicy.h"
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	class MigrationValidationPolicy final : public std::enable_shared_from_this<MigrationValidationPolicy>
	{
	private:
		static inline const std::shared_ptr<MigrationValidationPolicy> FailClosed = std::make_shared<MigrationValidationPolicy>();
		const std::shared_ptr<MigrationDescriptor> Migration;
		const std::shared_ptr<EvidencePolicyDescriptor> Evidence;
		const bool RejectCompatibilityProjectionMetadata;
		const bool RequireGuestStatePreservePolicy;

	public:
		MigrationValidationPolicy();

		MigrationValidationPolicy(std::shared_ptr<MigrationDescriptor> migration, std::shared_ptr<EvidencePolicyDescriptor> evidence, bool rejectCompatibilityProjectionMetadata, bool requireGuestStatePreservePolicy);

		static std::shared_ptr<MigrationValidationPolicy> getFailClosed();

		std::shared_ptr<MigrationDescriptor> getMigration() const;

		std::shared_ptr<EvidencePolicyDescriptor> getEvidence() const;

		bool getRejectCompatibilityProjectionMetadata() const;

		bool getRequireGuestStatePreservePolicy() const;

		std::shared_ptr<MigrationValidationResult> ValidateImport(MigrationPayloadClass payloadClass, EvidenceVisibilityClass evidenceClass, EvidenceRestorePolicy restorePolicy);
	};
}

