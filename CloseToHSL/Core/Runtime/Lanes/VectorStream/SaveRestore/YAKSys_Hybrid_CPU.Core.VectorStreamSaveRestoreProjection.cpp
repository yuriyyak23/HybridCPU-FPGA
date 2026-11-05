#include "YAKSys_Hybrid_CPU.Core.VectorStreamSaveRestoreProjection.h"
#include "../State/YAKSys_Hybrid_CPU.Core.VectorStreamExecutionExtensionDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public VectorStreamProjectionResult Validate(ExecutionExtensionDescriptor? extension, VectorStreamSnapshot snapshot, VectorStreamSaveMask requestedMask, bool compatibilityProjectionRequest)
	std::shared_ptr<VectorStreamProjectionResult> VectorStreamSaveRestoreProjection::Validate(std::shared_ptr<ExecutionExtensionDescriptor> extension, std::shared_ptr<VectorStreamSnapshot> snapshot, VectorStreamSaveMask requestedMask, bool compatibilityProjectionRequest)
	{
		if (extension == nullptr)
		{
			return Deny(VectorStreamProjectionDecision::MissingExecutionExtension, L"Vector-stream save/restore requires an execution extension descriptor.");
		}

		if (!extension->isRuntimeAuthoritative())
		{
			return Deny(VectorStreamProjectionDecision::RuntimeAuthorityMissing, L"Compatibility projection cannot own vector-stream save/restore authority.");
		}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: VectorStreamExecutionExtensionDescriptor? vectorStream = extension.VectorStream;
		std::shared_ptr<VectorStreamExecutionExtensionDescriptor> vectorStream = extension->getVectorStream();
		if (vectorStream == nullptr)
		{
			return Deny(VectorStreamProjectionDecision::MissingVectorStreamDescriptor, L"Vector-stream save/restore requires a vector-stream descriptor.");
		}

		if (!vectorStream->isRuntimeAuthoritative())
		{
			return Deny(VectorStreamProjectionDecision::RuntimeAuthorityMissing, L"Vector-stream compatibility projection cannot own architectural state.");
		}

		if (compatibilityProjectionRequest && !extension->getAllowsVectorStreamProjection())
		{
			return Deny(VectorStreamProjectionDecision::CompatibilityProjectionDenied, L"Vector-stream compatibility projection is not enabled by descriptor policy.");
		}

		VectorStreamSaveMask effectiveMask = static_cast<VectorStreamSaveMask>(
			static_cast<int>(requestedMask) & static_cast<int>(snapshot->getSaveMask()));
		if (!vectorStream->AllowsSaveRestore(effectiveMask))
		{
			return Deny(VectorStreamProjectionDecision::SaveMaskDenied, L"Vector-stream descriptor denies the requested save/restore mask.");
		}

		if (!vectorStream->AllowsVectorLength(snapshot->getVL()))
		{
			return Deny(VectorStreamProjectionDecision::SaveMaskDenied, L"Vector-stream descriptor denies the snapshot vector length.");
		}

		if (vectorStream->getReplayEpoch() != 0 && snapshot->getStreamReplayEpoch() != vectorStream->getReplayEpoch())
		{
			return Deny(VectorStreamProjectionDecision::SnapshotStale, L"Vector-stream snapshot replay epoch is stale.");
		}

		return VectorStreamProjectionResult::Allowed(effectiveMask);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public VectorStreamProjectionResult RestoreTo(ExecutionExtensionDescriptor? extension, VectorStreamSnapshot snapshot, ICanonicalCpuState state, VectorStreamSaveMask requestedMask, bool compatibilityProjectionRequest)
	std::shared_ptr<VectorStreamProjectionResult> VectorStreamSaveRestoreProjection::RestoreTo(std::shared_ptr<ExecutionExtensionDescriptor> extension, std::shared_ptr<VectorStreamSnapshot> snapshot, std::shared_ptr<ICanonicalCpuState> state, VectorStreamSaveMask requestedMask, bool compatibilityProjectionRequest)
	{
		std::shared_ptr<VectorStreamProjectionResult> result = Validate(extension, snapshot, requestedMask, compatibilityProjectionRequest);
		if (!result->IsAllowed)
		{
			return result;
		}

		snapshot->RestoreTo(state, result->EffectiveMask);
		return result;
	}

	std::shared_ptr<VectorStreamProjectionResult> VectorStreamSaveRestoreProjection::Deny(VectorStreamProjectionDecision decision, const std::wstring &message)
	{
		return VectorStreamProjectionResult::Denied(decision, message);
	}
}
