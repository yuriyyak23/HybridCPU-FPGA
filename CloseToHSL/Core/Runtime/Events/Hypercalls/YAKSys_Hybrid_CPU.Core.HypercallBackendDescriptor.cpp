#include "YAKSys_Hybrid_CPU.Core.HypercallBackendDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	HypercallBackendDescriptor::HypercallBackendDescriptor() : HypercallBackendDescriptor(HypercallBackendAuthority::Runtime, CapabilityBoundaryRequirement::None, std::make_shared<EvidenceBoundaryRequirement>(EvidenceBoundaryRequirement::None), true, false)
	{
	}

	HypercallBackendDescriptor::HypercallBackendDescriptor(HypercallBackendAuthority authority, std::shared_ptr<CapabilityBoundaryRequirement> capabilityRequirement, std::shared_ptr<EvidenceBoundaryRequirement> evidenceRequirement, bool requiresValidatedDomain, bool neutralBackendOwnerMaterialized) : HypercallBackendDescriptor(authority, capabilityRequirement, evidenceRequirement, requiresValidatedDomain, neutralBackendOwnerMaterialized, nullptr)
	{
	}

	HypercallBackendDescriptor::HypercallBackendDescriptor(HypercallBackendAuthority authority, std::shared_ptr<CapabilityBoundaryRequirement> capabilityRequirement, std::shared_ptr<EvidenceBoundaryRequirement> evidenceRequirement, bool requiresValidatedDomain, bool neutralBackendOwnerMaterialized, std::shared_ptr<NeutralHypercallBackendOwnerDescriptor> neutralBackendOwner) : Authority(authority), CapabilityRequirement(capabilityRequirement), EvidenceRequirement(evidenceRequirement), RequiresValidatedDomain(requiresValidatedDomain), NeutralBackendOwnerMaterialized(neutralBackendOwnerMaterialized), NeutralBackendOwner(neutralBackendOwner) {}

	std::shared_ptr<HypercallBackendDescriptor> HypercallBackendDescriptor::RuntimeOwnedDesignFence(std::shared_ptr<CapabilityBoundaryRequirement> capabilityRequirement, std::shared_ptr<EvidenceBoundaryRequirement> evidenceRequirement)
	{
		return std::make_shared<HypercallBackendDescriptor>(HypercallBackendAuthority::Runtime, capabilityRequirement, evidenceRequirement, true, false);
	}

	std::shared_ptr<HypercallBackendDescriptor> HypercallBackendDescriptor::RuntimeOwnedDraftOwnerFence(std::shared_ptr<CapabilityBoundaryRequirement> capabilityRequirement, std::shared_ptr<EvidenceBoundaryRequirement> evidenceRequirement, std::shared_ptr<NeutralHypercallBackendOwnerDescriptor> neutralBackendOwner)
	{
		return std::make_shared<HypercallBackendDescriptor>(HypercallBackendAuthority::Runtime, capabilityRequirement, evidenceRequirement, true, true, neutralBackendOwner);
	}

	HypercallBackendAuthority HypercallBackendDescriptor::getAuthority() const
	{
		return Authority;
	}

	std::shared_ptr<CapabilityBoundaryRequirement> HypercallBackendDescriptor::getCapabilityRequirement() const
	{
		return CapabilityRequirement;
	}

	std::shared_ptr<EvidenceBoundaryRequirement> HypercallBackendDescriptor::getEvidenceRequirement() const
	{
		return EvidenceRequirement;
	}

	bool HypercallBackendDescriptor::getRequiresValidatedDomain() const
	{
		return RequiresValidatedDomain;
	}

	bool HypercallBackendDescriptor::getNeutralBackendOwnerMaterialized() const
	{
		return NeutralBackendOwnerMaterialized;
	}

	std::shared_ptr<NeutralHypercallBackendOwnerDescriptor> HypercallBackendDescriptor::getNeutralBackendOwner() const
	{
		return NeutralBackendOwner;
	}

	bool HypercallBackendDescriptor::isRuntimeAuthoritative() const
	{
		return getAuthority() == HypercallBackendAuthority::Runtime;
	}
}
