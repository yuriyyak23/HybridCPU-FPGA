#include "TelemetryCounters.h"
#include "TraceEventKind.h"
#include "../../../Arch/CsrAddresses.h"

#include <algorithm>

using namespace YAKSys_Hybrid_CPU::Arch;

namespace YAKSys_Hybrid_CPU::Core
{

	unsigned long long TelemetryCounters::getCycleCount() const
	{
		return CycleCount;
	}

	void TelemetryCounters::setCycleCount(unsigned long long value)
	{
		CycleCount = value;
	}

	unsigned long long TelemetryCounters::getBundleRetiredCount() const
	{
		return BundleRetiredCount;
	}

	void TelemetryCounters::setBundleRetiredCount(unsigned long long value)
	{
		BundleRetiredCount = value;
	}

	unsigned long long TelemetryCounters::getInstrRetiredCount() const
	{
		return InstrRetiredCount;
	}

	void TelemetryCounters::setInstrRetiredCount(unsigned long long value)
	{
		InstrRetiredCount = value;
	}

	unsigned long long TelemetryCounters::getVmExitCount() const
	{
		return VmExitCount;
	}

	void TelemetryCounters::setVmExitCount(unsigned long long value)
	{
		VmExitCount = value;
	}

	unsigned long long TelemetryCounters::getBarrierCount() const
	{
		return BarrierCount;
	}

	void TelemetryCounters::setBarrierCount(unsigned long long value)
	{
		BarrierCount = value;
	}

	unsigned long long TelemetryCounters::getStealCount() const
	{
		return StealCount;
	}

	void TelemetryCounters::setStealCount(unsigned long long value)
	{
		StealCount = value;
	}

	unsigned long long TelemetryCounters::getReplayCount() const
	{
		return ReplayCount;
	}

	void TelemetryCounters::setReplayCount(unsigned long long value)
	{
		ReplayCount = value;
	}

	void TelemetryCounters::IncrementCycle()
	{
		setCycleCount(getCycleCount() + 1);
	}

	void TelemetryCounters::IncrementCycle(unsigned long long delta)
	{
		setCycleCount(getCycleCount() + delta);
	}

	void TelemetryCounters::IncrementBundleRetired()
	{
		setBundleRetiredCount(getBundleRetiredCount() + 1);
	}

	void TelemetryCounters::IncrementInstrRetired()
	{
		setInstrRetiredCount(getInstrRetiredCount() + 1);
	}

	void TelemetryCounters::IncrementInstrRetiredForVt(unsigned char vtId)
	{
		setInstrRetiredCount(getInstrRetiredCount() + 1);
		if (vtId < VtCount)
		{
			_instrCountPerVt[vtId]++;
		}
	}

	void TelemetryCounters::IncrementVmExit()
	{
		setVmExitCount(getVmExitCount() + 1);
	}

	void TelemetryCounters::IncrementBarrier()
	{
		setBarrierCount(getBarrierCount() + 1);
	}

	void TelemetryCounters::IncrementSteal()
	{
		setStealCount(getStealCount() + 1);
	}

	void TelemetryCounters::IncrementReplay()
	{
		setReplayCount(getReplayCount() + 1);
	}

	void TelemetryCounters::IncrementAmoDwordCount(unsigned char vtId)
	{
		if (vtId < VtCount)
		{
			_amoDwordCountPerVt[vtId]++;
		}
	}

	void TelemetryCounters::IncrementAmoWordCount(unsigned char vtId)
	{
		if (vtId < VtCount)
		{
			_amoWordCountPerVt[vtId]++;
		}
	}

	void TelemetryCounters::IncrementLrCount(unsigned char vtId)
	{
		if (vtId < VtCount)
		{
			_lrCountPerVt[vtId]++;
		}
	}

	void TelemetryCounters::IncrementScSuccess(unsigned char vtId)
	{
		if (vtId < VtCount)
		{
			_scSuccessCountPerVt[vtId]++;
		}
	}

	void TelemetryCounters::IncrementScFail(unsigned char vtId)
	{
		if (vtId < VtCount)
		{
			_scFailCountPerVt[vtId]++;
		}
	}

	unsigned long long TelemetryCounters::GetAmoDwordCount(unsigned char vtId)
	{
		return vtId < VtCount ? _amoDwordCountPerVt[vtId] : 0;
	}

	unsigned long long TelemetryCounters::GetAmoWordCount(unsigned char vtId)
	{
		return vtId < VtCount ? _amoWordCountPerVt[vtId] : 0;
	}

	unsigned long long TelemetryCounters::GetLrCount(unsigned char vtId)
	{
		return vtId < VtCount ? _lrCountPerVt[vtId] : 0;
	}

	unsigned long long TelemetryCounters::GetScSuccessCount(unsigned char vtId)
	{
		return vtId < VtCount ? _scSuccessCountPerVt[vtId] : 0;
	}

	unsigned long long TelemetryCounters::GetScFailCount(unsigned char vtId)
	{
		return vtId < VtCount ? _scFailCountPerVt[vtId] : 0;
	}

	unsigned long long TelemetryCounters::GetInstrCountForVt(unsigned char vtId)
	{
		return vtId < VtCount ? _instrCountPerVt[vtId] : 0;
	}

	unsigned long long TelemetryCounters::GetTotalAmoDwordCount()
	{
		unsigned long long sum = 0;
		for (int i = 0; i < VtCount; i++)
		{
			sum += _amoDwordCountPerVt[i];
		}
		return sum;
	}

