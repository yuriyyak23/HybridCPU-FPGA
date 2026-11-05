#pragma once

#include "../../../../../Runtime/Events/Traps/YAKSys_Hybrid_CPU.Core.TrapPolicyBitmap.h"
#include "../../../../../Runtime/Domains/Descriptors/TrapPolicy/YAKSys_Hybrid_CPU.Core.TrapPolicyDescriptor.h"
#include "../../../../../Runtime/Events/Traps/TrapPolicyBitmap.h"
#include "../../../../../Runtime/Events/Traps/TrapRequest.h"
#include "../../../../../Runtime/Domains/Validation/YAKSys_Hybrid_CPU.Core.DomainValidationResult.h"
#include "TrapDecision.h"
#include "VmxTrapProjectionMapper.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	struct TrapPolicyEvaluationResult final
	{
		const std::shared_ptr<DomainValidationResult> Validation;
		const std::shared_ptr<TrapDecision> Decision;
		TrapPolicyEvaluationResult(std::shared_ptr<DomainValidationResult> validation, std::shared_ptr<TrapDecision> decision)
			: Validation(std::move(validation)), Decision(std::move(decision)) {}
		bool getIsValid() const { return Validation != nullptr && Validation->isValid(); }
		static std::shared_ptr<TrapPolicyEvaluationResult> Fail(DomainValidationFailureReason reason, std::shared_ptr<TrapRequest> request, const std::wstring &message)
		{
			auto validation = DomainValidationResult::Fail(reason, message);
			auto decision = VmxTrapProjectionMapper::getDefault()->Project(NeutralTrapResult::Denied(std::move(request)));
			return std::make_shared<TrapPolicyEvaluationResult>(std::move(validation), std::move(decision));
		}
	};
}

namespace YAKSys_Hybrid_CPU::Core
{
	class TrapPolicyService final : public std::enable_shared_from_this<TrapPolicyService>
	{
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public TrapPolicyEvaluationResult Evaluate(TrapPolicyDescriptor? descriptor, System.Nullable<TrapPolicyBitmap> bitmap, TrapRequest request, bool domainValidated = true)
		std::shared_ptr<TrapPolicyEvaluationResult> Evaluate(std::shared_ptr<TrapPolicyDescriptor> descriptor, std::shared_ptr<TrapPolicyBitmap> bitmap, std::shared_ptr<TrapRequest> request, bool domainValidated = true);

	private:
		static TrapPolicyClass ToPolicyClass(TrapTargetKind targetKind);
	};
}
