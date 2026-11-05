#include "YAKSys_Hybrid_CPU.Core.Lane7StateBlock.h"
#include "../../../Execution/ExternalAccelerators/Tokens/AcceleratorTokenStatusWord.h"
#include "../../Completion/Routing/LaneCompletionRouting.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<Lane7Checkpoint> Lane7StateBlock::CreateCheckpoint()
	{
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# range operator:
		std::vector<std::shared_ptr<Lane7VirtualHandle>> handles;
        std::vector<std::shared_ptr<Lane7VirtualToken>> tokens;
        for (const auto& [key, value] : _handlesByValue) handles.push_back(std::make_shared<Lane7VirtualHandle>(*value));
        for (const auto& [key, value] : _tokensByVirtualValue) tokens.push_back(std::make_shared<Lane7VirtualToken>(*value));
        return std::make_shared<Lane7Checkpoint>(getExecutionDomainTag(), getAddressSpaceTag(), getVirtualizationEnabled(), handles, tokens, getOwnershipEpoch(), getTokenEpoch(), getCompletionEpoch());
	}

	void Lane7StateBlock::RestoreCheckpoint(std::shared_ptr<Lane7Checkpoint> checkpoint)
	{
		if (!checkpoint) throw std::invalid_argument("checkpoint");

		_handlesByValue.clear();
		_handleByOwner.clear();
		_tokensByVirtualValue.clear();
		getHostEvidence()->PrepareForRestore(EvidencePolicyDescriptor::getFailClosed());

		setExecutionDomainTag(checkpoint->getExecutionDomainTag());
		setAddressSpaceTag(checkpoint->getAddressSpaceTag());
		setVirtualizationEnabled(checkpoint->getVirtualizationEnabled());
		setOwnershipEpoch(checkpoint->getOwnershipEpoch());
		setTokenEpoch(checkpoint->getTokenEpoch());
		setCompletionEpoch(checkpoint->getCompletionEpoch());
		unsigned long long nextHandle = 0x7000'0000'0000'0001ULL;
		for (int index = 0; index < checkpoint->getVirtualHandles().size(); index++)
		{
			std::shared_ptr<Lane7VirtualHandle> handle = checkpoint->getVirtualHandles()[index];
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'typeof' operator:
			if (!handle || !handle->IsValid() || static_cast<int>(handle->AcceleratorId) < 1 || static_cast<int>(handle->AcceleratorId) > 6)
			{
				continue;
			}

			_handlesByValue[handle->Value] = handle;
			_handleByOwner[std::make_tuple(handle->ExecutionDomainTag, handle->OwnerVirtualThreadId, handle->AcceleratorId)] = handle->Value;
			if (handle->Value >= nextHandle)
			{
				nextHandle = handle->Value + 1;
			}
		}

		unsigned long long nextToken = 0x7100'0000'0000'0001ULL;
		for (int index = 0; index < checkpoint->getVirtualTokens().size(); index++)
		{
			std::shared_ptr<Lane7VirtualToken> token = checkpoint->getVirtualTokens()[index];
			if (!token->IsValid())
			{
				continue;
			}

			_tokensByVirtualValue[token->VirtualTokenId] = token;
			if (token->VirtualTokenId >= nextToken)
			{
				nextToken = token->VirtualTokenId + 1;
			}
		}

		_nextVirtualHandle = nextHandle;
		_nextVirtualToken = nextToken;
	}

	Lane7StateBlock::Lane7StateBlock() = default;

	bool Lane7StateBlock::getVirtualizationEnabled() const
	{
		return VirtualizationEnabled;
	}

	void Lane7StateBlock::setVirtualizationEnabled(bool value)
	{
		VirtualizationEnabled = value;
	}

	unsigned short Lane7StateBlock::getExecutionDomainTag() const
	{
		return ExecutionDomainTag;
	}

	void Lane7StateBlock::setExecutionDomainTag(unsigned short value)
	{
		ExecutionDomainTag = value;
	}

	unsigned short Lane7StateBlock::getAddressSpaceTag() const
	{
		return AddressSpaceTag;
	}

	void Lane7StateBlock::setAddressSpaceTag(unsigned short value)
	{
		AddressSpaceTag = value;
	}

	std::shared_ptr<Lane7QuotaPolicy> Lane7StateBlock::getQuotaPolicy() const
	{
		return QuotaPolicy;
	}

	void Lane7StateBlock::setQuotaPolicy(std::shared_ptr<Lane7QuotaPolicy> value)
	{
		QuotaPolicy = value;
	}

	unsigned long long Lane7StateBlock::getOwnershipEpoch() const
	{
		return OwnershipEpoch;
	}

	void Lane7StateBlock::setOwnershipEpoch(unsigned long long value)
	{
		OwnershipEpoch = value;
	}

	unsigned long long Lane7StateBlock::getPolicyEpoch() const
	{
		return PolicyEpoch;
	}

	void Lane7StateBlock::setPolicyEpoch(unsigned long long value)
	{
		PolicyEpoch = value;
	}

	unsigned long long Lane7StateBlock::getTokenEpoch() const
	{
		return TokenEpoch;
	}

	void Lane7StateBlock::setTokenEpoch(unsigned long long value)
	{
		TokenEpoch = value;
	}

	unsigned long long Lane7StateBlock::getCompletionEpoch() const
	{
		return CompletionEpoch;
	}

	void Lane7StateBlock::setCompletionEpoch(unsigned long long value)
	{
		CompletionEpoch = value;
	}

	unsigned long long Lane7StateBlock::getBackendBindingEpoch() const
	{
		return getHostEvidence()->getBackendBindingEpoch();
	}

	unsigned long long Lane7StateBlock::getPressureEpoch() const
	{
		return getHostEvidence()->getPressureEpoch();
	}

	std::shared_ptr<Lane7Fault> Lane7StateBlock::getLastFault() const
	{
		return LastFault;
	}

	void Lane7StateBlock::setLastFault(std::shared_ptr<Lane7Fault> value)
	{
		LastFault = value;
	}

	std::shared_ptr<Lane7PressureSnapshot> Lane7StateBlock::getLastPressure() const
	{
		return getHostEvidence()->getLastPressure();
	}

	std::shared_ptr<Lane7HostOwnedEvidenceStore> Lane7StateBlock::getHostEvidence() const
	{
		return HostEvidence;
	}

	std::shared_ptr<Lane7AcceleratorHandleNamespace> Lane7StateBlock::getHandleNamespace() const
	{
		if (!HandleNamespace) HandleNamespace = std::make_shared<Lane7AcceleratorHandleNamespace>(std::const_pointer_cast<Lane7StateBlock>(shared_from_this()));
		return HandleNamespace;
	}

	std::shared_ptr<Lane7AcceleratorTokenNamespace> Lane7StateBlock::getTokenNamespace() const
	{
		if (!TokenNamespace) TokenNamespace = std::make_shared<Lane7AcceleratorTokenNamespace>(std::const_pointer_cast<Lane7StateBlock>(shared_from_this()));
		return TokenNamespace;
	}

	std::shared_ptr<Lane7AcceleratorCompletionRouter> Lane7StateBlock::getCompletionRouter() const
	{
		if (!CompletionRouter) CompletionRouter = std::make_shared<Lane7AcceleratorCompletionRouter>(std::const_pointer_cast<Lane7StateBlock>(shared_from_this()));
		return CompletionRouter;
	}

	std::shared_ptr<Lane7AcceleratorAdmissionPolicy> Lane7StateBlock::getAdmissionPolicy() const
	{
		if (!AdmissionPolicy) AdmissionPolicy = std::make_shared<Lane7AcceleratorAdmissionPolicy>(std::const_pointer_cast<Lane7StateBlock>(shared_from_this()));
		return AdmissionPolicy;
	}

	int Lane7StateBlock::getActiveHandleCount() const
	{
		return _handlesByValue.size();
	}

	int Lane7StateBlock::getActiveVirtualTokenCount() const
	{
		return _tokensByVirtualValue.size();
	}

	unsigned long long Lane7StateBlock::ConfigureOwnership(unsigned short executionDomainTag, unsigned short addressSpaceTag, bool enabled, std::optional<std::shared_ptr<Lane7QuotaPolicy>> quotaPolicy)
	{
		if (enabled && executionDomainTag == 0)
		{
			throw std::out_of_range("Lane7 domain ownership requires a nonzero execution-domain tag.");
		}

		setExecutionDomainTag(enabled ? executionDomainTag : static_cast<unsigned short>(0));
		setAddressSpaceTag(enabled ? addressSpaceTag : static_cast<unsigned short>(0));
		setVirtualizationEnabled(enabled);
		setQuotaPolicy(std::make_shared<Lane7QuotaPolicy>((quotaPolicy && *quotaPolicy ? **quotaPolicy : Lane7QuotaPolicy::Default()).Normalize()));
		getHostEvidence()->ResetSchedulerWindow();
		return AdvanceOwnershipEpoch();
	}

	std::shared_ptr<Lane7VirtualHandle> Lane7StateBlock::AllocateVirtualHandle(unsigned short ownerVirtualThreadId, AcceleratorDeviceId acceleratorId, Lane7VirtualCapability capabilities)
	{
		std::shared_ptr<Lane7VirtualHandle> handle;
		std::shared_ptr<Lane7Fault> fault;
		if (!TryAllocateVirtualHandle(ownerVirtualThreadId, acceleratorId, capabilities, handle, fault))
		{
			throw std::runtime_error("Lane7 virtual handle allocation failed");
		}

		return handle;
	}

	bool Lane7StateBlock::TryAllocateVirtualHandle(unsigned short ownerVirtualThreadId, AcceleratorDeviceId acceleratorId, Lane7VirtualCapability capabilities, std::shared_ptr<Lane7VirtualHandle> &handle, std::shared_ptr<Lane7Fault> &fault)
	{
		handle = nullptr;
		if (!ValidateEnabled(fault))
		{
			return false;
		}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'typeof' operator:
		if ((static_cast<int>(acceleratorId) < 1 || static_cast<int>(acceleratorId) > 6) || capabilities == Lane7VirtualCapability::None)
		{
			fault = RecordFault(Lane7Fault::Abort(Lane7FaultKind::CapabilityDenied, L"Lane7 virtual accelerator handle allocation requires filtered virtual capabilities."));
			return false;
		}

		if (_handlesByValue.size() >= getQuotaPolicy()->MaxVirtualHandles)
		{
			fault = RecordFault(Lane7Fault::Replay(Lane7FaultKind::QuotaExceeded, L"Lane7 virtual accelerator handle quota is exhausted."));
			ObservePressure(ownerVirtualThreadId, true);
			return false;
		}

		unsigned long long value = AllocateVirtualHandleValue();
		handle = std::make_shared<Lane7VirtualHandle>(getExecutionDomainTag(), ownerVirtualThreadId, value, acceleratorId, capabilities, AdvanceOwnershipEpoch());
		_handlesByValue[value] = handle;
		_handleByOwner[std::make_tuple(getExecutionDomainTag(), ownerVirtualThreadId, acceleratorId)] = value;
		fault = Lane7Fault::None;
		return true;
	}

	bool Lane7StateBlock::TryReleaseVirtualHandle(unsigned long long virtualHandle)
	{
		std::shared_ptr<Lane7VirtualHandle> handle;
		if (!TryGetVirtualHandle(virtualHandle, handle))
		{
			return false;
		}

		_handlesByValue.erase(virtualHandle);
		_handleByOwner.erase(std::make_tuple(handle->ExecutionDomainTag, handle->OwnerVirtualThreadId, handle->AcceleratorId));
		getHostEvidence()->RemoveBackendBinding(virtualHandle);
		std::vector<unsigned long long> tokenIds;
		for (auto entry : _tokensByVirtualValue)
		{
			if (entry.second->VirtualHandle == virtualHandle)
			{
				tokenIds.push_back(entry.first);
			}
		}

		for (int index = 0; index < tokenIds.size(); index++)
		{
			ReleaseVirtualToken(tokenIds[index]);
		}

		AdvanceOwnershipEpoch();
		return true;
	}

	bool Lane7StateBlock::TryGetVirtualHandle(unsigned long long virtualHandle, std::shared_ptr<Lane7VirtualHandle> &handle)
	{
		handle.reset(); auto it = _handlesByValue.find(virtualHandle); if (it == _handlesByValue.end()) return false;
		handle = it->second; return true;
	}


	bool Lane7StateBlock::TryFindVirtualHandle(unsigned short executionDomainTag, unsigned short ownerVirtualThreadId, AcceleratorDeviceId acceleratorId, std::shared_ptr<Lane7VirtualHandle> &handle)
	{
		handle.reset(); auto it = _handleByOwner.find(std::make_tuple(executionDomainTag, ownerVirtualThreadId, acceleratorId));
		return it != _handleByOwner.end() && TryGetVirtualHandle(it->second, handle);
	}


	bool Lane7StateBlock::TryBindBackend(unsigned long long virtualHandle, unsigned long long backendGeneration, std::shared_ptr<Lane7BackendBinding> &binding, bool available, bool requiresRebind)
	{
		binding = nullptr;
		std::shared_ptr<Lane7VirtualHandle> handle;
		if (!TryGetVirtualHandle(virtualHandle, handle))
		{
			setLastFault(Lane7Fault::Abort(Lane7FaultKind::InvalidVirtualHandle, L"Lane7 backend binding requires an existing VM-local virtual accelerator handle."));
			return false;
		}

		if (!getHostEvidence()->TryBindBackend(handle->ExecutionDomainTag, handle->OwnerVirtualThreadId, virtualHandle, backendGeneration, available, requiresRebind, binding))
		{
			setLastFault(Lane7Fault::Abort(Lane7FaultKind::BackendUnavailable, L"Lane7 backend binding evidence epoch is exhausted and cannot be reused."));
			return false;
		}

		return true;
	}

	void Lane7StateBlock::DiscardBackendBindings()
	{
		getHostEvidence()->DiscardBackendBindings();
	}

	bool Lane7StateBlock::TryRebuildBackendBinding(unsigned long long virtualHandle, unsigned long long backendGeneration)
	{
		std::shared_ptr<Lane7BackendBinding> ignored; return TryBindBackend(virtualHandle, backendGeneration, ignored);
	}

	bool Lane7StateBlock::MarkBackendUnavailable(unsigned long long virtualHandle)
	{
		std::shared_ptr<Lane7BackendBinding> ignored; return TryBindBackend(virtualHandle, 0, ignored, false, false);
	}

	bool Lane7StateBlock::MarkBackendRebindRequired(unsigned long long virtualHandle)
	{
		std::shared_ptr<Lane7BackendBinding> ignored; return TryBindBackend(virtualHandle, 0, ignored, true, true);
	}

	bool Lane7StateBlock::TryValidateGuestSubmit(std::shared_ptr<AcceleratorCommandDescriptor> descriptor, unsigned short executionDomainTag, unsigned short addressSpaceTag, std::shared_ptr<Lane7VirtualHandle> &handle, std::shared_ptr<Lane7Fault> &fault)
	{
		if (!descriptor) throw std::invalid_argument("descriptor");
		handle = nullptr;
		if (!ValidateOwnership(executionDomainTag, addressSpaceTag, descriptor->OwnerBinding->OwnerVirtualThreadId, fault))
		{
			return false;
		}

		if (!TryFindVirtualHandle(executionDomainTag, descriptor->OwnerBinding->OwnerVirtualThreadId, descriptor->AcceleratorId, handle))
		{
			fault = RecordFault(Lane7Fault::Abort(Lane7FaultKind::InvalidVirtualHandle, L"Guest Lane7 submit requires a VM-local virtual accelerator handle bound to the descriptor owner."));
			return false;
		}

		if ((static_cast<int>(handle->Capabilities) & static_cast<int>(Lane7VirtualCapability::Submit)) == 0)
		{
			fault = RecordFault(Lane7Fault::Abort(Lane7FaultKind::CapabilityDenied, L"Guest Lane7 submit is denied by the filtered virtual capability mask."));
			return false;
		}

		if (!TryValidateBackendUsable(handle->Value, fault))
		{
			return false;
		}

		if (_tokensByVirtualValue.size() >= getQuotaPolicy()->MaxInflightTokens)
		{
			fault = RecordFault(Lane7Fault::Replay(Lane7FaultKind::QuotaExceeded, L"Guest Lane7 submit hit transient VM-local token quota pressure."));
			ObservePressure(handle->OwnerVirtualThreadId, true);
			return false;
		}

		fault = Lane7Fault::None;
		return true;
	}

	bool Lane7StateBlock::TryMapHostToken(std::shared_ptr<Lane7VirtualHandle> handle, unsigned short addressSpaceTag, std::shared_ptr<AcceleratorToken> token, std::shared_ptr<Lane7VirtualToken> &virtualToken, std::shared_ptr<Lane7Fault> &fault)
	{
		if (!token) throw std::invalid_argument("token");
		virtualToken = nullptr;
		if (!handle || !handle->IsValid() || !_handlesByValue.contains(handle->Value))
		{
			fault = RecordFault(Lane7Fault::Abort(Lane7FaultKind::InvalidVirtualHandle, L"Lane7 token virtualization requires a valid VM-local accelerator handle."));
			return false;
		}

		if (_tokensByVirtualValue.size() >= getQuotaPolicy()->MaxInflightTokens)
		{
			fault = RecordFault(Lane7Fault::Replay(Lane7FaultKind::QuotaExceeded, L"Lane7 token virtualization quota is exhausted."));
			ObservePressure(handle->OwnerVirtualThreadId, true);
			return false;
		}

		unsigned long long virtualTokenId = AllocateVirtualTokenValue();
		unsigned long long tokenEpoch = AdvanceTokenEpoch();
		virtualToken = std::make_shared<Lane7VirtualToken>(handle->ExecutionDomainTag, addressSpaceTag, handle->OwnerVirtualThreadId, handle->Value, virtualTokenId, tokenEpoch, AcceleratorTokenStatusWord::FromToken(*token), getCompletionEpoch());
		_tokensByVirtualValue[virtualTokenId] = virtualToken;
		if (!getHostEvidence()->TryBindToken(virtualToken, token->getHandle()))
		{
			_tokensByVirtualValue.erase(virtualTokenId);
			fault = RecordFault(Lane7Fault::Abort(Lane7FaultKind::InvalidVirtualToken, L"Lane7 host-owned token evidence store denied native token binding."));
			return false;
		}

		fault = Lane7Fault::None;
		return true;
	}

	bool Lane7StateBlock::TryResolveHostToken(unsigned long long virtualTokenId, unsigned short executionDomainTag, unsigned short addressSpaceTag, unsigned short ownerVirtualThreadId, std::shared_ptr<AcceleratorTokenHandle> &hostHandle, std::shared_ptr<Lane7VirtualToken> &virtualToken, std::shared_ptr<Lane7Fault> &fault)
	{
		hostHandle = AcceleratorTokenHandle::Invalid;
		virtualToken = nullptr;
		auto it = _tokensByVirtualValue.find(virtualTokenId);
		if (it == _tokensByVirtualValue.end() || !it->second || it->second->ExecutionDomainTag != executionDomainTag || (it->second->AddressSpaceTag != 0 && it->second->AddressSpaceTag != addressSpaceTag) || it->second->OwnerVirtualThreadId != ownerVirtualThreadId)
		{
			fault = RecordFault(Lane7Fault::Abort(Lane7FaultKind::InvalidVirtualToken, L"Guest Lane7 token lookup requires a virtual token owned by the current domain/address-space/VT.")); return false;
		}
		virtualToken = it->second;


		if (!getHostEvidence()->TryResolveHostToken(virtualTokenId, hostHandle))
		{
			fault = RecordFault(Lane7Fault::Abort(Lane7FaultKind::InvalidVirtualToken, L"Lane7 native token evidence is not materialized and must be rebuilt by host runtime."));
			return false;
		}

		fault = Lane7Fault::None;
		return true;
	}

	bool Lane7StateBlock::TryResolveVirtualTokenForHost(std::shared_ptr<AcceleratorTokenHandle> hostHandle, std::shared_ptr<Lane7VirtualToken> &virtualToken)
	{
		return getHostEvidence()->TryResolveVirtualTokenForHost(hostHandle, _tokensByVirtualValue, virtualToken);
	}

	bool Lane7StateBlock::UpdateVirtualTokenStatus(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<Lane7VirtualToken> &virtualToken)
	{
		if (!token) throw std::invalid_argument("token");
		if (!TryResolveVirtualTokenForHost(token->getHandle(), virtualToken))
		{
			return false;
		}

// C# TO C++ CONVERTER TASK: C# 'with expressions' are not converted by C# to C++ Converter:
virtualToken = std::make_shared<Lane7VirtualToken>(virtualToken->ExecutionDomainTag, virtualToken->AddressSpaceTag, virtualToken->OwnerVirtualThreadId, virtualToken->VirtualHandle, virtualToken->VirtualTokenId, virtualToken->TokenEpoch, AcceleratorTokenStatusWord::FromToken(*token), getCompletionEpoch());
		_tokensByVirtualValue[virtualToken->VirtualTokenId] = virtualToken;
		AdvanceTokenEpoch();
		return true;
	}

	bool Lane7StateBlock::ReleaseVirtualToken(unsigned long long virtualTokenId)
	{
		auto it = _tokensByVirtualValue.find(virtualTokenId);
		if (it == _tokensByVirtualValue.end())
		{
			return false;
		}

		auto token = it->second;
		_tokensByVirtualValue.erase(it);
		getHostEvidence()->ReleaseToken(virtualTokenId);

		AdvanceTokenEpoch();
		return token->IsValid();
	}

	bool Lane7StateBlock::TryBuildCompletion(std::shared_ptr<AcceleratorToken> token, unsigned short sourceOpcode, unsigned long long runtimeQueueSequence, std::shared_ptr<LaneCompletionDescriptor> &completion, std::shared_ptr<Lane7Fault> &fault)
	{
		if (!token) throw std::invalid_argument("token");
		completion = nullptr;
		std::shared_ptr<Lane7VirtualToken> virtualToken;
		if (!TryResolveVirtualTokenForHost(token->getHandle(), virtualToken))
		{
			fault = RecordFault(Lane7Fault::Abort(Lane7FaultKind::InvalidVirtualToken, L"Lane7 completion routing requires a VM-local virtual token mapping."));
			return false;
		}

		UpdateVirtualTokenStatus(token, virtualToken);
		unsigned long long completionEpoch = AdvanceCompletionEpoch();
		std::shared_ptr<AcceleratorCommandDescriptor> descriptor = token->getDescriptor();
		completion = std::make_shared<LaneCompletionDescriptor>(LaneCompletionSourceKind::ExternalAcceleratorLane7, 7, sourceOpcode, virtualToken->OwnerVirtualThreadId, virtualToken->ExecutionDomainTag, virtualToken->AddressSpaceTag, virtualToken->VirtualTokenId, virtualToken->TokenEpoch, descriptor->Identity.DescriptorIdentityHash, descriptor->Identity.NormalizedFootprintHash, descriptor->OwnerBinding->DomainTag, runtimeQueueSequence, static_cast<unsigned long long>(static_cast<unsigned char>(token->getState())), token->getState() == AcceleratorTokenState::DeviceComplete, completionEpoch);
		fault = Lane7Fault::None;
		return true;
	}

	unsigned long long Lane7StateBlock::BuildFilteredCapabilityWord(std::shared_ptr<Lane7VirtualHandle> handle)
	{
		unsigned long long packed = 0;
		packed |= 1ULL;
		packed |= (static_cast<unsigned long long>(handle->OwnerVirtualThreadId) & 0xFFFFULL) << 16;
		packed |= (static_cast<unsigned long long>(handle->Capabilities) & 0xFFFF'FFFFULL) << 32;
		return packed;
	}

	std::shared_ptr<Lane7PressureSnapshot> Lane7StateBlock::ObserveSubmitPollPressure(unsigned short ownerVirtualThreadId)
	{
		return getHostEvidence()->ObserveSubmitPollPressure(getExecutionDomainTag(), getAddressSpaceTag(), ownerVirtualThreadId, _tokensByVirtualValue.size(), getQuotaPolicy());
	}

	void Lane7StateBlock::ResetPressureWindow()
	{
		getHostEvidence()->ResetSchedulerWindow();
		ObservePressure(0, false);
	}

	std::shared_ptr<Lane7HostEvidenceRestoreResult> Lane7StateBlock::PrepareHostEvidenceForRestore(std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy)
	{
		return getHostEvidence()->PrepareForRestore(evidencePolicy);
	}

	std::shared_ptr<Lane7HostEvidenceRestoreResult> Lane7StateBlock::RebuildHostTokenAfterRestore(std::shared_ptr<Lane7VirtualToken> virtualToken, std::shared_ptr<AcceleratorTokenHandle> hostHandle)
	{
		return getHostEvidence()->RebuildTokenAfterRestore(virtualToken, hostHandle);
	}

	std::shared_ptr<Lane7HostEvidenceRestoreResult> Lane7StateBlock::RebuildBackendBindingAfterRestore(unsigned long long virtualHandle, unsigned long long backendGeneration, std::shared_ptr<Lane7BackendBinding> &binding)
	{
		binding = nullptr;
		std::shared_ptr<Lane7VirtualHandle> handle;
		if (!TryGetVirtualHandle(virtualHandle, handle))
		{
			return Lane7HostEvidenceRestoreResult::Rejected(L"Lane7 backend evidence rebuild requires an existing VM-local virtual accelerator handle.");
		}

		return getHostEvidence()->RebuildBackendAfterRestore(handle->ExecutionDomainTag, handle->OwnerVirtualThreadId, virtualHandle, backendGeneration, binding);
	}

	bool Lane7StateBlock::TryValidateBackendUsable(unsigned long long virtualHandle, std::shared_ptr<Lane7Fault> &fault)
	{
		std::shared_ptr<Lane7BackendBinding> binding;
		if (!getHostEvidence()->TryResolveBackendBinding(virtualHandle, binding))
		{
			fault = RecordFault(Lane7Fault::Fallback(Lane7FaultKind::FirstUseBindingRequired, L"Lane7 backend binding is not materialized; runtime fallback must bind or reject."));
			return false;
		}

		if (binding->RequiresRebind)
		{
			fault = RecordFault(Lane7Fault::Fallback(Lane7FaultKind::BackendRebindRequired, L"Lane7 backend binding is stale after migration/rebind epoch change."));
			return false;
		}

		if (!binding->Available)
		{
			fault = RecordFault(Lane7Fault::Fallback(Lane7FaultKind::BackendUnavailable, L"Lane7 backend is unavailable for the VM-local virtual accelerator handle."));
			return false;
		}

		fault = Lane7Fault::None;
		return true;
	}

	bool Lane7StateBlock::ValidateEnabled(std::shared_ptr<Lane7Fault> &fault)
	{
		if (!getVirtualizationEnabled() || getExecutionDomainTag() == 0)
		{
			fault = RecordFault(Lane7Fault::Abort(Lane7FaultKind::VirtualizationDisabled, L"Lane7 domain virtualization is disabled or lacks nonzero execution-domain ownership."));
			return false;
		}

		fault = Lane7Fault::None;
		return true;
	}

	bool Lane7StateBlock::ValidateOwnership(unsigned short executionDomainTag, unsigned short addressSpaceTag, unsigned short ownerVirtualThreadId, std::shared_ptr<Lane7Fault> &fault)
	{
		if (!ValidateEnabled(fault))
		{
			return false;
		}

		if (executionDomainTag == 0 || executionDomainTag != getExecutionDomainTag() || (getAddressSpaceTag() != 0 && addressSpaceTag != getAddressSpaceTag()))
		{
			fault = RecordFault(Lane7Fault::Abort(Lane7FaultKind::OwnershipMismatch, L"Guest Lane7 operation domain/address-space tags do not match Lane7 ownership."));
			return false;
		}

		if (ownerVirtualThreadId > std::numeric_limits<unsigned char>::max())
		{
			fault = RecordFault(Lane7Fault::Abort(Lane7FaultKind::OwnershipMismatch, L"Guest Lane7 owner VT exceeds the VM-local routable VT namespace."));
			return false;
		}

		fault = Lane7Fault::None;
		return true;
	}

	std::shared_ptr<Lane7PressureSnapshot> Lane7StateBlock::ObservePressure(unsigned short ownerVirtualThreadId, bool quotaExceeded)
	{
		return getHostEvidence()->ObserveSchedulerPressure(getExecutionDomainTag(), getAddressSpaceTag(), ownerVirtualThreadId, _tokensByVirtualValue.size(), getQuotaPolicy(), quotaExceeded);
	}

	std::shared_ptr<Lane7Fault> Lane7StateBlock::RecordFault(std::shared_ptr<Lane7Fault> fault)
	{
		setLastFault(fault);
		return fault;
	}

	unsigned long long Lane7StateBlock::AllocateVirtualHandleValue()
	{
		while (true)
		{
			unsigned long long value = _nextVirtualHandle++;
			if (_nextVirtualHandle == 0)
			{
				_nextVirtualHandle = 0x7000'0000'0000'0001ULL;
			}

			if (value != 0 && !_handlesByValue.contains(value))
			{
				return value;
			}
		}
	}

	unsigned long long Lane7StateBlock::AllocateVirtualTokenValue()
	{
		while (true)
		{
			unsigned long long value = _nextVirtualToken++;
			if (_nextVirtualToken == 0)
			{
				_nextVirtualToken = 0x7100'0000'0000'0001ULL;
			}

			if (value != 0 && !_tokensByVirtualValue.contains(value))
			{
				return value;
			}
		}
	}

	unsigned long long Lane7StateBlock::AdvanceOwnershipEpoch()
	{
//		unchecked
			setOwnershipEpoch(getOwnershipEpoch() + 1);
			if (getOwnershipEpoch() == 0)
			{
				setOwnershipEpoch(1);
			}
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.

		return getOwnershipEpoch();
	}

	unsigned long long Lane7StateBlock::AdvanceTokenEpoch()
	{
//		unchecked
			setTokenEpoch(getTokenEpoch() + 1);
			if (getTokenEpoch() == 0)
			{
				setTokenEpoch(1);
			}
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.

		return getTokenEpoch();
	}

	unsigned long long Lane7StateBlock::AdvanceCompletionEpoch()
	{
//		unchecked
			setCompletionEpoch(getCompletionEpoch() + 1);
			if (getCompletionEpoch() == 0)
			{
				setCompletionEpoch(1);
			}
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.

		return getCompletionEpoch();
	}
}
