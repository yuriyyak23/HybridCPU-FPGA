#pragma once

#include "../../Descriptors/IoDomain/YAKSys_Hybrid_CPU.Core.IoDomainDescriptor.h"
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class IoDomainRuntimeDecision : unsigned char
	{
		Allowed = 0,
		MissingDescriptor = 1,
		DescriptorAuthorityDenied = 2,
		DmaAuthorityDenied = 3,
		IommuAuthorityDenied = 4,
		MissingVirtualizationBlock = 5,
		MissingDmaWindow = 6,
		CompatibilityProjectionDenied = 7
	};

	struct IoDomainRuntimeRequest final {
		std::shared_ptr<IoDomainDescriptor> Descriptor;
		bool RequiresDmaAuthority = false;
		bool RequiresIommuAuthority = false;
		bool RequiresVirtualizationBlock = false;
		bool RequiresDmaWindow = false;
		bool RequiresCompatibilityProjection = false;
	};

	struct IoDomainRuntimeResult final {
		const IoDomainRuntimeDecision Decision;
		const std::wstring Reason;
		IoDomainRuntimeResult(IoDomainRuntimeDecision decision, std::wstring reason) : Decision(decision), Reason(std::move(reason)) {}
		bool isAllowed() const noexcept { return Decision == IoDomainRuntimeDecision::Allowed; }
		static const std::shared_ptr<IoDomainRuntimeResult> Allowed;
		static std::shared_ptr<IoDomainRuntimeResult> Denied(IoDomainRuntimeDecision decision, std::wstring reason) { return std::make_shared<IoDomainRuntimeResult>(decision, std::move(reason)); }
	};

	inline const std::shared_ptr<IoDomainRuntimeResult> IoDomainRuntimeResult::Allowed = std::make_shared<IoDomainRuntimeResult>(IoDomainRuntimeDecision::Allowed, L"I/O domain runtime admission allowed.");

	class IoDomainRuntime final : public std::enable_shared_from_this<IoDomainRuntime>
	{
	public:
		std::shared_ptr<IoDomainRuntimeResult> Validate(std::shared_ptr<IoDomainRuntimeRequest> request);
		bool CanAdmit(std::shared_ptr<IoDomainRuntimeRequest> request);
	};
}
