#pragma once

#include "../../../Governance/Virtualization/VirtualizationDecisionContractsV2.h"
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	struct AcceptedVirtualizationDecisionRegistryEntry final
	{
		const std::wstring OperationNamespace;
		const unsigned short NumericLeaf;
		const std::wstring OperationId;
		const std::shared_ptr<AcceptedVirtualizationDecision> Policy;

		AcceptedVirtualizationDecisionRegistryEntry(
			std::wstring operationNamespace,
			unsigned short numericLeaf,
			std::wstring operationId,
			std::shared_ptr<AcceptedVirtualizationDecision> policy)
			: OperationNamespace(std::move(operationNamespace)),
			  NumericLeaf(numericLeaf),
			  OperationId(std::move(operationId)),
			  Policy(std::move(policy))
		{
		}

		bool getRuntimeCapabilityGranted() const noexcept { return false; }
		bool getBackendExecutionAuthorized() const noexcept { return false; }
		bool getCompletionPublicationAuthorized() const noexcept { return false; }
		bool getRetirePublicationAuthorized() const noexcept { return false; }
	};

	/// <summary>
	/// Exact governance metadata for the attributable D2 decision. It cannot invoke,
	/// admit, complete or retire work.
	/// </summary>
	class Phase38AcceptedVirtualizationDecisionRegistry final
	{
	private:
		static std::shared_ptr<AcceptedVirtualizationDecisionRegistryEntry> Create();

	public:
		static std::shared_ptr<AcceptedVirtualizationDecisionRegistryEntry> getExactEntry();
		static bool TryResolvePolicy(const std::wstring &operationNamespace, unsigned short numericLeaf, std::shared_ptr<AcceptedVirtualizationDecisionRegistryEntry> &entry);
	};
}
