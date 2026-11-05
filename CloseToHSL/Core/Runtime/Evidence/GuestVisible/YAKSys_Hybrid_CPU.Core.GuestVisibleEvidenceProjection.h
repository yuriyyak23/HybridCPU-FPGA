#pragma once

#include "../../../Virtualization/Sideband/EvidenceTransport/YAKSys_Hybrid_CPU.Core.EvidenceSidebandEnvelope.h"
#include "../../Domains/Descriptors/Observability/YAKSys_Hybrid_CPU.Core.ObservabilityDescriptor.h"
#include "../../Domains/Descriptors/EvidencePolicy/YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"
#include "GuestVisibleEvidenceProjection.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class GuestVisibleEvidenceProjection final : public std::enable_shared_from_this<GuestVisibleEvidenceProjection>
	{
	public:
		GuestVisibleEvidenceProjectionDecision Evaluate(std::shared_ptr<EvidenceSidebandEnvelope> envelope, std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy, std::shared_ptr<ObservabilityDescriptor> observability);

		bool TryProject(std::shared_ptr<EvidenceSidebandEnvelope> envelope, std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy, std::shared_ptr<ObservabilityDescriptor> observability, std::shared_ptr<GuestVisibleEvidenceRecord> &record, GuestVisibleEvidenceProjectionDecision &decision);
	};
}
