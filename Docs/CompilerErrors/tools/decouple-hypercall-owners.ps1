$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$jobs=@(
 @{ h='CloseToHSL\\Core\Runtime\Completion\Records\DomainHypercallCompletionOwner.h'; cpp='CloseToHSL\\Core\Runtime\Completion\Records\DomainHypercallCompletionOwner.cpp'; inc='DomainHypercallRetireOwner.h' },
 @{ h='CloseToHSL\\Core\Runtime\Events\Hypercalls\DomainHypercallRuntimeExecutor.h'; cpp='CloseToHSL\\Core\Runtime\Events\Hypercalls\DomainHypercallRuntimeExecutor.cpp'; inc='../../Completion/Records/DomainHypercallCompletionOwner.h' }
)
foreach($job in $jobs){$line='#include "'+$job.inc+'"';$p=Join-Path $root $job.h;$s=Get-Content $p -Raw;$s=$s.Replace($line,'');Set-Content $p $s -Encoding UTF8 -NoNewline;$p=Join-Path $root $job.cpp;$s=Get-Content $p -Raw;if(!$s.Contains($line)){$s=$line+"`r`n"+$s};Set-Content $p $s -Encoding UTF8 -NoNewline}
