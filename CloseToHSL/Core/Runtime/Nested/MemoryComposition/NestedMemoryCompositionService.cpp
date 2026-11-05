#include "NestedMemoryCompositionService.h"
#include "../../Memory/Translation/YAKSys_Hybrid_CPU.Memory.NestedPageWalker.h"

using namespace HybridCPU_ISE::CloseToHSL::Memory::MMU;
using namespace YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Memory
{

	int NestedMemoryCompositionService::CountCachedTranslations()
	{
		return _tlb.CountNestedEntries();
	}

	int NestedMemoryCompositionService::InvalidateAll()
	{
		return _tlb.FlushNestedAll();
	}

	int NestedMemoryCompositionService::InvalidateByAddressSpaceTag(unsigned short addressSpaceTag)
	{
		return _tlb.FlushNestedByAddressSpaceTag(addressSpaceTag);
	}

	int NestedMemoryCompositionService::InvalidateBySecondStageRoot(unsigned long long secondStageRootIdentity)
	{
		return _tlb.FlushNestedBySecondStageRoot(secondStageRootIdentity);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public NestedMemoryCompositionResult Translate(NestedMemoryCompositionContext context, ulong l2GuestVirtualAddress, NestedMemoryAccessType accessType, Processor.MainMemoryArea? mainMemory = null)
	std::shared_ptr<NestedMemoryCompositionResult> NestedMemoryCompositionService::Translate(std::shared_ptr<NestedMemoryCompositionContext> context, unsigned long long l2GuestVirtualAddress, NestedMemoryAccessType accessType, std::shared_ptr<Processor::MainMemoryArea> mainMemory)
	{
		std::shared_ptr<Processor::MainMemoryArea> memory = (mainMemory != nullptr) ? mainMemory : Processor::MainMemory;
		std::shared_ptr<AddressSpaceId> addressSpace = context->ToAddressSpaceId();

		if (!context->IsValid)
		{
			std::shared_ptr<NestedTranslationResult> invalidControlFault = NestedTranslationResult::SecondStageMisconfiguration(l2GuestVirtualAddress, context->ChildTranslationControl.SecondStageRoot, accessType, 0, false);
			return NestedMemoryCompositionResult::Fault(NestedMemoryCompositionStage::HostDomainTranslation, invalidControlFault, addressSpace, L"Nested memory composition requires valid child and host translation controls.");
		}

		unsigned long long cachedHpa;
		unsigned long long cachedL2Gpa;
		unsigned char cachedPermissions;
		unsigned char cachedMemoryType;
		std::shared_ptr<NestedTlbTag> cachedTag;
		if (_tlb.TryTranslateNested(l2GuestVirtualAddress, addressSpace, cachedHpa, cachedL2Gpa, cachedPermissions, cachedMemoryType, cachedTag))
		{
			if (!NestedPageWalker::HasAccessPermission(cachedPermissions, accessType))
			{
				std::shared_ptr<NestedTranslationResult> permissionFault = NestedTranslationResult::SecondStageViolation(l2GuestVirtualAddress, cachedL2Gpa, accessType, 0, false);
				return NestedMemoryCompositionResult::Fault(NestedMemoryCompositionStage::ChildDomainTranslation, permissionFault, addressSpace, L"Nested TLB hit did not satisfy the requested access type.");
			}

			std::shared_ptr<NestedTranslationResult> hit = NestedTranslationResult::Success(l2GuestVirtualAddress, cachedL2Gpa, cachedHpa, accessType, cachedPermissions, cachedMemoryType, cachedTag);
			return NestedMemoryCompositionResult::Success(true, hit, 0, addressSpace);
		}

		unsigned long long l2GuestPhysicalAddress;
		unsigned char guestPermissions;
		std::shared_ptr<NestedMemoryCompositionResult> fault;
		if (!TryWalkL2GuestPageTables(context, l2GuestVirtualAddress, accessType, memory, l2GuestPhysicalAddress, guestPermissions, fault))
		{
			return fault;
		}

		unsigned long long l1GuestPhysicalAddress;
		unsigned long long hostPhysicalAddress;
		unsigned char l1Permissions;
		unsigned char l0Permissions;
		if (!TryTranslateL2GpaToHpa(context, l2GuestVirtualAddress, l2GuestPhysicalAddress, accessType, false, 0, memory, l1GuestPhysicalAddress, hostPhysicalAddress, l1Permissions, l0Permissions, fault))
		{
			return fault;
		}

		unsigned char permissions = static_cast<unsigned char>(guestPermissions & l1Permissions & l0Permissions);
		if (!NestedPageWalker::HasAccessPermission(permissions, accessType))
		{
			std::shared_ptr<NestedTranslationResult> permissionFault = NestedTranslationResult::SecondStageViolation(l2GuestVirtualAddress, l2GuestPhysicalAddress, accessType, 0, false);
			return NestedMemoryCompositionResult::Fault(NestedMemoryCompositionStage::ChildDomainTranslation, permissionFault, addressSpace, L"Composed L2 guest, child-domain, and host-domain translation permissions deny the access.");
		}

		std::shared_ptr<NestedTlbTag> tag = NestedTlbTag::Create(l2GuestVirtualAddress, addressSpace, permissions, context->HostTranslationControl.DefaultMemoryType);
		std::shared_ptr<NestedTranslationResult> translation = NestedTranslationResult::Success(l2GuestVirtualAddress, l2GuestPhysicalAddress, hostPhysicalAddress, accessType, permissions, context->HostTranslationControl.DefaultMemoryType, tag);

		_tlb.InsertNested(l2GuestVirtualAddress, l2GuestPhysicalAddress, hostPhysicalAddress, permissions, context->HostTranslationControl.DefaultMemoryType, addressSpace);

		return NestedMemoryCompositionResult::Success(false, translation, l1GuestPhysicalAddress, addressSpace);
	}

	bool NestedMemoryCompositionService::TryWalkL2GuestPageTables(std::shared_ptr<NestedMemoryCompositionContext> context, unsigned long long l2GuestVirtualAddress, NestedMemoryAccessType finalAccessType, std::shared_ptr<Processor::MainMemoryArea> memory, unsigned long long &l2GuestPhysicalAddress, unsigned char &permissions, std::shared_ptr<NestedMemoryCompositionResult> &fault)
	{
		l2GuestPhysicalAddress = 0;
		permissions = 0;

		unsigned long long pdeAddress = context->ChildTranslationControl.AddressSpaceRoot + (((l2GuestVirtualAddress >> 22) & 0x3FFULL) * 8ULL);
		unsigned long long pde;
		if (!TryReadL2GuestPhysicalWord(context, l2GuestVirtualAddress, pdeAddress, 2, memory, pde, fault))
		{
			return false;
		}

		if ((pde & PresentBit) == 0)
		{
			fault = GuestPageFault(context, l2GuestVirtualAddress, context->ChildTranslationControl.AddressSpaceRoot, finalAccessType, 2);
			return false;
		}

		unsigned long long ptBaseL2Gpa = pde & PpnMask;
		unsigned long long pteAddress = ptBaseL2Gpa + (((l2GuestVirtualAddress >> 12) & 0x3FFULL) * 8ULL);
		unsigned long long pte;
		if (!TryReadL2GuestPhysicalWord(context, l2GuestVirtualAddress, pteAddress, 1, memory, pte, fault))
		{
			return false;
		}

		if ((pte & PresentBit) == 0)
		{
			fault = GuestPageFault(context, l2GuestVirtualAddress, ptBaseL2Gpa, finalAccessType, 1);
			return false;
		}

		permissions = DecodeGuestPermissions(pte);
		l2GuestPhysicalAddress = (pte & PpnMask) | (l2GuestVirtualAddress & 0xFFFULL);
		fault = nullptr;
		return true;
	}

	bool NestedMemoryCompositionService::TryReadL2GuestPhysicalWord(std::shared_ptr<NestedMemoryCompositionContext> context, unsigned long long l2GuestVirtualAddress, unsigned long long l2GuestPhysicalAddress, int pageWalkLevel, std::shared_ptr<Processor::MainMemoryArea> memory, unsigned long long &value, std::shared_ptr<NestedMemoryCompositionResult> &fault)
	{
		unsigned long long hostPhysicalAddress;
		unsigned long long unusedL1GuestPhysicalAddress;
		unsigned char unusedL1Permissions;
		unsigned char unusedL0Permissions;
		if (!TryTranslateL2GpaToHpa(context, l2GuestVirtualAddress, l2GuestPhysicalAddress, NestedMemoryAccessType::Read, true, pageWalkLevel, memory, unusedL1GuestPhysicalAddress, hostPhysicalAddress, unusedL1Permissions, unusedL0Permissions, fault))
		{
			value = 0;
			return false;
		}

		value = memory->ReadPhysicalWord(hostPhysicalAddress);
		return true;
	}

	bool NestedMemoryCompositionService::TryTranslateL2GpaToHpa(std::shared_ptr<NestedMemoryCompositionContext> context, unsigned long long l2GuestVirtualAddress, unsigned long long l2GuestPhysicalAddress, NestedMemoryAccessType accessType, bool causedByPageWalk, int pageWalkLevel, std::shared_ptr<Processor::MainMemoryArea> memory, unsigned long long &l1GuestPhysicalAddress, unsigned long long &hostPhysicalAddress, unsigned char &l1Permissions, unsigned char &l0Permissions, std::shared_ptr<NestedMemoryCompositionResult> &fault)
	{
		std::shared_ptr<AddressSpaceId> addressSpace = context->ToAddressSpaceId();
		std::shared_ptr<NestedTranslationResult> l1Fault;
		if (!TryWalkSecondStage(context->ChildTranslationControl, l2GuestVirtualAddress, l2GuestPhysicalAddress, accessType, causedByPageWalk, pageWalkLevel, memory, l1GuestPhysicalAddress, l1Permissions, l1Fault))
		{
			hostPhysicalAddress = 0;
			l0Permissions = 0;
			fault = NestedMemoryCompositionResult::Fault(NestedMemoryCompositionStage::ChildDomainTranslation, l1Fault, addressSpace, L"L1 second-stage translation rejected the L2 GPA.");
			return false;
		}

		std::shared_ptr<NestedTranslationResult> l0Fault;
		if (!TryWalkSecondStage(context->HostTranslationControl, l2GuestVirtualAddress, l1GuestPhysicalAddress, accessType, causedByPageWalk, pageWalkLevel, memory, hostPhysicalAddress, l0Permissions, l0Fault))
		{
			fault = NestedMemoryCompositionResult::Fault(NestedMemoryCompositionStage::HostDomainTranslation, l0Fault, addressSpace, L"L0 second-stage/IOMMU policy rejected the L1 GPA.");
			return false;
		}

		fault = nullptr;
		return true;
	}

	bool NestedMemoryCompositionService::TryWalkSecondStage(const YAKSys_Hybrid_CPU::Core::MemoryDomainTranslationControl &control, unsigned long long l2GuestVirtualAddress, unsigned long long guestPhysicalAddress, NestedMemoryAccessType accessType, bool causedByPageWalk, int pageWalkLevel, std::shared_ptr<Processor::MainMemoryArea> memory, unsigned long long &translatedPhysicalAddress, unsigned char &permissions, std::shared_ptr<NestedTranslationResult> &fault)
	{
		translatedPhysicalAddress = 0;
		permissions = 0;
		fault = nullptr;

		if (!control.TranslationEnabled || !control.isValid())
		{
			fault = NestedTranslationResult::SecondStageMisconfiguration(l2GuestVirtualAddress, guestPhysicalAddress, accessType, pageWalkLevel, causedByPageWalk);
			return false;
		}

		unsigned long long epdeAddress = control.SecondStageRoot + (((guestPhysicalAddress >> 22) & 0x3FFULL) * 8ULL);
		unsigned long long epde = memory->ReadPhysicalWord(epdeAddress);
		if (IsMisconfiguredEntry(epde))
		{
			fault = NestedTranslationResult::SecondStageMisconfiguration(l2GuestVirtualAddress, guestPhysicalAddress, accessType, pageWalkLevel, causedByPageWalk);
			return false;
		}

		if ((epde & PresentBit) == 0)
		{
			fault = NestedTranslationResult::SecondStageViolation(l2GuestVirtualAddress, guestPhysicalAddress, accessType, pageWalkLevel, causedByPageWalk);
			return false;
		}

		unsigned long long tableBase = epde & PpnMask;
		unsigned long long epteAddress = tableBase + (((guestPhysicalAddress >> 12) & 0x3FFULL) * 8ULL);
		unsigned long long epte = memory->ReadPhysicalWord(epteAddress);
		if (IsMisconfiguredEntry(epte))
		{
			fault = NestedTranslationResult::SecondStageMisconfiguration(l2GuestVirtualAddress, guestPhysicalAddress, accessType, pageWalkLevel, causedByPageWalk);
			return false;
		}

		if ((epte & PresentBit) == 0)
		{
			fault = NestedTranslationResult::SecondStageViolation(l2GuestVirtualAddress, guestPhysicalAddress, accessType, pageWalkLevel, causedByPageWalk);
			return false;
		}

		permissions = DecodeSecondStagePermissions(epte);
		if (!NestedPageWalker::HasAccessPermission(permissions, accessType))
		{
			fault = NestedTranslationResult::SecondStageViolation(l2GuestVirtualAddress, guestPhysicalAddress, accessType, pageWalkLevel, causedByPageWalk);
			return false;
		}

		translatedPhysicalAddress = (epte & PpnMask) | (guestPhysicalAddress & 0xFFFULL);
		return true;
	}

	std::shared_ptr<NestedMemoryCompositionResult> NestedMemoryCompositionService::GuestPageFault(std::shared_ptr<NestedMemoryCompositionContext> context, unsigned long long l2GuestVirtualAddress, unsigned long long guestPhysicalAddress, NestedMemoryAccessType accessType, int pageWalkLevel)
	{
		std::shared_ptr<NestedTranslationResult> fault = NestedTranslationResult::GuestPageFault(l2GuestVirtualAddress, guestPhysicalAddress, accessType, pageWalkLevel);
		return NestedMemoryCompositionResult::Fault(NestedMemoryCompositionStage::L2GuestPageTable, fault, context->ToAddressSpaceId(), L"L2 guest page tables rejected the L2 GVA.");
	}

	unsigned char NestedMemoryCompositionService::DecodeGuestPermissions(unsigned long long entry)
	{
		unsigned char permissions = 0;
		if ((entry & ReadBit) != 0)
		{
			permissions |= NestedPageWalker::ReadPermission;
		}

		if ((entry & WriteBit) != 0)
		{
			permissions |= NestedPageWalker::WritePermission;
		}

		if ((entry & PresentBit) != 0)
		{
			permissions |= NestedPageWalker::ExecutePermission;
		}

		return permissions;
	}

	unsigned char NestedMemoryCompositionService::DecodeSecondStagePermissions(unsigned long long entry)
	{
		unsigned char permissions = 0;
		if ((entry & ReadBit) != 0)
		{
			permissions |= NestedPageWalker::ReadPermission;
		}

		if ((entry & WriteBit) != 0)
		{
			permissions |= NestedPageWalker::WritePermission;
		}

		if ((entry & ExecuteBit) != 0)
		{
			permissions |= NestedPageWalker::ExecutePermission;
		}

		return permissions;
	}

	bool NestedMemoryCompositionService::IsMisconfiguredEntry(unsigned long long entry)
	{
		return (entry & ReservedMisconfigBit) != 0;
	}

	unsigned long long MemoryDomainTranslationControlNestedExtensions::AddressSpaceTagEpochOrZero(std::shared_ptr<MemoryDomainTranslationControl> control)
	{
		return control->AddressSpaceTaggingEnabled ? control->AddressSpaceGeneration : 0;
	}
}
