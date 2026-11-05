#include "Phase64PinBasedControlsVmReadProjectionD2.h"
#include "../../Capabilities/CompatibilityControls/CompatibilityControlDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	bool Phase64PinBasedControlsVmReadProjectionD2::getRuntimeAuthorityGranted()
	{
		return false;
	}

	bool Phase64PinBasedControlsVmReadProjectionD2::getProductionCompositionAuthorized()
	{
		return false;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static PinBasedControlsProjectionResult Project(CompatibilityControlPolicyOwner? owner, System.Nullable<CompatibilityControlPolicySnapshot> snapshot)
	std::shared_ptr<PinBasedControlsProjectionResult> Phase64PinBasedControlsVmReadProjectionD2::Project(std::shared_ptr<CompatibilityControlPolicyOwner> owner, std::shared_ptr<CompatibilityControlPolicySnapshot> snapshot)
	{
		if (owner == nullptr || snapshot == nullptr)
		{
			return Deny(PinBasedControlsProjectionDecision::DeniedMissingOwnerOrSnapshot, L"Exact PinBasedControls projection requires a present neutral owner and snapshot.");
		}
		if (!owner->IsCurrent(snapshot))
		{
			return Deny(PinBasedControlsProjectionDecision::DeniedStaleForeignOrCrossDomainSnapshot, L"Exact PinBasedControls projection requires the owner's current exact-domain snapshot.");
		}

		CompatibilityEventRoutingPolicy policy = snapshot->getEventRoutingPolicy();
		if (static_cast<int>(policy & ~CompatibilityControlPolicyOwner::KnownEventRoutingPolicyMask) != 0)
		{
			return Deny(PinBasedControlsProjectionDecision::DeniedSemanticMismatch, L"Neutral event-routing policy contains no exact HybridCPU VMX8 mapping.");
		}

		unsigned long long value = 0;
		if (static_cast<int>(policy & CompatibilityEventRoutingPolicy::RuntimeTrapPolicyRequired) != 0)
		{
			value |= RuntimeTrapPolicyRequiredBit;
		}
		if (static_cast<int>(policy & CompatibilityEventRoutingPolicy::NeutralTrapResultRequired) != 0)
		{
			value |= NeutralTrapResultRequiredBit;
		}
		if (static_cast<int>(policy & CompatibilityEventRoutingPolicy::PublicationFenceRequired) != 0)
		{
			value |= PublicationFenceRequiredBit;
		}

		if ((value & UnsupportedMask) != 0)
		{
			return Deny(PinBasedControlsProjectionDecision::DeniedSemanticMismatch, L"Unsupported PinBasedControls bits cannot be projected.");
		}

		return std::make_shared<PinBasedControlsProjectionResult>(PinBasedControlsProjectionDecision::Granted, value, L"Current neutral policy mapped exactly to the frozen HybridCPU VMX8 field.");
	}

	std::shared_ptr<PinBasedControlsProjectionResult> Phase64PinBasedControlsVmReadProjectionD2::Deny(PinBasedControlsProjectionDecision decision, const std::wstring &reason)
	{
		return std::make_shared<PinBasedControlsProjectionResult>(decision, std::nullopt, reason);
	}
}
