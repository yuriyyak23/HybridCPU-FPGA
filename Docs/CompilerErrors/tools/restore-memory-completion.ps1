$ErrorActionPreference='Stop'
$dir=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'CloseToHSL\\Memory\Timing'
$h=Join-Path $dir 'MemoryCycleController.h'; $s=Get-Content $h -Raw
if (!$s.Contains('#include <span>')) { $s=$s.Replace('#include <vector>', "#include <vector>`r`n#include <span>") }
$s=$s.Replace('ReadOnlyMemory<unsigned char> getData() const;', 'std::span<const unsigned char> getData() const;').Replace('ReadOnlySpan<unsigned char> data', 'std::span<const unsigned char> data')
Set-Content $h $s -Encoding UTF8 -NoNewline
$p=Join-Path $dir 'MemoryCycleController.cpp'; $s=Get-Content $p -Raw
$a=$s.IndexOf("`tMemoryCompletion::MemoryCompletion("); $b=$s.IndexOf("`tstd::shared_ptr<MemoryRequestId> MemoryCompletion::getRequestId()", $a)
if($a -lt 0 -or $b -lt 0) {throw 'MemoryCompletion constructor missing'}
$constructor=@"
    MemoryCompletion::MemoryCompletion(std::shared_ptr<MemoryRequestId> requestId, bool succeeded, std::vector<unsigned char>& data, const std::wstring& failureReason, unsigned long long publishedCycle)
        : RequestId(std::move(requestId)), Succeeded(succeeded), FailureReason(failureReason), PublishedCycle(publishedCycle), _data(data) {}

"@
$s=$s.Substring(0,$a)+$constructor+$s.Substring($b)
$s=$s.Replace('ReadOnlyMemory<unsigned char> MemoryCompletion::getData() const', 'std::span<const unsigned char> MemoryCompletion::getData() const').Replace('ReadOnlySpan<unsigned char> data','std::span<const unsigned char> data')
Set-Content $p $s -Encoding UTF8 -NoNewline
