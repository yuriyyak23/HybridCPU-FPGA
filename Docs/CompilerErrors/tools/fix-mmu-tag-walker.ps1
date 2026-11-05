$base='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\'; $p=Join-Path $base 'Core\Runtime\Memory\Translation\NestedTlbTag.h'; $s=[IO.File]::ReadAllText($p).Replace('#pragma once','#pragma once'+"`r`n"+'#include "../AddressSpaces/AddressSpaceId.h"'+"`r`n"+'#include <memory>'+"`r`n"+'#include <stdexcept>'); $idx=$s.LastIndexOf("`r`n}"); if($idx -lt 0){throw 'Namespace end not found'}
$block=@"
	struct NestedTlbTag final {
		std::shared_ptr<AddressSpaceId> AddressSpace;
		unsigned long long VirtualPageNumber, SecondStageRootIdentity;
		unsigned char Permissions, MemoryType;
		unsigned long long TranslationEpoch;
		static std::shared_ptr<NestedTlbTag> Create(unsigned long long address, std::shared_ptr<AddressSpaceId> space, unsigned char permissions, unsigned char memoryType) {
			if (!space) throw std::invalid_argument("addressSpace");
			return std::make_shared<NestedTlbTag>(NestedTlbTag{space, address >> 12, space->SecondStageRootIdentity, permissions, memoryType, space->SecondStageEpoch ^ space->AddressSpaceTagEpoch ^ space->AddressSpaceGeneration});
		}
		bool MatchesLookup(unsigned long long address, const std::shared_ptr<AddressSpaceId>& space) const {
			if (!space || !AddressSpace) return false;
			const auto& a = *AddressSpace; const auto& b = *space;
			return VirtualPageNumber == (address >> 12) && a.DomainTag == b.DomainTag && a.AddressSpaceTag == b.AddressSpaceTag && a.SecondStageRootIdentity == b.SecondStageRootIdentity && a.SecondStageEpoch == b.SecondStageEpoch && a.AddressSpaceTagEpoch == b.AddressSpaceTagEpoch && a.AddressSpaceGeneration == b.AddressSpaceGeneration && SecondStageRootIdentity == b.SecondStageRootIdentity;
		}
	};
"@
$s=$s.Insert($idx,"`r`n"+$block); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $base 'Memory\MMU\TLB.h'; $s=[IO.File]::ReadAllText($p).Replace('#include <memory>','#include <memory>'+"`r`n"+'#include "../../Core/Runtime/Memory/Translation/NestedTlbTag.h"'); [IO.File]::WriteAllText($p,$s)
$root=Join-Path $base 'Memory\MMU\'; $h=Join-Path $root 'PageTableWalker.h'; $c=Join-Path $root 'PageTableWalker.cpp'; $type='std::shared_ptr<std::vector<std::shared_ptr<std::vector<unsigned long long>>>> pageDirectory'
$s=[IO.File]::ReadAllText($h).Replace('#pragma once','#pragma once'+"`r`n"+'#include <vector>').Replace('static inline const WalkResult NoEvent = WalkResult(false, false, -1, 0, 0, 0, 0);','static const WalkResult NoEvent;').Replace('std::shared_ptr<> pageDirectory',$type); [IO.File]::WriteAllText($h,$s)
$s=[IO.File]::ReadAllText($c).Replace('std::shared_ptr<> pageDirectory',$type); $marker='namespace HybridCPU_ISE::CloseToHSL::Memory::MMU'+"`r`n"+'{'; $i=$s.IndexOf($marker); if($i -lt 0){throw 'Namespace missing'}; $i += $marker.Length; $s=$s.Insert($i,"`r`n`t"+'const PageTableWalker::WalkResult PageTableWalker::WalkResult::NoEvent(false, false, -1, 0, 0, 0, 0);'+"`r`n"); $s=$s.Replace('pageDirectory[dirIndex] == nullptr','dirIndex >= pageDirectory->size() || (*pageDirectory)[dirIndex] == nullptr').Replace('pageDirectory[dirIdx] == nullptr','dirIdx >= pageDirectory->size() || (*pageDirectory)[dirIdx] == nullptr || tblIdx >= (*pageDirectory)[dirIdx]->size()').Replace('pageDirectory[dirIdx][tblIdx]','(*(*pageDirectory)[dirIdx])[tblIdx]'); [IO.File]::WriteAllText($c,$s)