#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class ShadowVmcsCompatibilityBridgeViolation : unsigned char
	{
		None = 0,
		ServiceStoresVmcsDescriptor = 1,
		ServiceCallsShadowVmcsDirectly = 2,
		BridgeNotCompatibilityOnly = 3,
		MissingProjectionServiceBoundary = 4,
		MissingTypedFailure = 5
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct ShadowVmcsCompatibilityBridgeRequest(bool ServiceStoresVmcsDescriptor, bool ServiceCallsShadowVmcsDirectly, bool BridgeIsCompatibilityOnly, bool HasProjectionServiceBoundary, bool ReturnsTypedFailure);
	struct ShadowVmcsCompatibilityBridgeRequest final {
		bool ServiceStoresVmcsDescriptor = false;
		bool ServiceCallsShadowVmcsDirectly = false;
		bool BridgeIsCompatibilityOnly = false;
		bool HasProjectionServiceBoundary = false;
		bool ReturnsTypedFailure = false;
	};


}
