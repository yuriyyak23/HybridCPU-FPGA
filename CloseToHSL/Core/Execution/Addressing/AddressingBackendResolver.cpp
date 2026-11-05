#include "AddressingBackendResolver.h"
#include "../ExternalAccelerators/Auth/AcceleratorOwnerDomainGuard.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Execution;

namespace YAKSys_Hybrid_CPU::Core::Execution::Addressing
{
	bool AddressingBackendParticipantIdentity::HasSameOwnerDomainAndDevice(const std::shared_ptr<const AddressingBackendParticipantIdentity>& current, AddressingBackendFaultKind& faultKind) const
	{
		faultKind = AddressingBackendFaultKind::None;
		if (!current || OwnerVirtualThreadId != current->OwnerVirtualThreadId || OwnerContextId != current->OwnerContextId || OwnerCoreId != current->OwnerCoreId || OwnerPodId != current->OwnerPodId || MemoryDomainTag != current->MemoryDomainTag)
		{
			faultKind = AddressingBackendFaultKind::OwnerDomainMismatch;
			return false;
		}
		if (DeviceId != current->DeviceId || DeviceIdBound != current->DeviceIdBound)
		{
			faultKind = AddressingBackendFaultKind::DeviceMismatch;
			return false;
		}
		return true;
	}

	std::shared_ptr<const AddressingBackendParticipantIdentity> AddressingBackendParticipantIdentity::FromDmaStreamCompute(const std::shared_ptr<DmaStreamComputeDescriptor>& descriptor, unsigned long long tokenId, unsigned long long issueAge, unsigned long long replayGeneration, unsigned long long mappingEpoch, bool mappingEpochKnown)
	{
		ArgumentNullException::ThrowIfNull(descriptor);
		auto owner = descriptor->OwnerBinding;
		auto guard = descriptor->OwnerGuardDecision;
		ArgumentNullException::ThrowIfNull(owner);
		ArgumentNullException::ThrowIfNull(guard);
		return std::make_shared<const AddressingBackendParticipantIdentity>(owner->OwnerVirtualThreadId, owner->OwnerContextId, owner->OwnerCoreId, owner->OwnerPodId, owner->OwnerDomainTag, guard->RuntimeOwnerContext.ActiveDomainCertificate, owner->DeviceId, guard->IsAllowed && owner->DeviceId == guard->RuntimeOwnerContext.DeviceId, mappingEpoch, mappingEpochKnown, tokenId, 0, descriptor->DescriptorIdentityHash, descriptor->NormalizedFootprintHash, issueAge, replayGeneration);
	}

	std::shared_ptr<const AddressingBackendParticipantIdentity> AddressingBackendParticipantIdentity::FromL7Accelerator(const std::shared_ptr<AcceleratorCommandDescriptor>& descriptor, unsigned long long tokenId, unsigned long long commandId, unsigned long long issueAge, unsigned long long replayGeneration)
	{
		ArgumentNullException::ThrowIfNull(descriptor);
		auto owner = descriptor->OwnerBinding;
		auto guard = descriptor->OwnerGuardDecision;
		ArgumentNullException::ThrowIfNull(owner);
		ArgumentNullException::ThrowIfNull(guard);
		return std::make_shared<const AddressingBackendParticipantIdentity>(owner->OwnerVirtualThreadId, owner->OwnerContextId, owner->OwnerCoreId, owner->OwnerPodId, owner->DomainTag, guard->Evidence ? guard->Evidence->ActiveDomainCertificate : 0, static_cast<unsigned int>(descriptor->AcceleratorId), guard->IsAllowed, guard->getMappingEpoch().Value, static_cast<bool>(guard->Evidence), tokenId, commandId, descriptor->Identity.DescriptorIdentityHash, descriptor->Identity.NormalizedFootprintHash, issueAge, replayGeneration);
	}

