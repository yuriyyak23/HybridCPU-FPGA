#include "YAKSys_Hybrid_CPU.Core.VmcsV2DescriptorProjection.h"
#include "VmcsV2DescriptorProjection.h"

using namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2;

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<VmcsV2DescriptorProjectionResult> VmcsV2DescriptorProjection::ValidateProjection(std::shared_ptr<VmcsV2DescriptorProjectionRequest> request)
	{
		if (!request->GeneratedAliasMapDeclared)
		{
			return VmcsV2DescriptorProjectionResult::Denied(VmcsV2DescriptorProjectionDecision::MissingGeneratedAliasMap, L"VMCSv2 descriptor projection requires a generated alias map.");
		}

		if (!request->DescriptorValidated)
		{
			return VmcsV2DescriptorProjectionResult::Denied(VmcsV2DescriptorProjectionDecision::DescriptorValidationDenied, L"VMCSv2 descriptor projection requires descriptor validation.");
		}

		if (!request->EvidenceValidated)
		{
			return VmcsV2DescriptorProjectionResult::Denied(VmcsV2DescriptorProjectionDecision::EvidenceValidationDenied, L"VMCSv2 descriptor projection requires evidence validation.");
		}

		if (!request->ReadOnlyProjection)
		{
			return VmcsV2DescriptorProjectionResult::Denied(VmcsV2DescriptorProjectionDecision::WritableProjectionDenied, L"VMCSv2 descriptor projection must be read-only.");
		}

		if (request->AttemptsAuthoritativeMutation)
		{
			return VmcsV2DescriptorProjectionResult::Denied(VmcsV2DescriptorProjectionDecision::AuthoritativeMutationDenied, L"VMCSv2 descriptor projection cannot own or mutate authoritative substrate state.");
		}

		return VmcsV2DescriptorProjectionResult::Allowed();
	}
}
