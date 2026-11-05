#include "YAKSys_Hybrid_CPU.Core.VectorStreamDomainRuntime.h"
#include "VectorStreamDomainRuntime.h"
#include "../../../Lanes/VectorStream/State/YAKSys_Hybrid_CPU.Core.VectorStreamExecutionExtensionDescriptor.h"
#include "../../../Lanes/VectorStream/State/VectorStreamState.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<VectorStreamDomainRuntimeResult> VectorStreamDomainRuntime::Validate(std::shared_ptr<VectorStreamDomainRuntimeRequest> request)
	{
		if (!request || !request->Extension)
		{
			return VectorStreamDomainRuntimeResult::Denied(VectorStreamDomainRuntimeDecision::MissingExecutionExtension, L"Vector-stream runtime requires an execution-extension descriptor.");
		}

		if (!request->Extension->isRuntimeAuthoritative())
		{
			return VectorStreamDomainRuntimeResult::Denied(VectorStreamDomainRuntimeDecision::ExtensionAuthorityDenied, L"Execution extension authority must be runtime-owned.");
		}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: VectorStreamExecutionExtensionDescriptor? vectorStream = request.Extension.VectorStream;
		std::shared_ptr<VectorStreamExecutionExtensionDescriptor> vectorStream = request->Extension->getVectorStream();
		if (vectorStream == nullptr)
		{
			return VectorStreamDomainRuntimeResult::Denied(VectorStreamDomainRuntimeDecision::MissingVectorStreamDescriptor, L"Vector-stream runtime requires a vector-stream descriptor.");
		}

		if (!vectorStream->isRuntimeAuthoritative())
		{
			return VectorStreamDomainRuntimeResult::Denied(VectorStreamDomainRuntimeDecision::VectorStreamAuthorityDenied, L"Vector-stream state authority must be runtime-owned.");
		}

		if (!vectorStream->getEnabled())
		{
			return VectorStreamDomainRuntimeResult::Denied(VectorStreamDomainRuntimeDecision::VectorStreamDisabled, L"Vector-stream runtime is disabled by descriptor.");
		}

		if (request->RequiredSaveRestoreMask != VectorStreamSaveMask::None && !vectorStream->AllowsSaveRestore(request->RequiredSaveRestoreMask))
		{
			return VectorStreamDomainRuntimeResult::Denied(VectorStreamDomainRuntimeDecision::SaveRestoreMaskDenied, L"Vector-stream save/restore mask is denied by descriptor.");
		}

		if (request->RequiredVectorLength != 0 && !vectorStream->AllowsVectorLength(request->RequiredVectorLength))
		{
			return VectorStreamDomainRuntimeResult::Denied(VectorStreamDomainRuntimeDecision::VectorLengthDenied, L"Vector length exceeds descriptor limits.");
		}

		if (request->RequiresStreamDescriptorTable && !vectorStream->getHasBoundStreamDescriptorTable())
		{
			return VectorStreamDomainRuntimeResult::Denied(VectorStreamDomainRuntimeDecision::MissingStreamDescriptorTable, L"Vector-stream runtime requires a bound stream descriptor table.");
		}

		if (request->RequiresCompatibilityProjection && !request->Extension->getAllowsVectorStreamProjection())
		{
			return VectorStreamDomainRuntimeResult::Denied(VectorStreamDomainRuntimeDecision::CompatibilityProjectionDenied, L"Execution extension denies vector-stream compatibility projection.");
		}

		return VectorStreamDomainRuntimeResult::Allowed;
	}

	bool VectorStreamDomainRuntime::CanAdmit(std::shared_ptr<VectorStreamDomainRuntimeRequest> request)
	{
		const auto result = Validate(request);
		return result && result->IsAllowed;
	}
}
