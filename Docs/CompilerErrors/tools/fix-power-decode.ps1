$path=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'Arch\\PowerControlCsr.cpp'
$text=[IO.File]::ReadAllText($path)
$pattern='(?s)(Processor::CPU_Core::CorePowerState PowerControlCsr::DecodePowerState\(unsigned long long rawValue\)\s*\{)\s*// C# TO C\+\+ CONVERTER TASK:.*?//\s*\};\s*(\})'
$body=@"
`$1
        using State = Processor::CPU_Core::CorePowerState;
        switch (rawValue) {
            case static_cast<unsigned long long>(State::C0_Active): return State::C0_Active;
            case static_cast<unsigned long long>(State::C1_Halt): return State::C1_Halt;
            case static_cast<unsigned long long>(State::C2_StopClock): return State::C2_StopClock;
            case static_cast<unsigned long long>(State::C3_Sleep): return State::C3_Sleep;
            case static_cast<unsigned long long>(State::C6_DeepPowerDown): return State::C6_DeepPowerDown;
            default: return State::ErrorState;
        }
    `$2
"@
$count=[regex]::Matches($text,$pattern).Count
if($count -eq 1){$text=[regex]::Replace($text,$pattern,$body);[IO.File]::WriteAllText($path,$text,[Text.UTF8Encoding]::new($false));Write-Output 'Fixed DecodePowerState'} elseif($count -eq 0 -and $text.Contains('switch (rawValue)')) {Write-Output 'Already fixed'}else{throw "Unexpected match count: $count"}