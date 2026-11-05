#include "AssistRuntime.h"
#include "../../MicroOps/Assist/AssistMicroOp.h"
#include "../../MicroOps/Types/MicroOp.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <algorithm>

namespace YAKSys_Hybrid_CPU::Core
{

	SlotClass AssistCarrierKindExtensions::ResolveSlotClass(AssistCarrierKind carrierKind)
	{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return carrierKind switch
	//		{
	//			AssistCarrierKind.LsuHosted => SlotClass.LsuClass,
	//			AssistCarrierKind.Lane6Dma => SlotClass.DmaStreamClass,
	//			_ => throw new ArgumentOutOfRangeException(nameof(carrierKind), carrierKind, "Unknown assist carrier kind.")
	//		};
		switch (carrierKind)
		{
		case AssistCarrierKind::LsuHosted: return SlotClass::LsuClass;
		case AssistCarrierKind::Lane6Dma: return SlotClass::DmaStreamClass;
		default: throw ArgumentOutOfRangeException(L"carrierKind", carrierKind, L"Unknown assist carrier kind.");
		}
	}

	bool AssistTupleSupport::IsSupportedCarrierTuple(AssistKind assistKind, AssistExecutionMode executionMode, AssistCarrierKind carrierKind)
	{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return assistKind switch
	//			{
	//				AssistKind.Vdsa => executionMode == AssistExecutionMode.StreamRegisterPrefetch &&
	//					carrierKind == AssistCarrierKind.Lane6Dma, AssistKind.Ldsa => (executionMode == AssistExecutionMode.CachePrefetch &&
	//					 carrierKind == AssistCarrierKind.LsuHosted) ||
	//					(executionMode == AssistExecutionMode.StreamRegisterPrefetch &&
	//					 carrierKind == AssistCarrierKind.Lane6Dma),
	//				AssistKind.DonorPrefetch =>
	//					(executionMode == AssistExecutionMode.CachePrefetch &&
	//					 carrierKind == AssistCarrierKind.LsuHosted) ||
	//					(executionMode == AssistExecutionMode.StreamRegisterPrefetch &&
	//					 carrierKind == AssistCarrierKind.Lane6Dma),
	//				_ => false
	//			};
		switch (assistKind)
		{
		case AssistKind::Vdsa:
			return executionMode == AssistExecutionMode::StreamRegisterPrefetch && carrierKind == AssistCarrierKind::Lane6Dma;
		case AssistKind::Ldsa:
		case AssistKind::DonorPrefetch:
			return (executionMode == AssistExecutionMode::CachePrefetch && carrierKind == AssistCarrierKind::LsuHosted) ||
				(executionMode == AssistExecutionMode::StreamRegisterPrefetch && carrierKind == AssistCarrierKind::Lane6Dma);
		default:
			return false;
		}
	}

	AssistDonorSourceDescriptor::AssistDonorSourceDescriptor(AssistDonorSourceKind kind, int donorVirtualThreadId, int targetVirtualThreadId, int ownerContextId, unsigned long long domainTag, int sourceCoreId, unsigned short sourcePodId, unsigned long long sourceAssistEpochId)
	{
		Kind = kind;
		DonorVirtualThreadId = donorVirtualThreadId;
		TargetVirtualThreadId = targetVirtualThreadId;
		OwnerContextId = ownerContextId;
		DomainTag = domainTag;
		SourceCoreId = sourceCoreId;
		SourcePodId = sourcePodId;
		SourceAssistEpochId = sourceAssistEpochId;
	}

	AssistDonorSourceKind AssistDonorSourceDescriptor::getKind() const
	{
		return Kind;
	}

	int AssistDonorSourceDescriptor::getDonorVirtualThreadId() const
	{
		return DonorVirtualThreadId;
	}

	int AssistDonorSourceDescriptor::getTargetVirtualThreadId() const
	{
		return TargetVirtualThreadId;
	}

	int AssistDonorSourceDescriptor::getOwnerContextId() const
	{
		return OwnerContextId;
	}

	unsigned long long AssistDonorSourceDescriptor::getDomainTag() const
	{
		return DomainTag;
	}

	int AssistDonorSourceDescriptor::getSourceCoreId() const
	{
		return SourceCoreId;
	}

	unsigned short AssistDonorSourceDescriptor::getSourcePodId() const
	{
		return SourcePodId;
	}

	unsigned long long AssistDonorSourceDescriptor::getSourceAssistEpochId() const
	{
		return SourceAssistEpochId;
	}

	bool AssistDonorSourceDescriptor::getHasExplicitCoreSource() const
	{
		return getSourceCoreId() >= 0;
	}

	bool AssistDonorSourceDescriptor::isCrossVirtualThread() const
	{
		return getDonorVirtualThreadId() != getTargetVirtualThreadId();
	}

	bool AssistDonorSourceDescriptor::TryCreate(AssistKind assistKind, AssistExecutionMode executionMode, AssistCarrierKind carrierKind, int donorVirtualThreadId, int targetVirtualThreadId, int ownerContextId, unsigned long long domainTag, int sourceCoreId, unsigned short sourcePodId, unsigned long long sourceAssistEpochId, AssistDonorSourceDescriptor &donorSource)
	{
		return TryCreate(assistKind, executionMode, carrierKind, donorVirtualThreadId, targetVirtualThreadId, ownerContextId, domainTag, sourceCoreId, sourcePodId, sourceAssistEpochId, false, false, false, false, false, donorSource);
	}

