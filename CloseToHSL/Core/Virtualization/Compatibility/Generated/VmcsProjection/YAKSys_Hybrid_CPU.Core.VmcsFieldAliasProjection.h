#pragma once

#include "../../../../Runtime/Domains/Descriptors/EvidencePolicy/YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"
#include "VmcsFieldAliasProjection.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class VmcsFieldAliasProjection final : public std::enable_shared_from_this<VmcsFieldAliasProjection>
	{
	public:
		std::shared_ptr<VmcsFieldAliasResult> ValidateAccess(std::shared_ptr<VmcsFieldAliasRequest> request, std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy);
	};
}
