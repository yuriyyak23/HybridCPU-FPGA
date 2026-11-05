#include "RetireRecord.h"

namespace YAKSys_Hybrid_CPU::Core::Registers::Retire
{

	RetireRecord::RetireRecord(RetireRecordKind kind, int vtId, int archReg, unsigned long long value)
	{
		Kind = kind;
		VtId = vtId;
		ArchReg = archReg;
		Value = value;
	}

	RetireRecordKind RetireRecord::getKind() const
	{
		return Kind;
	}

	int RetireRecord::getVtId() const
	{
		return VtId;
	}

	int RetireRecord::getArchReg() const
	{
		return ArchReg;
	}

	unsigned long long RetireRecord::getValue() const
	{
		return Value;
	}

	bool RetireRecord::isRegisterWrite() const
	{
		return getKind() == RetireRecordKind::RegisterWrite;
	}

	bool RetireRecord::isPcWrite() const
	{
		return getKind() == RetireRecordKind::PcWrite;
	}

	RetireRecord RetireRecord::RegisterWrite(int vtId, int archReg, unsigned long long value)
	{
		return RetireRecord(RetireRecordKind::RegisterWrite, vtId, archReg, value);
	}

	RetireRecord RetireRecord::PcWrite(int vtId, unsigned long long pc)
	{
		return RetireRecord(RetireRecordKind::PcWrite, vtId, 0, pc);
	}
}
