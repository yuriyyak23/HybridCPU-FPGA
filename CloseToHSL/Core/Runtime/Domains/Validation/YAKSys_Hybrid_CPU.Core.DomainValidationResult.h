#pragma once

#include "DomainValidationResult.h"
#include "../Services/YAKSys_Hybrid_CPU.Core.DomainRuntimeContext.h"
#include "../Services/YAKSys_Hybrid_CPU.Core.DomainRuntimeOperation.h"
#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
struct DomainBoundaryDescriptor; struct CapabilityBoundaryRequirement;
	class DomainValidationResult final : public std::enable_shared_from_this<DomainValidationResult>
	{
	private:
		const bool IsValid;
		const DomainValidationFailureReason FailureReason;
		const std::wstring Message;
		static const std::shared_ptr<DomainValidationResult> Passed;

		DomainValidationResult(bool isValid, DomainValidationFailureReason failureReason, const std::wstring &message);

	public:
		bool isValid() const;

		DomainValidationFailureReason getFailureReason() const;

		std::wstring getMessage() const;

		static std::shared_ptr<DomainValidationResult> getPassed();

		static std::shared_ptr<DomainValidationResult> Fail(DomainValidationFailureReason failureReason, const std::wstring &message = L"");

		static std::shared_ptr<DomainValidationResult> RequireRuntimeContext(std::shared_ptr<DomainRuntimeContext> context);

		static std::shared_ptr<DomainValidationResult> RequireRuntimeContext(std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<DomainBoundaryDescriptor> operationDomainRequirement);

		static std::shared_ptr<DomainValidationResult> RequireOperation(std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<DomainRuntimeOperation> operation, std::shared_ptr<CapabilityBoundaryRequirement> capabilityRequirement = nullptr);
	};
}
