$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Memory\Translation'
$p=Join-Path $r 'CpuInstructionTranslationRegions.h'
if(Test-Path -LiteralPath $p){throw 'Region header already exists'}
$lines=@(
'#pragma once',
'#include <limits>',
'#include "../../../Architecture/Registers/Architectural/RegisterIdentity.h"',
'namespace YAKSys_Hybrid_CPU::Core::Memory {',
'struct CpuInstructionTranslationScope final {',
'    unsigned long long DomainId = 0;',
'    int ContextId = 0, VirtualThreadId = 0;',
'    bool isValid() const noexcept { return DomainId != 0 && ContextId > 0 && Registers::VtId::IsRepresentable(VirtualThreadId); }',
'};',
'struct CpuInstructionTranslationRegion final {',
'    unsigned long long VirtualBase = 0, PhysicalBase = 0, Length = 0;',
'    bool Readable = false, Writable = false, Executable = false;',
'    bool isValid() const noexcept {',
'        const auto max = std::numeric_limits<unsigned long long>::max();',
'        return Length != 0 && VirtualBase <= max - (Length - 1) && PhysicalBase <= max - (Length - 1) && (Readable || Writable || Executable);',
'    }',
'};',
'struct CpuInstructionStageOneRegion final {',
'    unsigned long long VirtualBase = 0, GuestPhysicalBase = 0, Length = 0;',
'    bool Readable = false, Writable = false, Executable = false;',
'    bool isValid() const noexcept {',
'        const auto max = std::numeric_limits<unsigned long long>::max();',
'        return Length != 0 && VirtualBase <= max - (Length - 1) && GuestPhysicalBase <= max - (Length - 1) && (Readable || Writable || Executable);',
'    }',
'};',
'}',
'') -join "`r`n"
[IO.File]::WriteAllText($p,$lines,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'CpuInstructionTranslationContour.h'; $s=[IO.File]::ReadAllText($p); $m='#include <vector>'
if(-not $s.Contains($m)){throw 'Expected vector include missing'}
$s=$s.Replace($m,"#include `"CpuInstructionTranslationRegions.h`"`r`n"+$m); [IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
