#include "YAKSys_Hybrid_CPU.Core.TranslationInvalidationService.h"
#include "../../Domains/Descriptors/MemoryDomain/YAKSys_Hybrid_CPU.Core.MemoryDomainDescriptor.h"
#include "../AddressSpaces/YAKSys_Hybrid_CPU.Core.AddressSpaceDescriptor.h"
#include "../Translation/YAKSys_Hybrid_CPU.Core.MemoryTranslationPolicy.h"

using namespace HybridCPU_ISE::CloseToHSL::Memory::MMU;

namespace YAKSys_Hybrid_CPU::Core
{

	TranslationInvalidationService::TranslationInvalidationService() : TranslationInvalidationService(TranslationInvalidationHostBackend::getInstance())
	{
	}

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public TranslationInvalidationService(ITranslationInvalidationBackend backend)
	TranslationInvalidationService::TranslationInvalidationService(std::shared_ptr<ITranslationInvalidationBackend> backend) : _backend((backend != nullptr) ? backend : throw std::invalid_argument("backend"))
	{
	}

	std::shared_ptr<TranslationInvalidationResult> TranslationInvalidationService::Invalidate(std::shared_ptr<DomainRuntimeContext> context, TranslationInvalidationScope scope, unsigned long long descriptor, unsigned long long guestPhysicalAddress, unsigned long long sizeBytes, bool isSecondStageRoot, bool fenceSatisfied)
	{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MemoryDomainDescriptor? memory = context.Memory;
		std::shared_ptr<MemoryDomainDescriptor> memory = context->getMemory();
		if (memory == nullptr)
		{
			return Deny(TranslationInvalidationDecision::MissingMemoryDomain, L"Translation invalidation requires a memory-domain descriptor.");
		}

		if (!memory->isAuthoritativeMemoryStateOwner() || !memory->getOwnsSecondStageTranslation())
		{
			return Deny(TranslationInvalidationDecision::RuntimeAuthorityMissing, L"Compatibility projection cannot own translation invalidation authority.");
		}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: AddressSpaceDescriptor? addressSpace = memory.AddressSpace;
		std::shared_ptr<AddressSpaceDescriptor> addressSpace = memory->getAddressSpace();
		if (addressSpace == nullptr)
		{
			return Deny(TranslationInvalidationDecision::MissingAddressSpace, L"Translation invalidation requires a descriptor-owned address space.");
		}

		if (!addressSpace->isRuntimeAuthoritative())
		{
			return Deny(TranslationInvalidationDecision::RuntimeAuthorityMissing, L"Address-space projection cannot own invalidation authority.");
		}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MemoryTranslationPolicy? policy = memory.TranslationPolicy;
		std::shared_ptr<MemoryTranslationPolicy> policy = memory->getTranslationPolicy();
		if (policy == nullptr)
		{
			return Deny(TranslationInvalidationDecision::MissingTranslationPolicy, L"Translation invalidation requires an explicit memory translation policy.");
		}

		TranslationInvalidationPermission permission = ToPermission(scope);
		if (!policy->AllowsInvalidation(permission))
		{
			return Deny(TranslationInvalidationDecision::PermissionDenied, L"Memory translation policy denies the requested invalidation scope.");
		}

		if (policy->getRequireFenceBeforeInvalidation() && !fenceSatisfied)
		{
			return Deny(TranslationInvalidationDecision::FenceRequired, L"Translation invalidation requires a satisfied fence.");
		}

		if (scope == TranslationInvalidationScope::Address && !addressSpace->AllowsRange(guestPhysicalAddress, sizeBytes))
		{
			return Deny(TranslationInvalidationDecision::RangeDenied, L"Address-space descriptor denies the requested invalidation range.");
		}

		int invalidated = _backend->Invalidate(std::make_shared<TranslationInvalidationBackendRequest>(scope, descriptor, guestPhysicalAddress, sizeBytes, isSecondStageRoot));
		return TranslationInvalidationResult::Allowed(invalidated);
	}

	std::shared_ptr<TranslationInvalidationResult> TranslationInvalidationService::Deny(TranslationInvalidationDecision decision, const std::wstring &message)
	{
		return TranslationInvalidationResult::Denied(decision, message);
	}

	TranslationInvalidationPermission TranslationInvalidationService::ToPermission(TranslationInvalidationScope scope)
	{
		switch (scope)
		{
		case TranslationInvalidationScope::Address:
			return TranslationInvalidationPermission::Address;
		case TranslationInvalidationScope::AddressSpace:
			return TranslationInvalidationPermission::AddressSpace;
		case TranslationInvalidationScope::Global:
			return TranslationInvalidationPermission::Global;
		default:
			return TranslationInvalidationPermission::None;
		}
	}
}
