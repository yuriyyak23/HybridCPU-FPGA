#pragma once
#include "../../Guid.h"
#include <compare>
namespace HybridCPU::ExternalRuntime::Contracts {
class ExternalOperationPublicationEvidence;
struct HybridCpuExternalContractVersion final {
    unsigned short Major = 0, Minor = 0, Patch = 0;
    constexpr bool IsValid() const noexcept { return Major != 0; }
    bool operator==(const HybridCpuExternalContractVersion&) const = default;
    auto operator<=>(const HybridCpuExternalContractVersion&) const = default;
};
struct ExternalDomainLeaseHandle final { Guid Value; auto operator<=>(const ExternalDomainLeaseHandle&) const = default; };
struct ExternalDomainLeaseEpoch final { unsigned long long Value = 0; auto operator<=>(const ExternalDomainLeaseEpoch&) const = default; };
struct ExternalDomainLease final { ExternalDomainLeaseHandle Handle; ExternalDomainLeaseEpoch Epoch; auto operator<=>(const ExternalDomainLease&) const = default; };
struct ExternalOperationHandle final { Guid Value; auto operator<=>(const ExternalOperationHandle&) const = default; };
struct ExternalOperationGeneration final { unsigned long long Value = 0; auto operator<=>(const ExternalOperationGeneration&) const = default; };
struct ExternalOperationIdentity final { ExternalOperationHandle Handle; ExternalOperationGeneration Generation; auto operator<=>(const ExternalOperationIdentity&) const = default; };
}

