$src=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'Arch\\PowerControlCsr.cpp'
$text=[IO.File]::ReadAllText($src)
$method=[regex]::Match($text,'(?s)Processor::CPU_Core::CorePowerState PowerControlCsr::DecodePowerState\(unsigned long long rawValue\).*?\n\s*\}\s*\n\s*\}').Value
if(!$method) {throw 'Method not found'}
$pre='struct Processor { struct CPU_Core { enum class CorePowerState : int { C0_Active=0,C1_Halt=1,C2_StopClock=2,C3_Sleep=3,C6_DeepPowerDown=6,ErrorState=-1 }; }; }; class PowerControlCsr { public: static Processor::CPU_Core::CorePowerState DecodePowerState(unsigned long long); };'
$test='int main(){using S=Processor::CPU_Core::CorePowerState; for(unsigned long long i=0;i<10;i++){ S x=PowerControlCsr::DecodePowerState(i); if(i==0 || i==1 || i==2 || i==3 || i==6){if(static_cast<unsigned long long>(x)!=i)return 1;} else if(x!=S::ErrorState)return 2;} if(PowerControlCsr::DecodePowerState(0x100000000ULL)!=S::ErrorState)return 3; return 0; }'
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'power-decode-check.cpp'),$pre+"`r`n"+$method+"`r`n"+$test,[Text.Encoding]::ASCII)