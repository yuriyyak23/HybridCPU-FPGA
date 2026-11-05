$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Memory\Translation\NestedTranslationResult.h'; $s=[IO.File]::ReadAllText($p).Replace('#pragma once','#pragma once'+"`r`n"+'#include "NestedTlbTag.h"'+"`r`n"+'#include <memory>'); $idx=$s.LastIndexOf("`r`n}"); if($idx -lt 0){throw 'Namespace end not found'}
$block=@"
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
"@
$s=$s.Insert($idx,"`r`n"+$block); [IO.File]::WriteAllText($p,$s)
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Virtualization\Compatibility\Adapters\IO\YAKSys_Hybrid_CPU.Memory.IOMMU.cpp'; $s=[IO.File]::ReadAllText($p).Replace('pageWalkLevel: ', '').Replace('causedByPageWalk: ', '').Replace('domainControl->IsValid','domainControl->isValid()'); [IO.File]::WriteAllText($p,$s)