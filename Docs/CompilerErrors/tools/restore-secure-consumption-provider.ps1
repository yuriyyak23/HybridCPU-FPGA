$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2';$dir=Join-Path $root 'ExternalRuntime\Contracts';[IO.Directory]::CreateDirectory($dir) | Out-Null;$path=Join-Path $dir 'ExternalSecureExecutionConsumptionProvider.h';if(Test-Path $path){throw 'File already exists'}
$content=@"
#pragma once
#include <memory>
namespace HybridCPU::ExternalRuntime::Contracts {
class ExternalSecureExecutionAttempt;
class ExternalSecureExecutionConsumptionResult;
// Provider methods retain the original attempt; neither call authorizes execution.
class IExternalSecureExecutionConsumptionProvider {
public:
    virtual ~IExternalSecureExecutionConsumptionProvider() = default;
    virtual std::shared_ptr<ExternalSecureExecutionConsumptionResult> Consume(std::shared_ptr<ExternalSecureExecutionAttempt> attempt) = 0;
    virtual std::shared_ptr<ExternalSecureExecutionConsumptionResult> QueryConsumption(std::shared_ptr<ExternalSecureExecutionAttempt> attempt) = 0;
};
}
"@
$h=Join-Path $root 'CloseToHSL\Core\Pipeline\Safety\YAKSys_Hybrid_CPU.Core.SafetyVerifier.h';$s=[IO.File]::ReadAllText($h);$m='#include "../../../../Guid.h"';if(-not $s.Contains($m)){throw 'Include marker missing'};$s=$s.Replace($m,$m+"`r`n"+'#include "../../../../ExternalRuntime/Contracts/ExternalSecureExecutionConsumptionProvider.h"')
[IO.File]::WriteAllText($path,$content,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($h,$s,[Text.UTF8Encoding]::new($false))
