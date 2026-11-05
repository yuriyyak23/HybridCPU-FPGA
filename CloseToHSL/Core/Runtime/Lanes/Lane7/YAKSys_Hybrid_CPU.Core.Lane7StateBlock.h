#pragma once

#include "HostOwnedEvidence/Lane7HostOwnedEvidenceStore.h"
#include "Accelerators/Lane7AcceleratorHandleNamespace.h"
#include "Accelerators/Lane7AcceleratorTokenNamespace.h"
#include "Accelerators/Lane7AcceleratorCompletionRouter.h"
#include "Accelerators/Lane7AcceleratorAdmissionPolicy.h"
#include "YAKSys_Hybrid_CPU.Core.Lane7Checkpoint.h"
#include "../../../Execution/ExternalAccelerators/Descriptors/AcceleratorCommandDescriptor.h"
#include "Lane7StateBlock.h"
#include "../../../Execution/ExternalAccelerators/Tokens/AcceleratorToken.h"
#include "../../Domains/Descriptors/EvidencePolicy/YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"
#include <unordered_map>
#include <map>
#include <tuple>
#include <vector>
#include <limits>
#include <optional>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core
{
	class Lane7StateBlock final : public std::enable_shared_from_this<Lane7StateBlock>
	{
	private:
		bool VirtualizationEnabled = false;
		unsigned short ExecutionDomainTag = 0;
		unsigned short AddressSpaceTag = 0;
		std::shared_ptr<Lane7QuotaPolicy> QuotaPolicy = std::make_shared<Lane7QuotaPolicy>(Lane7QuotaPolicy::Default());
		unsigned long long OwnershipEpoch = 0;
		unsigned long long PolicyEpoch = 0;
		unsigned long long TokenEpoch = 0;
		unsigned long long CompletionEpoch = 0;
		std::shared_ptr<Lane7Fault> LastFault = Lane7Fault::None;
		const std::shared_ptr<Lane7HostOwnedEvidenceStore> HostEvidence = std::make_shared<Lane7HostOwnedEvidenceStore>();
		mutable std::shared_ptr<Lane7AcceleratorHandleNamespace> HandleNamespace;
		mutable std::shared_ptr<Lane7AcceleratorTokenNamespace> TokenNamespace;
		mutable std::shared_ptr<Lane7AcceleratorCompletionRouter> CompletionRouter;
		mutable std::shared_ptr<Lane7AcceleratorAdmissionPolicy> AdmissionPolicy;

	public:
		std::shared_ptr<Lane7Checkpoint> CreateCheckpoint();

		void RestoreCheckpoint(std::shared_ptr<Lane7Checkpoint> checkpoint);


	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<ulong, Lane7VirtualHandle> _handlesByValue = new();
		std::unordered_map<unsigned long long, std::shared_ptr<Lane7VirtualHandle>> _handlesByValue = std::unordered_map<unsigned long long, std::shared_ptr<Lane7VirtualHandle>>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<(ushort ExecutionDomainTag, ushort OwnerVirtualThreadId, AcceleratorDeviceId AcceleratorId), ulong> _handleByOwner = new();
		std::map<std::tuple<unsigned short, unsigned short, AcceleratorDeviceId>, unsigned long long> _handleByOwner;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<ulong, Lane7VirtualToken> _tokensByVirtualValue = new();
		std::unordered_map<unsigned long long, std::shared_ptr<Lane7VirtualToken>> _tokensByVirtualValue = std::unordered_map<unsigned long long, std::shared_ptr<Lane7VirtualToken>>();

		unsigned long long _nextVirtualHandle = 0x7000'0000'0000'0001ULL;
		unsigned long long _nextVirtualToken = 0x7100'0000'0000'0001ULL;

	public:
		Lane7StateBlock();

		bool getVirtualizationEnabled() const;
	private:
		void setVirtualizationEnabled(bool value);

	public:
		unsigned short getExecutionDomainTag() const;
	private:
		void setExecutionDomainTag(unsigned short value);

	public:
		unsigned short getAddressSpaceTag() const;
	private:
		void setAddressSpaceTag(unsigned short value);

	public:
		std::shared_ptr<Lane7QuotaPolicy> getQuotaPolicy() const;
	private:
		void setQuotaPolicy(std::shared_ptr<Lane7QuotaPolicy> value);

	public:
		unsigned long long getOwnershipEpoch() const;
	private:
		void setOwnershipEpoch(unsigned long long value);

	public:
		unsigned long long getPolicyEpoch() const;
	private:
		void setPolicyEpoch(unsigned long long value);

	public:
		unsigned long long getTokenEpoch() const;
	private:
		void setTokenEpoch(unsigned long long value);

	public:
		unsigned long long getCompletionEpoch() const;
	private:
		void setCompletionEpoch(unsigned long long value);

	public:
		unsigned long long getBackendBindingEpoch() const;

		unsigned long long getPressureEpoch() const;

		std::shared_ptr<Lane7Fault> getLastFault() const;
	private:
		void setLastFault(std::shared_ptr<Lane7Fault> value);

	public:
		std::shared_ptr<Lane7PressureSnapshot> getLastPressure() const;

		std::shared_ptr<Lane7HostOwnedEvidenceStore> getHostEvidence() const;

		std::shared_ptr<Lane7AcceleratorHandleNamespace> getHandleNamespace() const;

		std::shared_ptr<Lane7AcceleratorTokenNamespace> getTokenNamespace() const;

		std::shared_ptr<Lane7AcceleratorCompletionRouter> getCompletionRouter() const;

		std::shared_ptr<Lane7AcceleratorAdmissionPolicy> getAdmissionPolicy() const;

		int getActiveHandleCount() const;

		int getActiveVirtualTokenCount() const;

		unsigned long long ConfigureOwnership(unsigned short executionDomainTag, unsigned short addressSpaceTag, bool enabled = true, std::optional<std::shared_ptr<Lane7QuotaPolicy>> quotaPolicy = std::nullopt);

		std::shared_ptr<Lane7VirtualHandle> AllocateVirtualHandle(unsigned short ownerVirtualThreadId, AcceleratorDeviceId acceleratorId, Lane7VirtualCapability capabilities);

		bool TryAllocateVirtualHandle(unsigned short ownerVirtualThreadId, AcceleratorDeviceId acceleratorId, Lane7VirtualCapability capabilities, std::shared_ptr<Lane7VirtualHandle> &handle, std::shared_ptr<Lane7Fault> &fault);

		bool TryReleaseVirtualHandle(unsigned long long virtualHandle);

		bool TryGetVirtualHandle(unsigned long long virtualHandle, std::shared_ptr<Lane7VirtualHandle> &handle);

		bool TryFindVirtualHandle(unsigned short executionDomainTag, unsigned short ownerVirtualThreadId, AcceleratorDeviceId acceleratorId, std::shared_ptr<Lane7VirtualHandle> &handle);

		bool TryBindBackend(unsigned long long virtualHandle, unsigned long long backendGeneration, std::shared_ptr<Lane7BackendBinding> &binding, bool available = true, bool requiresRebind = false);

		void DiscardBackendBindings();

		bool TryRebuildBackendBinding(unsigned long long virtualHandle, unsigned long long backendGeneration = 0);

		bool MarkBackendUnavailable(unsigned long long virtualHandle);

		bool MarkBackendRebindRequired(unsigned long long virtualHandle);

		bool TryValidateGuestSubmit(std::shared_ptr<AcceleratorCommandDescriptor> descriptor, unsigned short executionDomainTag, unsigned short addressSpaceTag, std::shared_ptr<Lane7VirtualHandle> &handle, std::shared_ptr<Lane7Fault> &fault);

		bool TryMapHostToken(std::shared_ptr<Lane7VirtualHandle> handle, unsigned short addressSpaceTag, std::shared_ptr<AcceleratorToken> token, std::shared_ptr<Lane7VirtualToken> &virtualToken, std::shared_ptr<Lane7Fault> &fault);

		bool TryResolveHostToken(unsigned long long virtualTokenId, unsigned short executionDomainTag, unsigned short addressSpaceTag, unsigned short ownerVirtualThreadId, std::shared_ptr<AcceleratorTokenHandle> &hostHandle, std::shared_ptr<Lane7VirtualToken> &virtualToken, std::shared_ptr<Lane7Fault> &fault);

		bool TryResolveVirtualTokenForHost(std::shared_ptr<AcceleratorTokenHandle> hostHandle, std::shared_ptr<Lane7VirtualToken> &virtualToken);

		bool UpdateVirtualTokenStatus(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<Lane7VirtualToken> &virtualToken);

		bool ReleaseVirtualToken(unsigned long long virtualTokenId);

		bool TryBuildCompletion(std::shared_ptr<AcceleratorToken> token, unsigned short sourceOpcode, unsigned long long runtimeQueueSequence, std::shared_ptr<LaneCompletionDescriptor> &completion, std::shared_ptr<Lane7Fault> &fault);

		unsigned long long BuildFilteredCapabilityWord(std::shared_ptr<Lane7VirtualHandle> handle);

		std::shared_ptr<Lane7PressureSnapshot> ObserveSubmitPollPressure(unsigned short ownerVirtualThreadId);

		void ResetPressureWindow();

		std::shared_ptr<Lane7HostEvidenceRestoreResult> PrepareHostEvidenceForRestore(std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy);

		std::shared_ptr<Lane7HostEvidenceRestoreResult> RebuildHostTokenAfterRestore(std::shared_ptr<Lane7VirtualToken> virtualToken, std::shared_ptr<AcceleratorTokenHandle> hostHandle);

		std::shared_ptr<Lane7HostEvidenceRestoreResult> RebuildBackendBindingAfterRestore(unsigned long long virtualHandle, unsigned long long backendGeneration, std::shared_ptr<Lane7BackendBinding> &binding);

	private:
		bool TryValidateBackendUsable(unsigned long long virtualHandle, std::shared_ptr<Lane7Fault> &fault);

		bool ValidateEnabled(std::shared_ptr<Lane7Fault> &fault);

		bool ValidateOwnership(unsigned short executionDomainTag, unsigned short addressSpaceTag, unsigned short ownerVirtualThreadId, std::shared_ptr<Lane7Fault> &fault);

		std::shared_ptr<Lane7PressureSnapshot> ObservePressure(unsigned short ownerVirtualThreadId, bool quotaExceeded);

		std::shared_ptr<Lane7Fault> RecordFault(std::shared_ptr<Lane7Fault> fault);

		unsigned long long AllocateVirtualHandleValue();

		unsigned long long AllocateVirtualTokenValue();

		unsigned long long AdvanceOwnershipEpoch();

		unsigned long long AdvanceTokenEpoch();

		unsigned long long AdvanceCompletionEpoch();

	};
}
