#include "DisabledVirtualizationRepositoryOwnerReviewWorkflow.h"
#include <stdexcept>

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<DisabledVirtualizationRepositoryOwnerReviewWorkflow> DisabledVirtualizationRepositoryOwnerReviewWorkflow::getInstance()
	{
		static const std::shared_ptr<DisabledVirtualizationRepositoryOwnerReviewWorkflow> instance(new DisabledVirtualizationRepositoryOwnerReviewWorkflow());
		return instance;
	}

	DisabledVirtualizationRepositoryOwnerReviewWorkflow::DisabledVirtualizationRepositoryOwnerReviewWorkflow()
	{
	}

	std::shared_ptr<VirtualizationRepositoryOwnerReviewResult> DisabledVirtualizationRepositoryOwnerReviewWorkflow::Evaluate(std::shared_ptr<VirtualizationRepositoryOwnerReviewRequest> request)
	{
		if (request == nullptr)
		{
			throw std::invalid_argument("request");
		}

		if (!request->Attribution.CodeOwnersRulePresent || !request->Attribution.CodeOwnersRuleMatched)
		{
			return std::make_shared<VirtualizationRepositoryOwnerReviewResult>(VirtualizationRepositoryOwnerReviewDecision::DeniedCodeOwnersAttributionAbsent, L"Repository-owner review is unavailable because matching CODEOWNERS attribution is absent.");
		}

		return std::make_shared<VirtualizationRepositoryOwnerReviewResult>(VirtualizationRepositoryOwnerReviewDecision::DeniedWorkflowDisabled, L"Repository-owner review workflow preparation is disabled and cannot appoint an owner or accept D2.");
	}
}
