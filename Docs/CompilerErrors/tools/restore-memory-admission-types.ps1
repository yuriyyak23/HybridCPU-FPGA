$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Memory\Timing'
$p=Join-Path $r 'MemoryAdmissionTypes.h'; if(Test-Path -LiteralPath $p){throw 'Admission types header already exists'}
$lines=@(
'#pragma once',
'#include <optional>',
'#include <string>',
'#include <utility>',
'namespace HybridCPU_ISE::CloseToHSL::Memory::Timing {',
'struct MemoryRequestId final {',
'    unsigned long long Value = 0;',
'    bool isValid() const noexcept { return Value != 0; }',
'    bool operator==(const MemoryRequestId&) const = default;',
'};',
'enum class MemoryAdmissionStatus : unsigned char { Accepted, Backpressured, Rejected };',
'struct MemoryAdmissionResult final {',
'    MemoryAdmissionStatus Status = MemoryAdmissionStatus::Accepted;',
'    MemoryRequestId RequestId{};',
'    std::optional<std::wstring> Reason;',
'    static MemoryAdmissionResult Accepted(MemoryRequestId id) { return {MemoryAdmissionStatus::Accepted, id, std::nullopt}; }',
'    static MemoryAdmissionResult Backpressured() { return {MemoryAdmissionStatus::Backpressured, {}, std::nullopt}; }',
'    static MemoryAdmissionResult Rejected(std::wstring reason) { return {MemoryAdmissionStatus::Rejected, {}, std::move(reason)}; }',
'};',
'}',
'') -join "`r`n"
[IO.File]::WriteAllText($p,$lines,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'MemoryCycleController.h'; $s=[IO.File]::ReadAllText($p)
$pattern='enum class MemoryAdmissionStatus\s*:\s*unsigned char\s*\{[^}]*\}\s*;'
if(([regex]::Matches($s,$pattern)).Count -ne 1){throw 'Expected enum not found'}
$s=[regex]::Replace($s,$pattern,''); $s=$s.Replace('#pragma once',"#pragma once`r`n`r`n#include `"MemoryAdmissionTypes.h`"")
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
