#pragma once
#include "VirtualizationOperationDecisionTypes.h"
#include "VirtualizationOperationDecisionValidationResult.h"

#include <string>
#include <vector>
#include <any>
#include <memory>
#include "../../../../../tangible_string_helper.h"

namespace YAKSys_Hybrid_CPU::Core
{





// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct VirtualizationOperationDecisionManifest(uint SchemaVersion, string DecisionId, VirtualizationOperationDecisionState State, string OperationName, string DecisionOwner, NeutralHypercallBackendOwnerSource OwnerSource, string AcceptedCommitSha, IReadOnlyList<string> RequiredReviewers, IReadOnlyList<ulong> ExactNumericLeaves, string ValueSource, string CapabilityPolicy, string EvidenceClass, string MigrationClass, string DenialReason);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct VirtualizationDecisionAttributionEvidence(string DecisionOwner, string AcceptedCommitSha, bool CodeOwnersRulePresent, bool CodeOwnersRuleMatched, bool RequiredReviewersApproved, IReadOnlyList<string> ApprovedReviewers, bool CompatibilityFrontendSelfApproved);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct VirtualizationOperationDecisionValidationResult(VirtualizationOperationDecisionValidationDecision Decision, string Reason)
//	{
//		internal bool IsStructurallyValidGovernanceEvidence
//		{
//			get
//			{
//				return Decision == VirtualizationOperationDecisionValidationDecision.ValidGovernanceArtifactOnly;
//			}
//		}
//
//		internal bool BackendExecutionAuthorized
//		{
//			get
//			{
//				return false;
//			}
//		}
//		internal bool CompletionPublicationAuthorized
//		{
//			get
//			{
//				return false;
//			}
//		}
//		internal bool RetirePublicationAuthorized
//		{
//			get
//			{
//				return false;
//			}
//		}
//	}

	/// <summary>
	/// Validates the shape and repository attribution of a future D2 decision artifact.
	/// A valid result is governance evidence only and never runtime execution authority.
	/// </summary>
	class VirtualizationOperationDecisionManifestValidator final
	{
	public:
		static constexpr unsigned int CurrentSchemaVersion = 1;

		static std::shared_ptr<VirtualizationOperationDecisionValidationResult> Validate(std::shared_ptr<VirtualizationOperationDecisionManifest> manifest, std::shared_ptr<VirtualizationDecisionAttributionEvidence> attribution);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool IsCommitSha(string? value)
		static bool IsCommitSha(const std::wstring &value);

		static std::shared_ptr<VirtualizationOperationDecisionValidationResult> Deny(VirtualizationOperationDecisionValidationDecision decision, const std::wstring &reason);
	};

}
