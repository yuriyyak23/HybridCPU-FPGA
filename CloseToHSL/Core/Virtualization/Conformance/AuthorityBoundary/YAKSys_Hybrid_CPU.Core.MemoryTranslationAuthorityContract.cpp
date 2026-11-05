#include "YAKSys_Hybrid_CPU.Core.MemoryTranslationAuthorityContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	MemoryTranslationAuthorityViolation MemoryTranslationAuthorityContract::Evaluate(MemoryTranslationAuthorityRequest request)
	{
		if (request.UsesGuestCr3AsAuthority)
		{
			return MemoryTranslationAuthorityViolation::GuestCr3UsedAsAuthority;
		}

		if (request.UsesVmcsIdentityAsAuthority)
		{
			return MemoryTranslationAuthorityViolation::VmcsIdentityUsedAsAuthority;
		}

		if (!request.HasGenericAddressSpaceRoot)
		{
			return MemoryTranslationAuthorityViolation::MissingGenericAddressSpaceRoot;
		}

		return request.AuthorityViewIncludesCompatibilityIdentity ? MemoryTranslationAuthorityViolation::CompatibilityIdentityInAuthorityView : MemoryTranslationAuthorityViolation::None;
	}

	bool MemoryTranslationAuthorityContract::IsSatisfied(MemoryTranslationAuthorityRequest request)
	{
		return Evaluate(request) == MemoryTranslationAuthorityViolation::None;
	}

	bool MemoryTranslationAuthorityContract::IsSatisfied(MemoryTranslationControl control)
	{
		return false;
	}

	MemoryTranslationAuthorityViolation MemoryTranslationAuthorityContract::EvaluateDomainControl(MemoryTranslationControl control)
	{
		return MemoryTranslationAuthorityViolation::MissingDomainControl;
	}
}
