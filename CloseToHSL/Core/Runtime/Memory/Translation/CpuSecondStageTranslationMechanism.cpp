#include "CpuSecondStageTranslationMechanism.h"
#include <array>
#include <span>

namespace YAKSys_Hybrid_CPU::Core::Memory
{

	unsigned long long CpuSecondStageNeutralAuxiliary::Encode(CpuInstructionTranslationAccessKind accessKind, unsigned short accessSize, CpuSecondStageFaultKind kind, unsigned char pageWalkLevel)
	{
		return (static_cast<unsigned long long>(static_cast<unsigned char>(kind)) << 56) | (static_cast<unsigned long long>(static_cast<unsigned char>(accessKind)) << 48) | (static_cast<unsigned long long>(accessSize) << 32) | (static_cast<unsigned long long>(pageWalkLevel) << 24);
	}

	bool CpuSecondStageNeutralAuxiliary::TryDecode(unsigned long long value, CpuSecondStageFaultKind &kind, CpuInstructionTranslationAccessKind &accessKind, unsigned short &accessSize, unsigned char &pageWalkLevel)
	{
		kind = static_cast<CpuSecondStageFaultKind>((value >> 56) & 0xFF);
		accessKind = static_cast<CpuInstructionTranslationAccessKind>((value >> 48) & 0xFF);
		accessSize = static_cast<unsigned short>((value >> 32) & 0xFFFF);
		pageWalkLevel = static_cast<unsigned char>((value >> 24) & 0xFF);
		return (value & 0x00FF'FFFFULL) == 0 && kind >= CpuSecondStageFaultKind::NotPresent && kind <= CpuSecondStageFaultKind::SourceStale && (accessKind == CpuInstructionTranslationAccessKind::InstructionFetch || accessKind == CpuInstructionTranslationAccessKind::ScalarLoad || accessKind == CpuInstructionTranslationAccessKind::ScalarStore) && accessSize != 0 && (pageWalkLevel == 1 || pageWalkLevel == 2);
	}

	std::shared_ptr<CpuSecondStageTranslationResult> CpuSecondStageTranslationMechanism::Translate(std::shared_ptr<Core::CpuSecondStageTranslationSnapshot> snapshot, unsigned long long guestPhysicalAddress, CpuInstructionTranslationAccessKind accessKind, unsigned short accessSize, std::shared_ptr<Processor::MainMemoryArea> physicalMemory)
	{
		if (!snapshot || !snapshot->isValid() || accessSize == 0 || !snapshot->AddressSpace->AllowsRange(guestPhysicalAddress, accessSize))
		{
			return Fault(CpuSecondStageFaultKind::OutsideOwnedAddressSpace, accessKind, accessSize, 2);
		}

		unsigned long long end = guestPhysicalAddress + accessSize - 1;
		if (end < guestPhysicalAddress || (guestPhysicalAddress >> 12) != (end >> 12))
		{
			return Fault(CpuSecondStageFaultKind::OutsideOwnedAddressSpace, accessKind, accessSize, 1);
		}

		unsigned int directoryIndex = static_cast<unsigned int>((guestPhysicalAddress >> 22) & 0x3FF);
		unsigned long long directoryEntry = ReadCpuPhysicalWord(physicalMemory, snapshot->SecondStageRoot + directoryIndex * 8ULL);
		CpuSecondStageFaultKind directoryFault = ValidateEntry(directoryEntry, accessKind, false);
		if (directoryFault != CpuSecondStageFaultKind::None)
		{
			return Fault(directoryFault, accessKind, accessSize, 2);
		}

		unsigned long long tableBase = directoryEntry & PageFrameMask;
		unsigned int tableIndex = static_cast<unsigned int>((guestPhysicalAddress >> 12) & 0x3FF);
		unsigned long long tableEntry = ReadCpuPhysicalWord(physicalMemory, tableBase + tableIndex * 8ULL);
		CpuSecondStageFaultKind tableFault = ValidateEntry(tableEntry, accessKind, true);
		if (tableFault != CpuSecondStageFaultKind::None)
		{
			return Fault(tableFault, accessKind, accessSize, 1);
		}

		return CpuSecondStageTranslationResult::Success((tableEntry & PageFrameMask) | (guestPhysicalAddress & 0xFFFULL));
	}

	CpuSecondStageFaultKind CpuSecondStageTranslationMechanism::ValidateEntry(unsigned long long entry, CpuInstructionTranslationAccessKind accessKind, bool requireLeafPermission)
	{
		if ((entry & ReservedMisconfigurationBit) != 0)
		{
			return CpuSecondStageFaultKind::Misconfiguration;
		}
		if ((entry & PresentBit) == 0)
		{
			return CpuSecondStageFaultKind::NotPresent;
		}
		if (!requireLeafPermission)
		{
			return CpuSecondStageFaultKind::None;
		}

		bool temp_var169;
		switch (accessKind)
		{
			case CpuInstructionTranslationAccessKind::InstructionFetch:
				temp_var169 = (entry & ExecuteBit) != 0;
				break;
			case CpuInstructionTranslationAccessKind::ScalarLoad:
				temp_var169 = (entry & ReadBit) != 0;
				break;
			case CpuInstructionTranslationAccessKind::ScalarStore:
				temp_var169 = (entry & WriteBit) != 0;
				break;
			default:
				temp_var169 = false;
				break;
		}
		bool allowed = temp_var169;
		return allowed ? CpuSecondStageFaultKind::None : CpuSecondStageFaultKind::PermissionDenied;
	}

	unsigned long long CpuSecondStageTranslationMechanism::ReadCpuPhysicalWord(std::shared_ptr<Processor::MainMemoryArea> physicalMemory, unsigned long long physicalAddress)
	{
		std::array<unsigned char, sizeof(unsigned long long)> bytes{};
		if (!physicalMemory || !physicalMemory->TryReadPhysicalRange(physicalAddress, std::span<unsigned char>(bytes)))
		{
			return 0;
		}

		unsigned long long value = 0;
		for (std::size_t index = 0; index < bytes.size(); ++index)
		{
			value |= static_cast<unsigned long long>(bytes[index]) << (index * 8);
		}
		return value;
	}

	std::shared_ptr<CpuSecondStageTranslationResult> CpuSecondStageTranslationMechanism::Fault(CpuSecondStageFaultKind kind, CpuInstructionTranslationAccessKind accessKind, unsigned short accessSize, unsigned char pageWalkLevel)
	{
		return CpuSecondStageTranslationResult::Failed(kind, CpuSecondStageNeutralAuxiliary::Encode(accessKind, accessSize, kind, pageWalkLevel));
	}
}
