#pragma once

#include "CpuInstructionTranslationContour.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core::Memory
{

	enum class CpuSecondStageFaultKind : unsigned char
	{
		None = 0,
		NotPresent = 1,
		PermissionDenied = 2,
		Misconfiguration = 3,
		OutsideOwnedAddressSpace = 4,
		SourceStale = 5
	};

	struct CpuSecondStageTranslationResult final
	{
		const bool Succeeded;
		const unsigned long long CpuPhysicalAddress;
		const CpuSecondStageFaultKind FaultKind;
		const unsigned long long NeutralAuxiliary;

		CpuSecondStageTranslationResult(bool succeeded, unsigned long long cpuPhysicalAddress,
			CpuSecondStageFaultKind faultKind, unsigned long long neutralAuxiliary)
			: Succeeded(succeeded), CpuPhysicalAddress(cpuPhysicalAddress), FaultKind(faultKind), NeutralAuxiliary(neutralAuxiliary) {}

		static std::shared_ptr<CpuSecondStageTranslationResult> Success(unsigned long long cpuPhysicalAddress)
		{
			return std::make_shared<CpuSecondStageTranslationResult>(true, cpuPhysicalAddress, CpuSecondStageFaultKind::None, 0);
		}
		static std::shared_ptr<CpuSecondStageTranslationResult> Failed(CpuSecondStageFaultKind kind, unsigned long long auxiliary)
		{
			return std::make_shared<CpuSecondStageTranslationResult>(false, 0, kind, auxiliary);
		}
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct CpuSecondStageTranslationResult(bool Succeeded, ulong CpuPhysicalAddress, CpuSecondStageFaultKind FaultKind, ulong NeutralAuxiliary)
//	{
//		internal static CpuSecondStageTranslationResult Success(ulong cpuPhysicalAddress)
//		{
//			return new(true, cpuPhysicalAddress, CpuSecondStageFaultKind.None, 0);
//		}
//
//		internal static CpuSecondStageTranslationResult Failed(CpuSecondStageFaultKind kind, ulong auxiliary)
//		{
//			return new(false, 0, kind, auxiliary);
//		}
//	}

	/// <summary>
	/// Neutral typed encoding owned by the CPU translation contour. It is not a
	/// VMX/EPT qualification value. Compatibility projection must map each member
	/// explicitly and may not return this encoding unchanged.
	/// </summary>
	class CpuSecondStageNeutralAuxiliary final
	{
	public:
		static unsigned long long Encode(CpuInstructionTranslationAccessKind accessKind, unsigned short accessSize, CpuSecondStageFaultKind kind, unsigned char pageWalkLevel);

		static bool TryDecode(unsigned long long value, CpuSecondStageFaultKind &kind, CpuInstructionTranslationAccessKind &accessKind, unsigned short &accessSize, unsigned char &pageWalkLevel);
	};

	class CpuSecondStageTranslationMechanism final
	{
	private:
		static constexpr unsigned long long PageFrameMask = 0xFFFF'FFFF'FFFF'F000ULL;
		static constexpr unsigned long long PresentBit = 1ULL << 0;
		static constexpr unsigned long long ReadBit = 1ULL << 1;
		static constexpr unsigned long long WriteBit = 1ULL << 2;
		static constexpr unsigned long long ExecuteBit = 1ULL << 3;
		static constexpr unsigned long long ReservedMisconfigurationBit = 1ULL << 63;

	public:
		static std::shared_ptr<CpuSecondStageTranslationResult> Translate(std::shared_ptr<Core::CpuSecondStageTranslationSnapshot> snapshot, unsigned long long guestPhysicalAddress, CpuInstructionTranslationAccessKind accessKind, unsigned short accessSize, std::shared_ptr<Processor::MainMemoryArea> physicalMemory);

	private:
		static CpuSecondStageFaultKind ValidateEntry(unsigned long long entry, CpuInstructionTranslationAccessKind accessKind, bool requireLeafPermission);

		static unsigned long long ReadCpuPhysicalWord(std::shared_ptr<Processor::MainMemoryArea> physicalMemory, unsigned long long physicalAddress);

		static std::shared_ptr<CpuSecondStageTranslationResult> Fault(CpuSecondStageFaultKind kind, CpuInstructionTranslationAccessKind accessKind, unsigned short accessSize, unsigned char pageWalkLevel);
	};

}