	std::shared_ptr<AddressingBackendResolutionRequest> AddressingBackendResolutionRequest::ForDmaStreamCompute(const std::shared_ptr<DmaStreamComputeDescriptor>& descriptor, MemoryAddressSpaceKind addressSpace, AddressingBackendCapabilities capabilities, unsigned long long tokenId, unsigned long long issueAge, unsigned long long replayGeneration, unsigned long long mappingEpoch, bool mappingEpochKnown, std::shared_ptr<AddressingBackendParticipantIdentity> currentAuthority)
	{
		ArgumentNullException::ThrowIfNull(descriptor);
		std::vector<AddressingBackendRange> ranges;
		ranges.reserve(descriptor->NormalizedReadMemoryRanges.size() + descriptor->NormalizedWriteMemoryRanges.size());
		for (const auto& range : descriptor->NormalizedReadMemoryRanges) { ranges.push_back({range.Address, range.Length}); }
		for (const auto& range : descriptor->NormalizedWriteMemoryRanges) { ranges.push_back({range.Address, range.Length}); }
		return std::make_shared<AddressingBackendResolutionRequest>(addressSpace, AddressingBackendAccessKind::ReadWrite, ranges, AddressingBackendParticipantIdentity::FromDmaStreamCompute(descriptor, tokenId, issueAge, replayGeneration, mappingEpoch, mappingEpochKnown), capabilities, std::move(currentAuthority));
	}

