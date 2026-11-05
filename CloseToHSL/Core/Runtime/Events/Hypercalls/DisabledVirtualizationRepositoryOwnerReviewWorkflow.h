#pragma once

#include <memory>
#include <string>
#include <utility>
#include "VirtualizationOperationDecisionTypes.h"

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VirtualizationRepositoryOwnerReviewDecision : unsigned char
	{
		DeniedCodeOwnersAttributionAbsent = 0,
		DeniedWorkflowDisabled = 1
	};

struct VirtualizationRepositoryOwnerReviewRequest final
{
	const std::wstring DecisionId;
	const VirtualizationDecisionAttributionEvidence Attribution;

	VirtualizationRepositoryOwnerReviewRequest(std::wstring decisionId,
		VirtualizationDecisionAttributionEvidence attribution)
		: DecisionId(std::move(decisionId)), Attribution(std::move(attribution)) {}

	bool operator==(const VirtualizationRepositoryOwnerReviewRequest &) const = default;
};

struct VirtualizationRepositoryOwnerReviewResult final
{
	const VirtualizationRepositoryOwnerReviewDecision Decision;
	const std::wstring Reason;

	VirtualizationRepositoryOwnerReviewResult(VirtualizationRepositoryOwnerReviewDecision decision,
		std::wstring reason)
		: Decision(decision), Reason(std::move(reason)) {}

	bool getOwnerAppointmentAuthorized() const noexcept { return false; }
	bool getDecisionAcceptanceAuthorized() const noexcept { return false; }
	bool getBackendExecutionAuthorized() const noexcept { return false; }
	bool operator==(const VirtualizationRepositoryOwnerReviewResult &) const = default;
};

	/// <summary>
	/// Repository-owner review preparation only. This workflow has no approve,
	/// accept, appoint or execution operation and always remains disabled.
	/// </summary>
	class DisabledVirtualizationRepositoryOwnerReviewWorkflow final : public std::enable_shared_from_this<DisabledVirtualizationRepositoryOwnerReviewWorkflow>
	{
	private:
	public:
		static std::shared_ptr<DisabledVirtualizationRepositoryOwnerReviewWorkflow> getInstance();

	private:
		DisabledVirtualizationRepositoryOwnerReviewWorkflow();

	public:
		std::shared_ptr<VirtualizationRepositoryOwnerReviewResult> Evaluate(std::shared_ptr<VirtualizationRepositoryOwnerReviewRequest> request);
	};

}
