#pragma once

#include "NestedEvidencePolicy.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class NestedEvidencePolicy final : public std::enable_shared_from_this<NestedEvidencePolicy>
	{
	public:
		std::shared_ptr<NestedEvidencePolicyResult> Validate(std::shared_ptr<NestedEvidencePolicyRequest> request);

		bool CanExpose(std::shared_ptr<NestedEvidencePolicyRequest> request);

	private:
		static bool IsHostOwnedEvidence(EvidenceVisibilityClass evidenceClass);
	};
}
