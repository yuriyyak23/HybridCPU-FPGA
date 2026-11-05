#include "ReplayValidator.h"
#include "TraceEventKind.h"

namespace YAKSys_Hybrid_CPU::Core
{
	namespace
	{
		std::wstring TraceEventKindName(TraceEventKind kind)
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
			default: return std::to_wstring(static_cast<unsigned int>(kind));
			}
		}

		std::wstring PipelineStateName(PipelineState state)
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
			default: return std::to_wstring(static_cast<int>(state));
			}
		}
	}

	bool ReplayValidationResult::isDeterministic() const
	{
		return IsDeterministic;
	}

	int ReplayValidationResult::getComparedEventCount() const
	{
		return ComparedEventCount;
	}

	int ReplayValidationResult::getDivergenceIndex() const
	{
		return DivergenceIndex;
	}

	std::wstring ReplayValidationResult::getDivergenceDescription() const
	{
		return DivergenceDescription;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private ReplayValidationResult(bool isDeterministic, int comparedEventCount, int divergenceIndex, string? divergenceDescription)
	ReplayValidationResult::ReplayValidationResult(bool isDeterministic, int comparedEventCount, int divergenceIndex, const std::wstring &divergenceDescription)
		: IsDeterministic(isDeterministic), ComparedEventCount(comparedEventCount), DivergenceIndex(divergenceIndex), DivergenceDescription(divergenceDescription)
	{
	}

	ReplayValidationResult ReplayValidationResult::Deterministic(int comparedEventCount)
	{
		return ReplayValidationResult(true, comparedEventCount, -1, L"");
	}

	ReplayValidationResult ReplayValidationResult::Diverged(int comparedEventCount, int divergenceIndex, const std::wstring &divergenceDescription)
	{
		return ReplayValidationResult(false, comparedEventCount, divergenceIndex, divergenceDescription);
	}

	std::wstring ReplayValidationResult::ToString() const
	{
		return isDeterministic() ? std::format(L"Deterministic: {} events matched", getComparedEventCount()) : std::format(L"Diverged at event {}/{}: {}", getDivergenceIndex(), getComparedEventCount(), getDivergenceDescription());
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public ReplayValidationResult ValidateTrace(IReadOnlyList<V4TraceEvent> original, IReadOnlyList<V4TraceEvent> replay)
	ReplayValidationResult ReplayValidator::ValidateTrace(const std::vector<V4TraceEvent> &original, const std::vector<V4TraceEvent> &replay)
	{
		int compareCount = std::min(original.size(), replay.size());

		// Compute the bundle serial base for each trace independently so that
		// relative offsets can be compared without signed overflow.
		unsigned long long origBase = compareCount > 0 ? original[0].getBundleSerial() : 0ULL;
		unsigned long long repBase = compareCount > 0 ? replay[0].getBundleSerial() : 0ULL;

		for (int i = 0; i < compareCount; i++)
		{
			V4TraceEvent orig = original[i];
			V4TraceEvent rep = replay[i];

			if (orig.getKind() != rep.getKind())
			{
				return ReplayValidationResult::Diverged(compareCount, i, std::format(L"Kind mismatch at event {}: original={}, replay={}", i, TraceEventKindName(orig.getKind()), TraceEventKindName(rep.getKind())));
			}

			if (orig.getVtId() != rep.getVtId())
			{
				return ReplayValidationResult::Diverged(compareCount, i, std::format(L"VtId mismatch at event {}: original={}, replay={}", i, static_cast<unsigned int>(orig.getVtId()), static_cast<unsigned int>(rep.getVtId())));
			}

			if (orig.getFsmState() != rep.getFsmState())
			{
				return ReplayValidationResult::Diverged(compareCount, i, std::format(L"FsmState mismatch at event {}: original={}, replay={}", i, PipelineStateName(orig.getFsmState()), PipelineStateName(rep.getFsmState())));
			}

			if (orig.getPayload() != rep.getPayload())
			{
				return ReplayValidationResult::Diverged(compareCount, i, std::format(L"Payload mismatch at event {}: original=0x{:X}, replay=0x{:X}", i, orig.getPayload(), rep.getPayload()));
			}

			// BundleSerial must be offset-equal (replay starts from a different serial)
			unsigned long long origRelSerial = orig.getBundleSerial() - origBase;
			unsigned long long repRelSerial = rep.getBundleSerial() - repBase;
			if (origRelSerial != repRelSerial)
			{
				return ReplayValidationResult::Diverged(compareCount, i, std::format(L"BundleSerial mismatch at event {}: original relative={}, replay relative={}", i, origRelSerial, repRelSerial));
			}
		}

		// If one trace is longer, compare count differences constitute a divergence
		if (original.size() != replay.size())
		{
			return ReplayValidationResult::Diverged(compareCount, compareCount, std::format(L"Trace length mismatch: original={}, replay={}", original.size(), replay.size()));
		}

		return ReplayValidationResult::Deterministic(compareCount);
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public ReplayValidationResult ValidateTrace(IReadOnlyList<V4TraceEvent> original, IReadOnlyList<V4TraceEvent> replay, int maxBundleCount)
	ReplayValidationResult ReplayValidator::ValidateTrace(const std::vector<V4TraceEvent> &original, const std::vector<V4TraceEvent> &replay, int maxBundleCount)
	{
		std::vector<V4TraceEvent> croppedOriginal = CropToBundles(original, maxBundleCount);
		std::vector<V4TraceEvent> croppedReplay = CropToBundles(replay, maxBundleCount);
		return ValidateTrace(croppedOriginal, croppedReplay);
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static List<V4TraceEvent> CropToBundles(IReadOnlyList<V4TraceEvent> trace, int maxBundles)
	std::vector<V4TraceEvent> ReplayValidator::CropToBundles(const std::vector<V4TraceEvent> &trace, int maxBundles)
	{
		std::vector<V4TraceEvent> result;
		int bundleCount = 0;
		for (auto evt : trace)
		{
			result.push_back(evt);
			if (evt.getKind() == TraceEventKind::BundleRetired)
			{
				bundleCount++;
				if (bundleCount >= maxBundles)
				{
					break;
				}
			}
		}
		return result;
	}
}
