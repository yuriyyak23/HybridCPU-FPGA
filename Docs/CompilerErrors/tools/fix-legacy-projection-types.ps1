$p=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'CloseToHSL\\Core\Decoder\Rf06ExecutionContracts.h'
$s=[IO.File]::ReadAllText($p)
if(!$s.Contains('#include "LegacyAdmissionProjection.h"')) {$s=$s.Replace('#include "ExecutionBindingIdentity.h"','#include "ExecutionBindingIdentity.h"'+"`r`n"+'#include "LegacyAdmissionProjection.h"'); [IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false));Write-Output 'Installed projection types'} else {Write-Output 'Already installed'}
