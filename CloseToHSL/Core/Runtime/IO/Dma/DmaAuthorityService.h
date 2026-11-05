#pragma once
#include "DmaDomainBinding.h"
#include <memory>


namespace YAKSys_Hybrid_CPU::Core
{

	enum class DmaAuthorityDecision : unsigned char
	{
		Allowed = 0,
		MissingIoDomain = 1,
		MissingDmaWindow = 2,
		MissingIommuBinding = 3,
		RuntimeAuthorityMissing = 4,
		PermissionDenied = 5,
		RangeDenied = 6,
		FenceRequired = 7
	};
	struct DmaAuthorityResult final
	{
		const DmaAuthorityDecision Decision;
		const YAKSys_Hybrid_CPU::Memory::DmaFault Fault;
		DmaAuthorityResult(DmaAuthorityDecision decision, YAKSys_Hybrid_CPU::Memory::DmaFault fault) : Decision(decision), Fault(std::move(fault)) {}
		bool isAllowed() const noexcept { return Decision == DmaAuthorityDecision::Allowed; }
		static std::shared_ptr<DmaAuthorityResult> getAllowed() { static auto allowed = std::make_shared<DmaAuthorityResult>(DmaAuthorityDecision::Allowed, YAKSys_Hybrid_CPU::Memory::DmaFault{}); return allowed; }
		static std::shared_ptr<DmaAuthorityResult> Denied(DmaAuthorityDecision decision, YAKSys_Hybrid_CPU::Memory::DmaFault fault) { return std::make_shared<DmaAuthorityResult>(decision, std::move(fault)); }
	};
}
