$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$p=Join-Path $root 'CloseToHSL\\Core\Pipeline\Assist\Runtime\AssistRuntime.h';$s=Get-Content $p -Raw
$a=$s.IndexOf('class AssistInterCoreTransport');$b=$s.IndexOf('AssistInterCoreTransport() = default;',$a);if($a -lt 0 -or $b -lt 0){throw 'transport not found'};$b+='AssistInterCoreTransport() = default;'.Length
$block=$s.Substring($a,$b-$a);$block=[regex]::Replace($block,'(?m)^(\s*)const ((?:unsigned long long|unsigned short|int|std::shared_ptr<MicroOp>)) (\w+);','$1$2 $3{};');$s=$s.Substring(0,$a)+$block+$s.Substring($b);Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $root 'CloseToHSL\\Core\Pipeline\Assist\InterCore\YAKSys_Hybrid_CPU.Core.MicroOpScheduler.h';$s=Get-Content $p -Raw
$s=$s.Replace('const std::vector<AssistInterCoreTransport> _interCoreAssistPorts', 'std::vector<AssistInterCoreTransport> _interCoreAssistPorts').Replace('std::vector<OracleCounterexample> _counterexamples = std::vector<OracleCounterexample>(MAX_COUNTEREXAMPLES);','std::vector<OracleCounterexample> _counterexamples;')
Set-Content $p $s -Encoding UTF8 -NoNewline
