#pragma once

#include "VmxRetireModel.h"
#include "../../../../Runtime/Domains/Descriptors/Observability/YAKSys_Hybrid_CPU.Core.ObservabilityDescriptor.h"
#include "../../../../Runtime/Domains/Descriptors/EvidencePolicy/YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class RetireEvidenceDecision : unsigned char
	{
		Allowed = 0,
		InvalidRetireEffect = 1,
		GuestPublicationDenied = 2,
		HostCaptureDenied = 3,
		HostOwnedEvidenceDenied = 4
	};

	struct RetireEvidenceResult final
	{
		const RetireEvidenceDecision Decision;
		const std::wstring Reason;
		const bool IsAllowed;

		RetireEvidenceResult(RetireEvidenceDecision decision, std::wstring reason)
			: Decision(decision), Reason(std::move(reason)),
			  IsAllowed(decision == RetireEvidenceDecision::Allowed) {}

		static inline const std::shared_ptr<RetireEvidenceResult> Allowed =
			std::make_shared<RetireEvidenceResult>(RetireEvidenceDecision::Allowed, std::wstring{});

		static std::shared_ptr<RetireEvidenceResult> Denied(RetireEvidenceDecision decision, const std::wstring &reason)
		{
			return std::make_shared<RetireEvidenceResult>(decision, reason);
		}
	};

	struct RetireEvidencePublicationRequest final
	{
		const VmxRetireEffect RetireEffect;
		const EvidenceVisibilityClass EvidenceClass;
		const bool GuestVisiblePublication;

		RetireEvidencePublicationRequest(VmxRetireEffect retireEffect,
			EvidenceVisibilityClass evidenceClass, bool guestVisiblePublication)
			: RetireEffect(std::move(retireEffect)), EvidenceClass(evidenceClass),
			  GuestVisiblePublication(guestVisiblePublication) {}
	};

	class RetireEvidenceBoundary final : public std::enable_shared_from_this<RetireEvidenceBoundary>
	{
	public:
		std::shared_ptr<RetireEvidenceResult> ValidatePublication(
			std::shared_ptr<RetireEvidencePublicationRequest> request,
			std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy,
			std::shared_ptr<ObservabilityDescriptor> observability);

	private:
		static std::shared_ptr<RetireEvidenceResult> ValidateGuestPublication(
			EvidenceVisibilityClass evidenceClass,
			std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy,
			std::shared_ptr<ObservabilityDescriptor> observability);

		static std::shared_ptr<RetireEvidenceResult> ValidateHostLocalCapture(
			EvidenceVisibilityClass evidenceClass,
			std::shared_ptr<ObservabilityDescriptor> observability);
	};
}
