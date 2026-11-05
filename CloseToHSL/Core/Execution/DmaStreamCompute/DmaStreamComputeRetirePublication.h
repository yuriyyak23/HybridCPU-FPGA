#pragma once

#include "DmaStreamComputeToken.h"
#include "DmaStreamComputeValidationResult.h"
#include <stdexcept>
#include <memory>
#include <optional>

using namespace YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute
{
	struct DmaStreamComputeActiveTokenEntry;

	enum class DmaStreamComputeRetireObservationKind : unsigned char
	{
		StallOrDrain = 0,
		CommitReadyMemoryNotVisible = 1,
		FaultPublicationCandidate = 2,
		CancellationSuppressesPublication = 3,
		IllegalCommittedBeforeRetireObservation = 4
	};

	enum class DmaStreamComputeRetirePublicationSurfaceKind : unsigned char
	{
		ExplicitTestFutureSeam = 0,
		ProductionLane6RuntimeRetire = 1
	};

	class DmaStreamComputeRetirePublicationMetadata final
	{
	public:
		const unsigned long long TokenId, IssuingPc, BundleId, ArchitecturalInstructionAge;
		const unsigned char SlotIndex, LaneIndex;
		const SlotClass SlotClassValue, LaneClass;
		const int OwnerThreadId;
		const unsigned short OwnerVirtualThreadId;
		const unsigned int OwnerContextId, OwnerCoreId, OwnerPodId;
		const unsigned long long OwnerDomainTag, ActiveDomainCertificate;
		const unsigned int DeviceId;
		const unsigned long long DescriptorAddress, DescriptorIdentityHash, NormalizedFootprintHash, IssueCycle, CompletionCycle, ReplayEpoch;

		DmaStreamComputeRetirePublicationMetadata(unsigned long long tokenId, unsigned long long issuingPc, unsigned long long bundleId, unsigned long long architecturalInstructionAge, unsigned char slotIndex, unsigned char laneIndex, SlotClass slotClass, SlotClass laneClass, int ownerThreadId, unsigned short ownerVirtualThreadId, unsigned int ownerContextId, unsigned int ownerCoreId, unsigned int ownerPodId, unsigned long long ownerDomainTag, unsigned long long activeDomainCertificate, unsigned int deviceId, unsigned long long descriptorAddress, unsigned long long descriptorIdentityHash, unsigned long long normalizedFootprintHash, unsigned long long issueCycle, unsigned long long completionCycle, unsigned long long replayEpoch)
			: TokenId(tokenId), IssuingPc(issuingPc), BundleId(bundleId), ArchitecturalInstructionAge(architecturalInstructionAge), SlotIndex(slotIndex), LaneIndex(laneIndex), SlotClassValue(slotClass), LaneClass(laneClass), OwnerThreadId(ownerThreadId), OwnerVirtualThreadId(ownerVirtualThreadId), OwnerContextId(ownerContextId), OwnerCoreId(ownerCoreId), OwnerPodId(ownerPodId), OwnerDomainTag(ownerDomainTag), ActiveDomainCertificate(activeDomainCertificate), DeviceId(deviceId), DescriptorAddress(descriptorAddress), DescriptorIdentityHash(descriptorIdentityHash), NormalizedFootprintHash(normalizedFootprintHash), IssueCycle(issueCycle), CompletionCycle(completionCycle), ReplayEpoch(replayEpoch) {}

		bool HasRequiredFuturePreciseIdentity() const { return TokenId != 0 && SlotIndex < 8 && LaneIndex < 8 && OwnerThreadId >= 0 && DescriptorIdentityHash != 0 && DescriptorAddress != 0; }
		static std::shared_ptr<DmaStreamComputeRetirePublicationMetadata> FromActiveEntry(std::shared_ptr<DmaStreamComputeActiveTokenEntry> entry, unsigned long long architecturalInstructionAge, int ownerThreadId = -1, unsigned long long completionCycle = 0);
	};

	class DmaStreamComputeRetireObservation final
	{
	public:
		const DmaStreamComputeRetireObservationKind Kind;
		const DmaStreamComputeTokenState ObservedState;
		const std::shared_ptr<DmaStreamComputeRetirePublicationMetadata> Metadata;
		const std::optional<std::shared_ptr<DmaStreamComputeFaultRecord>> Fault;
		const DmaStreamComputeRetirePublicationSurfaceKind SurfaceKind;

		DmaStreamComputeRetireObservation(DmaStreamComputeRetireObservationKind kind, DmaStreamComputeTokenState observedState, std::shared_ptr<DmaStreamComputeRetirePublicationMetadata> metadata, std::optional<std::shared_ptr<DmaStreamComputeFaultRecord>> fault, DmaStreamComputeRetirePublicationSurfaceKind surfaceKind = DmaStreamComputeRetirePublicationSurfaceKind::ExplicitTestFutureSeam)
			: Kind(kind), ObservedState(observedState), Metadata(std::move(metadata)), Fault(std::move(fault)), SurfaceKind(surfaceKind)
		{
			ArgumentNullException::ThrowIfNull(Metadata);
		}

		bool IsExplicitFuturePublicationSurface() const { return SurfaceKind == DmaStreamComputeRetirePublicationSurfaceKind::ExplicitTestFutureSeam; }
		bool IsNormalPipelineExecutableLane6Path() const { return SurfaceKind == DmaStreamComputeRetirePublicationSurfaceKind::ProductionLane6RuntimeRetire; }
		bool MemoryMayBeVisible() const { return Kind == DmaStreamComputeRetireObservationKind::IllegalCommittedBeforeRetireObservation; }
		bool RequiresRetireStallOrDrain() const { return Kind == DmaStreamComputeRetireObservationKind::StallOrDrain; }
		bool RequiresCommitAttempt() const { return Kind == DmaStreamComputeRetireObservationKind::CommitReadyMemoryNotVisible; }
		bool RequiresExceptionPublication() const { return Kind == DmaStreamComputeRetireObservationKind::FaultPublicationCandidate && Fault.has_value() && Fault.value() && Fault.value()->getRequiresRetireExceptionPublication(); }
	};

	// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record DmaStreamComputeRetirePublicationMetadata
//	{
//		public DmaStreamComputeRetirePublicationMetadata(ulong tokenId, ulong issuingPc, ulong bundleId, ulong architecturalInstructionAge, byte slotIndex, byte laneIndex, SlotClass slotClass, SlotClass laneClass, int ownerThreadId, ushort ownerVirtualThreadId, uint ownerContextId, uint ownerCoreId, uint ownerPodId, ulong ownerDomainTag, ulong activeDomainCertificate, uint deviceId, ulong descriptorAddress, ulong descriptorIdentityHash, ulong normalizedFootprintHash, ulong issueCycle, ulong completionCycle, ulong replayEpoch)
//		{
//			TokenId = tokenId;
//			IssuingPc = issuingPc;
//			BundleId = bundleId;
//			ArchitecturalInstructionAge = architecturalInstructionAge;
//			SlotIndex = slotIndex;
//			LaneIndex = laneIndex;
//			SlotClass = slotClass;
//			LaneClass = laneClass;
//			OwnerThreadId = ownerThreadId;
//			OwnerVirtualThreadId = ownerVirtualThreadId;
//			OwnerContextId = ownerContextId;
//			OwnerCoreId = ownerCoreId;
//			OwnerPodId = ownerPodId;
//			OwnerDomainTag = ownerDomainTag;
//			ActiveDomainCertificate = activeDomainCertificate;
//			DeviceId = deviceId;
//			DescriptorAddress = descriptorAddress;
//			DescriptorIdentityHash = descriptorIdentityHash;
//			NormalizedFootprintHash = normalizedFootprintHash;
//			IssueCycle = issueCycle;
//			CompletionCycle = completionCycle;
//			ReplayEpoch = replayEpoch;
//		}
//
//		public ulong TokenId
//		{
//			get;
//		}
//
//		public ulong IssuingPc
//		{
//			get;
//		}
//
//		public ulong BundleId
//		{
//			get;
//		}
//
//		public ulong ArchitecturalInstructionAge
//		{
//			get;
//		}
//
//		public byte SlotIndex
//		{
//			get;
//		}
//
//		public byte LaneIndex
//		{
//			get;
//		}
//
//		public SlotClass SlotClass
//		{
//			get;
//		}
//
//		public SlotClass LaneClass
//		{
//			get;
//		}
//
//		public int OwnerThreadId
//		{
//			get;
//		}
//
//		public ushort OwnerVirtualThreadId
//		{
//			get;
//		}
//
//		public uint OwnerContextId
//		{
//			get;
//		}
//
//		public uint OwnerCoreId
//		{
//			get;
//		}
//
//		public uint OwnerPodId
//		{
//			get;
//		}
//
//		public ulong OwnerDomainTag
//		{
//			get;
//		}
//
//		public ulong ActiveDomainCertificate
//		{
//			get;
//		}
//
//		public uint DeviceId
//		{
//			get;
//		}
//
//		public ulong DescriptorAddress
//		{
//			get;
//		}
//
//		public ulong DescriptorIdentityHash
//		{
//			get;
//		}
//
//		public ulong NormalizedFootprintHash
//		{
//			get;
//		}
//
//		public ulong IssueCycle
//		{
//			get;
//		}
//
//		public ulong CompletionCycle
//		{
//			get;
//		}
//
//		public ulong ReplayEpoch
//		{
//			get;
//		}
//
//		public bool HasRequiredFuturePreciseIdentity
//		{
//			get
//			{
//				return TokenId != 0 && SlotIndex < 8 && LaneIndex < 8 && OwnerThreadId >= 0 && DescriptorIdentityHash != 0 && DescriptorAddress != 0;
//			}
//		}
//
//		public static DmaStreamComputeRetirePublicationMetadata FromActiveEntry(DmaStreamComputeActiveTokenEntry entry, ulong architecturalInstructionAge, int ownerThreadId = -1, ulong completionCycle = 0)
//		{
//			ArgumentNullException.ThrowIfNull(entry);
//
//			DmaStreamComputeDescriptor descriptor = entry.Token.Descriptor;
//			DmaStreamComputeOwnerBinding owner = descriptor.OwnerBinding;
//			DmaStreamComputeOwnerGuardContext guardContext = descriptor.OwnerGuardDecision.RuntimeOwnerContext;
//			int resolvedOwnerThreadId = ownerThreadId >= 0 ? ownerThreadId : owner.OwnerVirtualThreadId;
//
//			return new DmaStreamComputeRetirePublicationMetadata(entry.Handle.TokenId, entry.Metadata.IssuingPc, entry.Metadata.BundleId, architecturalInstructionAge, entry.Metadata.SlotIndex, entry.Metadata.LaneIndex, SlotClass.DmaStreamClass, SlotClass.DmaStreamClass, resolvedOwnerThreadId, owner.OwnerVirtualThreadId, owner.OwnerContextId, owner.OwnerCoreId, owner.OwnerPodId, owner.OwnerDomainTag, guardContext.ActiveDomainCertificate, owner.DeviceId, descriptor.DescriptorReference.DescriptorAddress, descriptor.DescriptorIdentityHash, descriptor.NormalizedFootprintHash, entry.Metadata.IssueCycle, completionCycle, entry.Metadata.ReplayEpoch);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record DmaStreamComputeRetireObservation
//	{
//		public DmaStreamComputeRetireObservation(DmaStreamComputeRetireObservationKind kind, DmaStreamComputeTokenState observedState, DmaStreamComputeRetirePublicationMetadata metadata, System.Nullable<DmaStreamComputeFaultRecord> fault, DmaStreamComputeRetirePublicationSurfaceKind surfaceKind = DmaStreamComputeRetirePublicationSurfaceKind.ExplicitTestFutureSeam)
//		{
//			ArgumentNullException.ThrowIfNull(metadata);
//			Kind = kind;
//			ObservedState = observedState;
//			Metadata = metadata;
//			Fault = fault;
//			SurfaceKind = surfaceKind;
//		}
//
//		public DmaStreamComputeRetireObservationKind Kind
//		{
//			get;
//		}
//
//		public DmaStreamComputeTokenState ObservedState
//		{
//			get;
//		}
//
//		public DmaStreamComputeRetirePublicationMetadata Metadata
//		{
//			get;
//		}
//
//		public System.Nullable<DmaStreamComputeFaultRecord> Fault
//		{
//			get;
//		}
//
//		public DmaStreamComputeRetirePublicationSurfaceKind SurfaceKind
//		{
//			get;
//		}
//
//		public bool IsExplicitFuturePublicationSurface
//		{
//			get
//			{
//				return SurfaceKind == DmaStreamComputeRetirePublicationSurfaceKind.ExplicitTestFutureSeam;
//			}
//		}
//
//		public bool IsNormalPipelineExecutableLane6Path
//		{
//			get
//			{
//				return SurfaceKind == DmaStreamComputeRetirePublicationSurfaceKind.ProductionLane6RuntimeRetire;
//			}
//		}
//
//		public bool MemoryMayBeVisible
//		{
//			get
//			{
//				return Kind == DmaStreamComputeRetireObservationKind.IllegalCommittedBeforeRetireObservation;
//			}
//		}
//
//		public bool RequiresRetireStallOrDrain
//		{
//			get
//			{
//				return Kind == DmaStreamComputeRetireObservationKind.StallOrDrain;
//			}
//		}
//
//		public bool RequiresCommitAttempt
//		{
//			get
//			{
//				return Kind == DmaStreamComputeRetireObservationKind.CommitReadyMemoryNotVisible;
//			}
//		}
//
//		public bool RequiresExceptionPublication
//		{
//			get
//			{
//				return Kind == DmaStreamComputeRetireObservationKind.FaultPublicationCandidate && (Fault == nullptr ? nullptr : Fault.RequiresRetireExceptionPublication) == true;
//			}
//		}
//	}

	class DmaStreamComputeRetirePublication final
	{
	public:
		static std::shared_ptr<DmaStreamComputeRetireObservation> ObserveFutureRetire(std::shared_ptr<DmaStreamComputeActiveTokenEntry> entry, unsigned long long architecturalInstructionAge, int ownerThreadId = -1, unsigned long long completionCycle = 0);

		static std::shared_ptr<DmaStreamComputeRetireObservation> ObserveProductionRetire(std::shared_ptr<DmaStreamComputeActiveTokenEntry> entry, unsigned long long architecturalInstructionAge, int ownerThreadId = -1, unsigned long long completionCycle = 0);

	private:
		static std::shared_ptr<DmaStreamComputeRetireObservation> ObserveRetire(std::shared_ptr<DmaStreamComputeActiveTokenEntry> entry, unsigned long long architecturalInstructionAge, int ownerThreadId, unsigned long long completionCycle, DmaStreamComputeRetirePublicationSurfaceKind surfaceKind);

	public:
		static std::shared_ptr<DmaStreamComputeCommitResult> NormalizeBackendExceptionToTokenFault(std::shared_ptr<DmaStreamComputeToken> token, std::runtime_error exception, unsigned long long faultAddress, bool isWrite, DmaStreamComputeFaultSourcePhase sourcePhase = DmaStreamComputeFaultSourcePhase::Backend);

		static DmaStreamComputeFaultPriorityClass ResolveSameTokenFaultPriority(DmaStreamComputeTokenFaultKind faultKind, DmaStreamComputeFaultSourcePhase sourcePhase);

		static DmaStreamComputeFaultPriorityClass ResolveValidationFaultPriority(DmaStreamComputeValidationFault validationFault);
	};
}
