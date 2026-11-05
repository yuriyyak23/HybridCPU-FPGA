$p=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'CloseToHSL\\Core\Decoder\Rf06ExecutionContracts.h'
$s=[IO.File]::ReadAllText($p)
if(!$s.Contains('#include "ExecutionPlacement.h"')){$s=$s.Replace('#include "CanonicalDecodedContracts.h"','#include "ExecutionPlacement.h"'+"`r`n"+'#include "CanonicalDecodedContracts.h"');[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false));Write-Output 'Included ExecutionPlacement'}else{Write-Output 'Already included'}
