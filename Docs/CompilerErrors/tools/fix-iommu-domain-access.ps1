$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Virtualization\Compatibility\Adapters\IO\'; $h=Join-Path $root 'YAKSys_Hybrid_CPU.Memory.IOMMU.h'; $c=Join-Path $root 'YAKSys_Hybrid_CPU.Memory.IOMMU.cpp'
$s=[IO.File]::ReadAllText($h).Replace('#include <vector>','#include <vector>'+"`r`n"+'#include <utility>'); $marker='//'+"`t`t"+'public static (bool Allowed, string Reason) CheckDomainAccess'; $i=$s.IndexOf($marker); if($i -lt 0){throw 'Domain method marker missing'}; $s=$s.Insert($i,"`t`t"+'static std::pair<bool, std::wstring> CheckDomainAccess(int threadId, unsigned long long address, unsigned long long size, bool isWrite);'+"`r`n"); [IO.File]::WriteAllText($h,$s)
$s=[IO.File]::ReadAllText($c)
$s=$s.Replace('(bool allowed, string _) = CheckDomainAccess(threadId, ioVirtualAddress, accessSize, isWrite: requestedPermissions.HasFlag(IOMMUAccessPermissions.Write));','auto [allowed, reason] = CheckDomainAccess(threadId, ioVirtualAddress, accessSize, (static_cast<int>(requestedPermissions) & static_cast<int>(IOMMUAccessPermissions::Write)) != 0);').Replace('(bool allowed, string reason) = CheckDomainAccess(threadId, ioVirtualAddress, (ulong)buffer.size(), isWrite: false);','auto [allowed, reason] = CheckDomainAccess(threadId, ioVirtualAddress, buffer.size(), false);').Replace('(bool allowed, string reason) = CheckDomainAccess(threadId, ioVirtualAddress, (ulong)buffer.size(), isWrite: true);','auto [allowed, reason] = CheckDomainAccess(threadId, ioVirtualAddress, buffer.size(), true);').Replace('_ = boundMemory->getReplayRelevantMutationEpoch();','(void)boundMemory->getReplayRelevantMutationEpoch();').Replace('unsigned char cachedPerms;'+"`r`n`t`t`t"+'if (_tlb.TryTranslate(currentVA, threadId, _, cachedPerms))','unsigned char cachedPerms;'+"`r`n`t`t`t"+'unsigned long long cachedPhysicalAddress;'+"`r`n`t`t`t"+'if (_tlb.TryTranslate(currentVA, threadId, cachedPhysicalAddress, cachedPerms))')
$s=$s.Replace('buffer.AsSpan(static_cast<int>(offset), static_cast<int>(count))','std::span<unsigned char>(buffer).subspan(static_cast<size_t>(offset), static_cast<size_t>(count))')
$marker="`tbool IOMMU::TryWarmTranslationForAssistRange(unsigned long long deviceID"; $i=$s.IndexOf($marker); if($i -lt 0){throw 'Insertion point missing'}
$block=@"
	std::pair<bool, std::wstring> IOMMU::CheckDomainAccess(int threadId, unsigned long long address, unsigned long long size, bool isWrite)
	{
		auto it = _threadDomains.find(threadId);
		if (it == _threadDomains.end()) return {false, std::format(L"Thread {} has no memory domain", threadId)};
		auto& domain = it->second;
		if (!domain.Contains(address, size)) return {false, std::format(L"Access 0x{:X}+{} outside domain [0x{:X}+{}]", address, size, domain.BaseAddress, domain.Size)};
		const auto required = isWrite ? MemoryDomainFlags::Write : MemoryDomainFlags::Read;
		if ((static_cast<int>(domain.Flags) & static_cast<int>(required)) == 0) return {false, isWrite ? L"Write access denied to read-only domain" : L"Read access denied to write-only domain"};
		return {true, L""};
	}

"@
$s=$s.Insert($i,$block); [IO.File]::WriteAllText($c,$s)