	bool AssistDonorSourceDescriptor::TryCreate(AssistKind assistKind, AssistExecutionMode executionMode, AssistCarrierKind carrierKind, int donorVirtualThreadId, int targetVirtualThreadId, int ownerContextId, unsigned long long domainTag, int sourceCoreId, unsigned short sourcePodId, unsigned long long sourceAssistEpochId, bool isWriteBackVectorSeed, bool isHotLoadScalarSeed, bool isHotStoreScalarSeed, bool isColdStoreScalarSeed, bool isDefaultStoreScalarSeed, AssistDonorSourceDescriptor &donorSource)
	{
		donorSource = YAKSys_Hybrid_CPU::Core::AssistDonorSourceDescriptor();

		bool isNormalizedDonor = donorVirtualThreadId >= 0 && donorVirtualThreadId < Processor::CPU_Core::SmtWays;
		bool isNormalizedTarget = targetVirtualThreadId >= 0 && targetVirtualThreadId < Processor::CPU_Core::SmtWays;
		if (!isNormalizedDonor || !isNormalizedTarget)
		{
			return false;
		}

	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			AssistDonorSourceKind kind = assistKind switch
	//			{
	//				AssistKind.Vdsa when sourceCoreId >= 0 &&
	//								   isWriteBackVectorSeed &&
	//								   donorVirtualThreadId == targetVirtualThreadId => AssistDonorSourceKind.InterCoreSameVtVectorWriteback,
	//				AssistKind.Vdsa when sourceCoreId >= 0 &&
	//								   isWriteBackVectorSeed
	//					=> AssistDonorSourceKind.InterCoreVtDonorVectorWriteback,
	//				AssistKind.Vdsa when sourceCoreId >= 0 &&
	//								   donorVirtualThreadId == targetVirtualThreadId
	//					=> AssistDonorSourceKind.InterCoreSameVtVector,
	//				AssistKind.Vdsa when sourceCoreId >= 0
	//					=> AssistDonorSourceKind.InterCoreVtDonorVector,
	//				AssistKind.Vdsa when donorVirtualThreadId != targetVirtualThreadId
	//					=> AssistDonorSourceKind.IntraCoreVtDonorVector,
	//				AssistKind.DonorPrefetch when sourceCoreId >= 0 &&
	//											  carrierKind == AssistCarrierKind.Lane6Dma &&
	//											  executionMode == AssistExecutionMode.StreamRegisterPrefetch &&
	//											  isHotLoadScalarSeed &&
	//											  donorVirtualThreadId == targetVirtualThreadId
	//					=> AssistDonorSourceKind.InterCoreSameVtHotLoadSeed,
	//				AssistKind.DonorPrefetch when sourceCoreId >= 0 &&
	//											  carrierKind == AssistCarrierKind.Lane6Dma &&
	//											  executionMode == AssistExecutionMode.StreamRegisterPrefetch &&
	//											  isHotLoadScalarSeed
	//					=> AssistDonorSourceKind.InterCoreVtDonorHotLoadSeed,
	//				AssistKind.DonorPrefetch when sourceCoreId >= 0 &&
	//											  carrierKind == AssistCarrierKind.Lane6Dma &&
	//											  executionMode == AssistExecutionMode.StreamRegisterPrefetch &&
	//											  isHotStoreScalarSeed &&
	//											  donorVirtualThreadId == targetVirtualThreadId
	//					=> AssistDonorSourceKind.InterCoreSameVtHotStoreSeed,
	//				AssistKind.DonorPrefetch when sourceCoreId >= 0 &&
	//											  carrierKind == AssistCarrierKind.Lane6Dma &&
	//											  executionMode == AssistExecutionMode.StreamRegisterPrefetch &&
	//											  isHotStoreScalarSeed
	//					=> AssistDonorSourceKind.InterCoreVtDonorHotStoreSeed,
	//				AssistKind.DonorPrefetch when sourceCoreId >= 0 &&
	//											  carrierKind == AssistCarrierKind.Lane6Dma &&
	//											  executionMode == AssistExecutionMode.StreamRegisterPrefetch &&
	//											  isDefaultStoreScalarSeed &&
	//											  donorVirtualThreadId == targetVirtualThreadId
	//					=> AssistDonorSourceKind.InterCoreSameVtDefaultStoreSeed,
	//				AssistKind.DonorPrefetch when sourceCoreId >= 0 &&
	//											  carrierKind == AssistCarrierKind.Lane6Dma &&
	//											  executionMode == AssistExecutionMode.StreamRegisterPrefetch &&
	//											  isDefaultStoreScalarSeed
	//					=> AssistDonorSourceKind.InterCoreVtDonorDefaultStoreSeed,
	//				AssistKind.Ldsa when sourceCoreId >= 0 &&
	//									 carrierKind == AssistCarrierKind.Lane6Dma &&
	//									 executionMode == AssistExecutionMode.StreamRegisterPrefetch &&
	//									 isColdStoreScalarSeed &&
	//									 donorVirtualThreadId == targetVirtualThreadId
	//					=> AssistDonorSourceKind.InterCoreSameVtColdStoreSeed,
	//				AssistKind.Ldsa when sourceCoreId >= 0 &&
	//									 carrierKind == AssistCarrierKind.Lane6Dma &&
	//									 executionMode == AssistExecutionMode.StreamRegisterPrefetch &&
	//									 isColdStoreScalarSeed
	//					=> AssistDonorSourceKind.InterCoreVtDonorColdStoreSeed,
	//				AssistKind.DonorPrefetch or AssistKind.Ldsa when sourceCoreId >= 0 &&
	//																	 donorVirtualThreadId != targetVirtualThreadId
	//					=> AssistDonorSourceKind.InterCoreVtDonorSeed,
	//				AssistKind.DonorPrefetch or AssistKind.Ldsa when sourceCoreId >= 0
	//					=> AssistDonorSourceKind.InterCoreSameVtSeed,
	//				_ => AssistDonorSourceKind.SameThreadSeed
	//			};

		AssistDonorSourceKind kind = AssistDonorSourceKind::SameThreadSeed;
		switch (assistKind)
		{
		case AssistKind::Vdsa:
			if (sourceCoreId >= 0)
			{
				if (isWriteBackVectorSeed)
				{
					kind = donorVirtualThreadId == targetVirtualThreadId
						? AssistDonorSourceKind::InterCoreSameVtVectorWriteback
						: AssistDonorSourceKind::InterCoreVtDonorVectorWriteback;
				}
				else
				{
					kind = donorVirtualThreadId == targetVirtualThreadId
						? AssistDonorSourceKind::InterCoreSameVtVector
						: AssistDonorSourceKind::InterCoreVtDonorVector;
				}
			}
			else if (donorVirtualThreadId != targetVirtualThreadId)
			{
				kind = AssistDonorSourceKind::IntraCoreVtDonorVector;
			}
			break;
		case AssistKind::DonorPrefetch:
			if (sourceCoreId >= 0 && carrierKind == AssistCarrierKind::Lane6Dma && executionMode == AssistExecutionMode::StreamRegisterPrefetch)
			{
				if (isHotLoadScalarSeed)
				{
					kind = donorVirtualThreadId == targetVirtualThreadId
						? AssistDonorSourceKind::InterCoreSameVtHotLoadSeed
						: AssistDonorSourceKind::InterCoreVtDonorHotLoadSeed;
				}
				else if (isHotStoreScalarSeed)
				{
					kind = donorVirtualThreadId == targetVirtualThreadId
						? AssistDonorSourceKind::InterCoreSameVtHotStoreSeed
						: AssistDonorSourceKind::InterCoreVtDonorHotStoreSeed;
				}
				else if (isDefaultStoreScalarSeed)
				{
					kind = donorVirtualThreadId == targetVirtualThreadId
						? AssistDonorSourceKind::InterCoreSameVtDefaultStoreSeed
						: AssistDonorSourceKind::InterCoreVtDonorDefaultStoreSeed;
				}
				else if (donorVirtualThreadId != targetVirtualThreadId)
				{
					kind = AssistDonorSourceKind::InterCoreVtDonorSeed;
				}
				else
				{
					kind = AssistDonorSourceKind::InterCoreSameVtSeed;
				}
			}
			else if (sourceCoreId >= 0)
			{
				kind = donorVirtualThreadId != targetVirtualThreadId
					? AssistDonorSourceKind::InterCoreVtDonorSeed
					: AssistDonorSourceKind::InterCoreSameVtSeed;
			}
			break;
		case AssistKind::Ldsa:
			if (sourceCoreId >= 0 && carrierKind == AssistCarrierKind::Lane6Dma && executionMode == AssistExecutionMode::StreamRegisterPrefetch && isColdStoreScalarSeed)
			{
				kind = donorVirtualThreadId == targetVirtualThreadId
					? AssistDonorSourceKind::InterCoreSameVtColdStoreSeed
					: AssistDonorSourceKind::InterCoreVtDonorColdStoreSeed;
			}
			else if (sourceCoreId >= 0)
			{
				kind = donorVirtualThreadId != targetVirtualThreadId
					? AssistDonorSourceKind::InterCoreVtDonorSeed
					: AssistDonorSourceKind::InterCoreSameVtSeed;
			}
			break;
		default:
			break;
		}
		donorSource = AssistDonorSourceDescriptor(kind, donorVirtualThreadId, targetVirtualThreadId, ownerContextId, domainTag, sourceCoreId, sourcePodId, sourceAssistEpochId);
		return donorSource.IsLegalFor(assistKind, executionMode, carrierKind);
	}

