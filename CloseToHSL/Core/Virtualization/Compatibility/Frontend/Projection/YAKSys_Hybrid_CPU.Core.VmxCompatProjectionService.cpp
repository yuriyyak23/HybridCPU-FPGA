#include "YAKSys_Hybrid_CPU.Core.VmxCompatProjectionService.h"
#include "VmxCompatProjectionService.h"

namespace YAKSys_Hybrid_CPU::Core
{

	VmxCompatProjectionService::VmxCompatProjectionService() : VmxCompatProjectionService(std::make_shared<CompatAliasMap>())
	{
	}

	VmxCompatProjectionService::VmxCompatProjectionService(std::shared_ptr<CompatAliasMap> aliasMap) : _aliasMap(aliasMap)
	{
	}

	std::shared_ptr<VmxCompatProjectionResult> VmxCompatProjectionService::ValidateProjection(std::shared_ptr<VmxCompatProjectionRequest> request)
	{
		std::shared_ptr<CompatAliasMapEntry> alias;
		if (!_aliasMap->TryGetEntry(request->SourceKind, request->SourceName, alias))
		{
			return VmxCompatProjectionResult::Denied(VmxCompatProjectionDecision::UnknownAlias, L"Compatibility projection source is not declared in the alias map.");
		}

		if (!alias->IsFrozenAbi)
		{
			return VmxCompatProjectionResult::Denied(VmxCompatProjectionDecision::UnfrozenAbiAlias, L"Compatibility projection source is not part of the frozen ABI.");
		}

		if (!request->DescriptorValidated)
		{
			return VmxCompatProjectionResult::Denied(VmxCompatProjectionDecision::DescriptorValidationDenied, L"Compatibility projection requires descriptor validation.");
		}

		if (!request->EvidenceValidated)
		{
			return VmxCompatProjectionResult::Denied(VmxCompatProjectionDecision::EvidenceValidationDenied, L"Compatibility projection requires evidence validation.");
		}

		if (request->AttemptsAuthoritativeMutation)
		{
			return VmxCompatProjectionResult::Denied(VmxCompatProjectionDecision::AuthoritativeMutationDenied, L"Compatibility projection cannot mutate authoritative substrate state directly.");
		}

		return VmxCompatProjectionResult::Allowed(alias);
	}
}
