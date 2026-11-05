#include "YAKSys_Hybrid_CPU.Core.VectorStreamSnapshot.h"
#include "../../../../../../tangible_exception_helper.h"

namespace YAKSys_Hybrid_CPU::Core
{
	namespace
	{
		std::vector<unsigned short> CopyPredicateMasks(const std::vector<unsigned short> &predicateMasks)
		{
			if (predicateMasks.size() != VectorStreamSnapshot::PredicateRegisterCount)
			{
				throw std::invalid_argument("Vector/stream snapshot requires exactly 16 predicate masks.");
			}
			return predicateMasks;
		}

		VectorStreamSaveMask IntersectSaveMasks(VectorStreamSaveMask left, VectorStreamSaveMask right)
		{
			return static_cast<VectorStreamSaveMask>(static_cast<int>(left) & static_cast<int>(right));
		}

		bool HasSaveMask(VectorStreamSaveMask mask, VectorStreamSaveMask flag)
		{
			return (static_cast<int>(mask) & static_cast<int>(flag)) != 0;
		}
	}

	std::vector<unsigned short> VectorStreamSnapshot::SnapshotPredicateMasks()
	{
		std::vector<unsigned short> snapshot(PredicateRegisterCount);
		std::copy_n(_predicateMasks.begin(), snapshot.size(), snapshot.begin());
		return snapshot;
	}

	void VectorStreamSnapshot::RestoreTo(std::shared_ptr<ICanonicalCpuState> state)
	{
		RestoreTo(state, getSaveMask());
	}

	void VectorStreamSnapshot::RestoreTo(std::shared_ptr<ICanonicalCpuState> state, VectorStreamSaveMask restoreMask)
	{
		ArgumentNullException::ThrowIfNull(state);

		VectorStreamSaveMask effectiveMask = IntersectSaveMasks(getSaveMask(), restoreMask);
		if (HasSaveMask(effectiveMask, VectorStreamSaveMask::VectorConfig))
		{
			state->SetVL(getVL());
			state->SetSEW(getSEW());
			state->SetLMUL(getLMUL());
			state->SetTailAgnostic(getTailAgnostic());
			state->SetMaskAgnostic(getMaskAgnostic());
			state->SetRoundingMode(getRoundingMode());
			state->SetVectorEnabled(getVectorEnabled());
		}

		if (HasSaveMask(effectiveMask, VectorStreamSaveMask::ExceptionPolicy))
		{
			state->SetExceptionMask(getExceptionMask());
			state->SetExceptionPriority(getExceptionPriority());
		}

		if (HasSaveMask(effectiveMask, VectorStreamSaveMask::ExceptionCounters))
		{
			state->ClearExceptionCounters();
			std::shared_ptr<IVectorExceptionCounterState> counterState = std::dynamic_pointer_cast<IVectorExceptionCounterState>(state);
			if (counterState != nullptr)
			{
				counterState->SetVectorExceptionCounters(getOverflowCount(), getUnderflowCount(), getDivByZeroCount(), getInvalidOpCount(), getInexactCount());
			}
		}

		if (HasSaveMask(effectiveMask, VectorStreamSaveMask::DirtyState))
		{
			state->SetVectorDirty(getVectorDirty());
		}

		if (HasSaveMask(effectiveMask, VectorStreamSaveMask::PredicateRegisters))
		{
			for (unsigned short index = 0; index < PredicateRegisterCount; index++)
			{
				state->SetPredicateMask(index, _predicateMasks[index]);
			}
		}
	}

	VectorStreamSnapshot::VectorStreamSnapshot(unsigned long long vl, unsigned char sew, unsigned char lmul, bool tailAgnostic, bool maskAgnostic, unsigned int exceptionMask, unsigned int exceptionPriority, unsigned char roundingMode, unsigned long long overflowCount, unsigned long long underflowCount, unsigned long long divByZeroCount, unsigned long long invalidOpCount, unsigned long long inexactCount, bool vectorDirty, bool vectorEnabled, const std::vector<unsigned short> &predicateMasks, unsigned long long streamDescriptorTableBase, unsigned long long streamDescriptorTableLimit, unsigned long long streamReplayEpoch, unsigned long long streamQueueEpoch, unsigned long long streamCompletionEpoch, VectorStreamSaveMask saveMask, unsigned long long architecturalEpoch)
		: VL(vl), SEW(sew), LMUL(lmul), TailAgnostic(tailAgnostic), MaskAgnostic(maskAgnostic),
		  ExceptionMask(exceptionMask), ExceptionPriority(exceptionPriority), RoundingMode(roundingMode),
		  OverflowCount(overflowCount), UnderflowCount(underflowCount), DivByZeroCount(divByZeroCount),
		  InvalidOpCount(invalidOpCount), InexactCount(inexactCount), VectorDirty(vectorDirty),
		  VectorEnabled(vectorEnabled), StreamDescriptorTableBase(streamDescriptorTableBase),
		  StreamDescriptorTableLimit(streamDescriptorTableLimit), StreamReplayEpoch(streamReplayEpoch),
		  StreamQueueEpoch(streamQueueEpoch), StreamCompletionEpoch(streamCompletionEpoch),
		  SaveMask(saveMask), ArchitecturalEpoch(architecturalEpoch),
		  _predicateMasks(CopyPredicateMasks(predicateMasks))
	{
	}

