$root=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'NonRTL\\Arch\Compat'; $h=Join-Path $root 'VLIW_Bundle.h'; $c=Join-Path $root 'VLIW_Bundle.cpp'
$ht=[IO.File]::ReadAllText($h); $ct=[IO.File]::ReadAllText($c)
if (!$ht.Contains('ReadOnlySpan<unsigned char>')) {Write-Output 'Already converted'; exit 0}
$ht=$ht.Replace('ReadOnlySpan<unsigned char>','std::span<const unsigned char>').Replace('Span<unsigned char>','std::span<unsigned char>')
$ct=$ct.Replace('#include "VLIW_Bundle.h"','#include "VLIW_Bundle.h"'+"`r`n"+'#include "tangible_exception_helper.h"'+"`r`n"+'#include <stdexcept>')
$ct=$ct.Replace('ReadOnlySpan<unsigned char>','std::span<const unsigned char>').Replace('Span<unsigned char>','std::span<unsigned char>')
$ct=$ct.Replace('ArgumentOutOfRangeException(L"index", L"Bundle index must be 0-7")','ArgumentOutOfRangeException(L"Bundle index must be 0-7 (index)")')
$ct=$ct.Replace('destination.Length < 256','destination.size() < 256').Replace('destination.Slice(', 'destination.subspan(')
$old='if (source.Length - offset < 256)'
$new='if (offset < 0) throw std::out_of_range("offset");' + "`r`n`t`t" + 'const auto start = static_cast<std::size_t>(offset);' + "`r`n`t`t" + 'if (start > source.size() || source.size() - start < 256)'
if(!$ct.Contains($old)){throw 'Expected bundle length check missing'}
$ct=$ct.Replace($old,$new)
[IO.File]::WriteAllText($h,$ht,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($c,$ct,[Text.UTF8Encoding]::new($false));Write-Output 'Converted VLIW bundle'
