#pragma once

#include "EvidencePolicyDescriptor.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class EvidencePolicyDescriptor final : public std::enable_shared_from_this<EvidencePolicyDescriptor>
	{
	private:
		static inline const std::shared_ptr<EvidencePolicyDescriptor> FailClosed = std::make_shared<EvidencePolicyDescriptor>();
		const bool AllowCompatibilityAliases;
		const bool AllowGuestArchitecturalState;
		const bool AllowMigrationSerializableState;

	public:
		EvidencePolicyDescriptor();

		EvidencePolicyDescriptor(bool allowCompatibilityAliases, bool allowGuestArchitecturalState, bool allowMigrationSerializableState);

		static std::shared_ptr<EvidencePolicyDescriptor> getFailClosed();

		bool getAllowCompatibilityAliases() const;

		bool getAllowGuestArchitecturalState() const;

		bool getAllowMigrationSerializableState() const;

		bool CanExposeToGuest(EvidenceVisibilityClass evidenceClass);

		bool CanSerializeAcrossMigration(EvidenceVisibilityClass evidenceClass, EvidenceRestorePolicy restorePolicy);

		bool MustRecomputeAfterRestore(EvidenceVisibilityClass evidenceClass);
	};
}
