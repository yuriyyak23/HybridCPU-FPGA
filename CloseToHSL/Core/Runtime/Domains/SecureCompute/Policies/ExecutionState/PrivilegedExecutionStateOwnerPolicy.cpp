#include "PrivilegedExecutionStateOwnerPolicy.h"
#include "../../Descriptors/ExecutionState/PrivilegedExecutionStateDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<PrivilegedExecutionStateOwnerPolicy> PrivilegedExecutionStateOwnerPolicy::getDefault()
	{
		return Default;
	}

	std::shared_ptr<PrivilegedExecutionStateOwnerResult> PrivilegedExecutionStateOwnerPolicy::Admit(std::shared_ptr<PrivilegedExecutionStateOwnerRequest> request)
	{
		if (request->Descriptor == nullptr)
		{
			return Deny(PrivilegedExecutionStateOwnerDecision::DeniedMissingDescriptor, L"Privileged execution-state ownership requires a descriptor.");
		}

		std::shared_ptr<PrivilegedExecutionStateDescriptor> descriptor = request->Descriptor;
		if (!descriptor->isMaterialized())
		{
			return Deny(PrivilegedExecutionStateOwnerDecision::DeniedUnmaterializedDescriptor, L"Privileged execution-state ownership requires materialized identity, epoch and legality policy.");
		}

		if (descriptor->DomainTag != request->RuntimeDomainTag)
		{
			return Deny(PrivilegedExecutionStateOwnerDecision::DeniedDomainTagMismatch, L"Privileged execution-state owner domain binding does not match the runtime domain.");
		}

		if (descriptor->AddressSpaceTag != request->RuntimeAddressSpaceTag)
		{
			return Deny(PrivilegedExecutionStateOwnerDecision::DeniedAddressSpaceTagMismatch, L"Privileged execution-state owner address-space binding does not match the runtime address space.");
		}

		if (!request->CurrentEpoch.IsCurrent(descriptor->PolicyEpoch))
		{
			return Deny(PrivilegedExecutionStateOwnerDecision::DeniedStaleEpoch, L"Privileged execution-state owner epoch is stale or unmaterialized.");
		}

		if (!descriptor->hasCanonicalRegisterKinds())
		{
			return Deny(PrivilegedExecutionStateOwnerDecision::DeniedRegisterKindMismatch, L"Privileged execution-state values must use canonical GuestCr0 and GuestCr4 kinds.");
		}

		if (descriptor->EvidenceClass != PrivilegedExecutionStateEvidenceClass::GuestVisibleReadOnlyProjection)
		{
			return Deny(PrivilegedExecutionStateOwnerDecision::DeniedEvidenceClass, L"Privileged execution-state owner requires an explicit guest-visible read-only evidence classification.");
		}

		if (descriptor->MigrationClass != PrivilegedExecutionStateMigrationClass::RevalidatedAfterRestore)
		{
			return Deny(PrivilegedExecutionStateOwnerDecision::DeniedMigrationClass, L"Privileged execution-state owner must be revalidated after restore.");
		}

		if (descriptor->LegalityPolicy.HasReservedBits(descriptor->GuestCr0))
		{
			return Deny(PrivilegedExecutionStateOwnerDecision::DeniedGuestCr0ReservedBits, L"GuestCr0 contains bits outside the descriptor-owned legality mask.");
		}

		if (!descriptor->LegalityPolicy.HasAllRequiredBits(descriptor->GuestCr0))
		{
			return Deny(PrivilegedExecutionStateOwnerDecision::DeniedGuestCr0RequiredBits, L"GuestCr0 is missing descriptor-required bits.");
		}

		if (descriptor->LegalityPolicy.HasReservedBits(descriptor->GuestCr4))
		{
			return Deny(PrivilegedExecutionStateOwnerDecision::DeniedGuestCr4ReservedBits, L"GuestCr4 contains bits outside the descriptor-owned legality mask.");
		}

		if (!descriptor->LegalityPolicy.HasAllRequiredBits(descriptor->GuestCr4))
		{
			return Deny(PrivilegedExecutionStateOwnerDecision::DeniedGuestCr4RequiredBits, L"GuestCr4 is missing descriptor-required bits.");
		}

		return PrivilegedExecutionStateOwnerResult::AllowedOwnerOnly;
	}

	std::shared_ptr<PrivilegedExecutionStateOwnerResult> PrivilegedExecutionStateOwnerPolicy::Deny(PrivilegedExecutionStateOwnerDecision decision, const std::wstring &reason)
	{
		return PrivilegedExecutionStateOwnerResult::Denied(decision, reason);
	}
}
