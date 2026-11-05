#include "Lane7HostOwnedEvidenceStore.h"
#include "../../../Evidence/HostOwned/HostOwnedEvidenceBoundary.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core
{

	int Lane7HostOwnedEvidenceStore::getActiveBackendBindingCount() const
	{
		return _backendBindings.size();
	}

	int Lane7HostOwnedEvidenceStore::getActiveTokenBindingCount() const
	{
		return _nativeTokenByVirtualToken.size();
	}

	unsigned long long Lane7HostOwnedEvidenceStore::getBackendBindingEpoch() const
	{
		return _backendBindingEpoch;
	}

	unsigned long long Lane7HostOwnedEvidenceStore::getPressureEpoch() const
	{
		return _pressureEpoch;
	}

	std::shared_ptr<Lane7PressureSnapshot> Lane7HostOwnedEvidenceStore::getLastPressure() const
	{
		return _lastPressure;
	}

	bool Lane7HostOwnedEvidenceStore::TryBindToken(std::shared_ptr<Lane7VirtualToken> virtualToken, std::shared_ptr<AcceleratorTokenHandle> hostHandle)
	{
		if (!virtualToken || !virtualToken->IsValid() || !hostHandle || !hostHandle->IsValid)
		{
			return false;
		}

		auto previous = _nativeTokenByVirtualToken.find(virtualToken->VirtualTokenId);
		if (previous != _nativeTokenByVirtualToken.end()) _virtualTokenByNativeHandle.erase(previous->second->Value);

		_nativeTokenByVirtualToken[virtualToken->VirtualTokenId] = hostHandle;
		_virtualTokenByNativeHandle[hostHandle->Value] = virtualToken->VirtualTokenId;
		return true;
	}

	bool Lane7HostOwnedEvidenceStore::TryResolveHostToken(unsigned long long virtualTokenId, std::shared_ptr<AcceleratorTokenHandle> &hostHandle)
	{
		hostHandle.reset(); auto it = _nativeTokenByVirtualToken.find(virtualTokenId);
		if (it == _nativeTokenByVirtualToken.end()) return false;
		hostHandle = it->second; return true;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public bool TryResolveVirtualTokenForHost(AcceleratorTokenHandle hostHandle, IReadOnlyDictionary<ulong, Lane7VirtualToken> virtualTokens, out Lane7VirtualToken virtualToken)
	bool Lane7HostOwnedEvidenceStore::TryResolveVirtualTokenForHost(std::shared_ptr<AcceleratorTokenHandle> hostHandle, const std::unordered_map<unsigned long long, std::shared_ptr<Lane7VirtualToken>> &virtualTokens, std::shared_ptr<Lane7VirtualToken> &virtualToken)
	{
		virtualToken.reset(); if (!hostHandle || !hostHandle->IsValid) return false;
		auto it = _virtualTokenByNativeHandle.find(hostHandle->Value); if (it == _virtualTokenByNativeHandle.end()) return false;
		auto found = virtualTokens.find(it->second); if (found == virtualTokens.end()) return false;
		virtualToken = found->second; return true;
	}


	bool Lane7HostOwnedEvidenceStore::ReleaseToken(unsigned long long virtualTokenId)
	{
		auto it = _nativeTokenByVirtualToken.find(virtualTokenId); if (it == _nativeTokenByVirtualToken.end()) return false;
		_virtualTokenByNativeHandle.erase(it->second->Value); _nativeTokenByVirtualToken.erase(it); return true;
	}


	void Lane7HostOwnedEvidenceStore::ClearTokenBindings()
	{
		_nativeTokenByVirtualToken.clear();
		_virtualTokenByNativeHandle.clear();
	}

	bool Lane7HostOwnedEvidenceStore::TryBindBackend(unsigned short executionDomainTag, unsigned short ownerVirtualThreadId, unsigned long long virtualHandle, unsigned long long backendGeneration, bool available, bool requiresRebind, std::shared_ptr<Lane7BackendBinding> &binding)
	{
		binding = nullptr;
		if (executionDomainTag == 0 || virtualHandle == 0)
		{
			return false;
		}

		unsigned long long bindingEpoch;
		if (!TryAdvanceBackendBindingEpoch(bindingEpoch))
		{
			return false;
		}

		binding = std::make_shared<Lane7BackendBinding>(executionDomainTag, ownerVirtualThreadId, virtualHandle, bindingEpoch, backendGeneration == 0 ? bindingEpoch : backendGeneration, available, requiresRebind);
		_backendBindings[virtualHandle] = binding;
		return true;
	}

	bool Lane7HostOwnedEvidenceStore::TryResolveBackendBinding(unsigned long long virtualHandle, std::shared_ptr<Lane7BackendBinding> &binding)
	{
		binding.reset(); auto it = _backendBindings.find(virtualHandle); if (it == _backendBindings.end()) return false;
		binding = it->second; return true;
	}


	bool Lane7HostOwnedEvidenceStore::RemoveBackendBinding(unsigned long long virtualHandle)
	{
		bool removed = _backendBindings.erase(virtualHandle);
		if (removed)
		{
			unsigned long long ignored; TryAdvanceBackendBindingEpoch(ignored);
		}

		return removed;
	}

	bool Lane7HostOwnedEvidenceStore::DiscardBackendBindings()
	{
		_backendBindings.clear();
		unsigned long long ignored; return TryAdvanceBackendBindingEpoch(ignored);
	}

	void Lane7HostOwnedEvidenceStore::ClearBackendBindings()
	{
		_backendBindings.clear();
	}

	std::shared_ptr<Lane7PressureSnapshot> Lane7HostOwnedEvidenceStore::ObserveSubmitPollPressure(unsigned short executionDomainTag, unsigned short addressSpaceTag, unsigned short ownerVirtualThreadId, int inflightTokens, std::shared_ptr<Lane7QuotaPolicy> quotaPolicy)
	{
		_submitPollCount++;
		bool quotaExceeded = _submitPollCount > quotaPolicy->MaxSubmitPollPerWindow;
		return ObserveSchedulerPressure(executionDomainTag, addressSpaceTag, ownerVirtualThreadId, inflightTokens, quotaPolicy, quotaExceeded);
	}

	std::shared_ptr<Lane7PressureSnapshot> Lane7HostOwnedEvidenceStore::ObserveSchedulerPressure(unsigned short executionDomainTag, unsigned short addressSpaceTag, unsigned short ownerVirtualThreadId, int inflightTokens, std::shared_ptr<Lane7QuotaPolicy> quotaPolicy, bool quotaExceeded)
	{
		unsigned long long pressureEpoch;
		bool epochAdvanced = TryAdvancePressureEpoch(pressureEpoch);
		bool failClosedPressure = !epochAdvanced || quotaExceeded;
		_lastPressure = std::make_shared<Lane7PressureSnapshot>(executionDomainTag, addressSpaceTag, ownerVirtualThreadId, inflightTokens, quotaPolicy->MaxInflightTokens, _submitPollCount, quotaPolicy->MaxSubmitPollPerWindow, failClosedPressure, failClosedPressure || _submitPollCount >= quotaPolicy->MaxSubmitPollPerWindow, pressureEpoch);
		return _lastPressure;
	}

	void Lane7HostOwnedEvidenceStore::ResetSchedulerWindow()
	{
		_submitPollCount = 0;
	}

	void Lane7HostOwnedEvidenceStore::ResetSchedulerEvidence()
	{
		_submitPollCount = 0;
		_pressureEpoch = 0;
		_lastPressure = nullptr;
	}

	std::shared_ptr<Lane7HostEvidenceRestoreResult> Lane7HostOwnedEvidenceStore::PrepareForRestore(std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy)
	{
		std::shared_ptr<HostOwnedEvidenceBoundary> boundary = std::make_shared<HostOwnedEvidenceBoundary>();
		if (!RequiresRecompute(boundary, evidencePolicy, EvidenceVisibilityClass::NativeTokenEvidence) || !RequiresRecompute(boundary, evidencePolicy, EvidenceVisibilityClass::BackendBindingEvidence) || !RequiresRecompute(boundary, evidencePolicy, EvidenceVisibilityClass::SchedulerEvidence))
		{
			return Lane7HostEvidenceRestoreResult::Rejected(L"Lane7 host-owned evidence cannot be imported as restored state.");
		}

		ClearTokenBindings();
		ClearBackendBindings();
		ResetSchedulerEvidence();
		return Lane7HostEvidenceRestoreResult::RebuildRequired();
	}

	std::shared_ptr<Lane7HostEvidenceRestoreResult> Lane7HostOwnedEvidenceStore::RebuildTokenAfterRestore(std::shared_ptr<Lane7VirtualToken> virtualToken, std::shared_ptr<AcceleratorTokenHandle> hostHandle)
	{
		return TryBindToken(virtualToken, hostHandle) ? Lane7HostEvidenceRestoreResult::Rebuilt() : Lane7HostEvidenceRestoreResult::Rejected(L"Lane7 host evidence rebuild requires a valid virtual token and fresh host token handle.");
	}

	std::shared_ptr<Lane7HostEvidenceRestoreResult> Lane7HostOwnedEvidenceStore::RebuildBackendAfterRestore(unsigned short executionDomainTag, unsigned short ownerVirtualThreadId, unsigned long long virtualHandle, unsigned long long backendGeneration, std::shared_ptr<Lane7BackendBinding> &binding)
	{
		return TryBindBackend(executionDomainTag, ownerVirtualThreadId, virtualHandle, backendGeneration, true, false, binding) ? Lane7HostEvidenceRestoreResult::Rebuilt() : Lane7HostEvidenceRestoreResult::Rejected(L"Lane7 backend evidence rebuild requires a valid virtual handle and an unexhausted binding epoch.");
	}

	bool Lane7HostOwnedEvidenceStore::RequiresRecompute(std::shared_ptr<HostOwnedEvidenceBoundary> boundary, std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy, EvidenceVisibilityClass evidenceClass)
	{
		return boundary->ValidateRestore(evidencePolicy, evidenceClass, EvidenceRestorePolicy::RecomputeAfterRestore)->Decision == HostOwnedEvidenceBoundaryDecision::RestoreRequiresRecompute;
	}

	bool Lane7HostOwnedEvidenceStore::TryAdvanceBackendBindingEpoch(unsigned long long &advancedEpoch)
	{
		return TryAdvanceEpoch(_backendBindingEpoch, advancedEpoch);
	}

	bool Lane7HostOwnedEvidenceStore::TryAdvancePressureEpoch(unsigned long long &advancedEpoch)
	{
		return TryAdvanceEpoch(_pressureEpoch, advancedEpoch);
	}

	bool Lane7HostOwnedEvidenceStore::TryAdvanceEpoch(unsigned long long &epoch, unsigned long long &advancedEpoch)
	{
		if (epoch == std::numeric_limits<unsigned long long>::max())
		{
			advancedEpoch = epoch;
			return false;
		}

		epoch++;
		if (epoch == 0)
		{
			epoch = 1;
		}

		advancedEpoch = epoch;
		return true;
	}
}
