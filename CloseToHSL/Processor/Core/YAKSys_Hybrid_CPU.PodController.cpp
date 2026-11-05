#include "YAKSys_Hybrid_CPU.PodController.h"
#include "../../Core/Pipeline/Assist/InterCore/YAKSys_Hybrid_CPU.Core.MicroOpScheduler.h"
#include "../../Memory/Subsystem/YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h"
#include "../../Core/Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <new>

using namespace YAKSys_Hybrid_CPU::Core;
using namespace HybridCPU_ISE::CloseToHSL::Memory::Cache;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU
{

	bool PodController::TryPeekInterCoreAssistTransport(int localCoreId, AssistInterCoreTransport &transport)
	{
		transport = YAKSys_Hybrid_CPU::Core::AssistInterCoreTransport();
		return static_cast<unsigned int>(localCoreId) < CORES_PER_POD && _scheduler->TryPeekInterCoreAssistTransport(localCoreId, transport);
	}

	bool PodController::TryConsumeInterCoreAssistTransport(int localCoreId, AssistInterCoreTransport &transport)
	{
		transport = YAKSys_Hybrid_CPU::Core::AssistInterCoreTransport();
		return static_cast<unsigned int>(localCoreId) < CORES_PER_POD && _scheduler->TryConsumeInterCoreAssistTransport(localCoreId, transport);
	}

	void PodController::ClearInterCoreAssistTransport(int localCoreId)
	{
		if (static_cast<unsigned int>(localCoreId) >= CORES_PER_POD)
		{
			return;
		}

		_scheduler->ClearInterCoreAssistNominationPort(localCoreId);
	}

	PodController::AssistOwnerSnapshot::AssistOwnerSnapshot(int virtualThreadId, int ownerContextId, unsigned long long domainTag, unsigned long long assistEpochId)
		: IsPublished(true), VirtualThreadId(virtualThreadId), OwnerContextId(ownerContextId), DomainTag(domainTag), AssistEpochId(assistEpochId)
	{
	}

	PodController::AssistOwnerSnapshot &PodController::AssistOwnerSnapshot::operator=(const AssistOwnerSnapshot &other)
	{
		if (this != &other)
		{
			this->~AssistOwnerSnapshot();
			::new (static_cast<void *>(this)) AssistOwnerSnapshot(other);
		}
		return *this;
	}

	bool PodController::AssistOwnerSnapshot::isPublished() const
	{
		return IsPublished;
	}

	int PodController::AssistOwnerSnapshot::getVirtualThreadId() const
	{
		return VirtualThreadId;
	}

	int PodController::AssistOwnerSnapshot::getOwnerContextId() const
	{
		return OwnerContextId;
	}

	unsigned long long PodController::AssistOwnerSnapshot::getDomainTag() const
	{
		return DomainTag;
	}

	unsigned long long PodController::AssistOwnerSnapshot::getAssistEpochId() const
	{
		return AssistEpochId;
	}

	bool PodController::AssistOwnerSnapshot::isValid() const
	{
		return isPublished();
	}

	bool PodController::AssistOwnerSnapshot::Matches(int virtualThreadId, int ownerContextId, unsigned long long domainTag) const
	{
		return getVirtualThreadId() == virtualThreadId && getOwnerContextId() == ownerContextId && getDomainTag() == domainTag;
	}

	void PodController::PublishAssistOwnerSnapshot(int localCoreId, int virtualThreadId, int ownerContextId, unsigned long long domainTag, unsigned long long assistEpochId)
	{
		if (static_cast<unsigned int>(localCoreId) >= CORES_PER_POD)
		{
			return;
		}

		_assistOwnerSnapshots[localCoreId] = AssistOwnerSnapshot(virtualThreadId, ownerContextId, domainTag, assistEpochId);
		_scheduler->ReconcileInterCoreAssistTransportOwnerSnapshot(localCoreId, getPodId(), true, _assistOwnerSnapshots[localCoreId]);
	}

	void PodController::InvalidateAssistOwnerSnapshot(int localCoreId)
	{
		if (static_cast<unsigned int>(localCoreId) >= CORES_PER_POD)
		{
			return;
		}

		_assistOwnerSnapshots[localCoreId] = YAKSys_Hybrid_CPU::PodController::AssistOwnerSnapshot();
		_scheduler->ReconcileInterCoreAssistTransportOwnerSnapshot(localCoreId, getPodId(), false, _assistOwnerSnapshots[localCoreId]);
	}

	bool PodController::TryGetAssistOwnerSnapshot(int localCoreId, AssistOwnerSnapshot &snapshot)
	{
		snapshot = YAKSys_Hybrid_CPU::PodController::AssistOwnerSnapshot();
		if (static_cast<unsigned int>(localCoreId) >= CORES_PER_POD)
		{
			return false;
		}

		snapshot = _assistOwnerSnapshots[localCoreId];
		return snapshot.isValid();
	}

	std::vector<PodController::AssistOwnerSnapshot> PodController::CreateEmptyAssistOwnerSnapshots()
	{
		return std::vector<AssistOwnerSnapshot>(CORES_PER_POD);
	}

	int PodController::getPodX() const
	{
		return PodX;
	}

	int PodController::getPodY() const
	{
		return PodY;
	}

	unsigned short PodController::getPodId() const
	{
		return static_cast<unsigned short>((getPodX() << 8) | getPodY());
	}

	unsigned int PodController::getBarrierTimeoutThreshold() const
	{
		return BarrierTimeoutThreshold;
	}

	void PodController::setBarrierTimeoutThreshold(unsigned int value)
	{
		BarrierTimeoutThreshold = value;
	}

	unsigned long long PodController::getDomainCertificate() const
	{
		return DomainCertificate;
	}

	void PodController::setDomainCertificate(unsigned long long value)
	{
		DomainCertificate = value;
	}

	std::shared_ptr<SharedPodCache> PodController::getL2Cache() const
	{
		return L2Cache;
	}

	PodController::PodController(int podX, int podY, std::shared_ptr<MicroOpScheduler> scheduler) : PodX(podX), PodY(podY), L2Cache(std::make_shared<SharedPodCache>()), _scheduler(scheduler)
	{
		ArgumentNullException::ThrowIfNull(scheduler);

	}

	bool PodController::IsBarrierReached(unsigned short affinityMask)
	{
		return (_barrierReachedMask & affinityMask) == affinityMask;
	}

	void PodController::SignalBarrier(int localCoreId)
	{
		if (static_cast<unsigned int>(localCoreId) < CORES_PER_POD)
		{
			_barrierReachedMask |= static_cast<unsigned short>(1 << localCoreId);
		}
	}

	void PodController::ResetBarrier()
	{
		_barrierReachedMask = 0;
		_barrierTimeoutCounter = 0;
	}

	bool PodController::TickBarrierTimeout()
	{
		if (_barrierReachedMask != 0)
		{
			_barrierTimeoutCounter++;
			return _barrierTimeoutCounter >= getBarrierTimeoutThreshold();
		}

		return false;
	}

	void PodController::BeginCycle()
	{
		// Phase 1: Latch nominations from previous cycle into pipeline register
		_scheduler->LatchNominations();

		// Plan 10: Update power gating based on the latched visible inter-core
		// nomination state. Must be called AFTER LatchNominations and BEFORE
		// ClearNominationPorts so the previous cycle's donor visibility is preserved.
		PowerController.UpdateFromFsp(_scheduler->GetLatchedInterCoreNominationReadyMask());

		// Clear live ports for fresh nominations
		_scheduler->ClearNominationPorts();
		_scheduler->ClearInterCoreAssistNominationPorts();

		getL2Cache()->AdvanceCycle();
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public MicroOp[] PackBundle(System.Collections.Generic.IReadOnlyList<MicroOp?> originalBundle, int currentThreadId, bool stealEnabled, byte stealMask, ResourceBitset globalResourceLocks = default, byte donorMask = 0, int localCoreId = -1, int bundleOwnerContextId = -1, ulong bundleOwnerDomainTag = 0, ulong assistRuntimeEpoch = 0, System.Nullable<MemorySubsystem> memSub = null, System.Nullable<PodController>[]? pods = null)
	std::vector<std::shared_ptr<MicroOp>> PodController::PackBundle(const std::vector<std::shared_ptr<MicroOp>> &originalBundle, int currentThreadId, bool stealEnabled, unsigned char stealMask, ResourceBitset globalResourceLocks, unsigned char donorMask, int localCoreId, int bundleOwnerContextId, unsigned long long bundleOwnerDomainTag, unsigned long long assistRuntimeEpoch, std::shared_ptr<MemorySubsystem> memSub, std::shared_ptr<std::vector<std::shared_ptr<PodController>>> pods)
	{
		return _scheduler->PackBundle(originalBundle, currentThreadId, stealEnabled, stealMask, globalResourceLocks, donorMask, getDomainCertificate(), localCoreId, getPodId(), bundleOwnerContextId, bundleOwnerDomainTag, assistRuntimeEpoch, memSub, (pods != nullptr) ? pods : std::make_shared<std::vector<std::shared_ptr<PodController>>>(Processor::Pods));
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public MicroOp[] PackBundleIntraCoreSmt(System.Collections.Generic.IReadOnlyList<MicroOp?> originalBundle, int ownerVirtualThreadId, int localCoreId, byte eligibleVirtualThreadMask, System.Nullable<MemorySubsystem> memSub = null)
	std::vector<std::shared_ptr<MicroOp>> PodController::PackBundleIntraCoreSmt(const std::vector<std::shared_ptr<MicroOp>> &originalBundle, int ownerVirtualThreadId, int localCoreId, unsigned char eligibleVirtualThreadMask, std::shared_ptr<MemorySubsystem> memSub)
	{
		return _scheduler->PackBundleIntraCoreSmt(originalBundle, ownerVirtualThreadId, localCoreId, eligibleVirtualThreadMask, memSub);
	}

	std::shared_ptr<MicroOpScheduler> PodController::getScheduler() const
	{
		return _scheduler;
	}
}
