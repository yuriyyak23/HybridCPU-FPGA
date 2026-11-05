#pragma once

#include "../../../../Memory/MMU/TLB.h"
#include "../../Memory/Translation/NestedTranslationResult.h"
#include "../../Memory/Translation/MemoryDomainTranslationControl.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <memory>
#include <string>

using namespace HybridCPU_ISE::CloseToHSL::Memory::MMU;
using namespace YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Memory
{
	enum class NestedMemoryCompositionStage : unsigned char
	{
		None = 0,
		L2GuestPageTable = 1,
		ChildDomainTranslation = 2,
		HostDomainTranslation = 3
	};
	struct NestedMemoryCompositionContext final
	{
		const YAKSys_Hybrid_CPU::Core::MemoryDomainTranslationControl ChildTranslationControl;
		const YAKSys_Hybrid_CPU::Core::MemoryDomainTranslationControl HostTranslationControl;
		const unsigned short ChildAddressSpaceTag;
		const unsigned long long ChildTranslationControlEpoch;
		const unsigned long long HostTranslationControlEpoch;
		const unsigned long long L2Generation;
		const bool IsValid;

		NestedMemoryCompositionContext(YAKSys_Hybrid_CPU::Core::MemoryDomainTranslationControl childTranslationControl,
			YAKSys_Hybrid_CPU::Core::MemoryDomainTranslationControl hostTranslationControl, unsigned short childAddressSpaceTag,
			unsigned long long childTranslationControlEpoch, unsigned long long hostTranslationControlEpoch, unsigned long long l2Generation)
			: ChildTranslationControl(childTranslationControl), HostTranslationControl(hostTranslationControl),
			  ChildAddressSpaceTag(childAddressSpaceTag), ChildTranslationControlEpoch(childTranslationControlEpoch),
			  HostTranslationControlEpoch(hostTranslationControlEpoch), L2Generation(l2Generation),
			  IsValid(childTranslationControl.TranslationEnabled && hostTranslationControl.TranslationEnabled &&
				childTranslationControl.isValid() && hostTranslationControl.isValid()) {}

		std::shared_ptr<AddressSpaceId> ToAddressSpaceId() const
		{
			const auto childRoot = ChildTranslationControl.SecondStageRoot;
			auto addressSpace = std::make_shared<AddressSpaceId>();
			addressSpace->DomainTag = HostTranslationControl.DomainTag;
			addressSpace->AddressSpaceTag = ChildAddressSpaceTag;
			addressSpace->SecondStageRootIdentity = ((childRoot << 17) | (childRoot >> 47)) ^ HostTranslationControl.SecondStageRoot ^ 0x9E37'79B9'7F4A'7C15ULL;
			addressSpace->SecondStageEpoch = ChildTranslationControlEpoch ^ HostTranslationControlEpoch;
			addressSpace->AddressSpaceTagEpoch = (HostTranslationControl.AddressSpaceTaggingEnabled ? HostTranslationControl.AddressSpaceGeneration : 0) ^ L2Generation;
			addressSpace->AddressSpaceGeneration = L2Generation;
			return addressSpace;
		}
	};

	struct NestedMemoryCompositionResult final
	{
		const bool Succeeded;
		const bool TlbHit;
		const NestedMemoryCompositionStage FaultStage;
		const std::shared_ptr<NestedTranslationResult> Translation;
		const unsigned long long L2GuestPhysicalAddress;
		const unsigned long long L1GuestPhysicalAddress;
		const unsigned long long HostPhysicalAddress;
		const std::shared_ptr<AddressSpaceId> AddressSpace;
		const std::wstring Message;
		const bool CausesL1Exit;
		const bool CausesL0Exit;

		NestedMemoryCompositionResult(bool succeeded, bool tlbHit, NestedMemoryCompositionStage faultStage,
			std::shared_ptr<NestedTranslationResult> translation, unsigned long long l2GuestPhysicalAddress,
			unsigned long long l1GuestPhysicalAddress, unsigned long long hostPhysicalAddress,
			std::shared_ptr<AddressSpaceId> addressSpace, std::wstring message)
			: Succeeded(succeeded), TlbHit(tlbHit), FaultStage(faultStage), Translation(std::move(translation)),
			  L2GuestPhysicalAddress(l2GuestPhysicalAddress), L1GuestPhysicalAddress(l1GuestPhysicalAddress),
			  HostPhysicalAddress(hostPhysicalAddress), AddressSpace(std::move(addressSpace)), Message(std::move(message)),
			  CausesL1Exit(!succeeded && faultStage == NestedMemoryCompositionStage::ChildDomainTranslation),
			  CausesL0Exit(!succeeded && faultStage == NestedMemoryCompositionStage::HostDomainTranslation) {}

		static std::shared_ptr<NestedMemoryCompositionResult> Success(bool tlbHit,
			std::shared_ptr<NestedTranslationResult> translation, unsigned long long l1GuestPhysicalAddress,
			std::shared_ptr<AddressSpaceId> addressSpace)
		{
			return std::make_shared<NestedMemoryCompositionResult>(true, tlbHit, NestedMemoryCompositionStage::None,
				translation, translation->GuestPhysicalAddress, l1GuestPhysicalAddress,
				translation->HostPhysicalAddress, std::move(addressSpace), L"");
		}

		static std::shared_ptr<NestedMemoryCompositionResult> Fault(NestedMemoryCompositionStage stage,
			std::shared_ptr<NestedTranslationResult> translation, std::shared_ptr<AddressSpaceId> addressSpace,
			std::wstring message)
		{
			return std::make_shared<NestedMemoryCompositionResult>(false, false, stage, translation,
				translation->GuestPhysicalAddress, 0, 0, std::move(addressSpace), std::move(message));
		}
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct NestedMemoryCompositionContext(MemoryDomainTranslationControl ChildTranslationControl, MemoryDomainTranslationControl HostTranslationControl, ushort ChildAddressSpaceTag, ulong ChildTranslationControlEpoch, ulong HostTranslationControlEpoch, ulong L2Generation)
//	{
//		public bool IsValid
//		{
//			get
//			{
//				return ChildTranslationControl.TranslationEnabled && HostTranslationControl.TranslationEnabled && ChildTranslationControl.IsValid && HostTranslationControl.IsValid;
//			}
//		}
//
//		public AddressSpaceId ToAddressSpaceId()
//		{
//			return new(HostTranslationControl.DomainTag, ChildAddressSpaceTag, MixSecondStageRoots(ChildTranslationControl.SecondStageRoot, HostTranslationControl.SecondStageRoot), ChildTranslationControlEpoch ^ HostTranslationControlEpoch, HostTranslationControl.AddressSpaceTagEpochOrZero() ^ L2Generation, L2Generation);
//		}
//
//		private static ulong MixSecondStageRoots(ulong childRoot, ulong hostRoot)
//		{
//			return ((childRoot << 17) | (childRoot >> 47)) ^ hostRoot ^ 0x9E37_79B9_7F4A_7C15UL;
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct NestedMemoryCompositionResult(bool Succeeded, bool TlbHit, NestedMemoryCompositionStage FaultStage, NestedTranslationResult Translation, ulong L2GuestPhysicalAddress, ulong L1GuestPhysicalAddress, ulong HostPhysicalAddress, AddressSpaceId AddressSpace, string Message)
//	{
//		public bool CausesL1Exit
//		{
//			get
//			{
//				return !Succeeded && FaultStage == NestedMemoryCompositionStage.ChildDomainTranslation;
//			}
//		}
//
//		public bool CausesL0Exit
//		{
//			get
//			{
//				return !Succeeded && FaultStage == NestedMemoryCompositionStage.HostDomainTranslation;
//			}
//		}
//
//		public static NestedMemoryCompositionResult Success(bool tlbHit, NestedTranslationResult translation, ulong l1GuestPhysicalAddress, AddressSpaceId addressSpace)
//		{
//			return new(true, tlbHit, NestedMemoryCompositionStage.None, translation, translation.GuestPhysicalAddress, l1GuestPhysicalAddress, translation.HostPhysicalAddress, addressSpace, string.Empty);
//		}
//
//		public static NestedMemoryCompositionResult Fault(NestedMemoryCompositionStage stage, NestedTranslationResult translation, AddressSpaceId addressSpace, string message)
//		{
//			return new(false, false, stage, translation, translation.GuestPhysicalAddress, 0, 0, addressSpace, message);
//		}
//	}

	class NestedMemoryCompositionService final : public std::enable_shared_from_this<NestedMemoryCompositionService>
	{
	private:
		static constexpr unsigned long long PpnMask = 0xFFFF'FFFF'FFFF'F000ULL;
		static constexpr unsigned long long PresentBit = 0x1ULL;
		static constexpr unsigned long long ReadBit = 0x2ULL;
		static constexpr unsigned long long WriteBit = 0x4ULL;
		static constexpr unsigned long long ExecuteBit = 0x8ULL;
		static constexpr unsigned long long ReservedMisconfigBit = 1ULL << 63;

		TLB _tlb;

	public:
		int CountCachedTranslations();

		int InvalidateAll();

		int InvalidateByAddressSpaceTag(unsigned short addressSpaceTag);

		int InvalidateBySecondStageRoot(unsigned long long secondStageRootIdentity);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public NestedMemoryCompositionResult Translate(NestedMemoryCompositionContext context, ulong l2GuestVirtualAddress, NestedMemoryAccessType accessType, Processor.MainMemoryArea? mainMemory = null)
		std::shared_ptr<NestedMemoryCompositionResult> Translate(std::shared_ptr<NestedMemoryCompositionContext> context, unsigned long long l2GuestVirtualAddress, NestedMemoryAccessType accessType, std::shared_ptr<Processor::MainMemoryArea> mainMemory = nullptr);

	private:
		bool TryWalkL2GuestPageTables(std::shared_ptr<NestedMemoryCompositionContext> context, unsigned long long l2GuestVirtualAddress, NestedMemoryAccessType finalAccessType, std::shared_ptr<Processor::MainMemoryArea> memory, unsigned long long &l2GuestPhysicalAddress, unsigned char &permissions, std::shared_ptr<NestedMemoryCompositionResult> &fault);

		bool TryReadL2GuestPhysicalWord(std::shared_ptr<NestedMemoryCompositionContext> context, unsigned long long l2GuestVirtualAddress, unsigned long long l2GuestPhysicalAddress, int pageWalkLevel, std::shared_ptr<Processor::MainMemoryArea> memory, unsigned long long &value, std::shared_ptr<NestedMemoryCompositionResult> &fault);

		bool TryTranslateL2GpaToHpa(std::shared_ptr<NestedMemoryCompositionContext> context, unsigned long long l2GuestVirtualAddress, unsigned long long l2GuestPhysicalAddress, NestedMemoryAccessType accessType, bool causedByPageWalk, int pageWalkLevel, std::shared_ptr<Processor::MainMemoryArea> memory, unsigned long long &l1GuestPhysicalAddress, unsigned long long &hostPhysicalAddress, unsigned char &l1Permissions, unsigned char &l0Permissions, std::shared_ptr<NestedMemoryCompositionResult> &fault);

		static bool TryWalkSecondStage(const YAKSys_Hybrid_CPU::Core::MemoryDomainTranslationControl &control, unsigned long long l2GuestVirtualAddress, unsigned long long guestPhysicalAddress, NestedMemoryAccessType accessType, bool causedByPageWalk, int pageWalkLevel, std::shared_ptr<Processor::MainMemoryArea> memory, unsigned long long &translatedPhysicalAddress, unsigned char &permissions, std::shared_ptr<NestedTranslationResult> &fault);

		std::shared_ptr<NestedMemoryCompositionResult> GuestPageFault(std::shared_ptr<NestedMemoryCompositionContext> context, unsigned long long l2GuestVirtualAddress, unsigned long long guestPhysicalAddress, NestedMemoryAccessType accessType, int pageWalkLevel);

		static unsigned char DecodeGuestPermissions(unsigned long long entry);

		static unsigned char DecodeSecondStagePermissions(unsigned long long entry);

		static bool IsMisconfiguredEntry(unsigned long long entry);
	};

	class MemoryDomainTranslationControlNestedExtensions final
	{
	public:
		static unsigned long long AddressSpaceTagEpochOrZero(std::shared_ptr<MemoryDomainTranslationControl> control);
	};

}
