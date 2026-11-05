#pragma once

#include "../../Core/Runtime/Memory/Invalidation/TranslationInvalidationService.h"
#include <memory>

// Description: Host memory backend that performs generic translation invalidation after descriptor/policy admission has already succeeded.
using namespace YAKSys_Hybrid_CPU::Core;

namespace HybridCPU_ISE::CloseToHSL::Memory::MMU
{

	class TranslationInvalidationHostBackend final : public ITranslationInvalidationBackend
	{
	public:
		static std::shared_ptr<TranslationInvalidationHostBackend> getInstance();

	private:
		TranslationInvalidationHostBackend();

	public:
		int Invalidate(std::shared_ptr<TranslationInvalidationBackendRequest> request) override;
	};

}
