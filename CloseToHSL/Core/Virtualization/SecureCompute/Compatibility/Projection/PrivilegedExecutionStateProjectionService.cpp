#include "PrivilegedExecutionStateProjectionService.h"
#include "../../../../Runtime/Domains/SecureCompute/Descriptors/ExecutionState/PrivilegedExecutionStateDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	namespace {
const wchar_t* VisibilityName(SecureComputeVmReadVisibilityDecision value) {
    switch (value) {
    case SecureComputeVmReadVisibilityDecision::AllowedReadOnlyProjection: return L"AllowedReadOnlyProjection";
    case SecureComputeVmReadVisibilityDecision::DeniedNoNeutralOwner: return L"DeniedNoNeutralOwner";
    case SecureComputeVmReadVisibilityDecision::DeniedNoReadOnlySource: return L"DeniedNoReadOnlySource";
    case SecureComputeVmReadVisibilityDecision::DeniedNoSecureVisibility: return L"DeniedNoSecureVisibility";
    case SecureComputeVmReadVisibilityDecision::DeniedNoMigrationClass: return L"DeniedNoMigrationClass";
    case SecureComputeVmReadVisibilityDecision::DeniedNoConformanceProof: return L"DeniedNoConformanceProof";
    default: return L"Unknown";
    }
}
}
	PrivilegedExecutionStateProjectionService::PrivilegedExecutionStateProjectionService() : PrivilegedExecutionStateProjectionService(PrivilegedExecutionStateOwnerPolicy::getDefault(), std::make_shared<SecureComputeVmReadVisibilityPolicy>())
	{
	}

	PrivilegedExecutionStateProjectionService::PrivilegedExecutionStateProjectionService(std::shared_ptr<PrivilegedExecutionStateOwnerPolicy> ownerPolicy, std::shared_ptr<SecureComputeVmReadVisibilityPolicy> visibilityPolicy) : _ownerPolicy(ownerPolicy), _visibilityPolicy(visibilityPolicy)
	{
	}

	std::shared_ptr<PrivilegedExecutionStateProjectionResult> PrivilegedExecutionStateProjectionService::Project(std::shared_ptr<PrivilegedExecutionStateProjectionRequest> request)
	{
		if (request->Descriptor == nullptr)
		{
			return Deny(PrivilegedExecutionStateProjectionDecision::DeniedNoReadOnlySource, nullptr, L"Guest control-register projection requires a neutral privileged execution-state descriptor value source.");
		}

// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if (request->Register != PrivilegedControlRegisterKind::GuestCr0 && request->Register != PrivilegedControlRegisterKind::GuestCr4)
		{
			return Deny(PrivilegedExecutionStateProjectionDecision::DeniedUnsupportedRegister, nullptr, L"Only GuestCr0 and GuestCr4 are admitted by the privileged execution-state projection contract.");
		}

		std::shared_ptr<PrivilegedExecutionStateOwnerResult> ownerAdmission = _ownerPolicy->Admit(std::make_shared<PrivilegedExecutionStateOwnerRequest>(request->Descriptor, request->RuntimeDomainTag, request->RuntimeAddressSpaceTag, request->CurrentEpoch));

		if (!ownerAdmission->IsAllowed)
		{
			PrivilegedExecutionStateProjectionDecision decision = ownerAdmission->Decision == PrivilegedExecutionStateOwnerDecision::DeniedMigrationClass ? PrivilegedExecutionStateProjectionDecision::DeniedNoMigrationClass : PrivilegedExecutionStateProjectionDecision::DeniedOwnerAdmission;

			return Deny(decision, ownerAdmission, ownerAdmission->Reason);
		}

		SecureComputeVmReadVisibilityDecision visibility = _visibilityPolicy->Validate(true, true, request->SecureVisibilityAllowed, request->MigrationClassified, request->ConformanceProven);

		if (visibility != SecureComputeVmReadVisibilityDecision::AllowedReadOnlyProjection)
		{
			return Deny(MapDeniedVisibility(visibility), ownerAdmission, std::wstring(L"Guest control-register projection denied by ") + VisibilityName(visibility) + L".");
		}

		std::shared_ptr<PrivilegedExecutionStateDescriptor> descriptor = request->Descriptor;
		unsigned long long value = request->Register == PrivilegedControlRegisterKind::GuestCr0 ? descriptor->GuestCr0.Value : descriptor->GuestCr4.Value;

		return std::make_shared<PrivilegedExecutionStateProjectionResult>(PrivilegedExecutionStateProjectionDecision::AllowedReadOnlyProjection, ownerAdmission, true, value, false, false, false, false, L"Neutral privileged execution-state value is admitted for read-only compatibility projection only.");
	}

	PrivilegedExecutionStateProjectionDecision PrivilegedExecutionStateProjectionService::MapDeniedVisibility(SecureComputeVmReadVisibilityDecision decision)
	{
	switch (decision) {
        case SecureComputeVmReadVisibilityDecision::DeniedNoSecureVisibility: return PrivilegedExecutionStateProjectionDecision::DeniedNoSecureVisibility;
        case SecureComputeVmReadVisibilityDecision::DeniedNoMigrationClass: return PrivilegedExecutionStateProjectionDecision::DeniedNoMigrationClass;
        case SecureComputeVmReadVisibilityDecision::DeniedNoConformanceProof: return PrivilegedExecutionStateProjectionDecision::DeniedNoConformanceProof;
        case SecureComputeVmReadVisibilityDecision::DeniedNoReadOnlySource: return PrivilegedExecutionStateProjectionDecision::DeniedNoReadOnlySource;
        default: return PrivilegedExecutionStateProjectionDecision::DeniedOwnerAdmission;
        }
	}

	std::shared_ptr<PrivilegedExecutionStateProjectionResult> PrivilegedExecutionStateProjectionService::Deny(PrivilegedExecutionStateProjectionDecision decision, std::shared_ptr<PrivilegedExecutionStateOwnerResult> ownerAdmission, const std::wstring &reason)
	{
		return PrivilegedExecutionStateProjectionResult::Denied(decision, ownerAdmission, reason);
	}
}
