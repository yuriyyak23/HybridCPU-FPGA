#pragma once

#include "../../../../../CloseToHSL/Core/State/Compat/ICpuState.h"
#include "../../../../../CloseToHSL/Core/State/ICpuState.h"
#include "../../../../../CloseToHSL/Core/Architecture/State/Pipeline/PipelineState.h"
#include "../../../../../CloseToHSL/Core/Architecture/State/Pipeline/PipelineTransitionTrigger.h"
#include <stdexcept>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Core::Compat
{
	/// <summary>
	/// Explicit compat shim that binds the legacy non-VT state surface to a selected VT.
	/// This keeps obsolete single-thread semantics outside the production live adapter.
	/// </summary>
	class LegacyCpuStateAdapter final : public ICpuState
	{
	private:
		const std::shared_ptr<ICanonicalCpuState> _canonicalState;
		const unsigned char _selectedVtId;

	public:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public LegacyCpuStateAdapter(ICanonicalCpuState canonicalState, byte selectedVtId)
		LegacyCpuStateAdapter(std::shared_ptr<ICanonicalCpuState> canonicalState, unsigned char selectedVtId);

		unsigned char getSelectedVtId() const;

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Obsolete("Use ReadRegister(byte vtId, int regId) for VT-scoped access.")] public ulong ReadIntRegister(ushort regID)
		unsigned long long ReadIntRegister(unsigned short regID) override;

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Obsolete("Use WriteRegister(byte vtId, int regId, ulong value) for VT-scoped access.")] public void WriteIntRegister(ushort regID, ulong value)
		void WriteIntRegister(unsigned short regID, unsigned long long value) override;

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Obsolete("Use ReadPc(byte vtId) for VT-scoped access.")] public ulong GetInstructionPointer()
		unsigned long long GetInstructionPointer() override;

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Obsolete("Use WritePc(byte vtId, ulong pc) for VT-scoped access.")] public void SetInstructionPointer(ulong ip)
		void SetInstructionPointer(unsigned long long ip) override;

		unsigned long long GetVL() override;
		void SetVL(unsigned long long vl) override;
		unsigned long long GetVLMAX() override;
		unsigned char GetSEW() override;
		void SetSEW(unsigned char sew) override;
		unsigned char GetLMUL() override;
		void SetLMUL(unsigned char lmul) override;
		bool GetTailAgnostic() override;
		void SetTailAgnostic(bool agnostic) override;
		bool GetMaskAgnostic() override;
		void SetMaskAgnostic(bool agnostic) override;
		unsigned int GetExceptionMask() override;
		void SetExceptionMask(unsigned int mask) override;
		unsigned int GetExceptionPriority() override;
		void SetExceptionPriority(unsigned int priority) override;
		unsigned char GetRoundingMode() override;
		void SetRoundingMode(unsigned char mode) override;
		unsigned long long GetOverflowCount() override;
		unsigned long long GetUnderflowCount() override;
		unsigned long long GetDivByZeroCount() override;
		unsigned long long GetInvalidOpCount() override;
		unsigned long long GetInexactCount() override;
		void ClearExceptionCounters() override;
		bool GetVectorDirty() override;
		void SetVectorDirty(bool dirty) override;
		bool GetVectorEnabled() override;
		void SetVectorEnabled(bool enabled) override;
		long long ReadRegister(unsigned char vtId, int regId) override;
		void WriteRegister(unsigned char vtId, int regId, unsigned long long value) override;
		unsigned short GetPredicateMask(unsigned short maskID) override;
		void SetPredicateMask(unsigned short maskID, unsigned short mask) override;
		unsigned long long ReadPc(unsigned char vtId) override;
		void WritePc(unsigned char vtId, unsigned long long pc) override;
		unsigned short GetCoreID() override;
		unsigned long long GetCycleCount() override;
		unsigned long long GetInstructionsRetired() override;
		double GetIPC() override;
		PipelineState GetCurrentPipelineState() override;
		void SetCurrentPipelineState(PipelineState state) override;
		void TransitionPipelineState(PipelineTransitionTrigger trigger) override;
	};
}
