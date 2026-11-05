#pragma once

#include "../../../Domains/Descriptors/ExecutionDomain/YAKSys_Hybrid_CPU.Core.ExecutionExtensionDescriptor.h"
#include "../../../Lanes/VectorStream/State/VectorStreamState.h"
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class VectorStreamDomainRuntimeDecision : unsigned char
	{
		Allowed = 0,
		MissingExecutionExtension = 1,
		ExtensionAuthorityDenied = 2,
		MissingVectorStreamDescriptor = 3,
		VectorStreamAuthorityDenied = 4,
		VectorStreamDisabled = 5,
		SaveRestoreMaskDenied = 6,
		VectorLengthDenied = 7,
		MissingStreamDescriptorTable = 8,
		CompatibilityProjectionDenied = 9
	};

	struct VectorStreamDomainRuntimeRequest final
	{
		const std::shared_ptr<ExecutionExtensionDescriptor> Extension;
		const VectorStreamSaveMask RequiredSaveRestoreMask;
		const unsigned long long RequiredVectorLength;
		const bool RequiresStreamDescriptorTable;
		const bool RequiresCompatibilityProjection;

		VectorStreamDomainRuntimeRequest(std::shared_ptr<ExecutionExtensionDescriptor> extension,
			VectorStreamSaveMask requiredSaveRestoreMask, unsigned long long requiredVectorLength,
			bool requiresStreamDescriptorTable, bool requiresCompatibilityProjection)
			: Extension(std::move(extension)), RequiredSaveRestoreMask(requiredSaveRestoreMask),
			  RequiredVectorLength(requiredVectorLength), RequiresStreamDescriptorTable(requiresStreamDescriptorTable),
			  RequiresCompatibilityProjection(requiresCompatibilityProjection) {}
	};

	struct VectorStreamDomainRuntimeResult final
	{
		const VectorStreamDomainRuntimeDecision Decision;
		const std::wstring Reason;
		const bool IsAllowed;

		VectorStreamDomainRuntimeResult(VectorStreamDomainRuntimeDecision decision, std::wstring reason)
			: Decision(decision), Reason(std::move(reason)), IsAllowed(decision == VectorStreamDomainRuntimeDecision::Allowed) {}

		static const std::shared_ptr<VectorStreamDomainRuntimeResult> Allowed;
		static std::shared_ptr<VectorStreamDomainRuntimeResult> Denied(VectorStreamDomainRuntimeDecision decision, const std::wstring &reason)
		{
			return std::make_shared<VectorStreamDomainRuntimeResult>(decision, reason);
		}
	};

	inline const std::shared_ptr<VectorStreamDomainRuntimeResult> VectorStreamDomainRuntimeResult::Allowed =
		std::make_shared<VectorStreamDomainRuntimeResult>(VectorStreamDomainRuntimeDecision::Allowed,
			L"Vector-stream domain runtime admission allowed.");
}
