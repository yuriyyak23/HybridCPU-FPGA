#include "TranslationInvalidationHostBackend.h"
#include "../../Core/Virtualization/Compatibility/Adapters/IO/YAKSys_Hybrid_CPU.Memory.IOMMU.h"

using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace HybridCPU_ISE::CloseToHSL::Memory::MMU
{

	std::shared_ptr<TranslationInvalidationHostBackend> TranslationInvalidationHostBackend::getInstance()
	{
		static const auto instance = std::shared_ptr<TranslationInvalidationHostBackend>(new TranslationInvalidationHostBackend());
		return instance;
	}

	TranslationInvalidationHostBackend::TranslationInvalidationHostBackend()
	{
	}

	int TranslationInvalidationHostBackend::Invalidate(std::shared_ptr<TranslationInvalidationBackendRequest> request)
	{
		return IOMMU::ApplyTranslationInvalidation(request->Scope, request->Descriptor, request->GuestPhysicalAddress, request->IsSecondStageRoot);
	}
}
