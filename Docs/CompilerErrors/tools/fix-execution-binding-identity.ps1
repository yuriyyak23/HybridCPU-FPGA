$p=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'CloseToHSL\\Core\Decoder\Rf06ExecutionContracts.h'
$s=[IO.File]::ReadAllText($p)
if(!$s.Contains('#include "ExecutionBindingIdentity.h"')){ $s=$s.Replace('#include "ExecutionPlacement.h"','#include "ExecutionPlacement.h"'+"`r`n"+'#include "ExecutionBindingIdentity.h"');[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false));Write-Output 'Installed binding identities'}else{Write-Output 'Already installed'}
