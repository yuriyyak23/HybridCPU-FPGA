
#include "YAKSys_Hybrid_CPU.Core.MicroOpScheduler.h"
#include "../../../Diagnostics/LoopPhaseClassProfile.h"
#include <algorithm>
#include <array>
#include <span>
#include <cstdint>
#include "../../MicroOps/Types/MicroOp.IO.h"
#include "../../Metadata/MicroOpAdmissionMetadata.h"
#include "../../Metadata/BundleMetadata.h"
#include "../../../../Memory/Banks/MultiBankMemoryArea.h"
#include "../../MicroOps/Assist/AssistMicroOp.h"
#include "../../../../Memory/Subsystem/YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../Certificates/StructuralIdentity/HardwareHash.h"
#include "../../MicroOps/Types/MicroOp.Misc.h"
#include "../../MicroOps/Memory/MicroOp.LoadStore.h"
#include "../../Scheduling/Rf06MemoryShadowOracleDifferential.h"
#include "../../Scheduling/LaneChoice/DeterministicLaneChooser.h"
#include "../../Certificates/YAKSys_Hybrid_CPU.Core.RuntimeLegalityService.h"
#include "../../Scheduling/Rf06ScalarSchedulerRouting.h"
#include "../../Scheduling/Smt/MicroOpScheduler.ResearchVirtualizationCanonicalComposition.h"
#include "../../Safety/SafetyVerifier.VirtualizationAdmission.h"
#include "../../../../Memory/Registers/YAKSys_Hybrid_CPU.Memory.StreamRegisterFile.h"
#include "../../../Virtualization/Compatibility/Frontend/Retire/VmxRetireModel.h"
#include "../../../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmcsField.h"

using namespace YAKSys_Hybrid_CPU::Core::Diagnostics;
using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace HybridCPU_ISE::CloseToHSL::Memory::Banks;
using namespace YAKSys_Hybrid_CPU::Core::Decoder;
using namespace YAKSys_Hybrid_CPU::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

#include <bit>

namespace YAKSys_Hybrid_CPU::Core
{

	void MicroOpScheduler::NominateInterCoreAssistCandidate(int coreId, AssistInterCoreTransport transport)
	{
		NominateInterCoreAssistCandidateOwned(coreId, transport);
	}

	InterCoreAssistNomination::InterCoreAssistNomination(std::shared_ptr<MicroOpScheduler> owner, int coreId) : owner(owner), coreId(coreId)
	{
	}

	void InterCoreAssistNomination::Withdraw()
	{
		if (owner->_interCoreAssistNominationOwners[coreId] == shared_from_this())
		{
			owner->ClearInterCoreAssistNominationPort(coreId);
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal InterCoreAssistNomination? NominateInterCoreAssistCandidateOwned(int coreId, AssistInterCoreTransport transport)
	std::shared_ptr<InterCoreAssistNomination> MicroOpScheduler::NominateInterCoreAssistCandidateOwned(int coreId, AssistInterCoreTransport transport)
	{
		if (static_cast<unsigned int>(coreId) >= NUM_PORTS || !transport.isValid() || transport.getDonorCoreId() != coreId || _coreStalled[coreId])
		{
			return nullptr;
		}

		_interCoreAssistPorts[coreId] = transport;
		_interCoreAssistPortValid[coreId] = true;
		auto nomination = std::make_shared<InterCoreAssistNomination>(shared_from_this(), coreId);
		_interCoreAssistNominationOwners[coreId] = nomination;
		setAssistNominationCount(getAssistNominationCount() + 1);
		setAssistInterCoreNominations(getAssistInterCoreNominations() + 1);
		return nomination;
	}

	void MicroOpScheduler::ClearInterCoreAssistNominationPorts()
	{
		for (int coreId = 0; coreId < NUM_PORTS; coreId++)
		{
			ClearInterCoreAssistNominationPort(coreId);
		}
	}

	bool MicroOpScheduler::IsCoreStalled(int coreId)
	{
		return static_cast<unsigned int>(coreId) < NUM_PORTS && _coreStalled[coreId];
	}

	void MicroOpScheduler::ClearInterCoreAssistNominationPort(int coreId)
	{
		if (static_cast<unsigned int>(coreId) >= NUM_PORTS)
		{
			return;
		}

		_interCoreAssistPorts[coreId] = YAKSys_Hybrid_CPU::Core::AssistInterCoreTransport();
		_interCoreAssistPortValid[coreId] = false;
		_interCoreAssistNominationOwners[coreId] = nullptr;
	}

	void MicroOpScheduler::ReconcileInterCoreAssistTransportOwnerSnapshot(int coreId, unsigned short podId, bool ownerSnapshotValid, PodController::AssistOwnerSnapshot const ownerSnapshot)
	{
		AssistInterCoreTransport transport;
		if (!TryPeekInterCoreAssistTransport(coreId, transport))
		{
			return;
		}

		if (IsInterCoreAssistTransportStale(podId, coreId, ownerSnapshotValid, ownerSnapshot, transport))
		{
			ClearInterCoreAssistNominationPort(coreId);
		}
	}

	bool MicroOpScheduler::TryPeekInterCoreAssistTransport(int coreId, AssistInterCoreTransport &transport)
	{
		transport = YAKSys_Hybrid_CPU::Core::AssistInterCoreTransport();
		if (static_cast<unsigned int>(coreId) >= NUM_PORTS || !_interCoreAssistPortValid[coreId] || !_interCoreAssistPorts[coreId].isValid())
		{
			return false;
		}

		transport = _interCoreAssistPorts[coreId];
		return true;
	}

	bool MicroOpScheduler::TryConsumeInterCoreAssistTransport(int coreId, AssistInterCoreTransport &transport)
	{
		if (!TryPeekInterCoreAssistTransport(coreId, transport))
		{
			return false;
		}

		ClearInterCoreAssistNominationPort(coreId);
		return true;
	}

	inline bool MicroOpScheduler::IsInterCoreAssistTransportStale(unsigned short donorPodId, int donorCoreId, bool ownerSnapshotValid, PodController::AssistOwnerSnapshot const ownerSnapshot, AssistInterCoreTransport const transport)
	{
		return !transport.isValid() || transport.getDonorPodId() != donorPodId || transport.getDonorCoreId() != donorCoreId || !ownerSnapshotValid || !ownerSnapshot.Matches(transport.getDonorVirtualThreadId(), transport.getDonorOwnerContextId(), transport.getDonorDomainTag()) || ownerSnapshot.getAssistEpochId() != transport.getDonorAssistEpochId();
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private static bool IsInterCoreAssistTransportStale(PodController? donorPod, int donorCoreId, in AssistInterCoreTransport transport)
	inline bool MicroOpScheduler::IsInterCoreAssistTransportStale(std::shared_ptr<PodController> donorPod, int donorCoreId, AssistInterCoreTransport const transport)
	{
		PodController::AssistOwnerSnapshot ownerSnapshot;
		return donorPod == nullptr || donorPod->getPodId() != transport.getDonorPodId() || donorPod->getScheduler() == nullptr || donorPod->getScheduler()->IsCoreStalled(donorCoreId) || !donorPod->TryGetAssistOwnerSnapshot(donorCoreId, ownerSnapshot) || IsInterCoreAssistTransportStale(donorPod->getPodId(), donorCoreId, true, ownerSnapshot, transport);
	}

	inline bool MicroOpScheduler::TryStealPodLocalInterCoreAssistTransport(int requestingCoreId, unsigned short requestingPodId, unsigned long long requestedDomainTag, AssistInterCoreTransport &transport)
	{
		transport = YAKSys_Hybrid_CPU::Core::AssistInterCoreTransport();
		for (int coreId = 0; coreId < NUM_PORTS; coreId++)
		{
			if (coreId == requestingCoreId || !_interCoreAssistPortValid[coreId] || !_interCoreAssistPorts[coreId].isValid())
			{
				continue;
			}

			AssistInterCoreTransport candidateTransport = _interCoreAssistPorts[coreId];
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: PodController? donorPod = Processor.GetPodById(candidateTransport.DonorPodId);
			std::shared_ptr<PodController> donorPod = Processor::GetPodById(candidateTransport.getDonorPodId());
			if (IsInterCoreAssistTransportStale(donorPod, coreId, candidateTransport))
			{
				RecordAssistInterCoreReject(candidateTransport, requestingPodId);
				ClearInterCoreAssistNominationPort(coreId);
				continue;
			}

			if (requestedDomainTag != 0)
			{
				InterCoreDomainGuardDecision domainGuard = _runtimeLegalityService->EvaluateInterCoreDomainGuard(candidateTransport.getSeed(), requestedDomainTag);
				RecordDomainIsolationProbe(domainGuard.getProbeResult());
				if (!domainGuard.isAllowed())
				{
					RecordAssistInterCoreReject(candidateTransport, requestingPodId, true);
					continue;
				}
			}

			ClearInterCoreAssistNominationPort(coreId);
			transport = candidateTransport;
			return true;
		}

		return false;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private bool TryStealInterCoreAssistTransport(int requestingCoreId, ushort requestingPodId, ulong requestedDomainTag, out AssistInterCoreTransport transport, PodController?[]? pods = null)
	inline bool MicroOpScheduler::TryStealInterCoreAssistTransport(int requestingCoreId, unsigned short requestingPodId, unsigned long long requestedDomainTag, AssistInterCoreTransport &transport, std::shared_ptr<std::vector<std::shared_ptr<PodController>>> pods)
	{
		return TryStealPodLocalInterCoreAssistTransport(requestingCoreId, requestingPodId, requestedDomainTag, transport) || TryStealCrossPodAssistTransport(requestingPodId, requestedDomainTag, transport, pods);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private int TryInjectInterCoreAssistCandidates(MicroOp[] result, ref BundleResourceCertificate bundleCert, ref BundleOpportunityState opportunityState, int ownerVirtualThreadId, int requestingCoreId, ushort requestingPodId, int bundleOwnerContextId, ulong bundleOwnerDomainTag, ulong requestedDomainTag, ulong assistRuntimeEpoch, ResourceBitset globalResourceLocks, int currentPassInjections, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null, System.Nullable<PodController>[]? pods = null)
	int MicroOpScheduler::TryInjectInterCoreAssistCandidates(std::vector<std::shared_ptr<MicroOp>> &result, std::shared_ptr<BundleResourceCertificate> &bundleCert, MicroOpScheduler::BundleOpportunityState &opportunityState, int ownerVirtualThreadId, int requestingCoreId, unsigned short requestingPodId, int bundleOwnerContextId, unsigned long long bundleOwnerDomainTag, unsigned long long requestedDomainTag, unsigned long long assistRuntimeEpoch, ResourceBitset globalResourceLocks, int currentPassInjections, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub, std::shared_ptr<std::vector<std::shared_ptr<PodController>>> pods)
	{
		if (!getAssistInjectionEnabled() || result.empty() || result.size() != 8 || requestingCoreId < 0 || bundleOwnerContextId < 0 || ResolveNextInjectableSlot(opportunityState, 0) < 0)
		{
			return 0;
		}

		AssistMemoryQuotaState assistQuotaState = getAssistMemoryQuotaPolicy().CreateState();
		AssistBackpressureState assistBackpressureState = getAssistBackpressurePolicy().CreateState(CaptureAssistBackpressureSnapshot());
		int injectedCount = 0;
		SafetyMask128 hwMask = SafetyMask128(globalResourceLocks.Low, globalResourceLocks.High);

		AssistInterCoreTransport transport;
		while (injectedCount < AssistBundleQuota && TryStealInterCoreAssistTransport(requestingCoreId, requestingPodId, requestedDomainTag, transport, pods))
		{
			unsigned short targetPodId = requestingPodId == std::numeric_limits<unsigned short>::max() ? transport.getDonorPodId() : requestingPodId;

			std::shared_ptr<AssistMicroOp> assistCandidate;
			if (!AssistMicroOp::TryCreateFromInterCoreTransport(transport, targetPodId, requestingCoreId, ownerVirtualThreadId, requestingCoreId, ownerVirtualThreadId, bundleOwnerContextId, bundleOwnerDomainTag, _currentReplayPhase.isActive() ? _currentReplayPhase.getEpochId(): 0, assistRuntimeEpoch, assistCandidate))
			{
				setAssistRejects(getAssistRejects() + 1);
				RecordAssistInterCoreReject(transport, requestingPodId);
				continue;
			}

			LegalityDecision legalityDecision = EvaluateInterCoreLegality(result, bundleCert, ownerVirtualThreadId, requestedDomainTag, assistCandidate, hwMask);
			if (!legalityDecision.isAllowed())
			{
				setAssistRejects(getAssistRejects() + 1);
				RecordAssistInterCoreReject(transport, requestingPodId);
				continue;
			}

			int lane;
			TypedSlotRejectReason rejectReason;
			if (!TryMaterializeLane(assistCandidate, opportunityState.getOccupancyMask(), lane, rejectReason))
			{
				RecordAssistInterCoreReject(transport, requestingPodId);
				RecordAssistReject(assistCandidate, rejectReason);
				continue;
			}

			if (!TryReserveAssistBackpressure(assistCandidate, assistBackpressureState, memSub))
			{
				RecordAssistInterCoreReject(transport, requestingPodId);
				continue;
			}

			if (!TryReserveAssistQuota(assistCandidate, assistQuotaState))
			{
				RecordAssistInterCoreReject(transport, requestingPodId);
				continue;
			}

			result[lane] = assistCandidate;
			assistCandidate->setFspInjected(true);
			bundleCert = bundleCert->WithOperation(assistCandidate);
			opportunityState = opportunityState.WithOccupiedSlot(lane);
			_runtimeLegalityService->RefreshInterCoreAfterMutation(_currentReplayPhase, bundleCert);
			_classCapacity.IncrementOccupancy(assistCandidate->getPlacement().RequiredSlotClass);
			ConsumeProjectedMemoryIssueStateIfNeeded(assistCandidate);
			RecordTypedSlotInject(assistCandidate, lane);
			RecordAssistInjection(assistCandidate);
			RecordAssistInterCoreInjection(assistCandidate);
			injectedCount++;
		}

		return injectedCount;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private bool TryStealCrossPodAssistTransport(ushort requestingPodId, ulong requestedDomainTag, out AssistInterCoreTransport transport, PodController?[]? pods = null)
	inline bool MicroOpScheduler::TryStealCrossPodAssistTransport(unsigned short requestingPodId, unsigned long long requestedDomainTag, AssistInterCoreTransport &transport, std::shared_ptr<std::vector<std::shared_ptr<PodController>>> pods)
	{
		transport = YAKSys_Hybrid_CPU::Core::AssistInterCoreTransport();
		if (requestingPodId == std::numeric_limits<unsigned short>::max() || pods == nullptr)
		{
			return false;
		}

		for (int podIndex = 0; podIndex < pods->size(); podIndex++)
		{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: PodController? donorPod = (*pods)[podIndex];
			std::shared_ptr<PodController> donorPod = (*pods)[podIndex];
			if (donorPod == nullptr || donorPod->getPodId() == requestingPodId)
			{
				continue;
			}

			for (int coreId = 0; coreId < PodController::CORES_PER_POD; coreId++)
			{
				AssistInterCoreTransport candidateTransport;
				if (!donorPod->TryPeekInterCoreAssistTransport(coreId, candidateTransport))
				{
					continue;
				}

				if (IsInterCoreAssistTransportStale(donorPod, coreId, candidateTransport))
				{
					RecordAssistInterCoreReject(candidateTransport, requestingPodId);
					donorPod->ClearInterCoreAssistTransport(coreId);
					continue;
				}

				if (requestedDomainTag != 0)
				{
					InterCoreDomainGuardDecision domainGuard = _runtimeLegalityService->EvaluateInterCoreDomainGuard(candidateTransport.getSeed(), requestedDomainTag);
					RecordDomainIsolationProbe(domainGuard.getProbeResult());
					if (!domainGuard.isAllowed())
					{
						RecordAssistInterCoreReject(candidateTransport, requestingPodId, true);
						continue;
					}
				}

				if (donorPod->TryConsumeInterCoreAssistTransport(coreId, transport))
				{
					return true;
				}
			}
		}

		return false;
	}

	bool MicroOpScheduler::getAssistInjectionEnabled() const
	{
		return AssistInjectionEnabled;
	}

	void MicroOpScheduler::setAssistInjectionEnabled(bool value)
	{
		AssistInjectionEnabled = value;
	}

	AssistMemoryQuota MicroOpScheduler::getAssistMemoryQuotaPolicy() const
	{
		return *AssistMemoryQuotaPolicy;
	}

	void MicroOpScheduler::setAssistMemoryQuotaPolicy(AssistMemoryQuota value)
	{
		AssistMemoryQuotaPolicy = std::make_shared<AssistMemoryQuota>(value);
	}

	YAKSys_Hybrid_CPU::Core::AssistBackpressurePolicy MicroOpScheduler::getAssistBackpressurePolicy() const
	{
		return *AssistBackpressurePolicy;
	}

	void MicroOpScheduler::setAssistBackpressurePolicy(YAKSys_Hybrid_CPU::Core::AssistBackpressurePolicy value)
	{
		AssistBackpressurePolicy = std::make_shared<YAKSys_Hybrid_CPU::Core::AssistBackpressurePolicy>(value);
	}

	long long MicroOpScheduler::getAssistNominationCount() const
	{
		return AssistNominationCount;
	}

	void MicroOpScheduler::setAssistNominationCount(long long value)
	{
		AssistNominationCount = value;
	}

	long long MicroOpScheduler::getAssistInjectionsCount() const
	{
		return AssistInjectionsCount;
	}

	void MicroOpScheduler::setAssistInjectionsCount(long long value)
	{
		AssistInjectionsCount = value;
	}

	long long MicroOpScheduler::getAssistRejects() const
	{
		return AssistRejects;
	}

	void MicroOpScheduler::setAssistRejects(long long value)
	{
		AssistRejects = value;
	}

	long long MicroOpScheduler::getAssistBoundaryRejects() const
	{
		return AssistBoundaryRejects;
	}

	void MicroOpScheduler::setAssistBoundaryRejects(long long value)
	{
		AssistBoundaryRejects = value;
	}

	long long MicroOpScheduler::getAssistInvalidations() const
	{
		return AssistInvalidations;
	}

	void MicroOpScheduler::setAssistInvalidations(long long value)
	{
		AssistInvalidations = value;
	}

	long long MicroOpScheduler::getAssistInterCoreNominations() const
	{
		return AssistInterCoreNominations;
	}

	void MicroOpScheduler::setAssistInterCoreNominations(long long value)
	{
		AssistInterCoreNominations = value;
	}

	long long MicroOpScheduler::getAssistInterCoreInjections() const
	{
		return AssistInterCoreInjections;
	}

	void MicroOpScheduler::setAssistInterCoreInjections(long long value)
	{
		AssistInterCoreInjections = value;
	}

	long long MicroOpScheduler::getAssistInterCoreRejects() const
	{
		return AssistInterCoreRejects;
	}

	void MicroOpScheduler::setAssistInterCoreRejects(long long value)
	{
		AssistInterCoreRejects = value;
	}

	long long MicroOpScheduler::getAssistInterCoreDomainRejects() const
	{
		return AssistInterCoreDomainRejects;
	}

	void MicroOpScheduler::setAssistInterCoreDomainRejects(long long value)
	{
		AssistInterCoreDomainRejects = value;
	}

	long long MicroOpScheduler::getAssistInterCorePodLocalInjections() const
	{
		return AssistInterCorePodLocalInjections;
	}

	void MicroOpScheduler::setAssistInterCorePodLocalInjections(long long value)
	{
		AssistInterCorePodLocalInjections = value;
	}

	long long MicroOpScheduler::getAssistInterCoreCrossPodInjections() const
	{
		return AssistInterCoreCrossPodInjections;
	}

	void MicroOpScheduler::setAssistInterCoreCrossPodInjections(long long value)
	{
		AssistInterCoreCrossPodInjections = value;
	}

	long long MicroOpScheduler::getAssistInterCorePodLocalRejects() const
	{
		return AssistInterCorePodLocalRejects;
	}

	void MicroOpScheduler::setAssistInterCorePodLocalRejects(long long value)
	{
		AssistInterCorePodLocalRejects = value;
	}

	long long MicroOpScheduler::getAssistInterCoreCrossPodRejects() const
	{
		return AssistInterCoreCrossPodRejects;
	}

	void MicroOpScheduler::setAssistInterCoreCrossPodRejects(long long value)
	{
		AssistInterCoreCrossPodRejects = value;
	}

	long long MicroOpScheduler::getAssistInterCorePodLocalDomainRejects() const
	{
		return AssistInterCorePodLocalDomainRejects;
	}

	void MicroOpScheduler::setAssistInterCorePodLocalDomainRejects(long long value)
	{
		AssistInterCorePodLocalDomainRejects = value;
	}

	long long MicroOpScheduler::getAssistInterCoreCrossPodDomainRejects() const
	{
		return AssistInterCoreCrossPodDomainRejects;
	}

	void MicroOpScheduler::setAssistInterCoreCrossPodDomainRejects(long long value)
	{
		AssistInterCoreCrossPodDomainRejects = value;
	}

	long long MicroOpScheduler::getAssistInterCoreSameVtVectorInjects() const
	{
		return AssistInterCoreSameVtVectorInjects;
	}

	void MicroOpScheduler::setAssistInterCoreSameVtVectorInjects(long long value)
	{
		AssistInterCoreSameVtVectorInjects = value;
	}

	long long MicroOpScheduler::getAssistInterCoreDonorVtVectorInjects() const
	{
		return AssistInterCoreDonorVtVectorInjects;
	}

	void MicroOpScheduler::setAssistInterCoreDonorVtVectorInjects(long long value)
	{
		AssistInterCoreDonorVtVectorInjects = value;
	}

	long long MicroOpScheduler::getAssistInterCoreSameVtVectorWritebackInjects() const
	{
		return AssistInterCoreSameVtVectorWritebackInjects;
	}

	void MicroOpScheduler::setAssistInterCoreSameVtVectorWritebackInjects(long long value)
	{
		AssistInterCoreSameVtVectorWritebackInjects = value;
	}

	long long MicroOpScheduler::getAssistInterCoreDonorVtVectorWritebackInjects() const
	{
		return AssistInterCoreDonorVtVectorWritebackInjects;
	}

	void MicroOpScheduler::setAssistInterCoreDonorVtVectorWritebackInjects(long long value)
	{
		AssistInterCoreDonorVtVectorWritebackInjects = value;
	}

	long long MicroOpScheduler::getAssistInterCoreLane6DefaultStoreDonorPrefetchInjects() const
	{
		return AssistInterCoreLane6DefaultStoreDonorPrefetchInjects;
	}

	void MicroOpScheduler::setAssistInterCoreLane6DefaultStoreDonorPrefetchInjects(long long value)
	{
		AssistInterCoreLane6DefaultStoreDonorPrefetchInjects = value;
	}

	long long MicroOpScheduler::getAssistInterCoreLane6HotLoadDonorPrefetchInjects() const
	{
		return AssistInterCoreLane6HotLoadDonorPrefetchInjects;
	}

	void MicroOpScheduler::setAssistInterCoreLane6HotLoadDonorPrefetchInjects(long long value)
	{
		AssistInterCoreLane6HotLoadDonorPrefetchInjects = value;
	}

	long long MicroOpScheduler::getAssistInterCoreLane6HotStoreDonorPrefetchInjects() const
	{
		return AssistInterCoreLane6HotStoreDonorPrefetchInjects;
	}

	void MicroOpScheduler::setAssistInterCoreLane6HotStoreDonorPrefetchInjects(long long value)
	{
		AssistInterCoreLane6HotStoreDonorPrefetchInjects = value;
	}

	long long MicroOpScheduler::getAssistInterCoreLane6DonorPrefetchInjects() const
	{
		return AssistInterCoreLane6DonorPrefetchInjects;
	}

	void MicroOpScheduler::setAssistInterCoreLane6DonorPrefetchInjects(long long value)
	{
		AssistInterCoreLane6DonorPrefetchInjects = value;
	}

	long long MicroOpScheduler::getAssistInterCoreLane6ColdStoreLdsaInjects() const
	{
		return AssistInterCoreLane6ColdStoreLdsaInjects;
	}

	void MicroOpScheduler::setAssistInterCoreLane6ColdStoreLdsaInjects(long long value)
	{
		AssistInterCoreLane6ColdStoreLdsaInjects = value;
	}

	long long MicroOpScheduler::getAssistInterCoreLane6LdsaInjects() const
	{
		return AssistInterCoreLane6LdsaInjects;
	}

	void MicroOpScheduler::setAssistInterCoreLane6LdsaInjects(long long value)
	{
		AssistInterCoreLane6LdsaInjects = value;
	}

	long long MicroOpScheduler::getAssistQuotaRejects() const
	{
		return AssistQuotaRejects;
	}

	void MicroOpScheduler::setAssistQuotaRejects(long long value)
	{
		AssistQuotaRejects = value;
	}

	long long MicroOpScheduler::getAssistQuotaIssueRejects() const
	{
		return AssistQuotaIssueRejects;
	}

	void MicroOpScheduler::setAssistQuotaIssueRejects(long long value)
	{
		AssistQuotaIssueRejects = value;
	}

	long long MicroOpScheduler::getAssistQuotaLineRejects() const
	{
		return AssistQuotaLineRejects;
	}

	void MicroOpScheduler::setAssistQuotaLineRejects(long long value)
	{
		AssistQuotaLineRejects = value;
	}

	long long MicroOpScheduler::getAssistQuotaLinesReserved() const
	{
		return AssistQuotaLinesReserved;
	}

	void MicroOpScheduler::setAssistQuotaLinesReserved(long long value)
	{
		AssistQuotaLinesReserved = value;
	}

	long long MicroOpScheduler::getAssistBackpressureRejects() const
	{
		return AssistBackpressureRejects;
	}

	void MicroOpScheduler::setAssistBackpressureRejects(long long value)
	{
		AssistBackpressureRejects = value;
	}

	long long MicroOpScheduler::getAssistBackpressureOuterCapRejects() const
	{
		return AssistBackpressureOuterCapRejects;
	}

	void MicroOpScheduler::setAssistBackpressureOuterCapRejects(long long value)
	{
		AssistBackpressureOuterCapRejects = value;
	}

	long long MicroOpScheduler::getAssistBackpressureMshrRejects() const
	{
		return AssistBackpressureMshrRejects;
	}

	void MicroOpScheduler::setAssistBackpressureMshrRejects(long long value)
	{
		AssistBackpressureMshrRejects = value;
	}

	long long MicroOpScheduler::getAssistBackpressureDmaSrfRejects() const
	{
		return AssistBackpressureDmaSrfRejects;
	}

	void MicroOpScheduler::setAssistBackpressureDmaSrfRejects(long long value)
	{
		AssistBackpressureDmaSrfRejects = value;
	}

	long long MicroOpScheduler::getAssistDonorPrefetchInjects() const
	{
		return AssistDonorPrefetchInjects;
	}

	void MicroOpScheduler::setAssistDonorPrefetchInjects(long long value)
	{
		AssistDonorPrefetchInjects = value;
	}

	long long MicroOpScheduler::getAssistLdsaInjects() const
	{
		return AssistLdsaInjects;
	}

	void MicroOpScheduler::setAssistLdsaInjects(long long value)
	{
		AssistLdsaInjects = value;
	}

	long long MicroOpScheduler::getAssistVdsaInjects() const
	{
		return AssistVdsaInjects;
	}

	void MicroOpScheduler::setAssistVdsaInjects(long long value)
	{
		AssistVdsaInjects = value;
	}

	long long MicroOpScheduler::getAssistSameVtInjects() const
	{
		return AssistSameVtInjects;
	}

	void MicroOpScheduler::setAssistSameVtInjects(long long value)
	{
		AssistSameVtInjects = value;
	}

	long long MicroOpScheduler::getAssistDonorVtInjects() const
	{
		return AssistDonorVtInjects;
	}

	void MicroOpScheduler::setAssistDonorVtInjects(long long value)
	{
		AssistDonorVtInjects = value;
	}

	AssistInvalidationReason MicroOpScheduler::getLastAssistInvalidationReason() const
	{
		return LastAssistInvalidationReason;
	}

	void MicroOpScheduler::setLastAssistInvalidationReason(AssistInvalidationReason value)
	{
		LastAssistInvalidationReason = value;
	}

	unsigned long long MicroOpScheduler::getLastAssistOwnershipSignature() const
	{
		return LastAssistOwnershipSignature;
	}

	void MicroOpScheduler::setLastAssistOwnershipSignature(unsigned long long value)
	{
		LastAssistOwnershipSignature = value;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void NominateAssistCandidate(int vtId, MicroOp? op)
	void MicroOpScheduler::NominateAssistCandidate(int vtId, std::shared_ptr<MicroOp> op)
	{
		if (static_cast<unsigned int>(vtId) >= SMT_WAYS)
		{
			return;
		}

		if (op == nullptr || !op->isAssist())
		{
			_smtAssistPorts[vtId] = nullptr;
			_smtAssistPortValid[vtId] = false;
			return;
		}

		_smtAssistPorts[vtId] = op;
		_smtAssistPortValid[vtId] = true;
		setAssistNominationCount(getAssistNominationCount() + 1);
	}

	void MicroOpScheduler::WithdrawAssistCandidate(int vtId, std::shared_ptr<MicroOp> expected)
	{
		if (static_cast<unsigned int>(vtId) >= SMT_WAYS || _smtAssistPorts[vtId] != expected)
		{
			return;
		}
		_smtAssistPorts[vtId] = nullptr;
		_smtAssistPortValid[vtId] = false;
	}

	void MicroOpScheduler::ClearAssistNominationPorts()
	{
		for (int vtId = 0; vtId < SMT_WAYS; vtId++)
		{
			_smtAssistPorts[vtId] = nullptr;
			_smtAssistPortValid[vtId] = false;
		}
	}

	void MicroOpScheduler::InvalidateAssistNominationState(AssistInvalidationReason reason)
	{
		if (reason == AssistInvalidationReason::None)
		{
			return;
		}

		ClearAssistNominationPorts();
		ClearInterCoreAssistNominationPorts();
		setAssistInvalidations(getAssistInvalidations() + 1);
		setLastAssistInvalidationReason(reason);
	}

	int MicroOpScheduler::CountReadyAssistCandidates(int ownerVirtualThreadId)
	{
		int readyCount = 0;
		for (int vtId = 0; vtId < SMT_WAYS; vtId++)
		{
			if (vtId == ownerVirtualThreadId || !_smtAssistPortValid[vtId] || _smtAssistPorts[vtId] == nullptr)
			{
				continue;
			}

			readyCount++;
		}

		return readyCount;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private int TryInjectAssistCandidates(MicroOp[] result, ref BundleResourceCertificate4Way bundleCert, ref SmtBundleMetadata4Way bundleMetadata, ref BoundaryGuardState boundaryGuard, ref ClassTemplateAdmissionState templateState, ref BundleOpportunityState opportunityState, ref byte bundleOccupancy, int ownerVirtualThreadId, int currentPassInjections, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
	int MicroOpScheduler::TryInjectAssistCandidates(std::vector<std::shared_ptr<MicroOp>> &result, BundleResourceCertificate4Way &bundleCert, SmtBundleMetadata4Way &bundleMetadata, BoundaryGuardState &boundaryGuard, MicroOpScheduler::ClassTemplateAdmissionState &templateState, MicroOpScheduler::BundleOpportunityState &opportunityState, unsigned char &bundleOccupancy, int ownerVirtualThreadId, int currentPassInjections, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub)
	{
		if (!getAssistInjectionEnabled())
		{
			ClearAssistNominationPorts();
			return 0;
		}

		if (ResolveNextInjectableSlot(opportunityState, 0) < 0)
		{
			ClearAssistNominationPorts();
			return 0;
		}

		if (IsSmtBundleBlockedByBoundaryGuard(bundleCert, bundleMetadata, boundaryGuard))
		{
			setAssistBoundaryRejects(getAssistBoundaryRejects() + CountReadyAssistCandidates(ownerVirtualThreadId));
			ClearAssistNominationPorts();
			return 0;
		}

		std::array<int, SMT_WAYS> vtOrderStorage{};
		std::span<int> vtOrder(vtOrderStorage);
		if (getCreditFairnessEnabled())
		{
			GetCreditRankedOrder(ownerVirtualThreadId, vtOrder);
		}
		else
		{
			for (int vt = 0; vt < SMT_WAYS; vt++)
			{
				vtOrder[vt] = vt;
			}
		}

		int assistInjectedCount = 0;
		AssistMemoryQuotaState assistQuotaState = getAssistMemoryQuotaPolicy().CreateState();
		AssistBackpressureState assistBackpressureState = getAssistBackpressurePolicy().CreateState(CaptureAssistBackpressureSnapshot());

		for (int orderIndex = 0; orderIndex < SMT_WAYS && assistInjectedCount < AssistBundleQuota; orderIndex++)
		{
			int vtId = vtOrder[orderIndex];
			if (vtId == ownerVirtualThreadId || !_smtAssistPortValid[vtId] || _smtAssistPorts[vtId] == nullptr)
			{
				continue;
			}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: MicroOp candidate = _smtAssistPorts[vtId]!;
			std::shared_ptr<MicroOp> candidate = _smtAssistPorts[vtId];

			if (getTypedSlotEnabled())
			{
				TypedSlotRejectReason rejectA;
				if (!TryClassAdmission(candidate, bundleCert, bundleMetadata, boundaryGuard, templateState, ownerVirtualThreadId, currentPassInjections + assistInjectedCount, rejectA))
				{
					RecordAssistReject(candidate, rejectA);
					continue;
				}

				int lane;
				TypedSlotRejectReason rejectB;
				if (!TryMaterializeLane(candidate, bundleOccupancy, lane, rejectB))
				{
					RecordAssistReject(candidate, rejectB);
					continue;
				}

				std::shared_ptr<AssistMicroOp> typedAssistBackpressureCandidate = std::dynamic_pointer_cast<AssistMicroOp>(candidate);
				if (typedAssistBackpressureCandidate != nullptr && !TryReserveAssistBackpressure(typedAssistBackpressureCandidate, assistBackpressureState, memSub))
				{
					continue;
				}

				std::shared_ptr<AssistMicroOp> typedAssistCandidate = std::dynamic_pointer_cast<AssistMicroOp>(candidate);
				if (typedAssistCandidate != nullptr && !TryReserveAssistQuota(typedAssistCandidate, assistQuotaState))
				{
					continue;
				}

				result[lane] = candidate;
				candidate->setFspInjected(true);
				bundleCert.AddOperation(candidate);
				bundleMetadata = bundleMetadata.WithOperation(candidate);
				boundaryGuard = boundaryGuard.WithOperation(candidate);
				opportunityState = opportunityState.WithOccupiedSlot(lane);
				bundleOccupancy |= static_cast<unsigned char>(1 << lane);
				_runtimeLegalityService->RefreshSmtAfterMutation(_currentReplayPhase, bundleCert, bundleMetadata, boundaryGuard);
				_classCapacity.IncrementOccupancy(candidate->getPlacement().RequiredSlotClass);
				ConsumeProjectedMemoryIssueStateIfNeeded(candidate);
				RecordTypedSlotInject(candidate, lane);
				RecordAssistInjection(candidate);
				assistInjectedCount++;
				continue;
			}

			int slot = ResolveNextInjectableSlot(opportunityState, 0);
			if (slot < 0)
			{
				break;
			}

			LegalityDecision legalityDecision = EvaluateSmtLegality(bundleCert, bundleMetadata, boundaryGuard, candidate);
			if (!legalityDecision.isAllowed())
			{
				setAssistRejects(getAssistRejects() + 1);
				continue;
			}

			std::shared_ptr<AssistMicroOp> legacyPlacementSensitiveAssist = std::dynamic_pointer_cast<AssistMicroOp>(candidate);
			if (legacyPlacementSensitiveAssist != nullptr && legacyPlacementSensitiveAssist->getCarrierSlotClass() == SlotClass::DmaStreamClass)
			{
				// Lane6/DMA assist carriers must not silently degrade into the legacy
				// slot-agnostic path, otherwise placement truth becomes implicit again.
				setAssistRejects(getAssistRejects() + 1);
				continue;
			}

			std::shared_ptr<AssistMicroOp> legacyAssistBackpressureCandidate = std::dynamic_pointer_cast<AssistMicroOp>(candidate);
			if (legacyAssistBackpressureCandidate != nullptr && !TryReserveAssistBackpressure(legacyAssistBackpressureCandidate, assistBackpressureState, memSub))
			{
				continue;
			}

			std::shared_ptr<AssistMicroOp> legacyAssistCandidate = std::dynamic_pointer_cast<AssistMicroOp>(candidate);
			if (legacyAssistCandidate != nullptr && !TryReserveAssistQuota(legacyAssistCandidate, assistQuotaState))
			{
				continue;
			}

			result[slot] = candidate;
			candidate->setFspInjected(true);
			bundleCert.AddOperation(candidate);
			bundleMetadata = bundleMetadata.WithOperation(candidate);
			boundaryGuard = boundaryGuard.WithOperation(candidate);
			opportunityState = opportunityState.WithOccupiedSlot(slot);
			bundleOccupancy = opportunityState.getOccupancyMask();
			_runtimeLegalityService->RefreshSmtAfterMutation(_currentReplayPhase, bundleCert, bundleMetadata, boundaryGuard);
			ConsumeProjectedMemoryIssueStateIfNeeded(candidate);
			RecordAssistInjection(candidate);
			assistInjectedCount++;
		}

		ClearAssistNominationPorts();
		return assistInjectedCount;
	}

	void MicroOpScheduler::RecordAssistReject(std::shared_ptr<MicroOp> candidate, TypedSlotRejectReason rejectReason)
	{
		setAssistRejects(getAssistRejects() + 1);
		RecordTypedSlotReject(rejectReason, candidate);
	}

	bool MicroOpScheduler::TryReserveAssistQuota(std::shared_ptr<AssistMicroOp> assistMicroOp, AssistMemoryQuotaState &assistQuotaState)
	{
		unsigned int reservedLineDemand;
		AssistQuotaRejectKind rejectKind;
		if (assistQuotaState.TryReserve(assistMicroOp, reservedLineDemand, rejectKind))
		{
			assistMicroOp->BindReservedPrefetchLines(reservedLineDemand);
			setAssistQuotaLinesReserved(getAssistQuotaLinesReserved() + reservedLineDemand);
			return true;
		}

		setAssistQuotaRejects(getAssistQuotaRejects() + 1);
		switch (rejectKind)
		{
			case AssistQuotaRejectKind::IssueCredits:
				setAssistQuotaIssueRejects(getAssistQuotaIssueRejects() + 1);
				break;
			case AssistQuotaRejectKind::LineCredits:
				setAssistQuotaLineRejects(getAssistQuotaLineRejects() + 1);
				break;
		}

		RecordAssistReject(assistMicroOp, TypedSlotRejectReason::AssistQuotaReject);
		return false;
	}

	void MicroOpScheduler::RecordAssistInjection(std::shared_ptr<MicroOp> candidate)
	{
		setAssistInjectionsCount(getAssistInjectionsCount() + 1);

		std::shared_ptr<AssistMicroOp> assistMicroOp = std::dynamic_pointer_cast<AssistMicroOp>(candidate);
		if (assistMicroOp == nullptr)
		{
			return;
		}

		setLastAssistOwnershipSignature(AssistOwnershipFingerprint::Compute(assistMicroOp));

		if (assistMicroOp->getDonorSource().isCrossVirtualThread())
		{
			setAssistDonorVtInjects(getAssistDonorVtInjects() + 1);
		}
		else
		{
			setAssistSameVtInjects(getAssistSameVtInjects() + 1);
		}

		switch (assistMicroOp->getKind())
		{
			case AssistKind::DonorPrefetch:
				setAssistDonorPrefetchInjects(getAssistDonorPrefetchInjects() + 1);
				break;
			case AssistKind::Ldsa:
				setAssistLdsaInjects(getAssistLdsaInjects() + 1);
				break;
			case AssistKind::Vdsa:
				setAssistVdsaInjects(getAssistVdsaInjects() + 1);
				break;
		}
	}

	void MicroOpScheduler::RecordAssistInterCoreInjection(std::shared_ptr<AssistMicroOp> assistMicroOp)
	{
		setAssistInterCoreInjections(getAssistInterCoreInjections() + 1);

		if (assistMicroOp->getDonorSource().getSourcePodId() != assistMicroOp->getPodId())
		{
			setAssistInterCoreCrossPodInjections(getAssistInterCoreCrossPodInjections() + 1);
		}
		else
		{
			setAssistInterCorePodLocalInjections(getAssistInterCorePodLocalInjections() + 1);
		}

		if (assistMicroOp->getKind() == AssistKind::Ldsa && assistMicroOp->getCarrierKind() == AssistCarrierKind::Lane6Dma)
		{
			if (assistMicroOp->getDonorSource().getKind() == AssistDonorSourceKind::InterCoreSameVtColdStoreSeed || assistMicroOp->getDonorSource().getKind() == AssistDonorSourceKind::InterCoreVtDonorColdStoreSeed)
			{
				setAssistInterCoreLane6ColdStoreLdsaInjects(getAssistInterCoreLane6ColdStoreLdsaInjects() + 1);
				return;
			}

			setAssistInterCoreLane6LdsaInjects(getAssistInterCoreLane6LdsaInjects() + 1);
			return;
		}

		if (assistMicroOp->getKind() == AssistKind::DonorPrefetch && assistMicroOp->getCarrierKind() == AssistCarrierKind::Lane6Dma)
		{
			if (assistMicroOp->getDonorSource().getKind() == AssistDonorSourceKind::InterCoreSameVtHotLoadSeed || assistMicroOp->getDonorSource().getKind() == AssistDonorSourceKind::InterCoreVtDonorHotLoadSeed)
			{
				setAssistInterCoreLane6HotLoadDonorPrefetchInjects(getAssistInterCoreLane6HotLoadDonorPrefetchInjects() + 1);
				return;
			}

			if (assistMicroOp->getDonorSource().getKind() == AssistDonorSourceKind::InterCoreSameVtHotStoreSeed || assistMicroOp->getDonorSource().getKind() == AssistDonorSourceKind::InterCoreVtDonorHotStoreSeed)
			{
				setAssistInterCoreLane6HotStoreDonorPrefetchInjects(getAssistInterCoreLane6HotStoreDonorPrefetchInjects() + 1);
				return;
			}

			if (assistMicroOp->getDonorSource().getKind() == AssistDonorSourceKind::InterCoreSameVtDefaultStoreSeed || assistMicroOp->getDonorSource().getKind() == AssistDonorSourceKind::InterCoreVtDonorDefaultStoreSeed)
			{
				setAssistInterCoreLane6DefaultStoreDonorPrefetchInjects(getAssistInterCoreLane6DefaultStoreDonorPrefetchInjects() + 1);
				return;
			}

			setAssistInterCoreLane6DonorPrefetchInjects(getAssistInterCoreLane6DonorPrefetchInjects() + 1);
			return;
		}

		if (assistMicroOp->getKind() != AssistKind::Vdsa)
		{
			return;
		}

		switch (assistMicroOp->getDonorSource().getKind())
		{
			case AssistDonorSourceKind::InterCoreSameVtVectorWriteback:
				setAssistInterCoreSameVtVectorWritebackInjects(getAssistInterCoreSameVtVectorWritebackInjects() + 1);
				break;
			case AssistDonorSourceKind::InterCoreVtDonorVectorWriteback:
				setAssistInterCoreDonorVtVectorWritebackInjects(getAssistInterCoreDonorVtVectorWritebackInjects() + 1);
				break;
			case AssistDonorSourceKind::InterCoreSameVtVector:
				setAssistInterCoreSameVtVectorInjects(getAssistInterCoreSameVtVectorInjects() + 1);
				break;
			case AssistDonorSourceKind::InterCoreVtDonorVector:
				setAssistInterCoreDonorVtVectorInjects(getAssistInterCoreDonorVtVectorInjects() + 1);
				break;
		}
	}

	void MicroOpScheduler::RecordAssistInterCoreReject(AssistInterCoreTransport const transport, unsigned short requestingPodId, bool isDomainReject)
	{
		setAssistInterCoreRejects(getAssistInterCoreRejects() + 1);

		bool isCrossPod = requestingPodId != std::numeric_limits<unsigned short>::max() && transport.getDonorPodId() != requestingPodId;

		if (isCrossPod)
		{
			setAssistInterCoreCrossPodRejects(getAssistInterCoreCrossPodRejects() + 1);
			if (isDomainReject)
			{
				setAssistInterCoreCrossPodDomainRejects(getAssistInterCoreCrossPodDomainRejects() + 1);
			}
		}
		else
		{
			setAssistInterCorePodLocalRejects(getAssistInterCorePodLocalRejects() + 1);
			if (isDomainReject)
			{
				setAssistInterCorePodLocalDomainRejects(getAssistInterCorePodLocalDomainRejects() + 1);
			}
		}

		if (isDomainReject)
		{
			setAssistInterCoreDomainRejects(getAssistInterCoreDomainRejects() + 1);
		}
	}

	long long MicroOpScheduler::getFairnessStarvationEvents() const
	{
		return FairnessStarvationEvents;
	}

	void MicroOpScheduler::setFairnessStarvationEvents(long long value)
	{
		FairnessStarvationEvents = value;
	}

	bool MicroOpScheduler::getCreditFairnessEnabled() const
	{
		return CreditFairnessEnabled;
	}

	void MicroOpScheduler::setCreditFairnessEnabled(bool value)
	{
		CreditFairnessEnabled = value;
	}

	bool MicroOpScheduler::getEnableLoopPhaseSampling() const
	{
		return EnableLoopPhaseSampling;
	}

	void MicroOpScheduler::setEnableLoopPhaseSampling(bool value)
	{
		EnableLoopPhaseSampling = value;
	}

	long long MicroOpScheduler::getBankPressureAvoidanceCount() const
	{
		return BankPressureAvoidanceCount;
	}

	void MicroOpScheduler::setBankPressureAvoidanceCount(long long value)
	{
		BankPressureAvoidanceCount = value;
	}

	bool MicroOpScheduler::getBankPressureTieBreakEnabled() const
	{
		return BankPressureTieBreakEnabled;
	}

	void MicroOpScheduler::setBankPressureTieBreakEnabled(bool value)
	{
		BankPressureTieBreakEnabled = value;
	}

	int MicroOpScheduler::getSpeculationBudgetMax() const
	{
		return SpeculationBudgetMax;
	}

	void MicroOpScheduler::setSpeculationBudgetMax(int value)
	{
		SpeculationBudgetMax = value;
	}

	long long MicroOpScheduler::getSpeculationBudgetExhaustionEvents() const
	{
		return SpeculationBudgetExhaustionEvents;
	}

	void MicroOpScheduler::setSpeculationBudgetExhaustionEvents(long long value)
	{
		SpeculationBudgetExhaustionEvents = value;
	}

	long long MicroOpScheduler::getPeakConcurrentSpeculativeOps() const
	{
		return PeakConcurrentSpeculativeOps;
	}

	void MicroOpScheduler::setPeakConcurrentSpeculativeOps(long long value)
	{
		PeakConcurrentSpeculativeOps = value;
	}

	bool MicroOpScheduler::getSpeculationBudgetEnabled() const
	{
		return SpeculationBudgetEnabled;
	}

	void MicroOpScheduler::setSpeculationBudgetEnabled(bool value)
	{
		SpeculationBudgetEnabled = value;
	}

	MicroOpScheduler::FspSnapshotFingerprint::FspSnapshotFingerprint(unsigned long long low, unsigned long long high)
	{
		Low = low;
		High = high;
	}

	unsigned long long MicroOpScheduler::FspSnapshotFingerprint::getLow() const
	{
		return Low;
	}

	unsigned long long MicroOpScheduler::FspSnapshotFingerprint::getHigh() const
	{
		return High;
	}

	bool MicroOpScheduler::FspSnapshotFingerprint::Equals(FspSnapshotFingerprint other) const
	{
		return getLow() == other.getLow() && getHigh() == other.getHigh();
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
	bool MicroOpScheduler::FspSnapshotFingerprint::Equals(std::any obj) const
	{
		const auto *other = std::any_cast<FspSnapshotFingerprint>(&obj);
		return other != nullptr && Equals(*other);
	}

	int MicroOpScheduler::FspSnapshotFingerprint::GetHashCode() const
	{
		const size_t lowHash = std::hash<unsigned long long>{}(getLow());
		const size_t highHash = std::hash<unsigned long long>{}(getHigh());
		return static_cast<int>(lowHash ^ (highHash + 0x9e3779b9u + (lowHash << 6) + (lowHash >> 2)));
	}

	MicroOpScheduler::LoopPhaseTracker::LoopPhaseTracker(unsigned long long loopPcAddress, int phaseEntryCount)
		: LoopPcAddress(loopPcAddress)
	{
		setPhaseEntryCount(phaseEntryCount);
	}

	unsigned long long MicroOpScheduler::LoopPhaseTracker::getLoopPcAddress() const
	{
		return LoopPcAddress;
	}

	int MicroOpScheduler::LoopPhaseTracker::getPhaseEntryCount() const
	{
		return PhaseEntryCount;
	}

	void MicroOpScheduler::LoopPhaseTracker::setPhaseEntryCount(int value)
	{
		PhaseEntryCount = value;
	}

	int MicroOpScheduler::LoopPhaseTracker::getIterationsSampled() const
	{
		return _sampleCount;
	}

	void MicroOpScheduler::LoopPhaseTracker::SetPhaseEntryCount(int phaseEntryCount)
	{
		setPhaseEntryCount(phaseEntryCount);
	}

	void MicroOpScheduler::LoopPhaseTracker::AddSample(ClassCapacityTemplate template_Keyword)
	{
		_samples[_nextSampleIndex] = template_Keyword;
		if (_sampleCount < MicroOpScheduler::LoopPhaseSampleCapacity)
		{
			_sampleCount++;
		}

		_nextSampleIndex = (_nextSampleIndex + 1) % MicroOpScheduler::LoopPhaseSampleCapacity;
	}

	std::shared_ptr<LoopPhaseClassProfile> MicroOpScheduler::LoopPhaseTracker::BuildProfile()
	{
		if (_sampleCount <= 0)
		{
			return std::make_shared<LoopPhaseClassProfile>(getLoopPcAddress(), 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
		}

		double aluMean = 0.0;
		double lsuMean = 0.0;
		double dmaMean = 0.0;
		double branchMean = 0.0;
		double systemMean = 0.0;
		int templateReuseMatches = 0;
		std::optional<ClassCapacityTemplate> previous;

		for (int sampleIndex = 0; sampleIndex < _sampleCount; sampleIndex++)
		{
			ClassCapacityTemplate sample = GetOrderedSample(sampleIndex);
			aluMean += sample.getAluFree();
			lsuMean += sample.getLsuFree();
			dmaMean += sample.getDmaStreamFree();
			branchMean += sample.getBranchControlFree();
			systemMean += sample.getSystemSingletonFree();

			if (previous.has_value() && previous->Equals(sample))
			{
				templateReuseMatches++;
			}

			previous = sample;
		}

		double sampleCount = _sampleCount;
		aluMean /= sampleCount;
		lsuMean /= sampleCount;
		dmaMean /= sampleCount;
		branchMean /= sampleCount;
		systemMean /= sampleCount;

		double aluVariance = 0.0;
		double lsuVariance = 0.0;
		double dmaVariance = 0.0;
		double branchVariance = 0.0;
		double systemVariance = 0.0;

		for (int sampleIndex = 0; sampleIndex < _sampleCount; sampleIndex++)
		{
			ClassCapacityTemplate sample = GetOrderedSample(sampleIndex);
			aluVariance += Square(sample.getAluFree() - aluMean);
			lsuVariance += Square(sample.getLsuFree() - lsuMean);
			dmaVariance += Square(sample.getDmaStreamFree() - dmaMean);
			branchVariance += Square(sample.getBranchControlFree() - branchMean);
			systemVariance += Square(sample.getSystemSingletonFree() - systemMean);
		}

		aluVariance /= sampleCount;
		lsuVariance /= sampleCount;
		dmaVariance /= sampleCount;
		branchVariance /= sampleCount;
		systemVariance /= sampleCount;

		double overallVariance = (aluVariance + lsuVariance + dmaVariance + branchVariance + systemVariance) / 5.0;
		double templateReuseRate = _sampleCount > 1 ? static_cast<double>(templateReuseMatches) / (_sampleCount - 1) : 0.0;

		return std::make_shared<LoopPhaseClassProfile>(getLoopPcAddress(), _sampleCount, aluVariance, lsuVariance, dmaVariance, branchVariance, systemVariance, overallVariance, templateReuseRate);
	}

	ClassCapacityTemplate MicroOpScheduler::LoopPhaseTracker::GetOrderedSample(int sampleIndex)
	{
		int oldestIndex = _sampleCount == MicroOpScheduler::LoopPhaseSampleCapacity ? _nextSampleIndex : 0;
		int actualIndex = (oldestIndex + sampleIndex) % MicroOpScheduler::LoopPhaseSampleCapacity;
		return _samples[actualIndex];
	}

	double MicroOpScheduler::LoopPhaseTracker::Square(double value)
	{
		return value * value;
	}

	MicroOpScheduler::BundleOpportunityState::BundleOpportunityState(unsigned char occupancyMask, unsigned char emptySlotMask, unsigned char referenceOpportunityMask)
	{
		OccupancyMask = occupancyMask;
		EmptySlotMask = emptySlotMask;
		ReferenceOpportunityMask = referenceOpportunityMask;
	}

	unsigned char MicroOpScheduler::BundleOpportunityState::getOccupancyMask() const
	{
		return OccupancyMask;
	}

	unsigned char MicroOpScheduler::BundleOpportunityState::getEmptySlotMask() const
	{
		return EmptySlotMask;
	}

	unsigned char MicroOpScheduler::BundleOpportunityState::getReferenceOpportunityMask() const
	{
		return ReferenceOpportunityMask;
	}

	int MicroOpScheduler::BundleOpportunityState::getReferenceOpportunityCount() const
	{
		return std::popcount(static_cast<unsigned int>(getReferenceOpportunityMask()));
	}

	inline int MicroOpScheduler::BundleOpportunityState::CountReplayEligibleReferenceOpportunities(unsigned char replayAwareMask) const
	{
		return std::popcount(static_cast<unsigned int>(getReferenceOpportunityMask() & replayAwareMask));
	}

	inline bool MicroOpScheduler::BundleOpportunityState::IsReplayEligibleReferenceOpportunity(int slotIndex, unsigned char replayAwareMask) const
	{
		if (static_cast<unsigned int>(slotIndex) >= 8)
		{
			return false;
		}

		unsigned char slotBit = static_cast<unsigned char>(1 << slotIndex);
		return (getReferenceOpportunityMask() & slotBit) != 0 && (replayAwareMask & slotBit) != 0;
	}

	inline int MicroOpScheduler::BundleOpportunityState::FindNextInjectableEmptySlot(int startIndex, bool hasStableDonorStructure, unsigned char stableDonorMask) const
	{
		unsigned char eligibleMask = hasStableDonorStructure ? static_cast<unsigned char>(getEmptySlotMask() & stableDonorMask) : getEmptySlotMask();
		return FindNextSlotInMask(eligibleMask, startIndex);
	}

	inline MicroOpScheduler::BundleOpportunityState MicroOpScheduler::BundleOpportunityState::WithOccupiedSlot(int slotIndex) const
	{
		if (static_cast<unsigned int>(slotIndex) >= 8)
		{
			return *this;
		}

		unsigned char slotBit = static_cast<unsigned char>(1 << slotIndex);
		return BundleOpportunityState(static_cast<unsigned char>(getOccupancyMask() | slotBit), static_cast<unsigned char>(getEmptySlotMask() & ~slotBit), static_cast<unsigned char>(getReferenceOpportunityMask() & ~slotBit));
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static BundleOpportunityState Create(IReadOnlyList<MicroOp?> bundle)
	inline MicroOpScheduler::BundleOpportunityState MicroOpScheduler::BundleOpportunityState::Create(const std::vector<std::shared_ptr<MicroOp>> &bundle)
	{
		unsigned char occupancyMask = 0;
		unsigned char emptySlotMask = 0;
		unsigned char referenceOpportunityMask = 0;

		const size_t slotCount = std::min(bundle.size(), size_t{8});
		for (int slotIndex = 0; slotIndex < 8; slotIndex++)
		{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MicroOp? slotOp = bundle != null && slotIndex < slotCount ? bundle[slotIndex] : null;
			std::shared_ptr<MicroOp> slotOp = !bundle.empty() && slotIndex < slotCount ? bundle[slotIndex] : nullptr;
			unsigned char slotBit = static_cast<unsigned char>(1 << slotIndex);
			if (slotOp == nullptr)
			{
				emptySlotMask |= slotBit;
				referenceOpportunityMask |= slotBit;
				continue;
			}

			occupancyMask |= slotBit;
			if (std::dynamic_pointer_cast<NopMicroOp>(slotOp) != nullptr)
			{
				if (slotOp->getAdmissionMetadata()->IsStealable)
				{
					referenceOpportunityMask |= slotBit;
				}
			}
		}

		return BundleOpportunityState(occupancyMask, emptySlotMask, referenceOpportunityMask);
	}

	inline int MicroOpScheduler::BundleOpportunityState::FindNextSlotInMask(unsigned char slotMask, int startIndex)
	{
		for (int slotIndex = std::max(startIndex, 0); slotIndex < 8; slotIndex++)
		{
			if ((slotMask & (1 << slotIndex)) != 0)
			{
				return slotIndex;
			}
		}

		return -1;
	}

	MicroOpScheduler::SmtNominationState::SmtNominationState(unsigned char readyMask) : ReadyMask(readyMask) {}

	unsigned char MicroOpScheduler::SmtNominationState::getReadyMask() const
	{
		return ReadyMask;
	}

	bool MicroOpScheduler::SmtNominationState::getHasReadyBackgroundCandidates() const
	{
		return (getReadyMask() & 0b1110) != 0;
	}

	inline bool MicroOpScheduler::SmtNominationState::IsReadyCandidate(int virtualThreadId) const
	{
		if (static_cast<unsigned int>(virtualThreadId) >= SMT_WAYS)
		{
			return false;
		}

		return (getReadyMask() & (1 << virtualThreadId)) != 0;
	}

	inline bool MicroOpScheduler::SmtNominationState::IsReadyNonOwnerCandidate(int virtualThreadId, int ownerVirtualThreadId) const
	{
		return virtualThreadId != ownerVirtualThreadId && IsReadyCandidate(virtualThreadId);
	}

	inline int MicroOpScheduler::SmtNominationState::CountReadyCandidatesExcluding(int ownerVirtualThreadId) const
	{
		unsigned char eligibleMask = getReadyMask();
		if (static_cast<unsigned int>(ownerVirtualThreadId) < SMT_WAYS)
		{
			eligibleMask = static_cast<unsigned char>(eligibleMask & ~(1 << ownerVirtualThreadId));
		}

		return std::popcount(static_cast<unsigned int>(eligibleMask));
	}

	inline MicroOpScheduler::SmtNominationState MicroOpScheduler::SmtNominationState::WithoutCandidate(int virtualThreadId) const
	{
		if (static_cast<unsigned int>(virtualThreadId) >= SMT_WAYS)
		{
			return *this;
		}

		return SmtNominationState(static_cast<unsigned char>(getReadyMask() & ~(1 << virtualThreadId)));
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static SmtNominationState Create(MicroOp?[] ports, bool[] validPorts)
	inline MicroOpScheduler::SmtNominationState MicroOpScheduler::SmtNominationState::Create(std::vector<std::shared_ptr<MicroOp>> &ports, std::vector<bool> &validPorts)
	{
		unsigned char readyMask = 0;
		for (int vt = 0; vt < SMT_WAYS; vt++)
		{
			if (validPorts[vt] && ports[vt] != nullptr)
			{
				readyMask |= static_cast<unsigned char>(1 << vt);
			}
		}

		return SmtNominationState(readyMask);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public InterCoreNominationSnapshot(ushort readyMask, MicroOp?[] candidates)
	MicroOpScheduler::InterCoreNominationSnapshot::InterCoreNominationSnapshot(unsigned short readyMask, std::vector<std::shared_ptr<MicroOp>> &candidates)
		: ReadyMask(readyMask), _candidates(candidates)
	{
	}

	unsigned short MicroOpScheduler::InterCoreNominationSnapshot::getReadyMask() const
	{
		return ReadyMask;
	}

	inline bool MicroOpScheduler::InterCoreNominationSnapshot::IsReadyCandidate(int coreId) const
	{
		if (static_cast<unsigned int>(coreId) >= NUM_PORTS)
		{
			return false;
		}

		return ((getReadyMask() >> coreId) & 1) != 0;
	}

	inline bool MicroOpScheduler::InterCoreNominationSnapshot::TryGetCandidate(int coreId, std::shared_ptr<MicroOp> &candidate) const
	{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: candidate = null!;
		candidate = nullptr;
		if (static_cast<unsigned int>(coreId) >= _candidates.size() || !IsReadyCandidate(coreId))
		{
			return false;
		}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MicroOp? nominatedCandidate = candidates[coreId];
		std::shared_ptr<MicroOp> nominatedCandidate = _candidates[coreId];
		if (nominatedCandidate == nullptr)
		{
			return false;
		}

		candidate = nominatedCandidate;
		return true;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static InterCoreNominationSnapshot Capture(MicroOp?[] ports, bool[] validPorts)
	MicroOpScheduler::InterCoreNominationSnapshot MicroOpScheduler::InterCoreNominationSnapshot::Capture(std::vector<std::shared_ptr<MicroOp>> &ports, std::vector<bool> &validPorts)
	{
		unsigned short readyMask = 0;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MicroOp?[] candidates = new MicroOp?[NUM_PORTS];
		std::vector<std::shared_ptr<MicroOp>> candidates(NUM_PORTS);
		for (int coreId = 0; coreId < NUM_PORTS; coreId++)
		{
			std::shared_ptr<MicroOp> candidate = std::dynamic_pointer_cast<MicroOp>(ports[coreId]);
			if (validPorts[coreId] && candidate != nullptr)
			{
				readyMask |= static_cast<unsigned short>(1 << coreId);
				candidates[coreId] = candidate;
			}
		}

		return InterCoreNominationSnapshot(readyMask, candidates);
	}

	MicroOpScheduler::InterCoreNominationSnapshot MicroOpScheduler::InterCoreNominationSnapshot::Capture(std::vector<std::deque<std::shared_ptr<MicroOp>>> &nominationQueues)
	{
		unsigned short readyMask = 0;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MicroOp?[] candidates = new MicroOp?[NUM_PORTS];
		std::vector<std::shared_ptr<MicroOp>> candidates(NUM_PORTS);
		const size_t visibleQueueCount = std::min(nominationQueues.size(), static_cast<size_t>(std::min(SMT_WAYS, NUM_PORTS)));
		for (int queueId = 0; queueId < visibleQueueCount; queueId++)
		{
			std::deque<std::shared_ptr<MicroOp>> queue = nominationQueues[queueId];
			if (queue.empty())
			{
				continue;
			}

			readyMask |= static_cast<unsigned short>(1 << queueId);
			candidates[queueId] = queue.front();
		}

		return InterCoreNominationSnapshot(readyMask, candidates);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SmtNominationSnapshot(SmtNominationState nominationState, MicroOp?[] candidates)
	MicroOpScheduler::SmtNominationSnapshot::SmtNominationSnapshot(SmtNominationState nominationState, std::vector<std::shared_ptr<MicroOp>> &candidates)
		: NominationState(nominationState), _candidates(candidates)
	{
	}

	MicroOpScheduler::SmtNominationState MicroOpScheduler::SmtNominationSnapshot::getNominationState() const
	{
		return NominationState;
	}

	inline bool MicroOpScheduler::SmtNominationSnapshot::TryGetCandidate(int virtualThreadId, std::shared_ptr<MicroOp> &candidate) const
	{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: candidate = null!;
		candidate = nullptr;
		if (static_cast<unsigned int>(virtualThreadId) >= _candidates.size() || !getNominationState().IsReadyCandidate(virtualThreadId))
		{
			return false;
		}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MicroOp? nominatedCandidate = candidates[virtualThreadId];
		std::shared_ptr<MicroOp> nominatedCandidate = _candidates[virtualThreadId];
		if (nominatedCandidate == nullptr)
		{
			return false;
		}

		candidate = nominatedCandidate;
		return true;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static SmtNominationSnapshot Capture(MicroOp?[] ports, SmtNominationState nominationState)
	MicroOpScheduler::SmtNominationSnapshot MicroOpScheduler::SmtNominationSnapshot::Capture(std::vector<std::shared_ptr<MicroOp>> &ports, SmtNominationState nominationState)
	{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MicroOp?[] candidates = new MicroOp?[SMT_WAYS];
		std::vector<std::shared_ptr<MicroOp>> candidates(SMT_WAYS);
		for (int vt = 0; vt < SMT_WAYS; vt++)
		{
			if (nominationState.IsReadyCandidate(vt))
			{
				candidates[vt] = ports[vt];
			}
		}

		return SmtNominationSnapshot(nominationState, candidates);
	}

	MicroOpScheduler::ClassTemplateAdmissionState::ClassTemplateAdmissionState(bool templateHit, TemplateBudget templateBudget)
		: TemplateHit(templateHit), _templateBudget(templateBudget)
	{
	}

	bool MicroOpScheduler::ClassTemplateAdmissionState::getTemplateHit() const
	{
		return TemplateHit;
	}

	TemplateBudget MicroOpScheduler::ClassTemplateAdmissionState::getTemplateBudgetSnapshot() const
	{
		return _templateBudget;
	}

	MicroOpScheduler::ClassTemplateAdmissionState MicroOpScheduler::ClassTemplateAdmissionState::Create(TemplateBudget templateBudget)
	{
		return ClassTemplateAdmissionState(true, templateBudget);
	}

	inline bool MicroOpScheduler::ClassTemplateAdmissionState::TryConsumeFastPathBudget(SlotClass slotClass)
	{
		if (!getTemplateHit())
		{
			return false;
		}

		if (_templateBudget.GetRemaining(slotClass) <= 0)
		{
			return false;
		}

		_templateBudget.Decrement(slotClass);
		return true;
	}

	long long MicroOpScheduler::getFspPipelineLatencyCycles() const
	{
		return FspPipelineLatencyCycles;
	}

	void MicroOpScheduler::setFspPipelineLatencyCycles(long long value)
	{
		FspPipelineLatencyCycles = value;
	}

	long long MicroOpScheduler::getTotalSchedulerCycles() const
	{
		return TotalSchedulerCycles;
	}

	void MicroOpScheduler::setTotalSchedulerCycles(long long value)
	{
		TotalSchedulerCycles = value;
	}

	long long MicroOpScheduler::getSuccessfulInjectionsCount() const
	{
		return SuccessfulInjectionsCount;
	}

	void MicroOpScheduler::setSuccessfulInjectionsCount(long long value)
	{
		SuccessfulInjectionsCount = value;
	}

	long long MicroOpScheduler::getSmtInjectionsCount() const
	{
		return SmtInjectionsCount;
	}

	void MicroOpScheduler::setSmtInjectionsCount(long long value)
	{
		SmtInjectionsCount = value;
	}

	long long MicroOpScheduler::getSmtRejectionsCount() const
	{
		return SmtRejectionsCount;
	}

	void MicroOpScheduler::setSmtRejectionsCount(long long value)
	{
		SmtRejectionsCount = value;
	}

	ReplayPhaseContext MicroOpScheduler::getCurrentReplayPhase() const
	{
		return _currentReplayPhase;
	}

	long long MicroOpScheduler::getReplayAwareCycles() const
	{
		return ReplayAwareCycles;
	}

	void MicroOpScheduler::setReplayAwareCycles(long long value)
	{
		ReplayAwareCycles = value;
	}

	long long MicroOpScheduler::getPhaseCertificateReadyHits() const
	{
		return PhaseCertificateReadyHits;
	}

	void MicroOpScheduler::setPhaseCertificateReadyHits(long long value)
	{
		PhaseCertificateReadyHits = value;
	}

	long long MicroOpScheduler::getPhaseCertificateReadyMisses() const
	{
		return PhaseCertificateReadyMisses;
	}

	void MicroOpScheduler::setPhaseCertificateReadyMisses(long long value)
	{
		PhaseCertificateReadyMisses = value;
	}

	long long MicroOpScheduler::getEstimatedPhaseCertificateChecksSaved() const
	{
		return EstimatedPhaseCertificateChecksSaved;
	}

	void MicroOpScheduler::setEstimatedPhaseCertificateChecksSaved(long long value)
	{
		EstimatedPhaseCertificateChecksSaved = value;
	}

	long long MicroOpScheduler::getPhaseCertificateInvalidations() const
	{
		return PhaseCertificateInvalidations;
	}

	void MicroOpScheduler::setPhaseCertificateInvalidations(long long value)
	{
		PhaseCertificateInvalidations = value;
	}

	long long MicroOpScheduler::getPhaseCertificateMutationInvalidations() const
	{
		return PhaseCertificateMutationInvalidations;
	}

	void MicroOpScheduler::setPhaseCertificateMutationInvalidations(long long value)
	{
		PhaseCertificateMutationInvalidations = value;
	}

	long long MicroOpScheduler::getPhaseCertificatePhaseMismatchInvalidations() const
	{
		return PhaseCertificatePhaseMismatchInvalidations;
	}

	void MicroOpScheduler::setPhaseCertificatePhaseMismatchInvalidations(long long value)
	{
		PhaseCertificatePhaseMismatchInvalidations = value;
	}

	ReplayPhaseInvalidationReason MicroOpScheduler::getLastPhaseCertificateInvalidationReason() const
	{
		return LastPhaseCertificateInvalidationReason;
	}

	void MicroOpScheduler::setLastPhaseCertificateInvalidationReason(ReplayPhaseInvalidationReason value)
	{
		LastPhaseCertificateInvalidationReason = value;
	}

	long long MicroOpScheduler::getDeterminismReferenceOpportunitySlots() const
	{
		return DeterminismReferenceOpportunitySlots;
	}

	void MicroOpScheduler::setDeterminismReferenceOpportunitySlots(long long value)
	{
		DeterminismReferenceOpportunitySlots = value;
	}

	long long MicroOpScheduler::getDeterminismReplayEligibleSlots() const
	{
		return DeterminismReplayEligibleSlots;
	}

	void MicroOpScheduler::setDeterminismReplayEligibleSlots(long long value)
	{
		DeterminismReplayEligibleSlots = value;
	}

	long long MicroOpScheduler::getDeterminismMaskedSlots() const
	{
		return DeterminismMaskedSlots;
	}

	void MicroOpScheduler::setDeterminismMaskedSlots(long long value)
	{
		DeterminismMaskedSlots = value;
	}

	long long MicroOpScheduler::getDeterminismEstimatedLostSlots() const
	{
		return DeterminismEstimatedLostSlots;
	}

	void MicroOpScheduler::setDeterminismEstimatedLostSlots(long long value)
	{
		DeterminismEstimatedLostSlots = value;
	}

	long long MicroOpScheduler::getDeterminismConstrainedCycles() const
	{
		return DeterminismConstrainedCycles;
	}

	void MicroOpScheduler::setDeterminismConstrainedCycles(long long value)
	{
		DeterminismConstrainedCycles = value;
	}

	long long MicroOpScheduler::getDomainIsolationProbeAttempts() const
	{
		return DomainIsolationProbeAttempts;
	}

	void MicroOpScheduler::setDomainIsolationProbeAttempts(long long value)
	{
		DomainIsolationProbeAttempts = value;
	}

	long long MicroOpScheduler::getDomainIsolationBlockedAttempts() const
	{
		return DomainIsolationBlockedAttempts;
	}

	void MicroOpScheduler::setDomainIsolationBlockedAttempts(long long value)
	{
		DomainIsolationBlockedAttempts = value;
	}

	long long MicroOpScheduler::getDomainIsolationCrossDomainBlocks() const
	{
		return DomainIsolationCrossDomainBlocks;
	}

	void MicroOpScheduler::setDomainIsolationCrossDomainBlocks(long long value)
	{
		DomainIsolationCrossDomainBlocks = value;
	}

	long long MicroOpScheduler::getDomainIsolationKernelToUserBlocks() const
	{
		return DomainIsolationKernelToUserBlocks;
	}

	void MicroOpScheduler::setDomainIsolationKernelToUserBlocks(long long value)
	{
		DomainIsolationKernelToUserBlocks = value;
	}

	long long MicroOpScheduler::getSerializingEpochCount() const
	{
		return _serializingEpochCounter;
	}

	long long MicroOpScheduler::getSerializingBoundaryRejects() const
	{
		return SerializingBoundaryRejects;
	}

	void MicroOpScheduler::setSerializingBoundaryRejects(long long value)
	{
		SerializingBoundaryRejects = value;
	}

	long long MicroOpScheduler::getClassCapacityRejects() const
	{
		return ClassCapacityRejects;
	}

	void MicroOpScheduler::setClassCapacityRejects(long long value)
	{
		ClassCapacityRejects = value;
	}

	long long MicroOpScheduler::getLaneConflictRejects() const
	{
		return LaneConflictRejects;
	}

	void MicroOpScheduler::setLaneConflictRejects(long long value)
	{
		LaneConflictRejects = value;
	}

	long long MicroOpScheduler::getTypedSlotResourceConflictRejects() const
	{
		return TypedSlotResourceConflictRejects;
	}

	void MicroOpScheduler::setTypedSlotResourceConflictRejects(long long value)
	{
		TypedSlotResourceConflictRejects = value;
	}

	long long MicroOpScheduler::getSmtOwnerContextGuardRejects() const
	{
		return SmtOwnerContextGuardRejects;
	}

	void MicroOpScheduler::setSmtOwnerContextGuardRejects(long long value)
	{
		SmtOwnerContextGuardRejects = value;
	}

	long long MicroOpScheduler::getSmtDomainGuardRejects() const
	{
		return SmtDomainGuardRejects;
	}

	void MicroOpScheduler::setSmtDomainGuardRejects(long long value)
	{
		SmtDomainGuardRejects = value;
	}

	long long MicroOpScheduler::getSmtBoundaryGuardRejects() const
	{
		return SmtBoundaryGuardRejects;
	}

	void MicroOpScheduler::setSmtBoundaryGuardRejects(long long value)
	{
		SmtBoundaryGuardRejects = value;
	}

	long long MicroOpScheduler::getSmtSharedResourceCertificateRejects() const
	{
		return SmtSharedResourceCertificateRejects;
	}

	void MicroOpScheduler::setSmtSharedResourceCertificateRejects(long long value)
	{
		SmtSharedResourceCertificateRejects = value;
	}

	long long MicroOpScheduler::getSmtRegisterGroupCertificateRejects() const
	{
		return SmtRegisterGroupCertificateRejects;
	}

	void MicroOpScheduler::setSmtRegisterGroupCertificateRejects(long long value)
	{
		SmtRegisterGroupCertificateRejects = value;
	}

	RejectKind MicroOpScheduler::getLastSmtLegalityRejectKind() const
	{
		return _lastSmtLegalityRejectKind;
	}

	LegalityAuthoritySource MicroOpScheduler::getLastSmtLegalityAuthoritySource() const
	{
		return _lastSmtLegalityAuthoritySource;
	}

	long long MicroOpScheduler::getSmtLegalityRejectByAluClass() const
	{
		return SmtLegalityRejectByAluClass;
	}

	void MicroOpScheduler::setSmtLegalityRejectByAluClass(long long value)
	{
		SmtLegalityRejectByAluClass = value;
	}

	long long MicroOpScheduler::getSmtLegalityRejectByLsuClass() const
	{
		return SmtLegalityRejectByLsuClass;
	}

	void MicroOpScheduler::setSmtLegalityRejectByLsuClass(long long value)
	{
		SmtLegalityRejectByLsuClass = value;
	}

	long long MicroOpScheduler::getSmtLegalityRejectByDmaStreamClass() const
	{
		return SmtLegalityRejectByDmaStreamClass;
	}

	void MicroOpScheduler::setSmtLegalityRejectByDmaStreamClass(long long value)
	{
		SmtLegalityRejectByDmaStreamClass = value;
	}

	long long MicroOpScheduler::getSmtLegalityRejectByMatrixTileStreamClass() const
	{
		return SmtLegalityRejectByMatrixTileStreamClass;
	}

	void MicroOpScheduler::setSmtLegalityRejectByMatrixTileStreamClass(long long value)
	{
		SmtLegalityRejectByMatrixTileStreamClass = value;
	}

	long long MicroOpScheduler::getSmtLegalityRejectByBranchControl() const
	{
		return SmtLegalityRejectByBranchControl;
	}

	void MicroOpScheduler::setSmtLegalityRejectByBranchControl(long long value)
	{
		SmtLegalityRejectByBranchControl = value;
	}

	long long MicroOpScheduler::getSmtLegalityRejectBySystemSingleton() const
	{
		return SmtLegalityRejectBySystemSingleton;
	}

	void MicroOpScheduler::setSmtLegalityRejectBySystemSingleton(long long value)
	{
		SmtLegalityRejectBySystemSingleton = value;
	}

	long long MicroOpScheduler::getTypedSlotScoreboardRejects() const
	{
		return TypedSlotScoreboardRejects;
	}

	void MicroOpScheduler::setTypedSlotScoreboardRejects(long long value)
	{
		TypedSlotScoreboardRejects = value;
	}

	long long MicroOpScheduler::getTypedSlotBankPendingRejects() const
	{
		return TypedSlotBankPendingRejects;
	}

	void MicroOpScheduler::setTypedSlotBankPendingRejects(long long value)
	{
		TypedSlotBankPendingRejects = value;
	}

	long long MicroOpScheduler::getBankPendingRejectBank0() const
	{
		return BankPendingRejectBank0;
	}

	void MicroOpScheduler::setBankPendingRejectBank0(long long value)
	{
		BankPendingRejectBank0 = value;
	}

	long long MicroOpScheduler::getBankPendingRejectBank1() const
	{
		return BankPendingRejectBank1;
	}

	void MicroOpScheduler::setBankPendingRejectBank1(long long value)
	{
		BankPendingRejectBank1 = value;
	}

	long long MicroOpScheduler::getBankPendingRejectBank2() const
	{
		return BankPendingRejectBank2;
	}

	void MicroOpScheduler::setBankPendingRejectBank2(long long value)
	{
		BankPendingRejectBank2 = value;
	}

	long long MicroOpScheduler::getBankPendingRejectBank3() const
	{
		return BankPendingRejectBank3;
	}

	void MicroOpScheduler::setBankPendingRejectBank3(long long value)
	{
		BankPendingRejectBank3 = value;
	}

	long long MicroOpScheduler::getBankPendingRejectBank4() const
	{
		return BankPendingRejectBank4;
	}

	void MicroOpScheduler::setBankPendingRejectBank4(long long value)
	{
		BankPendingRejectBank4 = value;
	}

	long long MicroOpScheduler::getBankPendingRejectBank5() const
	{
		return BankPendingRejectBank5;
	}

	void MicroOpScheduler::setBankPendingRejectBank5(long long value)
	{
		BankPendingRejectBank5 = value;
	}

	long long MicroOpScheduler::getBankPendingRejectBank6() const
	{
		return BankPendingRejectBank6;
	}

	void MicroOpScheduler::setBankPendingRejectBank6(long long value)
	{
		BankPendingRejectBank6 = value;
	}

	long long MicroOpScheduler::getBankPendingRejectBank7() const
	{
		return BankPendingRejectBank7;
	}

	void MicroOpScheduler::setBankPendingRejectBank7(long long value)
	{
		BankPendingRejectBank7 = value;
	}

	long long MicroOpScheduler::getBankPendingRejectBank8() const
	{
		return BankPendingRejectBank8;
	}

	void MicroOpScheduler::setBankPendingRejectBank8(long long value)
	{
		BankPendingRejectBank8 = value;
	}

	long long MicroOpScheduler::getBankPendingRejectBank9() const
	{
		return BankPendingRejectBank9;
	}

	void MicroOpScheduler::setBankPendingRejectBank9(long long value)
	{
		BankPendingRejectBank9 = value;
	}

	long long MicroOpScheduler::getBankPendingRejectBank10() const
	{
		return BankPendingRejectBank10;
	}

	void MicroOpScheduler::setBankPendingRejectBank10(long long value)
	{
		BankPendingRejectBank10 = value;
	}

	long long MicroOpScheduler::getBankPendingRejectBank11() const
	{
		return BankPendingRejectBank11;
	}

	void MicroOpScheduler::setBankPendingRejectBank11(long long value)
	{
		BankPendingRejectBank11 = value;
	}

	long long MicroOpScheduler::getBankPendingRejectBank12() const
	{
		return BankPendingRejectBank12;
	}

	void MicroOpScheduler::setBankPendingRejectBank12(long long value)
	{
		BankPendingRejectBank12 = value;
	}

	long long MicroOpScheduler::getBankPendingRejectBank13() const
	{
		return BankPendingRejectBank13;
	}

	void MicroOpScheduler::setBankPendingRejectBank13(long long value)
	{
		BankPendingRejectBank13 = value;
	}

	long long MicroOpScheduler::getBankPendingRejectBank14() const
	{
		return BankPendingRejectBank14;
	}

	void MicroOpScheduler::setBankPendingRejectBank14(long long value)
	{
		BankPendingRejectBank14 = value;
	}

	long long MicroOpScheduler::getBankPendingRejectBank15() const
	{
		return BankPendingRejectBank15;
	}

	void MicroOpScheduler::setBankPendingRejectBank15(long long value)
	{
		BankPendingRejectBank15 = value;
	}

	long long MicroOpScheduler::getMemoryClusteringEvents() const
	{
		return MemoryClusteringEvents;
	}

	void MicroOpScheduler::setMemoryClusteringEvents(long long value)
	{
		MemoryClusteringEvents = value;
	}

	long long MicroOpScheduler::getTypedSlotSpeculationBudgetRejects() const
	{
		return TypedSlotSpeculationBudgetRejects;
	}

	void MicroOpScheduler::setTypedSlotSpeculationBudgetRejects(long long value)
	{
		TypedSlotSpeculationBudgetRejects = value;
	}

	long long MicroOpScheduler::getTypedSlotHardwareBudgetRejects() const
	{
		return TypedSlotHardwareBudgetRejects;
	}

	void MicroOpScheduler::setTypedSlotHardwareBudgetRejects(long long value)
	{
		TypedSlotHardwareBudgetRejects = value;
	}

	long long MicroOpScheduler::getTypedSlotAssistQuotaRejects() const
	{
		return TypedSlotAssistQuotaRejects;
	}

	void MicroOpScheduler::setTypedSlotAssistQuotaRejects(long long value)
	{
		TypedSlotAssistQuotaRejects = value;
	}

	long long MicroOpScheduler::getTypedSlotAssistBackpressureRejects() const
	{
		return TypedSlotAssistBackpressureRejects;
	}

	void MicroOpScheduler::setTypedSlotAssistBackpressureRejects(long long value)
	{
		TypedSlotAssistBackpressureRejects = value;
	}

	long long MicroOpScheduler::getLaneReuseHits() const
	{
		return LaneReuseHits;
	}

	void MicroOpScheduler::setLaneReuseHits(long long value)
	{
		LaneReuseHits = value;
	}

	long long MicroOpScheduler::getLaneReuseMisses() const
	{
		return LaneReuseMisses;
	}

	void MicroOpScheduler::setLaneReuseMisses(long long value)
	{
		LaneReuseMisses = value;
	}

	long long MicroOpScheduler::getClassTemplateReuseHits() const
	{
		return ClassTemplateReuseHits;
	}

	void MicroOpScheduler::setClassTemplateReuseHits(long long value)
	{
		ClassTemplateReuseHits = value;
	}

	long long MicroOpScheduler::getClassTemplateInvalidations() const
	{
		return ClassTemplateInvalidations;
	}

	void MicroOpScheduler::setClassTemplateInvalidations(long long value)
	{
		ClassTemplateInvalidations = value;
	}

	long long MicroOpScheduler::getTypedSlotFastPathAccepts() const
	{
		return TypedSlotFastPathAccepts;
	}

	void MicroOpScheduler::setTypedSlotFastPathAccepts(long long value)
	{
		TypedSlotFastPathAccepts = value;
	}

	long long MicroOpScheduler::getStaticClassOvercommitRejects() const
	{
		return StaticClassOvercommitRejects;
	}

	void MicroOpScheduler::setStaticClassOvercommitRejects(long long value)
	{
		StaticClassOvercommitRejects = value;
	}

	long long MicroOpScheduler::getDynamicClassExhaustionRejects() const
	{
		return DynamicClassExhaustionRejects;
	}

	void MicroOpScheduler::setDynamicClassExhaustionRejects(long long value)
	{
		DynamicClassExhaustionRejects = value;
	}

	long long MicroOpScheduler::getPinnedLaneConflicts() const
	{
		return PinnedLaneConflicts;
	}

	void MicroOpScheduler::setPinnedLaneConflicts(long long value)
	{
		PinnedLaneConflicts = value;
	}

	long long MicroOpScheduler::getInvalidPinnedLaneRejects() const
	{
		return InvalidPinnedLaneRejects;
	}

	void MicroOpScheduler::setInvalidPinnedLaneRejects(long long value)
	{
		InvalidPinnedLaneRejects = value;
	}

	long long MicroOpScheduler::getLateBindingConflicts() const
	{
		return LateBindingConflicts;
	}

	void MicroOpScheduler::setLateBindingConflicts(long long value)
	{
		LateBindingConflicts = value;
	}

	long long MicroOpScheduler::getTypedSlotDomainRejects() const
	{
		return TypedSlotDomainRejects;
	}

	void MicroOpScheduler::setTypedSlotDomainRejects(long long value)
	{
		TypedSlotDomainRejects = value;
	}

	long long MicroOpScheduler::getTypedSlotStandardPathAccepts() const
	{
		return TypedSlotStandardPathAccepts;
	}

	void MicroOpScheduler::setTypedSlotStandardPathAccepts(long long value)
	{
		TypedSlotStandardPathAccepts = value;
	}

	long long MicroOpScheduler::getTotalLaneBindings() const
	{
		return TotalLaneBindings;
	}

	void MicroOpScheduler::setTotalLaneBindings(long long value)
	{
		TotalLaneBindings = value;
	}

	long long MicroOpScheduler::getClassTemplateDomainInvalidations() const
	{
		return ClassTemplateDomainInvalidations;
	}

	void MicroOpScheduler::setClassTemplateDomainInvalidations(long long value)
	{
		ClassTemplateDomainInvalidations = value;
	}

	long long MicroOpScheduler::getClassTemplateCapacityMismatchInvalidations() const
	{
		return ClassTemplateCapacityMismatchInvalidations;
	}

	void MicroOpScheduler::setClassTemplateCapacityMismatchInvalidations(long long value)
	{
		ClassTemplateCapacityMismatchInvalidations = value;
	}

	long long MicroOpScheduler::getNopDueToPinnedConstraint() const
	{
		return NopDueToPinnedConstraint;
	}

	void MicroOpScheduler::setNopDueToPinnedConstraint(long long value)
	{
		NopDueToPinnedConstraint = value;
	}

	long long MicroOpScheduler::getNopDueToNoClassCapacity() const
	{
		return NopDueToNoClassCapacity;
	}

	void MicroOpScheduler::setNopDueToNoClassCapacity(long long value)
	{
		NopDueToNoClassCapacity = value;
	}

	long long MicroOpScheduler::getNopDueToResourceConflict() const
	{
		return NopDueToResourceConflict;
	}

	void MicroOpScheduler::setNopDueToResourceConflict(long long value)
	{
		NopDueToResourceConflict = value;
	}

	long long MicroOpScheduler::getNopDueToDynamicState() const
	{
		return NopDueToDynamicState;
	}

	void MicroOpScheduler::setNopDueToDynamicState(long long value)
	{
		NopDueToDynamicState = value;
	}

	long long MicroOpScheduler::getNopAvoided() const
	{
		return NopAvoided;
	}

	void MicroOpScheduler::setNopAvoided(long long value)
	{
		NopAvoided = value;
	}

	long long MicroOpScheduler::getAluClassInjects() const
	{
		return AluClassInjects;
	}

	void MicroOpScheduler::setAluClassInjects(long long value)
	{
		AluClassInjects = value;
	}

	long long MicroOpScheduler::getLsuClassInjects() const
	{
		return LsuClassInjects;
	}

	void MicroOpScheduler::setLsuClassInjects(long long value)
	{
		LsuClassInjects = value;
	}

	long long MicroOpScheduler::getDmaStreamClassInjects() const
	{
		return DmaStreamClassInjects;
	}

	void MicroOpScheduler::setDmaStreamClassInjects(long long value)
	{
		DmaStreamClassInjects = value;
	}

	long long MicroOpScheduler::getMatrixTileStreamClassInjects() const
	{
		return MatrixTileStreamClassInjects;
	}

	void MicroOpScheduler::setMatrixTileStreamClassInjects(long long value)
	{
		MatrixTileStreamClassInjects = value;
	}

	long long MicroOpScheduler::getBranchControlInjects() const
	{
		return BranchControlInjects;
	}

	void MicroOpScheduler::setBranchControlInjects(long long value)
	{
		BranchControlInjects = value;
	}

	long long MicroOpScheduler::getCertificateRejectByAluClass() const
	{
		return CertificateRejectByAluClass;
	}

	void MicroOpScheduler::setCertificateRejectByAluClass(long long value)
	{
		CertificateRejectByAluClass = value;
	}

	long long MicroOpScheduler::getCertificateRejectByLsuClass() const
	{
		return CertificateRejectByLsuClass;
	}

	void MicroOpScheduler::setCertificateRejectByLsuClass(long long value)
	{
		CertificateRejectByLsuClass = value;
	}

	long long MicroOpScheduler::getCertificateRejectByDmaStreamClass() const
	{
		return CertificateRejectByDmaStreamClass;
	}

	void MicroOpScheduler::setCertificateRejectByDmaStreamClass(long long value)
	{
		CertificateRejectByDmaStreamClass = value;
	}

	long long MicroOpScheduler::getCertificateRejectByMatrixTileStreamClass() const
	{
		return CertificateRejectByMatrixTileStreamClass;
	}

	void MicroOpScheduler::setCertificateRejectByMatrixTileStreamClass(long long value)
	{
		CertificateRejectByMatrixTileStreamClass = value;
	}

	long long MicroOpScheduler::getCertificateRejectByBranchControl() const
	{
		return CertificateRejectByBranchControl;
	}

	void MicroOpScheduler::setCertificateRejectByBranchControl(long long value)
	{
		CertificateRejectByBranchControl = value;
	}

	long long MicroOpScheduler::getCertificateRejectBySystemSingleton() const
	{
		return CertificateRejectBySystemSingleton;
	}

	void MicroOpScheduler::setCertificateRejectBySystemSingleton(long long value)
	{
		CertificateRejectBySystemSingleton = value;
	}

	long long MicroOpScheduler::getCertificateRegGroupConflictVT0() const
	{
		return getRegGroupConflictsVT0();
	}

	long long MicroOpScheduler::getCertificateRegGroupConflictVT1() const
	{
		return getRegGroupConflictsVT1();
	}

	long long MicroOpScheduler::getCertificateRegGroupConflictVT2() const
	{
		return getRegGroupConflictsVT2();
	}

	long long MicroOpScheduler::getCertificateRegGroupConflictVT3() const
	{
		return getRegGroupConflictsVT3();
	}

	long long MicroOpScheduler::getRejectionsVT0() const
	{
		return RejectionsVT0;
	}

	void MicroOpScheduler::setRejectionsVT0(long long value)
	{
		RejectionsVT0 = value;
	}

	long long MicroOpScheduler::getRejectionsVT1() const
	{
		return RejectionsVT1;
	}

	void MicroOpScheduler::setRejectionsVT1(long long value)
	{
		RejectionsVT1 = value;
	}

	long long MicroOpScheduler::getRejectionsVT2() const
	{
		return RejectionsVT2;
	}

	void MicroOpScheduler::setRejectionsVT2(long long value)
	{
		RejectionsVT2 = value;
	}

	long long MicroOpScheduler::getRejectionsVT3() const
	{
		return RejectionsVT3;
	}

	void MicroOpScheduler::setRejectionsVT3(long long value)
	{
		RejectionsVT3 = value;
	}

	long long MicroOpScheduler::getRegGroupConflictsVT0() const
	{
		return RegGroupConflictsVT0;
	}

	void MicroOpScheduler::setRegGroupConflictsVT0(long long value)
	{
		RegGroupConflictsVT0 = value;
	}

	long long MicroOpScheduler::getRegGroupConflictsVT1() const
	{
		return RegGroupConflictsVT1;
	}

	void MicroOpScheduler::setRegGroupConflictsVT1(long long value)
	{
		RegGroupConflictsVT1 = value;
	}

	long long MicroOpScheduler::getRegGroupConflictsVT2() const
	{
		return RegGroupConflictsVT2;
	}

	void MicroOpScheduler::setRegGroupConflictsVT2(long long value)
	{
		RegGroupConflictsVT2 = value;
	}

	long long MicroOpScheduler::getRegGroupConflictsVT3() const
	{
		return RegGroupConflictsVT3;
	}

	void MicroOpScheduler::setRegGroupConflictsVT3(long long value)
	{
		RegGroupConflictsVT3 = value;
	}

	long long MicroOpScheduler::getHardPinnedInjects() const
	{
		return HardPinnedInjects;
	}

	void MicroOpScheduler::setHardPinnedInjects(long long value)
	{
		HardPinnedInjects = value;
	}

	long long MicroOpScheduler::getClassFlexibleInjects() const
	{
		return ClassFlexibleInjects;
	}

	void MicroOpScheduler::setClassFlexibleInjects(long long value)
	{
		ClassFlexibleInjects = value;
	}

	inline int MicroOpScheduler::GetPreviousPhaseLane(SlotClass slotClass)
	{
		unsigned char idx = static_cast<unsigned char>(slotClass);
		return idx < 8 ? _previousPhaseLane[idx] : -1;
	}

	inline void MicroOpScheduler::RecordPhaseLane(SlotClass slotClass, int lane)
	{
		unsigned char idx = static_cast<unsigned char>(slotClass);
		if (idx < 8)
		{
			_previousPhaseLane[idx] = lane;
		}
	}

	inline void MicroOpScheduler::ResetPreviousPhaseLanes()
	{
		for (int i = 0; i < _previousPhaseLane.size(); i++)
		{
			_previousPhaseLane[i] = -1;
		}
	}

	inline void MicroOpScheduler::CaptureClassTemplate(int domainScopeId)
	{
		_classCapacityTemplate = ClassCapacityTemplate(_classCapacity);
		_classTemplateValid = true;
		_classTemplateDomainId = domainScopeId;
	}

	inline int MicroOpScheduler::ResolveClassTemplateDomainScopeId(SmtBundleMetadata4Way bundleMetadata)
	{
		int ownerContextId = bundleMetadata.getHasKnownOwnerContext() ? bundleMetadata.getOwnerContextId() : bundleMetadata.getOwnerVirtualThreadId();

		if (ownerContextId == bundleMetadata.getOwnerVirtualThreadId() && bundleMetadata.getOwnerDomainTag() == 0)
		{
			return ownerContextId;
		}

		HardwareHash domainScopeHasher = HardwareHash();
		domainScopeHasher.Initialize();
		domainScopeHasher.Compress(static_cast<unsigned long long>(static_cast<unsigned int>(bundleMetadata.getOwnerVirtualThreadId())));
		domainScopeHasher.Compress(static_cast<unsigned long long>(static_cast<unsigned int>(ownerContextId)));
		domainScopeHasher.Compress(bundleMetadata.getOwnerDomainTag());
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((int)domainScopeHasher.Finalize());
		return static_cast<int>(domainScopeHasher.Finalize());
	}

	inline void MicroOpScheduler::RecordLoopPhaseEntry(ReplayPhaseContext phase)
	{
		if (!getEnableLoopPhaseSampling() || !phase.isActive())
		{
			return;
		}

		if (phase.getEpochId() == _lastLoopPhaseEntryEpochId && phase.getCachedPc() == _lastLoopPhaseEntryPc)
		{
			return;
		}

		_lastLoopPhaseEntryEpochId = phase.getEpochId();
		_lastLoopPhaseEntryPc = phase.getCachedPc();

		auto entry = _loopPhaseEntryCounts.find(phase.getCachedPc());
		const int phaseEntryCount = entry == _loopPhaseEntryCounts.end() ? 1 : entry->second + 1;

		_loopPhaseEntryCounts[phase.getCachedPc()] = phaseEntryCount;
		PruneLoopPhaseHeat();

		auto tracker = _loopPhaseTrackers.find(phase.getCachedPc());
		if (tracker != _loopPhaseTrackers.end())
		{
			tracker->second->SetPhaseEntryCount(phaseEntryCount);
			return;
		}

		if (phaseEntryCount < LoopPhaseActivationThreshold)
		{
			return;
		}

		PromoteLoopPhaseTracker(phase.getCachedPc(), phaseEntryCount);
	}

	inline void MicroOpScheduler::RecordLoopPhaseSample()
	{
		if (!getEnableLoopPhaseSampling() || !_currentReplayPhase.isActive())
		{
			return;
		}

		if (_currentReplayPhase.getEpochId() == _lastLoopPhaseSampleEpochId && _currentReplayPhase.getCachedPc() == _lastLoopPhaseSamplePc)
		{
			return;
		}

		unsigned long long loopPcAddress = _currentReplayPhase.getCachedPc();
		auto entry = _loopPhaseEntryCounts.find(loopPcAddress);
		if (entry == _loopPhaseEntryCounts.end() || entry->second < LoopPhaseActivationThreshold) return;
		const int phaseEntryCount = entry->second;

		auto trackerEntry = _loopPhaseTrackers.find(loopPcAddress);
		if (trackerEntry == _loopPhaseTrackers.end())
		{
			PromoteLoopPhaseTracker(loopPcAddress, phaseEntryCount);
			trackerEntry = _loopPhaseTrackers.find(loopPcAddress);
			if (trackerEntry == _loopPhaseTrackers.end()) return;
		}
		auto tracker = trackerEntry->second;

		_lastLoopPhaseSampleEpochId = _currentReplayPhase.getEpochId();
		_lastLoopPhaseSamplePc = loopPcAddress;
		tracker->SetPhaseEntryCount(phaseEntryCount);
		tracker->AddSample(ClassCapacityTemplate(_classCapacity));
	}

	void MicroOpScheduler::PromoteLoopPhaseTracker(unsigned long long loopPcAddress, int phaseEntryCount)
	{
		auto existing = _loopPhaseTrackers.find(loopPcAddress);
		if (existing != _loopPhaseTrackers.end())
		{
			existing->second->SetPhaseEntryCount(phaseEntryCount);
			return;
		}

		if (_loopPhaseTrackers.size() < LoopPhaseExportLimit)
		{
			_loopPhaseTrackers[loopPcAddress] = std::make_shared<LoopPhaseTracker>(loopPcAddress, phaseEntryCount);
			return;
		}

		unsigned long long coldestLoopPc = 0;
		int coldestHeat = std::numeric_limits<int>::max();
		for (auto entry : _loopPhaseTrackers)
		{
			int candidateHeat = entry.second->getPhaseEntryCount();
			if (candidateHeat < coldestHeat || (candidateHeat == coldestHeat && entry.first > coldestLoopPc))
			{
				coldestLoopPc = entry.first;
				coldestHeat = candidateHeat;
			}
		}

		if (phaseEntryCount <= coldestHeat)
		{
			return;
		}

		_loopPhaseTrackers.erase(coldestLoopPc);
		_loopPhaseTrackers[loopPcAddress] = std::make_shared<LoopPhaseTracker>(loopPcAddress, phaseEntryCount);
	}

	void MicroOpScheduler::PruneLoopPhaseHeat()
{
    if (_loopPhaseEntryCounts.size() <= LoopPhaseHeatPruneThreshold) return;
    std::vector<std::pair<unsigned long long, int>> entries(_loopPhaseEntryCounts.begin(), _loopPhaseEntryCounts.end());
    std::sort(entries.begin(), entries.end(), [](const auto &left, const auto &right) {
        return left.second != right.second ? left.second > right.second : left.first < right.first;
    });
    _loopPhaseEntryCounts.clear();
    const size_t keepCount = std::min(static_cast<size_t>(LoopPhaseExportLimit * 2), entries.size());
    for (size_t index = 0; index < keepCount; ++index) _loopPhaseEntryCounts[entries[index].first] = entries[index].second;
    for (const auto &entry : _loopPhaseTrackers)
        if (!_loopPhaseEntryCounts.contains(entry.first)) _loopPhaseEntryCounts[entry.first] = entry.second->getPhaseEntryCount();
}

const std::vector<std::shared_ptr<LoopPhaseClassProfile>> MicroOpScheduler::BuildLoopPhaseProfiles()
{
    if (!getEnableLoopPhaseSampling() || _loopPhaseTrackers.empty()) return {};
    struct ExportCandidate { int Heat; std::shared_ptr<LoopPhaseClassProfile> Profile; };
    std::vector<ExportCandidate> candidates;
    candidates.reserve(_loopPhaseTrackers.size());
    for (const auto &entry : _loopPhaseTrackers)
        if (entry.second->getIterationsSampled() > 0) candidates.push_back({entry.second->getPhaseEntryCount(), entry.second->BuildProfile()});
    std::sort(candidates.begin(), candidates.end(), [](const auto &left, const auto &right) {
        if (left.Heat != right.Heat) return left.Heat > right.Heat;
        if (left.Profile->OverallClassVariance != right.Profile->OverallClassVariance) return left.Profile->OverallClassVariance > right.Profile->OverallClassVariance;
        return left.Profile->LoopPcAddress < right.Profile->LoopPcAddress;
    });
    const size_t count = std::min(static_cast<size_t>(LoopPhaseExportLimit), candidates.size());
    std::vector<std::shared_ptr<LoopPhaseClassProfile>> profiles;
    profiles.reserve(count);
    for (size_t index = 0; index < count; ++index) profiles.push_back(candidates[index].Profile);
    return profiles;
}
inline MicroOpScheduler::ClassTemplateAdmissionState MicroOpScheduler::PrepareClassTemplateAdmissionState(int currentDomainId)
	{
		if (!_classTemplateValid || !_currentReplayPhase.isActive())
		{
			return YAKSys_Hybrid_CPU::Core::MicroOpScheduler::ClassTemplateAdmissionState();
		}

		if (_classTemplateDomainId != currentDomainId)
		{
			InvalidateClassTemplate(ReplayPhaseInvalidationReason::DomainBoundary);
			return YAKSys_Hybrid_CPU::Core::MicroOpScheduler::ClassTemplateAdmissionState();
		}

		if (!_classCapacityTemplate.IsCompatibleWith(_classCapacity))
		{
			InvalidateClassTemplate(ReplayPhaseInvalidationReason::ClassCapacityMismatch);
			return YAKSys_Hybrid_CPU::Core::MicroOpScheduler::ClassTemplateAdmissionState();
		}

		setClassTemplateReuseHits(getClassTemplateReuseHits() + 1);
		return ClassTemplateAdmissionState::Create(TemplateBudget(_classCapacityTemplate));
	}

	inline void MicroOpScheduler::InvalidateClassTemplate(ReplayPhaseInvalidationReason reason)
	{
		if (_classTemplateValid)
		{
			_classTemplateValid = false;
			setClassTemplateInvalidations(getClassTemplateInvalidations() + 1);
			setLastPhaseCertificateInvalidationReason(reason);

			// Phase 08: disaggregated invalidation tracking
			switch (reason)
			{
				case ReplayPhaseInvalidationReason::DomainBoundary:
					setClassTemplateDomainInvalidations(getClassTemplateDomainInvalidations() + 1);
					break;
				case ReplayPhaseInvalidationReason::ClassCapacityMismatch:
					setClassTemplateCapacityMismatchInvalidations(getClassTemplateCapacityMismatchInvalidations() + 1);
					break;
			}
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public MicroOpScheduler(IRuntimeLegalityService? runtimeLegalityService = null)
	MicroOpScheduler::MicroOpScheduler(std::shared_ptr<IRuntimeLegalityService> runtimeLegalityService) : _fspReplayPhase(false, 0, 0, 0, 0, 0, 0, ReplayPhaseInvalidationReason::None), _currentReplayPhase(false, 0, 0, 0, 0, 0, 0, ReplayPhaseInvalidationReason::None), _runtimeLegalityService((runtimeLegalityService != nullptr) ? runtimeLegalityService : RuntimeLegalityServiceFactory::CreateDefault(std::shared_ptr<ILegalityCertificateCacheTelemetrySink>(this, [](ILegalityCertificateCacheTelemetrySink *) {}))), _virtualizationAdmissionService(std::dynamic_pointer_cast<IVirtualizationAdmissionService>(_runtimeLegalityService))
	{

		// Initialize all scoreboard slots to -1 (free)
		for (int i = 0; i < SCOREBOARD_SLOTS; i++)
		{
			_scoreboard[i] = -1;
		}

		for (int vt = 0; vt < SMT_WAYS; vt++)
		{
			for (int s = 0; s < SCOREBOARD_SLOTS; s++)
			{
				_smtScoreboard[vt][s] = -1;
				_smtScoreboardType[vt][s] = ScoreboardEntryType::Free;
				_smtScoreboardBankId[vt][s] = -1;
			}
		}

		// Phase 2C: Initialize speculation budget to max
		_speculationBudget = SCOREBOARD_SLOTS / 2;

		// Phase 02: Initialize class-capacity totals from lane map
		_classCapacity.InitializeFromLaneMap();

		// Phase 04: Initialize per-class previous-lane to invalid (-1)
		for (int i = 0; i < _previousPhaseLane.size(); i++)
		{
			_previousPhaseLane[i] = -1;
		}

		// Phase 07: Initialize class template state
		_classTemplateValid = false;
		_classTemplateDomainId = 0;
	}

	inline bool MicroOpScheduler::HasClassCapacityFor(std::shared_ptr<MicroOp> candidate)
	{
		return _classCapacity.HasFreeCapacity(candidate->getPlacement().RequiredSlotClass);
	}

	SlotClassCapacityState MicroOpScheduler::GetClassCapacitySnapshot()
	{
		return _classCapacity;
	}

	long long MicroOpScheduler::GetPerVtInjectionCount(int vtId)
	{
		if (static_cast<unsigned int>(vtId) >= SMT_WAYS)
		{
			return 0;
		}

		return _perVtInjections[vtId];
	}

	inline MicroOpScheduler::InterCoreNominationSnapshot MicroOpScheduler::CaptureInterCoreNominationSnapshot()
	{
	#if defined(TESTING)
		if (getTestMode() && !getTestNominationQueues().empty())
		{
			return InterCoreNominationSnapshot::Capture(getTestNominationQueues());
		}
	#endif

		return InterCoreNominationSnapshot::Capture(_ports, _portValid);
	}

	inline void MicroOpScheduler::ConsumeInterCoreNomination(int coreId)
	{
		if (static_cast<unsigned int>(coreId) >= NUM_PORTS)
		{
			return;
		}

	#if defined(TESTING)
		if (getTestMode() && !getTestNominationQueues().empty() && coreId < getTestNominationQueues().size())
		{
			if (getTestNominationQueues()[coreId].size() > 0)
			{
				getTestNominationQueues()[coreId].pop_front();
			}

			return;
		}
	#endif

		_ports[coreId] = nullptr;
		_portValid[coreId] = false;
	}

	inline MicroOpScheduler::SmtNominationSnapshot MicroOpScheduler::CaptureSmtNominationSnapshot(SmtNominationState nominationState)
	{
		return SmtNominationSnapshot::Capture(_smtPorts, nominationState);
	}

	void MicroOpScheduler::ClearSmtNominationPorts()
	{
		for (int vt = 0; vt < SMT_WAYS; vt++)
		{
			_smtPorts[vt] = nullptr;
			_smtPortValid[vt] = false;
		}
	}

	long long MicroOpScheduler::getBankConflictsCount() const
	{
		return BankConflictsCount;
	}

	void MicroOpScheduler::setBankConflictsCount(long long value)
	{
		BankConflictsCount = value;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public ExecutionResult ScheduleWithArbitration(MicroOp? op, BankArbitrator arbitrator)
	MicroOpScheduler::ExecutionResult MicroOpScheduler::ScheduleWithArbitration(std::shared_ptr<MicroOp> op, std::shared_ptr<BankArbitrator> arbitrator)
	{
		if (op == nullptr)
		{
			return ExecutionResult::Failed;
		}

		// Non-memory ops always succeed
		if (!op->isMemoryOp())
		{
			return ExecutionResult::Success;
		}

		// Memory op: check bank availability
		std::shared_ptr<LoadMicroOp> loadOp = std::dynamic_pointer_cast<LoadMicroOp>(op);
		if (loadOp != nullptr)
		{
			int bankId = -1;
			if (!arbitrator->TryReserveBank(loadOp->getAddress(), bankId))
			{
				setBankConflictsCount(getBankConflictsCount() + 1);
				return ExecutionResult::Stall;
			}
		}
		else
		{
			std::shared_ptr<StoreMicroOp> storeOp = std::dynamic_pointer_cast<StoreMicroOp>(op);
			if (storeOp != nullptr)
			{
				int bankId = -1;
				if (!arbitrator->TryReserveBank(storeOp->getAddress(), bankId))
				{
					setBankConflictsCount(getBankConflictsCount() + 1);
					return ExecutionResult::Stall;
				}
			}
		}

		return ExecutionResult::Success;
	}

	bool MicroOpScheduler::getSuppressLsu() const
	{
		return SuppressLsu;
	}

	void MicroOpScheduler::setSuppressLsu(bool value)
	{
		SuppressLsu = value;
	}

	long long MicroOpScheduler::getMemoryWallSuppressionsCount() const
	{
		return MemoryWallSuppressionsCount;
	}

	void MicroOpScheduler::setMemoryWallSuppressionsCount(long long value)
	{
		MemoryWallSuppressionsCount = value;
	}

	long long MicroOpScheduler::getSuccessfulSpeculativeSteals() const
	{
		return SuccessfulSpeculativeSteals;
	}

	void MicroOpScheduler::setSuccessfulSpeculativeSteals(long long value)
	{
		SuccessfulSpeculativeSteals = value;
	}

	long long MicroOpScheduler::getFaultedSpeculativeSteals() const
	{
		return FaultedSpeculativeSteals;
	}

	void MicroOpScheduler::setFaultedSpeculativeSteals(long long value)
	{
		FaultedSpeculativeSteals = value;
	}

	long long MicroOpScheduler::getRejectedInjectionsCount() const
	{
		return RejectedInjectionsCount;
	}

	void MicroOpScheduler::setRejectedInjectionsCount(long long value)
	{
		RejectedInjectionsCount = value;
	}

	long long MicroOpScheduler::getRollbackEventsCount() const
	{
		return RollbackEventsCount;
	}

	void MicroOpScheduler::setRollbackEventsCount(long long value)
	{
		RollbackEventsCount = value;
	}

	void MicroOpScheduler::ProcessFaultedOperations(std::shared_ptr<std::vector<std::shared_ptr<MicroOp>>> bundle)
	{
		if (bundle == nullptr)
		{
			return;
		}

		for (int i = 0; i < static_cast<int>(bundle->size()); i++)
		{
			std::shared_ptr<MicroOp> op = (*bundle)[i];
			if (op == nullptr)
			{
				continue;
			}

			if (op->isSpeculative() && op->getFaulted())
			{
				// Restore OriginalResourceMask if it was saved before speculation
				if (op->getOriginalResourceMask() != 0)
				{
					op->setResourceMask(op->getOriginalResourceMask());
					op->setOriginalResourceMask(0);
				}

				op->setSpeculative(false);
				op->setFaulted(false);
				setFaultedSpeculativeSteals(getFaultedSpeculativeSteals() + 1);

				// Phase 2C: Release speculation budget on squash
				ReleaseSpeculationBudget();
			}
		}
	}

	void MicroOpScheduler::ReleaseSpeculationBudget()
	{
		if (getSpeculationBudgetEnabled() && _speculationBudget < getSpeculationBudgetMax())
		{
			_speculationBudget++;
		}
	}

	int MicroOpScheduler::getBarrierCount() const
	{
		return _barrierCount;
	}

	void MicroOpScheduler::AddManualBarrier(int vtId, int instructionIndex, std::vector<int> &threads)
	{
		if (_barrierCount >= _barriers.size())
		{
			return;
		}
		_barriers[_barrierCount] = BarrierEntry{vtId, instructionIndex, threads};
		_barrierCount++;
	}

	void MicroOpScheduler::OptimizeBarriers()
	{
		if (_barrierCount <= 1)
		{
			return;
		}

		int writeIdx = 0;
		for (int i = 0; i < _barrierCount; i++)
		{
			bool merged = false;
			if (writeIdx > 0)
			{
				int prevInstr = _barriers[writeIdx - 1].instrIndex;
				int curInstr = _barriers[i].instrIndex;
				// Merge if same VT and within 5 instructions
				if (_barriers[writeIdx - 1].vtId == _barriers[i].vtId && std::abs(curInstr - prevInstr) <= 5)
				{
					merged = true;
				}
			}

			if (!merged)
			{
				_barriers[writeIdx] = _barriers[i];
				writeIdx++;
			}
		}
		_barrierCount = writeIdx;
	}

	unsigned short MicroOpScheduler::GenerateAffinityMask(std::vector<int> &threads)
	{
		unsigned short mask = 0;
		if (threads.empty())
		{
			return mask;
		}
		for (int i = 0; i < threads.size(); i++)
		{
			if (threads[i] >= 0 && threads[i] < 16)
			{
				mask |= static_cast<unsigned short>(1 << threads[i]);
			}
		}
		return mask;
	}

	bool MicroOpScheduler::getEnableShadowOracle() const
	{
		return EnableShadowOracle;
	}

	void MicroOpScheduler::setEnableShadowOracle(bool value)
	{
		EnableShadowOracle = value;
	}

	bool MicroOpScheduler::getEnableRf06MemoryContractDifferential() const
	{
		return EnableRf06MemoryContractDifferential;
	}

	void MicroOpScheduler::setEnableRf06MemoryContractDifferential(bool value)
	{
		EnableRf06MemoryContractDifferential = value;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public ShadowOracleCycleSnapshot(MicroOp?[] bundleSlots, SmtNominationSnapshot nominationSnapshot)
	MicroOpScheduler::ShadowOracleCycleSnapshot::ShadowOracleCycleSnapshot(std::vector<std::shared_ptr<MicroOp>> &bundleSlots, SmtNominationSnapshot nominationSnapshot) : _bundleSlots(bundleSlots), NominationSnapshot(nominationSnapshot)
	{
	}

	MicroOpScheduler::SmtNominationSnapshot MicroOpScheduler::ShadowOracleCycleSnapshot::getNominationSnapshot() const
	{
		return NominationSnapshot;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public bool TryGetBundleSlot(int slotIndex, out MicroOp? bundleOp)
	inline bool MicroOpScheduler::ShadowOracleCycleSnapshot::TryGetBundleSlot(int slotIndex, std::shared_ptr<MicroOp> &bundleOp) const
	{
		bundleOp = nullptr;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MicroOp?[]? bundleSlots = _bundleSlots;
		if (static_cast<unsigned int>(slotIndex) >= _bundleSlots.size())
		{
			return false;
		}

		bundleOp = _bundleSlots[slotIndex];
		return true;
	}

	MicroOpScheduler::ShadowOracleCycleSnapshot MicroOpScheduler::ShadowOracleCycleSnapshot::Capture(std::vector<std::shared_ptr<MicroOp>> &bundle, SmtNominationSnapshot nominationSnapshot)
	{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MicroOp?[] bundleSlots = new MicroOp?[8];
		std::vector<std::shared_ptr<MicroOp>> bundleSlots(8);
		std::copy_n(bundle.begin(), std::min<std::size_t>(bundle.size(), bundleSlots.size()), bundleSlots.begin());
		return ShadowOracleCycleSnapshot(bundleSlots, nominationSnapshot);
	}

	inline MicroOpScheduler::ShadowOracleCycleSnapshot MicroOpScheduler::CaptureShadowOracleCycleSnapshot(std::vector<std::shared_ptr<MicroOp>> &bundle, SmtNominationState nominationState)
	{
		return ShadowOracleCycleSnapshot::Capture(bundle, CaptureSmtNominationSnapshot(nominationState));
	}

	OracleGapSummary MicroOpScheduler::GetOracleGapSummary()
	{
		return OracleGapSummary(_oracleTotalSlots, _oracleRealSlots, _oracleDonorRestrictionGap, _oracleFairnessOrderingGap, _oracleLegalityConservatismGap, _oracleDomainIsolationGap, _oracleSpeculationBudgetGap, _oracleCyclesWithGap, _oracleTotalCyclesAnalyzed);
	}

	const std::vector<OracleCounterexample> MicroOpScheduler::GetCounterexamples()
	{
		return _counterexamples;
	}

	inline int MicroOpScheduler::ComputeShadowOraclePackCount(ShadowOracleCycleSnapshot cycleSnapshot, int ownerVirtualThreadId)
	{
		// Build a fresh certificate for the existing bundle contents
		BundleResourceCertificate4Way oracleCert = BundleResourceCertificate4Way::Empty;
		for (int i = 0; i < 8; i++)
		{
			std::shared_ptr<MicroOp> bundleOp;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (cycleSnapshot.TryGetBundleSlot(i, out MicroOp? bundleOp) && bundleOp != null)
			if (cycleSnapshot.TryGetBundleSlot(i, bundleOp) && bundleOp != nullptr)
			{
				oracleCert.AddOperation(bundleOp);
			}
		}

		// Find all empty slots (ignoring replay-stable donor mask)

		std::array<int, 8> emptySlotsStorage{};
		std::span<int> emptySlots(emptySlotsStorage);
		int emptyCount = 0;
		for (int i = 0; i < 8; i++)
		{
			std::shared_ptr<MicroOp> bundleOp;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (!cycleSnapshot.TryGetBundleSlot(i, out MicroOp? bundleOp) || bundleOp == null)
			if (!cycleSnapshot.TryGetBundleSlot(i, bundleOp) || bundleOp == nullptr)
			{
				emptySlots[emptyCount++] = i;
			}
		}

		if (emptyCount == 0)
		{
			return 0;
		}

		unsigned short localOutstandingStoreBankMask = 0;
		unsigned char remainingHardwareMemoryIssueBudget = _hardwareOccupancySnapshot.getMemoryIssueBudget();
		unsigned int consumedHardwareMemoryBudgetByBank = 0;
		unsigned char remainingHardwareLoadIssueBudget = _hardwareOccupancySnapshot.getReadIssueBudget();
		unsigned char remainingHardwareStoreIssueBudget = _hardwareOccupancySnapshot.getWriteIssueBudget();
		unsigned int consumedHardwareLoadBudgetByBank = 0;
		unsigned int consumedHardwareStoreBudgetByBank = 0;
		std::vector<unsigned char> projectedOutstandingMemoryCount(SMT_WAYS);
		std::vector<unsigned char> projectedOutstandingMemoryCapacity(SMT_WAYS);
		for (int vt = 0; vt < SMT_WAYS; vt++)
		{
			int liveOutstandingMemoryCount = GetOutstandingMemoryCount(vt);
			int liveFreeScoreboardSlots = GetFreeSmtScoreboardSlotCount(vt);
			projectedOutstandingMemoryCount[vt] = SaturateScoreboardCount(liveOutstandingMemoryCount);
			projectedOutstandingMemoryCapacity[vt] = SaturateScoreboardCount(liveOutstandingMemoryCount + liveFreeScoreboardSlots);
		}

		for (int i = 0; i < 8; i++)
		{
			std::shared_ptr<MicroOp> bundleOp;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (!cycleSnapshot.TryGetBundleSlot(i, out MicroOp? bundleOp) || bundleOp == null)
			if (!cycleSnapshot.TryGetBundleSlot(i, bundleOp) || bundleOp == nullptr)
			{
				continue;
			}

			std::shared_ptr<LoadStoreMicroOp> bundleMemoryOp = std::dynamic_pointer_cast<LoadStoreMicroOp>(bundleOp);
			if (bundleMemoryOp != nullptr)
			{
				IncrementProjectedOutstandingMemoryCount(projectedOutstandingMemoryCount, bundleMemoryOp->getVirtualThreadId());

				if (remainingHardwareMemoryIssueBudget > 0)
				{
					remainingHardwareMemoryIssueBudget--;
				}

				consumedHardwareMemoryBudgetByBank = IncrementPackedConsumedHardwareBudget(consumedHardwareMemoryBudgetByBank, bundleMemoryOp->getMemoryBankId());

				if (std::dynamic_pointer_cast<LoadMicroOp>(bundleMemoryOp) != nullptr)
				{
					if (remainingHardwareLoadIssueBudget > 0)
					{
						remainingHardwareLoadIssueBudget--;
					}

					consumedHardwareLoadBudgetByBank = IncrementPackedConsumedHardwareBudget(consumedHardwareLoadBudgetByBank, bundleMemoryOp->getMemoryBankId());
				}
				else if (std::dynamic_pointer_cast<StoreMicroOp>(bundleMemoryOp) != nullptr)
				{
					if (remainingHardwareStoreIssueBudget > 0)
					{
						remainingHardwareStoreIssueBudget--;
					}

					consumedHardwareStoreBudgetByBank = IncrementPackedConsumedHardwareBudget(consumedHardwareStoreBudgetByBank, bundleMemoryOp->getMemoryBankId());
				}
			}

			std::shared_ptr<StoreMicroOp> bundleStore = std::dynamic_pointer_cast<StoreMicroOp>(bundleOp);
			if (bundleStore != nullptr && static_cast<unsigned int>(bundleStore->getMemoryBankId()) < 16)
			{
				localOutstandingStoreBankMask |= static_cast<unsigned short>(1 << bundleStore->getMemoryBankId());
			}
		}

		// Try all SMT candidates in ascending VT order (ignoring fairness ranking).
		// Oracle still ignores speculation budget, but now mirrors the live
		// bank-pending/store-bank plus projected per-VT scoreboard contour
		// and now also mirrors the sampled mixed hardware-memory budget.
		// for the working bundle.
		int oraclePacked = 0;
		int nextEmpty = 0;

		for (int vt = 0; vt < SMT_WAYS; vt++)
		{
			if (vt == ownerVirtualThreadId)
			{
				continue;
			}
			std::shared_ptr<MicroOp> candidate;
			if (!cycleSnapshot.getNominationSnapshot().TryGetCandidate(vt, candidate))
			{
				continue;
			}

			// Oracle preserves legality: check via certificate
			if (!oracleCert.CanInject(candidate))
			{
				continue;
			}

			// Oracle preserves domain isolation (but NOT speculation budget)
			if (getSuppressLsu() && candidate->isMemoryOp())
			{
				continue;
			}

			int candidateVirtualThreadId = candidate->getVirtualThreadId();
			std::shared_ptr<LoadStoreMicroOp> lsCandidate = std::dynamic_pointer_cast<LoadStoreMicroOp>(candidate);
			int candidateBankId = lsCandidate != nullptr ? lsCandidate->getMemoryBankId() : -1;
			AssertRf06MemoryContractDifferential(candidate, candidateVirtualThreadId, localOutstandingStoreBankMask, remainingHardwareMemoryIssueBudget, consumedHardwareMemoryBudgetByBank, remainingHardwareLoadIssueBudget, consumedHardwareLoadBudgetByBank, remainingHardwareStoreIssueBudget, consumedHardwareStoreBudgetByBank, projectedOutstandingMemoryCount, projectedOutstandingMemoryCapacity);
			if (candidate->isMemoryOp() && static_cast<unsigned int>(candidateVirtualThreadId) >= SMT_WAYS)
			{
				continue;
			}
			if (candidate->isMemoryOp() && projectedOutstandingMemoryCount[candidateVirtualThreadId] >= projectedOutstandingMemoryCapacity[candidateVirtualThreadId])
			{
				continue;
			}
			if (candidate->isMemoryOp() && candidateBankId >= 0 && IsBankPendingForVT(candidateBankId, candidateVirtualThreadId))
			{
				continue;
			}
			if (std::dynamic_pointer_cast<StoreMicroOp>(candidate) != nullptr && candidateBankId >= 0 && (localOutstandingStoreBankMask & (1 << candidateBankId)) != 0)
			{
				continue;
			}
			if (std::dynamic_pointer_cast<LoadStoreMicroOp>(candidate) != nullptr && (!HasOracleHardwareMemoryBudget(candidate, remainingHardwareMemoryIssueBudget, consumedHardwareMemoryBudgetByBank, remainingHardwareLoadIssueBudget, consumedHardwareLoadBudgetByBank, remainingHardwareStoreIssueBudget, consumedHardwareStoreBudgetByBank, candidateBankId)))
			{
				continue;
			}

			// Find next empty slot (no donor mask filtering)
			if (nextEmpty >= emptyCount)
			{
				break;
			}

			oracleCert.AddOperation(candidate);
			std::shared_ptr<LoadStoreMicroOp> memoryCandidate = std::dynamic_pointer_cast<LoadStoreMicroOp>(candidate);
			if (memoryCandidate != nullptr)
			{
				IncrementProjectedOutstandingMemoryCount(projectedOutstandingMemoryCount, memoryCandidate->getVirtualThreadId());

				if (remainingHardwareMemoryIssueBudget > 0)
				{
					remainingHardwareMemoryIssueBudget--;
				}

				consumedHardwareMemoryBudgetByBank = IncrementPackedConsumedHardwareBudget(consumedHardwareMemoryBudgetByBank, memoryCandidate->getMemoryBankId());

				if (std::dynamic_pointer_cast<LoadMicroOp>(memoryCandidate) != nullptr)
				{
					if (remainingHardwareLoadIssueBudget > 0)
					{
						remainingHardwareLoadIssueBudget--;
					}

					consumedHardwareLoadBudgetByBank = IncrementPackedConsumedHardwareBudget(consumedHardwareLoadBudgetByBank, memoryCandidate->getMemoryBankId());
				}
				else if (std::dynamic_pointer_cast<StoreMicroOp>(memoryCandidate) != nullptr)
				{
					if (remainingHardwareStoreIssueBudget > 0)
					{
						remainingHardwareStoreIssueBudget--;
					}

					consumedHardwareStoreBudgetByBank = IncrementPackedConsumedHardwareBudget(consumedHardwareStoreBudgetByBank, memoryCandidate->getMemoryBankId());
				}
			}
			if (std::dynamic_pointer_cast<StoreMicroOp>(candidate) != nullptr && candidateBankId >= 0 && candidateBankId < 16)
			{
				localOutstandingStoreBankMask |= static_cast<unsigned short>(1 << candidateBankId);
			}
			oraclePacked++;
			nextEmpty++;
		}

		return oraclePacked;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private void AssertRf06MemoryContractDifferential(MicroOp candidate, int candidateVirtualThreadId, ushort localOutstandingStoreBankMask, byte remainingHardwareMemoryIssueBudget, uint consumedHardwareMemoryBudgetByBank, byte remainingHardwareLoadIssueBudget, uint consumedHardwareLoadBudgetByBank, byte remainingHardwareStoreIssueBudget, uint consumedHardwareStoreBudgetByBank, IReadOnlyList<byte> projectedOutstandingMemoryCount, IReadOnlyList<byte> projectedOutstandingMemoryCapacity)
	inline void MicroOpScheduler::AssertRf06MemoryContractDifferential(std::shared_ptr<MicroOp> candidate, int candidateVirtualThreadId, unsigned short localOutstandingStoreBankMask, unsigned char remainingHardwareMemoryIssueBudget, unsigned int consumedHardwareMemoryBudgetByBank, unsigned char remainingHardwareLoadIssueBudget, unsigned int consumedHardwareLoadBudgetByBank, unsigned char remainingHardwareStoreIssueBudget, unsigned int consumedHardwareStoreBudgetByBank, const std::vector<unsigned char> &projectedOutstandingMemoryCount, const std::vector<unsigned char> &projectedOutstandingMemoryCapacity)
	{
		std::shared_ptr<LoadStoreMicroOp> carrier = std::dynamic_pointer_cast<LoadStoreMicroOp>(candidate);
		if (!getEnableRf06MemoryContractDifferential() || carrier == nullptr)
		{
			return;
		}
		std::shared_ptr<MemoryCapability> capability;
		if (!Rf06MemoryShadowOracle::TryProjectLegacyCarrier(carrier, capability))
		{
			return;
		}

		std::vector<unsigned char> memoryBudgets(MemoryBankId::BankCount);
		std::vector<unsigned char> readBudgets(MemoryBankId::BankCount);
		std::vector<unsigned char> writeBudgets(MemoryBankId::BankCount);
		std::vector<unsigned char> consumedMemory(MemoryBankId::BankCount);
		std::vector<unsigned char> consumedReads(MemoryBankId::BankCount);
		std::vector<unsigned char> consumedWrites(MemoryBankId::BankCount);
		for (int bank = 0; bank < MemoryBankId::BankCount; bank++)
		{
			memoryBudgets[bank] = _hardwareOccupancySnapshot.GetMemoryBudgetForBank(bank);
			readBudgets[bank] = _hardwareOccupancySnapshot.GetReadBudgetForBank(bank);
			writeBudgets[bank] = _hardwareOccupancySnapshot.GetWriteBudgetForBank(bank);
			consumedMemory[bank] = GetPackedConsumedHardwareBudget(consumedHardwareMemoryBudgetByBank, bank);
			consumedReads[bank] = GetPackedConsumedHardwareBudget(consumedHardwareLoadBudgetByBank, bank);
			consumedWrites[bank] = GetPackedConsumedHardwareBudget(consumedHardwareStoreBudgetByBank, bank);
		}

		unsigned short pendingBankMask = 0;
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: if (IsBankPendingForVT(capability.Bank!.Value.Value, candidateVirtualThreadId))
		if (IsBankPendingForVT(capability->Bank->Value, candidateVirtualThreadId))
		{
			pendingBankMask = static_cast<unsigned short>(1 << capability->Bank->Value);
		}

		std::shared_ptr<Rf06MemoryShadowState> state = Rf06MemoryShadowState::Create(remainingHardwareMemoryIssueBudget, remainingHardwareLoadIssueBudget, remainingHardwareStoreIssueBudget, memoryBudgets, readBudgets, writeBudgets, consumedMemory, consumedReads, consumedWrites, projectedOutstandingMemoryCount, projectedOutstandingMemoryCapacity, pendingBankMask, localOutstandingStoreBankMask);
		std::shared_ptr<Rf06MemoryShadowDecision> legacy = Rf06MemoryShadowOracle::EvaluateLegacyCarrier(carrier, state);
		std::shared_ptr<Rf06MemoryShadowDecision> contract = Rf06MemoryShadowOracle::EvaluateContract(capability, candidateVirtualThreadId, state);
		if (legacy->IsEligible != contract->IsEligible || legacy->RejectReason != contract->RejectReason)
		{
			throw InvalidOperationException(std::format("RF-06.4b memory ShadowOracle differential mismatch for VT {}: legacy={}, contract={}.", candidateVirtualThreadId, static_cast<int>(legacy->RejectReason), static_cast<int>(contract->RejectReason)));
		}
	}

	inline void MicroOpScheduler::AssertRf06MemoryContractProjection(std::shared_ptr<MicroOp> candidate)
	{
		std::shared_ptr<LoadStoreMicroOp> carrier = std::dynamic_pointer_cast<LoadStoreMicroOp>(candidate);
		if (!getEnableRf06MemoryContractDifferential() || carrier == nullptr)
		{
			return;
		}

		std::shared_ptr<MemoryCapability> capability;
		if (!Rf06MemoryShadowOracle::TryProjectLegacyCarrier(carrier, capability))
		{
			throw InvalidOperationException(std::format("RF-06.4b FSP memory carrier for VT {} has no representable immutable bank/footprint contract.", candidate->getVirtualThreadId()));
		}
	}

	inline bool MicroOpScheduler::HasOracleHardwareMemoryBudget(std::shared_ptr<MicroOp> candidate, unsigned char remainingHardwareMemoryIssueBudget, unsigned int consumedHardwareMemoryBudgetByBank, unsigned char remainingHardwareLoadIssueBudget, unsigned int consumedHardwareLoadBudgetByBank, unsigned char remainingHardwareStoreIssueBudget, unsigned int consumedHardwareStoreBudgetByBank, int memoryBankId)
	{
		if (remainingHardwareMemoryIssueBudget == 0)
		{
			return false;
		}

		if (static_cast<unsigned int>(memoryBankId) >= 16)
		{
			return true;
		}

		if (GetPackedConsumedHardwareBudget(consumedHardwareMemoryBudgetByBank, memoryBankId) >= _hardwareOccupancySnapshot.GetMemoryBudgetForBank(memoryBankId))
		{
			return false;
		}

		if (std::dynamic_pointer_cast<LoadMicroOp>(candidate) != nullptr)
		{
			return HasOracleDirectionalHardwareBudget(remainingHardwareLoadIssueBudget, consumedHardwareLoadBudgetByBank, _hardwareOccupancySnapshot.GetReadBudgetForBank(memoryBankId), memoryBankId);
		}

		if (std::dynamic_pointer_cast<StoreMicroOp>(candidate) != nullptr)
		{
			return HasOracleDirectionalHardwareBudget(remainingHardwareStoreIssueBudget, consumedHardwareStoreBudgetByBank, _hardwareOccupancySnapshot.GetWriteBudgetForBank(memoryBankId), memoryBankId);
		}

		return true;
	}

	inline bool MicroOpScheduler::HasOracleDirectionalHardwareBudget(unsigned char remainingDirectionalIssueBudget, unsigned int consumedDirectionalBudgetByBank, unsigned char bankBudget, int memoryBankId)
	{
		if (remainingDirectionalIssueBudget == 0)
		{
			return false;
		}

		if (static_cast<unsigned int>(memoryBankId) >= 16)
		{
			return true;
		}

		return GetPackedConsumedHardwareBudget(consumedDirectionalBudgetByBank, memoryBankId) < bankBudget;
	}

	inline void MicroOpScheduler::RecordOracleGap(ShadowOracleCycleSnapshot cycleSnapshot, int ownerVirtualThreadId, int realInjectedCount)
	{
		if (!getEnableShadowOracle())
		{
			return;
		}

		int oraclePacked = ComputeShadowOraclePackCount(cycleSnapshot, ownerVirtualThreadId);

		_oracleTotalSlots += oraclePacked;
		_oracleRealSlots += realInjectedCount;
		_oracleTotalCyclesAnalyzed++;

		int gap = std::max(0, oraclePacked - realInjectedCount);
		if (gap <= 0)
		{
			return;
		}

		_oracleCyclesWithGap++;

		// Classify the primary gap reason
		OracleGapCategory category = ClassifyGapCategory(cycleSnapshot, ownerVirtualThreadId);

		switch (category)
		{
			case OracleGapCategory::DonorRestriction:
				_oracleDonorRestrictionGap += gap;
				break;
			case OracleGapCategory::FairnessOrdering:
				_oracleFairnessOrderingGap += gap;
				break;
			case OracleGapCategory::LegalityConservatism:
				_oracleLegalityConservatismGap += gap;
				break;
			case OracleGapCategory::DomainIsolation:
				_oracleDomainIsolationGap += gap;
				break;
			case OracleGapCategory::SpeculationBudget:
				_oracleSpeculationBudgetGap += gap;
				break;
		}

		// Emit bounded counterexample
		if (_counterexamples.size() < MAX_COUNTEREXAMPLES)
		{
			std::string description;
			switch (category)
			{
			case OracleGapCategory::DonorRestriction:
				description = std::format("Replay-stable donor mask 0x{:02X} hid {} injectable slot(s)", static_cast<unsigned int>(_currentReplayPhase.getStableDonorMask()), gap);
				break;
			case OracleGapCategory::FairnessOrdering:
				description = std::format("Credit-based fairness ordering skipped {} ready candidate(s)", gap);
				break;
			case OracleGapCategory::LegalityConservatism:
				description = std::format("Safety mask or certificate conservatism rejected {} oracle-legal candidate(s)", gap);
				break;
			case OracleGapCategory::DomainIsolation:
				description = std::format("Domain isolation enforcement blocked {} candidate(s)", gap);
				break;
			case OracleGapCategory::SpeculationBudget:
				description = std::format("Speculation budget exhaustion prevented {} speculative injection(s)", gap);
				break;
			default:
				description = std::format("Unclassified gap of {} slot(s)", gap);
				break;
			}

			_counterexamples.push_back(OracleCounterexample(getTotalSchedulerCycles(), category, gap, std::wstring(description.begin(), description.end())));
		}
	}

	inline OracleGapCategory MicroOpScheduler::ClassifyGapCategory(ShadowOracleCycleSnapshot cycleSnapshot, int ownerVirtualThreadId)
	{
		// Priority order: donor restriction > speculation budget > domain isolation > fairness > legality

		// If replay phase is active and donor mask restricts slots, donor restriction is primary
		if (_currentReplayPhase.isActive() && _currentReplayPhase.getHasStableDonorStructure())
		{
			unsigned char donorMask = _currentReplayPhase.getStableDonorMask();
			if (donorMask != 0xFF)
			{
				return OracleGapCategory::DonorRestriction;
			}
		}

		// If speculation budget was exhausted this cycle
		if (getSpeculationBudgetEnabled() && _speculationBudget <= 0)
		{
			return OracleGapCategory::SpeculationBudget;
		}

		// If domain isolation blocked candidates recently
		if (getDomainIsolationBlockedAttempts() > 0 && _runtimeLegalityService->isKernelDomainIsolationEnabled())
		{
			// Check if any pending candidate has domain isolation issues
			for (int vt = 0; vt < SMT_WAYS; vt++)
			{
				if (vt == ownerVirtualThreadId)
				{
					continue;
				}
				std::shared_ptr<MicroOp> candidate;
				if (cycleSnapshot.getNominationSnapshot().TryGetCandidate(vt, candidate) && candidate->getPlacement().DomainTag == 0)
				{
					return OracleGapCategory::DomainIsolation;
				}
			}
		}

		// If fairness credit ordering was active
		if (getCreditFairnessEnabled())
		{
			return OracleGapCategory::FairnessOrdering;
		}

		return OracleGapCategory::LegalityConservatism;
	}

	bool MicroOpScheduler::getTestMode() const
	{
		return TestMode;
	}

	void MicroOpScheduler::setTestMode(bool value)
	{
		TestMode = value;
	}

	std::vector<std::deque<std::shared_ptr<MicroOp>>> MicroOpScheduler::getTestNominationQueues() const
	{
		return TestNominationQueues;
	}

	void MicroOpScheduler::setTestNominationQueues(const std::vector<std::deque<std::shared_ptr<MicroOp>>> &value)
	{
		TestNominationQueues = value;
	}

	std::wstring MicroOpScheduler::getLastRejectionReason() const
	{
		return LastRejectionReason;
	}

	void MicroOpScheduler::setLastRejectionReason(const std::wstring &value)
	{
		LastRejectionReason = value;
	}

	long long MicroOpScheduler::getTestRejectedDueToResourceConflict() const
	{
		return TestRejectedDueToResourceConflict;
	}

	void MicroOpScheduler::setTestRejectedDueToResourceConflict(long long value)
	{
		TestRejectedDueToResourceConflict = value;
	}

	long long MicroOpScheduler::getTestRejectedDueToMemoryConflict() const
	{
		return TestRejectedDueToMemoryConflict;
	}

	void MicroOpScheduler::setTestRejectedDueToMemoryConflict(long long value)
	{
		TestRejectedDueToMemoryConflict = value;
	}

	long long MicroOpScheduler::getTestRejectedDueToSafetyMask() const
	{
		return TestRejectedDueToSafetyMask;
	}

	void MicroOpScheduler::setTestRejectedDueToSafetyMask(long long value)
	{
		TestRejectedDueToSafetyMask = value;
	}

	long long MicroOpScheduler::getTestRejectedDueToFullBundle() const
	{
		return TestRejectedDueToFullBundle;
	}

	void MicroOpScheduler::setTestRejectedDueToFullBundle(long long value)
	{
		TestRejectedDueToFullBundle = value;
	}

	void MicroOpScheduler::InitializeTestMode()
	{
		setTestMode(true);
		setTestNominationQueues(std::vector<std::deque<std::shared_ptr<MicroOp>>>(4));
		for (int i = 0; i < 4; i++)
		{
			getTestNominationQueues()[i] = std::deque<std::shared_ptr<MicroOp>>();
		}
		ResetTestCounters();
	}

	void MicroOpScheduler::TestEnqueueMicroOp(int vtId, std::shared_ptr<MicroOp> op)
	{
		if (!getTestMode())
		{
			throw InvalidOperationException(L"TestEnqueueMicroOp can only be called when TestMode is enabled. Call InitializeTestMode() first.");
		}

		if (vtId < 0 || vtId >= 4)
		{
			throw ArgumentOutOfRangeException(L"vtId", vtId, L"Virtual thread ID must be 0-3 for 4-way SMT.");
		}

		if (op == nullptr)
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentNullException(nameof(op), "Cannot enqueue null micro-op.");
			throw std::invalid_argument("op");
		}

		getTestNominationQueues()[vtId].push_back(op);
	}

	void MicroOpScheduler::TestRecordRejection(const std::wstring &reason, RejectionType type)
	{
		if (!getTestMode())
		{
			return;
		}

		setLastRejectionReason(reason);

		switch (type)
		{
			case RejectionType::ResourceConflict:
				setTestRejectedDueToResourceConflict(getTestRejectedDueToResourceConflict() + 1);
				break;
			case RejectionType::MemoryConflict:
				setTestRejectedDueToMemoryConflict(getTestRejectedDueToMemoryConflict() + 1);
				break;
			case RejectionType::SafetyMask:
				setTestRejectedDueToSafetyMask(getTestRejectedDueToSafetyMask() + 1);
				break;
			case RejectionType::FullBundle:
				setTestRejectedDueToFullBundle(getTestRejectedDueToFullBundle() + 1);
				break;
		}
	}

	void MicroOpScheduler::ResetTestCounters()
	{
		setTestRejectedDueToResourceConflict(0);
		setTestRejectedDueToMemoryConflict(0);
		setTestRejectedDueToSafetyMask(0);
		setTestRejectedDueToFullBundle(0);
		setLastRejectionReason(L"");
	}

	int MicroOpScheduler::TestGetPendingNominationCount()
	{
		if (!getTestMode() || getTestNominationQueues().empty())
		{
			return 0;
		}

		int total = 0;
		for (int i = 0; i < 4; i++)
		{
			total += getTestNominationQueues()[i].size();
		}
		return total;
	}

	void MicroOpScheduler::TestClearNominationQueues()
	{
		if (!getTestMode() || getTestNominationQueues().empty())
		{
			return;
		}

		for (int i = 0; i < 4; i++)
		{
			getTestNominationQueues()[i].clear();
		}
	}

	int MicroOpScheduler::TestGetFairnessCredit(int vtId)
	{
		if (static_cast<unsigned int>(vtId) >= 4)
		{
			return 0;
		}
		return _fairnessCredits[vtId];
	}

	void MicroOpScheduler::TestSetFairnessCredit(int vtId, int credit)
	{
		if (static_cast<unsigned int>(vtId) >= 4)
		{
			return;
		}
		_fairnessCredits[vtId] = credit;
	}

	long long MicroOpScheduler::TestGetPerVtInjections(int vtId)
	{
		if (static_cast<unsigned int>(vtId) >= 4)
		{
			return 0;
		}
		return _perVtInjections[vtId];
	}

	long long MicroOpScheduler::TestGetPerVtRejections(int vtId)
	{
		int temp_var134;
		switch (vtId)
		{
			case 0:
				temp_var134 = getRejectionsVT0();
				break;
			case 1:
				temp_var134 = getRejectionsVT1();
				break;
			case 2:
				temp_var134 = getRejectionsVT2();
				break;
			case 3:
				temp_var134 = getRejectionsVT3();
				break;
			default:
				temp_var134 = 0;
				break;
		}
		return temp_var134;
	}

	long long MicroOpScheduler::TestGetPerVtRegGroupConflicts(int vtId)
	{
		int temp_var135;
		switch (vtId)
		{
			case 0:
				temp_var135 = getRegGroupConflictsVT0();
				break;
			case 1:
				temp_var135 = getRegGroupConflictsVT1();
				break;
			case 2:
				temp_var135 = getRegGroupConflictsVT2();
				break;
			case 3:
				temp_var135 = getRegGroupConflictsVT3();
				break;
			default:
				temp_var135 = 0;
				break;
		}
		return temp_var135;
	}

	int MicroOpScheduler::TestGetSpeculationBudget()
	{
		return _speculationBudget;
	}

	void MicroOpScheduler::TestSetSpeculationBudget(int budget)
	{
		_speculationBudget = budget;
	}

	void MicroOpScheduler::TestSetPhaseMetrics(SchedulerPhaseMetrics metrics)
	{
		setReplayAwareCycles(metrics.getReplayAwareCycles());
		setPhaseCertificateReadyHits(metrics.getPhaseCertificateReadyHits());
		setPhaseCertificateReadyMisses(metrics.getPhaseCertificateReadyMisses());
		setEstimatedPhaseCertificateChecksSaved(metrics.getEstimatedChecksSaved());
		setPhaseCertificateInvalidations(metrics.getPhaseCertificateInvalidations());
		setPhaseCertificateMutationInvalidations(metrics.getPhaseCertificateMutationInvalidations());
		setPhaseCertificatePhaseMismatchInvalidations(metrics.getPhaseCertificatePhaseMismatchInvalidations());
		setLastPhaseCertificateInvalidationReason(metrics.getLastCertificateInvalidationReason());
		setDeterminismReferenceOpportunitySlots(metrics.getDeterminismReferenceOpportunitySlots());
		setDeterminismReplayEligibleSlots(metrics.getDeterminismReplayEligibleSlots());
		setDeterminismMaskedSlots(metrics.getDeterminismMaskedSlots());
		setDeterminismEstimatedLostSlots(metrics.getDeterminismEstimatedLostSlots());
		setDeterminismConstrainedCycles(metrics.getDeterminismConstrainedCycles());
		setDomainIsolationProbeAttempts(metrics.getDomainIsolationProbeAttempts());
		setDomainIsolationBlockedAttempts(metrics.getDomainIsolationBlockedAttempts());
		setDomainIsolationCrossDomainBlocks(metrics.getDomainIsolationCrossDomainBlocks());
		setDomainIsolationKernelToUserBlocks(metrics.getDomainIsolationKernelToUserBlocks());
		setEligibilityMaskedCycles(metrics.getEligibilityMaskedCycles());
		setEligibilityMaskedReadyCandidates(metrics.getEligibilityMaskedReadyCandidates());
		_lastSmtEligibilitySnapshot = std::make_shared<SmtEligibilitySnapshot>(metrics.getLastEligibilityRequestedMask(), metrics.getLastEligibilityNormalizedMask(), metrics.getLastEligibilityReadyPortMask(), metrics.getLastEligibilityVisibleReadyMask(), metrics.getLastEligibilityMaskedReadyMask());
		setClassTemplateReuseHits(metrics.getClassTemplateReuseHits());
		setClassTemplateInvalidations(metrics.getClassTemplateInvalidations());
		setTypedSlotFastPathAccepts(metrics.getTypedSlotFastPathAccepts());
		setTypedSlotStandardPathAccepts(metrics.getTypedSlotStandardPathAccepts());
		setTotalLaneBindings(metrics.getTotalLaneBindings());
		setLaneReuseHits(metrics.getLaneReuseHits());
		setLaneReuseMisses(metrics.getLaneReuseMisses());
		setAluClassInjects(metrics.getAluClassInjects());
		setLsuClassInjects(metrics.getLsuClassInjects());
		setDmaStreamClassInjects(metrics.getDmaStreamClassInjects());
		setMatrixTileStreamClassInjects(metrics.getMatrixTileStreamClassInjects());
		setBranchControlInjects(metrics.getBranchControlInjects());
		setHardPinnedInjects(metrics.getHardPinnedInjects());
		setClassFlexibleInjects(metrics.getClassFlexibleInjects());
		setNopAvoided(metrics.getNopAvoided());
		setNopDueToPinnedConstraint(metrics.getNopDueToPinnedConstraint());
		setNopDueToNoClassCapacity(metrics.getNopDueToNoClassCapacity());
		setNopDueToResourceConflict(metrics.getNopDueToResourceConflict());
		setNopDueToDynamicState(metrics.getNopDueToDynamicState());
		setStaticClassOvercommitRejects(metrics.getStaticClassOvercommitRejects());
		setDynamicClassExhaustionRejects(metrics.getDynamicClassExhaustionRejects());
		setPinnedLaneConflicts(metrics.getPinnedLaneConflicts());
		setLateBindingConflicts(metrics.getLateBindingConflicts());
		setTypedSlotDomainRejects(metrics.getTypedSlotDomainRejects());
		setSmtOwnerContextGuardRejects(metrics.getSmtOwnerContextGuardRejects());
		setSmtDomainGuardRejects(metrics.getSmtDomainGuardRejects());
		setSmtBoundaryGuardRejects(metrics.getSmtBoundaryGuardRejects());
		setSmtSharedResourceCertificateRejects(metrics.getSmtSharedResourceCertificateRejects());
		setSmtRegisterGroupCertificateRejects(metrics.getSmtRegisterGroupCertificateRejects());
		_lastSmtLegalityRejectKind = metrics.getLastSmtLegalityRejectKind();
		_lastSmtLegalityAuthoritySource = metrics.getLastSmtLegalityAuthoritySource();
		setSmtLegalityRejectByAluClass(metrics.getSmtLegalityRejectByAluClass());
		setSmtLegalityRejectByLsuClass(metrics.getSmtLegalityRejectByLsuClass());
		setSmtLegalityRejectByDmaStreamClass(metrics.getSmtLegalityRejectByDmaStreamClass());
		setSmtLegalityRejectByMatrixTileStreamClass(metrics.getSmtLegalityRejectByMatrixTileStreamClass());
		setSmtLegalityRejectByBranchControl(metrics.getSmtLegalityRejectByBranchControl());
		setSmtLegalityRejectBySystemSingleton(metrics.getSmtLegalityRejectBySystemSingleton());
		setClassTemplateDomainInvalidations(metrics.getClassTemplateDomainInvalidations());
		setClassTemplateCapacityMismatchInvalidations(metrics.getClassTemplateCapacityMismatchInvalidations());
		setCertificateRejectByAluClass(metrics.getCertificateRejectByAluClass());
		setCertificateRejectByLsuClass(metrics.getCertificateRejectByLsuClass());
		setCertificateRejectByDmaStreamClass(metrics.getCertificateRejectByDmaStreamClass());
		setCertificateRejectByMatrixTileStreamClass(metrics.getCertificateRejectByMatrixTileStreamClass());
		setCertificateRejectByBranchControl(metrics.getCertificateRejectByBranchControl());
		setCertificateRejectBySystemSingleton(metrics.getCertificateRejectBySystemSingleton());
		setRejectionsVT0(metrics.getRejectionsVT0());
		setRejectionsVT1(metrics.getRejectionsVT1());
		setRejectionsVT2(metrics.getRejectionsVT2());
		setRejectionsVT3(metrics.getRejectionsVT3());
		setRegGroupConflictsVT0(metrics.getRegGroupConflictsVT0() != 0 ? metrics.getRegGroupConflictsVT0() : metrics.getCertificateRegGroupConflictVT0());
		setRegGroupConflictsVT1(metrics.getRegGroupConflictsVT1() != 0 ? metrics.getRegGroupConflictsVT1() : metrics.getCertificateRegGroupConflictVT1());
		setRegGroupConflictsVT2(metrics.getRegGroupConflictsVT2() != 0 ? metrics.getRegGroupConflictsVT2() : metrics.getCertificateRegGroupConflictVT2());
		setRegGroupConflictsVT3(metrics.getRegGroupConflictsVT3() != 0 ? metrics.getRegGroupConflictsVT3() : metrics.getCertificateRegGroupConflictVT3());
		setBankPendingRejectBank0(metrics.getBankPendingRejectBank0());
		setBankPendingRejectBank1(metrics.getBankPendingRejectBank1());
		setBankPendingRejectBank2(metrics.getBankPendingRejectBank2());
		setBankPendingRejectBank3(metrics.getBankPendingRejectBank3());
		setBankPendingRejectBank4(metrics.getBankPendingRejectBank4());
		setBankPendingRejectBank5(metrics.getBankPendingRejectBank5());
		setBankPendingRejectBank6(metrics.getBankPendingRejectBank6());
		setBankPendingRejectBank7(metrics.getBankPendingRejectBank7());
		setBankPendingRejectBank8(metrics.getBankPendingRejectBank8());
		setBankPendingRejectBank9(metrics.getBankPendingRejectBank9());
		setBankPendingRejectBank10(metrics.getBankPendingRejectBank10());
		setBankPendingRejectBank11(metrics.getBankPendingRejectBank11());
		setBankPendingRejectBank12(metrics.getBankPendingRejectBank12());
		setBankPendingRejectBank13(metrics.getBankPendingRejectBank13());
		setBankPendingRejectBank14(metrics.getBankPendingRejectBank14());
		setBankPendingRejectBank15(metrics.getBankPendingRejectBank15());
		setMemoryClusteringEvents(metrics.getMemoryClusteringEvents());
		setTypedSlotHardwareBudgetRejects(metrics.getTypedSlotHardwareBudgetRejects());
		setTypedSlotAssistQuotaRejects(metrics.getTypedSlotAssistQuotaRejects());
		setTypedSlotAssistBackpressureRejects(metrics.getTypedSlotAssistBackpressureRejects());
		setAssistNominationCount(metrics.getAssistNominations());
		setAssistInjectionsCount(metrics.getAssistInjections());
		setAssistRejects(metrics.getAssistRejects());
		setAssistBoundaryRejects(metrics.getAssistBoundaryRejects());
		setAssistInvalidations(metrics.getAssistInvalidations());
		setAssistInterCoreNominations(metrics.getAssistInterCoreNominations());
		setAssistInterCoreInjections(metrics.getAssistInterCoreInjections());
		setAssistInterCoreRejects(metrics.getAssistInterCoreRejects());
		setAssistInterCoreDomainRejects(metrics.getAssistInterCoreDomainRejects());
		setAssistInterCorePodLocalInjections(metrics.getAssistInterCorePodLocalInjections());
		setAssistInterCoreCrossPodInjections(metrics.getAssistInterCoreCrossPodInjections());
		setAssistInterCorePodLocalRejects(metrics.getAssistInterCorePodLocalRejects());
		setAssistInterCoreCrossPodRejects(metrics.getAssistInterCoreCrossPodRejects());
		setAssistInterCorePodLocalDomainRejects(metrics.getAssistInterCorePodLocalDomainRejects());
		setAssistInterCoreCrossPodDomainRejects(metrics.getAssistInterCoreCrossPodDomainRejects());
		setAssistInterCoreSameVtVectorInjects(metrics.getAssistInterCoreSameVtVectorInjects());
		setAssistInterCoreDonorVtVectorInjects(metrics.getAssistInterCoreDonorVtVectorInjects());
		setAssistInterCoreSameVtVectorWritebackInjects(metrics.getAssistInterCoreSameVtVectorWritebackInjects());
		setAssistInterCoreDonorVtVectorWritebackInjects(metrics.getAssistInterCoreDonorVtVectorWritebackInjects());
			setAssistInterCoreLane6DefaultStoreDonorPrefetchInjects(metrics.getAssistInterCoreLane6DefaultStoreDonorPrefetchInjects());
		setAssistInterCoreLane6HotLoadDonorPrefetchInjects(metrics.getAssistInterCoreLane6HotLoadDonorPrefetchInjects());
		setAssistInterCoreLane6HotStoreDonorPrefetchInjects(metrics.getAssistInterCoreLane6HotStoreDonorPrefetchInjects());
		setAssistInterCoreLane6DonorPrefetchInjects(metrics.getAssistInterCoreLane6DonorPrefetchInjects());
		setAssistInterCoreLane6ColdStoreLdsaInjects(metrics.getAssistInterCoreLane6ColdStoreLdsaInjects());
		setAssistInterCoreLane6LdsaInjects(metrics.getAssistInterCoreLane6LdsaInjects());
		setAssistQuotaRejects(metrics.getAssistQuotaRejects());
		setAssistQuotaIssueRejects(metrics.getAssistQuotaIssueRejects());
		setAssistQuotaLineRejects(metrics.getAssistQuotaLineRejects());
		setAssistQuotaLinesReserved(metrics.getAssistQuotaLinesReserved());
		setAssistBackpressureRejects(metrics.getAssistBackpressureRejects());
		setAssistBackpressureOuterCapRejects(metrics.getAssistBackpressureOuterCapRejects());
		setAssistBackpressureMshrRejects(metrics.getAssistBackpressureMshrRejects());
		setAssistBackpressureDmaSrfRejects(metrics.getAssistBackpressureDmaSrfRejects());
		setAssistDonorPrefetchInjects(metrics.getAssistDonorPrefetchInjects());
		setAssistLdsaInjects(metrics.getAssistLdsaInjects());
		setAssistVdsaInjects(metrics.getAssistVdsaInjects());
		setAssistSameVtInjects(metrics.getAssistSameVtInjects());
		setAssistDonorVtInjects(metrics.getAssistDonorVtInjects());
		setLastAssistInvalidationReason(metrics.getLastAssistInvalidationReason());
		setLastAssistOwnershipSignature(metrics.getLastAssistOwnershipSignature());

		_loopPhaseTrackers.clear();
		_loopPhaseEntryCounts.clear();
		if (!metrics.getLoopPhaseProfiles().empty())
		{
			for (auto profile : metrics.getLoopPhaseProfiles())
			{
				_loopPhaseEntryCounts[profile->LoopPcAddress] = profile->IterationsSampled;
				_loopPhaseTrackers[profile->LoopPcAddress] = std::make_shared<LoopPhaseTracker>(profile->LoopPcAddress, profile->IterationsSampled);
			}
		}
	}

	const std::vector<std::shared_ptr<LoopPhaseClassProfile>> MicroOpScheduler::TestGetLoopPhaseProfiles()
	{
		const std::vector<std::shared_ptr<LoopPhaseClassProfile>> temp_var = BuildLoopPhaseProfiles();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: return BuildLoopPhaseProfiles() ?? Array.Empty<LoopPhaseClassProfile>();
		return temp_var;
	}

	ReplayPhaseContext MicroOpScheduler::TestGetReplayPhaseContext()
	{
		return _currentReplayPhase;
	}

	bool MicroOpScheduler::TestTryClassAdmission(std::shared_ptr<MicroOp> candidate, BundleResourceCertificate4Way cert4Way, std::shared_ptr<std::vector<std::shared_ptr<MicroOp>>> workingBundle, int ownerVirtualThreadId, int currentPassInjections, TypedSlotRejectReason &rejectReason, bool templateHitThisCycle, TemplateBudget templateBudget)
	{
		ClassTemplateAdmissionState templateState = templateHitThisCycle ? ClassTemplateAdmissionState::Create(templateBudget) : YAKSys_Hybrid_CPU::Core::MicroOpScheduler::ClassTemplateAdmissionState();
		SmtBundleMetadata4Way bundleMetadata = SmtBundleMetadata4Way::Empty(ownerVirtualThreadId);
		BoundaryGuardState boundaryGuard = BoundaryGuardState::Open(_serializingEpochCounter);
		for (auto op : ((workingBundle != nullptr) ? *workingBundle : std::vector<std::shared_ptr<MicroOp>>{}))
		{
			bundleMetadata = bundleMetadata.WithOperation(op);
			boundaryGuard = boundaryGuard.WithOperation(op);
		}

		return TryClassAdmission(candidate, cert4Way, bundleMetadata, boundaryGuard, templateState, ownerVirtualThreadId, currentPassInjections, rejectReason);
	}

	bool MicroOpScheduler::TestTryMaterializeLane(std::shared_ptr<MicroOp> candidate, unsigned char bundleOccupancy, int &selectedLane, TypedSlotRejectReason &rejectReason)
	{
		return TryMaterializeLane(candidate, bundleOccupancy, selectedLane, rejectReason);
	}

	unsigned char MicroOpScheduler::TestComputeBundleOccupancy(std::vector<std::shared_ptr<MicroOp>> &bundle)
	{
		return ComputeBundleOccupancy(bundle);
	}

	void MicroOpScheduler::TestSetClassCapacity(SlotClassCapacityState state)
	{
		_classCapacity = state;
	}

	int MicroOpScheduler::TestGetPreviousPhaseLane(SlotClass slotClass)
	{
		return GetPreviousPhaseLane(slotClass);
	}

	void MicroOpScheduler::TestSetPreviousPhaseLane(SlotClass slotClass, int lane)
	{
		RecordPhaseLane(slotClass, lane);
	}

	void MicroOpScheduler::TestResetPreviousPhaseLanes()
	{
		ResetPreviousPhaseLanes();
	}

	CertificateRejectDetail MicroOpScheduler::TestGetLastCertificateRejectDetail()
	{
		return _lastCertificateRejectDetail;
	}

	TypedSlotRejectClassification MicroOpScheduler::TestGetLastRejectClassification()
	{
		return _lastRejectClassification;
	}

	std::shared_ptr<MicroOpScheduler::SmtEligibilitySnapshot> MicroOpScheduler::TestGetLastSmtEligibilitySnapshot()
	{
		return _lastSmtEligibilitySnapshot;
	}

	ClassCapacityTemplate MicroOpScheduler::TestGetClassCapacityTemplate()
	{
		return _classCapacityTemplate;
	}

	void MicroOpScheduler::TestSetClassCapacityTemplate(ClassCapacityTemplate template_Keyword)
	{
		_classCapacityTemplate = template_Keyword;
	}

	bool MicroOpScheduler::TestGetClassTemplateValid()
	{
		return _classTemplateValid;
	}

	void MicroOpScheduler::TestSetClassTemplateValid(bool valid)
	{
		_classTemplateValid = valid;
	}

	int MicroOpScheduler::TestGetClassTemplateDomainId()
	{
		return _classTemplateDomainId;
	}

	void MicroOpScheduler::TestSetClassTemplateDomainId(int domainId)
	{
		_classTemplateDomainId = domainId;
	}

	bool MicroOpScheduler::TestTryPrepareClassTemplateAdmissionState(int currentDomainId, TemplateBudget &templateBudget)
	{
		ClassTemplateAdmissionState templateState = PrepareClassTemplateAdmissionState(currentDomainId);
		templateBudget = templateState.getTemplateBudgetSnapshot();
		return templateState.getTemplateHit();
	}

	void MicroOpScheduler::TestRecordTypedSlotInject(std::shared_ptr<MicroOp> candidate, int lane)
	{
		RecordTypedSlotInject(candidate, lane);
	}

	void MicroOpScheduler::TestRecordTypedSlotReject(TypedSlotRejectReason reason)
	{
		if (reason == TypedSlotRejectReason::BankPendingReject)
		{
			RecordBankPendingReject(-1);
			return;
		}

		if (reason == TypedSlotRejectReason::HardwareBudgetReject)
		{
			setTypedSlotHardwareBudgetRejects(getTypedSlotHardwareBudgetRejects() + 1);
			setSmtRejectionsCount(getSmtRejectionsCount() + 1);
			setNopDueToDynamicState(getNopDueToDynamicState() + 1);
			return;
		}

		RecordTypedSlotReject(reason);
	}

	void MicroOpScheduler::TestRecordBankPendingReject(int memoryBankId)
	{
		RecordBankPendingReject(memoryBankId);
	}

	void MicroOpScheduler::TestRecordMemoryClusteringEvent()
	{
		RecordMemoryClusteringEvent();
	}

	void MicroOpScheduler::TestRecordTypedSlotReject(TypedSlotRejectReason reason, std::shared_ptr<MicroOp> candidate, CertificateRejectDetail certificateRejectDetail)
	{
		_lastCertificateRejectDetail = certificateRejectDetail;
		RecordTypedSlotReject(reason, candidate);
	}

	void MicroOpScheduler::TestInvalidateClassTemplate(ReplayPhaseInvalidationReason reason)
	{
		InvalidateClassTemplate(reason);
	}

	bool MicroOpScheduler::HasNominatedCandidate(int coreId)
	{
		if (static_cast<unsigned int>(coreId) >= NUM_PORTS)
		{
			return false;
		}
		return CaptureInterCoreNominationSnapshot().IsReadyCandidate(coreId);
	}

	inline bool MicroOpScheduler::TryClassAdmission(std::shared_ptr<MicroOp> candidate, BundleResourceCertificate4Way &cert4Way, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard, ClassTemplateAdmissionState &templateState, int ownerVirtualThreadId, int currentPassInjections, TypedSlotRejectReason &rejectReason)
	{
		rejectReason = TypedSlotRejectReason::None;
		_lastCertificateRejectDetail = CertificateRejectDetail::None;
		_lastSmtLegalityRejectKind = RejectKind::None;
		_lastSmtLegalityAuthoritySource = LegalityAuthoritySource::StructuralCertificate;

		// A1. Class-capacity (Phase 02 data).
		// Phase 07: Template-guided fast path — check remaining template budget first.
		// If template budget has remaining capacity for this class, skip full HasFreeCapacity check.
		// Unclassified ops bypass class-capacity (legacy path: no typed lanes).
		if (candidate->getPlacement().RequiredSlotClass != SlotClass::Unclassified)
		{
			bool capacityOk = false;
			if (templateState.TryConsumeFastPathBudget(candidate->getPlacement().RequiredSlotClass))
			{
				setTypedSlotFastPathAccepts(getTypedSlotFastPathAccepts() + 1);
				capacityOk = true;
			}

			if (!capacityOk && !_classCapacity.HasFreeCapacity(candidate->getPlacement().RequiredSlotClass))
			{
				rejectReason = currentPassInjections == 0 ? TypedSlotRejectReason::StaticClassOvercommit : TypedSlotRejectReason::DynamicClassExhaustion;
				return false;
			}
		}

		// A2. Legality service decision.
		// Phase 05: scheduler now consumes an explicit LegalityDecision from the
		// runtime-local legality service instead of interpreting certificate/mask-shaped
		// legality artifacts directly.
		LegalityDecision legalityDecision = EvaluateSmtLegality(cert4Way, bundleMetadata, boundaryGuard, candidate);
		_lastCertificateRejectDetail = legalityDecision.getCertificateDetail();
		_lastSmtLegalityRejectKind = legalityDecision.getRejectKind();
		_lastSmtLegalityAuthoritySource = legalityDecision.getAuthoritySource();
		if (!legalityDecision.isAllowed())
		{
			// Scheduler-visible typed-slot taxonomy intentionally collapses
			// checker-owned guard/certificate denials to ResourceConflict.
			// Fine-grained cause remains in _lastSmtLegalityRejectKind and
			// _lastSmtLegalityAuthoritySource for diagnostics.
			rejectReason = TypedSlotRejectReason::ResourceConflict;
			return false;
		}

		// A3. IsOuterCapBlocking — consolidates SuppressLsu, projected scoreboard
		// saturation, bank-pending, sampled memory budget, and speculation budget.
		if (std::dynamic_pointer_cast<AssistMicroOp>(candidate) == nullptr)
		{
			std::shared_ptr<LoadStoreMicroOp> ls = std::dynamic_pointer_cast<LoadStoreMicroOp>(candidate);
			int candidateBankId = ls != nullptr ? ls->getMemoryBankId() : -1;
			if (!TryPassOuterCap(candidate, candidate->getVirtualThreadId(), candidate->isMemoryOp(), candidateBankId, ownerVirtualThreadId, rejectReason))
			{
				return false;
			}
		}

		return true;
	}

	inline bool MicroOpScheduler::TryMaterializeLane(std::shared_ptr<MicroOp> candidate, unsigned char bundleOccupancy, int &selectedLane, TypedSlotRejectReason &rejectReason, LaneMaterializationRole role, unsigned char permittedLanes)
	{
		rejectReason = TypedSlotRejectReason::None;
		selectedLane = -1;

		if (candidate->getPlacement().PinningKind == SlotPinningKind::HardPinned)
		{
			LaneId laneId;
			if (!LaneId::TryCreate(candidate->getPlacement().PinnedLaneId, laneId))
			{
				rejectReason = TypedSlotRejectReason::InvalidPinnedLane;
				return false;
			}

			int lane = laneId.getValue();
			if ((bundleOccupancy & (1 << lane)) != 0 || (permittedLanes & (1 << lane)) == 0)
			{
				rejectReason = TypedSlotRejectReason::PinnedLaneConflict;
				return false;
			}
			selectedLane = lane;
			return true;
		}

		// ClassFlexible: intersect class lanes with free lanes (and replay mask).
		unsigned char classMask = SlotClassLaneMap::GetLaneMask(candidate->getPlacement().RequiredSlotClass);
		unsigned char freeLanes = static_cast<unsigned char>(classMask & ~bundleOccupancy & permittedLanes);

		// Donor structure constrains opportunistic injection, never the
		// bundle owner's original operation. Both use the same lane owner.
		if (role == LaneMaterializationRole::Opportunistic && _currentReplayPhase.getHasStableDonorStructure())
		{
			freeLanes &= _currentReplayPhase.getStableDonorMask();
		}

		if (freeLanes == 0)
		{
			rejectReason = TypedSlotRejectReason::LateBindingConflict;
			return false;
		}

		// Phase 04: Deterministic lane selection with replay hint.
		selectedLane = DeterministicLaneChooser::SelectWithReplayHint(freeLanes, _currentReplayPhase.isActive(), GetPreviousPhaseLane(candidate->getPlacement().RequiredSlotClass));

		// Phase 04: Tier 2 telemetry tracking.
		if (_currentReplayPhase.isActive())
		{
			int prevLane = GetPreviousPhaseLane(candidate->getPlacement().RequiredSlotClass);
			if (prevLane >= 0 && selectedLane == prevLane)
			{
				setLaneReuseHits(getLaneReuseHits() + 1);
			}
			else if (prevLane >= 0)
			{
				setLaneReuseMisses(getLaneReuseMisses() + 1);
			}
		}

		return true;
	}

	inline unsigned char MicroOpScheduler::GetReplayAwareStealMask(unsigned char stealMask, unsigned char donorMask)
	{
		if (!_currentReplayPhase.getHasStableDonorStructure())
		{
			return stealMask;
		}

		unsigned char stableDonorMask = _currentReplayPhase.getStableDonorMask();
		if (donorMask != 0)
		{
			stableDonorMask &= donorMask;
		}

		return static_cast<unsigned char>(stealMask & stableDonorMask);
	}

	inline int MicroOpScheduler::GetRankedVirtualThread(int scanIndex, bool forceTdm)
	{
		return forceTdm ? ((scanIndex + 1) % SMT_WAYS) : scanIndex;
	}

	inline void MicroOpScheduler::GetCreditRankedOrder(int ownerVirtualThreadId, std::span<int> ranked)
	{
		// Simple insertion sort on 4 elements — deterministic, HLS-friendly.
		int count = 0;
		for (int vt = 0; vt < SMT_WAYS; vt++)
		{
			if (vt == ownerVirtualThreadId)
			{
				continue;
			}
			ranked[count++] = vt;
		}

		// Sort by descending credit, ascending vtId on tie.
		for (int i = 0; i < count - 1; i++)
		{
			for (int j = i + 1; j < count; j++)
			{
				int a = ranked[i];
				int b = ranked[j];
				if (_fairnessCredits[b] > _fairnessCredits[a] || (_fairnessCredits[b] == _fairnessCredits[a] && b < a))
				{
					ranked[i] = b;
					ranked[j] = a;
				}
			}
		}
	}

	inline void MicroOpScheduler::AccumulateFairnessCredits(SmtNominationState nominationState, int ownerVirtualThreadId, std::span<const bool> wasInjected)
	{
		for (int vt = 0; vt < SMT_WAYS; vt++)
		{
			if (vt == ownerVirtualThreadId)
			{
				continue;
			}

			if (wasInjected[vt])
			{
				// Spend one credit on successful injection
				if (_fairnessCredits[vt] > 0)
				{
					_fairnessCredits[vt]--;
				}
			}
			else if (nominationState.IsReadyCandidate(vt))
			{
				// VT had a valid candidate but was not injected — accumulate credit
				if (_fairnessCredits[vt] < FAIRNESS_CREDIT_CAP)
				{
					_fairnessCredits[vt]++;
				}

				if (_fairnessCredits[vt] >= FAIRNESS_CREDIT_CAP)
				{
					setFairnessStarvationEvents(getFairnessStarvationEvents() + 1);
				}
			}
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private void PrepareProjectedMemoryIssuePass(System.Collections.Generic.IReadOnlyList<MicroOp?> bundle)
	inline void MicroOpScheduler::PrepareProjectedMemoryIssuePass(const std::vector<std::shared_ptr<MicroOp>> &bundle)
	{
	_remainingHardwareMemoryIssueBudget = _hardwareOccupancySnapshot.getMemoryIssueBudget();
	_consumedHardwareMemoryBudgetByBank = 0;
	_remainingHardwareLoadIssueBudget = _hardwareOccupancySnapshot.getReadIssueBudget();
	_remainingHardwareStoreIssueBudget = _hardwareOccupancySnapshot.getWriteIssueBudget();
	_consumedHardwareLoadBudgetByBank = 0;
	_consumedHardwareStoreBudgetByBank = 0;
	_bundleLocalOutstandingStoreBankMask = 0;
	InitializeProjectedOutstandingMemoryState();

	if (bundle.empty())
	{
		return;
	}

		for (int i = 0; i < bundle.size(); i++)
		{
			ConsumeProjectedMemoryIssueStateIfNeeded(bundle[i]);
		}
	}

	inline void MicroOpScheduler::InitializeProjectedOutstandingMemoryState()
	{
		for (int vt = 0; vt < SMT_WAYS; vt++)
		{
			int liveOutstandingMemoryCount = GetOutstandingMemoryCount(vt);
			int liveFreeScoreboardSlots = GetFreeSmtScoreboardSlotCount(vt);

			_projectedOutstandingMemoryCountByVt[vt] = SaturateScoreboardCount(liveOutstandingMemoryCount);
			_projectedOutstandingMemoryCapacityByVt[vt] = SaturateScoreboardCount(liveOutstandingMemoryCount + liveFreeScoreboardSlots);
		}
	}

	inline bool MicroOpScheduler::HasProjectedOutstandingMemoryCapacity(bool isMemoryOp, int virtualThreadId)
	{
		if (!isMemoryOp)
		{
			return true;
		}

		if (static_cast<unsigned int>(virtualThreadId) >= SMT_WAYS)
		{
			return false;
		}

		return _projectedOutstandingMemoryCountByVt[virtualThreadId] < _projectedOutstandingMemoryCapacityByVt[virtualThreadId];
	}

	inline bool MicroOpScheduler::HasHardwareMemoryBudget(std::shared_ptr<MicroOp> candidate, int memoryBankId)
	{
		if (std::dynamic_pointer_cast<LoadStoreMicroOp>(candidate) == nullptr)
		{
			return true;
		}

		return HasSharedHardwareMemoryBudget(memoryBankId) && HasDirectionalHardwareMemoryBudget(candidate, memoryBankId);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private void ConsumeProjectedMemoryIssueStateIfNeeded(MicroOp? candidate)
	inline void MicroOpScheduler::ConsumeProjectedMemoryIssueStateIfNeeded(std::shared_ptr<MicroOp> candidate)
	{
		std::shared_ptr<LoadStoreMicroOp> loadStoreCandidate = std::dynamic_pointer_cast<LoadStoreMicroOp>(candidate);
		if (loadStoreCandidate == nullptr)
		{
			return;
		}

		IncrementProjectedOutstandingMemoryCount(_projectedOutstandingMemoryCountByVt, loadStoreCandidate->getVirtualThreadId());

		if (_remainingHardwareMemoryIssueBudget > 0)
		{
			_remainingHardwareMemoryIssueBudget--;
		}

		IncrementConsumedHardwareMemoryBudget(loadStoreCandidate->getMemoryBankId());

		if (std::dynamic_pointer_cast<LoadMicroOp>(loadStoreCandidate) != nullptr)
		{
			if (_remainingHardwareLoadIssueBudget > 0)
			{
				_remainingHardwareLoadIssueBudget--;
			}

			IncrementConsumedHardwareLoadBudget(loadStoreCandidate->getMemoryBankId());
		}
		else
		{
			std::shared_ptr<StoreMicroOp> storeCandidate = std::dynamic_pointer_cast<StoreMicroOp>(loadStoreCandidate);
			if (storeCandidate != nullptr)
			{
				if (_remainingHardwareStoreIssueBudget > 0)
				{
					_remainingHardwareStoreIssueBudget--;
				}
        
				IncrementConsumedHardwareStoreBudget(storeCandidate->getMemoryBankId());
				MarkBundleLocalOutstandingStoreBank(storeCandidate->getMemoryBankId());
			}
		}
	}

	inline bool MicroOpScheduler::HasSharedHardwareMemoryBudget(int memoryBankId)
	{
		if (_remainingHardwareMemoryIssueBudget == 0)
		{
			return false;
		}

		if (static_cast<unsigned int>(memoryBankId) >= 16)
		{
			return true;
		}

		return GetConsumedHardwareMemoryBudget(memoryBankId) < _hardwareOccupancySnapshot.GetMemoryBudgetForBank(memoryBankId);
	}

	inline bool MicroOpScheduler::HasDirectionalHardwareMemoryBudget(std::shared_ptr<MicroOp> candidate, int memoryBankId)
	{
		if (std::dynamic_pointer_cast<LoadMicroOp>(candidate) != nullptr)
		{
			return HasDirectionalHardwareBudget(_remainingHardwareLoadIssueBudget, _consumedHardwareLoadBudgetByBank, _hardwareOccupancySnapshot.GetReadBudgetForBank(memoryBankId), memoryBankId);
		}

		if (std::dynamic_pointer_cast<StoreMicroOp>(candidate) != nullptr)
		{
			return HasDirectionalHardwareBudget(_remainingHardwareStoreIssueBudget, _consumedHardwareStoreBudgetByBank, _hardwareOccupancySnapshot.GetWriteBudgetForBank(memoryBankId), memoryBankId);
		}

		return true;
	}

	inline bool MicroOpScheduler::HasDirectionalHardwareBudget(unsigned char remainingDirectionalIssueBudget, unsigned int consumedDirectionalBudgetByBank, unsigned char bankBudget, int memoryBankId)
	{
		if (remainingDirectionalIssueBudget == 0)
		{
			return false;
		}

		if (static_cast<unsigned int>(memoryBankId) >= 16)
		{
			return true;
		}

		return GetPackedConsumedHardwareBudget(consumedDirectionalBudgetByBank, memoryBankId) < bankBudget;
	}

	inline unsigned char MicroOpScheduler::SaturateScoreboardCount(int value)
	{
		if (value <= 0)
		{
			return 0;
		}

		if (value >= SCOREBOARD_SLOTS)
		{
			return SCOREBOARD_SLOTS;
		}

		return static_cast<unsigned char>(value);
	}

	inline void MicroOpScheduler::IncrementProjectedOutstandingMemoryCount(std::vector<unsigned char> &projectedOutstandingCounts, int virtualThreadId)
	{
		if (static_cast<unsigned int>(virtualThreadId) >= SMT_WAYS)
		{
			return;
		}

		if (projectedOutstandingCounts[virtualThreadId] < SCOREBOARD_SLOTS)
		{
			projectedOutstandingCounts[virtualThreadId]++;
		}
	}

	inline bool MicroOpScheduler::HasBundleLocalOutstandingStoreBankPending(int bankId)
	{
		return static_cast<unsigned int>(bankId) < 16 && (_bundleLocalOutstandingStoreBankMask & (1 << bankId)) != 0;
	}

	inline void MicroOpScheduler::MarkBundleLocalOutstandingStoreBank(int bankId)
	{
		if (static_cast<unsigned int>(bankId) >= 16)
		{
			return;
		}

		_bundleLocalOutstandingStoreBankMask |= static_cast<unsigned short>(1 << bankId);
	}

	inline unsigned char MicroOpScheduler::GetConsumedHardwareMemoryBudget(int bankId)
	{
		return GetPackedConsumedHardwareBudget(_consumedHardwareMemoryBudgetByBank, bankId);
	}

	inline void MicroOpScheduler::IncrementConsumedHardwareMemoryBudget(int bankId)
	{
		_consumedHardwareMemoryBudgetByBank = IncrementPackedConsumedHardwareBudget(_consumedHardwareMemoryBudgetByBank, bankId);
	}

	inline void MicroOpScheduler::IncrementConsumedHardwareLoadBudget(int bankId)
	{
		_consumedHardwareLoadBudgetByBank = IncrementPackedConsumedHardwareBudget(_consumedHardwareLoadBudgetByBank, bankId);
	}

	inline void MicroOpScheduler::IncrementConsumedHardwareStoreBudget(int bankId)
	{
		_consumedHardwareStoreBudgetByBank = IncrementPackedConsumedHardwareBudget(_consumedHardwareStoreBudgetByBank, bankId);
	}

	inline unsigned char MicroOpScheduler::GetPackedConsumedHardwareBudget(unsigned int consumedPackedBudgetByBank, int bankId)
	{
		if (static_cast<unsigned int>(bankId) >= 16)
		{
			return 0;
		}

		int shift = bankId * 2;
		return static_cast<unsigned char>((consumedPackedBudgetByBank >> shift) & 0x3u);
	}

	inline unsigned int MicroOpScheduler::IncrementPackedConsumedHardwareBudget(unsigned int consumedPackedBudgetByBank, int bankId)
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

	inline int MicroOpScheduler::GetBankPressureScore(int bankId)
	{
		if (bankId < 0)
		{
			return 0;
		}

		int score = 0;
		for (int vt = 0; vt < SMT_WAYS; vt++)
		{
			for (int s = 0; s < SCOREBOARD_SLOTS; s++)
			{
				if (_smtScoreboard[vt][s] != -1 && _smtScoreboardBankId[vt][s] == bankId && (_smtScoreboardType[vt][s] == ScoreboardEntryType::OutstandingLoad || _smtScoreboardType[vt][s] == ScoreboardEntryType::OutstandingStore))
				{
					score++;
				}
			}
		}

		return score;
	}

	inline bool MicroOpScheduler::TryPassOuterCap(std::shared_ptr<MicroOp> candidate, int virtualThreadId, bool isMemoryOp, int memoryBankId, int bundleOwnerVtId, TypedSlotRejectReason &rejectReason)
	{
		rejectReason = TypedSlotRejectReason::None;

		if (getSuppressLsu() && isMemoryOp)
		{
			setMemoryWallSuppressionsCount(getMemoryWallSuppressionsCount() + 1);
			setSmtRejectionsCount(getSmtRejectionsCount() + 1);
			rejectReason = TypedSlotRejectReason::ScoreboardReject;
			return false;
		}

		if (!HasProjectedOutstandingMemoryCapacity(isMemoryOp, virtualThreadId))
		{
			setMshrScoreboardStalls(getMshrScoreboardStalls() + 1);
			setSmtRejectionsCount(getSmtRejectionsCount() + 1);
			rejectReason = TypedSlotRejectReason::ScoreboardReject;
			return false;
		}

		if (isMemoryOp && memoryBankId >= 0 && IsBankPendingForVT(memoryBankId, virtualThreadId))
		{
			RecordBankPendingReject(memoryBankId);
			setMshrScoreboardStalls(getMshrScoreboardStalls() + 1);
			setSmtRejectionsCount(getSmtRejectionsCount() + 1);
			rejectReason = TypedSlotRejectReason::BankPendingReject;
			return false;
		}

		if (std::dynamic_pointer_cast<StoreMicroOp>(candidate) != nullptr && memoryBankId >= 0 && HasBundleLocalOutstandingStoreBankPending(memoryBankId))
		{
			RecordBankPendingReject(memoryBankId);
			setMshrScoreboardStalls(getMshrScoreboardStalls() + 1);
			setSmtRejectionsCount(getSmtRejectionsCount() + 1);
			rejectReason = TypedSlotRejectReason::BankPendingReject;
			return false;
		}

		if (!HasHardwareMemoryBudget(candidate, memoryBankId))
		{
			setTypedSlotHardwareBudgetRejects(getTypedSlotHardwareBudgetRejects() + 1);
			setSmtRejectionsCount(getSmtRejectionsCount() + 1);
			rejectReason = TypedSlotRejectReason::HardwareBudgetReject;
			return false;
		}

		// Phase 2C: Speculation budget enforcement for cross-thread memory ops.
		// A candidate is speculative when it comes from a different VT than the bundle owner.
		if (getSpeculationBudgetEnabled() && isMemoryOp && bundleOwnerVtId >= 0 && virtualThreadId != bundleOwnerVtId)
		{
			if (_speculationBudget <= 0)
			{
				setSpeculationBudgetExhaustionEvents(getSpeculationBudgetExhaustionEvents() + 1);
				setSmtRejectionsCount(getSmtRejectionsCount() + 1);
				rejectReason = TypedSlotRejectReason::SpeculationBudgetReject;
				return false;
			}
		}

		return true;
	}

	inline bool MicroOpScheduler::IsSmtBundleBlockedByBoundaryGuard(BundleResourceCertificate4Way bundleCert, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard)
	{
		LegalityDecision boundaryDecision = _runtimeLegalityService->EvaluateSmtBoundaryGuard(_currentReplayPhase, bundleCert, bundleMetadata, boundaryGuard);
		if (boundaryDecision.isAllowed())
		{
			return false;
		}

		if (boundaryDecision.getRejectKind() == RejectKind::Boundary)
		{
			setSerializingBoundaryRejects(getSerializingBoundaryRejects() + 1);
			setSmtBoundaryGuardRejects(getSmtBoundaryGuardRejects() + 1);
		}

		_lastCertificateRejectDetail = boundaryDecision.getCertificateDetail();
		_lastSmtLegalityRejectKind = boundaryDecision.getRejectKind();
		_lastSmtLegalityAuthoritySource = boundaryDecision.getAuthoritySource();
		return true;
	}

	inline LegalityDecision MicroOpScheduler::EvaluateSmtLegality(BundleResourceCertificate4Way bundleCert, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard, std::shared_ptr<MicroOp> candidate)
	{
		return _runtimeLegalityService->EvaluateSmtLegality(_currentReplayPhase, bundleCert, bundleMetadata, boundaryGuard, candidate);
	}

	inline void MicroOpScheduler::InvalidatePhaseCertificateTemplates(ReplayPhaseInvalidationReason reason, bool invalidateInterCore, bool invalidateFourWay)
	{
		_runtimeLegalityService->Invalidate(reason, invalidateInterCore, invalidateFourWay);
	}

	bool MicroOpScheduler::TryInvalidatePhaseCertificateTemplatesForMemoryFootprintOverlap(std::shared_ptr<MicroOp> writer, std::shared_ptr<MicroOp> replayEvidence)
	{
		ReplayPhaseInvalidationReason reason;
		if (!SafetyVerifier::TryClassifyMemoryFootprintInvalidation(writer, replayEvidence, reason))
		{
			return false;
		}

		InvalidatePhaseCertificateTemplates(reason);
		return true;
	}

	bool MicroOpScheduler::TryIssueForegroundScalar(std::shared_ptr<MicroOp> candidate, std::shared_ptr<PostStageBIdentityTemplate> template_Keyword, TypedSlotRejectReason &rejection)
	{
		ArgumentNullException::ThrowIfNull(candidate);
		ArgumentNullException::ThrowIfNull(template_Keyword);
		if (candidate->getPostStageBIssuedAttempt() != nullptr || candidate->getPostStageBIdentityTemplate() != template_Keyword || candidate->getVirtualThreadId() != template_Keyword->getAdmission()->VirtualThreadId || candidate->getOwnerContextId() != template_Keyword->getAdmission()->OwnerContextId || !Rf06ScalarSchedulerRouting::HasEquivalentCarrierFacts(template_Keyword->getAdmission()->Contract, candidate, template_Keyword->getAdmission()->DomainTag))
		{
			throw InvalidOperationException(L"Foreground admission requires its original unissued source-preserving carrier.");
		}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MicroOp?[] empty = new MicroOp?[BundleMetadata.BundleSlotCount];
		std::vector<std::shared_ptr<MicroOp>> empty(BundleSlotCount);
		auto previousCapacity = _classCapacity;
		struct CapacityRestore { SlotClassCapacityState &target; SlotClassCapacityState previous; ~CapacityRestore() { target = previous; } } capacityRestore{_classCapacity, previousCapacity};
		{
			// Reference execution issues one operation. Existing pipeline RAW/WAW
			// interlocks run before this entry; dynamic outer caps remain Stage A.
			_classCapacity = SlotClassCapacity::ComputeFromBundle(empty);
			BundleResourceCertificate4Way certificate = BundleResourceCertificate4Way::Empty;
			SmtBundleMetadata4Way metadata = SmtBundleMetadata4Way::Empty(candidate->getVirtualThreadId()).WithOperation(candidate);
			BoundaryGuardState boundary = BoundaryGuardState::Open(_serializingEpochCounter);
			_runtimeLegalityService->PrepareSmt(_currentReplayPhase, certificate, metadata, boundary);
			ClassTemplateAdmissionState templateState = YAKSys_Hybrid_CPU::Core::MicroOpScheduler::ClassTemplateAdmissionState();
			if (!TryClassAdmission(candidate, certificate, metadata, boundary, templateState, candidate->getVirtualThreadId(), 0, rejection))
			{
				RecordTypedSlotReject(rejection, candidate);
				return false;
			}
			int lane;
			if (!TryMaterializeLane(candidate, 0, lane, rejection, LaneMaterializationRole::Foreground))
			{
				RecordTypedSlotReject(rejection, candidate);
				return false;
			}

			// Sole mint remains the existing scheduler successful Stage-B arm.
			MaterializePostStageBIssuedAttempt(candidate, template_Keyword, lane, metadata);
			return true;
		}
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal bool TryIssuePacketOwnerScalars(BundleIssuePacket packet, IReadOnlyList<WorkingBundleEntry> selected, byte executableMask, int ownerVt, byte currentSlot, out TypedSlotRejectReason rejection, out byte issuedWorkingMask)
	bool MicroOpScheduler::TryIssuePacketOwnerScalars(BundleIssuePacket packet, const std::vector<std::shared_ptr<WorkingBundleEntry>> &selected, unsigned char executableMask, int ownerVt, unsigned char currentSlot, TypedSlotRejectReason &rejection, unsigned char &issuedWorkingMask)
	{
		rejection = TypedSlotRejectReason::None;
		issuedWorkingMask = 0;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: var reserved = new MicroOp?[BundleMetadata.BundleSlotCount];
		auto reserved = std::vector<std::shared_ptr<MicroOp>>(BundleSlotCount);
		unsigned char occupied = 0;
		for (unsigned char lane = 0; lane < reserved.size(); lane++)
		{
			auto planned = packet.GetPhysicalLane(lane);
			if ((executableMask & (1 << lane)) == 0)
			{
				continue;
			}
			if (planned.getPhysicalLaneIndex() != lane || !planned.isOccupied() || planned.getMicroOp() == nullptr)
			{
				throw InvalidOperationException(L"Malformed original packet placement request.");
			}
			if (std::any_of(selected.begin(), selected.end(), [&planned](const auto &entry) { return entry->getCarrier() == planned.getMicroOp(); }))
			{
				continue;
			}
			reserved[lane] = planned.getMicroOp();
			occupied |= static_cast<unsigned char>(1 << lane);
		}
		auto previousCapacity = _classCapacity;
		struct CapacityRestore { SlotClassCapacityState &target; SlotClassCapacityState previous; ~CapacityRestore() { target = previous; } } capacityRestore{_classCapacity, previousCapacity};
		{
			_classCapacity = SlotClassCapacity::ComputeFromBundle(reserved);
			auto owner = selected[0]->getAdmission();
			if (owner->VirtualThreadId != ownerVt)
			{
				throw InvalidOperationException(L"Original packet owner required.");
			}
			BundleResourceCertificate4Way certificate = BundleResourceCertificate4Way::Empty;
			SmtBundleMetadata4Way metadata = SmtBundleMetadata4Way(ownerVt, owner->OwnerContextId, owner->DomainTag, 0, 0, 0);
			BoundaryGuardState boundary = BoundaryGuardState::Open(_serializingEpochCounter);
			for (auto op : reserved)
			{
				if (op != nullptr)
				{
				certificate.AddOperation(op);
				metadata = metadata.WithOperation(op);
				boundary = boundary.WithOperation(op);
				}
			}
			_runtimeLegalityService->PrepareSmt(_currentReplayPhase, certificate, metadata, boundary);
			ClassTemplateAdmissionState templateState = YAKSys_Hybrid_CPU::Core::MicroOpScheduler::ClassTemplateAdmissionState();
			// Source refresh may introduce an unconsumed lower source slot.
			// Original progress retains its current cursor; use that same cyclic
			// order so a partial grant contains the current owner operation.
			auto orderedSelected = selected;
			std::stable_sort(orderedSelected.begin(), orderedSelected.end(), [currentSlot](const auto &left, const auto &right)
			{
			const int leftOrder = (static_cast<int>(left->getWorkingSlotId().getValue()) - static_cast<int>(currentSlot) + BundleSlotCount) % BundleSlotCount;
			const int rightOrder = (static_cast<int>(right->getWorkingSlotId().getValue()) - static_cast<int>(currentSlot) + BundleSlotCount) % BundleSlotCount;
				return leftOrder < rightOrder;
			});
			for (const auto &entry : orderedSelected)
			{
				auto op = entry->getCarrier();
				auto template_Keyword = op->getPostStageBIdentityTemplate();
				if (op->getPostStageBIssuedAttempt() != nullptr || template_Keyword == nullptr || template_Keyword->getAdmission() != entry->getAdmission() || op->getVirtualThreadId() != ownerVt || op->getOwnerContextId() != entry->getAdmission()->OwnerContextId || !Rf06ScalarSchedulerRouting::HasEquivalentCarrierFacts(entry->getAdmission()->Contract, op, entry->getAdmission()->DomainTag))
				{
					throw InvalidOperationException(L"Packet lost original unissued working facts.");
				}
				std::vector<unsigned char> requested;
				for (int laneIndex = 0; laneIndex < BundleSlotCount; ++laneIndex)
				{
					if ((executableMask & (1 << laneIndex)) != 0 && packet.GetPhysicalLane(static_cast<unsigned char>(laneIndex)).getMicroOp() == op)
					{
						requested.push_back(static_cast<unsigned char>(laneIndex));
					}
				}
				if (requested.size() != 1)
				{
					throw InvalidOperationException(L"One original working entry requires one packet lane.");
				}
				if (!TryClassAdmission(op, certificate, metadata, boundary, templateState, ownerVt, 0, rejection))
				{
					RecordTypedSlotReject(rejection, op);
					break;
				}
				int lane;
				if (!TryMaterializeLane(op, occupied, lane, rejection, LaneMaterializationRole::Foreground, static_cast<unsigned char>(1 << requested[0])))
				{
					RecordTypedSlotReject(rejection, op);
					break;
				}
				MaterializePostStageBIssuedAttempt(op, template_Keyword, lane, metadata);
				issuedWorkingMask |= static_cast<unsigned char>(1 << entry->getWorkingSlotId().getValue());
				occupied |= static_cast<unsigned char>(1 << lane);
				_classCapacity.IncrementOccupancy(op->getPlacement().RequiredSlotClass);
				certificate.AddOperation(op);
				metadata = metadata.WithOperation(op);
				boundary = boundary.WithOperation(op);
				_runtimeLegalityService->RefreshSmtAfterMutation(_currentReplayPhase, certificate, metadata, boundary);
			}
			return issuedWorkingMask != 0;
		}
	}

	AssistBackpressureSnapshot MicroOpScheduler::CaptureAssistBackpressureSnapshot()
	{
		return AssistBackpressureSnapshot(static_cast<unsigned char>(std::min(_remainingHardwareMemoryIssueBudget, _remainingHardwareLoadIssueBudget)), _consumedHardwareLoadBudgetByBank, _hardwareOccupancySnapshot.getReadBankBudgetAtLeastOneMask(), _hardwareOccupancySnapshot.getReadBankBudgetAtLeastTwoMask(), _projectedOutstandingMemoryCountByVt[0], _projectedOutstandingMemoryCountByVt[1], _projectedOutstandingMemoryCountByVt[2], _projectedOutstandingMemoryCountByVt[3], _projectedOutstandingMemoryCapacityByVt[0], _projectedOutstandingMemoryCapacityByVt[1], _projectedOutstandingMemoryCapacityByVt[2], _projectedOutstandingMemoryCapacityByVt[3]);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private bool TryReserveAssistBackpressure(AssistMicroOp assistMicroOp, ref AssistBackpressureState assistBackpressureState, MemorySubsystem? memSub = null)
	bool MicroOpScheduler::TryReserveAssistBackpressure(std::shared_ptr<AssistMicroOp> assistMicroOp, AssistBackpressureState &assistBackpressureState, std::shared_ptr<MemorySubsystem> memSub)
	{
		bool dmaSrfAvailable = assistMicroOp->getCarrierKind() != AssistCarrierKind::Lane6Dma || CanAllocateAssistDmaSrf(assistMicroOp, assistBackpressureState.getDmaSrfPartitionPolicy(), memSub);

		AssistBackpressureRejectKind rejectKind;
		if (assistBackpressureState.TryReserve(assistMicroOp, dmaSrfAvailable, rejectKind))
		{
			return true;
		}

		setAssistBackpressureRejects(getAssistBackpressureRejects() + 1);
		switch (rejectKind)
		{
			case AssistBackpressureRejectKind::SharedOuterCap:
				setAssistBackpressureOuterCapRejects(getAssistBackpressureOuterCapRejects() + 1);
				break;
			case AssistBackpressureRejectKind::OutstandingMemory:
				setAssistBackpressureMshrRejects(getAssistBackpressureMshrRejects() + 1);
				break;
			case AssistBackpressureRejectKind::DmaStreamRegisterFile:
				setAssistBackpressureDmaSrfRejects(getAssistBackpressureDmaSrfRejects() + 1);
				break;
		}

		RecordAssistReject(assistMicroOp, TypedSlotRejectReason::AssistBackpressureReject);
		return false;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool CanAllocateAssistDmaSrf(AssistMicroOp assistMicroOp, AssistStreamRegisterPartitionPolicy partitionPolicy, MemorySubsystem? memSub = null)
	bool MicroOpScheduler::CanAllocateAssistDmaSrf(std::shared_ptr<AssistMicroOp> assistMicroOp, AssistStreamRegisterPartitionPolicy partitionPolicy, std::shared_ptr<MemorySubsystem> memSub)
	{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: StreamRegisterFile? streamRegisterFile = memSub == null ? null : memSub.StreamRegisters;
		std::shared_ptr<StreamRegisterFile> streamRegisterFile = memSub == nullptr ? nullptr : memSub->getStreamRegisters();
		if (streamRegisterFile == nullptr)
		{
			return true;
		}

		AssistStreamRegisterRejectKind rejectKind = AssistStreamRegisterRejectKind::None;
		return streamRegisterFile->CanAllocateAssistRegister(assistMicroOp->getBaseAddress(), assistMicroOp->getElementSize(), partitionPolicy, rejectKind);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public MicroOp[] PackBundle(System.Collections.Generic.IReadOnlyList<MicroOp?> originalBundle, int currentThreadId, bool stealEnabled, byte stealMask, ResourceBitset globalResourceLocks = default, byte donorMask = 0, ulong domainCertificate = 0, int localCoreId = -1, ushort assistPodId = ushort.MaxValue, int bundleOwnerContextId = -1, ulong bundleOwnerDomainTag = 0, ulong assistRuntimeEpoch = 0, System.Nullable<YAKSys_Hybrid_CPU.Memory.MemorySubsystem> memSub = null, System.Nullable<PodController>[]? pods = null)
	std::vector<std::shared_ptr<MicroOp>> MicroOpScheduler::PackBundle(const std::vector<std::shared_ptr<MicroOp>> &originalBundle, int currentThreadId, bool stealEnabled, unsigned char stealMask, ResourceBitset globalResourceLocks, unsigned char donorMask, unsigned long long domainCertificate, int localCoreId, unsigned short assistPodId, int bundleOwnerContextId, unsigned long long bundleOwnerDomainTag, unsigned long long assistRuntimeEpoch, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub, std::shared_ptr<std::vector<std::shared_ptr<PodController>>> pods)
	{
		setTotalSchedulerCycles(getTotalSchedulerCycles() + 1);

		if (!stealEnabled || originalBundle.empty() || originalBundle.size() != 8)
		{
			return std::vector<std::shared_ptr<MicroOp>>(originalBundle);
		}

		// Work on a copy so the original is not mutated
		std::vector<std::shared_ptr<MicroOp>> result(8);
		for (int slot = 0; slot < result.size(); slot++)
		{
			result[slot] = originalBundle[slot];
		}

		// Phase 02: Compute per-class occupancy from the current bundle
		_classCapacity = SlotClassCapacity::ComputeFromBundle(result);
		RecordLoopPhaseSample();
		PrepareProjectedMemoryIssuePass(result);

		std::shared_ptr<BundleResourceCertificate> bundleCert = BundleResourceCertificate::Create(result, currentThreadId, static_cast<unsigned long long>(getTotalSchedulerCycles()));
		_runtimeLegalityService->PrepareInterCore(_currentReplayPhase, bundleCert);
		unsigned char replayAwareStealMask = GetReplayAwareStealMask(stealMask, donorMask);
		BundleOpportunityState opportunityState = BundleOpportunityState::Create(result);
		RecordDeterminismTaxWindow(opportunityState.getReferenceOpportunityCount(), opportunityState.CountReplayEligibleReferenceOpportunities(replayAwareStealMask), CountPendingInterCoreCandidates(domainCertificate));
		int currentPassInjectedCount = 0;

		for (int slot = 0; slot < 8; slot++)
		{
			if (!opportunityState.IsReplayEligibleReferenceOpportunity(slot, replayAwareStealMask))
			{
				continue;
			}

			// Try to find a candidate via priority encoder
			std::shared_ptr<MicroOp> candidate = TryStealSlot(currentThreadId, domainCertificate);
			if (candidate == nullptr)
			{
				break; // No more candidates
			}

			// Safety verification: domain cert + safety mask + hardware state
			// Convert ResourceBitset → SafetyMask128 (same bit layout, zero-cost)
			SafetyMask128 hwMask = SafetyMask128(globalResourceLocks.Low, globalResourceLocks.High);

			LegalityDecision legalityDecision = EvaluateInterCoreLegality(result, bundleCert, currentThreadId, domainCertificate, candidate, hwMask);
			bool safe = legalityDecision.isAllowed();
			TypedSlotRejectReason outerCapReject = TypedSlotRejectReason::None;

			if (safe)
			{
				std::shared_ptr<LoadStoreMicroOp> loadStoreCandidate = std::dynamic_pointer_cast<LoadStoreMicroOp>(candidate);
				int candidateBankId = loadStoreCandidate != nullptr ? loadStoreCandidate->getMemoryBankId() : -1;
				safe = TryPassOuterCap(candidate, candidate->getVirtualThreadId(), candidate->isMemoryOp(), candidateBankId, currentThreadId, outerCapReject);
			}

			if (safe)
			{
				// Mark stolen memory ops as speculative (Phase 7)
				if (candidate->isMemoryOp() && candidate->getOwnerThreadId() != currentThreadId)
				{
					candidate->setSpeculative(true);
					setSuccessfulSpeculativeSteals(getSuccessfulSpeculativeSteals() + 1);

					// Phase 2C: Spend speculation budget
					if (getSpeculationBudgetEnabled())
					{
						_speculationBudget--;
						int concurrent = (getSpeculationBudgetMax() - _speculationBudget);
						if (concurrent > getPeakConcurrentSpeculativeOps())
						{
							setPeakConcurrentSpeculativeOps(concurrent);
						}
					}
				}

				result[slot] = candidate;
				candidate->setFspInjected(true);
				setSuccessfulInjectionsCount(getSuccessfulInjectionsCount() + 1);
				currentPassInjectedCount++;
				bundleCert = bundleCert->WithOperation(candidate);
				opportunityState = opportunityState.WithOccupiedSlot(slot);
				_runtimeLegalityService->RefreshInterCoreAfterMutation(_currentReplayPhase, bundleCert);

				// Phase 02: Update class-capacity after successful placement
				_classCapacity.IncrementOccupancy(candidate->getPlacement().RequiredSlotClass);
				ConsumeProjectedMemoryIssueStateIfNeeded(candidate);
			}
			else
			{
				setRejectedInjectionsCount(getRejectedInjectionsCount() + 1);

				if (outerCapReject != TypedSlotRejectReason::None)
				{
					RecordPerVtRejection(candidate->getVirtualThreadId());
	#if defined(TESTING)
					TestRecordRejection(std::format("Inter-core outer-cap reject at slot {0:s}: {1:s}", slot, outerCapReject), RejectionType::MemoryConflict);
	#endif
					continue;
				}

				std::wstring rejectDetail = legalityDecision.getCertificateDetail() != CertificateRejectDetail::None ? std::wstring(L", ") + std::to_wstring(static_cast<int>(legalityDecision.getCertificateDetail())) : std::wstring{};
	#if defined(TESTING)
				TestRecordRejection(std::format("Inter-core legality reject at slot {0:s}: {1:s} via {2:s}{3:s}", slot, legalityDecision.getRejectKind(), legalityDecision.getAuthoritySource(), rejectDetail), RejectionType::SafetyMask);
	#endif
			}
		}

		if (localCoreId >= 0)
		{
			TryInjectInterCoreAssistCandidates(result, bundleCert, opportunityState, currentThreadId, localCoreId, assistPodId, bundleOwnerContextId, bundleOwnerDomainTag, domainCertificate, assistRuntimeEpoch, globalResourceLocks, currentPassInjectedCount, memSub, pods);
		}

		return result;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private void PipelineFspStage1_Nominate(int ownerVirtualThreadId, SmtNominationState nominationState, IReadOnlyList<MicroOp?> bundle, BundleResourceCertificate4Way bundleMask, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard)
	inline void MicroOpScheduler::PipelineFspStage1_Nominate(int ownerVirtualThreadId, SmtNominationState nominationState, const std::vector<std::shared_ptr<MicroOp>> &bundle, BundleResourceCertificate4Way bundleMask, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard)
	{
		// Latch the exact scheduling generation consumed by SCHED2. SCHED1
		// performs no admission, but its nomination must not be combined with
		// a different owner, bundle, replay epoch, or mutable carrier state.
		_fspOwnerVirtualThreadId = ownerVirtualThreadId;
		CaptureFspBundleReferences(bundle);
		_fspBundleFingerprint = ComputeFspBundleFingerprint(bundle);
		_fspBundleCertificateIdentity = bundleMask.getStructuralIdentity();
		_fspBundleMetadata = bundleMetadata;
		_fspBoundaryGuard = boundaryGuard;
		_fspReplayPhase = _currentReplayPhase;

		for (int vt = 0; vt < SMT_WAYS; vt++)
		{
			// Clear pipeline register entry
			_fspPipelineReg[vt].Valid = false;
			_fspPipelineReg[vt].VirtualThreadId = vt;
			_fspPipelineReg[vt].Candidate = nullptr;
			_fspPipelineReg[vt].IdentityTemplate = nullptr;
			_fspPipelineReg[vt].CandidateFingerprint = YAKSys_Hybrid_CPU::Core::MicroOpScheduler::FspSnapshotFingerprint();
			if (nominationState.IsReadyNonOwnerCandidate(vt, ownerVirtualThreadId))
			{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MicroOp? candidate = _smtPorts[vt];
				std::shared_ptr<MicroOp> candidate = _smtPorts[vt];
				if (candidate != nullptr)
				{
					_fspPipelineReg[vt].Candidate = candidate;
					_fspPipelineReg[vt].IdentityTemplate = candidate->getPostStageBIdentityTemplate();
					_fspPipelineReg[vt].CandidateFingerprint = ComputeFspCandidateFingerprint(candidate);
					_fspPipelineReg[vt].Valid = true;
				}
			}
		}

		_fspCurrentStage = FspPipelineStage::SCHED2;
		setFspPipelineLatencyCycles(getFspPipelineLatencyCycles() + 1);
	}

	std::pair<BundleResourceCertificate4Way, int> MicroOpScheduler::PipelineFspStage2_Intersect(std::vector<std::shared_ptr<MicroOp>> &bundle, BundleResourceCertificate4Way bundleMask, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard, int nextEmptySlot)
	{
		if (_fspOwnerVirtualThreadId != bundleMetadata.getOwnerVirtualThreadId() ||
			!FspBundleReferencesMatch(bundle) ||
			!_fspBundleFingerprint.Equals(ComputeFspBundleFingerprint(bundle)) ||
			!_fspBundleCertificateIdentity.Equals(bundleMask.getStructuralIdentity()) ||
			!_fspBundleMetadata.Equals(bundleMetadata) ||
			!_fspBoundaryGuard.Equals(boundaryGuard) ||
			!ReplayPhaseMatches(_fspReplayPhase, _currentReplayPhase))
		{
			_fspCurrentStage = FspPipelineStage::SCHED1;
			return {bundleMask, nextEmptySlot};
		}

		if (IsSmtBundleBlockedByBoundaryGuard(bundleMask, bundleMetadata, boundaryGuard))
		{
			_fspCurrentStage = FspPipelineStage::SCHED1;
			return {bundleMask, nextEmptySlot};
		}

		_runtimeLegalityService->PrepareSmt(_currentReplayPhase, bundleMask, bundleMetadata, boundaryGuard);
		BundleOpportunityState opportunityState = BundleOpportunityState::Create(bundle);
		const int templateDomainScopeId = ResolveClassTemplateDomainScopeId(bundleMetadata);
		ClassTemplateAdmissionState templateState = getTypedSlotEnabled() ? PrepareClassTemplateAdmissionState(templateDomainScopeId) : ClassTemplateAdmissionState();
		unsigned char bundleOccupancy = opportunityState.getOccupancyMask();
		int realInjectedCount = 0;
		PrepareProjectedMemoryIssuePass(bundle);

		for (int vt = 0; vt < SMT_WAYS; ++vt)
		{
			if (!_fspPipelineReg[vt].Valid) continue;
			FspPipelineRegister &pipelineEntry = _fspPipelineReg[vt];
			const int candidateVt = pipelineEntry.VirtualThreadId;
			std::shared_ptr<MicroOp> candidate = pipelineEntry.Candidate;
			if (candidate == nullptr || _smtPorts[candidateVt] != candidate || candidate->getPostStageBIdentityTemplate() != pipelineEntry.IdentityTemplate || candidate->getPostStageBIssuedAttempt() != nullptr || !pipelineEntry.CandidateFingerprint.Equals(ComputeFspCandidateFingerprint(candidate)))
			{
				continue;
			}

			AssertRf06MemoryContractProjection(candidate);
			if (getTypedSlotEnabled())
			{
				TypedSlotRejectReason rejectReason = TypedSlotRejectReason::None;
				if (!TryClassAdmission(candidate, bundleMask, bundleMetadata, boundaryGuard, templateState, _fspOwnerVirtualThreadId, realInjectedCount, rejectReason))
				{
					RecordTypedSlotReject(rejectReason, candidate);
					continue;
				}
				int lane = -1;
				if (!TryMaterializeLane(candidate, bundleOccupancy, lane, rejectReason))
				{
					RecordTypedSlotReject(rejectReason, candidate);
					continue;
				}

				bundle[lane] = candidate;
				MaterializePostStageBIssuedAttempt(candidate, pipelineEntry.IdentityTemplate, lane, bundleMetadata);
				candidate->setFspInjected(true);
				bundleMask.AddOperation(candidate);
				opportunityState = opportunityState.WithOccupiedSlot(lane);
				bundleOccupancy |= static_cast<unsigned char>(1 << lane);
				setSmtInjectionsCount(getSmtInjectionsCount() + 1);
				++realInjectedCount;
				++_perVtInjections[candidateVt];
				RecordTypedSlotInject(candidate, lane);
				bundleMetadata = bundleMetadata.WithOperation(candidate);
				boundaryGuard = boundaryGuard.WithOperation(candidate);
				_runtimeLegalityService->RefreshSmtAfterMutation(_currentReplayPhase, bundleMask, bundleMetadata, boundaryGuard);
				_classCapacity.IncrementOccupancy(candidate->getPlacement().RequiredSlotClass);
				ConsumeProjectedMemoryIssueStateIfNeeded(candidate);
				if (candidate->getPlacement().PinningKind == SlotPinningKind::ClassFlexible) RecordPhaseLane(candidate->getPlacement().RequiredSlotClass, lane);
				if (_currentReplayPhase.isActive() && !_classTemplateValid) CaptureClassTemplate(templateDomainScopeId);
			}
			else
			{
				const int slot = ResolveNextInjectableSlot(opportunityState, 0);
				if (slot < 0) continue;
				LegalityDecision decision = EvaluateSmtLegality(bundleMask, bundleMetadata, boundaryGuard, candidate);
				if (!decision.isAllowed())
				{
					RecordPerVtRejection(candidateVt);
					setSmtRejectionsCount(getSmtRejectionsCount() + 1);
					continue;
				}
				std::shared_ptr<LoadStoreMicroOp> loadStore = std::dynamic_pointer_cast<LoadStoreMicroOp>(candidate);
				const int bankId = loadStore != nullptr ? loadStore->getMemoryBankId() : -1;
				TypedSlotRejectReason outerCapReject = TypedSlotRejectReason::None;
				if (!TryPassOuterCap(candidate, candidateVt, candidate->isMemoryOp(), bankId, _fspOwnerVirtualThreadId, outerCapReject))
				{
					RecordPerVtRejection(candidateVt);
					continue;
				}
				bundle[slot] = candidate;
				candidate->setFspInjected(true);
				bundleMask.AddOperation(candidate);
				opportunityState = opportunityState.WithOccupiedSlot(slot);
				bundleOccupancy = opportunityState.getOccupancyMask();
				setSmtInjectionsCount(getSmtInjectionsCount() + 1);
				++realInjectedCount;
				++_perVtInjections[candidateVt];
				RecordTypedSlotInject(candidate, slot);
				bundleMetadata = bundleMetadata.WithOperation(candidate);
				boundaryGuard = boundaryGuard.WithOperation(candidate);
				_runtimeLegalityService->RefreshSmtAfterMutation(_currentReplayPhase, bundleMask, bundleMetadata, boundaryGuard);
				ConsumeProjectedMemoryIssueStateIfNeeded(candidate);
			}

			_smtPorts[candidateVt] = nullptr;
			_smtPortValid[candidateVt] = false;
		}

		nextEmptySlot = ResolveNextInjectableSlot(opportunityState, 0);
		_fspCurrentStage = FspPipelineStage::SCHED1;
		return {bundleMask, nextEmptySlot};
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private void RebuildFspStageContext(IReadOnlyList<MicroOp?> bundle, int ownerVirtualThreadId, out BundleResourceCertificate4Way bundleMask, out SmtBundleMetadata4Way bundleMetadata, out BoundaryGuardState boundaryGuard)
	void MicroOpScheduler::RebuildFspStageContext(const std::vector<std::shared_ptr<MicroOp>> &bundle, int ownerVirtualThreadId, BundleResourceCertificate4Way &bundleMask, SmtBundleMetadata4Way &bundleMetadata, BoundaryGuardState &boundaryGuard)
	{
		bundleMask = BundleResourceCertificate4Way::Empty;
		bundleMetadata = SmtBundleMetadata4Way::Empty(ownerVirtualThreadId);
		boundaryGuard = BoundaryGuardState::Open(_serializingEpochCounter);
		for (int slot = 0; slot < bundle.size(); slot++)
		{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MicroOp? operation = bundle[slot];
			std::shared_ptr<MicroOp> operation = bundle[slot];
			if (operation == nullptr)
			{
				continue;
			}

			bundleMask.AddOperation(operation);
			bundleMetadata = bundleMetadata.WithOperation(operation);
			boundaryGuard = boundaryGuard.WithOperation(operation);
		}
	}

	bool MicroOpScheduler::ReplayPhaseMatches(ReplayPhaseContext expected, ReplayPhaseContext actual)
	{
		return expected.isActive() == actual.isActive() && expected.getEpochId() == actual.getEpochId() && expected.getCachedPc() == actual.getCachedPc() && expected.getEpochLength() == actual.getEpochLength() && expected.getCompletedReplays() == actual.getCompletedReplays() && expected.getValidSlotCount() == actual.getValidSlotCount() && expected.getStableDonorMask() == actual.getStableDonorMask() && expected.getLastInvalidationReason() == actual.getLastInvalidationReason();
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private void CaptureFspBundleReferences(IReadOnlyList<MicroOp?> bundle)
	void MicroOpScheduler::CaptureFspBundleReferences(const std::vector<std::shared_ptr<MicroOp>> &bundle)
	{
		for (int slot = 0; slot < _fspBundleReferences.size(); slot++)
		{
			_fspBundleReferences[slot] = slot < bundle.size() ? bundle[slot] : nullptr;
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private bool FspBundleReferencesMatch(IReadOnlyList<MicroOp?> bundle)
	bool MicroOpScheduler::FspBundleReferencesMatch(const std::vector<std::shared_ptr<MicroOp>> &bundle)
	{
		if (bundle.size() != _fspBundleReferences.size())
		{
			return false;
		}

		for (int slot = 0; slot < _fspBundleReferences.size(); slot++)
		{
			if (_fspBundleReferences[slot] != bundle[slot])
			{
				return false;
			}
		}

		return true;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static FspSnapshotFingerprint ComputeFspBundleFingerprint(IReadOnlyList<MicroOp?> bundle)
	MicroOpScheduler::FspSnapshotFingerprint MicroOpScheduler::ComputeFspBundleFingerprint(const std::vector<std::shared_ptr<MicroOp>> &bundle)
	{
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# deconstruction declarations:
		unsigned long long low = 14695981039346656037ULL;
		unsigned long long high = 7809847782465536322ULL;
		AddFspFingerprintValue(low, high, static_cast<unsigned long long>(bundle.size()));
		for (int slot = 0; slot < bundle.size(); slot++)
		{
			AddFspFingerprintValue(low, high, static_cast<unsigned long long>(static_cast<unsigned int>(slot)));
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MicroOp? operation = bundle[slot];
			std::shared_ptr<MicroOp> operation = bundle[slot];
			if (operation == nullptr)
			{
				AddFspFingerprintValue(low, high, 0);
				continue;
			}

			AddFspFingerprintValue(low, high, 1);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: AddFspFingerprintValue(ref low, ref high, unchecked((ulong)(uint)RuntimeHelpers.GetHashCode(operation)));
			AddFspFingerprintValue(low, high, static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(operation.get())));
			FspSnapshotFingerprint operationFingerprint = ComputeFspCandidateFingerprint(operation);
			AddFspFingerprintValue(low, high, operationFingerprint.getLow());
			AddFspFingerprintValue(low, high, operationFingerprint.getHigh());
		}

		return FspSnapshotFingerprint(low, high);
	}

	MicroOpScheduler::FspSnapshotFingerprint MicroOpScheduler::ComputeFspCandidateFingerprint(std::shared_ptr<MicroOp> candidate)
	{
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# deconstruction declarations:
		unsigned long long low = 14695981039346656037ULL;
		unsigned long long high = 7809847782465536322ULL;
		std::shared_ptr<MicroOpAdmissionMetadata> admission = candidate->getAdmissionMetadata();

		AddFspFingerprintValue(low, high, candidate->getOpCode());
		AddFspFingerprintValue(low, high, candidate->getPredicateMask());
		AddFspFingerprintValue(low, high, candidate->getDestRegID());
		AddFspFingerprintValue(low, high, candidate->getWritesRegister() ? 1ULL : 0ULL);
		AddFspFingerprintValue(low, high, candidate->getLatency());
		AddFspFingerprintValue(low, high, candidate->isMemoryOp() ? 1ULL : 0ULL);
		AddFspFingerprintValue(low, high, candidate->isControlFlow() ? 1ULL : 0ULL);
		AddFspFingerprintValue(low, high, candidate->isStealable() ? 1ULL : 0ULL);
		AddFspFingerprintValue(low, high, static_cast<unsigned long long>(candidate->getMemoryLocalityHint()));
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: AddFspFingerprintValue(ref low, ref high, unchecked((ulong)(uint)candidate.OwnerThreadId));
		AddFspFingerprintValue(low, high, static_cast<unsigned long long>(static_cast<unsigned int>(candidate->getOwnerThreadId())));
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: AddFspFingerprintValue(ref low, ref high, unchecked((ulong)(uint)candidate.OwnerContextId));
		AddFspFingerprintValue(low, high, static_cast<unsigned long long>(static_cast<unsigned int>(candidate->getOwnerContextId())));
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: AddFspFingerprintValue(ref low, ref high, unchecked((ulong)(uint)candidate.VirtualThreadId));
		AddFspFingerprintValue(low, high, static_cast<unsigned long long>(static_cast<unsigned int>(candidate->getVirtualThreadId())));
		AddFspFingerprintValue(low, high, static_cast<unsigned long long>(candidate->getClass()));
		AddFspFingerprintValue(low, high, static_cast<unsigned long long>(candidate->getInstructionClass()));
		AddFspFingerprintValue(low, high, static_cast<unsigned long long>(candidate->getSerializationClass()));
		AddFspFingerprintValue(low, high, candidate->getHasSideEffects() ? 1ULL : 0ULL);
		AddFspFingerprintValue(low, high, candidate->getSafetyMask().Low);
		AddFspFingerprintValue(low, high, candidate->getSafetyMask().High);
		AddFspFingerprintValue(low, high, candidate->getResourceMask().Low);
		AddFspFingerprintValue(low, high, candidate->getResourceMask().High);
		AddFspPlacementFingerprint(low, high, candidate->getPlacement());
		std::shared_ptr<LoadStoreMicroOp> loadStore = std::dynamic_pointer_cast<LoadStoreMicroOp>(candidate);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: AddFspFingerprintValue(ref low, ref high, candidate is LoadStoreMicroOp loadStore ? unchecked((ulong)(uint)loadStore.MemoryBankId) : ulong.MaxValue);
		AddFspFingerprintValue(low, high, loadStore != nullptr ? static_cast<unsigned long long>(static_cast<unsigned int>(loadStore->getMemoryBankId())) : std::numeric_limits<unsigned long long>::max());

		AddFspFingerprintValue(low, high, admission->IsStealable ? 1ULL : 0ULL);
		AddFspFingerprintValue(low, high, admission->IsControlFlow ? 1ULL : 0ULL);
		AddFspFingerprintValue(low, high, admission->IsMemoryOp ? 1ULL : 0ULL);
		AddFspFingerprintValue(low, high, admission->WritesRegister ? 1ULL : 0ULL);
		AddFspFingerprintValue(low, high, admission->HasSideEffects ? 1ULL : 0ULL);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: AddFspFingerprintValue(ref low, ref high, unchecked((ulong)(uint)admission.OwnerContextId));
		AddFspFingerprintValue(low, high, static_cast<unsigned long long>(static_cast<unsigned int>(admission->OwnerContextId)));
		AddFspFingerprintValue(low, high, admission->DomainTag);
		AddFspPlacementFingerprint(low, high, admission->Placement);
		AddFspFingerprintValue(low, high, admission->RegisterHazardMask);
		AddFspFingerprintValue(low, high, admission->StructuralSafetyMask.Low);
		AddFspFingerprintValue(low, high, admission->StructuralSafetyMask.High);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: AddFspFingerprintValue(ref low, ref high, unchecked((ulong)(uint)admission.AssistCoalescingDescriptor.GetHashCode()));
		AddFspFingerprintValue(low, high, admission->AssistCoalescingDescriptorValue.BaseAddress);
		AddFspFingerprintValue(low, high, admission->AssistCoalescingDescriptorValue.PrefetchLength);
		AddFspFingerprintValue(low, high, static_cast<unsigned long long>(static_cast<unsigned int>(admission->AssistCoalescingDescriptorValue.SourceRangeCount)));
		AddFspFingerprintValue(low, high, static_cast<unsigned long long>(static_cast<unsigned int>(admission->AssistCoalescingDescriptorValue.NormalizedRangeCount)));
		AddFspFingerprintValue(low, high, admission->AssistCoalescingDescriptorValue.LargestGapBytes);
		AddFspRegisterListFingerprint(low, high, admission->ReadRegisters);
		AddFspRegisterListFingerprint(low, high, admission->WriteRegisters);
		AddFspRangeListFingerprint(low, high, admission->ReadMemoryRanges);
		AddFspRangeListFingerprint(low, high, admission->NormalizedReadMemoryRanges);
		AddFspRangeListFingerprint(low, high, admission->WriteMemoryRanges);
		return FspSnapshotFingerprint(low, high);
	}

	void MicroOpScheduler::AddFspPlacementFingerprint(unsigned long long &low, unsigned long long &high, SlotPlacementMetadata placement)
	{
		AddFspFingerprintValue(low, high, static_cast<unsigned long long>(placement.RequiredSlotClass));
		AddFspFingerprintValue(low, high, static_cast<unsigned long long>(placement.PinningKind));
		AddFspFingerprintValue(low, high, placement.PinnedLaneId);
		AddFspFingerprintValue(low, high, placement.DomainTag);
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static void AddFspRegisterListFingerprint(ref ulong low, ref ulong high, IReadOnlyList<int> values)
	void MicroOpScheduler::AddFspRegisterListFingerprint(unsigned long long &low, unsigned long long &high, const std::vector<int> &values)
	{
		AddFspFingerprintValue(low, high, static_cast<unsigned long long>(values.size()));
		for (int index = 0; index < values.size(); index++)
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: AddFspFingerprintValue(ref low, ref high, unchecked((ulong)(uint)values[index]));
			AddFspFingerprintValue(low, high, static_cast<unsigned long long>(static_cast<unsigned int>(values[index])));
		}
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static void AddFspRangeListFingerprint(ref ulong low, ref ulong high, IReadOnlyList<(ulong Address, ulong Length)> ranges)
	void MicroOpScheduler::AddFspRangeListFingerprint(unsigned long long &low, unsigned long long &high, const std::vector<std::pair<unsigned long long, unsigned long long>> &ranges)
	{
		AddFspFingerprintValue(low, high, static_cast<unsigned long long>(ranges.size()));
		for (int index = 0; index < ranges.size(); index++)
		{
			AddFspFingerprintValue(low, high, ranges[index].first);
			AddFspFingerprintValue(low, high, ranges[index].second);
		}
	}

	void MicroOpScheduler::AddFspFingerprintValue(unsigned long long &low, unsigned long long &high, unsigned long long value)
	{
//		unchecked
			low = (low ^ value) * 1099511628211ULL;
			high = (high + value + 0x9E3779B97F4A7C15ULL) * 14029467366897019727ULL;
			high ^= high >> 29;
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
	}

	unsigned short MicroOpScheduler::GetLatchedInterCoreNominationReadyMask()
	{
		return _latchedInterCoreNominationReadyMask;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public void Nominate(int coreId, MicroOp? op)
	inline void MicroOpScheduler::Nominate(int coreId, std::shared_ptr<MicroOp> op)
	{
		if (static_cast<unsigned int>(coreId) >= NUM_PORTS)
		{
			return;
		}
		if (op == nullptr)
		{
			return;
		}
		if (op->isControlFlow())
		{
			return;
		}
		if (!op->getAdmissionMetadata()->IsStealable)
		{
			return;
		}
		if (_coreStalled[coreId])
		{
			return;
		}

		_ports[coreId] = op;
		_portValid[coreId] = true;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public void NominateSmtCandidate(int vtId, MicroOp? op)
	void MicroOpScheduler::NominateSmtCandidate(int vtId, std::shared_ptr<MicroOp> op)
	{
		if (static_cast<unsigned int>(vtId) >= SMT_WAYS)
		{
			return;
		}
		if (op == nullptr)
		{
			return;
		}
		if (op->getPostStageBIssuedAttempt() != nullptr)
		{
			return;
		}

		_smtPorts[vtId] = op;
		_smtPortValid[vtId] = true;
	}

	void MicroOpScheduler::WithdrawSmtCandidate(int vtId, std::shared_ptr<MicroOp> expected)
	{
		if (static_cast<unsigned int>(vtId) >= SMT_WAYS || _smtPorts[vtId] != expected)
		{
			return;
		}
		_smtPorts[vtId] = nullptr;
		_smtPortValid[vtId] = false;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public MicroOp? TryStealSlot(int requestingCoreId, ulong requestedDomainTag)
	inline std::shared_ptr<MicroOp> MicroOpScheduler::TryStealSlot(int requestingCoreId, unsigned long long requestedDomainTag)
	{
		return TryStealSlot(requestingCoreId, requestedDomainTag, CaptureInterCoreNominationSnapshot());
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private MicroOp? TryStealSlot(int requestingCoreId, ulong requestedDomainTag, InterCoreNominationSnapshot nominationSnapshot)
	inline std::shared_ptr<MicroOp> MicroOpScheduler::TryStealSlot(int requestingCoreId, unsigned long long requestedDomainTag, InterCoreNominationSnapshot nominationSnapshot)
	{
		for (int i = 0; i < NUM_PORTS; i++)
		{
			std::shared_ptr<MicroOp> candidate;
			if (!nominationSnapshot.TryGetCandidate(i, candidate))
			{
				continue;
			}

			// Domain isolation gate: keep the existing filtering behavior, but
			// classify rejected probes so stress evidence remains explicit.
			if (requestedDomainTag != 0)
			{
				InterCoreDomainGuardDecision domainGuard = _runtimeLegalityService->EvaluateInterCoreDomainGuard(candidate, requestedDomainTag);
				RecordDomainIsolationProbe(domainGuard.getProbeResult());
				if (!domainGuard.isAllowed())
				{
					continue;
				}
			}

			// Consume the port (single-use per cycle)
			ConsumeInterCoreNomination(i);
			return candidate;
		}

		return nullptr;
	}

	inline void MicroOpScheduler::SetCoreStalled(int coreId, bool stalled)
	{
		if (static_cast<unsigned int>(coreId) >= NUM_PORTS)
		{
			return;
		}
		_coreStalled[coreId] = stalled;

		// Annul existing nomination if stalling
		if (stalled)
		{
			_ports[coreId] = nullptr;
			_portValid[coreId] = false;
			ClearInterCoreAssistNominationPort(coreId);
		}
	}

	void MicroOpScheduler::LatchNominations()
	{
		_latchedInterCoreNominationReadyMask = CaptureInterCoreNominationSnapshot().getReadyMask();
	}

	void MicroOpScheduler::ClearNominationPorts()
	{
		for (int i = 0; i < NUM_PORTS; i++)
		{
			_ports[i] = nullptr;
			_portValid[i] = false;
		}
	}

	inline int MicroOpScheduler::SetScoreboardPending(int targetId, int ownerThreadId, long long currentCycle)
	{
		for (int i = 0; i < SCOREBOARD_SLOTS; i++)
		{
			if (_scoreboard[i] == -1)
			{
				_scoreboard[i] = targetId;
				return i;
			}
		}
		return -1; // Full
	}

	inline bool MicroOpScheduler::IsScoreboardPending(int targetId)
	{
		for (int i = 0; i < SCOREBOARD_SLOTS; i++)
		{
			if (_scoreboard[i] == targetId)
			{
				return true;
			}
		}
		return false;
	}

	inline void MicroOpScheduler::ClearScoreboardEntry(int slotIndex)
	{
		if (static_cast<unsigned int>(slotIndex) < SCOREBOARD_SLOTS)
		{
			_scoreboard[slotIndex] = -1;
		}
	}

	inline void MicroOpScheduler::ClearScoreboard()
	{
		for (int i = 0; i < SCOREBOARD_SLOTS; i++)
		{
			_scoreboard[i] = -1;
		}
	}

	inline int MicroOpScheduler::SetSmtScoreboardPending(int targetId, int virtualThreadId, long long currentCycle)
	{
		return SetSmtScoreboardPendingTyped(targetId, virtualThreadId, currentCycle, ScoreboardEntryType::Dma, -1);
	}

	int MicroOpScheduler::SetSmtScoreboardPendingTyped(int targetId, int virtualThreadId, long long currentCycle, ScoreboardEntryType entryType, int bankId)
	{
		if (static_cast<unsigned int>(virtualThreadId) >= SMT_WAYS)
		{
			return -1;
		}

		for (int i = 0; i < SCOREBOARD_SLOTS; i++)
		{
			if (_smtScoreboard[virtualThreadId][i] == -1)
			{
				_smtScoreboard[virtualThreadId][i] = targetId;
				_smtScoreboardType[virtualThreadId][i] = entryType;
				_smtScoreboardBankId[virtualThreadId][i] = bankId;
				return i;
			}
		}
		return -1; // Full
	}

	inline bool MicroOpScheduler::IsSmtScoreboardPending(int targetId, int virtualThreadId)
	{
		if (static_cast<unsigned int>(virtualThreadId) >= SMT_WAYS)
		{
			return false;
		}

		for (int i = 0; i < SCOREBOARD_SLOTS; i++)
		{
			if (_smtScoreboard[virtualThreadId][i] == targetId)
			{
				return true;
			}
		}
		return false;
	}

	void MicroOpScheduler::ClearSmtScoreboardEntry(int virtualThreadId, int slotIndex)
	{
		if (static_cast<unsigned int>(virtualThreadId) >= SMT_WAYS)
		{
			return;
		}
		if (static_cast<unsigned int>(slotIndex) >= SCOREBOARD_SLOTS)
		{
			return;
		}
		_smtScoreboard[virtualThreadId][slotIndex] = -1;
		_smtScoreboardType[virtualThreadId][slotIndex] = ScoreboardEntryType::Free;
		_smtScoreboardBankId[virtualThreadId][slotIndex] = -1;
	}

	inline void MicroOpScheduler::ClearSmtScoreboard()
	{
		for (int vt = 0; vt < SMT_WAYS; vt++)
		{
			for (int s = 0; s < SCOREBOARD_SLOTS; s++)
			{
				_smtScoreboard[vt][s] = -1;
				_smtScoreboardType[vt][s] = ScoreboardEntryType::Free;
				_smtScoreboardBankId[vt][s] = -1;
			}
		}
	}

	bool MicroOpScheduler::IsBankPendingForVT(int bankId, int virtualThreadId)
	{
		if (static_cast<unsigned int>(virtualThreadId) >= SMT_WAYS)
		{
			return false;
		}

		for (int i = 0; i < SCOREBOARD_SLOTS; i++)
		{
			if (_smtScoreboard[virtualThreadId][i] != -1 && _smtScoreboardBankId[virtualThreadId][i] == bankId && (_smtScoreboardType[virtualThreadId][i] == ScoreboardEntryType::OutstandingLoad || _smtScoreboardType[virtualThreadId][i] == ScoreboardEntryType::OutstandingStore))
			{
				return true;
			}
		}
		return false;
	}

	inline bool MicroOpScheduler::IsBankPendingGlobal(int bankId)
	{
		for (int vt = 0; vt < SMT_WAYS; vt++)
		{
			if (IsBankPendingForVT(bankId, vt))
			{
				return true;
			}
		}
		return false;
	}

	int MicroOpScheduler::GetOutstandingMemoryCount(int virtualThreadId)
	{
		if (static_cast<unsigned int>(virtualThreadId) >= SMT_WAYS)
		{
			return 0;
		}

		int count = 0;
		for (int i = 0; i < SCOREBOARD_SLOTS; i++)
		{
			ScoreboardEntryType entryType = _smtScoreboardType[virtualThreadId][i];
			if (entryType == ScoreboardEntryType::OutstandingLoad || entryType == ScoreboardEntryType::OutstandingStore)
			{
				count++;
			}
		}
		return count;
	}

	inline int MicroOpScheduler::GetFreeSmtScoreboardSlotCount(int virtualThreadId)
	{
		if (static_cast<unsigned int>(virtualThreadId) >= SMT_WAYS)
		{
			return 0;
		}

		int count = 0;
		for (int i = 0; i < SCOREBOARD_SLOTS; i++)
		{
			if (_smtScoreboard[virtualThreadId][i] == -1)
			{
				count++;
			}
		}

		return count;
	}

	long long MicroOpScheduler::getMshrScoreboardStalls() const
	{
		return MshrScoreboardStalls;
	}

	void MicroOpScheduler::setMshrScoreboardStalls(long long value)
	{
		MshrScoreboardStalls = value;
	}

	std::optional<std::shared_ptr<DomainHypercallCompositionResult>> MicroOpScheduler::getLastExactVirtualizationCompositionResult() const
	{
		return LastExactVirtualizationCompositionResult;
	}

	void MicroOpScheduler::setLastExactVirtualizationCompositionResult(const std::optional<std::shared_ptr<DomainHypercallCompositionResult>> &value)
	{
		LastExactVirtualizationCompositionResult = value;
	}

	std::shared_ptr<SafetyVerifier> MicroOpScheduler::getExactVirtualizationCanonicalVerifier() const
	{
		return ((std::dynamic_pointer_cast<RuntimeLegalityService>(_runtimeLegalityService)) == nullptr ? nullptr : (std::dynamic_pointer_cast<RuntimeLegalityService>(_runtimeLegalityService))->getCanonicalVirtualizationVerifier());
	}

	bool MicroOpScheduler::getHasExactVirtualizationComposition() const
	{
		return _exactVirtualizationComposition != nullptr;
	}

	void MicroOpScheduler::ConfigureExactVirtualizationComposition(std::shared_ptr<DomainHypercallCanonicalComposition> composition)
	{
		ArgumentNullException::ThrowIfNull(composition);
		if (_exactVirtualizationComposition != nullptr)
		{
			throw InvalidOperationException(L"Exact virtualization composition is already configured.");
		}

		_exactVirtualizationComposition = composition;
		setLastExactVirtualizationCompositionResult(std::nullopt);
	}

	void MicroOpScheduler::DisableExactVirtualizationComposition()
	{
		if (_exactVirtualizationComposition != nullptr)
		{
			_exactVirtualizationComposition->Disable();
		}
		_exactVirtualizationComposition = nullptr;
		setLastExactVirtualizationCompositionResult(std::nullopt);
	}

// C# TO C++ CONVERTER TASK: C++ does not support 'partial' methods:
// ORIGINAL LINE: partial void OnCanonicalVirtualizationOperandMaterialized(BundleIssuePacket issuePacket, IssuePacketLane issueLane, SmtBundleMetadata4Way bundleMetadata, VmxMicroOp carrier, SafetyVerifier.VirtualizationAdmissionCertificate e1, VirtualizationOperandSnapshot operand)
	void MicroOpScheduler::OnCanonicalVirtualizationOperandMaterialized(BundleIssuePacket issuePacket, IssuePacketLane issueLane, SmtBundleMetadata4Way bundleMetadata, std::shared_ptr<VmxMicroOp> carrier, std::shared_ptr<VirtualizationAdmissionCertificate> e1, std::shared_ptr<VirtualizationOperandSnapshot> operand)
	{
		(void)issuePacket;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: DomainHypercallCanonicalComposition? composition = _exactVirtualizationComposition;
		std::shared_ptr<DomainHypercallCanonicalComposition> composition = _exactVirtualizationComposition;
		if (composition == nullptr)
		{
			return;
		}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: SafetyVerifier? verifier = ExactVirtualizationCanonicalVerifier;
		std::shared_ptr<SafetyVerifier> verifier = getExactVirtualizationCanonicalVerifier();
		if (verifier == nullptr)
		{
			setLastExactVirtualizationCompositionResult(std::make_shared<DomainHypercallCompositionResult>(DomainHypercallCompositionDecision::MissingCanonicalVerifier, nullptr, L"Only the canonical RuntimeLegalityService SafetyVerifier may compose E4."));
			return;
		}

		setLastExactVirtualizationCompositionResult(composition->Prepare(verifier, _currentReplayPhase, bundleMetadata, carrier, issueLane.getSlotIndex(), issueLane.getPhysicalLaneIndex(), e1, operand));
	}

	std::optional<std::shared_ptr<VmReadScalarDeliveryResult>> MicroOpScheduler::getLastVmReadScalarDeliveryResult() const
	{
		return LastVmReadScalarDeliveryResult;
	}

	void MicroOpScheduler::setLastVmReadScalarDeliveryResult(const std::optional<std::shared_ptr<VmReadScalarDeliveryResult>> &value)
	{
		LastVmReadScalarDeliveryResult = value;
	}

	bool MicroOpScheduler::getHasExactVmReadScalarDelivery() const
	{
		return _exactVmReadScalarDelivery != nullptr;
	}

	bool MicroOpScheduler::getHasExactGuestPcSpFlagsVmReadScalarDelivery() const
	{
		return _exactGuestPcSpFlagsVmReadScalarDelivery != nullptr;
	}

	bool MicroOpScheduler::getHasExactMemoryOwnedVmReadScalarDelivery() const
	{
		return _exactMemoryOwnedVmReadScalarDelivery != nullptr;
	}

	void MicroOpScheduler::ConfigureExactVmReadScalarDelivery(std::shared_ptr<VmReadScalarDeliveryCanonicalComposition> composition)
	{
		ArgumentNullException::ThrowIfNull(composition);
		if (_exactVmReadScalarDelivery != nullptr)
		{
			throw InvalidOperationException(L"Exact VMREAD scalar delivery is already configured.");
		}
		if (!composition->isEnabled())
		{
			throw InvalidOperationException(L"Exact VMREAD scalar delivery must be enabled before scheduler binding.");
		}
		composition->ObserveReplayPhase(_currentReplayPhase);
		_exactVmReadScalarDelivery = composition;
		setLastVmReadScalarDeliveryResult(std::nullopt);
	}

	void MicroOpScheduler::DisableExactVmReadScalarDelivery()
	{
		if (_exactVmReadScalarDelivery != nullptr)
		{
			_exactVmReadScalarDelivery->Disable();
		}
		_exactVmReadScalarDelivery = nullptr;
		setLastVmReadScalarDeliveryResult(std::nullopt);
	}

	void MicroOpScheduler::ConfigureExactGuestPcSpFlagsVmReadScalarDelivery(std::shared_ptr<GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition> composition)
	{
		ArgumentNullException::ThrowIfNull(composition);
		if (_exactGuestPcSpFlagsVmReadScalarDelivery != nullptr)
		{
			throw InvalidOperationException(L"Exact GuestPc/GuestSp/GuestFlags VMREAD scalar delivery is already configured.");
		}
		if (!composition->isEnabled())
		{
			throw InvalidOperationException(L"Exact GuestPc/GuestSp/GuestFlags VMREAD scalar delivery must be enabled before scheduler binding.");
		}
		composition->ObserveReplayPhase(_currentReplayPhase);
		_exactGuestPcSpFlagsVmReadScalarDelivery = composition;
		setLastVmReadScalarDeliveryResult(std::nullopt);
	}

	void MicroOpScheduler::DisableExactGuestPcSpFlagsVmReadScalarDelivery()
	{
		if (_exactGuestPcSpFlagsVmReadScalarDelivery != nullptr)
		{
			_exactGuestPcSpFlagsVmReadScalarDelivery->Disable();
		}
		_exactGuestPcSpFlagsVmReadScalarDelivery = nullptr;
		setLastVmReadScalarDeliveryResult(std::nullopt);
	}

	void MicroOpScheduler::ConfigureExactMemoryOwnedVmReadScalarDelivery(std::shared_ptr<MemoryOwnedVmReadScalarDeliveryCanonicalComposition> composition)
	{
		ArgumentNullException::ThrowIfNull(composition);
		if (_exactMemoryOwnedVmReadScalarDelivery != nullptr)
		{
			throw InvalidOperationException(L"Exact memory-owned VMREAD scalar delivery is already configured.");
		}
		if (!composition->isEnabled())
		{
			throw InvalidOperationException(L"Exact memory-owned VMREAD scalar delivery must be enabled before scheduler binding.");
		}
		composition->ObserveReplayPhase(_currentReplayPhase);
		_exactMemoryOwnedVmReadScalarDelivery = composition;
		setLastVmReadScalarDeliveryResult(std::nullopt);
	}

	void MicroOpScheduler::DisableExactMemoryOwnedVmReadScalarDelivery()
	{
		if (_exactMemoryOwnedVmReadScalarDelivery != nullptr)
		{
			_exactMemoryOwnedVmReadScalarDelivery->Disable();
		}
		_exactMemoryOwnedVmReadScalarDelivery = nullptr;
		setLastVmReadScalarDeliveryResult(std::nullopt);
	}

	bool MicroOpScheduler::TryPrepareVmReadScalarAfterCanonicalValueRead(BundleIssuePacket issuePacket, IssuePacketLane issueLane, unsigned long long fieldSelector, unsigned long long restoreGeneration)
	{
		(void)issuePacket;
		std::shared_ptr<VmxMicroOp> carrier = std::dynamic_pointer_cast<VmxMicroOp>(issueLane.getMicroOp());
		VmxOperationKind operation;
		if (carrier == nullptr || carrier->getVirtualizationAdmission() == nullptr || !carrier->TryResolveFrozenOperation(operation) || operation != VmxOperationKind::VmRead || !issueLane.isOccupied() || issueLane.getPhysicalLaneIndex() != 7)
		{
			return false;
		}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: System.Nullable<VmcsField> selectedField = fieldSelector <= ushort.MaxValue ? unchecked((VmcsField)(ushort)fieldSelector) : null;
		std::optional<VmcsField> selectedField;
		if (fieldSelector <= std::numeric_limits<unsigned short>::max()) selectedField = static_cast<VmcsField>(static_cast<unsigned short>(fieldSelector));
		std::shared_ptr<VmReadScalarAttemptBinding> attempt = std::make_shared<VmReadScalarAttemptBinding>(carrier->getVirtualizationAdmission());
		if ((selectedField == VmcsField::GuestCr3 || selectedField == VmcsField::EptPointer || selectedField == VmcsField::Vpid || selectedField == VmcsField::Cr3TargetCount))
		{
			if (_exactMemoryOwnedVmReadScalarDelivery != nullptr)
			{
				setLastVmReadScalarDeliveryResult(_exactMemoryOwnedVmReadScalarDelivery->Prepare(_currentReplayPhase, carrier, attempt, fieldSelector, restoreGeneration));
			}
			else if (_exactGuestPcSpFlagsVmReadScalarDelivery != nullptr)
			{
				setLastVmReadScalarDeliveryResult(_exactGuestPcSpFlagsVmReadScalarDelivery->Prepare(_currentReplayPhase, carrier, attempt, fieldSelector, restoreGeneration));
			}
			else if (_exactVmReadScalarDelivery != nullptr)
			{
				setLastVmReadScalarDeliveryResult(_exactVmReadScalarDelivery->Prepare(_currentReplayPhase, carrier, carrier->getVirtualizationAdmission(), fieldSelector, restoreGeneration));
			}
			else
			{
				return false;
			}
		}
		else if ((selectedField == VmcsField::GuestPc || selectedField == VmcsField::GuestSp || selectedField == VmcsField::GuestFlags))
		{
			if (_exactGuestPcSpFlagsVmReadScalarDelivery != nullptr)
			{
				setLastVmReadScalarDeliveryResult(_exactGuestPcSpFlagsVmReadScalarDelivery->Prepare(_currentReplayPhase, carrier, attempt, fieldSelector, restoreGeneration));
			}
			else if (_exactVmReadScalarDelivery != nullptr)
			{
				setLastVmReadScalarDeliveryResult(_exactVmReadScalarDelivery->Prepare(_currentReplayPhase, carrier, carrier->getVirtualizationAdmission(), fieldSelector, restoreGeneration));
			}
			else if (_exactMemoryOwnedVmReadScalarDelivery != nullptr)
			{
				setLastVmReadScalarDeliveryResult(_exactMemoryOwnedVmReadScalarDelivery->Prepare(_currentReplayPhase, carrier, attempt, fieldSelector, restoreGeneration));
			}
			else
			{
				return false;
			}
		}
		else if ((selectedField == VmcsField::GuestCr0 || selectedField == VmcsField::GuestCr4))
		{
			if (_exactVmReadScalarDelivery != nullptr)
			{
				setLastVmReadScalarDeliveryResult(_exactVmReadScalarDelivery->Prepare(_currentReplayPhase, carrier, carrier->getVirtualizationAdmission(), fieldSelector, restoreGeneration));
			}
			else if (_exactGuestPcSpFlagsVmReadScalarDelivery != nullptr)
			{
				setLastVmReadScalarDeliveryResult(_exactGuestPcSpFlagsVmReadScalarDelivery->Prepare(_currentReplayPhase, carrier, attempt, fieldSelector, restoreGeneration));
			}
			else if (_exactMemoryOwnedVmReadScalarDelivery != nullptr)
			{
				setLastVmReadScalarDeliveryResult(_exactMemoryOwnedVmReadScalarDelivery->Prepare(_currentReplayPhase, carrier, attempt, fieldSelector, restoreGeneration));
			}
			else
			{
				return false;
			}
		}
		else if (_exactMemoryOwnedVmReadScalarDelivery != nullptr)
		{
			setLastVmReadScalarDeliveryResult(_exactMemoryOwnedVmReadScalarDelivery->Prepare(_currentReplayPhase, carrier, attempt, fieldSelector, restoreGeneration));
		}
		else if (_exactGuestPcSpFlagsVmReadScalarDelivery != nullptr)
		{
			setLastVmReadScalarDeliveryResult(_exactGuestPcSpFlagsVmReadScalarDelivery->Prepare(_currentReplayPhase, carrier, attempt, fieldSelector, restoreGeneration));
		}
		else if (_exactVmReadScalarDelivery != nullptr)
		{
			setLastVmReadScalarDeliveryResult(_exactVmReadScalarDelivery->Prepare(_currentReplayPhase, carrier, carrier->getVirtualizationAdmission(), fieldSelector, restoreGeneration));
		}
		else
		{
			return false;
		}
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		auto deliveryResult = getLastVmReadScalarDeliveryResult();
		if (!deliveryResult.has_value() || !deliveryResult.value() || !deliveryResult.value()->IsPrepared || !deliveryResult.value()->Receipt)
		{
			return false;
		}
		carrier->AttachVmReadScalarResultReceipt(deliveryResult.value()->Receipt, fieldSelector);
		return true;
	}

	bool MicroOpScheduler::TryPrepareCompletionReasonQualificationVmReadAfterCanonicalValueRead(std::shared_ptr<CompletionReasonQualificationVmReadScalarDeliveryCanonicalComposition> composition, IssuePacketLane issueLane, unsigned long long fieldSelector, unsigned long long restoreGeneration)
	{
		ArgumentNullException::ThrowIfNull(composition);
		std::shared_ptr<VmxMicroOp> carrier = std::dynamic_pointer_cast<VmxMicroOp>(issueLane.getMicroOp());
		if (carrier == nullptr || carrier->getVirtualizationAdmission() == nullptr || !issueLane.isOccupied() || issueLane.getPhysicalLaneIndex() != 7)
		{
			return false;
		}
		setLastVmReadScalarDeliveryResult(composition->Prepare(_currentReplayPhase, carrier, std::make_shared<VmReadScalarAttemptBinding>(carrier->getVirtualizationAdmission()), fieldSelector, restoreGeneration));
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		auto deliveryResult = getLastVmReadScalarDeliveryResult();
		if (!deliveryResult.has_value() || !deliveryResult.value() || !deliveryResult.value()->IsPrepared || !deliveryResult.value()->Receipt)
		{
			return false;
		}
		carrier->AttachVmReadScalarResultReceipt(deliveryResult.value()->Receipt, fieldSelector);
		return true;
	}

	bool MicroOpScheduler::TryPrepareCompletionSecondStageVmReadAfterCanonicalValueRead(std::shared_ptr<CompletionSecondStageVmReadScalarDeliveryCanonicalComposition> composition, IssuePacketLane issueLane, unsigned long long fieldSelector, unsigned long long restoreGeneration)
	{
		ArgumentNullException::ThrowIfNull(composition);
		std::shared_ptr<VmxMicroOp> carrier = std::dynamic_pointer_cast<VmxMicroOp>(issueLane.getMicroOp());
		if (carrier == nullptr || carrier->getVirtualizationAdmission() == nullptr || !issueLane.isOccupied() || issueLane.getPhysicalLaneIndex() != 7)
		{
			return false;
		}
		setLastVmReadScalarDeliveryResult(composition->Prepare(_currentReplayPhase, carrier, std::make_shared<VmReadScalarAttemptBinding>(carrier->getVirtualizationAdmission()), fieldSelector, restoreGeneration));
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		auto deliveryResult = getLastVmReadScalarDeliveryResult();
		if (!deliveryResult.has_value() || !deliveryResult.value() || !deliveryResult.value()->IsPrepared || !deliveryResult.value()->Receipt)
		{
			return false;
		}
		carrier->AttachVmReadScalarResultReceipt(deliveryResult.value()->Receipt, fieldSelector);
		return true;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool TryPreparePinBasedControlsVmReadAfterCanonicalValueRead(PinBasedControlsVmReadScalarDeliveryCanonicalComposition? composition, IssuePacketLane issueLane, ulong fieldSelector, ulong restoreGeneration)
	bool MicroOpScheduler::TryPreparePinBasedControlsVmReadAfterCanonicalValueRead(std::shared_ptr<PinBasedControlsVmReadScalarDeliveryCanonicalComposition> composition, IssuePacketLane issueLane, unsigned long long fieldSelector, unsigned long long restoreGeneration)
	{
		std::shared_ptr<VmxMicroOp> carrier = std::dynamic_pointer_cast<VmxMicroOp>(issueLane.getMicroOp());
		if (composition == nullptr || carrier == nullptr || carrier->getVirtualizationAdmission() == nullptr || !issueLane.isOccupied() || issueLane.getPhysicalLaneIndex() != 7)
		{
			return false;
		}
		setLastVmReadScalarDeliveryResult(composition->Prepare(_currentReplayPhase, carrier, std::make_shared<VmReadScalarAttemptBinding>(carrier->getVirtualizationAdmission()), fieldSelector, restoreGeneration));
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		auto deliveryResult = getLastVmReadScalarDeliveryResult();
		if (!deliveryResult.has_value() || !deliveryResult.value() || !deliveryResult.value()->IsPrepared || !deliveryResult.value()->Receipt)
		{
			return false;
		}
		carrier->AttachVmReadScalarResultReceipt(deliveryResult.value()->Receipt, fieldSelector);
		return true;
	}

	std::optional<std::shared_ptr<ResearchVirtualizationCanonicalCompositionResult>> MicroOpScheduler::getLastResearchVirtualizationCanonicalCompositionResult() const
	{
		return LastResearchVirtualizationCanonicalCompositionResult;
	}

	void MicroOpScheduler::setLastResearchVirtualizationCanonicalCompositionResult(const std::optional<std::shared_ptr<ResearchVirtualizationCanonicalCompositionResult>> &value)
	{
		LastResearchVirtualizationCanonicalCompositionResult = value;
	}

	void MicroOpScheduler::EnableResearchVirtualizationCanonicalComposition(std::shared_ptr<ResearchVirtualizationCanonicalIssueComposition> composition)
	{
		ArgumentNullException::ThrowIfNull(composition);
		_researchVirtualizationCanonicalComposition = composition;
		setLastResearchVirtualizationCanonicalCompositionResult(std::nullopt);
	}

	void MicroOpScheduler::DisableResearchVirtualizationCanonicalComposition()
	{
		_researchVirtualizationCanonicalComposition = nullptr;
		setLastResearchVirtualizationCanonicalCompositionResult(std::nullopt);
	}

	std::shared_ptr<SafetyVerifier> MicroOpScheduler::getResearchVirtualizationCanonicalVerifierForTesting() const
	{
		return ((std::dynamic_pointer_cast<RuntimeLegalityService>(_runtimeLegalityService)) == nullptr ? nullptr : (std::dynamic_pointer_cast<RuntimeLegalityService>(_runtimeLegalityService))->getResearchVirtualizationCanonicalVerifier());
	}

// C# TO C++ CONVERTER TASK: C++ does not support 'partial' methods:
// ORIGINAL LINE: partial void OnCanonicalVirtualizationAdmissionMaterialized(BundleIssuePacket issuePacket, IssuePacketLane issueLane, SmtBundleMetadata4Way bundleMetadata, VmxMicroOp carrier, SafetyVerifier.VirtualizationAdmissionCertificate e1)
	void MicroOpScheduler::OnCanonicalVirtualizationAdmissionMaterialized(BundleIssuePacket issuePacket, IssuePacketLane issueLane, SmtBundleMetadata4Way bundleMetadata, std::shared_ptr<VmxMicroOp> carrier, std::shared_ptr<VirtualizationAdmissionCertificate> e1)
	{
		(void)issuePacket;
#if defined(TESTING)
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: ResearchVirtualizationCanonicalIssueComposition? composition = _researchVirtualizationCanonicalComposition;
		std::shared_ptr<ResearchVirtualizationCanonicalIssueComposition> composition = _researchVirtualizationCanonicalComposition;
		if (composition == nullptr)
		{
			return;
		}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: SafetyVerifier? verifier = ResearchVirtualizationCanonicalVerifierForTesting;
		std::shared_ptr<SafetyVerifier> verifier = getResearchVirtualizationCanonicalVerifierForTesting();
		if (verifier == nullptr)
		{
			setLastResearchVirtualizationCanonicalCompositionResult(std::make_shared<ResearchVirtualizationCanonicalCompositionResult>(ResearchVirtualizationCanonicalCompositionDecision::DeniedNoCanonicalVerifier, std::nullopt, std::nullopt, std::nullopt, L"The canonical SafetyVerifier was unavailable at the TESTING-only issue seam."));
			return;
		}

		setLastResearchVirtualizationCanonicalCompositionResult(composition->Compose(verifier, _currentReplayPhase, bundleMetadata, carrier, issueLane.getSlotIndex(), issueLane.getPhysicalLaneIndex(), e1));
#else
		(void)issueLane;
		(void)bundleMetadata;
		(void)carrier;
		(void)e1;
#endif
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public MicroOp[] PackBundleIntraCoreSmt(System.Collections.Generic.IReadOnlyList<MicroOp?> bundle, int ownerVirtualThreadId, int localCoreId, byte eligibleVirtualThreadMask, System.Nullable<YAKSys_Hybrid_CPU.Memory.MemorySubsystem> memSub = null)
	std::vector<std::shared_ptr<MicroOp>> MicroOpScheduler::PackBundleIntraCoreSmt(const std::vector<std::shared_ptr<MicroOp>> &bundle, int ownerVirtualThreadId, int localCoreId, unsigned char eligibleVirtualThreadMask, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub)
	{
		if (bundle.empty() || bundle.size() != 8)
		{
			return std::vector<std::shared_ptr<MicroOp>>(bundle);
		}

		// Increment TDM global counter for the cycle
		GlobalCycleCounter++;

		std::vector<std::shared_ptr<MicroOp>> result(8);
		for (int slot = 0; slot < result.size(); slot++)
		{
			result[slot] = bundle[slot];
		}

		// Phase 02: Compute per-class occupancy from the current bundle
		_classCapacity = SlotClassCapacity::ComputeFromBundle(result);
		RecordLoopPhaseSample();

		// Compute cumulative safety mask of existing bundle operations
		// We use BundleResourceCertificate4Way to preserve per-VT register isolation!
		BundleResourceCertificate4Way bundleCert = BundleResourceCertificate4Way::Empty;
		SmtBundleMetadata4Way bundleMetadata = SmtBundleMetadata4Way::Empty(ownerVirtualThreadId);
		BoundaryGuardState boundaryGuard = BoundaryGuardState::Open(_serializingEpochCounter);
		for (int i = 0; i < 8; i++)
		{
			if (result[i] != nullptr)
			{
				bundleCert.AddOperation(result[i]);
				bundleMetadata = bundleMetadata.WithOperation(result[i]);
				boundaryGuard = boundaryGuard.WithOperation(result[i]);
			}
		}

		_runtimeLegalityService->PrepareSmt(_currentReplayPhase, bundleCert, bundleMetadata, boundaryGuard);
		unsigned char replayAwareMask = _currentReplayPhase.getHasStableDonorStructure() ? _currentReplayPhase.getStableDonorMask() : std::numeric_limits<unsigned char>::max();
		BundleOpportunityState opportunityState = BundleOpportunityState::Create(result);
		SmtNominationState nominationState = CreateEligibleSmtNominationState(eligibleVirtualThreadMask);
		RecordDeterminismTaxWindow(opportunityState.getReferenceOpportunityCount(), opportunityState.CountReplayEligibleReferenceOpportunities(replayAwareMask), CountPendingSmtCandidates(nominationState, ownerVirtualThreadId));

		// Next empty slot cursor
		int nextEmptySlot = ResolveNextInjectableSlot(opportunityState, 0);

		// 2-Stage Pipelined FSP path (HLS Timing Closure §1):
		// When pipelined mode is active AND SCHED2 is ready, use the
		// pre-decoded pipeline register bank instead of re-decoding candidates.
		if (getPipelinedFspEnabled() && _fspCurrentStage == FspPipelineStage::SCHED2)
		{
// C# TO C++ CONVERTER TASK: C++ has no equivalent to the C# deconstruction assignments:
			auto fspResult = PipelineFspStage2_Intersect(result, bundleCert, bundleMetadata, boundaryGuard, nextEmptySlot);
			bundleCert = fspResult.first;
			nextEmptySlot = fspResult.second;

			// Kick SCHED1 for the next cycle's candidates
			BundleResourceCertificate4Way nextBundleCert;
			SmtBundleMetadata4Way nextBundleMetadata;
			BoundaryGuardState nextBoundaryGuard;
			RebuildFspStageContext(result, ownerVirtualThreadId, nextBundleCert, nextBundleMetadata, nextBoundaryGuard);
			PipelineFspStage1_Nominate(ownerVirtualThreadId, CreateEligibleSmtNominationState(eligibleVirtualThreadMask), result, nextBundleCert, nextBundleMetadata, nextBoundaryGuard);

			ClearAssistNominationPorts();
			setTotalSchedulerCycles(getTotalSchedulerCycles() + 1);
			return result;
		}

		// If pipelined mode is active but SCHED1 hasn't run yet, prime the pipeline
		if (getPipelinedFspEnabled() && _fspCurrentStage == FspPipelineStage::SCHED1)
		{
			PipelineFspStage1_Nominate(ownerVirtualThreadId, nominationState, result, bundleCert, bundleMetadata, boundaryGuard);
			// First cycle: no injection yet (pipeline priming), return original bundle
			ClearAssistNominationPorts();
			setTotalSchedulerCycles(getTotalSchedulerCycles() + 1);
			return result;
		}

		// Advance to SCHED2 stage for pipelined checking
		setTotalSchedulerCycles(getTotalSchedulerCycles() + 1);
		PrepareProjectedMemoryIssuePass(result);

		// ── Single-cycle combinational path (original behavior) ──

		if (IsSmtBundleBlockedByBoundaryGuard(bundleCert, bundleMetadata, boundaryGuard))
		{
			setAssistBoundaryRejects(getAssistBoundaryRejects() + CountReadyAssistCandidates(ownerVirtualThreadId));
			ClearAssistNominationPorts();
			setTotalSchedulerCycles(getTotalSchedulerCycles() + 1);
			return result;
		}

		// TDM Logic: determine if this cycle is reserved for a background thread
		bool isTdmSlot = (GlobalCycleCounter % TDM_PERIOD) == 0;
		bool forceTdm = isTdmSlot && nominationState.getHasReadyBackgroundCandidates();

		// Phase 2A: Credit-based fairness ranking or legacy TDM ordering

		std::array<int, SMT_WAYS> vtOrderStorage{};
		std::span<int> vtOrder(vtOrderStorage);

		std::array<bool, SMT_WAYS> wasInjectedStorage{};
		std::span<bool> wasInjected(wasInjectedStorage);
		wasInjectedStorage.fill(false);

		if (getCreditFairnessEnabled())
		{
			GetCreditRankedOrder(ownerVirtualThreadId, vtOrder);
		}
		else
		{
			for (int i = 0; i < SMT_WAYS; i++)
			{
				vtOrder[i] = GetRankedVirtualThread(i, forceTdm);
			}
		}

		// Phase 2B: Collect candidates with pressure scores for tie-breaking.
		// Stage A / Stage B checks are deferred to Pass 2 (TryClassAdmission + TryMaterializeLane).

		std::array<int, SMT_WAYS> legalVtsStorage{};
		std::span<int> legalVts(legalVtsStorage);

		std::array<int, SMT_WAYS> legalPressureStorage{};
		std::span<int> legalPressure(legalPressureStorage);
		int legalCount = 0;

		for (int i = 0; i < SMT_WAYS; i++)
		{
			int vt = vtOrder[i];
			if (!nominationState.IsReadyNonOwnerCandidate(vt, ownerVirtualThreadId))
			{
				continue;
			}

			std::shared_ptr<MicroOp> candidate = _smtPorts[vt];
			if (candidate == nullptr)
			{
				continue;
			}

			std::shared_ptr<LoadStoreMicroOp> lsCandidate = std::dynamic_pointer_cast<LoadStoreMicroOp>(candidate);
			int candidateBankId = lsCandidate != nullptr ? lsCandidate->getMemoryBankId() : -1;

			int pressure = 0;
			if (getBankPressureTieBreakEnabled() && candidate->isMemoryOp() && candidateBankId >= 0)
			{
				pressure = GetBankPressureScore(candidateBankId);
			}

			legalVts[legalCount] = vt;
			legalPressure[legalCount] = pressure;
			legalCount++;
		}

		// Phase 2B: Sort legal candidates by pressure (ascending), preserving credit/vtId order on tie.
		if (getBankPressureTieBreakEnabled() && legalCount > 1)
		{
			for (int i = 0; i < legalCount - 1; i++)
			{
				for (int j = i + 1; j < legalCount; j++)
				{
					if (legalPressure[j] < legalPressure[i])
					{
// C# TO C++ CONVERTER TASK: C++ has no equivalent to the C# deconstruction assignments:
						(legalVts[i], legalVts[j]) = (legalVts[j], legalVts[i]);
// C# TO C++ CONVERTER TASK: C++ has no equivalent to the C# deconstruction assignments:
						(legalPressure[i], legalPressure[j]) = (legalPressure[j], legalPressure[i]);
						setBankPressureAvoidanceCount(getBankPressureAvoidanceCount() + 1);
					}
				}
			}
		}

		// Phase 5: Snapshot original bundle and SMT candidates for oracle gap analysis
		ShadowOracleCycleSnapshot oracleSnapshot = YAKSys_Hybrid_CPU::Core::MicroOpScheduler::ShadowOracleCycleSnapshot();
		if (getEnableShadowOracle())
		{
			oracleSnapshot = CaptureShadowOracleCycleSnapshot(result, nominationState);
		}

		int templateDomainScopeId = ResolveClassTemplateDomainScopeId(bundleMetadata);

		// Pass 2: Injection (guarded by TypedSlotEnabled)
		unsigned char bundleOccupancy = opportunityState.getOccupancyMask();
		int realInjectedCount = 0;

		// Phase 07: Level 1 class-template match at cycle start (gated by TypedSlotEnabled)
		ClassTemplateAdmissionState templateState = getTypedSlotEnabled() ? PrepareClassTemplateAdmissionState(templateDomainScopeId) : YAKSys_Hybrid_CPU::Core::MicroOpScheduler::ClassTemplateAdmissionState();

		for (int i = 0; i < legalCount; i++)
		{
			int vt = legalVts[i];
			std::shared_ptr<MicroOp> candidate = _smtPorts[vt];
			if (candidate == nullptr)
			{
				continue;
			}

			if (getTypedSlotEnabled())
			{
				// Phase 06: Two-stage class-admission + lane-materialization path
				TypedSlotRejectReason rejectA;
				if (!TryClassAdmission(candidate, bundleCert, bundleMetadata, boundaryGuard, templateState, ownerVirtualThreadId, realInjectedCount, rejectA))
				{
					RecordTypedSlotReject(rejectA, candidate);
					continue;
				}

				int lane;
				TypedSlotRejectReason rejectB;
				if (!TryMaterializeLane(candidate, bundleOccupancy, lane, rejectB))
				{
					RecordTypedSlotReject(rejectB, candidate);
					continue;
				}

				// Commit
				result[lane] = candidate;
				MaterializePostStageBIssuedAttempt(candidate, lane, bundleMetadata);
				candidate->setFspInjected(true);
				bundleCert.AddOperation(candidate);
				opportunityState = opportunityState.WithOccupiedSlot(lane);
				bundleOccupancy |= static_cast<unsigned char>(1 << lane);
				setSmtInjectionsCount(getSmtInjectionsCount() + 1);
				realInjectedCount++;
				wasInjected[vt] = true;
				_perVtInjections[vt]++;
				RecordTypedSlotInject(candidate, lane);
				bundleMetadata = bundleMetadata.WithOperation(candidate);
				boundaryGuard = boundaryGuard.WithOperation(candidate);
				_runtimeLegalityService->RefreshSmtAfterMutation(_currentReplayPhase, bundleCert, bundleMetadata, boundaryGuard);

				_classCapacity.IncrementOccupancy(candidate->getPlacement().RequiredSlotClass);
				ConsumeProjectedMemoryIssueStateIfNeeded(candidate);

				if (candidate->getPlacement().PinningKind == SlotPinningKind::ClassFlexible)
				{
					RecordPhaseLane(candidate->getPlacement().RequiredSlotClass, lane);
				}

				// Phase 07: Capture class template on first successful typed-slot injection
				if (_currentReplayPhase.isActive() && !_classTemplateValid)
				{
					CaptureClassTemplate(templateDomainScopeId);
				}
			}
			else
			{
				// Legacy path: exact slot search + CanInject
				int slot = ResolveNextInjectableSlot(opportunityState, 0);
				if (slot < 0)
				{
					continue;
				}

				LegalityDecision legalityDecision = EvaluateSmtLegality(bundleCert, bundleMetadata, boundaryGuard, candidate);
				if (!legalityDecision.isAllowed())
				{
					RecordPerVtRejection(candidate->getVirtualThreadId());
					setSmtRejectionsCount(getSmtRejectionsCount() + 1);
					continue;
				}

				std::shared_ptr<LoadStoreMicroOp> ls = std::dynamic_pointer_cast<LoadStoreMicroOp>(candidate);
				int candidateBankId = ls != nullptr ? ls->getMemoryBankId() : -1;
				TypedSlotRejectReason outerCapReject = TypedSlotRejectReason::None;
				if (!TryPassOuterCap(candidate, candidate->getVirtualThreadId(), candidate->isMemoryOp(), candidateBankId, ownerVirtualThreadId, outerCapReject))
				{
					RecordPerVtRejection(candidate->getVirtualThreadId());
					continue;
				}

				// Commit (legacy)
				result[slot] = candidate;
				candidate->setFspInjected(true);
				bundleCert.AddOperation(candidate);
				opportunityState = opportunityState.WithOccupiedSlot(slot);
				bundleOccupancy = opportunityState.getOccupancyMask();
				setSmtInjectionsCount(getSmtInjectionsCount() + 1);
				realInjectedCount++;
				wasInjected[vt] = true;
				_perVtInjections[vt]++;
				RecordTypedSlotInject(candidate, slot);
				bundleMetadata = bundleMetadata.WithOperation(candidate);
				boundaryGuard = boundaryGuard.WithOperation(candidate);
				_runtimeLegalityService->RefreshSmtAfterMutation(_currentReplayPhase, bundleCert, bundleMetadata, boundaryGuard);
				ConsumeProjectedMemoryIssueStateIfNeeded(candidate);
			}

			_smtPorts[vt] = nullptr;
			_smtPortValid[vt] = false;
			nominationState = nominationState.WithoutCandidate(vt);
		}

		// Phase 2A: Update fairness credits after injection decisions
		if (getCreditFairnessEnabled())
		{
			AccumulateFairnessCredits(nominationState, ownerVirtualThreadId, wasInjected);
		}

		// Phase 5: Record oracle gap after real injection is complete
		if (getEnableShadowOracle())
		{
			RecordOracleGap(oracleSnapshot, ownerVirtualThreadId, realInjectedCount);
		}

		TryInjectAssistCandidates(result, bundleCert, bundleMetadata, boundaryGuard, templateState, opportunityState, bundleOccupancy, ownerVirtualThreadId, realInjectedCount, memSub);

		return result;
	}

	inline void MicroOpScheduler::MaterializePostStageBIssuedAttempt(std::shared_ptr<MicroOp> candidate, int lane, SmtBundleMetadata4Way bundleMetadata)
	{
		MaterializePostStageBIssuedAttempt(candidate, candidate->getPostStageBIdentityTemplate(), lane, bundleMetadata);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private void MaterializePostStageBIssuedAttempt(MicroOp candidate, PostStageBIdentityTemplate? template, int lane, SmtBundleMetadata4Way bundleMetadata)
	inline void MicroOpScheduler::MaterializePostStageBIssuedAttempt(std::shared_ptr<MicroOp> candidate, std::shared_ptr<PostStageBIdentityTemplate> template_Keyword, int lane, SmtBundleMetadata4Way bundleMetadata)
	{
		if (template_Keyword != nullptr)
		{
			if (candidate->getPostStageBIssuedAttempt() != nullptr)
			{
				throw InvalidOperationException(L"A MicroOp cannot receive a second issued-attempt carrier in one Stage-B commit.");
			}

			candidate->setPostStageBIssuedAttempt(PostStageBIssuedAttempt::CreateAfterSuccessfulStageB(template_Keyword, std::make_shared<LaneId>(LaneId::Create(lane))));
			candidate->setPostStageBIdentityTemplate(nullptr);
		}

		std::shared_ptr<VmxMicroOp> vmx = std::dynamic_pointer_cast<VmxMicroOp>(candidate);
		if (vmx == nullptr)
		{
			return;
		}

		if (_virtualizationAdmissionService == nullptr)
		{
			return;
		}

		std::shared_ptr<VirtualizationAdmissionIssueResult> issue = _virtualizationAdmissionService->IssueVirtualizationAdmissionAfterStageB(_currentReplayPhase, bundleMetadata, vmx, vmx->getPlacement().PinnedLaneId, lane);
		if (!issue->IsIssued || issue->Certificate == nullptr)
		{
			return;
		}

		std::shared_ptr<VirtualizationAdmissionValidationResult> validation = _virtualizationAdmissionService->ValidateVirtualizationAdmission(_currentReplayPhase, bundleMetadata, vmx, vmx->getPlacement().PinnedLaneId, lane, issue->Certificate);
		if (validation->IsValidForFaultOnlyTransport)
		{
			vmx->AttachVirtualizationAdmission(issue->Certificate);
		}
	}

	bool MicroOpScheduler::TryAttachVirtualizationAdmissionAfterCanonicalLaneMaterialization(BundleIssuePacket issuePacket, IssuePacketLane issueLane)
	{
		std::shared_ptr<VmxMicroOp> vmx = std::dynamic_pointer_cast<VmxMicroOp>(issueLane.getMicroOp());
		if (vmx == nullptr || _virtualizationAdmissionService == nullptr || !issueLane.isOccupied() || issueLane.getPhysicalLaneIndex() != 7)
		{
			return false;
		}

		SmtBundleMetadata4Way bundleMetadata = SmtBundleMetadata4Way::Empty(issueLane.getOwnerThreadId());
		for (unsigned char laneIndex = 0; laneIndex < 8; laneIndex++)
		{
			IssuePacketLane packetLane = issuePacket.GetPhysicalLane(laneIndex);
			if (packetLane.isOccupied())
			{
				bundleMetadata = bundleMetadata.WithOperation(packetLane.getMicroOp());
			}
		}

		std::shared_ptr<VirtualizationAdmissionIssueResult> issue = _virtualizationAdmissionService->IssueVirtualizationAdmissionAfterStageB(_currentReplayPhase, bundleMetadata, vmx, issueLane.getSlotIndex(), issueLane.getPhysicalLaneIndex());
		if (!issue->IsIssued || issue->Certificate == nullptr)
		{
			return false;
		}

		std::shared_ptr<VirtualizationAdmissionValidationResult> validation = _virtualizationAdmissionService->ValidateVirtualizationAdmission(_currentReplayPhase, bundleMetadata, vmx, issueLane.getSlotIndex(), issueLane.getPhysicalLaneIndex(), issue->Certificate);
		if (!validation->IsValidForFaultOnlyTransport)
		{
			return false;
		}

		vmx->AttachVirtualizationAdmission(issue->Certificate);
		OnCanonicalVirtualizationAdmissionMaterialized(issuePacket, issueLane, bundleMetadata, vmx, issue->Certificate);
		return true;
	}

	bool MicroOpScheduler::TryAttachVirtualizationOperandSnapshotAfterCanonicalValueRead(BundleIssuePacket issuePacket, IssuePacketLane issueLane, unsigned long long rs1Value, unsigned long long restoreGeneration)
	{
		std::shared_ptr<VmxMicroOp> vmx = std::dynamic_pointer_cast<VmxMicroOp>(issueLane.getMicroOp());
		if (vmx == nullptr || vmx->getVirtualizationAdmission() == nullptr || _virtualizationAdmissionService == nullptr || !issueLane.isOccupied() || issueLane.getPhysicalLaneIndex() != 7)
		{
			return false;
		}

		SmtBundleMetadata4Way bundleMetadata = SmtBundleMetadata4Way::Empty(issueLane.getOwnerThreadId());
		for (unsigned char laneIndex = 0; laneIndex < 8; laneIndex++)
		{
			IssuePacketLane packetLane = issuePacket.GetPhysicalLane(laneIndex);
			if (packetLane.isOccupied())
			{
				bundleMetadata = bundleMetadata.WithOperation(packetLane.getMicroOp());
			}
		}

		std::shared_ptr<VirtualizationOperandCaptureResult> result = _virtualizationAdmissionService->CaptureVirtualizationOperandsAfterE1Validation(_currentReplayPhase, bundleMetadata, vmx, issueLane.getSlotIndex(), issueLane.getPhysicalLaneIndex(), vmx->getVirtualizationAdmission(), rs1Value, restoreGeneration);
				if (!result->isCaptured() || result->Snapshot == nullptr)
		{
			return false;
		}

		vmx->AttachVirtualizationOperandSnapshot(result->Snapshot);
		OnCanonicalVirtualizationOperandMaterialized(issuePacket, issueLane, bundleMetadata, vmx, vmx->getVirtualizationAdmission(), result->Snapshot);
		return true;
	}

	bool MicroOpScheduler::getPipelinedFspEnabled() const
	{
		return PipelinedFspEnabled;
	}

	void MicroOpScheduler::setPipelinedFspEnabled(bool value)
	{
		PipelinedFspEnabled = value;
	}

	bool MicroOpScheduler::getTypedSlotEnabled() const
	{
		return TypedSlotEnabled;
	}

	void MicroOpScheduler::setTypedSlotEnabled(bool value)
	{
		TypedSlotEnabled = value;
	}

	void MicroOpScheduler::SetReplayPhaseContext(ReplayPhaseContext phase, bool invalidateAssistOnDeactivate)
	{
		// RF-12.9bb: a raw zero epoch cannot denote an active replay
		// phase. Canonicalize it before any replay telemetry, sampling or
		// reuse mutation, then consume the established inactive contour.
		if (phase.isActive() && phase.getEpochId() == 0)
		{
			ReplayPhaseInvalidationReason reason = phase.getLastInvalidationReason() != ReplayPhaseInvalidationReason::None ? phase.getLastInvalidationReason() : ReplayPhaseInvalidationReason::InactivePhase;
			phase = ReplayPhaseContext(false, 0, phase.getCachedPc(), phase.getEpochLength(), phase.getCompletedReplays(), phase.getValidSlotCount(), phase.getStableDonorMask(), reason);
		}

		_currentReplayPhase = phase;
		if (_exactVmReadScalarDelivery != nullptr)
		{
			_exactVmReadScalarDelivery->ObserveReplayPhase(phase);
		}
		if (_exactGuestPcSpFlagsVmReadScalarDelivery != nullptr)
		{
			_exactGuestPcSpFlagsVmReadScalarDelivery->ObserveReplayPhase(phase);
		}
		if (_exactMemoryOwnedVmReadScalarDelivery != nullptr)
		{
			_exactMemoryOwnedVmReadScalarDelivery->ObserveReplayPhase(phase);
		}
		if (phase.isActive())
		{
			setReplayAwareCycles(getReplayAwareCycles() + 1);
			RecordLoopPhaseEntry(phase);
		}

		if (!phase.isActive())
		{
			ReplayPhaseInvalidationReason deactivationReason = phase.getLastInvalidationReason() != ReplayPhaseInvalidationReason::None ? phase.getLastInvalidationReason() : ReplayPhaseInvalidationReason::InactivePhase;

			// Phase 04: Reset per-class lane records when replay phase deactivates.
			ResetPreviousPhaseLanes();
			if (invalidateAssistOnDeactivate)
			{
				InvalidateAssistNominationState(AssistInvalidationReason::Replay);
			}

			// Phase 07: Invalidate class template when replay phase deactivates.
			InvalidateClassTemplate(deactivationReason);

			InvalidatePhaseCertificateTemplates(deactivationReason, true, true);
			return;
		}

		// Phase 04: Reset per-class lane records when replay epoch changes.
		if (phase.getEpochId() != _previousPhaseEpochId)
		{
			ResetPreviousPhaseLanes();
			_previousPhaseEpochId = phase.getEpochId();
			InvalidateAssistNominationState(AssistInvalidationReason::Replay);

			// Phase 07: Invalidate class template on epoch transition.
			InvalidateClassTemplate(ReplayPhaseInvalidationReason::ClassTemplateExpired);
		}

		_runtimeLegalityService->InvalidatePhaseMismatch(phase);
	}

	void MicroOpScheduler::SetHardwareOccupancySnapshot(HardwareOccupancySnapshot128 snapshot)
	{
		_hardwareOccupancySnapshot = snapshot;
	}

	void MicroOpScheduler::NotifySerializingCommit(bool invalidateAssist)
	{
		_serializingEpochCounter++;
		if (invalidateAssist)
		{
			InvalidateAssistNominationState(AssistInvalidationReason::SerializingBoundary);
		}

		// G35/G36: Invalidate all phase-certificate templates on serialising event.
		// No replay certificate may span a barrier, trap, or VM-transition boundary.
		InvalidatePhaseCertificateTemplates(ReplayPhaseInvalidationReason::SerializingEvent, true, true);

		// G35: Invalidate class-capacity template so typed-slot fast-path cannot
		// carry stale capacity assumptions across a serialising boundary.
		InvalidateClassTemplate(ReplayPhaseInvalidationReason::SerializingEvent);
	}

	SchedulerPhaseMetrics MicroOpScheduler::GetPhaseMetrics()
	{
		SchedulerPhaseMetrics temp_var = SchedulerPhaseMetrics();
		temp_var.setReplayAwareCycles(getReplayAwareCycles());
		temp_var.setPhaseCertificateReadyHits(getPhaseCertificateReadyHits());
		temp_var.setPhaseCertificateReadyMisses(getPhaseCertificateReadyMisses());
		temp_var.setEstimatedChecksSaved(getEstimatedPhaseCertificateChecksSaved());
		temp_var.setPhaseCertificateInvalidations(getPhaseCertificateInvalidations());
		temp_var.setPhaseCertificateMutationInvalidations(getPhaseCertificateMutationInvalidations());
		temp_var.setPhaseCertificatePhaseMismatchInvalidations(getPhaseCertificatePhaseMismatchInvalidations());
		temp_var.setLastCertificateInvalidationReason(getLastPhaseCertificateInvalidationReason());
		temp_var.setDeterminismReferenceOpportunitySlots(getDeterminismReferenceOpportunitySlots());
		temp_var.setDeterminismReplayEligibleSlots(getDeterminismReplayEligibleSlots());
		temp_var.setDeterminismMaskedSlots(getDeterminismMaskedSlots());
		temp_var.setDeterminismEstimatedLostSlots(getDeterminismEstimatedLostSlots());
		temp_var.setDeterminismConstrainedCycles(getDeterminismConstrainedCycles());
		temp_var.setDomainIsolationProbeAttempts(getDomainIsolationProbeAttempts());
		temp_var.setDomainIsolationBlockedAttempts(getDomainIsolationBlockedAttempts());
		temp_var.setDomainIsolationCrossDomainBlocks(getDomainIsolationCrossDomainBlocks());
		temp_var.setDomainIsolationKernelToUserBlocks(getDomainIsolationKernelToUserBlocks());
		temp_var.setEligibilityMaskedCycles(getEligibilityMaskedCycles());
		temp_var.setEligibilityMaskedReadyCandidates(getEligibilityMaskedReadyCandidates());
		temp_var.setLastEligibilityRequestedMask(_lastSmtEligibilitySnapshot->RequestedMask);
		temp_var.setLastEligibilityNormalizedMask(_lastSmtEligibilitySnapshot->NormalizedMask);
		temp_var.setLastEligibilityReadyPortMask(_lastSmtEligibilitySnapshot->ReadyPortMask);
		temp_var.setLastEligibilityVisibleReadyMask(_lastSmtEligibilitySnapshot->VisibleReadyMask);
		temp_var.setLastEligibilityMaskedReadyMask(_lastSmtEligibilitySnapshot->MaskedReadyMask);
		temp_var.setClassTemplateReuseHits(getClassTemplateReuseHits());
		temp_var.setClassTemplateInvalidations(getClassTemplateInvalidations());
		temp_var.setTypedSlotFastPathAccepts(getTypedSlotFastPathAccepts());
		temp_var.setTypedSlotStandardPathAccepts(getTypedSlotStandardPathAccepts());
		temp_var.setTotalLaneBindings(getTotalLaneBindings());
		temp_var.setLaneReuseHits(getLaneReuseHits());
		temp_var.setLaneReuseMisses(getLaneReuseMisses());
		temp_var.setAluClassInjects(getAluClassInjects());
		temp_var.setLsuClassInjects(getLsuClassInjects());
		temp_var.setDmaStreamClassInjects(getDmaStreamClassInjects());
		temp_var.setMatrixTileStreamClassInjects(getMatrixTileStreamClassInjects());
		temp_var.setBranchControlInjects(getBranchControlInjects());
		temp_var.setHardPinnedInjects(getHardPinnedInjects());
		temp_var.setClassFlexibleInjects(getClassFlexibleInjects());
		temp_var.setNopAvoided(getNopAvoided());
		temp_var.setNopDueToPinnedConstraint(getNopDueToPinnedConstraint());
		temp_var.setNopDueToNoClassCapacity(getNopDueToNoClassCapacity());
		temp_var.setNopDueToResourceConflict(getNopDueToResourceConflict());
		temp_var.setNopDueToDynamicState(getNopDueToDynamicState());
		temp_var.setStaticClassOvercommitRejects(getStaticClassOvercommitRejects());
		temp_var.setDynamicClassExhaustionRejects(getDynamicClassExhaustionRejects());
		temp_var.setPinnedLaneConflicts(getPinnedLaneConflicts());
		temp_var.setLateBindingConflicts(getLateBindingConflicts());
		temp_var.setTypedSlotDomainRejects(getTypedSlotDomainRejects());
		temp_var.setSmtOwnerContextGuardRejects(getSmtOwnerContextGuardRejects());
		temp_var.setSmtDomainGuardRejects(getSmtDomainGuardRejects());
		temp_var.setSmtBoundaryGuardRejects(getSmtBoundaryGuardRejects());
		temp_var.setSmtSharedResourceCertificateRejects(getSmtSharedResourceCertificateRejects());
		temp_var.setSmtRegisterGroupCertificateRejects(getSmtRegisterGroupCertificateRejects());
		temp_var.setLastSmtLegalityRejectKind(getLastSmtLegalityRejectKind());
		temp_var.setLastSmtLegalityAuthoritySource(getLastSmtLegalityAuthoritySource());
		temp_var.setSmtLegalityRejectByAluClass(getSmtLegalityRejectByAluClass());
		temp_var.setSmtLegalityRejectByLsuClass(getSmtLegalityRejectByLsuClass());
		temp_var.setSmtLegalityRejectByDmaStreamClass(getSmtLegalityRejectByDmaStreamClass());
		temp_var.setSmtLegalityRejectByMatrixTileStreamClass(getSmtLegalityRejectByMatrixTileStreamClass());
		temp_var.setSmtLegalityRejectByBranchControl(getSmtLegalityRejectByBranchControl());
		temp_var.setSmtLegalityRejectBySystemSingleton(getSmtLegalityRejectBySystemSingleton());
		temp_var.setClassTemplateDomainInvalidations(getClassTemplateDomainInvalidations());
		temp_var.setClassTemplateCapacityMismatchInvalidations(getClassTemplateCapacityMismatchInvalidations());
		temp_var.setCertificateRejectByAluClass(getCertificateRejectByAluClass());
		temp_var.setCertificateRejectByLsuClass(getCertificateRejectByLsuClass());
		temp_var.setCertificateRejectByDmaStreamClass(getCertificateRejectByDmaStreamClass());
		temp_var.setCertificateRejectByMatrixTileStreamClass(getCertificateRejectByMatrixTileStreamClass());
		temp_var.setCertificateRejectByBranchControl(getCertificateRejectByBranchControl());
		temp_var.setCertificateRejectBySystemSingleton(getCertificateRejectBySystemSingleton());
		temp_var.setCertificateRegGroupConflictVT0(getCertificateRegGroupConflictVT0());
		temp_var.setCertificateRegGroupConflictVT1(getCertificateRegGroupConflictVT1());
		temp_var.setCertificateRegGroupConflictVT2(getCertificateRegGroupConflictVT2());
		temp_var.setCertificateRegGroupConflictVT3(getCertificateRegGroupConflictVT3());
		temp_var.setRejectionsVT0(getRejectionsVT0());
		temp_var.setRejectionsVT1(getRejectionsVT1());
		temp_var.setRejectionsVT2(getRejectionsVT2());
		temp_var.setRejectionsVT3(getRejectionsVT3());
		temp_var.setRegGroupConflictsVT0(getRegGroupConflictsVT0());
		temp_var.setRegGroupConflictsVT1(getRegGroupConflictsVT1());
		temp_var.setRegGroupConflictsVT2(getRegGroupConflictsVT2());
		temp_var.setRegGroupConflictsVT3(getRegGroupConflictsVT3());
		temp_var.setBankPendingRejectBank0(getBankPendingRejectBank0());
		temp_var.setBankPendingRejectBank1(getBankPendingRejectBank1());
		temp_var.setBankPendingRejectBank2(getBankPendingRejectBank2());
		temp_var.setBankPendingRejectBank3(getBankPendingRejectBank3());
		temp_var.setBankPendingRejectBank4(getBankPendingRejectBank4());
		temp_var.setBankPendingRejectBank5(getBankPendingRejectBank5());
		temp_var.setBankPendingRejectBank6(getBankPendingRejectBank6());
		temp_var.setBankPendingRejectBank7(getBankPendingRejectBank7());
		temp_var.setBankPendingRejectBank8(getBankPendingRejectBank8());
		temp_var.setBankPendingRejectBank9(getBankPendingRejectBank9());
		temp_var.setBankPendingRejectBank10(getBankPendingRejectBank10());
		temp_var.setBankPendingRejectBank11(getBankPendingRejectBank11());
		temp_var.setBankPendingRejectBank12(getBankPendingRejectBank12());
		temp_var.setBankPendingRejectBank13(getBankPendingRejectBank13());
		temp_var.setBankPendingRejectBank14(getBankPendingRejectBank14());
		temp_var.setBankPendingRejectBank15(getBankPendingRejectBank15());
		temp_var.setMemoryClusteringEvents(getMemoryClusteringEvents());
		temp_var.setTypedSlotHardwareBudgetRejects(getTypedSlotHardwareBudgetRejects());
		temp_var.setTypedSlotAssistQuotaRejects(getTypedSlotAssistQuotaRejects());
		temp_var.setTypedSlotAssistBackpressureRejects(getTypedSlotAssistBackpressureRejects());
		temp_var.setSerializingBoundaryRejects(getSerializingBoundaryRejects());
		temp_var.setSerializingEpochCount(getSerializingEpochCount());
		temp_var.setAssistNominations(getAssistNominationCount());
		temp_var.setAssistInjections(getAssistInjectionsCount());
		temp_var.setAssistRejects(getAssistRejects());
		temp_var.setAssistBoundaryRejects(getAssistBoundaryRejects());
		temp_var.setAssistInvalidations(getAssistInvalidations());
		temp_var.setAssistInterCoreNominations(getAssistInterCoreNominations());
		temp_var.setAssistInterCoreInjections(getAssistInterCoreInjections());
		temp_var.setAssistInterCoreRejects(getAssistInterCoreRejects());
		temp_var.setAssistInterCoreDomainRejects(getAssistInterCoreDomainRejects());
		temp_var.setAssistInterCorePodLocalInjections(getAssistInterCorePodLocalInjections());
		temp_var.setAssistInterCoreCrossPodInjections(getAssistInterCoreCrossPodInjections());
		temp_var.setAssistInterCorePodLocalRejects(getAssistInterCorePodLocalRejects());
		temp_var.setAssistInterCoreCrossPodRejects(getAssistInterCoreCrossPodRejects());
		temp_var.setAssistInterCorePodLocalDomainRejects(getAssistInterCorePodLocalDomainRejects());
		temp_var.setAssistInterCoreCrossPodDomainRejects(getAssistInterCoreCrossPodDomainRejects());
		temp_var.setAssistInterCoreSameVtVectorInjects(getAssistInterCoreSameVtVectorInjects());
		temp_var.setAssistInterCoreDonorVtVectorInjects(getAssistInterCoreDonorVtVectorInjects());
		temp_var.setAssistInterCoreSameVtVectorWritebackInjects(getAssistInterCoreSameVtVectorWritebackInjects());
		temp_var.setAssistInterCoreDonorVtVectorWritebackInjects(getAssistInterCoreDonorVtVectorWritebackInjects());
		temp_var.setAssistInterCoreLane6DefaultStoreDonorPrefetchInjects(getAssistInterCoreLane6DefaultStoreDonorPrefetchInjects());
		temp_var.setAssistInterCoreLane6HotLoadDonorPrefetchInjects(getAssistInterCoreLane6HotLoadDonorPrefetchInjects());
		temp_var.setAssistInterCoreLane6HotStoreDonorPrefetchInjects(getAssistInterCoreLane6HotStoreDonorPrefetchInjects());
		temp_var.setAssistInterCoreLane6DonorPrefetchInjects(getAssistInterCoreLane6DonorPrefetchInjects());
		temp_var.setAssistInterCoreLane6ColdStoreLdsaInjects(getAssistInterCoreLane6ColdStoreLdsaInjects());
		temp_var.setAssistInterCoreLane6LdsaInjects(getAssistInterCoreLane6LdsaInjects());
		temp_var.setAssistQuotaRejects(getAssistQuotaRejects());
		temp_var.setAssistQuotaIssueRejects(getAssistQuotaIssueRejects());
		temp_var.setAssistQuotaLineRejects(getAssistQuotaLineRejects());
		temp_var.setAssistQuotaLinesReserved(getAssistQuotaLinesReserved());
		temp_var.setAssistBackpressureRejects(getAssistBackpressureRejects());
		temp_var.setAssistBackpressureOuterCapRejects(getAssistBackpressureOuterCapRejects());
		temp_var.setAssistBackpressureMshrRejects(getAssistBackpressureMshrRejects());
		temp_var.setAssistBackpressureDmaSrfRejects(getAssistBackpressureDmaSrfRejects());
		temp_var.setAssistDonorPrefetchInjects(getAssistDonorPrefetchInjects());
		temp_var.setAssistLdsaInjects(getAssistLdsaInjects());
		temp_var.setAssistVdsaInjects(getAssistVdsaInjects());
		temp_var.setAssistSameVtInjects(getAssistSameVtInjects());
		temp_var.setAssistDonorVtInjects(getAssistDonorVtInjects());
		temp_var.setLastAssistInvalidationReason(getLastAssistInvalidationReason());
		temp_var.setLastAssistOwnershipSignature(getLastAssistOwnershipSignature());
		temp_var.setLoopPhaseProfiles(BuildLoopPhaseProfiles());
		return temp_var;
	}

	void MicroOpScheduler::RecordLegalityCertificateCacheHit(long long estimatedChecksSaved)
	{
		setPhaseCertificateReadyHits(getPhaseCertificateReadyHits() + 1);
		setEstimatedPhaseCertificateChecksSaved(getEstimatedPhaseCertificateChecksSaved() + estimatedChecksSaved);
	}

	void MicroOpScheduler::RecordLegalityCertificateCacheMiss()
	{
		setPhaseCertificateReadyMisses(getPhaseCertificateReadyMisses() + 1);
	}

	void MicroOpScheduler::RecordLegalityCertificateCacheInvalidation(ReplayPhaseInvalidationReason reason)
	{
		if (reason == ReplayPhaseInvalidationReason::None)
		{
			return;
		}

		setPhaseCertificateInvalidations(getPhaseCertificateInvalidations() + 1);
		if (reason == ReplayPhaseInvalidationReason::CertificateMutation)
		{
			setPhaseCertificateMutationInvalidations(getPhaseCertificateMutationInvalidations() + 1);
		}

		if ((reason == ReplayPhaseInvalidationReason::PhaseMismatch || reason == ReplayPhaseInvalidationReason::InactivePhase))
		{
			setPhaseCertificatePhaseMismatchInvalidations(getPhaseCertificatePhaseMismatchInvalidations() + 1);
		}

		setLastPhaseCertificateInvalidationReason(reason);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private LegalityDecision EvaluateInterCoreLegality(IReadOnlyList<MicroOp?> bundle, BundleResourceCertificate bundleCert, int bundleOwnerThreadId, ulong requestedDomainTag, MicroOp candidate, SafetyMask128 globalHardwareMask = default)
	inline LegalityDecision MicroOpScheduler::EvaluateInterCoreLegality(const std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<BundleResourceCertificate> bundleCert, int bundleOwnerThreadId, unsigned long long requestedDomainTag, std::shared_ptr<MicroOp> candidate, SafetyMask128 globalHardwareMask)
	{
		return _runtimeLegalityService->EvaluateInterCoreLegality(_currentReplayPhase, bundle, bundleCert, bundleOwnerThreadId, requestedDomainTag, candidate, globalHardwareMask);
	}

	inline int MicroOpScheduler::CountPendingInterCoreCandidates(unsigned long long requestedDomainTag)
	{
		return CountPendingInterCoreCandidates(CaptureInterCoreNominationSnapshot(), requestedDomainTag);
	}

	inline int MicroOpScheduler::CountPendingInterCoreCandidates(InterCoreNominationSnapshot nominationSnapshot, unsigned long long requestedDomainTag)
	{
		int count = 0;
		for (int coreId = 0; coreId < NUM_PORTS; coreId++)
		{
			std::shared_ptr<MicroOp> candidate;
			if (!nominationSnapshot.TryGetCandidate(coreId, candidate))
			{
				continue;
			}

			if (requestedDomainTag != 0)
			{
				InterCoreDomainGuardDecision domainGuard = _runtimeLegalityService->EvaluateInterCoreDomainGuard(candidate, requestedDomainTag);
				if (!domainGuard.isAllowed())
				{
					continue;
				}
			}

			count++;
		}

		return count;
	}

	inline int MicroOpScheduler::CountPendingSmtCandidates(SmtNominationState nominationState, int ownerVirtualThreadId)
	{
		return nominationState.CountReadyCandidatesExcluding(ownerVirtualThreadId);
	}

	inline void MicroOpScheduler::RecordDeterminismTaxWindow(int referenceSlots, int replayEligibleSlots, int readyCandidates)
	{
		if (!_currentReplayPhase.isActive() || referenceSlots <= 0)
		{
			return;
		}

		setDeterminismReferenceOpportunitySlots(getDeterminismReferenceOpportunitySlots() + referenceSlots);
		setDeterminismReplayEligibleSlots(getDeterminismReplayEligibleSlots() + replayEligibleSlots);

		int maskedSlots = std::max(0, referenceSlots - replayEligibleSlots);
		setDeterminismMaskedSlots(getDeterminismMaskedSlots() + maskedSlots);

		if (maskedSlots > 0)
		{
			setDeterminismConstrainedCycles(getDeterminismConstrainedCycles() + 1);
			if (readyCandidates > 0)
			{
				setDeterminismEstimatedLostSlots(getDeterminismEstimatedLostSlots() + std::min(maskedSlots, readyCandidates));
			}
		}
	}

	inline void MicroOpScheduler::RecordDomainIsolationProbe(DomainIsolationProbeResult probe)
	{
		setDomainIsolationProbeAttempts(getDomainIsolationProbeAttempts() + 1);

		if (!probe.isAllowed())
		{
			setDomainIsolationBlockedAttempts(getDomainIsolationBlockedAttempts() + 1);
		}

		if (probe.isCrossDomainBlock())
		{
			setDomainIsolationCrossDomainBlocks(getDomainIsolationCrossDomainBlocks() + 1);
		}

		if (probe.isKernelToUserBlock())
		{
			setDomainIsolationKernelToUserBlocks(getDomainIsolationKernelToUserBlocks() + 1);
		}
	}

	inline void MicroOpScheduler::RecordTypedSlotReject(TypedSlotRejectReason reason)
	{
		switch (reason)
		{
			case TypedSlotRejectReason::StaticClassOvercommit:
			case TypedSlotRejectReason::DynamicClassExhaustion:
				setClassCapacityRejects(getClassCapacityRejects() + 1);
				setSmtRejectionsCount(getSmtRejectionsCount() + 1);
				// Phase 08: disaggregated + NOP tracking
				if (reason == TypedSlotRejectReason::StaticClassOvercommit)
				{
					setStaticClassOvercommitRejects(getStaticClassOvercommitRejects() + 1);
				}
				else
				{
					setDynamicClassExhaustionRejects(getDynamicClassExhaustionRejects() + 1);
				}
				setNopDueToNoClassCapacity(getNopDueToNoClassCapacity() + 1);
				break;
			case TypedSlotRejectReason::ResourceConflict:
				setTypedSlotResourceConflictRejects(getTypedSlotResourceConflictRejects() + 1);
				setSmtRejectionsCount(getSmtRejectionsCount() + 1);
				setNopDueToResourceConflict(getNopDueToResourceConflict() + 1); // Phase 08
				break;
			// Scoreboard/Bank/HardwareBudget/SpecBudget: SmtRejectionsCount already
			// incremented inside IsOuterCapBlocking — only bump typed counters.
			case TypedSlotRejectReason::ScoreboardReject:
				setTypedSlotScoreboardRejects(getTypedSlotScoreboardRejects() + 1);
				setNopDueToDynamicState(getNopDueToDynamicState() + 1); // Phase 08
				break;
			case TypedSlotRejectReason::BankPendingReject:
				break;
			case TypedSlotRejectReason::HardwareBudgetReject:
				setNopDueToDynamicState(getNopDueToDynamicState() + 1); // Phase 08
				break;
			case TypedSlotRejectReason::AssistQuotaReject:
				setTypedSlotAssistQuotaRejects(getTypedSlotAssistQuotaRejects() + 1);
				setSmtRejectionsCount(getSmtRejectionsCount() + 1);
				setNopDueToDynamicState(getNopDueToDynamicState() + 1);
				break;
			case TypedSlotRejectReason::AssistBackpressureReject:
				setTypedSlotAssistBackpressureRejects(getTypedSlotAssistBackpressureRejects() + 1);
				setSmtRejectionsCount(getSmtRejectionsCount() + 1);
				setNopDueToDynamicState(getNopDueToDynamicState() + 1);
				break;
			case TypedSlotRejectReason::SpeculationBudgetReject:
				setTypedSlotSpeculationBudgetRejects(getTypedSlotSpeculationBudgetRejects() + 1);
				setNopDueToDynamicState(getNopDueToDynamicState() + 1); // Phase 08
				break;
			case TypedSlotRejectReason::PinnedLaneConflict:
			case TypedSlotRejectReason::LateBindingConflict:
				setLaneConflictRejects(getLaneConflictRejects() + 1);
				setSmtRejectionsCount(getSmtRejectionsCount() + 1);
				// Phase 08: disaggregated + NOP tracking
				if (reason == TypedSlotRejectReason::PinnedLaneConflict)
				{
					setPinnedLaneConflicts(getPinnedLaneConflicts() + 1);
					setNopDueToPinnedConstraint(getNopDueToPinnedConstraint() + 1);
				}
				else
				{
					setLateBindingConflicts(getLateBindingConflicts() + 1);
				}
				break;
			case TypedSlotRejectReason::InvalidPinnedLane:
				setInvalidPinnedLaneRejects(getInvalidPinnedLaneRejects() + 1);
				setSmtRejectionsCount(getSmtRejectionsCount() + 1);
				break;
			case TypedSlotRejectReason::DomainReject:
				setTypedSlotDomainRejects(getTypedSlotDomainRejects() + 1); // Phase 08
				setSmtRejectionsCount(getSmtRejectionsCount() + 1);
				break;
		}
	}

	inline void MicroOpScheduler::RecordCertificateRejectByClass(SlotClass slotClass)
	{
		switch (slotClass)
		{
			case SlotClass::AluClass:
				setCertificateRejectByAluClass(getCertificateRejectByAluClass() + 1);
				break;
			case SlotClass::LsuClass:
				setCertificateRejectByLsuClass(getCertificateRejectByLsuClass() + 1);
				break;
			case SlotClass::DmaStreamClass:
				setCertificateRejectByDmaStreamClass(getCertificateRejectByDmaStreamClass() + 1);
				break;
			case SlotClass::MatrixTileStreamClass:
				setCertificateRejectByMatrixTileStreamClass(getCertificateRejectByMatrixTileStreamClass() + 1);
				break;
			case SlotClass::BranchControl:
				setCertificateRejectByBranchControl(getCertificateRejectByBranchControl() + 1);
				break;
			case SlotClass::SystemSingleton:
				setCertificateRejectBySystemSingleton(getCertificateRejectBySystemSingleton() + 1);
				break;
		}
	}

	inline void MicroOpScheduler::RecordSmtLegalityRejectByClass(SlotClass slotClass)
	{
		switch (slotClass)
		{
			case SlotClass::AluClass:
				setSmtLegalityRejectByAluClass(getSmtLegalityRejectByAluClass() + 1);
				break;
			case SlotClass::LsuClass:
				setSmtLegalityRejectByLsuClass(getSmtLegalityRejectByLsuClass() + 1);
				break;
			case SlotClass::DmaStreamClass:
				setSmtLegalityRejectByDmaStreamClass(getSmtLegalityRejectByDmaStreamClass() + 1);
				break;
			case SlotClass::MatrixTileStreamClass:
				setSmtLegalityRejectByMatrixTileStreamClass(getSmtLegalityRejectByMatrixTileStreamClass() + 1);
				break;
			case SlotClass::BranchControl:
				setSmtLegalityRejectByBranchControl(getSmtLegalityRejectByBranchControl() + 1);
				break;
			case SlotClass::SystemSingleton:
				setSmtLegalityRejectBySystemSingleton(getSmtLegalityRejectBySystemSingleton() + 1);
				break;
		}
	}

	inline void MicroOpScheduler::RecordCertificateRegisterGroupConflict(int virtualThreadId)
	{
		RecordPerVtRegGroupConflict(virtualThreadId);
	}

	inline void MicroOpScheduler::RecordSmtLegalityRejectBreakdown()
	{
		switch (_lastSmtLegalityRejectKind)
		{
			case RejectKind::OwnerMismatch:
				setSmtOwnerContextGuardRejects(getSmtOwnerContextGuardRejects() + 1);
				break;
			case RejectKind::DomainMismatch:
				setSmtDomainGuardRejects(getSmtDomainGuardRejects() + 1);
				break;
			case RejectKind::Boundary:
				setSmtBoundaryGuardRejects(getSmtBoundaryGuardRejects() + 1);
				break;
		}

		switch (_lastCertificateRejectDetail)
		{
			case CertificateRejectDetail::SharedResourceConflict:
				setSmtSharedResourceCertificateRejects(getSmtSharedResourceCertificateRejects() + 1);
				break;
			case CertificateRejectDetail::RegisterGroupConflict:
				setSmtRegisterGroupCertificateRejects(getSmtRegisterGroupCertificateRejects() + 1);
				break;
		}
	}

	inline void MicroOpScheduler::RecordPerVtRejection(int virtualThreadId)
	{
		switch (virtualThreadId)
		{
			case 0:
				setRejectionsVT0(getRejectionsVT0() + 1);
				break;
			case 1:
				setRejectionsVT1(getRejectionsVT1() + 1);
				break;
			case 2:
				setRejectionsVT2(getRejectionsVT2() + 1);
				break;
			case 3:
				setRejectionsVT3(getRejectionsVT3() + 1);
				break;
		}
	}

	inline void MicroOpScheduler::RecordPerVtRegGroupConflict(int virtualThreadId)
	{
		switch (virtualThreadId)
		{
			case 0:
				setRegGroupConflictsVT0(getRegGroupConflictsVT0() + 1);
				break;
			case 1:
				setRegGroupConflictsVT1(getRegGroupConflictsVT1() + 1);
				break;
			case 2:
				setRegGroupConflictsVT2(getRegGroupConflictsVT2() + 1);
				break;
			case 3:
				setRegGroupConflictsVT3(getRegGroupConflictsVT3() + 1);
				break;
		}
	}

	inline void MicroOpScheduler::RecordTypedSlotReject(TypedSlotRejectReason reason, std::shared_ptr<MicroOp> candidate)
	{
		// Increment counters via base overload
		RecordTypedSlotReject(reason);
		RecordPerVtRejection(candidate->getVirtualThreadId());

		if (reason == TypedSlotRejectReason::ResourceConflict)
		{
			RecordSmtLegalityRejectBreakdown();
			RecordSmtLegalityRejectByClass(candidate->getPlacement().RequiredSlotClass);

			if (_lastCertificateRejectDetail != CertificateRejectDetail::None)
			{
				RecordCertificateRejectByClass(candidate->getPlacement().RequiredSlotClass);
			}

			if (_lastCertificateRejectDetail == CertificateRejectDetail::RegisterGroupConflict)
			{
				RecordCertificateRegisterGroupConflict(candidate->getVirtualThreadId());
			}
		}

		// Phase 06: Compute structured classification for telemetry
		_lastRejectClassification = _runtimeLegalityService->ClassifyReject(reason, _lastCertificateRejectDetail, candidate->getPlacement().RequiredSlotClass, candidate->getPlacement().PinningKind);
	}

	inline void MicroOpScheduler::RecordBankPendingReject(int memoryBankId)
	{
		setTypedSlotBankPendingRejects(getTypedSlotBankPendingRejects() + 1);
		setNopDueToDynamicState(getNopDueToDynamicState() + 1);

		switch (memoryBankId)
		{
			case 0:
				setBankPendingRejectBank0(getBankPendingRejectBank0() + 1);
				break;
			case 1:
				setBankPendingRejectBank1(getBankPendingRejectBank1() + 1);
				break;
			case 2:
				setBankPendingRejectBank2(getBankPendingRejectBank2() + 1);
				break;
			case 3:
				setBankPendingRejectBank3(getBankPendingRejectBank3() + 1);
				break;
			case 4:
				setBankPendingRejectBank4(getBankPendingRejectBank4() + 1);
				break;
			case 5:
				setBankPendingRejectBank5(getBankPendingRejectBank5() + 1);
				break;
			case 6:
				setBankPendingRejectBank6(getBankPendingRejectBank6() + 1);
				break;
			case 7:
				setBankPendingRejectBank7(getBankPendingRejectBank7() + 1);
				break;
			case 8:
				setBankPendingRejectBank8(getBankPendingRejectBank8() + 1);
				break;
			case 9:
				setBankPendingRejectBank9(getBankPendingRejectBank9() + 1);
				break;
			case 10:
				setBankPendingRejectBank10(getBankPendingRejectBank10() + 1);
				break;
			case 11:
				setBankPendingRejectBank11(getBankPendingRejectBank11() + 1);
				break;
			case 12:
				setBankPendingRejectBank12(getBankPendingRejectBank12() + 1);
				break;
			case 13:
				setBankPendingRejectBank13(getBankPendingRejectBank13() + 1);
				break;
			case 14:
				setBankPendingRejectBank14(getBankPendingRejectBank14() + 1);
				break;
			case 15:
				setBankPendingRejectBank15(getBankPendingRejectBank15() + 1);
				break;
		}
	}

	void MicroOpScheduler::RecordMemoryClusteringEvent()
	{
		setMemoryClusteringEvents(getMemoryClusteringEvents() + 1);
	}

	inline void MicroOpScheduler::RecordTypedSlotInject(std::shared_ptr<MicroOp> candidate, int lane)
	{
		setTotalLaneBindings(getTotalLaneBindings() + 1);

		if (candidate->getPlacement().PinningKind == SlotPinningKind::HardPinned)
		{
			setHardPinnedInjects(getHardPinnedInjects() + 1);
		}
		else
		{
			setClassFlexibleInjects(getClassFlexibleInjects() + 1);
			setNopAvoided(getNopAvoided() + 1);
		}

		switch (candidate->getPlacement().RequiredSlotClass)
		{
			case SlotClass::AluClass:
				setAluClassInjects(getAluClassInjects() + 1);
				break;
			case SlotClass::LsuClass:
				setLsuClassInjects(getLsuClassInjects() + 1);
				break;
			case SlotClass::DmaStreamClass:
				setDmaStreamClassInjects(getDmaStreamClassInjects() + 1);
				break;
			case SlotClass::MatrixTileStreamClass:
				setMatrixTileStreamClassInjects(getMatrixTileStreamClassInjects() + 1);
				break;
			case SlotClass::BranchControl:
				setBranchControlInjects(getBranchControlInjects() + 1);
				break;
		}
	}

	inline int MicroOpScheduler::ResolveNextInjectableSlot(BundleOpportunityState opportunityState, int startIndex)
	{
		return opportunityState.FindNextInjectableEmptySlot(startIndex, _currentReplayPhase.getHasStableDonorStructure(), _currentReplayPhase.getStableDonorMask());
	}

	inline unsigned char MicroOpScheduler::ComputeBundleOccupancy(std::vector<std::shared_ptr<MicroOp>> &bundle)
	{
		return BundleOpportunityState::Create(bundle).getOccupancyMask();
	}

	unsigned char MicroOpScheduler::getAllEligibleVirtualThreadMask()
	{
		return static_cast<unsigned char>((1 << SMT_WAYS) - 1);
	}

	long long MicroOpScheduler::getEligibilityMaskedCycles() const
	{
		return EligibilityMaskedCycles;
	}

	void MicroOpScheduler::setEligibilityMaskedCycles(long long value)
	{
		EligibilityMaskedCycles = value;
	}

	long long MicroOpScheduler::getEligibilityMaskedReadyCandidates() const
	{
		return EligibilityMaskedReadyCandidates;
	}

	void MicroOpScheduler::setEligibilityMaskedReadyCandidates(long long value)
	{
		EligibilityMaskedReadyCandidates = value;
	}

	inline unsigned char MicroOpScheduler::NormalizeEligibleVirtualThreadMask(unsigned char eligibleVirtualThreadMask)
	{
		return static_cast<unsigned char>(eligibleVirtualThreadMask & getAllEligibleVirtualThreadMask());
	}

	inline MicroOpScheduler::SmtNominationState MicroOpScheduler::CreateEligibleSmtNominationState(unsigned char eligibleVirtualThreadMask)
	{
		unsigned char normalizedEligibleMask = NormalizeEligibleVirtualThreadMask(eligibleVirtualThreadMask);
		unsigned char readyPortMask = 0;
		unsigned char visibleReadyMask = 0;

		for (int vt = 0; vt < SMT_WAYS; vt++)
		{
			if (_smtPortValid[vt] && _smtPorts[vt] != nullptr)
			{
				unsigned char vtMask = static_cast<unsigned char>(1 << vt);
				readyPortMask |= vtMask;

				if ((normalizedEligibleMask & vtMask) != 0)
				{
					visibleReadyMask |= vtMask;
				}
			}
		}

		RecordSmtEligibilitySnapshot(std::make_shared<SmtEligibilitySnapshot>(eligibleVirtualThreadMask, normalizedEligibleMask, readyPortMask, visibleReadyMask, static_cast<unsigned char>(readyPortMask & ~normalizedEligibleMask)));

		return SmtNominationState(visibleReadyMask);
	}

	inline void MicroOpScheduler::RecordSmtEligibilitySnapshot(std::shared_ptr<SmtEligibilitySnapshot> snapshot)
	{
		_lastSmtEligibilitySnapshot = snapshot;
		if (snapshot->getHasMaskedReadyCandidates())
		{
			setEligibilityMaskedCycles(getEligibilityMaskedCycles() + 1);
			setEligibilityMaskedReadyCandidates(getEligibilityMaskedReadyCandidates() + snapshot->getMaskedReadyCount());
		}
	}

	inline MicroOpScheduler::SmtNominationSnapshot MicroOpScheduler::CaptureSmtNominationSnapshot(unsigned char eligibleVirtualThreadMask)
	{
		return CaptureSmtNominationSnapshot(CreateEligibleSmtNominationState(eligibleVirtualThreadMask));
	}
}

