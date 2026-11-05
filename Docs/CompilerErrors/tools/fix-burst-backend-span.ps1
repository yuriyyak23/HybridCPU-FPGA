$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\BurstIO'
$names=@('IBurstBackend.h','IOMMUBurstBackend.h','IOMMUBurstBackend.cpp','PhysicalMainMemoryBurstBackend.h','PhysicalMainMemoryBurstBackend.cpp')
foreach($name in $names){
 $p=Join-Path $r $name; $s=[IO.File]::ReadAllText($p)
 if(-not $s.Contains('Span<unsigned char>')){throw "Expected span missing in $name"}
 $s=$s.Replace('ReadOnlySpan<unsigned char>','std::span<const unsigned char>').Replace('Span<unsigned char>','std::span<unsigned char>')
 if($name -eq 'IBurstBackend.h'){$s=$s.Replace('#include <memory>',"#include <memory>`r`n#include <span>")}
 [IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
}
