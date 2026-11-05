$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Completion\Commit'
$p=Join-Path $r 'SystemSealKey.h'
if(Test-Path -LiteralPath $p){throw 'SystemSealKey header already exists'}
$lines=@(
'#pragma once',
'#ifndef NOMINMAX',
'#define NOMINMAX',
'#endif',
'#include <Windows.h>',
'#include <bcrypt.h>',
'#include <stdexcept>',
'#include <vector>',
'#pragma comment(lib, "bcrypt.lib")',
'namespace YAKSys_Hybrid_CPU::Core {',
'inline std::vector<unsigned char> GenerateSystemSealKey() {',
'    std::vector<unsigned char> key(32);',
'    if (BCryptGenRandom(nullptr, key.data(), static_cast<ULONG>(key.size()), BCRYPT_USE_SYSTEM_PREFERRED_RNG) < 0)',
'        throw std::runtime_error("Failed to generate completion receipt seal key.");',
'    return key;',
'}',
'}',
'') -join "`r`n"
[IO.File]::WriteAllText($p,$lines,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'ArchitecturalCompletionCommitOwner.h'; $s=[IO.File]::ReadAllText($p); $m='RandomNumberGenerator::GetBytes(32)'
if(-not $s.Contains($m)){throw 'Expected RNG expression missing'}
$s=$s.Replace('#include "NeutralArchitecturalCompletionFacts.h"',"#include `"NeutralArchitecturalCompletionFacts.h`"`r`n#include `"SystemSealKey.h`"`r`n#include <span>").Replace($m,'GenerateSystemSealKey()')
$s=$s.Replace('ReadOnlySpan<std::shared_ptr<ArchitecturalCompletionCandidate>> candidates','std::span<const std::shared_ptr<ArchitecturalCompletionCandidate>> candidates')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'ArchitecturalCompletionCommitOwner.cpp'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('ReadOnlySpan<std::shared_ptr<ArchitecturalCompletionCandidate>> candidates','std::span<const std::shared_ptr<ArchitecturalCompletionCandidate>> candidates')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
