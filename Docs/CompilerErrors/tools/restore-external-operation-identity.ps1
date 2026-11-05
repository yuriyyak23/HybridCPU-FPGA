$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2';$guid=Join-Path $root 'Guid.h';$s=[IO.File]::ReadAllText($guid);$m='bool operator==(const Guid&) const = default;';if(-not $s.Contains($m)){throw 'Guid equality missing'};$s=$s.Replace($m,$m+"`r`n    auto operator<=>(const Guid&) const = default;");$s=$s.Replace('#include <array>',"#include <array>`r`n#include <compare>")
$dir=Join-Path $root 'ExternalRuntime\Contracts';$identity=Join-Path $dir 'ExternalOperationIdentity.h';if(Test-Path $identity){throw 'Identity file exists'}
$content=@"
#pragma once
#include "../../Guid.h"
#include <compare>
namespace HybridCPU::ExternalRuntime::Contracts {
struct ExternalOperationHandle final { Guid Value; auto operator<=>(const ExternalOperationHandle&) const = default; };
struct ExternalOperationGeneration final { unsigned long long Value = 0; auto operator<=>(const ExternalOperationGeneration&) const = default; };
struct ExternalOperationIdentity final { ExternalOperationHandle Handle; ExternalOperationGeneration Generation; auto operator<=>(const ExternalOperationIdentity&) const = default; };
}
"@
$h=Join-Path $root 'CloseToHSL\Core\Pipeline\Safety\YAKSys_Hybrid_CPU.Core.SafetyVerifier.h';$hs=[IO.File]::ReadAllText($h);$m='#include "../../../../Guid.h"';if(-not $hs.Contains($m)){throw 'Guid include missing'};$hs=$hs.Replace($m,$m+"`r`n"+'#include "../../../../ExternalRuntime/Contracts/ExternalOperationIdentity.h"');$hs=$hs.Replace('#include <unordered_map>',"#include <unordered_map>`r`n#include <map>")
$old='std::unordered_map<std::shared_ptr<ExternalOperationIdentity>, std::shared_ptr<ExternalSecureMemoryReadRequest>> memoryReadOperations = std::unordered_map<std::shared_ptr<ExternalOperationIdentity>, std::shared_ptr<ExternalSecureMemoryReadRequest>>();';$new='std::map<ExternalOperationIdentity, std::shared_ptr<ExternalSecureMemoryReadRequest>> memoryReadOperations;';if(-not $hs.Contains($old)){throw 'Operation map declaration missing'};$hs=$hs.Replace($old,$new).Replace('revokedSecureCertificateDomains = [];','revokedSecureCertificateDomains = {};').Replace('secureSubmissions = [];','secureSubmissions = {};')
[IO.File]::WriteAllText($guid,$s,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($identity,$content,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($h,$hs,[Text.UTF8Encoding]::new($false))
