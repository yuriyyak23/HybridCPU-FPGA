$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\DmaStreamCompute'
$p=Join-Path $r 'DmaStreamComputeStagedWrite.h'
if(Test-Path -LiteralPath $p){throw 'Staged write header already exists'}
$lines=@(
'#pragma once',
'#include <span>',
'#include <vector>',
'namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute {',
'class DmaStreamComputeStagedWrite final {',
'public:',
'    const unsigned long long Address;',
'    const std::vector<unsigned char> Data;',
'    const unsigned long long Length;',
'    DmaStreamComputeStagedWrite(unsigned long long address, std::span<const unsigned char> data)',
'        : Address(address), Data(data.begin(), data.end()), Length(Data.size()) {}',
'};',
'}',
'') -join "`r`n"
[IO.File]::WriteAllText($p,$lines,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'DmaStreamComputeToken.h'; $s=[IO.File]::ReadAllText($p); $m='#include "DmaStreamComputeProgressDiagnostics.h"'
if(-not $s.Contains($m)){throw 'Expected diagnostics include missing'}
$s=$s.Replace($m,$m+"`r`n"+'#include "DmaStreamComputeStagedWrite.h"').Replace('ReadOnlySpan<unsigned char> data','std::span<const unsigned char> data')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'DmaStreamComputeToken.cpp'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('ReadOnlySpan<unsigned char> data','std::span<const unsigned char> data').Replace('stagedWrite->Data->Span','std::span<const unsigned char>(stagedWrite->Data)').Replace('backup->Data->Span','std::span<const unsigned char>(backup->Data)')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
