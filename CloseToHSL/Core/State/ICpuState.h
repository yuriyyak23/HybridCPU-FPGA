#pragma once

#include "../Architecture/State/Pipeline/PipelineState.h"
#include "../Architecture/State/Pipeline/PipelineTransitionTrigger.h"

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Canonical VT-scoped architectural state surface used by execution, pipeline, and VMX paths.
	/// All register and PC reads/writes must flow through the VT-aware members on this contract.
	/// </summary>
	class ICanonicalCpuState
	{
	public:
		virtual unsigned long long GetVL() = 0;
		virtual void SetVL(unsigned long long vl) = 0;

		virtual unsigned long long GetVLMAX() = 0;

		virtual unsigned char GetSEW() = 0;
		virtual void SetSEW(unsigned char sew) = 0;

		virtual unsigned char GetLMUL() = 0;
		virtual void SetLMUL(unsigned char lmul) = 0;

		virtual bool GetTailAgnostic() = 0;
		virtual void SetTailAgnostic(bool agnostic) = 0;

		virtual bool GetMaskAgnostic() = 0;
		virtual void SetMaskAgnostic(bool agnostic) = 0;

		virtual unsigned int GetExceptionMask() = 0;
		virtual void SetExceptionMask(unsigned int mask) = 0;

		virtual unsigned int GetExceptionPriority() = 0;
		virtual void SetExceptionPriority(unsigned int priority) = 0;

		virtual unsigned char GetRoundingMode() = 0;
		virtual void SetRoundingMode(unsigned char mode) = 0;

		virtual unsigned long long GetOverflowCount() = 0;
		virtual unsigned long long GetUnderflowCount() = 0;
		virtual unsigned long long GetDivByZeroCount() = 0;
		virtual unsigned long long GetInvalidOpCount() = 0;
		virtual unsigned long long GetInexactCount() = 0;

		virtual void ClearExceptionCounters() = 0;

		virtual bool GetVectorDirty() = 0;
		virtual void SetVectorDirty(bool dirty) = 0;

		virtual bool GetVectorEnabled() = 0;
		virtual void SetVectorEnabled(bool enabled) = 0;

		virtual long long ReadRegister(unsigned char vtId, int regId) = 0;
		virtual void WriteRegister(unsigned char vtId, int regId, unsigned long long value) = 0;

		virtual unsigned short GetPredicateMask(unsigned short maskID) = 0;
		virtual void SetPredicateMask(unsigned short maskID, unsigned short mask) = 0;

		virtual unsigned long long ReadPc(unsigned char vtId) = 0;
		virtual void WritePc(unsigned char vtId, unsigned long long pc) = 0;

		virtual unsigned short GetCoreID() = 0;

		virtual unsigned long long GetCycleCount() = 0;
		virtual unsigned long long GetInstructionsRetired() = 0;
		virtual double GetIPC() = 0;

		virtual PipelineState GetCurrentPipelineState() = 0;
		virtual void SetCurrentPipelineState(PipelineState state) = 0;
		virtual void TransitionPipelineState(PipelineTransitionTrigger trigger) = 0;
	};

}