	bool AssistDonorSourceDescriptor::TryCreate(AssistKind assistKind, AssistExecutionMode executionMode, AssistCarrierKind carrierKind, int donorVirtualThreadId, int targetVirtualThreadId, int ownerContextId, unsigned long long domainTag, AssistDonorSourceDescriptor &donorSource)
	{
		return TryCreate(assistKind, executionMode, carrierKind, donorVirtualThreadId, targetVirtualThreadId, ownerContextId, domainTag, false, false, false, false, false, donorSource);
	}

	bool AssistDonorSourceDescriptor::TryCreate(AssistKind assistKind, AssistExecutionMode executionMode, AssistCarrierKind carrierKind, int donorVirtualThreadId, int targetVirtualThreadId, int ownerContextId, unsigned long long domainTag, bool isWriteBackVectorSeed, bool isHotLoadScalarSeed, bool isHotStoreScalarSeed, bool isColdStoreScalarSeed, bool isDefaultStoreScalarSeed, AssistDonorSourceDescriptor &donorSource)
	{
		return TryCreate(assistKind, executionMode, carrierKind, donorVirtualThreadId, targetVirtualThreadId, ownerContextId, domainTag, -1, 0, 0, isWriteBackVectorSeed, isHotLoadScalarSeed, isHotStoreScalarSeed, isColdStoreScalarSeed, isDefaultStoreScalarSeed, donorSource);
	}

