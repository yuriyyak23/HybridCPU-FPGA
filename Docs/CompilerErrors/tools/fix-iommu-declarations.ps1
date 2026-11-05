$base='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\'; $p=Join-Path $base 'Runtime\Memory\Invalidation\TranslationInvalidationService.h'; $s=[IO.File]::ReadAllText($p); $i=$s.IndexOf("`tclass ITranslationInvalidationBackend"); if($i -lt 0){throw 'Backend marker missing'}
$block=@"
	struct TranslationInvalidationBackendRequest final {
		TranslationInvalidationScope Scope;
		unsigned long long Descriptor, GuestPhysicalAddress, SizeBytes;
		bool IsSecondStageRoot;
		TranslationInvalidationBackendRequest(TranslationInvalidationScope scope, unsigned long long descriptor, unsigned long long guestPhysicalAddress, unsigned long long sizeBytes, bool isSecondStageRoot) : Scope(scope), Descriptor(descriptor), GuestPhysicalAddress(guestPhysicalAddress), SizeBytes(sizeBytes), IsSecondStageRoot(isSecondStageRoot) {}
	};

"@
$s=$s.Insert($i,$block); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $base 'Virtualization\Compatibility\Adapters\IO\YAKSys_Hybrid_CPU.Memory.IOMMU.h'; $s=[IO.File]::ReadAllText($p).Replace('#include "../../../../Runtime/Memory/Translation/NestedTranslationResult.h"','#include "../../../../Runtime/Memory/Translation/NestedTranslationResult.h"'+"`r`n"+'#include "../../../../Runtime/IO/Dma/DmaDomainBinding.h"'); $s=$s.Replace('namespace YAKSys_Hybrid_CPU::Memory'+"`r`n"+'{','namespace YAKSys_Hybrid_CPU::Core { struct VmxDirtyLogStatus; }'+"`r`n"+'namespace YAKSys_Hybrid_CPU::Memory'+"`r`n"+'{'+"`r`n`t"+'struct DmaTranslationResult; struct NestedTranslationResult;'); $s=$s.Replace('static unsigned long long[][]? PageDirectory;','static std::shared_ptr<std::vector<std::shared_ptr<std::vector<unsigned long long>>>> PageDirectory;'); [IO.File]::WriteAllText($p,$s)