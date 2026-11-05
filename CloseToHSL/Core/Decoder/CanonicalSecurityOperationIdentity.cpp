#include "CanonicalSecurityOperationIdentity.h"

using namespace YAKSys_Hybrid_CPU::Arch::Generated;

namespace YAKSys_Hybrid_CPU::Core::Decoder
{

	CanonicalSecurityOperationKind CanonicalSecurityOperationIdentity::FromGeneratedBinding(const std::shared_ptr<GeneratedStaticBinding> &binding)
	{
		if (!binding || binding->Opcode == 0 || binding->CatalogSha256 != OrdinaryCatalogSha256)
		{
			return CanonicalSecurityOperationKind::Unknown;
		}

		const auto expected = Bindings.find(binding->Opcode);
		if (expected == Bindings.end() || !expected->second || *binding != *expected->second)
			return CanonicalSecurityOperationKind::Unknown;

		return CanonicalSecurityOperationKind::Ordinary;
	}

	CanonicalSecurityOperationKind CanonicalSecurityOperationIdentity::FromLegacyPolicyClass(SecureDomainOperationClass value)
	{
		switch (value)
		{
		case SecureDomainOperationClass::Ordinary:
			return CanonicalSecurityOperationKind::Ordinary;
		default:
			return CanonicalSecurityOperationKind::Unknown;
		}
	}
}