	bool AssistDonorSourceDescriptor::IsLegalFor(AssistKind assistKind, AssistExecutionMode executionMode, AssistCarrierKind carrierKind) const
	{
		if (getDonorVirtualThreadId() < 0 || getDonorVirtualThreadId() >= Processor::CPU_Core::SmtWays || getTargetVirtualThreadId() < 0 || getTargetVirtualThreadId() >= Processor::CPU_Core::SmtWays)
		{
			return false;
		}

		if (!AssistTupleSupport::IsSupportedCarrierTuple(assistKind, executionMode, carrierKind))
		{
			return false;
		}

	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return assistKind switch
	//			{
	//				AssistKind.Vdsa => (Kind == AssistDonorSourceKind.SameThreadSeed &&
	//									DonorVirtualThreadId == TargetVirtualThreadId &&
	//									!HasExplicitCoreSource) ||
	//								   (Kind == AssistDonorSourceKind.IntraCoreVtDonorVector &&
	//									DonorVirtualThreadId != TargetVirtualThreadId &&
	//									!HasExplicitCoreSource) ||
	//								   ((Kind == AssistDonorSourceKind.InterCoreSameVtVector ||
	//									 Kind == AssistDonorSourceKind.InterCoreSameVtVectorWriteback) &&
	//									DonorVirtualThreadId == TargetVirtualThreadId &&
	//									HasExplicitCoreSource) ||
	//								   ((Kind == AssistDonorSourceKind.InterCoreVtDonorVector ||
	//									 Kind == AssistDonorSourceKind.InterCoreVtDonorVectorWriteback) &&
	//									DonorVirtualThreadId != TargetVirtualThreadId &&
	//									HasExplicitCoreSource), AssistKind.DonorPrefetch when carrierKind == AssistCarrierKind.LsuHosted => (Kind == AssistDonorSourceKind.SameThreadSeed &&
	//					 DonorVirtualThreadId == TargetVirtualThreadId &&
	//					 !HasExplicitCoreSource),
	//				AssistKind.Ldsa when carrierKind == AssistCarrierKind.LsuHosted =>
	//					((Kind == AssistDonorSourceKind.SameThreadSeed &&
	//					  DonorVirtualThreadId == TargetVirtualThreadId &&
	//					  !HasExplicitCoreSource)),
	//				AssistKind.DonorPrefetch when carrierKind == AssistCarrierKind.Lane6Dma =>
	//					((Kind == AssistDonorSourceKind.InterCoreSameVtSeed &&
	//					  DonorVirtualThreadId == TargetVirtualThreadId &&
	//					  HasExplicitCoreSource) ||
	//					 (Kind == AssistDonorSourceKind.InterCoreSameVtHotLoadSeed &&
	//					  DonorVirtualThreadId == TargetVirtualThreadId &&
	//					  HasExplicitCoreSource) ||
	//					 (Kind == AssistDonorSourceKind.InterCoreSameVtHotStoreSeed &&
	//					  DonorVirtualThreadId == TargetVirtualThreadId &&
	//					  HasExplicitCoreSource) ||
	//					 (Kind == AssistDonorSourceKind.InterCoreSameVtDefaultStoreSeed &&
	//					  DonorVirtualThreadId == TargetVirtualThreadId &&
	//					  HasExplicitCoreSource) ||
	//					 (Kind == AssistDonorSourceKind.InterCoreVtDonorSeed &&
	//					  DonorVirtualThreadId != TargetVirtualThreadId &&
	//					  HasExplicitCoreSource) ||
	//					 (Kind == AssistDonorSourceKind.InterCoreVtDonorHotLoadSeed &&
	//					  DonorVirtualThreadId != TargetVirtualThreadId &&
	//					  HasExplicitCoreSource) ||
	//					 (Kind == AssistDonorSourceKind.InterCoreVtDonorHotStoreSeed &&
	//					  DonorVirtualThreadId != TargetVirtualThreadId &&
	//					  HasExplicitCoreSource) ||
	//					 (Kind == AssistDonorSourceKind.InterCoreVtDonorDefaultStoreSeed &&
	//					  DonorVirtualThreadId != TargetVirtualThreadId &&
	//					  HasExplicitCoreSource)),
	//				AssistKind.Ldsa when carrierKind == AssistCarrierKind.Lane6Dma =>
	//					((Kind == AssistDonorSourceKind.InterCoreSameVtSeed &&
	//					  DonorVirtualThreadId == TargetVirtualThreadId &&
	//					  HasExplicitCoreSource) ||
	//					 (Kind == AssistDonorSourceKind.InterCoreSameVtColdStoreSeed &&
	//					  DonorVirtualThreadId == TargetVirtualThreadId &&
	//					  HasExplicitCoreSource) ||
	//					 (Kind == AssistDonorSourceKind.InterCoreVtDonorSeed &&
	//					  DonorVirtualThreadId != TargetVirtualThreadId &&
	//					  HasExplicitCoreSource) ||
	//					 (Kind == AssistDonorSourceKind.InterCoreVtDonorColdStoreSeed &&
	//					  DonorVirtualThreadId != TargetVirtualThreadId &&
	//					  HasExplicitCoreSource)),
	//				_ => false
	//			};
		switch (assistKind)
		{
		case AssistKind::Vdsa:
			return (Kind == AssistDonorSourceKind::SameThreadSeed && DonorVirtualThreadId == TargetVirtualThreadId && !getHasExplicitCoreSource()) ||
				(Kind == AssistDonorSourceKind::IntraCoreVtDonorVector && DonorVirtualThreadId != TargetVirtualThreadId && !getHasExplicitCoreSource()) ||
				((Kind == AssistDonorSourceKind::InterCoreSameVtVector || Kind == AssistDonorSourceKind::InterCoreSameVtVectorWriteback) && DonorVirtualThreadId == TargetVirtualThreadId && getHasExplicitCoreSource()) ||
				((Kind == AssistDonorSourceKind::InterCoreVtDonorVector || Kind == AssistDonorSourceKind::InterCoreVtDonorVectorWriteback) && DonorVirtualThreadId != TargetVirtualThreadId && getHasExplicitCoreSource());
		case AssistKind::DonorPrefetch:
			if (carrierKind == AssistCarrierKind::LsuHosted)
			{
				return Kind == AssistDonorSourceKind::SameThreadSeed && DonorVirtualThreadId == TargetVirtualThreadId && !getHasExplicitCoreSource();
			}
			if (carrierKind == AssistCarrierKind::Lane6Dma)
			{
				return ((Kind == AssistDonorSourceKind::InterCoreSameVtSeed || Kind == AssistDonorSourceKind::InterCoreSameVtHotLoadSeed || Kind == AssistDonorSourceKind::InterCoreSameVtHotStoreSeed || Kind == AssistDonorSourceKind::InterCoreSameVtDefaultStoreSeed) && DonorVirtualThreadId == TargetVirtualThreadId && getHasExplicitCoreSource()) ||
					((Kind == AssistDonorSourceKind::InterCoreVtDonorSeed || Kind == AssistDonorSourceKind::InterCoreVtDonorHotLoadSeed || Kind == AssistDonorSourceKind::InterCoreVtDonorHotStoreSeed || Kind == AssistDonorSourceKind::InterCoreVtDonorDefaultStoreSeed) && DonorVirtualThreadId != TargetVirtualThreadId && getHasExplicitCoreSource());
			}
			return false;
		case AssistKind::Ldsa:
			if (carrierKind == AssistCarrierKind::LsuHosted)
			{
				return Kind == AssistDonorSourceKind::SameThreadSeed && DonorVirtualThreadId == TargetVirtualThreadId && !getHasExplicitCoreSource();
			}
			if (carrierKind == AssistCarrierKind::Lane6Dma)
			{
				return ((Kind == AssistDonorSourceKind::InterCoreSameVtSeed || Kind == AssistDonorSourceKind::InterCoreSameVtColdStoreSeed) && DonorVirtualThreadId == TargetVirtualThreadId && getHasExplicitCoreSource()) ||
					((Kind == AssistDonorSourceKind::InterCoreVtDonorSeed || Kind == AssistDonorSourceKind::InterCoreVtDonorColdStoreSeed) && DonorVirtualThreadId != TargetVirtualThreadId && getHasExplicitCoreSource());
			}
			return false;
		default:
			return false;
		}
	}

	bool AssistDonorSourceDescriptor::MatchesScope(int ownerContextId, unsigned long long domainTag) const
	{
		return getOwnerContextId() == ownerContextId && getDomainTag() == domainTag;
	}

	bool AssistDonorSourceDescriptor::MatchesCoreSource(int sourceCoreId, unsigned short sourcePodId) const
	{
		return getSourceCoreId() == sourceCoreId && getSourcePodId() == sourcePodId;
	}

	unsigned long long AssistOwnershipFingerprint::Compute(std::shared_ptr<AssistMicroOp> assistMicroOp)
	{
		ArgumentNullException::ThrowIfNull(assistMicroOp);

		unsigned long long hash = FnvOffsetBasis;
		hash = Mix(hash, static_cast<unsigned char>(assistMicroOp->getKind()));
		hash = Mix(hash, static_cast<unsigned char>(assistMicroOp->getExecutionMode()));
		hash = Mix(hash, static_cast<unsigned char>(assistMicroOp->getCarrierKind()));
		hash = Mix(hash, static_cast<unsigned char>(assistMicroOp->getCarrierSlotClass()));
		hash = Mix(hash, static_cast<unsigned char>(assistMicroOp->getDonorSource().getKind()));
		hash = Mix(hash, assistMicroOp->getCarrierVirtualThreadId());
		hash = Mix(hash, assistMicroOp->getDonorVirtualThreadId());
		hash = Mix(hash, assistMicroOp->getTargetVirtualThreadId());
		hash = Mix(hash, assistMicroOp->getCarrierCoreId());
		hash = Mix(hash, assistMicroOp->getTargetCoreId());
		hash = Mix(hash, assistMicroOp->getPodId());
		hash = Mix(hash, assistMicroOp->getOwnerContextId());
		hash = Mix(hash, assistMicroOp->getPlacement().DomainTag);
		hash = Mix(hash, assistMicroOp->getDonorSource().getOwnerContextId());
		hash = Mix(hash, assistMicroOp->getDonorSource().getDomainTag());
		hash = Mix(hash, assistMicroOp->getDonorSource().getSourceCoreId());
		hash = Mix(hash, assistMicroOp->getDonorSource().getSourcePodId());
		return hash == 0 ? 1ULL : hash;
	}

