$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$jobs=@(
 @{ h='CloseToHSL\\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.h'; cpp='CloseToHSL\\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.cpp'; include='../../Pipeline/MicroOps/Assist/AssistMicroOp.h' },
 @{ h='CloseToHSL\\Core\Pipeline\Assist\InterCore\YAKSys_Hybrid_CPU.Core.MicroOpScheduler.h'; cpp='CloseToHSL\\Core\Pipeline\Assist\InterCore\YAKSys_Hybrid_CPU.Core.MicroOpScheduler.cpp'; include='../../MicroOps/Assist/AssistMicroOp.h' },
 @{ h='CloseToHSL\\Core\Decoder\Rf06SpecializedCapabilityProjection.h'; cpp='CloseToHSL\\Core\Decoder\Rf06SpecializedCapabilityProjection.cpp'; include='../Pipeline/MicroOps/Assist/AssistMicroOp.h' }
)
foreach($job in $jobs){$p=Join-Path $root $job.h;$s=Get-Content $p -Raw;$line='#include "'+$job.include+'"';$s=$s.Replace($line,'namespace YAKSys_Hybrid_CPU::Core { class AssistMicroOp; }'); Set-Content $p $s -Encoding UTF8 -NoNewline; $p=Join-Path $root $job.cpp;if(Test-Path $p){$s=Get-Content $p -Raw;if(!$s.Contains($line)){$s=$line+"`r`n"+$s};Set-Content $p $s -Encoding UTF8 -NoNewline}}
$p=Join-Path $root 'CloseToHSL\\Core\Pipeline\Assist\InterCore\YAKSys_Hybrid_CPU.Core.MicroOpScheduler.h';$s=Get-Content $p -Raw;$line='#include "../../../../Memory/Banks/MultiBankMemoryArea.h"';$s=$s.Replace($line,'');Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $root 'CloseToHSL\\Core\Pipeline\Assist\InterCore\YAKSys_Hybrid_CPU.Core.MicroOpScheduler.cpp';$s=Get-Content $p -Raw;if(!$s.Contains($line)){$s=$line+"`r`n"+$s};Set-Content $p $s -Encoding UTF8 -NoNewline
