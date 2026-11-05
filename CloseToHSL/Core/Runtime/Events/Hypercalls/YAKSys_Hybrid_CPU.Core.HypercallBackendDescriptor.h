#pragma once

#include "HypercallBackendAdmissionPolicy.h"
#include <optional>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class HypercallBackendDescriptor final : public std::enable_shared_from_this<HypercallBackendDescriptor>
	{
	private:
		const HypercallBackendAuthority Authority;
		const std::shared_ptr<CapabilityBoundaryRequirement> CapabilityRequirement;
		const std::shared_ptr<EvidenceBoundaryRequirement> EvidenceRequirement;
		const bool RequiresValidatedDomain;
		const bool NeutralBackendOwnerMaterialized;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<NeutralHypercallBackendOwnerDescriptor> NeutralBackendOwner;
		std::shared_ptr<NeutralHypercallBackendOwnerDescriptor> NeutralBackendOwner;

	public:
		HypercallBackendDescriptor();

		HypercallBackendDescriptor(HypercallBackendAuthority authority, std::shared_ptr<CapabilityBoundaryRequirement> capabilityRequirement, std::shared_ptr<EvidenceBoundaryRequirement> evidenceRequirement, bool requiresValidatedDomain, bool neutralBackendOwnerMaterialized);

		HypercallBackendDescriptor(HypercallBackendAuthority authority, std::shared_ptr<CapabilityBoundaryRequirement> capabilityRequirement, std::shared_ptr<EvidenceBoundaryRequirement> evidenceRequirement, bool requiresValidatedDomain, bool neutralBackendOwnerMaterialized, std::shared_ptr<NeutralHypercallBackendOwnerDescriptor> neutralBackendOwner);

		static std::shared_ptr<HypercallBackendDescriptor> RuntimeOwnedDesignFence(std::shared_ptr<CapabilityBoundaryRequirement> capabilityRequirement, std::shared_ptr<EvidenceBoundaryRequirement> evidenceRequirement);

		static std::shared_ptr<HypercallBackendDescriptor> RuntimeOwnedDraftOwnerFence(std::shared_ptr<CapabilityBoundaryRequirement> capabilityRequirement, std::shared_ptr<EvidenceBoundaryRequirement> evidenceRequirement, std::shared_ptr<NeutralHypercallBackendOwnerDescriptor> neutralBackendOwner);

		HypercallBackendAuthority getAuthority() const;

		std::shared_ptr<CapabilityBoundaryRequirement> getCapabilityRequirement() const;

		std::shared_ptr<EvidenceBoundaryRequirement> getEvidenceRequirement() const;

		bool getRequiresValidatedDomain() const;

		bool getNeutralBackendOwnerMaterialized() const;

		std::shared_ptr<NeutralHypercallBackendOwnerDescriptor> getNeutralBackendOwner() const;

		bool isRuntimeAuthoritative() const;
	};
}
