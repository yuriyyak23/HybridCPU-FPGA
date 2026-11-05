#pragma once

#include "TranslationInvalidationService.h"
#include "../../../../Memory/MMU/TranslationInvalidationHostBackend.h"
#include "../../Domains/Services/YAKSys_Hybrid_CPU.Core.DomainRuntimeContext.h"
#include "../Translation/MemoryTranslationPolicy.h"
#include <string>
#include <stdexcept>
#include <memory>

using namespace HybridCPU_ISE::CloseToHSL::Memory::MMU;

namespace YAKSys_Hybrid_CPU::Core
{
	class TranslationInvalidationService final : public std::enable_shared_from_this<TranslationInvalidationService>
	{
	private:
		const std::shared_ptr<ITranslationInvalidationBackend> _backend;

	public:
		TranslationInvalidationService();

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public TranslationInvalidationService(ITranslationInvalidationBackend backend)
		TranslationInvalidationService(std::shared_ptr<ITranslationInvalidationBackend> backend);

		std::shared_ptr<TranslationInvalidationResult> Invalidate(std::shared_ptr<DomainRuntimeContext> context, TranslationInvalidationScope scope, unsigned long long descriptor, unsigned long long guestPhysicalAddress = 0, unsigned long long sizeBytes = 0, bool isSecondStageRoot = true, bool fenceSatisfied = false);

	private:
		static std::shared_ptr<TranslationInvalidationResult> Deny(TranslationInvalidationDecision decision, const std::wstring &message);

		static TranslationInvalidationPermission ToPermission(TranslationInvalidationScope scope);

	};
}
