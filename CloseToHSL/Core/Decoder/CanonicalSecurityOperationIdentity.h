#pragma once

#include "../../../NonRTL/Arch/Generated/GeneratedIsaShadowCatalog.g.h"
#include "ExecutionBindingIdentity.h"
#include "../Runtime/Domains/SecureCompute/Policies/Admission/SecureDomainAdmissionPolicy.h"
#include <string>
#include <memory>
#include <stdexcept>
#include <unordered_map>

using namespace YAKSys_Hybrid_CPU::Arch::Generated;

namespace YAKSys_Hybrid_CPU::Core::Decoder
{

	/// <summary>Decoded security meaning, distinct from legacy descriptor policy classes.</summary>
	enum class CanonicalSecurityOperationKind : unsigned char
	{
		Unknown = 0,
		Ordinary = 1
	};

	class CanonicalSecurityOperationIdentity final
	{
		// Explicit conversion of the reviewed ordinary-only generated catalog.
		// A changed catalog must be reviewed here; new encodings never inherit Ordinary.
	private:
		static inline const std::wstring OrdinaryCatalogSha256 = L"416607735e354770e0ec2707fe9b81ac3c3a7fccd344435e6dd8ccae6ebebdb0";

		static inline const std::unordered_map<unsigned int, std::shared_ptr<GeneratedStaticBinding>> Bindings = []
		{
			std::unordered_map<unsigned int, std::shared_ptr<GeneratedStaticBinding>> bindings;
			bindings.reserve(GeneratedIsaCatalog::Descriptors.size());
			for (const auto &descriptor : GeneratedIsaCatalog::Descriptors)
			{
				if (!descriptor)
					throw std::invalid_argument("Generated ISA catalog contains a null descriptor");
				auto binding = GeneratedStaticBinding::FromDescriptor(descriptor);
				if (!bindings.emplace(binding->Opcode, binding).second)
					throw std::logic_error("Generated ISA catalog contains duplicate opcodes");
			}
			return bindings;
		}();

	public:
		static CanonicalSecurityOperationKind FromGeneratedBinding(const std::shared_ptr<GeneratedStaticBinding> &binding);

		/// <summary>Compatibility configuration only; never derives secure machine authority.</summary>
		static CanonicalSecurityOperationKind FromLegacyPolicyClass(SecureDomainOperationClass value);
	};

}
