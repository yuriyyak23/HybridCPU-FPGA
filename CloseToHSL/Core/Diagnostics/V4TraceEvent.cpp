#include "V4TraceEvent.h"
#include <iomanip>
#include <sstream>

namespace YAKSys_Hybrid_CPU::Core
{
	namespace
	{
		const wchar_t* PipelineStateName(PipelineState state)
		{
			switch (state)
			{
			case PipelineState::Task: return L"Task";
			case PipelineState::VmEntry: return L"VmEntry";
			case PipelineState::GuestExecution: return L"GuestExecution";
			case PipelineState::VmExit: return L"VmExit";
			case PipelineState::Halted: return L"Halted";
			case PipelineState::Reset: return L"Reset";
			case PipelineState::WaitForClusterSync: return L"WaitForClusterSync";
			case PipelineState::PtwStall: return L"PtwStall";
			case PipelineState::ClockGatedDonor: return L"ClockGatedDonor";
			case PipelineState::TrapPending: return L"TrapPending";
			case PipelineState::WaitForEvent: return L"WaitForEvent";
			default: return L"Unknown";
			}
		}

		const wchar_t* TraceEventKindName(TraceEventKind kind)
		{
			switch (kind)
			{
			case TraceEventKind::BundleDispatched: return L"BundleDispatched";
			case TraceEventKind::BundleRetired: return L"BundleRetired";
			case TraceEventKind::BundleReplayed: return L"BundleReplayed";
			case TraceEventKind::AluExecuted: return L"AluExecuted";
			case TraceEventKind::LoadExecuted: return L"LoadExecuted";
			case TraceEventKind::StoreExecuted: return L"StoreExecuted";
			case TraceEventKind::FenceExecuted: return L"FenceExecuted";
			case TraceEventKind::BranchTaken: return L"BranchTaken";
			case TraceEventKind::BranchNotTaken: return L"BranchNotTaken";
			case TraceEventKind::JumpExecuted: return L"JumpExecuted";
			case TraceEventKind::LrExecuted: return L"LrExecuted";
			case TraceEventKind::ScSucceeded: return L"ScSucceeded";
			case TraceEventKind::ScFailed: return L"ScFailed";
			case TraceEventKind::AmoWordExecuted: return L"AmoWordExecuted";
			case TraceEventKind::AmoDwordExecuted: return L"AmoDwordExecuted";
			case TraceEventKind::TrapTaken: return L"TrapTaken";
			case TraceEventKind::PrivilegeReturn: return L"PrivilegeReturn";
			case TraceEventKind::WfiEntered: return L"WfiEntered";
			case TraceEventKind::CsrRead: return L"CsrRead";
			case TraceEventKind::CsrWrite: return L"CsrWrite";
			case TraceEventKind::VtYield: return L"VtYield";
			case TraceEventKind::VtWfe: return L"VtWfe";
			case TraceEventKind::VtSev: return L"VtSev";
			case TraceEventKind::PodBarrierEntered: return L"PodBarrierEntered";
			case TraceEventKind::PodBarrierExited: return L"PodBarrierExited";
			case TraceEventKind::VtBarrierEntered: return L"VtBarrierEntered";
			case TraceEventKind::VtBarrierExited: return L"VtBarrierExited";
			case TraceEventKind::VmxOn: return L"VmxOn";
			case TraceEventKind::VmxOff: return L"VmxOff";
			case TraceEventKind::VmEntry: return L"VmEntry";
			case TraceEventKind::VmEntryFailed: return L"VmEntryFailed";
			case TraceEventKind::VmExit: return L"VmExit";
			case TraceEventKind::VmcsRead: return L"VmcsRead";
			case TraceEventKind::VmcsWrite: return L"VmcsWrite";
			case TraceEventKind::FspPilfer: return L"FspPilfer";
			case TraceEventKind::FspBoundary: return L"FspBoundary";
			case TraceEventKind::FsmTransition: return L"FsmTransition";
			default: return L"Unknown";
			}
		}

		std::wstring FormatPayload(unsigned long long payload)
		{
			std::wostringstream stream;
			stream << std::uppercase << std::hex << payload;
			return stream.str();
		}
	}

	unsigned long long V4TraceEvent::getBundleSerial() const
	{
		return BundleSerial;
	}

	void V4TraceEvent::setBundleSerial(unsigned long long value)
	{
		BundleSerial = value;
	}

	unsigned char V4TraceEvent::getVtId() const
	{
		return VtId;
	}

	void V4TraceEvent::setVtId(unsigned char value)
	{
		VtId = value;
	}

	PipelineState V4TraceEvent::getFsmState() const
	{
		return FsmState;
	}

	void V4TraceEvent::setFsmState(PipelineState value)
	{
		FsmState = value;
	}

	TraceEventKind V4TraceEvent::getKind() const
	{
		return Kind;
	}

	void V4TraceEvent::setKind(TraceEventKind value)
	{
		Kind = value;
	}

	unsigned long long V4TraceEvent::getPayload() const
	{
		return Payload;
	}

	void V4TraceEvent::setPayload(unsigned long long value)
	{
		Payload = value;
	}

	V4TraceEvent V4TraceEvent::Create(unsigned long long bundleSerial, unsigned char vtId, PipelineState fsmState, TraceEventKind kind)
	{
		return V4TraceEvent::Create(bundleSerial, vtId, fsmState, kind, 0ULL);
	}

	V4TraceEvent V4TraceEvent::Create(unsigned long long bundleSerial, unsigned char vtId, PipelineState fsmState, TraceEventKind kind, unsigned long long payload)
	{
		V4TraceEvent result;
		result.setBundleSerial(bundleSerial);
		result.setVtId(vtId);
		result.setFsmState(fsmState);
		result.setKind(kind);
		result.setPayload(payload);
		return result;
	}

	std::wstring V4TraceEvent::ToString() const
	{
		return L"[Bundle=" + std::to_wstring(getBundleSerial()) + L" VT=" +
			std::to_wstring(getVtId()) + L" FSM=" + PipelineStateName(getFsmState()) +
			L"] " + TraceEventKindName(getKind()) + L" payload=0x" + FormatPayload(getPayload());
	}
}
