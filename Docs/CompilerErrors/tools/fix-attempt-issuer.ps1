$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$h=Join-Path $root 'CloseToHSL\\Core\Decoder\Rf06ExecutionContracts.h';$c=Join-Path $root 'CloseToHSL\\Core\Decoder\Rf06ExecutionContracts.cpp'
$ht=[IO.File]::ReadAllText($h);$ct=[IO.File]::ReadAllText($c)
if(!$ht.Contains('#include "OperationAttemptIssuer.h"')){
 $ht=$ht.Replace('#include "ExecutionBindingIdentity.h"','#include "ExecutionBindingIdentity.h"'+"`r`n"+'#include "OperationAttemptIssuer.h"')
 $pattern='(?s)\tclass OperationAttemptIssuer final\s*:\s*public std::enable_shared_from_this<OperationAttemptIssuer>\s*\{.*?\n\t\};'
 if([regex]::Matches($ht,$pattern).Count -ne 1){throw 'Issuer declaration not found'}
 $ht=[regex]::Replace($ht,$pattern,'')
 $pattern='(?s)\tunsigned long long OperationAttemptIssuer::IssueNextAttempt\(\)\s*\{.*?\n\t\}'
 if([regex]::Matches($ct,$pattern).Count -ne 1){throw 'Issuer implementation not found'}
 $ct=[regex]::Replace($ct,$pattern,'')
}
$decl='namespace YAKSys_Hybrid_CPU::Core::Decoder { class VliwOperationId; class AdmissionRecord; }'
if(!$ht.Contains($decl)){$ht=$ht.Replace('#include "CanonicalDecodedContracts.h"',$decl+"`r`n"+'#include "CanonicalDecodedContracts.h"')}
[IO.File]::WriteAllText($h,$ht,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($c,$ct,[Text.UTF8Encoding]::new($false));Write-Output 'Installed checked attempt issuer'
