#pragma once

#include "../../../Domains/Descriptors/EvidencePolicy/YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"
#include "../../../Evidence/HostOwned/YAKSys_Hybrid_CPU.Core.HostOwnedEvidenceBoundary.h"
#include "../../../Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h"
#include "../Lane7EvidenceTypes.h"
#include "../YAKSys_Hybrid_CPU.Core.Lane7VirtualToken.h"
#include <unordered_map>
#include <string>
#include <limits>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core
{

	enum class Lane7HostEvidenceRestoreDecision : unsigned char
	{
		RebuildRequired = 0,
		Rebuilt = 1,
		RestoreRejected = 2
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct Lane7HostEvidenceRestoreResult(Lane7HostEvidenceRestoreDecision Decision, string Reason)
//	{
//		public bool RequiresRebuild
//		{
//			get
//			{
//				return Decision == Lane7HostEvidenceRestoreDecision.RebuildRequired;
//			}
//		}
//
//		public static Lane7HostEvidenceRestoreResult RebuildRequired
//		{
//			get;
//		} = new(Lane7HostEvidenceRestoreDecision.RebuildRequired, "Lane7 host-owned token, backend binding, and scheduler evidence was cleared for restore and must be rebuilt from host runtime state.");
//
//		public static Lane7HostEvidenceRestoreResult Rebuilt
//		{
//			get;
//		} = new(Lane7HostEvidenceRestoreDecision.Rebuilt, "Lane7 host-owned evidence was rebuilt from fresh host runtime state.");
//
//		public static Lane7HostEvidenceRestoreResult Rejected(string reason)
//		{
//			return new(Lane7HostEvidenceRestoreDecision.RestoreRejected, reason);
//		}
//	}

	struct Lane7HostEvidenceRestoreResult final {
 Lane7HostEvidenceRestoreDecision Decision; std::wstring Reason;
 bool RequiresRebuild() const { return Decision == Lane7HostEvidenceRestoreDecision::RebuildRequired; }
 static std::shared_ptr<Lane7HostEvidenceRestoreResult> RebuildRequired() { return std::make_shared<Lane7HostEvidenceRestoreResult>(Lane7HostEvidenceRestoreDecision::RebuildRequired, L"Lane7 host-owned token, backend binding, and scheduler evidence was cleared for restore and must be rebuilt from host runtime state."); }
 static std::shared_ptr<Lane7HostEvidenceRestoreResult> Rebuilt() { return std::make_shared<Lane7HostEvidenceRestoreResult>(Lane7HostEvidenceRestoreDecision::Rebuilt, L"Lane7 host-owned evidence was rebuilt from fresh host runtime state."); }
 static std::shared_ptr<Lane7HostEvidenceRestoreResult> Rejected(std::wstring reason) { return std::make_shared<Lane7HostEvidenceRestoreResult>(Lane7HostEvidenceRestoreDecision::RestoreRejected, std::move(reason)); }
};
class Lane7HostOwnedEvidenceStore final : public std::enable_shared_from_this<Lane7HostOwnedEvidenceStore>
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<ulong, Lane7BackendBinding> _backendBindings = new();
		std::unordered_map<unsigned long long, std::shared_ptr<Lane7BackendBinding>> _backendBindings = std::unordered_map<unsigned long long, std::shared_ptr<Lane7BackendBinding>>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<ulong, AcceleratorTokenHandle> _nativeTokenByVirtualToken = new();
		std::unordered_map<unsigned long long, std::shared_ptr<AcceleratorTokenHandle>> _nativeTokenByVirtualToken = std::unordered_map<unsigned long long, std::shared_ptr<AcceleratorTokenHandle>>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<ulong, ulong> _virtualTokenByNativeHandle = new();
		std::unordered_map<unsigned long long, unsigned long long> _virtualTokenByNativeHandle = std::unordered_map<unsigned long long, unsigned long long>();
		int _submitPollCount = 0;
		std::shared_ptr<Lane7PressureSnapshot> _lastPressure;
		unsigned long long _backendBindingEpoch = 0;
		unsigned long long _pressureEpoch = 0;

	public:
		int getActiveBackendBindingCount() const;

		int getActiveTokenBindingCount() const;

		unsigned long long getBackendBindingEpoch() const;

		unsigned long long getPressureEpoch() const;

		std::shared_ptr<Lane7PressureSnapshot> getLastPressure() const;

		bool TryBindToken(std::shared_ptr<Lane7VirtualToken> virtualToken, std::shared_ptr<AcceleratorTokenHandle> hostHandle);

		bool TryResolveHostToken(unsigned long long virtualTokenId, std::shared_ptr<AcceleratorTokenHandle> &hostHandle);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public bool TryResolveVirtualTokenForHost(AcceleratorTokenHandle hostHandle, IReadOnlyDictionary<ulong, Lane7VirtualToken> virtualTokens, out Lane7VirtualToken virtualToken)
		bool TryResolveVirtualTokenForHost(std::shared_ptr<AcceleratorTokenHandle> hostHandle, const std::unordered_map<unsigned long long, std::shared_ptr<Lane7VirtualToken>> &virtualTokens, std::shared_ptr<Lane7VirtualToken> &virtualToken);

		bool ReleaseToken(unsigned long long virtualTokenId);

		void ClearTokenBindings();

		bool TryBindBackend(unsigned short executionDomainTag, unsigned short ownerVirtualThreadId, unsigned long long virtualHandle, unsigned long long backendGeneration, bool available, bool requiresRebind, std::shared_ptr<Lane7BackendBinding> &binding);

		bool TryResolveBackendBinding(unsigned long long virtualHandle, std::shared_ptr<Lane7BackendBinding> &binding);

		bool RemoveBackendBinding(unsigned long long virtualHandle);

		bool DiscardBackendBindings();

		void ClearBackendBindings();

		std::shared_ptr<Lane7PressureSnapshot> ObserveSubmitPollPressure(unsigned short executionDomainTag, unsigned short addressSpaceTag, unsigned short ownerVirtualThreadId, int inflightTokens, std::shared_ptr<Lane7QuotaPolicy> quotaPolicy);

		std::shared_ptr<Lane7PressureSnapshot> ObserveSchedulerPressure(unsigned short executionDomainTag, unsigned short addressSpaceTag, unsigned short ownerVirtualThreadId, int inflightTokens, std::shared_ptr<Lane7QuotaPolicy> quotaPolicy, bool quotaExceeded);

		void ResetSchedulerWindow();

		void ResetSchedulerEvidence();

		std::shared_ptr<Lane7HostEvidenceRestoreResult> PrepareForRestore(std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy);

		std::shared_ptr<Lane7HostEvidenceRestoreResult> RebuildTokenAfterRestore(std::shared_ptr<Lane7VirtualToken> virtualToken, std::shared_ptr<AcceleratorTokenHandle> hostHandle);

		std::shared_ptr<Lane7HostEvidenceRestoreResult> RebuildBackendAfterRestore(unsigned short executionDomainTag, unsigned short ownerVirtualThreadId, unsigned long long virtualHandle, unsigned long long backendGeneration, std::shared_ptr<Lane7BackendBinding> &binding);

	private:
		static bool RequiresRecompute(std::shared_ptr<HostOwnedEvidenceBoundary> boundary, std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy, EvidenceVisibilityClass evidenceClass);

		bool TryAdvanceBackendBindingEpoch(unsigned long long &advancedEpoch);

		bool TryAdvancePressureEpoch(unsigned long long &advancedEpoch);

		static bool TryAdvanceEpoch(unsigned long long &epoch, unsigned long long &advancedEpoch);
	};

}
