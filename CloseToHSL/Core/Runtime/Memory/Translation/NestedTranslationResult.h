#pragma once
#include "NestedTlbTag.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Memory
{
	enum class NestedMemoryAccessType : unsigned char
	{
		Read = 0,
		Write = 1,
		Execute = 2
	};

	enum class NestedTranslationStatus : unsigned char
	{
		Success = 0,
		GuestPageFault = 1,
		SecondStageViolation = 2,
		SecondStageMisconfiguration = 3,
		SingleStage = 4
	};

	/// <summary>
	/// Typed second-stage translation result: GVA to GPA to HPA, or the exact
	/// fault information needed for compatibility exit projection.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct NestedTranslationResult(NestedTranslationStatus Status, ulong GuestVirtualAddress, ulong GuestPhysicalAddress, ulong HostPhysicalAddress, NestedMemoryAccessType AccessType, int PageWalkLevel, byte Permissions, byte MemoryType, NestedTlbTag TlbTag, TranslationViolationInfo Violation)
//	{
//		public bool Succeeded
//		{
//			get
//			{
//				return Status is NestedTranslationStatus.Success or NestedTranslationStatus.SingleStage;
//			}
//		}
//
//		public bool IsSecondStageFault
//		{
//			get
//			{
//				return Status is NestedTranslationStatus.SecondStageViolation or NestedTranslationStatus.SecondStageMisconfiguration;
//			}
//		}
//
//		public static NestedTranslationResult Success(ulong guestVirtualAddress, ulong guestPhysicalAddress, ulong hostPhysicalAddress, NestedMemoryAccessType accessType, byte permissions, byte memoryType, NestedTlbTag tag)
//		{
//			return new(NestedTranslationStatus.Success, guestVirtualAddress, guestPhysicalAddress, hostPhysicalAddress, accessType, 0, permissions, memoryType, tag, default);
//		}
//
//		public static NestedTranslationResult SingleStage(ulong virtualAddress, ulong physicalAddress, NestedMemoryAccessType accessType, byte permissions)
//		{
//			return new(NestedTranslationStatus.SingleStage, virtualAddress, physicalAddress, physicalAddress, accessType, 0, permissions, 0, default, default);
//		}
//
//		public static NestedTranslationResult GuestPageFault(ulong guestVirtualAddress, ulong guestPhysicalAddress, NestedMemoryAccessType accessType, int pageWalkLevel)
//		{
//			return new(NestedTranslationStatus.GuestPageFault, guestVirtualAddress, guestPhysicalAddress, 0, accessType, pageWalkLevel, 0, 0, default, default);
//		}
//
//		public static NestedTranslationResult SecondStageViolation(ulong guestVirtualAddress, ulong guestPhysicalAddress, NestedMemoryAccessType accessType, int pageWalkLevel, bool causedByPageWalk)
//		{
//			return FromSecondStageFault(NestedTranslationStatus.SecondStageViolation, guestVirtualAddress, guestPhysicalAddress, accessType, pageWalkLevel, causedByPageWalk, misconfiguration: false);
//		}
//
//		public static NestedTranslationResult SecondStageMisconfiguration(ulong guestVirtualAddress, ulong guestPhysicalAddress, NestedMemoryAccessType accessType, int pageWalkLevel, bool causedByPageWalk)
//		{
//			return FromSecondStageFault(NestedTranslationStatus.SecondStageMisconfiguration, guestVirtualAddress, guestPhysicalAddress, accessType, pageWalkLevel, causedByPageWalk, misconfiguration: true);
//		}
//
//		private static NestedTranslationResult FromSecondStageFault(NestedTranslationStatus status, ulong guestVirtualAddress, ulong guestPhysicalAddress, NestedMemoryAccessType accessType, int pageWalkLevel, bool causedByPageWalk, bool misconfiguration)
//		{
//			TranslationViolationInfo violation = TranslationViolationInfo.Create(guestVirtualAddress, guestPhysicalAddress, accessType, pageWalkLevel, causedByPageWalk, misconfiguration);
//
//			return new(status, guestVirtualAddress, guestPhysicalAddress, 0, accessType, pageWalkLevel, 0, 0, default, violation);
//		}
//	}
	struct TranslationViolationInfo final {
		unsigned long long GuestVirtualAddress = 0, GuestPhysicalAddress = 0;
		NestedMemoryAccessType AccessType = NestedMemoryAccessType::Read;
		int PageWalkLevel = 0;
		unsigned long long QualificationBits = 0;
		bool IsMisconfiguration = false;
		static TranslationViolationInfo Create(unsigned long long gva, unsigned long long gpa, NestedMemoryAccessType access, int level, bool pageWalk, bool misconfiguration) {
			unsigned long long q = 1ULL << 7;
			switch (access) { case NestedMemoryAccessType::Read: q |= 1; break; case NestedMemoryAccessType::Write: q |= 2; break; case NestedMemoryAccessType::Execute: q |= 4; break; }
			if (pageWalk) q |= 1ULL << 8;
			if (misconfiguration) q |= 1ULL << 9;
			q |= (static_cast<unsigned int>(level) & 15ULL) << 12;
			return {gva, gpa, access, level, q, misconfiguration};
		}
	};
	struct NestedTranslationResult final {
		NestedTranslationStatus Status;
		unsigned long long GuestVirtualAddress, GuestPhysicalAddress, HostPhysicalAddress;
		NestedMemoryAccessType AccessType;
		int PageWalkLevel;
		unsigned char Permissions, MemoryType;
		std::shared_ptr<NestedTlbTag> TlbTag;
		TranslationViolationInfo Violation;
		bool Succeeded;
		bool isSecondStageFault() const { return Status == NestedTranslationStatus::SecondStageViolation || Status == NestedTranslationStatus::SecondStageMisconfiguration; }
		static std::shared_ptr<NestedTranslationResult> Success(unsigned long long gva, unsigned long long gpa, unsigned long long hpa, NestedMemoryAccessType access, unsigned char permissions, unsigned char memoryType, std::shared_ptr<NestedTlbTag> tag) { return std::make_shared<NestedTranslationResult>(NestedTranslationResult{NestedTranslationStatus::Success, gva, gpa, hpa, access, 0, permissions, memoryType, std::move(tag), {}, true}); }
		static std::shared_ptr<NestedTranslationResult> SingleStage(unsigned long long va, unsigned long long pa, NestedMemoryAccessType access, unsigned char permissions) { return std::make_shared<NestedTranslationResult>(NestedTranslationResult{NestedTranslationStatus::SingleStage, va, pa, pa, access, 0, permissions, 0, nullptr, {}, true}); }
		static std::shared_ptr<NestedTranslationResult> GuestPageFault(unsigned long long gva, unsigned long long gpa, NestedMemoryAccessType access, int level) { return std::make_shared<NestedTranslationResult>(NestedTranslationResult{NestedTranslationStatus::GuestPageFault, gva, gpa, 0, access, level, 0, 0, nullptr, {}, false}); }
		static std::shared_ptr<NestedTranslationResult> SecondStageViolation(unsigned long long gva, unsigned long long gpa, NestedMemoryAccessType access, int level, bool pageWalk) { return Fault(NestedTranslationStatus::SecondStageViolation, gva, gpa, access, level, pageWalk, false); }
		static std::shared_ptr<NestedTranslationResult> SecondStageMisconfiguration(unsigned long long gva, unsigned long long gpa, NestedMemoryAccessType access, int level, bool pageWalk) { return Fault(NestedTranslationStatus::SecondStageMisconfiguration, gva, gpa, access, level, pageWalk, true); }
	private:
		static std::shared_ptr<NestedTranslationResult> Fault(NestedTranslationStatus status, unsigned long long gva, unsigned long long gpa, NestedMemoryAccessType access, int level, bool pageWalk, bool misconfiguration) { return std::make_shared<NestedTranslationResult>(NestedTranslationResult{status, gva, gpa, 0, access, level, 0, 0, nullptr, TranslationViolationInfo::Create(gva, gpa, access, level, pageWalk, misconfiguration), false}); }
	};
}
