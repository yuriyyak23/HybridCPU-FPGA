#pragma once

#include "VmReadScalarDeliveryCanonicalComposition.h"
#include "../../Capabilities/CompatibilityControls/CompatibilityControlPolicyOwner.h"
#include "../../../Pipeline/Certificates/ReplayPhaseSubstrate.h"
namespace YAKSys_Hybrid_CPU::Core { class VmxMicroOp; }
#include <string>
#include <limits>
#include <stdexcept>
#include <optional>
#include <mutex>
#include "tangible_exception_helper.h"
#include <memory>
#include <atomic>

namespace YAKSys_Hybrid_CPU::Core
{

	class PinBasedControlsVmReadScalarDeliveryCanonicalComposition final : public IVmReadScalarResultReceiptOwner
	{
	public:
		static inline const std::wstring DecisionId = L"D2-HV-VMREAD-PIN-BASED-CONTROLS-SCALAR-0006";

	private:
		std::mutex _sync;
		const std::shared_ptr<CompatibilityControlPolicyOwner> _policyOwner;
		std::atomic<int> _enabled{0};
		unsigned long long _generation = 1;

	public:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal PinBasedControlsVmReadScalarDeliveryCanonicalComposition(CompatibilityControlPolicyOwner policyOwner, bool enabled)
		PinBasedControlsVmReadScalarDeliveryCanonicalComposition(std::shared_ptr<CompatibilityControlPolicyOwner> policyOwner, bool enabled);

		bool isEnabled() const;

		bool Disable();

		std::shared_ptr<VmReadScalarDeliveryResult> Prepare(ReplayPhaseContext replayPhase, std::shared_ptr<VmxMicroOp> carrier, std::shared_ptr<VmReadScalarAttemptBinding> attempt, unsigned long long fieldSelector, unsigned long long restoreGeneration);

		bool ValidateLive(std::shared_ptr<VmReadScalarResultReceipt> receipt, std::optional<unsigned long long> currentRestoreGeneration = std::nullopt) override;

		bool ValidateConsumedBinding(std::shared_ptr<VmReadScalarResultReceipt> receipt, unsigned long long currentRestoreGeneration) override;

	private:
		bool Validate(std::shared_ptr<VmReadScalarResultReceipt> receipt, std::optional<unsigned long long> currentRestoreGeneration);

		void AdvanceGeneration();

		static std::shared_ptr<VmReadScalarDeliveryResult> Deny(VmReadScalarDeliveryDecision decision, const std::wstring &reason);
	};

}
