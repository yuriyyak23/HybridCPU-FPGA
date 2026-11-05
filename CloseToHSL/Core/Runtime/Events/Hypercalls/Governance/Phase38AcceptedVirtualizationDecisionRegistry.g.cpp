#include "Phase38AcceptedVirtualizationDecisionRegistry.g.h"
#include "Phase38VirtualizationDecisionSpecV2.h"
#include "Phase38VirtualizationDecisionAcceptanceV2.h"

namespace YAKSys_Hybrid_CPU::Core
{
	std::shared_ptr<AcceptedVirtualizationDecisionRegistryEntry> Phase38AcceptedVirtualizationDecisionRegistry::getExactEntry()
	{
		static const auto entry = Create();
		return entry;
	}

	bool Phase38AcceptedVirtualizationDecisionRegistry::TryResolvePolicy(
		const std::wstring &operationNamespace,
		unsigned short numericLeaf,
		std::shared_ptr<AcceptedVirtualizationDecisionRegistryEntry> &entry)
	{
		const auto exactEntry = getExactEntry();
		if (operationNamespace == exactEntry->OperationNamespace && numericLeaf == exactEntry->NumericLeaf)
		{
			entry = exactEntry;
			return true;
		}

		entry = nullptr;
		return false;
	}

	std::shared_ptr<AcceptedVirtualizationDecisionRegistryEntry> Phase38AcceptedVirtualizationDecisionRegistry::Create()
	{
		const auto spec = Phase38VirtualizationDecisionSpecV2::getInstance();
		const auto acceptance = Phase38VirtualizationDecisionAcceptanceV2::getRecord();
		const auto adjacentPolicy = std::make_shared<VirtualizationDecisionAdjacentPolicySnapshotV2>(
			spec->AdjacentLeafPolicy,
			spec->CrossNamespacePolicy);
		const auto policy = std::make_shared<AcceptedVirtualizationDecision>(
			spec->DecisionId,
			spec->SpecDigest,
			acceptance->AcceptanceDigest,
			acceptance->SpecCommitSha,
			spec->OperationNamespace,
			spec->NumericLeaf,
			spec->OwnerId,
			spec->OwnerPolicyVersion,
			spec->OwnerEpoch,
			spec->EffectClass,
			adjacentPolicy);

		return std::make_shared<AcceptedVirtualizationDecisionRegistryEntry>(
			spec->OperationNamespace,
			spec->NumericLeaf,
			spec->OperationId,
			policy);
	}
}
