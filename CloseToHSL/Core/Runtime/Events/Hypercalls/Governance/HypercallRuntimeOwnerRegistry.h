#pragma once

#include "Phase38VirtualizationDecisionAcceptanceV2.h"
#include "../../../Governance/Virtualization/VirtualizationDecisionValidatorV2.h"
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	struct HypercallRuntimeOwnerAllocation final
	{
		const unsigned long long OwnerId;
		const unsigned int OwnerPolicyVersion;
		const unsigned int OwnerEpoch;
		const std::wstring DecisionId;
		const std::wstring SpecDigest;
		const std::wstring OwnerRole;
		const bool RuntimeOwnerLoaded = false;
		const bool BackendExecutionAuthorized = false;

		HypercallRuntimeOwnerAllocation(unsigned long long ownerId, unsigned int ownerPolicyVersion,
			unsigned int ownerEpoch, std::wstring decisionId, std::wstring specDigest, std::wstring ownerRole)
			: OwnerId(ownerId), OwnerPolicyVersion(ownerPolicyVersion), OwnerEpoch(ownerEpoch),
			  DecisionId(std::move(decisionId)), SpecDigest(std::move(specDigest)), OwnerRole(std::move(ownerRole)) {}
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal sealed record HypercallRuntimeOwnerAllocation(ulong OwnerId, uint OwnerPolicyVersion, uint OwnerEpoch, string DecisionId, string SpecDigest, string OwnerRole)
//	{
//		internal bool RuntimeOwnerLoaded
//		{
//			get
//			{
//				return false;
//			}
//		}
//		internal bool BackendExecutionAuthorized
//		{
//			get
//			{
//				return false;
//			}
//		}
//	}

	/// <summary>
	/// Repository allocation registry only. It binds the accepted HCOWNR identity and
	/// exposes no resolution to an executable owner service.
	/// </summary>
	class HypercallRuntimeOwnerRegistry final
	{
	public:
		static std::shared_ptr<HypercallRuntimeOwnerAllocation> getPhase38ProbeOwner();

		static bool TryGetAllocation(unsigned long long ownerId, std::shared_ptr<HypercallRuntimeOwnerAllocation> &allocation);
	};

}
