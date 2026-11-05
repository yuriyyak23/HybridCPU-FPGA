#pragma once

namespace YAKSys_Hybrid_CPU::Core::Registers::Retire
{
	enum class RetireRecordKind
	{
		RegisterWrite,
		PcWrite
	};

	/// <summary>
	/// Typed retire payload emitted by commit/writeback paths before architectural state is updated.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct RetireRecord
	class RetireRecord
	{
	private:
		RetireRecordKind Kind = RetireRecordKind::RegisterWrite;
		int VtId = 0;
		int ArchReg = 0;
		unsigned long long Value = 0;

		RetireRecord(RetireRecordKind kind, int vtId, int archReg, unsigned long long value);

	public:
		RetireRecordKind getKind() const;

		int getVtId() const;

		int getArchReg() const;

		unsigned long long getValue() const;

		bool isRegisterWrite() const;

		bool isPcWrite() const;

		static RetireRecord RegisterWrite(int vtId, int archReg, unsigned long long value);

		static RetireRecord PcWrite(int vtId, unsigned long long pc);

		RetireRecord() = default;
	};
}