	unsigned long long VectorStreamSnapshot::getVL() const
	{
		return VL;
	}

	unsigned char VectorStreamSnapshot::getSEW() const
	{
		return SEW;
	}

	unsigned char VectorStreamSnapshot::getLMUL() const
	{
		return LMUL;
	}

	bool VectorStreamSnapshot::getTailAgnostic() const
	{
		return TailAgnostic;
	}

	bool VectorStreamSnapshot::getMaskAgnostic() const
	{
		return MaskAgnostic;
	}

	unsigned int VectorStreamSnapshot::getExceptionMask() const
	{
		return ExceptionMask;
	}

	unsigned int VectorStreamSnapshot::getExceptionPriority() const
	{
		return ExceptionPriority;
	}

	unsigned char VectorStreamSnapshot::getRoundingMode() const
	{
		return RoundingMode;
	}

	unsigned long long VectorStreamSnapshot::getOverflowCount() const
	{
		return OverflowCount;
	}

	unsigned long long VectorStreamSnapshot::getUnderflowCount() const
	{
		return UnderflowCount;
	}

	unsigned long long VectorStreamSnapshot::getDivByZeroCount() const
	{
		return DivByZeroCount;
	}

	unsigned long long VectorStreamSnapshot::getInvalidOpCount() const
	{
		return InvalidOpCount;
	}

	unsigned long long VectorStreamSnapshot::getInexactCount() const
	{
		return InexactCount;
	}

	bool VectorStreamSnapshot::getVectorDirty() const
	{
		return VectorDirty;
	}

	bool VectorStreamSnapshot::getVectorEnabled() const
	{
		return VectorEnabled;
	}

	const std::vector<unsigned short> VectorStreamSnapshot::getPredicateMasks() const
	{
		return _predicateMasks;
	}

	unsigned long long VectorStreamSnapshot::getStreamDescriptorTableBase() const
	{
		return StreamDescriptorTableBase;
	}

	unsigned long long VectorStreamSnapshot::getStreamDescriptorTableLimit() const
	{
		return StreamDescriptorTableLimit;
	}

	unsigned long long VectorStreamSnapshot::getStreamReplayEpoch() const
	{
		return StreamReplayEpoch;
	}

	unsigned long long VectorStreamSnapshot::getStreamQueueEpoch() const
	{
		return StreamQueueEpoch;
	}

	unsigned long long VectorStreamSnapshot::getStreamCompletionEpoch() const
	{
		return StreamCompletionEpoch;
	}

	VectorStreamSaveMask VectorStreamSnapshot::getSaveMask() const
	{
		return SaveMask;
	}

	unsigned long long VectorStreamSnapshot::getArchitecturalEpoch() const
	{
		return ArchitecturalEpoch;
	}

	std::shared_ptr<VectorStreamSnapshot> VectorStreamSnapshot::Capture(std::shared_ptr<ICanonicalCpuState> state, unsigned long long streamDescriptorTableBase, unsigned long long streamDescriptorTableLimit, unsigned long long streamReplayEpoch, unsigned long long streamQueueEpoch, unsigned long long streamCompletionEpoch, VectorStreamSaveMask saveMask, unsigned long long architecturalEpoch)
	{
		ArgumentNullException::ThrowIfNull(state);

		std::vector<unsigned short> predicateMasks(PredicateRegisterCount);
		for (unsigned short index = 0; index < PredicateRegisterCount; index++)
		{
			predicateMasks[index] = state->GetPredicateMask(index);
		}

		return std::make_shared<VectorStreamSnapshot>(state->GetVL(), state->GetSEW(), state->GetLMUL(), state->GetTailAgnostic(), state->GetMaskAgnostic(), state->GetExceptionMask(), state->GetExceptionPriority(), state->GetRoundingMode(), state->GetOverflowCount(), state->GetUnderflowCount(), state->GetDivByZeroCount(), state->GetInvalidOpCount(), state->GetInexactCount(), state->GetVectorDirty(), state->GetVectorEnabled(), predicateMasks, streamDescriptorTableBase, streamDescriptorTableLimit, streamReplayEpoch, streamQueueEpoch, streamCompletionEpoch, saveMask, architecturalEpoch);
	}

	std::wstring VectorStreamSnapshot::ToString()
	{
		return std::format(L"Vector/stream snapshot: vl={}, sew={}, lmul={}, predicates={}, streamEpoch={}", getVL(), static_cast<unsigned int>(getSEW()), static_cast<unsigned int>(getLMUL()), _predicateMasks.size(), getStreamReplayEpoch());
	}

	bool VectorStreamSnapshot::ContainsHostEvidence(Vmcs::V2::VmcsV2HostEvidenceKind evidence)
	{
		return false;
	}
}
