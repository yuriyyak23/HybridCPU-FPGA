#include "YAKSys_Hybrid_CPU.Core.MemoryDomainRuntime.h"
#include "MemoryDomainRuntime.CpuSecondStage.h"
#include "../../../Memory/AddressSpaces/YAKSys_Hybrid_CPU.Core.AddressSpaceDescriptor.h"
#include "MemoryDomainRuntime.h"
#include <utility>

using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core
{
	namespace
	{
		bool SameIssuerSeal(const std::any& left, const std::any& right)
		{
			const auto* leftSeal = std::any_cast<std::shared_ptr<const unsigned char>>(&left);
			const auto* rightSeal = std::any_cast<std::shared_ptr<const unsigned char>>(&right);
			return leftSeal != nullptr && rightSeal != nullptr && *leftSeal == *rightSeal;
		}
	}

	MemoryDomainRuntime::SourceCapture::SourceCapture(std::any issuerSeal, std::shared_ptr<MemoryDomainDescriptor> sourceOwner, unsigned long long addressSpaceGeneration, unsigned long long bindingGeneration, unsigned long long domainTag, unsigned long long runtimeAddressSpaceTag, VmcsField field, unsigned long long value)
		: SourceOwner(std::move(sourceOwner)), AddressSpaceGeneration(addressSpaceGeneration), BindingGeneration(bindingGeneration), DomainTag(domainTag), RuntimeAddressSpaceTag(runtimeAddressSpaceTag), Field(field), Value(value), _issuerSeal(std::move(issuerSeal))
	{
	}

	std::shared_ptr<MemoryDomainDescriptor> MemoryDomainRuntime::SourceCapture::getSourceOwner() const
	{
		return SourceOwner;
	}

	unsigned long long MemoryDomainRuntime::SourceCapture::getAddressSpaceGeneration() const
	{
		return AddressSpaceGeneration;
	}

	unsigned long long MemoryDomainRuntime::SourceCapture::getBindingGeneration() const
	{
		return BindingGeneration;
	}

	unsigned long long MemoryDomainRuntime::SourceCapture::getDomainTag() const
	{
		return DomainTag;
	}

	unsigned long long MemoryDomainRuntime::SourceCapture::getRuntimeAddressSpaceTag() const
	{
		return RuntimeAddressSpaceTag;
	}

	VmcsField MemoryDomainRuntime::SourceCapture::getField() const
	{
		return Field;
	}

	unsigned long long MemoryDomainRuntime::SourceCapture::getValue() const
	{
		return Value;
	}

	bool MemoryDomainRuntime::SourceCapture::getRuntimeAuthorityGranted() const
	{
		return false;
	}

	bool MemoryDomainRuntime::SourceCapture::isReceipt() const
	{
		return false;
	}

	bool MemoryDomainRuntime::SourceCapture::WasIssuedBy(std::any seal)
	{
// C# TO C++ CONVERTER TASK: Cannot test for object identity between 2 non-pointer objects in C++:
		return SameIssuerSeal(_issuerSeal, seal);
	}

	std::shared_ptr<MemoryDomainDescriptor> MemoryDomainRuntime::getCurrentTranslationSource() const
	{
		{
		std::scoped_lock<std::mutex> lock(_translationSourceSync);
			return _currentTranslationSource;
		}
	}

	unsigned long long MemoryDomainRuntime::getCurrentAddressSpaceGeneration() const
	{
		{
		std::scoped_lock<std::mutex> lock(_translationSourceSync);
			return _currentAddressSpaceGeneration;
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal MemoryDomainSourceBindResult BindAuthoritativeTranslationView(MemoryDomainDescriptor? descriptor, ulong runtimeAddressSpaceTag)
	std::shared_ptr<MemoryDomainSourceBindResult> MemoryDomainRuntime::BindAuthoritativeTranslationView(std::shared_ptr<MemoryDomainDescriptor> descriptor, unsigned long long runtimeAddressSpaceTag)
	{
		InvalidateCpuSecondStageLifecycleBeforeSourceChange();
		return BindSource(descriptor, runtimeAddressSpaceTag, L"bound");
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal MemoryDomainSourceBindResult ReplaceAuthoritativeTranslationView(MemoryDomainDescriptor? descriptor, ulong runtimeAddressSpaceTag)
	std::shared_ptr<MemoryDomainSourceBindResult> MemoryDomainRuntime::ReplaceAuthoritativeTranslationView(std::shared_ptr<MemoryDomainDescriptor> descriptor, unsigned long long runtimeAddressSpaceTag)
	{
		InvalidateCpuSecondStageLifecycleBeforeSourceChange();
		return BindSource(descriptor, runtimeAddressSpaceTag, L"replaced");
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal MemoryDomainSourceBindResult RebindAuthoritativeTranslationViewAfterRestore(MemoryDomainDescriptor? descriptor, ulong runtimeAddressSpaceTag)
	std::shared_ptr<MemoryDomainSourceBindResult> MemoryDomainRuntime::RebindAuthoritativeTranslationViewAfterRestore(std::shared_ptr<MemoryDomainDescriptor> descriptor, unsigned long long runtimeAddressSpaceTag)
	{
		InvalidateCpuSecondStageLifecycleBeforeSourceChange();
		return BindSource(descriptor, runtimeAddressSpaceTag, L"rebound after restore");
	}

	void MemoryDomainRuntime::UnbindAuthoritativeTranslationView()
	{
		InvalidateCpuSecondStageLifecycleBeforeSourceChange();
		{
			std::scoped_lock<std::mutex> lock(_translationSourceSync);
			AdvanceAddressSpaceGeneration();
			AdvanceBindingGeneration();
			_currentTranslationSource = nullptr;
			_currentAddressSpaceGeneration = 0;
			_currentRuntimeAddressSpaceTag = 0;
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal MemoryDomainSourceCaptureResult CaptureVmReadScalarSource(MemoryDomainDescriptor? presentedDescriptor, ulong runtimeDomainTag, ulong runtimeAddressSpaceTag, VmcsField field)
	std::shared_ptr<MemoryDomainSourceCaptureResult> MemoryDomainRuntime::CaptureVmReadScalarSource(std::shared_ptr<MemoryDomainDescriptor> presentedDescriptor, unsigned long long runtimeDomainTag, unsigned long long runtimeAddressSpaceTag, VmcsField field)
	{
		{
			std::scoped_lock<std::mutex> lock(_translationSourceSync);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MemoryDomainDescriptor? current = _currentTranslationSource;
			std::shared_ptr<MemoryDomainDescriptor> current = _currentTranslationSource;
			if (current == nullptr)
			{
				return DenyCapture(MemoryDomainSourceCaptureDecision::SourceUnbound, L"Memory-domain translation source is not bound.");
			}
			if (presentedDescriptor != current)
			{
				return DenyCapture(MemoryDomainSourceCaptureDecision::StaleOrForeignDescriptor, L"Presented memory-domain descriptor is stale or foreign.");
			}
			std::shared_ptr<MemoryDomainTranslationControl> control = current->getTranslationControl();
			if (runtimeDomainTag == 0 || control->DomainTag != runtimeDomainTag)
			{
				return DenyCapture(MemoryDomainSourceCaptureDecision::DomainMismatch, L"Memory source and runtime domain identities must be non-zero and equal.");
			}
			if (runtimeAddressSpaceTag == 0 || runtimeAddressSpaceTag != _currentRuntimeAddressSpaceTag)
			{
				return DenyCapture(MemoryDomainSourceCaptureDecision::AddressSpaceMismatch, L"Memory source and runtime address-space identities must be non-zero and equal.");
			}
			if (_currentAddressSpaceGeneration == 0)
			{
				return DenyCapture(MemoryDomainSourceCaptureDecision::GenerationUnmaterialized, L"Runtime-owned address-space generation is not materialized.");
			}
			if (control->AddressSpaceGeneration != _currentAddressSpaceGeneration)
			{
				return DenyCapture(MemoryDomainSourceCaptureDecision::GenerationMismatch, L"Translation view generation does not match the live MemoryDomain generation.");
			}

			unsigned long long value;
			MemoryDomainSourceCaptureDecision denied;
			std::wstring reason;
			switch (field)
			{
				case VmcsField::GuestCr3:
					value = control->AddressSpaceRoot;
					break;
// C# TO C++ CONVERTER TASK: The 'when' keyword in 'case' statements is not converted by C# to C++ Converter:
// ORIGINAL LINE: case VmcsField.EptPointer when current.OwnsSecondStageTranslation && control.SecondStageRoot != 0:
				case VmcsField::EptPointer:
					if (current->getOwnsSecondStageTranslation() && control->SecondStageRoot != 0)
					{
						value = control->SecondStageRoot;
						break;
					}
					denied = MemoryDomainSourceCaptureDecision::SecondStageDenied;
					reason = L"EptPointer requires owned valid second-stage translation.";
					return DenyCapture(denied, reason);
// C# TO C++ CONVERTER TASK: The 'when' keyword in 'case' statements is not converted by C# to C++ Converter:
// ORIGINAL LINE: case VmcsField.Vpid when control.AddressSpaceTaggingEnabled && control.AddressSpaceTag != 0:
				case VmcsField::Vpid:
					if (control->AddressSpaceTaggingEnabled && control->AddressSpaceTag != 0)
					{
						value = control->AddressSpaceTag;
						break;
					}
					return DenyCapture(MemoryDomainSourceCaptureDecision::TaggingDenied, L"Vpid requires enabled tagging and a non-zero tag.");
// C# TO C++ CONVERTER TASK: The 'when' keyword in 'case' statements is not converted by C# to C++ Converter:
// ORIGINAL LINE: case VmcsField.Cr3TargetCount when control.AddressSpaceTargetCount <= MemoryDomainTranslationControl.MaxAddressSpaceTargetCount:
				case VmcsField::Cr3TargetCount:
					if (control->AddressSpaceTargetCount <= MemoryDomainTranslationControl::MaxAddressSpaceTargetCount)
					{
						value = control->AddressSpaceTargetCount;
						break;
					}
					return DenyCapture(MemoryDomainSourceCaptureDecision::TargetCountDenied, L"Cr3TargetCount exceeds the canonical target-count bound.");
				default:
					return DenyCapture(MemoryDomainSourceCaptureDecision::FieldDenied, L"Memory-domain VMREAD source capture denies the requested field.");
			}

			return std::make_shared<MemoryDomainSourceCaptureResult>(MemoryDomainSourceCaptureResult{MemoryDomainSourceCaptureDecision::Captured, std::make_shared<SourceCapture>(_translationCaptureIssuerSeal, current, _currentAddressSpaceGeneration, _translationBindingGeneration, runtimeDomainTag, runtimeAddressSpaceTag, field, value), L"Exact memory-domain scalar source was captured atomically with owner and generation."});
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool IsAuthenticCapture(SourceCapture? capture)
	bool MemoryDomainRuntime::IsAuthenticCapture(std::shared_ptr<SourceCapture> capture)
	{
		return capture != nullptr && capture->getAddressSpaceGeneration() != 0 && capture->getBindingGeneration() != 0 && capture->WasIssuedBy(_translationCaptureIssuerSeal);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private MemoryDomainSourceBindResult BindSource(MemoryDomainDescriptor? descriptor, ulong runtimeAddressSpaceTag, string action)
	std::shared_ptr<MemoryDomainSourceBindResult> MemoryDomainRuntime::BindSource(std::shared_ptr<MemoryDomainDescriptor> descriptor, unsigned long long runtimeAddressSpaceTag, const std::wstring &action)
	{
		{
			std::scoped_lock<std::mutex> lock(_translationSourceSync);
			if (descriptor == nullptr)
			{
				return DenyBind(MemoryDomainSourceBindDecision::MissingDescriptor, L"Memory-domain source binding requires a descriptor.");
			}
			if (!descriptor->getHasValidTranslationControl())
			{
				return DenyBind(MemoryDomainSourceBindDecision::InvalidTranslationControl, L"Memory-domain source binding requires valid translation control.");
			}
			if (!descriptor->getTranslationControl()->TranslationEnabled)
			{
				return DenyBind(MemoryDomainSourceBindDecision::TranslationDisabled, L"Memory-domain source binding requires enabled translation.");
			}
			if (descriptor->getTranslationControl()->DomainTag == 0)
			{
				return DenyBind(MemoryDomainSourceBindDecision::MissingDomainIdentity, L"Memory-domain source binding requires a non-zero domain identity.");
			}
			if (runtimeAddressSpaceTag == 0)
			{
				return DenyBind(MemoryDomainSourceBindDecision::MissingAddressSpaceIdentity, L"Memory-domain source binding requires a non-zero runtime address-space identity.");
			}

			unsigned long long generation = AdvanceAddressSpaceGeneration();
			unsigned long long bindingGeneration = AdvanceBindingGeneration();
// C# TO C++ CONVERTER TASK: C# 'with expressions' are not converted by C# to C++ Converter:
//			MemoryDomainTranslationControl canonicalControl = descriptor.TranslationControl with { AddressSpaceGeneration = generation};
			std::shared_ptr<MemoryDomainTranslationControl> canonicalControl = std::make_shared<MemoryDomainTranslationControl>(*descriptor->getTranslationControl());
			canonicalControl->AddressSpaceGeneration = generation;
			std::shared_ptr<MemoryDomainDescriptor> canonicalDescriptor = descriptor->WithTranslationControl(canonicalControl);
			_currentTranslationSource = canonicalDescriptor;
			_currentAddressSpaceGeneration = generation;
			_currentRuntimeAddressSpaceTag = runtimeAddressSpaceTag;
			const std::wstring reason = L"Authoritative memory-domain translation source was " + action + L" at generation " + std::to_wstring(generation) + L" and binding " + std::to_wstring(bindingGeneration) + L".";
			return std::make_shared<MemoryDomainSourceBindResult>(MemoryDomainSourceBindResult{MemoryDomainSourceBindDecision::Bound, canonicalDescriptor, generation, reason});
		}
	}

	unsigned long long MemoryDomainRuntime::AdvanceAddressSpaceGeneration()
	{
//		unchecked
			_addressSpaceGenerationCounter++;
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
		if (_addressSpaceGenerationCounter == 0)
		{
			_addressSpaceGenerationCounter = 1;
		}
		return _addressSpaceGenerationCounter;
	}

	unsigned long long MemoryDomainRuntime::AdvanceBindingGeneration()
	{
//		unchecked
			_translationBindingGeneration++;
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
		if (_translationBindingGeneration == 0)
		{
			_translationBindingGeneration = 1;
		}
		return _translationBindingGeneration;
	}

	std::shared_ptr<MemoryDomainSourceBindResult> MemoryDomainRuntime::DenyBind(MemoryDomainSourceBindDecision decision, const std::wstring &reason)
	{
		return std::make_shared<MemoryDomainSourceBindResult>(MemoryDomainSourceBindResult{decision, nullptr, 0, reason});
	}

	std::shared_ptr<MemoryDomainSourceCaptureResult> MemoryDomainRuntime::DenyCapture(MemoryDomainSourceCaptureDecision decision, const std::wstring &reason)
	{
		return std::make_shared<MemoryDomainSourceCaptureResult>(MemoryDomainSourceCaptureResult{decision, nullptr, reason});
	}

	MemoryDomainRuntime::CpuSecondStageTranslationBinding::CpuSecondStageTranslationBinding(std::any issuerSeal, std::shared_ptr<MemoryDomainRuntime> issuer, std::shared_ptr<MemoryDomainDescriptor> sourceOwner, unsigned long long runtimeAddressSpaceIdentity, unsigned long long addressSpaceGeneration, unsigned long long bindingGeneration)
		: Issuer(std::move(issuer)), SourceOwner(std::move(sourceOwner)), RuntimeAddressSpaceIdentity(runtimeAddressSpaceIdentity), AddressSpaceGeneration(addressSpaceGeneration), BindingGeneration(bindingGeneration), _issuerSeal(std::move(issuerSeal))
	{
	}

	std::shared_ptr<MemoryDomainRuntime> MemoryDomainRuntime::CpuSecondStageTranslationBinding::getIssuer() const
	{
		return Issuer;
	}

	std::shared_ptr<MemoryDomainDescriptor> MemoryDomainRuntime::CpuSecondStageTranslationBinding::getSourceOwner() const
	{
		return SourceOwner;
	}

	unsigned long long MemoryDomainRuntime::CpuSecondStageTranslationBinding::getRuntimeAddressSpaceIdentity() const
	{
		return RuntimeAddressSpaceIdentity;
	}

	unsigned long long MemoryDomainRuntime::CpuSecondStageTranslationBinding::getAddressSpaceGeneration() const
	{
		return AddressSpaceGeneration;
	}

	unsigned long long MemoryDomainRuntime::CpuSecondStageTranslationBinding::getBindingGeneration() const
	{
		return BindingGeneration;
	}

	bool MemoryDomainRuntime::CpuSecondStageTranslationBinding::WasIssuedBy(std::any seal)
	{
// C# TO C++ CONVERTER TASK: Cannot test for object identity between 2 non-pointer objects in C++:
		return SameIssuerSeal(_issuerSeal, seal);
	}

	void MemoryDomainRuntime::RegisterCpuSecondStageCompletionInvalidation(std::shared_ptr<CpuSecondStageTranslationBinding> binding, std::function<void()> invalidation)
	{
		ArgumentNullException::ThrowIfNull(binding);
		ArgumentNullException::ThrowIfNull(invalidation);
		{
			std::scoped_lock<std::mutex> lock(_translationSourceSync);
			if (binding->getIssuer() != shared_from_this() || !binding->WasIssuedBy(_cpuSecondStageIssuerSeal) || binding->getSourceOwner() != _currentTranslationSource || binding->getBindingGeneration() != _translationBindingGeneration)
			{
				throw InvalidOperationException(L"CPU second-stage lifecycle registration requires the exact live MemoryDomain binding.");
			}
			_cpuSecondStageLifecycleInvalidations.push_back(invalidation);
		}
	}

	void MemoryDomainRuntime::InvalidateCpuSecondStageLifecycleBeforeSourceChange()
	{
		std::vector<std::function<void()>> invalidations;
		{
			std::scoped_lock<std::mutex> lock(_translationSourceSync);
			invalidations = _cpuSecondStageLifecycleInvalidations;
			_cpuSecondStageLifecycleInvalidations.clear();
		}
		for (std::size_t index = 0; index < invalidations.size(); index++)
		{
			invalidations[index]();
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal CpuSecondStageSourceBindResult BindCanonicalCpuSecondStageTranslation(MemoryDomainDescriptor? descriptor, ulong runtimeAddressSpaceIdentity, ulong expectedDomainIdentity)
	std::shared_ptr<CpuSecondStageSourceBindResult> MemoryDomainRuntime::BindCanonicalCpuSecondStageTranslation(std::shared_ptr<MemoryDomainDescriptor> descriptor, unsigned long long runtimeAddressSpaceIdentity, unsigned long long expectedDomainIdentity)
	{
		std::shared_ptr<MemoryDomainRuntimeResult> admission = Validate(std::make_shared<MemoryDomainRuntimeRequest>(MemoryDomainRuntimeRequest{descriptor, true, true, true, false}));
		if (!admission->isAllowed())
		{
			return std::make_shared<CpuSecondStageSourceBindResult>(CpuSecondStageSourceBindResult{CpuSecondStageSourceBindDecision::AdmissionDenied, nullptr, admission->Reason});
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: if (descriptor!.TranslationPolicy is not { IsRuntimeAuthoritative: true } || descriptor.AddressSpace is not { IsRuntimeAuthoritative: true })
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if (descriptor == nullptr || descriptor->getTranslationPolicy() == nullptr || !descriptor->getTranslationPolicy()->isRuntimeAuthoritative() || descriptor->getAddressSpace() == nullptr || !descriptor->getAddressSpace()->isRuntimeAuthoritative())
		{
			return std::make_shared<CpuSecondStageSourceBindResult>(CpuSecondStageSourceBindResult{CpuSecondStageSourceBindDecision::AdmissionDenied, nullptr, L"CPU second-stage translation requires runtime-owned memory policy and address space."});
		}

		std::shared_ptr<MemoryDomainTranslationControl> control = descriptor->getTranslationControl();
		if (control->SecondStageRoot == 0)
		{
			return std::make_shared<CpuSecondStageSourceBindResult>(CpuSecondStageSourceBindResult{CpuSecondStageSourceBindDecision::MissingSecondStageRoot, nullptr, L"CPU second-stage translation requires a non-zero canonical MemoryDomain second-stage root."});
		}

		if (expectedDomainIdentity == 0 || control->DomainTag != expectedDomainIdentity)
		{
			return std::make_shared<CpuSecondStageSourceBindResult>(CpuSecondStageSourceBindResult{CpuSecondStageSourceBindDecision::ScopeMismatch, nullptr, L"CPU translation scope must match the canonical MemoryDomain identity."});
		}

		std::shared_ptr<MemoryDomainSourceBindResult> bound = BindAuthoritativeTranslationView(descriptor, runtimeAddressSpaceIdentity);
		if (!bound->isBound())
		{
			return std::make_shared<CpuSecondStageSourceBindResult>(CpuSecondStageSourceBindResult{CpuSecondStageSourceBindDecision::SourceBindDenied, nullptr, bound->Reason});
		}

		{
			std::scoped_lock<std::mutex> lock(_translationSourceSync);
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: MemoryDomainDescriptor canonical = bound.Descriptor!;
			std::shared_ptr<MemoryDomainDescriptor> canonical = bound->Descriptor;
			std::shared_ptr<CpuSecondStageTranslationBinding> binding = std::make_shared<CpuSecondStageTranslationBinding>(_cpuSecondStageIssuerSeal, shared_from_this(), canonical, runtimeAddressSpaceIdentity, _currentAddressSpaceGeneration, _translationBindingGeneration);
			return std::make_shared<CpuSecondStageSourceBindResult>(CpuSecondStageSourceBindResult{CpuSecondStageSourceBindDecision::Bound, binding, L"Canonical MemoryDomainRuntime issued an exact CPU second-stage source binding."});
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool TryCaptureCanonicalCpuSecondStageTranslation(CpuSecondStageTranslationBinding? binding, ulong expectedDomainIdentity, out CpuSecondStageTranslationSnapshot snapshot)
	bool MemoryDomainRuntime::TryCaptureCanonicalCpuSecondStageTranslation(std::shared_ptr<CpuSecondStageTranslationBinding> binding, unsigned long long expectedDomainIdentity, std::shared_ptr<CpuSecondStageTranslationSnapshot> &snapshot)
	{
		{
			std::scoped_lock<std::mutex> lock(_translationSourceSync);
			snapshot = nullptr;
			if (binding == nullptr || binding->getIssuer() != shared_from_this() || !binding->WasIssuedBy(_cpuSecondStageIssuerSeal) || binding->getSourceOwner() != _currentTranslationSource || binding->getAddressSpaceGeneration() != _currentAddressSpaceGeneration || binding->getBindingGeneration() != _translationBindingGeneration || binding->getRuntimeAddressSpaceIdentity() != _currentRuntimeAddressSpaceTag)
			{
				return false;
			}

			std::shared_ptr<MemoryDomainDescriptor> source = binding->getSourceOwner();
			std::shared_ptr<MemoryDomainTranslationControl> control = source->getTranslationControl();
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: AddressSpaceDescriptor? addressSpace = source.AddressSpace;
			std::shared_ptr<AddressSpaceDescriptor> addressSpace = source->getAddressSpace();
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
			if (!source->getOwnsSecondStageTranslation() || source->getTranslationPolicy() == nullptr || !source->getTranslationPolicy()->isRuntimeAuthoritative() || addressSpace == nullptr || !addressSpace->isRuntimeAuthoritative() || !control->TranslationEnabled || control->SecondStageRoot == 0 || control->DomainTag == 0 || control->DomainTag != expectedDomainIdentity || control->AddressSpaceGeneration != _currentAddressSpaceGeneration)
			{
				return false;
			}

			snapshot = std::make_shared<CpuSecondStageTranslationSnapshot>(source, control->SecondStageRoot, control->DomainTag, binding->getRuntimeAddressSpaceIdentity(), binding->getBindingGeneration(), _currentAddressSpaceGeneration, binding->getBindingGeneration(), addressSpace);
			return snapshot->isValid();
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool IsCurrentCanonicalCpuSecondStageTranslation(CpuSecondStageTranslationBinding? binding, in CpuSecondStageTranslationSnapshot snapshot)
	bool MemoryDomainRuntime::IsCurrentCanonicalCpuSecondStageTranslation(std::shared_ptr<CpuSecondStageTranslationBinding> binding, std::shared_ptr<CpuSecondStageTranslationSnapshot> snapshot)
	{
		{
			std::scoped_lock<std::mutex> lock(_translationSourceSync);
			return binding != nullptr && snapshot != nullptr && snapshot->isValid() && binding->getIssuer() == shared_from_this() && binding->WasIssuedBy(_cpuSecondStageIssuerSeal) && binding->getSourceOwner() == snapshot->SourceOwner && _currentTranslationSource == snapshot->SourceOwner && binding->getAddressSpaceGeneration() == snapshot->AddressSpaceGeneration && binding->getBindingGeneration() == snapshot->BindingGeneration && _currentAddressSpaceGeneration == snapshot->AddressSpaceGeneration && _translationBindingGeneration == snapshot->BindingGeneration && _currentRuntimeAddressSpaceTag == snapshot->AddressSpaceIdentity;
		}
	}

	std::shared_ptr<MemoryDomainRuntimeResult> MemoryDomainRuntime::Validate(std::shared_ptr<MemoryDomainRuntimeRequest> request)
	{
		if (request == nullptr || request->Descriptor == nullptr)
		{
			return MemoryDomainRuntimeResult::Denied(MemoryDomainRuntimeDecision::MissingDescriptor, L"Memory domain runtime requires a memory-domain descriptor.");
		}

		if (!request->Descriptor->isAuthoritativeMemoryStateOwner())
		{
			return MemoryDomainRuntimeResult::Denied(MemoryDomainRuntimeDecision::DescriptorAuthorityDenied, L"Memory state authority must belong to the memory-domain descriptor.");
		}

		if (request->RequiresAddressSpace && !request->Descriptor->getHasAddressSpace())
		{
			return MemoryDomainRuntimeResult::Denied(MemoryDomainRuntimeDecision::MissingAddressSpace, L"Memory domain runtime requires an address-space descriptor.");
		}

		if (request->RequiresTranslationPolicy && !request->Descriptor->getHasTranslationPolicy())
		{
			return MemoryDomainRuntimeResult::Denied(MemoryDomainRuntimeDecision::MissingTranslationPolicy, L"Memory domain runtime requires a translation policy.");
		}

		if (!request->Descriptor->getHasValidTranslationControl())
		{
			return MemoryDomainRuntimeResult::Denied(MemoryDomainRuntimeDecision::InvalidTranslationControl, L"Memory translation control is invalid.");
		}

		if (request->RequiresSecondStageTranslation && !request->Descriptor->getOwnsSecondStageTranslation())
		{
			return MemoryDomainRuntimeResult::Denied(MemoryDomainRuntimeDecision::SecondStageTranslationDenied, L"Second-stage translation authority is not owned by the memory-domain descriptor.");
		}

		if (request->RequiresDirtyTracking && !request->Descriptor->getHasDirtyTracking())
		{
			return MemoryDomainRuntimeResult::Denied(MemoryDomainRuntimeDecision::MissingDirtyTrackingDescriptor, L"Memory domain runtime requires a dirty-tracking descriptor.");
		}

		return MemoryDomainRuntimeResult::Allowed;
	}

	bool MemoryDomainRuntime::CanAdmit(std::shared_ptr<MemoryDomainRuntimeRequest> request)
	{
		return Validate(request)->isAllowed();
	}
}