	void TelemetryCounters::ApplyTraceEvent(V4TraceEvent evt)
	{
		switch (evt.getKind())
		{
			case TraceEventKind::BundleRetired:
				IncrementBundleRetired();
				break;

			case TraceEventKind::AluExecuted:
			case TraceEventKind::LoadExecuted:
			case TraceEventKind::StoreExecuted:
			case TraceEventKind::FenceExecuted:
			case TraceEventKind::BranchTaken:
			case TraceEventKind::BranchNotTaken:
			case TraceEventKind::JumpExecuted:
			case TraceEventKind::TrapTaken:
			case TraceEventKind::PrivilegeReturn:
			case TraceEventKind::WfiEntered:
			case TraceEventKind::CsrRead:
			case TraceEventKind::CsrWrite:
				IncrementInstrRetiredForVt(evt.getVtId());
				break;

			case TraceEventKind::LrExecuted:
				IncrementInstrRetiredForVt(evt.getVtId());
				IncrementLrCount(evt.getVtId());
				break;

			case TraceEventKind::ScSucceeded:
				IncrementInstrRetiredForVt(evt.getVtId());
				IncrementScSuccess(evt.getVtId());
				break;

			case TraceEventKind::ScFailed:
				IncrementInstrRetiredForVt(evt.getVtId());
				IncrementScFail(evt.getVtId());
				break;

			case TraceEventKind::AmoWordExecuted:
				IncrementInstrRetiredForVt(evt.getVtId());
				IncrementAmoWordCount(evt.getVtId());
				break;

			case TraceEventKind::AmoDwordExecuted:
				IncrementInstrRetiredForVt(evt.getVtId());
				IncrementAmoDwordCount(evt.getVtId());
				break;

			case TraceEventKind::VtYield:
			case TraceEventKind::VtWfe:
			case TraceEventKind::VtSev:
				IncrementInstrRetiredForVt(evt.getVtId());
				break;

			case TraceEventKind::PodBarrierEntered:
			case TraceEventKind::VtBarrierEntered:
				IncrementInstrRetiredForVt(evt.getVtId());
				IncrementBarrier();
				break;

			// Exited events do not count a second instruction retirement
			case TraceEventKind::PodBarrierExited:
			case TraceEventKind::VtBarrierExited:
				break;

			case TraceEventKind::VmxOn:
			case TraceEventKind::VmxOff:
			case TraceEventKind::VmEntry:
			case TraceEventKind::VmEntryFailed:
			case TraceEventKind::VmcsRead:
			case TraceEventKind::VmcsWrite:
				IncrementInstrRetiredForVt(evt.getVtId());
				break;

			case TraceEventKind::VmExit:
				IncrementVmExit();
				break;

			case TraceEventKind::FspPilfer:
				IncrementSteal();
				break;

			case TraceEventKind::BundleReplayed:
				IncrementReplay();
				break;

			// Other events (BundleDispatched, FspBoundary, FsmTransition) do not
			// map to simple counters.
			default:
				break;
		}
	}

	void TelemetryCounters::Reset()
	{
		setCycleCount(0);
		setBundleRetiredCount(0);
		setInstrRetiredCount(0);
		setVmExitCount(0);
		setBarrierCount(0);
		setStealCount(0);
		setReplayCount(0);
		std::fill(_amoDwordCountPerVt.begin(), _amoDwordCountPerVt.end(), 0ULL);
		std::fill(_amoWordCountPerVt.begin(), _amoWordCountPerVt.end(), 0ULL);
		std::fill(_lrCountPerVt.begin(), _lrCountPerVt.end(), 0ULL);
		std::fill(_scSuccessCountPerVt.begin(), _scSuccessCountPerVt.end(), 0ULL);
		std::fill(_scFailCountPerVt.begin(), _scFailCountPerVt.end(), 0ULL);
		std::fill(_instrCountPerVt.begin(), _instrCountPerVt.end(), 0ULL);
	}

	const std::unordered_map<unsigned short, long long> TelemetryCounters::ExportAsCsrSnapshot()
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: return new Dictionary<ushort, long> {{ CsrAddresses.Cycle, checked((long)CycleCount) }, { CsrAddresses.BundleRet, checked((long)BundleRetiredCount) }, { CsrAddresses.InstrRet, checked((long)InstrRetiredCount) }, { CsrAddresses.VmExitCnt, checked((long)VmExitCount) }, { CsrAddresses.BarrierCnt, checked((long)BarrierCount) }, { CsrAddresses.StealCnt, checked((long)StealCount) }, { CsrAddresses.ReplayCnt, checked((long)ReplayCount) }};
		return std::unordered_map<unsigned short, long long>
		{
			{CsrAddresses::Cycle, static_cast<long long>(getCycleCount())},
			{CsrAddresses::BundleRet, static_cast<long long>(getBundleRetiredCount())},
			{CsrAddresses::InstrRet, static_cast<long long>(getInstrRetiredCount())},
			{CsrAddresses::VmExitCnt, static_cast<long long>(getVmExitCount())},
			{CsrAddresses::BarrierCnt, static_cast<long long>(getBarrierCount())},
			{CsrAddresses::StealCnt, static_cast<long long>(getStealCount())},
			{CsrAddresses::ReplayCnt, static_cast<long long>(getReplayCount())}
		};
	}
}