	unsigned long long AssistOwnershipFingerprint::Mix(unsigned long long hash, unsigned char value)
	{
		hash ^= value;
		hash *= FnvPrime;
		return hash;
	}

	unsigned long long AssistOwnershipFingerprint::Mix(unsigned long long hash, unsigned short value)
	{
		hash = Mix(hash, static_cast<unsigned char>(value & 0xFF));
		hash = Mix(hash, static_cast<unsigned char>((value >> 8) & 0xFF));
		return hash;
	}

	unsigned long long AssistOwnershipFingerprint::Mix(unsigned long long hash, int value)
	{
//		unchecked
			unsigned int normalized = static_cast<unsigned int>(value);
			hash = Mix(hash, static_cast<unsigned char>(normalized & 0xFF));
			hash = Mix(hash, static_cast<unsigned char>((normalized >> 8) & 0xFF));
			hash = Mix(hash, static_cast<unsigned char>((normalized >> 16) & 0xFF));
			hash = Mix(hash, static_cast<unsigned char>((normalized >> 24) & 0xFF));
			return hash;
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
	}

	unsigned long long AssistOwnershipFingerprint::Mix(unsigned long long hash, unsigned long long value)
	{
		for (int shift = 0; shift < 64; shift += 8)
		{
			hash = Mix(hash, static_cast<unsigned char>((value >> shift) & 0xFF));
		}

		return hash;
	}

	AssistStreamRegisterPartitionPolicy::AssistStreamRegisterPartitionPolicy(unsigned char residentRegisterBudget, unsigned char loadingRegisterBudget)
		: ResidentRegisterBudget(residentRegisterBudget == 0 ? static_cast<unsigned char>(1) : residentRegisterBudget),
		  LoadingRegisterBudget(loadingRegisterBudget == 0 ? static_cast<unsigned char>(1) : loadingRegisterBudget)
	{
	}

	unsigned char AssistStreamRegisterPartitionPolicy::getResidentRegisterBudget() const
	{
		return ResidentRegisterBudget;
	}

	unsigned char AssistStreamRegisterPartitionPolicy::getLoadingRegisterBudget() const
	{
		return LoadingRegisterBudget;
	}

	AssistStreamRegisterPartitionPolicy AssistStreamRegisterPartitionPolicy::getDefault()
	{
		return Default;
	}

	AssistOwnerBinding::AssistOwnerBinding(int carrierVirtualThreadId, int donorVirtualThreadId, int targetVirtualThreadId, int ownerContextId, unsigned long long domainTag, unsigned long long replayEpochId, unsigned long long assistEpochId, YAKSys_Hybrid_CPU::Core::LocalityHint localityHint, AssistDonorSourceDescriptor donorSource, int carrierCoreId, int targetCoreId, unsigned short podId)
		: CarrierVirtualThreadId(carrierVirtualThreadId), DonorVirtualThreadId(donorVirtualThreadId), TargetVirtualThreadId(targetVirtualThreadId),
		  OwnerContextId(ownerContextId), DomainTag(domainTag), ReplayEpochId(replayEpochId), AssistEpochId(assistEpochId), LocalityHint(localityHint),
		  DonorSource(donorSource), CarrierCoreId(carrierCoreId), TargetCoreId(targetCoreId), PodId(podId)
	{
	}

	AssistOwnerBinding::AssistOwnerBinding(int carrierVirtualThreadId, int donorVirtualThreadId, int targetVirtualThreadId, int ownerContextId, unsigned long long domainTag, unsigned long long replayEpochId, unsigned long long assistEpochId, YAKSys_Hybrid_CPU::Core::LocalityHint localityHint) : AssistOwnerBinding(carrierVirtualThreadId, donorVirtualThreadId, targetVirtualThreadId, ownerContextId, domainTag, replayEpochId, assistEpochId, localityHint, AssistDonorSourceDescriptor(AssistDonorSourceKind::SameThreadSeed, donorVirtualThreadId, targetVirtualThreadId, ownerContextId, domainTag))
	{
	}

	int AssistOwnerBinding::getCarrierVirtualThreadId() const
	{
		return CarrierVirtualThreadId;
	}

	int AssistOwnerBinding::getDonorVirtualThreadId() const
	{
		return DonorVirtualThreadId;
	}

	int AssistOwnerBinding::getTargetVirtualThreadId() const
	{
		return TargetVirtualThreadId;
	}

	int AssistOwnerBinding::getOwnerContextId() const
	{
		return OwnerContextId;
	}

	unsigned long long AssistOwnerBinding::getDomainTag() const
	{
		return DomainTag;
	}

	unsigned long long AssistOwnerBinding::getReplayEpochId() const
	{
		return ReplayEpochId;
	}

	unsigned long long AssistOwnerBinding::getAssistEpochId() const
	{
		return AssistEpochId;
	}

	YAKSys_Hybrid_CPU::Core::LocalityHint AssistOwnerBinding::getLocalityHint() const
	{
		return LocalityHint;
	}

	AssistDonorSourceDescriptor AssistOwnerBinding::getDonorSource() const
	{
		return DonorSource;
	}

	int AssistOwnerBinding::getCarrierCoreId() const
	{
		return CarrierCoreId;
	}

	int AssistOwnerBinding::getTargetCoreId() const
	{
		return TargetCoreId;
	}

	unsigned short AssistOwnerBinding::getPodId() const
	{
		return PodId;
	}

	bool AssistOwnerBinding::getHasExplicitCoreOwnership() const
	{
		return getCarrierCoreId() >= 0 || getTargetCoreId() >= 0;
	}

	AssistInterCoreTransport::AssistInterCoreTransport(std::shared_ptr<MicroOp> seed, int donorCoreId, unsigned short donorPodId, unsigned long long donorAssistEpochId)
	{
	if (seed != nullptr)
	{
		Seed = seed;
	}
	else
	{
		throw std::invalid_argument("seed");
	}
		if (!AssistMicroOp::IsSupportedInterCoreSeed(seed))
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Inter-core assist transport only supports the landed current class set.", nameof(seed));
			throw std::invalid_argument("Inter-core assist transport only supports the landed current class set.");
		}

