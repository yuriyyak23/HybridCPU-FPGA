#pragma once

#include "../../Compatibility/Generated/CapabilityProjection/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSetSchema.h"
#include "VmxCapsBitSchemaConformanceContract.h"
#include <memory>
#include "../../../../../tangible_string_helper.h"

namespace YAKSys_Hybrid_CPU::Core
{
	class VmxCapsBitSchemaConformanceContract final : public std::enable_shared_from_this<VmxCapsBitSchemaConformanceContract>
	{
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public VmxCapsBitSchemaConformanceViolation Evaluate(CapabilityDescriptorSetSchema? schema, CapabilityBitSchemaArtifact artifact)
		VmxCapsBitSchemaConformanceViolation Evaluate(std::shared_ptr<CapabilityDescriptorSetSchema> schema, std::shared_ptr<CapabilityBitSchemaArtifact> artifact);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public bool IsSatisfied(CapabilityDescriptorSetSchema? schema, CapabilityBitSchemaArtifact artifact)
		bool IsSatisfied(std::shared_ptr<CapabilityDescriptorSetSchema> schema, std::shared_ptr<CapabilityBitSchemaArtifact> artifact);

		bool IsCurrentSchemaSatisfied();
	};
}
