$path=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'CloseToHSL\\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.h'
$text=[IO.File]::ReadAllText($path)
$enum=[regex]::Match($text,'(?s)enum class CorePowerState\s*:\s*(?:unsigned int|int)\s*\{.*?\};').Value
if(!$enum){throw 'Enum not found'}
$test='static_assert(sizeof(CorePowerState)==4); static_assert(static_cast<unsigned int>(CorePowerState::ErrorState)==0xFFFFFFFFu); static_assert(static_cast<unsigned int>(CorePowerState::C6_DeepPowerDown)==6u); int main(){return 0;}'
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'power-enum-check.cpp'),$enum+"`r`n"+$test,[Text.Encoding]::ASCII)