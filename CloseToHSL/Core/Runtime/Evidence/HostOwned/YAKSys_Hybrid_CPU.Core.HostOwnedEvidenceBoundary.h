#pragma once
#include "HostOwnedEvidenceBoundary.h"

#include "../../Domains/Descriptors/Observability/YAKSys_Hybrid_CPU.Core.ObservabilityDescriptor.h"
#include "../../Domains/Descriptors/EvidencePolicy/YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"
#include "../../Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class HostOwnedEvidenceBoundary final : public std::enable_shared_from_this<HostOwnedEvidenceBoundary>
	{
	public:
		std::shared_ptr<HostOwnedEvidenceBoundaryResult> ValidateGuestProjection(std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy, std::shared_ptr<ObservabilityDescriptor> observability, EvidenceVisibilityClass evidenceClass);

		std::shared_ptr<HostOwnedEvidenceBoundaryResult> ValidateHostLocalCapture(std::shared_ptr<ObservabilityDescriptor> observability, EvidenceVisibilityClass evidenceClass);

		std::shared_ptr<HostOwnedEvidenceBoundaryResult> ValidateRestore(std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy, EvidenceVisibilityClass evidenceClass, EvidenceRestorePolicy restorePolicy);
	};
}
