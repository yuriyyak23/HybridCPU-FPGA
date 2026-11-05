#include "LegacyCpuStateAdapter.h"

using namespace YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Core::Compat
{

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public LegacyCpuStateAdapter(ICanonicalCpuState canonicalState, byte selectedVtId)
	LegacyCpuStateAdapter::LegacyCpuStateAdapter(std::shared_ptr<ICanonicalCpuState> canonicalState, unsigned char selectedVtId) : _canonicalState((canonicalState != nullptr) ? canonicalState : throw std::invalid_argument("canonicalState")), _selectedVtId(selectedVtId)
	{
	}

	unsigned char LegacyCpuStateAdapter::getSelectedVtId() const
	{
		return _selectedVtId;
	}

	unsigned long long LegacyCpuStateAdapter::ReadIntRegister(unsigned short regID)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((ulong)_canonicalState.ReadRegister(_selectedVtId, regID));
		return static_cast<unsigned long long>(_canonicalState->ReadRegister(_selectedVtId, regID));
	}

	void LegacyCpuStateAdapter::WriteIntRegister(unsigned short regID, unsigned long long value)
	{
		_canonicalState->WriteRegister(_selectedVtId, regID, value);
	}

	unsigned long long LegacyCpuStateAdapter::GetInstructionPointer()
	{
		return _canonicalState->ReadPc(_selectedVtId);
	}

	void LegacyCpuStateAdapter::SetInstructionPointer(unsigned long long ip)
	{
		_canonicalState->WritePc(_selectedVtId, ip);
	}

	unsigned long long LegacyCpuStateAdapter::GetVL()
	{
		return _canonicalState->GetVL();
	}

	void LegacyCpuStateAdapter::SetVL(unsigned long long vl)
	{
		_canonicalState->SetVL(vl);
	}

	unsigned long long LegacyCpuStateAdapter::GetVLMAX()
	{
		return _canonicalState->GetVLMAX();
	}

	unsigned char LegacyCpuStateAdapter::GetSEW()
	{
		return _canonicalState->GetSEW();
	}

	void LegacyCpuStateAdapter::SetSEW(unsigned char sew)
	{
		_canonicalState->SetSEW(sew);
	}

	unsigned char LegacyCpuStateAdapter::GetLMUL()
	{
		return _canonicalState->GetLMUL();
	}

	void LegacyCpuStateAdapter::SetLMUL(unsigned char lmul)
	{
		_canonicalState->SetLMUL(lmul);
	}

	bool LegacyCpuStateAdapter::GetTailAgnostic()
	{
		return _canonicalState->GetTailAgnostic();
	}

	void LegacyCpuStateAdapter::SetTailAgnostic(bool agnostic)
	{
		_canonicalState->SetTailAgnostic(agnostic);
	}

	bool LegacyCpuStateAdapter::GetMaskAgnostic()
	{
		return _canonicalState->GetMaskAgnostic();
	}

	void LegacyCpuStateAdapter::SetMaskAgnostic(bool agnostic)
	{
		_canonicalState->SetMaskAgnostic(agnostic);
	}

	unsigned int LegacyCpuStateAdapter::GetExceptionMask()
	{
		return _canonicalState->GetExceptionMask();
	}

	void LegacyCpuStateAdapter::SetExceptionMask(unsigned int mask)
	{
		_canonicalState->SetExceptionMask(mask);
	}

	unsigned int LegacyCpuStateAdapter::GetExceptionPriority()
	{
		return _canonicalState->GetExceptionPriority();
	}

	void LegacyCpuStateAdapter::SetExceptionPriority(unsigned int priority)
	{
		_canonicalState->SetExceptionPriority(priority);
	}

	unsigned char LegacyCpuStateAdapter::GetRoundingMode()
	{
		return _canonicalState->GetRoundingMode();
	}

	void LegacyCpuStateAdapter::SetRoundingMode(unsigned char mode)
	{
		_canonicalState->SetRoundingMode(mode);
	}

	unsigned long long LegacyCpuStateAdapter::GetOverflowCount()
	{
		return _canonicalState->GetOverflowCount();
	}

	unsigned long long LegacyCpuStateAdapter::GetUnderflowCount()
	{
		return _canonicalState->GetUnderflowCount();
	}

	unsigned long long LegacyCpuStateAdapter::GetDivByZeroCount()
	{
		return _canonicalState->GetDivByZeroCount();
	}

	unsigned long long LegacyCpuStateAdapter::GetInvalidOpCount()
	{
		return _canonicalState->GetInvalidOpCount();
	}

	unsigned long long LegacyCpuStateAdapter::GetInexactCount()
	{
		return _canonicalState->GetInexactCount();
	}

	void LegacyCpuStateAdapter::ClearExceptionCounters()
	{
		_canonicalState->ClearExceptionCounters();
	}

	bool LegacyCpuStateAdapter::GetVectorDirty()
	{
		return _canonicalState->GetVectorDirty();
	}

	void LegacyCpuStateAdapter::SetVectorDirty(bool dirty)
	{
		_canonicalState->SetVectorDirty(dirty);
	}

	bool LegacyCpuStateAdapter::GetVectorEnabled()
	{
		return _canonicalState->GetVectorEnabled();
	}

	void LegacyCpuStateAdapter::SetVectorEnabled(bool enabled)
	{
		_canonicalState->SetVectorEnabled(enabled);
	}

	long long LegacyCpuStateAdapter::ReadRegister(unsigned char vtId, int regId)
	{
		return _canonicalState->ReadRegister(vtId, regId);
	}

	void LegacyCpuStateAdapter::WriteRegister(unsigned char vtId, int regId, unsigned long long value)
	{
		_canonicalState->WriteRegister(vtId, regId, value);
	}

	unsigned short LegacyCpuStateAdapter::GetPredicateMask(unsigned short maskID)
	{
		return _canonicalState->GetPredicateMask(maskID);
	}

	void LegacyCpuStateAdapter::SetPredicateMask(unsigned short maskID, unsigned short mask)
	{
		_canonicalState->SetPredicateMask(maskID, mask);
	}

	unsigned long long LegacyCpuStateAdapter::ReadPc(unsigned char vtId)
	{
		return _canonicalState->ReadPc(vtId);
	}

	void LegacyCpuStateAdapter::WritePc(unsigned char vtId, unsigned long long pc)
	{
		_canonicalState->WritePc(vtId, pc);
	}

	unsigned short LegacyCpuStateAdapter::GetCoreID()
	{
		return _canonicalState->GetCoreID();
	}

	unsigned long long LegacyCpuStateAdapter::GetCycleCount()
	{
		return _canonicalState->GetCycleCount();
	}

	unsigned long long LegacyCpuStateAdapter::GetInstructionsRetired()
	{
		return _canonicalState->GetInstructionsRetired();
	}

	double LegacyCpuStateAdapter::GetIPC()
	{
		return _canonicalState->GetIPC();
	}

	YAKSys_Hybrid_CPU::Core::PipelineState LegacyCpuStateAdapter::GetCurrentPipelineState()
	{
		return _canonicalState->GetCurrentPipelineState();
	}

	void LegacyCpuStateAdapter::SetCurrentPipelineState(PipelineState state)
	{
		_canonicalState->SetCurrentPipelineState(state);
	}

	void LegacyCpuStateAdapter::TransitionPipelineState(PipelineTransitionTrigger trigger)
	{
		_canonicalState->TransitionPipelineState(trigger);
	}
}
