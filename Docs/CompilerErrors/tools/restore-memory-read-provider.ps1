$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2';$path=Join-Path $root 'ExternalRuntime\Contracts\ExternalSecureMemoryReadProvider.h';if(Test-Path $path){throw 'Destination exists'}
$content=@"
#pragma once
#include <memory>
namespace HybridCPU::ExternalRuntime::Contracts {
class ExternalSecureMemoryReadRequest;
class ExternalSecureMemoryReadReservation;
class ExternalSecureMemoryReadReservationResult;
class ExternalSecureMemoryReadAttempt;
class ExternalSecureMemoryReadConsumptionResult;
class ExternalSecureMemoryReadSubmission;
class ExternalSecureMemoryReadResult;
class IExternalSecureMemoryReadProvider {
public:
    virtual ~IExternalSecureMemoryReadProvider() = default;
    virtual std::shared_ptr<ExternalSecureMemoryReadReservationResult> ReserveMemoryRead(std::shared_ptr<ExternalSecureMemoryReadRequest> request) = 0;
    virtual std::shared_ptr<ExternalSecureMemoryReadReservationResult> QueryMemoryReadReservation(std::shared_ptr<ExternalSecureMemoryReadRequest> request) = 0;
    virtual std::shared_ptr<ExternalSecureMemoryReadConsumptionResult> ConsumeMemoryRead(std::shared_ptr<ExternalSecureMemoryReadAttempt> attempt) = 0;
    virtual std::shared_ptr<ExternalSecureMemoryReadConsumptionResult> QueryMemoryReadConsumption(std::shared_ptr<ExternalSecureMemoryReadAttempt> attempt) = 0;
    virtual std::shared_ptr<ExternalSecureMemoryReadResult> SubmitMemoryRead(std::shared_ptr<ExternalSecureMemoryReadSubmission> submission) = 0;
    virtual std::shared_ptr<ExternalSecureMemoryReadResult> QueryMemoryRead(std::shared_ptr<ExternalSecureMemoryReadSubmission> submission) = 0;
    virtual std::shared_ptr<ExternalSecureMemoryReadReservationResult> ReleaseMemoryRead(std::shared_ptr<ExternalSecureMemoryReadReservation> reservation) = 0;
    virtual std::shared_ptr<ExternalSecureMemoryReadReservationResult> QueryMemoryReadRelease(std::shared_ptr<ExternalSecureMemoryReadReservation> reservation) = 0;
};
}
"@
$h=Join-Path $root 'CloseToHSL\Core\Pipeline\Safety\YAKSys_Hybrid_CPU.Core.SafetyVerifier.h';$s=[IO.File]::ReadAllText($h);$marker='#include "../../../../ExternalRuntime/Contracts/ExternalSecureExecutionConsumptionProvider.h"';if(-not $s.Contains($marker)){throw 'Include marker missing'};$s=$s.Replace($marker,$marker+"`r`n"+'#include "../../../../ExternalRuntime/Contracts/ExternalSecureMemoryReadProvider.h"')
[IO.File]::WriteAllText($path,$content,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($h,$s,[Text.UTF8Encoding]::new($false))
