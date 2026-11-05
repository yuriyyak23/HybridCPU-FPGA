#include "YAKSys_Hybrid_CPU.Core.ExecutionDomainRuntime.h"
#include "ExecutionDomainRuntime.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<ExecutionDomainRuntimeResult> ExecutionDomainRuntime::Validate(std::shared_ptr<ExecutionDomainRuntimeRequest> request)
	{
		if (request->Descriptor == nullptr)
		{
			return ExecutionDomainRuntimeResult::Denied(ExecutionDomainRuntimeDecision::MissingDescriptor, L"Execution domain runtime requires an execution-domain descriptor.");
		}

		if (!request->Descriptor->isAuthoritativeExecutionStateOwner())
		{
			return ExecutionDomainRuntimeResult::Denied(ExecutionDomainRuntimeDecision::DescriptorAuthorityDenied, L"Execution state authority must belong to the execution-domain descriptor.");
		}

		if (request->RequiresBundleLegality && !request->Descriptor->getHasBundleLegality())
		{
			return ExecutionDomainRuntimeResult::Denied(ExecutionDomainRuntimeDecision::MissingBundleLegalityDescriptor, L"Execution domain runtime requires a bundle-legality descriptor.");
		}

		if (request->RequiresSchedulingBudget && !request->Descriptor->getHasSchedulingBudget())
		{
			return ExecutionDomainRuntimeResult::Denied(ExecutionDomainRuntimeDecision::MissingSchedulingBudgetDescriptor, L"Execution domain runtime requires a scheduling-budget descriptor.");
		}

		if (request->RequiresCompatibilityProjection && !request->Descriptor->getCompatibilityProjectionEnabled())
		{
			return ExecutionDomainRuntimeResult::Denied(ExecutionDomainRuntimeDecision::CompatibilityProjectionDenied, L"Execution descriptor denies compatibility projection.");
		}

		return ExecutionDomainRuntimeResult::Allowed;
	}

	bool ExecutionDomainRuntime::CanAdmit(std::shared_ptr<ExecutionDomainRuntimeRequest> request)
	{
		return Validate(request)->isAllowed();
	}

	ExecutionDomainRuntime::SourceCapture::SourceCapture(std::shared_ptr<const unsigned char> issuerSeal, std::shared_ptr<ExecutionDomainDescriptor> sourceOwner, ExecutionDomainStateEpoch sourceEpoch, unsigned long long bindingGeneration, unsigned long long domainTag, unsigned long long addressSpaceTag, VmcsField field, unsigned long long value)
        : SourceOwner(std::move(sourceOwner)), SourceEpoch(sourceEpoch), BindingGeneration(bindingGeneration), DomainTag(domainTag), AddressSpaceTag(addressSpaceTag), Field(field), Value(value), _issuerSeal(std::move(issuerSeal)) {}

	std::shared_ptr<ExecutionDomainDescriptor> ExecutionDomainRuntime::SourceCapture::getSourceOwner() const
	{
		return SourceOwner;
	}

	ExecutionDomainStateEpoch ExecutionDomainRuntime::SourceCapture::getSourceEpoch() const
	{
		return SourceEpoch;
	}

	unsigned long long ExecutionDomainRuntime::SourceCapture::getBindingGeneration() const
	{
		return BindingGeneration;
	}

	unsigned long long ExecutionDomainRuntime::SourceCapture::getDomainTag() const
	{
		return DomainTag;
	}

	unsigned long long ExecutionDomainRuntime::SourceCapture::getAddressSpaceTag() const
	{
		return AddressSpaceTag;
	}

	VmcsField ExecutionDomainRuntime::SourceCapture::getField() const
	{
		return Field;
	}

	unsigned long long ExecutionDomainRuntime::SourceCapture::getValue() const
	{
		return Value;
	}

	bool ExecutionDomainRuntime::SourceCapture::isMaterialized() const
	{
		return getSourceEpoch().isMaterialized() && getBindingGeneration() != 0 && getDomainTag() != 0 && getAddressSpaceTag() != 0;
	}

	bool ExecutionDomainRuntime::SourceCapture::getRuntimeAuthorityGranted() const
	{
		return false;
	}

	bool ExecutionDomainRuntime::SourceCapture::isCapability() const
	{
		return false;
	}

	bool ExecutionDomainRuntime::SourceCapture::isReceipt() const
	{
		return false;
	}

	bool ExecutionDomainRuntime::SourceCapture::WasIssuedBy(const std::shared_ptr<const unsigned char> &issuerSeal) const
	{
// C# TO C++ CONVERTER TASK: Cannot test for object identity between 2 non-pointer objects in C++:
		return _issuerSeal != nullptr && _issuerSeal == issuerSeal;
	}

	ExecutionDomainStateEpoch ExecutionDomainRuntime::getCurrentSourceEpoch() const
	{
		{
		std::scoped_lock<std::mutex> lock(_sourceStateSync);
			return _currentSourceEpoch;
		}
	}

	std::shared_ptr<ExecutionDomainDescriptor> ExecutionDomainRuntime::getCurrentSourceDescriptor() const
	{
		{
		std::scoped_lock<std::mutex> lock(_sourceStateSync);
			return _currentSourceDescriptor;
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal ExecutionDomainSourceBindResult BindAuthoritativeReadOnlyState(ExecutionDomainDescriptor? descriptor, ulong addressSpaceTag)
	std::shared_ptr<ExecutionDomainSourceBindResult> ExecutionDomainRuntime::BindAuthoritativeReadOnlyState(std::shared_ptr<ExecutionDomainDescriptor> descriptor, unsigned long long addressSpaceTag)
	{
		return BindSource(descriptor, addressSpaceTag, L"bound");
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal ExecutionDomainSourceBindResult ReplaceAuthoritativeReadOnlyState(ExecutionDomainDescriptor? descriptor, ulong addressSpaceTag)
	std::shared_ptr<ExecutionDomainSourceBindResult> ExecutionDomainRuntime::ReplaceAuthoritativeReadOnlyState(std::shared_ptr<ExecutionDomainDescriptor> descriptor, unsigned long long addressSpaceTag)
	{
		return BindSource(descriptor, addressSpaceTag, L"replaced");
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal ExecutionDomainSourceBindResult RebindAuthoritativeReadOnlyStateAfterRestore(ExecutionDomainDescriptor? descriptor, ulong addressSpaceTag)
	std::shared_ptr<ExecutionDomainSourceBindResult> ExecutionDomainRuntime::RebindAuthoritativeReadOnlyStateAfterRestore(std::shared_ptr<ExecutionDomainDescriptor> descriptor, unsigned long long addressSpaceTag)
	{
		return BindSource(descriptor, addressSpaceTag, L"rebound after restore");
	}

	void ExecutionDomainRuntime::UnbindAuthoritativeReadOnlyState()
	{
		{
			std::scoped_lock<std::mutex> lock(_sourceStateSync);
			AdvanceSourceEpoch();
			AdvanceBindingGeneration();
			_currentSourceDescriptor = nullptr;
			_currentSourceEpoch = YAKSys_Hybrid_CPU::Core::ExecutionDomainStateEpoch();
			_currentSourceAddressSpaceTag = 0;
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal ExecutionDomainSourceCaptureResult CaptureVmReadScalarSource(ExecutionDomainDescriptor? presentedDescriptor, ulong runtimeDomainTag, ulong runtimeAddressSpaceTag, VmcsField field)
	std::shared_ptr<ExecutionDomainSourceCaptureResult> ExecutionDomainRuntime::CaptureVmReadScalarSource(std::shared_ptr<ExecutionDomainDescriptor> presentedDescriptor, unsigned long long runtimeDomainTag, unsigned long long runtimeAddressSpaceTag, VmcsField field)
	{
		{
			std::scoped_lock<std::mutex> lock(_sourceStateSync);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: ExecutionDomainDescriptor? current = _currentSourceDescriptor;
			std::shared_ptr<ExecutionDomainDescriptor> current = _currentSourceDescriptor;
			if (current == nullptr)
			{
				return DenyCapture(ExecutionDomainSourceCaptureDecision::SourceUnbound, L"Execution-domain read-only source is not bound.");
			}
			if (presentedDescriptor != current)
			{
				return DenyCapture(ExecutionDomainSourceCaptureDecision::StaleOrForeignDescriptor, L"Presented execution-domain descriptor is stale or belongs to another runtime binding.");
			}
			if (runtimeDomainTag == 0 || current->getDomainTag() != runtimeDomainTag)
			{
				return DenyCapture(ExecutionDomainSourceCaptureDecision::DomainMismatch, L"Execution-domain source and runtime domain identities must be non-zero and equal.");
			}
			if (runtimeAddressSpaceTag == 0 || _currentSourceAddressSpaceTag != runtimeAddressSpaceTag)
			{
				return DenyCapture(ExecutionDomainSourceCaptureDecision::AddressSpaceMismatch, L"Execution-domain source and runtime address-space identities must be non-zero and equal.");
			}
			if (!_currentSourceEpoch.isMaterialized())
			{
				return DenyCapture(ExecutionDomainSourceCaptureDecision::SourceEpochUnmaterialized, L"Authoritative execution-domain source epoch is not materialized.");
			}

			std::shared_ptr<ExecutionDomainReadOnlyStateView> view = current->getReadOnlyState();
			if (view->StateEpoch == 0 || view->StateEpoch != _currentSourceEpoch.getValue())
			{
				return DenyCapture(ExecutionDomainSourceCaptureDecision::SourceEpochMismatch, L"Execution-domain view epoch does not match the live authoritative source epoch.");
			}
			unsigned long long value;
			bool knownField;
			if (!TryReadExactField(view, field, value, knownField))
			{
				return DenyCapture(knownField ? ExecutionDomainSourceCaptureDecision::FieldUnmaterialized : ExecutionDomainSourceCaptureDecision::FieldDenied, knownField ? L"Execution-domain source does not materialize the requested VMCS field." : L"Execution-domain source capture denies the requested VMCS field.");
			}

			std::shared_ptr<SourceCapture> capture = std::make_shared<SourceCapture>(_sourceCaptureIssuerSeal, current, _currentSourceEpoch, _sourceBindingGeneration, runtimeDomainTag, runtimeAddressSpaceTag, field, value);
			return std::make_shared<ExecutionDomainSourceCaptureResult>(ExecutionDomainSourceCaptureDecision::Captured, capture, L"Exact execution-domain scalar source was captured atomically with its owner and epoch.");
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool IsAuthenticCapture(SourceCapture? capture)
	bool ExecutionDomainRuntime::IsAuthenticCapture(std::shared_ptr<SourceCapture> capture)
	{
		return capture != nullptr && capture->isMaterialized() && capture->WasIssuedBy(_sourceCaptureIssuerSeal);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private ExecutionDomainSourceBindResult BindSource(ExecutionDomainDescriptor? descriptor, ulong addressSpaceTag, string action)
	std::shared_ptr<ExecutionDomainSourceBindResult> ExecutionDomainRuntime::BindSource(std::shared_ptr<ExecutionDomainDescriptor> descriptor, unsigned long long addressSpaceTag, const std::wstring &action)
	{
		{
			std::scoped_lock<std::mutex> lock(_sourceStateSync);
			if (descriptor == nullptr)
			{
				return DenyBind(ExecutionDomainSourceBindDecision::MissingDescriptor, L"Execution-domain source binding requires a descriptor.");
			}
			if (!descriptor->isAuthoritativeExecutionStateOwner())
			{
				return DenyBind(ExecutionDomainSourceBindDecision::DescriptorAuthorityDenied, L"Execution-domain descriptor must retain canonical execution-state ownership.");
			}
			if (!descriptor->getCompatibilityProjectionEnabled())
			{
				return DenyBind(ExecutionDomainSourceBindDecision::CompatibilityProjectionDenied, L"Execution-domain descriptor disables compatibility projection.");
			}
			if (descriptor->getDomainTag() == 0)
			{
				return DenyBind(ExecutionDomainSourceBindDecision::MissingDomainIdentity, L"Execution-domain source binding requires a non-zero descriptor domain tag.");
			}
			if (addressSpaceTag == 0)
			{
				return DenyBind(ExecutionDomainSourceBindDecision::MissingAddressSpaceIdentity, L"Execution-domain source binding requires a non-zero address-space tag.");
			}
			const auto sourceView = descriptor->getReadOnlyState();
			if (sourceView == nullptr || !sourceView->getHasCompleteGuestPcSpFlags())
			{
				return DenyBind(ExecutionDomainSourceBindDecision::IncompleteSourceState, L"Execution-domain source binding requires complete materialized GuestPc/GuestSp/GuestFlags state.");
			}

			ExecutionDomainStateEpoch epoch = AdvanceSourceEpoch();
			unsigned long long bindingGeneration = AdvanceBindingGeneration();
			std::shared_ptr<ExecutionDomainReadOnlyStateView> canonicalView = std::make_shared<ExecutionDomainReadOnlyStateView>(sourceView->GuestPc, sourceView->GuestSp, sourceView->GuestFlags, sourceView->HasMaterializedGuestPc, sourceView->HasMaterializedGuestSp, sourceView->HasMaterializedGuestFlags, epoch.getValue());
			std::shared_ptr<ExecutionDomainDescriptor> canonicalDescriptor = descriptor->WithReadOnlyState(canonicalView);
			_currentSourceDescriptor = canonicalDescriptor;
			_currentSourceEpoch = epoch;
			_currentSourceAddressSpaceTag = addressSpaceTag;
			return std::make_shared<ExecutionDomainSourceBindResult>(ExecutionDomainSourceBindDecision::Bound, canonicalDescriptor, epoch, L"Authoritative execution-domain read-only source was " + action + L" at runtime epoch " + std::to_wstring(epoch.getValue()) + L" and binding generation " + std::to_wstring(bindingGeneration) + L".");
		}
	}

	ExecutionDomainStateEpoch ExecutionDomainRuntime::AdvanceSourceEpoch()
	{
//		unchecked
			_sourceEpochCounter++;
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
		if (_sourceEpochCounter == 0)
		{
			_sourceEpochCounter = 1;
		}
		return ExecutionDomainStateEpoch(_sourceEpochCounter);
	}

	unsigned long long ExecutionDomainRuntime::AdvanceBindingGeneration()
	{
//		unchecked
			_sourceBindingGeneration++;
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
		if (_sourceBindingGeneration == 0)
		{
			_sourceBindingGeneration = 1;
		}
		return _sourceBindingGeneration;
	}

	inline bool ExecutionDomainRuntime::TryReadExactField(std::shared_ptr<ExecutionDomainReadOnlyStateView> view, VmcsField field, unsigned long long &value, bool &knownField)
	{
		knownField = true;
		switch (field)
		{
            case VmcsField::GuestPc:
                if (view->HasMaterializedGuestPc) { value = view->GuestPc; return true; }
                value = 0; return false;
            case VmcsField::GuestSp:
                if (view->HasMaterializedGuestSp) { value = view->GuestSp; return true; }
                value = 0; return false;
            case VmcsField::GuestFlags:
                if (view->HasMaterializedGuestFlags) { value = view->GuestFlags; return true; }
                value = 0; return false;
			default:
				knownField = false;
				value = 0;
				return false;
		}
	}

	std::shared_ptr<ExecutionDomainSourceBindResult> ExecutionDomainRuntime::DenyBind(ExecutionDomainSourceBindDecision decision, const std::wstring &reason)
	{
		return std::make_shared<ExecutionDomainSourceBindResult>(decision, nullptr, ExecutionDomainStateEpoch(), reason);
	}

	std::shared_ptr<ExecutionDomainSourceCaptureResult> ExecutionDomainRuntime::DenyCapture(ExecutionDomainSourceCaptureDecision decision, const std::wstring &reason)
	{
		return std::make_shared<ExecutionDomainSourceCaptureResult>(decision, nullptr, reason);
	}
}
