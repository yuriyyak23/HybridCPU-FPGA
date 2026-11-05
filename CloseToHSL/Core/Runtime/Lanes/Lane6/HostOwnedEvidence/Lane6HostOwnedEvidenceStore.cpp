#include "Lane6HostOwnedEvidenceStore.h"
#include "../../../Evidence/HostOwned/YAKSys_Hybrid_CPU.Core.HostOwnedEvidenceBoundary.h"
#include "../../../Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h"
#include "../../../Evidence/HostOwned/HostOwnedEvidenceBoundary.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;

namespace YAKSys_Hybrid_CPU::Core
{

	int Lane6HostOwnedEvidenceStore::getActiveBindingCount() const
	{
		return _nativeTokenBindings.size();
	}

	bool Lane6HostOwnedEvidenceStore::TryBind(std::shared_ptr<Lane6VirtualToken> virtualToken, std::shared_ptr<DmaStreamComputeTokenHandle> hostHandle)
	{
		if (!virtualToken || !virtualToken->IsValid() || !hostHandle || hostHandle->IsDefault())
		{
			return false;
		}

		_nativeTokenBindings[*virtualToken] = hostHandle;
		return true;
	}

	bool Lane6HostOwnedEvidenceStore::TryResolve(std::shared_ptr<Lane6VirtualToken> virtualToken, std::shared_ptr<DmaStreamComputeTokenHandle> &hostHandle)
	{
		hostHandle.reset();
		if (!virtualToken) return false;
		auto it = _nativeTokenBindings.find(*virtualToken);
		if (it == _nativeTokenBindings.end()) return false;
		hostHandle = it->second;
		return true;
	}

	std::shared_ptr<Lane6HostEvidenceRestoreResult> Lane6HostOwnedEvidenceStore::PrepareForRestore(std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy)
	{
		std::shared_ptr<HostOwnedEvidenceBoundary> boundary = std::make_shared<HostOwnedEvidenceBoundary>();
		std::shared_ptr<HostOwnedEvidenceBoundaryResult> validation = boundary->ValidateRestore(evidencePolicy, EvidenceVisibilityClass::NativeTokenEvidence, EvidenceRestorePolicy::RecomputeAfterRestore);
		if (validation->Decision != HostOwnedEvidenceBoundaryDecision::RestoreRequiresRecompute)
		{
			return Lane6HostEvidenceRestoreResult::Rejected(L"Lane6 native token evidence cannot be imported as restored state.");
		}

		_nativeTokenBindings.clear();
		return Lane6HostEvidenceRestoreResult::RebuildRequired();
	}

	std::shared_ptr<Lane6HostEvidenceRestoreResult> Lane6HostOwnedEvidenceStore::RebuildAfterRestore(std::shared_ptr<Lane6VirtualToken> virtualToken, std::shared_ptr<DmaStreamComputeTokenHandle> hostHandle)
	{
		return TryBind(virtualToken, hostHandle) ? Lane6HostEvidenceRestoreResult::Rebuilt() : Lane6HostEvidenceRestoreResult::Rejected(L"Lane6 host evidence rebuild requires a valid virtual token and fresh host token handle.");
	}
}
