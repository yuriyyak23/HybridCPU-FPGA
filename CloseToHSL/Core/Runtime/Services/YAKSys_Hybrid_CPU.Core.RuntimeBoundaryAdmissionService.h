#pragma once

#include "../Domains/Authority/YAKSys_Hybrid_CPU.Core.DomainRuntimeAuthority.h"
#include "../Domains/SecureCompute/Admission/YAKSys_Hybrid_CPU.Core.SecureDomainAdmissionService.h"
#include "../Domains/SecureCompute/Policies/Memory/YAKSys_Hybrid_CPU.Core.SecureMemoryAdmissionPolicy.h"
#include "../Domains/Services/YAKSys_Hybrid_CPU.Core.DomainRuntimeOperation.h"
#include "RuntimeBoundaryAdmissionService.h"
#include <string>
#include <memory>

// Description: Generic runtime admission service that joins domain, capability, evidence, and root-authority gates.
namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Generic runtime policy input. SecureDescriptor is a legacy exact-reference
	/// mirror only; it cannot substitute for Context.SecureCompute or grant execution.
	/// </summary>




	class RuntimeBoundaryAdmissionService final : public std::enable_shared_from_this<RuntimeBoundaryAdmissionService>
	{
	private:
		const std::shared_ptr<DomainRuntimeAuthority> _authority;
		const std::shared_ptr<SecureDomainAdmissionService> _secureAdmission;
		const std::shared_ptr<SecureMemoryAdmissionPolicy> _secureMemoryAdmission;

	public:
		RuntimeBoundaryAdmissionService();

		RuntimeBoundaryAdmissionService(std::shared_ptr<DomainRuntimeAuthority> authority);

		RuntimeBoundaryAdmissionService(std::shared_ptr<DomainRuntimeAuthority> authority, std::shared_ptr<SecureDomainAdmissionService> secureAdmission);

		RuntimeBoundaryAdmissionService(std::shared_ptr<DomainRuntimeAuthority> authority, std::shared_ptr<SecureDomainAdmissionService> secureAdmission, std::shared_ptr<SecureMemoryAdmissionPolicy> secureMemoryAdmission);

		std::shared_ptr<RuntimeBoundaryAdmissionResult> Validate(std::shared_ptr<RuntimeBoundaryAdmissionRequest> request);

		bool CanAdmit(std::shared_ptr<RuntimeBoundaryAdmissionRequest> request);

	private:
		static bool IsFrontendAuthoritativeMutation(std::shared_ptr<DomainRuntimeOperation> operation);

		static std::shared_ptr<RuntimeBoundaryAdmissionResult> Deny(RuntimeBoundaryAdmissionDecision decision, const std::wstring &message, std::shared_ptr<DomainRuntimeAuthorityResult> authorityResult = nullptr);
	};
}
