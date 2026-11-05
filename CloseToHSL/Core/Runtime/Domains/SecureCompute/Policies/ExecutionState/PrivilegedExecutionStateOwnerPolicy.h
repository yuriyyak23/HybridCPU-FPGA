#pragma once

#include "../../Descriptors/ExecutionState/PrivilegedExecutionStateDescriptor.h"
#include <string>
#include <utility>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class PrivilegedExecutionStateOwnerDecision : unsigned char
	{
		AllowedOwnerMaterializedProjectionClosed = 0,
		DeniedMissingDescriptor = 1,
		DeniedUnmaterializedDescriptor = 2,
		DeniedDomainTagMismatch = 3,
		DeniedAddressSpaceTagMismatch = 4,
		DeniedStaleEpoch = 5,
		DeniedRegisterKindMismatch = 6,
		DeniedEvidenceClass = 7,
		DeniedMigrationClass = 8,
		DeniedGuestCr0ReservedBits = 9,
		DeniedGuestCr0RequiredBits = 10,
		DeniedGuestCr4ReservedBits = 11,
		DeniedGuestCr4RequiredBits = 12
	};

struct PrivilegedExecutionStateOwnerRequest {
    std::shared_ptr<PrivilegedExecutionStateDescriptor> Descriptor; unsigned long long RuntimeDomainTag{}, RuntimeAddressSpaceTag{}; PrivilegedExecutionStateEpoch CurrentEpoch{};
    PrivilegedExecutionStateOwnerRequest(std::shared_ptr<PrivilegedExecutionStateDescriptor> descriptor, unsigned long long domain, unsigned long long space, PrivilegedExecutionStateEpoch epoch) : Descriptor(std::move(descriptor)), RuntimeDomainTag(domain), RuntimeAddressSpaceTag(space), CurrentEpoch(epoch) {}
};
struct PrivilegedExecutionStateOwnerResult {
    const PrivilegedExecutionStateOwnerDecision Decision; const bool OwnerAccepted, ReadOnlyProjectionAuthorized, MutationAuthorized, BackendExecutionAuthorized, CompletionPublicationAuthorized, RetirePublicationAuthorized; const std::wstring Reason; const bool IsAllowed;
    PrivilegedExecutionStateOwnerResult(PrivilegedExecutionStateOwnerDecision d, bool owner, bool projection, bool mutation, bool backend, bool completion, bool retire, std::wstring reason) : Decision(d), OwnerAccepted(owner), ReadOnlyProjectionAuthorized(projection), MutationAuthorized(mutation), BackendExecutionAuthorized(backend), CompletionPublicationAuthorized(completion), RetirePublicationAuthorized(retire), Reason(std::move(reason)), IsAllowed(d == PrivilegedExecutionStateOwnerDecision::AllowedOwnerMaterializedProjectionClosed && owner) {}
    static std::shared_ptr<PrivilegedExecutionStateOwnerResult> Denied(PrivilegedExecutionStateOwnerDecision d, std::wstring reason) { return std::make_shared<PrivilegedExecutionStateOwnerResult>(d, false, false, false, false, false, false, std::move(reason)); }
    static const std::shared_ptr<PrivilegedExecutionStateOwnerResult> AllowedOwnerOnly;
};
inline const std::shared_ptr<PrivilegedExecutionStateOwnerResult> PrivilegedExecutionStateOwnerResult::AllowedOwnerOnly = std::make_shared<PrivilegedExecutionStateOwnerResult>(PrivilegedExecutionStateOwnerDecision::AllowedOwnerMaterializedProjectionClosed, true, false, false, false, false, false, L"Neutral privileged execution-state ownership is accepted; projection, mutation, execution and publication remain closed.");
	class PrivilegedExecutionStateOwnerPolicy final : public std::enable_shared_from_this<PrivilegedExecutionStateOwnerPolicy>
	{
	private:
		static inline const std::shared_ptr<PrivilegedExecutionStateOwnerPolicy> Default = std::make_shared<PrivilegedExecutionStateOwnerPolicy>();

	public:
		static std::shared_ptr<PrivilegedExecutionStateOwnerPolicy> getDefault();

		std::shared_ptr<PrivilegedExecutionStateOwnerResult> Admit(std::shared_ptr<PrivilegedExecutionStateOwnerRequest> request);

	private:
		static std::shared_ptr<PrivilegedExecutionStateOwnerResult> Deny(PrivilegedExecutionStateOwnerDecision decision, const std::wstring &reason);
	};

}
