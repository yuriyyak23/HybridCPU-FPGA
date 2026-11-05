#include "PinBasedControlsVmReadScalarDeliveryCanonicalComposition.h"
#include "../../../Pipeline/MicroOps/Types/MicroOp.IO.h"
#include "../../../Virtualization/Compatibility/Frontend/Retire/VmxRetireModel.h"
#include "../../../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmcsField.h"
#include "../../Governance/Virtualization/Phase64PinBasedControlsVmReadProjectionD2.h"

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal PinBasedControlsVmReadScalarDeliveryCanonicalComposition(CompatibilityControlPolicyOwner policyOwner, bool enabled)
	PinBasedControlsVmReadScalarDeliveryCanonicalComposition::PinBasedControlsVmReadScalarDeliveryCanonicalComposition(std::shared_ptr<CompatibilityControlPolicyOwner> policyOwner, bool enabled) : _policyOwner((policyOwner != nullptr) ? policyOwner : throw std::invalid_argument("policyOwner"))
	{
		_enabled = enabled ? 1 : 0;
	}

	bool PinBasedControlsVmReadScalarDeliveryCanonicalComposition::isEnabled() const
	{
		return _enabled.load(std::memory_order_acquire) != 0;
	}

	bool PinBasedControlsVmReadScalarDeliveryCanonicalComposition::Disable()
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			_enabled.store(0, std::memory_order_release);
			AdvanceGeneration();
			return true;
		}
	}

	std::shared_ptr<VmReadScalarDeliveryResult> PinBasedControlsVmReadScalarDeliveryCanonicalComposition::Prepare(ReplayPhaseContext replayPhase, std::shared_ptr<VmxMicroOp> carrier, std::shared_ptr<VmReadScalarAttemptBinding> attempt, unsigned long long fieldSelector, unsigned long long restoreGeneration)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			if (!isEnabled())
			{
				return Deny(VmReadScalarDeliveryDecision::Disabled, L"Exact PinBasedControls scalar-delivery profile is default-disabled.");
			}
			if (!replayPhase.isActive() || attempt->getOperation() != VmxOperationKind::VmRead || attempt->getAttemptId() == 0 || attempt->getBundleIdentity() == 0 || attempt->getReplayEpoch() != replayPhase.getEpochId() || carrier->getVirtualizationAdmission() != attempt->getCertificate())
			{
				return Deny(VmReadScalarDeliveryDecision::AdmissionDenied, L"Live canonical VMREAD E1 attempt is required.");
			}
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: if (fieldSelector > ushort.MaxValue || unchecked((VmcsField)(ushort)fieldSelector) != VmcsField.PinBasedControls)
			if (fieldSelector > std::numeric_limits<unsigned short>::max() || static_cast<VmcsField>(static_cast<unsigned short>(fieldSelector)) != VmcsField::PinBasedControls)
			{
				return Deny(VmReadScalarDeliveryDecision::FieldDenied, L"Only exact VmcsField.PinBasedControls is admitted.");
			}
			if (carrier->getRd() == 0 || carrier->getRd() > 31 || carrier->getRs2() != 0)
			{
				return Deny(VmReadScalarDeliveryDecision::DestinationDenied, L"VMREAD scalar delivery requires x1-x31 and reserved x0 Rs2.");
			}

			std::shared_ptr<CompatibilityControlPolicySnapshot> snapshot = _policyOwner->CaptureCurrent();
			if (snapshot->getIdentity()->DomainIdentity != attempt->getDomainTag())
			{
				return Deny(VmReadScalarDeliveryDecision::SourceDenied, L"Owner-issued compatibility-control policy does not match the VMREAD domain.");
			}
			std::shared_ptr<PinBasedControlsProjectionResult> projection = Phase64PinBasedControlsVmReadProjectionD2::Project(_policyOwner, snapshot);
			if (!projection->IsGranted || !projection->Value.has_value())
			{
				return Deny(VmReadScalarDeliveryDecision::ProjectionDenied, projection->Reason);
			}

			std::shared_ptr<VmReadScalarResultReceipt> receipt = std::make_shared<VmReadScalarResultReceipt>(shared_from_this(), attempt, DecisionId, _policyOwner, snapshot->getIdentity()->PolicyGeneration, snapshot, _generation, attempt->getAttemptId(), attempt->getIssuerGeneration(), attempt->getBundleIdentity(), attempt->getReplayEpoch(), restoreGeneration, attempt->getDomainTag(), VmcsField::PinBasedControls, carrier->getRd(), projection->Value.value());
			return std::make_shared<VmReadScalarDeliveryResult>(VmReadScalarDeliveryDecision::Prepared, receipt, L"Exact owner-issued PinBasedControls projection captured for compatibility-only scalar delivery.");
		}
	}

	bool PinBasedControlsVmReadScalarDeliveryCanonicalComposition::ValidateLive(std::shared_ptr<VmReadScalarResultReceipt> receipt, std::optional<unsigned long long> currentRestoreGeneration)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			return Validate(receipt, currentRestoreGeneration) && !receipt->isConsumed();
		}
	}

	bool PinBasedControlsVmReadScalarDeliveryCanonicalComposition::ValidateConsumedBinding(std::shared_ptr<VmReadScalarResultReceipt> receipt, unsigned long long currentRestoreGeneration)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			return Validate(receipt, currentRestoreGeneration);
		}
	}

	bool PinBasedControlsVmReadScalarDeliveryCanonicalComposition::Validate(std::shared_ptr<VmReadScalarResultReceipt> receipt, std::optional<unsigned long long> currentRestoreGeneration)
	{
		if (!receipt || !isEnabled() || receipt->getDecisionId() != DecisionId || receipt->getProfileGeneration() != _generation)
		{
			return false;
		}

		const auto capture = receipt->getCompatibilityControlCapture();
		const auto sourceOwner = receipt->getSourceOwner();
		const auto* typedSourceOwner = std::any_cast<std::shared_ptr<CompatibilityControlPolicyOwner>>(&sourceOwner);
		if (!capture || typedSourceOwner == nullptr || *typedSourceOwner != _policyOwner || !_policyOwner->IsCurrent(capture))
		{
			return false;
		}

		const auto identity = capture->getIdentity();
		return identity && receipt->getSourceEpoch() == identity->PolicyGeneration && receipt->getDomainTag() == identity->DomainIdentity && receipt->getField() == VmcsField::PinBasedControls && receipt->getAttemptId() != 0 && receipt->getBundleIdentity() != 0 && (!currentRestoreGeneration || receipt->getRestoreGeneration() == currentRestoreGeneration.value());
	}

	void PinBasedControlsVmReadScalarDeliveryCanonicalComposition::AdvanceGeneration()
	{
//		checked
			_generation++;
// C# TO C++ CONVERTER TASK: End of the original C# 'checked' block.
		if (_generation == 0)
		{
			throw InvalidOperationException(L"PinBasedControls delivery generation exhausted.");
		}
	}

	std::shared_ptr<VmReadScalarDeliveryResult> PinBasedControlsVmReadScalarDeliveryCanonicalComposition::Deny(VmReadScalarDeliveryDecision decision, const std::wstring &reason)
	{
		return std::make_shared<VmReadScalarDeliveryResult>(decision, nullptr, reason);
	}
}