		DonorCoreId = donorCoreId;
		DonorPodId = donorPodId;
		DonorAssistEpochId = donorAssistEpochId;
		DonorVirtualThreadId = seed->getVirtualThreadId() >= 0 && seed->getVirtualThreadId() < Processor::CPU_Core::SmtWays ? seed->getVirtualThreadId() : std::clamp(seed->getOwnerThreadId(), 0, Processor::CPU_Core::SmtWays - 1);
		DonorOwnerContextId = seed->getOwnerContextId();
		DonorDomainTag = seed->getPlacement().DomainTag;
	}

	std::shared_ptr<MicroOp> AssistInterCoreTransport::getSeed() const
	{
		return Seed;
	}

	int AssistInterCoreTransport::getDonorCoreId() const
	{
		return DonorCoreId;
	}

	unsigned short AssistInterCoreTransport::getDonorPodId() const
	{
		return DonorPodId;
	}

	unsigned long long AssistInterCoreTransport::getDonorAssistEpochId() const
	{
		return DonorAssistEpochId;
	}

	int AssistInterCoreTransport::getDonorVirtualThreadId() const
	{
		return DonorVirtualThreadId;
	}

	int AssistInterCoreTransport::getDonorOwnerContextId() const
	{
		return DonorOwnerContextId;
	}

	unsigned long long AssistInterCoreTransport::getDonorDomainTag() const
	{
		return DonorDomainTag;
	}

	bool AssistInterCoreTransport::isSupportedCurrentClassSet() const
	{
		return getSeed() != nullptr && AssistMicroOp::IsSupportedInterCoreSeed(getSeed());
	}

	bool AssistInterCoreTransport::isValid() const
	{
		return getSeed() != nullptr && isSupportedCurrentClassSet() && getDonorCoreId() >= 0 && getDonorVirtualThreadId() >= 0 && getDonorVirtualThreadId() < Processor::CPU_Core::SmtWays;
	}

	AssistMemoryQuota::AssistMemoryQuota(unsigned char issueCredits, unsigned short lineCredits, unsigned char hotLineCap, unsigned char coldLineCap)
		: IssueCredits(issueCredits), LineCredits(lineCredits), HotLineCap(hotLineCap == 0 ? static_cast<unsigned char>(1) : hotLineCap),
		  ColdLineCap(coldLineCap == 0 ? HotLineCap : coldLineCap)
	{
	}

	unsigned char AssistMemoryQuota::getIssueCredits() const
	{
		return IssueCredits;
	}

	unsigned short AssistMemoryQuota::getLineCredits() const
	{
		return LineCredits;
	}

	unsigned char AssistMemoryQuota::getHotLineCap() const
	{
		return HotLineCap;
	}

	unsigned char AssistMemoryQuota::getColdLineCap() const
	{
		return ColdLineCap;
	}

	unsigned char AssistMemoryQuota::ResolveLineCap(LocalityHint localityHint) const
	{
		return localityHint == LocalityHint::Cold ? getColdLineCap() : getHotLineCap();
	}

	AssistMemoryQuotaState AssistMemoryQuota::CreateState() const
	{
		return AssistMemoryQuotaState(*this);
	}

	AssistMemoryQuota AssistMemoryQuota::getDefault()
	{
		return Default;
	}

	AssistBackpressurePolicy::AssistBackpressurePolicy(unsigned char sharedOuterCapCredits, unsigned char lsuCarrierCredits, unsigned char dmaCarrierCredits, AssistStreamRegisterPartitionPolicy dmaSrfPartitionPolicy)
		: SharedOuterCapCredits(sharedOuterCapCredits == 0 ? static_cast<unsigned char>(1) : sharedOuterCapCredits),
		  LsuCarrierCredits(lsuCarrierCredits == 0 ? static_cast<unsigned char>(1) : lsuCarrierCredits),
		  DmaCarrierCredits(dmaCarrierCredits == 0 ? static_cast<unsigned char>(1) : dmaCarrierCredits),
		  DmaSrfPartitionPolicy(dmaSrfPartitionPolicy)
	{
	}

	unsigned char AssistBackpressurePolicy::getSharedOuterCapCredits() const
	{
		return SharedOuterCapCredits;
	}

	unsigned char AssistBackpressurePolicy::getLsuCarrierCredits() const
	{
		return LsuCarrierCredits;
	}

	unsigned char AssistBackpressurePolicy::getDmaCarrierCredits() const
	{
		return DmaCarrierCredits;
	}

	AssistStreamRegisterPartitionPolicy AssistBackpressurePolicy::getDmaSrfPartitionPolicy() const
	{
		return DmaSrfPartitionPolicy;
	}

	unsigned char AssistBackpressurePolicy::ResolveCarrierCredits(AssistCarrierKind carrierKind) const
	{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return carrierKind switch
	//		{
	//			AssistCarrierKind.LsuHosted => LsuCarrierCredits,
	//			AssistCarrierKind.Lane6Dma => DmaCarrierCredits,
	//			_ => SharedOuterCapCredits
	//		};
		switch (carrierKind)
		{
		case AssistCarrierKind::LsuHosted: return LsuCarrierCredits;
		case AssistCarrierKind::Lane6Dma: return DmaCarrierCredits;
		default: return SharedOuterCapCredits;
		}
	}

	AssistBackpressureState AssistBackpressurePolicy::CreateState(AssistBackpressureSnapshot snapshot) const
	{
		return AssistBackpressureState(*this, snapshot);
	}

	AssistBackpressurePolicy AssistBackpressurePolicy::getDefault()
	{
		return Default;
	}

	AssistBackpressureSnapshot::AssistBackpressureSnapshot(unsigned char sharedOuterCapCredits, unsigned int consumedSharedReadBudgetByBank, unsigned short sharedReadBudgetAtLeastOneMask, unsigned short sharedReadBudgetAtLeastTwoMask, unsigned char projectedOutstandingCountVt0, unsigned char projectedOutstandingCountVt1, unsigned char projectedOutstandingCountVt2, unsigned char projectedOutstandingCountVt3, unsigned char projectedOutstandingCapacityVt0, unsigned char projectedOutstandingCapacityVt1, unsigned char projectedOutstandingCapacityVt2, unsigned char projectedOutstandingCapacityVt3)
		: SharedOuterCapCredits(sharedOuterCapCredits), ConsumedSharedReadBudgetByBank(consumedSharedReadBudgetByBank),
		  SharedReadBudgetAtLeastOneMask(sharedReadBudgetAtLeastOneMask), SharedReadBudgetAtLeastTwoMask(sharedReadBudgetAtLeastTwoMask),
		  ProjectedOutstandingCountVt0(projectedOutstandingCountVt0), ProjectedOutstandingCountVt1(projectedOutstandingCountVt1),
		  ProjectedOutstandingCountVt2(projectedOutstandingCountVt2), ProjectedOutstandingCountVt3(projectedOutstandingCountVt3),
		  ProjectedOutstandingCapacityVt0(projectedOutstandingCapacityVt0), ProjectedOutstandingCapacityVt1(projectedOutstandingCapacityVt1),
		  ProjectedOutstandingCapacityVt2(projectedOutstandingCapacityVt2), ProjectedOutstandingCapacityVt3(projectedOutstandingCapacityVt3)
	{
	}

	unsigned char AssistBackpressureSnapshot::getSharedOuterCapCredits() const
	{
		return SharedOuterCapCredits;
	}

	unsigned int AssistBackpressureSnapshot::getConsumedSharedReadBudgetByBank() const
	{
		return ConsumedSharedReadBudgetByBank;
	}

	unsigned short AssistBackpressureSnapshot::getSharedReadBudgetAtLeastOneMask() const
	{
		return SharedReadBudgetAtLeastOneMask;
	}

	unsigned short AssistBackpressureSnapshot::getSharedReadBudgetAtLeastTwoMask() const
	{
		return SharedReadBudgetAtLeastTwoMask;
	}

	unsigned char AssistBackpressureSnapshot::getProjectedOutstandingCountVt0() const
	{
		return ProjectedOutstandingCountVt0;
	}

	unsigned char AssistBackpressureSnapshot::getProjectedOutstandingCountVt1() const
	{
		return ProjectedOutstandingCountVt1;
	}

	unsigned char AssistBackpressureSnapshot::getProjectedOutstandingCountVt2() const
	{
		return ProjectedOutstandingCountVt2;
	}

	unsigned char AssistBackpressureSnapshot::getProjectedOutstandingCountVt3() const
	{
		return ProjectedOutstandingCountVt3;
	}

	unsigned char AssistBackpressureSnapshot::getProjectedOutstandingCapacityVt0() const
	{
		return ProjectedOutstandingCapacityVt0;
	}

	unsigned char AssistBackpressureSnapshot::getProjectedOutstandingCapacityVt1() const
	{
		return ProjectedOutstandingCapacityVt1;
	}

	unsigned char AssistBackpressureSnapshot::getProjectedOutstandingCapacityVt2() const
	{
		return ProjectedOutstandingCapacityVt2;
	}

	unsigned char AssistBackpressureSnapshot::getProjectedOutstandingCapacityVt3() const
	{
		return ProjectedOutstandingCapacityVt3;
	}

	AssistCachePartitionPolicy::AssistCachePartitionPolicy(unsigned char totalLineBudget, unsigned char lsuHostedLineBudget, unsigned char dmaHostedLineBudget)
		: TotalLineBudget(totalLineBudget == 0 ? static_cast<unsigned char>(1) : totalLineBudget),
		  LsuHostedLineBudget(lsuHostedLineBudget == 0 ? TotalLineBudget : lsuHostedLineBudget),
		  DmaHostedLineBudget(dmaHostedLineBudget == 0 ? static_cast<unsigned char>(1) : dmaHostedLineBudget)
	{
	}

	unsigned char AssistCachePartitionPolicy::getTotalLineBudget() const
	{
		return TotalLineBudget;
	}

	unsigned char AssistCachePartitionPolicy::getLsuHostedLineBudget() const
	{
		return LsuHostedLineBudget;
	}

	unsigned char AssistCachePartitionPolicy::getDmaHostedLineBudget() const
	{
		return DmaHostedLineBudget;
	}

	unsigned char AssistCachePartitionPolicy::ResolveCarrierLineBudget(AssistCarrierKind carrierKind) const
	{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return carrierKind switch
	//		{
	//			AssistCarrierKind.LsuHosted => LsuHostedLineBudget,
	//			AssistCarrierKind.Lane6Dma => DmaHostedLineBudget,
	//			_ => TotalLineBudget
	//		};
		switch (carrierKind)
		{
		case AssistCarrierKind::LsuHosted: return LsuHostedLineBudget;
		case AssistCarrierKind::Lane6Dma: return DmaHostedLineBudget;
		default: return TotalLineBudget;
		}
	}

	AssistCachePartitionPolicy AssistCachePartitionPolicy::getDefault()
	{
		return Default;
	}

	AssistMemoryQuotaState::AssistMemoryQuotaState(AssistMemoryQuota quota) : _quota(quota)
	{
		_remainingIssueCredits = quota.getIssueCredits();
		_remainingLineCredits = quota.getLineCredits();
	}

	unsigned char AssistMemoryQuotaState::getRemainingIssueCredits() const
	{
		return _remainingIssueCredits;
	}

	unsigned short AssistMemoryQuotaState::getRemainingLineCredits() const
	{
		return _remainingLineCredits;
	}

	bool AssistMemoryQuotaState::TryReserve(std::shared_ptr<AssistMicroOp> assistMicroOp, unsigned int &reservedLineDemand, AssistQuotaRejectKind &rejectKind)
	{
		ArgumentNullException::ThrowIfNull(assistMicroOp);

		reservedLineDemand = assistMicroOp->EstimatePrefetchLineDemand(_quota);
		rejectKind = AssistQuotaRejectKind::None;

		if (_remainingIssueCredits == 0)
		{
			rejectKind = AssistQuotaRejectKind::IssueCredits;
			return false;
		}

		if (_remainingLineCredits < reservedLineDemand)
		{
			rejectKind = AssistQuotaRejectKind::LineCredits;
			return false;
		}

		_remainingIssueCredits--;
		_remainingLineCredits -= static_cast<unsigned short>(reservedLineDemand);
		return true;
	}

	AssistBackpressureState::AssistBackpressureState(AssistBackpressurePolicy policy, AssistBackpressureSnapshot snapshot) : _policy(policy), _sharedReadBudgetAtLeastOneMask(snapshot.getSharedReadBudgetAtLeastOneMask()), _sharedReadBudgetAtLeastTwoMask(snapshot.getSharedReadBudgetAtLeastTwoMask()), _projectedOutstandingCapacityVt0(snapshot.getProjectedOutstandingCapacityVt0()), _projectedOutstandingCapacityVt1(snapshot.getProjectedOutstandingCapacityVt1()), _projectedOutstandingCapacityVt2(snapshot.getProjectedOutstandingCapacityVt2()), _projectedOutstandingCapacityVt3(snapshot.getProjectedOutstandingCapacityVt3())
	{
		_remainingSharedOuterCapCredits = std::min(policy.getSharedOuterCapCredits(), snapshot.getSharedOuterCapCredits());
		_remainingLsuCarrierCredits = policy.getLsuCarrierCredits();
		_remainingDmaCarrierCredits = policy.getDmaCarrierCredits();
		_projectedOutstandingCountVt0 = snapshot.getProjectedOutstandingCountVt0();
		_projectedOutstandingCountVt1 = snapshot.getProjectedOutstandingCountVt1();
		_projectedOutstandingCountVt2 = snapshot.getProjectedOutstandingCountVt2();
		_projectedOutstandingCountVt3 = snapshot.getProjectedOutstandingCountVt3();
		_consumedSharedReadBudgetByBank = snapshot.getConsumedSharedReadBudgetByBank();
	}

	AssistStreamRegisterPartitionPolicy AssistBackpressureState::getDmaSrfPartitionPolicy() const
	{
		return _policy.getDmaSrfPartitionPolicy();
	}

	bool AssistBackpressureState::TryReserve(std::shared_ptr<AssistMicroOp> assistMicroOp, bool dmaSrfAvailable, AssistBackpressureRejectKind &rejectKind)
	{
		ArgumentNullException::ThrowIfNull(assistMicroOp);

		rejectKind = AssistBackpressureRejectKind::None;
		int targetVirtualThreadId = assistMicroOp->getTargetVirtualThreadId();
		if (!HasProjectedOutstandingCapacity(targetVirtualThreadId))
		{
			rejectKind = AssistBackpressureRejectKind::OutstandingMemory;
			return false;
		}

		if (!HasSharedOuterCap(assistMicroOp->getMemoryBankId()) || !HasCarrierCredits(assistMicroOp->getCarrierKind()))
		{
			rejectKind = AssistBackpressureRejectKind::SharedOuterCap;
			return false;
		}

		if (assistMicroOp->getCarrierKind() == AssistCarrierKind::Lane6Dma && !dmaSrfAvailable)
		{
			rejectKind = AssistBackpressureRejectKind::DmaStreamRegisterFile;
			return false;
		}

		ConsumeSharedOuterCap(assistMicroOp->getMemoryBankId());
		ConsumeCarrierCredits(assistMicroOp->getCarrierKind());
		IncrementProjectedOutstandingCount(targetVirtualThreadId);
		return true;
	}

	bool AssistBackpressureState::HasProjectedOutstandingCapacity(int virtualThreadId) const
	{
		bool temp_var98;
		switch (virtualThreadId)
		{
			case 0:
				temp_var98 = _projectedOutstandingCountVt0 < _projectedOutstandingCapacityVt0;
				break;
			case 1:
				temp_var98 = _projectedOutstandingCountVt1 < _projectedOutstandingCapacityVt1;
				break;
			case 2:
				temp_var98 = _projectedOutstandingCountVt2 < _projectedOutstandingCapacityVt2;
				break;
			case 3:
				temp_var98 = _projectedOutstandingCountVt3 < _projectedOutstandingCapacityVt3;
				break;
			default:
				temp_var98 = false;
				break;
		}
		return temp_var98;
	}

	bool AssistBackpressureState::HasSharedOuterCap(int memoryBankId) const
	{
		if (_remainingSharedOuterCapCredits == 0)
		{
			return false;
		}

		if (static_cast<unsigned int>(memoryBankId) >= 16)
		{
			return true;
		}

		return GetConsumedSharedReadBudget(memoryBankId) < GetSharedReadBudgetForBank(memoryBankId);
	}

	bool AssistBackpressureState::HasCarrierCredits(AssistCarrierKind carrierKind) const
	{
		bool temp_var99;
		switch (carrierKind)
		{
			case AssistCarrierKind::LsuHosted:
				temp_var99 = _remainingLsuCarrierCredits > 0;
				break;
			case AssistCarrierKind::Lane6Dma:
				temp_var99 = _remainingDmaCarrierCredits > 0;
				break;
			default:
				temp_var99 = false;
				break;
		}
		return temp_var99;
	}

	void AssistBackpressureState::ConsumeSharedOuterCap(int memoryBankId)
	{
		if (_remainingSharedOuterCapCredits > 0)
		{
			_remainingSharedOuterCapCredits--;
		}

		_consumedSharedReadBudgetByBank = IncrementPackedConsumedHardwareBudget(_consumedSharedReadBudgetByBank, memoryBankId);
	}

	void AssistBackpressureState::ConsumeCarrierCredits(AssistCarrierKind carrierKind)
	{
		switch (carrierKind)
		{
			case AssistCarrierKind::LsuHosted:
				if (_remainingLsuCarrierCredits > 0)
				{
					_remainingLsuCarrierCredits--;
				}
				break;
			case AssistCarrierKind::Lane6Dma:
				if (_remainingDmaCarrierCredits > 0)
				{
					_remainingDmaCarrierCredits--;
				}
				break;
		}
	}

	void AssistBackpressureState::IncrementProjectedOutstandingCount(int virtualThreadId)
	{
		switch (virtualThreadId)
		{
			case 0:
				if (_projectedOutstandingCountVt0 < 8)
				{
					_projectedOutstandingCountVt0++;
				}
				break;
			case 1:
				if (_projectedOutstandingCountVt1 < 8)
				{
					_projectedOutstandingCountVt1++;
				}
				break;
			case 2:
				if (_projectedOutstandingCountVt2 < 8)
				{
					_projectedOutstandingCountVt2++;
				}
				break;
			case 3:
				if (_projectedOutstandingCountVt3 < 8)
				{
					_projectedOutstandingCountVt3++;
				}
				break;
		}
	}

	unsigned char AssistBackpressureState::GetSharedReadBudgetForBank(int bankId) const
	{
		if (static_cast<unsigned int>(bankId) >= 16)
		{
			return 0;
		}

		unsigned short bankMask = static_cast<unsigned short>(1 << bankId);
		if ((_sharedReadBudgetAtLeastTwoMask & bankMask) != 0)
		{
			return 2;
		}

		return (_sharedReadBudgetAtLeastOneMask & bankMask) != 0 ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0);
	}

	unsigned char AssistBackpressureState::GetConsumedSharedReadBudget(int bankId) const
	{
		if (static_cast<unsigned int>(bankId) >= 16)
		{
			return 0;
		}

		int shift = bankId * 2;
		return static_cast<unsigned char>((_consumedSharedReadBudgetByBank >> shift) & 0x3u);
	}

	unsigned int AssistBackpressureState::IncrementPackedConsumedHardwareBudget(unsigned int consumedPackedBudgetByBank, int bankId)
	{
		if (static_cast<unsigned int>(bankId) >= 16)
		{
			return consumedPackedBudgetByBank;
		}

		int shift = bankId * 2;
		unsigned int current = (consumedPackedBudgetByBank >> shift) & 0x3u;
		if (current >= 3)
		{
			return consumedPackedBudgetByBank;
		}

		unsigned int next = current + 1u;
		return (consumedPackedBudgetByBank & ~(0x3u << shift)) | (next << shift);
	}
}
