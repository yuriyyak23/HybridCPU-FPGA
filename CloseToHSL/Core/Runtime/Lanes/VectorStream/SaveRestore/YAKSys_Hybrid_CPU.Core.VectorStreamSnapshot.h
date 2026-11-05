#pragma once

#include "../State/VectorStreamState.h"
#include "../../../../State/ICpuState.h"
#include "../../../../System/Vmcs/V2/VmcsV2HostEvidenceKind.h"
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <format>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class VectorStreamSnapshot final : public std::enable_shared_from_this<VectorStreamSnapshot>
	{
	private:
		const unsigned long long VL;
		const unsigned char SEW;
		const unsigned char LMUL;
		const bool TailAgnostic;
		const bool MaskAgnostic;
		const unsigned int ExceptionMask;
		const unsigned int ExceptionPriority;
		const unsigned char RoundingMode;
		const unsigned long long OverflowCount;
		const unsigned long long UnderflowCount;
		const unsigned long long DivByZeroCount;
		const unsigned long long InvalidOpCount;
		const unsigned long long InexactCount;
		const bool VectorDirty;
		const bool VectorEnabled;
		const unsigned long long StreamDescriptorTableBase;
		const unsigned long long StreamDescriptorTableLimit;
		const unsigned long long StreamReplayEpoch;
		const unsigned long long StreamQueueEpoch;
		const unsigned long long StreamCompletionEpoch;
		const VectorStreamSaveMask SaveMask;
		const unsigned long long ArchitecturalEpoch;

	public:
		std::vector<unsigned short> SnapshotPredicateMasks();

		void RestoreTo(std::shared_ptr<ICanonicalCpuState> state);

		void RestoreTo(std::shared_ptr<ICanonicalCpuState> state, VectorStreamSaveMask restoreMask);



		static constexpr int PredicateRegisterCount = 16;

	private:
		const std::vector<unsigned short> _predicateMasks;

	public:
		VectorStreamSnapshot(unsigned long long vl, unsigned char sew, unsigned char lmul, bool tailAgnostic, bool maskAgnostic, unsigned int exceptionMask, unsigned int exceptionPriority, unsigned char roundingMode, unsigned long long overflowCount, unsigned long long underflowCount, unsigned long long divByZeroCount, unsigned long long invalidOpCount, unsigned long long inexactCount, bool vectorDirty, bool vectorEnabled, const std::vector<unsigned short> &predicateMasks, unsigned long long streamDescriptorTableBase, unsigned long long streamDescriptorTableLimit, unsigned long long streamReplayEpoch, unsigned long long streamQueueEpoch, unsigned long long streamCompletionEpoch, VectorStreamSaveMask saveMask, unsigned long long architecturalEpoch);

		unsigned long long getVL() const;

		unsigned char getSEW() const;

		unsigned char getLMUL() const;

		bool getTailAgnostic() const;

		bool getMaskAgnostic() const;

		unsigned int getExceptionMask() const;

		unsigned int getExceptionPriority() const;

		unsigned char getRoundingMode() const;

		unsigned long long getOverflowCount() const;

		unsigned long long getUnderflowCount() const;

		unsigned long long getDivByZeroCount() const;

		unsigned long long getInvalidOpCount() const;

		unsigned long long getInexactCount() const;

		bool getVectorDirty() const;

		bool getVectorEnabled() const;

		const std::vector<unsigned short> getPredicateMasks() const;

		unsigned long long getStreamDescriptorTableBase() const;

		unsigned long long getStreamDescriptorTableLimit() const;

		unsigned long long getStreamReplayEpoch() const;

		unsigned long long getStreamQueueEpoch() const;

		unsigned long long getStreamCompletionEpoch() const;

		VectorStreamSaveMask getSaveMask() const;

		unsigned long long getArchitecturalEpoch() const;

		static std::shared_ptr<VectorStreamSnapshot> Capture(std::shared_ptr<ICanonicalCpuState> state, unsigned long long streamDescriptorTableBase, unsigned long long streamDescriptorTableLimit, unsigned long long streamReplayEpoch, unsigned long long streamQueueEpoch, unsigned long long streamCompletionEpoch, VectorStreamSaveMask saveMask, unsigned long long architecturalEpoch);

		std::wstring ToString();


		bool ContainsHostEvidence(Vmcs::V2::VmcsV2HostEvidenceKind evidence);
	};
}
