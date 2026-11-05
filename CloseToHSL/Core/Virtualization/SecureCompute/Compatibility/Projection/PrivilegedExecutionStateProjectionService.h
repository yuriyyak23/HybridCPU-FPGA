#pragma once

#include "../../../../Runtime/Domains/SecureCompute/Policies/ExecutionState/PrivilegedExecutionStateOwnerPolicy.h"
#include "YAKSys_Hybrid_CPU.Core.SecureComputeVmReadVisibilityPolicy.h"
#include "SecureComputeVmReadVisibilityPolicy.h"
#include <string>
#include <format>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class PrivilegedExecutionStateProjectionDecision : unsigned char
	{
		AllowedReadOnlyProjection = 0,
		DeniedNoReadOnlySource = 1,
		DeniedOwnerAdmission = 2,
		DeniedNoSecureVisibility = 3,
		DeniedNoMigrationClass = 4,
		DeniedNoConformanceProof = 5,
		DeniedUnsupportedRegister = 6
	};

struct PrivilegedExecutionStateProjectionRequest {
    const PrivilegedControlRegisterKind Register; const std::shared_ptr<PrivilegedExecutionStateDescriptor> Descriptor; const unsigned long long RuntimeDomainTag, RuntimeAddressSpaceTag; const PrivilegedExecutionStateEpoch CurrentEpoch; const bool SecureVisibilityAllowed, MigrationClassified, ConformanceProven;
    PrivilegedExecutionStateProjectionRequest(PrivilegedControlRegisterKind reg, std::shared_ptr<PrivilegedExecutionStateDescriptor> descriptor, unsigned long long domain, unsigned long long space, PrivilegedExecutionStateEpoch epoch, bool visibility, bool migration, bool conformance) : Register(reg), Descriptor(std::move(descriptor)), RuntimeDomainTag(domain), RuntimeAddressSpaceTag(space), CurrentEpoch(epoch), SecureVisibilityAllowed(visibility), MigrationClassified(migration), ConformanceProven(conformance) {}
};
struct PrivilegedExecutionStateProjectionResult {
    const PrivilegedExecutionStateProjectionDecision Decision; const std::shared_ptr<PrivilegedExecutionStateOwnerResult> OwnerAdmission; const bool ValueAvailable; const unsigned long long Value; const bool BackendSuccessAuthorized, MutationAuthorized, CompletionPublicationAuthorized, RetirePublicationAuthorized; const std::wstring Reason; const bool IsAllowed;
    PrivilegedExecutionStateProjectionResult(PrivilegedExecutionStateProjectionDecision decision, std::shared_ptr<PrivilegedExecutionStateOwnerResult> owner, bool available, unsigned long long value, bool backend, bool mutation, bool completion, bool retire, std::wstring reason) : Decision(decision), OwnerAdmission(std::move(owner)), ValueAvailable(available), Value(value), BackendSuccessAuthorized(backend), MutationAuthorized(mutation), CompletionPublicationAuthorized(completion), RetirePublicationAuthorized(retire), Reason(std::move(reason)), IsAllowed(decision == PrivilegedExecutionStateProjectionDecision::AllowedReadOnlyProjection && OwnerAdmission && OwnerAdmission->IsAllowed && available && !backend && !mutation && !completion && !retire) {}
    static std::shared_ptr<PrivilegedExecutionStateProjectionResult> Denied(PrivilegedExecutionStateProjectionDecision decision, std::shared_ptr<PrivilegedExecutionStateOwnerResult> owner, std::wstring reason) { return std::make_shared<PrivilegedExecutionStateProjectionResult>(decision, std::move(owner), false, 0, false, false, false, false, std::move(reason)); }
};
	class PrivilegedExecutionStateProjectionService final : public std::enable_shared_from_this<PrivilegedExecutionStateProjectionService>
	{
	private:
		const std::shared_ptr<PrivilegedExecutionStateOwnerPolicy> _ownerPolicy;
		const std::shared_ptr<SecureComputeVmReadVisibilityPolicy> _visibilityPolicy;

	public:
		PrivilegedExecutionStateProjectionService();

		PrivilegedExecutionStateProjectionService(std::shared_ptr<PrivilegedExecutionStateOwnerPolicy> ownerPolicy, std::shared_ptr<SecureComputeVmReadVisibilityPolicy> visibilityPolicy);

		std::shared_ptr<PrivilegedExecutionStateProjectionResult> Project(std::shared_ptr<PrivilegedExecutionStateProjectionRequest> request);

	private:
		static PrivilegedExecutionStateProjectionDecision MapDeniedVisibility(SecureComputeVmReadVisibilityDecision decision);

		static std::shared_ptr<PrivilegedExecutionStateProjectionResult> Deny(PrivilegedExecutionStateProjectionDecision decision, std::shared_ptr<PrivilegedExecutionStateOwnerResult> ownerAdmission, const std::wstring &reason);
	};

}
