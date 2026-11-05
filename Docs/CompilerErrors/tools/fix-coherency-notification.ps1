$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Memory\'; $h=Join-Path $root 'MemoryCoherencyObserver.h'; $c=Join-Path $root 'MemoryCoherencyObserver.cpp'; $s=[IO.File]::ReadAllText($h).Replace('#include "DataCacheRangeResults.h"','#include "DataCacheRangeResults.h"'+"`r`n"+'#include "../Architecture/MemoryModel/Addressing/MemoryRangeOverlap.h"')
$idx=$s.IndexOf("`tclass MemoryCoherencyCacheParticipant"); if($idx -lt 0){throw 'Insertion point missing'}
$block=@"
	struct MemoryCoherencyWriteNotification {
		unsigned long long Address, Length, DomainTag;
		MemoryCoherencyWriteSourceKind SourceKind;
		MemoryCoherencyWriteNotification(unsigned long long address, unsigned long long length, unsigned long long domainTag, MemoryCoherencyWriteSourceKind sourceKind) : Address(address), Length(length), DomainTag(domainTag), SourceKind(sourceKind) {}
		bool isWellFormed() const { return MemoryRangeOverlap::IsNonEmptyWellFormed(Address, Length); }
	};

"@
$s=$s.Insert($idx,$block); [IO.File]::WriteAllText($h,$s); $s=[IO.File]::ReadAllText($c).Replace('notification->IsWellFormed','notification->isWellFormed()'); [IO.File]::WriteAllText($c,$s)