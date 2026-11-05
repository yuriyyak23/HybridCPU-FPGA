#pragma once

#include "YAKSys_Hybrid_CPU.Core.VectorStreamSnapshot.h"
#include "../../../Domains/Descriptors/ExecutionDomain/YAKSys_Hybrid_CPU.Core.ExecutionExtensionDescriptor.h"
#include "../State/VectorStreamState.h"
#include "../../../../State/ICpuState.h"
#include "VectorStreamSaveRestoreProjection.h"
#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	struct VectorStreamProjectionResult final
	{
		const VectorStreamProjectionDecision Decision;
		const VectorStreamSaveMask EffectiveMask;
		const std::wstring Message;
		const bool IsAllowed;

		VectorStreamProjectionResult(VectorStreamProjectionDecision decision, VectorStreamSaveMask effectiveMask, std::wstring message)
			: Decision(decision), EffectiveMask(effectiveMask), Message(std::move(message)),
			  IsAllowed(decision == VectorStreamProjectionDecision::Allowed) {}

		static std::shared_ptr<VectorStreamProjectionResult> Allowed(VectorStreamSaveMask effectiveMask)
		{
			return std::make_shared<VectorStreamProjectionResult>(VectorStreamProjectionDecision::Allowed, effectiveMask, std::wstring{});
		}
		static std::shared_ptr<VectorStreamProjectionResult> Denied(VectorStreamProjectionDecision decision, const std::wstring &message)
		{
			return std::make_shared<VectorStreamProjectionResult>(decision, VectorStreamSaveMask::None, message);
		}
	};

	class VectorStreamSaveRestoreProjection final : public std::enable_shared_from_this<VectorStreamSaveRestoreProjection>
	{
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public VectorStreamProjectionResult Validate(ExecutionExtensionDescriptor? extension, VectorStreamSnapshot snapshot, VectorStreamSaveMask requestedMask, bool compatibilityProjectionRequest)
		std::shared_ptr<VectorStreamProjectionResult> Validate(std::shared_ptr<ExecutionExtensionDescriptor> extension, std::shared_ptr<VectorStreamSnapshot> snapshot, VectorStreamSaveMask requestedMask, bool compatibilityProjectionRequest);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public VectorStreamProjectionResult RestoreTo(ExecutionExtensionDescriptor? extension, VectorStreamSnapshot snapshot, ICanonicalCpuState state, VectorStreamSaveMask requestedMask, bool compatibilityProjectionRequest)
		std::shared_ptr<VectorStreamProjectionResult> RestoreTo(std::shared_ptr<ExecutionExtensionDescriptor> extension, std::shared_ptr<VectorStreamSnapshot> snapshot, std::shared_ptr<ICanonicalCpuState> state, VectorStreamSaveMask requestedMask, bool compatibilityProjectionRequest);

	private:
		static std::shared_ptr<VectorStreamProjectionResult> Deny(VectorStreamProjectionDecision decision, const std::wstring &message);
	};
}
