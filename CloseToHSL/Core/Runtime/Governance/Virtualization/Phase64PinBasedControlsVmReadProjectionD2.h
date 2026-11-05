#pragma once

#include "../../../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmcsField.h"
#include "../../Capabilities/CompatibilityControls/CompatibilityControlPolicyOwner.h"
#include <string>
#include <memory>
#include <optional>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class PinBasedControlsProjectionDecision : unsigned char
	{
		Granted = 0,
		DeniedMissingOwnerOrSnapshot = 1,
		DeniedStaleForeignOrCrossDomainSnapshot = 2,
		DeniedSemanticMismatch = 3
	};

struct PinBasedControlsProjectionResult final
{
    const PinBasedControlsProjectionDecision Decision;
    const std::optional<unsigned long long> Value;
    const std::wstring Reason;
    const bool IsGranted;
    PinBasedControlsProjectionResult(PinBasedControlsProjectionDecision decision, std::optional<unsigned long long> value, std::wstring reason)
        : Decision(decision), Value(std::move(value)), Reason(std::move(reason)), IsGranted(decision == PinBasedControlsProjectionDecision::Granted) {}
};

	class Phase64PinBasedControlsVmReadProjectionD2 final
	{
	public:
		static inline const std::wstring DecisionId = L"D2-HV-VMREAD-PIN-BASED-CONTROLS-0005";
		static constexpr VmcsField ExactField = VmcsField::PinBasedControls;
		static constexpr unsigned long long RuntimeTrapPolicyRequiredBit = 1ULL << 0;
		static constexpr unsigned long long NeutralTrapResultRequiredBit = 1ULL << 1;
		static constexpr unsigned long long PublicationFenceRequiredBit = 1ULL << 2;
		static constexpr unsigned long long SupportedMask = RuntimeTrapPolicyRequiredBit | NeutralTrapResultRequiredBit | PublicationFenceRequiredBit;
		static constexpr unsigned long long UnsupportedMask = ~SupportedMask;
		static bool getRuntimeAuthorityGranted();
		static bool getProductionCompositionAuthorized();

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static PinBasedControlsProjectionResult Project(CompatibilityControlPolicyOwner? owner, System.Nullable<CompatibilityControlPolicySnapshot> snapshot)
		static std::shared_ptr<PinBasedControlsProjectionResult> Project(std::shared_ptr<CompatibilityControlPolicyOwner> owner, std::shared_ptr<CompatibilityControlPolicySnapshot> snapshot);

	private:
		static std::shared_ptr<PinBasedControlsProjectionResult> Deny(PinBasedControlsProjectionDecision decision, const std::wstring &reason);
	};

}
