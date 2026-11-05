#pragma once

#include "../../../Domains/Descriptors/EvidencePolicy/YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"
#include "../YAKSys_Hybrid_CPU.Core.Lane6VirtualToken.h"
#include <unordered_map>
#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute { struct DmaStreamComputeTokenHandle; }
using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;

namespace YAKSys_Hybrid_CPU::Core
{


	enum class Lane6HostEvidenceRestoreDecision : unsigned char
	{
		RebuildRequired = 0,
		Rebuilt = 1,
		RestoreRejected = 2
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct Lane6HostEvidenceRestoreResult(Lane6HostEvidenceRestoreDecision Decision, string Reason)
//	{
//		public bool RequiresRebuild
//		{
//			get
//			{
//				return Decision == Lane6HostEvidenceRestoreDecision.RebuildRequired;
//			}
//		}
//
//		public static Lane6HostEvidenceRestoreResult RebuildRequired
//		{
//			get;
//		} = new(Lane6HostEvidenceRestoreDecision.RebuildRequired, "Lane6 native token evidence was cleared for restore and must be rebuilt from host runtime state.");
//
//		public static Lane6HostEvidenceRestoreResult Rebuilt
//		{
//			get;
//		} = new(Lane6HostEvidenceRestoreDecision.Rebuilt, "Lane6 native token evidence was rebuilt from host runtime state.");
//
//		public static Lane6HostEvidenceRestoreResult Rejected(string reason)
//		{
//			return new(Lane6HostEvidenceRestoreDecision.RestoreRejected, reason);
//		}
//	}

	struct Lane6HostEvidenceRestoreResult {
 Lane6HostEvidenceRestoreDecision Decision;
 std::wstring Reason;
 bool RequiresRebuild() const noexcept { return Decision == Lane6HostEvidenceRestoreDecision::RebuildRequired; }
 static std::shared_ptr<Lane6HostEvidenceRestoreResult> RebuildRequired() { return std::make_shared<Lane6HostEvidenceRestoreResult>(Lane6HostEvidenceRestoreDecision::RebuildRequired, L"Lane6 native token evidence was cleared for restore and must be rebuilt from host runtime state."); }
 static std::shared_ptr<Lane6HostEvidenceRestoreResult> Rebuilt() { return std::make_shared<Lane6HostEvidenceRestoreResult>(Lane6HostEvidenceRestoreDecision::Rebuilt, L"Lane6 native token evidence was rebuilt from host runtime state."); }
 static std::shared_ptr<Lane6HostEvidenceRestoreResult> Rejected(std::wstring reason) { return std::make_shared<Lane6HostEvidenceRestoreResult>(Lane6HostEvidenceRestoreDecision::RestoreRejected, std::move(reason)); }
};
class Lane6HostOwnedEvidenceStore final : public std::enable_shared_from_this<Lane6HostOwnedEvidenceStore>
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<Lane6VirtualToken, DmaStreamComputeTokenHandle> _nativeTokenBindings = new();
		std::unordered_map<Lane6VirtualToken, std::shared_ptr<DmaStreamComputeTokenHandle>, Lane6VirtualTokenHash> _nativeTokenBindings;

	public:
		int getActiveBindingCount() const;

		bool TryBind(std::shared_ptr<Lane6VirtualToken> virtualToken, std::shared_ptr<DmaStreamComputeTokenHandle> hostHandle);

		bool TryResolve(std::shared_ptr<Lane6VirtualToken> virtualToken, std::shared_ptr<DmaStreamComputeTokenHandle> &hostHandle);

		std::shared_ptr<Lane6HostEvidenceRestoreResult> PrepareForRestore(std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy);

		std::shared_ptr<Lane6HostEvidenceRestoreResult> RebuildAfterRestore(std::shared_ptr<Lane6VirtualToken> virtualToken, std::shared_ptr<DmaStreamComputeTokenHandle> hostHandle);
	};

}