	std::shared_ptr<AddressingBackendResolutionRequest> AddressingBackendResolutionRequest::ForL7Accelerator(const std::shared_ptr<AcceleratorCommandDescriptor>& descriptor, MemoryAddressSpaceKind addressSpace, AddressingBackendCapabilities capabilities, unsigned long long tokenId, unsigned long long commandId, unsigned long long issueAge, unsigned long long replayGeneration, std::shared_ptr<AddressingBackendParticipantIdentity> currentAuthority)
	{
		ArgumentNullException::ThrowIfNull(descriptor);
		std::vector<AddressingBackendRange> ranges;
		auto appendRanges = [&ranges](const auto& source) { for (const auto& range : source) { if (!range) throw std::invalid_argument("Null accelerator memory range."); ranges.push_back({range->Address, range->Length}); } };
		appendRanges(descriptor->NormalizedFootprint.SourceRanges);
		appendRanges(descriptor->NormalizedFootprint.DestinationRanges);
		appendRanges(descriptor->NormalizedFootprint.ScratchRanges);
		return std::make_shared<AddressingBackendResolutionRequest>(addressSpace, AddressingBackendAccessKind::ReadWrite, ranges, AddressingBackendParticipantIdentity::FromL7Accelerator(descriptor, tokenId, commandId, issueAge, replayGeneration), capabilities, std::move(currentAuthority));
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public AddressingBackendResolver(IBurstBackend? physicalBackend = null, IBurstBackend? iommuBackend = null)
	AddressingBackendResolver::AddressingBackendResolver(std::shared_ptr<IBurstBackend> physicalBackend, std::shared_ptr<IBurstBackend> iommuBackend) : _physicalBackend(physicalBackend), _iommuBackend(iommuBackend)
	{
	}

	std::shared_ptr<AddressingBackendResolution> AddressingBackendResolver::Resolve(std::shared_ptr<AddressingBackendResolutionRequest> request)
	{
		ArgumentNullException::ThrowIfNull(request);

		if (!request->HasCompleteRangeTruth())
		{
			return AddressingBackendResolution::Faulted(AddressingBackendFaultKind::IncompleteRangeTruth, L"Addressing backend resolution requires complete, non-empty, non-overflowing range truth.");
		}

		if (!request->HasCapability(AddressingBackendCapabilities::ExplicitAddressSpaceContract))
		{
			return AddressingBackendResolution::Rejected(AddressingBackendFaultKind::CapabilityMismatch, L"Executable backend selection requires an explicit address-space contract; DSC1 reserved fields and carrier hints are not authority.");
		}

		if (!request->HasCapability(AddressingBackendCapabilities::TypedFaultClassification))
		{
			return AddressingBackendResolution::Rejected(AddressingBackendFaultKind::CapabilityMismatch, L"Executable backend selection requires typed fault classification before memory access.");
		}

		std::optional<std::shared_ptr<AddressingBackendResolution>> authorityFailure = ValidateAuthority(request);
		if (authorityFailure)
		{
			return *authorityFailure;
		}

		switch (request->AddressSpace)
		{
		case MemoryAddressSpaceKind::Physical: return ResolvePhysical(request);
		case MemoryAddressSpaceKind::IommuTranslated: return ResolveIommu(request);
		default: return AddressingBackendResolution::Rejected(AddressingBackendFaultKind::UnsupportedAddressSpace, L"Unsupported or future address-space mode rejects before memory access and cannot fall back to physical memory.");
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DmaStreamComputeFaultRecord CreateDmaStreamComputeFaultRecord(DmaStreamComputeDescriptor descriptor, AddressingBackendFaultKind faultKind, ulong faultAddress, bool isWrite, string? message = null)
	std::shared_ptr<DmaStreamComputeFaultRecord> AddressingBackendResolver::CreateDmaStreamComputeFaultRecord(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, AddressingBackendFaultKind faultKind, unsigned long long faultAddress, bool isWrite, const std::wstring &message)
	{
		ArgumentNullException::ThrowIfNull(descriptor);
		if (faultKind == AddressingBackendFaultKind::None)
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Use a non-None addressing backend fault kind.", nameof(faultKind));
			throw std::invalid_argument("Use a non-None addressing backend fault kind.");
		}

		const DmaStreamComputeOwnerGuardContext& guard = descriptor->OwnerGuardDecision->RuntimeOwnerContext;
		std::wstring faultName;
		switch (faultKind) {
		case AddressingBackendFaultKind::UnsupportedAddressSpace: faultName = L"UnsupportedAddressSpace"; break;
		case AddressingBackendFaultKind::CapabilityMismatch: faultName = L"CapabilityMismatch"; break;
		case AddressingBackendFaultKind::IncompleteRangeTruth: faultName = L"IncompleteRangeTruth"; break;
		case AddressingBackendFaultKind::OwnerDomainMismatch: faultName = L"OwnerDomainMismatch"; break;
		case AddressingBackendFaultKind::DeviceMismatch: faultName = L"DeviceMismatch"; break;
		case AddressingBackendFaultKind::MissingDeviceBinding: faultName = L"MissingDeviceBinding"; break;
		case AddressingBackendFaultKind::MissingMappingEpoch: faultName = L"MissingMappingEpoch"; break;
		case AddressingBackendFaultKind::MappingEpochRevoked: faultName = L"MappingEpochRevoked"; break;
		case AddressingBackendFaultKind::TranslationFault: faultName = L"TranslationFault"; break;
		case AddressingBackendFaultKind::PermissionFault: faultName = L"PermissionFault"; break;
		case AddressingBackendFaultKind::AlignmentFault: faultName = L"AlignmentFault"; break;
		case AddressingBackendFaultKind::BoundsFault: faultName = L"BoundsFault"; break;
		case AddressingBackendFaultKind::DmaDeviceFault: faultName = L"DmaDeviceFault"; break;
		case AddressingBackendFaultKind::BackendMemoryFault: faultName = L"BackendMemoryFault"; break;
		default: faultName = L"None"; break;
		}
		const std::wstring faultMessage = StringHelper::isEmptyOrWhiteSpace(message) ? L"Phase06 addressing backend fault: " + faultName + L"." : message;
		return std::make_shared<DmaStreamComputeFaultRecord>(MapFaultKind(faultKind), faultMessage, faultAddress, isWrite, descriptor->OwnerBinding->OwnerVirtualThreadId, descriptor->OwnerBinding->OwnerDomainTag, guard.ActiveDomainCertificate, MapSourcePhase(faultKind), false, std::nullopt, DmaStreamComputeFaultPublicationContract::FuturePreciseRetireRequiresPublicationMetadata);
	}

	std::shared_ptr<AddressingBackendResolution> AddressingBackendResolver::ResolvePhysical(std::shared_ptr<AddressingBackendResolutionRequest> request)
	{
		if (!request->HasCapability(AddressingBackendCapabilities::PhysicalMainMemoryBackend) || _physicalBackend == nullptr)
		{
			return AddressingBackendResolution::Rejected(AddressingBackendFaultKind::CapabilityMismatch, L"Physical address-space selection requires an explicit physical main-memory backend wrapper.");
		}

		return AddressingBackendResolution::Selected(AddressingBackendKind::PhysicalMainMemory, _physicalBackend, L"Physical address-space selection chose the explicit physical main-memory backend wrapper.");
	}

	std::shared_ptr<AddressingBackendResolution> AddressingBackendResolver::ResolveIommu(std::shared_ptr<AddressingBackendResolutionRequest> request)
	{
		if (!request->HasCapability(AddressingBackendCapabilities::IommuBurstBackend) || _iommuBackend == nullptr)
		{
			return AddressingBackendResolution::Rejected(AddressingBackendFaultKind::CapabilityMismatch, L"IOMMU-translated address-space selection requires an explicit IOMMU backend; physical fallback is forbidden.");
		}

		if (!request->HasCapability(AddressingBackendCapabilities::MappingEpoch) || !request->AcceptedIdentity->MappingEpochKnown)
		{
			return AddressingBackendResolution::Rejected(AddressingBackendFaultKind::MissingMappingEpoch, L"IOMMU-translated address-space selection requires a captured mapping epoch before memory access.");
		}

		return AddressingBackendResolution::Selected(AddressingBackendKind::IommuBurst, _iommuBackend, L"IOMMU-translated address-space selection chose the explicit IOMMU burst backend.");
	}

	std::optional<std::shared_ptr<AddressingBackendResolution>> AddressingBackendResolver::ValidateAuthority(std::shared_ptr<AddressingBackendResolutionRequest> request)
	{
		if (!request->HasCapability(AddressingBackendCapabilities::OwnerDomainDeviceBinding))
		{
			return AddressingBackendResolution::Rejected(AddressingBackendFaultKind::CapabilityMismatch, L"Backend selection requires owner/domain/device binding authority.");
		}

		std::shared_ptr<const AddressingBackendParticipantIdentity> identity = request->AcceptedIdentity;
		if (!identity->DeviceIdBound)
		{
			return AddressingBackendResolution::Rejected(AddressingBackendFaultKind::MissingDeviceBinding, L"Backend selection requires an explicit bound device ID; thread/context inference is not authority.");
		}

		if (!identity->HasOwnerDomainAuthority())
		{
			return AddressingBackendResolution::Rejected(AddressingBackendFaultKind::OwnerDomainMismatch, L"Backend selection requires owner/domain authority from the guard plane.");
		}

		if (request->CurrentAuthority == nullptr)
		{
			return std::nullopt;
		}

		std::shared_ptr<const AddressingBackendParticipantIdentity> current = request->CurrentAuthority;
		if (!current->HasOwnerDomainAuthority())
		{
			return AddressingBackendResolution::Rejected(AddressingBackendFaultKind::OwnerDomainMismatch, L"Current owner/domain authority is no longer covered by the guard-plane certificate.");
		}

		AddressingBackendFaultKind faultKind;
		if (!identity->HasSameOwnerDomainAndDevice(current, faultKind))
		{
			return AddressingBackendResolution::Rejected(faultKind, L"Current owner/domain/device authority no longer matches accepted backend-selection evidence.");
		}

		if (request->AddressSpace == MemoryAddressSpaceKind::IommuTranslated && !current->MappingEpochKnown)
		{
			return AddressingBackendResolution::Rejected(AddressingBackendFaultKind::MissingMappingEpoch, L"Current IOMMU authority must carry a mapping epoch before translated memory access.");
		}

		if (request->AddressSpace == MemoryAddressSpaceKind::IommuTranslated && identity->MappingEpochKnown && current->MappingEpochKnown && identity->MappingEpoch != current->MappingEpoch)
		{
			return AddressingBackendResolution::Rejected(AddressingBackendFaultKind::MappingEpochRevoked, L"IOMMU mapping epoch changed after accepted backend-selection evidence.");
		}

		return std::nullopt;
	}

	DmaStreamComputeTokenFaultKind AddressingBackendResolver::MapFaultKind(AddressingBackendFaultKind faultKind)
	{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return faultKind switch
	//			{
	//				AddressingBackendFaultKind.TranslationFault or AddressingBackendFaultKind.MissingMappingEpoch or AddressingBackendFaultKind.MappingEpochRevoked => DmaStreamComputeTokenFaultKind.TranslationFault,
	//ignore
	//ignore
	//
	//				AddressingBackendFaultKind.PermissionFault => DmaStreamComputeTokenFaultKind.PermissionFault,
	//
	//				AddressingBackendFaultKind.OwnerDomainMismatch => DmaStreamComputeTokenFaultKind.DomainViolation,
	//
	//				AddressingBackendFaultKind.DeviceMismatch or AddressingBackendFaultKind.MissingDeviceBinding or AddressingBackendFaultKind.DmaDeviceFault => DmaStreamComputeTokenFaultKind.DmaDeviceFault,
	//ignore
	//ignore
	//
	//				AddressingBackendFaultKind.AlignmentFault => DmaStreamComputeTokenFaultKind.AlignmentFault,
	//
	//				AddressingBackendFaultKind.UnsupportedAddressSpace or AddressingBackendFaultKind.CapabilityMismatch => DmaStreamComputeTokenFaultKind.UnsupportedAbiOrOperation,
	//ignore
	//
	//				_ => DmaStreamComputeTokenFaultKind.MemoryFault
	//			};
		switch (faultKind)
		{
		case AddressingBackendFaultKind::TranslationFault:
		case AddressingBackendFaultKind::MissingMappingEpoch:
		case AddressingBackendFaultKind::MappingEpochRevoked: return DmaStreamComputeTokenFaultKind::TranslationFault;
		case AddressingBackendFaultKind::PermissionFault: return DmaStreamComputeTokenFaultKind::PermissionFault;
		case AddressingBackendFaultKind::OwnerDomainMismatch: return DmaStreamComputeTokenFaultKind::DomainViolation;
		case AddressingBackendFaultKind::DeviceMismatch:
		case AddressingBackendFaultKind::MissingDeviceBinding:
		case AddressingBackendFaultKind::DmaDeviceFault: return DmaStreamComputeTokenFaultKind::DmaDeviceFault;
		case AddressingBackendFaultKind::AlignmentFault: return DmaStreamComputeTokenFaultKind::AlignmentFault;
		case AddressingBackendFaultKind::UnsupportedAddressSpace:
		case AddressingBackendFaultKind::CapabilityMismatch: return DmaStreamComputeTokenFaultKind::UnsupportedAbiOrOperation;
		default: return DmaStreamComputeTokenFaultKind::MemoryFault;
		}
	}

	DmaStreamComputeFaultSourcePhase AddressingBackendResolver::MapSourcePhase(AddressingBackendFaultKind faultKind)
	{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return faultKind switch
	//			{
	//				AddressingBackendFaultKind.TranslationFault or AddressingBackendFaultKind.PermissionFault or AddressingBackendFaultKind.MissingMappingEpoch or AddressingBackendFaultKind.MappingEpochRevoked => DmaStreamComputeFaultSourcePhase.Iommu,
	//ignore
	//ignore
	//ignore
	//
	//				AddressingBackendFaultKind.OwnerDomainMismatch or AddressingBackendFaultKind.DeviceMismatch or AddressingBackendFaultKind.MissingDeviceBinding or AddressingBackendFaultKind.CapabilityMismatch or AddressingBackendFaultKind.UnsupportedAddressSpace => DmaStreamComputeFaultSourcePhase.Admission,
	//ignore
	//ignore
	//ignore
	//ignore
	//
	//				_ => DmaStreamComputeFaultSourcePhase.Backend
	//			};
		switch (faultKind)
		{
		case AddressingBackendFaultKind::TranslationFault:
		case AddressingBackendFaultKind::PermissionFault:
		case AddressingBackendFaultKind::MissingMappingEpoch:
		case AddressingBackendFaultKind::MappingEpochRevoked: return DmaStreamComputeFaultSourcePhase::Iommu;
		case AddressingBackendFaultKind::OwnerDomainMismatch:
		case AddressingBackendFaultKind::DeviceMismatch:
		case AddressingBackendFaultKind::MissingDeviceBinding:
		case AddressingBackendFaultKind::CapabilityMismatch:
		case AddressingBackendFaultKind::UnsupportedAddressSpace: return DmaStreamComputeFaultSourcePhase::Admission;
		default: return DmaStreamComputeFaultSourcePhase::Backend;
		}
	}
}
