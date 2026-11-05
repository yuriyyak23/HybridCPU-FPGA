#include "YAKSys_Hybrid_CPU.Core.RuntimeBoundaryAdmissionService.h"
#include "../Domains/Services/YAKSys_Hybrid_CPU.Core.DomainRuntimeContext.h"
#include "../Domains/SecureCompute/Descriptors/Domain/YAKSys_Hybrid_CPU.Core.SecureComputeDomainDescriptor.h"
#include "../Domains/SecureCompute/Policies/Admission/SecureDomainAdmissionPolicy.h"
#include "../Domains/Services/DomainRuntimeOperation.h"

namespace YAKSys_Hybrid_CPU::Core
{

	RuntimeBoundaryAdmissionService::RuntimeBoundaryAdmissionService() : RuntimeBoundaryAdmissionService(std::make_shared<DomainRuntimeAuthority>(), std::make_shared<SecureDomainAdmissionService>(), std::make_shared<SecureMemoryAdmissionPolicy>())
	{
	}

	RuntimeBoundaryAdmissionService::RuntimeBoundaryAdmissionService(std::shared_ptr<DomainRuntimeAuthority> authority) : RuntimeBoundaryAdmissionService(authority, std::make_shared<SecureDomainAdmissionService>(), std::make_shared<SecureMemoryAdmissionPolicy>())
	{
	}

	RuntimeBoundaryAdmissionService::RuntimeBoundaryAdmissionService(std::shared_ptr<DomainRuntimeAuthority> authority, std::shared_ptr<SecureDomainAdmissionService> secureAdmission) : RuntimeBoundaryAdmissionService(authority, secureAdmission, std::make_shared<SecureMemoryAdmissionPolicy>())
	{
	}

	RuntimeBoundaryAdmissionService::RuntimeBoundaryAdmissionService(std::shared_ptr<DomainRuntimeAuthority> authority, std::shared_ptr<SecureDomainAdmissionService> secureAdmission, std::shared_ptr<SecureMemoryAdmissionPolicy> secureMemoryAdmission) : _authority(authority), _secureAdmission(secureAdmission), _secureMemoryAdmission(secureMemoryAdmission)
	{
	}

