#pragma once

#include "YAKSys_Hybrid_CPU.Core.RuntimeCapabilityIds.h"
#include "../../Governance/Virtualization/VirtualizationDecisionContractsV2.h"
#include "../../Governance/Virtualization/VirtualizationDecisionValidatorV2.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	struct VirtualizationCapabilityAllocation final
	{
		const unsigned long long CapabilityMask;
		const unsigned int AllocationPolicyVersion;
		const unsigned long long AllocationGeneration;
		const std::wstring DecisionId;
		const VirtualizationDelegationPolicyV2 DelegationPolicy;
		const VirtualizationRevocationPolicyV2 RevocationPolicy;
		const VirtualizationCapabilityMigrationClassV2 MigrationClass;
		const VirtualizationEvidenceVisibilityV2 EvidenceVisibility;
		const VirtualizationProjectionPolicyV2 ProjectionPolicy;
		VirtualizationCapabilityAllocation(unsigned long long capabilityMask, unsigned int allocationPolicyVersion, unsigned long long allocationGeneration, std::wstring decisionId, VirtualizationDelegationPolicyV2 delegationPolicy, VirtualizationRevocationPolicyV2 revocationPolicy, VirtualizationCapabilityMigrationClassV2 migrationClass, VirtualizationEvidenceVisibilityV2 evidenceVisibility, VirtualizationProjectionPolicyV2 projectionPolicy)
			: CapabilityMask(capabilityMask), AllocationPolicyVersion(allocationPolicyVersion), AllocationGeneration(allocationGeneration), DecisionId(std::move(decisionId)), DelegationPolicy(delegationPolicy), RevocationPolicy(revocationPolicy), MigrationClass(migrationClass), EvidenceVisibility(evidenceVisibility), ProjectionPolicy(projectionPolicy) {}
		bool isGrant() const noexcept { return false; }
		bool getRuntimeCapabilityGranted() const noexcept { return false; }
	};


// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal sealed record VirtualizationCapabilityAllocation(ulong CapabilityMask, uint AllocationPolicyVersion, ulong AllocationGeneration, string DecisionId, VirtualizationDelegationPolicyV2 DelegationPolicy, VirtualizationRevocationPolicyV2 RevocationPolicy, VirtualizationCapabilityMigrationClassV2 MigrationClass, VirtualizationEvidenceVisibilityV2 EvidenceVisibility, VirtualizationProjectionPolicyV2 ProjectionPolicy)
//	{
//		internal bool IsGrant
//		{
//			get
//			{
//				return false;
//			}
//		}
//		internal bool RuntimeCapabilityGranted
//		{
//			get
//			{
//				return false;
//			}
//		}
//	}

	/// <summary>
	/// Stable capability-number allocation metadata. It creates no CapabilityGrant
	/// and is not loaded into a CapabilityDescriptorSet.
	/// </summary>
	class VirtualizationCapabilityAllocationRegistry final
	{
	private:
		static inline const std::shared_ptr<VirtualizationCapabilityAllocation> Phase38Probe = std::make_shared<VirtualizationCapabilityAllocation>(RuntimeCapabilityIds::VmCallProbeNoStateV1Mask, 1, 1, VirtualizationDecisionValidatorV2::ExpectedDecisionId, VirtualizationDelegationPolicyV2::NonDelegable, VirtualizationRevocationPolicyV2::RuntimeRevocable, VirtualizationCapabilityMigrationClassV2::DomainLocal, VirtualizationEvidenceVisibilityV2::HostOnly, VirtualizationProjectionPolicyV2::NeverProject);

	public:
		static std::shared_ptr<VirtualizationCapabilityAllocation> getPhase38Probe();
	};

}
