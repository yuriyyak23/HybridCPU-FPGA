#pragma once

#include "IVmxEventSink.h"
#include "../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmExitReason.h"
#include "VmxEventKind.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	struct VmxDebugTraceCounters final
	{
		const unsigned long long VmxEventCount;
		const unsigned long long VmExitCount;
		const unsigned long long VmFailCount;
		const unsigned long long VmAbortCount;
		const unsigned long long InvalidationCount;
		const unsigned long long DroppedPostedEventCount;
		const unsigned long long LastExitReason;

		VmxDebugTraceCounters(unsigned long long vmxEventCount, unsigned long long vmExitCount,
			unsigned long long vmFailCount, unsigned long long vmAbortCount,
			unsigned long long invalidationCount, unsigned long long droppedPostedEventCount,
			unsigned long long lastExitReason)
			: VmxEventCount(vmxEventCount), VmExitCount(vmExitCount), VmFailCount(vmFailCount),
			  VmAbortCount(vmAbortCount), InvalidationCount(invalidationCount),
			  DroppedPostedEventCount(droppedPostedEventCount), LastExitReason(lastExitReason) {}

		bool ContainsHostEvidence() const noexcept { return false; }
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmxDebugTraceCounters(ulong VmxEventCount, ulong VmExitCount, ulong VmFailCount, ulong VmAbortCount, ulong InvalidationCount, ulong DroppedPostedEventCount, ulong LastExitReason)
//	{
//		public bool ContainsHostEvidence
//		{
//			get
//			{
//				return false;
//			}
//		}
//	}

	class VmxDebugTracePlane final : public IVmxEventSink
	{
	private:
		unsigned long long VmxEventCount = 0;
		unsigned long long VmExitCount = 0;
		unsigned long long VmFailCount = 0;
		unsigned long long VmAbortCount = 0;
		unsigned long long InvalidationCount = 0;
		unsigned long long DroppedPostedEventCount = 0;
		VmExitReason LastExitReason = static_cast<VmExitReason>(0);

	public:
		void RecordVmxEvent(VmxEventKind kind, unsigned short vtId, VmExitReason exitReason = VmExitReason::None) override;

		void RecordEvent(VmxEventKind kind, VmExitReason exitReason = VmExitReason::None);

		unsigned long long getVmxEventCount() const;
	private:
		void setVmxEventCount(unsigned long long value);

	public:
		unsigned long long getVmExitCount() const;
	private:
		void setVmExitCount(unsigned long long value);

	public:
		unsigned long long getVmFailCount() const;
	private:
		void setVmFailCount(unsigned long long value);

	public:
		unsigned long long getVmAbortCount() const;
	private:
		void setVmAbortCount(unsigned long long value);

	public:
		unsigned long long getInvalidationCount() const;
	private:
		void setInvalidationCount(unsigned long long value);

	public:
		unsigned long long getDroppedPostedEventCount() const;
	private:
		void setDroppedPostedEventCount(unsigned long long value);

	public:
		VmExitReason getLastExitReason() const;
	private:
		void setLastExitReason(VmExitReason value);

	public:
		void RecordExit(VmExitReason reason);

		void RecordFail();

		void RecordAbort();

		void RecordInvalidation();

		void RecordDroppedPostedEvents(unsigned long long count);

		std::shared_ptr<VmxDebugTraceCounters> Snapshot();

		void Reset();

		void DiscardTraceHandles();
	};

}
