#include "YAKSys_Hybrid_CPU.Core.ShadowVmcsCompatibilityBridgeContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	ShadowVmcsCompatibilityBridgeViolation ShadowVmcsCompatibilityBridgeContract::Evaluate(std::shared_ptr<ShadowVmcsCompatibilityBridgeRequest> request)
	{
		if (request == nullptr)
		{
			return ShadowVmcsCompatibilityBridgeViolation::BridgeNotCompatibilityOnly;
		}

		if (request->ServiceStoresVmcsDescriptor)
		{
			return ShadowVmcsCompatibilityBridgeViolation::ServiceStoresVmcsDescriptor;
		}

		if (request->ServiceCallsShadowVmcsDirectly)
		{
			return ShadowVmcsCompatibilityBridgeViolation::ServiceCallsShadowVmcsDirectly;
		}

		if (!request->BridgeIsCompatibilityOnly)
		{
			return ShadowVmcsCompatibilityBridgeViolation::BridgeNotCompatibilityOnly;
		}

		if (!request->ReturnsTypedFailure)
		{
			return ShadowVmcsCompatibilityBridgeViolation::MissingTypedFailure;
		}

		return request->HasProjectionServiceBoundary ? ShadowVmcsCompatibilityBridgeViolation::None : ShadowVmcsCompatibilityBridgeViolation::MissingProjectionServiceBoundary;
	}

	bool ShadowVmcsCompatibilityBridgeContract::IsSatisfied(std::shared_ptr<ShadowVmcsCompatibilityBridgeRequest> request)
	{
		return Evaluate(request) == ShadowVmcsCompatibilityBridgeViolation::None;
	}
}
