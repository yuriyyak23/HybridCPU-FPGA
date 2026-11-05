$dir='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\Safety'; $h=Join-Path $dir 'SafetyVerifier.SecureComputeAdmission.h'; $c=Join-Path $dir 'SafetyVerifier.SecureComputeAdmission.cpp'
$hs=[IO.File]::ReadAllText($h);$cs=[IO.File]::ReadAllText($c)
$types=@('ExternalSecureExecutionAttempt','ExternalSecureExecutionConsumptionResult','ExternalSecureNoEffectSubmission','ExternalSecureNoEffectExecutionResult','ExternalSecureRegisterLocalSubmission','ExternalSecureRegisterLocalExecutionResult')
foreach($type in $types){$old='std::optional<std::shared_ptr<'+$type+'>> &'; $new='std::optional<std::shared_ptr<'+$type+'>> '; if(-not $hs.Contains($old) -or -not $cs.Contains($old)){throw "Signature not found: $type"}; $hs=$hs.Replace($old,$new);$cs=$cs.Replace($old,$new)}
$marker='using namespace HybridCPU::ExternalRuntime::Contracts;'; $hs=$hs.Replace($marker,'namespace HybridCPU::ExternalRuntime::Contracts { class ExternalSecureExecutionConsumptionResult; }'+"`r`n"+$marker)
$start=$cs.IndexOf("`tSecureComputeCanonicalAdmissionResult::SecureComputeCanonicalAdmissionResult(");$brace=$cs.IndexOf("`r`n`t{",$start);$end=$cs.IndexOf("`r`n`t}",$brace)
if($start -lt 0 -or $brace -lt 0 -or $end -lt 0){throw 'Constructor missing'}
$init=@"
        : Request(std::move(request)), Decision(decision), Canonical(std::move(canonical)),
          PlacedIntent(std::move(placedIntent)), MachinePermitConsumption(std::move(machinePermitConsumption)),
          BackendAttempt(std::move(backendAttempt)), BackendConsumption(std::move(backendConsumption)),
          NoEffectSubmission(std::move(noEffectSubmission)), NoEffectOutcome(std::move(noEffectOutcome)),
          NoEffectRetirement(std::move(noEffectRetirement)), RegisterLocalSubmission(std::move(registerLocalSubmission)),
          RegisterLocalOutcome(std::move(registerLocalOutcome)), RegisterLocalRetirement(std::move(registerLocalRetirement))
    {}
"@
$cs=$cs.Remove($brace,$end+4-$brace).Insert($brace,"`r`n"+$init)
$hs=$hs.Replace('#include <memory>',"#include <memory>`r`n#include <utility>")
[IO.File]::WriteAllText($h,$hs,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($c,$cs,[Text.UTF8Encoding]::new($false))
