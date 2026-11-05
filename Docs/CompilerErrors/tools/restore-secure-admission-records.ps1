$base='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Domains\SecureCompute\Policies'
$p=Join-Path $base 'Admission\SecureDomainAdmissionPolicy.h'; $s=[IO.File]::ReadAllText($p); $s=$s.Replace('#pragma once',"#pragma once`r`n#include <memory>`r`n#include <string>`r`n#include <utility>")
$i=$s.IndexOf("// C# TO C++ CONVERTER TASK: C# 'records'"); if($i -lt 0){throw 'domain anchor missing'}
$s=$s.Substring(0,$i)+@'
struct SecureDomainAdmissionResult {
    const SecureDomainAdmissionDecision Decision; const std::wstring Reason; const bool IsAllowed;
    SecureDomainAdmissionResult(SecureDomainAdmissionDecision d, std::wstring r) : Decision(d), Reason(std::move(r)), IsAllowed(d == SecureDomainAdmissionDecision::AllowedNoEffect || d == SecureDomainAdmissionDecision::AllowedSecureOperation) {}
    static std::shared_ptr<SecureDomainAdmissionResult> Denied(SecureDomainAdmissionDecision d, std::wstring r) { return std::make_shared<SecureDomainAdmissionResult>(d, std::move(r)); }
    static const std::shared_ptr<SecureDomainAdmissionResult> AllowedNoEffect, AllowedSecureOperation;
};
inline const std::shared_ptr<SecureDomainAdmissionResult> SecureDomainAdmissionResult::AllowedNoEffect = std::make_shared<SecureDomainAdmissionResult>(SecureDomainAdmissionDecision::AllowedNoEffect, L"");
inline const std::shared_ptr<SecureDomainAdmissionResult> SecureDomainAdmissionResult::AllowedSecureOperation = std::make_shared<SecureDomainAdmissionResult>(SecureDomainAdmissionDecision::AllowedSecureOperation, L"");
}
'@
[IO.File]::WriteAllText($p,$s)
$p=Join-Path $base 'Memory\SecureMemoryAdmissionPolicy.h'; $s=[IO.File]::ReadAllText($p); $s=$s.Replace('#pragma once',"#pragma once`r`n#include `"../../../../Capabilities/CapabilityBoundaryRequirement.h`"`r`n#include <memory>`r`n#include <string>`r`n#include <utility>")
$i=$s.IndexOf("// C# TO C++ CONVERTER WARNING: Nullable reference types"); if($i -lt 0){throw 'memory anchor missing'}
$s=$s.Substring(0,$i)+@'
struct SecureMemoryAccessRequest {
    SecureMemoryAccessKind Kind{}; unsigned long long Address{}, Length{};
    SecureMemoryAccessOrigin Origin = SecureMemoryAccessOrigin::RuntimeBoundary;
    CapabilityBoundaryRequirement CapabilityRequirement{0, CapabilityGrantScope::None, false};
    std::shared_ptr<CapabilityDescriptorSet> Capabilities;
    SecureMemoryAccessRequest(SecureMemoryAccessKind kind = SecureMemoryAccessKind::RuntimeTouch, unsigned long long address = 0, unsigned long long length = 0, SecureMemoryAccessOrigin origin = SecureMemoryAccessOrigin::RuntimeBoundary, CapabilityBoundaryRequirement requirement = {0, CapabilityGrantScope::None, false}, std::shared_ptr<CapabilityDescriptorSet> capabilities = nullptr)
        : Kind(kind), Address(address), Length(length), Origin(origin), CapabilityRequirement(requirement), Capabilities(std::move(capabilities)) {}
};
struct SecureMemoryAdmissionResult {
    const SecureMemoryAdmissionDecision Decision; const std::wstring Reason; const bool IsAllowed;
    SecureMemoryAdmissionResult(SecureMemoryAdmissionDecision d, std::wstring r) : Decision(d), Reason(std::move(r)), IsAllowed(d == SecureMemoryAdmissionDecision::AllowedSecureMemory) {}
    static std::shared_ptr<SecureMemoryAdmissionResult> Denied(SecureMemoryAdmissionDecision d, std::wstring r) { return std::make_shared<SecureMemoryAdmissionResult>(d, std::move(r)); }
    static const std::shared_ptr<SecureMemoryAdmissionResult> AllowedSecureMemory;
};
inline const std::shared_ptr<SecureMemoryAdmissionResult> SecureMemoryAdmissionResult::AllowedSecureMemory = std::make_shared<SecureMemoryAdmissionResult>(SecureMemoryAdmissionDecision::AllowedSecureMemory, L"");
}
'@
[IO.File]::WriteAllText($p,$s)