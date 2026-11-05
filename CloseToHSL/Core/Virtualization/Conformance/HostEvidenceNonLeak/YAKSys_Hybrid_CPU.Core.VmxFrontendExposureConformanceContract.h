#pragma once

#include "VmxFrontendExposureConformanceContract.h"
#include "../../../Runtime/Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class VmxFrontendExposureConformanceContract final : public std::enable_shared_from_this<VmxFrontendExposureConformanceContract>
	{
	public:
		VmxFrontendExposureViolation Evaluate(std::shared_ptr<VmxFrontendExposureRequest> request);

		bool IsSatisfied(std::shared_ptr<VmxFrontendExposureRequest> request);

	private:
		static bool IsHostOwnedEvidence(EvidenceVisibilityClass evidenceClass);
	};
}
