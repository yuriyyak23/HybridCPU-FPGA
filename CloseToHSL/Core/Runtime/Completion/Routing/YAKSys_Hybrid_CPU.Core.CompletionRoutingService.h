#pragma once

#include "../../Domains/Descriptors/CompletionRoute/YAKSys_Hybrid_CPU.Core.CompletionRouteDescriptor.h"
#include "../../Domains/Validation/YAKSys_Hybrid_CPU.Core.DomainValidationResult.h"
#include "LaneCompletionRouting.h"
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	struct CompletionRoutingAdmissionResult final
	{
		const std::shared_ptr<DomainValidationResult> Validation;
		const std::shared_ptr<LaneCompletionRoutingResult> Routing;

		CompletionRoutingAdmissionResult(std::shared_ptr<DomainValidationResult> validation,
			std::shared_ptr<LaneCompletionRoutingResult> routing)
			: Validation(std::move(validation)), Routing(std::move(routing)) {}

		bool IsRouted() const
		{
			return Validation != nullptr && Validation->isValid() && Routing != nullptr && Routing->Routed();
		}

		static std::shared_ptr<CompletionRoutingAdmissionResult> Fail(DomainValidationFailureReason reason,
			std::shared_ptr<LaneCompletionDescriptor> completion, LaneCompletionRouteStatus status,
			const std::wstring& message)
		{
			auto routing = LaneCompletionRoutingResult::Create(status, std::move(completion), nullptr,
				InterruptPostDisposition::None, 0, message);
			return std::make_shared<CompletionRoutingAdmissionResult>(DomainValidationResult::Fail(reason, message), std::move(routing));
		}
	};

	class CompletionRoutingService final : public std::enable_shared_from_this<CompletionRoutingService>
	{
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public CompletionRoutingAdmissionResult RoutePostedCompletion(CompletionRouteDescriptor? descriptor, LaneCompletionRouter router, LaneCompletionDescriptor completion)
		std::shared_ptr<CompletionRoutingAdmissionResult> RoutePostedCompletion(std::shared_ptr<CompletionRouteDescriptor> descriptor, std::shared_ptr<LaneCompletionRouter> router, std::shared_ptr<LaneCompletionDescriptor> completion);
	};
}
