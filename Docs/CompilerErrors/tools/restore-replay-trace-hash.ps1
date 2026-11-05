$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$p=Join-Path $root 'CloseToHSL\\Core\Pipeline\MicroOps\Replay\ReplayToken.h';$s=Get-Content $p -Raw
$s=$s.Replace('const std::vector<std::shared_ptr<TraceEvent>>& events','const std::vector<TraceEvent>& events')
Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $root 'CloseToHSL\\Core\Pipeline\MicroOps\Replay\ReplayToken.cpp';$s=Get-Content $p -Raw
$s=$s.Replace('#include "ReplayToken.h"', "#include `"ReplayToken.h`"`r`n#include `"../../../Diagnostics/TraceSink.h`"`r`n#include `"../../../Decoder/Sha256Digest.h`"`r`n#include <bit>`r`n#include <cstdint>")
$marker='    bool ReplayToken::MatchesConfig('
if(!$s.Contains($marker)){throw 'insertion marker missing'}
$method=@"
    std::wstring ReplayToken::ComputeTraceHash(const std::vector<TraceEvent>& events) {
        if (events.empty()) return L"";
        std::string text;
        for (const auto& event : events) {
            text += std::to_string(event.PC); text += '|';
            text += std::to_string(event.BundleId); text += '|';
            text += std::to_string(event.OpIndex); text += '|';
            text += std::to_string(std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(event.Opcode))); text += '|';
            text += std::to_string(event.ExceptionCount); text += '\n';
        }
        return YAKSys_Hybrid_CPU::Core::Decoder::Sha256Hex(std::span<const unsigned char>(reinterpret_cast<const unsigned char*>(text.data()), text.size()));
    }

"@
$s=$s.Replace($marker,$method+$marker);Set-Content $p $s -Encoding UTF8 -NoNewline
