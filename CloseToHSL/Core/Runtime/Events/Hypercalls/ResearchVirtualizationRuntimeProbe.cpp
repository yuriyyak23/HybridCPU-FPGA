#include "ResearchVirtualizationRuntimeProbe.h"

namespace YAKSys_Hybrid_CPU::Core
{

#if defined(TESTING)
	ResearchVirtualizationOperationContext::ResearchVirtualizationOperationContext(int virtualThreadId, int ownerContextId, unsigned long long domainTag, unsigned long long addressSpaceTag, unsigned long long capabilityGeneration, unsigned long long evidenceGeneration, unsigned long long restoreGeneration)
	{
		if (virtualThreadId < 0 || ownerContextId < 0 || domainTag == 0 || addressSpaceTag == 0 || capabilityGeneration == 0 || evidenceGeneration == 0 || restoreGeneration == 0)
		{
			throw ArgumentOutOfRangeException(L"virtualThreadId", L"The research runtime context requires materialized VT/domain/address-space/capability/evidence/restore identities.");
		}

		VirtualThreadId = virtualThreadId;
		OwnerContextId = ownerContextId;
		DomainTag = domainTag;
		AddressSpaceTag = addressSpaceTag;
		setCapabilityGeneration(capabilityGeneration);
		setEvidenceGeneration(evidenceGeneration);
		setRestoreGeneration(restoreGeneration);
	}
#endif

#if defined(TESTING)
	ResearchVirtualizationOperationContext::IdentitySnapshot::IdentitySnapshot(std::any contextSeal, unsigned long long contextGeneration, std::shared_ptr<ResearchVirtualizationProbeIdentity> identity)
	{
		ContextSeal = contextSeal;
		ContextGeneration = contextGeneration;
		Identity = identity;
	}
#endif

#if defined(TESTING)
	std::any ResearchVirtualizationOperationContext::IdentitySnapshot::getContextSeal() const
	{
		return ContextSeal;
	}
#endif

#if defined(TESTING)
	unsigned long long ResearchVirtualizationOperationContext::IdentitySnapshot::getContextGeneration() const
	{
		return ContextGeneration;
	}
#endif

#if defined(TESTING)
	std::shared_ptr<ResearchVirtualizationProbeIdentity> ResearchVirtualizationOperationContext::IdentitySnapshot::getIdentity() const
	{
		return Identity;
	}
#endif

#if defined(TESTING)
	ResearchVirtualizationOperationContext::MaterializationLease::MaterializationLease(std::any contextSeal, unsigned long long contextGeneration, int virtualThreadId, int ownerContextId, unsigned long long domainTag, unsigned long long addressSpaceTag, unsigned long long capabilityGeneration, unsigned long long evidenceGeneration, unsigned long long restoreGeneration)
	{
		ContextSeal = contextSeal;
		ContextGeneration = contextGeneration;
		VirtualThreadId = virtualThreadId;
		OwnerContextId = ownerContextId;
		DomainTag = domainTag;
		AddressSpaceTag = addressSpaceTag;
		CapabilityGeneration = capabilityGeneration;
		EvidenceGeneration = evidenceGeneration;
		RestoreGeneration = restoreGeneration;
	}
#endif

#if defined(TESTING)
	std::any ResearchVirtualizationOperationContext::MaterializationLease::getContextSeal() const
	{
		return ContextSeal;
	}
#endif

#if defined(TESTING)
	unsigned long long ResearchVirtualizationOperationContext::MaterializationLease::getContextGeneration() const
	{
		return ContextGeneration;
	}
#endif

#if defined(TESTING)
	int ResearchVirtualizationOperationContext::MaterializationLease::getVirtualThreadId() const
	{
		return VirtualThreadId;
	}
#endif

#if defined(TESTING)
	int ResearchVirtualizationOperationContext::MaterializationLease::getOwnerContextId() const
	{
		return OwnerContextId;
	}
#endif

#if defined(TESTING)
	unsigned long long ResearchVirtualizationOperationContext::MaterializationLease::getDomainTag() const
	{
		return DomainTag;
	}
#endif

#if defined(TESTING)
	unsigned long long ResearchVirtualizationOperationContext::MaterializationLease::getAddressSpaceTag() const
	{
		return AddressSpaceTag;
	}
#endif

#if defined(TESTING)
	unsigned long long ResearchVirtualizationOperationContext::MaterializationLease::getCapabilityGeneration() const
	{
		return CapabilityGeneration;
	}
#endif

#if defined(TESTING)
	unsigned long long ResearchVirtualizationOperationContext::MaterializationLease::getEvidenceGeneration() const
	{
		return EvidenceGeneration;
	}
#endif

#if defined(TESTING)
	unsigned long long ResearchVirtualizationOperationContext::MaterializationLease::getRestoreGeneration() const
	{
		return RestoreGeneration;
	}
#endif

#if defined(TESTING)
	int ResearchVirtualizationOperationContext::getVirtualThreadId() const
	{
		return VirtualThreadId;
	}
#endif

#if defined(TESTING)
	int ResearchVirtualizationOperationContext::getOwnerContextId() const
	{
		return OwnerContextId;
	}
#endif

#if defined(TESTING)
	unsigned long long ResearchVirtualizationOperationContext::getDomainTag() const
	{
		return DomainTag;
	}
#endif

#if defined(TESTING)
	unsigned long long ResearchVirtualizationOperationContext::getAddressSpaceTag() const
	{
		return AddressSpaceTag;
	}
#endif

#if defined(TESTING)
	unsigned long long ResearchVirtualizationOperationContext::getCapabilityGeneration() const
	{
		return CapabilityGeneration;
	}
#endif

#if defined(TESTING)
	void ResearchVirtualizationOperationContext::setCapabilityGeneration(unsigned long long value)
	{
		CapabilityGeneration = value;
	}
#endif

#if defined(TESTING)
	unsigned long long ResearchVirtualizationOperationContext::getEvidenceGeneration() const
	{
		return EvidenceGeneration;
	}
#endif

#if defined(TESTING)
	void ResearchVirtualizationOperationContext::setEvidenceGeneration(unsigned long long value)
	{
		EvidenceGeneration = value;
	}
#endif

#if defined(TESTING)
	unsigned long long ResearchVirtualizationOperationContext::getRestoreGeneration() const
	{
		return RestoreGeneration;
	}
#endif

#if defined(TESTING)
	void ResearchVirtualizationOperationContext::setRestoreGeneration(unsigned long long value)
	{
		RestoreGeneration = value;
	}
#endif

#if defined(TESTING)
	std::shared_ptr<MaterializationLease> ResearchVirtualizationOperationContext::CaptureMaterializationLease()
	{
		{
			std::scoped_lock<std::mutex> lock(_contextSeal);
			return new(_contextSeal, _contextGeneration, getVirtualThreadId(), getOwnerContextId(), getDomainTag(), getAddressSpaceTag(), getCapabilityGeneration(), getEvidenceGeneration(), getRestoreGeneration());
		}
	}
#endif

#if defined(TESTING)
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal IdentitySnapshot? Materialize(MaterializationLease lease, ulong carrierAttemptId, ulong replayEpoch)
	std::shared_ptr<IdentitySnapshot> ResearchVirtualizationOperationContext::Materialize(std::shared_ptr<MaterializationLease> lease, unsigned long long carrierAttemptId, unsigned long long replayEpoch)
	{
		ArgumentNullException::ThrowIfNull(lease);

		{
			std::scoped_lock<std::mutex> lock(_contextSeal);
// C# TO C++ CONVERTER TASK: Cannot test for object identity between 2 non-pointer objects in C++:
			if (!this->ReferenceEquals(lease->getContextSeal(), _contextSeal) || lease->getContextGeneration() != _contextGeneration || lease->getVirtualThreadId() != getVirtualThreadId() || lease->getOwnerContextId() != getOwnerContextId() || lease->getDomainTag() != getDomainTag() || lease->getAddressSpaceTag() != getAddressSpaceTag() || lease->getCapabilityGeneration() != getCapabilityGeneration() || lease->getEvidenceGeneration() != getEvidenceGeneration() || lease->getRestoreGeneration() != getRestoreGeneration())
			{
				return nullptr;
			}

			return new(_contextSeal, _contextGeneration, std::make_shared<ResearchVirtualizationProbeIdentity>(lease->getVirtualThreadId(), lease->getOwnerContextId(), lease->getDomainTag(), lease->getAddressSpaceTag(), carrierAttemptId, replayEpoch, lease->getCapabilityGeneration(), lease->getEvidenceGeneration(), lease->getRestoreGeneration()));
		}
	}
#endif

#if defined(TESTING)
	std::shared_ptr<IdentitySnapshot> ResearchVirtualizationOperationContext::Capture(unsigned long long carrierAttemptId, unsigned long long replayEpoch)
	{
		{
			std::scoped_lock<std::mutex> lock(_contextSeal);
			return new(_contextSeal, _contextGeneration, std::make_shared<ResearchVirtualizationProbeIdentity>(getVirtualThreadId(), getOwnerContextId(), getDomainTag(), getAddressSpaceTag(), carrierAttemptId, replayEpoch, getCapabilityGeneration(), getEvidenceGeneration(), getRestoreGeneration()));
		}
	}
#endif

#if defined(TESTING)
	bool ResearchVirtualizationOperationContext::IsLive(std::shared_ptr<IdentitySnapshot> snapshot)
	{
		{
			std::scoped_lock<std::mutex> lock(_contextSeal);
// C# TO C++ CONVERTER TASK: Cannot test for object identity between 2 non-pointer objects in C++:
			return snapshot != nullptr && this->ReferenceEquals(snapshot->getContextSeal(), _contextSeal) && snapshot->getContextGeneration() == _contextGeneration;
		}
	}
#endif

#if defined(TESTING)
	bool ResearchVirtualizationOperationContext::IsLive(std::any contextSeal, unsigned long long contextGeneration)
	{
		{
			std::scoped_lock<std::mutex> lock(_contextSeal);
// C# TO C++ CONVERTER TASK: Cannot test for object identity between 2 non-pointer objects in C++:
			return this->ReferenceEquals(contextSeal, _contextSeal) && contextGeneration == _contextGeneration;
		}
	}
#endif

#if defined(TESTING)
	void ResearchVirtualizationOperationContext::Invalidate()
	{
		{
			std::scoped_lock<std::mutex> lock(_contextSeal);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _contextGeneration = checked(_contextGeneration + 1);
			_contextGeneration = _contextGeneration + 1;
		}
	}
#endif

#if defined(TESTING)
	void ResearchVirtualizationOperationContext::AdvanceCapabilityGeneration()
	{
		{
			std::scoped_lock<std::mutex> lock(_contextSeal);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: CapabilityGeneration = checked(CapabilityGeneration + 1);
			setCapabilityGeneration(getCapabilityGeneration() + 1);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _contextGeneration = checked(_contextGeneration + 1);
			_contextGeneration = _contextGeneration + 1;
		}
	}
#endif

#if defined(TESTING)
	void ResearchVirtualizationOperationContext::AdvanceEvidenceGeneration()
	{
		{
			std::scoped_lock<std::mutex> lock(_contextSeal);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: EvidenceGeneration = checked(EvidenceGeneration + 1);
			setEvidenceGeneration(getEvidenceGeneration() + 1);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _contextGeneration = checked(_contextGeneration + 1);
			_contextGeneration = _contextGeneration + 1;
		}
	}
#endif

#if defined(TESTING)
	void ResearchVirtualizationOperationContext::AdvanceRestoreGeneration()
	{
		{
			std::scoped_lock<std::mutex> lock(_contextSeal);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: RestoreGeneration = checked(RestoreGeneration + 1);
			setRestoreGeneration(getRestoreGeneration() + 1);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _contextGeneration = checked(_contextGeneration + 1);
			_contextGeneration = _contextGeneration + 1;
		}
	}
#endif

#if defined(TESTING)
	ResearchVirtualizationRuntimeOwner::PolicySnapshot::PolicySnapshot(std::any ownerSeal, unsigned long long policyGeneration, ResearchVirtualizationProbeOperation operation)
	{
		OwnerSeal = ownerSeal;
		PolicyGeneration = policyGeneration;
		Operation = operation;
	}
#endif

#if defined(TESTING)
	std::any ResearchVirtualizationRuntimeOwner::PolicySnapshot::getOwnerSeal() const
	{
		return OwnerSeal;
	}
#endif

#if defined(TESTING)
	unsigned long long ResearchVirtualizationRuntimeOwner::PolicySnapshot::getPolicyGeneration() const
	{
		return PolicyGeneration;
	}
#endif

#if defined(TESTING)
	ResearchVirtualizationProbeOperation ResearchVirtualizationRuntimeOwner::PolicySnapshot::getOperation() const
	{
		return Operation;
	}
#endif

#if defined(TESTING)
	ResearchVirtualizationRuntimeOwner::ExecutionReceipt::ExecutionReceipt(unsigned long long policyGeneration, unsigned long long verifierGeneration, ResearchVirtualizationProbeOperation operation, std::shared_ptr<ResearchVirtualizationProbeIdentity> identity)
	{
		PolicyGeneration = policyGeneration;
		VerifierGeneration = verifierGeneration;
		Operation = operation;
		Identity = identity;
	}
#endif

#if defined(TESTING)
	unsigned long long ResearchVirtualizationRuntimeOwner::ExecutionReceipt::getPolicyGeneration() const
	{
		return PolicyGeneration;
	}
#endif

#if defined(TESTING)
	unsigned long long ResearchVirtualizationRuntimeOwner::ExecutionReceipt::getVerifierGeneration() const
	{
		return VerifierGeneration;
	}
#endif

#if defined(TESTING)
	ResearchVirtualizationProbeOperation ResearchVirtualizationRuntimeOwner::ExecutionReceipt::getOperation() const
	{
		return Operation;
	}
#endif

#if defined(TESTING)
	std::shared_ptr<ResearchVirtualizationProbeIdentity> ResearchVirtualizationRuntimeOwner::ExecutionReceipt::getIdentity() const
	{
		return Identity;
	}
#endif

#if defined(TESTING)
	int ResearchVirtualizationRuntimeOwner::ExecutionReceipt::getPayloadLength() const
	{
		return 0;
	}
#endif

#if defined(TESTING)
	int ResearchVirtualizationRuntimeOwner::ExecutionReceipt::getStateMutationCount() const
	{
		return 0;
	}
#endif

#if defined(TESTING)
	bool ResearchVirtualizationRuntimeOwner::ExecutionReceipt::getCompletionPublicationAuthorized() const
	{
		return false;
	}
#endif

#if defined(TESTING)
	bool ResearchVirtualizationRuntimeOwner::ExecutionReceipt::getRetirePublicationAuthorized() const
	{
		return false;
	}
#endif

#if defined(TESTING)
	std::shared_ptr<PolicySnapshot> ResearchVirtualizationRuntimeOwner::CapturePolicy()
	{
		{
			std::scoped_lock<std::mutex> lock(_stateGate);
			return new(_ownerSeal, _policyGeneration, ResearchVirtualizationProbeOperation::StateMinimalLiveness);
		}
	}
#endif

#if defined(TESTING)
	std::shared_ptr<ResearchVirtualizationProbeExecutionResult> ResearchVirtualizationRuntimeOwner::Execute(std::shared_ptr<SafetyVerifier> verifier, std::shared_ptr<SafetyVerifier::ResearchVirtualizationOperationAdmissionCertificate> certificate, std::shared_ptr<ResearchVirtualizationOperationContext> context)
	{
		ArgumentNullException::ThrowIfNull(verifier);
		ArgumentNullException::ThrowIfNull(certificate);
		ArgumentNullException::ThrowIfNull(context);

		{
			std::scoped_lock<std::mutex> lock(_stateGate);
// C# TO C++ CONVERTER TASK: Cannot test for object identity between 2 non-pointer objects in C++:
			if (!this->ReferenceEquals(certificate->getOwnerSeal(), _ownerSeal))
			{
				return Deny(ResearchVirtualizationProbeExecutionDecision::DeniedForeignOwner, L"The operation certificate names a different neutral research owner.");
			}

			if (certificate->getOwnerPolicyGeneration() != _policyGeneration)
			{
				return Deny(ResearchVirtualizationProbeExecutionDecision::DeniedStalePolicyGeneration, L"The neutral research-owner policy changed before execution.");
			}

			if (!context->IsLive(certificate->getContextSeal(), certificate->getContextGeneration()))
			{
				return Deny(ResearchVirtualizationProbeExecutionDecision::DeniedStaleRuntimeContext, L"The runtime identity context was foreign or invalidated before execution.");
			}

			if (!verifier->IsResearchVirtualizationOperationAdmissionLive(certificate))
			{
				return Deny(ResearchVirtualizationProbeExecutionDecision::DeniedStaleAdmission, L"SafetyVerifier revoked the E1/E2 admission generation before execution.");
			}

			if (!_consumedAttempts.insert(certificate->getIdentity()->CarrierAttemptId))
			{
				return Deny(ResearchVirtualizationProbeExecutionDecision::DeniedDuplicateAttempt, L"The admitted carrier attempt was already consumed.");
			}

			return new(ResearchVirtualizationProbeExecutionDecision::Executed, std::make_shared<ExecutionReceipt>(_policyGeneration, certificate->getVerifierGeneration(), certificate->getOperation(), certificate->getIdentity()), L"The neutral research probe executed without payload, state mutation, completion or retire publication.");
		}
	}
#endif

#if defined(TESTING)
	void ResearchVirtualizationRuntimeOwner::InvalidatePolicy()
	{
		{
			std::scoped_lock<std::mutex> lock(_stateGate);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _policyGeneration = checked(_policyGeneration + 1);
			_policyGeneration = _policyGeneration + 1;
		}
	}
#endif

#if defined(TESTING)
	std::shared_ptr<ResearchVirtualizationProbeExecutionResult> ResearchVirtualizationRuntimeOwner::Deny(ResearchVirtualizationProbeExecutionDecision decision, const std::wstring &reason)
	{
		return new(decision, nullptr, reason);
	}
#endif
}
