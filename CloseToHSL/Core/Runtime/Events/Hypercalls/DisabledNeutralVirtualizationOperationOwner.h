#pragma once

#include <memory>
#include <string>
#include "VirtualizationOperationDecisionValidationResult.h"

namespace YAKSys_Hybrid_CPU::Core
{

	enum class NeutralVirtualizationOperationOwnerDecision : unsigned char
	{
		DeniedOwnerInterfaceDisabled = 0,
		DeniedDecisionArtifactMissing = 1
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct NeutralVirtualizationOperationOwnerRequest(string OperationName, VirtualizationOperationDecisionValidationResult DecisionValidation);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct NeutralVirtualizationOperationOwnerResult(NeutralVirtualizationOperationOwnerDecision Decision, string Reason)
//	{
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
	/// Governance-facing placeholder only. It deliberately exposes no execution method.
	/// </summary>
	struct NeutralVirtualizationOperationOwnerRequest final {
    std::wstring OperationName;
    VirtualizationOperationDecisionValidationResult DecisionValidation;
    bool operator==(const NeutralVirtualizationOperationOwnerRequest&) const = default;
};
struct NeutralVirtualizationOperationOwnerResult final {
    NeutralVirtualizationOperationOwnerDecision Decision;
    std::wstring Reason;
    bool BackendExecutionAuthorized() const noexcept { return false; }
    bool CompletionPublicationAuthorized() const noexcept { return false; }
    bool RetirePublicationAuthorized() const noexcept { return false; }
    bool operator==(const NeutralVirtualizationOperationOwnerResult&) const = default;
};

	class INeutralVirtualizationOperationOwner
	{
	public:
		virtual std::shared_ptr<NeutralVirtualizationOperationOwnerResult> Resolve(std::shared_ptr<NeutralVirtualizationOperationOwnerRequest> request) = 0;
	};

	class DisabledNeutralVirtualizationOperationOwner final : public INeutralVirtualizationOperationOwner
	{
	private:
		static const std::shared_ptr<DisabledNeutralVirtualizationOperationOwner> Instance;

	public:
		static std::shared_ptr<DisabledNeutralVirtualizationOperationOwner> getInstance();

	private:
		DisabledNeutralVirtualizationOperationOwner();

	public:
		std::shared_ptr<NeutralVirtualizationOperationOwnerResult> Resolve(std::shared_ptr<NeutralVirtualizationOperationOwnerRequest> request) override;
	};

}
