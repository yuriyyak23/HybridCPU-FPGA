#pragma once
#include "../../../../Capabilities/CapabilityBoundaryRequirement.h"
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureMemoryAccessKind : unsigned char
	{
		RuntimeTouch = 0,
		HostRead = 1,
		DmaRead = 2,
		DmaWrite = 3,
		Measurement = 4
	};

	enum class SecureMemoryAccessOrigin : unsigned char
	{
		RuntimeBoundary = 0,
		IoDma = 1,
		HypercallArgument = 2
	};

	enum class SecureMemoryAdmissionDecision : unsigned char
	{
		AllowedSecureMemory = 0,
		DeniedMissingDescriptor = 1,
		DeniedUnmaterializedDescriptor = 2,
		DeniedRegionNotFound = 3,
		DeniedStalePolicyEpoch = 4,
		DeniedPrivateHostRead = 5,
		DeniedHostInspection = 6,
		DeniedPrivateDma = 7,
		DeniedSharedRequiresExplicitPolicy = 8,
		DeniedDmaRequiresTypedGrant = 9,
		DeniedMeasuredRegionMissing = 10,
		DeniedSharedBufferBinding = 11,
		DeniedRuntimeMutableClassification = 12
	};

struct SecureMemoryAccessRequest {
    SecureMemoryAccessKind Kind{}; unsigned long long Address{}, Length{};
    SecureMemoryAccessOrigin Origin = SecureMemoryAccessOrigin::RuntimeBoundary;
    CapabilityBoundaryRequirement CapabilityRequirement{0, CapabilityGrantScope::None, false};
    std::shared_ptr<CapabilityDescriptorSet> Capabilities;
    SecureMemoryAccessRequest(SecureMemoryAccessKind kind = SecureMemoryAccessKind::RuntimeTouch, unsigned long long address = 0, unsigned long long length = 0, SecureMemoryAccessOrigin origin = SecureMemoryAccessOrigin::RuntimeBoundary, CapabilityBoundaryRequirement requirement = {0, CapabilityGrantScope::None, false}, std::shared_ptr<CapabilityDescriptorSet> capabilities = nullptr)
        : Kind(kind), Address(address), Length(length), Origin(origin), CapabilityRequirement(requirement), Capabilities(std::move(capabilities)) {}
};
struct SecureMemoryAdmissionResult {
    const SecureMemoryAdmissionDecision Decision; const std::wstring Reason; const bool IsAllowed;
    SecureMemoryAdmissionResult(SecureMemoryAdmissionDecision d, std::wstring r) : Decision(d), Reason(std::move(r)), IsAllowed(d == SecureMemoryAdmissionDecision::AllowedSecureMemory) {}
    static std::shared_ptr<SecureMemoryAdmissionResult> Denied(SecureMemoryAdmissionDecision d, std::wstring r) { return std::make_shared<SecureMemoryAdmissionResult>(d, std::move(r)); }
    static const std::shared_ptr<SecureMemoryAdmissionResult> AllowedSecureMemory;
};
inline const std::shared_ptr<SecureMemoryAdmissionResult> SecureMemoryAdmissionResult::AllowedSecureMemory = std::make_shared<SecureMemoryAdmissionResult>(SecureMemoryAdmissionDecision::AllowedSecureMemory, L"");
}