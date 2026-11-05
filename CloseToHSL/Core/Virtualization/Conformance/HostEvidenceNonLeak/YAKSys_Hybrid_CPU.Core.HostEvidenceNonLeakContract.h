#pragma once

#include "../../../Runtime/Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h"
#include "HostEvidenceNonLeakContract.h"
#include "../../Sideband/EvidenceTransport/YAKSys_Hybrid_CPU.Core.EvidenceSidebandEnvelope.h"
#include "../../../Runtime/Domains/Descriptors/EvidencePolicy/YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"
#include "../../Sideband/CompletionTransport/YAKSys_Hybrid_CPU.Core.CompletionSidebandEnvelope.h"
#include "../../Sideband/DescriptorTransport/YAKSys_Hybrid_CPU.Core.DescriptorSidebandEnvelope.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class HostEvidenceNonLeakContract final : public std::enable_shared_from_this<HostEvidenceNonLeakContract>
	{
	public:
		bool IsHostOwnedEvidenceClass(EvidenceVisibilityClass visibilityClass);

		HostEvidenceNonLeakViolation ValidateGuestVisibility(EvidenceVisibilityClass visibilityClass, bool isHostOwned, bool isGuestVisible);

		HostEvidenceNonLeakViolation ValidateEvidenceEnvelope(std::shared_ptr<EvidenceSidebandEnvelope> envelope, std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy);

		HostEvidenceNonLeakViolation ValidateCompletionEnvelope(std::shared_ptr<CompletionSidebandEnvelope> envelope, std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy);

		HostEvidenceNonLeakViolation ValidateDescriptorEnvelope(std::shared_ptr<DescriptorSidebandEnvelope> envelope, std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy);
	};
}
