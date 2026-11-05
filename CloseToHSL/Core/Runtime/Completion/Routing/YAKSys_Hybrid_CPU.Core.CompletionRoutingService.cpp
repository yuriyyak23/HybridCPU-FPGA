#include "YAKSys_Hybrid_CPU.Core.CompletionRoutingService.h"

namespace YAKSys_Hybrid_CPU::Core
{
	std::shared_ptr<CompletionRoutingAdmissionResult> CompletionRoutingService::RoutePostedCompletion(
		std::shared_ptr<CompletionRouteDescriptor> descriptor,
		std::shared_ptr<LaneCompletionRouter> router,
		std::shared_ptr<LaneCompletionDescriptor> completion)
	{
		if (descriptor == nullptr || router == nullptr || completion == nullptr)
		{
			const auto reason = descriptor == nullptr
				? DomainValidationFailureReason::MissingCompletionRouteDescriptor
				: DomainValidationFailureReason::EventQueueRejected;
			return CompletionRoutingAdmissionResult::Fail(reason, completion,
				LaneCompletionRouteStatus::DroppedInvalid,
				L"Completion routing requires a runtime-owned route descriptor, router, and completion.");
		}

		if (!descriptor->isRuntimeAuthoritative())
		{
			return CompletionRoutingAdmissionResult::Fail(DomainValidationFailureReason::RuntimeAuthorityMissing,
				completion, LaneCompletionRouteStatus::DroppedInvalid,
				L"Compatibility projection cannot own completion routing.");
		}

		if (!completion->IsValid())
		{
			return CompletionRoutingAdmissionResult::Fail(DomainValidationFailureReason::EventQueueRejected,
				completion, LaneCompletionRouteStatus::DroppedInvalid,
				L"Completion routing rejected an invalid lane completion descriptor.");
		}

		if (!descriptor->AllowsSource(completion->SourceKind))
		{
			return CompletionRoutingAdmissionResult::Fail(DomainValidationFailureReason::CompletionSourceDenied,
				completion, LaneCompletionRouteStatus::IgnoredRouteDisabled,
				L"Completion route descriptor denies the completion source.");
		}

		std::shared_ptr<EventInjectionDescriptor> event;
		std::shared_ptr<LaneCompletionRoutingResult> routing;
		const bool routed = router->TryBuildPostedCompletion(completion, event, routing);

		const auto validation = routed
			? DomainValidationResult::getPassed()
			: DomainValidationResult::Fail(DomainValidationFailureReason::EventQueueRejected,
				routing != nullptr ? routing->Message : L"Completion router rejected the posted completion.");
		return std::make_shared<CompletionRoutingAdmissionResult>(validation, std::move(routing));
	}
}