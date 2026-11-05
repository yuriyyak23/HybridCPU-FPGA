$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Memory\Registers'
$p=Join-Path $r 'YAKSys_Hybrid_CPU.Memory.StreamRegisterFile.h'; $s=[IO.File]::ReadAllText($p)
$m='inline bool CanAllocateAssistRegister('
if(-not $s.Contains($m) -or $s.Contains('enum class RegisterState : unsigned char;')) {throw 'Unexpected header state'}
$s=$s.Replace('#include <vector>',"#include <vector>`r`n#include <span>")
$s=$s.Replace('using namespace YAKSys_Hybrid_CPU::Core::Memory;', '')
$s=$s.Replace($m,"enum class RegisterState : unsigned char;`r`n`r`n`t`t"+$m)
$s=$s.Replace('ReadOnlySpan<unsigned char>','std::span<const unsigned char>').Replace('Span<unsigned char>','std::span<unsigned char>')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'YAKSys_Hybrid_CPU.Memory.StreamRegisterFile.cpp'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('ReadOnlySpan<unsigned char>','std::span<const unsigned char>').Replace('Span<unsigned char>','std::span<unsigned char>')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
