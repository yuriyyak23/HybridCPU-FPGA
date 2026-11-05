#include "YAKSys_Hybrid_CPU.Memory.NestedPageWalker.h"

using namespace YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Memory
{

	bool NestedPageWalker::HasAccessPermission(unsigned char permissions, NestedMemoryAccessType accessType)
	{
		bool temp_var170;
		switch (accessType)
		{
			case NestedMemoryAccessType::Read:
				temp_var170 = (permissions & ReadPermission) != 0;
				break;
			case NestedMemoryAccessType::Write:
				temp_var170 = (permissions & WritePermission) != 0;
				break;
			case NestedMemoryAccessType::Execute:
				temp_var170 = (permissions & ExecutePermission) != 0;
				break;
			default:
				temp_var170 = false;
				break;
		}
		return temp_var170;
	}

	bool NestedPageWalker::WalkGuestPageTable(std::shared_ptr<MemoryDomainTranslationControl> control, unsigned long long guestVirtualAddress, NestedMemoryAccessType accessType, std::shared_ptr<Processor::MainMemoryArea> resolvedMemory, unsigned long long &guestPhysicalAddress, unsigned char &permissions, std::shared_ptr<NestedTranslationResult> &fault)
	{
		guestPhysicalAddress = 0;
		permissions = 0;
		fault = nullptr;

		unsigned long long cr3Hpa;
		unsigned char ignoredPermissions = 0, ignoredMemoryType = 0;
		if (!WalkSecondStage(control, control->AddressSpaceRoot, guestVirtualAddress, NestedMemoryAccessType::Read, true, 2, resolvedMemory, cr3Hpa, ignoredPermissions, ignoredMemoryType, fault))
		{
			return false;
		}

		unsigned int guestDirIndex = static_cast<unsigned int>((guestVirtualAddress >> 22) & 0x3FF);
		unsigned long long pdeAddrHpa = cr3Hpa + guestDirIndex * 8;
		unsigned long long pde = resolvedMemory->ReadPhysicalWord(pdeAddrHpa);
		if ((pde & PRESENT_BIT) == 0)
		{
			fault = NestedTranslationResult::GuestPageFault(guestVirtualAddress, control->AddressSpaceRoot, accessType, 2);
			return false;
		}

		unsigned long long ptBaseGpa = pde & PPN_MASK;
		unsigned long long ptBaseHpa;
		if (!WalkSecondStage(control, ptBaseGpa, guestVirtualAddress, NestedMemoryAccessType::Read, true, 1, resolvedMemory, ptBaseHpa, ignoredPermissions, ignoredMemoryType, fault))
		{
			return false;
		}

		unsigned int guestTableIndex = static_cast<unsigned int>((guestVirtualAddress >> 12) & 0x3FF);
		unsigned long long pteAddrHpa = ptBaseHpa + guestTableIndex * 8;
		unsigned long long pte = resolvedMemory->ReadPhysicalWord(pteAddrHpa);
		if ((pte & PRESENT_BIT) == 0)
		{
			fault = NestedTranslationResult::GuestPageFault(guestVirtualAddress, ptBaseGpa, accessType, 1);
			return false;
		}

		guestPhysicalAddress = (pte & PPN_MASK) | (guestVirtualAddress & 0xFFF);
		permissions = DecodeGuestPermissions(pte);
		return true;
	}

	bool NestedPageWalker::WalkSecondStage(std::shared_ptr<MemoryDomainTranslationControl> control, unsigned long long guestPhysicalAddress, unsigned long long guestVirtualAddress, NestedMemoryAccessType accessType, bool causedByPageWalk, int pageWalkLevel, std::shared_ptr<Processor::MainMemoryArea> resolvedMemory, unsigned long long &hostPhysicalAddress, unsigned char &permissions, unsigned char &memoryType, std::shared_ptr<NestedTranslationResult> &fault)
	{
		hostPhysicalAddress = 0;
		permissions = 0;
		memoryType = control->DefaultMemoryType;
		fault = nullptr;

		unsigned int dirIndex = static_cast<unsigned int>((guestPhysicalAddress >> 22) & 0x3FF);
		unsigned long long epdeAddress = control->SecondStageRoot + dirIndex * 8;
		unsigned long long epde = resolvedMemory->ReadPhysicalWord(epdeAddress);
		if (IsMisconfiguredEntry(epde))
		{
			fault = NestedTranslationResult::SecondStageMisconfiguration(guestVirtualAddress, guestPhysicalAddress, accessType, pageWalkLevel, causedByPageWalk);
			return false;
		}

		if ((epde & PRESENT_BIT) == 0)
		{
			fault = NestedTranslationResult::SecondStageViolation(guestVirtualAddress, guestPhysicalAddress, accessType, pageWalkLevel, causedByPageWalk);
			return false;
		}

		unsigned long long secondStageBase = epde & PPN_MASK;
		unsigned int tableIndex = static_cast<unsigned int>((guestPhysicalAddress >> 12) & 0x3FF);
		unsigned long long epteAddress = secondStageBase + tableIndex * 8;
		unsigned long long epte = resolvedMemory->ReadPhysicalWord(epteAddress);
		if (IsMisconfiguredEntry(epte))
		{
			fault = NestedTranslationResult::SecondStageMisconfiguration(guestVirtualAddress, guestPhysicalAddress, accessType, pageWalkLevel, causedByPageWalk);
			return false;
		}

		if ((epte & PRESENT_BIT) == 0)
		{
			fault = NestedTranslationResult::SecondStageViolation(guestVirtualAddress, guestPhysicalAddress, accessType, pageWalkLevel, causedByPageWalk);
			return false;
		}

		permissions = DecodeSecondStagePermissions(epte);
		if (!HasAccessPermission(permissions, accessType))
		{
			fault = NestedTranslationResult::SecondStageViolation(guestVirtualAddress, guestPhysicalAddress, accessType, pageWalkLevel, causedByPageWalk);
			return false;
		}

		hostPhysicalAddress = (epte & PPN_MASK) | (guestPhysicalAddress & 0xFFF);
		return true;
	}

	unsigned char NestedPageWalker::DecodeGuestPermissions(unsigned long long entry)
	{
		unsigned char permissions = 0;
		if ((entry & READ_BIT) != 0)
		{
			permissions |= ReadPermission;
		}
		if ((entry & WRITE_BIT) != 0)
		{
			permissions |= WritePermission;
		}
		if ((entry & PRESENT_BIT) != 0)
		{
			permissions |= ExecutePermission;
		}
		return permissions;
	}

	unsigned char NestedPageWalker::DecodeSecondStagePermissions(unsigned long long entry)
	{
		unsigned char permissions = 0;
		if ((entry & READ_BIT) != 0)
		{
			permissions |= ReadPermission;
		}
		if ((entry & WRITE_BIT) != 0)
		{
			permissions |= WritePermission;
		}
		if ((entry & EXECUTE_BIT) != 0)
		{
			permissions |= ExecutePermission;
		}
		return permissions;
	}

	bool NestedPageWalker::IsMisconfiguredEntry(unsigned long long entry)
	{
		return (entry & RESERVED_MISCONFIG_BIT) != 0;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static bool TranslateNested(ulong addressSpaceRoot, ulong secondStageRoot, ulong guestVirtualAddress, out ulong hostPhysicalAddress, out byte permissions, Processor.MainMemoryArea? mainMemory = null)
	bool NestedPageWalker::TranslateNested(unsigned long long addressSpaceRoot, unsigned long long secondStageRoot, unsigned long long guestVirtualAddress, unsigned long long &hostPhysicalAddress, unsigned char &permissions, std::shared_ptr<Processor::MainMemoryArea> mainMemory)
	{
		std::shared_ptr<MemoryDomainTranslationControl> domainControl = std::make_shared<MemoryDomainTranslationControl>(MemoryDomainTranslationControl{true, false, addressSpaceRoot, secondStageRoot, 0, 0, 0, MemoryDomainTranslationControl::WriteBackMemoryType});

		std::shared_ptr<NestedTranslationResult> result = TranslateNestedDetailed(domainControl, guestVirtualAddress, NestedMemoryAccessType::Read, 0, 0, mainMemory);

		hostPhysicalAddress = result->HostPhysicalAddress;
		permissions = result->Permissions;
		return result->Succeeded;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static NestedTranslationResult TranslateNestedDetailed(MemoryDomainTranslationControl control, ulong guestVirtualAddress, NestedMemoryAccessType accessType, ulong secondStageEpoch, ulong addressSpaceTagEpoch, Processor.MainMemoryArea? mainMemory = null)
	std::shared_ptr<NestedTranslationResult> NestedPageWalker::TranslateNestedDetailed(std::shared_ptr<MemoryDomainTranslationControl> control, unsigned long long guestVirtualAddress, NestedMemoryAccessType accessType, unsigned long long secondStageEpoch, unsigned long long addressSpaceTagEpoch, std::shared_ptr<Processor::MainMemoryArea> mainMemory)
	{
		std::shared_ptr<Processor::MainMemoryArea> resolvedMemory = (mainMemory != nullptr) ? mainMemory : Processor::MainMemory;

		if (!control->TranslationEnabled)
		{
			return NestedTranslationResult::SingleStage(guestVirtualAddress, guestVirtualAddress, accessType, ReadPermission | WritePermission | ExecutePermission);
		}

		if (!control->isValid())
		{
			return NestedTranslationResult::SecondStageMisconfiguration(guestVirtualAddress, control->SecondStageRoot, accessType, 0, false);
		}

		unsigned long long guestPhysicalAddress;
		unsigned char guestPerms;
		std::shared_ptr<NestedTranslationResult> fault;
		if (!WalkGuestPageTable(control, guestVirtualAddress, accessType, resolvedMemory, guestPhysicalAddress, guestPerms, fault))
		{
			return fault;
		}

		unsigned long long hostPhysicalAddress;
		unsigned char secondStagePerms;
		unsigned char memoryType;
		if (!WalkSecondStage(control, guestPhysicalAddress, guestVirtualAddress, accessType, false, 0, resolvedMemory, hostPhysicalAddress, secondStagePerms, memoryType, fault))
		{
			return fault;
		}

		unsigned char permissions = static_cast<unsigned char>(guestPerms & secondStagePerms);
		std::shared_ptr<AddressSpaceId> addressSpace = std::make_shared<AddressSpaceId>(control->ToAddressSpaceId(secondStageEpoch, addressSpaceTagEpoch));
		std::shared_ptr<NestedTlbTag> tag = NestedTlbTag::Create(guestVirtualAddress, addressSpace, permissions, memoryType);

		return NestedTranslationResult::Success(guestVirtualAddress, guestPhysicalAddress, hostPhysicalAddress, accessType, permissions, memoryType, tag);
	}
}
