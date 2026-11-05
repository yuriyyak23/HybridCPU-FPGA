#include "YAKSys_Hybrid_CPU.Core.SecureMemoryAdmissionPolicy.h"
#include "../../../../Capabilities/Descriptors/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSet.h"

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureMemoryAdmissionResult Admit(SecureMemoryDomainDescriptor? descriptor, SecureMemoryAccessRequest request)
	std::shared_ptr<SecureMemoryAdmissionResult> SecureMemoryAdmissionPolicy::Admit(std::shared_ptr<SecureMemoryDomainDescriptor> descriptor, std::shared_ptr<SecureMemoryAccessRequest> request)
	{
		return Admit(descriptor, request, nullptr);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureMemoryAdmissionResult Admit(SecureMemoryDomainDescriptor? descriptor, SecureMemoryAccessRequest request, System.Nullable<SecureIoDomainDescriptor> secureIoPolicy)
	std::shared_ptr<SecureMemoryAdmissionResult> SecureMemoryAdmissionPolicy::Admit(std::shared_ptr<SecureMemoryDomainDescriptor> descriptor, std::shared_ptr<SecureMemoryAccessRequest> request, std::shared_ptr<SecureIoDomainDescriptor> secureIoPolicy)
	{
		if (descriptor == nullptr)
		{
			return Deny(SecureMemoryAdmissionDecision::DeniedMissingDescriptor, L"Secure memory access requires a secure memory descriptor.");
		}

		if (!descriptor->isMaterialized())
		{
			return Deny(SecureMemoryAdmissionDecision::DeniedUnmaterializedDescriptor, L"Secure memory descriptor must be materialized before Stage B memory policy is active.");
		}

		std::shared_ptr<SecureMemoryRegionDescriptor> region;
		if (!descriptor->TryFindRegion(request->Address, request->Length, region))
		{
			return Deny(request->Kind == SecureMemoryAccessKind::Measurement ? SecureMemoryAdmissionDecision::DeniedMeasuredRegionMissing : SecureMemoryAdmissionDecision::DeniedRegionNotFound, L"Secure memory access must target an explicit secure memory region.");
		}

		if (!region->IsCurrentFor(descriptor->getPolicyEpoch()))
		{
			return Deny(SecureMemoryAdmissionDecision::DeniedStalePolicyEpoch, L"Secure memory region policy epoch is stale or unmaterialized.");
		}

	switch (request->Kind) {
        case SecureMemoryAccessKind::RuntimeTouch: return AdmitRuntimeTouch(region);
        case SecureMemoryAccessKind::HostRead: return AdmitHostRead(region);
        case SecureMemoryAccessKind::DmaRead:
        case SecureMemoryAccessKind::DmaWrite: return AdmitDma(descriptor, secureIoPolicy, region, request);
        case SecureMemoryAccessKind::Measurement: return AdmitMeasurement(region);
        default: return Deny(SecureMemoryAdmissionDecision::DeniedRegionNotFound, L"Secure memory access kind is not recognized by the secure memory policy.");
        }
	}

	std::shared_ptr<SecureMemoryAdmissionResult> SecureMemoryAdmissionPolicy::AdmitHostRead(std::shared_ptr<SecureMemoryRegionDescriptor> region)
	{
		if (region->IsPrivate)
		{
			return Deny(SecureMemoryAdmissionDecision::DeniedPrivateHostRead, L"Private secure memory is not host readable.");
		}

		if (!region->IsHostReadable)
		{
			return Deny(SecureMemoryAdmissionDecision::DeniedHostInspection, L"Host inspection requires an explicit shared secure memory region.");
		}

		return SecureMemoryAdmissionResult::AllowedSecureMemory;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static SecureMemoryAdmissionResult AdmitDma(SecureMemoryDomainDescriptor descriptor, SecureIoDomainDescriptor? secureIoPolicy, SecureMemoryRegionDescriptor region, SecureMemoryAccessRequest request)
	std::shared_ptr<SecureMemoryAdmissionResult> SecureMemoryAdmissionPolicy::AdmitDma(std::shared_ptr<SecureMemoryDomainDescriptor> descriptor, std::shared_ptr<SecureIoDomainDescriptor> secureIoPolicy, std::shared_ptr<SecureMemoryRegionDescriptor> region, std::shared_ptr<SecureMemoryAccessRequest> request)
	{
		if (region->IsPrivate)
		{
			return Deny(SecureMemoryAdmissionDecision::DeniedPrivateDma, L"DMA to private secure memory is denied, including I/O and hypercall argument paths.");
		}

		if (!region->IsShared || !region->IsHostReadable || !descriptor->getAllowsExplicitSharedDma())
		{
			return Deny(SecureMemoryAdmissionDecision::DeniedSharedRequiresExplicitPolicy, L"Secure DMA is allowed only for explicit shared buffers under secure memory DMA policy.");
		}

		std::shared_ptr<SecureSharedBufferDescriptor> sharedBuffer;
		if (!TryFindSharedBuffer(descriptor, secureIoPolicy, request, sharedBuffer))
		{
			return Deny(SecureMemoryAdmissionDecision::DeniedSharedBufferBinding, L"Secure DMA requires a materialized shared-buffer descriptor with direction, owner, lifetime and evidence class.");
		}

		if (!request->CapabilityRequirement.RequiresTypedGrant || !request->CapabilityRequirement.IsSatisfiedBy((request->Capabilities != nullptr) ? request->Capabilities : CapabilityDescriptorSet::getEmpty()))
		{
			return Deny(SecureMemoryAdmissionDecision::DeniedDmaRequiresTypedGrant, L"Secure DMA to an explicit shared buffer requires a typed grant.");
		}

		if (!sharedBuffer->Grant.MatchesEpoch(*descriptor->getPolicyEpoch()))
		{
			return Deny(SecureMemoryAdmissionDecision::DeniedSharedBufferBinding, L"Secure DMA shared-buffer grant must match the secure memory policy epoch.");
		}

		return SecureMemoryAdmissionResult::AllowedSecureMemory;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool TryFindSharedBuffer(SecureMemoryDomainDescriptor descriptor, SecureIoDomainDescriptor? secureIoPolicy, SecureMemoryAccessRequest request, out SecureSharedBufferDescriptor sharedBuffer)
	bool SecureMemoryAdmissionPolicy::TryFindSharedBuffer(std::shared_ptr<SecureMemoryDomainDescriptor> descriptor, std::shared_ptr<SecureIoDomainDescriptor> secureIoPolicy, std::shared_ptr<SecureMemoryAccessRequest> request, std::shared_ptr<SecureSharedBufferDescriptor> &sharedBuffer)
	{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			SecureSharedBufferDirection requiredDirection = request.Kind switch
	//			{
	//				SecureMemoryAccessKind.DmaRead => SecureSharedBufferDirection.DomainToDevice,
	//				SecureMemoryAccessKind.DmaWrite => SecureSharedBufferDirection.DeviceToDomain,
	//				_ => SecureSharedBufferDirection.None,
	//			};

		const auto requiredDirection = request->Kind == SecureMemoryAccessKind::DmaRead ? SecureSharedBufferDirection::DomainToDevice : request->Kind == SecureMemoryAccessKind::DmaWrite ? SecureSharedBufferDirection::DeviceToDomain : SecureSharedBufferDirection::None;

		sharedBuffer = nullptr;
		return secureIoPolicy != nullptr && secureIoPolicy->TryFindSharedBuffer(request->Address, request->Length, requiredDirection, descriptor->getDomainTag(), descriptor->getPolicyEpoch(), sharedBuffer);
	}

	std::shared_ptr<SecureMemoryAdmissionResult> SecureMemoryAdmissionPolicy::AdmitMeasurement(std::shared_ptr<SecureMemoryRegionDescriptor> region)
	{
		return region->IsMeasured ? SecureMemoryAdmissionResult::AllowedSecureMemory : Deny(SecureMemoryAdmissionDecision::DeniedMeasuredRegionMissing, L"Secure measurement requires an explicit measured memory region.");
	}

	std::shared_ptr<SecureMemoryAdmissionResult> SecureMemoryAdmissionPolicy::AdmitRuntimeTouch(std::shared_ptr<SecureMemoryRegionDescriptor> region)
	{
		return region->HasRuntimeMutableClassification ? SecureMemoryAdmissionResult::AllowedSecureMemory : Deny(SecureMemoryAdmissionDecision::DeniedRuntimeMutableClassification, L"Runtime-mutable secure memory requires dirty policy and migration classification.");
	}

	std::shared_ptr<SecureMemoryAdmissionResult> SecureMemoryAdmissionPolicy::Deny(SecureMemoryAdmissionDecision decision, const std::wstring &reason)
	{
		return SecureMemoryAdmissionResult::Denied(decision, reason);
	}
}