	std::shared_ptr<RuntimeBoundaryAdmissionResult> RuntimeBoundaryAdmissionService::Validate(std::shared_ptr<RuntimeBoundaryAdmissionRequest> request)
	{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: DomainRuntimeContext? context = request.Context;
		std::shared_ptr<DomainRuntimeContext> context = request->Context;
		if (context == nullptr)
		{
			return Deny(RuntimeBoundaryAdmissionDecision::MissingDomainContext, L"Runtime boundary admission requires a domain runtime context.");
		}

		if (!request->DomainBoundary->IsSatisfiedBy(context))
		{
			return Deny(RuntimeBoundaryAdmissionDecision::DomainBoundaryDenied, L"Runtime boundary admission requires descriptor-owned execution, memory and I/O domains.");
		}

		if (!request->CapabilityRequirement->IsSatisfiedBy(context->getCapabilities()))
		{
			return Deny(RuntimeBoundaryAdmissionDecision::CapabilityBoundaryDenied, L"Runtime boundary admission requires a typed capability grant.");
		}

		if (!request->EvidenceRequirement.IsSatisfiedBy(request->EvidencePolicy))
		{
			return Deny(RuntimeBoundaryAdmissionDecision::EvidenceBoundaryDenied, L"Runtime boundary admission requires evidence policy approval.");
		}

		if (IsFrontendAuthoritativeMutation(request->Operation))
		{
			return Deny(RuntimeBoundaryAdmissionDecision::FrontendAuthoritativeMutationDenied, L"Compatibility frontend cannot directly mutate authoritative runtime state.");
		}

		// The legacy request field is only an exact mirror, never a fallback
		// authority carrier. All policy input comes from the runtime context.
		if (request->SecureDescriptor != nullptr && request->SecureDescriptor != context->getSecureCompute())
		{
			return Deny(RuntimeBoundaryAdmissionDecision::SecureDomainBoundaryDenied, L"Request secure descriptor must mirror the exact runtime context descriptor; request-only or foreign carriers are denied.");
		}
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: SecureComputeDomainDescriptor? secureDescriptor = context.SecureCompute;
		std::shared_ptr<SecureComputeDomainDescriptor> secureDescriptor = context->getSecureCompute();
		if (request->Operation->isNoStateExecution() && secureDescriptor != nullptr)
		{
			return Deny(RuntimeBoundaryAdmissionDecision::SecureDomainBoundaryDenied, L"Ordinary no-state runtime execution cannot enter a SecureCompute domain.");
		}

		if (request->SecureOperationClass != SecureDomainOperationClass::Ordinary)
		{
			std::shared_ptr<SecureDomainAdmissionResult> secureAdmission = _secureAdmission->Admit(secureDescriptor, request->SecureOperationClass, request->SecureMeasurement, request->SecureMemory);

			if (!secureAdmission->IsAllowed)
			{
				return Deny(RuntimeBoundaryAdmissionDecision::SecureDomainBoundaryDenied, secureAdmission->Reason);
			}

			if (secureDescriptor == nullptr)
			{
				return Deny(RuntimeBoundaryAdmissionDecision::SecureDomainBoundaryDenied, L"Secure descriptor is required only for secure-domain operation classes.");
			}

			if (!context->IsBoundToDomain(secureDescriptor->getDomainTag()))
			{
				return Deny(RuntimeBoundaryAdmissionDecision::SecureDomainBoundaryDenied, L"Secure domain descriptor must match the neutral runtime domain tag.");
			}

// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
			if (request->SecureMemory && request->SecureMemory->isMaterialized() && !request->SecureMemory->IsBoundTo(secureDescriptor->getDomainTag(), context->getAddressSpaceTag()))
			{
				return Deny(RuntimeBoundaryAdmissionDecision::SecureDomainBoundaryDenied, L"Secure memory descriptor must match the secure domain and neutral address-space tags.");
			}

// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
			if (request->SecureMemoryAccess != nullptr)
			{
				std::shared_ptr<SecureMemoryAdmissionResult> memoryAdmission = _secureMemoryAdmission->Admit(request->SecureMemory, request->SecureMemoryAccess, secureDescriptor->getIoPolicy());

				if (!memoryAdmission->IsAllowed)
				{
					return Deny(RuntimeBoundaryAdmissionDecision::SecureDomainBoundaryDenied, memoryAdmission->Reason);
				}
			}

			// Legacy policy classification is caller supplied. Descriptor policy
			// approval cannot stand in for canonical decode, binding or issue permission.
			// No secure machine producer is connected to this generic request seam.
			return Deny(RuntimeBoundaryAdmissionDecision::SecureDomainBoundaryDenied, L"Secure policy classification does not authorize execution; canonical machine admission is unavailable.");
		}

		std::shared_ptr<DomainRuntimeAuthorityResult> authority = _authority->Validate(request->RootAuthority, context, request->Operation, request->DomainBoundary, request->CapabilityRequirement);

		if (!authority->IsAllowed)
		{
			return Deny(RuntimeBoundaryAdmissionDecision::RuntimeAuthorityDenied, authority->Message, authority);
		}

		return RuntimeBoundaryAdmissionResult::Allowed(authority);
	}

	bool RuntimeBoundaryAdmissionService::CanAdmit(std::shared_ptr<RuntimeBoundaryAdmissionRequest> request)
	{
		return Validate(request)->IsAllowed;
	}

	bool RuntimeBoundaryAdmissionService::IsFrontendAuthoritativeMutation(std::shared_ptr<DomainRuntimeOperation> operation)
	{
		return operation->getSource() == DomainRuntimeOperationSource::CompatibilityFrontend && operation->getCanMutateAuthoritativeState() && operation->getKind() != DomainRuntimeOperationKind::ActivateCompatibilityFrontend && operation->getKind() != DomainRuntimeOperationKind::DeactivateCompatibilityFrontend;
	}

	std::shared_ptr<RuntimeBoundaryAdmissionResult> RuntimeBoundaryAdmissionService::Deny(RuntimeBoundaryAdmissionDecision decision, const std::wstring &message, std::shared_ptr<DomainRuntimeAuthorityResult> authorityResult)
	{
		return RuntimeBoundaryAdmissionResult::Denied(decision, message, authorityResult);
	}
}
