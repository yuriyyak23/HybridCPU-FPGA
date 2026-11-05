#pragma once

#include "../../../Descriptors/EvidencePolicy/YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"
#include "SecureEvidencePolicy.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureEvidencePolicy final : public std::enable_shared_from_this<SecureEvidencePolicy>
	{
	private:
		static inline const std::shared_ptr<SecureEvidencePolicy> FailClosed = std::make_shared<SecureEvidencePolicy>();
		const bool AllowGuestVisibleEvidence;
		const bool AllowMigrationSerializableEvidence;
		const bool AllowCompatibilityAliasEvidence;
		const bool AllowDebugEvidence;

	public:
		SecureEvidencePolicy();

		SecureEvidencePolicy(bool allowGuestVisibleEvidence, bool allowMigrationSerializableEvidence, bool allowCompatibilityAliasEvidence, bool allowDebugEvidence);

		static std::shared_ptr<SecureEvidencePolicy> getFailClosed();

		bool getAllowGuestVisibleEvidence() const;

		bool getAllowMigrationSerializableEvidence() const;

		bool getAllowCompatibilityAliasEvidence() const;

		bool getAllowDebugEvidence() const;

		bool CanExposeToGuest(std::shared_ptr<EvidencePolicyDescriptor> basePolicy, SecureEvidenceVisibilityClass evidenceClass);

		bool CanSerializeAcrossMigration(std::shared_ptr<EvidencePolicyDescriptor> basePolicy, SecureEvidenceVisibilityClass evidenceClass);

		bool MustQuarantine(SecureEvidenceVisibilityClass evidenceClass);
	};
}
